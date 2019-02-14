#pragma once

#include "Button.h"

#define XBOX_CTRL_NUM_BUTTONS 26

extern const char* button_xbox_ctrl_names[XBOX_CTRL_NUM_BUTTONS][2];


/* Represents the xbox device currently being configured in the gui */
class EmuDevice
{
	public:
		EmuDevice(int type, HWND hwnd);
		~EmuDevice();
		std::string GetName() { return m_name; }
		Button* FindButtonById(int id);
		Button* FindButtonByIndex(int index);
		void BindXInput();


	private:
		std::vector<Button*> m_buttons;
		std::string m_name;
		HWND m_hwnd;
};
