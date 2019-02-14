#pragma once

#include "windows.h"
#include <string>


/* Represents the gui buttons of the xbox device currently being configured */
class Button
{
	public:
		Button(const char* name, const char* xname, int id, int index, HWND hwnd) : m_name(name),
			m_xinput_button(xname), m_id(id), m_index(index), m_button_hwnd(GetDlgItem(hwnd, m_id)) {};
		void EnableButton(bool enable) const;
		void UpdateText(const char* text) const;
		void UpdateText() const;
		void ClearText() const;
		void GetText(char* const text, size_t size) const;
		std::string GetName() const { return m_name; }
		int GetId() const { return m_id; }
		int GetIndex() const { return m_index; }


	private:
		std::string m_name;
		std::string m_xinput_button;
		int m_id;
		int m_index;
		HWND m_button_hwnd;
};
