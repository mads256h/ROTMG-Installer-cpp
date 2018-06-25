#pragma once
#include <Windows.h>

void MemoryPatcher()
{
	HANDLE hProcess = GetCurrentProcess();


	const CHAR overwrite[] = { 'X', 'X', 'X' };

	unsigned long long addr = 0x008E0000;
	unsigned long long possibleAddr = addr;
	int state = 0;
	int error = 0;
	do
	{
		SetLastError(0);

		CHAR buffer1[512];

		ReadProcessMemory(hProcess, (void*)addr, &buffer1, sizeof(buffer1), NULL);

		error = GetLastError();

		for (size_t i = 0; i < 512; i++)
		{
			CHAR buffer = buffer1[i];

			switch (state)
			{
			case 0:
				if (buffer == 'F')
				{
					possibleAddr = addr + i;
					state = 1;
				}
				else
				{
					state = 0;
				}
				break;

			case 1:
				if (buffer == 'W')
				{
					state = 2;
				}
				else
				{
					state = 0;
				}
				break;

			case 2:
				if (buffer == 'S')
				{
					WriteProcessMemory(hProcess, reinterpret_cast<void*>(possibleAddr), overwrite, sizeof(overwrite), NULL);
					state = 0;
				}
				state = 0;

			default:
				state = 0;
				break;
			}

		}

		addr += sizeof(buffer1);

	} while (error == 0 || error == 299 && addr <= 0x20000000);
}
