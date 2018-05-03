// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4134.inl
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
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 4134, 12)

        // D3DDevice_GetBackBuffer+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0xFF },

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x07
        { 0x0D, 0x75 },
        { 0x0E, 0x07 },

        // D3DDevice_GetBackBuffer+0x18 : sbb eax, eax
        { 0x18, 0x1B },
        { 0x19, 0xC0 },

        // D3DDevice_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x207C]
        { 0x1D, 0x8B },
        { 0x1E, 0x84 },
        { 0x1F, 0x81 },
        { 0x20, 0x7C },
        { 0x21, 0x20 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 4134, 9)
#else
OOVPA_XREF(D3DDevice_SetRenderTarget, 4134, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x17, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived TODO : Verify offset
#endif
        // D3DDevice_SetRenderTarget+0x00 : sub esp, 0x3C
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x3C },

        // D3DDevice_SetRenderTarget+014 : jnz +0x0C
        { 0x14, 0x75 },
        { 0x15, 0x0C },

        // D3DDevice_SetRenderTarget+0x44 : push ebp
        { 0x44, 0x55 },

        // D3DDevice_SetRenderTarget+0x79 : shr ecx, 0x14
        { 0x79, 0xC1 },
        { 0x7A, 0xE9 },
        { 0x7B, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4134, 13)

        // D3DDevice_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B },
        { 0x1C, 0x96 },
        { 0x1D, 0x74 },
        { 0x1E, 0x20 },

        // D3DDevice_SetRenderState_ZEnable+0x31 : mov dword ptr [eax], 0x4030C
        { 0x31, 0xC7 },
        { 0x32, 0x00 },
        { 0x33, 0x0C },
        { 0x34, 0x03 },
        { 0x35, 0x04 },

        // D3DDevice_SetRenderState_ZEnable+0x98 : retn 0x04
        { 0x98, 0xC2 },
        { 0x99, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4134, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 },
        { 0x07, 0xC3 },
        { 0x08, 0x01 },

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B },
        { 0x12, 0x86 },
        { 0x13, 0x80 },
        { 0x14, 0x03 },

        // D3DDevice_SetVertexShader+0x90 : retn 0x04
        { 0x90, 0xC2 },
        { 0x91, 0x04 },

        // D3DDevice_SetVertexShader+0xB0 : mov dword ptr [eax], 0x4194C
        { 0xB0, 0xC7 },
        { 0xB1, 0x00 },
        { 0xB2, 0x4C },
        { 0xB3, 0x19 },
        { 0xB4, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4134, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0940
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0xF0 },
        { 0x0F, 0x09 },

        // D3DDevice_SetMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 },
        { 0x13, 0x11 },

        // D3DDevice_SetMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 },
        { 0x20, 0xC9 },
        { 0x21, 0x00 },
        { 0x22, 0x10 },

        // D3DDevice_SetMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 4134, 11)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0xF0 },
        { 0x0E, 0x09 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0xB9 },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 4134, 12)

        // D3DDevice_SetBackMaterial+0x0C : add edi, 0x0A34
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x34 },
        { 0x0F, 0x0A },

        // D3DDevice_SetBackMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 },
        { 0x13, 0x11 },

        // D3DDevice_SetBackMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 },
        { 0x20, 0xC9 },
        { 0x21, 0x00 },
        { 0x22, 0x10 },

        // D3DDevice_SetBackMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 4134, 11)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0x34 },
        { 0x0E, 0x0A },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0xB9 },
        { 0x12, 0x11 },

        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B },
        { 0x10, 0x88 },
        { 0x11, 0x70 },
        { 0x12, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B },
        { 0x16, 0x88 },
        { 0x17, 0x7C },
        { 0x18, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1B : jnz +0x0D
        { 0x1B, 0x75 },
        { 0x1C, 0x0D },

        // D3DDevice_SetRenderState_MultiSampleMode+0x2A : retn 0x04
        { 0x2A, 0xC2 },
        { 0x2B, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 4134, 12,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x0D, 0x72 },
        { 0x0E, 0x0E },
        { 0x31, 0xBA },
        { 0x32, 0x90 },
        { 0x33, 0x1D },
        { 0x34, 0x04 },
        { 0x35, 0x00 },
        { 0x45, 0x83 },
        { 0x46, 0xE1 },
        { 0x47, 0x3F },
        { 0xAA, 0xC2 },
        { 0xAB, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4134, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2430
        { 0x17, 0x05 },
        { 0x18, 0x30 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
/* NOTE: This may look the similar as SetSwapCallback. 
   It might be a Offset 0Ch is increment +4. */
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4134, 12)

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
        { 0x0C, 0x2C }, // 28 vs 2C
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
/* This may look the same as 4361, but accuracy is not gauranteed.
   It might be a SetVerticalBlankCallback that has an identical signature... */
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4134, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x2428], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x28 }, // 28 vs 2C
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4134, 18)

        { 0x01, 0x0D },

        { 0x0D, 0x56 },
        { 0x0E, 0x8B },
        { 0x0F, 0x74 },
        { 0x10, 0x24 },
        { 0x11, 0x08 },
        { 0x12, 0x74 },
        { 0x13, 0x08 },
        { 0x14, 0x39 },
        { 0x15, 0x35 },

        { 0x22, 0x6A },
        { 0x23, 0x00 },
        { 0x24, 0x56 },
        { 0x25, 0x6A },
        { 0x26, 0x0B },
        { 0x27, 0x50 },

        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4134, 13)

        { 0x01, 0x0D },

        { 0x23, 0x74 },
        { 0x24, 0x28 },
        { 0x25, 0x8B },
        { 0x26, 0x90 },

        { 0x2B, 0x6A },
        { 0x2C, 0x00 },
        { 0x2D, 0x56 },
        { 0x2E, 0x6A },
        { 0x2F, 0x0E },
        { 0x30, 0x52 },

        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },

/*
        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x2268]
        { 0x25, 0x8B },
        { 0x26, 0x90 },
        { 0x27, 0x68 },
        { 0x28, 0x22 },*/

OOVPA_END;

// ******************************************************************
// * D3DDevice_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EnableOverlay, 4134, 11)

        // D3DDevice_EnableOverlay+0x0B : mov ecx, [eax+0x8700]
        { 0x0B, 0x8B },
        { 0x0C, 0x88 },
        { 0x0E, 0x87 },

        // D3DDevice_EnableOverlay+0x15 : jz +0x0A
        { 0x15, 0x74 },
        { 0x16, 0x0A },

        // D3DDevice_EnableOverlay+0x5A : mov [eax+0x8918], ecx
        { 0x5A, 0x89 },
        { 0x5B, 0x88 },
        { 0x5C, 0x18 },
        { 0x5D, 0x89 },

        // D3DDevice_EnableOverlay+0x60 : retn 0x04
        { 0x60, 0xC2 },
        { 0x61, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetModelView, 4134, 7)

        { 0x05, 0x57 },
        { 0x0A, 0x85 },
        { 0x10, 0xB0 },
        { 0x16, 0x10 },
        { 0x1C, 0x5E },
        { 0x22, 0xC1 },
        { 0x28, 0xE0 },
OOVPA_END;

// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 4134, 28, // Also for 4361, 4627

    XREF_D3D_MakeRequestedSpace,
    XRefZero)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x08 },
        { 0x03, 0x56 },
        { 0x04, 0x8B },
        { 0x05, 0x35 },

        { 0x0A, 0xF6 },
        { 0x0B, 0x46 },
        { 0x0C, 0x08 },
        { 0x0D, 0x04 },
        { 0x0E, 0x8B },
        { 0x0F, 0x0E },
        { 0x10, 0x57 },
        { 0x11, 0x74 },
        { 0x12, 0x26 },
        { 0x13, 0x8B },
        { 0x14, 0x86 },
        { 0x15, 0x50 },
        { 0x16, 0x03 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x8B },
        { 0x1A, 0x78 },
        { 0x1B, 0x04 },
        { 0x1C, 0x8B },
        { 0x1D, 0x96 },
        { 0x1E, 0x54 },
        { 0x1F, 0x03 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush2, 4134, 12)

        { 0x00, 0x56 },
        { 0x07, 0x6A },

        { 0x10, 0x8B },
        { 0x11, 0x44 },
        { 0x12, 0x24 },
        { 0x13, 0x08 },
        { 0x14, 0x40 },
        { 0x15, 0x50 },
        { 0x16, 0x56 },
        { 0x17, 0xE8 },
        { 0x23, 0xC2 },
        { 0x24, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_XREF(D3DDevice_EndPush, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE ), // Derived

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 4134, 10)

        { 0x07, 0x57 },
        { 0x1C, 0xBE },

        { 0x50, 0xFC },
        { 0x51, 0xFD },
        { 0x52, 0xFF },
        { 0x53, 0xFF },
        { 0x54, 0x8B },
        { 0x55, 0x46 },
        { 0x56, 0x08 },
        { 0x57, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView, 4134, 7)

        { 0x15, 0x25 },
        { 0x2F, 0x81 },
        { 0x43, 0x43 },
        { 0x5A, 0x04 },
        { 0x72, 0x8D },
        { 0x88, 0x00 },
        { 0x9F, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 4134, 10)

        { 0x00, 0x56 },
        { 0x08, 0x06 },

        { 0x15, 0x24 },
        { 0x16, 0x08 },
        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0xCD },
        { 0x1A, 0x80 },
        { 0x1B, 0x19 },
        { 0x1F, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInput, 4134, 16)

        { 0x03, 0x04 },
        { 0x04, 0x83 },
        { 0x05, 0xEC },
        { 0x06, 0x40 },
        { 0x07, 0x85 },
        { 0x08, 0xC0 },
        { 0x09, 0x53 },
        { 0x0A, 0x8B },
        { 0x0B, 0x1D },

        { 0x19, 0x25 },
        { 0x1A, 0xFF },
        { 0x1B, 0xFF },
        { 0x1C, 0xFF },
        { 0x1D, 0xBF },
        { 0x1E, 0x83 },
        { 0x1F, 0xC8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache, 4134, 15)

        { 0x02, 0x2D },

        { 0x07, 0x8B },
        { 0x08, 0x45 },
        { 0x09, 0x1C },
        { 0x0A, 0x56 },
        { 0x0B, 0x50 },
        { 0x0C, 0x8B },
        { 0x0D, 0xCD },

        { 0x19, 0x00 },
        { 0x1A, 0x8B },
        { 0x1B, 0x4D },
        { 0x1C, 0x04 },
        { 0x1D, 0xD1 },
        { 0x1E, 0xEE },
        { 0x1F, 0x3B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback, 4134, 13)

        { 0x02, 0x35 },

        { 0x07, 0x8B },
        { 0x0E, 0xA1 },

        { 0x1F, 0x0C },
        { 0x20, 0x8B },
        { 0x21, 0x54 },
        { 0x22, 0x24 },
        { 0x23, 0x10 },
        { 0x24, 0xC7 },
        { 0x25, 0x00 },
        { 0x26, 0x8C },
        { 0x27, 0x1D },
        { 0x28, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x18 },
        { 0x08, 0x20 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunVertexStateShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunVertexStateShader, 4134, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0xC0 },
        { 0x2A, 0x40 },
        { 0x35, 0xF4 },
        { 0x40, 0xC7 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 4134, 16) // Up to 4531

        { 0x02, 0x35 },

        { 0x07, 0x57 },

        { 0x0E, 0x70 }, // 4361 0x7004
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0xB9 },
        { 0x13, 0x10 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0xF3 },
        { 0x18, 0xA5 },
        { 0x19, 0x5F },
        { 0x1A, 0x5E },
        { 0x1B, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 4134, 7)

        { 0x08, 0x06 },
        { 0x13, 0x8B },
        { 0x1C, 0x04 },
        { 0x26, 0x0F },
        { 0x30, 0x24 },
        { 0x3A, 0x24 },
        { 0x44, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4134, 12)

        { 0x14, 0x3D },
        { 0x36, 0x8B },

        { 0x50, 0x83 },
        { 0x51, 0xC3 },
        { 0x52, 0x08 },
        { 0x53, 0xFF },
        { 0x54, 0x45 },
        { 0x55, 0xF8 },
        { 0x56, 0x8B },
        { 0x57, 0x45 },
        { 0x58, 0xF8 },

        { 0x6A, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginStateBlock, 4134, 1+5,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_ClearStateBlockFlags ),

        // D3DDevice_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_BeginStateBlock+0x05 : mov [eax+8], 0x20
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_EndStateBlock, 4134, 1+5,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_RecordStateBlock ),

        // D3DDevice_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_EndStateBlock+0x05 : and [eax+8], 0xFFFFFFDF
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x08 },
        { 0x08, 0xDF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x84 },
        { 0x08, 0x03 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x70 },
        { 0x08, 0x03 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4134, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x043C]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x3C },
        { 0x08, 0x04 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x043C], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x3C },
        { 0x0F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 4134, 11)

        { 0x02, 0x35 },

        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0x8D },
        { 0x1A, 0x00 },
        { 0x1B, 0x19 },
        { 0x1C, 0x04 },
        { 0x1D, 0x00 },
        { 0x1E, 0x0F },
        { 0x1F, 0xBF },
        { 0x25, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache, 4134, 10)

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x08, 0x06 },
        { 0x0D, 0x05 },
        { 0x13, 0xC7 },
        { 0x15, 0x10 },
        { 0x17, 0x04 },
        { 0x1C, 0x00 },
        { 0x21, 0xC0 },
        { 0x26, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale, 4134, 16)

        { 0x09, 0x86 },

        { 0x10, 0xC1 },
        { 0x11, 0xE9 },
        { 0x12, 0x04 },
        { 0x13, 0x83 },
        { 0x14, 0xE1 },
        { 0x15, 0x0F },
        { 0x16, 0x85 },
        { 0x17, 0xC9 },

        { 0x28, 0xD8 },
        { 0x29, 0x4C },
        { 0x2A, 0x24 },
        { 0x2B, 0x0C },
        { 0x2C, 0x83 },
        { 0x2D, 0xE0 },
        { 0x2E, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_MakeSpace
// ******************************************************************
OOVPA_XREF(D3DDevice_MakeSpace, 4134, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0A, XREF_D3D_MakeRequestedSpace ),

        { 0x00, 0xA1 },
        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTexture, named with 2 suffix to match EMUPATCH(D3DDevice_GetTexture2)
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_GetTexture2, 4134, 23) // Up to 4531
#else
OOVPA_XREF(D3DDevice_GetTexture2, 4134, 1+23, // Up to 4531

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0E, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived
#endif
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        { 0x0A, 0x56 },
        { 0x0B, 0x8D },
        { 0x0C, 0xB4 },
        { 0x0D, 0x81 },
        { 0x0E, 0x78 }, // GetTexture 780A vs GetPalette 880A
        { 0x0F, 0x0A },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x8B },
        { 0x13, 0x06 },
        { 0x14, 0x85 },
        { 0x15, 0xC0 },
        { 0x16, 0x74 },

        { 0x18, 0x50 },
        { 0x19, 0xE8 },

        { 0x1E, 0x8B },
        { 0x1F, 0x16 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateVB
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateVB, 4134, 17)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x0C },

        { 0x41, 0x0F },
        { 0x42, 0x84 },
        { 0x43, 0x41 },
        { 0x44, 0x01 },
        { 0x45, 0x00 },
        { 0x46, 0x00 },
        { 0x47, 0x8B },
        { 0x48, 0x86 },
        { 0x49, 0x80 },
        { 0x4A, 0x03 },
        { 0x4B, 0x00 },
        { 0x4C, 0x00 },

        { 0x66, 0x3B },
        { 0x67, 0xC1 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateUP, 4134, 15)

        { 0x00, 0xA1 },

        { 0x2F, 0x8B },
        { 0x30, 0x86 },
        { 0x31, 0x80 },
        { 0x32, 0x03 },
        { 0x33, 0x00 },
        { 0x34, 0x00 },
        { 0x35, 0x8B },
        { 0x36, 0x4E },
        { 0x37, 0x04 },
        { 0x38, 0x53 },
        { 0x39, 0x8B },
        { 0x3A, 0x58 },

        { 0x4F, 0x3B },
        { 0x50, 0xC1 },
OOVPA_END;

// ******************************************************************
// * Rollback support signature(s)
// ******************************************************************
#define D3DDevice_BeginVisibilityTest_4134 D3DDevice_BeginVisibilityTest_3925
#define D3DDevice_EndVisibilityTest_4134 D3DDevice_EndVisibilityTest_3925
