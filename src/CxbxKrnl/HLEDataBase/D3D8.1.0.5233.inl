// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.5233.inl
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
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 5233, 7)

        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
OOVPA_END;
#endif

#if 0 // Moved to 4627
// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInlineFast, 5233, 13)

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x00 : push esi
        { 0x00, 0x56 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x26 : cmp eax, 0x10
        { 0x26, 0x83 },
        { 0x27, 0xF8 },
        { 0x28, 0x10 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x29 : jnb +0x1F
        { 0x29, 0x73 },
        { 0x2A, 0x1F },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x33 : or eax, 0x0B80
        { 0x33, 0x0D },
        { 0x34, 0x80 },
        { 0x35, 0x0B },
        { 0x36, 0x00 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x3B : rep movsd
        { 0x3B, 0xF3 },
        { 0x3C, 0xA5 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x43 : emms
        { 0x43, 0x0F },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 5233, 7)

        { 0x1E, 0x75 },
        { 0x3E, 0x0D },
        { 0x60, 0x8B },
        { 0x7E, 0x04 },
        { 0x9E, 0x20 },
        { 0xBE, 0x08 },
        { 0xE1, 0xF6 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 5233, 12)

        // D3DDevice_CreateTexture2+0x04 : lea eax, [esp+0x20]
        { 0x04, 0x8D },
        { 0x07, 0x20 },

        // D3DDevice_CreateTexture2+0x10 : setz dl
        { 0x10, 0x0F },
        { 0x11, 0x94 },
        { 0x12, 0xC2 },

        // D3DDevice_CreateTexture2+0x2E : push 0x00
        { 0x2E, 0x6A },
        { 0x2F, 0x00 },

        // D3DDevice_CreateTexture2+0x57 : call [abs]
        { 0x57, 0xE8 },

        // D3DDevice_CreateTexture2+0x7C : jnz +0x0D
        { 0x7C, 0x75 },
        { 0x7D, 0x0D },

        // D3DDevice_CreateTexture2+0xB2 : retn 0x1C
        { 0xB2, 0xC2 },
        { 0xB3, 0x1C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 5233, 2+13,

    XREF_D3DDevice_SetRenderState_CullMode,
	XRefTwo)

		XREF_ENTRY( 0x03, XREF_D3DDEVICE ), // Derived

		XREF_ENTRY( 0x2B, XREF_D3DRS_CULLMODE ), // Derived

        // D3DDevice_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 },
        { 0x1B, 0x08 },
        { 0x1C, 0x03 },
        { 0x1D, 0x04 },

        // D3DDevice_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 },
        { 0x25, 0xC0 },
        { 0x26, 0x08 },

        // D3DDevice_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 },
        { 0x31, 0x04 },

        // D3DDevice_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 },
        { 0x54, 0xC2 },
        { 0x55, 0x04 },
        { 0x56, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTexture, 5233, 1+10,

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x13, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived

        { 0x16, 0x00 },
        { 0x17, 0x85 },
        { 0x18, 0xC0 },
        { 0x19, 0x89 },
        { 0x1A, 0x6C },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x89 },
        { 0xDE, 0xFF },
        { 0xFE, 0xE2 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 5233, 9)

        { 0x1B, 0xE8 },
        { 0x48, 0x00 },
        { 0x49, 0x48 },
        { 0x4A, 0x83 },
        { 0x4B, 0xE0 },
        { 0x4C, 0x01 },
        { 0x6D, 0xFB },
        { 0x7C, 0x6A },
        { 0xA6, 0xE8 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5233, 12) // Up to 5849

        { 0x00, 0x83 },
        { 0x13, 0x89 },
        { 0x14, 0x88 },
        { 0x74, 0x4F },

        { 0x80, 0xE1 },
        { 0x81, 0xFE },
        { 0x82, 0x0F },
        { 0x83, 0xAF },
        { 0x84, 0xCA },
        { 0x85, 0x25 },
        { 0x86, 0xFF },
        { 0x87, 0xFF },
OOVPA_END;

#if 0 // No longer used, replaced by generic 5233 version
// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5233, 11)

        // D3DDevice_UpdateOverlay+0x13 : mov [eax+0x17BC], ecx
        { 0x13, 0x89 },
        { 0x14, 0x88 },
        { 0x15, 0xBC },
        { 0x16, 0x17 },

        // D3DDevice_UpdateOverlay+0x92 : and eax, 0xFFFFFFFE
        { 0x92, 0x83 },
        { 0x93, 0xE0 },
        { 0x94, 0xFE },

        // D3DDevice_UpdateOverlay+0xB0 : mov [esi+0x8930], eax
        { 0xB0, 0x89 },
        { 0xB1, 0x86 },
        { 0xB2, 0x30 },
        { 0xB3, 0x89 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 5233, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x1FB0]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xB0 },
        { 0x08, 0x1F },

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x1988]
        { 0x0C, 0x8B },
        { 0x0D, 0xB0 },
        { 0x0E, 0x88 },
        { 0x0F, 0x19 },

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F },
        { 0x17, 0x95 },
        { 0x18, 0xC2 },

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5233, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x195C
        { 0x17, 0x05 },
        { 0x18, 0x5C },
        { 0x19, 0x19 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 5233, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x10 },
        { 0x21, 0x05 },
        { 0x28, 0x00 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;
#endif

#if 0 // Moved to 5120
// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 5120, 8)

        { 0x1E, 0xE1 },
        { 0x42, 0x84 },
        { 0x5E, 0x24 },
        { 0x7E, 0x8B },
        { 0x9E, 0x1C },
        { 0xBE, 0x00 },
        { 0xDE, 0xBD },
        { 0xFE, 0x4C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 5233, 8)

        { 0x07, 0x2C },
        { 0x28, 0x44 },
        { 0x60, 0x38 },
        { 0x7D, 0x54 },
        { 0xAC, 0x03 },
        { 0xB8, 0xD9 },
        { 0xD4, 0x1C },
        { 0xFE, 0x4C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5233, 8)

        // D3DDevice_SetScreenSpaceOffset+0x13 : fstp [esi+0x0AA8]
        { 0x13, 0xD9 },
        { 0x14, 0x9E },
        { 0x15, 0xA8 },
        { 0x16, 0x0A },

        // D3DDevice_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 },
        { 0x34, 0x05 },

        // D3DDevice_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 },
        { 0x47, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 5233, 13)

        // D3DDevice_SetRenderState_MultiSampleMode+0x04 : mov ecx, ds:dword_XXXX
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov edx, [ecx+XXXXh]
        { 0x0F, 0x8B },
        { 0x10, 0x91 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp edx, [ecx+XXXXh]
        { 0x15, 0x3B },
        { 0x16, 0x91 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1B : jnz short
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1D : xor edx, edx
        { 0x1D, 0x33 },
        { 0x1E, 0xD2 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x24 : retn 4
        { 0x24, 0xC2 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5233, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE ), // Derived

        { 0x03, 0x04 },
        { 0x0F, 0x8B },
        { 0x15, 0x3B },
        { 0x1C, 0x14 },
        { 0x1D, 0x8D },
        { 0x2C, 0xE8 },
        { 0x32, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5028 version
// ******************************************************************
// * D3D::CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget, 5233, 8,

    XREF_D3D_CommonSetRenderTarget,
    XRefZero)

        { 0x02, 0x1C },
        { 0x20, 0x30 },
        { 0x6B, 0x06 },
        { 0x85, 0x4C },
        { 0xA5, 0x10 },
        { 0xB8, 0x0C },
        { 0xDC, 0xD9 },
        { 0xFF, 0x75 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_XREF(D3DDevice_BlockOnFence, 5233, 1+5,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_D3D_BlockOnTime ),

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0D, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_XREF(D3DDevice_SetPalette, 5233, 1+12,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x10, XREF_OFFSET_D3DDEVICE_M_PALETTES ), // Derived

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B },
        { 0x0E, 0x84 },
        { 0x0F, 0xBE },
        { 0x10, 0x48 },
        { 0x11, 0x0B },

        // D3DDevice_SetPalette+0x51 : add ebp, 0x80000
        { 0x51, 0x81 },
        { 0x52, 0xC5 },
        { 0x53, 0x00 },
        { 0x54, 0x00 },
        { 0x55, 0x08 },

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 },
        { 0x8B, 0x08 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 5233, 12)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x17C8]
        { 0x1C, 0x8B },
        { 0x1D, 0x80 },
        { 0x1E, 0xC8 },
        { 0x1F, 0x17 },

        // D3DDevice_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A },
        { 0x23, 0x00 },
        { 0x24, 0x56 },
        { 0x25, 0x6A },
        { 0x26, 0x0B },
        { 0x27, 0x50 },

        // D3DDevice_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 5233, 12)

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x17C8]
        { 0x25, 0x8B },
        { 0x26, 0x90 },
        { 0x27, 0xC8 },
        { 0x28, 0x17 },

        // D3DDevice_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A },
        { 0x2C, 0x00 },
        { 0x2D, 0x56 },
        { 0x2E, 0x6A },
        { 0x2F, 0x0E },
        { 0x30, 0x52 },

        // D3DDevice_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5233, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1958], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x58 }, // 54 vs 58
        { 0x0D, 0x19 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5233, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x1954], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x54 }, // 54 vs 58
        { 0x0D, 0x19 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 5233, 7)

        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderTargetFast, 5233, 1+27,
	
	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x15, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived

		// D3DDevice_SetRenderTargetFast+0x00 : push ebp
		{ 0x00, 0x55 },

		// D3DDevice_SetRenderTargetFast+0x01 : mov ebp, [esp+8]
		{ 0x01, 0x8B },
		{ 0x02, 0x6C },
		{ 0x03, 0x24 },
		{ 0x04, 0x08 },

		// D3DDevice_SetRenderTargetFast+0x05 : push esi
		{ 0x05, 0x56 },

		// D3DDevice_SetRenderTargetFast+0x06 : push edi
		{ 0x06, 0x57 },

		// D3DDevice_SetRenderTargetFast+0x07 : mov edi, [ds:{addr}]
		{ 0x07, 0x8B },
		{ 0x08, 0x3D },

		// D3DDevice_SetRenderTargetFast+0x0D : push ebp
		{ 0x0D, 0x55 },

		// D3DDevice_SetRenderTargetFast+0x0E : call {addr}
		{ 0x0E, 0xE8 },

		// D3DDevice_SetRenderTargetFast+0x13 : mov eax, [edi+{offset}]
		{ 0x13, 0x8B },
		{ 0x14, 0x87 },
	
		// D3DDevice_SetRenderTargetFast+0x19 : mov ecx, [eax+0x14]
		{ 0x19, 0x8B },
		{ 0x1A, 0x48 },
		{ 0x1B, 0x14 },
	
		// D3DDevice_SetRenderTargetFast+0x1C : test ecx, ecx
		{ 0x1C, 0x85 },
		{ 0x1D, 0xC9 },

		// D3DDevice_SetRenderTargetFast+0x1E : jz + 0x02
		{ 0x1E, 0x74 },
		{ 0x1F, 0x02 },
	
        { 0x3C, 0x89 },
        { 0x5F, 0x8B },
        { 0x7A, 0x04 },
        { 0x99, 0x04 },
        { 0xB8, 0x02 },
        { 0xD7, 0x0C },
        { 0xF6, 0xC5 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVisibilityTestResult
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVisibilityTestResult, 5233, 7)

        { 0x0E, 0x08 },
        { 0x10, 0xFF },
        { 0x2A, 0x16 },
        { 0x3C, 0x76 },
        { 0x3F, 0x0C },
        { 0x51, 0x0A },
        { 0x5F, 0x0C },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4831 version
// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 5233, 9)

        { 0x1A, 0x0C },
        { 0x1B, 0x8B },
        { 0x1C, 0x45 },
        { 0x1D, 0x10 },
        { 0x1E, 0x85 },
        { 0x5B, 0xF3 },
        { 0x60, 0xE0 },
        { 0x64, 0xC0 },
        { 0x68, 0x2D },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 5233, 7)

        { 0x11, 0x8D },
        { 0x22, 0x50 },
        { 0x34, 0x0C },
        { 0x46, 0x08 },
        { 0x58, 0x00 },
        { 0x6D, 0x8B },
        { 0x7C, 0x88 },
OOVPA_END;
#endif
