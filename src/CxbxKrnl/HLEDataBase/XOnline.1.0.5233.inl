// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XOnline.1.0.5233.inl
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

#if 0 // No longer used, replaced by generic XNet 4361 version
// ******************************************************************
// * XNetStartup
// ******************************************************************
OOVPA_NO_XREF(XNetStartup, 5233, 8)

        { 0x01, 0xC0 },
        { 0x04, 0x50 },
        { 0x07, 0x24 },
        { 0x0A, 0x68 },
        { 0x0F, 0xE8 },
        { 0x14, 0xC2 },
        { 0x15, 0x04 },
        { 0x16, 0x00 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 5233, 8,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x21, 0xA8 },
        { 0x3E, 0x80 },
        { 0x5E, 0x00 },
        { 0x7E, 0x04 },
        { 0xA2, 0x85 },
        { 0xBE, 0xEC },
        { 0xDE, 0xF9 },
        { 0xFE, 0x33 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4361 version
// ******************************************************************
// * XOnlineLogon
// ******************************************************************
OOVPA_XREF(XOnlineLogon, 5233, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_CXo_XOnlineLogon ),

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x5D },
        { 0x04, 0x8B },
        { 0x05, 0x0D },
        { 0x0A, 0xE9 },
OOVPA_END;
#endif
#if 0 // Moved to XNet 5120
// ******************************************************************
// * CXnSock::connect
// ******************************************************************
OOVPA_NO_XREF(connect, 5233, 12) // Up to 5849

        { 0x00, 0x55 },
        { 0x19, 0xE8 },

        { 0x30, 0xF0 },
        { 0x31, 0x85 },
        { 0x32, 0xF6 },
        { 0x33, 0x75 },
        { 0x34, 0x08 },
        { 0x35, 0x83 },
        { 0x36, 0xC8 },
        { 0x37, 0xFF },
        { 0x38, 0xE9 },

        { 0x61, 0xE8 },
OOVPA_END;
#endif
