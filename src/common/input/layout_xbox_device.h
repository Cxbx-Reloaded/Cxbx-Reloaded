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
#include "gui/resource/ResCxbx.h"

// Must have the same button order as defined in XINPUT_GAMEPAD
inline int button_xbox_ctrl_id[XBOX_CTRL_NUM_BUTTONS] = {
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

// Must have the same button order as defined in SBC_GAMEPAD
inline int button_sbc_id[SBC_NUM_BUTTONS] = {
	IDC_BTN_MAIN_WEAPON,
	IDC_BTN_SUB_WEAPON,
	IDC_BTN_LOCK_ON,
	IDC_BTN_EJECT,
	IDC_BTN_COCKPIT_HATCH,
	IDC_BTN_IGNITION,
	IDC_BTN_START,
	IDC_BTN_OPEN_CLOSE,
	IDC_BTN_MAP_ZOOM_IN_OUT,
	IDC_BTN_MODE_SELECT,
	IDC_BTN_SUB_MONITOR_MODE_SELECT,
	IDC_BTN_ZOOM_IN,
	IDC_BTN_ZOOM_OUT,
	IDC_BTN_FSS,
	IDC_BTN_MANIPULATOR,
	IDC_BTN_LINE_COLOR_CHANGE,
	IDC_BTN_WASHING,
	IDC_BTN_EXTINGUISHER,
	IDC_BTN_CHAFF,
	IDC_BTN_TANK_DETACH,
	IDC_BTN_OVERRIDE,
	IDC_BTN_NIGHT_SCOPE,
	IDC_BTN_FUNC1,
	IDC_BTN_FUNC2,
	IDC_BTN_FUNC3,
	IDC_BTN_MAIN_WEAPON_CONTROL,
	IDC_BTN_SUB_WEAPON_CONTROL,
	IDC_BTN_MAGAZINE_CHANGE,
	IDC_BTN_COM1,
	IDC_BTN_COM2,
	IDC_BTN_COM3,
	IDC_BTN_COM4,
	IDC_BTN_COM5,
	IDC_BTN_SIGHT_CHANGE,
	IDC_FILT_CONTROL_SYSTEM,
	IDC_OXYGEN_SUPPLY_SYSTEM,
	IDC_FUEL_FLOW_RATE,
	IDC_BUFFER_MATERIAL,
	IDC_VT_LOCATION_MEASUREMENT,
	IDC_AIMING_POSX,
	IDC_AIMING_NEGX,
	IDC_AIMING_POSY,
	IDC_AIMING_NEGY,
	IDC_LEVER_LEFT,
	IDC_LEVER_RIGHT,
	IDC_SIGHT_CHANGE_POSX,
	IDC_SIGHT_CHANGE_NEGX,
	IDC_SIGHT_CHANGE_POSY,
	IDC_SIGHT_CHANGE_NEGY,
	IDC_BTN_LEFT_PEDAL,
	IDC_BTN_MIDDLE_PEDAL,
	IDC_BTN_RIGHT_PEDAL,
	IDC_RADIO_TD_UP,
	IDC_RADIO_TD_DOWN,
	IDC_GEAR_UP,
	IDC_GEAR_DOWN,
};

// Must have the same button order as defined in button_lightgun_names
inline int button_lightgun_id[LIGHTGUN_NUM_BUTTONS] = {
	IDC_LG_STICK_UP,
	IDC_LG_STICK_DOWN,
	IDC_LG_STICK_LEFT,
	IDC_LG_STICK_RIGHT,
	IDC_LG_START,
	IDC_LG_SEBA,
	IDC_LG_TRIGGER,
	IDC_LG_GRIP,
	IDC_LG_A,
	IDC_LG_B,
	IDC_LG_AIM_POSX,
	IDC_LG_AIM_NEGX,
	IDC_LG_AIM_POSY,
	IDC_LG_AIM_NEGY,
	IDC_TURBO_LEFT,
	IDC_TURBO_RIGHT,
};
#endif

inline constexpr const char* button_xbox_ctrl_names[XBOX_CTRL_NUM_BUTTONS] = {
	"D Pad Up",
	"D Pad Down",
	"D Pad Left",
	"D Pad Right",
	"Start",
	"Back",
	"L Thumb",
	"R Thumb",
	"A",
	"B",
	"X",
	"Y",
	"Black",
	"White",
	"L Trigger",
	"R Trigger",
	"Left Axis X+",
	"Left Axis X-",
	"Left Axis Y+",
	"Left Axis Y-",
	"Right Axis X+",
	"Right Axis X-",
	"Right Axis Y+",
	"Right Axis Y-",
	"Motor",
};

inline constexpr const char *button_sbc_names[SBC_NUM_BUTTONS] = {
	"RightJoyMainWeapon",
	"RightJoyFire",
	"RightJoyLockOn",
	"Eject",
	"CockpitHatch",
	"Ignition",
	"Start",
	"MultiMonOpenClose",
	"MultiMonMapZoomInOut",
	"MultiMonModeSelect",
	"MultiMonSubMonitor",
	"MainMonZoomIn",
	"MainMonZoomOut",
	"FunctionFSS",
	"FunctionManipulator",
	"FunctionLineColorChange",
	"Washing",
	"Extinguisher",
	"Chaff",
	"FunctionTankDetach",
	"FunctionOverride",
	"FunctionNightScope",
	"FunctionF1",
	"FunctionF2",
	"FunctionF3",
	"WeaponConMain",
	"WeaponConSub",
	"WeaponConMagazine",
	"Comm1",
	"Comm2",
	"Comm3",
	"Comm4",
	"Comm5",
	"LeftJoySightChange",
	"ToggleFilterControl",
	"ToggleOxygenSupply",
	"ToggleFuelFlowRate",
	"ToggleBufferMaterial",
	"ToggleVTLocation",
	"Aiming X+",
	"Aiming X-",
	"Aiming Y+",
	"Aiming Y-",
	"LeverLeft",
	"LeverRight",
	"SightChange X+",
	"SightChange X-",
	"SightChange Y+",
	"SightChange Y-",
	"LeftPedal",
	"MiddlePedal",
	"RightPedal",
	"TunerDial Up",
	"TunerDial Down",
	"GearLever Up",
	"GearLever Down",
};

inline constexpr const char *button_lightgun_names[LIGHTGUN_NUM_BUTTONS] = {
	"Stick Up",
	"Stick Down",
	"Stick Left",
	"Stick Right",
	"START",
	"SE/BA",
	"Trigger",
	"Grip",
	"A",
	"B",
	"Aim X+",
	"Aim X-",
	"Aim Y+",
	"Aim Y-",
	"Turbo Left",
	"Turbo Right",
};

constexpr bool check_button_name_size(unsigned max_num_buttons)
{
	switch (max_num_buttons)
	{
	case XBOX_CTRL_NUM_BUTTONS: {
		for (unsigned i = 0; i < max_num_buttons; i++) {
			if (std::char_traits<char>::length(button_xbox_ctrl_names[i]) > (XBOX_BUTTON_NAME_LENGTH - 1)) {
				return false;
			}
		}
	}
	break;

	case SBC_NUM_BUTTONS: {
		for (unsigned i = 0; i < max_num_buttons; i++) {
			if (std::char_traits<char>::length(button_sbc_names[i]) > (XBOX_BUTTON_NAME_LENGTH - 1)) {
				return false;
			}
		}
	}
	break;

	default:
		return false;
	}

	return true;
}

// Sanity checks: the button buffers used in the input system are 30 bytes large, so ensure that we do not exceed them
static_assert(check_button_name_size(XBOX_CTRL_NUM_BUTTONS));
static_assert(check_button_name_size(SBC_NUM_BUTTONS));
