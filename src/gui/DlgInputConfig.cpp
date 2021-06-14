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

#include "windows.h"
#include "controllers/DlgDukeControllerConfig.h"
#include "controllers/DlgSBControllerConfig.h"
#include "resource/ResCxbx.h"
#include "input\InputManager.h"
#include "Logging.h"
#include "Settings.hpp"
#include "common\IPCHybrid.hpp"
#include "EmuShared.h"


// Windows dialog procedure for the input menu
static INT_PTR CALLBACK DlgInputConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// Window procedure of the subclass
LRESULT CALLBACK WindowsCtrlSubProcNumericFilter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
HWND g_ChildWnd = NULL;
static bool g_bHasOptChanges = false;
static bool g_bHasInputChanges[4] = { false, false, false, false };


void SyncInputSettings(int port_num, int dev_type, bool is_opt)
{
	if (g_ChildWnd) {
		if (!is_opt) {
			// Sync updated input to kernel process to use run-time settings.
			g_EmuShared->SetInputDevTypeSettings(&g_Settings->m_input_port[port_num].Type, port_num);
			g_EmuShared->SetInputSlotTypeSettings(&g_Settings->m_input_port[port_num].TopSlotType, port_num, SLOT_TOP);
			g_EmuShared->SetInputSlotTypeSettings(&g_Settings->m_input_port[port_num].BottomSlotType, port_num, SLOT_BOTTOM);

			if (dev_type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
				std::string dev_name = g_Settings->m_input_port[port_num].DeviceName;
				std::string profile_name = g_Settings->m_input_port[port_num].ProfileName;

				g_EmuShared->SetInputDevNameSettings(dev_name.c_str(), port_num);
				auto it = std::find_if(g_Settings->m_input_profiles[dev_type].begin(),
					g_Settings->m_input_profiles[dev_type].end(), [&profile_name](const auto &profile) {
						if (profile.ProfileName == profile_name) {
							return true;
						}
						return false;
					});
				if (it != g_Settings->m_input_profiles[dev_type].end()) {
					char controls_name[HIGHEST_NUM_BUTTONS][HOST_BUTTON_NAME_LENGTH];
					for (int index = 0; index < dev_num_buttons[dev_type]; index++) {
						strncpy(controls_name[index], it->ControlList[index].c_str(), 30);
					}
					g_EmuShared->SetInputBindingsSettings(controls_name, dev_num_buttons[dev_type], port_num);
				}
			}
		}
		else {
			g_EmuShared->SetInputGeneralSettings(&g_Settings->m_input_general);
			port_num = PORT_INVALID;
		}
#if 0 // lle usb
		ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_INPUT_SYNC, PORT_DEC(Gui2XboxPortArray[port_num]),
			reinterpret_cast<std::uintptr_t>(g_ChildWnd));
#else
		ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_INPUT_SYNC, port_num, reinterpret_cast<std::uintptr_t>(g_ChildWnd));
#endif
	}
}

void UpdateInputOpt(HWND hwnd)
{
	char buffer[30];
	SendMessage(GetDlgItem(hwnd, IDC_MOUSE_RANGE), WM_GETTEXT, 30, reinterpret_cast<LPARAM>(buffer));
	g_Settings->m_input_general.MoAxisRange = std::stol(buffer);
	SendMessage(GetDlgItem(hwnd, IDC_WHEEL_RANGE), WM_GETTEXT, 30, reinterpret_cast<LPARAM>(buffer));
	g_Settings->m_input_general.MoWheelRange = std::stol(buffer);
	LRESULT ret = SendMessage(GetDlgItem(hwnd, IDC_IGNORE_KBMO_UNFOCUS), BM_GETCHECK, 0, 0);
	g_Settings->m_input_general.IgnoreKbMoUnfocus = (ret == BST_CHECKED);
}

void ShowInputConfig(HWND hwnd, HWND ChildWnd)
{
	g_InputDeviceManager.Initialize(true, hwnd);
	g_ChildWnd = ChildWnd;

	// Show dialog box
	DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_INPUT_CFG), hwnd, DlgInputConfigProc);
}

INT_PTR CALLBACK DlgInputConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		// Set window icon
		SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

		for (int i = 0, j = 0; i != 4; i++) {
			HWND hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_PORT1 + i);
			for (auto dev_type : input_support_list) {
				LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(GetInputDeviceName(to_underlying(dev_type)).c_str()));
				SendMessage(hHandle, CB_SETITEMDATA, index, to_underlying(dev_type));
				if (g_Settings->m_input_port[i].Type == to_underlying(dev_type)) {
					SendMessage(hHandle, CB_SETCURSEL, index, 0);
					if (g_Settings->m_input_port[i].Type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
						EnableWindow(GetDlgItem(hWndDlg, IDC_CONFIGURE_PORT1 + i), FALSE);
					}
				}
				j++;
			}
			j = 0;
		}

		for (auto i : { IDC_MOUSE_RANGE, IDC_WHEEL_RANGE }) {
			HWND hEditControlArray = GetDlgItem(hWndDlg, i);
			SetWindowSubclass(hEditControlArray, WindowsCtrlSubProcNumericFilter, i, 0);
			SendMessage(hEditControlArray, EM_SETLIMITTEXT, 6, 0);
			SendMessage(hEditControlArray, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((i == IDC_MOUSE_RANGE) ?
				std::to_string(g_Settings->m_input_general.MoAxisRange).c_str() :
				std::to_string(g_Settings->m_input_general.MoWheelRange).c_str()));
		}
		SendMessage(GetDlgItem(hWndDlg, IDC_IGNORE_KBMO_UNFOCUS), BM_SETCHECK, static_cast<WPARAM>(g_Settings->m_input_general.IgnoreKbMoUnfocus), 0);

		// Reset option/input changes flag
		g_bHasOptChanges = false;
		g_bHasInputChanges[0] = false;
		g_bHasInputChanges[1] = false;
		g_bHasInputChanges[2] = false;
		g_bHasInputChanges[3] = false;
	}
	break;

	case WM_CLOSE:
	{
		if (g_bHasOptChanges) {
			UpdateInputOpt(hWndDlg);
			g_InputDeviceManager.UpdateOpt(true);
			SyncInputSettings(0, 0, true);
		}

		for (int port = PORT_1; port <= PORT_4; port++) {
			if (g_bHasInputChanges[port]) {
				HWND hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_PORT1 + port);
				int DeviceType = SendMessage(hHandle, CB_GETITEMDATA, SendMessage(hHandle, CB_GETCURSEL, 0, 0), 0);
				g_Settings->m_input_port[port].Type = DeviceType;
				SyncInputSettings(port, DeviceType, false);
			}
		}

		g_InputDeviceManager.Shutdown();
		g_ChildWnd = NULL;
		EndDialog(hWndDlg, wParam);
	}
	break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_CONFIGURE_PORT1:
		case IDC_CONFIGURE_PORT2:
		case IDC_CONFIGURE_PORT3:
		case IDC_CONFIGURE_PORT4: {
			if (HIWORD(wParam) == BN_CLICKED) {
				int port =
					LOWORD(wParam) == IDC_CONFIGURE_PORT1 ? 0 :
					LOWORD(wParam) == IDC_CONFIGURE_PORT2 ? 1 :
					LOWORD(wParam) == IDC_CONFIGURE_PORT3 ? 2 :
					LOWORD(wParam) == IDC_CONFIGURE_PORT4 ? 3 :
					-1;
				assert(port != -1);
				HWND hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_PORT1 + port);
				int DeviceType = SendMessage(hHandle, CB_GETITEMDATA, SendMessage(hHandle, CB_GETCURSEL, 0, 0), 0);
				assert(DeviceType > to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
					DeviceType < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX));

				switch (DeviceType)
				{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): 
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S):
				case to_underlying(XBOX_INPUT_DEVICE::ARCADE_STICK): {
					DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_XID_DUKE_CFG), hWndDlg, DlgXidControllerConfigProc,
						(DeviceType << 8) | port);
				}
				break;

				case to_underlying(XBOX_INPUT_DEVICE::STEEL_BATTALION_CONTROLLER): {
					DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_SBC_CFG), hWndDlg, DlgSBControllerConfigProc,
						(DeviceType << 8) | port);
				}
				break;

				default:
					break;
				}

				g_bHasInputChanges[port] = true;
			}
		}
		break;

		case IDC_DEVICE_PORT1:
		case IDC_DEVICE_PORT2:
		case IDC_DEVICE_PORT3:
		case IDC_DEVICE_PORT4:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				LRESULT dev_type = SendMessage(GetDlgItem(hWndDlg, LOWORD(wParam)), CB_GETITEMDATA,
					SendMessage(GetDlgItem(hWndDlg, LOWORD(wParam)), CB_GETCURSEL, 0, 0), 0);
				int port =
					LOWORD(wParam) == IDC_DEVICE_PORT1 ? 0 :
					LOWORD(wParam) == IDC_DEVICE_PORT2 ? 1 :
					LOWORD(wParam) == IDC_DEVICE_PORT3 ? 2 :
					LOWORD(wParam) == IDC_DEVICE_PORT4 ? 3 :
					-1;
				assert(port != -1);
				if (dev_type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
					EnableWindow(GetDlgItem(hWndDlg, IDC_CONFIGURE_PORT1 + port), FALSE);
				}
				else {
					EnableWindow(GetDlgItem(hWndDlg, IDC_CONFIGURE_PORT1 + port), TRUE);
				}

				g_bHasInputChanges[port] = true;
			}
		}
		break;

		case IDC_MOUSE_RANGE:
		case IDC_WHEEL_RANGE:
		{
			if (HIWORD(wParam) == EN_CHANGE) {
				g_bHasOptChanges = true;
			}
		}
		break;

		case IDC_IGNORE_KBMO_UNFOCUS:
		{
			if (HIWORD(wParam) == BN_CLICKED) {
				g_bHasOptChanges = true;
			}
		}
		break;

		}
	}
	break;
	}
	return FALSE;
}

LRESULT CALLBACK WindowsCtrlSubProcNumericFilter(HWND hWnd, UINT uMsg, WPARAM wParam,
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	// Remove the window subclass when this window is destroyed
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, WindowsCtrlSubProcNumericFilter, uIdSubclass);
		break;

	// Override the default system behaviour and process WM_CHAR ourselves
	case WM_GETDLGCODE:
		if (lParam) {
			LPMSG lpmsg = reinterpret_cast<LPMSG>(lParam);
			if (lpmsg->message == WM_CHAR) {
				return DLGC_WANTCHARS;
			}
		}
		break;

	case WM_CHAR:
	{
		// Make sure we only allow decimal numbers and some special keys to delete characters
		if (!((wParam >= '0' && wParam <= '9')
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
