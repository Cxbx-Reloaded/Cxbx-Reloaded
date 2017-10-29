// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4242.inl
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
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4242, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x30 }, // 2C vs 30
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4242, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x2C }, // 2C vs 30
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4242, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0440]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x40 },
        { 0x08, 0x04 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x05 : mov [eax+0x0440], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x40 },
        { 0x0F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4242, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY(0x19, XREF_D3D_TextureState_TexCoordIndex), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x0D : shl eax, 0x07
        { 0x0D, 0xC1 },
        { 0x0E, 0xE0 },
        { 0x0F, 0x07 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x24 : cmp eax, ecx
        { 0x24, 0x3B },
        { 0x25, 0xC1 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x6B : mov esi, 0x2400
        { 0x6B, 0xBE },
        { 0x6D, 0x24 },

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB3, 0xC1 },
        { 0xB4, 0xE2 },
        { 0xB5, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4242, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2434
        { 0x17, 0x05 },
        { 0x18, 0x34 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CMiniport_IsFlipPending
// ******************************************************************
OOVPA_NO_XREF(CMiniport_IsFlipPending, 4242, 17)

        { 0x00, 0x8B },
        { 0x01, 0x81 },
        { 0x02, 0xF4 },
        { 0x03, 0x01 },
        { 0x04, 0x00 },
        { 0x05, 0x00 },
        { 0x06, 0x83 },
        { 0x07, 0xE0 },
        { 0x08, 0x01 },
        { 0x09, 0x8B },
        { 0x0A, 0x84 },
        { 0x0B, 0xC1 },
        { 0x0C, 0xB4 },
        { 0x0D, 0x01 },
        { 0x0E, 0x00 },
        { 0x0F, 0x00 },
        { 0x10, 0xC3 },
OOVPA_END;
