// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Common->XBPortMapping.h
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
#ifndef XBPORTMAPPING_H
#define XBPORTMAPPING_H

extern void SetXboxPortToHostPort(DWORD dwXboxPort, DWORD dwHostType, DWORD dwHostPort);

extern void GetXboxPortToHostPort(DWORD dwXboxPort, DWORD &dwHostType, DWORD &dwHostPort);

extern DWORD GetXboxPortMapHostType(DWORD dwXboxPort);

extern DWORD GetXboxPortMapHostPort(DWORD dwXboxPort);
// ******************************************************************
// * Set configuration
// ******************************************************************
extern void XBPortMappingSet(Settings::s_controller_port &controller_port_settings);

// ******************************************************************
// * Get configuration
// ******************************************************************
extern void XBPortMappingGet(Settings::s_controller_port &controller_port_settings);
#endif
