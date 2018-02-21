#pragma once
#include <string>

#include <Windows.h>

inline void Error(const std::wstring& text)
{
	MessageBox(NULL, text.c_str(), L"Error!", MB_ICONERROR);
	exit(1);
}