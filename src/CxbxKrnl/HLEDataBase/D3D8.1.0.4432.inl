// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4432.inl
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
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4432, 12)

        { 0x08, 0x06 },
        { 0x0E, 0xE8 },

        { 0x45, 0x48 },
        { 0x46, 0x0C },
        { 0x47, 0x83 },
        { 0x48, 0xC0 },
        { 0x49, 0x10 },
        { 0x4A, 0x89 },
        { 0x4B, 0x06 },
        { 0x4C, 0xA1 },

        { 0x5A, 0x74 },
        { 0x60, 0x2A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4432, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2448
        { 0x17, 0x05 },
        { 0x18, 0x48 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4432, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x2444], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x40 }, // 40 vs 44
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4432, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x2444], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x44 }, // 40 vs 44
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewportOffsetAndScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewportOffsetAndScale, 4432, 12) // Up to 5849

        { 0x00, 0xA1 },
        { 0x19, 0x05 },

        { 0x45, 0x00 },
        { 0x46, 0x8B },
        { 0x47, 0x50 },
        { 0x48, 0x08 },
        { 0x49, 0x33 },
        { 0x4A, 0xC9 },
        { 0x4B, 0x84 },
        { 0x4C, 0xF6 },

        { 0x5B, 0x74 },
        { 0x5C, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes, 4432, 15)

        { 0x03, 0x0C },
        { 0x04, 0x48 },
        { 0x05, 0x83 },
        { 0x06, 0xF8 },
        { 0x07, 0x03 },
        { 0x08, 0x56 },
        { 0x09, 0x8B },
        { 0x0A, 0x35 },

        { 0x0F, 0x77 },
        { 0x10, 0x5F },
        { 0x11, 0xFF },
        { 0x12, 0x24 },
        { 0x13, 0x85 },

        { 0x18, 0x8B },
        { 0x1F, 0x0C },
OOVPA_END;
