// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->Common->Input->SDL2_Device.h
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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef SDL2DEVICE_H_
#define SDL2DEVICE_H_
#if 0 // Reenable this when LLE USB actually works
#include "InputConfig.h"


/* Button state, analogous to XIDGamepadReport */
struct ButtonState {
	std::atomic_uint16_t wButtons;         // all non-analog buttons
	std::atomic_uint8_t bAnalogButtons[8]; // X, Y, A, B, white, black, left/right trigger
	std::atomic_int16_t sThumbLX;          // analog stick, left X
	std::atomic_int16_t sThumbLY;          // analog stick, left Y
	std::atomic_int16_t sThumbRX;          // analog stick, right X
	std::atomic_int16_t sThumbRY;          // analog stick, right Y
};


class SDL2Devices
{
	public:
		// index of this joystick
		int m_Index;
		// joystick object as used by SDL
		SDL_Joystick* m_Joystick;
		// id of this controller
		SDL_JoystickID m_jyID;
		// attach/detach state of this joystick
		int m_Attached;
		// indicates the xbox device emulated by this joystick
		int m_Type = DEVICE_INVALID;

		// update analog button state
		void UpdateAnalogButtonState(uint8_t xbox_button, uint8_t state);
		// update digital button state
		void UpdateDigitalButtonState(uint8_t xbox_button, uint8_t state);
		// update hat state
		void UpdateHatState(uint8_t state);
		// update axis state
		void UpdateAxisState(uint8_t xbox_button, int16_t state);
		// returns the xbox button bound with the sdl key provided
		int GetBoundButton(int sdl_key);
		// update bBindingsChanged flag
		void SetChangedBinding(bool bFlag) { m_bBindingsChanged = bFlag; }
		// read the current button state of a device
		bool ReadButtonState(uint16_t* wButtons, uint8_t* bAnalogButtons, int16_t* sThumbLX,
			int16_t* sThumbLY, int16_t* sThumbRX, int16_t* sThumbRY);


	private:
		// default bindings (hardcoded for now)
		const int m_ButtonMap_Analog[8][2] = {
			{ GAMEPAD_A, SDL_CONTROLLER_BUTTON_A },
			{ GAMEPAD_B, SDL_CONTROLLER_BUTTON_B },
			{ GAMEPAD_X, SDL_CONTROLLER_BUTTON_X },
			{ GAMEPAD_Y, SDL_CONTROLLER_BUTTON_Y },
			{ GAMEPAD_BLACK, SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
			{ GAMEPAD_WHITE, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
			{ GAMEPAD_LEFT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERLEFT },
			{ GAMEPAD_RIGHT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERRIGHT },
		};
		const int m_ButtonMap_Binary[8][2] = {
			{ GAMEPAD_BACK, SDL_CONTROLLER_BUTTON_BACK },
			{ GAMEPAD_START, SDL_CONTROLLER_BUTTON_START },
			{ GAMEPAD_LEFT_THUMB, SDL_CONTROLLER_BUTTON_LEFTSTICK },
			{ GAMEPAD_RIGHT_THUMB, SDL_CONTROLLER_BUTTON_RIGHTSTICK },
			{ GAMEPAD_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_UP },
			{ GAMEPAD_DPAD_DOWN, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
			{ GAMEPAD_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
			{ GAMEPAD_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
		};
		const int m_AxisMap[4][2] = {
			{ GAMEPAD_LEFT_THUMB_X, 0 },
			{ GAMEPAD_LEFT_THUMB_Y, 1 },
			{ GAMEPAD_RIGHT_THUMB_X, 4 },
			{ GAMEPAD_RIGHT_THUMB_Y, 2 },
		};
		// m_HatIndex is added to HAT_CONSTANT to avoid collisions between it and the sdl button indices 
		int m_HatIndex = HAT_CONSTANT;
		// current button state of this joystick
		ButtonState m_State;
		// signals that the current bindings should be updated
		std::atomic_bool m_bBindingsChanged;
		// indicates that the button state has been updated since the last time it was read
		std::atomic_bool bStateDirty;
};
#endif
#endif
