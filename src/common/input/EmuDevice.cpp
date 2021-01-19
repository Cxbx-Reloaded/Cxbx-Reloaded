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
#include "layout_xbox_device.h"
#include "gui/resource/ResCxbx.h"


EmuDevice::EmuDevice(int type, HWND hwnd)
{
	m_hwnd = hwnd;

	switch (type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
		for (size_t i = 0; i < ARRAY_SIZE(button_xbox_ctrl_id); i++) {
			m_buttons.push_back(new Button(button_xbox_ctrl_id[i], i, hwnd));

			// Install the subclass for the button control
			SetWindowSubclass(GetDlgItem(hwnd, button_xbox_ctrl_id[i]), ButtonSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		for (size_t i = 0; i < ARRAY_SIZE(button_sbc_id); i++) {
			m_buttons.push_back(new Button(button_sbc_id[i], i, hwnd));

			// Not sure yet if this is necessary
			// Install the subclass for the button control
			//SetWindowSubclass(GetDlgItem(hwnd, button_sbc_id[i]), ButtonSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
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

void EmuDevice::BindDefault(int api)
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [&api](const auto button) {
		button->UpdateText(button->GetName(api, button->GetIndex()).c_str());
		});
}

void EmuDevice::ClearButtons()
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [](const auto button) {
		button->ClearText();
		});
}
