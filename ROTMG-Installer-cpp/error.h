#pragma once
#include <string>
#include <sstream>

#include <Windows.h>
#include "moviedeleter.h"

inline void Error(const std::wstring& text)
{
	std::wstringstream ss;
	ss << text;
	ss << "\r\n";
	ss << L"Error code: ";
	ss << GetLastError();

	MovieDeleter::DeleteKeys();

	if (File::Exists(DecryptedClientLocation))
	{
		File::Delete(DecryptedClientLocation);
	}

	if (File::Exists(ProcessIdLocation))
	{
		File::Delete(ProcessIdLocation);
	}

	MessageBox(NULL, ss.str().c_str(), L"Error!", MB_ICONERROR);
	exit(1);
}
