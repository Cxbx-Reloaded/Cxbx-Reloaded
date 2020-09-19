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

// Derived from DInputKeyboardMouse.cpp of Dolphin emulator
// https://github.com/dolphin-emu/dolphin

#define LOG_PREFIX CXBXR_MODULE::DINP

#include "DInputKeyboardMouse.h"
#include "InputManager.h"
#include "core\kernel\support\Emu.h"

// Unfortunately, sdl doesn't seem to be able to capture keyboard/mouse input from windows it didn't create (we currently use
// win32 for that). So unless we create sdl windows, we will have to keep dinput around to handle keyboard/mouse input.

namespace DInput
{
	static const struct kb_keys {
		const uint8_t code;
		const char* const name;
	}
	named_keys[] = {
#include "DInputKeyboardCodes.h"
	};

	void InitKeyboardMouse(IDirectInput8* idi8)
	{
		// From Dolphin: "mouse and keyboard are a combined device, to allow shift+click and stuff
		// if that's dumb, I will make a VirtualDevice class that just uses ranges of inputs/outputs from
		// other devices so there can be a separated Keyboard and mouse, as well as combined KeyboardMouse"

		LPDIRECTINPUTDEVICE8 kb_device = nullptr;
		LPDIRECTINPUTDEVICE8 mo_device = nullptr;

		if (SUCCEEDED(idi8->CreateDevice(GUID_SysKeyboard, &kb_device, nullptr)) &&
			SUCCEEDED(kb_device->SetDataFormat(&c_dfDIKeyboard)) &&
			SUCCEEDED(kb_device->SetCooperativeLevel(nullptr, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)) &&
			SUCCEEDED(idi8->CreateDevice(GUID_SysMouse, &mo_device, nullptr)) &&
			SUCCEEDED(mo_device->SetDataFormat(&c_dfDIMouse2)) &&
			SUCCEEDED(mo_device->SetCooperativeLevel(nullptr, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			g_InputDeviceManager.AddDevice(std::make_shared<KeyboardMouse>(kb_device, mo_device));
			bKbMoEnumerated = true;
			return;
		}

		if (kb_device) {
			kb_device->Release();
		}

		if (mo_device) {
			mo_device->Release();
		}
	}

	void PopulateDevices()
	{
		bKbMoEnumerated = false;
		IDirectInput8* idi8;
		if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8,
			(LPVOID*)& idi8, nullptr)))
		{
			return;
		}

		InitKeyboardMouse(idi8);
		idi8->Release();
	}

	void GetDeviceChanges()
	{
		if (bKbMoEnumerated) {
			return;
		}
		PopulateDevices();
	}

	KeyboardMouse::KeyboardMouse(const LPDIRECTINPUTDEVICE8 kb_device, const LPDIRECTINPUTDEVICE8 mo_device)
		: m_kb_device(kb_device), m_mo_device(mo_device)
	{
		m_kb_device->Acquire();
		m_mo_device->Acquire();

		std::memset(&m_state_in, 0, sizeof(m_state_in));

		// KEYBOARD
		// add keys
		for (uint8_t i = 0; i < sizeof(named_keys) / sizeof(*named_keys); ++i) {
			AddInput(new Key(i, m_state_in.keyboard[named_keys[i].code]));
		}

		// MOUSE
		// get caps
		DIDEVCAPS mouse_caps;
		std::memset(&mouse_caps, 0, sizeof(mouse_caps));
		mouse_caps.dwSize = sizeof(mouse_caps);
		m_mo_device->GetCapabilities(&mouse_caps);

		// mouse buttons
		for (unsigned char i = 0; i < mouse_caps.dwButtons; ++i) {
			AddInput(new Button(i, m_state_in.mouse.rgbButtons[i]));
		}

		// mouse axes
		for (unsigned int i = 0; i < mouse_caps.dwAxes; ++i) {
			const LONG &ax = (&m_state_in.mouse.lX)[i];

			// each axis gets a negative and a positive input instance associated with it
			AddInput(new Axis(i, ax, (2 == i) ? mo_wheel_range_neg : mo_axis_range_neg));
			AddInput(new Axis(i, ax, (2 == i) ? mo_wheel_range_pos : mo_axis_range_pos));
		}
	}

	KeyboardMouse::~KeyboardMouse()
	{
		// kb
		m_kb_device->Unacquire();
		m_kb_device->Release();

		// mouse
		m_mo_device->Unacquire();
		m_mo_device->Release();
	}

	bool KeyboardMouse::UpdateInput()
	{
		DIMOUSESTATE2 tmp_mouse;

		HRESULT kb_hr = m_kb_device->GetDeviceState(sizeof(m_state_in.keyboard), &m_state_in.keyboard);
		HRESULT mo_hr = m_mo_device->GetDeviceState(sizeof(tmp_mouse), &tmp_mouse);

		if (DIERR_INPUTLOST == kb_hr || DIERR_NOTACQUIRED == kb_hr) {
			m_kb_device->Acquire();
		}

		if (DIERR_INPUTLOST == mo_hr || DIERR_NOTACQUIRED == mo_hr) {
			m_mo_device->Acquire();
		}

		if (SUCCEEDED(kb_hr) && SUCCEEDED(mo_hr))
		{
			// NOTE: the /= 2 will cause the mouse input to naturally decay to zero if the mouse did not change its position
			for (unsigned int i = 0; i < 3; ++i) {
				((&m_state_in.mouse.lX)[i] += (&tmp_mouse.lX)[i]) /= 2;
			}

			std::memcpy(m_state_in.mouse.rgbButtons, tmp_mouse.rgbButtons, sizeof(m_state_in.mouse.rgbButtons));

			return true;
		}

		return false;
	}

	std::string KeyboardMouse::GetDeviceName() const
	{
		return "KeyboardMouse";
	}

	std::string KeyboardMouse::GetAPI() const
	{
		return "DInput";
	}

	std::string KeyboardMouse::Key::GetName() const
	{
		return named_keys[m_index].name;
	}

	std::string KeyboardMouse::Button::GetName() const
	{
		return std::string("Click ") + char('0' + m_index);
	}

	std::string KeyboardMouse::Axis::GetName() const
	{
		static char tmpstr[] = "Axis ..";
		tmpstr[5] = (char)('X' + m_index);
		tmpstr[6] = (m_range < 0 ? '-' : '+');
		return tmpstr;
	}

	ControlState KeyboardMouse::Key::GetState() const
	{
		return (m_key != 0);
	}

	ControlState KeyboardMouse::Button::GetState() const
	{
		return (m_button != 0);
	}

	ControlState KeyboardMouse::Axis::GetState() const
	{
		return std::clamp(ControlState(m_axis) / m_range, 0.0, 1.0);
	}
}
