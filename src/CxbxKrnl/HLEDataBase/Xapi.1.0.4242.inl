// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4242.inl
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
// * XInputOpen
// ******************************************************************
OOVPA_NO_XREF(XInputOpen, 4242, 12)

        // XInputOpen+0x14 : push 0x57
        { 0x14, 0x6A },
        { 0x15, 0x57 },

        // XInputOpen+0x1D : jmp +0x33
        { 0x1D, 0xEB },
        { 0x1E, 0x33 },

        // XInputOpen+0x33 : add edx, 0x10
        { 0x33, 0x83 },
        { 0x34, 0xC2 },
        { 0x35, 0x10 },

        // XInputOpen+0x47 : jmp +0x06
        { 0x47, 0x75 },
        { 0x48, 0x06 },

        // XInputOpen+0x52 : leave
        { 0x52, 0xC9 },

        // XInputOpen+0x53 : retn 0x10
        { 0x53, 0xC2 },
        { 0x54, 0x10 },
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 4242, 13)

        { 0x00, 0x53 },
        { 0x01, 0x56 },

        { 0x0A, 0x8B },
        { 0x0B, 0x54 },
        { 0x0C, 0x24 },
        { 0x0D, 0x0C },
        { 0x0E, 0x8B },
        { 0x0F, 0x8A },
        { 0x10, 0xA3 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },

        { 0x5B, 0xF3 },
OOVPA_END;

// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState, 4242, 12)

        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D },
        { 0x05, 0x81 },
        { 0x06, 0xA3 },

        // XInputSetState+0x15 : push 0x57
        { 0x15, 0x6A },
        { 0x16, 0x57 },

        // XInputSetState+0x18 : jmp +0x19
        { 0x18, 0xEB },
        { 0x19, 0x19 },

        // XInputSetState+0x2B : mov [edx+0x41], al
        { 0x2B, 0x88 },
        { 0x2C, 0x42 },
        { 0x2D, 0x41 },

        // XInputSetState+0x33 : retn 0x08
        { 0x33, 0xC2 },
        { 0x34, 0x08 },
OOVPA_END;

// ******************************************************************
// * XMountMUA
// ******************************************************************
OOVPA_NO_XREF(XMountMUA, 4242, 8) // generic version

        { 0x22, 0x8A },
        { 0x3E, 0x89 },
        { 0x5E, 0x5C },
        { 0x7E, 0xF4 },
        { 0x9E, 0x50 },
        { 0xBE, 0xF8 },
        { 0xDE, 0x8D },
        { 0xFE, 0x09 },
OOVPA_END;

// ******************************************************************
// * XFormatUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XFormatUtilityDrive, 4242, 12)

        { 0x02, 0xEC },
        { 0x10, 0x50 },
        { 0x1E, 0xEC },

        { 0x40, 0xF8 },
        { 0x41, 0x8D },
        { 0x42, 0x45 },
        { 0x43, 0xE4 },
        { 0x44, 0x50 },
        { 0x45, 0x8D },
        { 0x46, 0x45 },
        { 0x47, 0xF4 },

        { 0x6D, 0x33 },
OOVPA_END;

// ******************************************************************
// * XMountMURootA
// ******************************************************************
OOVPA_NO_XREF(XMountMURootA, 4242, 12)

        { 0x16, 0xBF },
        { 0x22, 0x8A },
        { 0x39, 0x05 },

        { 0x50, 0x55 },
        { 0x51, 0x58 },
        { 0x52, 0xE9 },
        { 0x53, 0x0E },
        { 0x54, 0x01 },
        { 0x55, 0x00 },
        { 0x56, 0x00 },
        { 0x57, 0x66 },

        { 0x72, 0xE8 },
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 4242, 7)

        // XapiInitProcess+0x03 : sub esp, 30h
        { 0x05, 0x30 },

        // XapiInitProcess+0x0F : push 0x0C
        { 0x0F, 0x6A },
        { 0x10, 0x0C },

        // XapiInitProcess+0x17 : repe stosd
        { 0x17, 0xF3 },
        { 0x18, 0xAB },

        // XapiInitProcess+0x42 : jnz +0x0B
        { 0x42, 0x75 },
        { 0x43, 0x0B },
OOVPA_END;
