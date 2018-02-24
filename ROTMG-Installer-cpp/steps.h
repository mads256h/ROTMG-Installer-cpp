#pragma once

#include <atomic>

#include <Windows.h>

#include "path.h"
#include "constants.h"
#include "downloader.h"
#include "json.hpp"
#include "file.h"
#include "antidecompiler.h"
#include "process.h"
#include "moviedeleter.h"
#include "injector.h"

class Steps
{
public:
	static void Run(HINSTANCE hInstance)
	{
		//If our folder does not exist: Create it.
		if (!Path::Exists(FolderLocation))
		{
			Path::Create(FolderLocation);
		}

		if (File::Exists(ProcessIdLocation))
		{
			File::Delete(ProcessIdLocation);
		}

		std::ofstream processIdFile(ProcessIdLocation);

		if (processIdFile.is_open())
		{
			processIdFile << GetCurrentProcessId();

			processIdFile.close();
		}



		if (!Path::Exists(TrustedFolderLocation))
		{
			Path::Create(TrustedFolderLocation);
		}

		if (File::Exists(TrustedFile))
		{
			File::Delete(TrustedFile);
		}

		std::ofstream trustedFileStream(TrustedFile);

		if (trustedFileStream.is_open())
		{
			trustedFileStream << Converter::ToString(FolderLocation);
			
			trustedFileStream.close();
		}
		else
		{
			Error(L"Could not write trusted file.\r\nPlease close all flashplayer instances and try again.");
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

		if (File::Exists(DecryptedClientLocation))
		{
			File::Delete(DecryptedClientLocation);
		}

		File::Encrypt(Path::Combine(FolderLocation, L"flashcache.tmp"), DecryptedClientLocation);

		//Create the flashplayer arguments and run it.
		std::wstring runParams;
		runParams.append(Path::Combine(FolderLocation, L"flashplayer.exe"));
		runParams.append(L" ");
		runParams.append(Path::Combine(FolderLocation, L"flashcache.cache"));

		auto process = Process::Run(runParams);

		std::string dllLocation = Converter::ToString(Path::Combine(FolderLocation, L"flashicons.icns"));

		if (!File::Exists(Converter::ToWString(dllLocation)))
			Error(L"Can't find Flash Icons.\r\nTry restarting the program");

		Injector::Inject(process, dllLocation);

		process.WaitForMainWindow(); //Wait until we have a window.
		process.SetIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROTMGINSTALLERCPP))); //Change the icon.
		process.SetTitle(L"Noobhereo and mads256c's client"); //Set the title.
		process.DisableResizing(); //Disable Resizing.

		if (File::Exists(DecryptedClientLocation))
		{
			File::Delete(DecryptedClientLocation);
		}

		MovieDeleter::DeleteKeys(); //Delete the flashplayer keys.
		process.WaitForExit(); //Wait until we have exited.

		MovieDeleter::DeleteKeys();

		if (File::Exists(DecryptedClientLocation))
		{
			File::Delete(DecryptedClientLocation);
		}

		if (File::Exists(ProcessIdLocation))
		{
			File::Delete(ProcessIdLocation);
		}

	}

	Steps() = delete;
};
