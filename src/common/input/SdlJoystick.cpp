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

#define LOG_PREFIX CXBXR_MODULE::SDL

#include <assert.h>
#include <thread>
#include "core\kernel\support\Emu.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "SdlJoystick.h"
#include "XInputPad.h"
#include "DInputKeyboardMouse.h"
#include "InputManager.h"

// Acknowledgment: Inspired by the Dolphin emulator SDL input subsystem (GPLv2)
// https://github.com/dolphin-emu/dolphin

// These values are those used by Dolphin!
static const uint16_t RUMBLE_PERIOD = 10;
static const uint16_t RUMBLE_LENGTH_MAX = 500; 


namespace Sdl
{
	uint32_t ExitEvent_t;
	uint32_t PopulateEvent_t;
	uint32_t UpdateInputEvent_t;
	uint32_t DeviceRemoveAck_t;
	int SdlInitStatus = SDL_NOT_INIT;
	bool SdlPopulateOK = false;

	void Init(std::mutex& Mtx, std::condition_variable& Cv)
	{
		SDL_Event Event;
		uint32_t CustomEvent_t;
		std::unique_lock<std::mutex> lck(Mtx);

		if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
			EmuLog(LOG_LEVEL::WARNING, "Failed to initialize SDL subsystem! The error was: %s", SDL_GetError());
			SdlInitStatus = SDL_INIT_ERROR;
			lck.unlock();
			Cv.notify_one();
			return;
		}
		CustomEvent_t = SDL_RegisterEvents(4);
		if (CustomEvent_t == (uint32_t)-1) {
			SDL_Quit();
			EmuLog(LOG_LEVEL::WARNING, "Failed to create SDL custom events!");
			SdlInitStatus = SDL_INIT_ERROR;
			lck.unlock();
			Cv.notify_one();
			return;
		}
		ExitEvent_t = CustomEvent_t;
		PopulateEvent_t = CustomEvent_t + 1;
		UpdateInputEvent_t = CustomEvent_t + 2;
		DeviceRemoveAck_t = CustomEvent_t + 3;

		SetThreadAffinityMask(GetCurrentThread(), g_CPUOthers);

		// Drain all joystick add/remove events to avoid creating duplicated
		// devices when we call PopulateDevices
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type)
			{
				case SDL_JOYDEVICEADDED: { break; }
				case SDL_JOYDEVICEREMOVED: { break; }
				default:
					break;
			}
		}
		SdlInitStatus = SDL_INIT_SUCCESS;
		lck.unlock();
		Cv.notify_one();

		while (SDL_WaitEvent(&Event))
		{
			if (Event.type == SDL_JOYDEVICEADDED) {
				OpenSdlDevice(Event.jdevice.which);
			}
			else if (Event.type == SDL_JOYDEVICEREMOVED) {
				CloseSdlDevice(Event.jdevice.which);
			}
			else if (Event.type == SDL_JOYAXISMOTION ||
					 Event.type == SDL_JOYHATMOTION  ||
					 Event.type == SDL_JOYBALLMOTION ||
					 Event.type == SDL_JOYBUTTONDOWN ||
					 Event.type == SDL_JOYBUTTONUP) {
				SDL_JoystickID id;
				switch (Event.type)
				{
					case SDL_JOYAXISMOTION:
						id = Event.jaxis.which;
						break;

					case SDL_JOYHATMOTION:
						id = Event.jhat.which;
						break;

					case SDL_JOYBALLMOTION:
						id = Event.jball.which;
						break;

					case SDL_JOYBUTTONDOWN:
					case SDL_JOYBUTTONUP:
						id = Event.jbutton.which;
						break;

					default: {
						// unreachable
					}
				}
				auto dev = g_InputDeviceManager.FindDevice(id);
				if (dev != nullptr) {
					dynamic_cast<SdlJoystick*>(dev.get())->SetDirty();
				}
			}
			else if (Event.type == PopulateEvent_t) {
				for (int i = 0; i < SDL_NumJoysticks(); i++) {
					OpenSdlDevice(i);
				}
				SdlPopulateOK = true;
				Cv.notify_one();
			}
			else if (Event.type == ExitEvent_t) {
				break;
			}
			else if (Event.type == UpdateInputEvent_t) {
				XInput::GetDeviceChanges();
				DInput::GetDeviceChanges();
				g_InputDeviceManager.UpdateDevices(*static_cast<int*>(Event.user.data1), false);
				delete Event.user.data1;
				Event.user.data1 = nullptr;
			}
			else if (Event.type == DeviceRemoveAck_t) {
				g_InputDeviceManager.UpdateDevices(*static_cast<int*>(Event.user.data1), true);
				delete Event.user.data1;
				Event.user.data1 = nullptr;
			}
		}

		SDL_Quit();
	}

	void DeInit(std::thread& Thr)
	{
		SdlInitStatus = SDL_NOT_INIT;
		if (!Thr.joinable()) {
			return;
		}

		SDL_Event ExitEvent;
		SDL_memset(&ExitEvent, 0, sizeof(SDL_Event));
		ExitEvent.type = ExitEvent_t;
		SDL_PushEvent(&ExitEvent);

		Thr.join();
	}

	void OpenSdlDevice(const int Index)
	{
		SDL_Joystick* pJoystick = SDL_JoystickOpen(Index);
		if (pJoystick) {
			auto Device = std::make_shared<Sdl::SdlJoystick>(pJoystick, Index);
			if (Device->IsXInput()) {
				EmuLog(LOG_LEVEL::INFO, "Rejected joystick %i. It will be handled by XInput", Index);
				return;
			}
			// only add the device if it has some I/O controls
			if (!Device->GetInputs().empty() || !Device->GetOutputs().empty()) {
				g_InputDeviceManager.AddDevice(std::move(Device));
			}
			else {
				EmuLog(LOG_LEVEL::INFO, "Rejected joystick %i. No controls detected", Index);
			}
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "Failed to open joystick %i. The error was %s", Index, SDL_GetError());
		}
	}

	void CloseSdlDevice(const int Index)
	{
		g_InputDeviceManager.RemoveDevice([&Index](const auto& Device) {
			const SdlJoystick* joystick = dynamic_cast<const SdlJoystick*>(Device);
			return joystick && SDL_JoystickInstanceID(joystick->GetSDLJoystick()) == Index;
		});
	}

	void PopulateDevices()
	{
		SDL_Event PopulateEvent;
		SDL_memset(&PopulateEvent, 0, sizeof(SDL_Event));
		PopulateEvent.type = PopulateEvent_t;
		SDL_PushEvent(&PopulateEvent);
	}

	SdlJoystick::SdlJoystick(SDL_Joystick* const Joystick, const int Index)
		: m_Joystick(Joystick), m_Sdl_ID(SDL_JoystickInstanceID(Joystick)),
		m_DeviceName(StripSpaces(SDL_JoystickNameForIndex(Index))), m_bIsXInput(false)
	{
		uint8_t i;
		int NumButtons, NumAxes, NumHats, NumBalls;
		std::string lcasename;

		// From Dolphin emu:
		// "really bad HACKS: do not use SDL for an XInput device,
		// too many people on the forums pick the SDL device and ask:
		// "why don't my 360 gamepad triggers/rumble work correctly?".
		// Checking the name is probably good (and hacky) enough,
		// but I'll double check with the num of buttons/axes"
		// ergo720: also added check for xbox one and generic xinput controllers
		lcasename = GetDeviceName();
		std::transform(lcasename.begin(), lcasename.end(), lcasename.begin(), tolower);
		NumButtons = SDL_JoystickNumButtons(Joystick);
		NumAxes = SDL_JoystickNumAxes(Joystick);
		NumHats = SDL_JoystickNumHats(Joystick);
		NumBalls = SDL_JoystickNumBalls(Joystick);

		if ((std::string::npos != lcasename.find("xbox 360")) &&
			(10 == NumButtons) && (5 == NumAxes) &&
			(1 == NumHats) && (0 == NumBalls))
		{
			// this device won't be used
			m_bIsXInput = true;
			return;
		}
		/* FIXME: is this correct? */
		else if ((std::string::npos != lcasename.find("xbox one")) &&
			(10 == NumButtons) && (5 == NumAxes) &&
			(1 == NumHats) && (0 == NumBalls))
		{
			// this device won't be used
			m_bIsXInput = true;
			return;
		}
		else if (std::string::npos != lcasename.find("xinput"))
		{
			// this device won't be used
			m_bIsXInput = true;
			return;
		}
		else if (NumButtons > 255 || NumAxes > 255 ||
			NumHats > 255 || NumBalls > 255)
		{
			// From Dolphin:
			// "This device is invalid, don't use it.
			// Some crazy devices (HP webcam 2100) end up as HID devices.
			// SDL tries parsing these as joysticks"
			return;
		}

		// get buttons
		for (i = 0; i != NumButtons; ++i) {
			AddInput(new Button(i, m_Joystick));
		}

		// get hats
		for (i = 0; i != NumHats; ++i) {
			// each hat gets 4 input instances associated with it (up down left right)
			for (uint8_t d = 0; d != 4; ++d) {
				AddInput(new Hat(i, m_Joystick, d));
			}
		}

		// get axes
		for (i = 0; i != NumAxes; ++i) {
			// each axis gets a negative and a positive input instance associated with it
			AddAnalogInputs(new Axis(i, m_Joystick, -32768), new Axis(i, m_Joystick, 32767));
		}

		// try to get supported ff effects
		m_Haptic = SDL_HapticOpenFromJoystick(m_Joystick);
		if (m_Haptic)
		{
			const unsigned int SupportedEffects = SDL_HapticQuery(m_Haptic);

			// constant effect
			if (SupportedEffects & SDL_HAPTIC_CONSTANT) {
				AddOutput(new ConstantEffect(m_Haptic));
			}

			// ramp effect
			if (SupportedEffects & SDL_HAPTIC_RAMP) {
				AddOutput(new RampEffect(m_Haptic));
			}

			// sine effect
			if (SupportedEffects & SDL_HAPTIC_SINE) {
				AddOutput(new SineEffect(m_Haptic));
			}

			// triangle effect
			if (SupportedEffects & SDL_HAPTIC_TRIANGLE) {
				AddOutput(new TriangleEffect(m_Haptic));
			}

			// left-right effect
			if (SupportedEffects & SDL_HAPTIC_LEFTRIGHT) {
				AddOutput(new LeftRightEffect(m_Haptic));
			}
		}
		else {
			EmuLog(LOG_LEVEL::INFO, "Joystick %i doesn't support any haptic effects", Index);
		}

		// init dirty flag
		m_bDirty = false;
	}

	SdlJoystick::~SdlJoystick()
	{
		if (m_Haptic)
		{
			// stop/destroy all effects
			SDL_HapticStopAll(m_Haptic);
			// close haptic first
			SDL_HapticClose(m_Haptic);
		}

		// close joystick
		SDL_JoystickClose(m_Joystick);
	}

	std::string SdlJoystick::GetDeviceName() const
	{
		return m_DeviceName;
	}

	std::string SdlJoystick::GetAPI() const
	{
		return "SDL";
	}

	SDL_Joystick* SdlJoystick::GetSDLJoystick() const
	{
		return m_Joystick;
	}

	SDL_JoystickID SdlJoystick::GetId(SDL_JoystickID id) const
	{
		// ignore id argument
		return m_Sdl_ID;
	}

	bool SdlJoystick::UpdateInput()
	{
		SDL_JoystickUpdate();
		if (m_bDirty) {
			m_bDirty = false;
			return true;
		}
		return m_bDirty;
	}

	std::string SdlJoystick::ConstantEffect::GetName() const
	{
		return "Constant";
	}

	std::string SdlJoystick::RampEffect::GetName() const
	{
		return "Ramp";
	}

	std::string SdlJoystick::SineEffect::GetName() const
	{
		return "Sine";
	}

	std::string SdlJoystick::TriangleEffect::GetName() const
	{
		return "Triangle";
	}

	std::string SdlJoystick::LeftRightEffect::GetName() const
	{
		return "LeftRight";
	}

	std::string SdlJoystick::Button::GetName() const
	{
		std::ostringstream ss;
		ss << "Button " << (int)m_Index;
		return ss.str();
	}

	std::string SdlJoystick::Axis::GetName() const
	{
		std::ostringstream ss;
		ss << "Axis " << (int)m_Index << (m_Range < 0 ? '-' : '+');
		return ss.str();
	}

	std::string SdlJoystick::Hat::GetName() const
	{
		static char tmpstr[] = "Hat . .";
		// From Dolphin: "I don't think more than 10 hats are supported"
		tmpstr[4] = (char)('0' + m_Index);
		tmpstr[6] = "NESW"[m_Direction];
		return tmpstr;
	}

	ControlState SdlJoystick::Button::GetState() const
	{
		return SDL_JoystickGetButton(m_Js, m_Index);
	}

	ControlState SdlJoystick::Axis::GetState() const
	{
		return std::max(0.0, ControlState(SDL_JoystickGetAxis(m_Js, m_Index)) / m_Range);
	}

	ControlState SdlJoystick::Hat::GetState() const
	{
		return (SDL_JoystickGetHat(m_Js, m_Index) & (1 << m_Direction)) > 0;
	}

	void SdlJoystick::HapticEffect::SetState(ControlState StateLeft, ControlState StateRight)
	{
		memset(&m_Effect, 0, sizeof(m_Effect));
		if (StateLeft || StateRight)
		{
			SetSDLHapticEffect(StateLeft, StateRight);
		}
		else
		{
			// this module uses type==0 to indicate 'off'
			m_Effect.type = 0;
		}
		Update();
	}

	void SdlJoystick::HapticEffect::Update()
	{
		if (m_ID == -1 && m_Effect.type > 0)
		{
			m_ID = SDL_HapticNewEffect(m_Haptic, &m_Effect);
			if (m_ID > -1) {
				SDL_HapticRunEffect(m_Haptic, m_ID, 1);
			}
		}
		else if (m_ID > -1 && m_Effect.type == 0)
		{
			SDL_HapticStopEffect(m_Haptic, m_ID);
			SDL_HapticDestroyEffect(m_Haptic, m_ID);
			m_ID = -1;
		}
		else if (m_ID > -1)
		{
			SDL_HapticUpdateEffect(m_Haptic, m_ID, &m_Effect);
		}
	}

	// I'm not really sure if a simple arithmetic mean is enough here, if someone has a better idea they are welcome
	// to change the code below
	void SdlJoystick::ConstantEffect::SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight)
	{
		m_Effect.type = SDL_HAPTIC_CONSTANT;
		m_Effect.constant.length = RUMBLE_LENGTH_MAX;
		m_Effect.constant.level = (Sint16)(((StateLeft + StateRight) / 2 ) * 0x7FFF);
	}

	void SdlJoystick::RampEffect::SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight)
	{
		m_Effect.type = SDL_HAPTIC_RAMP;
		m_Effect.ramp.length = RUMBLE_LENGTH_MAX;
		m_Effect.ramp.start = (Sint16)(((StateLeft + StateRight) / 2) * 0x7FFF);
	}

	void SdlJoystick::SineEffect::SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight)
	{
		m_Effect.type = SDL_HAPTIC_SINE;
		m_Effect.periodic.period = RUMBLE_PERIOD;
		m_Effect.periodic.magnitude = (Sint16)(((StateLeft + StateRight) / 2) * 0x7FFF);
		m_Effect.periodic.offset = 0;
		m_Effect.periodic.phase = 18000;
		m_Effect.periodic.length = RUMBLE_LENGTH_MAX;
		m_Effect.periodic.delay = 0;
		m_Effect.periodic.attack_length = 0;
	}

	void SdlJoystick::TriangleEffect::SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight)
	{
		m_Effect.type = SDL_HAPTIC_TRIANGLE;
		m_Effect.periodic.period = RUMBLE_PERIOD;
		m_Effect.periodic.magnitude = (Sint16)(((StateLeft + StateRight) / 2) * 0x7FFF);
		m_Effect.periodic.offset = 0;
		m_Effect.periodic.phase = 18000;
		m_Effect.periodic.length = RUMBLE_LENGTH_MAX;
		m_Effect.periodic.delay = 0;
		m_Effect.periodic.attack_length = 0;
	}

	void SdlJoystick::LeftRightEffect::SetSDLHapticEffect(ControlState StateLeft, ControlState StateRight)
	{
		m_Effect.type = SDL_HAPTIC_LEFTRIGHT;
		m_Effect.leftright.length = RUMBLE_LENGTH_MAX;
		// max ranges tuned to 'feel' similar in magnitude to triangle/sine on xbox360 controller
		m_Effect.leftright.large_magnitude = (Uint16)(StateRight * 0x4000);
		m_Effect.leftright.small_magnitude = (Uint16)(StateLeft * 0xFFFF);
	}
}
