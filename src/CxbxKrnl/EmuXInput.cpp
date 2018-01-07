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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "XINP"

#include "Emu.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "EmuXInput.h"

#include <xinput.h>


// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XINPUT_STATE		g_Controller;
static XINPUT_VIBRATION g_Vibration;
static BOOL				g_bXInputInitialized = FALSE;


// ******************************************************************
// * patch: XInputPCPoll
// ******************************************************************
void XTL::EmuXInputPCPoll( XTL::PXINPUT_STATE Controller )
{
	//
	// Get the PC's XInput values
	// 

	if( XInputGetState( 0, &g_Controller ) != ERROR_SUCCESS )
		return;

	//
	// Now convert those values to Xbox XInput
	//
	// Analog Sticks
	Controller->Gamepad.sThumbLX = g_Controller.Gamepad.sThumbLX;
	Controller->Gamepad.sThumbLY = g_Controller.Gamepad.sThumbLY;
	Controller->Gamepad.sThumbRX = g_Controller.Gamepad.sThumbRX;
	Controller->Gamepad.sThumbRY = g_Controller.Gamepad.sThumbRY;

	// Analog Buttons
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_A] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_B] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_X] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_Y] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_WHITE] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_BLACK] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 255 : 0;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_LEFT_TRIGGER] = g_Controller.Gamepad.bLeftTrigger;
	Controller->Gamepad.bAnalogButtons[XB_XINPUT_GAMEPAD_RIGHT_TRIGGER] = g_Controller.Gamepad.bRightTrigger;

	// Digital Buttons
	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_BACK;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_BACK;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_START;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_START;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_LEFT_THUMB;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_LEFT_THUMB;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_RIGHT_THUMB;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_RIGHT_THUMB;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_DPAD_UP;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_DPAD_UP;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_DPAD_DOWN;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_DPAD_DOWN;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_DPAD_LEFT;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_DPAD_LEFT;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
		Controller->Gamepad.wButtons |= XB_XINPUT_GAMEPAD_DPAD_RIGHT;
	} else {
		Controller->Gamepad.wButtons &= ~XB_XINPUT_GAMEPAD_DPAD_RIGHT;
	}
}
