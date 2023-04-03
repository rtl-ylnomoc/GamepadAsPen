#pragma once
#include "Globals.hpp"
#include "OtherFunctions.hpp"
#include <numbers>


inline void HandlePenModifiersState() {
	dwKeyStates = 0;
	if (GetKeyState(VK_SHIFT)) {
		dwKeyStates |= POINTER_MOD_SHIFT;
	}
	if (GetKeyState(VK_LCONTROL)) {
		dwKeyStates |= POINTER_MOD_CTRL;
	}
}

inline void HandlePenBarrelStateChange() {
	// seems to be equivalent to rmb so I could have just mapped it as a gamepad shortcut to rmb mouse input
	if ((!ptInfo->penInfo.penFlags & PEN_FLAG_BARREL) && (pRaw->data.hid.bRawData[11] & GP_RB)) {
		_RPT0(_CRT_WARN, "BARREL PRESSED\n");
		ptInfo->penInfo.penFlags |= PEN_FLAG_BARREL | PEN_FLAG_ERASER; // unsure about these flags
		pointerFlags |= POINTER_FLAG_SECONDBUTTON | POINTER_FLAG_HASTRANSFORM;
		ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_DOWN;
	}
	else if ((ptInfo->penInfo.penFlags & PEN_FLAG_BARREL) && (pRaw->data.hid.bRawData[11] & GP_RB)) {
		_RPT0(_CRT_WARN, "BARREL UPDATE\n");
		pointerFlags |= POINTER_FLAG_SECONDBUTTON;
	}
	else if ((ptInfo->penInfo.penFlags & PEN_FLAG_BARREL) && !(pRaw->data.hid.bRawData[11] & GP_RB)) {
		_RPT0(_CRT_WARN, "BARREL RELEASED\n");
		pointerFlags |= POINTER_FLAG_HASTRANSFORM;
		ptInfo->penInfo.penFlags = PEN_FLAG_NONE;
		ButtonChangeType = POINTER_CHANGE_SECONDBUTTON_UP;
	}
}

inline void HandlePenPressureStateChange() {
	curPressure = ConvertPressure(pRaw->data.hid.bRawData[10]);
	if (!ptInfo->penInfo.pressure && !curPressure) {
		ptInfo->penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_UPDATE;
	}
	else if (!ptInfo->penInfo.pressure && curPressure) {
		pointerFlags |= POINTER_FLAG_FIRSTBUTTON | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN;
		ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_DOWN;
	}
	else if (ptInfo->penInfo.pressure && curPressure) {
		pointerFlags |= POINTER_FLAG_FIRSTBUTTON | POINTER_FLAG_INCONTACT;
	}
	else if (ptInfo->penInfo.pressure && !curPressure) {
		pointerFlags |= POINTER_FLAG_UP;
		ButtonChangeType = POINTER_CHANGE_FIRSTBUTTON_UP;
	}
}

inline void HandlePenRotationChange() {
	if (!rotationLocked) {
		rotationX = (pRaw->data.hid.bRawData[5] | pRaw->data.hid.bRawData[6] << 8) - 32768;
		rotationY = 32768 - (pRaw->data.hid.bRawData[7] | pRaw->data.hid.bRawData[8] << 8);
		rotationLength = sqrt(rotationX * rotationX + rotationY * rotationY);
		if (rotationX > 0) {
			ptInfo->penInfo.rotation = static_cast<int>(450 + asin(rotationY / rotationLength) /
				std::numbers::pi_v<float> *180) % 360;
		}
		else {
			ptInfo->penInfo.rotation = static_cast<int>((450 - asin(rotationY / rotationLength) /
				std::numbers::pi_v<float>) * 180) % 360;
		}
	}
}

inline void HandlePenTiltChange() {
	if (!tiltLocked) {
		ptInfo->penInfo.tiltX = (pRaw->data.hid.bRawData[1] | pRaw->data.hid.bRawData[2] << 8) / 364 - 90;
		ptInfo->penInfo.tiltY = (pRaw->data.hid.bRawData[3] | pRaw->data.hid.bRawData[4] << 8) / 364 - 90;
	}
}

inline void HandlePenBarrelStateUpdate() {
	if (ptInfo->penInfo.penFlags & PEN_FLAG_BARREL) {
		pointerFlags |= POINTER_FLAG_SECONDBUTTON;
		_RPT2(_CRT_WARN, "NO GAMEPAD INPUT CHANGE AND BARREL PRESSED: UPDATING\nPRESSURE: %3d FLAGS: %3d\n",
			ptInfo->penInfo.pressure, ptInfo->penInfo.pointerInfo.pointerFlags);
	}
}

inline void HandlePenPressureStateUpdate() {
	if (ptInfo->penInfo.pressure) {
		pointerFlags |= POINTER_FLAG_FIRSTBUTTON | POINTER_FLAG_INCONTACT;
	}
}