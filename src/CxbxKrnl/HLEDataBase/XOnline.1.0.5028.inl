// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.5028.inl
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
OOVPA_XREF(CXo_XOnlineLogon, 5028, 15,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x02, 0xEC },

        { 0x15, 0x00 },
        { 0x1C, 0xE8 },

        { 0x21, 0xA8 },
        { 0x22, 0x01 },
        { 0x23, 0x75 },
        { 0x24, 0x0C },
        { 0x25, 0xC7 },
        { 0x26, 0x45 },
        { 0x27, 0xF8 },
        { 0x28, 0x00 },
        { 0x29, 0x10 },
        { 0x2A, 0x15 },
        { 0x2B, 0x80 },
        { 0x2C, 0xE9 },
OOVPA_END;

// ******************************************************************
// * XOnline_5028
// ******************************************************************
OOVPATable XOnline_5028[] ={

	REGISTER_OOVPA(XnInit, 4627, XREF),
	REGISTER_OOVPA(XNetStartup, 4361, PATCH),
	REGISTER_OOVPA(WSAStartup, 4361, PATCH),
	REGISTER_OOVPA(socket, 4627, PATCH),
	REGISTER_OOVPA(bind, 4627, PATCH),
	REGISTER_OOVPA(listen, 4627, PATCH),
	REGISTER_OOVPA(ioctlsocket, 4627, PATCH),
	REGISTER_OOVPA(connect, 3911, PATCH),
	REGISTER_OOVPA(send, 3911, PATCH),
	REGISTER_OOVPA(recv, 3911, PATCH),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 4627, PATCH),
	REGISTER_OOVPA(XoUpdateLaunchNewImageInternal, 4627, XREF),
	REGISTER_OOVPA(CXo_XOnlineLogon, 5028, XREF),
	REGISTER_OOVPA(XOnlineLogon, 4361, PATCH),
};

// ******************************************************************
// * XOnline_5028_SIZE
// ******************************************************************
uint32 XOnline_5028_SIZE = sizeof(XOnline_5028);
