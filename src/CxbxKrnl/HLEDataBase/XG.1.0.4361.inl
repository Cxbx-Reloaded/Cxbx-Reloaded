// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XG.1.0.4361.inl
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

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XGIsSwizzledFormat
// ******************************************************************
OOVPA_NO_XREF(XGIsSwizzledFormat, 4361, 12)

        // XGIsSwizzledFormat+0x04 : cmp eax, 0x2D
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0x2D },

        // XGIsSwizzledFormat+0x0C : jge +0x31
        { 0x0C, 0x7D },
        { 0x0D, 0x31 },

        // XGIsSwizzledFormat+0x17 : cmp eax, 0x0B
        { 0x17, 0x83 },
        { 0x18, 0xF8 },
        { 0x19, 0x0B },

        // XGIsSwizzledFormat+0x1A : jz +0x23
        { 0x1A, 0x74 },
        { 0x1B, 0x23 },

        // XGIsSwizzledFormat+0x42 : jmp +0xE4
        { 0x42, 0xEB },
        { 0x43, 0xE4 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XGSwizzleRect
// ******************************************************************
OOVPA_NO_XREF(XGSwizzleRect, 4361, 10)

        // XGSwizzleRect+0x2E : pop eax
        { 0x2E, 0x58 },

        // XGSwizzleRect+0x4A : cmp dword ptr [ebp+0x74], 1
        { 0x4A, 0x83 },
        { 0x4B, 0x7D },
        { 0x4C, 0x74 },
        { 0x4D, 0x01 },

        // XGSwizzleRect+0xD8 : mov edx, [ebp+0x74]
        { 0xD8, 0x8B },
        { 0xD9, 0x55 },
        { 0xDA, 0x74 },

        // XGSwizzleRect+0xEC : jmp +0x0B
        { 0xEC, 0xEB },
        { 0xED, 0x0B },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * XGSetVertexBufferHeader
// ******************************************************************
OOVPA_NO_XREF(XGSetVertexBufferHeader, 4361, 8) // 4134

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x18 },
        { 0x0A, 0x08 },
        { 0x0D, 0x00 },
        { 0x10, 0x00 },
        { 0x13, 0x48 },
        { 0x16, 0x18 },
OOVPA_END;
#endif

#if 0 // Moved to 3911
// ******************************************************************
// * XGCompressRect
// ******************************************************************
OOVPA_NO_XREF(XGCompressRect, 4361, 8)

        { 0x1E, 0x00 },
        { 0x3E, 0xA8 },
        { 0x5E, 0x85 },
        { 0x80, 0x85 },
        { 0x9E, 0x07 },
        { 0xBE, 0x80 },
        { 0xDE, 0x74 },
        { 0xFE, 0x8B },
OOVPA_END;
#endif

#if 0 // Moved to 4134
// ******************************************************************
// * XGSetIndexBufferHeader
// ******************************************************************
OOVPA_NO_XREF(XGSetIndexBufferHeader, 4361, 13)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x18 },
        { 0x0A, 0x08 },

        { 0x0C, 0xC7 },
        { 0x0D, 0x00 },
        { 0x0E, 0x01 },
        { 0x0F, 0x00 },
        { 0x10, 0x01 },
        { 0x11, 0x00 },
        { 0x12, 0x89 },
        { 0x13, 0x48 },

        { 0x16, 0x18 },
OOVPA_END;
#endif

// ******************************************************************
// * XFONT_OpenBitmapFontFromMemory
// ******************************************************************
OOVPA_NO_XREF(XFONT_OpenBitmapFontFromMemory, 4361, 8)

        { 0x0B, 0x75 },
        { 0x1A, 0x8B },
        { 0x28, 0x8B },
        { 0x32, 0x08 },
        { 0x3F, 0x8B },
        { 0x4C, 0x8B },
        { 0x59, 0x45 },
        { 0x66, 0x0C },
OOVPA_END;
