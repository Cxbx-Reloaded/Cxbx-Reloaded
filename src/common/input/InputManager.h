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

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <thread>
#include "InputDevice.h"
#include "EmuDevice.h"

// Prevent a collision with the SetPort provided by Windows
#ifdef WIN32
#undef SetPort
#endif

extern int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)];

typedef struct _input_controller {
	XBOX_INPUT_DEVICE type;
	const char* name;
} input_controller;

static input_controller input_support_list[] = {
	{ XBOX_INPUT_DEVICE::DEVICE_INVALID, "None" },
	{ XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE, "MS Controller Duke" },
	{ XBOX_INPUT_DEVICE::MS_CONTROLLER_S, "MS Controller S" },
};

#pragma pack(1)

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

#pragma pack()


class InputDeviceManager
{
public:
	void Initialize(bool is_gui);
	void Shutdown();
	// read/write the input/output from/to the device attached to the supplied xbox port
	bool UpdateXboxPortInput(int usb_port, void* Buffer, int Direction, int xid_type);
	// add the device to the list of availble devices
	void AddDevice(std::shared_ptr<InputDevice> Device);
	// remove the device from the list of availble devices
	void RemoveDevice(std::function<bool(const InputDevice*)> Callback);
	// update device list
	void RefreshDevices();
	// get the name of the devices currently detected
	std::vector<std::string> GetDeviceList() const;
	// find device from its gui name
	std::shared_ptr<InputDevice> FindDevice(const std::string& QualifiedName) const;
	// find device from its sdl id
	std::shared_ptr<InputDevice> FindDevice(SDL_JoystickID id) const;
	// find device from its xbox port
	std::shared_ptr<InputDevice> FindDevice(int usb_port, int dummy) const;
	// attach/detach guest devices to the emulated machine
	void UpdateDevices(int port, bool ack);
	// update input options
	void UpdateOpt(bool is_gui);


private:
	// update input for an xbox controller
	bool UpdateInputXpad(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction);
	// bind a host device to an emulated device
	void BindHostDevice(int port, int usb_port, int type);
	// connect a device to the emulated machine
	void ConnectDevice(int port, int usb_port, int type);
	// disconnect a device from the emulated machine
	void DisconnectDevice(int port, int usb_port, bool ack);

	// all enumerated devices currently detected and supported
	std::vector<std::shared_ptr<InputDevice>> m_Devices;
	// These serve double duty. They are used to signal errors during the initialization and
	// later to signal that sdl has finished to refresh its devices
	mutable std::mutex m_Mtx;
	std::condition_variable m_Cv;
	// input polling thread
	std::thread m_PollingThread;
	// used to indicate that the manager is shutting down
	bool m_bPendingShutdown;
};

extern InputDeviceManager g_InputDeviceManager;

// hle input functions
bool ConstructHleInputDevice(int Type, int Port);
void DestructHleInputDevice(int Port);

#endif
