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
// *   src->Common->Input->XInputPad.h
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

#include <windows.h>
#include <XInput.h>
#include "InputDevice.h"


namespace XInput
{
	typedef enum _XINPUT_INIT_STATUS : int
	{
		XINPUT_NOT_INIT = -2,
		XINPUT_INIT_ERROR,
		XINPUT_INIT_SUCCESS,
	}
	XINPUT_INIT_STATUS;

	extern int XInputInitStatus;
	extern uint8_t DevicesConnected;

	// initialize XInput
	void Init(std::mutex& Mtx);
	// shutdown XInput
	void DeInit();
	// detect device connect/disconnect
	void GetDeviceChanges();
	// refresh the device list in response to a refresh command from the input GUI
	void PopulateDevices();

	class XDevice : public InputDevice
	{
		private:
			class Button : public InputDevice::Input
			{
				public:
					Button(uint8_t Index, const WORD& Buttons) : m_Buttons(Buttons), m_Index(Index) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					// button state
					const WORD& m_Buttons;
					// button index
					uint8_t m_Index;
			};

			class Axis : public InputDevice::Input
			{
				public:
					Axis(uint8_t Index, const SHORT& Axis, SHORT Range) : m_Index(Index), m_Axis(Axis), m_Range(Range) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					// axis state
					const SHORT& m_Axis;
					// max value of the axis
					const SHORT m_Range;
					// axis index
					const uint8_t m_Index;
			};

			class Trigger : public InputDevice::Input
			{
				public:
					Trigger(uint8_t Index, const BYTE& Trigger, BYTE Range)
						: m_Trigger(Trigger), m_Range(Range), m_Index(Index) {}
					std::string GetName() const override;
					ControlState GetState() const override;

				private:
					// trigger state
					const BYTE& m_Trigger;
					// max value of the trigger
					const BYTE m_Range;
					// trigger index
					const uint8_t m_Index;
			};

			class Motor : public InputDevice::Output
			{
				public:
					Motor(XDevice* Parent, WORD& MotorLeft, WORD& MotorRight, WORD Range)
						: m_MotorLeft(MotorLeft), m_MotorRight(MotorRight), m_Range(Range), m_Parent(Parent) {}
					std::string GetName() const override;
					void SetState(ControlState StateLeft, ControlState StateRight) override;

				private:
					// left motor state
					WORD& m_MotorLeft;
					// right motor state
					WORD& m_MotorRight;
					// max value of the motor strength
					const WORD m_Range;
					// parent object of the motor
					XDevice* m_Parent;
			};

		public:
			void UpdateInput() override;

			XDevice(const XINPUT_CAPABILITIES& Capabilities, uint8_t Index);

			std::string GetDeviceName() const override;
			std::string GetAPI() const override;
			uint8_t GetXInputDevice() const;

			void UpdateMotors();

		private:
			XINPUT_STATE m_state_in;
			XINPUT_VIBRATION m_state_out{};
			const BYTE m_Subtype;
			const uint8_t m_Index;
	};
}
