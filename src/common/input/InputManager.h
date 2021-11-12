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

#define PORT_INVALID     -1
#define PORT_1            0
#define PORT_2            1
#define PORT_3            2
#define PORT_4            3
#define XBOX_NUM_PORTS    4

#define SLOT_TOP             0
#define SLOT_BOTTOM          1
#define XBOX_CTRL_NUM_SLOTS  2

#define CTRL_OFFSET 0
#define MU_OFFSET   4
#define MAX_DEVS    (XBOX_NUM_PORTS + XBOX_CTRL_NUM_SLOTS * XBOX_NUM_PORTS)

#define XID_PACKET_HEADER 2

extern int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)];

inline XBOX_INPUT_DEVICE input_support_list[] = {
	XBOX_INPUT_DEVICE::DEVICE_INVALID,
	XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE,
	XBOX_INPUT_DEVICE::MS_CONTROLLER_S,
	XBOX_INPUT_DEVICE::LIGHTGUN,
	XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER,
	XBOX_INPUT_DEVICE::ARCADE_STICK,
	XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER,
	XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER,
};

inline XBOX_INPUT_DEVICE slot_support_list[] = {
	XBOX_INPUT_DEVICE::DEVICE_INVALID,
	XBOX_INPUT_DEVICE::MEMORY_UNIT,
	// Microphone
	// Phantasy star online keyboard
};

#pragma pack(1)

// Class-specific xid descriptor, used by libusb
struct XidDesc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdXid;
	uint8_t bType;
	uint8_t bSubType;
	uint8_t bMaxInputReportSize;
	uint8_t bMaxOutputReportSize;
	uint16_t wAlternateProductIds[4];
};

// xpad in/out buffers stripped of the first two bytes as used by xinput
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

// xpad in/out buffers as used by xid
struct XidGamepadInput {
	uint8_t bReportId;
	uint8_t bLength;
	XpadInput InBuffer;
};

struct XidGamepadOutput {
	uint8_t bReportId;
	uint8_t bLength;
	XpadOutput OutBuffer;
};

// same as above, but for the SBC
struct SBCInput {
	uint16_t wButtons[3];
	uint8_t  bPad1;
	uint8_t  sAimingX;
	uint8_t  bPad2;
	uint8_t  sAimingY;
	uint8_t  bPad3;
	int8_t   sRotationLever;
	uint8_t  bPad4;
	int8_t   sSightChangeX;
	uint8_t  bPad5;
	int8_t   sSightChangeY;
	uint8_t  bPad6;
	uint8_t  wLeftPedal;
	uint8_t  bPad7;
	uint8_t  wMiddlePedal;
	uint8_t  bPad8;
	uint8_t  wRightPedal;
	uint8_t  ucTunerDial;
	uint8_t  ucGearLever;
};

struct SBCOutput {
	uint8_t  LedState[20];
};

struct XidSBCInput {
	uint8_t bReportId;
	uint8_t bLength;
	SBCInput InBuffer;
};

struct XidSBCOutput {
	uint8_t bReportId;
	uint8_t bLength;
	SBCOutput OutBuffer;
};

#pragma pack()

union InputBuff {
	XidGamepadInput ctrl;
	XidSBCInput sbc;
};

struct DeviceInfo {
	xbox::HANDLE hHandle;      // device handle returned by xapi
	bool bAutoPoll;            // autopoll on/off, as instructed by the title in XInputOpen
	bool bAutoPollDefault;     // default autopoll value, depending on device type
	uint8_t ucType;            // xapi type
	uint8_t ucSubType;         // xapi subtype
	uint8_t ucInputStateSize;  // input state size in bytes, does not include dwPacketNumber
	uint8_t ucFeedbackSize;    // feedback size in bytes, does not include FeedbackHeader
	uint32_t dwPacketNumber;
	InputBuff buff;
};

struct DeviceState {
	std::string port;
	int port_idx;
	XBOX_INPUT_DEVICE type;
	bool bPendingRemoval;
	bool bSignaled;
	DeviceInfo info;
	DeviceState *slots[XBOX_CTRL_NUM_SLOTS];
	DeviceState *upstream;
};

extern DeviceState g_devs[MAX_DEVS];


class InputDeviceManager
{
public:
	void Initialize(bool is_gui, HWND hwnd);
	void Shutdown();
	// read/write the input/output from/to the device attached to the supplied xbox port
	bool UpdateXboxPortInput(int port, void *buffer, int direction, int type);
	// add the device to the list of availble devices
	void AddDevice(std::shared_ptr<InputDevice> Device);
	// remove the device from the list of availble devices
	void RemoveDevice(std::function<bool(const InputDevice*)> Callback);
	// update device list
	void RefreshDevices();
	// get the name of the devices currently detected
	std::vector<std::string> GetDeviceList(std::function<bool(const InputDevice *)> Callback) const;
	// find device from its gui name
	std::shared_ptr<InputDevice> FindDevice(const std::string& QualifiedName) const;
	// find device from its sdl id
	std::shared_ptr<InputDevice> FindDevice(SDL_JoystickID id) const;
	// find device from its xbox port
	std::shared_ptr<InputDevice> FindDevice(std::string_view port) const;
	// attach/detach guest devices to the emulated machine
	void UpdateDevices(std::string_view port, bool ack);
	// update input options
	void UpdateOpt(bool is_gui);
	// device hotplug event handler
	void HotplugHandler(bool is_sdl);


private:
	// update input for an xbox controller
	bool UpdateInputXpad(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction, const std::string &Port);
	// update input for a Steel Battalion controller
	bool UpdateInputSBC(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction, int Port_num, const std::string &Port);
	// update input for a passthrough xbox device
	bool UpdateInputHw(std::shared_ptr<InputDevice> &Device, void *Buffer, int Direction);
	// bind a host device to an emulated device
	void BindHostDevice(int type, std::string_view port);
	// connect a device to the emulated machine
	void ConnectDevice(DeviceState *dev, DeviceState *upstream, int type, std::string_view port);
	// disconnect a device from the emulated machine
	void DisconnectDevice(DeviceState *dev, std::string_view port, bool ack);

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
	// handle of the rendering or the input gui window
	HWND m_hwnd;
};

extern InputDeviceManager g_InputDeviceManager;

// hle input functions
void ConstructHleInputDevice(DeviceState *dev, DeviceState *upstream, int type, std::string_view port);
void DestructHleInputDevice(DeviceState *dev);

#endif
