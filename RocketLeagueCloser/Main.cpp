#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//#define ID_TRAY_TEST 23984722

#include <thread>
#include <mutex>

#include <string>

#include "resource.h"
#include "RocketLeagueCloser.h"

UINT WM_TASKBAR = 0;
HWND Hwnd;
HMENU Hmenu;
NOTIFYICONDATA notifyIconData;
TCHAR szTIP[64] = TEXT("RocketLeagueCloser");
char szClassName[] = "RocketLeagueCloser";

LRESULT CALLBACK windowProcedure(HWND, UINT, WPARAM, LPARAM);
void minimize();
void restore();
void initNotifyIconData();

RocketLeagueCloser rlc;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	/*AllocConsole();
	freopen("CONOUT$", "w", stdout);*/

	rlc.run();

	MSG messages;
	WNDCLASSEX wincl;
	WM_TASKBAR = RegisterWindowMessageA("TaskbarCreated");

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = windowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);

	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	if (!RegisterClassEx(&wincl)) {
		return 0;
	}

	Hwnd = CreateWindowEx(
		0,
		szClassName,
		szClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		170,
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	SetWindowLong(Hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);

	initNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);

	restore();

	while (GetMessage(&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return messages.wParam;
}

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBAR && !IsWindowVisible(Hwnd)) {
		Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		return 0;
	}

	switch (message) {

	case WM_ACTIVATE:
		Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		break;

	case WM_CREATE:
		ShowWindow(Hwnd, SW_HIDE);
		Hmenu = CreatePopupMenu();
		//AppendMenu(Hmenu, MF_STRING, ID_TRAY_TEST, TEXT("Test"));
		AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
		break;

	case WM_SYSCOMMAND:
		switch (wParam & 0xFFF0) {
		case SC_MINIMIZE:
		case SC_CLOSE:
			minimize();
			return 0;
			break;
		}
		break;

	case WM_SYSICON: {
		if (lParam == WM_LBUTTONUP) {
			restore();
			SetForegroundWindow(Hwnd);
		}
		else if (lParam == WM_RBUTTONDOWN) {
			POINT curPoint;
			GetCursorPos(&curPoint);
			SetForegroundWindow(Hwnd);
			UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL);
			SendMessage(hwnd, WM_NULL, 0, 0);
			switch (clicked) {
			case ID_TRAY_EXIT: {
				Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
				PostQuitMessage(0);
				break;
			}
			/*case ID_TRAY_TEST: {
				rlc.test();
				break;
			}*/
			}
		}
	}
					 break;

	case WM_NCHITTEST: {
		UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
		if (uHitTest == HTCLIENT)
			return HTCAPTION;
		else
			return uHitTest;
	}

	case WM_CLOSE:
		minimize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


void minimize() {

}


void restore() {
	rlc.open();
}

void initNotifyIconData() {
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));
	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = Hwnd;
	notifyIconData.uID = ID_TRAY_APP_ICON;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIconData.uCallbackMessage = WM_SYSICON;
	notifyIconData.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	strncpy_s(notifyIconData.szTip, szTIP, sizeof(szTIP));

	notifyIconData.uVersion = 0;
	Shell_NotifyIcon(NIM_SETVERSION, &notifyIconData);
}
