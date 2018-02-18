#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "converter.h"


class Process
{
public:
	static Process GetProcessByName(const std::wstring filename);
	static std::vector<Process> GetProcesses();
	static Process Run(std::wstring path);
	static BOOL IsMainWindow(HWND handle);

	Process(std::wstring name, DWORD id, HANDLE handle);
	std::wstring Name;
	DWORD Id;
	HANDLE Handle;

	bool Kill();
	HWND GetMainWindow() const;
	void WaitForMainWindow() const;
};

#endif // PROCESS_H