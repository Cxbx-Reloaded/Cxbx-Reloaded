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
#include "DlgDukeControllerConfig.h"
#include "resource/ResCxbx.h"
#include "input\InputManager.h"
#include "Logging.h"
#include "Settings.hpp"
#include "common\IPCHybrid.hpp"
#include "EmuShared.h"


// Windows dialog procedure for the input menu
static INT_PTR CALLBACK DlgInputConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND g_ChildWnd = NULL;


void SyncInputSettings(int port_num, int dev_type)
{
	if (g_ChildWnd) {
		// Sync updated input to kernel process to use run-time settings.
		g_EmuShared->SetInputDevTypeSettings(&g_Settings->m_input[port_num].Type, port_num);

		if (dev_type != to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
			std::string dev_name = g_Settings->m_input[port_num].DeviceName;
			std::string profile_name = g_Settings->m_input[port_num].ProfileName;

			g_EmuShared->SetInputDevNameSettings(dev_name.c_str(), port_num);
			auto it = std::find_if(g_Settings->m_input_profiles[dev_type].begin(),
				g_Settings->m_input_profiles[dev_type].end(), [&profile_name](const auto& profile) {
					if (profile.ProfileName == profile_name) {
						return true;
					}
					return false;
				});
			if (it != g_Settings->m_input_profiles[dev_type].end()) {
				char controls_name[XBOX_CTRL_NUM_BUTTONS][30];
				for (int index = 0; index < dev_num_buttons[dev_type]; index++) {
					strncpy(controls_name[index], it->ControlList[index].c_str(), 30);
				}
				g_EmuShared->SetInputBindingsSettings(controls_name, XBOX_CTRL_NUM_BUTTONS, port_num);
			}
		}
#if 0 // lle usb
		ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_INPUT_SYNC, PORT_DEC(Gui2XboxPortArray[port_num]),
			reinterpret_cast<std::uintptr_t>(g_ChildWnd));
#else
		ipc_send_kernel_update(IPC_UPDATE_KERNEL::CONFIG_INPUT_SYNC, port_num, reinterpret_cast<std::uintptr_t>(g_ChildWnd));
#endif
	}
}

void ShowInputConfig(HWND hwnd, HWND ChildWnd)
{
	g_InputDeviceManager.Initialize(true);
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
		HWND hHandle;

		// Set window icon
		SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_CXBX)));

		for (int i = 0, j = 0; i != 4; i++) {
			hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_PORT1 + i);
			for (auto str : { "None", "MS Controller Duke", "MS Controller S" }) {
				LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str));
				SendMessage(hHandle, CB_SETITEMDATA, index,
					to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) + j);
				if (g_Settings->m_input[i].Type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) + j) {
					SendMessage(hHandle, CB_SETCURSEL, index, 0);
					if (g_Settings->m_input[i].Type == to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
						EnableWindow(GetDlgItem(hWndDlg, IDC_CONFIGURE_PORT1 + i), FALSE);
					}
				}
				j++;
			}
			j = 0;
		}
	}
	break;

	case WM_CLOSE:
	{
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
				switch (DeviceType)
				{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): 
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
					DialogBoxParam(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_XID_DUKE_CFG), hWndDlg, DlgXidControllerConfigProc,
						(DeviceType << 8) | port);
				}
				break;

				default:
					break;
				}
				assert(DeviceType > to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) &&
					DeviceType < to_underlying(XBOX_INPUT_DEVICE::DEVICE_MAX));

				// Also inform the kernel process if it exists
				SyncInputSettings(port, DeviceType);
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

				g_Settings->m_input[port].Type = dev_type;

				// Also inform the kernel process if it exists
				SyncInputSettings(port, dev_type);
			}
		}
		break;
		}
	}
	break;
	}
	return FALSE;
}
