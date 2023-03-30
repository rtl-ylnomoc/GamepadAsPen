#pragma once
#include "../resource.h"
#include <Windows.h>

// RESOURCES
constexpr int IDI_APP_ICON = IDI_ICON1;
constexpr int IDI_CLOSE_ICON = IDI_ICON2;
constexpr int IDI_MINIMIZE_ICON = IDI_ICON3;
// WINDOW PROPERTIES
constexpr short width = 210;
constexpr short height = 42;
// GAMEPAD BUTTONS
// COMMON	[11]
constexpr byte		GP_A			=	0b00000001;	// inputs[0]
constexpr byte		GP_B			=	0b00000010; // inputs[1]
constexpr byte		GP_X			=	0b00000100; // inputs[2]
constexpr byte		GP_Y			=	0b00001000; // inputs[3]
constexpr byte		GP_LB			=	0b00010000; // inputs[4]
constexpr byte		GP_RB			=	0b00100000; // inputs[5]
constexpr byte		GP_BACK			=	0b01000000; // inputs[6]
constexpr byte		GP_START		=	0b10000000; // inputs[7]
// STICK	[12]
constexpr byte		LEFT_STICK_BTN	=	0b00000001; // inputs[8]
constexpr byte		RIGHT_STICK_BTN	=	0b00000010; // inputs[9]
// DPAD		[12]	(DPAD buttons are not flags, just check for equality)
constexpr byte		DPAD_UP			=	0b00000100; // inputs[10]
constexpr byte		DPAD_UP_RIGHT	=	0b00001000; // inputs[10]
constexpr byte		DPAD_RIGHT		=	0b00001100; // inputs[10]
constexpr byte		DPAD_DOWN_RIGHT	=	0b00010000; // inputs[10]
constexpr byte		DPAD_DOWN		=	0b00010100; // inputs[10]
constexpr byte		DPAD_DOWN_LEFT	=	0b00011000; // inputs[10]
constexpr byte		DPAD_LEFT		=	0b00011100; // inputs[10]
constexpr byte		DPAD_UP_LEFT	=	0b00100000; // inputs[10]
