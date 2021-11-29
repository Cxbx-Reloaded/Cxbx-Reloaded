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

// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file of Dolphin at https://github.com/dolphin-emu/dolphin/blob/master/license.txt.

// Partially derived from Device.cpp of Dolphin emulator
// https://github.com/dolphin-emu/dolphin

#include "InputDevice.h"
#include "InputManager.h"
#include "common\util\CxbxUtil.h"
#include <algorithm>
#include <charconv>


std::string GetInputDeviceName(int dev_type)
{
	std::string str;

	switch (dev_type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
		str = "MS Gamepad Duke";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
		str = "MS Gamepad S";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHTGUN):
		str = "EMS TopGun II";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL):
		str = "Steering wheel";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT):
		str = "Memory unit";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE):
		str = "IR dongle";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER):
		str = "Steel battalion controller";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK):
		str = "Arcade joystick";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::HW_STEEL_BATTALION_CONTROLLER):
		str = "Passthrough steel battalion controller";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::HW_XBOX_CONTROLLER):
		str = "Passthrough original xbox gamepad";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID):
		str = "None";
		break;

	case to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX):
		str = "Invalid";
		break;

	default:
		str = "Unknown";
	}

	return str;
}

std::string PortUserFormat(std::string_view port)
{
	int port_num, slot;
	PortStr2Int(port, &port_num, &slot);
	++port_num;
	if (slot != PORT_INVALID) {
		++slot;
		return std::to_string(port_num) + "." + std::to_string(slot);
	}
	else {
		return std::to_string(port_num);
	}
}

void PortStr2Int(std::string_view port, int *port_num, int *slot)
{
	*slot = PORT_INVALID;
	auto ret = std::from_chars(port.data(), port.data() + port.size(), *port_num);
	assert(ret.ec != std::errc::invalid_argument);
	if (ret.ptr != port.data() + port.size()) {
		++ret.ptr;
		ret = std::from_chars(ret.ptr, port.data() + port.size(), *slot);
		assert(ret.ec != std::errc::invalid_argument);
		assert(ret.ptr == port.data() + port.size());
	}
}

// Destructor, delete all inputs/outputs on device destruction
InputDevice::~InputDevice()
{
	// delete inputs
	for (InputDevice::Input* input : m_Inputs) {
		delete input;
	}

	// delete outputs
	for (InputDevice::Output* output : m_Outputs) {
		delete output;
	}
}

void InputDevice::AddInput(Input* const In)
{
	m_Inputs.push_back(In);
}

void InputDevice::AddOutput(Output* const Out)
{
	m_Outputs.push_back(Out);
}

std::string InputDevice::GetQualifiedName() const
{
	return this->GetAPI() + "/" + std::to_string(GetId()) + "/" + this->GetDeviceName();
}

const std::vector<InputDevice::IoControl*> InputDevice::GetIoControls()
{
	std::vector<IoControl*> vec;
	std::for_each(m_Inputs.begin(), m_Inputs.end(), [&vec](const auto input) {
		vec.push_back(dynamic_cast<IoControl*>(input));
		});
	std::for_each(m_Outputs.begin(), m_Outputs.end(), [&vec](const auto output) {
		vec.push_back(dynamic_cast<IoControl*>(output));
		});
	return vec;
}

const auto InputDevice::FindPort(std::string_view Port) const
{
	return std::find_if(m_XboxPort.begin(), m_XboxPort.end(), [Port](std::string_view CurrPort) {
		if (CurrPort == Port) {
			return true;
		}
		return false;
		});
}

void InputDevice::SetPort2(std::string_view Port, bool Connect)
{
	if (Connect) {
		m_XboxPort.emplace_back(Port);
	}
	else {
		const auto &it = FindPort(Port);
		if (it != m_XboxPort.end()) {
			m_XboxPort.erase(it);
		}
	}
}

bool InputDevice::GetPort(std::string_view Port) const
{
	return FindPort(Port) != m_XboxPort.end() ? true : false;
}
