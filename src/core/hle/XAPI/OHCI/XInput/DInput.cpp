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
// *   src->core->HLE->XAPI->OHCI->XInput->DInput.cpp
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::DINP

#include "core\kernel\support\Emu.h"
#include "core\kernel\support\EmuXTL.h"
#include "EmuShared.h"
#include "common\Win32\DInputController.h"
#include "Logging.h"

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static DInputController g_DInputController;

// ******************************************************************
// * XTL::EmuDInputInit
// ******************************************************************
bool XTL::EmuDInputInit()
{
    g_EmuShared->GetControllerDInputSettings(&g_DInputController.m_settings);

    g_DInputController.ListenBegin(g_hEmuWindow);

    if(g_DInputController.HasError())
        return false;

    return true;
}

// ******************************************************************
// * XTL::EmuDInputCleanup
// ******************************************************************
void XTL::EmuDInputCleanup()
{
    g_DInputController.ListenEnd();
}

//emulated dwPacketNumber for DirectInput controller
DWORD dwPacketNumber_DirectInput = 0;

// ******************************************************************
// * XTL::EmuDInputPoll
// ******************************************************************
void XTL::EmuDInputPoll(XTL::PX_XINPUT_STATE pXboxController)
{
    g_DInputController.ListenPoll(pXboxController);
	//increment of emulated PacketNumber and report back to Controller.
	dwPacketNumber_DirectInput++;
	pXboxController->dwPacketNumber = dwPacketNumber_DirectInput;

    if(g_DInputController.HasError())
        MessageBox(NULL, g_DInputController.GetError().c_str(), "Cxbx-Reloaded [*UNHANDLED!*]", MB_OK);  // TODO: Handle this!

    return;
}
