#pragma once

#ifndef ANTIDECOMPILER_H
#define ANTIDECOMPILER_H

#include <thread>
#include <chrono>

#include <windows.h>

#include "process.h"
#include "converter.h"

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
	//The function that is ran inside the thread.
	static void start()
	{
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
			L"ffdec.exe" };

		//Used to use the secounds literal.
		using namespace std::chrono_literals;

		//Used to kill JPEXS.
		bool killJava = false;
		//Run until the program quits.
		while (true)
		{
			//Get all running processes.
			auto processes = Process::GetProcesses();

			//For each process in processes.
			for (Process process : processes)
			{
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

			//Sleep the thread. So it does not use as many resources.
			std::this_thread::sleep_for(0.5s);
		}
	}
};

#endif // ANTIDECOMPILER_H