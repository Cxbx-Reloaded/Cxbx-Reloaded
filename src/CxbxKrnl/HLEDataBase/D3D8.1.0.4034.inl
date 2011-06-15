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
SOOVPA<7> IDirect3DDevice8_GetGammaRamp_1_0_4034 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x8B },
        { 0x0A, 0x00 },
        { 0x10, 0xE1 },
        { 0x16, 0x0C },
        { 0x1C, 0x00 },
        { 0x22, 0x00 },
        { 0x28, 0x04 },
    }
};


// ******************************************************************
// * IDirect3DDevice8_GetCreationParameters
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_GetCreationParameters_1_0_4034 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x06, 0x8B },
        { 0x0E, 0x8B },
        { 0x16, 0x4E },
        { 0x1E, 0x83 },
        { 0x26, 0x00 },
        { 0x2E, 0x5E },
        { 0x36, 0xC9 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetViewport_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetViewport_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTransform_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetRenderState_CullMode_1_0_4034 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_DXSRSCULLMODE, // XRef Is Saved
    0,                  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
SOOVPA<7> D3DDevice_BeginVisibilityTest_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0A, 0x46 },
        { 0x13, 0xC7 },
        { 0x16, 0x17 },
        { 0x1C, 0x00 },
        { 0x22, 0x48 },
        { 0x28, 0x06 },
    }
};

// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
SOOVPA<7> D3DDevice_EndVisibilityTest_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0B, 0x8B },
        { 0x16, 0x5E },
        { 0x22, 0x07 },
        { 0x2E, 0x00 },
        { 0x3A, 0x81 },
        { 0x46, 0x89 },
        { 0x55, 0x5F },
    }
};

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
SOOVPA<8> D3DDevice_CopyRects_1_0_4034 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x57 },
        { 0x3E, 0xF6 },
        { 0x5E, 0xEB },
        { 0x7E, 0x00 },
        { 0x9E, 0x10 },
        { 0xBE, 0x24 },
        { 0xDE, 0x22 },
        { 0xFE, 0x14 },
    }
};

// ******************************************************************
// * D3DDevice_DeletePatch
// ******************************************************************
SOOVPA<7> D3DDevice_DeletePatch_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x0D },
        { 0x0C, 0xA1 },
        { 0x13, 0x34 },
        { 0x1A, 0x4E },
        { 0x21, 0x00 },
        { 0x2A, 0x56 },
        { 0x30, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_DrawIndexedVertices_1_0_4034 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetStreamSource_1_0_4034 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
SOOVPA<7> D3DDevice_SetTextureState_BorderColor_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x24 },
        { 0x26, 0x24 },
        { 0x2E, 0x89 },
        { 0x39, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
SOOVPA<7> D3DDevice_SetTextureState_ColorKeyColor_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x89 },
        { 0x26, 0x04 },
        { 0x2E, 0x07 },
        { 0x36, 0xC2 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_DrawVertices_1_0_4034 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_LoadVertexShader_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_DrawRectPatch
// ******************************************************************
SOOVPA<8> D3DDevice_DrawRectPatch_1_0_4034 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x6C },
        { 0x3E, 0x3C },
        { 0x5E, 0x54 },
        { 0x7F, 0x89 },
        { 0x9E, 0x00 },
        { 0xBE, 0xD9 },
        { 0xDE, 0xD9 },
        { 0xFE, 0x86 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetIndices_1_0_4034 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_EdgeAntiAlias_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x03 },
        { 0x21, 0x48 },
        { 0x28, 0x89 },
        { 0x2F, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_FillMode_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x13, 0x8B },
        { 0x19, 0x8B },
        { 0x22, 0x4C },
        { 0x2B, 0x8C },
        { 0x34, 0x08 },
        { 0x40, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
SOOVPA<8> D3DDevice_SetRenderState_BackFillMode_1_0_4034 = 
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x00, 0x8B },
        { 0x0B, 0xA3 },
        { 0x14, 0x04 },
        { 0x23, 0x8B },
        { 0x2A, 0xC9 },
        { 0x35, 0xC7 },
        { 0x40, 0x08 },
        { 0x4C, 0x5F },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_FogColor_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x0E },
        { 0x26, 0xE7 },
        { 0x30, 0x00 },
        { 0x3A, 0x89 },
        { 0x44, 0x0D },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
SOOVPA<6> D3DDevice_SetRenderState_FrontFace_1_0_4034 = 
{
    0,  // Large == 0
    6,  // Count == 6

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x2A, 0x89 },
        { 0x30, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_LogicOp_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0xC1 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_MultiSampleMask_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0B, 0x8B },
        { 0x17, 0x8B },
        { 0x23, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0x83 },
        { 0x40, 0x7C },
        { 0x4B, 0x08 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_NormalizeNormals_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0E, 0xE8 },
        { 0x16, 0x08 },
        { 0x1E, 0x48 },
        { 0x26, 0x0D },
        { 0x2E, 0x00 },
        { 0x36, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_ShadowFunc_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0C, 0x72 },
        { 0x13, 0x8B },
        { 0x1A, 0x1E },
        { 0x21, 0xFF },
        { 0x28, 0x08 },
        { 0x31, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_Simple_1_0_4034 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_StencilEnable_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x13, 0x8B },
        { 0x20, 0x02 },
        { 0x34, 0x85 },
        { 0x42, 0x75 },
        { 0x53, 0x89 },
        { 0x64, 0x00 },
        { 0x75, 0x83 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_StencilFail_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0C, 0x72 },
        { 0x1A, 0xC9 },
        { 0x2A, 0x85 },
        { 0x36, 0x74 },
        { 0x44, 0x83 },
        { 0x52, 0x24 },
        { 0x60, 0x10 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_YuvEnable_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x04, 0x56 },
        { 0x0B, 0xA3 },
        { 0x10, 0x8B },
        { 0x16, 0x05 },
        { 0x1C, 0x8B },
        { 0x25, 0x89 },
        { 0x28, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_TwoSidedLighting_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x46 },
        { 0x35, 0x8B },
        { 0x43, 0x75 },
        { 0x4C, 0x00 },
        { 0x59, 0x3D },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_VertexBlend_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x07, 0xCA },
        { 0x13, 0x89 },
        { 0x19, 0x8B },
        { 0x25, 0x8B },
        { 0x2B, 0x28 },
        { 0x34, 0x08 },
        { 0x3D, 0x5E },
    }
};

// ******************************************************************
// * Direct3D_CheckDeviceFormat
// ******************************************************************
SOOVPA<7> Direct3D_CheckDeviceFormat_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0E, 0x18 },
        { 0x1E, 0x00 },
        { 0x2E, 0x42 },
        { 0x41, 0x40 },
        { 0x4E, 0x74 },
        { 0x5E, 0x08 },
        { 0x6E, 0x3C },
    }
};

// ******************************************************************
// * Direct3D_SetPushBufferSize
// ******************************************************************
SOOVPA<7> Direct3D_SetPushBufferSize_1_0_4034 = 
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0D, 0x89 },
        { 0x0E, 0x0D },
        { 0x13, 0xC2 },
        { 0x14, 0x08 },
    }
};

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
/*SOOVPA<10> Get2DSurfaceDesc_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
};*/

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_4034 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_LightEnable_1_0_4034 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4034 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
SOOVPA<10> Get2DSurfaceDesc_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
LOOVPA<9> IDirect3DDevice8_Clear_1_0_4034 =
{
    1,  // Large == 1
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetLight_1_0_4034 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
SOOVPA<11> IDirect3DVertexBuffer8_Lock_1_0_4034 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTexture_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetPalette_1_0_4034 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4034 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetMaterial_1_0_4034 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetVertexShader_1_0_4034 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
SOOVPA<11> D3DDevice_Swap_1_0_4034 = 
{
    0,  // Large == 0
    11, // Count == 11

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
SOOVPA<9> IDirect3D8_KickOffAndWaitForIdle_1_0_4034 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateImageSurface
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_CreateImageSurface_1_0_4034 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetBackBuffer_1_0_4034 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3D8_1_0_4034
// ******************************************************************
OOVPATable D3D8_1_0_4034[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 1.0.3925 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
	// IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_4034,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
        #endif
    },

    // ********************** BEG WARNING UNTESTED!!! *******************

#if 0
    // IDirect3DDevice8::BeginVisibilityTest
    {
        (OOVPA*)&IDirect3DDevice8_BeginVisibilityTest_1_0_3925,

        XTL::EmuIDirect3DDevice8_BeginVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginVisibilityTest"
        #endif
    },
    // IDirect3DDevice8::EndVisibilityTest
    {
        (OOVPA*)&IDirect3DDevice8_EndVisibilityTest_1_0_3925,

        XTL::EmuIDirect3DDevice8_EndVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndVisibilityTest"
        #endif
    },
    // IDirect3DDevice8::LoadVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader"
        #endif
    },
    // IDirect3DDevice8::SelectVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SelectVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SelectVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShader"
        #endif
    },
    // IDirect3DDevice8::Release
    {
        (OOVPA*)&IDirect3DDevice8_Release_1_0_3925,

        XTL::EmuIDirect3DDevice8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Release"
        #endif
    },
    // IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_3925,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },
    // IDirect3DDevice8::SetVerticalBlankCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetVerticalBlankCallback_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVerticalBlankCallback"
        #endif
    },
    // IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget"
        #endif
    },
    // IDirect3DDevice8::GetTransform
    {
        (OOVPA*)&IDirect3DDevice8_GetTransform_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTransform"
        #endif
    },
    // IDirect3DDevice8::SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
    // IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
        #endif
    },
    // IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_3925,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::Begin
    {
        (OOVPA*)&IDirect3DDevice8_Begin_1_0_3925,

        XTL::EmuIDirect3DDevice8_Begin,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Begin"
        #endif
    },
    // IDirect3DDevice8::SetVertexData2f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData2f_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData2f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2f"
        #endif
    },
    // IDirect3DDevice8::SetVertexData4f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData4f_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData4f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4f"
        #endif
    },
    // IDirect3DDevice8::SetVertexDataColor
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexDataColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexDataColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexDataColor"
        #endif
    },
    // IDirect3DDevice8::End
    {
        (OOVPA*)&IDirect3DDevice8_End_1_0_3925,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_3925,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
        #endif
    },
    // IDirect3DDevice8::CreatePalette
    {
        (OOVPA*)&IDirect3DDevice8_CreatePalette_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreatePalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePalette"
        #endif
    },
    // IDirect3DDevice8::SetPalette
    {
        (OOVPA*)&IDirect3DDevice8_SetPalette_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant"
        #endif
    },
    // IDirect3DDevice8::SetFlickerFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetFlickerFilter_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetFlickerFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetFlickerFilter"
        #endif
    },
    // IDirect3DDevice8::SetSoftDisplayFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetSoftDisplayFilter_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSoftDisplayFilter"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BorderColor
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BorderColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BorderColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_PSTextureModes
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_PSTextureModes_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_PSTextureModes,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_PSTextureModes"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilFail
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    /*{
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
        #endif
    },*/
    // IDirect3DDevice8::SetRenderState_Simple
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
    // IDirect3DDevice8::Present
    {
        (OOVPA*)&IDirect3DDevice8_Present_1_0_3925,

        XTL::EmuIDirect3DDevice8_Present,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Present"
        #endif
    },
    // IDirect3DDevice8::SetShaderConstantMode
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_3925,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },
    // IDirect3DResource8::Register
    {
        (OOVPA*)&IDirect3DResource8_Register_1_0_3925,

        XTL::EmuIDirect3DResource8_Register,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Register"
        #endif
    },
    // IDirect3DResource8::IsBusy
    {
        (OOVPA*)&IDirect3DResource8_IsBusy_1_0_3925,

        XTL::EmuIDirect3DResource8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_IsBusy"
        #endif
    },
    // Get2DSurfacDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_3925,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },

    // ********************** END WARNING UNTESTED!!! *******************
#endif

	// IDirect3DDevice8::GetGammaRamp
    {
        (OOVPA*)&IDirect3DDevice8_GetGammaRamp_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetGammaRamp"
        #endif
    },
	// IDirect3DDevice8::GetCreationParameters
    {
        (OOVPA*)&IDirect3DDevice8_GetCreationParameters_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetCreationParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetCreationParameters"
        #endif
    },
	// IDirect3DDevice8::GetViewport
    {
        (OOVPA*)&IDirect3DDevice8_GetViewport_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetViewport"
        #endif
    },
	// IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
	// IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
        #endif
    },
	// IDirect3DDevice8::CreatePixelShader (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
        #endif
    },
	// D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
    {
        (OOVPA*)&X_D3DDevice_GetVertexShaderSize_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetVertexShaderSize,

        #ifdef _DEBUG_TRACE
        "D3DDevice_GetVertexShaderSize"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
        #endif
    },
	// IDirect3DDevice8::CreateCubeTexture (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateCubeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateCubeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateCubeTexture"
        #endif
    },
    // IDirect3DDevice8::GetDeviceCaps (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetDeviceCaps_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetDeviceCaps,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDeviceCaps"
        #endif
    },
	// IDirect3DDevice8::CreatePalette (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreatePalette_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreatePalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePalette"
        #endif
    },
    // IDirect3DDevice8::CreateTexture (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture"
        #endif
    },
    // IDirect3DDevice8::CreateVolumeTexture (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVolumeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVolumeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVolumeTexture"
        #endif
    },
    // IDirect3DDevice8::CreateIndexBuffer (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateIndexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateIndexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateIndexBuffer"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZBias_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZBias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZBias"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer"
        #endif
    },
    // IDirect3DResource8::Release (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_3925,

        XTL::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release"
        #endif
    },
    // IDirect3DResource8::AddRef (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DResource8_AddRef_1_0_3925,

        XTL::EmuIDirect3DResource8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_AddRef"
        #endif
    },
    // IDirect3DPalette8::Lock (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DPalette8_Lock_1_0_3925,

        XTL::EmuIDirect3DPalette8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DPalette8_Lock"
        #endif
    },
    // IDirect3DTexture8::LockRect (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_LockRect"
        #endif
    },
    // IDirect3DVolumeTexture8::LockBox (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DVolumeTexture8_LockBox_1_0_3925,

        XTL::EmuIDirect3DVolumeTexture8_LockBox,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVolumeTexture8_LockBox"
        #endif
    },
    // IDirect3DCubeTexture8::LockRect (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DCubeTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DCubeTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DCubeTexture8_LockRect"
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_3925,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel"
        #endif
    },
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_3925,

        XTL::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect"
        #endif
    },
	// Lock2DSurface (* unchanged since 3925 *)
    {
        (OOVPA*)&Lock2DSurface_1_0_3925,

        XTL::EmuLock2DSurface,

        #ifdef _DEBUG_TRACE
        "EmuLock2DSurface"
        #endif
    },
	// IDirect3DDevice8_SetGammaRamp (* unchanged since 3925 *)
	{
		(OOVPA*)&IDirect3DDevice8_SetGammaRamp_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetGammaRamp,

		#ifdef _DEBUG_TRACE
		"IDirect3DDevice8_SetGammaRamp"
		#endif
	},
	// IDirect3DDevice8::DrawVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_4034,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
	// IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_4034,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
	// D3DDevice_BeginVisibilityTest
    {
        (OOVPA*)&D3DDevice_BeginVisibilityTest_1_0_4034,
        XTL::EmuIDirect3DDevice8_BeginVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginVisibilityTest"
        #endif
    },
    // D3DDevice_EndVisibilityTest
    {
        (OOVPA*)&D3DDevice_EndVisibilityTest_1_0_4034,
        XTL::EmuIDirect3DDevice8_EndVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndVisibilityTest"
        #endif
    },
	// D3DDevice_CopyRects
    {
        (OOVPA*)&D3DDevice_CopyRects_1_0_4034,
        XTL::EmuIDirect3DDevice8_CopyRects,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CopyRects"
        #endif
    },
	// D3DDevice_DeletePatch (TODO)
    /*{
        (OOVPA*)&D3DDevice_DeletePatch_1_0_4034,
        XTL::EmuIDirect3DDevice8_DeletePatch,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeletePatch"
        #endif
    },*/
	// D3DDevice_SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
	// D3DDevice_SetTextureState_BorderColor
    {
        (OOVPA*)&D3DDevice_SetTextureState_BorderColor_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BorderColor"
        #endif
    },
	// D3DDevice_SetTextureState_ColorKeyColor
    {
        (OOVPA*)&D3DDevice_SetTextureState_ColorKeyColor_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
	// D3DDevice_LoadVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_4034,
        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader"
        #endif
    },
	// D3DDevice_DrawRectPatch
    {
        (OOVPA*)&D3DDevice_DrawRectPatch_1_0_4034,
        XTL::EmuIDirect3DDevice8_DrawRectPatch,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawRectPatch"
        #endif
    },
	// D3DDevice_SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
        #endif
    },
	// D3DDevice_SetRenderState_EdgeAntiAlias
    {
        (OOVPA*)&D3DDevice_SetRenderState_EdgeAntiAlias_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias"
        #endif
    },
	// D3DDevice_SetRenderState_FillMode
    {
        (OOVPA*)&D3DDevice_SetRenderState_FillMode_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },
	// D3DDevice_SetRenderState_BackFillMode
    {
        (OOVPA*)&D3DDevice_SetRenderState_BackFillMode_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_BackFillMode"
        #endif
    },
	// D3DDevice_SetRenderState_FogColor
    {
        (OOVPA*)&D3DDevice_SetRenderState_FogColor_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
	// D3DDevice_SetRenderState_FrontFace
    {
        (OOVPA*)&D3DDevice_SetRenderState_FrontFace_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FrontFace"
        #endif
    },
	// D3DDevice_SetRenderState_LogicOp
    {
        (OOVPA*)&D3DDevice_SetRenderState_LogicOp_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LogicOp"
        #endif
    },
	// D3DDevice_SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
    },
	// D3DDevice_SetRenderState_MultiSampleMask
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleMask_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMask"
        #endif
    },
	// D3DDevice_SetRenderState_NormalizeNormals
    {
        (OOVPA*)&D3DDevice_SetRenderState_NormalizeNormals_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
        #endif
    },
	// D3DDevice_SetRenderState_ShadowFunc
    {
        (OOVPA*)&D3DDevice_SetRenderState_ShadowFunc_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
	// D3DDevice_SetRenderState_Simple
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple"
        #endif
    },
	// D3DDevice_SetRenderState_StencilEnable
    {
        (OOVPA*)&D3DDevice_SetRenderState_StencilEnable_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
	// D3DDevice_SetRenderState_StencilFail
    {
        (OOVPA*)&D3DDevice_SetRenderState_StencilFail_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
        #endif
    },
	// D3DDevice_SetRenderState_YuvEnable
    {
        (OOVPA*)&D3DDevice_SetRenderState_YuvEnable_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
	// D3DDevice_SetRenderState_TwoSidedLighting
    {
        (OOVPA*)&D3DDevice_SetRenderState_TwoSidedLighting_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TwoSidedLighting"
        #endif
    },
	// D3DDevice_SetRenderState_VertexBlend
    {
        (OOVPA*)&D3DDevice_SetRenderState_VertexBlend_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
	// IDirect3D8_CheckDeviceFormat
    {
        (OOVPA*)&Direct3D_CheckDeviceFormat_1_0_4034,
        XTL::EmuIDirect3D8_CheckDeviceFormat,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CheckDeviceFormat"
        #endif
    },
	// IDirect3D8_SetPushBufferSize
    {
        (OOVPA*)&Direct3D_SetPushBufferSize_1_0_4034,
        XTL::EmuIDirect3D8_SetPushBufferSize,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_SetPushBufferSize"
        #endif
    },
	// Get2DSurfacDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_4034,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },
	// IDirect3DTexture8::GetSurfaceLevel (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_3925,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel"
        #endif
    },
	// D3DDevice_SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
	// D3DDevice_LightEnable
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_4034,
        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
	// D3DDevice_SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4034,
        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
        #endif
    },
	// Direct3D_GetAdapterIdentifier (* unchanged since 3925 *)
	{
		(OOVPA*)&Direct3D_GetAdapterIdentifier_1_0_3925,
		XTL::EmuIDirect3D8_GetAdapterIdentifier,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3D8_GetAdapterIdentifier"
		#endif
    },
	// IDirect3DSurface8::GetDesc (* unchanged since 3925 *)
    {
        (OOVPA*)&D3DSurface_GetDesc_1_0_3925,

        XTL::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc"
        #endif
    },
	// Get2DSurfacDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_4034,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },
	// IDirect3DDevice8::SetLight
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
	// IDirect3DVertexBuffer8::Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_4034,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },
	// IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
	// IDirect3DDevice8::SetPalette
    {
        (OOVPA*)&IDirect3DDevice8_SetPalette_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette" 
        #endif
    },
	// IDirect3DDevice8::SetRenderState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
        #endif
    },
	// IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
	// IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
	// IDirect3DDevice8::Swap
    {
        (OOVPA*)&D3DDevice_Swap_1_0_4034,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
	// IDirect3D8::KickOffAndWaitForIdle
    {
        (OOVPA*)&IDirect3D8_KickOffAndWaitForIdle_1_0_4034,

        XTL::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle"
        #endif
    },
	// IDirect3DDevice8::CreateImageSurface (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateImageSurface_1_0_4034,

        XTL::EmuIDirect3DDevice8_CreateImageSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateImageSurface"
        #endif
    },
	// IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
	// D3DDevice_
    /*{
        (OOVPA*)&D3DDevice__1_0_4034,
        XTL::EmuIDirect3DDevice8_,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_"
        #endif
    },
	// D3DDevice_
    {
        (OOVPA*)&D3DDevice__1_0_4034,
        XTL::EmuIDirect3DDevice8_,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_"
        #endif
    },
	// D3DDevice_
    {
        (OOVPA*)&D3DDevice__1_0_4034,
        XTL::EmuIDirect3DDevice8_,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_"
        #endif
    },*/
};

// ******************************************************************
// * D3D8_1_0_4034_SIZE
// ******************************************************************
uint32 D3D8_1_0_4034_SIZE = sizeof(D3D8_1_0_4034);
