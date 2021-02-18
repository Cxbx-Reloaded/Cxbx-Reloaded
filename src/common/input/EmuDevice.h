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

#include "Button.h"
#include "common\util\CxbxUtil.h"
#include <array>


/* Represents the guest device currently being configured in the gui */
class EmuDevice
{
public:
	EmuDevice(int type, HWND hwnd, void *wnd);
	~EmuDevice();
	Button* FindButtonById(int id);
	Button* FindButtonByIndex(int index);
	template<size_t size>
	void BindDefault(const std::array<const char *, size> &arr);
	void ClearButtons();


private:
	void CreateTooltipWindow();

	std::vector<Button*> m_buttons;
	HWND m_hwnd;
	HWND m_tooltip_hwnd;
};

template void EmuDevice::BindDefault(const std::array<const char *, XBOX_CTRL_NUM_BUTTONS> &arr);
