#pragma once
#include <string>
#include <sstream>

#include <Windows.h>

inline void Error(const std::wstring& text)
{
	std::wstringstream ss;
	ss << text;
	ss << "\r\n";
	ss << L"Error code: ";
	ss << GetLastError();

	MessageBox(NULL, ss.str().c_str(), L"Error!", MB_ICONERROR);
	exit(1);
}