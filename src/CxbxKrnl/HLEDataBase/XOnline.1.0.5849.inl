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
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 5849, 15,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x02, 0xEC },

        { 0x10, 0xFC },
        { 0x18, 0xE9 },

        { 0x37, 0xA8 },
        { 0x38, 0x01 },
        { 0x39, 0x75 },
        { 0x3A, 0x0C },
        { 0x3B, 0xC7 },
        { 0x3C, 0x45 },
        { 0x3D, 0x0C },
        { 0x3E, 0x00 },
        { 0x3F, 0x10 },
        { 0x40, 0x15 },
        { 0x41, 0x80 },
        { 0x42, 0xE9 },
OOVPA_END;

// ******************************************************************
// * XOnline_5849
// ******************************************************************
OOVPATable XOnline_5849[] = {

	REGISTER_OOVPA(XnInit, 5788, XREF),
	REGISTER_OOVPA(XNetStartup, 5233, PATCH),
	REGISTER_OOVPA(WSAStartup, 5558, PATCH),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 4627, PATCH),
	REGISTER_OOVPA(XoUpdateLaunchNewImageInternal, 5788, XREF),
	REGISTER_OOVPA(socket, 5558, PATCH),
	REGISTER_OOVPA(bind, 4627, PATCH),
	REGISTER_OOVPA(listen, 4627, PATCH),
	REGISTER_OOVPA(ioctlsocket, 4627, PATCH),
	REGISTER_OOVPA(connect, 5233, PATCH),
	REGISTER_OOVPA(send, 3911, PATCH),
	REGISTER_OOVPA(recv, 3911, PATCH),
	REGISTER_OOVPA(CXo_XOnlineLogon, 5849, XREF),
	REGISTER_OOVPA(XOnlineLogon, 4361, PATCH),
};

// ******************************************************************
// * XOnline_5849_SIZE
// ******************************************************************
uint32 XOnline_5849_SIZE = sizeof(XOnline_5849);
