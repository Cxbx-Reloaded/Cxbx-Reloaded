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

#pragma once

#include "windows.h"
#include <string>

#define XBOX_CTRL_NUM_BUTTONS 25


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
