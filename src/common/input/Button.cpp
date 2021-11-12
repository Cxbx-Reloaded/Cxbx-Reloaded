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

#include "Button.h"
#include "InputWindow.h"
#include "layout_xbox_device.h" // TODO: Needs a better fix for custom input device support.
#include "gui/resource/ResCxbx.h"


void Button::EnableButton(bool enable) const
{
	EnableWindow(m_button_hwnd, enable);
}

void Button::UpdateText(const char* text) const
{
	SendMessage(m_button_hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
}

void Button::ClearText() const
{
	SendMessage(m_button_hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(""));
}

void Button::GetText(char* const text, size_t size) const
{
	SendMessage(m_button_hwnd, WM_GETTEXT, size, reinterpret_cast<LPARAM>(text));
}

void Button::AddTooltip(HWND hwnd, HWND tooltip_hwnd, char *text) const
{
	assert((hwnd != NULL) && (tooltip_hwnd != NULL));

	TOOLINFO tool = { 0 };
	tool.cbSize = sizeof(tool);
	tool.hwnd = hwnd;
	tool.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	tool.uId = reinterpret_cast<UINT_PTR>(m_button_hwnd);
	tool.lpszText = text;
	SendMessage(tooltip_hwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&tool));
}

LRESULT CALLBACK ButtonDukeSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, ButtonDukeSubclassProc, uIdSubclass);
	}
	break;

	case WM_RBUTTONDOWN: {
		Button *button = reinterpret_cast<Button *>(dwRefData);
		if (wParam & MK_SHIFT) {
			static_cast<DukeInputWindow *>(button->GetWnd())->SwapMoCursorAxis(button);
			static_cast<DukeInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), BUTTON_SWAP);
		}
		else if (!(wParam & ~MK_RBUTTON)) {
			button->ClearText();
			static_cast<DukeInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), BUTTON_CLEAR);
			if (button->GetId() == IDC_SET_MOTOR) {
				static_cast<DukeInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), RUMBLE_CLEAR);
			}
		}
	}
	break;

	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ButtonSbcSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, ButtonSbcSubclassProc, uIdSubclass);
	}
	break;

	case WM_RBUTTONDOWN: {
		Button *button = reinterpret_cast<Button *>(dwRefData);
		if (wParam & MK_SHIFT) {
			static_cast<SbcInputWindow *>(button->GetWnd())->SwapMoCursorAxis(button);
			static_cast<SbcInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), BUTTON_SWAP);
		}
		else if (!(wParam & ~MK_RBUTTON)) {
			button->ClearText();
			static_cast<SbcInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), BUTTON_CLEAR);
		}
	}
	break;

	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ButtonLightgunSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, ButtonSbcSubclassProc, uIdSubclass);
	}
	break;

	case WM_RBUTTONDOWN: {
		Button *button = reinterpret_cast<Button *>(dwRefData);
		button->ClearText();
		static_cast<LightgunInputWindow *>(button->GetWnd())->UpdateProfile(std::string(), BUTTON_CLEAR);
	}
	break;

	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
