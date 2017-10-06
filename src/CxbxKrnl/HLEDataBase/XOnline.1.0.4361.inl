// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XOnline.1.0.4361.cpp
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
// * XNetStartup
// ******************************************************************
OOVPA_XREF(XNetStartup, 4361, 8,

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

// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_XREF(WSAStartup, 4361, 9,

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

// ******************************************************************
// * XnInit
// ******************************************************************
OOVPA_XREF(XnInit, 4361, 10,

    XREF_XnInit,
    XRefZero)

        // XnInit+0x03 : sub esp, 0x0214
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x02 },

        // XnInit+0x19 : cmpxchg [ecx], edx
        { 0x19, 0x0F },
        { 0x1A, 0xB1 },
        { 0x1B, 0x11 },

        // XnInit+0xBC : push 0x01EC
        { 0xBC, 0x68 },
        { 0xBD, 0xEC },
        { 0xBE, 0x01 },
OOVPA_END;
#if 0 // Moved to XNet 3911
// ******************************************************************
// * CXnSock::socket
// ******************************************************************
OOVPA_NO_XREF(socket, 4361, 9)

        // socket+0x10 : push 0x276D
        { 0x10, 0x68 },
        { 0x11, 0x6D },
        { 0x12, 0x27 },

        // socket+0x52 : cmp eax, 0x11
        { 0x52, 0x83 },
        { 0x53, 0xF8 },
        { 0x54, 0x11 },

        // socket+0xD9 : mov [esi+4], ecx
        { 0xD9, 0x89 },
        { 0xDA, 0x4E },
        { 0xDB, 0x04 },
OOVPA_END;
#endif
#if 0 // Moved to XNet 3911
// ******************************************************************
// * CXnSock::bind
// ******************************************************************
OOVPA_NO_XREF(bind, 4361, 10)

        // bind+0x11 : push 0x276D
        { 0x11, 0x68 },
        { 0x12, 0x6D },
        { 0x13, 0x27 },

        // bind+0x4C : mov [eax], cx
        { 0x4C, 0x66 },
        { 0x4D, 0x89 },
        { 0x4E, 0x08 },

        // bind+0x5B : xor eax, eax
        { 0x5B, 0x33 },
        { 0x5C, 0xC0 },

        // bind+0x80 : retn 0x0C
        { 0x80, 0xC2 },
        { 0x81, 0x0C },
OOVPA_END;
#endif
#if 0 // Moved to XNet 3911
// ******************************************************************
// * CXnSock::listen
// ******************************************************************
OOVPA_NO_XREF(listen, 4361, 9)

        // listen+0x10 : push 0x276D
        { 0x10, 0x68 },
        { 0x11, 0x6D },
        { 0x12, 0x27 },

        // listen+0x42 : test al, 0x10
        { 0x42, 0xA8 },
        { 0x43, 0x10 },

        // listen+0x51 : mov ecx, edi
        { 0x51, 0x8B },
        { 0x52, 0xCF },

        // listen+0x7F : retn 0x08
        { 0x7F, 0xC2 },
        { 0x80, 0x08 },
OOVPA_END;
#endif
#if 0 // Moved to XNet 3911
// ******************************************************************
// * CXnSock::ioctlsocket
// ******************************************************************
OOVPA_NO_XREF(ioctlsocket, 4361, 10)

        // ioctlsocket+0x12 : push 0x276D
        { 0x12, 0x68 },
        { 0x13, 0x6D },
        { 0x14, 0x27 },

        // ioctlsocket+0x61 : lea edx, [esi+0x24]
        { 0x61, 0x8D },
        { 0x62, 0x56 },
        { 0x63, 0x24 },

        // ioctlsocket+0x80 : mov cl, al
        { 0x80, 0x8A },
        { 0x81, 0xC8 },

        // ioctlsocket+0xC5 : retn 0x0C
        { 0xC5, 0xC2 },
        { 0xC6, 0x0C },
OOVPA_END;
#endif
// ******************************************************************
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 4361, 12,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x02, 0xEC },

        { 0x0D, 0xFC },
        { 0x0E, 0x75 },
        { 0x0F, 0x07 },
        { 0x10, 0xB8 },
        { 0x11, 0x05 },
        { 0x12, 0x00 },
        { 0x13, 0x15 },
        { 0x14, 0x80 },
        { 0x15, 0xEB },

        { 0x24, 0xA8 },
        { 0x32, 0x33 },
OOVPA_END;

// ******************************************************************
// * XOnlineLogon
// ******************************************************************
OOVPA_XREF(XOnlineLogon, 4361, 5,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_CXo_XOnlineLogon ),

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x0A, 0xE9 },
OOVPA_END;

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 4361, 13)

        { 0x00, 0x56 },
        { 0x01, 0x33 },
        { 0x02, 0xF3 },
        { 0x03, 0xB8 },
        { 0x04, 0x00 },
        { 0x05, 0x00 },
        { 0x06, 0x00 },
        { 0x07, 0x00 },
        { 0x08, 0xB9 },

        { 0x0D, 0xBA },
        { 0x1A, 0xE8 },

        { 0x27, 0x75 },
        { 0x2C, 0x15 },
OOVPA_END;

// ******************************************************************
// * XOnline_4361
// ******************************************************************
OOVPATable XOnline_4361[] = {

	REGISTER_OOVPA(XnInit, 4361, XREF),
	REGISTER_OOVPA(XNetStartup, 4361, PATCH),
	REGISTER_OOVPA(WSAStartup, 4361, PATCH),
	REGISTER_OOVPA(CXo_XOnlineLogon, 4361, XREF),
	REGISTER_OOVPA(XOnlineLogon, 4361, PATCH),
	REGISTER_OOVPA(XNetGetEthernetLinkStatus, 4361, PATCH),
	REGISTER_OOVPA(socket, 3911, PATCH),
	REGISTER_OOVPA(bind, 3911, PATCH),
	REGISTER_OOVPA(listen, 3911, PATCH),
	REGISTER_OOVPA(ioctlsocket, 3911, PATCH),
	REGISTER_OOVPA(connect, 3911, PATCH),
	REGISTER_OOVPA(send, 3911, PATCH),
	REGISTER_OOVPA(recv, 3911, PATCH),
};

// ******************************************************************
// * XOnline_4361_SIZE
// ******************************************************************
uint32 XOnline_4361_SIZE = sizeof(XOnline_4361);
