#pragma once
#include "Globals.hpp"
#include "OtherFunctions.hpp"


inline void PressShift() {
	// even if this solution was obvious in retrospect, had to look into AHK source code for it
	keybd_event(VK_LSHIFT, 42, 0, 0);
	shiftPressed = true;
}

inline void PressGamepadButtons(char& inpSent, INPUT inputs[], BYTE gpadBtns) {
	if (gpadBtns & GP_A) {
		// Gradient in Krita / Grab in Blender
		AddKeyPress(inputs, inpSent, 0x47); // G key
	}
	if (gpadBtns & GP_B) {
		// Brush
		AddKeyPress(inputs, inpSent, 0x42); // B key
	}
	if (gpadBtns & GP_X) {
		// Eraser
		AddKeyPress(inputs, inpSent, 0x45); // E key
	}
	if (gpadBtns & GP_Y) {
		// Layer select in Krita / Rotate in Blender
		AddKeyPress(inputs, inpSent, 0x52); // R key
	}
	if (gpadBtns & GP_LB) {
		// Color picker
		AddKeyPress(inputs, inpSent, CtrAlt);
		AddMousePress(inputs, inpSent, MOUSEEVENTF_LEFTDOWN);
	}
	if (gpadBtns & GP_BACK) {
		// Smoothing in Blender (just holds shift)
		PressShift(); // this is the ONLY key that has problems with SendInput (ex. Blender)
	}
	if (gpadBtns & GP_START) {
		// Alternate mode in Blender (just holds ctrl)
		AddKeyPress(inputs, inpSent, VK_LCONTROL);
	}
	// SEND GPAD INPUT (if at least one GPAD input is set)
	if (inpSent && !SendInput(inpSent, inputs, sizeof(INPUT))) {
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
}

inline void PressStickButtons(BYTE stickDpadBtns) {
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

inline void PressDpadButtons(char& inpSent, INPUT inputs[], BYTE stickDpadBtns) {
	if ((stickDpadBtns >> 2) == (DPAD_UP >> 2)) {
		// Higher opacity in Krita
		AddKeyPress(inputs, inpSent, 0x4F); // O key
	}
	if ((stickDpadBtns >> 2) == (DPAD_RIGHT >> 2)) {
		// Redo
		AddKeyPress(inputs, inpSent, VK_LCONTROL);
		PressShift();
		AddKeyPress(inputs, inpSent, 0x5a);
	}
	if ((stickDpadBtns >> 2) == (DPAD_DOWN >> 2)) {
		// Lower opacity in Krita
		AddKeyPress(inputs, inpSent, 0x49); // I key
	}
	if ((stickDpadBtns >> 2) == (DPAD_LEFT >> 2)) {
		// Undo
		AddKeyPress(inputs, inpSent, VK_LCONTROL);
		AddKeyPress(inputs, inpSent, 0x5A); // Z
	}
	// SEND DPAD INPUT (if at least one DPAD input is set)
	if (inpSent && !SendInput(inpSent, inputs, sizeof(INPUT))) {
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
}

inline void DepressShift() {
	keybd_event(VK_LSHIFT, 42, KEYEVENTF_KEYUP, 0);
	shiftPressed = false;
}

inline void DepressGamepadDpadButtons(char& inpSent, INPUT inputs[]) {
	// Depresses and clears previously injected inputs via SendInput.
	for (int i = inpSent - 1; i > -1; i--) {
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
	if (!SendInput(inpSent, inputs, sizeof(INPUT))) {
		_RPTF1(_CRT_ERROR, "SEND INPUT FAILED [%3d]\n", GetLastError());
	}
	ZeroMemory(inputs, sizeof(INPUT) * inpSent); // reset input buffer
	inpSent = 0;
}

inline void DepressStickButtons() {
	tiltLocked = (tiltLocked + (stickBtnPressed & 1)) % 2;
	rotationLocked = (rotationLocked + (stickBtnPressed >> 1)) % 2;
	stickBtnPressed = 0;
	_RPT2(_CRT_WARN, "LOCKS [%3d] [%3d]\n", tiltLocked, rotationLocked);
}

inline void HandleShortcuts(char& inpSent, INPUT inputs[], BYTE gpadBtns, BYTE stickDpadBtns) {
	if (shiftPressed && !gpadBtns && !stickDpadBtns) {
		// have to depress shift separately because it uses keybd_event which is more reliable for shift presses
		DepressShift();
	}
	// For some reason, gamepad sends two WM_INPUTs per one button press,
	// so i have to ignore next non-zero ones and then assign locked state values when no stick buttons are pressed.
	if (stickBtnPressed && !(stickDpadBtns << 6)) {
		DepressStickButtons();
	}
	// For the same reason here I depress buttons on the next message (inpSent)
	// which has no buttons pressed (!gpadBtns && !stickDpadBtns), so that they all stay pressed when at least one shortcut is pressed.
	// It is done intentionally to be able to, for example, hover pointer with a color picker shortcut.
	if (inpSent) {
		// doesn't allow for sending new inputs if previous ones aren't cleared
		if (!gpadBtns && !stickDpadBtns) {
			DepressGamepadDpadButtons(inpSent, inputs);
		}
	}
	// common gamepad buttons have higher priority than stick and dpad buttons hence else if.
	else if (gpadBtns) {
		PressGamepadButtons(inpSent, inputs, gpadBtns);
	}
	else if (stickDpadBtns) {
			// checks if it's in duplicate input message (was pressed already and wasn't depressed)
		if (!stickBtnPressed) {
			PressStickButtons(stickDpadBtns);
		}
		PressDpadButtons(inpSent, inputs, stickDpadBtns);
	}
}
