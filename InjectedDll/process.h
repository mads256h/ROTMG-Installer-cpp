#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <thread>
#include <chrono>

#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "converter.h"

//The interface for Process.
class Process
{
public:
	static Process GetProcessByName(const std::wstring& processName);
	static std::vector<Process> GetProcesses();
	static Process Run(const std::wstring& path);
	static BOOL IsMainWindow(HWND handle);

	std::wstring Name;
	DWORD Id;

	bool Kill();
	HANDLE GetHandle(DWORD access = PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION) const;
	HWND GetMainWindow() const;
	void WaitForExit() const;
	void WaitForMainWindow() const;
	void SetIcon(HICON hIcon) const;
	void SetTitle(const std::wstring& title) const;
	void DisableResizing() const;

private:
	Process(const std::wstring& name, DWORD id);
};

#endif // PROCESS_H