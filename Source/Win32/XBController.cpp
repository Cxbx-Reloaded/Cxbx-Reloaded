// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->XBController.cpp
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
#include "XBController.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace XTL
{
    #include "EmuXTL.h"
};

// This is ridiculous
#define FIELD_OFFSET(type,field)  ((ULONG)&(((type *)0)->field))

// ******************************************************************
// * func: XBController::XBController
// ******************************************************************
XBController::XBController()
{
    m_CurrentState = XBCTRL_STATE_NONE;

    int v=0;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        m_DeviceName[v][0] = '\0';

        m_InputDevice[v].m_Device = NULL;
        m_InputDevice[v].m_Flags  = 0;
    }

    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        m_ObjectConfig[v].dwDevice = -1;
        m_ObjectConfig[v].dwInfo   = -1;
        m_ObjectConfig[v].dwFlags  = 0;
    }

    m_pDirectInput8 = NULL;

    m_dwInputDeviceCount = 0;
}

// ******************************************************************
// * func: XBController::~XBController
// ******************************************************************
XBController::~XBController()
{
    if(m_CurrentState == XBCTRL_STATE_CONFIG)
        ConfigEnd();
    else if(m_CurrentState == XBCTRL_STATE_LISTEN)
        ListenEnd();
}

// ******************************************************************
// * func: XBController::Load
// ******************************************************************
void XBController::Load(const char *szRegistryKey)
{
    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State", false);
        return;
    }

    // ******************************************************************
    // * Load Configuration from Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            int v=0;

            // ******************************************************************
            // * Load Device Names
            // ******************************************************************
            {
                char szValueName[64];

                for(v=0;v<XBCTRL_MAX_DEVICES;v++)
                {
                    // default is a null string
                    m_DeviceName[v][0] = '\0';

                    sprintf(szValueName, "DeviceName 0x%.02X", v);

                    dwType = REG_SZ; dwSize = 260;
                    RegQueryValueEx(hKey, szValueName, NULL, &dwType, (PBYTE)m_DeviceName[v], &dwSize);
                }
            }

            // ******************************************************************
            // * Load Object Configuration
            // ******************************************************************
            {
                char szValueName[64];

                for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
                {
                    // default object configuration
                    m_ObjectConfig[v].dwDevice = -1;
                    m_ObjectConfig[v].dwInfo   = -1;
                    m_ObjectConfig[v].dwFlags  = 0;

                    sprintf(szValueName, "Object : \"%s\"", m_DeviceNameLookup[v]);

                    dwType = REG_BINARY; dwSize = sizeof(XBCtrlObjectCfg);
                    RegQueryValueEx(hKey, szValueName, NULL, &dwType, (PBYTE)&m_ObjectConfig[v], &dwSize);
                }
            }

            RegCloseKey(hKey);
        }
    }
}

// ******************************************************************
// * func: XBController::Save
// ******************************************************************
void XBController::Save(const char *szRegistryKey)
{
    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State", false);
        return;
    }

    // ******************************************************************
    // * Save Configuration to Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            int v=0;

            // ******************************************************************
            // * Save Device Names
            // ******************************************************************
            {
                char szValueName[64];

                for(v=0;v<XBCTRL_MAX_DEVICES;v++)
                {
                    sprintf(szValueName, "DeviceName 0x%.02X", v);

                    dwType = REG_SZ; dwSize = 260;

                    if(m_DeviceName[v][0] == '\0')
                        RegDeleteValue(hKey, szValueName);
                    else
                        RegSetValueEx(hKey, szValueName, NULL, dwType, (PBYTE)m_DeviceName[v], dwSize);
                }
            }

            // ******************************************************************
            // * Save Object Configuration
            // ******************************************************************
            {
                char szValueName[64];

                for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
                {
                    sprintf(szValueName, "Object : \"%s\"", m_DeviceNameLookup[v]);

                    dwType = REG_BINARY; dwSize = sizeof(XBCtrlObjectCfg);

                    if(m_ObjectConfig[v].dwDevice != -1)
                        RegSetValueEx(hKey, szValueName, NULL, dwType, (PBYTE)&m_ObjectConfig[v], dwSize);
                }
            }

            RegCloseKey(hKey);
        }
    }
}

// ******************************************************************
// * func: XBController::ConfigBegin
// ******************************************************************
void XBController::ConfigBegin(HWND hwnd, XBCtrlObject object)
{
    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State", false);
        return;
    }

    m_CurrentState = XBCTRL_STATE_CONFIG;

    DInputInit(hwnd);

    if(GetError() != 0)
        return;

    lPrevMouseX = -1;
    lPrevMouseY = -1;
    lPrevMouseZ = -1;

    CurConfigObject = object;

    return;
}

// ******************************************************************
// * func: XBController::ConfigPoll
// ******************************************************************
bool XBController::ConfigPoll(char *szStatus)
{
    if(m_CurrentState != XBCTRL_STATE_CONFIG)
    {
        SetError("Invalid State", false);
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
                for(int b=0;b<2;b++)
                    if(abs(JoyState.rglSlider[b]) > DETECT_SENSITIVITY_JOYSTICK)
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rglSlider[b]);
            else 
                for(int b=0;b<4;b++)
                    if(abs(JoyState.rgdwPOV[b]) > DETECT_SENSITIVITY_POV)
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rgdwPOV[b]);
            else
                for(int b=0;b<32;b++)
                {
                    if(JoyState.rgbButtons[b] > DETECT_SENSITIVITY_BUTTON)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIJOYSTATE, rgbButtons[b]);
                        dwFlags |= DEVICE_FLAG_BUTTON;
                    }
                }

            // ******************************************************************
            // * Retrieve Object Info
            // ******************************************************************
            if(dwHow != -1)
            {
                char *szDirection = (dwFlags & DEVICE_FLAG_AXIS) ? (dwFlags & DEVICE_FLAG_POSITIVE) ? "Positive " : "Negative " : "";

                m_InputDevice[v].m_Device->GetDeviceInfo(&DeviceInstance);

                m_InputDevice[v].m_Device->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);

                Map(CurConfigObject, DeviceInstance.tszInstanceName, dwHow, dwFlags);

                printf("Cxbx: Detected %s%s on %s\n", szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName, ObjectInstance.dwType);

                sprintf(szStatus, "Success: %s Mapped to '%s%s' on '%s'!", m_DeviceNameLookup[CurConfigObject], szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName);

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

                printf("Cxbx: Detected Key %d on SysKeyboard\n", dwHow);

                sprintf(szStatus, "Success: %s Mapped to Key %d on SysKeyboard", m_DeviceNameLookup[CurConfigObject], dwHow);

                return true;
            }
        }
        // ******************************************************************
        // * Detect Mouse Input
        // ******************************************************************
        else if(m_InputDevice[v].m_Flags & DEVICE_FLAG_MOUSE)
        {
            XTL::DIMOUSESTATE2 MouseState;

            m_InputDevice[v].m_Device->GetDeviceState(sizeof(MouseState), &MouseState);

            dwFlags = DEVICE_FLAG_MOUSE;

            // ******************************************************************
            // * Detect Button State Change
            // ******************************************************************
            for(int r=0;r<4;r++)
            {
                // 0x80 is the mask for button push
                if(MouseState.rgbButtons[r] & 0x80)
                {
                    dwHow = r;
                    dwFlags |= DEVICE_FLAG_MOUSE_CLICK;
                    break;
                }
            }
            // ******************************************************************
            // * Check for Success
            // ******************************************************************
            if(dwHow != -1)
            {
                Map(CurConfigObject, "SysMouse", dwHow, dwFlags);

                printf("Cxbx: Detected Button %d on SysMouse\n", dwHow);

                sprintf(szStatus, "Success: %s Mapped to Button %d on SysMouse", m_DeviceNameLookup[CurConfigObject], dwHow);

                return true;
            }
            // ******************************************************************
            // * Check for Mouse Movement
            // ******************************************************************
            else
            {
                LONG lAbsDeltaX=0, lAbsDeltaY=0, lAbsDeltaZ=0;
                LONG lDeltaX=0, lDeltaY=0, lDeltaZ=0;

                if(lPrevMouseX == -1 || lPrevMouseY == -1 || lPrevMouseZ == -1)
                    lDeltaX = lDeltaY = lDeltaZ = 0;
                else
                {
                    lDeltaX = MouseState.lX - lPrevMouseX;
                    lDeltaY = MouseState.lY - lPrevMouseY;
                    lDeltaZ = MouseState.lZ - lPrevMouseZ;

                    lAbsDeltaX = abs(lDeltaX);
                    lAbsDeltaY = abs(lDeltaY);
                    lAbsDeltaZ = abs(lDeltaZ);
                }

                LONG lMax = (lAbsDeltaX > lAbsDeltaY) ? lAbsDeltaX : lAbsDeltaY;

                if(lAbsDeltaZ > lMax)
                    lMax = lAbsDeltaZ;

                lPrevMouseX = MouseState.lX;
                lPrevMouseY = MouseState.lY;
                lPrevMouseZ = MouseState.lZ;

                if(lMax > DETECT_SENSITIVITY_MOUSE)
                {
                    dwFlags |= DEVICE_FLAG_AXIS;

                    if(lMax == lAbsDeltaX)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIMOUSESTATE, lX);
                        dwFlags |= (lDeltaX > 0) ? DEVICE_FLAG_POSITIVE : DEVICE_FLAG_NEGATIVE;
                    }
                    else if(lMax == lAbsDeltaY)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIMOUSESTATE, lY);
                        dwFlags |= (lDeltaY > 0) ? DEVICE_FLAG_POSITIVE : DEVICE_FLAG_NEGATIVE;
                    }
                    else if(lMax == lAbsDeltaZ)
                    {
                        dwHow = FIELD_OFFSET(XTL::DIMOUSESTATE, lZ);
                        dwFlags |= (lDeltaZ > 0) ? DEVICE_FLAG_POSITIVE : DEVICE_FLAG_NEGATIVE;
                    }
                }

                // ******************************************************************
                // * Check for Success
                // ******************************************************************
                if(dwHow != -1)
                {
                    char *szDirection = (dwFlags & DEVICE_FLAG_POSITIVE) ? "Positive" : "Negative";
                    char *szObjName = "Unknown";

                    ObjectInstance.dwSize = sizeof(ObjectInstance);

                    if(m_InputDevice[v].m_Device->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET) == DI_OK)
                        szObjName = ObjectInstance.tszName;

                    Map(CurConfigObject, "SysMouse", dwHow, dwFlags);

                    printf("Cxbx: Detected Movement on the %s%s on SysMouse\n", szDirection, szObjName);

                    sprintf(szStatus, "Success: %s Mapped to %s%s on SysMouse", m_DeviceNameLookup[CurConfigObject], szDirection, szObjName);

                    return true;
                }
            }
        }
    }

    return false;
}

// ******************************************************************
// * func: XBController::ConfigEnd
// ******************************************************************
void XBController::ConfigEnd()
{
    if(m_CurrentState != XBCTRL_STATE_CONFIG)
    {
        SetError("Invalid State", false);
        return;
    }

    DInputCleanup();

    m_CurrentState = XBCTRL_STATE_NONE;

    return;
}

// ******************************************************************
// * func: XBController::ListenBegin
// ******************************************************************
void XBController::ListenBegin(HWND hwnd)
{
    int v=0;

    if(m_CurrentState != XBCTRL_STATE_NONE)
    {
        SetError("Invalid State", false);
        return;
    }

    m_CurrentState = XBCTRL_STATE_LISTEN;

    DInputInit(hwnd);

    for(v=XBCTRL_MAX_DEVICES-1;v>=m_dwInputDeviceCount;v--)
        m_DeviceName[v][0] = '\0';

    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        if(m_ObjectConfig[v].dwDevice >= m_dwInputDeviceCount)
        {
            printf("Warning: Device Mapped to %s was not found!\n", m_DeviceNameLookup[v]);
            m_ObjectConfig[v].dwDevice = -1;
        }
    }

    return;
}

// ******************************************************************
// * func: XBController::ListenPoll
// ******************************************************************
void XBController::ListenPoll(XTL::XINPUT_STATE *Controller)
{
    if(Controller == NULL)
        return;

    XTL::LPDIRECTINPUTDEVICE8 pDevice=NULL;

    HRESULT hRet=0;
    DWORD dwFlags=0;

    // ******************************************************************
    // * Default values necessary for axis
    // ******************************************************************
    Controller->Gamepad.sThumbLX = 0;
    Controller->Gamepad.sThumbLY = 0;
    Controller->Gamepad.sThumbRX = 0;
    Controller->Gamepad.sThumbRY = 0;

    // ******************************************************************
    // * Poll all devices
    // ******************************************************************
    for(int v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        int dwDevice = m_ObjectConfig[v].dwDevice;
        int dwFlags  = m_ObjectConfig[v].dwFlags;
        int dwInfo   = m_ObjectConfig[v].dwInfo;

        if(dwDevice == -1)
            continue;

        pDevice = m_InputDevice[dwDevice].m_Device;

        hRet = pDevice->Poll();

        if(FAILED(hRet))
        {
            hRet = pDevice->Acquire();

            while(hRet == DIERR_INPUTLOST)
                hRet = pDevice->Acquire();
        }

        SHORT wValue = 0;

        // ******************************************************************
        // * Interpret PC Joystick Input
        // ******************************************************************
        if(dwFlags & DEVICE_FLAG_JOYSTICK)
        {
            XTL::DIJOYSTATE JoyState = {0};

            if(pDevice->GetDeviceState(sizeof(JoyState), &JoyState) != DI_OK)
                continue;

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
            BYTE KeyboardState[256] = {0};

            if(pDevice->GetDeviceState(sizeof(KeyboardState), &KeyboardState) != DI_OK)
                continue;

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
            XTL::DIMOUSESTATE2 MouseState = {0};

            if(pDevice->GetDeviceState(sizeof(MouseState), &MouseState) != DI_OK)
                continue;

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
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_B:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_X:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_Y:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_WHITE:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_BLACK:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_LTRIGGER:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_RTRIGGER:
                    Controller->Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] = (wValue / 128);
                    break;
                case XBCTRL_OBJECT_DPADUP:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_UP;
                    break;
                case XBCTRL_OBJECT_DPADDOWN:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_DOWN;
                    break;
                case XBCTRL_OBJECT_DPADLEFT:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_LEFT;
                    break;
                case XBCTRL_OBJECT_DPADRIGHT:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_DPAD_RIGHT;
                    break;
                case XBCTRL_OBJECT_BACK:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_START;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_START;
                    break;
                case XBCTRL_OBJECT_START:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_BACK;
                    break;
                case XBCTRL_OBJECT_LTHUMB:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_LEFT_THUMB;
                    break;
                case XBCTRL_OBJECT_RTHUMB:
                    if(wValue > 0)
                        Controller->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
                    else
                        Controller->Gamepad.wButtons &= ~XINPUT_GAMEPAD_RIGHT_THUMB;
                    break;
            }
        }
    }

    return;
}

// ******************************************************************
// * func: XBController::ListenEnd
// ******************************************************************
void XBController::ListenEnd()
{
    if(m_CurrentState != XBCTRL_STATE_LISTEN)
    {
        SetError("Invalid State", false);
        return;
    }

    DInputCleanup();

    m_CurrentState = XBCTRL_STATE_NONE;

    return;
}

// ******************************************************************
// * func: XBController::DeviceIsUsed
// ******************************************************************
bool XBController::DeviceIsUsed(const char *szDeviceName)
{
    for(int v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(m_DeviceName[v][0] != '\0')
        {
            if(strncmp(m_DeviceName[v], szDeviceName, 255) == 0)
                return true;
        }
    }

    return false;
}

// ******************************************************************
// * func: XBController::DInputInit
// ******************************************************************
void XBController::DInputInit(HWND hwnd)
{
    m_dwInputDeviceCount = NULL;

    // ******************************************************************
    // * Create DirectInput Object
    // ******************************************************************
    {
        HRESULT hRet = XTL::DirectInput8Create
        (
            GetModuleHandle(NULL),
            DIRECTINPUT_VERSION,
            XTL::IID_IDirectInput8,
            (void**)&m_pDirectInput8,
            NULL
        );

        if(FAILED(hRet))
        {
            SetError("Could not initialized DirectInput8", true);
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

    // ******************************************************************
    // * Enumerate Controller objects
    // ******************************************************************
    for(m_dwCurObject=0;m_dwCurObject<m_dwInputDeviceCount;m_dwCurObject++)
        m_InputDevice[m_dwCurObject].m_Device->EnumObjects(WrapEnumObjectsCallback, this, DIDFT_ALL);
}

// ******************************************************************
// * func: XBController::DInputCleanup
// ******************************************************************
void XBController::DInputCleanup()
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
// * func: XBController::Map
// ******************************************************************
void XBController::Map(XBCtrlObject object, const char *szDeviceName, int dwInfo, int dwFlags)
{
    // Initialize InputMapping instance
    m_ObjectConfig[object].dwDevice = Insert(szDeviceName);
    m_ObjectConfig[object].dwInfo   = dwInfo;
    m_ObjectConfig[object].dwFlags  = dwFlags;

    // Purse unused device slots
    for(int v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        bool inuse = false;

        for(int r=0;r<XBCTRL_OBJECT_COUNT;r++)
        {
            if(m_ObjectConfig[r].dwDevice == v)
                inuse=true;
        }

        if(!inuse)
            m_DeviceName[v][0] = '\0';
    }
}

// ******************************************************************
// * func: XBController::Insert
// ******************************************************************
int XBController::Insert(const char *szDeviceName)
{
    int v=0;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
        if(strcmp(m_DeviceName[v], szDeviceName) == 0)
            return v;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(m_DeviceName[v][0] == '\0')
        {
            strncpy(m_DeviceName[v], szDeviceName, 255);

            return v;
        }
    }

    MessageBox(NULL, "Unexpected Circumstance (Too Many Controller Devices)! Please contact caustik!", "Cxbx", MB_OK | MB_ICONEXCLAMATION);

    ExitProcess(1);

    return 0;
}

// ******************************************************************
// * func: XBController::ReorderObjects
// ******************************************************************
void XBController::ReorderObjects(const char *szDeviceName, int pos)
{
    int old = -1, v=0;

    // locate old device name position
    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
    {
        if(strcmp(m_DeviceName[v], szDeviceName) == 0)
        {
            old = v;
            break;
        }
    }

    // Swap names, if necessary
    if(old != pos)
    {
        strcpy(m_DeviceName[old], m_DeviceName[pos]);
        strcpy(m_DeviceName[pos], szDeviceName);
    }

    // Update all old values
    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        if(m_ObjectConfig[v].dwDevice == old)
            m_ObjectConfig[v].dwDevice = pos;
        else if(m_ObjectConfig[v].dwDevice == pos)
            m_ObjectConfig[v].dwDevice = old;
    }

    return;
}

// ******************************************************************
// * func: XBController::EnumGameCtrlCallback
// ******************************************************************
BOOL XBController::EnumGameCtrlCallback(XTL::LPCDIDEVICEINSTANCE lpddi)
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
// * func: XBController::EnumObjectsCallback
// ******************************************************************
BOOL XBController::EnumObjectsCallback(XTL::LPCDIDEVICEOBJECTINSTANCE lpddoi)
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
            return DIENUM_STOP;
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
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: WrapEnumGameCtrlCallback
// ******************************************************************
BOOL CALLBACK WrapEnumGameCtrlCallback(XTL::LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    XBController *context = (XBController*)pvRef;

    return context->EnumGameCtrlCallback(lpddi);
}

// ******************************************************************
// * func: WrapEnumObjectsCallback
// ******************************************************************
BOOL CALLBACK WrapEnumObjectsCallback(XTL::LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    XBController *context = (XBController*)pvRef;

    return context->EnumObjectsCallback(lpddoi);
}

// ******************************************************************
// * Input Device Name Lookup Table
// ******************************************************************
const char *XBController::m_DeviceNameLookup[XBCTRL_OBJECT_COUNT] = 
{
    // ******************************************************************
    // * Analog Axis
    // ******************************************************************
    "LThumbPosX", "LThumbNegX", "LThumbPosY", "LThumbNegY",
    "RThumbPosX", "RThumbNegX", "RThumbPosY", "RThumbNegY",

    // ******************************************************************
    // * Analog Buttons
    // ******************************************************************
    "X", "Y", "A", "B", "White", "Black", "LTrigger", "RTrigger",

    // ******************************************************************
    // * Digital Buttons
    // ******************************************************************
    "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
    "Back", "Start", "LThumb", "RThumb",
};
