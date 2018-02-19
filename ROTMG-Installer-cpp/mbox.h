#pragma once

#include <string>
#include <atomic>
#include <thread>

#include <Windows.h>

#include "resource.h"
#include <winuser.h>


static std::atomic<bool> shouldDestroy = false;

class MBox
{
public:

	static void Create(const std::wstring& text)
	{
		std::thread t(run, text);
		t.detach();
	}

	static void Destroy()
	{
		shouldDestroy = true;
	}

	MBox() = delete;

private:
	static void run(const std::wstring& text)
	{
		HWND dialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG_DOWNLOAD), NULL, NULL);
		ShowWindow(dialog, SW_SHOW);

		SetDlgItemText(dialog, IDC_STATIC_STATUS, text.c_str());

		MSG msg;

		while (GetMessage(&msg, NULL, 0, 0) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (shouldDestroy)
			{
				ShowWindow(dialog, SW_HIDE);
				DestroyWindow(dialog);
				dialog = NULL;
				return;
			}

		}
	}
};
