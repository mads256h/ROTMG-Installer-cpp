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

	//If our folder does not exist: Create it.
	if (!Path::Exists(FolderLocation))
	{
		Path::Create(FolderLocation);
	}

	//Download the UpdateInfo JSON.
	Downloader::DownloadFile(UpdateInfoUrl, UpdateInfoLocation);

	//Parse the JSON.
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
	 
	//Delete the UpdateInfo file.
	if (File::Exists(UpdateInfoLocation))
	{
		File::Delete(UpdateInfoLocation);
	}

	//Downloads the files in the json.
	Downloader::DownloadComponent(js);

	//Starts the anti-decompiler.
	AntiDecompiler::Start();

	//Create the flashplayer arguments and run it.
	std::wstring runParams;
	runParams.append(Path::Combine(FolderLocation, L"flashplayer.exe"));
	runParams.append(L" ");
	runParams.append(Path::Combine(FolderLocation, L"flashcache.tmp"));

	auto process = Process::Run(runParams);

	process.WaitForMainWindow(); //Wait until we have a window.
	process.SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROTMGINSTALLERCPP))); //Change the icon.
	process.SetTitle(L"Noobhereo and mads256c's client"); //Set the title.
	process.DisableResizing(); //Disable Resizing.

	MovieDeleter::DeleteKeys(); //Delete the flashplayer keys.
	process.WaitForExit(); //Wait until we have exited.

	MovieDeleter::DeleteKeys();

	return 0;
}
