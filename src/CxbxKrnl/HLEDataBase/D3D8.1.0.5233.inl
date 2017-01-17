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
// * IDirect3D8_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_CreateDevice_1_0_5233, 8)

        // IDirect3D8_CreateDevice+0x0A : jnz +0x0A
        { 0x0A, 0x75 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x80 : repe stosd
        { 0x80, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x81, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x83 : mov eax, esi
        { 0x83, 0x8B }, // (Offset,Value)-Pair #5
        { 0x84, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x91 : retn 0x18
        { 0x91, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x92, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233, 7)

        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetViewport_1_0_5233, 9)

        // IDirect3DDevice8_SetViewport+0x0A : mov eax, [esi+0x15B4]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0C, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x15 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetViewport+0x63 : shl edi, cl
        { 0x63, 0xD3 }, // (Offset,Value)-Pair #5
        { 0x64, 0xE7 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetViewport+0xDF : and ecx, 0x0F
        { 0xDF, 0x83 }, // (Offset,Value)-Pair #7
        { 0xE0, 0xE1 }, // (Offset,Value)-Pair #8
        { 0xE1, 0x0F }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LoadVertexShader_1_0_5233, 7)

        { 0x09, 0x45 },
        { 0x14, 0x75 },
        { 0x21, 0x8B },
        { 0x2D, 0x8B },
        { 0x35, 0x04 },
        { 0x40, 0x00 },
        { 0x4B, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233, 12)

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x26 : cmp eax, 0x10
        { 0x26, 0x83 }, // (Offset,Value)-Pair #1
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x28, 0x10 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x29 : jnb +0x1F
        { 0x29, 0x73 }, // (Offset,Value)-Pair #4
        { 0x2A, 0x1F }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x33 : or eax, 0x0B80
        { 0x33, 0x0D }, // (Offset,Value)-Pair #6
        { 0x34, 0x80 }, // (Offset,Value)-Pair #7
        { 0x35, 0x0B }, // (Offset,Value)-Pair #8
        { 0x36, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x3B : rep movsd
        { 0x3B, 0xF3 }, // (Offset,Value)-Pair #10
        { 0x3C, 0xA5 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x43 : emms
        { 0x43, 0x0F }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
OOVPA_NO_XREF_LARGE(IDirect3DDevice8_Clear_1_0_5233, 8)
        { 0x1D, 0x8A },
        { 0x3C, 0x8B },
        { 0x5B, 0x00 },
        { 0x7A, 0xFD },
        { 0x99, 0x25 },
        { 0xB8, 0x00 },
        { 0xD7, 0x75 },
        { 0xF6, 0xFF },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPixelShader_1_0_5233, 7)

        { 0x1E, 0x75 },
        { 0x3E, 0x0D },
        { 0x60, 0x8B },
        { 0x7E, 0x04 },
        { 0x9E, 0x20 },
        { 0xBE, 0x08 },
        { 0xE1, 0xF6 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateTexture2_1_0_5233, 12)

        // IDirect3DDevice8_CreateTexture2+0x04 : lea eax, [esp+0x20]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x07, 0x20 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateTexture2+0x10 : setz dl
        { 0x10, 0x0F }, // (Offset,Value)-Pair #3
        { 0x11, 0x94 }, // (Offset,Value)-Pair #4
        { 0x12, 0xC2 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateTexture2+0x2E : push 0x00
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #6
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateTexture2+0x57 : call [abs]
        { 0x57, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateTexture2+0x7C : jnz +0x0D
        { 0x7C, 0x75 }, // (Offset,Value)-Pair #9
        { 0x7D, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateTexture2+0xB2 : retn 0x1C
        { 0xB2, 0xC2 }, // (Offset,Value)-Pair #11
        { 0xB3, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_SetRenderState_CullMode_1_0_5233, 13,

    XREF_DXSRSCULLMODE,
    XRefZero)

        // IDirect3DDevice8_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #2
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #5
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x26, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #10
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x04 }, // (Offset,Value)-Pair #12
        { 0x56, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_5233, 7)

        { 0x0F, 0xB4 },
        { 0x10, 0x15 },
        { 0x16, 0x0D },
        { 0x1F, 0x3C },
        { 0x34, 0x36 },
        { 0x58, 0x05 },
        { 0x75, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_5344, 10)

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0D : mov eax, [edi+XXXXh]
        { 0x0D, 0x8B },
        { 0x0E, 0x87 },

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+1D : and dl, 3Ch
        { 0x1D, 0x80 },
        { 0x1E, 0xE2 },
        { 0x1F, 0x3C },

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+20 : xor esi, esi
        { 0x20, 0x33 },
        { 0x21, 0xF6 },

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+22 : cmp dl, 20h
        { 0x22, 0x80 },
        { 0x23, 0xFA },
        { 0x24, 0x20 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5233, 9)

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x036C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0x6C }, // (Offset,Value)-Pair #3
        { 0x0A, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x2F : nop
        { 0x2F, 0x90 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x41 : retn 0x04
        { 0x41, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x42, 0x04 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_ZEnable_1_0_5233, 8)

        { 0x13, 0x8B },
        { 0x22, 0xC9 },
        { 0x34, 0x89 },
        { 0x46, 0x0C },
        { 0x5A, 0x74 },
        { 0x6D, 0x8B },
        { 0x7C, 0xCE },
        { 0x8E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetIndices_1_0_5233, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTexture_1_0_5233, 8)

        { 0x1E, 0x44 },
        { 0x3E, 0xE8 },
        { 0x5F, 0x8B },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x89 },
        { 0xDE, 0xFF },
        { 0xFE, 0xE2 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureB_1_0_5233, 8)

        { 0x1B, 0x89 },
        { 0x2B, 0xF8 },
        { 0x3B, 0x51 },
        { 0x4B, 0x00 },
        { 0x5B, 0x1C },
        { 0x6B, 0x41 },
        { 0x7B, 0x00 },
        { 0x8B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Begin
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Begin_1_0_5233, 7)

        { 0x07, 0xE8 },
        { 0x0C, 0x8B },
        { 0x13, 0xE8 },
        { 0x1A, 0x24 },
        { 0x21, 0x00 },
        { 0x28, 0x89 },
        { 0x2F, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Swap_1_0_5233, 7)

        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0x3F },
        { 0x8F, 0x8E },
        { 0xAC, 0xC6 },
        { 0xC9, 0xC0 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawIndexedVertices_1_0_5233, 8)

        { 0x1E, 0x5E },
        { 0x3E, 0x17 },
        { 0x5E, 0x00 },
        { 0x7E, 0x8B },
        { 0x9E, 0x89 },
        { 0xBE, 0x2B },
        { 0xDE, 0x00 },
        { 0xFE, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DPalette8_Lock2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DPalette8_Lock2_1_0_5233, 7)

        { 0x02, 0x24 },
        { 0x06, 0x8B },
        { 0x0A, 0x75 },
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DTexture8_LockRect
// ******************************************************************
OOVPA_NO_XREF(IDirect3DTexture8_LockRect_1_0_5233, 7)

        { 0x03, 0x14 },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x8B },
        { 0x17, 0x50 },
        { 0x20, 0xC2 },
        { 0x21, 0x14 },
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_5233, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x15C0]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x55, 0x15 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_5344, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x15F4]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0xF4 }, // (Offset,Value)-Pair #7
        { 0x55, 0x15 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2_1_0_5233, 12)

        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer+0x15 : mov esi, [ecx+eax*4+0x15C0]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0xC0 }, // (Offset,Value)-Pair #9
        { 0x19, 0x15 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_UpdateOverlay_1_0_5233, 11)

        // IDirect3DDevice8_UpdateOverlay+0x13 : mov [eax+0x17BC], ecx
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x88 }, // (Offset,Value)-Pair #2
        { 0x15, 0xBC }, // (Offset,Value)-Pair #3
        { 0x16, 0x17 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x92 : and eax, 0xFFFFFFFE
        { 0x92, 0x83 }, // (Offset,Value)-Pair #5
        { 0x93, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x94, 0xFE }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0xB0 : mov [esi+0x8930], eax
        { 0xB0, 0x89 }, // (Offset,Value)-Pair #8
        { 0xB1, 0x86 }, // (Offset,Value)-Pair #9
        { 0xB2, 0x30 }, // (Offset,Value)-Pair #10
        { 0xB3, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5233, 12)

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x1FB0]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x08, 0x1F }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x1988]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x0E, 0x88 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x19 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5233, 11)

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x195C
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x5C }, // (Offset,Value)-Pair #8
        { 0x19, 0x19 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetShaderConstantMode_1_0_5233, 8)

        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0x89 },
        { 0xF6, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetRenderTarget2_1_0_5233, 6)

        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x74 },
        { 0x16, 0x8B },
        { 0x17, 0xC6 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233, 7)

        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x75 },
        { 0x12, 0x5E },
        { 0x1A, 0x8B },
        { 0x1B, 0xC6 },
OOVPA_END;

// ******************************************************************
// * IDirect3D8_GetDeviceCaps
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_GetDeviceCaps_1_0_5233, 7)

        { 0x04, 0x85 },
        { 0x0A, 0x08 },
        { 0x10, 0x83 },
        { 0x16, 0x08 },
        { 0x1C, 0xC2 },
        { 0x22, 0x0C },
        { 0x29, 0x33 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawVertices_1_0_5233, 7)

        { 0x12, 0x8B },
        { 0x26, 0x8B },
        { 0x37, 0x00 },
        { 0x4A, 0x27 },
        { 0x5D, 0x89 },
        { 0x70, 0x75 },
        { 0x83, 0x40 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetLight_1_0_5233, 12)

        // IDirect3DDevice8_SetLight+0x11 : mov edi, [ebp+0x390]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #1
        { 0x12, 0xBD }, // (Offset,Value)-Pair #2
        { 0x13, 0x90 }, // (Offset,Value)-Pair #3
        { 0x14, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetLight+0x30 : shl eax, 0x04
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x31, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x32, 0x04 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetLight+0xBD : rep stosb
        { 0xBD, 0xF3 }, // (Offset,Value)-Pair #8
        { 0xBE, 0xAB }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetLight+0xFD : mov ecx, [ebx+0x68]
        { 0xFD, 0x8B }, // (Offset,Value)-Pair #10
        { 0xFE, 0x4B }, // (Offset,Value)-Pair #11
        { 0xFF, 0x68 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetLight_1_0_5344, 8)

        { 0x1E, 0x00 },
        { 0x3E, 0x89 },
        { 0x5E, 0x00 },
        { 0x7E, 0x03 },
        { 0x9E, 0xC8 },
        { 0xBE, 0xC9 },
        { 0xDE, 0x03 },
        { 0xFE, 0xC1 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LightEnable_1_0_5233, 13)

        // IDirect3DDevice8_LightEnable+0x0F : cmp ebx, [esi+0x0390]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x90 }, // (Offset,Value)-Pair #3
        { 0x12, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 }, // (Offset,Value)-Pair #5
        { 0x6C, 0xC0 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_LightEnable+0x9E : mov eax, [eax+0x8C]
        { 0x9E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x9F, 0x80 }, // (Offset,Value)-Pair #8
        { 0xA0, 0x8C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_LightEnable+0xBC : jz +0x12
        { 0xBC, 0x74 }, // (Offset,Value)-Pair #10
        { 0xBD, 0x12 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_LightEnable+0xE1 : retn 0x08
        { 0xE1, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xE2, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LightEnable_1_0_5344, 8)

        { 0x1E, 0x8D },
        { 0x3E, 0xC7 },
        { 0x5E, 0xC7 },
        { 0x7E, 0x00 },
        { 0x9E, 0x85 },
        { 0xBE, 0x01 },
        { 0xDE, 0x00 },
        { 0xFE, 0x83 },
OOVPA_END;

// ******************************************************************
// * IDirect3DVertexBuffer8_GetDesc
// ******************************************************************
OOVPA_NO_XREF(IDirect3DVertexBuffer8_GetDesc_1_0_5233, 7)

        { 0x02, 0x24 },
        { 0x06, 0x74 },
        { 0x0A, 0xC7 },
        { 0x0E, 0x00 },
        { 0x15, 0x89 },
        { 0x16, 0x46 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShader_1_0_5233, 14)

        // IDirect3DDevice8_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShader+0x11 : mov eax, [esi+0x037C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0x7C }, // (Offset,Value)-Pair #6
        { 0x14, 0x03 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetVertexShader+0x90 : retn 0x04
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x91, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShader+0xB0 : mov dword ptr [eax], 0x4194C
        { 0xB0, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB1, 0x00 }, // (Offset,Value)-Pair #11
        { 0xB2, 0x4C }, // (Offset,Value)-Pair #12
        { 0xB3, 0x19 }, // (Offset,Value)-Pair #13
        { 0xB4, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTileNoWait
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTileNoWait_1_0_5233, 11)

        // IDirect3DDevice8_SetTileNoWait+0x06 : sub esp, 0x18
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xEC }, // (Offset,Value)-Pair #2
        { 0x08, 0x18 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTileNoWait+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 }, // (Offset,Value)-Pair #4
        { 0x16, 0x46 }, // (Offset,Value)-Pair #5
        { 0x17, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTileNoWait+0x3D : lea edi, [edx+ecx*8+0x1660]
        { 0x3D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x3E, 0xBC }, // (Offset,Value)-Pair #8
        { 0x3F, 0xCA }, // (Offset,Value)-Pair #9
        { 0x40, 0x60 }, // (Offset,Value)-Pair #10
        { 0x41, 0x16 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_AddRef_1_0_5233, 10)

        // IDirect3DDevice8_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_AddRef+0x05 : mov ecx, [eax+0x04FC]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0xFC }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_AddRef+0x0C : mov [eax+0x04FC], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0xFC }, // (Offset,Value)-Pair #9
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Release_1_0_5233, 8)

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
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_KickOffAndWaitForIdle_1_0_5233, 9)

        // IDirect3D8_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3D8_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x2C]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x2C }, // (Offset,Value)-Pair #4

        // IDirect3D8_KickOffAndWaitForIdle+0x08 : push 2
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3D8_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // IDirect3D8_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3D8_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CopyRects
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CopyRects_1_0_5233, 8)

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
// * IDirect3DDevice8_SetScissors
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScissors_1_0_5233, 8)

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
// * IDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5233, 8)

        // IDirect3DDevice8_SetScreenSpaceOffset+0x13 : fstp [esi+0x0AA8]
        { 0x13, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x14, 0x9E }, // (Offset,Value)-Pair #2
        { 0x15, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x0A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetScreenSpaceOffset+0x33 : jb +0x05
        { 0x33, 0x72 }, // (Offset,Value)-Pair #5
        { 0x34, 0x05 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetScreenSpaceOffset+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x47, 0x08 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_5233, 13)

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x04 : mov ecx, ds:dword_XXXX
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x0F : mov edx, [ecx+XXXXh]
        { 0x0F, 0x8B },
        { 0x10, 0x91 },

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x15 : cmp edx, [ecx+XXXXh]
        { 0x15, 0x3B },
        { 0x16, 0x91 },

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x1B : jnz short
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x1D : xor edx, edx
        { 0x1D, 0x33 },
        { 0x1E, 0xD2 },

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x24 : retn 4
        { 0x24, 0xC2 },
        { 0x25, 0x04 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_5233, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE },

        { 0x03, 0x04 },
        { 0x0F, 0x8B },
        { 0x15, 0x3B },
        { 0x1C, 0x14 },
        { 0x1D, 0x8D },
        { 0x2C, 0xE8 },
        { 0x32, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_5344, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE },

        { 0x03, 0x04 },
        { 0x0F, 0x8B },
        { 0x15, 0x3B },
        { 0x1C, 0x07 }, // **
        { 0x1D, 0x33 }, // **
        { 0x1F, 0xE8 }, // **
        { 0x25, 0x04 }, // **
OOVPA_END;

// ******************************************************************
// * D3D::CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget_1_0_5233, 8,

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
OOVPA_XREF(D3D_SetFence_1_0_5233, 7,

    XREF_D3D_SETFENCE,
    XRefZero)

        { 0x0E, 0x05 },
        { 0x18, 0xC9 },
        { 0x3E, 0x3F },
        { 0x5E, 0x28 },
        { 0x86, 0x5D },
        { 0x98, 0xE8 },
        { 0xA2, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_InsertFence
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_InsertFence_1_0_5233, 5,

    XRefNoSaveIndex,
    XRefOne)

        { 0x03, XREF_D3D_SETFENCE },

        { 0x00, 0x6A },
        { 0x01, 0x00 },
        { 0x02, 0xE8 },
        { 0x07, 0xC3 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_IsFencePending
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_IsFencePending_1_0_5233, 6,

    XRefNoSaveIndex,
    XRefOne)

        { 0x01, XREF_D3DDEVICE },

        { 0x07, 0x30 },
        { 0x0A, 0x2C },
        { 0x10, 0xD1 },
        { 0x17, 0x1B },
        { 0x1C, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime_1_0_5233, 6,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

        { 0x09, 0x30 },
        { 0x27, 0x07 },
        { 0x55, 0x7E },
        { 0x7B, 0x58 },
        { 0xE3, 0x80 },
        { 0xF5, 0x2C },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockOnFence
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_BlockOnFence_1_0_5233, 6,

    XRefNoSaveIndex,
    XRefOne)

        { 0x08, XREF_D3D_BLOCKONTIME },

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0D, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPalette_1_0_5233, 12)

        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x48 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0B }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x51 : add ebp, 0x80000
        { 0x51, 0x81 }, // (Offset,Value)-Pair #6
        { 0x52, 0xC5 }, // (Offset,Value)-Pair #7
        { 0x53, 0x00 }, // (Offset,Value)-Pair #8
        { 0x54, 0x00 }, // (Offset,Value)-Pair #9
        { 0x55, 0x08 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x8B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetFlickerFilter_1_0_5233, 12)

        // IDirect3DDevice8_SetFlickerFilter+0x1C : mov eax, [eax+0x17C8]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A }, // (Offset,Value)-Pair #5
        { 0x23, 0x00 }, // (Offset,Value)-Pair #6
        { 0x24, 0x56 }, // (Offset,Value)-Pair #7
        { 0x25, 0x6A }, // (Offset,Value)-Pair #8
        { 0x26, 0x0B }, // (Offset,Value)-Pair #9
        { 0x27, 0x50 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x40, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetFlickerFilter_1_0_5344, 12)

        // IDirect3DDevice8_SetFlickerFilter+0x1C : mov eax, [eax+0x17C8]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xF8 }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A }, // (Offset,Value)-Pair #5
        { 0x23, 0x00 }, // (Offset,Value)-Pair #6
        { 0x24, 0x56 }, // (Offset,Value)-Pair #7
        { 0x25, 0x6A }, // (Offset,Value)-Pair #8
        { 0x26, 0x0B }, // (Offset,Value)-Pair #9
        { 0x27, 0x50 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x40, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetSoftDisplayFilter_1_0_5233, 12)

        // IDirect3DDevice8_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x17C8]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x28, 0x17 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x56 }, // (Offset,Value)-Pair #7
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #8
        { 0x2F, 0x0E }, // (Offset,Value)-Pair #9
        { 0x30, 0x52 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetSoftDisplayFilter_1_0_5344, 12)

        // IDirect3DDevice8_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x17F8]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #3
        { 0x28, 0x17 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetSoftDisplayFilter+0x2B : push 0; push esi; push 0x0E; push edx
        { 0x2B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x56 }, // (Offset,Value)-Pair #7
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #8
        { 0x2F, 0x0E }, // (Offset,Value)-Pair #9
        { 0x30, 0x52 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetSoftDisplayFilter+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_5344, 13)

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #3
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #5

        // ***
        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x18 : mov dword ptr [eax], 417C4h
        { 0x18, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x19, 0x00 }, // (Offset,Value)-Pair #7
        { 0x1A, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x1B, 0x17 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x04 }, // (Offset,Value)-Pair #10
        { 0x1D, 0x00 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Unknown1
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Unknown1_1_0_5233, 9)

	// IDirect3DDevice8_Unknown1+0x00 : xor eax, eax
	{ 0x00, 0x33 },
	{ 0x01, 0xC0 },
	// IDirect3DDevice8_Unknown1+0x13 : call ds:AvSendTVEncoderOption
	{ 0x13, 0xFF },
	{ 0x14, 0x15 },
	{ 0x15, 0x08 },
	{ 0x16, 0x94 },
	{ 0x17, 0x19 },
	{ 0x18, 0x00 },
	// IDirect3DDevice8_Unknown1+0x1E : retn
	{ 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush_1_0_5344, 6)

        { 0x06, 0x6A },
        { 0x07, 0x00 },
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x12, 0x89 },
        { 0x16, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPush_1_0_5344, 8)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVerticalBlankCallback_1_0_5233, 12)

        // IDirect3DDevice8_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x1958], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x58 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x19 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_SampleAlpha_1_0_5233, 7)

        { 0x0B, 0xC1 },
        { 0x15, 0x0B },
        { 0x1C, 0x8B },
        { 0x28, 0x85 },
        { 0x30, 0x07 },
        { 0x3B, 0xC7 },
        { 0x44, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask_1_0_5233, 8)

        { 0x0A, 0xA3 },
        { 0x12, 0x56 },
        { 0x1C, 0x8B },
        { 0x2A, 0x85 },
        { 0x30, 0x01 },
        { 0x3D, 0xC7 },
        { 0x44, 0x78 },
        { 0x4E, 0x04 },
OOVPA_END;

// ******************************************************************
// * Direct3D_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(Direct3D_EnumAdapterModes_1_0_5233, 8)

        { 0x20, 0x8B },
        { 0x3E, 0xE3 },
        { 0x5E, 0x75 },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x0F },
        { 0xE2, 0x89 },
        { 0xFE, 0x5D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_1_0_5233, 7)

        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f_1_0_5233, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x80 },
        { 0x21, 0x0C },
        { 0x28, 0x89 },
        { 0x2F, 0xC0 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_End
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_End_1_0_5233, 12)

        // IDirect3DDevice8_End+0x13 : mov dword ptr [eax], 0x417FC
        { 0x13, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x14, 0x00 }, // (Offset,Value)-Pair #2
        { 0x15, 0xFC }, // (Offset,Value)-Pair #3
        { 0x16, 0x17 }, // (Offset,Value)-Pair #4
        { 0x17, 0x04 }, // (Offset,Value)-Pair #5
        { 0x18, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_End+0x28 : test ah, 0x10
        { 0x28, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x29, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x2A, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_End+0x2B : jz +0x07
        { 0x2B, 0x74 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x07 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_End+0x3C : retn
        { 0x3C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport_1_0_5344, 8)

        { 0x1E, 0x87 },
        { 0x3E, 0xC0 },
        { 0x5E, 0x49 },
        { 0x7E, 0xD6 },
        { 0x9E, 0xE2 },
        { 0xBE, 0xC1 },
        { 0xDE, 0xC9 },
        { 0xFE, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform_1_0_5344, 8)

        { 0x1E, 0x8B },
        { 0x3E, 0x00 },
        { 0x5E, 0xC4 },
        { 0x7E, 0x62 },
        { 0x9E, 0x00 },
        { 0xBE, 0x42 },
        { 0xDE, 0x42 },
        { 0xFE, 0xCB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2_1_0_5344, 7)

        { 0x07, 0x8B },
        { 0x10, 0x01 },
        { 0x19, 0x15 },
        { 0x22, 0x8B },
        { 0x2B, 0xC0 },
        { 0x34, 0x15 },
        { 0x3D, 0x8B },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback_1_0_5344, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x88 },
        { 0x0D, 0x19 },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback_1_0_5344, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x84 },
        { 0x0D, 0x19 },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport_1_0_5344, 7)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0E, 0x0A },
        { 0x12, 0x06 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTargetFast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTargetFast_1_0_5344, 8)

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
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset_1_0_5344, 7)

        { 0x08, 0x44 },
        { 0x12, 0xCE },
        { 0x1F, 0xD8 },
        { 0x26, 0x0A },
        { 0x30, 0x3B },
        { 0x3A, 0x8B },
        { 0x44, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors_1_0_5344, 8)

        { 0x1D, 0x44 },
        { 0x3C, 0x8B },
        { 0x5D, 0xD9 },
        { 0x7D, 0xD8 },
        { 0x99, 0x0A },
        { 0xB8, 0xE8 },
        { 0xD7, 0x24 },
        { 0xF6, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial_1_0_5344, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xE0 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D8_1_0_5233
// ******************************************************************
OOVPATable D3D8_1_0_5233[] = {

    // IDirect3D8::CreateDevice
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDevice_1_0_5233, XTL::EmuIDirect3D8_CreateDevice),
	// IDirect3DDevice8::GetDisplayFieldStatus
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233, XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus),
	// IDirect3D8::CheckDeviceFormat (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3D8_CheckDeviceFormat_1_0_4361, XTL::EmuIDirect3D8_CheckDeviceFormat),
	// IDirect3D8::KickOffAndWaitForIdle
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle_1_0_5233, XTL::EmuIDirect3D8_KickOffAndWaitForIdle),
	// IDirect3DDevice8::CopyRects
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CopyRects_1_0_5233, XTL::EmuIDirect3DDevice8_CopyRects),
	// IDirect3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScissors_1_0_5233, XTL::EmuIDirect3DDevice8_SetScissors),
	// IDirect3DDevice8::GetGammaRamp (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetGammaRamp_1_0_4034, XTL::EmuIDirect3DDevice8_GetGammaRamp),
	// IDirect3DDevice8::SetGammaRamp (* unchanged since 4928 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetGammaRamp_1_0_4928, XTL::EmuIDirect3DDevice8_SetGammaRamp),
	// IDirect3DDevice8_GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDeviceCaps_1_0_3925, XTL::EmuIDirect3DDevice8_GetDeviceCaps),
	// IDirect3DDevice8::SetShaderConstantMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetShaderConstantMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetShaderConstantMode),
	// IDirect3DDevice8::GetRenderTarget2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetRenderTarget2_1_0_5233, XTL::EmuIDirect3DDevice8_GetRenderTarget2),
	// IDirect3DDevice8::GetDepthStencilSurface2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2),
    // D3D::CommonSetRenderTarget (XREF)
	OOVPA_TABLE_XREF(D3D_CommonSetRenderTarget_1_0_5233),
	// IDirect3DDevice8::SetRenderTarget (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderTarget_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderTarget),
	// IDirect3DDevice8::AddRef
	OOVPA_TABLE_PATCH(IDirect3DDevice8_AddRef_1_0_5233, XTL::EmuIDirect3DDevice8_AddRef),
	// IDirect3DDevice8::Reset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Reset_1_0_4134, XTL::EmuIDirect3DDevice8_Reset),
	// IDirect3DDevice8::SetTileNoWait
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTileNoWait_1_0_5233, XTL::EmuIDirect3DDevice8_SetTileNoWait),
	// IDirect3DDevice8::CreateIndexBuffer2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateIndexBuffer2_1_0_4627, XTL::EmuIDirect3DDevice8_CreateIndexBuffer2),
	// IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexShader),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_EndStateBlock),
    // D3DDevice_RunVertexStateShader (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_RunVertexStateShader_1_0_4627, XTL::EmuIDirect3DDevice8_RunVertexStateShader),
    // D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderSize_1_0_3925, XTL::EmuIDirect3DDevice8_GetVertexShaderSize),
	// IDirect3DDevice8::SetVertexShaderConstant1 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant1_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1),
	// IDirect3DDevice8::SetVertexShaderConstant4 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4),
	// IDirect3DDevice8::SetVertexShaderConstantNotInline
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233, XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline),
	// IDirect3DDevice8::DeletePixelShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DeletePixelShader_1_0_4134, XTL::EmuIDirect3DDevice8_DeletePixelShader),
	// IDirect3DDevice8::CreatePixelShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePixelShader),
	// IDirect3DDevice8::SetPixelShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_5233, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::CreateTexture2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2_1_0_5233, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::CreateTexture2C (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2C_1_0_4627, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::SetIndices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndices_1_0_5233, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetTexture
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_5233, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SetTextureB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureB_1_0_5233, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SetTextureC (* unchanged since 4928 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureC_1_0_4928, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::GetDisplayMode (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayMode_1_0_4627, XTL::EmuIDirect3DDevice8_GetDisplayMode),
	// IDirect3DDevice8::SetVertexData2f (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData2f_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::Begin
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Begin_1_0_5233, XTL::EmuIDirect3DDevice8_Begin),
	// IDirect3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData2f_1_0_5233, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::End
	OOVPA_TABLE_PATCH(IDirect3DDevice8_End_1_0_5233, XTL::EmuIDirect3DDevice8_End),
	// IDirect3DDevice8::Clear
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_5233, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::Swap
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Swap_1_0_5233, XTL::EmuIDirect3DDevice8_Swap),
	// IDirect3DDevice8::EnableOverlay (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EnableOverlay_1_0_4361, XTL::EmuIDirect3DDevice8_EnableOverlay),
	// IDirect3DDevice8::CreateVertexBuffer2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexBuffer2_1_0_4627, XTL::EmuIDirect3DDevice8_CreateVertexBuffer2),
	// IDirect3DDevice8::SetTextureState_TexCoordIndex (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv),
	// IDirect3DDevice8::SetTextureState_BorderColor (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor),
	// IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend),
	// IDirect3DDevice8::SetRenderState_CullMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_CullModeB (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullModeB_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_NormalizeNormals (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// IDirect3DDevice8::SetRenderState_TextureFactor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// IDirect3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZBias_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZBias),
	// IDirect3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// IDirect3DDevice8::SetRenderState_FillMode (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FillMode_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// IDirect3DDevice8::SetRenderState_BackFillMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode),
	// IDirect3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FogColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
    // *******
    // Duplicated functions generally dangerous, but until such time as XDK 5344
    // has it's own OOVP's, fine here
    // *******
    // IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_5344, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
	// IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// IDirect3DDevice8::SetRenderState_ZEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8::SetRenderState_LineWidth (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_LineWidth_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth),
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable),
	// IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable),
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead),
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead),
	// IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed),
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode),
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
    // *******
    // Duplicated functions generally dangerous, but until such time as XDK 5344
    // has it's own OOVP's, fine here
    // *******
    // IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_5344, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
	// IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc),
	// IDirect3DDevice8::SwitchTexture (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SwitchTexture_1_0_4361, XTL::EmuIDirect3DDevice8_SwitchTexture),
	// IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable),
	// IDirect3DDevice8::SetTransform (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTransform_1_0_4134, XTL::EmuIDirect3DDevice8_SetTransform),
	// IDirect3DDevice8::SetStreamSource (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_4361, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// IDirect3D8::GetDeviceCaps
	OOVPA_TABLE_PATCH(IDirect3D8_GetDeviceCaps_1_0_5233, XTL::EmuIDirect3D8_GetDeviceCaps),
	// IDirect3DDevice8::SetLight
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_5233, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DDevice8::SetLight
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_5344, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DDevice8::LightEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_5233, XTL::EmuIDirect3DDevice8_LightEnable),
	// IDirect3DDevice8::LightEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_5344, XTL::EmuIDirect3DDevice8_LightEnable),
	// IDirect3DDevice8::DrawIndexedVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_5233, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// IDirect3DDevice8::SetMaterial (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetMaterial_1_0_4627, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DDevice8::SetMaterial
	OOVPA_TABLE_PATCH(D3DDevice_SetMaterial_1_0_5344, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DVertexBuffer8::GetDesc
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_GetDesc_1_0_5233, XTL::EmuIDirect3DVertexBuffer8_GetDesc),
	// IDirect3DVertexBuffer8::Lock2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock2_1_0_4627, XTL::EmuIDirect3DVertexBuffer8_Lock2),
	// IDirect3DResource8::Register (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Register_1_0_3925, XTL::EmuIDirect3DResource8_Register),
	// IDirect3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2_1_0_5233, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::UpdateOverlay
	OOVPA_TABLE_PATCH(IDirect3DDevice8_UpdateOverlay_1_0_5233, XTL::EmuIDirect3DDevice8_UpdateOverlay),
	// IDirect3DDevice8::GetOverlayUpdateStatus
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5233, XTL::EmuIDirect3DDevice8_GetOverlayUpdateStatus),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5233, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::GetViewport (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetViewport_1_0_4627, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_5233, XTL::EmuIDirect3DDevice8_SetViewport),
	// IDirect3DResource8::AddRef (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_AddRef_1_0_3925, XTL::EmuIDirect3DResource8_AddRef),
	// IDirect3DResource8::Release (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_3925, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DResource8::IsBusy (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_IsBusy_1_0_4361, XTL::EmuIDirect3DResource8_IsBusy),
	// IDirect3DPalette8::Lock2
	OOVPA_TABLE_PATCH(IDirect3DPalette8_Lock2_1_0_5233, XTL::EmuIDirect3DPalette8_Lock2),
	// IDirect3DTexture8::LockRect
	OOVPA_TABLE_PATCH(IDirect3DTexture8_LockRect_1_0_5233, XTL::EmuIDirect3DTexture8_LockRect),
	// Lock2DSurface (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, XTL::EmuLock2DSurface),
	// Get2DSurfaceDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_5233, XTL::EmuGet2DSurfaceDesc),
	// Get2DSurfaceDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_5344, XTL::EmuGet2DSurfaceDesc),
	// IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_GetDesc_1_0_4361, XTL::EmuIDirect3DSurface8_GetDesc),
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRect_1_0_3925, XTL::EmuIDirect3DSurface8_LockRect),
	// IDirect3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DBaseTexture8_GetLevelCount_1_0_4361, XTL::EmuIDirect3DBaseTexture8_GetLevelCount),
	// IDirect3DTexture8::GetSurfaceLevel2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel2_1_0_4627, XTL::EmuIDirect3DTexture8_GetSurfaceLevel2),
	// IDirect3DDevice8::SetVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_5233, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::DrawVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVertices_1_0_5233, XTL::EmuIDirect3DDevice8_DrawVertices),
    // IDirect3DDevice8::GetViewportOffsetAndScale (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetViewportOffsetAndScale_1_0_4627, XTL::EmuIDirect3DDevice8_GetViewportOffsetAndScale),
    // IDirect3DDevice8::DeleteVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_DeleteVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_DeleteVertexShader),
    // IDirect3DDevice8::SelectVertexShaderDirect (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_SelectVertexShaderDirect_1_0_4627, XTL::EmuIDirect3DDevice8_SelectVertexShaderDirect),
	// IDirect3DDevice8::LoadVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LoadVertexShader_1_0_5233, XTL::EmuIDirect3DDevice8_LoadVertexShader),
	// IDirect3DDevice::Release
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Release_1_0_5233, XTL::EmuIDirect3DDevice8_Release),
	// IDirect3DDevice8::SetScreenSpaceOffset
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScreenSpaceOffset_1_0_5233, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
    // D3D::SetFence (XREF)
	OOVPA_TABLE_XREF(D3D_SetFence_1_0_5233),
	// IDirect3DDevice8::InsertFence
	OOVPA_TABLE_PATCH(IDirect3DDevice8_InsertFence_1_0_5233, XTL::EmuIDirect3DDevice8_InsertFence),
	// IDirect3DDevice8::IsFencePending
	OOVPA_TABLE_PATCH(IDirect3DDevice8_IsFencePending_1_0_5233, XTL::EmuIDirect3DDevice8_IsFencePending),
    // D3D::BlockOnTime (XREF)
	OOVPA_TABLE_XREF(D3D_BlockOnTime_1_0_5233),
	// IDirect3DDevice8::BlockOnFence
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockOnFence_1_0_5233, XTL::EmuIDirect3DDevice8_BlockOnFence),
	// IDirect3DDevice8::GetTransform (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTransform_1_0_4361, XTL::EmuIDirect3DDevice8_GetTransform),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPalette_1_0_5233, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetCreationParameters_1_0_4034, XTL::EmuIDirect3DDevice8_GetCreationParameters),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetFlickerFilter_1_0_5233, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetFlickerFilter_1_0_5344, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSoftDisplayFilter_1_0_5233, XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter),
	// IDirect3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSoftDisplayFilter_1_0_5344, XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter),
	// IDirect3DDevice8_Unknown1
	/*
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Unknown1_1_0_5233, XTL::EmuIDirect3DDevice8_Unknown1,
	*/
	// IDirect3DDevice8::SetRenderState_TwoSidedLighting (* unchanged since 4134 *)
	// Beware of the typo...
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
    // ******
    // Duplicate references are normally bad, but this XDK is aliased to XDK 5344
    // until such time as a separate XDK file is generated for 5344, this will have to do
    // ******
	// IDirect3DDevice8::SetRenderState_TwoSidedLighting (* targeting 5344 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_5344, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
	// IDirect3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace),
	// D3DDevice_SetRenderState_LogicOp (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LogicOp_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp),
	// IDirect3DDevice8::BeginPush
	OOVPA_TABLE_PATCH(D3DDevice_BeginPush_1_0_5344, XTL::EmuIDirect3DDevice8_BeginPush),
	// IDirect3DDevice8::BeginVisibilityTest (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginVisibilityTest_1_0_4627, XTL::EmuIDirect3DDevice8_BeginVisibilityTest),
	// IDirect3DDevice8::EndPush
	OOVPA_TABLE_PATCH(D3DDevice_EndPush_1_0_5344, XTL::EmuIDirect3DDevice8_EndPush),
	// IDirect3DDevice8::EndPush (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndPush_1_0_4627, XTL::EmuIDirect3DDevice8_EndPush),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVerticalBlankCallback_1_0_5233, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// D3DDevice_SetRenderState_SampleAlpha
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_SampleAlpha_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_SampleAlpha),
	// D3DDevice_SetRenderState_MultiSampleMask
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMask_1_0_5233, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask),
	// IDirect3DDevice8::SetVertexData4ub (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4ub_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4ub),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4f_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(D3DDevice_SetViewport_1_0_5344, XTL::EmuIDirect3DDevice8_SetViewport),
	/**
	 * Having two versions of D3Device::SetTransform in this file creates problems,
	 * in order to support Futurama, this second verison (added for Whacked support)
	 * has been disabled.
	// IDirect3DDevice8::SetTransform
	OOVPA_TABLE_PATCH(D3DDevice_SetTransform_1_0_5344, XTL::EmuIDirect3DDevice8_SetTransform),**/
	// IDirect3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(D3DDevice_GetBackBuffer2_1_0_5344, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetVerticalBlankCallback_1_0_5344, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetSwapCallback_1_0_5344, XTL::EmuIDirect3DDevice8_SetSwapCallback),
	// IDirect3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(D3DDevice_GetViewport_1_0_5344, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8::SetRenderTargetFast
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderTargetFast_1_0_5344, XTL::EmuIDirect3DDevice8_SetRenderTargetFast),
	// IDirect3DDevice8::SetScreenSpaceOffset
	OOVPA_TABLE_PATCH(D3DDevice_SetScreenSpaceOffset_1_0_5344, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
	// IDirect3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(D3DDevice_SetScissors_1_0_5344, XTL::EmuIDirect3DDevice8_SetScissors),
	// IDirect3DCubeTexture8::GetCubeMapSurface2 (* Unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface2_1_0_4627, XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface2),
	// IDirect3DDevice8::ApplyStateBlock (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_ApplyStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_ApplyStateBlock),
};

// ******************************************************************
// * D3D8_1_0_5233_SIZE
// ******************************************************************
uint32 D3D8_1_0_5233_SIZE = sizeof(D3D8_1_0_5233);
