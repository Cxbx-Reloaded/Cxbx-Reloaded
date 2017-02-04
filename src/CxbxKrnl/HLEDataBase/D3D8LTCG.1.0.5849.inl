// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8LTCG.1.0.5849.cpp
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

// Verteks 09/03/2009
// For Burnout 3 Takedown

// ******************************************************************
// * D3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(D3D_CreateDevice_LTCG, 5849, 8)

        // D3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // D3D_CreateDevice+0x7F : rep stos
        { 0x7F, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x80, 0xAB }, // (Offset,Value)-Pair #4

        // D3D_CreateDevice+0x82 : mov eax, esi
        { 0x82, 0x8B }, // (Offset,Value)-Pair #5
        { 0x83, 0xC6 }, // (Offset,Value)-Pair #6

        // D3D_CreateDevice+0x8F : retn 0x18
        { 0x8F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x90, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader_LTCG, 5849, 9)

        // D3D_SetPixelShader+0x1E : jnz +0x67
        { 0x1E, 0x75 }, // (Offset,Value)-Pair #1
        { 0x1F, 0x67 }, // (Offset,Value)-Pair #2

        // D3D_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #3
        { 0x3F, 0x00 }, // (Offset,Value)-Pair #4

        // D3D_SetPixelShader+0x60 : push eax
        { 0x60, 0x50 }, // (Offset,Value)-Pair #5

        // D3D_SetPixelShader+0x61 : shr eax, 1
        { 0x61, 0xD1 }, // (Offset,Value)-Pair #6
        { 0x62, 0xE8 }, // (Offset,Value)-Pair #7

        // D3D_SetPixelShader+0xE1 : jb +0x0E
        { 0xE1, 0x72 }, // (Offset,Value)-Pair #8
        { 0xE2, 0x0E }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexShader_LTCG, 5849, 8)

        { 0x1E, 0x85 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x8B },
        { 0xDE, 0x73 },
        { 0xFD, 0xC7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices_LTCG, 5849, 8)

        { 0x13, 0x89 },
        { 0x3E, 0x3B },
        { 0x5E, 0x05 },
        { 0x7E, 0xC3 },
        { 0x9E, 0xC1 },
        { 0xC4, 0x03 },
        { 0xDE, 0xE2 },
        { 0xFD, 0xEA },
OOVPA_END;

// Verteks 10/03/2009

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture_LTCG, 5849, 8)

        { 0x1E, 0x7C },
        { 0x3C, 0xE8 },
        { 0x5F, 0xE8 },
        { 0x7E, 0x45 },
        { 0x9E, 0x08 },
        { 0xBE, 0x0C },
        { 0xDE, 0x0F },
        { 0xFE, 0x8D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport_LTCG, 5849, 8)

        { 0x1E, 0xAE },
        { 0x3E, 0x53 },
        { 0x5E, 0x00 },
        { 0x7E, 0x12 },
        { 0x9E, 0x75 },
        { 0xBE, 0x4C },
        { 0xDE, 0xE1 },
        { 0xF9, 0x7D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight_LTCG, 5849, 8)

        { 0x1E, 0x00 },
        { 0x3E, 0x89 },
        { 0x5E, 0x00 },
        { 0x7E, 0x07 },
        { 0x9E, 0xC8 },
        { 0xBE, 0xC9 },
        { 0xDE, 0x07 },
        { 0xFD, 0xC1 },
OOVPA_END;

/* Same as 5558
// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform_LTCG, 5849, 8)

        { 0x1D, 0xB9 },
        { 0x3E, 0x0F },
        { 0x5E, 0xD9 },
        { 0x7A, 0x00 },
        { 0x99, 0x83 },
        { 0xB8, 0xE0 },
        { 0xD7, 0xD9 },
        { 0xF6, 0xF6 },
OOVPA_END;
*/

/* Same as 5849
// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial_LTCG, 5849, 9)

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
*/

// ******************************************************************
// * D3DDevice_SetStreamSource
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStreamSource_LTCG, 5849, 14)
        // D3DDevice_SetStreamSource+0x07 : mov ebx, [esp+0x0C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x5C }, // (Offset,Value)-Pair #2
        { 0x09, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x0C }, // (Offset,Value)-Pair #4

        // D3DDevice_SetStreamSource+0x28 : jz +0x26
        { 0x28, 0x74 }, // (Offset,Value)-Pair #5
        { 0x29, 0x26 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetStreamSource+0x70 : or ecx, 0x40
        { 0x70, 0x83 }, // (Offset,Value)-Pair #7
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x72, 0x40 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetStreamSource+0x7E : or ecx, 0x70
        { 0x7E, 0x83 }, // (Offset,Value)-Pair #10
        { 0x7F, 0xC9 }, // (Offset,Value)-Pair #11
        { 0x80, 0x70 }, // (Offset,Value)-Pair #12

        // D3DDevice_SetStreamSource+0x89 : retn 0x0C
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x8A, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

/* Same as 5849
// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1_LTCG, 5849, 11)
        // D3DDevice_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant1+0x0F : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstant1+0x1D : mov [eax-0x18], ecx
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #6
        { 0x1E, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstant1+0x5D : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
OOVPA_END;
*/

/* Same as 5849
// ******************************************************************
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer2_LTCG, 5849, 7)
        { 0x08, 0xE8 },
        { 0x12, 0x1E },
        { 0x1C, 0x50 },
        { 0x26, 0x68 },
        { 0x31, 0x33 },
        { 0x3A, 0xFF },
        { 0x44, 0x01 },
OOVPA_END;
*/

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_TexCoordIndex_LTCG, 5849, 10)

        // D3DDevice_SetTextureState_TexCoordIndex+0x14 : shl eax, 0x07
        { 0x14, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x15, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x16, 0x07 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_TexCoordIndex+0x26 : cmp eax, ecx
        { 0x26, 0x3B }, // (Offset,Value)-Pair #4
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_TexCoordIndex+0x7A : mov ebp, 0x2400
        { 0x7A, 0xBD }, // (Offset,Value)-Pair #6
        { 0x7C, 0x24 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl ebx, 0x04
        { 0xB3, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB4, 0xE3 }, // (Offset,Value)-Pair #9
        { 0xB5, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv_LTCG, 5849, 12)
        // D3DDevice_SetTextureState_BumpEnv+0x18 : jnz +0x03
        { 0x18, 0x75 }, // (Offset,Value)-Pair #1
        { 0x19, 0x03 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetTextureState_BumpEnv+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_BumpEnv+0x3B : mov ecx, [esp+0x18]
        { 0x3B, 0x8B }, // (Offset,Value)-Pair #6
        { 0x3C, 0x4C }, // (Offset,Value)-Pair #7
        { 0x3D, 0x24 }, // (Offset,Value)-Pair #8
        { 0x3E, 0x18 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetTextureState_BumpEnv+0x59 : shl esi, 5
        { 0x59, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x5A, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x5B, 0x05 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor_LTCG, 5849, 15)
        // D3DDevice_SetTextureState_BorderColor+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #2

        // D3DDevice_SetTextureState_BorderColor+0x22 : shl edx, 6
        { 0x22, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x23, 0xE2 }, // (Offset,Value)-Pair #4
        { 0x24, 0x06 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_BorderColor+0x34 : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x34, 0x83 }, // (Offset,Value)-Pair #6
        { 0x35, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x36, 0x08 }, // (Offset,Value)-Pair #8
        { 0x37, 0x89 }, // (Offset,Value)-Pair #9
        { 0x38, 0x06 }, // (Offset,Value)-Pair #10
        { 0x39, 0xC1 }, // (Offset,Value)-Pair #11
        { 0x3A, 0xE1 }, // (Offset,Value)-Pair #12
        { 0x3B, 0x07 }, // (Offset,Value)-Pair #13

        // D3DDevice_SetTextureState_BorderColor+0x43 : retn 0x08
        { 0x43, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x44, 0x08 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode_LTCG, 5849, 13,

    XREF_DXSRSCULLMODE,
	XRefZero)

        // D3DDevice_SetRenderState_CullMode+0x23 : mov dword ptr [eax], 0x40308
        { 0x23, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x25, 0x08 }, // (Offset,Value)-Pair #2
        { 0x26, 0x03 }, // (Offset,Value)-Pair #3
        { 0x27, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_CullMode+0x2E : add eax, 8
        { 0x2E, 0x83 }, // (Offset,Value)-Pair #5
        { 0x2F, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x30, 0x08 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_CullMode+0x3A : retn 4
        { 0x3A, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x3B, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_CullMode+0x5C : add edx, 0x404
        { 0x5D, 0x81 }, // (Offset,Value)-Pair #10
        { 0x5E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x5F, 0x04 }, // (Offset,Value)-Pair #12
        { 0x60, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor_LTCG, 5849, 7)
        { 0x09, 0x84 },
        { 0x14, 0x46 },
        { 0x28, 0x24 },
        { 0x33, 0xBA },
        { 0x3D, 0x4A },
        { 0x48, 0x5E },
        { 0x55, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals_LTCG, 5849, 8)
        // D3DDevice_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_NormalizeNormals+0x2E : or [addr], 0x200
        { 0x2E, 0x81 }, // (Offset,Value)-Pair #3
        { 0x2F, 0x0D }, // (Offset,Value)-Pair #4
        { 0x34, 0x00 }, // (Offset,Value)-Pair #5
        { 0x35, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_NormalizeNormals+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x40, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable_LTCG, 5849, 8)
        { 0x0C, 0x72 },
        { 0x2B, 0xC9 },
        { 0x3D, 0x89 },
        { 0x4F, 0x0C },
        { 0x63, 0x74 },
        { 0x74, 0x8B },
        { 0x8A, 0x8B },
        { 0x97, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable_LTCG, 5849, 8)
        { 0x12, 0x8B },
        { 0x2D, 0x33 },
        { 0x40, 0x74 },
        { 0x4A, 0x74 },
        { 0x63, 0x89 },
        { 0x79, 0xB9 },
        { 0x92, 0x33 },
        { 0x9F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
//OOVPA_NO_XREF(D3DDevice_Swap_LTCG, 5849, 7)
//        { 0x1B, 0xE8 },
//        { 0x38, 0xC3 },
//        { 0x55, 0xC0 },
//        { 0x72, 0xEB },
//        { 0x93, 0x85 },
//        { 0xAA, 0xC6 },
//        { 0xC9, 0x0A },
//OOVPA_END;

/* Same as 4627
// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f_LTCG, 5849, 14)
        // D3DDevice_SetVertexData2f+0x20 : lea edx, ds:abs[ecx*8]
        { 0x20, 0x8D }, // (Offset,Value)-Pair #1
        { 0x21, 0x14 }, // (Offset,Value)-Pair #2
        { 0x22, 0xCD }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexData2f+0x2B : mov [eax], edx; mov edx, [esp+0x10]
        { 0x2B, 0x89 }, // (Offset,Value)-Pair #4
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #6
        { 0x2E, 0x54 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData2f+0x37 : add eax, 0x0C
        { 0x37, 0x83 }, // (Offset,Value)-Pair #10
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12

        // D3DDevice_SetVertexData2f+0x3D : retn 0x0C
        { 0x3D, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3E, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;
*/

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End_LTCG, 5849, 7)
        { 0x07, 0x8B },
        { 0x13, 0x50 },
        { 0x1C, 0xC7 },
        { 0x22, 0xC7 },
        { 0x2B, 0x08 },
        { 0x34, 0xE1 },
        { 0x49, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode_LTCG, 5849, 11)
        // D3DDevice_SetRenderState_FillMode+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_FillMode+0x2A : mov ecx, [esp+0x08]
        { 0x2A, 0x8B }, // (Offset,Value)-Pair #3
        { 0x2B, 0x4C }, // (Offset,Value)-Pair #4
        { 0x2C, 0x24 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_FillMode+0x2E : jnz +0x02
        { 0x2E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x02 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_FillMode+0x3E : add eax, 0x0C
        { 0x3E, 0x83 }, // (Offset,Value)-Pair #9
        { 0x3F, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x40, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor_LTCG, 5849, 10)
        // D3DDevice_SetRenderState_FogColor+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_FogColor+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #3
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_FogColor+0x2E : shl edi, 0x10
        { 0x2E, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x2F, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x30, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_FogColor+0x49 : mov [esi], eax
        { 0x49, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x06 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP_LTCG, 5849, 8)
        { 0x1E, 0xA1 },
        { 0x3E, 0x8B },
        { 0x5E, 0x08 },
        { 0x7E, 0xD0 },
        { 0x9E, 0x14 },
        { 0xBE, 0x8D },
        { 0xDE, 0x3B },
        { 0xFE, 0x81 },
OOVPA_END;

// Verteks 11/03/2009
// blueshogun96 7/17/2010

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock2_LTCG, 5849, 10)
		// D3DVertexBuffer_Lock2+0x05 : test bl, 0x10
		{ 0x05, 0xF6 },
		{ 0x06, 0xC3 },
		{ 0x07, 0x10 },
		// D3DVertexBuffer_Lock2+0x7F : or eax, 0x8000000
		{ 0x7F, 0x0D },
		{ 0x80, 0x00 },
		{ 0x81, 0x00 },
		{ 0x82, 0x00 },
		{ 0x83, 0x80 },
		// D3DVertexBuffer_Lock2+0x85 : retn 8
		{ 0x85, 0xC2 },
		{ 0x86, 0x08 },
OOVPA_END;


// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable_LTCG, 5849, 13)
        // D3DDevice_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_YuvEnable+0x10 : mov eax, [esi]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #5
        { 0x11, 0x06 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_YuvEnable+0x12 : cmp eax, [esi+4]
        { 0x12, 0x3B }, // (Offset,Value)-Pair #7
        { 0x13, 0x46 }, // (Offset,Value)-Pair #8
        { 0x14, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_YuvEnable+0x15 : jb +0x0E
        { 0x15, 0x72 }, // (Offset,Value)-Pair #10
        { 0x16, 0x0E }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_YuvEnable+0x30 : retn 0x04
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x31, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc_LTCG, 5849, 13)

        // D3DDevice_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_ShadowFunc+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_ShadowFunc+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #5
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_ShadowFunc+0x26 : lea edx, [ecx-0x0200]
        { 0x26, 0x8D }, // (Offset,Value)-Pair #7
        { 0x27, 0x91 }, // (Offset,Value)-Pair #8
        { 0x28, 0x00 }, // (Offset,Value)-Pair #9
        { 0x29, 0xFE }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_ShadowFunc+0x2F : add eax, 0x08
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #11
        { 0x30, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x31, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_OcclusionCullEnable_LTCG, 5849, 12)

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x15 : jb +0x0E
        { 0x15, 0x72 }, // (Offset,Value)-Pair #1
        { 0x16, 0x0E }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x4A : cmp [abs], 0x1E00
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #3
        { 0x50, 0x00 }, // (Offset,Value)-Pair #4
        { 0x51, 0x1E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x59 : mov dword ptr [eax], 0x41D84
        { 0x59, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x5A, 0x00 }, // (Offset,Value)-Pair #7
        { 0x5B, 0x84 }, // (Offset,Value)-Pair #8
        { 0x5C, 0x1D }, // (Offset,Value)-Pair #9
        { 0x5D, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x68 : retn 0x04
        { 0x68, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x69, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias_LTCG, 5849, 10)

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x1C : mov ecx, [esp+0x08]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x1D, 0x4C }, // (Offset,Value)-Pair #6
        { 0x1E, 0x24 }, // (Offset,Value)-Pair #7
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x38 : retn 0x04
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// I am not sure about this
// ******************************************************************
// * D3DTexture_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_LTCG, 5849, 9)

        // Get2DSurfaceDesc+0x1E : movzx ecx, byte ptr [edi+0x0D]
        { 0x1E, 0x0F }, // (Offset,Value)-Pair #1
        { 0x1F, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x20, 0x4F }, // (Offset,Value)-Pair #3
        { 0x21, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x45 : mov edx, [eax+0x1A14]
        { 0x45, 0x8B }, // (Offset,Value)-Pair #5
        { 0x46, 0x90 }, // (Offset,Value)-Pair #6
        { 0x47, 0x14 }, // (Offset,Value)-Pair #7
        { 0x48, 0x1A }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0x9E : retn
        { 0x9E, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode_LTCG, 5849, 8)

        { 0x19, 0xFD },
        { 0x38, 0x8B },
        { 0x60, 0x0B },
        { 0x7A, 0xBA },
        { 0xA4, 0xC0 },
        { 0xB2, 0x00 },
        { 0xDA, 0x89 },
        { 0xF9, 0x00 },
OOVPA_END;

/*
I don't know which one of these is the right one :
0x0034E040 2 args
0x0034E200 2 args
0x0034F150 3 args
// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 5849, 7)

        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
OOVPA_END;
*/

/* Maybe this : 0x0034D730 (NOT THIS)
   It's : 0x0034CC00 (same as 3925)

   0035FEE8 accessed by CreateDevice (must contain 0035DA40 ?)
0034D730  /$ A1 E8FE3500              mov     eax, dword ptr ds:[35FEE8]
0034D735  |. 8B88 041A0000            mov     ecx, dword ptr ds:[eax+1A04] <- Exception here
0034D73B  |. 85C9                     test    ecx, ecx
0034D73D  |. 74 24                    je      short default.0034D763
0034D73F  |. 8B01                     mov     eax, dword ptr ds:[ecx]
0034D741  |. A9 FFFF0000              test    eax, 0FFFF
0034D746  |. 75 19                    jnz     short default.0034D761
0034D748  |. 25 00000700              and     eax, 70000
0034D74D  |. 3D 00000500              cmp     eax, default.00050000
0034D752  |. 75 0D                    jnz     short default.0034D761
0034D754  |. 8B41 14                  mov     eax, dword ptr ds:[ecx+14]
0034D757  |. 85C0                     test    eax, eax
0034D759  |. 74 06                    je      short default.0034D761
0034D75B  |. 50                       push    eax
0034D75C  |. E8 9FF4FFFF              call    default.0034CC00             <- D3DResource_AddRef
0034D761  |> FF01                     inc     dword ptr ds:[ecx]
0034D763  |> 8BC1                     mov     eax, ecx
0034D765  \. C3                       retn
// ******************************************************************
// * D3DResource_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DResource_AddRef_LTCG, 5849, 10)
        // D3DResource_AddRef+0x11 : test eax, 0xFFFF
        { 0x11, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x12, 0xFF }, // (Offset,Value)-Pair #2
        { 0x13, 0xFF }, // (Offset,Value)-Pair #3

        // D3DResource_AddRef+0x18 : and eax, 0x00070000
        { 0x18, 0x25 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x07 }, // (Offset,Value)-Pair #5

        // D3DResource_AddRef+0x1D : cmp eax, 0x00050000
        { 0x1D, 0x3D }, // (Offset,Value)-Pair #6
        { 0x20, 0x05 }, // (Offset,Value)-Pair #7

        // D3DResource_AddRef+0x2B : push eax
        { 0x2B, 0x50 }, // (Offset,Value)-Pair #8

        // D3DResource_AddRef+0x34 : mov eax, ecx
        { 0x33, 0x8B }, // (Offset,Value)-Pair #9
        { 0x34, 0xC1 }, // (Offset,Value)-Pair #10
OOVPA_END;
*/

// 0x0034CC40
// ******************************************************************
// * D3DResource_Release
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Release_LTCG, 5849, 11)

        // D3DResource_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // D3DResource_Release+0x14 : and eax, 0x00070000
        { 0x14, 0x25 }, // (Offset,Value)-Pair #5
        { 0x17, 0x07 }, // (Offset,Value)-Pair #6

        // D3DResource_Release+0x19 : cmp eax, 0x00050000
        { 0x19, 0x3D }, // (Offset,Value)-Pair #7
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #8

        // D3DResource_Release+0x3D : pop edi
        { 0x3D, 0x5F }, // (Offset,Value)-Pair #9

        // D3DResource_Release+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// Verteks 05/04/2009
// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor (Not sure)
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor_LTCG, 5849, 10)

        // D3DDevice_SetTextureState_ColorKeyColor+0x05 : cmp esi, 0x5C
        { 0x05, 0x83 },
        { 0x06, 0xFE },
        { 0x07, 0x5C },

        // D3DDevice_SetTextureState_ColorKeyColor+0x29 : cmp esi, 0x88
        { 0x29, 0x81 },
        { 0x2A, 0xFE },
        { 0x2B, 0x88 },

        // D3DDevice_SetTextureState_ColorKeyColor+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },

        // D3DDevice_SetTextureState_ColorKeyColor+0x5A : retn 0x08
        { 0x5A, 0xC2 },
        { 0x5B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Unknown1_3
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Unknown1_3_LTCG, 5849, 13)

        // D3DDevice_Unknown1_3+0x04 : cmp eax, 0x0C
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0x0C },

        // D3DDevice_Unknown1_3+0x48 : and ecx, 0xFFFFBFF1
        { 0x48, 0x81 },
        { 0x49, 0xE1 },
        { 0x4A, 0xF1 },
        { 0x4B, 0xBF },
        { 0x4C, 0xFF },
        { 0x4D, 0xFF },

        // D3DDevice_Unknown1_3+0x69 : retn 0x0C
        { 0x69, 0xC2 },
        { 0x6A, 0x0C },

        // D3DDevice_Unknown1_3+0xD7 : retn 0x0C
        { 0xD7, 0xC2 },
        { 0xD8, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF_LARGE(D3DDevice_Clear_LTCG, 5849, 9)
        // D3DDevice_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // D3DDevice_Clear+0x000B : mov ebx, [ebp+0x1A04]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0x04 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x1A }, // (Offset,Value)-Pair #7

        // D3DDevice_Clear+0x03D1 : retn 0x18
        { 0x03D1, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x03D2, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap_LTCG, 5849, 6)

        // D3DDevice_Swap+0x00 : push esi
		{ 0x00, 0x56 },
		// D3DDevice_Swap+0x32 : test ah, 0x40
		{ 0x32, 0xF6 },
		{ 0x33, 0xC4 },
		{ 0x34, 0x40 },
		// D3DDevice_Swap+0xEA : retn 4
		{ 0xEA, 0xC2 },
		{ 0xEB, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader_LTCG, 5849, 10)

		// D3DDevice_SetVertexShader+0x27 : test al, 0x10
		{ 0x27, 0xA8 },
		{ 0x28, 0x10 },
		// D3DDevice_SetVertexShader+0x70 : or eax, 0x400
		{ 0x70, 0x0D },
		{ 0x71, 0x00 },
		{ 0x72, 0x04 },
		{ 0x73, 0x00 },
		{ 0x74, 0x00 },
		// D3DDevice_SetVertexShader+0xCD : add eax, 8
		{ 0xCD, 0x83 },
		{ 0xCE, 0xC0 },
		{ 0xCF, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices_LTCG, 5849, 14)

        // D3DDevice_DrawVertices+0x2B : and ecx, 0x200
		{ 0x2B, 0x81 },
		{ 0x2C, 0xC1 },
		{ 0x2D, 0x00 },
		{ 0x2E, 0x02 },
		{ 0x2F, 0x00 },
		{ 0x30, 0x00 },
		// D3DDevice_DrawVertices+0x6F : cmp edi, 0x100
		{ 0x6F, 0x81 },
		{ 0x70, 0xFF },
		{ 0x71, 0x00 },
		{ 0x72, 0x01 },
		{ 0x73, 0x00 },
		{ 0x74, 0x00 },
		// D3DDevice_DrawVertices+0xCC : retn 0xC
		{ 0xCC, 0xC2 },
		{ 0xCD, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3D8LTCG_5849
// ******************************************************************
OOVPATable D3D8LTCG_5849[] = {

	OOVPA_TABLE_PATCH(D3D_CreateDevice_LTCG, 5849, D3D_CreateDevice),
	OOVPA_TABLE_PATCH(D3DDevice_SetPixelShader_LTCG, 5849, D3DDevice_SetPixelShader),
	OOVPA_TABLE_PATCH(D3DDevice_CreateVertexShader_LTCG, 5849, D3DDevice_CreateVertexShader),
	OOVPA_TABLE_PATCH(D3DDevice_DrawIndexedVertices_LTCG, 5849, D3DDevice_DrawIndexedVertices),
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexShader_LTCG, 5849, D3DDevice_SetVertexShader),
	OOVPA_TABLE_PATCH(D3DDevice_SetTexture_LTCG, 5849, D3DDevice_SetTexture),
	OOVPA_TABLE_PATCH(D3DDevice_SetViewport_LTCG, 5849, D3DDevice_SetViewport),
	OOVPA_TABLE_PATCH(D3DDevice_SetLight_LTCG, 5849, D3DDevice_SetLight),
	OOVPA_TABLE_ENTRY(D3DDevice_SetTransform, 5558),
	OOVPA_TABLE_ENTRY(D3DDevice_SetMaterial, 5849),
	OOVPA_TABLE_PATCH(D3DDevice_SetStreamSource_LTCG, 5849, D3DDevice_SetStreamSource),
	OOVPA_TABLE_ENTRY(D3DDevice_SetVertexShaderConstant1, 5849),
	OOVPA_TABLE_ENTRY(D3DDevice_CreateVertexBuffer2, 5849),
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_TexCoordIndex_LTCG, 5849, D3DDevice_SetTextureState_TexCoordIndex),
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_BumpEnv_LTCG, 5849, D3DDevice_SetTextureState_BumpEnv),
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_BorderColor_LTCG, 5849, D3DDevice_SetTextureState_BorderColor),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_CullMode_LTCG, 5849, D3DDevice_SetRenderState_CullMode),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_TextureFactor_LTCG, 5849, D3DDevice_SetRenderState_TextureFactor),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_NormalizeNormals_LTCG, 5849, D3DDevice_SetRenderState_NormalizeNormals),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ZEnable_LTCG, 5849, D3DDevice_SetRenderState_ZEnable),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_StencilEnable_LTCG, 5849, D3DDevice_SetRenderState_StencilEnable),
	OOVPA_TABLE_PATCH(D3DDevice_Swap_LTCG, 5849, D3DDevice_Swap),
	OOVPA_TABLE_ENTRY(D3DDevice_SetVertexData2f, 4627),
	OOVPA_TABLE_PATCH(D3DDevice_End_LTCG, 5849, D3DDevice_End),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FillMode_LTCG, 5849, D3DDevice_SetRenderState_FillMode),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_FogColor_LTCG, 5849, D3DDevice_SetRenderState_FogColor),
	OOVPA_TABLE_PATCH(D3DDevice_DrawVertices_LTCG, 5849, D3DDevice_DrawVertices),
	OOVPA_TABLE_PATCH(D3DDevice_DrawVerticesUP_LTCG, 5849, D3DDevice_DrawVerticesUP),
	OOVPA_TABLE_ENTRY(D3DDevice_SetVertexShaderConstant4, 4627),
	OOVPA_TABLE_PATCH(D3DVertexBuffer_Lock2_LTCG, 5849, D3DVertexBuffer_Lock2),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_YuvEnable_LTCG, 5849, D3DDevice_SetRenderState_YuvEnable),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_ShadowFunc_LTCG, 5849, D3DDevice_SetRenderState_ShadowFunc),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_OcclusionCullEnable_LTCG, 5849, D3DDevice_SetRenderState_OcclusionCullEnable),
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_EdgeAntiAlias_LTCG, 5849, D3DDevice_SetRenderState_EdgeAntiAlias),
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_LTCG, 5849, Get2DSurfaceDesc),
	OOVPA_TABLE_PATCH(D3DDevice_SetShaderConstantMode_LTCG, 5849, D3DDevice_SetShaderConstantMode),
	OOVPA_TABLE_ENTRY(D3DResource_AddRef, 3925),
	OOVPA_TABLE_PATCH(D3DResource_Release_LTCG, 5849, D3DResource_Release),
	OOVPA_TABLE_PATCH(D3DDevice_SetTextureState_ColorKeyColor_LTCG, 5849, D3DDevice_SetTextureState_ColorKeyColor),
	OOVPA_TABLE_PATCH(D3DDevice_Clear_LTCG, 5849, D3DDevice_Clear),
};

uint32 D3D8LTCG_5849_SIZE = sizeof(D3D8LTCG_5849);
