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
#include "RawDevice.h"
#include "SteelBattalionController.h"
#include "LibusbXboxController.h"
#include "DInputKeyboardMouse.h"
#include "InputManager.h"
#include "..\devices\usb\XidGamepad.h"
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog
#include "EmuShared.h"
#include "devices\usb\OHCI.h"
#include "core/common/video/RenderBase.hpp"
#include <charconv>

// hle input specific
#include "core\hle\XAPI\Xapi.h"


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

void UpdateXppState(DeviceState *dev, XBOX_INPUT_DEVICE type, std::string_view port);


InputDeviceManager g_InputDeviceManager;

void InputDeviceManager::Initialize(bool is_gui, HWND hwnd)
{
	// Sdl::Init must be called last since it blocks when it succeeds
	std::unique_lock<std::mutex> lck(m_Mtx);
	m_bPendingShutdown = false;
	m_hwnd = hwnd;

	m_PollingThread = std::thread([this, is_gui]() {
		// This code can run in both cxbx.exe and cxbxr-ldr.exe, but will not have
		// the affinity policy when running in the former.
		if (g_AffinityPolicy) {
			g_AffinityPolicy->SetAffinityOther();
		}

		XInput::Init(m_Mtx);
		RawInput::Init(m_Mtx, is_gui, m_hwnd);
		Sdl::Init(m_Mtx, m_Cv, is_gui);
		});

	m_Cv.wait(lck, []() {
		return (Sdl::InitStatus != Sdl::NOT_INIT) &&
			(XInput::InitStatus != XInput::NOT_INIT) &&
			(RawInput::InitStatus != RawInput::NOT_INIT);
		});
	lck.unlock();

	if (Sdl::InitStatus < 0 || XInput::InitStatus < 0 || RawInput::InitStatus < 0) {
		CxbxrKrnlAbort("Failed to initialize input subsystem! Consult debug log for more information");
	}

	UpdateOpt(is_gui);
	RefreshDevices();

	if (!is_gui) {
		for (unsigned i = 0; i < MAX_DEVS; ++i) {
			g_devs[i].type = XBOX_INPUT_DEVICE::DEVICE_INVALID;
			g_devs[i].port = std::to_string(PORT_INVALID);
		}

		for (unsigned i = 0; i < XBOX_NUM_PORTS; ++i) {
			int type;
			g_EmuShared->GetInputDevTypeSettings(&type, i);
			if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
				std::string port = std::to_string(i);
				switch (type)
				{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
					ConstructHleInputDevice(&g_devs[CTRL_OFFSET + i], nullptr, type, port);
					BindHostDevice(type, port);
					for (unsigned slot = 0; slot < XBOX_CTRL_NUM_SLOTS; ++slot) {
						g_EmuShared->GetInputSlotTypeSettings(&type, i, slot);
						if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
							assert(type == to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT));
							ConstructHleInputDevice(&g_devs[MU_OFFSET + (XBOX_CTRL_NUM_SLOTS * i) + slot], &g_devs[CTRL_OFFSET + i], type, port + "." + std::to_string(slot));
						}
					}
				}
				break;

				case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
				case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
					ConstructHleInputDevice(&g_devs[CTRL_OFFSET + i], nullptr, type, port);
					BindHostDevice(type, port);
					break;

				default:
					assert(0);
				}
			}
		}
	}

	RawInput::IgnoreHotplug = false;
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
	RawInput::DeInit();
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

void InputDeviceManager::UpdateDevices(std::string_view port, bool ack)
{
	DeviceState *dev, *upstream;
	int port_num, slot, type;
	PortStr2Int(port, &port_num, &slot);
	dev = &g_devs[port_num];

	if (slot == PORT_INVALID) { // Port references a device attached to an xbox port
		upstream = nullptr;
		g_EmuShared->GetInputDevTypeSettings(&type, port_num);
	}
	else { // Port references a device attached to a slot port
		upstream = dev;
		dev = dev->slots[slot];
		g_EmuShared->GetInputSlotTypeSettings(&type, port_num, slot);
	}

	// updating a slot
	if (dev == nullptr) {
		// connect slot
		if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
			g_devs[MU_OFFSET + (XBOX_CTRL_NUM_SLOTS * port_num) + slot].type == XBOX_INPUT_DEVICE::DEVICE_INVALID) {
			ConnectDevice(&g_devs[MU_OFFSET + (XBOX_CTRL_NUM_SLOTS * port_num) + slot], upstream, type, port);
		}
		// disconnect slot
		else if (type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
			g_devs[MU_OFFSET + (XBOX_CTRL_NUM_SLOTS * port_num) + slot].type != XBOX_INPUT_DEVICE::DEVICE_INVALID) {
			DisconnectDevice(&g_devs[MU_OFFSET + (XBOX_CTRL_NUM_SLOTS * port_num) + slot], port, ack);
		}
		// update bindings slot
		else {
			// MUs don't have any host devices attached, so this is a nop for now
		}
	}
	// connect
	else if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		dev->type == XBOX_INPUT_DEVICE::DEVICE_INVALID) {
		ConnectDevice(dev, upstream, type, port);
	}
	// disconnect
	else if (type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		dev->type != XBOX_INPUT_DEVICE::DEVICE_INVALID) {
		// We don't need to check if we need to destroy child devices because the UpdateInputEvent_t message always
		// calls us on the entire slot connectivity of a port
		DisconnectDevice(dev, port, ack);
	}
	// update bindings
	else {
		auto host_dev = g_InputDeviceManager.FindDevice(port);
		if (host_dev != nullptr) {
			host_dev->SetPort(port, false);
		}
		if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
			if (type != to_underlying(dev->type)) {
				// This will happen when the user changes the type of an existing xbox device type connected to a port.
				// We don't need to check of we need to destroy child devices because the UpdateInputEvent_t message always
				// calls us on the entire slot connectivity if the device has slots available
				if (dev->bPendingRemoval == false) {
					dev->bPendingRemoval = true;
					return;
				}
				else {
					DestructHleInputDevice(dev);
					ConstructHleInputDevice(dev, upstream, type, port);
				}
			}
			BindHostDevice(type, port);
		}
	}
}

void InputDeviceManager::ConnectDevice(DeviceState *dev, DeviceState *upstream, int type, std::string_view port)
{
	ConstructHleInputDevice(dev, upstream, type, port);
	BindHostDevice(type, port);
}

void InputDeviceManager::DisconnectDevice(DeviceState *dev, std::string_view port, bool ack)
{
	if (ack) {
		DestructHleInputDevice(dev);
	}
	else {
		dev->bPendingRemoval = true;
	}
	auto host_dev = g_InputDeviceManager.FindDevice(port);
	if (host_dev != nullptr) {
		host_dev->SetPort(port, false);
	}
}

void InputDeviceManager::BindHostDevice(int type, std::string_view port)
{
	if (type == to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT)) {
		// MUs don't have any host devices bound, so we just return
		return;
	}

	char dev_name[50];
	char dev_control_names[HIGHEST_NUM_BUTTONS][HOST_BUTTON_NAME_LENGTH];
	int port_num, slot;
	PortStr2Int(port, &port_num, &slot);
	g_EmuShared->GetInputDevNameSettings(dev_name, port_num);
	g_EmuShared->GetInputBindingsSettings(dev_control_names, dev_num_buttons[type], port_num);

	auto dev = FindDevice(std::string(dev_name));
	if (dev != nullptr) {
		std::string port_str(port);
		dev->ClearBindings(port_str);
		std::vector<InputDevice::IoControl *> controls = dev->GetIoControls();
		for (int index = 0; index < dev_num_buttons[type]; index++) {
			std::string dev_button(dev_control_names[index]);
			auto it = std::find_if(controls.begin(), controls.end(), [&dev_button](const auto control) {
				if (control->GetName() == dev_button) {
					return true;
				}
				return false;
				});
			dev->SetBindings(index, (it != controls.end()) ? *it : nullptr, port_str);
		}
		dev->SetPort(port, true);
	}
}

bool InputDeviceManager::UpdateXboxPortInput(int port, void* buffer, int direction, int type)
{
	assert(direction == DIRECTION_IN || direction == DIRECTION_OUT);
	assert(type > to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		type < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX));
	bool has_changed = false;

	// First check if ImGui is focus, then ignore any input update occur.
	// If somebody else is currently holding the lock, we won't wait and instead report no input changes
	if (!g_renderbase->IsImGuiFocus() && m_Mtx.try_lock()) {
		for (auto &dev : m_Devices) {
			std::string port_str = std::to_string(port);
			if (dev->GetPort(port_str)) {
				switch (type)
				{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
				case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
					has_changed = UpdateInputXpad(dev, buffer, direction, port_str);
					m_Mtx.unlock();
					return has_changed;

				case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
					has_changed = UpdateInputSBC(dev, buffer, direction, port, port_str);
					m_Mtx.unlock();
					return has_changed;

				case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
					assert(0);
					break;

				case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
				case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
				case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
					EmuLog(LOG_LEVEL::ERROR2, "An unsupported device is attached at port %d! The device was %s",
						PortUserFormat(port_str).c_str(), GetInputDeviceName(type).c_str());
					break;

				}
			}
		}
		m_Mtx.unlock();
	}

	return has_changed;
}

bool InputDeviceManager::UpdateInputXpad(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction, const std::string &Port)
{
	std::map<int, InputDevice::IoControl*> bindings = Device->GetBindings(Port);
	assert(bindings.size() == static_cast<size_t>(dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE)]));

	auto libusb = std::dynamic_pointer_cast<libusbXbox::LibusbXboxController>(Device);
	if (libusb != nullptr)
	{
		if (Direction == DIRECTION_IN)
		{
			if (!Device->UpdateInput()) {
				return false;
			}

			memcpy(Buffer, libusb->GetRawControlData(), sizeof(XpadInput));
		}
		else
		{
			// XpadOutput isn't in the right format for the controller. It'll need to be formatted
			const XpadOutput* output = static_cast<XpadOutput*>(Buffer);
			uint8_t buffer[6];
			memset(buffer, 0, 6);
			buffer[1] = 6;
			buffer[3] = output->left_actuator_strength / 256;
			buffer[5] = output->right_actuator_strength / 256;
			libusb->SetRumble(buffer, 6);
		}
	}
	else
	{
		if (Direction == DIRECTION_IN) {
			if (!Device->UpdateInput()) {
				return false;
			}

			//XpadInput* in_buf = reinterpret_cast<XpadInput*>(static_cast<uint8_t*>(Buffer) + 2); lle usb
			XpadInput* in_buf = static_cast<XpadInput*>(Buffer);
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
		else
		{
			if (bindings[24] != nullptr)
			{
				//XpadOutput* out_buf = reinterpret_cast<XpadOutput*>(static_cast<uint8_t*>(Buffer) + 2); lle usb
				XpadOutput* out_buf = reinterpret_cast<XpadOutput*>(Buffer);
				dynamic_cast<InputDevice::Output*>(bindings[24])->SetState(out_buf->left_actuator_strength / static_cast<ControlState>(0xFFFF),
					out_buf->right_actuator_strength / static_cast<ControlState>(0xFFFF));
			}
		}
	}

	return true;
}

bool InputDeviceManager::UpdateInputSBC(std::shared_ptr<InputDevice>& Device, void* Buffer, int Direction, int Port_num, const std::string &Port)
{
	std::map<int, InputDevice::IoControl*> bindings = Device->GetBindings(Port);
	assert(bindings.size() == static_cast<size_t>(dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER)]));

	auto sbc = std::dynamic_pointer_cast<SBC::SteelBattalionController>(Device);
	if (sbc != nullptr)
	{
		if (Direction == DIRECTION_IN)
		{
			auto rawControlData = sbc->GetRawControlData();
			// Copy the raw data data into the buffer (skipping the first two bytes, because they're not used)
			memcpy(Buffer, rawControlData, sizeof(SBCInput));
		}
		else
		{
			sbc->SetLEDBuffer((const uint8_t*)Buffer, sizeof(SBCOutput));
		}
	}
	else
	{
		// NOTE: the output state is not supported
		if (Direction == DIRECTION_IN) {
			if (!Device->UpdateInput()) {
				return false;
			}

			static uint16_t last_in_state[XBOX_NUM_PORTS] = { 0, 0, 0, 0 };
			SBCInput* in_buf = static_cast<SBCInput*>(Buffer);

			for (int i = 0; i < 39; i++) {
				ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
				if (state) {
					in_buf->wButtons[i / 16] |= (1 << (i % 16));
				}
				else {
					in_buf->wButtons[i / 16] &= ~(1 << (i % 16));
				}
			}

			for (int i = 39; i < 49; i += 2) {
				ControlState state_plus = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
				ControlState state_minus = (bindings[i + 1] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i + 1])->GetState() : 0.0;
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
				ControlState state = (bindings[i] != nullptr) ? (dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() * 0xFF) : 0.0;
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
				ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
				uint16_t curr_in_state = static_cast<uint16_t>(!!state);
				if ((~curr_in_state) & ((last_in_state[Port_num] >> j) & 1)) {
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
				(last_in_state[Port_num] &= ~(1 << j)) |= (curr_in_state << j);
			}

			for (int i = 56; i < 79; i++)
			{
				ControlState state = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
				switch (i)
				{
					/* Shifter States */
				case 56:
					if (state) in_buf->ucGearLever = 7;
					break;
				case 57:
					if (state) in_buf->ucGearLever = 8;
					break;
				case 58:
					if (state) in_buf->ucGearLever = 9;
					break;
				case 59:
					if (state) in_buf->ucGearLever = 10;
					break;
				case 60:
					if (state) in_buf->ucGearLever = 11;
					break;
				case 61:
					if (state) in_buf->ucGearLever = 12;
					break;
				case 62:
					if (state) in_buf->ucGearLever = 13;
					break;

					/* TunerDial States */
				case 63:
					if (state) in_buf->ucTunerDial = 0;
					break;
				case 64:
					if (state) in_buf->ucTunerDial = 1;
					break;
				case 65:
					if (state) in_buf->ucTunerDial = 2;
					break;
				case 66:
					if (state) in_buf->ucTunerDial = 3;
					break;
				case 67:
					if (state) in_buf->ucTunerDial = 4;
					break;
				case 68:
					if (state) in_buf->ucTunerDial = 5;
					break;
				case 69:
					if (state) in_buf->ucTunerDial = 6;
					break;
				case 70:
					if (state) in_buf->ucTunerDial = 7;
					break;
				case 71:
					if (state) in_buf->ucTunerDial = 8;
					break;
				case 72:
					if (state) in_buf->ucTunerDial = 9;
					break;
				case 73:
					if (state) in_buf->ucTunerDial = 10;
					break;
				case 74:
					if (state) in_buf->ucTunerDial = 11;
					break;
				case 75:
					if (state) in_buf->ucTunerDial = 12;
					break;
				case 76:
					if (state) in_buf->ucTunerDial = 13;
					break;
				case 77:
					if (state) in_buf->ucTunerDial = 14;
					break;
				case 78:
					if (state) in_buf->ucTunerDial = 15;
					break;
				}
			}
		}
	}
	return true;
}

void InputDeviceManager::RefreshDevices()
{
	std::unique_lock<std::mutex> lck(m_Mtx);
	Sdl::PopulateOK = false;
	m_Devices.clear();
	lck.unlock();
	XInput::PopulateDevices();
	DInput::PopulateDevices();
	Sdl::PopulateDevices();
	SBC::SteelBattalionController::PopulateDevices();
	libusbXbox::LibusbXboxController::PopulateDevices();
	lck.lock();
	m_Cv.wait(lck, []() {
		return Sdl::PopulateOK;
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

std::shared_ptr<InputDevice> InputDeviceManager::FindDevice(std::string_view port) const
{
	std::lock_guard<std::mutex> lck(m_Mtx);

	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [port](const auto& Device) {
		return Device->GetPort(port);
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
		Settings::s_input_general input_general;
		g_EmuShared->GetInputGeneralSettings(&input_general);
		DInput::mo_axis_range_pos = input_general.MoAxisRange;
		DInput::mo_wheel_range_pos = input_general.MoWheelRange;
		DInput::mo_axis_range_neg = -(input_general.MoAxisRange);
		DInput::mo_wheel_range_neg = -(input_general.MoWheelRange);
		DInput::IgnoreKbMoUnfocus = input_general.IgnoreKbMoUnfocus;
	}
	else {
		DInput::mo_axis_range_pos = g_Settings->m_input_general.MoAxisRange;
		DInput::mo_wheel_range_pos = g_Settings->m_input_general.MoWheelRange;
		DInput::mo_axis_range_neg = -(g_Settings->m_input_general.MoAxisRange);
		DInput::mo_wheel_range_neg = -(g_Settings->m_input_general.MoWheelRange);
	}
}

void InputDeviceManager::HotplugHandler(bool is_sdl)
{
	// RawInput will start to send WM_INPUT_DEVICE_CHANGE as soon as RegisterRawInputDevices succeeds, but at that point, the input manager
	// is still not completely initialized, so we ignore hotplug events during initialization
	if (m_bPendingShutdown || RawInput::IgnoreHotplug) {
		return;
	}

	// NOTE1: sdl devices are monitored by sdl with the SDL_JOYDEVICEADDED and SDL_JOYDEVICEREMOVED messages,
	// and xinput devices are monitored by rawinput with the WM_INPUT_DEVICE_CHANGE message
	// NOTE2: sdl devices are already added/removed to/from m_Devices with the above events, so don't need to update m_Devices here again
	if (!is_sdl) {
		std::unique_lock<std::mutex> lck(m_Mtx);

		auto it = std::remove_if(m_Devices.begin(), m_Devices.end(), [](const auto &Device) {
			if (StrStartsWith(Device->GetAPI(), "XInput")) {
				return true;
			}
			return false;
			});
		if (it != m_Devices.end()) {
			m_Devices.erase(it, m_Devices.end());
		}

		lck.unlock();
		XInput::PopulateDevices();
	}

	for (int port = PORT_1; port <= PORT_4; ++port) {
		int type;
		g_EmuShared->GetInputDevTypeSettings(&type, port);
		if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
			BindHostDevice(type, std::to_string(port));
		}
	}
}
