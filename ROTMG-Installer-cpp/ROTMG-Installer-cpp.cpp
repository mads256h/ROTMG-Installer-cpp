// ROTMG-Installer-cpp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ROTMG-Installer-cpp.h"
#include "steps.h"

#include <Commctrl.h>
#pragma comment(lib, "comctl32.lib")


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	INITCOMMONCONTROLSEX cc;
	cc.dwSize = sizeof(cc);
	cc.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES | ICC_PROGRESS_CLASS;

	InitCommonControlsEx(&cc);


	Steps::Run(hInstance);

	return 0;
}
