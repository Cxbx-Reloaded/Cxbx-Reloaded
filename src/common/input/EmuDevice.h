#pragma once

#include "Button.h"
#include "common\util\CxbxUtil.h"

#define XBOX_CTRL_NUM_BUTTONS 25

extern const char* button_xbox_ctrl_names[XBOX_CTRL_NUM_BUTTONS][2];
extern int dev_num_buttons[to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX)];


/* Represents the guest device currently being configured in the gui */
class EmuDevice
{
	public:
		EmuDevice(int type, HWND hwnd);
		~EmuDevice();
		Button* FindButtonById(int id);
		Button* FindButtonByIndex(int index);
		void BindXInput();
		void ClearButtons();


	private:
		std::vector<Button*> m_buttons;
		HWND m_hwnd;
};
