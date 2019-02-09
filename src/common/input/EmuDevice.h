#pragma once

#include "Button.h"


/* Represents the xbox device currently being configured in the gui */
class EmuDevice
{
	public:
		EmuDevice(int type, HWND hwnd);
		~EmuDevice();
		std::string GetName() { return m_name; }
		Button* FindButton(int id);


	private:
		std::vector<Button*> m_buttons;
		std::string m_name;
		HWND m_hwnd;
};
