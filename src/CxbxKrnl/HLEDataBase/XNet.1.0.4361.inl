// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XNet.1.0.4361.cpp
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

// ******************************************************************
// * XnInit
// ******************************************************************
//Generic OOVPA as of 4361 and newer.
OOVPA_XREF(XnInit, 4361, 15,

    XREF_XnInit,
    XRefZero)

        { 0x00, 0x55 },

        { 0x14, 0xBA },
        { 0x15, 0x01 },
        { 0x16, 0x00 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x0F },
        { 0x1A, 0xB1 },
        { 0x1B, 0x11 },
        { 0x1C, 0x33 },
        { 0x1D, 0xF6 },
        { 0x1E, 0x3B },
        { 0x1F, 0xC6 },

        { 0x3C, 0x68 },
        { 0x3D, 0x4E },
OOVPA_END;

// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_XREF(WSAStartup, 4361, 9,

    XRefNoSaveIndex,
    XRefOne)

        // WSAStartup+0x0F : call [XnInit]
        XREF_ENTRY( 0x14, XREF_XnInit ),

        // WSAStartup+0x00 : push [esp+0x08]
        { 0x00, 0xFF },
        { 0x01, 0x74 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },

        // WSAStartup+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
OOVPA_END;

// ******************************************************************
// * XNetStartup
// ******************************************************************
OOVPA_XREF(XNetStartup, 4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        // XNetStartup+0x0F : call [XnInit]
        XREF_ENTRY( 0x10, XREF_XnInit ),

        // XNetStartup+0x00 : xor eax, eax
        { 0x00, 0x33 },
        { 0x01, 0xC0 },

        // XNetStartup+0x02 : push eax
        { 0x02, 0x50 },

        // XNetStartup+0x03 : push eax
        { 0x03, 0x50 },

        // XNetStartup+0x04 : push eax
        { 0x04, 0x50 },

        // XNetStartup+0x14 : retn 0x04
        { 0x14, 0xC2 },
        { 0x15, 0x04 },
OOVPA_END;

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 4361, 13)

        { 0x00, 0x56 },
        { 0x01, 0x33 },
        { 0x02, 0xF3 },
        { 0x03, 0xB8 },
        { 0x04, 0x00 },
        { 0x05, 0x00 },
        { 0x06, 0x00 },
        { 0x07, 0x00 },
        { 0x08, 0xB9 },

        { 0x0D, 0xBA },
        { 0x1A, 0xE8 },

        { 0x27, 0x75 },
        { 0x2C, 0x15 },
OOVPA_END;
