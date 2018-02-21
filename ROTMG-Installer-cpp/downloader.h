#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib,"winhttp.lib")

#include "file.h"
#include "path.h"
#include "json.hpp"
#include "constants.h"
#include "resource.h"
#include "mbox.h"
#include "url.h"
#include "error.h"

class DownloadException : public std::exception
{
	const char* what() const throw() override
	{
		return "There was an error while downloading!";
	}
} downloadException;

//Where we store the information from the JSON UpdateInfo file.
struct FileInfo
{

	FileInfo(const std::string& name, const std::string& fileName, const std::string& downloadUrl, const std::string& md5)
	{
		Name = name;
		FileName = fileName;
		DownloadUrl = downloadUrl;
		MD5 = md5;
	}

	std::string Name;
	std::string FileName;
	std::string DownloadUrl;
	std::string MD5;


};

bool downloading;

//Downloads things.
class Downloader {
public:
	//Downloads a file from url and puts it into path.
	static void DownloadFile(const std::wstring& url, const std::wstring& path, const std::wstring& info)
	{
		//Check if the directory we download to exists. If it does not then create it.
		if (!Path::Exists(Path::GetDirectory(path)))
		{
			Path::Create(Path::GetDirectory(path));
		}

		//Check if the file we want to download already exists. If it does delete the file.
		if (File::Exists(path))
		{
			File::Delete(path);
		}

		Url url_split(Converter::ToString(url));

		std::wstring str(L"Updating ");
		str.append(info);
		str.append(L"...");

		MBox::Create(str, L"Connecting...");

		const HINTERNET hSession = WinHttpOpen(L"Client Updater", NULL, NULL, NULL, NULL);

		if (!hSession)
			Error(L"Could not create a WinHttp session.\r\nTry restarting your computer!");


		const HINTERNET hConnect = WinHttpConnect(hSession, Converter::ToWString(url_split.host_).c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL);

		if (!hConnect)
			Error(L"Could not connect to the server.\r\nCheck your internet connection.");


		const HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", Converter::ToWString(url_split.path_).c_str(), NULL, NULL, NULL, WINHTTP_FLAG_SECURE);

		if (!hRequest)
			Error(L"Could not connect to the server.\r\nCheck your internet connection.");


		MBox::SetStatus2(L"Sending request...");

		BOOL bResult = WinHttpSendRequest(hRequest, NULL, 0, NULL, NULL, NULL, NULL);

		if (!bResult)
			Error(L"Could not send the request.\r\nCheck your internet connection.");


		MBox::SetStatus2(L"Waiting on response...");

		bResult = WinHttpReceiveResponse(hRequest, NULL);

		if (!bResult)
			Error(L"Could not recieve a response.\r\nCheck your internet connection.");


		MBox::SetStatus2(L"Downloading...");

		FILE* pFile;
		if (fopen_s(&pFile, Converter::ToString(path).c_str(), "w+b") != 0)
		{
			Error(L"Could not open the file stream.");
		}

		DWORD dwSize;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer;
		do
		{
			dwSize = 0;

			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				switch (GetLastError())
				{
				case ERROR_WINHTTP_CONNECTION_ERROR:
				case ERROR_WINHTTP_TIMEOUT:
					Error(L"A connection error occured.\r\nCheck your internet connection.");
					break;

				default:
					Error(L"An unknown download error occured.\r\nCheck your internet connection or try restarting your computer.");
				}
			}

			pszOutBuffer = new CHAR[dwSize + 1];

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, static_cast<LPVOID>(pszOutBuffer), dwSize, &dwDownloaded))
			{
				switch (GetLastError())
				{
				case ERROR_WINHTTP_CONNECTION_ERROR:
				case ERROR_WINHTTP_TIMEOUT:
					Error(L"A connection error occured.\r\nCheck your internet connection.");
					break;

				default:
					Error(L"An unknown download error occured.\r\nCheck your internet connection or try restarting your computer.");
				}
			}
			else
			{
				if (dwSize != 0)
				{
					if (dwDownloaded == 0)
					{
						MBox::SetProgress(0);
					}
					else
					{
						const int prog = static_cast<double>(dwDownloaded) / static_cast<double>(dwSize) * 100.0;
						MBox::SetProgress(prog);

					}

					
				}
				fwrite(pszOutBuffer, static_cast<size_t>(dwDownloaded), static_cast<size_t>(1), pFile);
			}


		}
		while (dwSize > 0);

		delete[] pszOutBuffer;

		fclose(pFile);

		MBox::Destroy();

	}

	//Downloads the stuff from the json file.
	static void DownloadComponents(const nlohmann::json& json)
	{
		//For each entries.
		for (auto it = json.begin(); it != json.end(); ++it)
		{
			//Create a new FileInfo. Using JSON data.
			FileInfo fileInfo(it.key(), json[it.key()]["FileName"], json[it.key()]["DownloadUrl"], json[it.key()]["MD5"]);

			//If the file does not exist: Download it.
			if (!File::Exists(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))))
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)), Converter::ToWString(fileInfo.Name));
				continue;
			}

			//If the file exists check the MD5 hash. If it does not match; download the file.
			std::string md5hash(File::MD5Hash(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))));
			if (_strcmpi(fileInfo.MD5.c_str(), md5hash.c_str()) != 0)
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)), Converter::ToWString(fileInfo.Name));
			}

		}
	}


	Downloader() = delete;
};

#endif // DOWNLOADER_H