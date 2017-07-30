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
// *   Cxbx->Common->Win32->XBAudio.cpp
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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#include "XBAudio.h"

#include <cstdio>

// ******************************************************************
// * func: XBAudio::XBAudio
// ******************************************************************
XBAudio::XBAudio() : m_bLegacyAudioHack(false), m_bPCM(true), m_bXADPCM(false), m_bUnknownCodec(false)
{
    m_binAudioAdapter = { 0 };
}

// ******************************************************************
// * func: XBAudio::~XBAudio
// ******************************************************************
XBAudio::~XBAudio()
{
}

// ******************************************************************
// * func: XBAudio::Load
// ******************************************************************
void XBAudio::Load(const char *szRegistryKey)
{
    // ******************************************************************
    // * Load Configuration from Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {

            dwType = REG_DWORD; dwSize = sizeof(m_binAudioAdapter);
            RegQueryValueEx(hKey, "AudioAdapter", NULL, &dwType, (PBYTE)&m_binAudioAdapter, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bLegacyAudioHack);
            RegQueryValueEx(hKey, "LegacyAudioHack", NULL, &dwType, (PBYTE)&m_bLegacyAudioHack, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bPCM);
            RegQueryValueEx(hKey, "PCM", NULL, &dwType, (PBYTE)&m_bPCM, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bXADPCM);
            RegQueryValueEx(hKey, "XADPCM", NULL, &dwType, (PBYTE)&m_bXADPCM, &dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bUnknownCodec);
            RegQueryValueEx(hKey, "UnknownCodec", NULL, &dwType, (PBYTE)&m_bUnknownCodec, &dwSize);

            RegCloseKey(hKey);
        }
    }
}

// ******************************************************************
// * func: XBAudio::Save
// ******************************************************************
void XBAudio::Save(const char *szRegistryKey)
{
    // ******************************************************************
    // * Save Configuration to Registry
    // ******************************************************************
    {
        DWORD   dwDisposition, dwType, dwSize;
        HKEY    hKey;

        if(RegCreateKeyEx(HKEY_CURRENT_USER, szRegistryKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {

            dwType = REG_BINARY; dwSize = sizeof(m_binAudioAdapter);
            RegSetValueEx(hKey, "AudioAdapter", 0, dwType, (PBYTE)&m_binAudioAdapter, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bLegacyAudioHack);
            RegSetValueEx(hKey, "LegacyAudioHack", 0, dwType, (PBYTE)&m_bLegacyAudioHack, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bPCM);
            RegSetValueEx(hKey, "PCM", 0, dwType, (PBYTE)&m_bPCM, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bXADPCM);
            RegSetValueEx(hKey, "XADPCM", 0, dwType, (PBYTE)&m_bXADPCM, dwSize);

            dwType = REG_DWORD; dwSize = sizeof(m_bUnknownCodec);
            RegSetValueEx(hKey, "UnknownCodec", 0, dwType, (PBYTE)&m_bUnknownCodec, dwSize);

            RegCloseKey(hKey);
        }
    }
}
