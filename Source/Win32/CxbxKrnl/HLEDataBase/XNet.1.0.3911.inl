// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XNet.1.0.3911.cpp
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
SOOVPA<9> XNetStartup_1_0_3911 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // XNetStartup+0x07 : call [XnInit]
        { 0x07, XREF_XNINIT },  // (Offset,Value)-Pair #1

        // XNetStartup+0x00 : push 0
        { 0x00, 0x6A }, // (Offset,Value)-Pair #2
        { 0x01, 0x00 }, // (Offset,Value)-Pair #3

        // XNetStartup+0x02 : push [esp+0x08]
        { 0x02, 0xFF }, // (Offset,Value)-Pair #4
        { 0x03, 0x74 }, // (Offset,Value)-Pair #5
        { 0x04, 0x24 }, // (Offset,Value)-Pair #6
        { 0x05, 0x08 }, // (Offset,Value)-Pair #7

        // XNetStartup+0x0B : retn 0x04
        { 0x0B, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * WSAStartup
// ******************************************************************
SOOVPA<11> WSAStartup_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // WSAStartup+0x07 : call [XnInit]
        { 0x07, XREF_XNINIT },  // (Offset,Value)-Pair #1

        // WSAStartup+0x01 : push 1; xor ebx, ebx
        { 0x01, 0x6A }, // (Offset,Value)-Pair #2
        { 0x02, 0x01 }, // (Offset,Value)-Pair #3
        { 0x03, 0x33 }, // (Offset,Value)-Pair #4
        { 0x04, 0xDB }, // (Offset,Value)-Pair #5

        // WSAStartup+0x1B : mov word ptr [ecx+2], 0x0202
        { 0x1B, 0x66 }, // (Offset,Value)-Pair #6
        { 0x1C, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x1D, 0x41 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x02 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x02 }, // (Offset,Value)-Pair #10
        { 0x20, 0x02 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XnInit
// ******************************************************************
SOOVPA<11> XnInit_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_XNINIT, // XRef Is Saved
    0,           // XRef Not Used

    {
        // XnInit+0x31 : push 0x3554454E
        { 0x31, 0x68 }, // (Offset,Value)-Pair #1
        { 0x32, 0x4E }, // (Offset,Value)-Pair #2
        { 0x33, 0x45 }, // (Offset,Value)-Pair #3
        { 0x34, 0x54 }, // (Offset,Value)-Pair #4
        { 0x35, 0x35 }, // (Offset,Value)-Pair #5

        // XnInit+0xA4 : inc dword ptr [eax+0x2B4]
        { 0xA4, 0xFF }, // (Offset,Value)-Pair #6
        { 0xA5, 0x80 }, // (Offset,Value)-Pair #7
        { 0xA6, 0xB4 }, // (Offset,Value)-Pair #8
        { 0xA7, 0x02 }, // (Offset,Value)-Pair #9

        // XnInit+0xBD : retn 0x08
        { 0xBD, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xBE, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
SOOVPA<8> XNetGetEthernetLinkStatus_1_0_3911 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XNetGetEthernetLinkStatus+0x12 : cmpxchg [ecx], edx
        { 0x12, 0x0F }, // (Offset,Value)-Pair #1
        { 0x13, 0xB1 }, // (Offset,Value)-Pair #2
        { 0x14, 0x11 }, // (Offset,Value)-Pair #3

        // XNetStartup+0x17 : jz +0x08
        { 0x17, 0x74 }, // (Offset,Value)-Pair #4
        { 0x18, 0x08 }, // (Offset,Value)-Pair #5

        // XNetStartup+0x1F : jmp +0xE2
        { 0x1F, 0xEB }, // (Offset,Value)-Pair #6
        { 0x20, 0xE2 }, // (Offset,Value)-Pair #7

        // XNetStartup+0x3F : retn
        { 0x3F, 0xC3 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * XNet_1_0_3911
// ******************************************************************
OOVPATable XNet_1_0_3911[] =
{
    // XNetStartup
    {
        (OOVPA*)&XNetStartup_1_0_3911,
            
        XTL::EmuXNetStartup,

        #ifdef _DEBUG_TRACE
        "EmuXNetStartup"
        #endif
    },
    // WSAStartup
    {
        (OOVPA*)&WSAStartup_1_0_3911,
            
        XTL::EmuWSAStartup,

        #ifdef _DEBUG_TRACE
        "EmuWSAStartup"
        #endif
    },
    // XnInit (XREF)
    {
        (OOVPA*)&XnInit_1_0_3911, 0,

        #ifdef _DEBUG_TRACE
        "XnInit (XRef)"
        #endif
    },
    // XNetGetEthernetLinkStatus
    {
        (OOVPA*)&XNetGetEthernetLinkStatus_1_0_3911,
            
        XTL::EmuXNetGetEthernetLinkStatus,

        #ifdef _DEBUG_TRACE
        "EmuXNetGetEthernetLinkStatus"
        #endif
    },
};

// ******************************************************************
// * XNet_1_0_3911_SIZE
// ******************************************************************
uint32 XNet_1_0_3911_SIZE = sizeof(XNet_1_0_3911);
