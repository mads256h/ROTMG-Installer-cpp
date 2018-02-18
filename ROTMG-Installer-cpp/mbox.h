#pragma once

#include <string>

#include <Windows.h>

#include "resource.h"
#include <winuser.h>


class MBox
{
public:

	MBox(const std::wstring text)
	{
		dialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG_DOWNLOAD), NULL, NULL);
		ShowWindow(dialog, SW_SHOW);

		SetDlgItemText(dialog, IDC_STATIC_STATUS, text.c_str());
	}

	void Destroy()
	{
		ShowWindow(dialog, SW_HIDE);
		DestroyWindow(dialog);
		dialog = NULL;
	}

	void SetText(const std::wstring text)
	{
		SetDlgItemText(dialog, IDC_STATIC_STATUS, text.c_str());
	}

private:
	HWND dialog = NULL;
};
