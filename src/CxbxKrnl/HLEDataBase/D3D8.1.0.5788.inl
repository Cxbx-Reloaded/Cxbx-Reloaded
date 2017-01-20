// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5788.cpp
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
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest_1_0_5788, 7)

        { 0x10, 0x09 },
        { 0x12, 0x0E },
        { 0x18, 0x04 },
        { 0x34, 0xCC },
        { 0x3D, 0x0E },
        { 0x50, 0x0C },
        { 0x58, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVisibilityTestResult
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVisibilityTestResult_1_0_5788, 7)

        { 0x0E, 0x08 },
        { 0x10, 0xFF },
        { 0x2A, 0x16 },
        { 0x3C, 0x76 },
        { 0x3F, 0x0C },
        { 0x51, 0x0A },
        { 0x5F, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport_1_0_5788, 8)

        { 0x09, 0x05 },
        { 0x14, 0x04 },
        { 0x1D, 0x08 },
        { 0x20, 0x0C },
        { 0x26, 0x10 },
        { 0x2B, 0x40 },
        { 0x2F, 0x14 },
        { 0x31, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale_1_0_5788, 7)

        { 0x1E, 0x24 },
        { 0x3E, 0x06 },
        { 0x62, 0xD9 },
        { 0x7E, 0xC4 },
        { 0x9E, 0x81 },
        { 0xBE, 0xFF },
        { 0xDE, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2_1_0_5788, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer2+0x18 : mov esi, [ecx+eax*4+0x21C0]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0x14 }, // (Offset,Value)-Pair #9
        { 0x19, 0x1A }, // (Offset,Value)-Pair #10

        // D3DDevice_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2_1_0_5788, 10)

        // D3DDevice_GetDepthStencilSurface2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetDepthStencilSurface2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetDepthStencilSurface2+0x06 : mov esi, [eax + 0x1A08]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0x08 }, // (Offset,Value)-Pair #5
        { 0x09, 0x1A }, // (Offset,Value)-Pair #6

        // D3DDevice_GetDepthStencilSurface2+0x0E : jnz +0x04
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetDepthStencilSurface2+0x15 : call [addr]
        { 0x15, 0xE8 }, // (Offset,Value)-Pair #9

        // D3DDevice_GetDepthStencilSurface2+0x1D : retn
        { 0x1D, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexShader_1_0_5788, 8)

        { 0x1E, 0x85 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x24 },
        { 0xDE, 0x83 },
        { 0xFE, 0xC7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePixelShader_1_0_5788, 8)

        { 0x07, 0x00 },
        { 0x10, 0xC0 },
        { 0x19, 0x08 },
        { 0x22, 0x78 },
        { 0x2B, 0x00 },
        { 0x34, 0xB9 },
        { 0x3D, 0x24 },
        { 0x46, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f_1_0_5788, 7)

        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor_1_0_5788, 7)

        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x19 },
        { 0x26, 0xB6 },
        { 0x30, 0x00 },
        { 0x3A, 0xFF },
        { 0x44, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor_1_0_5788, 7)

        { 0x09, 0x84 },
        { 0x14, 0x46 },
        { 0x1F, 0x24 },
        { 0x2A, 0xBA },
        { 0x35, 0x4A },
        { 0x40, 0x5E },
        { 0x4D, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices_1_0_5788, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices_1_0_5788, 8)

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
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP_1_0_5788, 8)

        { 0x1F, 0x05 },
        { 0x3A, 0x89 },
        { 0x64, 0xF0 },
        { 0x73, 0x13 },
        { 0x94, 0xAF },
        { 0xAB, 0x12 },
        { 0xE4, 0xDB },
        { 0xFF, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial_1_0_5788, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x00 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBig
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBig_1_0_5788, 7)

        { 0x07, 0x01 },
        { 0x10, 0x8D },
        { 0x19, 0x3B },
        { 0x22, 0x8B },
        { 0x2B, 0x00 },
        { 0x34, 0xCA },
        { 0x40, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer2_1_0_5788, 7)

        { 0x08, 0xE8 },
        { 0x12, 0x1E },
        { 0x1C, 0x50 },
        { 0x26, 0x68 },
        { 0x31, 0x33 },
        { 0x3A, 0xFF },
        { 0x44, 0x01 },
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer8_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock_1_0_5788, 8)

        { 0x01, 0x51 },
        { 0x07, 0x8B },
        { 0x08, 0x4C },
        { 0x0A, 0x04 },
        { 0x0D, 0x24 },
        { 0x10, 0xC1 },
        { 0x13, 0x33 },
        { 0x16, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock2_1_0_5788, 7)

        { 0x08, 0x56 },
        { 0x12, 0x06 },
        { 0x1D, 0xC7 },
        { 0x26, 0x00 },
        { 0x30, 0xC3 },
        { 0x3E, 0x8B },
        { 0x44, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable_1_0_5788, 8)

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
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices_1_0_5788, 7)

        { 0x12, 0x8B },
        { 0x26, 0x8B },
        { 0x37, 0x00 },
        { 0x4A, 0x27 },
        { 0x5D, 0x89 },
        { 0x70, 0x75 },
        { 0x83, 0x40 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget_1_0_5788, 8)

        { 0x1E, 0x44 },
        { 0x3E, 0x2C },
        { 0x5E, 0x8B },
        { 0x7E, 0xDE },
        { 0xA0, 0x89 },
        { 0xBE, 0x09 },
        { 0xDE, 0x8B },
        { 0xFE, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3D_AllocContiguousMemory
// ******************************************************************
OOVPA_NO_XREF(D3D_AllocContiguousMemory_1_0_5788, 7)

        { 0x05, 0x10 },
        { 0x0C, 0x00 },
        { 0x13, 0x00 },
        { 0x1A, 0x08 },
        { 0x21, 0xBC },
        { 0x28, 0x0D },
        { 0x2F, 0x24 },
OOVPA_END;

// ******************************************************************
// * D3DTexture8_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_5788, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x1A14]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0x14 }, // (Offset,Value)-Pair #7
        { 0x55, 0x1A }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateTexture2_1_0_5788, 8,

    XRefNoSaveIndex, // ?? XREF_D3DDevice_CreateTexture2,
    XRefZero)

        { 0x14, 0x4C },
        { 0x2A, 0x8B },
        { 0x42, 0x8B },
        { 0x56, 0x80 },
        { 0x70, 0x85 },
        { 0x82, 0x5E },
        { 0x98, 0x89 },
        { 0xAE, 0x1C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1_1_0_5788, 11)

        // D3DDevice_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant1+0x0E : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstant1+0x28 : mov [eax-0x18], ecx
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #6
        { 0x1E, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant1+0x3D : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstant1+0x47 : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f_1_0_5788, 8)

        { 0x0D, 0x05 },
        { 0x17, 0x8D },
        { 0x19, 0xCD },
        { 0x1F, 0x4C },
        { 0x25, 0x54 },
        { 0x27, 0x10 },
        { 0x30, 0x0C },
        { 0x35, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP_1_0_5788, 8)

        { 0x1E, 0x72 },
        { 0x3E, 0xC0 },
        { 0x5E, 0xC6 },
        { 0x7E, 0x14 },
        { 0x9E, 0x00 },
        { 0xBE, 0x81 },
        { 0xDE, 0x8B },
        { 0xFE, 0x7D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer_1_0_5788, 6)

        { 0x09, 0x00 },
        { 0x11, 0x8B },
        { 0x2F, 0x08 },
        { 0x43, 0x57 },
        { 0x58, 0xC8 },
        { 0x63, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer_1_0_5788, 6)

        { 0x09, 0x00 },
        { 0x15, 0x50 },
        { 0x47, 0x7B },
        { 0x61, 0x78 },
        { 0x77, 0x07 },
        { 0x8C, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer_1_0_5788, 6)

        { 0x0E, 0xED },
        { 0x42, 0x18 },
        { 0x62, 0x04 },
        { 0x8D, 0x1C },
        { 0xAF, 0x0F },
        { 0xF6, 0x41 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_XREF(D3DDevice_SetVerticalBlankCallback_1_0_5788, 7,

    XRefNoSaveIndex,
    XRefOne)

        { 0x06, XREF_D3DDEVICE },

        { 0x01, 0x44 },
        { 0x03, 0x04 },
        { 0x05, 0x0D },
        { 0x0C, 0xB8 },
        { 0x0D, 0x1D },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport_1_0_5788, 8)

        { 0x1E, 0x86 },
        { 0x3E, 0x1B },
        { 0x5E, 0x8B },
        { 0x7E, 0x6D },
        { 0x9E, 0x81 },
        { 0xBE, 0x0C },
        { 0xDE, 0x75 },
        { 0xFE, 0x85 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank_1_0_5788, 7)

        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xC0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial_1_0_5788, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x44 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting_1_0_5788, 8)

        { 0x0E, 0x05 },
        { 0x1E, 0x89 },
        { 0x30, 0x00 },
        { 0x40, 0x89 },
        { 0x52, 0x8B },
        { 0x60, 0x75 },
        { 0x6E, 0x48 },
        { 0x7E, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release_1_0_5788, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x28 },
        { 0x21, 0x05 },
        { 0x28, 0x00 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3D::KickOffAndWaitForIdle
// ******************************************************************
/*OOVPA_XREF(D3D_KickOffAndWaitForIdle_1_0_5788, 5,

    XREF_D3D_KickOffAndWaitForIdle,
    XRefZero)

        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x2C },
        { 0x0A, 0x51 },
        { 0x10, 0xC3 },
OOVPA_END;
*/

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTileNoWait_1_0_5788, 10)

	// D3DDevice_SetTileNoWait+0x00: sub esp, 0x1C
	{ 0x00, 0x83 },
	{ 0x01, 0xEC },
	{ 0x02, 0x1C },
	// D3DDevice_SetTileNoWait+0x45: lea ebx, [ebx+ebp*8+0x1AC0]
	{ 0x45, 0x8D },
	{ 0x46, 0x9C },
	{ 0x47, 0xEB },
	{ 0x48, 0xC0 },
	{ 0x49, 0x1A },
	{ 0x4A, 0x00 },
	{ 0x4B, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback_1_0_5788, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xB4 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail_1_0_5788, 8)

        { 0x0C, 0x89 },
        { 0x1E, 0x8B },
        { 0x28, 0x74 },
        { 0x36, 0xD2 },
        { 0x44, 0x3D },
        { 0x52, 0xC7 },
        { 0x60, 0x04 },
        { 0x6E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend_1_0_5788, 7)

        { 0x07, 0xCA },
        { 0x13, 0x89 },
        { 0x19, 0x8B },
        { 0x25, 0x8B },
        { 0x2B, 0x28 },
        { 0x34, 0x08 },
        { 0x3D, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice8::SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth_1_0_5788, 13)

        // D3DDevice_SetRenderState_LineWidth+0x00 : push ebx
        { 0x00, 0x53 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_LineWidth+0x0C : mov [esp+8+argo_0], ebx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x5C }, // (Offset,Value)-Pair #3
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_LineWidth+0x28 : fstp [esp+10h+var_10]
        { 0x28, 0xD9 }, // (Offset,Value)-Pair #6
        { 0x29, 0x1C }, // (Offset,Value)-Pair #7
        { 0x2A, 0x24 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_LineWidth+0x30 : mov edi, eax
        { 0x30, 0x8B }, // (Offset,Value)-Pair #9
        { 0x31, 0xF8 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_LineWidth+0x41 : cmp eax, [esi+4]
        { 0x41, 0x3B }, // (Offset,Value)-Pair #11
        { 0x42, 0x46 }, // (Offset,Value)-Pair #12
        { 0x43, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;


// ******************************************************************
// * D3DDevice8::SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes_1_0_5788, 13)

        // _D3DDevice_SetDepthClipPlanes+0x00 : mov eax, [esp+Flags]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x0C }, // (Offset,Value)-Pair #4

        // _D3DDevice_SetDepthClipPlanes+0x00 : ja short loc_27ABD0 ; jumptable 0027AB71 default case
        { 0x0F, 0x77 }, // (Offset,Value)-Pair #5
        { 0x10, 0x5F }, // (Offset,Value)-Pair #6

        // _D3DDevice_SetDepthClipPlanes+0x00 : jmp ds:off_27ABEC[eax*4] ; switch jump
        { 0x11, 0xFF }, // (Offset,Value)-Pair #7
        { 0x12, 0x24 }, // (Offset,Value)-Pair #8
        { 0x13, 0x85 }, // (Offset,Value)-Pair #9
        { 0x14, 0xEC }, // (Offset,Value)-Pair #10
        { 0x15, 0xAB }, // (Offset,Value)-Pair #11
        { 0x16, 0x27 }, // (Offset,Value)-Pair #12
        { 0x17, 0x00 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette_1_0_5788, 7)

        { 0x10, 0x98 },
        { 0x22, 0x0F },
        { 0x34, 0x00 },
        { 0x46, 0x9C },
        { 0x58, 0xC5 },
        { 0x6E, 0x8B },
        { 0x7C, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram_1_0_5788, 7)

        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0xE0 },
        { 0x32, 0x5F },
        { 0x3F, 0x24 },
        { 0x4C, 0x83 },
        { 0x5C, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPushBufferOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPushBufferOffset_1_0_5788, 7)

        { 0x13, 0xCA },
        { 0x28, 0xFF },
        { 0x3D, 0x89 },
        { 0x52, 0x12 },
        { 0x67, 0xAB },
        { 0x7C, 0x51 },
        { 0x91, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace_1_0_5788, 9,

    XREF_D3D_MakeRequestedSpace,
    XRefZero)

        { 0x32, 0xC2 },
        { 0x33, 0x08 },
        { 0x8E, 0x83 },
        { 0x8F, 0xC0 },
        { 0x90, 0x04 },
        { 0xBD, 0xD1 },
        { 0xBE, 0xEB },
	{ 0xBF, 0x03 },
	{ 0xC0, 0xDF },
OOVPA_END;

// ******************************************************************
// * D3D::Unknown (D3D::MakeSpace?)
// ******************************************************************
// * FOR DEBUGGING ONLY!
// ******************************************************************
OOVPA_XREF(D3D_Unknown_1_0_5788, 6,

    XRefNoSaveIndex,
    XRefOne)

	{ 0x0A, XREF_D3D_MakeRequestedSpace },

        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
	{ 0x0E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache_1_0_5788, 10)

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
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef_1_0_5788, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x38 },
        { 0x0A, 0x00 },
        { 0x0D, 0x88 },
        { 0x10, 0x00 },
        { 0x13, 0xC1 },
OOVPA_END;

// ******************************************************************
// * D3D::ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags_1_0_5788, 9,

    XREF_CLEARSTATEBLOCKFLAGS,
    XRefZero)

        // D3D_ClearStateBlockFlags+0x0A : mov ecx, 0x87
        { 0x0A, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x87 }, // (Offset,Value)-Pair #2

        // D3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x07A4]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #3
        { 0x17, 0x8A }, // (Offset,Value)-Pair #4
        { 0x18, 0xA4 }, // (Offset,Value)-Pair #5
        { 0x19, 0x07 }, // (Offset,Value)-Pair #6

        // D3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x31, 0x90 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3D::RecordStateBlock
// ******************************************************************
OOVPA_XREF(D3D_RecordStateBlock_1_0_5788, 10,

    XREF_RECORDSTATEBLOCK,
    XRefZero)

        // D3D_RecordStateBlock+0x0F : mov eax, [edi+0x07A4]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x87 }, // (Offset,Value)-Pair #2
        { 0x11, 0xA4 }, // (Offset,Value)-Pair #3
        { 0x12, 0x07 }, // (Offset,Value)-Pair #4

        // D3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 }, // (Offset,Value)-Pair #5
        { 0x96, 0xD9 }, // (Offset,Value)-Pair #6

        // D3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xD6, 0x46 }, // (Offset,Value)-Pair #8
        { 0xD7, 0x0C }, // (Offset,Value)-Pair #9
        { 0xD8, 0x01 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode_1_0_5788, 7)

        { 0x0B, 0xA3 },
        { 0x14, 0x04 },
        { 0x23, 0x8B },
        { 0x2A, 0xC9 },
        { 0x35, 0xC7 },
        { 0x40, 0x08 },
        { 0x4C, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5788, 8)

        { 0x0D, 0x8B },
        { 0x1D, 0x80 },
        { 0x2B, 0x08 },
        { 0x3A, 0x01 },
        { 0x49, 0xC7 },
        { 0x58, 0x05 },
        { 0x67, 0xC0 },
        { 0x76, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile_1_0_5788, 7)

        { 0x06, 0x8B },
        { 0x0E, 0x94 },
        { 0x16, 0x8B },
        { 0x1E, 0x04 },
        { 0x26, 0x48 },
        { 0x2E, 0x8B },
        { 0x36, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap_1_0_5788, 7)

        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0x3F },
        { 0x8F, 0x8E },
        { 0xAC, 0xC6 },
        { 0xC9, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap (2)
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap2_1_0_5788, 11)

        // D3DDevice_Swap+0x2A : mov ebx, 5
        { 0x2A, 0xBB }, // (Offset,Value)-Pair #1
        { 0x2B, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_Swap+0x37 : test bl, 3
        { 0x37, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x38, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x39, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_Swap+0x60 : inc dword ptr [esi+0x2478]
        { 0x60, 0xFF }, // (Offset,Value)-Pair #6
        { 0x61, 0x86 }, // (Offset,Value)-Pair #7
        { 0x62, 0x78 }, // (Offset,Value)-Pair #8
        { 0x63, 0x24 }, // (Offset,Value)-Pair #9

        // D3DDevice_Swap+0xE0 : retn 4
        { 0xE0, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xE1, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3D_CDevice_KickOff_1_0_5788, 13)

	// D3D::CDevice::KickOff+0x0A : mov edx, [ecx+0x35C]
	{ 0x0A, 0x8B },
	{ 0x0B, 0x96 },
	{ 0x0C, 0x70 },
	{ 0x0D, 0x07 },
	{ 0x0E, 0x00 },
	{ 0x0F, 0x00 },

	// D3D::CDevice::KickOff+0x14 : test ch, 0x20
	{ 0x14, 0xF6 },
	{ 0x15, 0xC4 },
	{ 0x16, 0x20 },

	// D3D::CDevice::KickOff+0x24 : sfence
	{ 0x24, 0x0F },
	{ 0x25, 0xAE },
	{ 0x26, 0xF8 },

	// D3D::CDevice::KickOff+0xAF : retn
	{ 0xAF, 0xC3 }
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter_1_0_5788, 7)

        { 0x0D, 0x56 },
        { 0x14, 0x8B },
        { 0x1F, 0x95 },
        { 0x2A, 0x00 },
        { 0x37, 0x33 },
        { 0x44, 0x01 },
        { 0x4D, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter_1_0_5788, 7)

        { 0x07, 0xC9 },
        { 0x10, 0x24 },
        { 0x1A, 0x74 },
        { 0x22, 0x6A },
        { 0x2E, 0xC7 },
        { 0x34, 0x01 },
        { 0x3E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode_1_0_5788, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5788, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x74 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_5788, 7)

        { 0x0D, 0xCE },
        { 0x1E, 0x8B },
        { 0x2B, 0xF9 },
        { 0x3A, 0xCF },
        { 0x49, 0x00 },
        { 0x58, 0xC1 },
        { 0x6B, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask_1_0_5788, 8)

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
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant_1_0_5788, 7)

        { 0x1E, 0x85 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC0 },
        { 0x7E, 0x0F },
        { 0x9F, 0x8B },
        { 0xBE, 0x00 },
        { 0xDE, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus_1_0_5788, 7)

        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetScissors_1_0_5788, 7)

        { 0x0B, 0x74 },
        { 0x14, 0x11 },
        { 0x1F, 0x04 },
        { 0x2A, 0x85 },
        { 0x35, 0xE1 },
        { 0x40, 0xC1 },
        { 0x4B, 0xA4 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors_1_0_5788, 8)

        { 0x1D, 0x44 },
        { 0x3C, 0x8B },
        { 0x5D, 0xD9 },
        { 0x7D, 0xD8 },
        { 0x99, 0x0E },
        { 0xB8, 0xE8 },
        { 0xD7, 0x24 },
        { 0xF6, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial_1_0_5788, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x44 },
        { 0x0E, 0x0F },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock_1_0_5788, 8)

        { 0x1E, 0x3C },
        { 0x3E, 0xC0 },
        { 0x5E, 0x7E },
        { 0x7E, 0x00 },
        { 0x9E, 0x74 },
        { 0xC0, 0x8B },
        { 0xDE, 0x83 },
        { 0xFE, 0x45 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock_1_0_5788, 9)

        // D3DDevice_DeleteStateBlock(x)+26: mov eax, [esi+4]
        { 0x13, 0x8B },
        { 0x14, 0x46 },
        { 0x15, 0x04 },

        // D3DDevice_DeleteStateBlock(x)+1B:
        { 0x23, 0x43 }, // inc ebx
        { 0x24, 0x3B }, // cmp ebx, [edi]
        { 0x25, 0x1F },

        // D3DDevice_DeleteStateBlock(x)+34: cmp [edi+8], ebp
        { 0x3C, 0x39 },
        { 0x3D, 0x6F },
        { 0x3E, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZCmpAlwaysRead_1_0_5788, 7,

    XRefNoSaveIndex,
    XRefOne)

	{ 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD },

        { 0x01, 0x44 },
        { 0x04, 0xA3 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTexture2_1_0_5788, 8)

        { 0x03, 0x04 },
        { 0x0A, 0x56 },
        { 0x0D, 0x81 },
        { 0x0E, 0x88 },
        { 0x12, 0x8B },
        { 0x17, 0x06 },
        { 0x1E, 0x8B },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderSize
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderSize_1_0_5788, 13)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },

        { 0x08, 0x8B },
        { 0x09, 0x4C },
        { 0x0A, 0x24 },
        { 0x0B, 0x04 },

        { 0x0C, 0x8B },
        { 0x0D, 0x51 },
        { 0x0E, 0x07 },

        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3D8_1_0_5788
// ******************************************************************
OOVPATable D3D8_1_0_5788[] = {

    // D3D8::CreateDevice (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3D_CreateDevice_1_0_4627, D3D_CreateDevice),
	// D3DDevice8::BeginVisibilityTest (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_BeginVisibilityTest_1_0_4627, D3DDevice_BeginVisibilityTest),
	// D3DDevice8::EndVisibilityTest
	OOVPA_TABLE_PATCH(D3DDevice_EndVisibilityTest_1_0_5788, D3DDevice_EndVisibilityTest),
	// D3DDevice8::GetVisibilityTestResult
	OOVPA_TABLE_PATCH(D3DDevice_GetVisibilityTestResult_1_0_5788, D3DDevice_GetVisibilityTestResult),
	// D3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(D3DDevice_GetViewport_1_0_5788, D3DDevice_GetViewport),
	// D3DDevice8::SetBackBufferScale
	OOVPA_TABLE_PATCH(D3DDevice_SetBackBufferScale_1_0_5788, D3DDevice_SetBackBufferScale),
	// D3DDevice8::GetCreationParameters (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetCreationParameters_1_0_4034, D3DDevice_GetCreationParameters),
	// D3DDevice8::Clear (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_Clear_1_0_5233, D3DDevice_Clear),
	// D3DDevice8::Swap
	OOVPA_TABLE_PATCH(D3DDevice_Swap_1_0_5788, D3DDevice_Swap),
	// D3DDevice8::Swap (2)
	OOVPA_TABLE_PATCH(D3DDevice_Swap2_1_0_5788, D3DDevice_Swap),
	// D3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(D3DDevice_GetBackBuffer2_1_0_5788, D3DDevice_GetBackBuffer2),
	// D3DDevice8::GetDepthStencilSurface2
	OOVPA_TABLE_PATCH(D3DDevice_GetDepthStencilSurface2_1_0_5788, D3DDevice_GetDepthStencilSurface2),
	// D3DDevice8::CreateVertexShader
	OOVPA_TABLE_PATCH(D3DDevice_CreateVertexShader_1_0_5788, D3DDevice_CreateVertexShader),
	// D3DDevice8::CreatePixelShader
	OOVPA_TABLE_PATCH(D3DDevice_CreatePixelShader_1_0_5788, D3DDevice_CreatePixelShader),
	// D3DDevice8::SetPixelShader (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShader_1_0_5233, D3DDevice_SetPixelShader),
	// D3DDevice8::GetDisplayMode (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetDisplayMode_1_0_4627, D3DDevice_GetDisplayMode),
	// D3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData2f_1_0_5788, D3DDevice_SetVertexData2f),
	// D3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4f_1_0_5788, D3DDevice_SetVertexData4f),
	// D3DDevice8::SetVertexDataColor
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexDataColor_1_0_5788, D3DDevice_SetVertexDataColor),
	// D3DDevice8::End (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_End_1_0_5558, D3DDevice_End),
	// D3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetVerticalBlankCallback_1_0_5788, D3DDevice_SetVerticalBlankCallback),
	// D3DDevice8::SetTextureState_TexCoordIndex (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_TexCoordIndex_1_0_4627, D3DDevice_SetTextureState_TexCoordIndex),
	// D3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_BumpEnv_1_0_4361, D3DDevice_SetTextureState_BumpEnv),
	// D3DDevice8::SetTextureState_BorderColor (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_BorderColor_1_0_4361, D3DDevice_SetTextureState_BorderColor),
	// D3DDevice8::SetRenderState_CullMode (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_CullMode_1_0_5233, D3DDevice_SetRenderState_CullMode),
	// D3DDevice8::SetRenderState_TextureFactor
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TextureFactor_1_0_5788, D3DDevice_SetRenderState_TextureFactor),
	// D3DDevice8::SetRenderState_NormalizeNormals (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_NormalizeNormals_1_0_4432, D3DDevice_SetRenderState_NormalizeNormals),
	// D3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ZBias_1_0_3925, D3DDevice_SetRenderState_ZBias),
	// D3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_EdgeAntiAlias_1_0_4361, D3DDevice_SetRenderState_EdgeAntiAlias),
	// D3DDevice8::SetRenderState_FillMode (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FillMode_1_0_4361, D3DDevice_SetRenderState_FillMode),
	// D3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FogColor_1_0_4134, D3DDevice_SetRenderState_FogColor),
	// D3DDevice8::SetRenderState_Simple (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_Simple_1_0_4361, D3DDevice_SetRenderState_Simple),
	// D3DDevice8::SetRenderState_ZEnable (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ZEnable_1_0_5233, D3DDevice_SetRenderState_ZEnable),
	// D3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilEnable_1_0_4361, D3DDevice_SetRenderState_StencilEnable),
	// D3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilFail_1_0_4134, D3DDevice_SetRenderState_StencilFail),
	// D3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_OcclusionCullEnable_1_0_4134, D3DDevice_SetRenderState_OcclusionCullEnable),
	// D3DDevice8::SetRenderState_MultiSampleAntiAlias (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_4627, D3DDevice_SetRenderState_MultiSampleAntiAlias),
	// D3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ShadowFunc_1_0_4134, D3DDevice_SetRenderState_ShadowFunc),
	// D3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_YuvEnable_1_0_4134, D3DDevice_SetRenderState_YuvEnable),
	// D3DDevice8::SetVertexShader (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShader_1_0_5558, D3DDevice_SetVertexShader),
	// D3DDevice8::SetIndices
	OOVPA_TABLE_PATCH(D3DDevice_SetIndices_1_0_5788, D3DDevice_SetIndices),
	// D3DDevice8::SetTexture (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTexture_1_0_5233, D3DDevice_SetTexture),
	// D3DDevice8::SwitchTexture (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SwitchTexture_1_0_4361, D3DDevice_SwitchTexture),
    // D3D::CDevice::LazySetStateVB (XREF) (* unchanged since 5558 *)
	OOVPA_TABLE_XREF(D3D_CDevice_LazySetStateVB_1_0_5558),
	// D3DDevice8::BeginPush (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_BeginPush_1_0_5558, D3DDevice_BeginPush),
	// D3DDevice8::EndPush (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_EndPush_1_0_4627, D3DDevice_EndPush),
	// /* Not implemented yet.
    // D3DDevice8::BeginPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_BeginPushBuffer_1_0_5788, D3DDevice_BeginPushBuffer),
	// D3DDevice8::EndPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_EndPushBuffer_1_0_5788, D3DDevice_EndPushBuffer),
	// */
    // D3DDevice8::RunPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_RunPushBuffer_1_0_5788, D3DDevice_RunPushBuffer),
	// D3DDevice8::Begin (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_Begin_1_0_5233, D3DDevice_Begin),
    // D3DDevice8::BeginStateBig
	OOVPA_TABLE_XREF(D3DDevice_BeginStateBig_1_0_5788), 
	// D3DDevice8::GetTransform (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetTransform_1_0_4361, D3DDevice_GetTransform),
	// D3DDevice8::SetTransform (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTransform_1_0_5558, D3DDevice_SetTransform),
	// D3DDevice8::CreateIndexBuffer2
	OOVPA_TABLE_PATCH(D3DDevice_CreateIndexBuffer2_1_0_5558, D3DDevice_CreateIndexBuffer2),
	// D3DDevice8::DrawIndexedVertices
	OOVPA_TABLE_PATCH(D3DDevice_DrawIndexedVertices_1_0_5788, D3DDevice_DrawIndexedVertices),
	// D3DDevice8::DrawIndexedVerticesUP
	OOVPA_TABLE_PATCH(D3DDevice_DrawIndexedVerticesUP_1_0_5788, D3DDevice_DrawIndexedVerticesUP),
	// D3DDevice8::SetMaterial
	OOVPA_TABLE_PATCH(D3DDevice_SetMaterial_1_0_5788, D3DDevice_SetMaterial),
	// D3DDevice8::SetStreamSource (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetStreamSource_1_0_4361, D3DDevice_SetStreamSource),
	// D3DResource8::Register (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DResource_Register_1_0_3925, D3DResource_Register),
	// D3DResource8::AddRef (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DResource_AddRef_1_0_3925, D3DResource_AddRef),
	// D3DResource8::Release (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DResource_Release_1_0_3925, D3DResource_Release),
	// D3DResource8::IsBusy (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DResource_IsBusy_1_0_4361, D3DResource_IsBusy),
    // D3D::BlockOnResource (XREF) (* unchanged since 5558 *)
	OOVPA_TABLE_XREF(D3D_BlockOnResource_1_0_5558),
	// D3DResource8::BlockUntilNotBusy (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DResource_BlockUntilNotBusy_1_0_5558, D3DResource_BlockUntilNotBusy),
	// D3DVertexBuffer8::GetDesc (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DVertexBuffer_GetDesc_1_0_5233, D3DVertexBuffer_GetDesc),
	// D3DSurface8::GetDesc (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DSurface_GetDesc_1_0_4361, D3DSurface_GetDesc),
	// D3DDevice8::CopyRects (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_CopyRects_1_0_5233, D3DDevice_CopyRects),
	// D3DPalette8::Lock2 (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DPalette_Lock2_1_0_5233, D3DPalette_Lock2),
	// D3DTexture8::LockRect
	OOVPA_TABLE_PATCH(D3DTexture_LockRect_1_0_5558, D3DTexture_LockRect),
	// D3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DSurface_LockRect_1_0_3925, D3DSurface_LockRect),
	// D3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DBaseTexture_GetLevelCount_1_0_4361, D3DBaseTexture_GetLevelCount),
	// D3DTexture8::GetSurfaceLevel2 (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DTexture_GetSurfaceLevel2_1_0_4627, D3DTexture_GetSurfaceLevel2),
	// D3DDevice8::SetLight
	OOVPA_TABLE_PATCH(D3DDevice_SetLight_1_0_5558, D3DDevice_SetLight),
	// D3DDevice8::CreateVertexBuffer2
	OOVPA_TABLE_PATCH(D3DDevice_CreateVertexBuffer2_1_0_5788, D3DDevice_CreateVertexBuffer2),
	// D3DVertexBuffer8_Lock
	OOVPA_TABLE_PATCH(D3DVertexBuffer_Lock_1_0_5788, D3DVertexBuffer_Lock),
	// D3DVertexBuffer8::Lock2
	OOVPA_TABLE_PATCH(D3DVertexBuffer_Lock2_1_0_5788, D3DVertexBuffer_Lock2),
	// D3DDevice_LightEnable
	OOVPA_TABLE_PATCH(D3DDevice_LightEnable_1_0_5788, D3DDevice_LightEnable),
	// D3DDevice_SetModelView (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetModelView_1_0_5558, D3DDevice_SetModelView),
	// D3DDevice_PersistDisplay (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_PersistDisplay_1_0_5558, D3DDevice_PersistDisplay),
	// D3D8::SetPushBufferSize (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3D_SetPushBufferSize_1_0_5558, D3D_SetPushBufferSize),
	// D3D8::CheckDeviceMultiSampleType (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3D_CheckDeviceMultiSampleType_1_0_5558, D3D_CheckDeviceMultiSampleType),
	// D3D8::GetDeviceCaps (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3D_GetDeviceCaps_1_0_5233, D3D_GetDeviceCaps),
	// D3DDevice_DrawVertices
	OOVPA_TABLE_PATCH(D3DDevice_DrawVertices_1_0_5788, D3DDevice_DrawVertices),
	// D3D8_AllocContiguousMemory
	OOVPA_TABLE_PATCH(D3D_AllocContiguousMemory_1_0_5788, D3D_AllocContiguousMemory),
	// D3DTexture8_GetLevelDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_5788, Get2DSurfaceDesc),
	// D3DDevice8::CreateTexture2
	OOVPA_TABLE_PATCH(D3DDevice_CreateTexture2_1_0_5788, D3DDevice_CreateTexture2),
	// Lock2DSurface
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, Lock2DSurface),
	// D3DDevice8::SetVertexShaderConstant4
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShaderConstant4_1_0_4627, D3DDevice_SetVertexShaderConstant4),
	// D3DDevice8::SetVertexShaderConstant1
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShaderConstant1_1_0_5788, D3DDevice_SetVertexShaderConstant1),
	// D3DDevice8::DrawVerticesUP
	OOVPA_TABLE_PATCH(D3DDevice_DrawVerticesUP_1_0_5788, D3DDevice_DrawVerticesUP),
	// D3DDevice8::SetRenderTarget
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderTarget_1_0_5788, D3DDevice_SetRenderTarget),
	// D3DDevice8::SetShaderConstantMode (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetShaderConstantMode_1_0_5233, D3DDevice_SetShaderConstantMode),
	// D3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(D3DDevice_SetViewport_1_0_5788, D3DDevice_SetViewport),
	// D3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(D3DDevice_BlockUntilVerticalBlank_1_0_5788, D3DDevice_BlockUntilVerticalBlank),
	// D3DDevice_GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetDeviceCaps_1_0_3925, D3DDevice_GetDeviceCaps),
	// D3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FrontFace_1_0_4134, D3DDevice_SetRenderState_FrontFace),
	// D3DDevice8::GetModelView (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetModelView_1_0_5558, D3DDevice_GetModelView),
	// D3DDevice8::SetBackMaterial
	OOVPA_TABLE_PATCH(D3DDevice_SetBackMaterial_1_0_5788, D3DDevice_SetBackMaterial),
	// D3DDevice8::SetRenderState_TwoSidedLighting
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TwoSidedLighting_1_0_5788, D3DDevice_SetTextureState_TwoSidedLighting),
	// D3D8::KickOffAndWaitForIdle (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3D_KickOffAndWaitForIdle_1_0_5233, D3D_KickOffAndWaitForIdle),
	// D3D::SetTileNoWait
	OOVPA_TABLE_PATCH(D3DDevice_SetTileNoWait_1_0_5788, D3DDevice_SetTileNoWait),
	// D3DDevice8::SetGammaRamp (* unchanged since 4928 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetGammaRamp_1_0_4928, D3DDevice_SetGammaRamp),
	// D3DDevice8::GetGammaRamp (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetGammaRamp_1_0_4034, D3DDevice_GetGammaRamp),
	// D3DDevice_SetSwapCallback
	OOVPA_TABLE_PATCH(D3DDevice_SetSwapCallback_1_0_5788, D3DDevice_SetSwapCallback),
	// D3DDevice8::GetRenderTarget2 (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_GetRenderTarget2_1_0_5233, D3DDevice_GetRenderTarget2),
	// D3DDevice_SetRenderState_StencilFail
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilFail_1_0_5788, D3DDevice_SetRenderState_StencilFail),
	// D3DDevice_SetRenderState_VertexBlend
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_VertexBlend_1_0_5788, D3DDevice_SetRenderState_VertexBlend),
	// D3DDevice_SetRenderState_VertexBlend
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_VertexBlend_1_0_5788, D3DDevice_SetRenderState_VertexBlend),
	// D3DDevice8::Reset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_Reset_1_0_4134, D3DDevice_Reset),
	// D3DDevice::Release
	OOVPA_TABLE_PATCH(D3DDevice_Release_1_0_5788, D3DDevice_Release),
	// D3DDevice_InsertCallback (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_InsertCallback_1_0_5558, D3DDevice_InsertCallback),
	// D3DDevice::SetRenderState_LineWidth
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LineWidth_1_0_5788, D3DDevice_SetRenderState_LineWidth),
	// D3DDevice_SetRenderState_SampleAlpha (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_SampleAlpha_1_0_5233, D3DDevice_SetRenderState_SampleAlpha),
	// D3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_ColorKeyColor_1_0_4134, D3DDevice_SetTextureState_ColorKeyColor),
	// D3DDevice8::IsFencePending (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_IsFencePending_1_0_5558, D3DDevice_IsFencePending),
	// D3DDevice8::EnableOverlay (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_EnableOverlay_1_0_4361, D3DDevice_EnableOverlay),
	// D3DDevice_SetRenderState_LogicOp (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LogicOp_1_0_5558, D3DDevice_SetRenderState_LogicOp),
	// D3DDevice8::CreatePalette2 (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_CreatePalette2_1_0_5558, D3DDevice_CreatePalette2),
	// D3DDevice_SetPalette
	OOVPA_TABLE_PATCH(D3DDevice_SetPalette_1_0_5788, D3DDevice_SetPalette),
	// D3DDevice_LoadVertexShaderProgram
	OOVPA_TABLE_PATCH(D3DDevice_LoadVertexShaderProgram_1_0_5788, D3DDevice_LoadVertexShaderProgram),
	// D3DDevice_GetPushBufferOffset
	OOVPA_TABLE_PATCH(D3DDevice_GetPushBufferOffset_1_0_5788, D3DDevice_GetPushBufferOffset),
	// D3D::MakeRequestedSpace
	OOVPA_TABLE_XREF(D3D_MakeRequestedSpace_1_0_5788),
	// D3D::Unknown
	OOVPA_TABLE_XREF(D3D_Unknown_1_0_5788),
	// D3DDevice8::SetVertexShaderConstantNotInline (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShaderConstantNotInline_1_0_5233, D3DDevice_SetVertexShaderConstantNotInline),
	// D3DDevice_FlushVertexCache
	OOVPA_TABLE_PATCH(D3DDevice_FlushVertexCache_1_0_5788, D3DDevice_FlushVertexCache),
	// D3DDevice_GetVertexShaderSize
	OOVPA_TABLE_PATCH(D3DDevice_GetVertexShaderSize_1_0_5788, D3DDevice_GetVertexShaderSize),
	// D3DDevice8::LoadVertexShader (* unchanged since 5233 *)
	OOVPA_TABLE_PATCH(D3DDevice_LoadVertexShader_1_0_5233, D3DDevice_LoadVertexShader),
	// D3DDevice8::SelectVertexShader (* unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SelectVertexShader_1_0_5558, D3DDevice_SelectVertexShader),
	// D3DDevice_RunVertexStateShader (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_RunVertexStateShader_1_0_4627, D3DDevice_RunVertexStateShader),
	// D3DDevice8::AddRef
	OOVPA_TABLE_PATCH(D3DDevice_AddRef_1_0_5788, D3DDevice_AddRef),
	// D3D::ClearStateBlockFlags
	OOVPA_TABLE_XREF(D3D_ClearStateBlockFlags_1_0_5788),
	// D3D::RecordStateBlock
	OOVPA_TABLE_XREF(D3D_RecordStateBlock_1_0_5788),
	// D3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_BeginStateBlock_1_0_4361, D3DDevice_BeginStateBlock),
	// D3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_EndStateBlock_1_0_4361, D3DDevice_EndStateBlock),
	// D3DDevice_SetRenderState_BackFillMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_BackFillMode_1_0_5788, D3DDevice_SetRenderState_BackFillMode),
	// D3DDevice_SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5788, D3DDevice_SetRenderState_Dxt1NoiseEnable),
	// D3DDevice_GetTile
	OOVPA_TABLE_PATCH(D3DDevice_GetTile_1_0_5788, D3DDevice_GetTile),
	// D3D::CDevice::KickOff
	OOVPA_TABLE_PATCH(D3D_CDevice_KickOff_1_0_5788, D3DDevice_KickOff),
	// D3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead_1_0_4134, D3DDevice_SetRenderState_RopZCmpAlwaysRead),
	// D3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_RopZRead_1_0_4134, D3DDevice_SetRenderState_RopZRead),
	// D3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_DoNotCullUncompressed_1_0_4134, D3DDevice_SetRenderState_DoNotCullUncompressed),
	// D3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(D3DDevice_SetSoftDisplayFilter_1_0_5788, D3DDevice_SetSoftDisplayFilter),
	// D3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(D3DDevice_SetFlickerFilter_1_0_5788, D3DDevice_SetFlickerFilter),
	// D3DDevice8::SetRenderState_MultiSampleMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMode_1_0_5788, D3DDevice_SetRenderState_MultiSampleMode),
	// D3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5788, D3DDevice_SetRenderState_MultiSampleRenderTargetMode),
	// D3DDevice8::SetRenderState_MultiSampleAntiAlias
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_5788, D3DDevice_SetRenderState_MultiSampleAntiAlias),
	// D3DDevice8::SetRenderState_MultiSampleMask
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMask_1_0_5788, D3DDevice_SetRenderState_MultiSampleMask),
	// D3DDevice8::SetPixelShaderConstant
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShaderConstant_1_0_5788, D3DDevice_SetPixelShaderConstant),
	// D3DDevice_GetDisplayFieldStatus
	OOVPA_TABLE_PATCH(D3DDevice_GetDisplayFieldStatus_1_0_5788, D3DDevice_GetDisplayFieldStatus),
	// D3DCubeTexture8::GetCubeMapSurface2 (* Unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface2_1_0_4627, D3DCubeTexture_GetCubeMapSurface2),
	// D3DDevice8::SetVertexShaderConstant1Fast (* Unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShaderConstant1Fast_1_0_5558, D3DDevice_SetVertexShaderConstant1),
	// D3DDevice_SetPixelShaderProgram (* Unchanged since 5558 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShaderProgram_1_0_5558, D3DDevice_SetPixelShaderProgram),
	// D3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(D3DDevice_SetScissors_1_0_5788, D3DDevice_SetScissors),
	// D3DDevice8::GetScissors
	OOVPA_TABLE_PATCH(D3DDevice_GetScissors_1_0_5788, D3DDevice_GetScissors),
	// D3DDevice8::GetBackMaterial
	OOVPA_TABLE_PATCH(D3DDevice_GetBackMaterial_1_0_5788, D3DDevice_GetBackMaterial),
	// D3DDevice8::ApplyStateBlock (* unchanged since 4627 *)
	OOVPA_TABLE_PATCH(D3DDevice_ApplyStateBlock_1_0_4627, D3DDevice_ApplyStateBlock),
	// D3DDevice8::CaptureStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_CaptureStateBlock_1_0_5788, D3DDevice_CaptureStateBlock),
	// D3DDevice8::DeleteStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_DeleteStateBlock_1_0_5788, D3DDevice_DeleteStateBlock),
	// D3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilCullEnable_1_0_4134, D3DDevice_SetRenderState_StencilCullEnable),
	// D3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_OcclusionCullEnable_1_0_4134, D3DDevice_SetRenderState_OcclusionCullEnable),
	// D3DDevice8::SetRenderState_RopZCmpAlwaysRead
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_RopZCmpAlwaysRead_1_0_5788, D3DDevice_SetRenderState_RopZCmpAlwaysRead),
	// D3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_RopZRead_1_0_4134, D3DDevice_SetRenderState_RopZRead),
	// D3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_DoNotCullUncompressed_1_0_4134, D3DDevice_SetRenderState_DoNotCullUncompressed),
	// D3DDevice_GetTexture2
	OOVPA_TABLE_PATCH(D3DDevice_GetTexture2_1_0_5788, D3DDevice_GetTexture2),
	// D3DDevice8::SetRenderTargetFast (* unchanged since 5344 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderTargetFast_1_0_5344, D3DDevice_SetRenderTargetFast),
	// D3DDevice_SetDepthClipPlanes
	OOVPA_TABLE_PATCH(D3DDevice_SetDepthClipPlanes_1_0_5788, D3DDevice_SetDepthClipPlanes),
};

// ******************************************************************
// * D3D8_1_0_5788_SIZE
// ******************************************************************
uint32 D3D8_1_0_5788_SIZE = sizeof(D3D8_1_0_5788);
