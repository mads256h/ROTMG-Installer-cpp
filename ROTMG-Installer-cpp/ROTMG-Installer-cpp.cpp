// ROTMG-Installer-cpp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ROTMG-Installer-cpp.h"
#include "process.h"
#include "constants.h"
#include "path.h"
#include "downloader.h"
#include "antidecompiler.h"
#include "moviedeleter.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);


	if (!Path::Exists(FolderLocation))
	{
		Path::Create(FolderLocation);
	}

	Downloader::DownloadFile(UpdateInfoUrl, UpdateInfoLocation);

	nlohmann::json js;
	try
	{
		js = nlohmann::json::parse(File::Read(UpdateInfoLocation).c_str());
	}
	catch (nlohmann::json::parse_error)
	{
		MessageBox(NULL, L"JSON parsing failed.\r\nCheck your internet connection!", L"JSON parsing failed.", MB_OK);
		return 1;
	}
	 

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

	MovieDeleter::DeleteKeys();
	process.WaitForExit();

	MovieDeleter::DeleteKeys();

	return 0;
}
