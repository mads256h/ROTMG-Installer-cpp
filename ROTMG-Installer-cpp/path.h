#ifndef PATH_H
#define PATH_H

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
} winApiException;

class PathTooLongException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The path is too long!";
	}
} pathTooLongException;

class PathIsNotADirectoryException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The path is not a directory!";
	}
} pathIsNotADirectoryException;

class DirectoryExistsException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The directory exists!";
	}
} directoryExistsException;

class Path
{
public:

	static std::wstring Combine(const std::wstring path1, const std::wstring path2)
	{
		using namespace std::experimental::filesystem;
		const path dir1(path1);
		const path dir2(path2);

		path combined = dir1 / dir2;

		return Converter::ToWString(combined.string());
	}

	static void Create(const std::wstring path)
	{
		if (Exists(path))
			throw directoryExistsException;


		if (CreateDirectory(path.c_str(), NULL) == FALSE)
		{
			throw winApiException;
		}
	}

	static bool Exists(const std::wstring path)
	{
		DWORD attributes = GetFileAttributes(path.c_str());

		if (attributes == INVALID_FILE_ATTRIBUTES)
			return false;

		if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;

		return false;
	}

	static std::wstring GetAppDataPath()
	{
		TCHAR szPath[MAX_PATH];
		if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		{
			throw winApiException;
		}

		std::wstring appDataPath(szPath);
		return appDataPath;
	}

	static std::wstring GetDirectory(const std::wstring path)
	{
		WCHAR fileName[MAX_PATH];
		wcscpy(fileName, path.c_str());
		PathRemoveFileSpec(fileName);

		std::wstring retString(fileName);
		return retString;
	}

	static std::wstring GetFileName(const std::wstring path)
	{
		std::wstring retString(PathFindFileName(path.c_str()));

		return retString;
	}



private:
	Path() {}
};

#endif // PATH_H