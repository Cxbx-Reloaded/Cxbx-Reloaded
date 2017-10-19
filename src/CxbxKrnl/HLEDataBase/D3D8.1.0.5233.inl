// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5233.cpp
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

		XREF_ENTRY(0x03, XREF_D3DDEVICE), // Derived
		XREF_ENTRY(0x2B, XREF_D3DRS_CULLMODE), // Derived

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
OOVPA_NO_XREF(D3DDevice_SetTexture, 5233, 10)

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
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE ),

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
OOVPA_XREF(D3DDevice_BlockOnFence, 5233, 6,

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
OOVPA_NO_XREF(D3DDevice_SetPalette, 5233, 12)

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
OOVPA_NO_XREF(D3DDevice_SetRenderTargetFast, 5233, 8)

        { 0x1D, 0xC9 },
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

// ******************************************************************
// * D3D8_5233
// ******************************************************************
OOVPATable D3D8_5233[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4034, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),

	REGISTER_OOVPA(Direct3D_CreateDevice, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 3911, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 3911, DISABLED),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5120, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 3911, PATCH),
	REGISTER_OOVPA(D3D_SetTileNoWait, 4627, XREF),
	REGISTER_OOVPA(D3DDevice_SetTile, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, ALIAS, D3DDevice_SetVertexShaderConstant1Fast),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, ALIAS, D3DDevice_SetVertexShaderConstantNotInlineFast),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4034, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBig, 5028, XREF), 
	REGISTER_OOVPA(D3DDevice_End, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 3911, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3911, PATCH),// Just calls Lock3DSurface
	REGISTER_OOVPA(Lock2DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4034, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3911, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4432, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4034, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5028, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4034, PATCH), // Beware of the typo...
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 3911, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, XREF),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 5120, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTargetFast, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetStreamSource2, 4627, PATCH),

	REGISTER_OOVPA(D3DDevice_IsBusy, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 5028, PATCH),
	REGISTER_OOVPA(D3D_MakeRequestedSpace, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_LazySetStateVB, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetDepthClipPlanes, 4432, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStipple, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4134, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 5233
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4627, PATCH),
	// ******************************************************************

};

// ******************************************************************
// * D3D8_5233_SIZE
// ******************************************************************
uint32 D3D8_5233_SIZE = sizeof(D3D8_5233);
