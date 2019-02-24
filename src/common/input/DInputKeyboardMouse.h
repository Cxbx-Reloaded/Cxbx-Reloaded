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

#pragma once

#include "InputDevice.h"
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"


namespace DInput
{
	extern bool bKbMoEnumerated;
	void GetDeviceChanges();
	void PopulateDevices();

	class KeyboardMouse : public InputDevice
	{
		public:
			KeyboardMouse(const LPDIRECTINPUTDEVICE8 kb_device, const LPDIRECTINPUTDEVICE8 mo_device);
			~KeyboardMouse();
			std::string GetDeviceName() const override;
			std::string GetAPI() const override;
			bool UpdateInput() override;


		private:
			class Key : public Input
			{
				public:
					Key(uint8_t index, const BYTE& key) : m_index(index), m_key(key) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					const BYTE& m_key;
					const uint8_t m_index;
			};

			class Button : public Input
			{
				public:
					Button(uint8_t index, const BYTE& button) : m_button(button), m_index(index) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					const BYTE& m_button;
					const uint8_t m_index;
			};

			class Axis : public Input
			{
				public:
					Axis(uint8_t index, const LONG& axis, LONG range) : m_axis(axis), m_range(range), m_index(index) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					const LONG& m_axis;
					const LONG m_range;
					const uint8_t m_index;
			};

			class Cursor : public Input
			{
				public:
					Cursor(uint8_t index, const ControlState& axis, const bool positive)
					: m_axis(axis), m_index(index), m_positive(positive) {}
					std::string GetName() const override;
					bool IsDetectable() override { return false; }
					ControlState GetState() const override;

				private:
					const ControlState& m_axis;
					const uint8_t m_index;
					const bool m_positive;
			};

			struct State
			{
				BYTE keyboard[256];
				DIMOUSESTATE2 mouse;
				struct
				{
					ControlState x, y;
				} cursor;
			};

			const LPDIRECTINPUTDEVICE8 m_kb_device;
			const LPDIRECTINPUTDEVICE8 m_mo_device;
			DWORD m_last_update;
			State m_state_in;
	};
}
