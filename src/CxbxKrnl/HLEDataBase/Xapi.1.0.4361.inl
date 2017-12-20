// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.4361.inl
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
// * XapiSetupPerTitleDriveLetters
// ******************************************************************
OOVPA_NO_XREF(XapiSetupPerTitleDriveLetters, 4361, 10)

        // XapiSetupPerTitleDriveLetters+0x09 : lea eax, [ebp-0x0C]
        { 0x09, 0x8D },
        { 0x0A, 0x45 },
        { 0x0B, 0xF4 },

        // XapiSetupPerTitleDriveLetters+0x17 : edd esp, 0x0C
        { 0x17, 0x83 },
        { 0x18, 0xC4 },
        { 0x19, 0x0C },

        // XapiSetupPerTitleDriveLetters+0x35 : jl 0x1A
        { 0x35, 0x7C },
        { 0x36, 0x1A },

        // XapiSetupPerTitleDriveLetters+0x52 : retn 0x08
        { 0x52, 0xC2 },
        { 0x53, 0x08 },
OOVPA_END;
