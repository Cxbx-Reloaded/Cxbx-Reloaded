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
// *   src->Common->Input->SDL2_Device.cpp
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

#include "SDL2_Device.h"


int SDL2Devices::GetBoundButton(int sdl_key)
{
	int i;

	for (i = 0; i < 8; i++) {
		if (m_ButtonMap_Analog[i][1] == sdl_key) {
			return m_ButtonMap_Analog[i][0];
		}
	}

	for (i = 0; i < 8; i++) {
		if (m_ButtonMap_Binary[i][1] == sdl_key) {
			return m_ButtonMap_Binary[i][0];
		}
	}

	if (m_HatIndex == sdl_key) {
		return m_HatIndex;
	}

	for (i = 0; i < 4; i++) {
		if (m_AxisMap[i][1] == sdl_key) {
			return m_AxisMap[i][0];
		}
	}

	return GAMEPAD_INVALID;
}

void SDL2Devices::UpdateAnalogButtonState(uint8_t xbox_button, uint8_t state)
{
	for (int i = 0; i < 6; i++) {
		if (m_ButtonMap_Analog[i][0] == xbox_button) {
			// At the moment, we don't support intermediate values for the analog buttons, so report them as full pressed or released
			m_State.bAnalogButtons[m_ButtonMap_Analog[i][0]] = state ? 0xFF : 0;
			bStateDirty = true;
			return;
		}
	}

	if (xbox_button == GAMEPAD_LEFT_TRIGGER) {
		m_State.bAnalogButtons[m_ButtonMap_Analog[6][0]] = SDL_GameControllerGetAxis(m_Gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 8;
		bStateDirty = true;
		return;
	}

	if (xbox_button == GAMEPAD_RIGHT_TRIGGER) {
		m_State.bAnalogButtons[m_ButtonMap_Analog[7][0]] = SDL_GameControllerGetAxis(m_Gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 8;
		bStateDirty = true;
		return;
	}

	assert(0);
}

void SDL2Devices::UpdateDigitalButtonState(uint8_t xbox_button, uint8_t state)
{
	for (int i = 0; i < 8; i++) {
		if (m_ButtonMap_Binary[i][0] == xbox_button) {
			if (state) {
				m_State.wButtons |= BUTTON_MASK(m_ButtonMap_Binary[i][0]);
				bStateDirty = true;
				return;
			}
			else {
				m_State.wButtons &= ~(BUTTON_MASK(m_ButtonMap_Binary[i][0]));
				bStateDirty = true;
				return;
			}
		}
	}

	assert(0);
}

void SDL2Devices::UpdateHatState(uint8_t state)
{
	m_State.wButtons &= ~0xF;

	switch (state)
	{
		case SDL_HAT_LEFTUP: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[4][0]) | BUTTON_MASK(m_ButtonMap_Binary[6][0]));
			break;
		}

		case SDL_HAT_UP: {
			m_State.wButtons |= BUTTON_MASK(m_ButtonMap_Binary[4][0]);
			break;
		}

		case SDL_HAT_RIGHTUP: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[4][0]) | BUTTON_MASK(m_ButtonMap_Binary[7][0]));
			break;
		}

		case SDL_HAT_RIGHT: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[7][0]));
			break;
		}

		case SDL_HAT_RIGHTDOWN: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[7][0]) | BUTTON_MASK(m_ButtonMap_Binary[5][0]));
			break;
		}

		case SDL_HAT_DOWN: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[5][0]));
			break;
		}

		case SDL_HAT_LEFTDOWN: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[5][0]) | BUTTON_MASK(m_ButtonMap_Binary[6][0]));
			break;
		}

		case SDL_HAT_LEFT: {
			m_State.wButtons |= (BUTTON_MASK(m_ButtonMap_Binary[6][0]));
			break;
		}

		case SDL_HAT_CENTERED: {
			break;
		}

		default:
			assert(0);
	}

	bStateDirty = true;
}

void SDL2Devices::UpdateAxisState(uint8_t xbox_button, int16_t state)
{
	switch (xbox_button)
	{
		case GAMEPAD_LEFT_THUMB_X: {
			m_State.sThumbLX = state;
			break;
		}

		case GAMEPAD_LEFT_THUMB_Y: {
			m_State.sThumbLY = -state - 1; // not sure of this yet
			break;
		}

		case GAMEPAD_RIGHT_THUMB_X: {
			m_State.sThumbRX = state;
			break;
		}

		case GAMEPAD_RIGHT_THUMB_Y: {
			m_State.sThumbRY = -state - 1; // not sure of this yet
			break;
		}

		default:
			assert(0);
	}
}
