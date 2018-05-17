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
static XINPUT_STATE		g_Controller;//global controller input state for host.
static BOOL				g_bXInputInitialized = FALSE;

//
//
//
DWORD XTL::XInputGamepad_Connected(void)
{
	DWORD dwResult;
	DWORD gamepad_connected = 0;
	for (DWORD i = 0; i< 4; i++)
	{
		ZeroMemory(&g_Controller, sizeof(XINPUT_STATE));

		// query each port for gamepad state
		dwResult = XInputGetState(i, &g_Controller);
		
		//success means gamepad is connected
		if (dwResult == ERROR_SUCCESS)
		{
			gamepad_connected++;
		}
	}
	return gamepad_connected;
}
// ******************************************************************
// * patch: XInputPCPoll
// ******************************************************************
void XTL::EmuXInputPCPoll( DWORD dwPort,XTL::PX_XINPUT_STATE pXboxController )
{
	//
	// Get the PC's XInput values
	// 

	if( XInputGetState( dwPort, &g_Controller ) != ERROR_SUCCESS )
		return;

	//Packet# must be updated to trigger the xbe processing the input state.
	pXboxController->dwPacketNumber = g_Controller.dwPacketNumber;

	//
	// Now convert those values to Xbox XInput
	//
	// Analog Sticks
	pXboxController->Gamepad.sThumbLX = g_Controller.Gamepad.sThumbLX;
	pXboxController->Gamepad.sThumbLY = g_Controller.Gamepad.sThumbLY;
	pXboxController->Gamepad.sThumbRX = g_Controller.Gamepad.sThumbRX;
	pXboxController->Gamepad.sThumbRY = g_Controller.Gamepad.sThumbRY;

	// Analog Buttons
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_A] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_B] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_X] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_Y] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_WHITE] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_BLACK] = (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 255 : 0;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_LEFT_TRIGGER] = g_Controller.Gamepad.bLeftTrigger;
	pXboxController->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_RIGHT_TRIGGER] = g_Controller.Gamepad.bRightTrigger;

	// Digital Buttons
	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_BACK;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_BACK;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_START) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_START;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_START;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_LEFT_THUMB;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_LEFT_THUMB;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_RIGHT_THUMB;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_RIGHT_THUMB;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_UP;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_UP;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_DOWN;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_DOWN;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_LEFT;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_LEFT;
	}

	if (g_Controller.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
		pXboxController->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_RIGHT;
	} else {
		pXboxController->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_RIGHT;
	}
}


// ******************************************************************
// * Native implementation of XInputSetState
// ******************************************************************
void XTL::EmuXInputSetState(DWORD dwPort, XTL::PX_XINPUT_FEEDBACK Feedback)
{
	XINPUT_VIBRATION FrameVibration =
	{
		Feedback->Rumble.wLeftMotorSpeed,
		Feedback->Rumble.wRightMotorSpeed
	};
	
	//
	// Set the PC XInput state

	XInputSetState(dwPort, &FrameVibration);
}
