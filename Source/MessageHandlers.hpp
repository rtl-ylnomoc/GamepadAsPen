#pragma once
#include "Buttons.hpp"
#include "Globals.hpp"
#include "Constants.hpp"
#include "ShortcutInputHandlers.hpp"
#include "PenInputHandlers.hpp"


inline LRESULT HandleWindowCreate(const HWND hWnd) {
	SetTimer(hWnd, 0, USER_TIMER_MINIMUM, 0); // timer for updating simulated tablet pen when no WM_INPUT
	AddCheckBox(hWnd);
	AddMinimizeButton(hWnd);
	AddCloseButton(hWnd);
	ShowWindow(hWnd, SW_RESTORE); // showing here cuz it fixes initialization bugs with transparency.
	return EXIT_SUCCESS;
}

inline LRESULT HandleWindowCommand(const HWND hWnd, const WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case 0: {
		// Checkbox that toggles between Ctrl and Alt buttons for LB shortcut
		CtrAlt += CtrAlt == VK_LCONTROL ? 2 : -2;
		return EXIT_SUCCESS;
	}
	case 1: {
		// Minimize button
		// SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		ShowWindow(hWnd, SW_FORCEMINIMIZE);
		return EXIT_SUCCESS;
	}
	case 2: {
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
	cbSize = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pRaw,
		&dwSize, sizeof(RAWINPUTHEADER)); // dwSizeHid = 15, count = 1
	if (dwSize == -1) {
		_RPTF1(_CRT_ERROR, "GET RAWINPUT DATA FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	_RPTF1(_CRT_WARN, "RAW RETURN [%3d]\t LPARAM [%6d]\n", cbSize, lParam);
	// GAMEPAD KEYS SHORTCUTS
	gpadBtns = pRaw->data.hid.bRawData[11] & ~GP_RB;
	stickDpadBtns = pRaw->data.hid.bRawData[12];
	// For some reason, gamepad sends two WM_INPUTs per one button press,
	// so i have to ignore next non-zero ones and then assign locked state values when no stick buttons are pressed.
	if (stickBtnPressed && !(stickDpadBtns << 6)) {
		HandleStickButtonsDepression();
	}
	// For the same reason here I depress buttons on the next message (inpSent)
	// which has no buttons pressed (!gpadBtns && !stickDpadBtns), so that they all stay pressed when at least one button is pressed.
	// It is done intentionally to be able to, for example, hover pointer with a color picker shortcut. Remove !btns checks if you don't need it.
	if (inpSent) { // doesn't allow for sending new inputs if previous ones aren't cleared
		if (!gpadBtns && !stickDpadBtns) {
			HandleGamepadDpadButtonsDepression(inputs, inpSent);
		}
	}
	else if (gpadBtns) { // common gamepad buttons have higher priority than stick and dpad buttons hence else if.
		HandleGamepadButtonsPress();
	}
	else if (stickDpadBtns) {
		if (!stickBtnPressed) { // checks if it's in duplicate input message (was pressed already and wasn't depressed)
			HandleStickButtonsPress();
		}
		HandleDpadButtonsPress();
	}
	// PEN POINTER
	// CURSOR POSITION (follows mouse)
	if (!GetCursorPos(&curPos)) {
		_RPTF1(_CRT_ERROR, "GET CURSOR POSITION FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	ptInfo->penInfo.pointerInfo.ptPixelLocation = curPos;
	ptInfo->penInfo.pointerInfo.ptPixelLocationRaw = curPos;
	// STATE INITIALIZATION
	curPressure = abs(pRaw->data.hid.bRawData[10] - 128) * 8; // LT max is 1016, RT max is 1024
	pointerFlags = POINTER_FLAG_CONFIDENCE; // initial flags (POINTER_FLAG_PRIMARY doesn't do anything I think)
	ButtonChangeType = POINTER_CHANGE_NONE;
	// KEY MODIFIERS
	HandlePenModifiersState();	// using globals a lot to not deal with multiple return values
	// BARREL
	HandlePenBarrelStateChange();
	// PRESSURE (overrides barrel button change)
	HandlePenPressureStateChange();
	// STATE VALUES ASSIGNING
	ptInfo->penInfo.pressure = curPressure;
	ptInfo->penInfo.pointerInfo.pointerFlags = pointerFlags;
	ptInfo->penInfo.pointerInfo.ButtonChangeType = ButtonChangeType;
	ptInfo->penInfo.pointerInfo.dwKeyStates = dwKeyStates;
	// ROTATION (these two are using ptInfo global implicitly to avoid more mess)
	HandlePenRotationChange();
	// TILT
	HandlePenTiltChange();
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
	// Case for updating pen pointer when no gamepad input. Otherwise the cursor position will reset. TODO: MOVE COMMENT
	// PEN POINTER
	// CURSOR POSITION
	if (!GetCursorPos(&curPos)) {
		_RPTF1(_CRT_ERROR, "GET CURSOR POSITION FAILED [%3d]\n", GetLastError());
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	ptInfo->penInfo.pointerInfo.ptPixelLocation = curPos;
	ptInfo->penInfo.pointerInfo.ptPixelLocationRaw = curPos;
	// KEY MODIFIERS
	HandlePenModifiersState();
	// STATE INITIALIZATION
	pointerFlags = POINTER_FLAG_CONFIDENCE | POINTER_FLAG_UPDATE;
	ButtonChangeType = POINTER_CHANGE_NONE;
	// BARREL
	HandlePenBarrelStateUpdate();
	// PRESSURE
	HandlePenPressureStateUpdate();
	// STATE VALUES ASSIGNING
	ptInfo->penInfo.pointerInfo.dwKeyStates = dwKeyStates;
	ptInfo->penInfo.pointerInfo.pointerFlags = pointerFlags;
	ptInfo->penInfo.pointerInfo.ButtonChangeType = ButtonChangeType;
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
	if (inpSent) {
		HandleGamepadDpadButtonsDepression(inputs, inpSent);
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

	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}
