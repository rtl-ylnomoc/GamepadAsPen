#pragma once
#include "Buttons.hpp"
#include "Globals.hpp"
#include "Constants.hpp"
#include "ShortcutInputHandlers.hpp"
#include "PenInputHandlers.hpp"


inline LRESULT HandleWindowCreate(const HWND hWnd) {
	SetTimer(hWnd, 0, USER_TIMER_MINIMUM, nullptr); // timer for updating simulated tablet pen when no WM_INPUT
	AddBackground(hWnd); // order of windows matter
	AddCaptions(hWnd);
	AddColorPickerToggler(hWnd);
	SendMessage(hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0); // should remove selection borders
	ShowWindow(hWnd, SW_RESTORE); // showing here because it fixes weird bugs.
	return EXIT_SUCCESS;
}

inline LRESULT HandleWindowCommand(const HWND hWnd, const WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case 1: {
		// Checkbox that toggles between Ctrl and Alt buttons for color picker shortcut
		CtrAlt += CtrAlt == VK_LCONTROL ? 2 : -2; // VK_LCONTROL and VK_LMENU are two values apart
		return EXIT_SUCCESS;
	}
	case 2: {
		// Minimize button
		ShowWindow(hWnd, SW_FORCEMINIMIZE);
		return EXIT_SUCCESS;
	}
	case 3: {
		// Close button
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		return EXIT_SUCCESS;
	}
	default: return EXIT_SUCCESS;
	}
}

inline LRESULT HandleNonClientHitTest(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	LRESULT result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	switch (result) {
	case HTCLIENT: // Makes client area movable as if it's non-client caption area.
		return HTCAPTION;
	default:
		return result;
	}
}

inline LRESULT HandleGamepadInput(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
#ifdef _DEBUG
	bufPtr = 0;
	start = std::chrono::steady_clock::now();
	_RPT0(_CRT_WARN, "GAMEPAD SENT RAWINPUT: HANDLING\n");
#endif
	// GAMEPAD
	// GETTING RAW INPUT

	if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pRaw, &dwSize, sizeof(RAWINPUTHEADER)) == -1) {
		_RPTF1(_CRT_ERROR, "GET RAWINPUT DATA FAILED [%3d]\n", GetLastError()); // dwSizeHid = 15, count = 1
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	_RPT1(_CRT_WARN, "RAW RETURN [%3d]\t LPARAM [%6d]\n", dwSize, lParam);
	// GAMEPAD KEYS SHORTCUTS
	gpadBtns = pRaw->data.hid.bRawData[11] & ~GP_RB; // RB is used for pen barrel button
	stickDpadBtns = pRaw->data.hid.bRawData[12];
	HandleShortcuts(inpSent, inputs, gpadBtns, stickDpadBtns);
	// PEN POINTER
	// STATE INITIALIZATION
	dwKeyStates = 0;
	curPressure = ConvertPressure(pRaw->data.hid.bRawData[10]);
	pointerFlags = POINTER_FLAG_CONFIDENCE; // initial flags (POINTER_FLAG_PRIMARY doesn't do anything I think)
	ButtonChangeType = POINTER_CHANGE_NONE;
	// KEY MODIFIERS
	HandlePenModifiersState(ptInfo, dwKeyStates); // using globals a lot to not deal with multiple return values
	// BARREL
	HandlePenBarrelStateChange(ptInfo, pointerFlags, ButtonChangeType);
	// PRESSURE (overrides barrel button change)
	HandlePenPressureStateChange(ptInfo, curPressure, pointerFlags, ButtonChangeType);
	// ROTATION (these two are using global locks implicitly)
	HandlePenRotationChange(ptInfo);
	// TILT
	HandlePenTiltChange(ptInfo);
	// CURSOR POSITION (follows mouse)
	if (!GetCursorPos(&curPos)) {
		_RPTF1(_CRT_ERROR, "GET CURSOR POSITION FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	ptInfo->penInfo.pointerInfo.ptPixelLocation = curPos;
	ptInfo->penInfo.pointerInfo.ptPixelLocationRaw = curPos;
	// INJECTING POINTER INPUT
	if (!InjectSyntheticPointerInput(hPen, ptInfo, 1)) {
		_RPTF1(_CRT_ERROR, "INJECT POINTER INPUT FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// DEBUG INFO (wrapped in ifdef mainly to hide it in VS)
#ifdef _DEBUG
	bufPtr += sprintf_s(buffer + bufPtr, BUF_SIZE - bufPtr, "CURSOR POS: %4ld,%4ld\n", curPos.x, curPos.y);

	counter++;
	bufPtr += sprintf_s(buffer + bufPtr, BUF_SIZE - bufPtr, "RS SHORT X, Y: (%5u, %5u)\t",
	                    pRaw->data.hid.bRawData[5] | pRaw->data.hid.bRawData[6] << 8,
	                    pRaw->data.hid.bRawData[7] | pRaw->data.hid.bRawData[8] << 8);
	bufPtr += sprintf_s(buffer + bufPtr, BUF_SIZE - bufPtr, "LS SHORT X, Y: (%5u, %5u)\n",
	                    pRaw->data.hid.bRawData[1] | pRaw->data.hid.bRawData[2] << 8,
	                    pRaw->data.hid.bRawData[3] | pRaw->data.hid.bRawData[4] << 8);
	bufPtr += sprintf_s(buffer + bufPtr, BUF_SIZE - bufPtr, "[ 0]: %3d ", pRaw->data.hid.bRawData[0]);
	for (byte i = 9; i < 15; ++i) // other gamepad data bytes
	{
		bufPtr += sprintf_s(buffer + bufPtr, BUF_SIZE - bufPtr, "[%2d]: %3d, ", i, pRaw->data.hid.bRawData[i]);
	}
	_RPT0(_CRT_WARN, buffer);

	_RPT4(_CRT_WARN, "\nRAW INPUT AMOUNT %3d SIZE %2d\nPRESSURE: %3d FLAGS: %5d\n",
	      pRaw->data.hid.dwCount, pRaw->data.hid.dwSizeHid, // const 1 and 15 but size is 32 in descriptor
	      ptInfo->penInfo.pressure, ptInfo->penInfo.pointerInfo.pointerFlags);

	_RPT1(_CRT_WARN, "Between WM_INPUT time: %5d\t",
	      static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>((std::chrono::steady_clock::now
		      () - end)).count()));
	end = std::chrono::steady_clock::now();
	_RPT1(_CRT_WARN, "WM_INPUT handling time: %4d [microsecs]\n\n",
	      static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>((end - start)).count()));
#endif

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline LRESULT HandleTimerTick(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	// Case for updating pen pointer when there's no gamepad input. Otherwise the cursor position will be reset when there's no WM_INPUT.
	// STATE INITIALIZATION (no pressure change)
	dwKeyStates = 0;
	pointerFlags = POINTER_FLAG_CONFIDENCE | POINTER_FLAG_UPDATE; // initial flags for updating
	ButtonChangeType = POINTER_CHANGE_NONE;
	// KEY MODIFIERS
	HandlePenModifiersState(ptInfo, dwKeyStates);
	// BARREL
	HandlePenBarrelStateUpdate(ptInfo, pointerFlags);
	// PRESSURE
	HandlePenPressureStateUpdate(ptInfo, pointerFlags);
	// CURSOR POSITION
	if (!GetCursorPos(&curPos)) {
		_RPTF1(_CRT_ERROR, "GET CURSOR POSITION FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	ptInfo->penInfo.pointerInfo.ptPixelLocation = curPos;
	ptInfo->penInfo.pointerInfo.ptPixelLocationRaw = curPos;
	// INJECTING POINTER INPUT
	if (!InjectSyntheticPointerInput(hPen, ptInfo, 1)) {
		_RPTF1(_CRT_ERROR, "INJECT POINTER INPUT FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline LRESULT HandleNonClientDoubleClick(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (wParam) {
	case HTCAPTION: // Disables fullscreen on double click.
		return EXIT_SUCCESS;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline LRESULT HandleWindowDestroy() {
	_RPT1(_CRT_WARN, "EXIT TIME [%3ld]\n", std::chrono::steady_clock::now());
	if (shiftPressed) {
		DepressShift();
	}
	if (inpSent) {
		DepressGamepadDpadButtons(inpSent, inputs);
	}
	PostQuitMessage(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

inline LRESULT CALLBACK HandleMessages(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:				return HandleWindowCreate(hWnd);

	case WM_COMMAND:			return HandleWindowCommand(hWnd, wParam);

	case WM_INPUT:				return HandleGamepadInput(hWnd, uMsg, wParam, lParam);

	case WM_TIMER:				return HandleTimerTick(hWnd, uMsg, wParam, lParam);

	case WM_NCHITTEST:			return HandleNonClientHitTest(hWnd, uMsg, wParam, lParam);

	case WM_NCLBUTTONDBLCLK:	return HandleNonClientDoubleClick(hWnd, uMsg, wParam, lParam);

	case WM_DESTROY:			return HandleWindowDestroy();

	default:					return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
