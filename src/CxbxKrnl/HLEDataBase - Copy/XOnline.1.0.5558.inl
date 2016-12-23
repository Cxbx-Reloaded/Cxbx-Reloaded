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

#if 0
// ******************************************************************
// * XNetStartup
// ******************************************************************
SOOVPA<8> XNetStartup_1_0_5344 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x01, 0xC0 },
        { 0x04, 0x50 },
        { 0x07, 0x24 },
        { 0x0A, 0x68 },
        { 0x0F, 0xE8 },
        { 0x14, 0xC2 },
        { 0x15, 0x04 },
        { 0x16, 0x00 },
    }
};

// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
SOOVPA<8> XNetGetEthernetLinkStatus_1_0_5344 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x08, 0x33 },
        { 0x10, 0x8A },
        { 0x19, 0x00 },
        { 0x22, 0x56 },
        { 0x2B, 0xFF },
        { 0x34, 0xF0 },
        { 0x3D, 0x24 },
        { 0x46, 0x5B },
    }
};

// ******************************************************************
// * CXo::XOnlineLogon
// ******************************************************************
SOOVPA<8> CXo_XOnlineLogon_1_0_5344 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_CXo_XOnlineLogon,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x21, 0xA8 },
        { 0x3E, 0x80 },
        { 0x5E, 0x00 },
        { 0x7E, 0x04 },
        { 0xA2, 0x85 },
        { 0xBE, 0xEC },
        { 0xDE, 0xF9 },
        { 0xFE, 0x33 },
    }
};

// ******************************************************************
// * XOnlineLogon
// ******************************************************************
SOOVPA<8> XOnlineLogon_1_0_5344 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x0B, XREF_CXo_XOnlineLogon },

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x5D },
        { 0x04, 0x8B },
        { 0x05, 0x0D },
        { 0x0A, 0xE9 },
    }
};
#endif

// ******************************************************************
// * WSAStartup
// ******************************************************************
SOOVPA<7> WSAStartup_1_0_5558 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x6A },
        { 0x0E, 0x68 },
        { 0x13, 0xE8 },
        { 0x18, 0xC2 },
        { 0x1A, 0x00 },
    }
};

// ******************************************************************
// * XOnline_1_0_5558
// ******************************************************************
// * TODO: Verify all of these
// ******************************************************************
OOVPATable XOnline_1_0_5558[] =
{
    // XNetStartup
    {
        (OOVPA*)&XNetStartup_1_0_5344,

        XTL::EmuXNetStartup,

        #ifdef _DEBUG_TRACE
		"EmuXNetStartup (5344)"
        #endif
    },
	// WSAStartup
    {
        (OOVPA*)&WSAStartup_1_0_5558,

        XTL::EmuWSAStartup,

        #ifdef _DEBUG_TRACE
        "EmuWSAStartup"
        #endif
    },
    // XnInit (XREF) (* unchanged since 4627 *)
    {
        (OOVPA*)&XnInit_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "XnInit (XRef)"
        #endif
    },
    // socket
    /*{
        (OOVPA*)&socket_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emusocket),

        #ifdef _DEBUG_TRACE
        "Emusocket"
        #endif
    },*/
    // bind (* unchanged since 4361 *)
    {
        (OOVPA*)&bind_1_0_4361,

        MFPtoFP<XTL::EmuThis>(&XTL::EmuThis::Emubind),

        #ifdef _DEBUG_TRACE
        "Emubind"
        #endif
    },
    // listen
    /*{
        (OOVPA*)&listen_1_0_4361,

        MFPtoFP<XTL::EmuThis>(XTL::EmuThis::Emulisten),

        #ifdef _DEBUG_TRACE
        "Emulisten"
        #endif
    },*/
    // ioctlsocket (* unchanged since 4361 *)
    {
        (OOVPA*)&ioctlsocket_1_0_4361,

        MFPtoFP<XTL::EmuThis>(&XTL::EmuThis::Emuioctlsocket),

        #ifdef _DEBUG_TRACE
        "Emuioctlsocket"
        #endif
    },
	// XNetGetEthernetLinkStatus
    {
        (OOVPA*)&XNetGetEthernetLinkStatus_1_0_5344,
        XTL::EmuXNetGetEthernetLinkStatus,

        #ifdef _DEBUG_TRACE
        "EmuXNetGetEthernetLinkStatus (5344)"
        #endif
    },
};

// ******************************************************************
// * XOnline_1_0_5558_SIZE
// ******************************************************************
uint32 XOnline_1_0_5558_SIZE = sizeof(XOnline_1_0_5558);
