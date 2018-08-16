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
#define LOG_PREFIX CXBXR_MODULE::XBPM

#undef FIELD_OFFSET     // prevent macro redefinition warnings
/* prevent name collisions */
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h>
};

#include <Shlwapi.h>
#include "CxbxKrnl/EmuShared.h"
#include "Common/Settings.hpp"
#include "Logging.h"

static Settings::s_controller_port g_ControllerPortMap;

void XBPortMappingSet(Settings::s_controller_port &controller_port_settings)
{
    g_ControllerPortMap = controller_port_settings;
}

// ******************************************************************
// * func: Save
// ******************************************************************
void XBPortMappingGet(Settings::s_controller_port &controller_port_settings)
{
    controller_port_settings = g_ControllerPortMap;
}

//Set HostType and HostPort setting from global array per xbox port. The setted value will take effect from next time xbe loading.
void SetXboxPortToHostPort(DWORD dwXboxPort, DWORD dwHostType, DWORD dwHostPort)
{
    //set host type and host port in global array per xbox port, will be used when xbe get reloaded.
    //only host type and host port can be set in this time. because the xbox DeviceType can only be determined when loading the xbe.
    g_ControllerPortMap.XboxPortMapHostType[dwXboxPort] = dwHostType;
    g_ControllerPortMap.XboxPortMapHostPort[dwXboxPort] = dwHostPort;
}
//retrieve HostType and HostPort setting from global array per xbox port.
void GetXboxPortToHostPort(DWORD dwXboxPort, DWORD &dwHostType, DWORD &dwHostPort)
{
    //get Host Type and Host Port per xbox port
    dwHostType = g_ControllerPortMap.XboxPortMapHostType[dwXboxPort];
    dwHostPort = g_ControllerPortMap.XboxPortMapHostPort[dwXboxPort];
}

DWORD GetXboxPortMapHostType(DWORD dwXboxPort)
{
    return g_ControllerPortMap.XboxPortMapHostType[dwXboxPort];
}

DWORD GetXboxPortMapHostPort(DWORD dwXboxPort)
{
    return g_ControllerPortMap.XboxPortMapHostPort[dwXboxPort];
}
