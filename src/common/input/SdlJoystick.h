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
// *   src->Common->Input->SdlJoystick.h
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

#ifndef SDL2DEVICE_H_
#define SDL2DEVICE_H_
#if 1 // Reenable this when LLE USB actually works
//#include "InputConfig.h"
#include "InputDevice.h"
#include "SDL.h"
#include <SDL_haptic.h>


class SdlJoystick : public InputDevice
{
	public:
		SdlJoystick(SDL_Joystick* const Joystick, const int Index);
		~SdlJoystick();
		// retrieves the name of the device
		std::string GetDeviceName() const override;
		// returns "SDL"
		std::string GetAPI() const override;
		// updates the state of this joystick
		void UpdateInput() override;


	private:
		class Button : public InputDevice::Input
		{
			public:
				Button(uint8_t Index, SDL_Joystick* Js) : m_Index(Index), m_Js(Js) {}
				std::string GetName() const override;
				ControlState GetState() const override;

			private:
				// pointer to the device this button belongs to
				SDL_Joystick* const m_Js;
				// arbitrary index assigned to this button
				const uint8_t m_Index;
		};

		class Hat : public InputDevice::Input
		{
			public:
				Hat(uint8_t Index, SDL_Joystick* Js, uint8_t Direction) : m_Index(Index), m_Js(Js), m_Direction(Direction) {}
				std::string GetName() const override;
				ControlState GetState() const override;

			private:
				// pointer to the device this hat button belongs to
				SDL_Joystick* const m_Js;
				// identifies the direction of the hat button (arbitrary, assigned by us)
				const uint8_t m_Direction;
				// arbitrary index assigned to this hat button
				const uint8_t m_Index;
		};

		class Axis : public InputDevice::Input
		{
			public:
				std::string GetName() const override;
				Axis(uint8_t Index, SDL_Joystick* Js, int16_t Range) : m_Index(Index), m_Js(Js), m_Range(Range) {}
				ControlState GetState() const override;

			private:
				// pointer to the device this axis belongs to
				SDL_Joystick* const m_Js;
				// max value of this axis
				const int16_t m_Range;
				// arbitrary index assigned to this axis
				const uint8_t m_Index;
		};

		class HapticEffect : public Output
		{
			public:
				HapticEffect(SDL_Haptic* Haptic) : m_Haptic(Haptic), m_ID(-1) {}
				~HapticEffect()
				{
					m_Effect.type = 0;
					Update();
				}

			protected:
				void Update();
				virtual void SetSDLHapticEffect(ControlState state) = 0;

				// the type of haptic effect
				SDL_HapticEffect m_Effect;
				// haptic device identifier as assigned by SDL, if available
				SDL_Haptic* m_Haptic;
				// id of the effect as returned by SDL
				int m_ID;

			private:
				virtual void SetState(ControlState state) override final;
		};

		class ConstantEffect : public HapticEffect
		{
			public:
				ConstantEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
				std::string GetName() const override;

			private:
				void SetSDLHapticEffect(ControlState state) override;
		};

		class RampEffect : public HapticEffect
		{
			public:
				RampEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
				std::string GetName() const override;

			private:
				void SetSDLHapticEffect(ControlState state) override;
		};

		class SineEffect : public HapticEffect
		{
			public:
				SineEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
				std::string GetName() const override;

			private:
				void SetSDLHapticEffect(ControlState state) override;
		};

		class TriangleEffect : public HapticEffect
		{
			public:
				TriangleEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
				std::string GetName() const override;

			private:
				void SetSDLHapticEffect(ControlState state) override;
		};

		class LeftRightEffect : public HapticEffect
		{
			public:
				LeftRightEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
				std::string GetName() const override;

			private:
				void SetSDLHapticEffect(ControlState state) override;
		};

		// the friendly name of the device
		std::string m_DeviceName;
		// pointer to the device as assigned by SDL
		SDL_Joystick* m_Joystick;
		// haptic device identifier as assigned by SDL, if available
		SDL_Haptic* m_Haptic;
};

class SdlDevice
{
	public:
		// device is attached to the xbox
		bool m_Attached;
		// indicates the xbox device emulated by this joystick/gamepad
		int m_Type;

		// QueryDeviceID() and create arbitrary dev id assigned by InputDeviceManager
		// update analog button state
		void UpdateAnalogButtonState(uint8_t xbox_button, uint8_t state);
		// update digital button state
		void UpdateDigitalButtonState(uint8_t xbox_button, uint8_t state);
		// update hat state
		void UpdateHatState(uint8_t state);
		// update axis state
		void UpdateAxisState(uint8_t xbox_button, int16_t state);
		// returns the xbox button bound with the sdl key provided
		int GetBoundButton(int sdl_key);
		// update bBindingsChanged flag
		void SetChangedBinding(bool bFlag) { m_bBindingsChanged = bFlag; }
		// read the current button state of a device
		bool ReadButtonState(uint16_t* wButtons, uint8_t* bAnalogButtons, int16_t* sThumbLX,
			int16_t* sThumbLY, int16_t* sThumbRX, int16_t* sThumbRY);


	private:
		// default bindings (hardcoded for now)
		const int m_ButtonMap_Analog[8][2] = {
			{ GAMEPAD_A, SDL_CONTROLLER_BUTTON_A },
			{ GAMEPAD_B, SDL_CONTROLLER_BUTTON_B },
			{ GAMEPAD_X, SDL_CONTROLLER_BUTTON_X },
			{ GAMEPAD_Y, SDL_CONTROLLER_BUTTON_Y },
			{ GAMEPAD_BLACK, SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
			{ GAMEPAD_WHITE, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
			{ GAMEPAD_LEFT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERLEFT },
			{ GAMEPAD_RIGHT_TRIGGER, SDL_CONTROLLER_AXIS_TRIGGERRIGHT },
		};
		const int m_ButtonMap_Binary[8][2] = {
			{ GAMEPAD_BACK, SDL_CONTROLLER_BUTTON_BACK },
			{ GAMEPAD_START, SDL_CONTROLLER_BUTTON_START },
			{ GAMEPAD_LEFT_THUMB, SDL_CONTROLLER_BUTTON_LEFTSTICK },
			{ GAMEPAD_RIGHT_THUMB, SDL_CONTROLLER_BUTTON_RIGHTSTICK },
			{ GAMEPAD_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_UP },
			{ GAMEPAD_DPAD_DOWN, SDL_CONTROLLER_BUTTON_DPAD_DOWN },
			{ GAMEPAD_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_LEFT },
			{ GAMEPAD_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
		};
		const int m_AxisMap[4][2] = {
			{ GAMEPAD_LEFT_THUMB_X, 0 },
			{ GAMEPAD_LEFT_THUMB_Y, 1 },
			{ GAMEPAD_RIGHT_THUMB_X, 4 },
			{ GAMEPAD_RIGHT_THUMB_Y, 2 },
		};
		// indicates that this joystick is supported by the sdl game controller api
		bool m_bUsesGamepadApi;

		// m_HatIndex is added to HAT_CONSTANT to avoid collisions between it and the sdl button indices 
		int m_HatIndex = HAT_CONSTANT;
		// current button state of this joystick
		ButtonState m_State;
		// signals that the current bindings should be updated
		std::atomic_bool m_bBindingsChanged;
		// indicates that the button state has been updated since the last time it was read
		std::atomic_bool bStateDirty;
};
#endif
#endif
