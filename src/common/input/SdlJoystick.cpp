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
// *   src->Common->Input->SdlJoystick.cpp
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

#if 1 // Reenable this when LLE USB actually works
#define LOG_PREFIX CXBXR_MODULE::SDL

#include "core\kernel\support\Emu.h"
#include "SdlJoystick.h"
#include <assert.h>

// These values are those used by Dolphin!
static const u16 RUMBLE_PERIOD = 10;
static const u16 RUMBLE_LENGTH_MAX = 500; 


SdlJoystick::SdlJoystick(SDL_Joystick* const Joystick, const int Index)
	: m_Joystick(Joystick), m_DeviceName(StripSpaces(SDL_JoystickNameForIndex(Index)))
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
		return;
	}
	/* FIXME: is this correct? */
	else if ((std::string::npos != lcasename.find("xbox one")) &&
		(10 == NumButtons) && (5 == NumAxes) &&
		(1 == NumHats) && (0 == NumBalls))
	{
		// this device won't be used
		return;
	}
	else if (std::string::npos != lcasename.find("xinput"))
	{
		// this device won't be used
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
	for (i = 0; i != NumHats; ++i)
	{
		// each hat gets 4 input instances associated with it (up down left right)
		for (uint8_t d = 0; d != 4; ++d) {
			AddInput(new Hat(i, m_Joystick, d));
		}
	}

	// get axes
	for (i = 0; i != NumAxes; ++i)
	{
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

void SdlJoystick::UpdateInput()
{
	SDL_JoystickUpdate();
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

void SdlJoystick::HapticEffect::SetState(ControlState state)
{
	memset(&m_Effect, 0, sizeof(m_Effect));
	if (state)
	{
		SetSDLHapticEffect(state);
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

void SdlJoystick::ConstantEffect::SetSDLHapticEffect(ControlState state)
{
	m_Effect.type = SDL_HAPTIC_CONSTANT;
	m_Effect.constant.length = RUMBLE_LENGTH_MAX;
	m_Effect.constant.level = (Sint16)(state * 0x7FFF);
}

void SdlJoystick::RampEffect::SetSDLHapticEffect(ControlState state)
{
	m_Effect.type = SDL_HAPTIC_RAMP;
	m_Effect.ramp.length = RUMBLE_LENGTH_MAX;
	m_Effect.ramp.start = (Sint16)(state * 0x7FFF);
}

void SdlJoystick::SineEffect::SetSDLHapticEffect(ControlState state)
{
	m_Effect.type = SDL_HAPTIC_SINE;
	m_Effect.periodic.period = RUMBLE_PERIOD;
	m_Effect.periodic.magnitude = (Sint16)(state * 0x7FFF);
	m_Effect.periodic.offset = 0;
	m_Effect.periodic.phase = 18000;
	m_Effect.periodic.length = RUMBLE_LENGTH_MAX;
	m_Effect.periodic.delay = 0;
	m_Effect.periodic.attack_length = 0;
}

void SdlJoystick::TriangleEffect::SetSDLHapticEffect(ControlState state)
{
	m_Effect.type = SDL_HAPTIC_TRIANGLE;
	m_Effect.periodic.period = RUMBLE_PERIOD;
	m_Effect.periodic.magnitude = (Sint16)(state * 0x7FFF);
	m_Effect.periodic.offset = 0;
	m_Effect.periodic.phase = 18000;
	m_Effect.periodic.length = RUMBLE_LENGTH_MAX;
	m_Effect.periodic.delay = 0;
	m_Effect.periodic.attack_length = 0;
}

void SdlJoystick::LeftRightEffect::SetSDLHapticEffect(ControlState state)
{
	m_Effect.type = SDL_HAPTIC_LEFTRIGHT;
	m_Effect.leftright.length = RUMBLE_LENGTH_MAX;
	// max ranges tuned to 'feel' similar in magnitude to triangle/sine on xbox360 controller
	m_Effect.leftright.large_magnitude = (Uint16)(state * 0x4000);
	m_Effect.leftright.small_magnitude = (Uint16)(state * 0xFFFF);
}

#endif
