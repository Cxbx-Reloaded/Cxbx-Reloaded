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

#include "InputManager.h"
#include "common\Settings.hpp"
#include <Commctrl.h>

#define PROFILE_LOAD   1
#define PROFILE_SAVE   2
#define PROFILE_DELETE 3
#define RUMBLE_SET     4
#define RUMBLE_UPDATE  5
#define RUMBLE_TEST    6


LRESULT CALLBACK ProfileNameSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


class InputWindow
{
public:
	void Initialize(HWND hwnd, HWND hwnd_krnl, int port_num, int dev_type);
	void InitRumble(HWND hwnd);
	~InputWindow();
	void UpdateDeviceList();
	void BindButton(int ControlID);
	void BindXInput();
	void ClearBindings();
	void UpdateProfile(std::string& name, int command);
	void UpdateRumble(int command);
	void UpdateCurrentDevice();


private:
	typedef std::vector<Settings::s_input_profiles>::iterator ProfileIt;
	InputDevice::Input* DetectInput(InputDevice* const Device, int ms);
	void DetectOutput(int ms);
	ProfileIt FindProfile(std::string& name);
	void LoadProfile(std::string& name);
	void SaveProfile(std::string& name);
	void DeleteProfile(std::string& name);
	void OverwriteProfile(std::string& name);
	void LoadDefaultProfile();

	// xbox device under configuration
	EmuDevice* m_DeviceConfig;
	// handle of the window
	HWND m_hwnd_window;
	// handle of the device list combobox
	HWND m_hwnd_device_list;
	// handle of the profile list combobox
	HWND m_hwnd_profile_list;
	// handle of the kernel window
	HWND m_hwnd_krnl;
	// handle of the rumble window
	HWND m_hwnd_rumble;
	// handle of the rumble combobox
	HWND m_hwnd_rumble_list;
	// type of the device
	int m_dev_type;
	// num of buttons of device under configuration
	int m_max_num_buttons;
	// xbox port number the host device attaches to (0 - 3)
	int m_port_num;
	// host device under configuration
	std::string m_host_dev;
	// currently selected rumble control
	std::string m_rumble;
};

extern InputWindow* g_InputWindow;
