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

#include "Button.h"
#include "InputManager.h"
#include "layout_xbox_device.h"
#include "gui/resource/ResCxbx.h"


static const char *tooltip_text_toggle = "Left-click: start input detection\nRight-click: clear binding\nShift + right-click: toggle mouse input mode";
static const char *tooltip_text_no_toggle = "Left-click: start input detection\nRight-click: clear binding";

EmuDevice::EmuDevice(int type, HWND hwnd, void *wnd)
{
	m_hwnd = hwnd;
	CreateTooltipWindow();

	switch (type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
	case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK): {
		for (size_t i = 0; i < ARRAY_SIZE(button_xbox_ctrl_id); i++) {
			m_buttons.push_back(new Button(button_xbox_ctrl_id[i], i, hwnd, wnd));
			m_buttons.back()->AddTooltip(m_hwnd, m_tooltip_hwnd, tooltip_text_toggle);

			// Install the subclass for the button control
			SetWindowSubclass(GetDlgItem(hwnd, button_xbox_ctrl_id[i]), ButtonDukeSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		for (size_t i = 0; i < ARRAY_SIZE(button_sbc_id); i++) {
			m_buttons.push_back(new Button(button_sbc_id[i], i, hwnd, wnd));
			m_buttons.back()->AddTooltip(m_hwnd, m_tooltip_hwnd, tooltip_text_no_toggle);

			// Install the subclass for the button control
			SetWindowSubclass(GetDlgItem(hwnd, button_sbc_id[i]), ButtonSbcSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHTGUN): {
		for (size_t i = 0; i < ARRAY_SIZE(button_lightgun_id); i++) {
			m_buttons.push_back(new Button(button_lightgun_id[i], i, hwnd, wnd));
			m_buttons.back()->AddTooltip(m_hwnd, m_tooltip_hwnd, tooltip_text_no_toggle);

			// Install the subclass for the button control
			SetWindowSubclass(GetDlgItem(hwnd, button_lightgun_id[i]), ButtonLightgunSubclassProc, 0, reinterpret_cast<DWORD_PTR>(m_buttons[i]));
		}
	}
	break;

	}
}

EmuDevice::~EmuDevice()
{
	DestroyWindow(m_tooltip_hwnd);
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

template<size_t size>
void EmuDevice::BindDefault(const std::array<const char *, size> &arr)
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [&arr](const auto button) {
		button->UpdateText(arr[button->GetIndex()]);
		});
}

void EmuDevice::ClearButtons()
{
	std::for_each(m_buttons.begin(), m_buttons.end(), [](const auto button) {
		button->ClearText();
		});
}

void EmuDevice::CreateTooltipWindow()
{
	m_tooltip_hwnd = CreateWindow(TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_hwnd, NULL,
		GetModuleHandle(NULL), NULL);

	SendMessage(m_tooltip_hwnd, TTM_ACTIVATE, TRUE, 0);
	SendMessage(m_tooltip_hwnd, TTM_SETMAXTIPWIDTH, 0, 500);
	SendMessage(m_tooltip_hwnd, TTM_SETDELAYTIME, TTDT_AUTOPOP, 15000);
}

template void EmuDevice::BindDefault(const std::array<const char*, XBOX_CTRL_NUM_BUTTONS>& arr);
template void EmuDevice::BindDefault(const std::array<const char *, LIGHTGUN_NUM_BUTTONS> &arr);
