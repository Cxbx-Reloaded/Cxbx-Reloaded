#pragma once

#include "InputManager.h"


class InputWindow
{
	public:
		// wait for an input from the specified device
		InputDevice::Input* Detect(InputDevice* Device);
		// assign the binding to the control
		void BindButton(int ControlID, std::string DeviceName);
		// construct emu device to configurate
		void ConstructEmuDevice(int Type, HWND hwnd);
		// destroy emu device
		void DestroyEmuDevice();


	private:
		// xbox device under configuration
		EmuDevice* m_DeviceConfig;
};
