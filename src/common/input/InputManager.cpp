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
	SDL_JoystickEventState(SDL_IGNORE);
	m_bInitOK = true;
	m_bExitOK = false;

	SetThreadAffinityMask(GetCurrentThread(), g_CPUOthers);
	
	while (true)
	{
		if (SDL_PollEvent(&Event))
		{
			if (Event.type == SDL_JOYDEVICEADDED) {
				OpenSdlDevice(Event.jdevice.which);
			}
			else if (Event.type == SDL_JOYDEVICEREMOVED) {
				DisconnectDeviceFromXbox(Event.jdevice.which + 1);
			}
			else if (Event.type == ExitEvent_t) {
				break;
			}
		}
		Sleep(1);
	}

	SDL_Quit();
	m_bExitOK = true;
}

void InputDeviceManager::AddDevice(InputDevice* Device)
{
	// If we are shutdown (or in process of shutting down) ignore this request:
	if (!m_bInitOK) {
		return;
	}

	//std::lock_guard<std::mutex> lk(m_devices_mutex);
	// Try to find an ID for this device
	int ID = 0;
	while (true)
	{
		const auto it =
			std::find_if(m_Devices.begin(), m_Devices.end(), [&Device, &ID](const auto& d) {
			return d->GetAPI() == Device->GetAPI() && d->GetDeviceName() == Device->GetDeviceName() &&
				d->GetId() == id;
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

	//if (!m_is_populating_devices)
	//	InvokeDevicesChangedCallbacks();
}

void InputDeviceManager::UpdateInput()
{
	// Don't block the UI or CPU thread (to avoid a short but noticeable frame drop)
	if (m_devices_mutex.try_lock()) // TODO
	{
		//std::lock_guard<std::mutex> lk(m_devices_mutex, std::adopt_lock);
		for (const auto& d : m_Devices) {
			d->UpdateInput();
		}
	}
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

void InputDeviceManager::UpdateButtonState(SDL_JoystickID id, uint8_t button, uint8_t state)
{
	SdlDevice* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_SdlDevices) {
		if (Obj->m_jyID == id) {
			ControllerObj = Obj;
			break;
		}
	}

	if (ControllerObj == nullptr) {
		return;
	}

	xbox_button = ControllerObj->GetBoundButton(button);

	if (xbox_button == GAMEPAD_INVALID) {
		return;
	}

	switch (xbox_button)
	{
		case GAMEPAD_A:
		case GAMEPAD_B:
		case GAMEPAD_X:
		case GAMEPAD_Y:
		case GAMEPAD_BLACK:
		case GAMEPAD_WHITE:
		case GAMEPAD_LEFT_TRIGGER:
		case GAMEPAD_RIGHT_TRIGGER: {
			ControllerObj->UpdateAnalogButtonState(xbox_button, state);
			break;
		}

		case GAMEPAD_BACK:
		case GAMEPAD_START:
		case GAMEPAD_LEFT_THUMB:
		case GAMEPAD_RIGHT_THUMB:
		case GAMEPAD_DPAD_UP:
		case GAMEPAD_DPAD_DOWN:
		case GAMEPAD_DPAD_LEFT:
		case GAMEPAD_DPAD_RIGHT: {
			ControllerObj->UpdateDigitalButtonState(xbox_button, state);
			break;
		}

		default:
			break;
	}
}

void InputDeviceManager::UpdateHatState(SDL_JoystickID id, uint8_t hat_index, uint8_t state)
{
	SdlDevice* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_SdlDevices) {
		if (Obj->m_jyID == id) {
			ControllerObj = Obj;
			break;
		}
	}

	if (ControllerObj == nullptr) {
		return;
	}

	xbox_button = ControllerObj->GetBoundButton(hat_index + HAT_CONSTANT);

	if (xbox_button == GAMEPAD_INVALID) {
		return;
	}

	ControllerObj->UpdateHatState(state);
}

void InputDeviceManager::UpdateAxisState(SDL_JoystickID id, uint8_t axis_index, int16_t state)
{
	SdlDevice* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_SdlDevices) {
		if (Obj->m_jyID == id) {
			ControllerObj = Obj;
			break;
		}
	}

	if (ControllerObj == nullptr) {
		return;
	}

	xbox_button = ControllerObj->GetBoundButton(axis_index);

	if (xbox_button == GAMEPAD_INVALID) {
		return;
	}

	ControllerObj->UpdateAxisState(xbox_button, state);
}

int InputDeviceManager::IsValidController(int index)
{
	SdlDevice* pDev;
	SDL_Joystick* pJoystick;

	if (SDL_IsGameController(index)) {
		pDev = new SdlDevice();
		pDev->m_Index = index;
		m_SdlDevices.push_back(pDev);
	}
	else {
		// this joystick is not supported at the moment
		return -1;
	}

	pJoystick = SDL_JoystickOpen(pDev->m_Index);
	if (pJoystick == nullptr) {
		EmuLog(LOG_LEVEL::WARNING, "Failed to open game controller %s. The error was %s\n", SDL_GameControllerNameForIndex(pDev->m_Index), SDL_GetError());
		delete pDev;
		m_SdlDevices.erase(m_SdlDevices.begin() + index);
		return -1;
	}
	else if (pDev->m_Index > 3) {
		printf("More than 4 controllers detected. Putting game controller %s in detached state\n", SDL_JoystickName(pJoystick));
		pDev->m_Attached = 0;
		return -1;
	}
	else {
		printf("Found game controller %s\n", SDL_JoystickName(pJoystick));
		pDev->m_Joystick = pJoystick;
		pDev->m_jyID = SDL_JoystickInstanceID(pJoystick);
		return 0;
	}
}

SdlDevice* InputDeviceManager::FindDeviceFromXboxPort(int port)
{
	if (port > 4 || port < 1) { return nullptr; };

	for (auto it = m_SdlDevices.begin(); it != m_SdlDevices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			return *it;
		}
	}
	return nullptr;
}
#endif
