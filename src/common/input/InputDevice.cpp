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

#include "InputDevice.h"
#include "common\util\CxbxUtil.h"
#include <algorithm>


std::string GetInputDeviceName(int dev_type)
{
	std::string str;

	switch (dev_type)
	{
	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
		str = "MS Gamepad Duke";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
		str = "MS Gamepad S";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::LIGHT_GUN): {
		str = "Light gun";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::STEERING_WHEEL): {
		str = "Steering wheel";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::MEMORY_UNIT): {
		str = "Memory unit";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::IR_DONGLE): {
		str = "IR dongle";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
		str = "Steel battalion controller";
	}
	break;

	case to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID):
	case to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX): {
		str = "Invalid";
	}
	break;

	default:
		str = "Unknown";
	}

	return str;
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
