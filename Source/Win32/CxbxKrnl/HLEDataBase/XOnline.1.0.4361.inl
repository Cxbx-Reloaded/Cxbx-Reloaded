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
SOOVPA<8> XNetStartup_1_0_4361 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // XNetStartup+0x0F : call [XnInit]
        { 0x10, XREF_XNINIT },  // (Offset,Value)-Pair #1

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
    }
};

// ******************************************************************
// * WSAStartup
// ******************************************************************
SOOVPA<9> WSAStartup_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // WSAStartup+0x0F : call [XnInit]
        { 0x14, XREF_XNINIT },  // (Offset,Value)-Pair #1

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
    }
};

// ******************************************************************
// * XnInit
// ******************************************************************
SOOVPA<10> XnInit_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_XNINIT, // XRef Is Saved
    0,           // XRef Not Used

    {
        // XnInit+0x03 : sub esp, 0x0214
        { 0x03, 0x81 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3
        { 0x06, 0x02 }, // (Offset,Value)-Pair #4

        // XnInit+0x19 : cmpxchg [ecx], edx
        { 0x19, 0x0F }, // (Offset,Value)-Pair #5
        { 0x1A, 0xB1 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x11 }, // (Offset,Value)-Pair #7

        // XnInit+0xBC : push 0x01EC
        { 0xBC, 0x68 }, // (Offset,Value)-Pair #8
        { 0xBD, 0xEC }, // (Offset,Value)-Pair #9
        { 0xBE, 0x01 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * socket
// ******************************************************************
SOOVPA<9> socket_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // socket+0x10 : push 0x276D
        { 0x10, 0x68 }, // (Offset,Value)-Pair #1
        { 0x11, 0x6D }, // (Offset,Value)-Pair #2
        { 0x12, 0x27 }, // (Offset,Value)-Pair #3

        // socket+0x52 : cmp eax, 0x11
        { 0x52, 0x83 }, // (Offset,Value)-Pair #4
        { 0x53, 0xF8 }, // (Offset,Value)-Pair #5
        { 0x54, 0x11 }, // (Offset,Value)-Pair #6

        // socket+0xD9 : mov [esi+4], ecx
        { 0xD9, 0x89 }, // (Offset,Value)-Pair #7
        { 0xDA, 0x4E }, // (Offset,Value)-Pair #8
        { 0xDB, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * bind
// ******************************************************************
SOOVPA<10> bind_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // bind+0x11 : push 0x276D
        { 0x11, 0x68 }, // (Offset,Value)-Pair #1
        { 0x12, 0x6D }, // (Offset,Value)-Pair #2
        { 0x13, 0x27 }, // (Offset,Value)-Pair #3

        // bind+0x4C : mov [eax], cx
        { 0x4C, 0x66 }, // (Offset,Value)-Pair #4
        { 0x4D, 0x89 }, // (Offset,Value)-Pair #5
        { 0x4E, 0x08 }, // (Offset,Value)-Pair #6

        // bind+0x5B : xor eax, eax
        { 0x5B, 0x33 }, // (Offset,Value)-Pair #7
        { 0x5C, 0xC0 }, // (Offset,Value)-Pair #8

        // bind+0x80 : retn 0x0C
        { 0x80, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x81, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * listen
// ******************************************************************
SOOVPA<9> listen_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // listen+0x10 : push 0x276D
        { 0x10, 0x68 }, // (Offset,Value)-Pair #1
        { 0x11, 0x6D }, // (Offset,Value)-Pair #2
        { 0x12, 0x27 }, // (Offset,Value)-Pair #3

        // listen+0x42 : test al, 0x10
        { 0x42, 0xA8 }, // (Offset,Value)-Pair #4
        { 0x43, 0x10 }, // (Offset,Value)-Pair #5

        // listen+0x51 : mov ecx, edi
        { 0x51, 0x8B }, // (Offset,Value)-Pair #6
        { 0x52, 0xCF }, // (Offset,Value)-Pair #7

        // listen+0x7F : retn 0x08
        { 0x7F, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x80, 0x08 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * ioctlsocket
// ******************************************************************
SOOVPA<10> ioctlsocket_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // ioctlsocket+0x12 : push 0x276D
        { 0x12, 0x68 }, // (Offset,Value)-Pair #1
        { 0x13, 0x6D }, // (Offset,Value)-Pair #2
        { 0x14, 0x27 }, // (Offset,Value)-Pair #3

        // ioctlsocket+0x61 : lea edx, [esi+0x24]
        { 0x61, 0x8D }, // (Offset,Value)-Pair #4
        { 0x62, 0x56 }, // (Offset,Value)-Pair #5
        { 0x63, 0x24 }, // (Offset,Value)-Pair #6

        // ioctlsocket+0x80 : mov cl, al
        { 0x80, 0x8A }, // (Offset,Value)-Pair #7
        { 0x81, 0xC8 }, // (Offset,Value)-Pair #8

        // ioctlsocket+0xC5 : retn 0x0C
        { 0xC5, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xC6, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XOnline_1_0_4361
// ******************************************************************
OOVPATable XOnline_1_0_4361[] =
{
    // XNetStartup
    {
        (OOVPA*)&XNetStartup_1_0_4361,
            
        XTL::EmuXNetStartup,

        #ifdef _DEBUG_TRACE
        "EmuXNetStartup"
        #endif
    },
    // WSAStartup
    {
        (OOVPA*)&WSAStartup_1_0_4361,
            
        XTL::EmuWSAStartup,

        #ifdef _DEBUG_TRACE
        "EmuWSAStartup"
        #endif
    },
    // XnInit (XREF)
    {
        (OOVPA*)&XnInit_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "XnInit (XRef)"
        #endif
    },
    // socket
    {
        (OOVPA*)&socket_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emusocket),

        #ifdef _DEBUG_TRACE
        "Emusocket"
        #endif
    },
    // bind
    {
        (OOVPA*)&bind_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emubind),

        #ifdef _DEBUG_TRACE
        "Emubind"
        #endif
    },
    // listen
    {
        (OOVPA*)&listen_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emulisten),

        #ifdef _DEBUG_TRACE
        "Emulisten"
        #endif
    },
    // ioctlsocket
    {
        (OOVPA*)&ioctlsocket_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emuioctlsocket),

        #ifdef _DEBUG_TRACE
        "Emuioctlsocket"
        #endif
    },
};

// ******************************************************************
// * XOnline_1_0_4361_SIZE
// ******************************************************************
uint32 XOnline_1_0_4361_SIZE = sizeof(XOnline_1_0_4361);
