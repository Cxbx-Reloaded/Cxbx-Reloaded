// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.5788.inl
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
OOVPA_XREF(XnInit, 5788, 8,

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
// * XoUpdateLaunchNewImageInternal
// ******************************************************************
OOVPA_XREF(XoUpdateLaunchNewImageInternal, 5788, 16,

    XREF_XoUpdateLaunchNewImageInternal,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0xF0 },
        { 0x06, 0x02 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x53 },
        { 0x0A, 0x56 },
        { 0x0B, 0x8B },
        { 0x0C, 0x75 },
        { 0x0D, 0x08 },

        { 0x6E, 0xEB },
        { 0x88, 0x3D },
OOVPA_END;

// ******************************************************************
// * XOnline_5788
// ******************************************************************
OOVPATable XOnline_5788[] = {

	REGISTER_OOVPA(XnInit, 5788, XREF),
	REGISTER_OOVPA(XNetStartup, 5233, PATCH),
	REGISTER_OOVPA(WSAStartup, 5558, PATCH),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 4627, PATCH),
	REGISTER_OOVPA(socket, 5558, PATCH),
	REGISTER_OOVPA(bind, 4627, PATCH),
	REGISTER_OOVPA(listen, 4627, PATCH),
	REGISTER_OOVPA(ioctlsocket, 4627, PATCH),
	REGISTER_OOVPA(connect, 5233, PATCH),
	REGISTER_OOVPA(send, 3911, PATCH),
	REGISTER_OOVPA(recv, 3911, PATCH),
	REGISTER_OOVPA(XoUpdateLaunchNewImageInternal, 5788, XREF),
	REGISTER_OOVPA(CXo_XOnlineLogon, 5558, XREF),
	REGISTER_OOVPA(XOnlineLogon, 4361, PATCH),
};

// ******************************************************************
// * XOnline_5788_SIZE
// ******************************************************************
uint32 XOnline_5788_SIZE = sizeof(XOnline_5788);
