// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XNet.1.0.4627.inl
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

#if 0 // No longer used, replaced by generic 4361 version
// ******************************************************************
// * XnInit
// ******************************************************************
OOVPA_XREF(XnInit, 4627, 18, 

    XREF_XnInit,
	XRefZero)

		// XnInit+0x0000 : push ebp
        { 0x0000, 0x55 },

        // XnInit+0x0003 : sub esp, 0x0218
        { 0x0003, 0x81 },
        { 0x0004, 0xEC },
        { 0x0005, 0x18 },
        { 0x0006, 0x02 },

        // XnInit+0x0019 : cmpxchg [ecx], edx
        { 0x0019, 0x0F },
        { 0x001A, 0xB1 },
        { 0x001B, 0x11 },

        // XnInit+0x003C : push 0x4E455444
        { 0x003C, 0x68 },
        { 0x003D, 0x4E },
        { 0x003E, 0x45 },

		// XnInit+0x01FA : OR DWORD PTR SS:[EBP-4],FFFFFFFF
        { 0x01FA, 0x83 },
        { 0x01FB, 0x4D },
        { 0x01FC, 0xFC },
        { 0x01FD, 0xFF },

		// XnInit+0x0203 : retn 18h
        { 0x0203, 0xC2 },
        { 0x0204, 0x18 },
        { 0x0205, 0x00 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4361 version
// ******************************************************************
// * XNetStartup
// ******************************************************************
OOVPA_XREF(XNetStartup, 4627, 1+7,

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
#endif
#if 0 // No longer used, replaced by generic 4361 version
// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_XREF(WSAStartup, 4627, 1+8,

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
#endif

// ******************************************************************
// * CXnSock::socket
// ******************************************************************
OOVPA_NO_XREF(socket, 4627, 14) // Up to 5344

        { 0x00, 0x51 },
        { 0x01, 0x85 },
        { 0x02, 0xC9 },
        { 0x03, 0x89 },
        { 0x04, 0x4C },
        { 0x05, 0x24 },
        { 0x06, 0x00 },
        { 0x07, 0x74 },
        { 0x08, 0x0A },
        { 0x09, 0x66 },
        { 0x0A, 0x83 },
        { 0x0B, 0xB9 },

        { 0x1E, 0xC8 },
        { 0x1F, 0xFF },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * socket
// ******************************************************************
OOVPA_NO_XREF(socket, 4627, 20)

        // socket+0x13 : push 0x276D
        { 0x0013, 0x68 },
        { 0x0014, 0x6D },
        { 0x0015, 0x27 },

        // socket+0x69 : cmp edi, 0x11
        { 0x0069, 0x83 },
        { 0x006A, 0xFF },
        { 0x006B, 0x11 },

		// socket+0x8C : SETNE DL
        { 0x008C, 0x0F },
        { 0x008D, 0x95 },
        { 0x008E, 0xC2 },

		// socket+0xAE : MOV EAX,273B
        { 0x00AE, 0xB8 },
        { 0x00AF, 0x3B },
        { 0x00B0, 0x27 },
        { 0x00B1, 0x00 },
        { 0x00B2, 0x00 },

        // socket+0x0116 : mov [esi+4], ecx
        { 0x0116, 0x89 },
        { 0x0117, 0x4E },
        { 0x0118, 0x04 },

		// socket+0x012D : retn 0C
        { 0x012D, 0xC2 },
        { 0x012E, 0x0C },
        { 0x012F, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * CXnSock::connect
// ******************************************************************
OOVPA_NO_XREF(connect, 4627, 24)

        // connect+0x00 : push ebp
        { 0x00, 0x55 },

		// connect+0x0B : je short +09
        { 0x0B, 0x74 },
		{ 0x0C, 0x09 },

		// connect+0x16 : PUSH 276D
        { 0x16, 0x68 },
		{ 0x17, 0x6D },
        { 0x18, 0x27 },
		{ 0x19, 0x00 },
		{ 0x1A, 0x00 },

		// connect+0x42 : TEST DL,40
        { 0x42, 0xF6 },
		{ 0x43, 0xC2 },
        { 0x44, 0x40 },

		// connect+0x68 : PUSH DWORD PTR SS:[EBP+8]
        { 0x68, 0xFF },
		{ 0x69, 0x75 },
        { 0x6A, 0x08 },

		// connect+0x8F : MOV ESI,80072733
        { 0x8F, 0xBE },
		{ 0x90, 0x33 },
        { 0x91, 0x27 },
		{ 0x92, 0x07 },
		{ 0x93, 0x80 },

		// connect+0xCD : PUSH -1
        { 0xCD, 0x6A },
		{ 0xCE, 0xFF },

		// connect+0xE6 : retn 0C
        { 0xE6, 0xC2 },
		{ 0xE7, 0x0C },
        { 0xE8, 0x00 },
OOVPA_END;

// ******************************************************************
// * CXnSock::send
// ******************************************************************
OOVPA_NO_XREF(send, 4627, 24)

        // send+0x00 : push ebp
        { 0x00, 0x55 },

		// send+0x17 : push 0x276D
        { 0x17, 0x68 },
        { 0x18, 0x6D },
        { 0x19, 0x27 },

		// send+0x24 : jmp
        { 0x24, 0xEB },
        { 0x25, 0x6A },

		// send+0x51 : LEA EAX,DWORD PTR SS:[EBP-20]
        { 0x51, 0x8D },
        { 0x52, 0x45 },
        { 0x53, 0xE0 },

		// send+0x6C : JL SHORT +04
        { 0x6C, 0x7C },
        { 0x6D, 0x04 },

		// send+0x7B : MOV DWORD PTR DS:[ESI+8],2B434F53
        { 0x7B, 0xC7 },
        { 0x7C, 0x46 },
        { 0x7D, 0x08 },
        { 0x7E, 0x53 },
        { 0x7F, 0x4F },
        { 0x80, 0x43 },
        { 0x81, 0x2B },

		// send+0x8C : MOV EAX,DWORD PTR SS:[EBP-10]
        { 0x8C, 0x8B },
        { 0x8D, 0x45 },
        { 0x8E, 0xF0 },

		// send+0x92 : retn 10h
        { 0x92, 0xC2 },
        { 0x93, 0x10 },
        { 0x94, 0x00 },
OOVPA_END;

// ******************************************************************
// * CXnSock::recv
// ******************************************************************
OOVPA_NO_XREF(recv, 4627, 24)

        // recv+0x00 : push ebp
        { 0x00, 0x55 },

		// recv+0x17 : push 0x276D
        { 0x17, 0x68 },
        { 0x18, 0x6D },
        { 0x19, 0x27 },

		// recv+0x24 : jmp
        { 0x24, 0xEB },
        { 0x25, 0x66 },

		// recv+0x51 : MOV DWORD PTR SS:[EBP-14],EAX
        { 0x51, 0x89 },
        { 0x52, 0x45 },
        { 0x53, 0xEC },

		// recv+0x68 : JL SHORT +04
        { 0x68, 0x7C },
        { 0x69, 0x04 },

		// recv+0x77 : MOV DWORD PTR DS:[ESI+8],2B434F53
        { 0x77, 0xC7 },
        { 0x78, 0x46 },
        { 0x79, 0x08 },
        { 0x7A, 0x53 },
        { 0x7B, 0x4F },
        { 0x7C, 0x43 },
        { 0x7D, 0x2B },

		// recv+0x88 : MOV EAX,DWORD PTR SS:[EBP+8]
        { 0x88, 0x8B },
        { 0x89, 0x45 },
        { 0x8A, 0x08 },

		// recv+0x8E : retn 10h
        { 0x8E, 0xC2 },
        { 0x8F, 0x10 },
        { 0x90, 0x00 },
OOVPA_END;

// ******************************************************************
// * CXnSock::ioctlsocket
// ******************************************************************
//Generic OOVPA as of 4627 and newer.
OOVPA_NO_XREF(ioctlsocket, 4627, 13)

        { 0x00, 0x55 },
        { 0x0C, 0x99 },
        { 0x1D, 0x83 },

        { 0x30, 0x3B },
        { 0x31, 0xF3 },
        { 0x32, 0x75 },
        { 0x33, 0x08 },
        { 0x34, 0x83 },
        { 0x35, 0xC8 },
        { 0x36, 0xFF },
        { 0x37, 0xE9 },
        { 0x38, 0x87 },

        { 0x57, 0x15 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * ioctlsocket
// ******************************************************************
OOVPA_NO_XREF(ioctlsocket, 4627, 10)

        // ioctlsocket+0x13 : push 0x276D
        { 0x13, 0x68 },
        { 0x14, 0x6D },
        { 0x15, 0x27 },

        // ioctlsocket+0x62 : lea edx, [esi+0x24]
        { 0x62, 0x8D },
        { 0x63, 0x56 },
        { 0x64, 0x24 },

        // ioctlsocket+0x81 : mov cl, al
        { 0x81, 0x8A },
        { 0x82, 0xC8 },

        // ioctlsocket+0xC6 : retn 0x0C
        { 0xC6, 0xC2 },
        { 0xC7, 0x0C },
OOVPA_END;
#endif

// ******************************************************************
// * CXnSock::bind
// ******************************************************************
//Generic OOVPA as of 4627 and newer.
OOVPA_NO_XREF(bind, 4627, 12)

        { 0x00, 0x56 },
        { 0x1D, 0xE8 },

        { 0x30, 0x27 },
        { 0x31, 0x00 },
        { 0x32, 0x00 },
        { 0x33, 0xEB },
        { 0x34, 0x32 },
        { 0x35, 0x8B },
        { 0x36, 0x4C },
        { 0x37, 0x24 },
        { 0x38, 0x10 },

        { 0x53, 0xE8 },
OOVPA_END;

// ******************************************************************
// * CXnSock::listen
// ******************************************************************
//Generic OOVPA as of 4627 and newer.
OOVPA_NO_XREF(listen, 4627, 13)

        { 0x00, 0x57 },
        { 0x09, 0xBF },
        { 0x1B, 0x83 },

        { 0x30, 0x83 },
        { 0x31, 0xC8 },
        { 0x32, 0xFF },
        { 0x33, 0xEB },
        { 0x34, 0x49 },
        { 0x35, 0x8B },
        { 0x36, 0x46 },
        { 0x37, 0x0C },
        { 0x38, 0xA8 },

        { 0x55, 0xE8 },
OOVPA_END;

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
