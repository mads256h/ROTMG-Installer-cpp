#include "process.h"
#include <sstream>
#include "moviedeleter.h"
#include "file.h"
#include "constants.h"

class ProcessCouldNotStartException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The process could not start!";
	}
} processCouldNotStartException;

class NoMainWindowHandleException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The process does not have a main window";
	}
} noMainWindowHandleException;

struct HandleData {
	DWORD processId;
	HWND bestHandle;
};

//Gets a Process object by name
Process Process::GetProcessByName(const std::wstring& processName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		//If the process name and the current looped process match return it.
		if (_wcsicmp(pEntry.szExeFile, processName.c_str()) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				static_cast<DWORD>(pEntry.th32ProcessID));
			if (hProcess != NULL)
			{
				Process proc(pEntry.szExeFile, pEntry.th32ProcessID);
				CloseHandle(hProcess);
				CloseHandle(hSnapShot);
				return proc;
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);

	//We could not find it return this.
	//TODO: Throw an exception instead.
	Process proc(L"", 0);

	return proc;

}

//Gets a list of all running processes.
std::vector<Process> Process::GetProcesses()
{
	std::vector<Process> processes;

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	//Loop through all processes and add it to the list.
	while (hRes)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pEntry.th32ProcessID));
		if (hProcess != NULL)
		{
			Process proc(pEntry.szExeFile, pEntry.th32ProcessID);
			CloseHandle(hProcess);
			processes.push_back(proc);
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);

	//Return the list.
	return processes;
}

//Runs a executable.
Process Process::Run(const std::wstring& path)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInfo, sizeof(processInfo));

	WCHAR* cmdline = new WCHAR[MAX_PATH];

	wcscpy_s(cmdline, MAX_PATH, path.c_str());

	if (CreateProcess(NULL,
		cmdline,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&processInfo) == FALSE)
	{
		MovieDeleter::DeleteKeys();

		if (File::Exists(DecryptedClientLocation))
		{
			File::Delete(DecryptedClientLocation);
		}

		if (File::Exists(ProcessIdLocation))
		{
			File::Delete(ProcessIdLocation);
		}

		std::stringstream ss;
		ss << "Process could not start!\r\n";
		ss << "Error code: ";
		ss << GetLastError();
		
		MessageBoxA(NULL, ss.str().c_str(), "Process could not start!", MB_ICONERROR);
		throw processCouldNotStartException;
	}
	//Get the process we just started and return it.
	Process process(path, processInfo.dwProcessId);

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	return process;
}

//Creates a new instance of Process. This should never be used directly.
Process::Process(const std::wstring& name, const DWORD id)
{
	Name = name;
	Id = id;
}


//Kills the process.
bool Process::Kill()
{
	HANDLE Handle = GetHandle();
	if (Handle != NULL)
	{
		TerminateProcess(Handle, 9);
		CloseHandle(Handle);
		
		Name = L"";
		Id = 0;

		return true;
	}

	return false;
}

//Used to get the main window handle.
BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	HandleData& data = *reinterpret_cast<HandleData*>(lParam);
	DWORD processId = 0;
	GetWindowThreadProcessId(handle, &processId);
	if (data.processId != processId || !Process::IsMainWindow(handle))
	{
		return TRUE;
	}
	data.bestHandle = handle;
	return FALSE;
}

HANDLE Process::GetHandle(DWORD access) const
{
	return OpenProcess(access, FALSE, Id);
}



//Gets the process's main window handle.
HWND Process::GetMainWindow() const
{
	HandleData data;
	data.processId = Id;
	data.bestHandle = 0;
	EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));
	return data.bestHandle;
}

//Waits until the program exists.
void Process::WaitForExit() const
{
	HANDLE Handle = GetHandle();
	DWORD exitcode;
	GetExitCodeProcess(Handle, &exitcode);
	while (exitcode == STILL_ACTIVE && Handle != NULL)
	{
		using namespace std::chrono_literals;

		std::this_thread::sleep_for(1s);

		GetExitCodeProcess(Handle, &exitcode);
	}

	if (Handle != NULL)
	CloseHandle(Handle);
}

//Waits until the main window is created.
void Process::WaitForMainWindow() const
{
	while (GetMainWindow() == static_cast<HWND>(0))
	{
		using namespace std::chrono_literals;

		std::this_thread::sleep_for(0.2s);
	}
}

//Sets the main windows icon.
void Process::SetIcon(HICON hIcon) const
{
	HWND mainWindow = GetMainWindow();
	if (mainWindow == static_cast<HWND>(NULL))
	{
		throw noMainWindowHandleException;
	}
	
	SendMessage(mainWindow, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	SendMessage(mainWindow, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
}

//Sets the main windows title.
void Process::SetTitle(const std::wstring& title) const
{
	HWND mainWindow = GetMainWindow();
	if (mainWindow == static_cast<HWND>(NULL))
	{
		throw noMainWindowHandleException;
	}

	SetWindowText(mainWindow, title.c_str());
}

//Disable resizing main window.
void Process::DisableResizing() const
{
	HWND mainWindow = GetMainWindow();
	if (mainWindow == static_cast<HWND>(NULL))
	{
		throw noMainWindowHandleException;
	}

	HMENU hSysmenu = GetSystemMenu(mainWindow, FALSE);

	DeleteMenu(hSysmenu, SC_MAXIMIZE, MF_BYCOMMAND);
	DeleteMenu(hSysmenu, SC_SIZE, MF_BYCOMMAND);

	auto style = GetWindowLong(mainWindow, GWL_STYLE);

	style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);

	SetWindowLong(mainWindow, GWL_STYLE, style);

}

//Checks if the handle is the main window.
BOOL Process::IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == static_cast<HWND>(NULL) && IsWindowVisible(handle);
}

