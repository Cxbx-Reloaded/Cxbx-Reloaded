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

#include"Button.h"
#include "InputManager.h"
#include "..\..\gui\ResCxbx.h"

static int button_xbox_ctrl_id[XBOX_CTRL_NUM_BUTTONS] = {
	IDC_SET_DPAD_UP,
	IDC_SET_DPAD_DOWN,
	IDC_SET_DPAD_LEFT,
	IDC_SET_DPAD_RIGHT,
	IDC_SET_START,
	IDC_SET_BACK,
	IDC_SET_LTHUMB,
	IDC_SET_RTHUMB,
	IDC_SET_A,
	IDC_SET_B,
	IDC_SET_X,
	IDC_SET_Y,
	IDC_SET_BLACK,
	IDC_SET_WHITE,
	IDC_SET_LTRIGGER,
	IDC_SET_RTRIGGER,
	IDC_SET_LEFT_POSX,
	IDC_SET_LEFT_NEGX,
	IDC_SET_LEFT_POSY,
	IDC_SET_LEFT_NEGY,
	IDC_SET_RIGHT_POSX,
	IDC_SET_RIGHT_NEGX,
	IDC_SET_RIGHT_POSY,
	IDC_SET_RIGHT_NEGY,
	IDC_SET_MOTOR,
};

const char* button_xbox_ctrl_names[XBOX_CTRL_NUM_BUTTONS][2] = {
	"D Pad Up",      "Pad N",
	"D Pad Down",    "Pad S",
	"D Pad Left",    "Pad W",
	"D Pad Right",   "Pad E",
	"Start",         "Start",
	"Back",          "Back",
	"L Thumb",       "Thumb L",
	"R Thumb",       "Thumb R",
	"A",             "Button A",
	"B",             "Button B",
	"X",             "Button X",
	"Y",             "Button Y",
	"Black",         "Shoulder R",
	"White",         "Shoulder L",
	"L Trigger",     "Trigger L",
	"R Trigger",     "Trigger R",
	"Left Axis X+",  "Left X+",
	"Left Axis X-",  "Left X-",
	"Left Axis Y+",  "Left Y+",
	"Left Axis Y-",  "Left Y-",
	"Right Axis X+", "Right X+",
	"Right Axis X-", "Right X-",
	"Right Axis Y+", "Right Y+",
	"Right Axis Y-", "Right Y-",
	"Motor",         "LeftRight",
};


EmuDevice::EmuDevice(int type, HWND hwnd)
{
	switch (type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		m_hwnd = hwnd;
		for (int i = 0; i < ARRAY_SIZE(button_xbox_ctrl_id); i++) {
			m_buttons.push_back(new Button(button_xbox_ctrl_names[i][0], button_xbox_ctrl_names[i][1],
				button_xbox_ctrl_id[i], i, hwnd));

			// Install the subclass for the button control
			SetWindowSubclass(GetDlgItem(hwnd, button_xbox_ctrl_id[i]), ButtonSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
	break;

	default:
		break;
	}
}

EmuDevice::~EmuDevice()
{
	for (auto button : m_buttons) {
		delete button;
	}
}

Button* EmuDevice::FindButtonById(int id)
{
	auto it = std::find_if(m_buttons.begin(), m_buttons.end(), [&id](const auto button) {
		if (button->GetId() == id) {
			return true;
		}
		return false;
		});
	assert(it != m_buttons.end());
	return *it;
}

Button* EmuDevice::FindButtonByIndex(int index)
{
	return m_buttons[index];
}

void EmuDevice::BindXInput()
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [](const auto button) {
		button->UpdateText();
		});
}

void EmuDevice::ClearButtons()
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [](const auto button) {
		button->ClearText();
		});
}
