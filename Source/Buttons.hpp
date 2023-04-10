#pragma once
#include "Constants.hpp"
#include "Globals.hpp"

inline void AddBackground(const HWND appHWnd) {
	CreateWindow(
		// window for removing weird nontransparent white borders and providing background for all other buttons
		TEXT("STATIC"),
		0,
		WS_VISIBLE | WS_CHILD,
		0, 0, // x, y
		WIDTH, HEIGHT, // width, height
		appHWnd,
		0,
		wc.hInstance,
		NULL);
}

inline void AddCaptions(const HWND appHWnd) {
	SendMessage(CreateWindow( // label window
		TEXT("STATIC"),
		TEXT("Gamepad As Pen"), // label text
		WS_VISIBLE | WS_CHILD,
		10, // x
		12, // y: offset from border + checkbox height
		WIDTH - 10 * 2, 30, // width, height
		appHWnd,
		0,
		wc.hInstance,
		NULL),
	            WM_SETFONT,
	            reinterpret_cast<WPARAM>(
		            CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		                       DEFAULT_PITCH | FF_DONTCARE, TEXT("Cascadia Code"))),
	            TRUE);
	SendMessage(CreateWindow( // minimize window
		TEXT("BUTTON"),
		TEXT("-"), // button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON | BS_FLAT, // styles
		WIDTH - 20 - 10 - 10 - 20, // x: appwidth - clsbtnwidth - offset from border - padding - minbtnwidth
		1 + 10, // y
		20, 20, // width, height
		appHWnd,
		reinterpret_cast<HMENU>(2),
		wc.hInstance,
		NULL),
	            BM_SETIMAGE, IMAGE_ICON,
	            reinterpret_cast<LPARAM>(
		            LoadIcon(wc.hInstance, MAKEINTRESOURCE(MINIMIZE_ICON))));
	SendMessage(CreateWindow( // close window
		TEXT("BUTTON"),
		TEXT("X"), // button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON, // styles
		WIDTH - 20 - 10, // x: appwidth - btnwidth - offset from border
		1 + 10, // y: magical 1 (needed for symmetry with horizontal offset) + offset from border
		20, 20, // width, height
		appHWnd,
		reinterpret_cast<HMENU>(3),
		wc.hInstance,
		NULL),
	            BM_SETIMAGE, IMAGE_ICON,
	            reinterpret_cast<LPARAM>(
		            LoadIcon(wc.hInstance, MAKEINTRESOURCE(CLOSE_ICON))));
}

// font is invisible on white bg... upd. after days of trying to fix it ...
// upd.. fixed it during another night 4:40 now (gdi client area extension was the issue)
inline void AddColorPickerToggler(const HWND appHWnd) {
	SendMessage(CreateWindow( // color picker window
		TEXT("BUTTON"),
		TEXT(" Ctrl+LMB -> Alt+LMB"), // checkbox text
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, // styles
		20, // x
		40, // y: mostly magic now cuz forgot but looks good
		8 * 20, 30, // width: char_width * chars, height
		appHWnd,
		reinterpret_cast<HMENU>(1),
		wc.hInstance,
		NULL),
	            WM_SETFONT,
	            reinterpret_cast<WPARAM>(
		            CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		                       DEFAULT_PITCH | FF_DONTCARE, TEXT("Cascadia Code"))),
	            TRUE);
}
