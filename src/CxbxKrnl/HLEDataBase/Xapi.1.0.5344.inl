// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5344.cpp
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

#if 0 // Moved to 4831
// ******************************************************************
// * XInputGetDeviceDescription
// ******************************************************************
OOVPA_NO_XREF(XInputGetDeviceDescription, 5344, 14)

        { 0x04, 0xEC },
        { 0x0B, 0x15 },

        { 0x13, 0x45 },
        { 0x14, 0x08 },
        { 0x15, 0x8B },
        { 0x16, 0x30 },
        { 0x17, 0x3B },
        { 0x18, 0xF3 },
        { 0x19, 0x88 },
        { 0x1A, 0x4D },
        { 0x1B, 0xFF },
        { 0x1C, 0x0F },
        { 0x1D, 0x84 },

        { 0x30, 0x45 },
        //{ 0x31, 0xF8 }, // 4831 0xF4 vs 5344 0xF8
OOVPA_END;
#endif

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 5344, 11)

        { 0x00, 0xB8 },

        { 0x18, 0x75 },
        { 0x19, 0x19 },
        { 0x1A, 0x83 },
        { 0x1B, 0xC1 },
        { 0x1C, 0x04 },
        { 0x1D, 0x3B },
        { 0x1E, 0xC8 },
        { 0x1F, 0x72 },

        { 0x30, 0xC2 },
        { 0x41, 0xEE },
OOVPA_END;
