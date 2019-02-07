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
#include "DlgXidControllerConfig.h"
#include "ResCxbx.h"
#include "common\util\CxbxUtil.h"
#include "input\BoundDevice.h"


// Windows dialog procedure for the Duke/S controller menu
INT_PTR CALLBACK DlgXidControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int g_ControllerType = to_underlying(XBOX_INPUT_DEVICE::DEVICE_INVALID);


INT_PTR CALLBACK DlgXidControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			// Ensure that LPARAM is a valid xbox port
			assert(lParam < 5 && lParam > 0);

			// Ensure that the controller type is valid
			assert(g_ControllerType == XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE ||
				g_ControllerType == XBOX_INPUT_DEVICE::MS_CONTROLLER_S);

			// Set window icon
			SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));

			// Set window title
			std::string title("Xbox ");
			switch (g_ControllerType)
			{
				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE): {
					title += "Controller Duke at port ";
				}
				break;

				case to_underlying(XBOX_INPUT_DEVICE::MS_CONTROLLER_S): {
					title += "Controller S at port ";
				}
				break;

				default:
					break;
			}
			SendMessage(hWndDlg, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>((title + std::to_string(static_cast<long>(lParam))).c_str()));

			// Enumerate devices
			g_InputDeviceManager->RefreshDevices();

			// Populate device list
			HWND hHandle = GetDlgItem(hWndDlg, IDC_DEVICE_LIST);
			std::vector<std::string> dev_list = g_InputDeviceManager->GetDeviceList();
			for (const auto& str : dev_list) {
				SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
			}
			if (dev_list.empty()) {
				SendMessage(hHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("No devices detected"));
			}
			SendMessage(hHandle, CB_SETCURSEL, 0, 0);
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
