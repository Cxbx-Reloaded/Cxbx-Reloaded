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

#define LOG_PREFIX CXBXR_MODULE::GUI

#include "InputWindow.h"
#include "gui/resource/ResCxbx.h"
#include "common\IPCHybrid.hpp"
#include "EmuShared.h"
#include "Logging.h"
#include <future>


constexpr ControlState INPUT_DETECT_THRESHOLD = 0.35; // NOTE: this should probably be made user configurable

InputWindow::~InputWindow()
{
	delete m_DeviceConfig;
	m_DeviceConfig = nullptr;
}

bool InputWindow::IsProfileSaved()
{
	if (m_bHasChanges) {
		PopupReturn ret = PopupQuestion(m_hwnd_window, "Current configuration is not saved. Save before closing?");
		switch (ret)
		{
		case PopupReturn::Yes: {
			char name[50];
			SendMessage(m_hwnd_profile_list, WM_GETTEXT, sizeof(name), reinterpret_cast<LPARAM>(name));
			if (SaveProfile(std::string(name))) {
				return true;
			}
			return false;
		}

		case PopupReturn::No: {
			return true;
		}

		case PopupReturn::Cancel:
		default: {
			return false;
		}

		}
	}
	return true;
}

void InputWindow::UpdateDeviceList()
{
	g_InputDeviceManager.RefreshDevices();

	// Populate device list
	LRESULT num_devices = SendMessage(m_hwnd_device_list, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < num_devices; i++) {
		SendMessage(m_hwnd_device_list, CB_DELETESTRING, 0, 0);
	}

	std::vector<std::string> dev_list = g_InputDeviceManager.GetDeviceList();
	for (const auto& str : dev_list) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
	}
	if (!dev_list.empty()) {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, 0, 0);
	}

	UpdateCurrentDevice();
}

// Copyright 2016 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file of Dolphin at https://github.com/dolphin-emu/dolphin/blob/master/license.txt.

// DetectInput is derived from InputReference::Detect() in ControlReference.cpp of Dolphin emulator
// https://github.com/dolphin-emu/dolphin

InputDevice::Input* InputWindow::DetectInput(InputDevice* const Device, int ms)
{
	using namespace std::chrono;

	// The intent of the initial_states vector is to detect inputs that were pressed already when this function was called.
	// Without it, the initial mouse click used to select a gui button is very likely to be registered as valid input and bound
	Device->UpdateInput(); // Requirement to get current state than previous state. testcase: enter key / cursor
	std::vector<InputDevice::Input*>::const_iterator i = Device->GetInputs().begin(),
		e = Device->GetInputs().end(), b = i, s = e;
	std::vector<bool> initial_states(Device->GetInputs().size());
	for (std::vector<bool>::iterator state = initial_states.begin(); i != e; i++) {
		*state++ = ((*i)->GetState() > (1 - INPUT_DETECT_THRESHOLD));
	}

	auto now = system_clock::now();
	auto timeout = now + milliseconds(ms);

	while (now <= timeout) {
		Device->UpdateInput();
		std::vector<bool>::iterator state = initial_states.begin();
		for (; i != e && s == e; i++, state++) {
			if ((*i)->IsDetectable() && ((*i)->GetState() > INPUT_DETECT_THRESHOLD)) {
				if (*state == false) {
					// input was not initially pressed or it was but released afterwards
					s = i;
					break;
				}
				else if ((*i)->GetState() < (1 - INPUT_DETECT_THRESHOLD)) {
					*state = false;
				}
			}
		}
		if (s != e && (*s)->GetState() < (1 - INPUT_DETECT_THRESHOLD)) {
			return *s;
		}
		std::this_thread::sleep_for(milliseconds(10));
		now += milliseconds(10);
		i = b;
	}

	return nullptr; // no input
}

void InputWindow::BindButton(int ControlID)
{
	// Check if binding thread is still active
	// testcase: spacebar and enter keys; without this fix will cause repeat binding result.
	if (m_bIsBinding) {
		return;
	}

	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		m_bIsBinding = true;

		// Don't block the message processing loop
		std::thread([this, dev, ControlID]() {
			EnableWindow(m_hwnd_window, FALSE);
			char current_text[HOST_BUTTON_NAME_LENGTH];
			Button* xbox_button = m_DeviceConfig->FindButtonById(ControlID);
			xbox_button->GetText(current_text, sizeof(current_text));
			xbox_button->UpdateText("...");
			std::future<InputDevice::Input*> fut = std::async(std::launch::async, &InputWindow::DetectInput, this, dev.get(), INPUT_TIMEOUT);
			InputDevice::Input* dev_button = fut.get();
			if (dev_button) {
				xbox_button->UpdateText(dev_button->GetName().c_str());
				m_bHasChanges = true;
			}
			else {
				xbox_button->UpdateText(current_text);
			}
			m_bIsBinding = false;
			EnableWindow(m_hwnd_window, TRUE);
		}).detach();
	}
}

void InputWindow::UpdateProfile(const std::string &name, int command)
{
	switch (command)
	{
	case PROFILE_LOAD:
		LoadProfile(name);
		break;

	case PROFILE_SAVE:
		SaveProfile(name);
		break;

	case PROFILE_DELETE:
		DeleteProfile(name);
		break;

	case BUTTON_CLEAR:
	case BUTTON_SWAP:
		m_bHasChanges = true;
		break;
	}
}

InputWindow::ProfileIt InputWindow::FindProfile(const std::string& name)
{
	auto it = std::find_if(g_Settings->m_input_profiles[m_dev_type].begin(),
		g_Settings->m_input_profiles[m_dev_type].end(), [&name](const auto& profile) {
			return profile.ProfileName == name;
		});
	return it;
}

void InputWindow::LoadProfile(const std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	m_host_dev = profile->DeviceName;
	EnableDefaultButton();
	LRESULT dev_str_index = SendMessage(m_hwnd_device_list, CB_FINDSTRINGEXACT, 1, reinterpret_cast<LPARAM>(m_host_dev.c_str()));
	if (dev_str_index == CB_ERR) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(m_host_dev.c_str()));
	}
	else {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, dev_str_index, 0);
	}
	SendMessage(m_hwnd_profile_list, CB_SETCURSEL, SendMessage(m_hwnd_profile_list, CB_FINDSTRINGEXACT, 1,
		reinterpret_cast<LPARAM>(profile->ProfileName.c_str())), 0);
	for (int index = 0; index < m_max_num_buttons; index++) {
		m_DeviceConfig->FindButtonByIndex(index)->UpdateText(profile->ControlList[index].c_str());
	}
	g_Settings->m_input_port[m_port_num].DeviceName = profile->DeviceName;
	g_Settings->m_input_port[m_port_num].ProfileName = profile->ProfileName;
	m_bHasChanges = false;
}

bool InputWindow::SaveProfile(const std::string& name)
{
	if (name == std::string()) {
		PopupError(m_hwnd_window, "Cannot save. Profile name must not be empty.");
		return false;
	}
	if (m_host_dev == std::string()) {
		PopupError(m_hwnd_window, "Cannot save. No input devices detected.", "Cxbx-Reloaded");
		return false;
	}
	OverwriteProfile(name);
	Settings::s_input_profiles profile;
	profile.Type = m_dev_type;
	profile.ProfileName = name;
	profile.DeviceName = m_host_dev;
	for (int index = 0; index < m_max_num_buttons; index++) {
		char dev_button[HOST_BUTTON_NAME_LENGTH];
		m_DeviceConfig->FindButtonByIndex(index)->GetText(dev_button, sizeof(dev_button));
		profile.ControlList.push_back(dev_button);
	}
	SendMessage(m_hwnd_profile_list, CB_SETCURSEL, SendMessage(m_hwnd_profile_list, CB_ADDSTRING, 0,
		reinterpret_cast<LPARAM>(profile.ProfileName.c_str())), 0);
	g_Settings->m_input_port[m_port_num].DeviceName = profile.DeviceName;
	g_Settings->m_input_port[m_port_num].ProfileName = profile.ProfileName;
	g_Settings->m_input_profiles[m_dev_type].push_back(std::move(profile));
	m_bHasChanges = false;
	return true;
}

void InputWindow::DeleteProfile(const std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	SendMessage(m_hwnd_profile_list, CB_DELETESTRING, SendMessage(m_hwnd_profile_list, CB_FINDSTRINGEXACT, 1,
		reinterpret_cast<LPARAM>(profile->ProfileName.c_str())), 0);
	if (profile->ProfileName == g_Settings->m_input_port[m_port_num].ProfileName) {
		SendMessage(m_hwnd_profile_list, CB_SETCURSEL, -1, 0);
		UpdateCurrentDevice();
		ClearBindings();
		g_Settings->m_input_port[m_port_num].DeviceName = "";
		g_Settings->m_input_port[m_port_num].ProfileName = "";
		m_bHasChanges = false;
	}
	else {
		LoadProfile(g_Settings->m_input_port[m_port_num].ProfileName);
	}
	g_Settings->m_input_profiles[m_dev_type].erase(profile);
}

void InputWindow::OverwriteProfile(const std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	SendMessage(m_hwnd_profile_list, CB_DELETESTRING, SendMessage(m_hwnd_profile_list, CB_FINDSTRINGEXACT, 1,
		reinterpret_cast<LPARAM>(profile->ProfileName.c_str())), 0);
	SendMessage(m_hwnd_profile_list, CB_SETCURSEL, -1, 0);
	g_Settings->m_input_profiles[m_dev_type].erase(profile);
}

void InputWindow::LoadDefaultProfile()
{
	for (unsigned int index = 0; index < g_Settings->m_input_profiles[m_dev_type].size(); index++) {
		SendMessage(m_hwnd_profile_list, CB_ADDSTRING, 0,
			reinterpret_cast<LPARAM>(g_Settings->m_input_profiles[m_dev_type][index].ProfileName.c_str()));
	}
	LoadProfile(g_Settings->m_input_port[m_port_num].ProfileName);
}

void InputWindow::UpdateCurrentDevice()
{
	char device_name[50];
	SendMessage(m_hwnd_device_list, WM_GETTEXT, sizeof(device_name), reinterpret_cast<LPARAM>(device_name));
	m_host_dev = device_name;
	EnableDefaultButton();
}

void InputWindow::SwapMoCursorAxis(Button *button)
{
	// Axis X+ <-> Cursor X+
	// Axis X- <-> Cursor X-
	// Axis Y+ <-> Cursor Y-
	// Axis Y- <-> Cursor Y+
	if (StrEndsWith(m_host_dev, "KeyboardMouse")) {
		assert(button != nullptr);
		char control_name[HOST_BUTTON_NAME_LENGTH];
		button->GetText(control_name, sizeof(control_name));
		if (StrStartsWith(control_name, "Axis")) {
			switch (control_name[5])
			{
			case 'X':
				if (control_name[6] == '+') {
					button->UpdateText("Cursor X+");
				}
				else {
					button->UpdateText("Cursor X-");
				}
				break;

			case 'Y':
				if (control_name[6] == '+') {
					button->UpdateText("Cursor Y-");
				}
				else {
					button->UpdateText("Cursor Y+");
				}
				break;

			}

			return;
		}

		if (StrStartsWith(control_name, "Cursor")) {
			switch (control_name[7])
			{
			case 'X':
				if (control_name[8] == '+') {
					button->UpdateText("Axis X+");
				}
				else {
					button->UpdateText("Axis X-");
				}
				break;

			case 'Y':
				if (control_name[8] == '+') {
					button->UpdateText("Axis Y-");
				}
				else {
					button->UpdateText("Axis Y+");
				}
				break;

			}
		}
	}
}
