#ifndef ANTIDECOMPILER_H
#define ANTIDECOMPILER_H

#include <thread>
#include <iostream>
#include <chrono>

#include <windows.h>

#include "process.h"
#include "converter.h"

class AntiDecompiler {
public:


	static void Start()
	{
		std::thread t(start);
		t.detach();
	}


private:
	AntiDecompiler() {}



	static void start()
	{
		std::string bannedProcessNames[8] = { "FlashDecompiler.exe",
			"FlashDecompilerGold.exe",
			"SWFDecompiler.exe",
			"SWFCatcher.exe",
			"SWFEditor.exe",
			"asv30demo.exe",
			"asv30full.exe",
			"asv30.exe" };

		using namespace std::chrono_literals;

		std::cout << "Started AntiDecompiler thread!";

		while (true)
		{
			auto processes = Process::GetProcesses();
			for (Process process : processes)
			{
				for (auto processName : bannedProcessNames)
					if (_wcsicmp(process.Name.c_str(), Converter::ToWString(processName).c_str()) == 0)
						process.Kill();
			}

#ifndef _DEBUG
			if (IsDebuggerPresent() == TRUE)
			{
				MessageBox(NULL, L"A URL context could not be created!", L"WINAPI Error", MB_OK);
				exit(1);
			}
#endif _DEBUG

			std::this_thread::sleep_for(0.5s);
		}
	}
};

#endif // ANTIDECOMPILER_H
