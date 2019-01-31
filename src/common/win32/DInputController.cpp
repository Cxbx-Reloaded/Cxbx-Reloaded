// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

// Without this, you'll get a ton of errors from the std library for some unknown reason...
#include "Logging.h"

#include "DInputController.h"

#include "EmuShared.h"
#include "core\kernel\support\EmuXTL.h"

// This is ridiculous
#define FIELD_OFFSET(type,field)  ((ULONG)&(((type *)0)->field))

// ******************************************************************
// * func: DInputController::DInputController
// ******************************************************************
DInputController::DInputController()
{
    m_CurrentState = XBCTRL_STATE_NONE;

    int v=0;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++) {
        /* TODO: Only perform get settings from Settings class.
        m_settings.DeviceName[v][0] = '\0';
        */

        m_InputDevice[v].m_Device = NULL;
        m_InputDevice[v].m_Flags  = 0;
    }
    /* TODO: Only perform get settings from Settings class.
    for(v=0;v<XBCTRL_OBJECT_COUNT;v++) {
        m_settings.ObjectConfig[v].dwDevice = -1;
        m_settings.ObjectConfig[v].dwInfo   = -1;
        m_settings.ObjectConfig[v].dwFlags  = 0;
    }
    */

    m_pDirectInput8 = NULL;

    m_dwInputDeviceCount = 0;
}

// ******************************************************************
// * func: DInputController::~DInputController
// ******************************************************************
DInputController::~DInputController()
{
    if(m_CurrentState == XBCTRL_STATE_CONFIG)
        ConfigEnd();
    else if(m_CurrentState == XBCTRL_STATE_LISTEN)
        ListenEnd();
}

// ******************************************************************
// * func: DInputController::ConfigBegin
// ******************************************************************
void DInputController::ConfigBegin(HWND hwnd, XBCtrlObject object)
{
    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State");
        return;
    }

    m_CurrentState = XBCTRL_STATE_CONFIG;

    DInputInit(hwnd);

    if(HasError())
        return;

    lPrevMouseX = -1;
    lPrevMouseY = -1;
    lPrevMouseZ = -1;

    CurConfigObject = object;

    return;
}

// ******************************************************************
// * func: DInputController::ConfigPoll
// ******************************************************************
bool DInputController::ConfigPoll(char *szStatus)
{
    if(m_CurrentState != XBCTRL_STATE_CONFIG)
    {
        SetError("Invalid State");
        return false;
    }

    XTL::DIDEVICEINSTANCE        DeviceInstance;
    XTL::DIDEVICEOBJECTINSTANCE  ObjectInstance;

    DeviceInstance.dwSize = sizeof(XTL::DIDEVICEINSTANCE);
    ObjectInstance.dwSize = sizeof(XTL::DIDEVICEOBJECTINSTANCE);

    // ******************************************************************
    // * Monitor for significant device state changes
    // ******************************************************************
    for(int v=m_dwInputDeviceCount-1;v>=0;v--)
    {
        // ******************************************************************
        // * Poll the current device
        // ******************************************************************
        {
            HRESULT hRet = m_InputDevice[v].m_Device->Poll();

            if(FAILED(hRet))
            {
                hRet = m_InputDevice[v].m_Device->Acquire();

                while(hRet == DIERR_INPUTLOST)
                    hRet = m_InputDevice[v].m_Device->Acquire();
            }
        }

        DWORD dwHow = -1, dwFlags = m_InputDevice[v].m_Flags;

        // ******************************************************************
        // * Detect Joystick Input
        // ******************************************************************
        if(m_InputDevice[v].m_Flags & DEVICE_FLAG_JOYSTICK)
        {
            XTL::DIJOYSTATE JoyState;

            // ******************************************************************
            // * Get Joystick State
            // ******************************************************************
            {
                HRESULT hRet = m_InputDevice[v].m_Device->GetDeviceState(sizeof(XTL::DIJOYSTATE), &JoyState);

                if(FAILED(hRet))
                    continue;
            }

            dwFlags = DEVICE_FLAG_JOYSTICK;

            if(abs(JoyState.lX) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow   = FIELD_OFFSET(XTL::DIJOYSTATE, lX);
                dwFlags |= (JoyState.lX > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lY) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, lY);
                dwFlags |= (JoyState.lY > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lZ) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, lZ);
                dwFlags |= (JoyState.lZ > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRx) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, lRx);
                dwFlags |= (JoyState.lRx > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRy) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, lRy);
                dwFlags |= (JoyState.lRy > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRz) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, lRz);
                dwFlags |= (JoyState.lRz > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else
            {
                for(int b=0;b<2;b++)
                {
                    if(abs(JoyState.rglSlider[b]) > DETECT_SENSITIVITY_JOYSTICK)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rglSlider[b]);
                        dwFlags |= (JoyState.rglSlider[b] > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
                    }
                }
            }

            /* temporarily disabled
            if(dwHow == -1)
            {
                for(int b=0;b<4;b++)
                {
                    if(abs(JoyState.rgdwPOV[b]) > DETECT_SENSITIVITY_POV)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rgdwPOV[b]);
                    }
                }
            }
            //*/

            if(dwHow == -1)
            {
                for(int b=0;b<32;b++)
                {
                    if(JoyState.rgbButtons[b] > DETECT_SENSITIVITY_BUTTON)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rgbButtons[b]);
                        dwFlags |= DEVICE_FLAG_BUTTON;
                    }
                }
            }

            // ******************************************************************
            // * Retrieve Object Info
            // ******************************************************************
            if(dwHow != -1)
            {
                const char *szDirection = (dwFlags & DEVICE_FLAG_AXIS) ? (dwFlags & DEVICE_FLAG_POSITIVE) ? "Positive " : "Negative " : "";

                m_InputDevice[v].m_Device->GetDeviceInfo(&DeviceInstance);

                m_InputDevice[v].m_Device->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);

                Map(CurConfigObject, DeviceInstance.tszInstanceName, dwHow, dwFlags);

                printf("Cxbx-Reloaded: Detected %s%s on %s%lu\n", szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName, ObjectInstance.dwType);

                sprintf(szStatus, "Success: %s Mapped to '%s%s' on '%s'!", Settings::s_controller_dinput::XboxControllerObjectNameLookup[CurConfigObject], szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName);

                return true;
            }
        }
        // ******************************************************************
        // * Detect Keyboard Input
        // ******************************************************************
        else if(m_InputDevice[v].m_Flags & DEVICE_FLAG_KEYBOARD)
        {
            BYTE KeyState[256];

            m_InputDevice[v].m_Device->GetDeviceState(256, KeyState);

            dwFlags = DEVICE_FLAG_KEYBOARD;

            // ******************************************************************
            // * Check for Keyboard State Change
            // ******************************************************************
            for(int r=0;r<256;r++)
            {
                if(KeyState[r] != 0)
                {
                    dwHow = r;
                    break;
                }
            }

            // ******************************************************************
            // * Check for Success
            // ******************************************************************
            if(dwHow != -1)
            {
                Map(CurConfigObject, "SysKeyboard", dwHow, dwFlags);

                printf("Cxbx-Reloaded: Detected Key %d on SysKeyboard\n", dwHow);

                sprintf(szStatus, "Success: %s Mapped to Key %d on SysKeyboard", Settings::s_controller_dinput::XboxControllerObjectNameLookup[CurConfigObject], dwHow);

                return true;
            }
        }
    }

    return false;
}

// ******************************************************************
// * func: DInputController::ConfigEnd
// ******************************************************************
void DInputController::ConfigEnd()
{
    if(m_CurrentState != XBCTRL_STATE_CONFIG)
    {
        SetError("Invalid State");
        return;
    }

    DInputCleanup();

    m_CurrentState = XBCTRL_STATE_NONE;

    return;
}

// ******************************************************************
// * func: DInputController::ListenBegin
// ******************************************************************
void DInputController::ListenBegin(HWND hwnd)
{
    int v=0;

    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State");
        return;
    }

    m_CurrentState = XBCTRL_STATE_LISTEN;

    DInputInit(hwnd);

    for(v=XBCTRL_MAX_DEVICES-1;v>=m_dwInputDeviceCount;v--)
        m_settings.DeviceName[v][0] = '\0';

    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        if(m_settings.ObjectConfig[v].dwDevice >= m_dwInputDeviceCount)
        {
            printf("Warning: Device Mapped to %s was not found!\n", Settings::s_controller_dinput::XboxControllerObjectNameLookup[v]);
            m_settings.ObjectConfig[v].dwDevice = -1;
        }
    }

    return;
}

// ******************************************************************
// * func: DInputController::ListenPoll
// ******************************************************************
void DInputController::ListenPoll(XTL::X_XINPUT_STATE *Controller)
{
    if(Controller == NULL)
        return;

    XTL::LPDIRECTINPUTDEVICE8 pDevice=NULL;
    XTL::LPDIRECTINPUTDEVICE8 pPrevDevice=(XTL::LPDIRECTINPUTDEVICE8)-1;

    HRESULT hRet=0;
    DWORD dwFlags=0;

    // Test case: Smashing Drive, pointer of X_XINPUT_STATE is reused for other controller ports.
    // By setting Gamepad to zero'd no longer have conflict with input changes from other controller port.
    // ******************************************************************
    // * Set default values
    // ******************************************************************
    Controller->Gamepad = { 0 };

    XTL::DIJOYSTATE JoyState = { 0 };
	BYTE KeyboardState[256] = { 0 };
	XTL::DIMOUSESTATE2 MouseState = { 0 };

    // ******************************************************************
    // * Poll all devices
    // ******************************************************************
    for(int v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        int dwDevice = m_settings.ObjectConfig[v].dwDevice;
        int dwFlags  = m_settings.ObjectConfig[v].dwFlags;
        int dwInfo   = m_settings.ObjectConfig[v].dwInfo;

        if(dwDevice == -1)
            continue;

        pDevice = m_InputDevice[dwDevice].m_Device;
		if (pDevice == nullptr)
			continue;

		// Only poll on device-switch
		if (pPrevDevice != pDevice) {
			pPrevDevice = pDevice;
			hRet = pDevice->Poll();
			if (FAILED(hRet)) {
				hRet = pDevice->Acquire();
				while (hRet == DIERR_INPUTLOST)
					hRet = pDevice->Acquire();
			}

			if (dwFlags & DEVICE_FLAG_JOYSTICK) {
				JoyState = { 0 };
				if (pDevice->GetDeviceState(sizeof(JoyState), &JoyState) != DI_OK)
					continue;
			}
			else if (dwFlags & DEVICE_FLAG_KEYBOARD) {
				memset(KeyboardState, 0, sizeof(KeyboardState));
				if (pDevice->GetDeviceState(sizeof(KeyboardState), &KeyboardState) != DI_OK)
					continue;
			}
			else if (dwFlags & DEVICE_FLAG_MOUSE) {
				MouseState = { 0 };

				if (pDevice->GetDeviceState(sizeof(MouseState), &MouseState) != DI_OK)
					continue;
			}
		}

        SHORT wValue = 0;

        // ******************************************************************
        // * Interpret PC Joystick Input
        // ******************************************************************
        if(dwFlags & DEVICE_FLAG_JOYSTICK)
        {
            if(dwFlags & DEVICE_FLAG_AXIS)
            {
                LONG *pdwAxis = (LONG*)((uint32)&JoyState + dwInfo);
                wValue = (SHORT)(*pdwAxis);

                if(dwFlags & DEVICE_FLAG_NEGATIVE)
                {
                    if(wValue < 0)
                        wValue = abs(wValue+1);
                    else
                        wValue = 0;
                }
                else if(dwFlags & DEVICE_FLAG_POSITIVE)
                {
                    if(wValue < 0)
                        wValue = 0;
                }
            }
            else if(dwFlags & DEVICE_FLAG_BUTTON)
            {
                BYTE *pbButton = (BYTE*)((uint32)&JoyState + dwInfo);

                if(*pbButton & 0x80)
                    wValue = 32767;
                else
                    wValue = 0;
            }
        }
        // ******************************************************************
        // * Interpret PC KeyBoard Input
        // ******************************************************************
        else if(dwFlags & DEVICE_FLAG_KEYBOARD)
        {
            BYTE bKey = KeyboardState[dwInfo];

            if(bKey & 0x80)
                wValue = 32767;
            else
                wValue = 0;
        }
        // ******************************************************************
        // * Interpret PC Mouse Input
        // ******************************************************************
        else if(dwFlags & DEVICE_FLAG_MOUSE)
        {
            if(dwFlags & DEVICE_FLAG_MOUSE_CLICK)
            {
                if(MouseState.rgbButtons[dwInfo] & 0x80)
                    wValue = 32767;
                else
                    wValue = 0;
            }
            else if(dwFlags & DEVICE_FLAG_AXIS)
            {
                static LONG lAccumX = 0;
                static LONG lAccumY = 0;
                static LONG lAccumZ = 0;

                lAccumX += MouseState.lX * 300;
                lAccumY += MouseState.lY * 300;
                lAccumZ += MouseState.lZ * 300;

                if(lAccumX > 32767)
                    lAccumX = 32767;
                else if(lAccumX < -32768)
                    lAccumX = -32768;

                if(lAccumY > 32767)
                    lAccumY = 32767;
                else if(lAccumY < -32768)
                    lAccumY = -32768;

                if(lAccumZ > 32767)
                    lAccumZ = 32767;
                else if(lAccumZ < -32768)
                    lAccumZ = -32768;

                if(dwInfo == FIELD_OFFSET(XTL::DIMOUSESTATE, lX))
                    wValue = (WORD)lAccumX;
                else if(dwInfo == FIELD_OFFSET(XTL::DIMOUSESTATE, lY))
                    wValue = (WORD)lAccumY;
                else if(dwInfo == FIELD_OFFSET(XTL::DIMOUSESTATE, lZ))
                    wValue = (WORD)lAccumZ;

                if(dwFlags & DEVICE_FLAG_NEGATIVE)
                {
                    if(wValue < 0)
                        wValue = abs(wValue+1);
                    else
                        wValue = 0;
                }
                else if(dwFlags & DEVICE_FLAG_POSITIVE)
                {
                    if(wValue < 0)
                        wValue = 0;
                }
            }
        }

        // ******************************************************************
        // * Map Xbox Joystick Input
        // ******************************************************************
        if(v >= XBCTRL_OBJECT_LTHUMBPOSX && v <= XBCTRL_OBJECT_RTHUMB)
        {
            switch(v)
            {
                case XBCTRL_OBJECT_LTHUMBPOSY:
                    Controller->Gamepad.sThumbLY += wValue;
                    break;
                case XBCTRL_OBJECT_LTHUMBNEGY:
                    Controller->Gamepad.sThumbLY -= wValue;
                    break;
                case XBCTRL_OBJECT_RTHUMBPOSY:
                    Controller->Gamepad.sThumbRY += wValue;
                    break;
                case XBCTRL_OBJECT_RTHUMBNEGY:
                    Controller->Gamepad.sThumbRY -= wValue;
                    break;
                case XBCTRL_OBJECT_LTHUMBPOSX:
                    Controller->Gamepad.sThumbLX += wValue;
                    break;
                case XBCTRL_OBJECT_LTHUMBNEGX:
                    Controller->Gamepad.sThumbLX -= wValue;
                    break;
                case XBCTRL_OBJECT_RTHUMBPOSX:
                    Controller->Gamepad.sThumbRX += wValue;
                    break;
                case XBCTRL_OBJECT_RTHUMBNEGX:
                    Controller->Gamepad.sThumbRX -= wValue;
                    break;
                case XBCTRL_OBJECT_A:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_A] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_B:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_B] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_X:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_X] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_Y:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_Y] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_WHITE:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_WHITE] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_BLACK:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_BLACK] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_LTRIGGER:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_LEFT_TRIGGER] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_RTRIGGER:
                    Controller->Gamepad.bAnalogButtons[X_XINPUT_GAMEPAD_RIGHT_TRIGGER] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_DPADUP:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_UP;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_UP;
                    break;
                case XBCTRL_OBJECT_DPADDOWN:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_DOWN;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_DOWN;
                    break;
                case XBCTRL_OBJECT_DPADLEFT:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_LEFT;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_LEFT;
                    break;
                case XBCTRL_OBJECT_DPADRIGHT:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_DPAD_RIGHT;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_DPAD_RIGHT;
                    break;
                case XBCTRL_OBJECT_BACK:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_BACK;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_BACK;
                    break;
                case XBCTRL_OBJECT_START:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_START;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_START;
                    break;
                case XBCTRL_OBJECT_LTHUMB:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_LEFT_THUMB;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_LEFT_THUMB;
                    break;
                case XBCTRL_OBJECT_RTHUMB:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= X_XINPUT_GAMEPAD_RIGHT_THUMB;
                    else
                        Controller->Gamepad.wButtons &= ~X_XINPUT_GAMEPAD_RIGHT_THUMB;
                    break;
            }
        }
    }

    return;
}

// ******************************************************************
// * func: DInputController::ListenEnd
// ******************************************************************
void DInputController::ListenEnd()
{
    if(m_CurrentState != XBCTRL_STATE_LISTEN)
    {
        SetError("Invalid State");
        return;
    }

    DInputCleanup();

    m_CurrentState = XBCTRL_STATE_NONE;

    return;
}

// ******************************************************************
// * func: DInputController::DeviceIsUsed
// ******************************************************************
bool DInputController::DeviceIsUsed(const char *szDeviceName)
{
    for(int v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(m_settings.DeviceName[v][0] != '\0')
        {
            if(strncmp(m_settings.DeviceName[v], szDeviceName, 255) == 0)
                return true;
        }
    }

    return false;
}

// ******************************************************************
// * func: DInputController::DInputInit
// ******************************************************************
void DInputController::DInputInit(HWND hwnd)
{
    m_dwInputDeviceCount = NULL;

    // ******************************************************************
    // * Create DirectInput Object
    // ******************************************************************
    {
        HRESULT hRet = XTL::DirectInput8Create
        (
            hActiveModule,
            DIRECTINPUT_VERSION,
            XTL::IID_IDirectInput8,
            (void**)&m_pDirectInput8,
            NULL
        );

        if(FAILED(hRet))
        {
            SetFatalError("Could not initialized DirectInput8");
            return;
        }
    }


    // ******************************************************************
    // * Create all the devices available (well...most of them)
    // ******************************************************************
    if(m_pDirectInput8 != 0)
    {
        HRESULT hRet = m_pDirectInput8->EnumDevices
        (
            DI8DEVCLASS_GAMECTRL,
            WrapEnumGameCtrlCallback,
            this,
            DIEDFL_ATTACHEDONLY
        );

        if(m_CurrentState == XBCTRL_STATE_CONFIG || DeviceIsUsed("SysKeyboard"))
        {
            hRet = m_pDirectInput8->CreateDevice(XTL::GUID_SysKeyboard, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);

            if(!FAILED(hRet))
            {
                m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_KEYBOARD;

                m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&XTL::c_dfDIKeyboard);
            }

            if(m_CurrentState == XBCTRL_STATE_LISTEN)
                ReorderObjects("SysKeyboard", m_dwInputDeviceCount - 1);
        }

        if(m_CurrentState == XBCTRL_STATE_CONFIG || DeviceIsUsed("SysMouse"))
        {
            hRet = m_pDirectInput8->CreateDevice(XTL::GUID_SysMouse, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);

            if(!FAILED(hRet))
            {
                m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_MOUSE;

                m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&XTL::c_dfDIMouse2);
            }

            if(m_CurrentState == XBCTRL_STATE_LISTEN)
                ReorderObjects("SysMouse", m_dwInputDeviceCount - 1);
        }
    }

    // ******************************************************************
    // * Enumerate Controller objects
    // ******************************************************************
    for(m_dwCurObject=0;m_dwCurObject<m_dwInputDeviceCount;m_dwCurObject++)
        m_InputDevice[m_dwCurObject].m_Device->EnumObjects(WrapEnumObjectsCallback, this, DIDFT_ALL);

    // ******************************************************************
    // * Set cooperative level and acquire
    // ******************************************************************
    {
        for(int v=m_dwInputDeviceCount-1;v>=0;v--)
        {
            m_InputDevice[v].m_Device->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
            m_InputDevice[v].m_Device->Acquire();

            HRESULT hRet = m_InputDevice[v].m_Device->Poll();

            if(FAILED(hRet))
            {
                hRet = m_InputDevice[v].m_Device->Acquire();

                while(hRet == DIERR_INPUTLOST)
                    hRet = m_InputDevice[v].m_Device->Acquire();

                if(hRet != DIERR_INPUTLOST)
                    break;
            }
        }
    }
}

// ******************************************************************
// * func: DInputController::DInputCleanup
// ******************************************************************
void DInputController::DInputCleanup()
{
    for(int v=m_dwInputDeviceCount-1;v>=0;v--)
    {
        m_InputDevice[v].m_Device->Unacquire();
        m_InputDevice[v].m_Device->Release();
        m_InputDevice[v].m_Device = 0;
    }

    m_dwInputDeviceCount = 0;

    if(m_pDirectInput8 != 0)
    {
        m_pDirectInput8->Release();
        m_pDirectInput8 = 0;
    }

    return;
}

// ******************************************************************
// * func: DInputController::Map
// ******************************************************************
void DInputController::Map(XBCtrlObject object, const char *szDeviceName, int dwInfo, int dwFlags)
{
    // Initialize InputMapping instance
    m_settings.ObjectConfig[object].dwDevice = Insert(szDeviceName);
    m_settings.ObjectConfig[object].dwInfo   = dwInfo;
    m_settings.ObjectConfig[object].dwFlags  = dwFlags;

    // Purge unused device slots
    for(int v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        bool inuse = false;

        for(int r=0;r<XBCTRL_OBJECT_COUNT;r++)
        {
            if(m_settings.ObjectConfig[r].dwDevice == v)
                inuse=true;
        }

        if(!inuse)
            m_settings.DeviceName[v][0] = '\0';
    }
}

// ******************************************************************
// * func: DInputController::Insert
// ******************************************************************
int DInputController::Insert(const char *szDeviceName)
{
    int v=0;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
        if(strcmp(m_settings.DeviceName[v], szDeviceName) == 0)
            return v;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(m_settings.DeviceName[v][0] == '\0')
        {
            strncpy(m_settings.DeviceName[v], szDeviceName, 255);

            return v;
        }
    }

    MessageBox(NULL, "Unexpected Circumstance (Too Many Controller Devices)! Please contact caustik!", "Cxbx-Reloaded", MB_OK | MB_ICONEXCLAMATION);

	EmuShared::Cleanup();
    ExitProcess(1);

    return 0;
}

// ******************************************************************
// * func: DInputController::ReorderObjects
// ******************************************************************
void DInputController::ReorderObjects(const char *szDeviceName, int pos)
{
    int old = -1, v=0;

    // locate old device name position
    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(strcmp(m_settings.DeviceName[v], szDeviceName) == 0)
        {
            old = v;
            break;
        } else {
            // If old device name was not found, do not continue
            // int old will equal -1, which leads to an out-of-bounds
            // memory access below
            return;
        }
    }

    // Swap names, if necessary
    if(old != pos)
    {
        strcpy(m_settings.DeviceName[old], m_settings.DeviceName[pos]);
        strcpy(m_settings.DeviceName[pos], szDeviceName);
    }

    // Update all old values
    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        if(m_settings.ObjectConfig[v].dwDevice == old)
            m_settings.ObjectConfig[v].dwDevice = pos;
        else if(m_settings.ObjectConfig[v].dwDevice == pos)
            m_settings.ObjectConfig[v].dwDevice = old;
    }

    return;
}

// ******************************************************************
// * func: DInputController::EnumGameCtrlCallback
// ******************************************************************
BOOL DInputController::EnumGameCtrlCallback(XTL::LPCDIDEVICEINSTANCE lpddi)
{
    if(m_CurrentState == XBCTRL_STATE_LISTEN && !DeviceIsUsed(lpddi->tszInstanceName))
        return DIENUM_CONTINUE;

    HRESULT hRet = m_pDirectInput8->CreateDevice(lpddi->guidInstance, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);

    if(!FAILED(hRet))
    {
        m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_JOYSTICK;

        m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&XTL::c_dfDIJoystick);

        if(m_CurrentState == XBCTRL_STATE_LISTEN)
            ReorderObjects(lpddi->tszInstanceName, m_dwInputDeviceCount - 1);
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: DInputController::EnumObjectsCallback
// ******************************************************************
BOOL DInputController::EnumObjectsCallback(XTL::LPCDIDEVICEOBJECTINSTANCE lpddoi)
{
    if(lpddoi->dwType & DIDFT_AXIS)
    {
        XTL::DIPROPRANGE diprg;

        diprg.diph.dwSize       = sizeof(XTL::DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(XTL::DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = lpddoi->dwType;
        diprg.lMin              = 0 - 32768;
        diprg.lMax              = 0 + 32767;

        HRESULT hRet = m_InputDevice[m_dwCurObject].m_Device->SetProperty(DIPROP_RANGE, &diprg.diph);

        if(FAILED(hRet))
        {
            if(hRet == E_NOTIMPL)
                return DIENUM_CONTINUE;
            else
                return DIENUM_STOP;
        }
    }
    else if(lpddoi->dwType & DIDFT_BUTTON)
    {
        XTL::DIPROPRANGE diprg;

        diprg.diph.dwSize       = sizeof(XTL::DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(XTL::DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = lpddoi->dwType;
        diprg.lMin              = 0;
        diprg.lMax              = 255;

        HRESULT hRet = m_InputDevice[m_dwCurObject].m_Device->SetProperty(DIPROP_RANGE, &diprg.diph);

        if(FAILED(hRet))
        {
            if(hRet == E_NOTIMPL)
                return DIENUM_CONTINUE;
            else
                return DIENUM_STOP;
        }
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: WrapEnumGameCtrlCallback
// ******************************************************************
BOOL CALLBACK WrapEnumGameCtrlCallback(XTL::LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    DInputController *context = (DInputController*)pvRef;

    return context->EnumGameCtrlCallback(lpddi);
}

// ******************************************************************
// * func: WrapEnumObjectsCallback
// ******************************************************************
BOOL CALLBACK WrapEnumObjectsCallback(XTL::LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    DInputController *context = (DInputController*)pvRef;

    return context->EnumObjectsCallback(lpddoi);
}
