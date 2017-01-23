// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.5849.inl
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
OOVPA_XREF(XnInit, 5849, 8,

    XREF_XNINIT,
    XRefZero)

        { 0x1E, 0x3B },
        { 0x3E, 0x45 },
        { 0x5E, 0x00 },
        { 0x81, 0x57 },
        { 0xA2, 0x57 },
        { 0xC0, 0x33 },
        { 0xDE, 0xFE },
        { 0xFE, 0x48 },
OOVPA_END;

// ******************************************************************
// * XNetStartup
// ******************************************************************
OOVPA_NO_XREF(XNetStartup, 5849, 8)

        { 0x01, 0xC0 },
        { 0x04, 0x50 },
        { 0x07, 0x24 },
        { 0x0A, 0x68 },
        { 0x0F, 0xE8 },
        { 0x14, 0xC2 },
        { 0x15, 0x04 },
        { 0x16, 0x00 },
OOVPA_END;

// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_NO_XREF(WSAStartup, 5849, 7)

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x6A },
        { 0x0E, 0x68 },
        { 0x13, 0xE8 },
        { 0x18, 0xC2 },
        { 0x1A, 0x00 },
OOVPA_END;

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
#define XNetGetEthernetLinkStatus_5849 XNetGetEthernetLinkStatus_4627

// ******************************************************************
// * XOnline_5849
// ******************************************************************
OOVPATable XOnline_5849[] = {
    // XNetStartup
	OOVPA_TABLE_ENTRY(XNetStartup, 5849),
	// WSAStartup
	OOVPA_TABLE_ENTRY(WSAStartup, 5849),
    // XnInit (XREF)
	OOVPA_TABLE_XREF(XnInit, 5849),
	// XNetGetEthernetLinkStatus
	OOVPA_TABLE_ENTRY(XNetGetEthernetLinkStatus, 5849),
};

// ******************************************************************
// * XOnline_5849_SIZE
// ******************************************************************
uint32 XOnline_5849_SIZE = sizeof(XOnline_5849);
