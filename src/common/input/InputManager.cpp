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

// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file of Dolphin at https://github.com/dolphin-emu/dolphin/blob/master/license.txt.

// Partially derived from ControllerInterface.cpp of Dolphin emulator
// https://github.com/dolphin-emu/dolphin

#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::INPSYS


#include <core\kernel\exports\xboxkrnl.h> // For PKINTERRUPT, etc.
#include "SdlJoystick.h"
#include "XInputPad.h"
#include "DInputKeyboardMouse.h"
#include "InputManager.h"
#include "..\devices\usb\XidGamepad.h"
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog
#include "EmuShared.h"
#include "devices\usb\OHCI.h"

// hle input specific
#include "core\hle\XAPI\Xapi.h"

int Gui2XboxPortArray[4] = {
	3,
	4,
	1,
	2
};

int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)] = {
	XBOX_CTRL_NUM_BUTTONS, // MS_CONTROLLER_DUKE
	XBOX_CTRL_NUM_BUTTONS, // MS_CONTROLLER_S
	0,
	0,
	0,
	0,
	SBC_NUM_BUTTONS, // STEEL_BATTALION_CONTROLLER
	XBOX_CTRL_NUM_BUTTONS, // ARCADE_STICK
};

extern CXBX_CONTROLLER_HOST_BRIDGE g_XboxControllerHostBridge[4]; // hle xinput


InputDeviceManager g_InputDeviceManager;

void InputDeviceManager::Initialize(bool is_gui, HWND hwnd)
{
	// Sdl::Init must be called last since it blocks when it succeeds
	std::unique_lock<std::mutex> lck(m_Mtx);
	m_bPendingShutdown = false;
	m_hwnd = hwnd;

	m_PollingThread = std::thread([this, is_gui]() {
		XInput::Init(m_Mtx);
		Sdl::Init(m_Mtx, m_Cv, is_gui);
		});

	m_Cv.wait(lck, []() {
		return (Sdl::SdlInitStatus != Sdl::SDL_NOT_INIT) &&
			(XInput::XInputInitStatus != XInput::XINPUT_NOT_INIT);
		});
	lck.unlock();

	if (Sdl::SdlInitStatus < 0 || XInput::XInputInitStatus < 0) {
		CxbxKrnlCleanup("Failed to initialize input subsystem! Consult debug log for more information");
	}

	UpdateOpt(is_gui);
	RefreshDevices();

	if (!is_gui) {
		UpdateDevices(PORT_1, false);
		UpdateDevices(PORT_2, false);
		UpdateDevices(PORT_3, false);
		UpdateDevices(PORT_4, false);
	}
}

void InputDeviceManager::Shutdown()
{
	// Prevent additional devices from being added during shutdown.
	m_bPendingShutdown = true;

	std::lock_guard<std::mutex> lk(m_Mtx);
	for (const auto& d : m_Devices)
	{
		// Set outputs to ZERO before destroying device
		for (InputDevice::Output* o : d->GetOutputs()) {
			o->SetState(0.0, 0.0);
		}
	}
	m_Devices.clear();

	XInput::DeInit();
	Sdl::DeInit(m_PollingThread);
}

void InputDeviceManager::AddDevice(std::shared_ptr<InputDevice> Device)
{
	// If we are shutting down, ignore this request
	if (m_bPendingShutdown) {
		return;
	}

	std::lock_guard<std::mutex> lk(m_Mtx);
	// Try to find an ID for this device
	int ID = 0;
	while (true)
	{
		const auto it =
			std::find_if(m_Devices.begin(), m_Devices.end(), [&Device, &ID](const auto& d) {
			return d->GetAPI() == Device->GetAPI() && d->GetDeviceName() == Device->GetDeviceName() &&
				d->GetId() == ID;
				});
		if (it == m_Devices.end()) { // no device with the same name with this ID, so we can use it
			break;
		}
		else {
			ID++;
		}
	}
	Device->SetId(ID);

	EmuLog(LOG_LEVEL::INFO, "Added device: %s", Device->GetQualifiedName().c_str());
	m_Devices.emplace_back(std::move(Device));
}

void InputDeviceManager::RemoveDevice(std::function<bool(const InputDevice*)> Callback)
{
	// If we are shutting down, ignore this request
	if (m_bPendingShutdown) {
		return;
	}

	std::lock_guard<std::mutex> lk(m_Mtx);
	auto it = std::remove_if(m_Devices.begin(), m_Devices.end(), [&Callback](const auto& Device) {
		if (Callback(Device.get()))
		{
			EmuLog(LOG_LEVEL::INFO, "Removed device: %s", Device->GetQualifiedName().c_str());
			return true;
		}
		return false;
		});
	if (it != m_Devices.end()) {
		m_Devices.erase(it, m_Devices.end());
	}
}

void InputDeviceManager::UpdateDevices(int port, bool ack)
{
	if (port > PORT_4 || port < PORT_1) {
		EmuLog(LOG_LEVEL::WARNING, "Invalid port number. The port was %d", PORT_INC(port));
		return;
	}
	int type;
#if 0 // lle usb
	int usb_port = PORT_DEC(Gui2XboxPortArray[port]);
#else
	int usb_port = port;
#endif
	g_EmuShared->GetInputDevTypeSettings(&type, port);

	// connect
	if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		//g_XidDeviceObjArray[usb_port].xid_dev == nullptr) { lle usb
		g_XboxControllerHostBridge[usb_port].XboxType == XBOX_INPUT_DEVICE::DEVICE_INVALID) {
		ConnectDevice(port, usb_port, type);
	}
	// disconnect
	else if (type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		//g_XidDeviceObjArray[usb_port].xid_dev != nullptr) { lle usb
		g_XboxControllerHostBridge[usb_port].XboxType != XBOX_INPUT_DEVICE::DEVICE_INVALID) {
		DisconnectDevice(port, usb_port, ack);
	}
	// update bindings
	else {
		auto dev = g_InputDeviceManager.FindDevice(usb_port, 0);
		if (dev != nullptr) {
			dev->SetPort(usb_port, false);
		}
		if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
			if (type != to_underlying(g_XboxControllerHostBridge[port].XboxType)) {
				// this will happen when the user changes the type of an existing xbox device type connected to a port
				if (g_XboxControllerHostBridge[port].bPendingRemoval == false) {
					g_XboxControllerHostBridge[port].bPendingRemoval = true;
					return;
				}
				else {
					DestructHleInputDevice(port);
					if (!ConstructHleInputDevice(type, port)) {
						return;
					}
				}
			}
			BindHostDevice(port, usb_port, type);
		}
	}
}

void InputDeviceManager::ConnectDevice(int port, int usb_port, int type)
{
#if 0 // lle usb
	switch (type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		if (ConstructHub(usb_port)) {
			if (!ConstructXpadDuke(usb_port)) {
				DestructHub(usb_port);
				return;
			}
			g_XidDeviceObjArray[usb_port].xid_type = type;
			EmuLog(LOG_LEVEL::INFO, "Attached device %s to port %d", GetInputDeviceName(type).c_str(), PORT_INC(port));
		}
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
	case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
	case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
	case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		EmuLog(LOG_LEVEL::INFO, "%s: device %s is not yet supported", __func__, GetInputDeviceName(type).c_str());
		return;
	}

	default:
		EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type (type was %d)", type);
		return;
	}
#else
	if (!ConstructHleInputDevice(type, port)) {
		return;
	}
	EmuLog(LOG_LEVEL::INFO, "Attached device %s to port %d", GetInputDeviceName(type).c_str(), PORT_INC(port));
#endif
	BindHostDevice(port, usb_port, type);
}

void InputDeviceManager::DisconnectDevice(int port, int usb_port, bool ack)
{
#if 0 // lle usb
	if (g_XidDeviceObjArray[usb_port].xid_dev != nullptr) {
		int type = g_XidDeviceObjArray[usb_port].xid_type;

		switch (type)
		{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			if (ack) {
				assert(g_HubObjArray[usb_port] != nullptr);
				DestructHub(usb_port);
				DestructXpadDuke(usb_port);
				g_HostController->OHCI_SetRemovalFlag(usb_port, false);
				EmuLog(LOG_LEVEL::INFO, "Detached device %s from port %d", GetInputDeviceName(type).c_str(), PORT_INC(port));
			}
			else {
				g_HostController->OHCI_SetRemovalFlag(usb_port, true);
			}
		}
		break;

		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
		case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
		case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
		case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
		case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
			EmuLog(LOG_LEVEL::INFO, "%s: device %s is not yet supported", __func__, GetInputDeviceName(type).c_str());
			return;
		}

		default:
			EmuLog(LOG_LEVEL::WARNING, "Attempted to detach an unknown device type (type was %d)", type);
			return;
		}
#endif
	if (g_XboxControllerHostBridge[port].XboxType != XBOX_INPUT_DEVICE::DEVICE_INVALID) {
		if (ack) {
			int type = to_underlying(g_XboxControllerHostBridge[port].XboxType);
			DestructHleInputDevice(port);
			EmuLog(LOG_LEVEL::INFO, "Detached device %s from port %d", GetInputDeviceName(type).c_str(), PORT_INC(port));
		}
		else {
			g_XboxControllerHostBridge[port].bPendingRemoval = true;
		}
		auto dev = g_InputDeviceManager.FindDevice(usb_port, 0);
		if (dev != nullptr) {
			dev->SetPort(usb_port, false);
		}
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "Attempted to detach a device not attached to the emulated machine");
	}
}

void InputDeviceManager::BindHostDevice(int port, int usb_port, int type)
{
	char dev_name[50];
	char dev_control_names[HIGHEST_NUM_BUTTONS][HOST_BUTTON_NAME_LENGTH];

	g_EmuShared->GetInputDevNameSettings(dev_name, port);
	g_EmuShared->GetInputBindingsSettings(dev_control_names, dev_num_buttons[type], port);

	auto dev = FindDevice(std::string(dev_name));
	if (dev != nullptr) {
		std::vector<InputDevice::IoControl*> controls = dev->GetIoControls();
		for (int index = 0; index < dev_num_buttons[type]; index++) {
			std::string dev_button(dev_control_names[index]);
			auto it = std::find_if(controls.begin(), controls.end(), [&dev_button](const auto control) {
				if (control->GetName() == dev_button) {
					return true;
				}
				return false;
				});
			dev->SetBindings(index, (it != controls.end()) ? *it : nullptr);
		}
		dev->SetPort(usb_port, true);
	}
}

bool InputDeviceManager::UpdateXboxPortInput(int usb_port, void* Buffer, int Direction, int xid_type)
{
	assert(Direction == DIRECTION_IN || Direction == DIRECTION_OUT);
	assert(xid_type > to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		xid_type < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX));
	bool has_changed = false;

	// If somebody else is currently holding the lock, we won't wait and instead report no input changes
	if (m_Mtx.try_lock()) {
		for (auto &dev_ptr : m_Devices) {
			if (dev_ptr->GetPort(usb_port)) {
				switch (xid_type)
				{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
				case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK): {
					has_changed = UpdateInputXpad(dev_ptr, Buffer, Direction);
				}
				break;

				case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
					has_changed = UpdateInputSBC(dev_ptr, Buffer, Direction, usb_port);
				}
				break;

				case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
				case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
				case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
				case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE): {
					EmuLog(LOG_LEVEL::WARNING, "An unsupported device is attached at port %d! The device was %s",
						Gui2XboxPortArray[usb_port], GetInputDeviceName(xid_type).c_str());
				}
				break;

				default: {
					EmuLog(LOG_LEVEL::WARNING, "An unknown device attached at port %d! The type was %s",
						Gui2XboxPortArray[usb_port], GetInputDeviceName(xid_type).c_str());
				}
				}
			}
		}
		m_Mtx.unlock();
	}
	return has_changed;
}

bool InputDeviceManager::UpdateInputXpad(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction)
{
	std::map<int, InputDevice::IoControl*> bindings = Device->GetBindings();
	assert(bindings.size() == static_cast<size_t>(dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE)]));

	if (Direction == DIRECTION_IN) {
		if (!Device->UpdateInput()) {
			return false;
		}

		//XpadInput* in_buf = reinterpret_cast<XpadInput*>(static_cast<uint8_t*>(Buffer) + 2); lle usb
		XpadInput *in_buf = static_cast<XpadInput *>(Buffer);
		for (int i = 0; i < 8; i++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
			if (state) {
				in_buf->wButtons |= (1 << i);
			}
			else {
				in_buf->wButtons &= ~(1 << i);
			}
		}

		for (int i = 8, j = 0; i < 16; i++, j++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
			in_buf->bAnalogButtons[j] = static_cast<uint8_t>(state * 0xFF);
		}

		for (int i = 16; i < 24; i += 2) {
			ControlState state_plus = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
			ControlState state_minus = (bindings[i + 1] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i + 1])->GetState() : 0.0;
			ControlState state = state_plus ? state_plus * 0x7FFF : state_minus ? -state_minus * 0x8000 : 0.0;
			switch (i)
			{
			case 16:
				in_buf->sThumbLX = static_cast<int16_t>(state);
				break;

			case 18:
				in_buf->sThumbLY = static_cast<int16_t>(state);
				break;

			case 20:
				in_buf->sThumbRX = static_cast<int16_t>(state);
				break;

			case 22:
				in_buf->sThumbRY = static_cast<int16_t>(state);
				break;

			}
		}
	}
	else {
		if (bindings[24] != nullptr) {
			//XpadOutput* out_buf = reinterpret_cast<XpadOutput*>(static_cast<uint8_t*>(Buffer) + 2); lle usb
			XpadOutput* out_buf = reinterpret_cast<XpadOutput*>(Buffer);
			dynamic_cast<InputDevice::Output*>(bindings[24])->SetState(out_buf->left_actuator_strength / static_cast<ControlState>(0xFFFF),
				out_buf->right_actuator_strength / static_cast<ControlState>(0xFFFF));
		}
	}

	return true;
}

bool InputDeviceManager::UpdateInputSBC(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction, int Port)
{
	std::map<int, InputDevice::IoControl*> bindings = Device->GetBindings();
	assert(bindings.size() == static_cast<size_t>(dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER)]));

	// NOTE: the output state is not supported
	if (Direction == DIRECTION_IN) {
		if (!Device->UpdateInput()) {
			return false;
		}

		// We change the toggle buttons only when a press -> release input transaction is completed
		// 0  -> CockpitHatch
		// 1  -> Ignition
		// 2  -> ToggleFilterControl
		// 3  -> ToggleOxygenSupply
		// 4  -> ToggleFuelFlowRate
		// 5  -> ToggleBuffreMaterial
		// 6  -> ToggleVTLocation
		// 7  -> TunerDial Up
		// 8  -> TunerDial Down
		// 9  -> GearLever Up
		// 10 -> GearLever Down
		static uint16_t last_in_state[4] = { 0, 0, 0, 0 };
		SBCInput *in_buf = static_cast<SBCInput *>(Buffer);
		for (int i = 0; i < 4; i++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			if (state) {
				in_buf->wButtons[0] |= (1 << i);
			}
			else {
				in_buf->wButtons[0] &= ~(1 << i);
			}
		}

		// CockpitHatch and Ignition are toggle buttons
		for (int i = 4, j = 0; i < 6; i++, j++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			uint16_t curr_in_state = static_cast<uint16_t>(!!state);
			if ((~curr_in_state) & ((last_in_state[Port] >> j) & 1)) {
				in_buf->wButtons[0] ^= (1 << i);
			}
			(last_in_state[Port] &= ~(1 << j)) |= (curr_in_state << j);
		}

		for (int i = 6; i < 34; i++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			if (state) {
				in_buf->wButtons[i / 16] |= (1 << (i % 16));
			}
			else {
				in_buf->wButtons[i / 16] &= ~(1 << (i % 16));
			}
		}

		// The last five digital buttons are toggle buttons
		for (int i = 34, j = 2; i < 39; i++, j++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			uint16_t curr_in_state = static_cast<uint16_t>(!!state);
			if ((~curr_in_state) & ((last_in_state[Port] >> j) & 1)) {
				in_buf->wButtons[2] ^= (1 << (i % 16));
			}
			(last_in_state[Port] &= ~(1 << j)) |= (curr_in_state << j);
		}

		for (int i = 39; i < 49; i += 2) {
			ControlState state_plus = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			ControlState state_minus = (bindings[i + 1] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i + 1])->GetState() : 0.0;
			switch (i)
			{
			case 39:
				// NOTE: the center of the stick is 0x7F, not zero
				in_buf->sAimingX = static_cast<uint8_t>(state_plus ? (0x80 * state_plus) + 0x7F : state_minus ? (1.0 - state_minus) * 0x7F : 0x7F);
				break;

			case 41:
				// NOTE: the center of the stick is 0x7F, not zero
				in_buf->sAimingY = static_cast<uint8_t>(state_plus ? (1.0 - state_plus) * 0x7F : state_minus ? (0x80 * state_minus) + 0x7F : 0x7F);
				break;

			case 43:
				// left: negative, right: positive
				in_buf->sRotationLever = static_cast<int8_t>(static_cast<int8_t>(state_plus ? -state_plus * 0x80 : state_minus ? state_minus * 0x7F : 0.0));
				break;

			case 45:
				// left: positive, right: negative
				in_buf->sSightChangeX = static_cast<int8_t>(state_plus ? state_plus * 0x7F : state_minus ? -state_minus * 0x80 : 0.0);
				break;

			case 47:
				// up: negative, down: positive
				in_buf->sSightChangeY = static_cast<int8_t>(state_plus ? -state_plus * 0x80 : state_minus ? state_minus * 0x7F : 0.0);
				break;

			}
		}

		for (int i = 49; i < 52; i++) {
			ControlState state = (bindings[i] != nullptr) ? (dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() * 0xFF) : 0.0;
			switch (i)
			{
			case 49:
				in_buf->wLeftPedal = static_cast<uint8_t>(state);
				break;

			case 50:
				in_buf->wMiddlePedal = static_cast<uint8_t>(state);
				break;

			case 51:
				in_buf->wRightPedal = static_cast<uint8_t>(state);
				break;

			}
		}

		// TunerDial and GearLever work like toggles
		for (int i = 52, j = 7; i < 56; i++, j++) {
			ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input *>(bindings[i])->GetState() : 0.0;
			uint16_t curr_in_state = static_cast<uint16_t>(!!state);
			if ((~curr_in_state) & ((last_in_state[Port] >> j) & 1)) {
				switch (i)
				{
				case 52:
					if (in_buf->ucTunerDial != 12) {
						in_buf->ucTunerDial += 1;
					}
					break;

				case 53:
					if (in_buf->ucTunerDial != 0) {
						in_buf->ucTunerDial -= 1;
					}
					break;

				case 54:
					if (in_buf->ucGearLever != 13) {
						in_buf->ucGearLever += 1;
					}
					break;

				case 55:
					if (in_buf->ucGearLever != 7) {
						in_buf->ucGearLever -= 1;
					}
					break;
				}
			}
			(last_in_state[Port] &= ~(1 << j)) |= (curr_in_state << j);
		}
	}

	return true;
}

void InputDeviceManager::RefreshDevices()
{
	std::unique_lock<std::mutex> lck(m_Mtx);
	Sdl::SdlPopulateOK = false;
	m_Devices.clear();
	lck.unlock();
	XInput::PopulateDevices();
	DInput::PopulateDevices();
	Sdl::PopulateDevices();
	lck.lock();
	m_Cv.wait(lck, []() {
		return Sdl::SdlPopulateOK;
		});
	for (auto &dev : m_Devices) {
		if (StrStartsWith(dev->GetDeviceName(), "KeyboardMouse")) {
			static_cast<DInput::KeyboardMouse *>(dev.get())->SetHwnd(m_hwnd);
			break;
		}
	}
}

std::vector<std::string> InputDeviceManager::GetDeviceList() const
{
	std::vector<std::string> dev_list;
	std::lock_guard<std::mutex> lck(m_Mtx);

	std::for_each(m_Devices.begin(), m_Devices.end(), [&dev_list](const auto& Device) {
		dev_list.push_back(Device->GetQualifiedName());
		});

	return dev_list;
}

std::shared_ptr<InputDevice> InputDeviceManager::FindDevice(const std::string& QualifiedName) const
{
	std::lock_guard<std::mutex> lck(m_Mtx);

	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [&QualifiedName](const auto& Device) {
		return QualifiedName == Device->GetQualifiedName();
		});
	if (it != m_Devices.end()) {
		return *it;
	}
	else {
		return nullptr;
	}
}

std::shared_ptr<InputDevice> InputDeviceManager::FindDevice(SDL_JoystickID id) const
{
	std::lock_guard<std::mutex> lck(m_Mtx);

	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [id](const auto& Device) {
		return id == Device->GetId(id);
		});
	if (it != m_Devices.end()) {
		return *it;
	}
	else {
		return nullptr;
	}
}

std::shared_ptr<InputDevice> InputDeviceManager::FindDevice(int usb_port, int dummy) const
{
	// Ignore dummy, it's just used to overload the function

	if (usb_port == PORT_INVALID) {
		return nullptr;
	}

	std::lock_guard<std::mutex> lck(m_Mtx);

	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [usb_port](const auto& Device) {
		return Device->GetPort(usb_port);
		});
	if (it != m_Devices.end()) {
		return *it;
	}
	else {
		return nullptr;
	}
}

void InputDeviceManager::UpdateOpt(bool is_gui)
{
	if (!is_gui) {
		long axis_range, wheel_range;
		g_EmuShared->GetInputMoAxisSettings(&axis_range);
		g_EmuShared->GetInputMoWheelSettings(&wheel_range);
		DInput::mo_axis_range_pos = axis_range;
		DInput::mo_wheel_range_pos = wheel_range;
		DInput::mo_axis_range_neg = -(axis_range);
		DInput::mo_wheel_range_neg = -(wheel_range);
	}
	else {
		DInput::mo_axis_range_pos = g_Settings->m_input_general.MoAxisRange;
		DInput::mo_wheel_range_pos = g_Settings->m_input_general.MoWheelRange;
		DInput::mo_axis_range_neg = -(g_Settings->m_input_general.MoAxisRange);
		DInput::mo_wheel_range_neg = -(g_Settings->m_input_general.MoWheelRange);
	}
}
