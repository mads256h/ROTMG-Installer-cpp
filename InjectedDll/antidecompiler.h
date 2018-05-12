#pragma once

#ifndef ANTIDECOMPILER_H
#define ANTIDECOMPILER_H

#include <thread>
#include <chrono>

#include <windows.h>

#include "process.h"
#include "converter.h"
#include "file.h"
#include "constants.h"

//Holds all anti-decompilation code. It is just a simple process detector and killer.
class AntiDecompiler {
public:

	//Runs the anti-decompilation code on another thread.
	static void Start()
	{
		start();
	}

	AntiDecompiler() = delete;

private:
	static void suspendResumeAllThreads(bool suspend)
	{
		DWORD processId = GetCurrentProcessId();
		HANDLE hThisThread = GetCurrentThread();
		DWORD threadId = GetThreadId(hThisThread);
		CloseHandle(hThisThread);
		HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
		if (h != INVALID_HANDLE_VALUE) {
			THREADENTRY32 te;
			te.dwSize = sizeof(te);
			if (Thread32First(h, &te)) {
				do {
					if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
						sizeof(te.th32OwnerProcessID)) {

						if (te.th32OwnerProcessID == processId && te.th32ThreadID != threadId)
						{
							HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

							if (suspend)
							{
								SuspendThread(hThread);
							}
							else
							{
								ResumeThread(hThread);
							}

							CloseHandle(hThread);
						}
					}
					te.dwSize = sizeof(te);
				} while (Thread32Next(h, &te));
			}
			CloseHandle(h);
		}
		return;
	}

	//The function that is ran inside the thread.
	static void start()
	{
		DWORD clientUpdaterProcId;
		if (File::Exists(ProcessIdLocation))
		{
			std::ifstream procIdReader(ProcessIdLocation);

			if (procIdReader.is_open())
			{
				procIdReader >> clientUpdaterProcId;

				procIdReader.close();
			}
			else
			{
				suspendResumeAllThreads(true);
				MessageBox(NULL, L"Please update ClientUpdater.exe", L"Update ClientUpdater.exe", MB_ICONINFORMATION);
				exit(1);
			}
		}
		else
		{
			suspendResumeAllThreads(true);
			MessageBox(NULL, L"Please update ClientUpdater.exe", L"Update ClientUpdater.exe", MB_ICONINFORMATION);
			exit(1);
		}

		//Banned processes will be killed when detected.
		const WCHAR* bannedProcessNames[] = {
			L"FlashDecompiler.exe",
			L"FlashDecompilerGold.exe",
			L"SWFDecompiler.exe",
			L"SWFCatcher.exe",
			L"SWFEditor.exe",
			L"asv30demo.exe",
			L"asv30full.exe",
			L"asv30.exe",
			L"ffdec.exe",
			L"Wireshark.exe"};

		//Used to use the secounds literal.
		using namespace std::chrono_literals;

		//Used to kill JPEXS.
		bool killJava = false;
		//Run until the program quits.
		while (true)
		{
			bool clientUpdaterFound = false;

			//Get all running processes.
			auto processes = Process::GetProcesses();

			//For each process in processes.
			for (Process process : processes)
			{
				if (process.Id == clientUpdaterProcId)
				{
					clientUpdaterFound = true;
					continue;
				}

				if (process.Name.find(L"cheatengine") != std::wstring::npos)
				{
					if (File::Exists(DecryptedClientLocation))
					{
						File::Delete(DecryptedClientLocation);
					}
					exit(1);
				}


				//Check for the javactivex executeable. If we find it kill it and set killJava to true.
				if (process.Name.find(L"javactivex") != std::wstring::npos)
				{
					process.Kill();
					killJava = true;
					continue;
				}

				//If killJava is true and we have the javaw.exe executable. Kill it and set killJava to false.
				if (killJava && _wcsicmp(process.Name.c_str(), L"javaw.exe") == 0)
				{
					process.Kill();
					killJava = false;
					continue;
				}

				//Kill the banned processes.
				for (auto processName : bannedProcessNames)
					if (_wcsicmp(process.Name.c_str(), processName) == 0)
						process.Kill();
			}

			if (!clientUpdaterFound)
			{
				if (File::Exists(DecryptedClientLocation))
				{
					File::Delete(DecryptedClientLocation);
				}

				exit(1);
			}

			//Sleep the thread. So it does not use as many resources.
			std::this_thread::sleep_for(0.5s);
		}
	}
};

#endif // ANTIDECOMPILER_H