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

#ifndef SDL_H_
#define SDL_H_

#include "InputDevice.h"
#include <SDL_haptic.h>
#include <thread>


namespace Sdl
{
	typedef enum _SDL_INIT_STATUS : int
	{
		SDL_NOT_INIT = -2,
		SDL_INIT_ERROR,
		SDL_INIT_SUCCESS,
	}
	SDL_INIT_STATUS;

	extern uint32_t ExitEvent_t;
	extern uint32_t PopulateEvent_t;
	extern uint32_t UpdateInputEvent_t;
	extern uint32_t DeviceRemoveAck_t;
	extern int SdlInitStatus;
	extern bool SdlPopulateOK;

	// initialize SDL
	void Init(std::mutex& Mtx, std::condition_variable& Cv, bool is_gui);
	// shutdown SDL
	void DeInit(std::thread& Thr);
	// open the sdl joystick with the specified index
	void OpenSdlDevice(const int Index);
	// close the sdl joystick with the specified index
	void CloseSdlDevice(const int Index);
	// refresh the device list in response to a refresh command from the input GUI
	void PopulateDevices();

	class SdlJoystick : public InputDevice
	{
	public:
		SdlJoystick(SDL_Joystick* const Joystick, const int Index);
		~SdlJoystick();
		// retrieves the name of the device
		std::string GetDeviceName() const override;
		// returns "SDL"
		std::string GetAPI() const override;
		// retrieves the joystick identifier used by SDL
		SDL_Joystick* GetSDLJoystick() const;
		// updates the state of this joystick
		bool UpdateInput() override;
		// retrieves the sdl id of this device
		SDL_JoystickID GetId(SDL_JoystickID id) const override;
		// sets dirty flag
		void SetDirty() { m_bDirty = true; }
		// checks if the device supports xinput
		bool IsXInput() const { return m_bIsXInput; }

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
			virtual void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) = 0;

			// the type of haptic effect
			SDL_HapticEffect m_Effect;
			// haptic device identifier as assigned by SDL, if available
			SDL_Haptic* m_Haptic;
			// id of the effect as returned by SDL
			int m_ID;

		private:
			virtual void SetState(ControlState StateLeft, ControlState StateRight) override final;
		};

		class ConstantEffect : public HapticEffect
		{
		public:
			ConstantEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
			std::string GetName() const override;

		private:
			void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) override;
		};

		class RampEffect : public HapticEffect
		{
		public:
			RampEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
			std::string GetName() const override;

		private:
			void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) override;
		};

		class SineEffect : public HapticEffect
		{
		public:
			SineEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
			std::string GetName() const override;

		private:
			void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) override;
		};

		class TriangleEffect : public HapticEffect
		{
		public:
			TriangleEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
			std::string GetName() const override;

		private:
			void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) override;
		};

		class LeftRightEffect : public HapticEffect
		{
		public:
			LeftRightEffect(SDL_Haptic* Haptic) : HapticEffect(Haptic) {}
			std::string GetName() const override;

		private:
			void SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight) override;
		};

		// the friendly name of the device
		std::string m_DeviceName;
		// pointer to the device as assigned by SDL
		SDL_Joystick* m_Joystick;
		// sdl-assigned id to this device
		SDL_JoystickID m_Sdl_ID;
		// haptic device identifier as assigned by SDL, if available
		SDL_Haptic* m_Haptic;
		// indicates if the device supports xinput
		bool m_bIsXInput;
	};
}

#endif
