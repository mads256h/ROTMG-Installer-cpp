// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "antidecompiler.h"
#include "memorypatcher.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(AntiDecompiler::Start), static_cast<void*>(NULL), NULL, NULL);
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MemoryPatcher), NULL, NULL, NULL);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

