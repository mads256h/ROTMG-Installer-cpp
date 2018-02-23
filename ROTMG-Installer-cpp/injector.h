#pragma once

#include <string>

#include <Windows.h>

#include "process.h"
#include "error.h"

class Injector
{
public:

	static void Inject(Process process, std::string& dllPath)
	{
		HANDLE hProcess = process.GetHandle(PROCESS_ALL_ACCESS);
		LPVOID address = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		if (address == NULL)
			Error(L"Could not find LoadLibraryA.");

		LPVOID arg = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dllPath.c_str()), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (arg == NULL)
			Error(L"Could not allocate memory.");

		int n = WriteProcessMemory(hProcess, arg, dllPath.c_str(), strlen(dllPath.c_str()), NULL);
		if (n == NULL)
			Error(L"Could not write memory.");

		HANDLE threadId = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)address, arg, NULL, NULL);
		if (threadId == NULL)
			Error(L"Could not create thread.");
	
	}

	Injector() = delete;
};