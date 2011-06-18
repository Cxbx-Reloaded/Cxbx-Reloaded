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
#define _XBOXKRNL_DEFEXTRN_

#include "Emu.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "EmuXInput.h"

//
// Prevent naming collisions
// 
#undef XINPUT_GAMEPAD_DPAD_UP
#undef XINPUT_GAMEPAD_DPAD_DOWN
#undef XINPUT_GAMEPAD_DPAD_LEFT 
#undef XINPUT_GAMEPAD_DPAD_RIGHT
#undef XINPUT_GAMEPAD_START  
#undef XINPUT_GAMEPAD_BACK      
#undef XINPUT_GAMEPAD_LEFT_THUMB
#undef XINPUT_GAMEPAD_RIGHT_THUMB 
#undef XINPUT_GAMEPAD_LEFT_SHOULDER
#undef XINPUT_GAMEPAD_RIGHT_SHOULDER
#undef XINPUT_GAMEPAD_A 
#undef XINPUT_GAMEPAD_B 
#undef XINPUT_GAMEPAD_X 
#undef XINPUT_GAMEPAD_Y 

#include <xinput.h>


// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XINPUT_STATE		g_Controller;
static XINPUT_VIBRATION g_Vibration;
static BOOL				g_bXInputInitialized = FALSE;


// ******************************************************************
// * func: EmuXInputPoll
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

	Controller->Gamepad.sThumbLX = g_Controller.Gamepad.sThumbLX;
	Controller->Gamepad.sThumbLY = g_Controller.Gamepad.sThumbLY;
	Controller->Gamepad.sThumbRX = g_Controller.Gamepad.sThumbRX;
	Controller->Gamepad.sThumbRY = g_Controller.Gamepad.sThumbRY;

	Controller->Gamepad.wButtons = g_Controller.Gamepad.wButtons & 0x00FF;

	Controller->Gamepad.bAnalogButtons[0] = (g_Controller.Gamepad.wButtons & 0x1000) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[1] = (g_Controller.Gamepad.wButtons & 0x2000) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[2] = (g_Controller.Gamepad.wButtons & 0x4000) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[3] = (g_Controller.Gamepad.wButtons & 0x8000) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[4] = (g_Controller.Gamepad.wButtons & 0x0100) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[5] = (g_Controller.Gamepad.wButtons & 0x0200) ? 1 : 0;
	Controller->Gamepad.bAnalogButtons[6] = g_Controller.Gamepad.bLeftTrigger;
	Controller->Gamepad.bAnalogButtons[7] = g_Controller.Gamepad.bRightTrigger;
}