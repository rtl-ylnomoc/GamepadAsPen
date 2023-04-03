#pragma once
#include "Constants.hpp"


inline int ConvertPressure(BYTE byte) { // properly converts byte info from both triggers to pressure (128: 0, {0, 255}: 1024)
	return byte > 128 ? (byte - 127) * 8 : -(byte - 128) * 8;
}

inline void AddKeyPress(INPUT inputs[], char& freeIndex, const WORD vk) {
	inputs[freeIndex].type = INPUT_KEYBOARD;
	inputs[freeIndex++].ki.wVk = vk;
}

inline void AddMousePress(INPUT inputs[], char& freeIndex, const DWORD flags) {
	inputs[freeIndex].type = INPUT_MOUSE;
	inputs[freeIndex++].mi.dwFlags = flags;
}
