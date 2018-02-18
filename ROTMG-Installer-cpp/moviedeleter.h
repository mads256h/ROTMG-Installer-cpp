#pragma once

#include <Windows.h>

#include "constants.h"

//Deletes the registy values that flashplayer uses. So the user does not get the location of the flash file.
class MovieDeleter
{
public:
	static void DeleteKeys()
	{
		HKEY hKey = NULL;

		//Open the registry key.
		RegOpenKeyEx(HKEY_CURRENT_USER,
			MovieKey,
			NULL,
			KEY_SET_VALUE,
			&hKey);

		//Delete the values.
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