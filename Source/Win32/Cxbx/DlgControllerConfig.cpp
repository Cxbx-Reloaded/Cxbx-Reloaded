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
// * Exported Global(s)
// ******************************************************************
InputConfig g_InputConfig;

// ******************************************************************
// * Static Function(s)
// ******************************************************************
static INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL    CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
static BOOL    CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);
static void             ConfigureInput(HWND hWndDlg, HWND hWndButton, InputDeviceComponent idc);

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static LPDIRECTINPUT8       g_pDirectInput8                     = NULL;
static LPDIRECTINPUTDEVICE8 g_pInputDev[MAX_INPUT_DEVICES]      = {0};
static DWORD                g_pInputDevFlags[MAX_INPUT_DEVICES] = {0};
static int                  g_pInputCur                         = 0;
static bool                 g_bConfigDone                       = true;

// ******************************************************************
// * Show Controller Configuration Dialog Window
// ******************************************************************
void ShowControllerConfig(HWND hwnd)
{
    g_EmuShared->GetInputConfiguration(&g_InputConfig);

    DialogBox
    (
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_CONTROLLER_CFG),
        hwnd,
        DlgControllerConfigProc
    );
}

// ******************************************************************
// * Controller Configuration Dialog Procedure
// ******************************************************************
INT_PTR CALLBACK DlgControllerConfigProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch(uMsg)
    {
        case WM_INITDIALOG:
            SetClassLong(hWndDlg, GCL_HICON, (LONG)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CXBX)));
            SetFocus(GetDlgItem(hWndDlg, IDC_SET_X));
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
                    g_EmuShared->SetInputConfiguration(&g_InputConfig);
                    EndDialog(hWndDlg, wParam);
                    break;
                case IDC_SET_LEFT_X:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_LTHUMBX);
                    break;
                case IDC_SET_LEFT_Y:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_LTHUMBY);
                    break;
                case IDC_SET_RIGHT_X:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_RTHUMBX);
                    break;
                case IDC_SET_RIGHT_Y:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_RTHUMBY);
                    break;
                case IDC_SET_X:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_X);
                    break;
                case IDC_SET_Y:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_Y);
                    break;
                case IDC_SET_A:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_A);
                    break;
                case IDC_SET_B:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_B);
                    break;
                case IDC_SET_WHITE:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_WHITE);
                    break;
                case IDC_SET_BLACK:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_BLACK);
                    break;
                case IDC_SET_LTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_LTRIGGER);
                    break;
                case IDC_SET_RTRIGGER:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_RTRIGGER);
                    break;
                case IDC_SET_DPAD_UP:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_DPADUP);
                    break;
                case IDC_SET_DPAD_DOWN:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_DPADDOWN);
                    break;
                case IDC_SET_DPAD_LEFT:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_DPADLEFT);
                    break;
                case IDC_SET_DPAD_RIGHT:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_DPADRIGHT);
                    break;
                case IDC_SET_BACK:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_BACK);
                    break;
                case IDC_SET_START:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_START);
                    break;
                case IDC_SET_LTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_LTHUMB);
                    break;
                case IDC_SET_RTHUMB:
                    ConfigureInput(hWndDlg, hWndButton, INPUT_DEVICE_COMPONENT_RTHUMB);
                    break;
            } 
        }
        break;
    } 
    return FALSE; 
}

// ******************************************************************
// * Enumerate Game Controller(s)
// ******************************************************************
BOOL CALLBACK EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    HRESULT hRet = g_pDirectInput8->CreateDevice(lpddi->guidInstance, &g_pInputDev[g_pInputCur], NULL);
    
    if(!FAILED(hRet))
	{
		g_pInputDevFlags[g_pInputCur] = INPUT_MAPPING_JOYSTICK;

        g_pInputDev[g_pInputCur++]->SetDataFormat(&c_dfDIJoystick);
	}

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * Enumerate Game Controller Object(s)
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
        diprg.lMin              = 0 - 32768; 
        diprg.lMax              = 0 + 32767; 

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
void ConfigureInput(HWND hWndDlg, HWND hWndButton, InputDeviceComponent idc)
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
    // * Create all the devices available (well...most of them)
    // ******************************************************************
    if(g_pDirectInput8 != 0)
    {
        HRESULT hRet;

        hRet = g_pDirectInput8->EnumDevices
        (
            DI8DEVCLASS_GAMECTRL,
            EnumGameCtrlCallback,
            NULL,
            DIEDFL_ATTACHEDONLY
        );

        hRet = g_pDirectInput8->CreateDevice(GUID_SysKeyboard, &g_pInputDev[g_pInputCur], NULL);

        if(!FAILED(hRet))
		{
			g_pInputDevFlags[g_pInputCur] = INPUT_MAPPING_KEYBOARD;

            g_pInputDev[g_pInputCur++]->SetDataFormat(&c_dfDIKeyboard);
		}

        hRet = g_pDirectInput8->CreateDevice(GUID_SysMouse, &g_pInputDev[g_pInputCur], NULL);

        if(!FAILED(hRet))
		{
			g_pInputDevFlags[g_pInputCur] = INPUT_MAPPING_MOUSE;

            g_pInputDev[g_pInputCur++]->SetDataFormat(&c_dfDIMouse2);
		}
    }


    // ******************************************************************
    // * Set cooperative level and acquire
    // ******************************************************************
    {
        for(int v=g_pInputCur-1;v>=0;v--)
        {
            g_pInputDev[v]->SetCooperativeLevel(hWndDlg, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
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
    // * Initialize mouse input
    // ******************************************************************
    LONG LastMouse_lX = -1;
    LONG LastMouse_lY = -1;
    LONG LastMouse_lZ = -1;

    // ******************************************************************
    // * Wait for input, or 5 second timeout
    // ******************************************************************
    {
        DIDEVICEINSTANCE        DeviceInstance;
        DIDEVICEOBJECTINSTANCE  ObjectInstance;

        DeviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
        ObjectInstance.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

        for(int v=100;v>0 && !g_bConfigDone;v--)
        {
            // ******************************************************************
            // * Update the button text every second
            // ******************************************************************
            if(v%20 == 0)
            {
                char szBuffer[255];

                sprintf(szBuffer, "%d", (v+19)/20);

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
                DWORD dwFlags = g_pInputDevFlags[v];

                // ******************************************************************
                // * Detect Joystick Input
                // ******************************************************************
				if(g_pInputDevFlags[v] & INPUT_MAPPING_JOYSTICK)
                {
					DIJOYSTATE InputState = {0};

					g_pInputDev[v]->GetDeviceState(sizeof(DIJOYSTATE), &InputState);

                    int b=0;

                    if(abs(InputState.lX) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow   = FIELD_OFFSET(DIJOYSTATE, lX);
                        dwFlags |= (InputState.lX > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(InputState.lY) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lY);
                        dwFlags |= (InputState.lY > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(InputState.lZ) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lZ);
                        dwFlags |= (InputState.lZ > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(InputState.lRx) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRx);
                        dwFlags |= (InputState.lRx > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(InputState.lRy) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRy);
                        dwFlags |= (InputState.lRy > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else if(abs(InputState.lRz) > JOYSTICK_DETECT_SENSITIVITY)
                    {
                        dwHow = FIELD_OFFSET(DIJOYSTATE, lRz);
                        dwFlags |= (InputState.lRz > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                    }
                    else for(b=0;b<2;b++)
                            if(abs(InputState.rglSlider[b]) > JOYSTICK_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rglSlider[b]);
                    else for(b=0;b<4;b++)
                            if(abs(InputState.rgdwPOV[b]) > POV_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rgdwPOV[b]);
                    else for(b=0;b<32;b++)
                            if(InputState.rgbButtons[b] > BUTTON_DETECT_SENSITIVITY)
                                dwHow = FIELD_OFFSET(DIJOYSTATE, rgbButtons[b]);

					// ******************************************************************
					// * Retrieve Object Info
					// ******************************************************************
					if(dwHow != -1)
					{
						g_pInputDev[v]->GetDeviceInfo(&DeviceInstance);

						g_pInputDev[v]->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);

                        g_InputConfig.Map(idc, DeviceInstance.tszInstanceName, ObjectInstance.dwType, dwFlags);

						printf("Cxbx: Detected %s on %s (dwType : %.08X)\n", ObjectInstance.tszName, DeviceInstance.tszInstanceName, ObjectInstance.dwType);

						sprintf(szNewText, "%s Successfully Mapped To %s On %s!", szOrgText, ObjectInstance.tszName, DeviceInstance.tszInstanceName);
					}
                }
                // ******************************************************************
                // * Detect Keyboard Input
                // ******************************************************************
				else if(g_pInputDevFlags[v] & INPUT_MAPPING_KEYBOARD)
				{
					BYTE InputState[256];

					g_pInputDev[v]->GetDeviceState(256, InputState);

					for(int v=0;v<256;v++)
					{
						if(InputState[v] != 0)
						{
							dwHow = v;
							break;
						}
					}

					if(dwHow != -1)
					{
                        g_InputConfig.Map(idc, "SysKeyboard", dwHow, dwFlags);

						printf("Cxbx: Detected Key %d on SysKeyboard\n", dwHow);

						sprintf(szNewText, "%s Successfully Mapped To Key %d On %s!", szOrgText, dwHow, "SysKeyboard");
					}
				}
                // ******************************************************************
                // * Detect Mouse Input
                // ******************************************************************
				else if(g_pInputDevFlags[v] & INPUT_MAPPING_MOUSE)
				{
					DIMOUSESTATE2 InputState = {0};

					g_pInputDev[v]->GetDeviceState(sizeof(DIMOUSESTATE2), &InputState);

                    // detect button changes
					for(int b=0;b<8;b++)
					{
						if(InputState.rgbButtons[b] & 0x80)
						{
							dwHow = b;
							dwFlags &= INPUT_MAPPING_MOUSE_CLICK;
							break;
						}
					}

                    // found a button change
                    if(dwHow != -1)
					{
                        g_InputConfig.Map(idc, "SysMouse", dwHow, dwFlags);

						printf("Cxbx: Detected Button %d on SysMouse\n", dwHow);

						sprintf(szNewText, "%s Successfully Mapped To Button %d On %s!", szOrgText, dwHow, "SysMouse");
					}
                    // no button changes
                    else
                    {
                        LONG delta_lX=0, delta_lY=0, delta_lZ=0;
                        LONG absd_lX=0, absd_lY=0, absd_lZ=0;

                        if(InputState.lX == -1 || InputState.lY == -1 || InputState.lZ == -1)
                            delta_lX = delta_lY = delta_lZ = 0;
                        else
                        {
                            delta_lX = InputState.lX - LastMouse_lX;
                            delta_lY = InputState.lY - LastMouse_lY;
                            delta_lZ = InputState.lZ - LastMouse_lZ;

                            absd_lX  = abs(delta_lX);
                            absd_lY  = abs(delta_lY);
                            absd_lZ  = abs(delta_lZ);
                        }

                        int max = (absd_lX > absd_lY) ? absd_lX : absd_lY;

                        max = (max > absd_lZ) ? max : absd_lZ;

                        if(max > MOUSE_DETECT_SENSITIVITY)
                        {
                            if(max == absd_lX && absd_lX > MOUSE_DETECT_SENSITIVITY)
                            {
                                dwHow = FIELD_OFFSET(DIMOUSESTATE2, lX);
                                dwFlags |= (delta_lX > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                            }
                            else if(max == absd_lY && absd_lY > MOUSE_DETECT_SENSITIVITY)
                            {
                                dwHow = FIELD_OFFSET(DIMOUSESTATE2, lY);
                                dwFlags |= (delta_lY > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                            }
                            else if(max == absd_lZ && absd_lZ > MOUSE_DETECT_SENSITIVITY)
                            {
                                dwHow = FIELD_OFFSET(DIMOUSESTATE2, lZ);
                                dwFlags |= (delta_lZ > 0) ? INPUT_MAPPING_AXIS_POSITIVE : INPUT_MAPPING_AXIS_NEGATIVE;
                            }

                            LastMouse_lX = InputState.lX;
                            LastMouse_lY = InputState.lY;
                            LastMouse_lZ = InputState.lZ;

                            if(dwHow != -1)
					        {
						        g_pInputDev[v]->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);

                                g_InputConfig.Map(idc, "SysMouse", dwHow, dwFlags);

                                printf("Cxbx: Detected Movement on the %s %s on SysMouse\n", (dwFlags & INPUT_MAPPING_AXIS_POSITIVE) ? "Positive" : "Negative", ObjectInstance.tszName);

						        sprintf(szNewText, "%s Successfully Mapped To %s %s On %s!", szOrgText, (dwFlags & INPUT_MAPPING_AXIS_POSITIVE) ? "Positive" : "Negative", ObjectInstance.tszName, "SysMouse");
                            }                    
                        }
                    }
				}

				if(dwHow != -1)
					g_bConfigDone = true;
            }

            Sleep(50);
        }
    }

    // ******************************************************************
    // * Cleanup Devices
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

    // ******************************************************************
    // * Update Window
    // ******************************************************************
	{
		SetWindowText(hWndButton, szOrgText);

		SetWindowText(GetDlgItem(hWndDlg, IDC_CONFIG_STATUS), szNewText);

		MSG Msg;

		while(PeekMessage(&Msg, hWndDlg, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE));
		while(PeekMessage(&Msg, hWndDlg, WM_KEYFIRST,   WM_KEYLAST,   PM_REMOVE));

	}

    g_bConfigDone = true;
}
