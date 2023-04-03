#pragma once
#include "Constants.hpp"
#include "Globals.hpp"


inline void AddCloseButton(const HWND appHWnd) {
	SendMessage(CreateWindow(
		TEXT("BUTTON"),
		TEXT("X"),			// Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON, // Styles
		WIDTH - 20 - 10,	// x: appwidth - btnwidth - offset from border
		1 + 10,				// y: magical 1 (needed for symmetry with horizontal offset) + offset from border
		20, 20,				// width, height
		appHWnd,			// Parent window
		reinterpret_cast<HMENU>(2),
		wc.hInstance,
		NULL),
		BM_SETIMAGE, IMAGE_ICON,
		reinterpret_cast<LPARAM>(
			LoadIcon(wc.hInstance, MAKEINTRESOURCE(CLOSE_ICON))));
}

inline void AddMinimizeButton(const HWND appHWnd) {
	SendMessage(CreateWindow(
		TEXT("BUTTON"),
		TEXT("-"),			// Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON, // Styles
		WIDTH - 20 - 10 - 10 - 20, // x: appwidth - clsbtnwidth - offset from border - padding - minbtnwidth
		1 + 10,
		20, 20,
		appHWnd,
		reinterpret_cast<HMENU>(1),
		wc.hInstance,
		NULL),
		BM_SETIMAGE, IMAGE_ICON,
		reinterpret_cast<LPARAM>(
			LoadIcon(wc.hInstance, MAKEINTRESOURCE(MINIMIZE_ICON))));
}

inline void AddCheckBox(const HWND appHWnd) {
	SendMessage(CreateWindow(
		TEXT("BUTTON"),
		TEXT(" Ctrl->Alt"),
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		10, 15,				// x, y
		80, 12,				// width, height
		appHWnd,
		reinterpret_cast<HMENU>(0),
		wc.hInstance,
		NULL),	// font is invisible on white bg (after days of trying to fix it i give up. I've found a solution, but it may break a lot of code. I've gathered enough vitriol for ms devs though.)
		WM_SETFONT,
		reinterpret_cast<WPARAM>(
			CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Cascadia Code"))),
		TRUE);
}