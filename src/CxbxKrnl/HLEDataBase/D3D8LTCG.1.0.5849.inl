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
// * IDirect3D8_CreateDevice
// ******************************************************************
SOOVPA<8> IDirect3D8_CreateDevice_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x7F : rep stos
        { 0x7F, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x80, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x82 : mov eax, esi
        { 0x82, 0x8B }, // (Offset,Value)-Pair #5
        { 0x83, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x8F : retn 0x18
        { 0x8F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x90, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetPixelShader_1_0_5849_LTCG =
{
    0,  // Large == 0
    9,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3D8_SetPixelShader+0x1E : jnz +0x67
        { 0x1E, 0x75 }, // (Offset,Value)-Pair #1
        { 0x1F, 0x67 }, // (Offset,Value)-Pair #2

        // IDirect3D8_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #3
        { 0x3F, 0x00 }, // (Offset,Value)-Pair #4
        
        // IDirect3D8_SetPixelShader+0x60 : push eax
        { 0x60, 0x50 }, // (Offset,Value)-Pair #5
        
        // IDirect3D8_SetPixelShader+0x61 : shr eax, 1
        { 0x61, 0xD1 }, // (Offset,Value)-Pair #6
        { 0x62, 0xE8 }, // (Offset,Value)-Pair #7
        
        // IDirect3D8_SetPixelShader+0xE1 : jb +0x0E
        { 0xE1, 0x72 }, // (Offset,Value)-Pair #8
        { 0xE2, 0x0E }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CreateVertexShader_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x85 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x8B },
        { 0xDE, 0x73 },
        { 0xFD, 0xC7 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawIndexedVertices_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x13, 0x89 },
        { 0x3E, 0x3B },
        { 0x5E, 0x05 },
        { 0x7E, 0xC3 },
        { 0x9E, 0xC1 },
        { 0xC4, 0x03 },
        { 0xDE, 0xE2 },
        { 0xFD, 0xEA },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetVertexShader_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x89 },
        { 0x3B, 0x33 },
        { 0x5E, 0x83 },
        { 0x80, 0x0A },
        { 0x9E, 0x00 },
        { 0xBE, 0x04 },
        { 0xDE, 0x83 },
        { 0xF4, 0x08 },
    }
};

// Verteks 10/03/2009

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetTexture_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x7C },
        { 0x3C, 0xE8 },
        { 0x5F, 0xE8 },
        { 0x7E, 0x45 },
        { 0x9E, 0x08 },
        { 0xBE, 0x0C },
        { 0xDE, 0x0F },
        { 0xFE, 0x8D },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetViewport_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0xAE },
        { 0x3E, 0x53 },
        { 0x5E, 0x00 },
        { 0x7E, 0x12 },
        { 0x9E, 0x75 },
        { 0xBE, 0x4C },
        { 0xDE, 0xE1 },
        { 0xF9, 0x7D },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetLight_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x00 },
        { 0x3E, 0x89 },
        { 0x5E, 0x00 },
        { 0x7E, 0x07 },
        { 0x9E, 0xC8 },
        { 0xBE, 0xC9 },
        { 0xDE, 0x07 },
        { 0xFD, 0xC1 },
    }
};

/* Same as 5558
// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetTransform_1_0_5849_LTCG = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1D, 0xB9 },
        { 0x3E, 0x0F },
        { 0x5E, 0xD9 },
        { 0x7A, 0x00 },
        { 0x99, 0x83 },
        { 0xB8, 0xE0 },
        { 0xD7, 0xD9 },
        { 0xF6, 0xF6 },
    }
};
*/

/* Same as 5849
// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetMaterial_1_0_5849_LTCG = 
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x00 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
    }
};
*/

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetStreamSource_1_0_5849_LTCG =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetStreamSource+0x07 : mov ebx, [esp+0x0C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x5C }, // (Offset,Value)-Pair #2
        { 0x09, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x0C }, // (Offset,Value)-Pair #4

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
    }
};

/* Same as 5849
// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexShaderConstant1_1_0_5849_LTCG =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1+0x0F : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1+0x1D : mov [eax-0x18], ecx
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #6
        { 0x1E, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1+0x5D : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
    }
};
*/

/* Same as 5849
// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_CreateVertexBuffer2_1_0_5849_LTCG = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x08, 0xE8 },
        { 0x12, 0x1E },
        { 0x1C, 0x50 },
        { 0x26, 0x68 },
        { 0x31, 0x33 },
        { 0x3A, 0xFF },
        { 0x44, 0x01 },
    }
};
*/

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_5849_LTCG =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x14 : shl eax, 0x07
        { 0x14, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x15, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x16, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x26 : cmp eax, ecx
        { 0x26, 0x3B }, // (Offset,Value)-Pair #4
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x7A : mov ebp, 0x2400
        { 0x7A, 0xBD }, // (Offset,Value)-Pair #6
        { 0x7C, 0x24 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xB3 : shl ebx, 0x04
        { 0xB3, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB4, 0xE3 }, // (Offset,Value)-Pair #9
        { 0xB5, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_BumpEnv
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetTextureState_BumpEnv_1_0_5849_LTCG =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetTextureState_BumpEnv+0x18 : jnz +0x03
        { 0x18, 0x75 }, // (Offset,Value)-Pair #1
        { 0x19, 0x03 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x3B : mov ecx, [esp+0x18]
        { 0x3B, 0x8B }, // (Offset,Value)-Pair #6
        { 0x3C, 0x4C }, // (Offset,Value)-Pair #7
        { 0x3D, 0x24 }, // (Offset,Value)-Pair #8
        { 0x3E, 0x18 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x59 : shl esi, 5
        { 0x59, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x5A, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x5B, 0x05 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
SOOVPA<15> IDirect3DDevice8_SetTextureState_BorderColor_1_0_5849_LTCG =
{
    0,  // Large == 0
    15, // Count == 15

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetTextureState_BorderColor+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_BorderColor+0x22 : shl edx, 6
        { 0x22, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x23, 0xE2 }, // (Offset,Value)-Pair #4
        { 0x24, 0x06 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_BorderColor+0x34 : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x34, 0x83 }, // (Offset,Value)-Pair #6
        { 0x35, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x36, 0x08 }, // (Offset,Value)-Pair #8
        { 0x37, 0x89 }, // (Offset,Value)-Pair #9
        { 0x38, 0x06 }, // (Offset,Value)-Pair #10
        { 0x39, 0xC1 }, // (Offset,Value)-Pair #11
        { 0x3A, 0xE1 }, // (Offset,Value)-Pair #12
        { 0x3B, 0x07 }, // (Offset,Value)-Pair #13

        // IDirect3DDevice8_SetTextureState_BorderColor+0x43 : retn 0x08
        { 0x43, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x44, 0x08 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_CullMode_1_0_5849_LTCG =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_DXSRSCULLMODE, // XRef Is Saved
    0,                  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_CullMode+0x22 : mov dword ptr [eax], 0x40308
        { 0x22, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x24, 0x08 }, // (Offset,Value)-Pair #2
        { 0x25, 0x03 }, // (Offset,Value)-Pair #3
        { 0x26, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_CullMode+0x2D : add eax, 8
        { 0x2D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x2E, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x2F, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_CullMode+0x39 : retn 4
        { 0x39, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x3A, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullMode+0x5C : add edx, 0x404
        { 0x5C, 0x81 }, // (Offset,Value)-Pair #10
        { 0x5D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x5E, 0x04 }, // (Offset,Value)-Pair #12
        { 0x5F, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5849_LTCG =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x09, 0x84 },
        { 0x14, 0x46 },
        { 0x28, 0x24 },
        { 0x33, 0xBA },
        { 0x3D, 0x4A },
        { 0x48, 0x5E },
        { 0x55, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x2E : or [addr], 0x200
        { 0x2E, 0x81 }, // (Offset,Value)-Pair #3
        { 0x2F, 0x0D }, // (Offset,Value)-Pair #4
        { 0x34, 0x00 }, // (Offset,Value)-Pair #5
        { 0x35, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x40, 0x04 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_ZEnable_1_0_5849_LTCG = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0C, 0x72 },
        { 0x2B, 0xC9 },
        { 0x3D, 0x89 },
        { 0x4F, 0x0C },
        { 0x63, 0x74 },
        { 0x74, 0x8B },
        { 0x8A, 0x8B },
        { 0x97, 0x06 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_StencilEnable_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used
    {
        { 0x12, 0x8B },
        { 0x2D, 0x33 },
        { 0x40, 0x74 },
        { 0x4A, 0x74 },
        { 0x63, 0x89 },
        { 0x79, 0xB9 },
        { 0x92, 0x33 },
        { 0x9F, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_Swap_1_0_5849_LTCG =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0xEB },
        { 0x93, 0x85 },
        { 0xAA, 0xC6 },
        { 0xC9, 0x0A },
    }
};

/* Same as 4627
// ******************************************************************
// * IDirect3DDevice8_SetVertexData2f
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetVertexData2f_1_0_5849_LTCG =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexData2f+0x20 : lea edx, ds:abs[ecx*8]
        { 0x20, 0x8D }, // (Offset,Value)-Pair #1
        { 0x21, 0x14 }, // (Offset,Value)-Pair #2
        { 0x22, 0xCD }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexData2f+0x2B : mov [eax], edx; mov edx, [esp+0x10]
        { 0x2B, 0x89 }, // (Offset,Value)-Pair #4
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #6
        { 0x2E, 0x54 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData2f+0x37 : add eax, 0x0C
        { 0x37, 0x83 }, // (Offset,Value)-Pair #10
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_SetVertexData2f+0x3D : retn 0x0C
        { 0x3D, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3E, 0x0C }, // (Offset,Value)-Pair #14
    }
};
*/

// ******************************************************************
// * IDirect3DDevice8_End
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_End_1_0_5849_LTCG =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x07, 0x8B },
        { 0x13, 0x50 },
        { 0x1C, 0xC7 },
        { 0x22, 0xC7 },
        { 0x2B, 0x08 },
        { 0x34, 0xE1 },
        { 0x49, 0xC3 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FillMode_1_0_5849_LTCG =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_FillMode+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_FillMode+0x2A : mov ecx, [esp+0x08]
        { 0x2A, 0x8B }, // (Offset,Value)-Pair #3
        { 0x2B, 0x4C }, // (Offset,Value)-Pair #4
        { 0x2C, 0x24 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_FillMode+0x2E : jnz +0x02
        { 0x2E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x02 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_FillMode+0x3E : add eax, 0x0C
        { 0x3E, 0x83 }, // (Offset,Value)-Pair #9
        { 0x3F, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x40, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_FogColor_1_0_5849_LTCG =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_FogColor+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_FogColor+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #3
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_FogColor+0x2E : shl edi, 0x10
        { 0x2E, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x2F, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x30, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_FogColor+0x49 : mov [esi], eax
        { 0x49, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x06 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_DrawVertices_1_0_5849_LTCG = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x11, 0x8B },
        { 0x26, 0x46 },
        { 0x3C, 0xD1 },
        { 0x45, 0x3B },
        { 0x5D, 0x4C },
        { 0x70, 0xFF },
        { 0x83, 0xC1 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawVerticesUP_1_0_5849_LTCG = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0xA1 },
        { 0x3E, 0x8B },
        { 0x5E, 0x08 },
        { 0x7E, 0xD0 },
        { 0x9E, 0x14 },
        { 0xBE, 0x8D },
        { 0xDE, 0x3B },
        { 0xFE, 0x81 },
    }
};

// Verteks 11/03/2009

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
SOOVPA<7> IDirect3DVertexBuffer8_Lock2_1_0_5849_LTCG = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x08, 0x56 },
        { 0x12, 0x06 },
        { 0x26, 0xC7 },
        { 0x2F, 0x00 },
        { 0x39, 0xC3 },
        { 0x68, 0x80 },
        { 0x7A, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_YuvEnable_1_0_5849_LTCG =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x10 : mov eax, [esi]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #5
        { 0x11, 0x06 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x12 : cmp eax, [esi+4]
        { 0x12, 0x3B }, // (Offset,Value)-Pair #7
        { 0x13, 0x46 }, // (Offset,Value)-Pair #8
        { 0x14, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x15 : jb +0x0E
        { 0x15, 0x72 }, // (Offset,Value)-Pair #10
        { 0x16, 0x0E }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x30 : retn 0x04
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x31, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_5849_LTCG =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #5
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x26 : lea edx, [ecx-0x0200]
        { 0x26, 0x8D }, // (Offset,Value)-Pair #7
        { 0x27, 0x91 }, // (Offset,Value)-Pair #8
        { 0x28, 0x00 }, // (Offset,Value)-Pair #9
        { 0x29, 0xFE }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x2F : add eax, 0x08
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #11
        { 0x30, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x31, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_5849_LTCG =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x15 : jb +0x0E
        { 0x15, 0x72 }, // (Offset,Value)-Pair #1
        { 0x16, 0x0E }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x4A : cmp [abs], 0x1E00
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #3
        { 0x50, 0x00 }, // (Offset,Value)-Pair #4
        { 0x51, 0x1E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x59 : mov dword ptr [eax], 0x41D84
        { 0x59, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x5A, 0x00 }, // (Offset,Value)-Pair #7
        { 0x5B, 0x84 }, // (Offset,Value)-Pair #8
        { 0x5C, 0x1D }, // (Offset,Value)-Pair #9
        { 0x5D, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x68 : retn 0x04
        { 0x68, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x69, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_5849_LTCG =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x1C : mov ecx, [esp+0x08]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x1D, 0x4C }, // (Offset,Value)-Pair #6
        { 0x1E, 0x24 }, // (Offset,Value)-Pair #7
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x38 : retn 0x04
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// I am not sure about this
// ******************************************************************
// * IDirect3DTexture8_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
SOOVPA<9> Get2DSurfaceDesc_1_0_5849_LTCG =
{
    0,  // Large == 0
    9, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetShaderConstantMode_1_0_5849_LTCG =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x19, 0xFD },
        { 0x38, 0x8B },
        { 0x60, 0x0B },
        { 0x7A, 0xBA },
        { 0xA4, 0xC0 },
        { 0xB2, 0x00 },
        { 0xDA, 0x89 },
        { 0xF9, 0x00 },
    }
};

/*
I don't know which one of these is the right one :
0x0034E040 2 args
0x0034E200 2 args
0x0034F150 3 args
// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetIndices_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
    }
};
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
0034D75C  |. E8 9FF4FFFF              call    default.0034CC00             <- IDirect3DResource8_AddRef
0034D761  |> FF01                     inc     dword ptr ds:[ecx]
0034D763  |> 8BC1                     mov     eax, ecx
0034D765  \. C3                       retn
// ******************************************************************
// * IDirect3DResource8_AddRef
// ******************************************************************
SOOVPA<10> IDirect3DResource8_AddRef_1_0_5849_LTCG =
{
    0,  // Large == 0
    10, // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DResource8_AddRef+0x11 : test eax, 0xFFFF
        { 0x11, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x12, 0xFF }, // (Offset,Value)-Pair #2
        { 0x13, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DResource8_AddRef+0x18 : and eax, 0x00070000
        { 0x18, 0x25 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DResource8_AddRef+0x1D : cmp eax, 0x00050000
        { 0x1D, 0x3D }, // (Offset,Value)-Pair #6
        { 0x20, 0x05 }, // (Offset,Value)-Pair #7

        // IDirect3DResource8_AddRef+0x2B : push eax
        { 0x2B, 0x50 }, // (Offset,Value)-Pair #8

        // IDirect3DResource8_AddRef+0x34 : mov eax, ecx
        { 0x33, 0x8B }, // (Offset,Value)-Pair #9
        { 0x34, 0xC1 }, // (Offset,Value)-Pair #10
    }
};
*/

// 0x0034CC40
// ******************************************************************
// * IDirect3DResource8_Release
// ******************************************************************
SOOVPA<11> IDirect3DResource8_Release_1_0_5849_LTCG =
{
    0,  // Large == 0
    11, // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DResource8_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_Release+0x14 : and eax, 0x00070000
        { 0x14, 0x25 }, // (Offset,Value)-Pair #5
        { 0x17, 0x07 }, // (Offset,Value)-Pair #6

        // IDirect3DResource8_Release+0x19 : cmp eax, 0x00050000
        { 0x19, 0x3D }, // (Offset,Value)-Pair #7
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #8

        // IDirect3DResource8_Release+0x3D : pop edi
        { 0x3D, 0x5F }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Release+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// Verteks 05/04/2009
// ******************************************************************
// * IDirect3DDevice8_SetTextureState_ColorKeyColor (Not sure)
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_5849_LTCG =
{
    0,  // Large == 0
    10, // Count == 10
    
    -1, // XRef Not Saved
    0,  // XRef Not Used
    
    {
        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x05 : cmp esi, 0x5C
        { 0x05, 0x83 },
        { 0x06, 0xFE },
        { 0x07, 0x5C },
        
        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x29 : cmp esi, 0x88
        { 0x29, 0x81 },
        { 0x2A, 0xFE },
        { 0x2B, 0x88 },
        
        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x4A : retn 0x08
        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
        
        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x5A : retn 0x08
        { 0x5A, 0xC2 },
        { 0x5B, 0x08 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Unknown1_3
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_Unknown1_3_1_0_5849_LTCG =
{
    0,  // Large == 0
    13, // Count == 13
    
    -1, // XRef Not Saved
    0,  // XRef Not Used
    
    {
        // IDirect3DDevice8_Unknown1_3+0x04 : cmp eax, 0x0C
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0x0C },
        
        // IDirect3DDevice8_Unknown1_3+0x48 : and ecx, 0xFFFFBFF1
        { 0x48, 0x81 },
        { 0x49, 0xE1 },
        { 0x4A, 0xF1 },
        { 0x4B, 0xBF },
        { 0x4C, 0xFF },
        { 0x4D, 0xFF },
        
        // IDirect3DDevice8_Unknown1_3+0x69 : retn 0x0C
        { 0x69, 0xC2 },
        { 0x6A, 0x0C },
        
        // IDirect3DDevice8_Unknown1_3+0xD7 : retn 0x0C
        { 0xD7, 0xC2 },
        { 0xD8, 0x0C },
    }
};

// ******************************************************************
// * D3D8LTCG_1_0_5849
// ******************************************************************
OOVPATable D3D8LTCG_1_0_5849[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_5849_LTCG,
        
        XTL::EmuIDirect3D8_CreateDevice,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
    // IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetPixelShader,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_CreateVertexShader,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetVertexShader,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetTexture,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetViewport,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
    // IDirect3DDevice8::SetLight
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetLight,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::SetTransform (* unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_5558,
        
        XTL::EmuIDirect3DDevice8_SetTransform,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
        #endif
    },
    // IDirect3DDevice8::SetMaterial (* unchanged since 5849 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_5849,
        
        XTL::EmuIDirect3DDevice8_SetMaterial,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetStreamSource,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant1 (* unchanged since 5849 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant1_1_0_5849,
        
        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant1"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer2 (* unchanged since 5849 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer2_1_0_5849,
        
        XTL::EmuIDirect3DDevice8_CreateVertexBuffer2,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer2"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BumpEnv
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BumpEnv_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BumpEnv"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BorderColor
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BorderColor_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BorderColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_NormalizeNormals
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_Swap,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
    // IDirect3DDevice8::SetVertexData2f (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData2f_1_0_4627,
        
        XTL::EmuIDirect3DDevice8_SetVertexData2f,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2f"
        #endif
    },
    // IDirect3DDevice8::End
    {
        (OOVPA*)&IDirect3DDevice8_End_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_End,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
    // IDirect3DDevice8::DrawVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_DrawVertices,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3DDevice8::DrawVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawVerticesUP_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_DrawVerticesUP,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant4 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627,
        
        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant4"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock2
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock2_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DVertexBuffer8_Lock2,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock2"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_YuvEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_YuvEnable_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ShadowFunc
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_OcclusionCullEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_EdgeAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias"
        #endif
    },
    // IDirect3DTexture8::GetLevelDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_5849_LTCG,
        
        XTL::EmuGet2DSurfaceDesc,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetLevelDesc"
        #endif
    },
    // IDirect3DDevice8::SetShaderConstantMode
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
    // IDirect3DResource8::AddRef
    {
        (OOVPA*)&IDirect3DResource8_AddRef_1_0_3925,
        
        XTL::EmuIDirect3DResource8_AddRef,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_AddRef"
        #endif
    },
    // IDirect3DResource8::Release
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DResource8_Release,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release"
        #endif
    },
    
    // IDirect3DDevice8::SetTextureState_ColorKeyColor
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },

    // IDirect3DDevice8::Unknown1_3
    /*{
        (OOVPA*)&IDirect3DDevice8_Unknown1_3_1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_Unknown1_3,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Unknown1_3"
        #endif
    },*/

    /*
    // IDirect3DDevice8::
    {
        (OOVPA*)&IDirect3DDevice8__1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_"
        #endif
    },
    */
    /*
    // IDirect3DDevice8::
    {
        (OOVPA*)&IDirect3DDevice8__1_0_5849_LTCG,
        
        XTL::EmuIDirect3DDevice8_,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_"
        #endif
    },
    */
};

uint32 D3D8LTCG_1_0_5849_SIZE = sizeof(D3D8LTCG_1_0_5849);