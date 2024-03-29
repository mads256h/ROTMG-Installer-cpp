#pragma once

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
};

class FileException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "There was an error processing the file!";
	}
};


//A static class for use with files.
class File {
public:

	//Deletes the file specified by path.
	static void Delete(const std::wstring& path, const bool ignoreError = false)
	{
		//Check if the file exists. If it does not and ignoreError is false. Throw a fileNotFoundException.
		if (Exists(path))
		{
			//If we cannot delete the file and ignoreError is false. Throw a fileException.
			if (DeleteFile(path.c_str()) == FALSE && !ignoreError)
			{
				throw FileException();
			}

			//If the file still exists after deletion. Throw a fileException.
			if (Exists(path))
			{
				throw FileException();
			}
		}
		else if (!ignoreError)
		{
			throw FileNotFoundException();
		}
	}

	//Check if path is a file and it exists.
	static bool Exists(const std::wstring& path)
	{
		//Opens the file. If it is successful it closes the file again.
		FILE *file;
		if (fopen_s(&file, Converter::ToString(path).c_str(), "r") == 0)
		{
			fclose(file);
			return true;
		}
		else
		{
			return false;
		}
	}

	//Read the file in path.
	static std::string Read(const std::wstring& path)
	{
		//If the file does not exist. Throw a fileNotFoundException.
		if (Exists(path))
		{
			//The file stream
			std::ifstream ifstream(path, std::ios::binary);

			//Creates a new string based on the contents in the file stream.
			std::string retString((std::istreambuf_iterator<char>(ifstream)), std::istreambuf_iterator<char>());

			return retString;
		}
		throw FileNotFoundException();

	}

	//Get the MD5 from a file.
	static std::string MD5Hash(const std::wstring& path)
	{
		//If the does not exist. Throw a fileNotFoundException.
		if (!Exists(path))
			throw FileNotFoundException();


		MD5 md5;
		std::string md5hash(md5.digestFile(Converter::ToString(path).c_str()));
		
		return md5hash;
	}


	static void Encrypt(const std::wstring& input, const std::wstring& output)
	{
		if (Exists(output))
		{
			throw FileException();
		}

		if (!Exists(input))
		{
			throw FileNotFoundException();
		}

		char key = 'O'; //Any char will work
		std::string toEncrypt = Read(input);
		std::string output1 = toEncrypt;

		for (size_t i = 0; i < toEncrypt.size(); i++)
			output1[i] = toEncrypt[i] ^ key;

		std::ofstream outputfile(output, std::ios::binary);

		if (outputfile.is_open())
		{
			outputfile << output1;
			outputfile.close();
		}
		else
		{
			throw FileException();
		}
	}

	File() = delete;
};

