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
	{ XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER, "Steel Battalion Controller" },
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

struct SBCInput { //Was told not to copy this, did it anyways for the sake of having the proper input size and potentially modifying it's bindings from there.
	uint16_t    wButtons[3];
	int16_t   sAimingX;
	int16_t   sAimingY;
	int16_t   sRotationLever;//maybe only high byte was used.
	int16_t   sSightChangeX;
	int16_t   sSightChangeY;
	uint16_t    wLeftPedal;//maybe only high byte was used.
	uint16_t    wMiddlePedal;//maybe only high byte was used.
	uint16_t    wRightPedal;//maybe only high byte was used.
	uint8_t   ucTunerDial;//low nibble, The 9 o'clock postion is 0, and the 6 o'clock position is 12. The blank area between the 6 and 9 o'clock positions is 13, 14, and 15 clockwise.
	uint8_t   ucGearLever;//GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
};

struct SBCOutput { //Likewise, with 0x14
	uint16_t StartLED; //Commented out because it stops compilation
	uint16_t bLen;
	uint16_t EmergencyEject;
	uint16_t CockpitHatch;
	uint16_t Ignition;
	uint16_t Start;
	uint16_t OpenClose;
	uint16_t MapZoomInOut;
	uint16_t ModeSelect;
	uint16_t SubMonitorModeSelect;
	uint16_t MainMonitorZoomIn;
	uint16_t MainMonitorZoomOut;
	uint16_t ForecastShootingSystem;
	uint16_t Manipulator;
	uint16_t LineColorChange;
	uint16_t Washing;
	uint16_t Extinguisher;
	uint16_t Chaff;
	uint16_t TankDetach;
	uint16_t Override;
	uint16_t NightScope;
	uint16_t F1;
	uint16_t F2;
	uint16_t F3;
	uint16_t MainWeaponControl;
	uint16_t SubWeaponControl;
	uint16_t MagazineChange;
	uint16_t Comm1;
	uint16_t Comm2;
	uint16_t Comm3;
	uint16_t Comm4;
	uint16_t Comm5;
	uint16_t Blank; //This might not be correct, as XboxDevWiki shows an empty box for this entry
	uint16_t GearR;
	uint16_t GearN;
	uint16_t Gear1;
	uint16_t Gear2;
	uint16_t Gear3;
	uint16_t Gear4;
	uint16_t Gear5;
	uint16_t Unused; //Also might not be correct, but unsure
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
	// update input for a Steel Battalion controller
	bool UpdateInputSBC(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction);
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
