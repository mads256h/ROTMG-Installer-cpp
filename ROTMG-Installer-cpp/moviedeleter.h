#pragma once

#include <Windows.h>

#include "constants.h"

class MovieDeleter
{
public:
	static void DeleteKeys()
	{
		HKEY hKey = NULL;

		RegOpenKeyEx(HKEY_CURRENT_USER,
			MovieKey,
			NULL,
			KEY_SET_VALUE,
			&hKey);

		RegDeleteValue(hKey, L"RecentMovie1");
		RegDeleteValue(hKey, L"RecentMovie2");
		RegDeleteValue(hKey, L"RecentMovie3");
		RegDeleteValue(hKey, L"RecentMovie4");
		RegDeleteValue(hKey, L"RecentMovie5");
		RegDeleteValue(hKey, L"RecentMovie6");
		RegDeleteValue(hKey, L"RecentMovie7");
		RegDeleteValue(hKey, L"RecentMovie8");
		RegDeleteValue(hKey, L"RecentMovie9");
	}

private:
	MovieDeleter() = default;
};