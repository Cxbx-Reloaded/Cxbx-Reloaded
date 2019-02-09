#pragma once

#include "windows.h"
#include <string>


/* Represents the gui buttons of the xbox device currently being configured */
class Button
{
	public:
		Button(const char* name, int id, int index, HWND hwnd) : m_name(name), m_id(id),
			m_index(index), m_button_hwnd(GetDlgItem(hwnd, m_id)) {};
		void EnableControl(bool enable);
		void UpdateText(const char* text);
		void GetText(char* const text, size_t size);
		std::string GetName() { return m_name; }
		int GetId() { return m_id; }
		int GetIndex() { return m_index; }


	private:
		std::string m_name;
		int m_id;
		int m_index;
		HWND m_button_hwnd;
};
