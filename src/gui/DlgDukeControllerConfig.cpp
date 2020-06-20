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

#define LOG_PREFIX CXBXR_MODULE::GUI

#include "Windows.h"
#include "resource/ResCxbx.h"
#include "input\InputWindow.h"
#include "gui\DlgInputConfig.h"
#include "common/Logging.h"


static INT_PTR CALLBACK DlgRumbleConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK DlgXidControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		int port_num = lParam & 0xFF;
		int dev_type = (lParam & 0xFF00) >> 8;

		// Ensure that port_num is a valid xbox port
		assert(port_num >= PORT_1 && port_num <= PORT_4);

		// Ensure that the controller type is valid
		assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE) ||
			dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S));

		g_InputWindow = new InputWindow;
		g_InputWindow->Initialize(hWndDlg, port_num, dev_type);
	}
	break;

	case WM_CLOSE:
	{
		if (g_InputWindow->IsProfileSaved()) {
			delete g_InputWindow;
			g_InputWindow = nullptr;
			EndDialog(hWndDlg, wParam);
		}
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_DEVICE_LIST: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				g_InputWindow->UpdateCurrentDevice();
			}
		}
		break;

		case IDC_XID_PROFILE_NAME: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				char name[50];
				HWND hwnd = GetDlgItem(hWndDlg, IDC_XID_PROFILE_NAME);
				LRESULT str_idx = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
				if (str_idx != CB_ERR) {
					SendMessage(hwnd, CB_GETLBTEXT, str_idx, reinterpret_cast<LPARAM>(name));
					g_InputWindow->UpdateProfile(std::string(name), PROFILE_LOAD);
				}
			}
		}
		break;

		case IDC_XID_PROFILE_SAVE:
		case IDC_XID_PROFILE_DELETE: {
			if (HIWORD(wParam) == BN_CLICKED) {
				char name[50];
				SendMessage(GetDlgItem(hWndDlg, IDC_XID_PROFILE_NAME), WM_GETTEXT,
					sizeof(name), reinterpret_cast<LPARAM>(name));
				g_InputWindow->UpdateProfile(std::string(name), (LOWORD(wParam) == IDC_XID_PROFILE_SAVE) ? PROFILE_SAVE : PROFILE_DELETE);
			}
		}
		break;

		case IDC_XID_DEFAULT: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->BindDefault();
			}
		}
		break;

		case IDC_XID_CLEAR: {
			if (HIWORD(wParam) == BN_CLICKED) {
				if (PopupQuestionEx(hWndDlg, LOG_LEVEL::WARNING, PopupButtons::YesNo, PopupReturn::No, "Are you sure you want to remove all button bindings?") == PopupReturn::Yes) {
					g_InputWindow->ClearBindings();
				}
			}
		}
		break;

		case IDC_REFRESH_DEVICES: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->UpdateDeviceList();
			}
		}
		break;

		case IDC_SET_X:
		case IDC_SET_Y:
		case IDC_SET_A:
		case IDC_SET_B:
		case IDC_SET_WHITE:
		case IDC_SET_BLACK:
		case IDC_SET_LTRIGGER:
		case IDC_SET_RTRIGGER:
		case IDC_SET_LTHUMB:
		case IDC_SET_RTHUMB:
		case IDC_SET_START:
		case IDC_SET_BACK:
		case IDC_SET_DPAD_LEFT:
		case IDC_SET_DPAD_RIGHT:
		case IDC_SET_DPAD_UP:
		case IDC_SET_DPAD_DOWN:
		case IDC_SET_LEFT_POSY:
		case IDC_SET_LEFT_NEGX:
		case IDC_SET_LEFT_NEGY:
		case IDC_SET_LEFT_POSX:
		case IDC_SET_RIGHT_POSY:
		case IDC_SET_RIGHT_NEGY:
		case IDC_SET_RIGHT_NEGX:
		case IDC_SET_RIGHT_POSX: {
			if (HIWORD(wParam) == BN_CLICKED) {
				g_InputWindow->BindButton(LOWORD(wParam));
			}
		}
		break;

		case IDC_SET_MOTOR: {
			if (HIWORD(wParam) == BN_CLICKED) {
				// Show rumble dialog box
				DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_RUMBLE_CFG), hWndDlg, DlgRumbleConfigProc);
				g_InputWindow->UpdateRumble(RUMBLE_UPDATE);
			}
		}
		break;
		}
	}
	break;
	}
	return FALSE;
}

INT_PTR CALLBACK DlgRumbleConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		g_InputWindow->InitRumble(hWndDlg);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_RUMBLE_LIST: {
			g_InputWindow->UpdateRumble(RUMBLE_SET);
		}
		break;

		case IDC_RUMBLE_TEST: {
			g_InputWindow->UpdateRumble(RUMBLE_TEST);
		}
		break;
		}
	}
	break;

	case WM_CLOSE:
	{
		EndDialog(hWndDlg, wParam);
	}
	break;
	}
	return FALSE;
}

LRESULT CALLBACK ProfileNameSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, ProfileNameSubclassProc, uIdSubclass);
	}
	break;

	// Override the default system behaviour and process WM_CHAR messages ourselves
	case WM_GETDLGCODE: {
		if (lParam) {
			LPMSG lpmsg = reinterpret_cast<LPMSG>(lParam);
			if (lpmsg->message == WM_CHAR) {
				return DLGC_WANTCHARS;
			}
		}
	}
	break;

	case WM_CHAR:
	{
		// Make sure that we only allow printable ascii characters and some special keys to delete characters
		if (!((wParam >= ' ' && wParam <= '~')
			|| wParam == VK_CANCEL
			|| wParam == VK_CLEAR
			|| wParam == VK_DELETE
			|| wParam == VK_BACK))
		{
			return FALSE;
		}
	}
	break;

	// Don't allow pasting operations, they can be used to bypass the filtering done in WM_CHAR
	case WM_PASTE:
		return FALSE;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
