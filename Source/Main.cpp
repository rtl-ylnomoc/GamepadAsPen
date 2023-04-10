#include "Constants.hpp"
#include "Globals.hpp"
#include "MessageHandlers.hpp"
#include <Windows.h>

// Using lots of globals to use stack and allocations less.
// Also, the window procedure then would not match the lpfWndProc signature apparently.
// upd. Removed most of built-in winapi app functionality that causes problems.

int CALLBACK wWinMain(const HINSTANCE hInstance, const HINSTANCE, TCHAR* szCmdLine, const int nCmdShow) {
	MSG msg{};
	// wc is global
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(APP_ICON));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = HandleMessages;
	wc.lpszClassName = TEXT("GAMEPADASPEN");
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DROPSHADOW;
	if (!RegisterClassEx(&wc)) {
		return EXIT_FAILURE;
	}

	// CREATE APP WINDOW
	constexpr long x = (1920 - WIDTH) / 2; // center coordinates (if user monitor has 1920x1080 resolution)
	constexpr long y = (1080 - HEIGHT) / 2;
	// for some reason can't remove borders using flags in this function
	HWND hWnd = CreateWindowEx(WS_EX_LAYERED, wc.lpszClassName, TEXT("Gamepad As Pen"), WS_OVERLAPPEDWINDOW,
	                           x, y, // default centered position,
	                           WIDTH, HEIGHT, // dimensions
	                           nullptr, nullptr, wc.hInstance, nullptr);
	if (hWnd == INVALID_HANDLE_VALUE) {
		return EXIT_FAILURE;
	}

	// CENTER WINDOW
	const auto lpmi = new MONITORINFO{sizeof(MONITORINFO)};
	if (!GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), lpmi)) {
		_RPTF1(_CRT_ERROR, "GET MONITOR INFO FAILED [%3d]\n", GetLastError());
	}
	else {
		long x = lpmi->rcMonitor.right - (lpmi->rcMonitor.right - lpmi->rcMonitor.left + WIDTH) / 2;
		long y = lpmi->rcMonitor.bottom - (lpmi->rcMonitor.bottom - lpmi->rcMonitor.top + HEIGHT) / 2;
	}
	SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE);

	// REMOVE BORDERS
	SetWindowLong(
		hWnd,
		GWL_STYLE,
		GetWindowLong(hWnd, GWL_STYLE) & ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU));
	_RPTF1(_CRT_WARN, "STYLES   [%8x]\n", GetWindowLong(hWnd, GWL_STYLE));

	SetWindowLong(
		hWnd,
		GWL_EXSTYLE,
		GetWindowLong(hWnd, GWL_EXSTYLE)) & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	_RPTF1(_CRT_WARN, "EXSTYLES [%8x]\n", GetWindowLong(hWnd, GWL_EXSTYLE));

	// MAKE TRANSPARENT
	SetLayeredWindowAttributes(hWnd, 0, 230, LWA_ALPHA); // color key doesn't work sadly

	// REGISTER GAMEPAD (no clue if raw input solution works for non xbox controllers)
	if (!RegisterRawInputDevices(
		new RAWINPUTDEVICE{
			.usUsagePage = 0x01,	// HID_USAGE_PAGE_GENERIC
			.usUsage = 0x05,		// HID_USAGE_GENERIC_GAMEPAD
			.dwFlags = RIDEV_INPUTSINK,
			.hwndTarget = hWnd
		},
		1,
		sizeof(RAWINPUTDEVICE))) {
		_RPTF1(_CRT_ERROR, "REGISTER DEVICE FAILED [%3d]\n", GetLastError());
		return EXIT_FAILURE;
	}

	// CREATE PEN POINTER (declared in globals)
	hPen = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_DEFAULT);
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
