#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <string>

#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")

#include "file.h"
#include "path.h"
#include "json.hpp"
#include "constants.h"

class DownloadException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "There was an error while downloading!";
	}
} downloadException;

struct FileInfo
{

	FileInfo(std::string name, std::string fileName, std::string downloadUrl, std::string md5)
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


class Downloader {
public:
	static void DownloadFile(const std::wstring url, const std::wstring path)
	{
		if (!Path::Exists(Path::GetDirectory(path)))
		{
			Path::Create(Path::GetDirectory(path));
		}

		if (File::Exists(path))
		{
			File::Delete(path);
		}

		HRESULT hResult = URLDownloadToFile(0, url.c_str(), path.c_str(), 0, 0);

		//Checks if there is an error.
		switch (hResult)
		{
		case S_OK:
			if (!File::Exists(path))
			{
				throw downloadException;
			}
			return;

		case INET_E_CANNOT_CONNECT:
		case INET_E_CONNECTION_TIMEOUT:
		case INET_E_DOWNLOAD_FAILURE:
		case INET_E_RESOURCE_NOT_FOUND:
			MessageBox(NULL, L"Could not connect to the server.\r\nMake sure your internet connection is working.", L"Download Error", MB_OK);
			exit(1);
			break;

		default:
			throw downloadException;
		}

	}

	static void DownloadComponent(const nlohmann::json json)
	{
		for (auto it = json.begin(); it != json.end(); ++it)
		{
			FileInfo fileInfo(it.key(), json[it.key()]["FileName"], json[it.key()]["DownloadUrl"], json[it.key()]["MD5"]);

			if (!File::Exists(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))))
			{
				std::wstring mboxInfo(L"Downloading ");
				mboxInfo.append(Converter::ToWString(fileInfo.Name));

				//MessageBox(NULL, mboxInfo.c_str(), L"Downloading", MB_OK);

				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)));
				continue;
			}

			std::string md5hash(File::MD5Hash(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))));

			if (_strcmpi(fileInfo.MD5.c_str(), md5hash.c_str()) != 0)
			{
				std::wstring mboxInfo(L"Downloading ");
				mboxInfo.append(Converter::ToWString(fileInfo.Name));

				//MessageBox(NULL, mboxInfo.c_str(), L"Downloading", MB_OK);

				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)));
			}

		}
	}

private:
	Downloader() {}
};

#endif // DOWNLOADER_H