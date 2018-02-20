#pragma once

#include <atomic>
#include <thread>

#include <Windows.h>

#include "path.h"
#include "constants.h"
#include "downloader.h"
#include "json.hpp"
#include "file.h"
#include "antidecompiler.h"
#include "process.h"
#include "moviedeleter.h"

class Steps
{
	Steps() = delete;

public:
	static void Run(HINSTANCE hInstance)
	{
		//If our folder does not exist: Create it.
		if (!Path::Exists(FolderLocation))
		{
			Path::Create(FolderLocation);
		}

		
		//Download the UpdateInfo JSON.
		Downloader::DownloadFile(UpdateInfoUrl, UpdateInfoLocation, L"Update Info");

		//Parse the JSON.
		nlohmann::json js;
		try
		{
			js = nlohmann::json::parse(File::Read(UpdateInfoLocation).c_str());
		}
		catch (nlohmann::json::parse_error&)
		{
			MessageBox(NULL, L"JSON parsing failed.\r\nCheck your internet connection!", L"JSON parsing failed.", MB_OK);
			exit(1);
		}

		//Delete the UpdateInfo file.
		if (File::Exists(UpdateInfoLocation))
		{
			File::Delete(UpdateInfoLocation);
		}

		//Downloads the files in the json.
		Downloader::DownloadComponents(js);

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

	}
};
