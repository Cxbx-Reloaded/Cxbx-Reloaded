#include "InputWindow.h"
#include "..\..\gui\ResCxbx.h"
#include "common\IPCHybrid.hpp"
#include "EmuShared.h"
#include <future>


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
	if (dev_list.empty()) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("No devices detected"));
	}
	SendMessage(m_hwnd_device_list, CB_SETCURSEL, 0, 0);
}

InputDevice::Input* InputWindow::Detect(InputDevice* const Device, int ms)
{
	using namespace std::chrono;

	auto now = system_clock::now();
	auto timeout = now + milliseconds(ms);
	std::vector<InputDevice::Input*>::const_iterator i = Device->GetInputs().begin(),
		e = Device->GetInputs().end(), b = i;

	while (now <= timeout) {
		Device->UpdateInput();
		for (; i != e; i++) {
			if ((*i)->GetState() > (1 - INPUT_DETECT_THRESHOLD)) {
				return *i; // user pressed a button
			}
		}
		std::this_thread::sleep_for(milliseconds(10));
		now += milliseconds(10);
		i = b;
	}

	return nullptr; // no input
}

void InputWindow::BindButton(int ControlID, std::string DeviceName, int ms)
{
	auto dev = g_InputDeviceManager.FindDevice(DeviceName);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, dev, ControlID, ms]() {
			char current_text[50];
			Button* xbox_button = m_DeviceConfig->FindButtonById(ControlID);
			xbox_button->GetText(current_text, sizeof(current_text));
			xbox_button->UpdateText("...");
			EnableWindow(m_hwnd_window, FALSE);
			std::future<InputDevice::Input*> fut = std::async(std::launch::async, &InputWindow::Detect, this, dev.get(), ms);
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
	char device_name[50];
	SendMessage(m_hwnd_device_list, WM_GETTEXT, sizeof(device_name), reinterpret_cast<LPARAM>(device_name));
	if (std::strncmp(device_name, "XInput", std::strlen("XInput")) == 0) {
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
	char device_name[50];
	SendMessage(m_hwnd_device_list, WM_GETTEXT, sizeof(device_name), reinterpret_cast<LPARAM>(device_name));
	if (std::strncmp(device_name, "No devices detected", std::strlen("No devices detected")) == 0) {
		return false;
	}
	DeleteProfile(name);
	Settings::s_input_profiles profile;
	profile.Type = m_dev_type;
	profile.ProfileName = name;
	profile.DeviceName = device_name;
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
	char device_name[50], profile_name[50];
	SendMessage(m_hwnd_profile_list, WM_GETTEXT, sizeof(profile_name), reinterpret_cast<LPARAM>(profile_name));
	if (!SaveProfile(std::string(profile_name))) {
		return;
	}
	SendMessage(m_hwnd_device_list, WM_GETTEXT, sizeof(device_name), reinterpret_cast<LPARAM>(device_name));

	g_Settings->m_input[m_port_num].DeviceName = device_name;
	g_Settings->m_input[m_port_num].ProfileName = profile_name;

	// Also inform the kernel process if it exists
	if (m_hwnd_krnl) {
		// Sync updated input to kernel process to use run-time settings.
		g_EmuShared->SetInputSettings(&g_Settings->m_input);
		g_EmuShared->SetInputProfileSettings(&g_Settings->m_input_profiles);
		ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_INPUT_SYNC, m_port_num + 1,
			reinterpret_cast<std::uintptr_t>(m_hwnd_krnl));
	}
}
