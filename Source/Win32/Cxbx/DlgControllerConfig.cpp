// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->DlgControllerConfig.cpp
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
#include "DlgControllerConfig.h"
#include "ResCxbx.h"
#include "EmuShared.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <stdio.h>

// ******************************************************************
// * exported globals
// ******************************************************************
InputConfig g_InputConfig;

// ******************************************************************
// * static functions
// ******************************************************************
static INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL    CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
static BOOL    CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
static void             ConfigureInput(HWND hWndDlg, HWND hWndButton, void (InputConfig::*MapFunc)(const char *, int, int));

// ******************************************************************
// * static variables
// ******************************************************************
static LPDIRECTINPUT8       g_pDirectInput8                  = NULL;
static LPDIRECTINPUTDEVICE8 g_pInputDev[MAX_INPUT_DEVICES]   = {0};
static int                  g_pInputCur                      = 0;
static bool                 g_bConfigDone                    = true;

// ******************************************************************
// * Show Controller configuration dialog window
// ******************************************************************
void ShowControllerConfig(HWND hwnd)
{
    DialogBox
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_CONTROLLER_CFG),
        hwnd,
        DlgControllerConfigProc
    );
}

// ******************************************************************
// * Controller configuration dialog procedure
// ******************************************************************
INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            break;
        case WM_CLOSE:
            EndDialog(hWndDlg, wParam); 
            break;
        case WM_COMMAND:
        {
            HWND hWndButton = GetDlgItem(hWndDlg, LOWORD(wParam));

            switch(LOWORD(wParam))
            { 
                case IDC_INPUT_CONFIG_CANCEL:
                    EndDialog(hWndDlg, wParam); 
                    break;
                case IDC_INPUT_CONFIG_ACCEPT:
                    g_EmuShared->UpdateInputConfiguration(&g_InputConfig);
                    EndDialog(hWndDlg, wParam);
                    break;
                case IDC_SET_LEFT_X:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapLThumbX);
                    break;
                case IDC_SET_LEFT_Y:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapLThumbY);
                    break;
                case IDC_SET_RIGHT_X:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapRThumbX);
                    break;
                case IDC_SET_RIGHT_Y:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapRThumbY);
                    break;
                case IDC_SET_X:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapX);
                    break;
                case IDC_SET_Y:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapY);
                    break;
                case IDC_SET_A:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapA);
                    break;
                case IDC_SET_B:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapB);
                    break;
                case IDC_SET_WHITE:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapWhite);
                    break;
                case IDC_SET_BLACK:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapBlack);
                    break;
                case IDC_SET_LTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapLTrigger);
                    break;
                case IDC_SET_RTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapRTrigger);
                    break;
                case IDC_SET_DPAD_UP:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapDPadUp);
                    break;
                case IDC_SET_DPAD_DOWN:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapDPadDown);
                    break;
                case IDC_SET_DPAD_LEFT:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapDPadLeft);
                    break;
                case IDC_SET_DPAD_RIGHT:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapDPadRight);
                    break;
                case IDC_SET_BACK:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapBack);
                    break;
                case IDC_SET_START:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapStart);
                    break;
                case IDC_SET_LTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapLThumb);
                    break;
                case IDC_SET_RTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, InputConfig::MapRThumb);
                    break;
            } 
        }
        break;
    } 
    return FALSE; 
}

// ******************************************************************
// * Enumerate all game controllers
// ******************************************************************
BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    HRESULT hRet = g_pDirectInput8->CreateDevice(lpddi->guidInstance, &g_pInputDev[g_pInputCur], NULL);
    
    if(FAILED(hRet))
        return DIENUM_CONTINUE;

    g_pInputDev[g_pInputCur++]->SetDataFormat(&c_dfDIJoystick);

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * Enumerate all game controller objects
// ******************************************************************
BOOL CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
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
// * ConfigureInput
// ******************************************************************
void ConfigureInput(HWND hWndDlg, HWND hWndButton, void (InputConfig::*MapFunc)(const char *, int, int))
{
    if(!g_bConfigDone)
        return;

    g_bConfigDone = false;

    char szOrgText[32];
    char szNewText[255] = "Recieved no user input, try again...";

    SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), "Waiting for your input...");

    GetWindowText(hWndButton, szOrgText, 32);

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
            g_pInputDev[v]->SetCooperativeLevel(hWndDlg, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
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

    // ******************************************************************
    // * Wait for input, or 5 second timeout
    // ******************************************************************
    {
        DIJOYSTATE              tInputState = {0};
        DIDEVICEINSTANCE        DeviceInstance;
        DIDEVICEOBJECTINSTANCE  ObjectInstance;

        DeviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
        ObjectInstance.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

        for(int v=500;v>0 && !g_bConfigDone;v--)
        {
            // ******************************************************************
            // * Update the button text every second
            // ******************************************************************
            if(v%100 == 0)
            {
                char szBuffer[255];

                sprintf(szBuffer, "%d", (v+99)/100);

                SetWindowText(hWndButton, szBuffer);
            }

            // ******************************************************************
            // * Monitor for significant device state changes
            // ******************************************************************
            for(int v=g_pInputCur-1;v>=0 && !g_bConfigDone;v--)
            {
                HRESULT hRet = g_pInputDev[v]->Poll();

                if(FAILED(hRet))
                {
                    hRet = g_pInputDev[v]->Acquire();

                    while(hRet == DIERR_INPUTLOST)
                        hRet = g_pInputDev[v]->Acquire();

                    return;
                }

                DWORD dwHow = -1;
                DWORD dwFlags = 0;

                g_pInputDev[v]->GetDeviceState(sizeof(DIJOYSTATE), &tInputState);

                // ******************************************************************
                // * Determinate where (if anywhere) there was a state change
                // ******************************************************************
                {
                    int v=0;

                    if(abs(tInputState.lX) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow   = FIELD_OFFSET(DIJOYSTATE, lX);
                        dwFlags = (tInputState.lX > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(tInputState.lY) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lY);
                        dwFlags = (tInputState.lY > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(tInputState.lZ) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lZ);
                        dwFlags = (tInputState.lZ > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(tInputState.lRx) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRx);
                        dwFlags = (tInputState.lRx > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(tInputState.lRy) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRy);
                        dwFlags = (tInputState.lRy > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(tInputState.lRz) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRz);
                        dwFlags = (tInputState.lRz > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else for(v=0;v<2;v++)
                            if(abs(tInputState.rglSlider[v]) > JOYSTICK_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rglSlider[v]);
                    else for(v=0;v<4;v++)
                            if(abs(tInputState.rgdwPOV[v]) > POV_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rgdwPOV[v]);
                    else for(v=0;v<32;v++)
                            if(tInputState.rgbButtons[v] > BUTTON_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rgbButtons[v]);
                }

                // ******************************************************************
                // * Retrieve Object Info
                // ******************************************************************
                if(dwHow != -1)
                {
                    g_pInputDev[v]->GetDeviceInfo(&DeviceInstance);

                    g_pInputDev[v]->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);
 
                    (g_InputConfig.*MapFunc)(DeviceInstance.tszInstanceName, ObjectInstance.dwType, dwFlags);

                    printf("Cxbx: Detected %s on %s (dwType : %.08X)\n", ObjectInstance.tszName, DeviceInstance.tszInstanceName, ObjectInstance.dwType);

                    sprintf(szNewText, "%s Successfully Mapped To %s On %s!", szOrgText, ObjectInstance.tszName, DeviceInstance.tszInstanceName);

                    g_bConfigDone = true;
                }
            }

            Sleep(20);
        }
    }

    // ******************************************************************
    // * Cleanup devices
    // ******************************************************************
    {
        for(int v=g_pInputCur-1;v>=0;v--)
        {
            g_pInputDev[v]->Unacquire();
            g_pInputDev[v]->Release();
            g_pInputDev[v] = 0;
        }

        g_pInputCur = 0;

        if(g_pDirectInput8 != 0)
        {
            g_pDirectInput8->Release();
            g_pDirectInput8 = 0;
        }
    }

    SetWindowText(hWndButton, szOrgText);

    SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), szNewText);

    g_bConfigDone = true;
}
