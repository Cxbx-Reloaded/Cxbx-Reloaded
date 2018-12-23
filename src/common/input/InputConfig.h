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
// *   src->Common->Input->InputConfig.h
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

#ifndef INPUTCONFIG_H_
#define INPUTCONFIG_H_
#if 1 // Reenable this when LLE USB actually works
#include <vector>
#include <atomic>
#include "SDL.h"

#define GAMEPAD_A                 0
#define GAMEPAD_B                 1
#define GAMEPAD_X                 2
#define GAMEPAD_Y                 3
#define GAMEPAD_BLACK             4
#define GAMEPAD_WHITE             5
#define GAMEPAD_LEFT_TRIGGER      6
#define GAMEPAD_RIGHT_TRIGGER     7

#define GAMEPAD_DPAD_UP           8
#define GAMEPAD_DPAD_DOWN         9
#define GAMEPAD_DPAD_LEFT         10
#define GAMEPAD_DPAD_RIGHT        11
#define GAMEPAD_START             12
#define GAMEPAD_BACK              13
#define GAMEPAD_LEFT_THUMB        14
#define GAMEPAD_RIGHT_THUMB       15

#define GAMEPAD_LEFT_THUMB_X      16
#define GAMEPAD_LEFT_THUMB_Y      17
#define GAMEPAD_RIGHT_THUMB_X     18
#define GAMEPAD_RIGHT_THUMB_Y     19

#define GAMEPAD_INVALID          -1

#define HAT_CONSTANT             255

#define BUTTON_MASK(button) (1 << ((button) - GAMEPAD_DPAD_UP))


/* enum indicating the device type to attach to the virtual xbox */
typedef enum {
	DEVICE_INVALID = 0,
	MS_CONTROLLER_DUKE,
	MS_CONTROLLER_S,
	LIGHT_GUN,
	STEERING_WHEEL,
	MEMORY_UNIT,
	IR_DONGLE,
	STEEL_BATTALION_CONTROLLER,
}
XBOX_INPUT_DEVICE;

class SDL2Devices; // forward declare


class InputDeviceManager
{
	public:
		InputDeviceManager();
		~InputDeviceManager();

		// enumerate all available sdl2 controllers
		int EnumSdl2Devices();
		// start input event processing thread
		void StartInputThread();
		// connect the enumerated device to the virtual xbox
		int ConnectDeviceToXbox(int port, int type);
		// disconnect a device from the emulated xbox
		void DisconnectDeviceFromXbox(int port);
		// find the device attached to the supplied xbox port
		SDL2Devices* FindDeviceFromXboxPort(int port);


	private:
		// all enumerated devices currently detected and supported
		std::vector<SDL2Devices*> m_Sdl2Devices;

		// assign the button binding to the devices
		//void AssignBindings();
		// input thread
		static void InputThread(InputDeviceManager* pVoid);
		// updates the button state of a joystick
		void UpdateButtonState(SDL_JoystickID id, uint8_t button, uint8_t state);
		// updates the hat state of a joystick
		void UpdateHatState(SDL_JoystickID id, uint8_t hat_index, uint8_t state);
		// updates the axis state of a joystick
		void UpdateAxisState(SDL_JoystickID id, uint8_t axis_index, int16_t state);
		// checks if the controller attached can be used by sdl
		int IsValidController(int index);
};

extern InputDeviceManager* g_InputDeviceManager;
#endif
#endif
