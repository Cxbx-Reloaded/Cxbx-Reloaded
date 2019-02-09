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
// *   src->Common->Input->XInputPad.cpp
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

#define LOG_PREFIX CXBXR_MODULE::XINP

#include "XInputPad.h"
#include "core\kernel\support\Emu.h"
#include "InputManager.h"

#ifndef XINPUT_GAMEPAD_GUIDE
#define XINPUT_GAMEPAD_GUIDE 0x0400
#endif


namespace XInput
{
	typedef decltype(&XInputGetCapabilities) XInputGetCapabilities_t;
	typedef decltype(&XInputSetState) XInputSetState_t;
	typedef decltype(&XInputGetState) XInputGetState_t;

	static HMODULE hXInput = nullptr;
	static XInputGetCapabilities_t PXInputGetCapabilities = nullptr;
	static XInputSetState_t PXInputSetState = nullptr;
	static XInputGetState_t PXInputGetState = nullptr;

	static bool haveGuideButton = false;
	int XInputInitStatus = XINPUT_NOT_INIT;
	uint8_t DevicesConnected = 0;

	static const struct
	{
		const char* const name;
		const WORD bitmask;
	} named_buttons[] = { { "Button A", XINPUT_GAMEPAD_A },
						  { "Button B", XINPUT_GAMEPAD_B },
						  { "Button X", XINPUT_GAMEPAD_X },
						  { "Button Y", XINPUT_GAMEPAD_Y },
						  { "Pad N", XINPUT_GAMEPAD_DPAD_UP },
						  { "Pad S", XINPUT_GAMEPAD_DPAD_DOWN },
						  { "Pad W", XINPUT_GAMEPAD_DPAD_LEFT },
						  { "Pad E", XINPUT_GAMEPAD_DPAD_RIGHT },
						  { "Start", XINPUT_GAMEPAD_START },
						  { "Back", XINPUT_GAMEPAD_BACK },
						  { "Shoulder L", XINPUT_GAMEPAD_LEFT_SHOULDER },
						  { "Shoulder R", XINPUT_GAMEPAD_RIGHT_SHOULDER },
						  { "Guide", XINPUT_GAMEPAD_GUIDE },
						  { "Thumb L", XINPUT_GAMEPAD_LEFT_THUMB },
						  { "Thumb R", XINPUT_GAMEPAD_RIGHT_THUMB } };

	static const char* const named_triggers[] = { "Trigger L", "Trigger R" };

	static const char* const named_axes[] = { "Left X", "Left Y", "Right X", "Right Y" };

	static const char* const named_motors[] = { "Motor L", "Motor R" };

	void Init(std::mutex& Mtx)
	{
		std::unique_lock<std::mutex> lck(Mtx);

		// Because we use _WIN32_WINNT=0x0601, this will load the XInput 9.1.0 library
		hXInput = ::LoadLibrary(XINPUT_DLL);
		if (!hXInput) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to initialize XInput subsystem!");
			XInputInitStatus = XINPUT_INIT_ERROR;
			return;
		}

		PXInputGetCapabilities =
			(XInputGetCapabilities_t)::GetProcAddress(hXInput, "XInputGetCapabilities");
		PXInputSetState = (XInputSetState_t)::GetProcAddress(hXInput, "XInputSetState");

		// Ordinal 100 is the same as XInputGetState, except it doesn't dummy out the guide
		// button info. Try loading it and fall back if needed.
		PXInputGetState = (XInputGetState_t)::GetProcAddress(hXInput, (LPCSTR)100);
		if (PXInputGetState) {
			haveGuideButton = true;
		}
		else {
			PXInputGetState = (XInputGetState_t)::GetProcAddress(hXInput, "XInputGetState");
		}

		if (!PXInputGetCapabilities || !PXInputSetState || !PXInputGetState)
		{
			::FreeLibrary(hXInput);
			hXInput = nullptr;
			EmuLog(LOG_LEVEL::WARNING, "Failed to find XInput functions!");
			XInputInitStatus = XINPUT_INIT_ERROR;
			return;
		}
		XInputInitStatus = XINPUT_INIT_SUCCESS;
	}

	void DeInit()
	{
		if (hXInput)
		{
			::FreeLibrary(hXInput);
			hXInput = nullptr;
		}
	}
	
	void GetDeviceChanges()
	{
		XINPUT_CAPABILITIES caps;
		DWORD ret;
		for (uint8_t i = 0, mask = 1; i != 4; ++i, mask <<= 1) {
			ret = PXInputGetCapabilities(i, 0, &caps);
			if ((ERROR_SUCCESS == ret) && !(DevicesConnected & mask)) {
				auto Device = std::make_shared<XDevice>(caps, i);
				// only add the device if it has some I/O controls
				if (!Device->GetInputs().empty() || !Device->GetOutputs().empty()) {
					g_InputDeviceManager.AddDevice(std::move(Device));
				}
				else {
					EmuLog(LOG_LEVEL::INFO, "Rejected XInput device %i. No controls detected", i);
				}
				DevicesConnected |= mask;
			}
			else if ((ERROR_DEVICE_NOT_CONNECTED == ret) && (DevicesConnected & mask)) {
				g_InputDeviceManager.RemoveDevice([&i](const auto& Device) {
					const XDevice* dev = dynamic_cast<const XDevice*>(Device);
					return dev && (dev->GetXInputDevice() == i);
				});
				DevicesConnected &= ~mask;
			}
		}
	}

	void PopulateDevices()
	{
		DevicesConnected = 0;
		GetDeviceChanges();
	}

	XDevice::XDevice(const XINPUT_CAPABILITIES& Capabilities, uint8_t Index) : m_Subtype(Capabilities.SubType), m_Index(Index)
	{
		// XInputGetCaps can be broken on some devices, so we'll just ignore it
		// and assume all gamepad + vibration capabilities are supported

		// get supported buttons
		for (int i = 0; i != sizeof(named_buttons) / sizeof(*named_buttons); ++i) {
			// Only add guide button if we have the 100 ordinal XInputGetState
			if (!(named_buttons[i].bitmask & XINPUT_GAMEPAD_GUIDE) || haveGuideButton) {
				AddInput(new Button(i, m_state_in.Gamepad.wButtons));
			}
		}

		// get supported triggers
		for (int i = 0; i != sizeof(named_triggers) / sizeof(*named_triggers); ++i) {
			AddInput(new Trigger(i, (&m_state_in.Gamepad.bLeftTrigger)[i], 255));
		}

		// get supported axes
		for (int i = 0; i != sizeof(named_axes) / sizeof(*named_axes); ++i) {
			const SHORT& ax = (&m_state_in.Gamepad.sThumbLX)[i];

			// each axis gets a negative and a positive input instance associated with it
			AddInput(new Axis(i, ax, -32768));
			AddInput(new Axis(i, ax, 32767));
		}

		// get supported motors
		for (int i = 0; i != sizeof(named_motors) / sizeof(*named_motors); ++i) {
			AddOutput(new Motor(i, this, (&m_state_out.wLeftMotorSpeed)[i], 65535));
		}

		ZeroMemory(&m_state_in, sizeof(m_state_in));
	}

	std::string XDevice::GetDeviceName() const
	{
		switch (m_Subtype)
		{
			case XINPUT_DEVSUBTYPE_GAMEPAD:
				return "Gamepad";
			// disabled, as the XInput 9.1.0 library will always report XINPUT_DEVSUBTYPE_GAMEPAD
			//case XINPUT_DEVSUBTYPE_WHEEL:
			//	return "Wheel";
			//case XINPUT_DEVSUBTYPE_ARCADE_STICK:
			//	return "Arcade Stick";
			//case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
			//	return "Flight Stick";
			//case XINPUT_DEVSUBTYPE_DANCE_PAD:
			//	return "Dance Pad";
			//case XINPUT_DEVSUBTYPE_GUITAR:
			//	return "Guitar";
			//case XINPUT_DEVSUBTYPE_DRUM_KIT:
			//	return "Drum Kit";
			default:
				return "Device";
		}
	}

	std::string XDevice::GetAPI() const
	{
		return "XInput";
	}

	uint8_t XDevice::GetXInputDevice() const
	{
		return m_Index;
	}

	// Update I/O

	void XDevice::UpdateInput()
	{
		PXInputGetState(m_Index, &m_state_in);
	}

	void XDevice::UpdateMotors()
	{
		// only calls XInputSetState if the state changed
		if (memcmp(&m_state_out, &m_current_state_out, sizeof(m_state_out)))
		{
			m_current_state_out = m_state_out;
			PXInputSetState(m_Index, &m_state_out);
		}
	}

	// GET name/source/id

	std::string XDevice::Button::GetName() const
	{
		return named_buttons[m_Index].name;
	}

	std::string XDevice::Axis::GetName() const
	{
		return std::string(named_axes[m_Index]) + (m_Range < 0 ? '-' : '+');
	}

	std::string XDevice::Trigger::GetName() const
	{
		return named_triggers[m_Index];
	}

	std::string XDevice::Motor::GetName() const
	{
		return named_motors[m_Index];
	}

	// GET / SET STATES

	ControlState XDevice::Button::GetState() const
	{
		return (m_Buttons & named_buttons[m_Index].bitmask) > 0;
	}

	ControlState XDevice::Trigger::GetState() const
	{
		return ControlState(m_Trigger) / m_Range;
	}

	ControlState XDevice::Axis::GetState() const
	{
		return std::max(0.0, ControlState(m_Axis) / m_Range);
	}

	void XDevice::Motor::SetState(ControlState state)
	{
		m_Motor = (WORD)(state * m_Range);
		m_Parent->UpdateMotors();
	}
}
