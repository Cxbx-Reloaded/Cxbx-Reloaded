#include "InputWindow.h"
#include "..\..\gui\ResCxbx.h"
#include <future>


InputWindow* g_InputWindow;
static int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)] = {
	CTRL_NUM_BUTTONS,
	0,
	0,
	0,
	0,
	0,
	0,
};


void InputWindow::Initialize(HWND hwnd, int port_num, unsigned int dev_type)
{
	// Save window/device specific variables
	m_hwnd_window = hwnd;
	m_hwnd_device_list = GetDlgItem(m_hwnd_window, IDC_DEVICE_LIST);
	m_dev_type = dev_type;
	m_max_num_buttons = dev_num_buttons[dev_type];
	m_port_num = port_num;

	// Set window icon
	SetClassLong(m_hwnd_window, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

	// Set window title
	std::string title("Xbox ");
	switch (m_dev_type)
	{
		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
			title += "Controller Duke at port ";
		}
		break;

		case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
			title += "Controller S at port ";
		}
		break;

		default:
			break;
	}
	SendMessage(m_hwnd_window, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>((title + std::to_string(port_num)).c_str()));

	SendMessage(m_hwnd_device_list, EM_SETLIMITTEXT, 50, 0);

	// construct emu device
	m_DeviceConfig = new EmuDevice(m_dev_type, m_hwnd_window);

	// Enumerate devices
	UpdateDeviceList();

	// Load currently saved profile for this port/device type
	LoadDefaultProfile();
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
		e = Device->GetInputs().end();

	while (now <= timeout) {
		for (; i != e; i++) {
			if ((*i)->GetState()) {
				return *i; // user pressed a button
			}
		}
		std::this_thread::sleep_for(milliseconds(10));
		now += milliseconds(10);
	}

	return nullptr; // no input
}

void InputWindow::BindButton(int ControlID, std::string DeviceName, int ms)
{
	auto dev = g_InputDeviceManager.FindDevice(DeviceName);
	if (dev != nullptr) {
		// Don't block the message processing loop
		std::thread([this, &dev, ControlID, ms]() {
			char current_text[50];
			Button* xbox_button = m_DeviceConfig->FindButtonById(ControlID);
			xbox_button->GetText(current_text, sizeof(current_text));
			xbox_button->UpdateText("...");
			EnableWindow(m_hwnd_window, FALSE);
			std::future<InputDevice::Input*> fut = std::async(std::launch::async, &InputWindow::Detect, this, dev.get(), ms);
			InputDevice::Input* dev_button = fut.get();
			if (dev_button) {
				dev->SetBindings(xbox_button->GetIndex(), dev_button);
				xbox_button->UpdateText(dev_button->GetName().c_str());
			}
			else {
				xbox_button->UpdateText(current_text);
			}
			EnableWindow(m_hwnd_window, TRUE);
		});
	}
}

InputWindow::ProfileIt InputWindow::FindProfile(std::string& name)
{
	auto it = std::find_if(g_Settings->m_input_profiles[m_dev_type].begin(),
		g_Settings->m_input_profiles[m_dev_type].end(), [&name](const auto& profile) {
		return profile.ProfileName == name;
	});
	return it;
}

void InputWindow::LoadProfile(std::string& name)
{
	ProfileIt profile = FindProfile(name);
	LRESULT dev_str_index;
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	dev_str_index = SendMessage(m_hwnd_device_list, CB_FINDSTRINGEXACT, 1, reinterpret_cast<LPARAM>(profile->DeviceName.c_str));
	if (dev_str_index == CB_ERR) {
		SendMessage(m_hwnd_device_list, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(profile->DeviceName.c_str));
	}
	else {
		SendMessage(m_hwnd_device_list, CB_SETCURSEL, dev_str_index, 0);
	}
	std::for_each(profile->ControlList.begin(), profile->ControlList.end(), [this](const auto& dev_button_str) {
		static int index = 0;
		m_DeviceConfig->FindButtonByIndex(index)->UpdateText(dev_button_str.c_str());
		index++;
	});
}

void InputWindow::SaveProfile(std::string& name)
{
	if (name == std::string()) {
		return;
	}
	std::string device_name = reinterpret_cast<const char*>
		(SendMessage(m_hwnd_device_list, CB_GETITEMDATA, SendMessage(m_hwnd_device_list, CB_GETCURSEL, 0, 0), 0));
	if (device_name == "No devices detected") {
		return;
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
	g_Settings->m_input_profiles[m_dev_type].push_back(std::move(profile));
}

void InputWindow::DeleteProfile(std::string& name)
{
	ProfileIt profile = FindProfile(name);
	if (profile == g_Settings->m_input_profiles[m_dev_type].end()) {
		return;
	}
	g_Settings->m_input_profiles[m_dev_type].erase(profile);
}

void InputWindow::LoadDefaultProfile()
{
	if (g_Settings->m_input[m_port_num].Type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
		return;
	}
	LoadProfile(g_Settings->m_input[m_port_num].ProfileName);
}
