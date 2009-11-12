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
// * XnInit
// ******************************************************************
SOOVPA<12> XnInit_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_XNINIT, // XRef Is Saved
    0,           // XRef Not Used

    {
        // XnInit+0x03 : sub esp, 0x0218
        { 0x03, 0x81 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x18 }, // (Offset,Value)-Pair #3
        { 0x06, 0x02 }, // (Offset,Value)-Pair #4

        // XnInit+0x19 : cmpxchg [ecx], edx
        { 0x19, 0x0F }, // (Offset,Value)-Pair #5
        { 0x1A, 0xB1 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x11 }, // (Offset,Value)-Pair #7

        // XnInit+0x3C : push 0x4454454E
        { 0x3C, 0x68 }, // (Offset,Value)-Pair #8
        { 0x3D, 0x4E }, // (Offset,Value)-Pair #9
		{ 0x3E, 0x45 }, // (Offset,Value)-Pair #10
		{ 0x3F, 0x54 }, // (Offset,Value)-Pair #11
		{ 0x40, 0x44 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * XOnline_1_0_4627
// ******************************************************************
OOVPATable XOnline_1_0_4627[] =
{
    // XNetStartup (* unchanged since 4361 *)
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
        (OOVPA*)&XnInit_1_0_4627, 0,

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
    // bind (* unchanged since 4361 *)
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
    // ioctlsocket (* unchanged since 4361 *)
    {
        (OOVPA*)&ioctlsocket_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emuioctlsocket),

        #ifdef _DEBUG_TRACE
        "Emuioctlsocket"
        #endif
    },
};

// ******************************************************************
// * XOnline_1_0_4627_SIZE
// ******************************************************************
uint32 XOnline_1_0_4627_SIZE = sizeof(XOnline_1_0_4627);
