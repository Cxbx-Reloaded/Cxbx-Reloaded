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

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <string.h>

// ******************************************************************
// * Constructor
// ******************************************************************
InputConfig::InputConfig()
{
    for(int v=0;v<MAX_INPUT_DEVICES;v++)
        m_DeviceName[v][0] = '\0';
}

// ******************************************************************
// * Deconstructor
// ******************************************************************
InputConfig::~InputConfig()
{
}

// ******************************************************************
// * Map a given input control mapping
// ******************************************************************
void InputConfig::Map(InputMapping &IM, const char *DeviceName, int dwType, int dwFlags)
{
    // initialize IM
    IM.dwDevice = Insert(DeviceName);
    IM.dwType   = dwType;
    IM.dwFlags  = dwFlags;

    // purge unused slots
    for(int v=0;v<MAX_INPUT_DEVICES;v++)
    {
        if(m_LThumbX.dwDevice   != v && m_LThumbY.dwDevice   != v &&
           m_RThumbX.dwDevice   != v && m_RThumbY.dwDevice   != v &&
           m_X.dwDevice         != v && m_Y.dwDevice         != v &&
           m_A.dwDevice         != v && m_B.dwDevice         != v &&
           m_White.dwDevice     != v && m_Black.dwDevice     != v &&
           m_LTrigger.dwDevice  != v && m_RTrigger.dwDevice  != v &&
           m_DPadUp.dwDevice    != v && m_DPadDown.dwDevice  != v && 
           m_DPadLeft.dwDevice  != v && m_DPadRight.dwDevice != v &&
           m_Back.dwDevice      != v && m_Start.dwDevice     != v &&
           m_LThumb.dwDevice    != v && m_RThumb.dwDevice    != v)
        {
            m_DeviceName[v][0] = '\0';
        }
    }

    return;
}

// ******************************************************************
// * Find the look-up value for a DeviceName (creates if needed)
// ******************************************************************
int InputConfig::Insert(const char *DeviceName)
{
    int v=0;
    for(v=0;v<MAX_INPUT_DEVICES;v++)
        if(m_DeviceName[v][0] != '\0' && strcmp(DeviceName, m_DeviceName[v]) == 0)
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