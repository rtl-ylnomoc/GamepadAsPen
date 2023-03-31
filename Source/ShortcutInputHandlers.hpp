#pragma once
#include "Globals.hpp"
#include "OtherFunctions.hpp"


inline void HandleGamepadButtonsPress() {
	if (gpadBtns & GP_X) {
		// Eraser
		AddKeyPress(inputs, inpSent, 0x45); // E
	}
	if (gpadBtns & GP_B) {
		// Brush
		AddKeyPress(inputs, inpSent, 0x42); // B key
	}
	if (gpadBtns & GP_LB) {
		// Color picker
		AddKeyPress(inputs, inpSent, CtrAlt);
		AddMousePress(inputs, inpSent, MOUSEEVENTF_LEFTDOWN);
	}
	// SEND GPAD INPUT
	if (inpSent && !SendInput(inpSent, inputs, sizeof(INPUT))) {
		// Send input if at least one input is set
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
}

inline void HandleStickButtonsPress() {
	_RPT1(_CRT_WARN, "STICK INPUT [%3d]\n", stickDpadBtns);
	if (stickDpadBtns & LEFT_STICK_BTN) {
		// Toggle tilt lock
		stickBtnPressed += 0b01;
	}
	if (stickDpadBtns & RIGHT_STICK_BTN) {
		// Toggle rotation lock
		stickBtnPressed += 0b10;
	}
}

inline void HandleDpadButtonsPress() {
	if ((stickDpadBtns >> 2) == (DPAD_UP >> 2)) {
		// Higher opacity in Krita
		AddKeyPress(inputs, inpSent, 0x4F); // O key
	}
	if ((stickDpadBtns >> 2) == (DPAD_RIGHT >> 2)) {
		// Undo
		AddKeyPress(inputs, inpSent, VK_LCONTROL);
		AddKeyPress(inputs, inpSent, VK_LSHIFT);
		AddKeyPress(inputs, inpSent, 0x5A); // Z
	}
	if ((stickDpadBtns >> 2) == (DPAD_DOWN >> 2)) {
		// Lower opacity in Krita
		AddKeyPress(inputs, inpSent, 0x49); // I key
	}
	if ((stickDpadBtns >> 2) == (DPAD_LEFT >> 2)) {
		// Redo
		AddKeyPress(inputs, inpSent, VK_LCONTROL);
		AddKeyPress(inputs, inpSent, 0x5A); // Z
	}
	// SEND DPAD INPUT
	if (inpSent && !SendInput(inpSent, inputs, sizeof(INPUT))) {
		// Send input if at least one input is set
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
}

inline void HandleGamepadDpadButtonsDepression(INPUT inputs[], char& amount) {
	// Depresses and clears previously injected inputs.
	for (int i = amount - 1; i > -1; i--) {
		_RPT2(_CRT_WARN, "CLEARING INPUT [%3d] TYPE [%3d]\n", i, inputs[i].type);
		if (inputs[i].type == INPUT_KEYBOARD) {
			_RPTF2(_CRT_WARN, "INPUT [%3d] [%3d]\n", sizeof(KEYBDINPUT), sizeof(INPUT));
			inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
		}
		else {
			_RPTF2(_CRT_WARN, "INPUT [%3d] [%3d]\n", sizeof(MOUSEINPUT), sizeof(INPUT));
			inputs[i].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		}
	}
	if (!SendInput(amount, inputs, sizeof(INPUT))) {
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
	ZeroMemory(inputs, sizeof(INPUT) * amount); // reset input buffer
	amount = 0;
}

inline void HandleStickButtonsDepression() {
	_RPT2(_CRT_WARN, "STICK INPUT [%3d] [%3d]\n", stickDpadBtns, stickBtnPressed);
	_RPT2(_CRT_WARN, "BOOL BIT FLAGS [%3d] [%3d]\n", stickBtnPressed & 1, stickBtnPressed >> 1);
	tiltLocked = (tiltLocked + (stickBtnPressed & 1)) % 2;
	rotationLocked = (rotationLocked + (stickBtnPressed >> 1)) % 2;
	stickBtnPressed = 0;
	_RPT2(_CRT_WARN, "LOCKS [%3d] [%3d]\n", tiltLocked, rotationLocked);
}