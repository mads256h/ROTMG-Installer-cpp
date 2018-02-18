#ifndef FILE_H
#define FILE_H

#include <string>
#include <fstream>
#include <streambuf>
#include <windows.h>
#include <exception>

#include "converter.h"
#include "md5.h"

class FileNotFoundException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The file was not found!";
	}
} fileNotFoundException;

class FileException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "There was an error processing the file!";
	}
} fileException;

class File {
public:

	static void Delete(const std::wstring fileName, const bool ignoreError = false)
	{
		if (Exists(fileName))
		{
			//If noError is not 0. It failed to delete the file.
			auto noError = DeleteFile(fileName.c_str());

			if (noError == 0 && !ignoreError)
			{
				throw fileException;
			}

			if (Exists(fileName))
			{
				throw fileException;
			}
		}
		else if (!ignoreError)
		{
			throw fileNotFoundException;
		}
	}

	static bool Exists(const std::wstring fileName)
	{
		FILE *file;
		if (fopen_s(&file, Converter::ToString(fileName).c_str(), "r") == 0)
		{
			fclose(file);
			return true;
		}
		else
		{
			return false;
		}
	}


	static std::string Read(const std::wstring fileName)
	{
		if (Exists(fileName))
		{
			//The file stream
			std::ifstream ifstream(fileName);

			//Creates a new string based on the contents in the file stream.
			std::string string((std::istreambuf_iterator<char>(ifstream)), std::istreambuf_iterator<char>());

			return string;
		}
		throw fileNotFoundException;

	}

	static std::string MD5Hash(const std::wstring fileName)
	{
		if (!Exists(fileName))
			throw fileNotFoundException;
		MD5 md5;
		std::string md5hash(md5.digestFile(Converter::ToString(fileName).c_str()));
		
		return md5hash;
	}

private:
	File() {}
};

#endif // FILE_H
