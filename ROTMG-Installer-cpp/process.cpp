#include "process.h"

class ProcessCouldNotStartException : public std::exception
{
	virtual const char* what() const throw()
	{
		return "The process could not start!";
	}
} processCouldNotStartException;

struct HandleData {
	DWORD processId;
	HWND bestHandle;
};

Process Process::GetProcessByName(const std::wstring filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		//std::wcout << pEntry.szExeFile << L" ";

		//std::wcout << filename << "\r\n";

		if (_wcsicmp(pEntry.szExeFile, filename.c_str()) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				Process proc(pEntry.szExeFile, pEntry.th32ProcessID, hProcess);
				CloseHandle(hSnapShot);
				return proc;
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	Process proc(L"", 0, NULL);

	return proc;

}

std::vector<Process> Process::GetProcesses()
{
	std::vector<Process> processes;

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
			(DWORD)pEntry.th32ProcessID);
		if (hProcess != NULL)
		{
			Process proc(pEntry.szExeFile, pEntry.th32ProcessID, hProcess);
			processes.push_back(proc);
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);

	return processes;
}


Process Process::Run(std::wstring path)
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
		throw processCouldNotStartException;
	}

	Process process(path, processInfo.dwProcessId, processInfo.hProcess);

	return process;
}


Process::Process(const std::wstring name, DWORD id, HANDLE handle)
{
	Name = name;
	Id = id;
	Handle = handle;
}

bool Process::Kill()
{
	if (Handle != NULL)
	{
		TerminateProcess(Handle, 9);
		CloseHandle(Handle);
		return true;
	}

	return false;
}

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

HWND Process::GetMainWindow() const
{
	HandleData data;
	data.processId = Id;
	data.bestHandle = 0;
	EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&data));
	return data.bestHandle;
}


void Process::WaitForMainWindow() const
{
	while (GetMainWindow() == static_cast<HWND>(0))
	{
		std::cout << "Waiting!" << std::endl;
	}
}




BOOL Process::IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == static_cast<HWND>(0) && IsWindowVisible(handle);
}

