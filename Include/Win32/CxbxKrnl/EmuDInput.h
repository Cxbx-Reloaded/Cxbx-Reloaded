// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDInput.h
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
#ifndef EMUDINPUT_H
#define EMUDINPUT_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// ******************************************************************
// * func: EmuDInputInit
// ******************************************************************
extern bool EmuDInputInit();

// ******************************************************************
// * func: EmuDInputCleanup
// ******************************************************************
extern void EmuDInputCleanup();

// ******************************************************************
// * func: EmuDInputPoll
// ******************************************************************
extern void EmuDInputPoll(PXINPUT_STATE Controller);

// ******************************************************************
// * offsets into analog button array
// ******************************************************************
#define XINPUT_GAMEPAD_A			    0
#define XINPUT_GAMEPAD_B			    1
#define XINPUT_GAMEPAD_X			    2
#define XINPUT_GAMEPAD_Y			    3
#define XINPUT_GAMEPAD_BLACK		    4
#define XINPUT_GAMEPAD_WHITE		    5
#define XINPUT_GAMEPAD_LEFT_TRIGGER		6
#define XINPUT_GAMEPAD_RIGHT_TRIGGER	7

// ******************************************************************
// * masks for digital buttons
// ******************************************************************
#define XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XINPUT_GAMEPAD_START            0x00000010
#define XINPUT_GAMEPAD_BACK             0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

#endif
