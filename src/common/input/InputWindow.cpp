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

#include "InputWindow.h"
#include "..\..\gui\ResCxbx.h"
#include "common\IPCHybrid.hpp"
#include "EmuShared.h"
#include <future>

#define INPUT_TIMEOUT  5000
#define OUTPUT_TIMEOUT 3000


constexpr ControlState INPUT_DETECT_THRESHOLD = 0.55; // arbitrary number, using what Dolphin uses
InputWindow* g_InputWindow = nullptr;
int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)] = {
	XBOX_CTRL_NUM_BUTTONS,
	0,
	0,
	0,
	0,
	0,
	0,
};


void InputWindow::Initialize(HWND hwnd, HWND hwnd_krnl, int port_num, int dev_type)
{
	// Save window/device specific variables
	m_hwnd_window = hwnd;
	m_hwnd_device_list = GetDlgItem(m_hwnd_window, IDC_DEVICE_LIST);
	m_hwnd_profile_list = GetDlgItem(m_hwnd_window, IDC_XID_PROFILE_NAME);
	m_hwnd_krnl = hwnd_krnl;
	m_dev_type = dev_type;
	m_max_num_buttons = dev_num_buttons[dev_type];
	m_port_num = port_num - 1;

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title;
	switch (m_dev_type)
	{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			title += "Xbox Controller Duke at port ";
		}
		break;

		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
			title += "Xbox Controller S at port ";
		}
		break;

		default:
			break;
	}
	SendMessage(m_hwnd_window, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>((title + std::to_string(port_num)).c_str()));

	// construct emu device
	m_DeviceConfig = new EmuDevice(m_dev_type, m_hwnd_window);

	// Enumerate devices
	UpdateDeviceList();

	// Load currently saved profile for this port/device type
	LoadDefaultProfile();

	// Load currently selected host device
	UpdateCurrentDevice();

	// Load rumble binding
	char rumble[30];
	m_DeviceConfig->FindButtonByIndex(m_max_num_buttons - 1)->GetText(rumble, sizeof(rumble));
	m_rumble = rumble;

	// Install the subclass for the profile combobox
	SetWindowSubclass(GetWindow(m_hwnd_profile_list, GW_CHILD), ProfileNameSubclassProc, 0, 0);
}

InputWindow::~InputWindow()
{
	SaveBindingsToDevice();
	delete m_DeviceConfig;
	m_DeviceConfig = nullptr;
}

void InputWindow::UpdateDeviceList()
{
	g_InputDeviceManager.RefreshDevices();

	// Populate device list
	LRESULT num_devices = SendMessage(m_hwnd_device_list, CB_GETCOUNT, 0, 0);
	if (num_devices > 0) {
		for (int i = 0; i < num_devices; i++) {
			SendMessage(m_hwnd_device_list, CB_DELETESTRING, i, 0);
		}
	}

	std::vector<std::string> dev_list = g_InputDeviceManager.GetDeviceList();
	for (const auto& str : dev_list) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
	}
	if (!dev_list.empty()) {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, 0, 0);
	}
}

InputDevice::Input* InputWindow::DetectInput(InputDevice* const Device, int ms)
{
	using namespace std::chrono;

	// The intent of the initial_states vector is to detect inputs that were pressed already when this function was called.
	// Without it, the initial mouse click used to select a gui button is very likely to be registered as valid input and bound
	std::vector<InputDevice::Input*>::const_iterator i = Device->GetInputs().begin(),
		e = Device->GetInputs().end(), b = i;
	std::vector<bool> initial_states(Device->GetInputs().size());
	for (std::vector<bool>::iterator state = initial_states.begin(); i != e; i++) {
		*state++ = ((*i)->GetState() > (1 - INPUT_DETECT_THRESHOLD));
	}

	auto now = system_clock::now();
	auto timeout = now + milliseconds(ms);

	while (now <= timeout) {
		Device->UpdateInput();
		std::vector<bool>::iterator state = initial_states.begin();
		for (; i != e; i++, state++) {
			if ((*i)->IsDetectable() && (*i)->GetState() > INPUT_DETECT_THRESHOLD) {
				if (*state == false) {
					// input was not initially pressed or it was but released afterwards
					return *i;
				}
			}
			else if ((*i)->GetState() < (1 - INPUT_DETECT_THRESHOLD)) {
				*state = false;
			}
		}
		std::this_thread::sleep_for(milliseconds(10));
		now += milliseconds(10);
		i = b;
	}

	return nullptr; // no input
}

void InputWindow::BindButton(int ControlID)
{
	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, dev, ControlID]() {
			EnableWindow(m_hwnd_window, FALSE);
			char current_text[30];
			Button* xbox_button = m_DeviceConfig->FindButtonById(ControlID);
			xbox_button->GetText(current_text, sizeof(current_text));
			xbox_button->UpdateText("...");
			std::future<InputDevice::Input*> fut = std::async(std::launch::async, &InputWindow::DetectInput, this, dev.get(), INPUT_TIMEOUT);
			InputDevice::Input* dev_button = fut.get();
			if (dev_button) {
				xbox_button->UpdateText(dev_button->GetName().c_str());
			}
			else {
				xbox_button->UpdateText(current_text);
			}
			EnableWindow(m_hwnd_window, TRUE);
		}).detach();
	}
}

void InputWindow::BindXInput()
{
	if (std::strncmp(m_host_dev.c_str(), "XInput", std::strlen("XInput")) == 0) {
		m_DeviceConfig->BindXInput();
	}
}

void InputWindow::ClearBindings()
{
	m_DeviceConfig->ClearButtons();
}

InputWindow::ProfileIt InputWindow::FindProfile(std::string& name)
{
	auto it = std::find_if(g_Settings->m_input_profiles[m_dev_type].begin(),
		g_Settings->m_input_profiles[m_dev_type].end(), [&name](const auto& profile) {
		return profile.ProfileName == name;
	});
	return it;
}

void InputWindow::UpdateProfile(std::string& name, int command)
{
	switch (command)
	{
		case PROFILE_LOAD: {
			LoadProfile(name);
		}
		break;

		case PROFILE_SAVE: {
			SaveProfile(name);
		}
		break;

		case PROFILE_DELETE: {
			DeleteProfile(name);
		}
		break;

		default:
			break;
	}
}

void InputWindow::LoadProfile(std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	LRESULT dev_str_index = SendMessage(m_hwnd_device_list, CB_FINDSTRINGEXACT, 1, reinterpret_cast<LPARAM>(profile->DeviceName.c_str()));
	if (dev_str_index == CB_ERR) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(profile->DeviceName.c_str()));
	}
	else {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, dev_str_index, 0);
	}
	for (int index = 0; index < m_max_num_buttons; index++) {
		m_DeviceConfig->FindButtonByIndex(index)->UpdateText(profile->ControlList[index].c_str());
	}
}

bool InputWindow::SaveProfile(std::string& name)
{
	if (name == std::string()) {
		return false;
	}
	if (m_host_dev == std::string()) {
		return false;
	}
	DeleteProfile(name);
	Settings::s_input_profiles profile;
	profile.Type = m_dev_type;
	profile.ProfileName = name;
	profile.DeviceName = m_host_dev;
	for (int index = 0; index < m_max_num_buttons; index++) {
		char dev_button[30];
		m_DeviceConfig->FindButtonByIndex(index)->GetText(dev_button, sizeof(dev_button));
		profile.ControlList.push_back(dev_button);
	}
	SendMessage(m_hwnd_profile_list, CB_SETCURSEL, SendMessage(m_hwnd_profile_list, CB_ADDSTRING, 0,
		reinterpret_cast<LPARAM>(profile.ProfileName.c_str())), 0);
	g_Settings->m_input_profiles[m_dev_type].push_back(std::move(profile));
	return true;
}

void InputWindow::DeleteProfile(std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	SendMessage(m_hwnd_profile_list, CB_DELETESTRING, SendMessage(m_hwnd_profile_list, CB_FINDSTRINGEXACT, 1,
		reinterpret_cast<LPARAM>(profile->ProfileName.c_str())), 0);
	SendMessage(m_hwnd_profile_list, CB_SETCURSEL, 1, 0);
	g_Settings->m_input_profiles[m_dev_type].erase(profile);
}

void InputWindow::LoadDefaultProfile()
{
	for (uint index = 0; index < g_Settings->m_input_profiles[m_dev_type].size(); index++) {
		SendMessage(m_hwnd_profile_list, CB_ADDSTRING, 0,
			reinterpret_cast<LPARAM>(g_Settings->m_input_profiles[m_dev_type][index].ProfileName.c_str()));
	}
	LoadProfile(g_Settings->m_input[m_port_num].ProfileName);
}

void InputWindow::SaveBindingsToDevice()
{
	char profile_name[50];
	SendMessage(m_hwnd_profile_list, WM_GETTEXT, sizeof(profile_name), reinterpret_cast<LPARAM>(profile_name));
	if (!SaveProfile(std::string(profile_name))) {
		return;
	}

	g_Settings->m_input[m_port_num].DeviceName = m_host_dev;
	g_Settings->m_input[m_port_num].ProfileName = profile_name;
}

void InputWindow::UpdateCurrentDevice()
{
	char device_name[50];
	SendMessage(m_hwnd_device_list, WM_GETTEXT, sizeof(device_name), reinterpret_cast<LPARAM>(device_name));
	m_host_dev = device_name;
}

void InputWindow::InitRumble(HWND hwnd)
{
	m_hwnd_rumble = hwnd;
	HWND hwnd_rumble_list = GetDlgItem(m_hwnd_rumble, IDC_RUMBLE_LIST);
	SendMessage(hwnd_rumble_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(""));
	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		auto outputs = dev->GetOutputs();
		for (const auto out : outputs) {
			SendMessage(hwnd_rumble_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(out->GetName().c_str()));
		}
	}
	SendMessage(hwnd_rumble_list, CB_SETCURSEL, 0, 0);
	m_rumble = std::string();
}

void InputWindow::UpdateRumble(int command)
{
	switch (command)
	{
		case RUMBLE_SET: {
			char rumble[30];
			SendMessage(GetDlgItem(m_hwnd_rumble, IDC_RUMBLE_LIST), WM_GETTEXT, sizeof(rumble), reinterpret_cast<LPARAM>(rumble));
			m_rumble = rumble;
		}
		break;

		case RUMBLE_UPDATE: {
			m_DeviceConfig->FindButtonByIndex(m_max_num_buttons - 1)->UpdateText(m_rumble.c_str());
		}
		break;

		case RUMBLE_TEST: {
			DetectOutput(OUTPUT_TIMEOUT);
		}
		break;

		default:
			break;
	}
}

void InputWindow::DetectOutput(int ms)
{
	if (m_rumble == std::string()) {
		return;
	}
	auto dev = g_InputDeviceManager.FindDevice(m_host_dev);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, dev, ms]() {
			HWND hwnd_rumble_test = GetDlgItem(m_hwnd_rumble, IDC_RUMBLE_TEST);
			SendMessage(hwnd_rumble_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("..."));
			EnableWindow(m_hwnd_rumble, FALSE);
			auto outputs = dev->GetOutputs();
			for (const auto out : outputs) {
				if (out->GetName() == m_rumble) {
					out->SetState(1.0, 1.0);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			SendMessage(hwnd_rumble_test, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Test"));
			EnableWindow(m_hwnd_rumble, TRUE);
		}).detach();
	}
}
