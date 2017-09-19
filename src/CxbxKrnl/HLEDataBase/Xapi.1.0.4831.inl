// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4831.cpp
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************


// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState, 4831, 12)

        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D },
        { 0x05, 0x81 },
        { 0x06, 0xA3 },

        // XInputSetState+0x12 : push 0x57
        { 0x12, 0x6A },
        { 0x13, 0x57 },

        // XInputSetState+0x15 : jmp +0x19
        { 0x15, 0xEB },
        { 0x16, 0x19 },

        // XInputSetState+0x28 : mov [edx+0x41], al
        { 0x28, 0x88 },
        { 0x29, 0x42 },
        { 0x2A, 0x41 },

        // XInputSetState+0x30 : retn 0x08
        { 0x30, 0xC2 },
        { 0x31, 0x08 },
OOVPA_END;


// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 4831, 11,

    XREF_FCLOSEDEVICE,
    XRefZero)

        { 0x13, 0x8B },
        { 0x14, 0x86 },
        { 0x15, 0xA3 },

        { 0x4C, 0x89 },
        { 0x4D, 0x86 },
        { 0x4E, 0x9E },

        { 0x87, 0x89 },
        { 0x88, 0x86 },
        { 0x89, 0xA7 },

        { 0x95, 0xC9 },
        { 0x96, 0xC3 },
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 4831, 12)

        { 0x0E, 0x8B },
        { 0x0F, 0x8A },
        { 0x10, 0xA3 },

        { 0x1A, 0x6A },
        { 0x1B, 0x57 },

        { 0x1D, 0xEB },
        { 0x1E, 0x3E },

        { 0x3A, 0x8B },
        { 0x3B, 0x8A },
        { 0x3C, 0xA3 },

        { 0x69, 0xC2 },
        { 0x6A, 0x08 },
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 4831, 10)

        { 0x08, 0xF8 },
        { 0x0C, 0x1D },
        { 0x11, 0x56 },
        { 0x1F, 0x0F },
        { 0x95, 0x89 },
        { 0x96, 0x7D },
        { 0x97, 0xC0 },
        { 0x98, 0x89 },
        { 0x99, 0x55 },
        { 0x9A, 0xD0 },
OOVPA_END;

// ******************************************************************
// * XGetDeviceEnumerationStatus
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceEnumerationStatus, 4831, 14)

        { 0x04, 0x15 },
        { 0x0A, 0x35 },
        { 0x10, 0x09 },

        { 0x17, 0x00 },
        { 0x18, 0x74 },
        { 0x19, 0x03 },
        { 0x1A, 0x33 },
        { 0x1B, 0xF6 },
        { 0x1C, 0x46 },
        { 0x1D, 0x8A },
        { 0x1E, 0xC8 },
        { 0x1F, 0xFF },

        { 0x25, 0x8B },
        { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 4831, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x13, 0x6A },
        { 0x14, 0x0C },
        { 0x15, 0x59 },
        { 0x16, 0x33 },
        { 0x17, 0xC0 },
        { 0x18, 0x8D },
        { 0x19, 0x7D },
        { 0x1A, 0xCC },

        { 0x1D, 0x8D },
        { 0x1E, 0x45 },
OOVPA_END;
