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
// *   Cxbx->Win32->Common->XBPortMapping.cpp
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

#define LOG_PREFIX "XBPM"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
/* prevent name collisions */
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include <Shlwapi.h>
//#include "CxbxKrnl.h"
//#include "Logging.h"
//#include "Emu.h"
//#include "EmuKrnl.h" // For DefaultLaunchDataPage
//#include "EmuFile.h"
//#include "EmuFS.h"
//#include "EmuShared.h"
//#include "HLEIntercept.h"
//#include "CxbxVSBC/CxbxVSBC.h"
//#include "Windef.h"
//#include <vector>


DWORD g_XboxPortMapHostType[] = { 1,1,1,1 };
DWORD g_XboxPortMapHostPort[] = { 0,1,2,3 };

// XInputSetState status waiters

void XBPortMappingLoad(const char *szRegistryKey)
{
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            LSTATUS result;
            result = RegQueryValueEx(hKey, "XboxPort0HostType", NULL, &dwType, (PBYTE)&g_XboxPortMapHostType[0], &dwSize);
            if (result != ERROR_SUCCESS) {
                //default to use XInput
                g_XboxPortMapHostType[0] = 1;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort0HostPort", NULL, &dwType, (PBYTE)&g_XboxPortMapHostPort[0], &dwSize);
            if (result != ERROR_SUCCESS) {
                g_XboxPortMapHostPort[0] = 0;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort1HostType", NULL, &dwType, (PBYTE)&g_XboxPortMapHostType[1], &dwSize);
            if (result != ERROR_SUCCESS) {
                //default to use XInput
                g_XboxPortMapHostType[1] = 1;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort1HostPort", NULL, &dwType, (PBYTE)&g_XboxPortMapHostPort[1], &dwSize);
            if (result != ERROR_SUCCESS) {
                g_XboxPortMapHostPort[1] = 1;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort2HostType", NULL, &dwType, (PBYTE)&g_XboxPortMapHostType[2], &dwSize);
            if (result != ERROR_SUCCESS) {
                //default to use XInput
                g_XboxPortMapHostType[2] = 1;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort2HostPort", NULL, &dwType, (PBYTE)&g_XboxPortMapHostPort[2], &dwSize);
            if (result != ERROR_SUCCESS) {
                g_XboxPortMapHostPort[2] = 2;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort3HostType", NULL, &dwType, (PBYTE)&g_XboxPortMapHostType[3], &dwSize);
            if (result != ERROR_SUCCESS) {
                //default to use XInput
                g_XboxPortMapHostType[3] = 1;
            }

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, "XboxPort3HostPort", NULL, &dwType, (PBYTE)&g_XboxPortMapHostPort[3], &dwSize);
            if (result != ERROR_SUCCESS) {
                g_XboxPortMapHostPort[3] = 3;
            }

            RegCloseKey(hKey);
        }
    }
}

// ******************************************************************
// * func: Save
// ******************************************************************
void XBPortMappingSave(const char *szRegistryKey)
{
    // ******************************************************************
    // * Save Configuration to Registry
    // ******************************************************************
    //if (g_SaveOnExit) {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort0HostType", 0, dwType, (PBYTE)&g_XboxPortMapHostType[0], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort0HostPort", 0, dwType, (PBYTE)&g_XboxPortMapHostPort[0], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort1HostType", 0, dwType, (PBYTE)&g_XboxPortMapHostType[1], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort1HostPort", 0, dwType, (PBYTE)&g_XboxPortMapHostPort[1], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort2HostType", 0, dwType, (PBYTE)&g_XboxPortMapHostType[2], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort2HostPort", 0, dwType, (PBYTE)&g_XboxPortMapHostPort[2], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort3HostType", 0, dwType, (PBYTE)&g_XboxPortMapHostType[3], dwSize);

            dwType = REG_DWORD; dwSize = sizeof(DWORD);
            RegSetValueEx(hKey, "XboxPort3HostPort", 0, dwType, (PBYTE)&g_XboxPortMapHostPort[3], dwSize);


            RegCloseKey(hKey);
        }
    //}
}

//Set HostType and HostPort setting from global array per xbox port. The setted value will take effect from next time xbe loading.
void SetXboxPortToHostPort(DWORD dwXboxPort, DWORD dwHostType, DWORD dwHostPort)
{
    //set host type and host port in global array per xbox port, will be used when xbe get reloaded.
    //only host type and host port can be set in this time. because the xbox DeviceType can only be determined when loading the xbe.
    g_XboxPortMapHostType[dwXboxPort] = dwHostType;
    g_XboxPortMapHostPort[dwXboxPort] = dwHostPort;
}
//retrieve HostType and HostPort setting from global array per xbox port.
void GetXboxPortToHostPort(DWORD dwXboxPort, DWORD &dwHostType, DWORD &dwHostPort)
{
    //get Host Type and Host Port per xbox port
    dwHostType = g_XboxPortMapHostType[dwXboxPort];
    dwHostPort = g_XboxPortMapHostPort[dwXboxPort];
}

DWORD GetXboxPortMapHostType(DWORD dwXboxPort)
{
    return g_XboxPortMapHostType[dwXboxPort];
}

DWORD GetXboxPortMapHostPort(DWORD dwXboxPort)
{
    return g_XboxPortMapHostPort[dwXboxPort];
}
