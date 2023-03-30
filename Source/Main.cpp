#pragma comment (lib, "dwmapi.lib")
#include "Constants.hpp"
#include "Globals.hpp"
#include "MessageHandlers.hpp"
#include <dwmapi.h>
#include <Windows.h>

// Using lots of globals to use stack and allocations less.
// Also, the window procedure then would not match the lpfWndProc signature apparently.
// BUG FIXED: when color picker action was performed on the window caption buttons, the window stops receiving messages
// upd. removed most of built-in winapi app functionality that causes problems.

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, TCHAR* szCmdLine, int nCmdShow) {
	MSG msg{};
	HWND hWnd{};
	// wc is global
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = HandleMessages;
	wc.lpszClassName = TEXT("GPADASPEN");
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DROPSHADOW;

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	// CREATE APP WINDOW
	// for some reason can't remove borders using flags in this function
	hWnd = CreateWindowEx(WS_EX_LAYERED, wc.lpszClassName, TEXT("Gamepad As Pen"), WS_OVERLAPPEDWINDOW, 0, 0, width, height,
	                      nullptr, nullptr, wc.hInstance, nullptr);
	if (hWnd == INVALID_HANDLE_VALUE) {
		return EXIT_FAILURE;
	}

	// REMOVE BORDERS
	LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
	SetWindowLong(hWnd, GWL_STYLE, lStyle);
	_RPTF1(_CRT_WARN, "STYLES [%3d]\n", GetWindowLong(hWnd, GWL_STYLE));

	// MAKE TRANSPARENT
	SetLayeredWindowAttributes(hWnd, 0, 230, LWA_ALPHA); // colorkey doesn't work sadly

	// ADD SHADOW
	const MARGINS shadow_on = {0, -10, 0, -10};
	DwmExtendFrameIntoClientArea(hWnd, &shadow_on);

	// CENTER WINDOW
	HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	auto lpmi = new MONITORINFO{sizeof(MONITORINFO)};
	short x = 710; // assume user monitor has 1920x1080 resolution
	short y = 290;
	if (!GetMonitorInfo(hMon, lpmi)) {
		_RPTF1(_CRT_ERROR, "GET MONITOR INFO FAILED [%3d]\n", GetLastError());
	}
	else {
		x = lpmi->rcMonitor.right - (lpmi->rcMonitor.right - lpmi->rcMonitor.left + width) / 2;
		y = lpmi->rcMonitor.bottom - (lpmi->rcMonitor.bottom - lpmi->rcMonitor.top + height) / 2;
	}
	SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE);

	// REGISTER GAMEPAD (don't know if raw input solution works for non xbox controllers)
	const auto Rid = new RAWINPUTDEVICE;
	Rid->usUsagePage = 0x01; // HID_USAGE_PAGE_GENERIC
	Rid->usUsage = 0x05;	// HID_USAGE_GENERIC_GAMEPAD
	Rid->dwFlags = RIDEV_INPUTSINK;
	Rid->hwndTarget = hWnd;
	if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]))) {
		_RPTF1(_CRT_ERROR, "REGISTER DEVICE FAILED [%3d]\n", GetLastError());
		return EXIT_FAILURE;
	}

	// PEN POINTER STATUS CHECK (defined in globals)
	if (!hPen) {
		_RPTF1(_CRT_ERROR, "CREATE SYNTHETIC POINTER FAILED [%3d]\n", GetLastError());
		return EXIT_FAILURE;
	}

	// START MESSAGE HANDLING LOOP (ShowWindow() moved in WM_CREATE to avoid some visual bugs)
	UpdateWindow(hWnd);
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}
