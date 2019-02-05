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
// *  (c) 2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

//#define LOG_PREFIX CXBXR_MODULE::?

#include <windows.h>

#include "Cxbx.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "gui/resource/ResCxbx.h"

#include "common\IPCHybrid.hpp"
#include "EmuShared.h"
#include "common\Settings.hpp"
#include "Logging.h"


void ipc_send_gui_update(IPC_UPDATE_GUI command, const unsigned int value)
{
	// Don't send if kernel process didn't receive hwnd from GUI process.
	if (CxbxKrnl_hEmuParent == nullptr) {
		return;
	}

	UINT cmdParam;
	// Bind requested command before send to GUI process
	switch (command) {
		case IPC_UPDATE_GUI::LLE_FLAGS:
			cmdParam = ID_GUI_STATUS_LLE_FLAGS;
			break;

		case IPC_UPDATE_GUI::XBOX_LED_COLOUR:
			cmdParam = ID_GUI_STATUS_XBOX_LED_COLOUR;
			break;

		case IPC_UPDATE_GUI::LOG_ENABLED:
			cmdParam = ID_GUI_STATUS_LOG_ENABLED;
			break;

		case IPC_UPDATE_GUI::KRNL_IS_READY:
			cmdParam = ID_GUI_STATUS_KRNL_IS_READY;
			break;

		default:
			cmdParam = 0;
			break;
	}

	// Verify command parameter is valid
	if (cmdParam != 0) {
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, MAKEWPARAM(WM_COMMAND, cmdParam), value);
	}
}

void ipc_send_kernel_update(IPC_UPDATE_KERNEL command, const unsigned int value, const unsigned int hwnd)
{
	// Don't send if GUI process didn't create kernel process.
	if (hwnd == NULL) {
		return;
	}

	UINT cmdParam;
	// Bind requested command before send to kernel process
	switch (command) {
		case IPC_UPDATE_KERNEL::CONFIG_LOGGING_SYNC:
			cmdParam = ID_SYNC_CONFIG_LOGGING;
			break;

		default:
			cmdParam = 0;
			break;
	}

	// Verify command parameter is valid
	if (cmdParam != 0) {
		SendMessage(reinterpret_cast<HWND>(hwnd), WM_COMMAND, MAKEWPARAM(cmdParam, 0), value);
	}
}

