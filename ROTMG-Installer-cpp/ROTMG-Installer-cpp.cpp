// ROTMG-Installer-cpp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ROTMG-Installer-cpp.h"
#include "process.h"
#include "constants.h"
#include "path.h"
#include "downloader.h"
#include "antidecompiler.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    // Anti debugging code:
#ifndef _DEBUG
	if (IsDebuggerPresent() == TRUE)
	{
		MessageBox(NULL, L"A URL context could not be created!", L"WINAPI Error", MB_OK);
		return 1;
	}
#endif _DEBUG

	if (!Path::Exists(FolderLocation))
	{
		Path::Create(FolderLocation);
	}

	Downloader::DownloadFile(UpdateInfoUrl, UpdateInfoLocation);

	const auto js = nlohmann::json::parse(File::Read(UpdateInfoLocation).c_str());

	if (File::Exists(UpdateInfoLocation))
	{
		File::Delete(UpdateInfoLocation);
	}

	Downloader::DownloadComponent(js);

	AntiDecompiler::Start();

	std::wstring runParams;
	runParams.append(Path::Combine(FolderLocation, L"flashplayer.exe"));
	runParams.append(L" ");
	runParams.append(Path::Combine(FolderLocation, L"flashcache.tmp"));

	auto process = Process::Run(runParams);

	process.WaitForMainWindow();
	process.SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROTMGINSTALLERCPP)));
	process.SetTitle(L"Noobhereo and mads256c's client");
	process.DisableResizing();
	process.WaitForExit();

	return 0;
}
