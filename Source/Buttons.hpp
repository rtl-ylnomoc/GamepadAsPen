#pragma once
#include "Constants.hpp"
#include "Globals.hpp"


inline void AddCloseButton(HWND appHWnd) {
	HWND closeButton = CreateWindow(
		TEXT("BUTTON"),
		TEXT("X"), // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON, // Styles
		width - 20 - 10, // x: appwidth - btnwidth - offset from border
		1 + 10, // y: magical 1 (needed for symmetry with horizontal offset) + offset from border
		20, // Button width
		20, // Button height
		appHWnd, // Parent window
		reinterpret_cast<HMENU>(2),
		wc.hInstance,
		NULL);
	SendMessage(closeButton, BM_SETIMAGE, IMAGE_ICON,
		reinterpret_cast<LPARAM>(
			LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_CLOSE_ICON))));
}

inline void AddMinimizeButton(HWND appHWnd) {
	HWND minimizeButton = CreateWindow(
		TEXT("BUTTON"),
			TEXT("-"), // Button text
				WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON, // Styles
				width - 20 - 10 - 10 - 20, // x: appwidth - btnwidth - offset from border - padding - btnwdith2
				1 + 10,
				20,
				20,
				appHWnd,
				reinterpret_cast<HMENU>(1),
				wc.hInstance,
				NULL);
	SendMessage(minimizeButton, BM_SETIMAGE, IMAGE_ICON,
		reinterpret_cast<LPARAM>(
			LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_MINIMIZE_ICON))));
}

inline void AddCheckBox(HWND appHWnd) {
	HWND checkBox = CreateWindow(TEXT("BUTTON"),
		TEXT(" Ctrl->Alt"),
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		10, 15, // x, y
		80, 12, // width, height
		appHWnd,
		reinterpret_cast<HMENU>(0),
		wc.hInstance,
		NULL);
	SendMessage(checkBox,	// font is invisible on white bg lol i leave it like that
		WM_SETFONT,
		reinterpret_cast<WPARAM>(
			CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, TEXT("Cascadia Code"))),
		TRUE);
}