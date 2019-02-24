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

#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::INPSYS

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include "SdlJoystick.h"
#include "XInputPad.h"
#include "DInputKeyboardMouse.h"
#include "InputManager.h"
#include "..\devices\usb\XidGamepad.h"
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog
#include "EmuShared.h"
#include "devices\usb\OHCI.h"


InputDeviceManager g_InputDeviceManager;

void InputDeviceManager::Initialize(bool is_gui)
{
	// Sdl::Init must be called last since it blocks when it succeeds
	std::unique_lock<std::mutex> lck(m_Mtx);
	m_bPendingShutdown = false;
	
	m_PollingThread = std::thread([this]() {
		XInput::Init(m_Mtx);
		Sdl::Init(m_Mtx, m_Cv);
	});

	m_Cv.wait(lck, []() {
		return (Sdl::SdlInitStatus != Sdl::SDL_NOT_INIT) &&
			(XInput::XInputInitStatus != XInput::XINPUT_NOT_INIT);
	});
	lck.unlock();

	if (Sdl::SdlInitStatus < 0 || XInput::XInputInitStatus < 0) {
		CxbxKrnlCleanup("Failed to initialize input subsystem! Consult debug log for more information");
	}

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
	Device->SetPort(PORT_INVALID);
	Device->SetType(to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID));

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
	int type;
	g_EmuShared->GetInputDevTypeSettings(&type, port);

	if (type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		g_HubObjArray[port] == nullptr) {
		ConnectDevice(port, type);
	}
	else if (type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
		g_HubObjArray[port] != nullptr) {
		DisconnectDevice(port, ack);
	}
}

void InputDeviceManager::ConnectDevice(int port, int type)
{
	switch (type)
	{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			if (ConstructHub(port)) {
				if (!ConstructXpadDuke(port)) {
					DestructHub(port);
				}
				g_XidDeviceObjArray[port].xid_type = type;
			}	
		}
		break;

		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
		case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
		case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
		case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
		case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
			EmuLog(LOG_LEVEL::INFO, "Device type %d is not yet supported", type);
			return;
		}

		default:
			EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type (type was %d)", type);
			return;
	}

	BindHostDevice(port, type);
}

void InputDeviceManager::DisconnectDevice(int port, bool ack)
{
	if (port > PORT_4 || port < PORT_1) {
		EmuLog(LOG_LEVEL::WARNING, "Invalid port number. The port was %d", port);
		return;
	}

	if (g_XidDeviceObjArray[port].xid_dev != nullptr) {
		assert(g_HubObjArray[port] != nullptr);
		int type = g_XidDeviceObjArray[port].xid_type;

		switch (type)
		{
			case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
				if (ack) {
					DestructHub(port);
					DestructXpadDuke(port);
					g_HostController->OHCI_SetRemovalFlag(port, false);
				}
				else {
					g_HostController->OHCI_SetRemovalFlag(port, true);
				}
			}
			break;

			case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
			case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
			case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
			case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
			case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
			case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
				EmuLog(LOG_LEVEL::INFO, "Device type %d is not yet supported", type);
			}
			break;

			default:
				EmuLog(LOG_LEVEL::WARNING, "Attempted to detach an unknown device type (type was %d)", type);
		}
		auto dev = g_InputDeviceManager.FindDeviceByPort(port);
		if (dev != nullptr) {
			dev->SetType(to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID));
			dev->SetPort(PORT_INVALID);
		}
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "Attempted to detach a device not attached to the emulated machine");
	}
}

void InputDeviceManager::BindHostDevice(int port, int type)
{
	char dev_name[50];
	char dev_control_names[XBOX_CTRL_NUM_BUTTONS][30];

	g_EmuShared->GetInputDevNameSettings(dev_name, port);
	g_EmuShared->GetInputBindingsSettings(dev_control_names, XBOX_CTRL_NUM_BUTTONS, port);

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
		dev->SetType(type);
		dev->SetPort(port);
	}
}

bool InputDeviceManager::UpdateXboxPortInput(int Port, void* Buffer, int Direction)
{
	assert(Direction == DIRECTION_IN || Direction == DIRECTION_OUT);
	bool has_changed = false;

	// If somebody else is currently holding the lock, we won't wait and instead report no input changes
	if (m_Mtx.try_lock()) {
		for (auto dev_ptr : m_Devices) {
			if (dev_ptr->GetPort() == Port) {
				switch (dev_ptr->GetType())
				{
					case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
						has_changed = UpdateInputXpad(dev_ptr, Buffer, Direction);
					}
					break;

					case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
					case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN):
					case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
					case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
					case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
					case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
						EmuLog(LOG_LEVEL::WARNING, "Port %d has an unsupported device attached! The type was %d", Port, dev_ptr->GetType());
					}
					break;

					default: {
						EmuLog(LOG_LEVEL::WARNING, "Port %d has an unknown device attached! The type was %d", Port, dev_ptr->GetType());
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
		XpadInput* in_buf = reinterpret_cast<XpadInput*>(static_cast<uint8_t*>(Buffer) + 2);
		if (!Device->UpdateInput()) {
			return false;
		}

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

		/* FIXME: is this correct? */
		for (int i = 16, j = 0; i < 24; i += 2, j++) {
			ControlState state_plus = (bindings[i] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i])->GetState() : 0.0;
			ControlState state_minus = (bindings[i+1] != nullptr) ? dynamic_cast<InputDevice::Input*>(bindings[i+1])->GetState() : 0.0;
			ControlState state = state_plus ? state_plus * 0x7FFF : state_minus ? state_minus * 0x8000 : 0.0;
			switch (j)
			{
				case 0: {
					in_buf->sThumbLX = static_cast<int16_t>(state);
				}
				break;

				case 1: {
					in_buf->sThumbLY = static_cast<int16_t>(state);
				}
				break;

				case 2: {
					in_buf->sThumbRX = static_cast<int16_t>(state);
				}
				break;

				case 3: {
					in_buf->sThumbRY = static_cast<int16_t>(state);
				}
				break;

				default: {
					// unreachable
				}
			}
		}
	}
	else {
		if (bindings[24] != nullptr) {
			XpadOutput* out_buf = reinterpret_cast<XpadOutput*>(static_cast<uint8_t*>(Buffer) + 2);
			dynamic_cast<InputDevice::Output*>(bindings[24])->SetState(out_buf->left_actuator_strength / 0xFFFF,
				out_buf->right_actuator_strength / 0xFFFF);
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

std::shared_ptr<InputDevice> InputDeviceManager::FindDevice(std::string& QualifiedName) const
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

std::shared_ptr<InputDevice> InputDeviceManager::FindDeviceByPort(int port) const
{
	std::lock_guard<std::mutex> lck(m_Mtx);

	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [port](const auto& Device) {
		return port == Device->GetPort();
	});
	if (it != m_Devices.end()) {
		return *it;
	}
	else {
		return nullptr;
	}
}
