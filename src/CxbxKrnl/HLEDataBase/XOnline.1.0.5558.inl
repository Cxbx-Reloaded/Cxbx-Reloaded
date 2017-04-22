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

// ******************************************************************
// * XOnline_5558
// ******************************************************************
// * TODO: Verify all of these
// ******************************************************************
OOVPATable XOnline_5558[] = {

	REGISTER_OOVPA(XNetStartup, 5233, PATCH),
	REGISTER_OOVPA(WSAStartup, 5558, PATCH),
	REGISTER_OOVPA(XnInit, 4627, XREF),
	// REGISTER_OOVPA(socket, 4361, EMUTHIS),
	REGISTER_OOVPA(bind, 4361, EMUTHIS),
	// REGISTER_OOVPA(listen, 4361, EMUTHIS),
	REGISTER_OOVPA(ioctlsocket, 4361, EMUTHIS),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 4627, PATCH),
};

// ******************************************************************
// * XOnline_5558_SIZE
// ******************************************************************
uint32 XOnline_5558_SIZE = sizeof(XOnline_5558);
