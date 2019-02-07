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
// *   src->Common->Input->BoundDevice.h
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

#include "BoundDevice.h"
#include "SdlJoystick.h"
#include "XInputPad.h"
#include <algorithm>


BoundDevice* g_InputDeviceManager;

void BoundDevice::RefreshDevices()
{
	std::unique_lock<std::mutex> lck(m_Mtx);
	Sdl::SdlPopulateOK = false;
	m_Devices.clear();
	XInput::PopulateDevices();
	Sdl::PopulateDevices();
	m_Cv.wait(lck, []() {
		return Sdl::SdlPopulateOK;
	});
}

std::vector<std::string> BoundDevice::GetDeviceList()
{
	std::vector<std::string> dev_list;

	std::for_each(m_Devices.begin(), m_Devices.end(), [&dev_list](const auto& Device) {
		dev_list.push_back(Device.get()->GetQualifiedName());
	});

	return dev_list;
}
