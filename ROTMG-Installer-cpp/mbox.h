#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <mutex>

#include <Windows.h>

#include <boost/thread/thread.hpp>

#include "resource.h"
#include <winuser.h>


static std::mutex enforceOneThread;

static boost::thread thread;

static std::atomic<HWND> dialog;



class MBox
{
public:

	static void Create(const std::wstring& status1, const std::wstring& status2)
	{
		enforceOneThread.lock();
		thread = boost::thread(run, status1, status2);
		enforceOneThread.unlock();
	}

	static void SetStatus1(const std::wstring& text)
	{
		SetDlgItemText(dialog, IDC_STATIC_STATUS, text.c_str());
	}

	static void SetStatus2(const std::wstring& text)
	{
		SetDlgItemText(dialog, IDC_STATIC_STATUS2, text.c_str());
	}

	static void SetProgress(const int pos)
	{

		HWND progressBar = GetDlgItem(dialog, IDC_PROGRESS_PROGRESS);

		//LRESULT l1 = SendMessage(progressBar, PBM_SETRANGE, NULL, HIWORD(max));
		LRESULT l2 = SendMessage(progressBar, PBM_SETPOS, pos, NULL);
	}

	static void Destroy()
	{
		ShowWindow(dialog, SW_HIDE);
		DestroyWindow(dialog);
		thread.interrupt();
	}


	MBox() = delete;

private:
	static void run(const std::wstring& status1, const std::wstring& status2)
	{
		enforceOneThread.lock();

		dialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG_DOWNLOAD), NULL, NULL);
		ShowWindow(dialog, SW_SHOW);

		SetDlgItemText(dialog, IDC_STATIC_STATUS, status1.c_str());

		SetDlgItemText(dialog, IDC_STATIC_STATUS2, status2.c_str());

		HWND hProgressbar = GetDlgItem(dialog, IDC_PROGRESS_PROGRESS);

		SendMessage(dialog, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));

		MSG msg;



		while (true)
		{
			getMessage(&msg, dialog);
			
			if (boost::this_thread::interruption_requested())
				break;

		}
		enforceOneThread.unlock();
	}

	static void getMessage(LPMSG msg, HWND handle)
	{
		if (MsgWaitForMultipleObjects(0, NULL, FALSE, 10, QS_ALLINPUT) == WAIT_OBJECT_0)
		{
			while (PeekMessage(msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(msg);
				DispatchMessage(msg);
			}
		}
	}
};
