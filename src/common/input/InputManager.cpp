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
#if 1 // Reenable this when LLE USB actually works
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::SDL

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include "InputManager.h"
#include "SdlJoystick.h"
#include "..\devices\usb\XidGamepad.h"
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog
#include <thread>


InputDeviceManager* g_InputDeviceManager = nullptr;
constexpr ControlState INPUT_DETECT_THRESHOLD = 0.55; // arbitrary number, using what Dolphin uses

InputDeviceManager::InputDeviceManager()
{
	// Delegate the sdl initialization to another thread. This is because, as per SDL documentation, SDL_PollEvent can only be called
	// from the same thread that initialized the video subsystem

	m_bExitOK = true;
	m_bInitOK = false;
	std::thread(&InputDeviceManager::InputMainLoop, this).detach();
}

InputDeviceManager::~InputDeviceManager()
{
	if (m_bInitOK) {
		SDL_PushEvent(&m_ExitLoop);
	}
	while (!m_bExitOK) {}
}

void InputDeviceManager::OpenSdlDevice(const int Index)
{
	InputDevice* pDev;
	SDL_Joystick* pJoystick;

	pJoystick = SDL_JoystickOpen(Index);
	if (pJoystick) {
		pDev = new SdlJoystick(pJoystick, Index);
		// only add the device if it has some I/O controls
		if (!pDev->GetInputs.empty() || !pDev->GetOutputs.empty()) {
			AddDevice(pDev);
		}
		else {
			EmuLog(LOG_LEVEL::INFO, "Rejected joystick %i. No controls detected\n", Index);
			delete pDev;
		}
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "Failed to open joystick %i. The error was %s\n", Index, SDL_GetError());
	}
}

void InputDeviceManager::CloseSdlDevice(const int Index)
{
	RemoveDevice([&Index](const auto& d) {
		const SdlJoystick* joystick = dynamic_cast<const SdlJoystick*>(device);
		return joystick && SDL_JoystickInstanceID(joystick->GetSDLJoystick()) == Index;
	});
}

void InputDeviceManager::InputMainLoop()
{
	uint32_t ExitEvent_t;
	SDL_Event Event;

	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
		CxbxKrnlCleanupEx(CXBXR_MODULE::INIT, "Failed to initialize SDL subsystem. The error was: %s", SDL_GetError());
		return;
	}
	ExitEvent_t = SDL_RegisterEvents(1);
	if (ExitEvent_t == (uint32_t)-1) {
		CxbxKrnlCleanupEx(CXBXR_MODULE::INIT, "Failed to create SDL exit event.");
		return;
	}
	SDL_memset(&m_ExitLoop, 0, sizeof(SDL_Event));
	m_ExitLoop.type = ExitEvent_t;
	m_bInitOK = true;
	m_bExitOK = false;

	SetThreadAffinityMask(GetCurrentThread(), g_CPUOthers);
	
	while (true)
	{
		while (SDL_PollEvent(&Event))
		{
			if (Event.type == SDL_JOYDEVICEADDED) {
				OpenSdlDevice(Event.jdevice.which);
			}
			else if (Event.type == SDL_JOYDEVICEREMOVED) {
				CloseSdlDevice(Event.jdevice.which);
			}
			else if (Event.type == ExitEvent_t) { // this doesn't work now...
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	SDL_Quit();
	m_bExitOK = true;
}

void InputDeviceManager::AddDevice(InputDevice* Device)
{
	std::vector<InputDevice::Input*> Inputs;
	int ID;

	// If we are shutdown (or in process of shutting down) ignore this request:
	if (!m_bInitOK) {
		return;
	}

	//std::lock_guard<std::mutex> lk(m_devices_mutex);
	// Try to find an ID for this device
	ID = 0;
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
	Device->SetXPort(XBOX_PORT_INVALID);
	Device->SetXType(to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID));

	EmuLog(LOG_LEVEL::INFO, "Added device: %s", Device->GetQualifiedName().c_str());
	m_Devices.emplace_back(std::move(Device));

	//if (!m_is_populating_devices)
	//	InvokeDevicesChangedCallbacks();
}

void InputDeviceManager::RemoveDevice(std::function<bool(const InputDevice*)> Callback)
{
	//std::lock_guard<std::mutex> lk(m_devices_mutex);
	auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [&Callback](const auto& dev) {
		if (Callback(dev)))
		{
			EmuLog(LOG_LEVEL::INFO, "Removed device: %s", dev->GetQualifiedName().c_str());
			return true;
		}
		return false;
	});
	delete *it;
	m_Devices.erase(it);
}

int InputDeviceManager::ConnectDeviceToXbox(int port, int type)
{
	int ret = -1;
	std::vector<SdlDevice*>::iterator it;

	if (port > 4 || port < 1) { return ret; };

	for (it = m_SdlDevices.begin(); it != m_SdlDevices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			--port;
			break;
		}
	}

	if (it == m_SdlDevices.end()) {
		EmuLog(LOG_LEVEL::WARNING, "Attempted to connect a device not yet enumerated.");
		return ret;
	}

	switch (type)
	{
		case MS_CONTROLLER_DUKE: {
			if (g_HubObjArray[port] == nullptr) {
				g_HubObjArray[port] = new Hub;
				ret = g_HubObjArray[port]->Init(port + 1);
				if (ret) {
					delete g_HubObjArray[port];
					g_HubObjArray[port] = nullptr;
					break;
				}
				if (g_XidControllerObjArray[port] == nullptr) {
					g_XidControllerObjArray[port] = new XidGamepad;
					ret = g_XidControllerObjArray[port]->Init(port + 1);
					if (ret) {
						g_HubObjArray[port]->HubDestroy();
						delete g_HubObjArray[port];
						g_HubObjArray[port] = nullptr;
						delete g_XidControllerObjArray[port];
						g_XidControllerObjArray[port] = nullptr;
					}
				}
				else {
					ret = -1;
					g_HubObjArray[port]->HubDestroy();
					delete g_HubObjArray[port];
					g_HubObjArray[port] = nullptr;
					EmuLog(LOG_LEVEL::WARNING, "Xid controller already present at port %d.2", port + 1);
				}
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "Hub already present at port %d", port + 1);
			}
			break;
		}

		case MS_CONTROLLER_S:
		case LIGHT_GUN:
		case STEERING_WHEEL:
		case MEMORY_UNIT:
		case IR_DONGLE:
		case STEEL_BATTALION_CONTROLLER: {
			EmuLog(LOG_LEVEL::INFO, "This device type is not yet supported");
			break;
		}

		default:
			EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type");
	}

	if (!ret) {
		(*it)->m_Type = type;
		(*it)->m_Attached = 1;
	}

	return ret;
}

void InputDeviceManager::DisconnectDeviceFromXbox(int port)
{
	std::vector<SdlDevice*>::iterator it;

	if (port < 1) { return; }

	for (it = m_SdlDevices.begin(); it != m_SdlDevices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			--port;
			break;
		}
	}

	if (it == m_SdlDevices.end()) {
		// Not necessarily a bug. This could also be triggered by detaching an unsupported joystick
		return;
	}

	if (port + 1 > 4) {
		delete (*it);
		m_SdlDevices.erase(it);
		return;
	}

	switch ((*it)->m_Type)
	{
		case MS_CONTROLLER_DUKE: {
			if (g_HubObjArray[port] != nullptr && g_XidControllerObjArray[port] != nullptr) {
				g_HubObjArray[port]->HubDestroy();
				delete g_HubObjArray[port];
				g_HubObjArray[port] = nullptr;
				delete g_XidControllerObjArray[port];
				g_XidControllerObjArray[port] = nullptr;
				delete (*it);
				m_SdlDevices.erase(it);
				// Here, we could also see if there are detached devices that have a matching type and bound buttons, so that it can immediately
				// be used instead of remaining inactive (example: 5 controllers and 1st is detached -> 5th can be used if it has bindings)
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "Attempted to disconnect a device not attached to the Xbox.\n");
			}
			break;
		}

		case MS_CONTROLLER_S:
		case LIGHT_GUN:
		case STEERING_WHEEL:
		case MEMORY_UNIT:
		case IR_DONGLE:
		case STEEL_BATTALION_CONTROLLER: {
			printf("This device type is not yet supported\n");
			break;
		}

		default:
			EmuLog(LOG_LEVEL::WARNING, "Attempted to detach an unknown device type\n");
	}
}

bool InputDeviceManager::UpdateXboxPortInput(int Port, void* Buffer, int Direction)
{
	InputDevice* pDev;

	assert(Direction == DIRECTION_IN || Direction == DIRECTION_OUT);
	if (Port > XBOX_PORT_4 || Port < XBOX_PORT_1) { return false; };

	pDev = nullptr;
	for (auto it : m_Devices) {
		if (it->GetXPort == Port) {
			pDev = it;
			break;
		}
	}
	if (pDev == nullptr) {
		return false;
	}

	switch (pDev->GetXType())
	{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			UpdateInputXpad(pDev, Buffer, Direction);
		}
		break;

		default: {
			EmuLog(LOG_LEVEL::WARNING, "Port %d has an unsupported xbox device attached! The type was %d", Port, pDev->GetXType());
			return false;
		}
	}

	return true;
}

void InputDeviceManager::UpdateInputXpad(InputDevice* Device, void* Buffer, int Direction)
{
	int i;
	std::map<int, InputDevice::Input*> bindings;

	if (Direction == DIRECTION_IN) {
		XpadInput* in_buf = reinterpret_cast<XpadInput*>(static_cast<uint8_t*>(Buffer) + 2);
		bindings = Device->GetBindings();
		Device->UpdateInput();
		for (i = GAMEPAD_A; i < GAMEPAD_DPAD_UP; i++) {
			const auto it = std::find_if(bindings.begin(), bindings.end(), [&i](const auto& d) {
				return i == d.first;
			});
			if (it != bindings.end()) {
				if (i == GAMEPAD_LEFT_TRIGGER || i == GAMEPAD_RIGHT_TRIGGER) {
					in_buf->bAnalogButtons[i] = it->second->GetState(); //>> 7; likely to fix
				}
				else {
					// At the moment, we don't support intermediate values for the analog buttons, so report them as full pressed or released
					in_buf->bAnalogButtons[i] = it->second->GetState() ? 0xFF : 0; // likely to fix
				}
			}
		}
		for (i = GAMEPAD_DPAD_UP; i < GAMEPAD_LEFT_THUMB_X; i++) {
			const auto it = std::find_if(bindings.begin(), bindings.end(), [&i](const auto& d) {
				return i == d.first;
			});
			if (it != bindings.end()) {
				if (it->second->GetState()) { // likely to fix
					in_buf->wButtons |= BUTTON_MASK(i);
				}
				else {
					in_buf->wButtons &= ~(BUTTON_MASK(i));
				}
			}
		}
		for (i = GAMEPAD_LEFT_THUMB_X; i < GAMEPAD_BUTTON_MAX; i++) {
			const auto it = std::find_if(bindings.begin(), bindings.end(), [&i](const auto& d) {
				return i == d.first;
			});
			if (it != bindings.end()) {
				switch (i)
				{
					case GAMEPAD_LEFT_THUMB_X: {
						in_buf->sThumbLX = it->second->GetState(); // likely to fix
					}
					break;

					case GAMEPAD_LEFT_THUMB_Y: {
						in_buf->sThumbLY = -it->second->GetState() - 1;
					}
					break;

					case GAMEPAD_RIGHT_THUMB_X: {
						in_buf->sThumbRX = it->second->GetState();
					}
					break;

					case GAMEPAD_RIGHT_THUMB_Y: {
						in_buf->sThumbRY = -it->second->GetState() - 1;
					}
					break;

					default: {
						// unreachable
					}
				}
			}
		}
	}
	else {
		// TODO
	}

}
#endif
