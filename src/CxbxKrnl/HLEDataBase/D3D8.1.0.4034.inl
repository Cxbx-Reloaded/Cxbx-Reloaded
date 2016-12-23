// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4034.cpp
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
// * IDirect3DDevice8_GetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetGammaRamp_1_0_4034, 7)
        { 0x05, 0x8B },
        { 0x0A, 0x00 },
        { 0x10, 0xE1 },
        { 0x16, 0x0C },
        { 0x1C, 0x00 },
        { 0x22, 0x00 },
        { 0x28, 0x04 },
OOVPA_END;


// ******************************************************************
// * IDirect3DDevice8_GetCreationParameters
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetCreationParameters_1_0_4034, 7)

        { 0x06, 0x8B },
        { 0x0E, 0x8B },
        { 0x16, 0x4E },
        { 0x1E, 0x83 },
        { 0x26, 0x00 },
        { 0x2E, 0x5E },
        { 0x36, 0xC9 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetViewport_1_0_4034, 10)

        // IDirect3DDevice8_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetViewport+0x0B : lea esi, [eax+0x0B50]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x0B }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetViewport_1_0_4034, 10)

        // IDirect3DDevice8_SetViewport+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetViewport+0x2D : mov ebx, [esp+0x14]
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #4
        { 0x2E, 0x5C }, // (Offset,Value)-Pair #5
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #6
        { 0x30, 0x14 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetViewport+0x53 : jnz +0x12
        { 0x53, 0x75 }, // (Offset,Value)-Pair #8
        { 0x54, 0x12 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetViewport+0x9B : inc edx
        { 0x9B, 0x42 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTransform_1_0_4034, 10)

        // IDirect3DDevice8_SetTransform+0x44 : fld dword ptr [edx+0x28]
        { 0x44, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x45, 0x42 }, // (Offset,Value)-Pair #2
        { 0x46, 0x28 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTransform+0x51 : test ah, 0x44
        { 0x51, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x52, 0xC4 }, // (Offset,Value)-Pair #5
        { 0x53, 0x44 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTransform+0x5F : fnstsw ax
        { 0x5F, 0xDF }, // (Offset,Value)-Pair #7
        { 0x60, 0xE0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTransform+0x84 : fdivp st(1), st
        { 0x84, 0xDE }, // (Offset,Value)-Pair #9
        { 0x85, 0xF9 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_SetRenderState_CullMode_1_0_4034, 14,

    XREF_DXSRSCULLMODE,
    XRefZero)

        // IDirect3DDevice8_SetRenderState_CullMode+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #3
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #6
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x26, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x31, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #11
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x55, 0x04 }, // (Offset,Value)-Pair #13
        { 0x56, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0A, 0x46 },
        { 0x13, 0xC7 },
        { 0x16, 0x17 },
        { 0x1C, 0x00 },
        { 0x22, 0x48 },
        { 0x28, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest_1_0_4034, 7)

        { 0x0B, 0x8B },
        { 0x16, 0x5E },
        { 0x22, 0x07 },
        { 0x2E, 0x00 },
        { 0x3A, 0x81 },
        { 0x46, 0x89 },
        { 0x55, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects_1_0_4034, 8)

        { 0x1E, 0x57 },
        { 0x3E, 0xF6 },
        { 0x5E, 0xEB },
        { 0x7E, 0x00 },
        { 0x9E, 0x10 },
        { 0xBE, 0x24 },
        { 0xDE, 0x22 },
        { 0xFE, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeletePatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeletePatch_1_0_4034, 7)

        { 0x05, 0x0D },
        { 0x0C, 0xA1 },
        { 0x13, 0x34 },
        { 0x1A, 0x4E },
        { 0x21, 0x00 },
        { 0x2A, 0x56 },
        { 0x30, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawIndexedVertices_1_0_4034, 11)

        // IDirect3DDevice8_DrawIndexedVertices+0x0E : mov eax, [esi+0x1C]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x46 }, // (Offset,Value)-Pair #2
        { 0x10, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawIndexedVertices+0x20 : push 0x0209
        { 0x20, 0x68 }, // (Offset,Value)-Pair #4
        { 0x21, 0x09 }, // (Offset,Value)-Pair #5
        { 0x22, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_DrawIndexedVertices+0x64 : dec eax
        { 0x64, 0x48 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawIndexedVertices+0xF1 : prefetchnta byte ptr [esi+0x3C]
        { 0xF1, 0x0F }, // (Offset,Value)-Pair #8
        { 0xF2, 0x18 }, // (Offset,Value)-Pair #9
        { 0xF3, 0x46 }, // (Offset,Value)-Pair #10
        { 0xF4, 0x3C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetStreamSource_1_0_4034, 14)

        // IDirect3DDevice8_SetStreamSource+0x08 : mov edi, [esp+8+0x04]
        { 0x08, 0x8B }, // (Offset,Value)-Pair #1
        { 0x09, 0x7C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetStreamSource+0x28 : jz +0x26
        { 0x28, 0x74 }, // (Offset,Value)-Pair #5
        { 0x29, 0x26 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetStreamSource+0x70 : or ecx, 0x40
        { 0x70, 0x83 }, // (Offset,Value)-Pair #7
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x72, 0x40 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetStreamSource+0x7E : or ecx, 0x70
        { 0x7E, 0x83 }, // (Offset,Value)-Pair #10
        { 0x7F, 0xC9 }, // (Offset,Value)-Pair #11
        { 0x80, 0x70 }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_SetStreamSource+0x89 : retn 0x0C
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x8A, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x24 },
        { 0x26, 0x24 },
        { 0x2E, 0x89 },
        { 0x39, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x89 },
        { 0x26, 0x04 },
        { 0x2E, 0x07 },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawVertices_1_0_4034, 12)

        // IDirect3DDevice8_DrawVertices+0x12 : mov edi, [esp+0x0C+0x0C]
        { 0x12, 0x8B }, // (Offset,Value)-Pair #1
        { 0x13, 0x7C }, // (Offset,Value)-Pair #2
        { 0x14, 0x24 }, // (Offset,Value)-Pair #3
        { 0x15, 0x18 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawVertices+0x16 : lea esi, [edi-1]
        { 0x16, 0x8D }, // (Offset,Value)-Pair #5
        { 0x17, 0x77 }, // (Offset,Value)-Pair #6
        { 0x18, 0xFF }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawVertices+0x7E : shl edi, 0x18
        { 0x7E, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x7F, 0xE7 }, // (Offset,Value)-Pair #9
        { 0x80, 0x18 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_DrawVertices+0x9C : retn 0x0C
        { 0x9C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x9D, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LoadVertexShader_1_0_4034, 10)

        // IDirect3DDevice8_LoadVertexShader+0x07 : mov al, [ebx+0x08]
        { 0x07, 0x8A }, // (Offset,Value)-Pair #1
        { 0x08, 0x43 }, // (Offset,Value)-Pair #2
        { 0x09, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_LoadVertexShader+0x32 : mov dword ptr [eax], 0x00041E9C
        { 0x32, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x33, 0x00 }, // (Offset,Value)-Pair #5
        { 0x34, 0x9C }, // (Offset,Value)-Pair #6
        { 0x35, 0x1E }, // (Offset,Value)-Pair #7
        { 0x36, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_LoadVertexShader+0x4E : mov [ebx], ebx
        { 0x4E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x13 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawRectPatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawRectPatch_1_0_4034, 8)

        { 0x1E, 0x6C },
        { 0x3E, 0x3C },
        { 0x5E, 0x54 },
        { 0x7F, 0x89 },
        { 0x9E, 0x00 },
        { 0xBE, 0xD9 },
        { 0xDE, 0xD9 },
        { 0xFE, 0x86 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetIndices_1_0_4034, 11)

        // IDirect3DDevice8_SetIndices+0x0E : jz +0x10
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #1
        { 0x0F, 0x10 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetIndices+0x1E : jmp +0x0A
        { 0x1E, 0xEB }, // (Offset,Value)-Pair #3
        { 0x1F, 0x0A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetIndices+0x59 : mov [esi+0x1C], ecx
        { 0x59, 0x89 }, // (Offset,Value)-Pair #5
        { 0x5A, 0x4E }, // (Offset,Value)-Pair #6
        { 0x5B, 0x1C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetIndices+0x76 : mov [esi+0x0504], edi
        { 0x76, 0x89 }, // (Offset,Value)-Pair #8
        { 0x77, 0xBE }, // (Offset,Value)-Pair #9
        { 0x78, 0x04 }, // (Offset,Value)-Pair #10
        { 0x79, 0x05 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x03 },
        { 0x21, 0x48 },
        { 0x28, 0x89 },
        { 0x2F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x13, 0x8B },
        { 0x19, 0x8B },
        { 0x22, 0x4C },
        { 0x2B, 0x8C },
        { 0x34, 0x08 },
        { 0x40, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode_1_0_4034, 8)

        { 0x00, 0x8B },
        { 0x0B, 0xA3 },
        { 0x14, 0x04 },
        { 0x23, 0x8B },
        { 0x2A, 0xC9 },
        { 0x35, 0xC7 },
        { 0x40, 0x08 },
        { 0x4C, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor_1_0_4034, 7)

        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x0E },
        { 0x26, 0xE7 },
        { 0x30, 0x00 },
        { 0x3A, 0x89 },
        { 0x44, 0x0D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace_1_0_4034, 6)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x2A, 0x89 },
        { 0x30, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp_1_0_4034, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_4034, 7)

        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0xC1 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask_1_0_4034, 7)

        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0x83 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x26, 0x0D },
        { 0x2E, 0x00 },
        { 0x36, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc_1_0_4034, 7)

        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x1E },
        { 0x21, 0xFF },
        { 0x28, 0x08 },
        { 0x31, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_Simple_1_0_4034, 13)

        // IDirect3DDevice8_SetRenderState_Simple+0x05 : add eax, 0x08
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_Simple+0x0E : jnb +0x0C
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_Simple+0x15 : mov [eax-8], ecx
        { 0x15, 0x89 }, // (Offset,Value)-Pair #6
        { 0x16, 0x48 }, // (Offset,Value)-Pair #7
        { 0x17, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_Simple+0x18 : mov [eax-4], ecx
        { 0x18, 0x89 }, // (Offset,Value)-Pair #9
        { 0x19, 0x50 }, // (Offset,Value)-Pair #10
        { 0x1A, 0xFC }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_Simple+0x25 : jmp +0xD9
        { 0x25, 0xEB }, // (Offset,Value)-Pair #12
        { 0x26, 0xD9 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable_1_0_4034, 7)

        { 0x13, 0x8B },
        { 0x20, 0x02 },
        { 0x34, 0x85 },
        { 0x42, 0x75 },
        { 0x53, 0x89 },
        { 0x64, 0x00 },
        { 0x75, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail_1_0_4034, 7)

        { 0x0C, 0x72 },
        { 0x1A, 0xC9 },
        { 0x2A, 0x85 },
        { 0x36, 0x74 },
        { 0x44, 0x83 },
        { 0x52, 0x24 },
        { 0x60, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable_1_0_4034, 7)

        { 0x04, 0x56 },
        { 0x0B, 0xA3 },
        { 0x10, 0x8B },
        { 0x16, 0x05 },
        { 0x1C, 0x8B },
        { 0x25, 0x89 },
        { 0x28, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting_1_0_4034, 7)

        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x46 },
        { 0x35, 0x8B },
        { 0x43, 0x75 },
        { 0x4C, 0x00 },
        { 0x59, 0x3D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend_1_0_4034, 7)

        { 0x07, 0xCA },
        { 0x13, 0x89 },
        { 0x19, 0x8B },
        { 0x25, 0x8B },
        { 0x2B, 0x28 },
        { 0x34, 0x08 },
        { 0x3D, 0x5E },
OOVPA_END;

// ******************************************************************
// * Direct3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CheckDeviceFormat_1_0_4034, 7)

        { 0x0E, 0x18 },
        { 0x1E, 0x00 },
        { 0x2E, 0x42 },
        { 0x41, 0x40 },
        { 0x4E, 0x74 },
        { 0x5E, 0x08 },
        { 0x6E, 0x3C },
OOVPA_END;

// ******************************************************************
// * Direct3D_SetPushBufferSize
// ******************************************************************
OOVPA_NO_XREF(Direct3D_SetPushBufferSize_1_0_4034, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0D, 0x89 },
        { 0x0E, 0x0D },
        { 0x13, 0xC2 },
        { 0x14, 0x08 },
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
/*OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_4034, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x62 : mov eax, [eax+0x21DC]
        { 0x56, 0x8B }, // (Offset,Value)-Pair #5
        { 0x57, 0x80 }, // (Offset,Value)-Pair #6
        { 0x58, 0xDC }, // (Offset,Value)-Pair #7
        { 0x59, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
    }
OOVPA_END;
*/

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_ZEnable_1_0_4034, 13)

        // IDirect3DDevice8_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x96 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xF4 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnable+0x31 : mov dword ptr [eax], 0x4030C
        { 0x31, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x32, 0x00 }, // (Offset,Value)-Pair #8
        { 0x33, 0x0C }, // (Offset,Value)-Pair #9
        { 0x34, 0x03 }, // (Offset,Value)-Pair #10
        { 0x35, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_ZEnable+0x98 : retn 0x04
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x99, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_LightEnable_1_0_4034, 13)

        // IDirect3DDevice8_LightEnable+0x0F : cmp ebx, [esi+0x050C]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x0C }, // (Offset,Value)-Pair #3
        { 0x12, 0x05 }, // (Offset,Value)-Pair #4

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
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4034, 9)

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x04E8]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

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
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_4034, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21FC]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0xFC }, // (Offset,Value)-Pair #7
        { 0x55, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
OOVPA_NO_XREF_LARGE(IDirect3DDevice8_Clear_1_0_4034, 9)
        // IDirect3DDevice8_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_Clear+0x000B : mov ebx, [ebp+0x21F4]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0xF4 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x21 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x039A, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetLight_1_0_4034, 12)

        // IDirect3DDevice8_SetLight+0x11 : mov edi, [ebp+0x50C]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #1
        { 0x12, 0xBD }, // (Offset,Value)-Pair #2
        { 0x13, 0x0C }, // (Offset,Value)-Pair #3
        { 0x14, 0x05 }, // (Offset,Value)-Pair #4

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
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
OOVPA_NO_XREF(IDirect3DVertexBuffer8_Lock_1_0_4034, 11)

        // IDirect3DVertexBuffer8_Lock+0x01 : mov bl, [esp+0x18]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x18 }, // (Offset,Value)-Pair #4

        // IDirect3DVertexBuffer8_Lock+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #6

        // IDirect3DVertexBuffer8_Lock+0x2F : test bl, 0xA0
        { 0x2F, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x31, 0xA0 }, // (Offset,Value)-Pair #9

        // IDirect3DVertexBuffer8_Lock+0x50 : retn 0x14
        { 0x50, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x51, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTexture_1_0_4034, 10)

        // IDirect3DDevice8_SetTexture+0x09 : push edi
        { 0x09, 0x57 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetTexture+0x21 : jz +0x20
        { 0x21, 0x74 }, // (Offset,Value)-Pair #2
        { 0x22, 0x20 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTexture+0x33 : test ecx, 0x78FFFF
        { 0x33, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x34, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x37, 0x78 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTexture+0x39 : mov [eax], edx
        { 0x39, 0x89 }, // (Offset,Value)-Pair #7
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTexture+0xC9 : jz +0x4D
        { 0xC9, 0x74 }, // (Offset,Value)-Pair #9
        { 0xCA, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPalette_1_0_4034, 12)

        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0C08]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x08 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0C }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x76 : add edi, 0x41B20
        { 0x76, 0x81 }, // (Offset,Value)-Pair #6
        { 0x77, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x78, 0x20 }, // (Offset,Value)-Pair #8
        { 0x79, 0x1B }, // (Offset,Value)-Pair #9
        { 0x7A, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x8B, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4034, 10)

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x13 : shl eax, 0x07
        { 0x13, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x15, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x25 : cmp eax, ecx
        { 0x25, 0x3B }, // (Offset,Value)-Pair #4
        { 0x26, 0xC1 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x6C : mov edi, 0x2400
        { 0x6C, 0xBF }, // (Offset,Value)-Pair #6
        { 0x6E, 0x24 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xB4 : shl edx, 0x04
        { 0xB4, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB5, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB6, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetMaterial_1_0_4034, 12)

        // IDirect3DDevice8_SetMaterial+0x0C : add edi, 0x0B70
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x0B }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x13, 0x11 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x20, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x21, 0x00 }, // (Offset,Value)-Pair #9
        { 0x22, 0x10 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2E, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShader_1_0_4034, 14)

        // IDirect3DDevice8_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShader+0x11 : mov eax, [esi+0x04F8]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x14, 0x04 }, // (Offset,Value)-Pair #7

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
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap_1_0_4034, 11)

        // IDirect3DDevice8_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x30 : mov [esi+0x2C38], eax
        { 0x30, 0x89 }, // (Offset,Value)-Pair #6
        { 0x31, 0x86 }, // (Offset,Value)-Pair #7
        { 0x32, 0x38 }, // (Offset,Value)-Pair #8
        { 0x33, 0x2C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xB0 : retn 4
        { 0xB0, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xB1, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_KickOffAndWaitForIdle_1_0_4034, 9)

        // IDirect3D8_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3D8_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // IDirect3D8_KickOffAndWaitForIdle+0x08 : push 0
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3D8_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // IDirect3D8_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3D8_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateImageSurface_1_0_4034, 9)

        // IDirect3DDevice8_CreateImageSurface+0x2A : push 0x18
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x18 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateImageSurface+0x2C : push 0x40
        { 0x2C, 0x6A }, // (Offset,Value)-Pair #3
        { 0x2D, 0x40 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CreateImageSurface+0x3B : push 0x0404
        { 0x3B, 0x68 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x04 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateImageSurface+0x61 : retn 0x10
        { 0x61, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x62, 0x10 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer_1_0_4034, 12)

        // IDirect3DDevice8_GetBackBuffer+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer+0x0D : jnz +0x07
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer+0x18 : sbb eax, eax
        { 0x18, 0x1B }, // (Offset,Value)-Pair #6
        { 0x19, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x21FC]
        { 0x1D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1E, 0x84 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x20, 0xFC }, // (Offset,Value)-Pair #11
        { 0x21, 0x21 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3D8_1_0_4034
// ******************************************************************
OOVPATable D3D8_1_0_4034[] = {

    // IDirect3D8::CreateDevice (* unchanged since 1.0.3925 *)
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDevice_1_0_3925, XTL::EmuIDirect3D8_CreateDevice),
	// IDirect3DDevice8::Clear
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_4034, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DResource8::Register
	OOVPA_TABLE_PATCH(IDirect3DResource8_Register_1_0_3925, XTL::EmuIDirect3DResource8_Register),
	// IDirect3DDevice8::CreatePalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePalette_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePalette),
	// ********************** BEG WARNING UNTESTED!!! *******************

#if 0
    // IDirect3DDevice8::BeginVisibilityTest
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginVisibilityTest_1_0_3925, XTL::EmuIDirect3DDevice8_BeginVisibilityTest),
	// IDirect3DDevice8::EndVisibilityTest
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndVisibilityTest_1_0_3925, XTL::EmuIDirect3DDevice8_EndVisibilityTest),
	// IDirect3DDevice8::LoadVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LoadVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_LoadVertexShader),
	// IDirect3DDevice8::SelectVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SelectVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_SelectVertexShader),
	// IDirect3DDevice8::Release
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Release_1_0_3925, XTL::EmuIDirect3DDevice8_Release),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_3925, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVerticalBlankCallback_1_0_3925, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// IDirect3DDevice8::SetRenderTarget
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderTarget_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderTarget),
	// IDirect3DDevice8::GetTransform
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTransform_1_0_3925, XTL::EmuIDirect3DDevice8_GetTransform),
	// IDirect3DDevice8::SetStreamSource
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_3925, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// IDirect3DDevice8::SetVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::SetPixelShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::SetIndices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndices_1_0_3925, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_3925, XTL::EmuIDirect3DDevice8_SetViewport),
	// IDirect3DDevice8::SetTexture
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_3925, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::DrawIndexedVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_3925, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// IDirect3DDevice8::Begin
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Begin_1_0_3925, XTL::EmuIDirect3DDevice8_Begin),
	// IDirect3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData2f_1_0_3925, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData4f_1_0_3925, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// IDirect3DDevice8::SetVertexDataColor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexDataColor_1_0_3925, XTL::EmuIDirect3DDevice8_SetVertexDataColor),
	// IDirect3DDevice8::End
	OOVPA_TABLE_PATCH(IDirect3DDevice8_End_1_0_3925, XTL::EmuIDirect3DDevice8_End),
	// IDirect3DDevice8::Clear
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_3925, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPalette_1_0_3925, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::SetVertexShaderConstant
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant_1_0_3925, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetFlickerFilter_1_0_3925, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSoftDisplayFilter_1_0_3925, XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter),
	// IDirect3DDevice8::SetTextureState_TexCoordIndex
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_3925, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetTextureState_BorderColor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BorderColor_1_0_3925, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// IDirect3DDevice8::SetRenderState_PSTextureModes
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_PSTextureModes_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_PSTextureModes),
	// IDirect3DDevice8::SetRenderState_StencilFail
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8::SetRenderState_CullMode
    /*
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	*/
	// IDirect3DDevice8::SetRenderState_Simple
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// IDirect3DDevice8::SetRenderState_FogColor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FogColor_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// IDirect3DDevice8::SetRenderState_FillMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FillMode_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// IDirect3DDevice8::SetRenderState_StencilEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnable_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
	// IDirect3DDevice8::SetRenderState_ZEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::Present
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Present_1_0_3925, XTL::EmuIDirect3DDevice8_Present),
	// IDirect3DDevice8::SetShaderConstantMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetShaderConstantMode_1_0_3925, XTL::EmuIDirect3DDevice8_SetShaderConstantMode),
	// IDirect3DDevice8::GetBackBuffer
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer_1_0_3925, XTL::EmuIDirect3DDevice8_GetBackBuffer),
	// IDirect3DDevice8::GetDepthStencilSurface
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface_1_0_3925, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface),
	// IDirect3DVertexBuffer8::Lock
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock_1_0_3925, XTL::EmuIDirect3DVertexBuffer8_Lock),
	// IDirect3DResource8::IsBusy
	OOVPA_TABLE_PATCH(IDirect3DResource8_IsBusy_1_0_3925, XTL::EmuIDirect3DResource8_IsBusy),
	// Get2DSurfacDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_3925, XTL::EmuGet2DSurfaceDesc),
	// ********************** END WARNING UNTESTED!!! *******************
#endif

	// IDirect3DDevice8::GetGammaRamp
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetGammaRamp_1_0_4034, XTL::EmuIDirect3DDevice8_GetGammaRamp),
	// IDirect3DDevice8::GetCreationParameters
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetCreationParameters_1_0_4034, XTL::EmuIDirect3DDevice8_GetCreationParameters),
	// IDirect3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetViewport_1_0_4034, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_4034, XTL::EmuIDirect3DDevice8_SetViewport),
	// IDirect3DDevice8::SetTransform
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTransform_1_0_4034, XTL::EmuIDirect3DDevice8_SetTransform),
	// IDirect3DDevice8::CreatePixelShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePixelShader),
	// D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderSize_1_0_3925, XTL::EmuIDirect3DDevice8_GetVertexShaderSize),
	// IDirect3DDevice8::SetRenderState_CullMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::CreateCubeTexture (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateCubeTexture_1_0_3925, XTL::EmuIDirect3DDevice8_CreateCubeTexture),
	// IDirect3DDevice8::GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDeviceCaps_1_0_3925, XTL::EmuIDirect3DDevice8_GetDeviceCaps),
	// IDirect3DDevice8::CreatePalette (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePalette_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePalette),
	// IDirect3DDevice8::CreateTexture (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture_1_0_3925, XTL::EmuIDirect3DDevice8_CreateTexture),
	// IDirect3DDevice8::CreateVolumeTexture (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVolumeTexture_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVolumeTexture),
	// IDirect3DDevice8::CreateIndexBuffer (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateIndexBuffer_1_0_3925, XTL::EmuIDirect3DDevice8_CreateIndexBuffer),
	// IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexShader),
	// IDirect3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZBias_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZBias),
	// IDirect3DDevice8::CreateVertexBuffer (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexBuffer_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexBuffer),
	// IDirect3DResource8::Release (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_3925, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DResource8::AddRef (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_AddRef_1_0_3925, XTL::EmuIDirect3DResource8_AddRef),
	// IDirect3DPalette8::Lock (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DPalette8_Lock_1_0_3925, XTL::EmuIDirect3DPalette8_Lock),
	// IDirect3DTexture8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DTexture8_LockRect),
	// IDirect3DVolumeTexture8::LockBox (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DVolumeTexture8_LockBox_1_0_3925, XTL::EmuIDirect3DVolumeTexture8_LockBox),
	// IDirect3DCubeTexture8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DCubeTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DCubeTexture8_LockRect),
	// IDirect3DTexture8::GetSurfaceLevel (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel_1_0_3925, XTL::EmuIDirect3DTexture8_GetSurfaceLevel),
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRect_1_0_3925, XTL::EmuIDirect3DSurface8_LockRect),
	// Lock2DSurface (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, XTL::EmuLock2DSurface),
	// IDirect3DDevice8_SetGammaRamp (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetGammaRamp_1_0_3925, XTL::EmuIDirect3DDevice8_SetGammaRamp),
	// IDirect3DDevice8::DrawVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVertices_1_0_4034, XTL::EmuIDirect3DDevice8_DrawVertices),
	// IDirect3DDevice8::DrawIndexedVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_4034, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// D3DDevice_BeginVisibilityTest
	OOVPA_TABLE_PATCH(D3DDevice_BeginVisibilityTest_1_0_4034, XTL::EmuIDirect3DDevice8_BeginVisibilityTest),
	// D3DDevice_EndVisibilityTest
	OOVPA_TABLE_PATCH(D3DDevice_EndVisibilityTest_1_0_4034, XTL::EmuIDirect3DDevice8_EndVisibilityTest),
	// D3DDevice_CopyRects
	OOVPA_TABLE_PATCH(D3DDevice_CopyRects_1_0_4034, XTL::EmuIDirect3DDevice8_CopyRects),
	// D3DDevice_DeletePatch (TODO)
	/*
	OOVPA_TABLE_PATCH(D3DDevice_DeletePatch_1_0_4034, XTL::EmuIDirect3DDevice8_DeletePatch),
	*/
	// D3DDevice_SetStreamSource
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_4034, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// D3DDevice_SetTextureState_BorderColor
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_BorderColor_1_0_4034, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// D3DDevice_SetTextureState_ColorKeyColor
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_ColorKeyColor_1_0_4034, XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor),
	// D3DDevice_LoadVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LoadVertexShader_1_0_4034, XTL::EmuIDirect3DDevice8_LoadVertexShader),
	// IDirect3DDevice8::GetVisibilityTestResult (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetVisibilityTestResult_1_0_3925, XTL::EmuIDirect3DDevice8_GetVisibilityTestResult),
	// D3DDevice_DeleteVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_DeleteVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_DeleteVertexShader),
	// D3DDevice_DrawRectPatch
	OOVPA_TABLE_PATCH(D3DDevice_DrawRectPatch_1_0_4034, XTL::EmuIDirect3DDevice8_DrawRectPatch),
	// D3DDevice_SetIndices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndices_1_0_4034, XTL::EmuIDirect3DDevice8_SetIndices),
	// D3DDevice_SetRenderState_EdgeAntiAlias
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_EdgeAntiAlias_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// D3DDevice_SetRenderState_FillMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FillMode_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// D3DDevice_SetRenderState_BackFillMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_BackFillMode_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode),
	// D3DDevice_SetRenderState_FogColor
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FogColor_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// D3DDevice_SetRenderState_FrontFace
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FrontFace_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace),
	// D3DDevice_SetRenderState_LogicOp
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LogicOp_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp),
	// D3DDevice_SetRenderState_MultiSampleAntiAlias
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// D3DDevice_SetRenderState_MultiSampleMask
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMask_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask),
	// D3DDevice_SetRenderState_NormalizeNormals
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_NormalizeNormals_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// D3DDevice_SetRenderState_ShadowFunc
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ShadowFunc_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc),
	// D3DDevice_SetRenderState_Simple
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// D3DDevice_SetRenderState_StencilEnable
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilEnable_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// D3DDevice_SetRenderState_StencilFail
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilFail_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// D3DDevice_SetRenderState_YuvEnable
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_YuvEnable_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable),
	// D3DDevice_SetRenderState_TwoSidedLighting
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TwoSidedLighting_1_0_4034, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
	// D3DDevice_SetRenderState_VertexBlend
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_VertexBlend_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend),
	// IDirect3D8_CheckDeviceFormat
	OOVPA_TABLE_PATCH(Direct3D_CheckDeviceFormat_1_0_4034, XTL::EmuIDirect3D8_CheckDeviceFormat),
	// IDirect3D8_SetPushBufferSize
	OOVPA_TABLE_PATCH(Direct3D_SetPushBufferSize_1_0_4034, XTL::EmuIDirect3D8_SetPushBufferSize),
	// Get2DSurfacDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_4034, XTL::EmuGet2DSurfaceDesc),
	// IDirect3DTexture8::GetSurfaceLevel (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel_1_0_3925, XTL::EmuIDirect3DTexture8_GetSurfaceLevel),
	// D3DDevice_SetRenderState_ZEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// D3DDevice_LightEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_4034, XTL::EmuIDirect3DDevice8_LightEnable),
	// D3DDevice_SetRenderState_TextureFactor
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// Direct3D_GetAdapterIdentifier (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Direct3D_GetAdapterIdentifier_1_0_3925, XTL::EmuIDirect3D8_GetAdapterIdentifier),
	// IDirect3DSurface8::GetDesc (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(D3DSurface_GetDesc_1_0_3925, XTL::EmuIDirect3DSurface8_GetDesc),
	// Get2DSurfacDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_4034, XTL::EmuGet2DSurfaceDesc),
	// IDirect3DDevice8::SetLight
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_4034, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DVertexBuffer8::Lock
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock_1_0_4034, XTL::EmuIDirect3DVertexBuffer8_Lock),
	// IDirect3DDevice8::SetTexture
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_4034, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPalette_1_0_4034, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::SetRenderState_TexCoordIndex
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4034, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetMaterial
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetMaterial_1_0_4034, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DDevice8::SetVertexShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_4034, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::Swap
	OOVPA_TABLE_PATCH(D3DDevice_Swap_1_0_4034, XTL::EmuIDirect3DDevice8_Swap),
	// IDirect3D8::KickOffAndWaitForIdle
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle_1_0_4034, XTL::EmuIDirect3D8_KickOffAndWaitForIdle),
	// IDirect3DDevice8::CreateImageSurface (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateImageSurface_1_0_4034, XTL::EmuIDirect3DDevice8_CreateImageSurface),
	// IDirect3DDevice8::GetBackBuffer
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer_1_0_4034, XTL::EmuIDirect3DDevice8_GetBackBuffer),
	// D3DDevice_
    /*
	OOVPA_TABLE_PATCH(D3DDevice__1_0_4034, XTL::EmuIDirect3DDevice8_),
	// D3DDevice_
	OOVPA_TABLE_PATCH(D3DDevice__1_0_4034, XTL::EmuIDirect3DDevice8_),
	// D3DDevice_
	OOVPA_TABLE_PATCH(D3DDevice__1_0_4034, XTL::EmuIDirect3DDevice8_),
	*/
};

// ******************************************************************
// * D3D8_1_0_4034_SIZE
// ******************************************************************
uint32 D3D8_1_0_4034_SIZE = sizeof(D3D8_1_0_4034);
