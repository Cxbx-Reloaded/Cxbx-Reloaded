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

#include <stdio.h>

// ******************************************************************
// * func: XBController::XBController
// ******************************************************************
XBController::XBController()
{
    int v=0;

    for(v=0;v<XBCTRL_MAX_DEVICES;v++)
        m_DeviceName[v][0] = '\0';

    for(v=0;v<XBCTRL_OBJECT_COUNT;v++)
    {
        m_ObjectConfig[v].dwDevice = -1;
        m_ObjectConfig[v].dwInfo   = -1;
        m_ObjectConfig[v].dwFlags  = 0;
    }
}

// ******************************************************************
// * func: XBController::Load
// ******************************************************************
void XBController::Load(const char *szRegistryKey)
{
    Lock();

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

    Unlock();
}

// ******************************************************************
// * func: XBController::Save
// ******************************************************************
void XBController::Save(const char *szRegistryKey)
{
    Lock();

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

                    if(m_DeviceName[v][0] != '\0')
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

    Unlock();
}

// ******************************************************************
// * func: XBController::ConfigBegin
// ******************************************************************
void XBController::ConfigBegin(HWND hwnd, XBCtrlObject object)
{
    // ******************************************************************
    // * Create DirectInput Object
    // ******************************************************************
    {
        HRESULT hRet = DirectInput8Create
        (
            GetModuleHandle(NULL),
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            (void**)&m_pDirectInput8,
            NULL
        );

        if(FAILED(hRet))
        {
            SetError("Could not initialized DirectInput8", true);
            goto cleanup;
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

        hRet = m_pDirectInput8->CreateDevice(GUID_SysKeyboard, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);

        if(!FAILED(hRet))
		{
			m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_KEYBOARD;

            m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&c_dfDIKeyboard);
		}

        hRet = m_pDirectInput8->CreateDevice(GUID_SysMouse, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);

        if(!FAILED(hRet))
		{
			m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_MOUSE;

            m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&c_dfDIMouse2);
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
    {
        for(m_dwCurObject=0;m_dwCurObject<m_dwInputDeviceCount;m_dwCurObject++)
            m_InputDevice[m_dwCurObject].m_Device->EnumObjects(WrapEnumObjectsCallback, this, DIDFT_ALL);
    }

    // ******************************************************************
    // * Last Mouse Input for Delta Calculations
    // ******************************************************************
    LastMouse_lX = -1;
    LastMouse_lY = -1;
    LastMouse_lZ = -1;

    CurConfigObject = object;

cleanup:

    return;
}

// ******************************************************************
// * func: XBController::ConfigPoll
// ******************************************************************
bool XBController::ConfigPoll(char *szStatus)
{
    DIDEVICEINSTANCE        DeviceInstance;
    DIDEVICEOBJECTINSTANCE  ObjectInstance;

    DeviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
    ObjectInstance.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

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
			DIJOYSTATE JoyState;

            // ******************************************************************
            // * Get Joystick State
            // ******************************************************************
            {
                HRESULT hRet = m_InputDevice[v].m_Device->GetDeviceState(sizeof(DIJOYSTATE), &JoyState);

                if(FAILED(hRet))
                    continue;
            }

            dwFlags = DEVICE_FLAG_JOYSTICK;

            if(abs(JoyState.lX) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow   = FIELD_OFFSET(DIJOYSTATE, lX);
                dwFlags |= (JoyState.lX > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lY) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(DIJOYSTATE, lY);
                dwFlags |= (JoyState.lY > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lZ) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(DIJOYSTATE, lZ);
                dwFlags |= (JoyState.lZ > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRx) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(DIJOYSTATE, lRx);
                dwFlags |= (JoyState.lRx > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRy) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(DIJOYSTATE, lRy);
                dwFlags |= (JoyState.lRy > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else if(abs(JoyState.lRz) > DETECT_SENSITIVITY_JOYSTICK)
            {
                dwHow = FIELD_OFFSET(DIJOYSTATE, lRz);
                dwFlags |= (JoyState.lRz > 0) ? (DEVICE_FLAG_AXIS | DEVICE_FLAG_POSITIVE) : (DEVICE_FLAG_AXIS | DEVICE_FLAG_NEGATIVE);
            }
            else 
                for(int b=0;b<2;b++)
                    if(abs(JoyState.rglSlider[b]) > DETECT_SENSITIVITY_JOYSTICK)
                        dwHow = FIELD_OFFSET(DIJOYSTATE, rglSlider[b]);
            else 
                for(int b=0;b<4;b++)
                    if(abs(JoyState.rgdwPOV[b]) > DETECT_SENSITIVITY_POV)
                        dwHow = FIELD_OFFSET(DIJOYSTATE, rgdwPOV[b]);
            else
                for(int b=0;b<32;b++)
                    if(JoyState.rgbButtons[b] > DETECT_SENSITIVITY_BUTTON)
                        dwHow = FIELD_OFFSET(DIJOYSTATE, rgbButtons[b]);

            // ******************************************************************
			// * Retrieve Object Info
			// ******************************************************************
			if(dwHow != -1)
			{
                char *szDirection = (dwFlags & DEVICE_FLAG_AXIS) ? (dwFlags & DEVICE_FLAG_POSITIVE) ? "Positive " : "Negative " : "";

                m_InputDevice[v].m_Device->GetDeviceInfo(&DeviceInstance);

				m_InputDevice[v].m_Device->GetObjectInfo(&ObjectInstance, dwHow, DIPH_BYOFFSET);

                Map(CurConfigObject, DeviceInstance.tszInstanceName, dwHow, dwFlags);

				printf("Cxbx: Detected %s %s on %s\n", szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName, ObjectInstance.dwType);

				sprintf(szStatus, "Success: %s Mapped to '%s%s' on '%s'!", m_DeviceNameLookup[CurConfigObject], szDirection, ObjectInstance.tszName, DeviceInstance.tszInstanceName);

                return true;
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
    // ******************************************************************
    // * Cleanup DirectInput
    // ******************************************************************
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
            if(m_ObjectConfig[r].dwDevice == v)
                inuse=true;

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
// * func: XBController::EnumGameCtrlCallback
// ******************************************************************
BOOL XBController::EnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi)
{
    HRESULT hRet = m_pDirectInput8->CreateDevice(lpddi->guidInstance, &m_InputDevice[m_dwInputDeviceCount].m_Device, NULL);
    
    if(!FAILED(hRet))
	{
		m_InputDevice[m_dwInputDeviceCount].m_Flags = DEVICE_FLAG_JOYSTICK;

        m_InputDevice[m_dwInputDeviceCount++].m_Device->SetDataFormat(&c_dfDIJoystick);
	}

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: XBController::EnumObjectsCallback
// ******************************************************************
BOOL XBController::EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi)
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

        HRESULT hRet = m_InputDevice[m_dwCurObject].m_Device->SetProperty(DIPROP_RANGE, &diprg.diph);

        if(FAILED(hRet))
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

// ******************************************************************
// * func: WrapEnumGameCtrlCallback
// ******************************************************************
BOOL CALLBACK WrapEnumGameCtrlCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    XBController *context = (XBController*)pvRef;

    return context->EnumGameCtrlCallback(lpddi);
}

// ******************************************************************
// * func: WrapEnumObjectsCallback
// ******************************************************************
BOOL CALLBACK WrapEnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
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
