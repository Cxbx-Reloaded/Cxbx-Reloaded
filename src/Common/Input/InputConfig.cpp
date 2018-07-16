// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->Common->Input->InputConfig.cpp
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

#include "InputConfig.h"


InputDeviceManager::InputDeviceManager()
{
	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
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
	SDL2Devices* pDev;
	SDL_GameController* pController;
	std::vector<SDL2Devices*>::iterator it;

	NumOfJoysticks = SDL_NumJoysticks();
	if (NumOfJoysticks < 0) {
		EmuWarning("Failed to enumerate joysticks. The error was: %s", SDL_GetError());
		return -1;
	}
	SDL_GameControllerButtonBind;
	for (int i = 0; i < NumOfJoysticks; i++) {
		if (SDL_IsGameController(i)) {
			pDev = new SDL2Devices();
			pDev->m_Index = i;
			m_Sdl2Devices.push_back(pDev);
		}
		// this joystick not supported at the moment
	}

	for (it = m_Sdl2Devices.begin(); it != m_Sdl2Devices.end();) {
		pController = SDL_GameControllerOpen((*it)->m_Index);
		if (pController == nullptr) {
			EmuWarning("Failed to open game controller %s. The error was %s\n", SDL_GameControllerNameForIndex((*it)->m_Index), SDL_GetError());
			delete (*it);
			it = m_Sdl2Devices.erase(it);
		}
		else {
			printf("Found game controller %s\n", SDL_GameControllerName(pController));
			(*it)->m_Gamepad = pController;
			(*it)->m_jyID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pController));
			++it;
		}
	}
}

void InputDeviceManager::StartInputThread()
{
	std::thread(InputThread, this).detach();
}

void InputDeviceManager::InputThread(InputDeviceManager* pVoid)
{
	bool bContinue = true;
	SDL_Event event;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		CxbxKrnlCleanup("Failed to initialize SDL2 video subsystem. The error was: %s\n", SDL_GetError());
		return;
	}

	SDL_JoystickEventState(SDL_ENABLE);
	SDL_GameControllerEventState(SDL_ENABLE);

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

				case SDL_CONTROLLERBUTTONUP:
				case SDL_CONTROLLERBUTTONDOWN: {
					pVoid->UpdateButtonState(event.cbutton.which, event.cbutton.button, event.cbutton.state);
					break;
				}

				case SDL_CONTROLLERAXISMOTION: {
					pVoid->UpdateAxisState(event.caxis.which, event.caxis.axis, event.caxis.value);
					break;
				}

				case SDL_QUIT: {
					bContinue = false;
					break;
				}

				default:
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
