// ROTMG-Installer-cpp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ROTMG-Installer-cpp.h"
#include "steps.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	Steps::Run(hInstance);

	return 0;
}
