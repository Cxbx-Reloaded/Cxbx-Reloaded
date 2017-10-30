// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.5558.inl
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

#if 0 // Moved to 5455
// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 5455, 8)

        { 0x1D, 0x85 },
        { 0x3C, 0x8A },
        { 0x5B, 0x00 },
        { 0x7A, 0x45 },
        { 0x99, 0xD4 },
        { 0xB8, 0x02 },
        { 0xD7, 0x8D },
        { 0xF6, 0x46 },
OOVPA_END;
#endif
#if 0 // Moved to 5455
// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 5455, 7)

        { 0x0D, 0x0C },
        { 0x1C, 0x5E },
        { 0x2B, 0xBB },
        { 0x3A, 0x80 },
        { 0x49, 0x08 },
        { 0x58, 0xF3 },
        { 0x67, 0x15 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 5028 version
// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 5558, 8)

        { 0x22, 0x6A },
        { 0x3E, 0x01 },
        { 0x5E, 0x7D },
        { 0x7E, 0x8B },
        { 0x9F, 0x68 },
        { 0xBE, 0x01 },
        { 0xDE, 0x6A },
        { 0xFE, 0x02 },
OOVPA_END;
#endif
#if 0 // Moved to 5659
// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 5659, 8)

        { 0x22, 0x6A },
        { 0x3E, 0x01 },
        { 0x5E, 0xEB },
        { 0x7E, 0x8B },
        { 0x9F, 0x68 },
        { 0xBE, 0x01 },
        { 0xDE, 0x6A },
        { 0xFE, 0x02 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 5558, 7)

        { 0x07, 0xF0 },
        { 0x10, 0xD2 },
        { 0x19, 0x19 },
        { 0x22, 0x52 },
        { 0x2B, 0xE8 },
        { 0x34, 0x6A },
        { 0x3D, 0x10 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XapiFiberStartup
// ******************************************************************
OOVPA_NO_XREF(XapiFiberStartup, 5558, 7)

        { 0x07, 0xE8 },
        { 0x10, 0xA1 },
        { 0x1C, 0x8B },
        { 0x25, 0xFF },
        { 0x2B, 0x0C },
        { 0x34, 0xC3 },
        { 0x3D, 0x00 },
OOVPA_END;
#endif
#if 0 // Moved to 5455
// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 5455, 7,

    XREF_XID_fCloseDevice,
    XRefZero)

        { 0x11, 0x45 },
        { 0x24, 0xFF },
        { 0x37, 0x89 },
        { 0x4A, 0x86 },
        { 0x5D, 0x53 },
        { 0x70, 0xFF },
        { 0x84, 0x89 },
OOVPA_END;
#endif
#if 0 // Moved to 5455
// ******************************************************************
// * XInputClose
// ******************************************************************
OOVPA_XREF(XInputClose, 5455, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x05, XREF_XID_fCloseDevice ),

        { 0x00, 0x8B },
        { 0x01, 0x4C },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0xE8 },
        { 0x09, 0xC2 },
        { 0x0A, 0x04 },
        { 0x0B, 0x00 },
OOVPA_END;
#endif
#if 0 // Moved to 5455
// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA, 5455, 11)

        { 0x0B, 0x08 },

        { 0x18, 0x0B },
        { 0x19, 0x57 },
        { 0x1A, 0x33 },
        { 0x1B, 0xFF },
        { 0x1C, 0x80 },
        { 0x1D, 0xE3 },
        { 0x1E, 0xDF },
        { 0x1F, 0x80 },

        { 0xC1, 0x83 },
        { 0xD0, 0x15 },
OOVPA_END;
#endif
