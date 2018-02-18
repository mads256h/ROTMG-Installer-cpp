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

//Where we store the information from the JSON UpdateInfo file.
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

//Downloads things.
class Downloader {
public:
	//Downloads a file from url and puts it into path.
	static void DownloadFile(const std::wstring url, const std::wstring path)
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

		//Try to download the file. Get the error code in hResult.
		HRESULT hResult = URLDownloadToFile(NULL, url.c_str(), path.c_str(), 0, NULL);

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

	//Downloads the stuff from the json file.
	static void DownloadComponent(const nlohmann::json json)
	{
		//For each entries.
		for (auto it = json.begin(); it != json.end(); ++it)
		{
			//Create a new FileInfo. Using JSON data.
			FileInfo fileInfo(it.key(), json[it.key()]["FileName"], json[it.key()]["DownloadUrl"], json[it.key()]["MD5"]);

			//If the file does not exist: Download it.
			if (!File::Exists(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))))
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)));
				continue;
			}

			//If the file exists check the MD5 hash. If it does not match; download the file.
			std::string md5hash(File::MD5Hash(Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName))));
			if (_strcmpi(fileInfo.MD5.c_str(), md5hash.c_str()) != 0)
			{
				DownloadFile(Converter::ToWString(fileInfo.DownloadUrl), Path::Combine(FolderLocation, Converter::ToWString(fileInfo.FileName)));
			}

		}
	}

private:
	Downloader() {}
};

#endif // DOWNLOADER_H