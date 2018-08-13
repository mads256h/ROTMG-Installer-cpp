#pragma once

#include <string>
#include <exception>
#include <iostream>
#include <filesystem>

#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include "converter.h"


class WinApiException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Something in the WinApi went wrong!";
	}
};

class PathTooLongException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The path is too long!";
	}
};

class PathIsNotADirectoryException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The path is not a directory!";
	}
};

class DirectoryExistsException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The directory exists!";
	}
};

//This class has to do with directories and paths.
class Path
{
public:
	//Combines two paths. Useful for combining a directory and a filename.
	static std::wstring Combine(const std::wstring& path1, const std::wstring& path2)
	{
		using namespace std::experimental::filesystem;
		const path dir1(path1);
		const path dir2(path2);

		path combined = dir1 / dir2;

		return Converter::ToWString(combined.string());
	}

	//Creates a directory.
	static void Create(const std::wstring& path)
	{
		//If the directory already exists throw a directoryExistsException.
		if (Exists(path))
			throw DirectoryExistsException();

		if (SHCreateDirectoryEx(NULL, path.c_str(), NULL) != 0)
		{
			throw WinApiException();
		}
	}

	//Checks if a directory exists.
	static bool Exists(const std::wstring& path)
	{
		//Get the FileAttributes of the path. If the attribute is FILE_ATTRIBUTE_DIRECTORY it is a directory and it exists.
		DWORD attributes = GetFileAttributes(path.c_str());

		if (attributes == INVALID_FILE_ATTRIBUTES)
			return false;

		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;

		return false;
	}

	//Gets the AppData directory.
	static std::wstring GetAppDataPath()
	{
		TCHAR szPath[MAX_PATH];
		if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		{
			throw WinApiException();
		}

		std::wstring appDataPath(szPath);
		return appDataPath;
	}

	//Gets the directory part of a path.
	static std::wstring GetDirectory(const std::wstring& path)
	{
		WCHAR fileName[MAX_PATH];
		wcscpy(fileName, path.c_str());
		PathRemoveFileSpec(fileName);

		std::wstring retString(fileName);
		return retString;
	}

	//Gets the fileName part of a path.
	static std::wstring GetFileName(const std::wstring& path)
	{
		std::wstring retString(PathFindFileName(path.c_str()));

		return retString;
	}


	Path() = delete;
};