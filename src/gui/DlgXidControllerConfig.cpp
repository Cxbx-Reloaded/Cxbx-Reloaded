// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->gui->DlgXidControllerConfig.cpp
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

#include "Windows.h"
#include "ResCxbx.h"
#include "common\util\CxbxUtil.h"
#include "input\InputWindow.h"

#define INPUT_TIMEOUT 5000


INT_PTR CALLBACK DlgXidControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			int port_num = lParam & 7;
			int dev_type = lParam >> 8;

			// Ensure that port_num is a valid xbox port
			assert(port_num < 5 && port_num > 0);

			// Ensure that the controller type is valid
			assert(dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE) ||
				dev_type == to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S));

			g_InputWindow = new InputWindow;
			g_InputWindow->Initialize(hWndDlg, port_num, dev_type);
		}
		break;

		case WM_CLOSE:
		{
			delete g_InputWindow;
			g_InputWindow = nullptr;
			EndDialog(hWndDlg, wParam);
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_XID_PROFILE_LOAD:
				case IDC_XID_PROFILE_SAVE:
				case IDC_XID_PROFILE_DELETE: {
					if (HIWORD(wParam) == BN_CLICKED) {
						char name[50];
						SendMessage(GetDlgItem(hWndDlg, IDC_XID_PROFILE_NAME), WM_GETTEXT,
							sizeof(name), reinterpret_cast<LPARAM>(name));
						int command = (LOWORD(wParam) == IDC_XID_PROFILE_LOAD) ? PROFILE_LOAD :
							(LOWORD(wParam) == IDC_XID_PROFILE_SAVE) ? PROFILE_SAVE : PROFILE_DELETE;
						g_InputWindow->UpdateProfile(std::string(name), command);
					}
				}
				break;

				case IDC_XINP_DEFAULT: {
					if (HIWORD(wParam) == BN_CLICKED) {
						g_InputWindow->BindXInput();
					}
					break;
				}

				case IDC_XID_CLEAR: {
					if (HIWORD(wParam) == BN_CLICKED) {
						g_InputWindow->ClearBindings();
					}
				}
				break;

				case IDC_REFRESH_DEVICES: {
					if (HIWORD(wParam) == BN_CLICKED) {
						g_InputWindow->UpdateDeviceList();
					}
				}
				break;

				case IDC_SET_LMOTOR:
				case IDC_SET_RMOTOR:
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
						char name[50];
						SendMessage(GetDlgItem(hWndDlg, IDC_DEVICE_LIST), WM_GETTEXT,
							sizeof(name), reinterpret_cast<LPARAM>(name));
						g_InputWindow->BindButton(LOWORD(wParam), name, INPUT_TIMEOUT);
					}
				}
				break;
			}
		}
		break;

	}
	return FALSE;
}
