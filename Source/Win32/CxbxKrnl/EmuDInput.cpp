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
#include "Cxbx.h"
#include "Emu.h"

using namespace win32;

// ******************************************************************
// * globals
// ******************************************************************
LPDIRECTINPUT8          g_pDirectInput8 = NULL;         
LPDIRECTINPUTDEVICE8    g_pGameCtrl     = NULL;
xboxkrnl::XINPUT_STATE  g_EmuController1   = {0};

// ******************************************************************
// * statics
// ******************************************************************
static BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
static BOOL CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

// ******************************************************************
// * func: EmuInitDInput
// ******************************************************************
void xboxkrnl::EmuInitDInput()
{
    // ******************************************************************
    // * Create DirectInput object
    // ******************************************************************
    {
        HRESULT hRet = DirectInput8Create
        (
            GetModuleHandle(NULL),
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            (void**)&g_pDirectInput8,
            NULL
        );

        if(hRet != DI_OK)
            EmuPanic();
    }

    // ******************************************************************
    // * Enumerate Controller(s)
    // ******************************************************************
    {
        HRESULT hRet = g_pDirectInput8->EnumDevices
        (
            DI8DEVCLASS_GAMECTRL,
            EnumGameCtrlCallback,
            NULL,
            DIEDFL_ATTACHEDONLY
        );

        if(hRet != DI_OK)
            EmuPanic();

        if(g_pGameCtrl == NULL)
            return;
    }

    // ******************************************************************
    // * Set Controller data format
    // ******************************************************************
    {
        HRESULT hRet = g_pGameCtrl->SetDataFormat(&c_dfDIJoystick);

        if(FAILED(hRet))
            EmuPanic();
    }

    // ******************************************************************
    // * Set cooperative level
    // ******************************************************************
    {
        HRESULT hRet = g_pGameCtrl->SetCooperativeLevel(g_EmuWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

        if(FAILED(hRet))
            EmuPanic();
    }

    // ******************************************************************
    // * Enumerate Controller objects
    // ******************************************************************
    {
        HRESULT hRet = g_pGameCtrl->EnumObjects(EnumObjectsCallback, NULL, DIDFT_ALL);

        if(FAILED(hRet))
            EmuPanic();
    }
}

// ******************************************************************
// * func: EnumGameCtrlCallback
// ******************************************************************
static BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    HRESULT hRet = g_pDirectInput8->CreateDevice(lpddi->guidInstance, &g_pGameCtrl, NULL);

    // keep on enumerating if this device could not be created
    if(FAILED(hRet))
        return DIENUM_CONTINUE;

    // once we find a working device, stop enumeration (we just want one)
    return DIENUM_STOP;
}

// ******************************************************************
// * func: EnumObjectsCallback
// ******************************************************************
static BOOL CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    if(lpddoi->dwType & DIDFT_AXIS)
    {
        DIPROPRANGE diprg; 

        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = lpddoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -32768; 
        diprg.lMax              = 32767; 
    
        // set axis range
        HRESULT hRet = g_pGameCtrl->SetProperty(DIPROP_RANGE, &diprg.diph);

        if(FAILED(hRet))
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: EmuPollController
// ******************************************************************
void xboxkrnl::EmuPollController()
{
    DIJOYSTATE ControllerState;

    if(g_pGameCtrl == NULL)
        return;

    HRESULT hRet = g_pGameCtrl->Poll();

    if(FAILED(hRet))
    {
        hRet = g_pGameCtrl->Acquire();

        while(hRet == DIERR_INPUTLOST)
            hRet = g_pGameCtrl->Acquire();

        return;
    }

    hRet = g_pGameCtrl->GetDeviceState(sizeof(DIJOYSTATE), &ControllerState);
    
    if(FAILED(hRet))
        return;

    g_EmuController1.dwPacketNumber++;
    g_EmuController1.Gamepad.sThumbRX = (short)ControllerState.lX;
    g_EmuController1.Gamepad.sThumbRY = (short)(-1 - ControllerState.lY);
    g_EmuController1.Gamepad.sThumbLX = (short)ControllerState.lRx;
    g_EmuController1.Gamepad.sThumbLY = (short)(-1 - ControllerState.lRy);

    return;
}