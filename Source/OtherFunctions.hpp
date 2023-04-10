#pragma once
#include "Constants.hpp"


inline int ConvertPressure(BYTE byte) {
	// Properly converts byte info to pressure from both triggers:
	// ([10] byte: pressure)
	// (128: 0; {0, 255}: 1024)
	return byte > 128 ? (byte - 127) * 8 : -(byte - 128) * 8;
}

inline void AddKeyPress(INPUT inputs[], char& inpSent, const WORD vk) {
	inputs[inpSent].type = INPUT_KEYBOARD;
	inputs[inpSent++].ki.wVk = vk;
}

inline void AddMousePress(INPUT inputs[], char& inpSent, const DWORD flags) {
	inputs[inpSent].type = INPUT_MOUSE;
	inputs[inpSent++].mi.dwFlags = flags;
}
