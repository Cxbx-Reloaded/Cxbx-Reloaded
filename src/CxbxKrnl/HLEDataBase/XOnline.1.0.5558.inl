// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.5558.inl
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
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 5558, 14,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x02, 0xEC },

        { 0x15, 0xE9 },
        { 0x1C, 0xF0 },

        { 0x27, 0xA8 },
        { 0x28, 0x01 },
        { 0x29, 0x75 },
        { 0x2A, 0x0C },
        { 0x2B, 0xC7 },
        { 0x2C, 0x45 },
//      { 0x2D, 0x10 },
        { 0x2E, 0x00 },
        { 0x2F, 0x10 },
        { 0x30, 0x15 },
        { 0x31, 0x80 },
        { 0x32, 0xE9 },
OOVPA_END;

#if 0 // No longer used, replaced by generic XNet 4361 version
// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_NO_XREF(WSAStartup, 5558, 7)

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x6A },
        { 0x0E, 0x68 },
        { 0x13, 0xE8 },
        { 0x18, 0xC2 },
        { 0x1A, 0x00 },
OOVPA_END;
#endif
#if 0 // Moved to XNet 5455
// ******************************************************************
// * CXnSock::socket
// ******************************************************************
OOVPA_NO_XREF(socket, 5558, 10) // Up to 5849

        { 0x00, 0x51 },
        { 0x0A, 0xB9 },

        { 0xB8, 0x83 },
        { 0xB9, 0xC8 },
        { 0xBA, 0xFF },
        { 0xBB, 0xEB },
        { 0xBC, 0x73 },
        { 0xBD, 0x53 },
        { 0xBE, 0xFF },
        { 0xBF, 0x15 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4361 version
// ******************************************************************
// * XOnlineLogon
// ******************************************************************
OOVPA_XREF(XOnlineLogon, 5558, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_CXo_XOnlineLogon ),

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x8B },
        { 0x04, 0x0D },
        { 0x09, 0x5D },
        { 0x0A, 0xE9 },
OOVPA_END;
#endif
