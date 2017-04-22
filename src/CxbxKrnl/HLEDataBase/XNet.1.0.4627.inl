// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XNet.1.0.4627.cpp
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
OOVPA_XREF(XnInit, 4627, 18, 

    XREF_XNINIT,
	XRefZero)

		// XnInit+0x0000 : push ebp
        { 0x0000, 0x55 }, // (Offset,Value)-Pair #1

        // XnInit+0x0003 : sub esp, 0x0218
        { 0x0003, 0x81 }, // (Offset,Value)-Pair #2
        { 0x0004, 0xEC }, // (Offset,Value)-Pair #3
        { 0x0005, 0x18 }, // (Offset,Value)-Pair #4
        { 0x0006, 0x02 }, // (Offset,Value)-Pair #5

        // XnInit+0x0019 : cmpxchg [ecx], edx
        { 0x0019, 0x0F }, // (Offset,Value)-Pair #6
        { 0x001A, 0xB1 }, // (Offset,Value)-Pair #7
        { 0x001B, 0x11 }, // (Offset,Value)-Pair #8

        // XnInit+0x003C : push 0x4E455444
        { 0x003C, 0x68 }, // (Offset,Value)-Pair #9
        { 0x003D, 0x4E }, // (Offset,Value)-Pair #10
        { 0x003E, 0x45 }, // (Offset,Value)-Pair #11

		// XnInit+0x01FA : OR DWORD PTR SS:[EBP-4],FFFFFFFF
        { 0x01FA, 0x83 }, // (Offset,Value)-Pair #12
        { 0x01FB, 0x4D }, // (Offset,Value)-Pair #13
        { 0x01FC, 0xFC }, // (Offset,Value)-Pair #14
        { 0x01FD, 0xFF }, // (Offset,Value)-Pair #15

		// XnInit+0x0203 : retn 18h
        { 0x0203, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x0204, 0x18 }, // (Offset,Value)-Pair #17
        { 0x0205, 0x00 }, // (Offset,Value)-Pair #18
OOVPA_END;

// ******************************************************************
// * XNetStartup
// ******************************************************************
OOVPA_XREF(XNetStartup, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        // XNetStartup+0x0F : call [XnInit]
        XREF_ENTRY( 0x10, XREF_XNINIT ),  // (Offset,Value)-Pair #1

        // XNetStartup+0x00 : xor eax, eax
        { 0x00, 0x33 }, // (Offset,Value)-Pair #2
        { 0x01, 0xC0 }, // (Offset,Value)-Pair #3

        // XNetStartup+0x02 : push eax
        { 0x02, 0x50 }, // (Offset,Value)-Pair #4

        // XNetStartup+0x03 : push eax
        { 0x03, 0x50 }, // (Offset,Value)-Pair #5

        // XNetStartup+0x04 : push eax
        { 0x04, 0x50 }, // (Offset,Value)-Pair #6

        // XNetStartup+0x14 : retn 0x04
        { 0x14, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x15, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_XREF(WSAStartup, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        // WSAStartup+0x0F : call [XnInit]
        XREF_ENTRY( 0x14, XREF_XNINIT ),  // (Offset,Value)-Pair #1

        // WSAStartup+0x00 : push [esp+0x08]
        { 0x00, 0xFF }, // (Offset,Value)-Pair #2
        { 0x01, 0x74 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x08 }, // (Offset,Value)-Pair #5

        // WSAStartup+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #6
        { 0x05, 0x74 }, // (Offset,Value)-Pair #7
        { 0x06, 0x24 }, // (Offset,Value)-Pair #8
        { 0x07, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

///////
// ******************************************************************
// * socket
// ******************************************************************
OOVPA_NO_XREF(socket, 4627, 20)

        // socket+0x13 : push 0x276D
        { 0x0013, 0x68 }, // (Offset,Value)-Pair #1
        { 0x0014, 0x6D }, // (Offset,Value)-Pair #2
        { 0x0015, 0x27 }, // (Offset,Value)-Pair #3

        // socket+0x69 : cmp edi, 0x11
        { 0x0069, 0x83 }, // (Offset,Value)-Pair #4
        { 0x006A, 0xFF }, // (Offset,Value)-Pair #5
        { 0x006B, 0x11 }, // (Offset,Value)-Pair #6

		// socket+0x8C : SETNE DL
        { 0x008C, 0x0F }, // (Offset,Value)-Pair #7
        { 0x008D, 0x95 }, // (Offset,Value)-Pair #8
        { 0x008E, 0xC2 }, // (Offset,Value)-Pair #9

		// socket+0xAE : MOV EAX,273B
        { 0x00AE, 0xB8 }, // (Offset,Value)-Pair #10
        { 0x00AF, 0x3B }, // (Offset,Value)-Pair #11
        { 0x00B0, 0x27 }, // (Offset,Value)-Pair #12
        { 0x00B1, 0x00 }, // (Offset,Value)-Pair #13
        { 0x00B2, 0x00 }, // (Offset,Value)-Pair #14

        // socket+0x0116 : mov [esi+4], ecx
        { 0x0116, 0x89 }, // (Offset,Value)-Pair #15
        { 0x0117, 0x4E }, // (Offset,Value)-Pair #16
        { 0x0118, 0x04 }, // (Offset,Value)-Pair #17

		// socket+0x012D : retn 0C
        { 0x012D, 0xC2 }, // (Offset,Value)-Pair #18
        { 0x012E, 0x0C }, // (Offset,Value)-Pair #19
        { 0x012F, 0x00 }, // (Offset,Value)-Pair #20
OOVPA_END;

// ******************************************************************
// * socket
// ******************************************************************
OOVPA_NO_XREF(connect, 4627, 24)

        // connect+0x00 : push ebp
        { 0x00, 0x55 }, // (Offset,Value)-Pair #1

		// connect+0x0B : je short +09
        { 0x0B, 0x74 }, // (Offset,Value)-Pair #2
		{ 0x0C, 0x09 }, // (Offset,Value)-Pair #3

		// connect+0x16 : PUSH 276D
        { 0x16, 0x68 }, // (Offset,Value)-Pair #4
		{ 0x17, 0x6D }, // (Offset,Value)-Pair #5
        { 0x18, 0x27 }, // (Offset,Value)-Pair #6
		{ 0x19, 0x00 }, // (Offset,Value)-Pair #7
		{ 0x1A, 0x00 }, // (Offset,Value)-Pair #8

		// connect+0x42 : TEST DL,40
        { 0x42, 0xF6 }, // (Offset,Value)-Pair #9
		{ 0x43, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x44, 0x40 }, // (Offset,Value)-Pair #11

		// connect+0x68 : PUSH DWORD PTR SS:[EBP+8]
        { 0x68, 0xFF }, // (Offset,Value)-Pair #12
		{ 0x69, 0x75 }, // (Offset,Value)-Pair #13
        { 0x6A, 0x08 }, // (Offset,Value)-Pair #14

		// connect+0x8F : MOV ESI,80072733
        { 0x8F, 0xBE }, // (Offset,Value)-Pair #15
		{ 0x90, 0x33 }, // (Offset,Value)-Pair #16
        { 0x91, 0x27 }, // (Offset,Value)-Pair #17
		{ 0x92, 0x07 }, // (Offset,Value)-Pair #18
		{ 0x93, 0x80 }, // (Offset,Value)-Pair #19

		// connect+0xCD : PUSH -1
        { 0xCD, 0x6A }, // (Offset,Value)-Pair #20
		{ 0xCE, 0xFF }, // (Offset,Value)-Pair #21

		// connect+0xE6 : retn 0C
        { 0xE6, 0xC2 }, // (Offset,Value)-Pair #22
		{ 0xE7, 0x0C }, // (Offset,Value)-Pair #23
        { 0xE8, 0x00 }, // (Offset,Value)-Pair #24
OOVPA_END;

// ******************************************************************
// * send
// ******************************************************************
OOVPA_NO_XREF(send, 4627, 24)

        // send+0x00 : push ebp
        { 0x00, 0x55 }, // (Offset,Value)-Pair #1

		// send+0x17 : push 0x276D
        { 0x17, 0x68 }, // (Offset,Value)-Pair #2
        { 0x18, 0x6D }, // (Offset,Value)-Pair #3
        { 0x19, 0x27 }, // (Offset,Value)-Pair #4

		// send+0x24 : jmp
        { 0x24, 0xEB }, // (Offset,Value)-Pair #5
        { 0x25, 0x6A }, // (Offset,Value)-Pair #6

		// send+0x51 : LEA EAX,DWORD PTR SS:[EBP-20]
        { 0x51, 0x8D }, // (Offset,Value)-Pair #7
        { 0x52, 0x45 }, // (Offset,Value)-Pair #8
        { 0x53, 0xE0 }, // (Offset,Value)-Pair #9

		// send+0x6C : JL SHORT +04
        { 0x6C, 0x7C }, // (Offset,Value)-Pair #10
        { 0x6D, 0x04 }, // (Offset,Value)-Pair #11

		// send+0x7B : MOV DWORD PTR DS:[ESI+8],2B434F53
        { 0x7B, 0xC7 }, // (Offset,Value)-Pair #12
        { 0x7C, 0x46 }, // (Offset,Value)-Pair #13
        { 0x7D, 0x08 }, // (Offset,Value)-Pair #14
        { 0x7E, 0x53 }, // (Offset,Value)-Pair #15
        { 0x7F, 0x4F }, // (Offset,Value)-Pair #16
        { 0x80, 0x43 }, // (Offset,Value)-Pair #17
        { 0x81, 0x2B }, // (Offset,Value)-Pair #18

		// send+0x8C : MOV EAX,DWORD PTR SS:[EBP-10]
        { 0x8C, 0x8B }, // (Offset,Value)-Pair #19
        { 0x8D, 0x45 }, // (Offset,Value)-Pair #20
        { 0x8E, 0xF0 }, // (Offset,Value)-Pair #21

		// send+0x92 : retn 10h
        { 0x92, 0xC2 }, // (Offset,Value)-Pair #22
        { 0x93, 0x10 }, // (Offset,Value)-Pair #23
        { 0x94, 0x00 }, // (Offset,Value)-Pair #24
OOVPA_END;

// ******************************************************************
// * recv
// ******************************************************************
OOVPA_NO_XREF(recv, 4627, 24)

        // recv+0x00 : push ebp
        { 0x00, 0x55 }, // (Offset,Value)-Pair #1

		// recv+0x17 : push 0x276D
        { 0x17, 0x68 }, // (Offset,Value)-Pair #2
        { 0x18, 0x6D }, // (Offset,Value)-Pair #3
        { 0x19, 0x27 }, // (Offset,Value)-Pair #4

		// recv+0x24 : jmp
        { 0x24, 0xEB }, // (Offset,Value)-Pair #5
        { 0x25, 0x66 }, // (Offset,Value)-Pair #6

		// recv+0x51 : MOV DWORD PTR SS:[EBP-14],EAX
        { 0x51, 0x89 }, // (Offset,Value)-Pair #7
        { 0x52, 0x45 }, // (Offset,Value)-Pair #8
        { 0x53, 0xEC }, // (Offset,Value)-Pair #9

		// recv+0x68 : JL SHORT +04
        { 0x68, 0x7C }, // (Offset,Value)-Pair #10
        { 0x69, 0x04 }, // (Offset,Value)-Pair #11

		// recv+0x77 : MOV DWORD PTR DS:[ESI+8],2B434F53
        { 0x77, 0xC7 }, // (Offset,Value)-Pair #12
        { 0x78, 0x46 }, // (Offset,Value)-Pair #13
        { 0x79, 0x08 }, // (Offset,Value)-Pair #14
        { 0x7A, 0x53 }, // (Offset,Value)-Pair #15
        { 0x7B, 0x4F }, // (Offset,Value)-Pair #16
        { 0x7C, 0x43 }, // (Offset,Value)-Pair #17
        { 0x7D, 0x2B }, // (Offset,Value)-Pair #18

		// recv+0x88 : MOV EAX,DWORD PTR SS:[EBP+8]
        { 0x88, 0x8B }, // (Offset,Value)-Pair #19
        { 0x89, 0x45 }, // (Offset,Value)-Pair #20
        { 0x8A, 0x08 }, // (Offset,Value)-Pair #21

		// recv+0x8E : retn 10h
        { 0x8E, 0xC2 }, // (Offset,Value)-Pair #22
        { 0x8F, 0x10 }, // (Offset,Value)-Pair #23
        { 0x90, 0x00 }, // (Offset,Value)-Pair #24
OOVPA_END;

// ******************************************************************
// * ioctlsocket
// ******************************************************************
OOVPA_NO_XREF(ioctlsocket, 4627, 10)

        // ioctlsocket+0x13 : push 0x276D
        { 0x13, 0x68 }, // (Offset,Value)-Pair #1
        { 0x14, 0x6D }, // (Offset,Value)-Pair #2
        { 0x15, 0x27 }, // (Offset,Value)-Pair #3

        // ioctlsocket+0x62 : lea edx, [esi+0x24]
        { 0x62, 0x8D }, // (Offset,Value)-Pair #4
        { 0x63, 0x56 }, // (Offset,Value)-Pair #5
        { 0x64, 0x24 }, // (Offset,Value)-Pair #6

        // ioctlsocket+0x81 : mov cl, al
        { 0x81, 0x8A }, // (Offset,Value)-Pair #7
        { 0x82, 0xC8 }, // (Offset,Value)-Pair #8

        // ioctlsocket+0xC6 : retn 0x0C
        { 0xC6, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xC7, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * XNet_4627
// ******************************************************************
OOVPATable XNet_4627[] = {
	REGISTER_OOVPA(XNetStartup, 4627, PATCH), // same as xonline 4361
	REGISTER_OOVPA(WSAStartup, 4627, PATCH), // same as xonline 4361
	REGISTER_OOVPA(XnInit, 4627, XREF),
	REGISTER_OOVPA(socket, 4627, PATCH),
	REGISTER_OOVPA(connect, 4627, PATCH),
	REGISTER_OOVPA(send, 4627, PATCH),
	REGISTER_OOVPA(recv, 4627, PATCH),
	REGISTER_OOVPA(ioctlsocket, 4627, PATCH),
};

// ******************************************************************
// * XNet_4627_SIZE
// ******************************************************************
uint32 XNet_4627_SIZE = sizeof(XNet_4627);
