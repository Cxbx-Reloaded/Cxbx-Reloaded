// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4034.inl
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

/*

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 4361, 14)

        // XInputGetCapabilities+0x03 : sub esp, 0x48
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x48 },

        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
        { 0x24, 0xF6 },
        { 0x25, 0x46 },
        { 0x26, 0x04 },
        { 0x27, 0x02 },

        // XInputGetCapabilities+0x3A : stosb
        { 0x3A, 0xAA },

        // XInputGetCapabilities+0x7D : mov [ebp+var_48], 0x30
        { 0x7D, 0xC6 },
        { 0x7E, 0x45 },
        { 0x7F, 0xB8 },
        { 0x80, 0x30 },

        // XInputGetCapabilities+0xEE : cmp [eax], ebx
        { 0xEE, 0x39 },
        { 0xEF, 0x18 },
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 4361, 12)

        // XInputGetState+0x0E : mov ecx, [edx+0x0A3]
        { 0x0E, 0x8B },
        { 0x0F, 0x8A },
        { 0x10, 0xA3 },

        // XInputGetState+0x1D : push 0x57
        { 0x1D, 0x6A },
        { 0x1E, 0x57 },

        // XInputGetState+0x20 : jmp +0x3E
        { 0x20, 0xEB },
        { 0x21, 0x3E },

        // XInputGetState+0x2E : mov ebx, 0x048F
        { 0x2E, 0xBB },
        { 0x2F, 0x8F },
        { 0x30, 0x04 },

        // XInputGetState+0x6C : retn 8
        { 0x6C, 0xC2 },
        { 0x6D, 0x08 },
OOVPA_END;

// ******************************************************************
// * CreateThread
// ******************************************************************
OOVPA_NO_XREF(CreateThread, 4361, 8)

        // CreateThread+0x0A : mov eax, ds:10130h
        { 0x0A, 0xA1 },
        { 0x0B, 0x30 },
        { 0x0C, 0x01 },

        // CreateThread+0x1C : and ecx, 0xFFFFFF01
        { 0x1C, 0x81 },
        { 0x1D, 0xE1 },
        { 0x1E, 0x01 },

        // CreateThread+0x6B : retn 0x18
        { 0x6B, 0xC2 },
        { 0x6C, 0x18 },
OOVPA_END;

// ******************************************************************
// * CloseHandle
// ******************************************************************
OOVPA_NO_XREF(CloseHandle, 4361, 10)

        // CloseHandle+0x00 : push [esp+4]
        { 0x00, 0xFF },
        { 0x01, 0x74 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // CloseHandle+0x04 : call ds:[addr]
        { 0x04, 0xFF },
        { 0x05, 0x15 },

        // CloseHandle+0x11 : jmp +0x08
        { 0x11, 0xEB },
        { 0x12, 0x08 },

        // CloseHandle+0x1B : retn 4
        { 0x1B, 0xC2 },
        { 0x1C, 0x04 },
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 4361, 7)

        // XapiInitProcess+0x00 : sub esp, 30h
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
*/
