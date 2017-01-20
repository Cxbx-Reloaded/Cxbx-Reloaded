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
// * XnInit -> Belongs in XNet lib
// ******************************************************************
//OOVPA_XREF(XnInit, 4627, 12,
//
//    XREF_XNINIT,
//    XRefZero)
//
//        // XnInit+0x03 : sub esp, 0x0218
//        { 0x03, 0x81 }, // (Offset,Value)-Pair #1
//        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
//        { 0x05, 0x18 }, // (Offset,Value)-Pair #3
//        { 0x06, 0x02 }, // (Offset,Value)-Pair #4
//
//        // XnInit+0x19 : cmpxchg [ecx], edx
//        { 0x19, 0x0F }, // (Offset,Value)-Pair #5
//        { 0x1A, 0xB1 }, // (Offset,Value)-Pair #6
//        { 0x1B, 0x11 }, // (Offset,Value)-Pair #7
//
//        // XnInit+0x3C : push 0x4454454E
//        { 0x3C, 0x68 }, // (Offset,Value)-Pair #8
//        { 0x3D, 0x4E }, // (Offset,Value)-Pair #9
//	{ 0x3E, 0x45 }, // (Offset,Value)-Pair #10
//	{ 0x3F, 0x54 }, // (Offset,Value)-Pair #11
//	{ 0x40, 0x44 }, // (Offset,Value)-Pair #12
//OOVPA_END;

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 4627, 8)

        { 0x08, 0x33 },
        { 0x10, 0x8A },
        { 0x19, 0x00 },
        { 0x22, 0x56 },
        { 0x2B, 0xFF },
        { 0x34, 0xF0 },
        { 0x3D, 0x24 },
        { 0x46, 0x5B },
OOVPA_END;

// ******************************************************************
// * XoUpdateLaunchNewImageInternal
// ******************************************************************
OOVPA_XREF(XoUpdateLaunchNewImageInternal, 4627, 8,

    XREF_XoUpdateLaunchNewImageInternal,
    XRefZero)

        { 0x1E, 0xDB },
        { 0x3E, 0x1B },
        { 0x5E, 0xF6 },
        { 0x81, 0x83 },
        { 0x9E, 0xE8 },
        { 0xBE, 0x50 },
        { 0xDE, 0xE8 },
        { 0xFE, 0x15 },
OOVPA_END;

// ******************************************************************
// * XOnline_4627
// ******************************************************************
OOVPATable XOnline_4627[] ={

    // XNetStartup (* unchanged since 4361 *)
	OOVPA_TABLE_ENTRY(XNetStartup, 4361),
	// WSAStartup
	OOVPA_TABLE_ENTRY(WSAStartup, 4361),
    // XnInit (XREF)
	OOVPA_TABLE_XREF(XnInit, 4627),
	// socket
	OOVPA_TABLE_PATCH_EmuThis(socket, 4361, socket),
	// bind (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH_EmuThis(bind, 4361, bind),
	// listen
	OOVPA_TABLE_PATCH_EmuThis(listen, 4361, listen),
	// ioctlsocket (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH_EmuThis(ioctlsocket, 4361, ioctlsocket),
	// XNetGetEthernetLinkStatus
	OOVPA_TABLE_ENTRY(XNetGetEthernetLinkStatus, 4627),
	// XoUpdateLaunchNewImageInternal (XREF)
	OOVPA_TABLE_XREF(XoUpdateLaunchNewImageInternal, 4627),
};

// ******************************************************************
// * XOnline_4627_SIZE
// ******************************************************************
uint32 XOnline_4627_SIZE = sizeof(XOnline_4627);
