// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#ifndef CXBXR_EMU_EXPORTS
#include "gui/ResCxbx.h"

static int button_xbox_ctrl_id[XBOX_CTRL_NUM_BUTTONS] = {
	IDC_SET_DPAD_UP,
	IDC_SET_DPAD_DOWN,
	IDC_SET_DPAD_LEFT,
	IDC_SET_DPAD_RIGHT,
	IDC_SET_START,
	IDC_SET_BACK,
	IDC_SET_LTHUMB,
	IDC_SET_RTHUMB,
	IDC_SET_A,
	IDC_SET_B,
	IDC_SET_X,
	IDC_SET_Y,
	IDC_SET_BLACK,
	IDC_SET_WHITE,
	IDC_SET_LTRIGGER,
	IDC_SET_RTRIGGER,
	IDC_SET_LEFT_POSX,
	IDC_SET_LEFT_NEGX,
	IDC_SET_LEFT_POSY,
	IDC_SET_LEFT_NEGY,
	IDC_SET_RIGHT_POSX,
	IDC_SET_RIGHT_NEGX,
	IDC_SET_RIGHT_POSY,
	IDC_SET_RIGHT_NEGY,
	IDC_SET_MOTOR,
};
#endif

static const char* button_xbox_ctrl_names[XBOX_CTRL_NUM_BUTTONS][3] = {
	"D Pad Up",      "Pad N",      "UP",
	"D Pad Down",    "Pad S",      "DOWN",
	"D Pad Left",    "Pad W",      "LEFT",
	"D Pad Right",   "Pad E",      "RIGHT",
	"Start",         "Start",      "RETURN",
	"Back",          "Back",       "SPACE",
	"L Thumb",       "Thumb L",    "B",
	"R Thumb",       "Thumb R",    "M",
	"A",             "Button A",   "S",
	"B",             "Button B",   "D",
	"X",             "Button X",   "W",
	"Y",             "Button Y",   "E",
	"Black",         "Shoulder R", "C",
	"White",         "Shoulder L", "X",
	"L Trigger",     "Trigger L",  "Q",
	"R Trigger",     "Trigger R",  "R",
	"Left Axis X+",  "Left X+",    "H",
	"Left Axis X-",  "Left X-",    "F",
	"Left Axis Y+",  "Left Y+",    "T",
	"Left Axis Y-",  "Left Y-",    "G",
	"Right Axis X+", "Right X+",   "L",
	"Right Axis X-", "Right X-",   "J",
	"Right Axis Y+", "Right Y+",   "I",
	"Right Axis Y-", "Right Y-",   "K",
	"Motor",         "LeftRight",  "",
};
