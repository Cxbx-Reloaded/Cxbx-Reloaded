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

// ******************************************************************
// * Direct3D_CreateDevice
// ******************************************************************
#define Direct3D_CreateDevice_5233 Direct3D_CreateDevice_5028

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

// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInline, 5233, 12)

        // D3DDevice_SetVertexShaderConstantNotInline+0x26 : cmp eax, 0x10
        { 0x26, 0x83 }, // (Offset,Value)-Pair #1
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x28, 0x10 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstantNotInline+0x29 : jnb +0x1F
        { 0x29, 0x73 }, // (Offset,Value)-Pair #4
        { 0x2A, 0x1F }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstantNotInline+0x33 : or eax, 0x0B80
        { 0x33, 0x0D }, // (Offset,Value)-Pair #6
        { 0x34, 0x80 }, // (Offset,Value)-Pair #7
        { 0x35, 0x0B }, // (Offset,Value)-Pair #8
        { 0x36, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstantNotInline+0x3B : rep movsd
        { 0x3B, 0xF3 }, // (Offset,Value)-Pair #10
        { 0x3C, 0xA5 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetVertexShaderConstantNotInline+0x43 : emms
        { 0x43, 0x0F }, // (Offset,Value)-Pair #12
OOVPA_END;

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

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 5233, 12)

        // D3DDevice_CreateTexture2+0x04 : lea eax, [esp+0x20]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x07, 0x20 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateTexture2+0x10 : setz dl
        { 0x10, 0x0F }, // (Offset,Value)-Pair #3
        { 0x11, 0x94 }, // (Offset,Value)-Pair #4
        { 0x12, 0xC2 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateTexture2+0x2E : push 0x00
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #6
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateTexture2+0x57 : call [abs]
        { 0x57, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_CreateTexture2+0x7C : jnz +0x0D
        { 0x7C, 0x75 }, // (Offset,Value)-Pair #9
        { 0x7D, 0x0D }, // (Offset,Value)-Pair #10

        // D3DDevice_CreateTexture2+0xB2 : retn 0x1C
        { 0xB2, 0xC2 }, // (Offset,Value)-Pair #11
        { 0xB3, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 5233, 2+13,

    XREF_SETRENDERSTATE_CULLMODE,
	XRefTwo)

		XREF_ENTRY(0x03, XREF_D3DDEVICE), // Derived
		XREF_ENTRY(0x2B, XREF_D3DRS_CULLMODE), // Derived

        // D3DDevice_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #2
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #5
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x26, 0x08 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #10
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x04 }, // (Offset,Value)-Pair #12
        { 0x56, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
#define D3DDevice_SetRenderState_TextureFactor_5233 D3DDevice_SetRenderState_TextureFactor_5028

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
#define D3DDevice_SetRenderState_ZEnable_5233 D3DDevice_SetRenderState_ZEnable_5028


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

// ******************************************************************
// * D3DTexture_LockRect
// ******************************************************************
#define D3DTexture_LockRect_5233 D3DTexture_LockRect_3925


// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
#define D3DDevice_GetBackBuffer2_5233 D3DDevice_GetBackBuffer2_5028

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 5233, 11)

        // D3DDevice_UpdateOverlay+0x13 : mov [eax+0x17BC], ecx
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x88 }, // (Offset,Value)-Pair #2
        { 0x15, 0xBC }, // (Offset,Value)-Pair #3
        { 0x16, 0x17 }, // (Offset,Value)-Pair #4

        // D3DDevice_UpdateOverlay+0x92 : and eax, 0xFFFFFFFE
        { 0x92, 0x83 }, // (Offset,Value)-Pair #5
        { 0x93, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x94, 0xFE }, // (Offset,Value)-Pair #7

        // D3DDevice_UpdateOverlay+0xB0 : mov [esi+0x8930], eax
        { 0xB0, 0x89 }, // (Offset,Value)-Pair #8
        { 0xB1, 0x86 }, // (Offset,Value)-Pair #9
        { 0xB2, 0x30 }, // (Offset,Value)-Pair #10
        { 0xB3, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 5233, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x1FB0]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x08, 0x1F }, // (Offset,Value)-Pair #4

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x1988]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x0E, 0x88 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x19 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5233, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x195C
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x5C }, // (Offset,Value)-Pair #8
        { 0x19, 0x19 }, // (Offset,Value)-Pair #9

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
#define D3DDevice_GetDepthStencilSurface2_5233 D3DDevice_GetDepthStencilSurface2_5028

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
#define D3DDevice_SetLight_5233 D3DDevice_SetLight_5028

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
#define D3DDevice_LightEnable_5233 D3DDevice_LightEnable_5028

// ******************************************************************
// * D3DVertexBuffer_GetDesc
// ******************************************************************
#define D3DVertexBuffer_GetDesc_5233 D3DVertexBuffer_GetDesc_5028


// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
#define D3DDevice_SetTile_5233 D3DDevice_SetTile_5028

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
#define D3DDevice_AddRef_5233 D3DDevice_AddRef_5028

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

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 5233, 8)

        { 0x1E, 0xE1 },
        { 0x42, 0x84 },
        { 0x5E, 0x24 },
        { 0x7E, 0x8B },
        { 0x9E, 0x1C },
        { 0xBE, 0x00 },
        { 0xDE, 0xBD },
        { 0xFE, 0x4C },
OOVPA_END;

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

// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 5233, 8)

        // D3DDevice_SetScreenSpaceOffset+0x13 : fstp [esi+0x0AA8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x14, 0x9E }, // (Offset,Value)-Pair #2
        { 0x15, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x0A }, // (Offset,Value)-Pair #4

        // D3DDevice_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

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

// ******************************************************************
// * D3D::CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget, 5233, 8,

    XREF_COMMONSETRENDERTARGET,
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

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
#define D3D_SetFence_5233 D3D_SetFence_5028


// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
#define D3DDevice_IsFencePending_5233 D3DDevice_IsFencePending_5028


// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_XREF(D3DDevice_BlockOnFence, 5233, 6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_D3D_BLOCKONTIME ),

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0D, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 5233, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x48 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0B }, // (Offset,Value)-Pair #5

        // D3DDevice_SetPalette+0x51 : add ebp, 0x80000
        { 0x51, 0x81 }, // (Offset,Value)-Pair #6
        { 0x52, 0xC5 }, // (Offset,Value)-Pair #7
        { 0x53, 0x00 }, // (Offset,Value)-Pair #8
        { 0x54, 0x00 }, // (Offset,Value)-Pair #9
        { 0x55, 0x08 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x8B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 5233, 12)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x17C8]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A }, // (Offset,Value)-Pair #5
        { 0x23, 0x00 }, // (Offset,Value)-Pair #6
        { 0x24, 0x56 }, // (Offset,Value)-Pair #7
        { 0x25, 0x6A }, // (Offset,Value)-Pair #8
        { 0x26, 0x0B }, // (Offset,Value)-Pair #9
        { 0x27, 0x50 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x40, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 5233, 12)

        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x17C8]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x28, 0x17 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x56 }, // (Offset,Value)-Pair #7
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #8
        { 0x2F, 0x0E }, // (Offset,Value)-Pair #9
        { 0x30, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_Unknown1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Unknown1, 5233, 9)

	// D3DDevice_Unknown1+0x00 : xor eax, eax
	{ 0x00, 0x33 },
	{ 0x01, 0xC0 },
	// D3DDevice_Unknown1+0x13 : call ds:AvSendTVEncoderOption
	{ 0x13, 0xFF },
	{ 0x14, 0x15 },
	{ 0x15, 0x08 },
	{ 0x16, 0x94 },
	{ 0x17, 0x19 },
	{ 0x18, 0x00 },
	// D3DDevice_Unknown1+0x1E : retn
	{ 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
#define D3DDevice_BeginPush_5233 D3DDevice_BeginPush_5028


// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 5233, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1958], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x58 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x19 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

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

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
#define D3DDevice_End_5233 D3DDevice_End_5028

// ******************************************************************
// * D3D8_5233
// ******************************************************************
OOVPATable D3D8_5233[] = {

	REGISTER_OOVPA(Direct3D_CreateDevice, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 5233, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 5233, PATCH),
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 5233, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 5028, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 5233, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 5028, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4361, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 5233, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 5028, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 4134, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 4361, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 5233, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 5233, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 5233, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 5028, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 5233, PATCH),
	// REGISTER_OOVPA(D3DDevice_Unknown1, 5233, PATCH)
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH), // Beware of the typo...
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4361, PATCH), // Was 4627
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 5233, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_SampleAlpha, 5028, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4134, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4627, PATCH),
};

// ******************************************************************
// * D3D8_5233_SIZE
// ******************************************************************
uint32 D3D8_5233_SIZE = sizeof(D3D8_5233);
