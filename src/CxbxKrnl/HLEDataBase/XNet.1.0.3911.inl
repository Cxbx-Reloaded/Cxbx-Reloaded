// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XNet.1.0.3911.inl
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
OOVPA_XREF(XNetStartup, 3911, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        // XNetStartup+0x07 : call [XnInit]
        XREF_ENTRY( 0x07, XREF_XnInit ),

        // XNetStartup+0x00 : push 0
        { 0x00, 0x6A },
        { 0x01, 0x00 },

        // XNetStartup+0x02 : push [esp+0x08]
        { 0x02, 0xFF },
        { 0x03, 0x74 },
        { 0x04, 0x24 },
        { 0x05, 0x08 },

        // XNetStartup+0x0B : retn 0x04
        { 0x0B, 0xC2 },
        { 0x0C, 0x04 },
OOVPA_END;

// ******************************************************************
// * WSAStartup
// ******************************************************************
OOVPA_XREF(WSAStartup, 3911, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        // WSAStartup+0x07 : call [XnInit]
        XREF_ENTRY( 0x07, XREF_XnInit ),

        // WSAStartup+0x01 : push 1; xor ebx, ebx
        { 0x01, 0x6A },
        { 0x02, 0x01 },
        { 0x03, 0x33 },
        { 0x04, 0xDB },

        // WSAStartup+0x1B : mov word ptr [ecx+2], 0x0202
        { 0x1B, 0x66 },
        { 0x1C, 0xC7 },
        { 0x1D, 0x41 },
        { 0x1E, 0x02 },
        { 0x1F, 0x02 },
        { 0x20, 0x02 },
OOVPA_END;

// ******************************************************************
// * XnInit
// ******************************************************************
// For only XNETS library, XNET library is different OOVPA.
OOVPA_XREF(XnInit, 3911, 11,

    XREF_XnInit,
    XRefZero)

        // XnInit+0x31 : push 0x3554454E
        { 0x31, 0x68 },
        { 0x32, 0x4E },
        { 0x33, 0x45 },
        { 0x34, 0x54 },
        { 0x35, 0x35 },

        // XnInit+0xA4 : inc dword ptr [eax+0x2B4]
        { 0xA4, 0xFF },
        { 0xA5, 0x80 },
        { 0xA6, 0xB4 },
        { 0xA7, 0x02 },

        // XnInit+0xBD : retn 0x08
        { 0xBD, 0xC2 },
        { 0xBE, 0x08 },
OOVPA_END;

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 3911, 14)

        { 0x00, 0x56 },
        { 0x01, 0x33 },
        { 0x02, 0xF6 },
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

        { 0x3F, 0xC3 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 3911, 8)

        // XNetGetEthernetLinkStatus+0x12 : cmpxchg [ecx], edx
        { 0x12, 0x0F },
        { 0x13, 0xB1 },
        { 0x14, 0x11 },

        // XNetStartup+0x17 : jz +0x08
        { 0x17, 0x74 },
        { 0x18, 0x08 },

        // XNetStartup+0x1F : jmp +0xE2
        { 0x1F, 0xEB },
        { 0x20, 0xE2 },

        // XNetStartup+0x3F : retn
        { 0x3F, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * CXnSock::socket
// ******************************************************************
OOVPA_NO_XREF(socket, 3911, 9)

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

// ******************************************************************
// * CXnSock::bind
// ******************************************************************
OOVPA_NO_XREF(bind, 3911, 10)

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

// ******************************************************************
// * CXnSock::listen
// ******************************************************************
OOVPA_NO_XREF(listen, 3911, 10)

        // listen+0x00 : push edi
        { 0x00, 0x57 },

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

// ******************************************************************
// * CXnSock::ioctlsocket
// ******************************************************************
OOVPA_NO_XREF(ioctlsocket, 3911, 11)

        // ioctlsocket+0x00 : push ebp
        { 0x00, 0x55 },

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

// ******************************************************************
// * CXnSock::send
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(send, 3911, 14)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x20 },
        { 0x06, 0x57 },
        { 0x07, 0x8B },
        { 0x08, 0xF9 },
        { 0x09, 0x85 },
        { 0x0A, 0xFF },
        { 0x0B, 0x74 },

        { 0x12, 0x00 },
        { 0x1A, 0x00 },
OOVPA_END;

// ******************************************************************
// * CXnSock::connect
// ******************************************************************
OOVPA_NO_XREF(connect, 3911, 14) // Up to 5028

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x53 },
        { 0x04, 0x56 },
        { 0x05, 0x8B },
        { 0x06, 0xD9 },
        { 0x07, 0x33 },
        { 0x08, 0xF6 },
        { 0x09, 0x3B },
        { 0x0A, 0xDE },
        { 0x0B, 0x74 },

        { 0x12, 0x00 },
        { 0x19, 0x00 },
OOVPA_END;

// ******************************************************************
// * CXnSock::recv
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(recv, 3911, 14)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x1C },
        { 0x06, 0x57 },
        { 0x07, 0x8B },
        { 0x08, 0xF9 },
        { 0x09, 0x85 },
        { 0x0A, 0xFF },
        { 0x0B, 0x74 },

        { 0x12, 0x00 },
        { 0x1A, 0x00 },
OOVPA_END;
