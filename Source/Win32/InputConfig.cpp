// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->InputConfig.cpp
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
#include "InputConfig.h"

#include <windows.h>
#include <string.h>
#include <stdio.h>

// ******************************************************************
// * Input Device Name Lookup Table
// ******************************************************************
const char *g_InputDeviceName[INPUT_DEVICE_COMPONENT_COUNT]=
{
    // ******************************************************************
    // * Analog Axis
    // ******************************************************************
    "LThumbX", "LThumbY", "RThumbX", "RThumbY",

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

// ******************************************************************
// * Init
// ******************************************************************
void InputConfig::Init()
{
    for(int v=0;v<MAX_INPUT_DEVICES;v++)
        m_DeviceName[v][0] = '\0';

    for(int r=0;r<INPUT_DEVICE_COMPONENT_COUNT;r++)
    {
        m_InputMapping[r].dwDevice = -1;
        m_InputMapping[r].dwInfo   = -1;
        m_InputMapping[r].dwFlags  = 0;
    }
}

// ******************************************************************
// * Map a given input control mapping
// ******************************************************************
void InputConfig::Map(InputDeviceComponent idc, const char *DeviceName, int dwInfo, int dwFlags)
{
    // Initialize InputMapping instance
    m_InputMapping[idc].dwDevice = Insert(DeviceName);
    m_InputMapping[idc].dwInfo   = dwInfo;
    m_InputMapping[idc].dwFlags  = dwFlags;

    // Purse unused device slots
    for(int v=0;v<MAX_INPUT_DEVICES;v++)
    {
        bool inuse = false;

        for(int r=0;r<INPUT_DEVICE_COMPONENT_COUNT;r++)
            if(m_InputMapping[r].dwDevice == v)
                inuse=true;

        if(!inuse)
            m_DeviceName[v][0] = '\0';
    }

    return;
}

// ******************************************************************
// * Map a given input control mapping
// ******************************************************************
void InputConfig::Get(InputDeviceComponent idc, int *dwDevice, int *dwInfo, int *dwFlags)
{
    *dwDevice = m_InputMapping[idc].dwDevice;
    *dwInfo   = m_InputMapping[idc].dwInfo;
    *dwFlags  = m_InputMapping[idc].dwFlags;

    return;
}

// ******************************************************************
// * Find the look-up value for a DeviceName (creates if needed)
// ******************************************************************
int InputConfig::Insert(const char *DeviceName)
{
    int v=0;

    for(v=0;v<MAX_INPUT_DEVICES;v++)
        if(strcmp(m_DeviceName[v], DeviceName) == 0)
            return v;

    for(v=0;v<MAX_INPUT_DEVICES;v++)
    {
        if(m_DeviceName[v][0] == '\0')
        {
            strncpy(m_DeviceName[v], DeviceName, 255);

            return v;
        }
    }

    MessageBox(NULL, "Unexpected Circumstance (Too Many Controller Devices)! Please contact caustik!", "Cxbx", MB_OK | MB_ICONEXCLAMATION);

    ExitProcess(1);

    return 0;
}
