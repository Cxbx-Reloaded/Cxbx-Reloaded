#pragma once

#include "InputManager.h"
#include "common\Settings.hpp"

#define PROFILE_LOAD   1
#define PROFILE_SAVE   2
#define PROFILE_DELETE 3
#define RUMBLE_SET     4
#define RUMBLE_UPDATE  5
#define RUMBLE_TEST    6


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
		bool SaveProfile(std::string& name);
		void DeleteProfile(std::string& name);
		void LoadDefaultProfile();
		void SaveBindingsToDevice();

		// guest device under configuration
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
