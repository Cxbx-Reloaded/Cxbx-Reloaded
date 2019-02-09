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
// *   src->gui->DlgInputConfig.cpp
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
#include "DlgInputConfig.h"
#include "DlgXidControllerConfig.h"
#include "ResCxbx.h"
#include "input\InputManager.h"
#include "Logging.h"


// Generic function called from the gui to display the input menu
void ShowInputConfig(HWND hwnd);
// Windows dialog procedure for the input menu
static INT_PTR CALLBACK DlgInputConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void ShowInputConfig(HWND hwnd)
{
	g_InputDeviceManager.Initialize(true);

	// Show dialog box
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INPUT_CFG), hwnd, DlgInputConfigProc);
}

INT_PTR CALLBACK DlgInputConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			HWND hHandle;

			// Set window icon
			SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			for (int i = 0, j = 0; i != 4; i++) {
				hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_PORT1 + i);
				for (auto str : { "None", "MS Controller Duke" }) {
					LRESULT index = SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str));
					SendMessage(hHandle, CB_SETITEMDATA, index,
						to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID) + j);
					j++;
				}
				j = 0;
				SendMessage(hHandle, CB_SETCURSEL, 0, 0); // TODO: hardcoded for now
				EnableWindow(GetDlgItem(hWndDlg, IDC_CONFIGURE_PORT1 + i), FALSE);
			}
		}
		break;

		case WM_CLOSE:
		{
			g_InputDeviceManager.Shutdown();
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
				case IDC_CONFIGURE_PORT4:
				{
					if (HIWORD(wParam) == BN_CLICKED) {
						HWND hHandle = GetDlgItem(hWndDlg, LOWORD(wParam) - 4);
						g_ControllerType = SendMessage(hHandle, CB_GETITEMDATA, SendMessage(hHandle, CB_GETCURSEL, 0, 0), 0);
						switch (g_ControllerType)
						{
							case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE):
							{
								DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_XID_DUKE_CFG), hWndDlg, DlgXidControllerConfigProc, LOWORD(wParam) & 7);
							}
							break;

							default:
								break;
						}
					}
				}
				break;

				case IDC_DEVICE_PORT1:
				case IDC_DEVICE_PORT2:
				case IDC_DEVICE_PORT3:
				case IDC_DEVICE_PORT4:
				{
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						if (SendMessage(reinterpret_cast<HWND>(lParam), CB_GETITEMDATA,
							SendMessage(reinterpret_cast<HWND>(lParam), CB_GETCURSEL, 0, 0), 0)
							== to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID)) {
							EnableWindow(GetDlgItem(hWndDlg, LOWORD(wParam) + 4), FALSE);
						}
						else {
							EnableWindow(GetDlgItem(hWndDlg, LOWORD(wParam) + 4), TRUE);
						}
					}
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}
