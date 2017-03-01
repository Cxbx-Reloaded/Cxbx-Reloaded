// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.4627.inl
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// * XNetStartup
// ******************************************************************
OOVPA_NO_XREF(XNetStartup, 5344, 8)

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
// * XNetGetEthernetLinkStatus
// ******************************************************************
#define XNetGetEthernetLinkStatus_5344 XNetGetEthernetLinkStatus_4627

// ******************************************************************
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 5344, 8,

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

// ******************************************************************
// * XOnlineLogon
// ******************************************************************
OOVPA_XREF(XOnlineLogon, 5344, 8,

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

// ******************************************************************
// * XOnline_4627
// ******************************************************************
OOVPATable XOnline_5233[] = {

	REGISTER_OOVPA(XNetStartup, 5344, PATCH),
	REGISTER_OOVPA(WSAStartup, 4361, PATCH),
	REGISTER_OOVPA(XnInit, 4627, XREF),
	// REGISTER_OOVPA(socket, 4361, EMUTHIS),
	REGISTER_OOVPA(bind, 4361, EMUTHIS),
	// REGISTER_OOVPA(listen, 4361, EMUTHIS),
	REGISTER_OOVPA(ioctlsocket, 4361, EMUTHIS),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 5344, PATCH),
	REGISTER_OOVPA(CXo_XOnlineLogon, 5344, XREF),
	REGISTER_OOVPA(XOnlineLogon, 5344, PATCH),
};

// ******************************************************************
// * XOnline_5233_SIZE
// ******************************************************************
uint32 XOnline_5233_SIZE = sizeof(XOnline_5233);
