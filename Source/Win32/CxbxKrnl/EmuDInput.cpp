// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDInput.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#include "EmuD3D8.h"
#include "EmuDInput.h"
#include "EmuShared.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// ******************************************************************
// * exported globals
// ******************************************************************
XBController       g_XBController;

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xapi
{
    #include <EmuXapi.h>
};

// ******************************************************************
// * func: EmuDInputInit
// ******************************************************************
bool EmuDInputInit()
{
    g_EmuShared->GetXBController(&g_XBController);

    g_XBController.ListenBegin(g_EmuWindow);

    if(g_XBController.GetError())
        return false;

    return true;
}

// ******************************************************************
// * func: EmuDInputCleanup
// ******************************************************************
void EmuDInputCleanup()
{
    g_XBController.ListenEnd();

    if(g_XBController.GetError())
        MessageBox(NULL, g_XBController.GetError(), "Cxbx [*UNHANDLED!*]", MB_OK);  // TODO: Handle this!
}

// ******************************************************************
// * func: EmuPollController
// ******************************************************************
void EmuDInputPoll(xapi::PXINPUT_STATE Controller)
{
    g_XBController.ListenPoll(Controller);

    if(g_XBController.GetError())
        MessageBox(NULL, g_XBController.GetError(), "Cxbx [*UNHANDLED!*]", MB_OK);  // TODO: Handle this!

    return;
}