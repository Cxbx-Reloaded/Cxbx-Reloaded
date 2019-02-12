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
			unsigned int dev_type = lParam >> 8;

			// Ensure that port_num is a valid xbox port
			assert(port_num < 5 && port_num > 0);

			// Ensure that the controller type is valid
			assert(dev_type == XBOX_INPUT_DEVICE::MS_CONTROLLER_DUKE ||
				dev_type == XBOX_INPUT_DEVICE::MS_CONTROLLER_S);

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
	}
	return FALSE;
}
