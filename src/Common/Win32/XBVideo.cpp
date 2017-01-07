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
// *   Cxbx->Win32->XBVideo.cpp
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
#include "XBVideo.h"

#include <cstdio>

// ******************************************************************
// * func: XBVideo::XBVideo
// ******************************************************************
XBVideo::XBVideo() : m_bVSync(false), m_bFullscreen(false), m_bHardwareYUV(false)
{
    strcpy(m_szVideoResolution, "Automatic (Default)");
}

// ******************************************************************
// * func: XBVideo::~XBVideo
// ******************************************************************
XBVideo::~XBVideo()
{
}

// ******************************************************************
// * func: XBVideo::Load
// ******************************************************************
void XBVideo::Load(const char *szRegistryKey)
{
    // ******************************************************************
    // * Load Configuration from Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_SZ; dwSize = sizeof(m_szVideoResolution);
            RegQueryValueEx(hKey, "VideoResolution", NULL, &dwType, (PBYTE)m_szVideoResolution, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_dwDisplayAdapter);
            RegQueryValueEx(hKey, "DisplayAdapter", NULL, &dwType, (PBYTE)&m_dwDisplayAdapter, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_dwDirect3DDevice);
            RegQueryValueEx(hKey, "Direct3DDevice", NULL, &dwType, (PBYTE)&m_dwDirect3DDevice, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bFullscreen);
            RegQueryValueEx(hKey, "Fullscreen", NULL, &dwType, (PBYTE)&m_bFullscreen, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bVSync);
            RegQueryValueEx(hKey, "VSync", NULL, &dwType, (PBYTE)&m_bVSync, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bHardwareYUV);
            RegQueryValueEx(hKey, "HardwareYUV", NULL, &dwType, (PBYTE)&m_bHardwareYUV, &dwSize);

            RegCloseKey(hKey);
        }
    }
}

// ******************************************************************
// * func: XBVideo::Save
// ******************************************************************
void XBVideo::Save(const char *szRegistryKey)
{
    // ******************************************************************
    // * Save Configuration to Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            dwType = REG_SZ; dwSize = sizeof(m_szVideoResolution);
            RegSetValueEx(hKey, "VideoResolution", 0, dwType, (PBYTE)m_szVideoResolution, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_dwDisplayAdapter);
            RegSetValueEx(hKey, "DisplayAdapter", 0, dwType, (PBYTE)&m_dwDisplayAdapter, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_dwDirect3DDevice);
            RegSetValueEx(hKey, "Direct3DDevice", 0, dwType, (PBYTE)&m_dwDirect3DDevice, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bFullscreen);
            RegSetValueEx(hKey, "Fullscreen", 0, dwType, (PBYTE)&m_bFullscreen, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bVSync);
            RegSetValueEx(hKey, "VSync", 0, dwType, (PBYTE)&m_bVSync, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bHardwareYUV);
            RegSetValueEx(hKey, "HardwareYUV", 0, dwType, (PBYTE)&m_bHardwareYUV, dwSize);

            RegCloseKey(hKey);
        }
    }
}
