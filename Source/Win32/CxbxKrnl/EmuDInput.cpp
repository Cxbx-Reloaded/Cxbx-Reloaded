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
#include "InputConfig.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xapi
{
    #include <EmuXapi.h>
};

// ******************************************************************
// * globals
// ******************************************************************
LPDIRECTINPUT8          g_pDirectInput8 = NULL;
LPDIRECTINPUTDEVICE8    g_pInputDev[MAX_INPUT_DEVICES]   = {0};
int                     g_pInputCur                      = 0;
xapi::XINPUT_STATE      g_EmuController1;


// ******************************************************************
// * statics
// ******************************************************************
static BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
static BOOL CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

// ******************************************************************
// * func: EmuInitDInput
// ******************************************************************
void EmuInitDInput()
{
    // ******************************************************************
    // * Create DirectInput object
    // ******************************************************************
    {
        DirectInput8Create
        (
            GetModuleHandle(NULL),
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            (void**)&g_pDirectInput8,
            NULL
        );
    }

    // ******************************************************************
    // * Enumerate Controller(s)
    // ******************************************************************
    if(g_pDirectInput8 != 0)
    {
        g_pDirectInput8->EnumDevices
        (
            DI8DEVCLASS_GAMECTRL,
            EnumGameCtrlCallback,
            NULL,
            DIEDFL_ATTACHEDONLY
        );
    }

    // ******************************************************************
    // * Set cooperative level and acquire
    // ******************************************************************
    {
        for(int v=g_pInputCur-1;v>=0;v--)
        {
            g_pInputDev[v]->SetCooperativeLevel(g_EmuWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
            g_pInputDev[v]->SetDataFormat(&c_dfDIJoystick);
            g_pInputDev[v]->Acquire();

            HRESULT hRet = g_pInputDev[v]->Poll();

            if(FAILED(hRet))
            {
                hRet = g_pInputDev[v]->Acquire();

                while(hRet == DIERR_INPUTLOST)
                    hRet = g_pInputDev[v]->Acquire();

                if(hRet != DIERR_INPUTLOST)
                    break;
            }
        }
    }

    // ******************************************************************
    // * Enumerate Controller objects
    // ******************************************************************
    {
        for(int v=0;v<g_pInputCur;v++)
            g_pInputDev[v]->EnumObjects(EnumObjectsCallback, (LPVOID)v, DIDFT_ALL);
    }
}

// ******************************************************************
// * func: EnumGameCtrlCallback
// ******************************************************************
static BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    HRESULT hRet = g_pDirectInput8->CreateDevice(lpddi->guidInstance, &g_pInputDev[g_pInputCur], NULL);
    
    if(FAILED(hRet))
        return DIENUM_CONTINUE;

    g_pInputDev[g_pInputCur++]->SetDataFormat(&c_dfDIJoystick);

    return DIENUM_CONTINUE;

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
        diprg.diph.dwObj        = lpddoi->dwType;
        diprg.lMin              = -32768; 
        diprg.lMax              = 32767; 
    
        // set axis range
        HRESULT hRet = g_pInputDev[(int)pvRef]->SetProperty(DIPROP_RANGE, &diprg.diph);

        if(FAILED(hRet))
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: EmuPollController
// ******************************************************************
void EmuPollController()
{
    /*
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
*/
    return;
}