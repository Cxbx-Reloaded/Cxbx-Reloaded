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
#if 0 // Reenable this when LLE USB actually works
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::SDL2

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For PKINTERRUPT, etc.
};

#include "InputConfig.h"
#include "SDL2_Device.h"
#include "..\devices\usb\XidGamepad.h"
#include "core\kernel\exports\EmuKrnl.h" // For EmuLog
#include <thread>


InputDeviceManager* g_InputDeviceManager = nullptr;

InputDeviceManager::InputDeviceManager()
{
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
		CxbxKrnlCleanup("Failed to initialize SDL2 input subsystem. The error was: %s\n", SDL_GetError());
		return;
	}
}

InputDeviceManager::~InputDeviceManager()
{
	SDL_Quit();
}

int InputDeviceManager::EnumSdl2Devices()
{
	int NumOfJoysticks;
	int NumInvalidJoysticks;
	SDL2Devices* pDev;
	SDL_Joystick* pJoystick;
	std::vector<SDL2Devices*>::iterator it;

	NumOfJoysticks = SDL_NumJoysticks();
	if (NumOfJoysticks < 0) {
		EmuLog(LOG_LEVEL::WARNING, "Failed to enumerate joysticks. The error was: %s", SDL_GetError());
		return 0;
	}

	NumInvalidJoysticks = 0;

	for (int i = 0; i < NumOfJoysticks; i++) {
		pDev = new SDL2Devices();
		pDev->m_Index = i;
		m_Sdl2Devices.push_back(pDev);
	}

	for (it = m_Sdl2Devices.begin(); it != m_Sdl2Devices.end();) {
		pJoystick = SDL_JoystickOpen((*it)->m_Index);
		if (pJoystick == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to open joystick %s. The error was %s\n", SDL_GameControllerNameForIndex((*it)->m_Index), SDL_GetError());
			delete (*it);
			it = m_Sdl2Devices.erase(it);
			NumInvalidJoysticks++;
		}
		else {
			printf("Found joystick %s\n", SDL_JoystickName(pJoystick));
			(*it)->m_Joystick = pJoystick;
			(*it)->m_jyID = SDL_JoystickInstanceID(pJoystick);
			(*it)->m_Attached = 1;
			++it;
		}
	}

	return NumOfJoysticks - NumInvalidJoysticks;
}

int InputDeviceManager::ConnectDeviceToXbox(int port, int type)
{
	int ret = -1;
	std::vector<SDL2Devices*>::iterator it;

	if (port > 4 || port < 1) { return ret; };

	for (it = m_Sdl2Devices.begin(); it != m_Sdl2Devices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			--port;
			break;
		}
	}

	if (it == m_Sdl2Devices.end()) {
		EmuLog(LOG_LEVEL::WARNING, "Attempted to connect a device not yet enumerated.\n");
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
					EmuLog(LOG_LEVEL::WARNING, "Xid controller already present at port %d.2\n", port + 1);
				}
			}
			else {
				EmuLog(LOG_LEVEL::WARNING, "Hub already present at port %d\n", port + 1);
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
			EmuLog(LOG_LEVEL::WARNING, "Attempted to attach an unknown device type\n");
	}

	if (!ret) {
		(*it)->m_Type = type;
		(*it)->m_Attached = 1;
	}

	return ret;
}

void InputDeviceManager::DisconnectDeviceFromXbox(int port)
{
	std::vector<SDL2Devices*>::iterator it;

	if (port < 1) { return; }

	for (it = m_Sdl2Devices.begin(); it != m_Sdl2Devices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			--port;
			break;
		}
	}

	if (it == m_Sdl2Devices.end()) {
		// Not necessarily a bug. This could also be triggered by detaching an unsupported joystick
		return;
	}

	if (port + 1 > 4) {
		delete (*it);
		m_Sdl2Devices.erase(it);
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
				m_Sdl2Devices.erase(it);
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

void InputDeviceManager::StartInputThread()
{
	std::thread(InputThread, this).detach();
}

void InputDeviceManager::InputThread(InputDeviceManager* pVoid)
{
	SetThreadAffinityMask(GetCurrentThread(), g_CPUOthers);
	bool bContinue = true;
	SDL_Event event;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		CxbxKrnlCleanup("Failed to initialize SDL2 video subsystem. The error was: %s\n", SDL_GetError());
		return;
	}

	SDL_JoystickEventState(SDL_ENABLE);

	while (bContinue)
	{
		if (SDL_WaitEvent(&event))
		{
			switch (event.type)
			{
				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN: {
					pVoid->UpdateButtonState(event.jbutton.which, event.jbutton.button, event.jbutton.state);
					break;
				}

				case SDL_JOYHATMOTION: {
					pVoid->UpdateHatState(event.jhat.which, event.jhat.hat, event.jhat.value);
					break;
				}

				case SDL_JOYAXISMOTION: {
					pVoid->UpdateAxisState(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
					break;
				}

				case SDL_JOYDEVICEADDED: {
					bool found = false;
					for (auto dev : pVoid->m_Sdl2Devices) {
						if (dev->m_Index == event.jdevice.which) {
							// already enumerated, skipping
							found = true;
							break;
						}
					}
					if (!found) {
						// for now we only support a single controller at port 1, more will be added later
						if (!pVoid->IsValidController(event.jdevice.which) && event.jdevice.which == 0) {
							pVoid->ConnectDeviceToXbox(1, MS_CONTROLLER_DUKE);
						}
					}
					break;
				}

				case SDL_JOYDEVICEREMOVED: {
					pVoid->DisconnectDeviceFromXbox(event.jdevice.which + 1);
					break;
				}

				case SDL_QUIT: {
					bContinue = false;
					break;
				}

				default:
					break;
			}
		}
	}
}

void InputDeviceManager::UpdateButtonState(SDL_JoystickID id, uint8_t button, uint8_t state)
{
	SDL2Devices* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_Sdl2Devices) {
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
	SDL2Devices* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_Sdl2Devices) {
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
	SDL2Devices* ControllerObj = nullptr;
	int xbox_button;

	for (auto Obj : m_Sdl2Devices) {
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
	SDL2Devices* pDev;
	SDL_Joystick* pJoystick;

	if (SDL_IsGameController(index)) {
		pDev = new SDL2Devices();
		pDev->m_Index = index;
		m_Sdl2Devices.push_back(pDev);
	}
	else {
		// this joystick is not supported at the moment
		return -1;
	}

	pJoystick = SDL_JoystickOpen(pDev->m_Index);
	if (pJoystick == nullptr) {
		EmuLog(LOG_LEVEL::WARNING, "Failed to open game controller %s. The error was %s\n", SDL_GameControllerNameForIndex(pDev->m_Index), SDL_GetError());
		delete pDev;
		m_Sdl2Devices.erase(m_Sdl2Devices.begin() + index);
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

SDL2Devices* InputDeviceManager::FindDeviceFromXboxPort(int port)
{
	if (port > 4 || port < 1) { return nullptr; };

	for (auto it = m_Sdl2Devices.begin(); it != m_Sdl2Devices.end(); ++it) {
		if ((*it)->m_Index == (port - 1)) {
			return *it;
		}
	}
	return nullptr;
}
#endif
