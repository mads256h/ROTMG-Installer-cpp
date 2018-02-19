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
	static Process GetProcessByName(std::wstring filename);
	static std::vector<Process> GetProcesses();
	static Process Run(std::wstring path);
	static BOOL IsMainWindow(HWND handle);

	std::wstring Name;
	DWORD Id;
	HANDLE Handle;

	bool Kill();
	HWND GetMainWindow() const;
	void WaitForExit() const;
	void WaitForMainWindow() const;
	void SetIcon(HICON hIcon) const;
	void SetTitle(std::wstring title) const;
	void DisableResizing() const;
	
	~Process();

private:
	Process(std::wstring name, DWORD id, HANDLE handle);
	

};

#endif // PROCESS_H