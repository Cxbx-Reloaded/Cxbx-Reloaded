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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_
#if 1 // Reenable this when LLE USB actually works
#include "InputDevice.h"
#include "EmuDevice.h"

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

#define GAMEPAD_BUTTON_MAX        20
#define BUTTON_MASK(button) (1 << ((button) - GAMEPAD_DPAD_UP))


// xpad in/out buffers stripped of the first two bytes
struct XpadInput {
	uint16_t wButtons;
	uint8_t bAnalogButtons[8];
	int16_t sThumbLX;         
	int16_t sThumbLY;
	int16_t sThumbRX;
	int16_t sThumbRY;
};

struct XpadOutput {
	uint16_t left_actuator_strength;
	uint16_t right_actuator_strength;
};


class InputDeviceManager
{
	public:
		void Initialize(bool GUImode);
		void Shutdown();
		// connect the enumerated device to the virtual xbox
		int ConnectDeviceToXbox(int port, int type);
		// disconnect a device from the emulated xbox
		void DisconnectDeviceFromXbox(int port);
		// read/write the input/output from/to the device attached to the supplied xbox port
		bool UpdateXboxPortInput(int Port, void* Buffer, int Direction);
		// add the device to the list of availble devices
		void AddDevice(std::shared_ptr<InputDevice> Device);
		// remove the device from the list of availble devices
		void RemoveDevice(std::function<bool(const InputDevice*)> Callback);
		// update device list
		void RefreshDevices();
		// get the name of the devices currently detected
		std::vector<std::string> GetDeviceList() const;
		// assign the binding to the control
		void BindButton(int ControlID, std::string DeviceName);
		// construct emu device to configurate
		void ConstructEmuDevice(int Type, HWND hwnd);
		// destroy emu device
		void DestroyEmuDevice();


	private:
		// all enumerated devices currently detected and supported
		std::vector<std::shared_ptr<InputDevice>> m_Devices;
		// These serve double duty. They are used to signal errors during the initialization and
		// later to signal that sdl has finished to refresh its devices
		std::mutex m_Mtx;
		std::condition_variable m_Cv;
		// input polling thread
		std::thread m_PollingThread;
		// xbox device under configuration
		EmuDevice* m_DeviceConfig;
		// used to indicate that the manager was initialized correctly
		bool m_bInitOK;
		// update input for an xbox controller
		void UpdateInputXpad(InputDevice* Device, void* Buffer, int Direction);
		// find device from its gui name
		std::shared_ptr<InputDevice> FindDevice(std::string& QualifiedName) const;
		// wait for an input from the specified device
		InputDevice::Input* Detect(InputDevice* Device);
};

extern InputDeviceManager g_InputDeviceManager;

#endif
#endif
