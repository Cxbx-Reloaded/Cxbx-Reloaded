// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5849.cpp
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
// * IDirect3DDevice8_EndVisibilityTest
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_EndVisibilityTest_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x10, 0x09 },
        { 0x12, 0x0E },
        { 0x18, 0x04 },
        { 0x34, 0xCC },
        { 0x3D, 0x0E },
        { 0x50, 0x0C },
        { 0x58, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetVisibilityTestResult
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_GetVisibilityTestResult_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x0E, 0x08 },
        { 0x10, 0xFF },
        { 0x2A, 0x16 },
        { 0x3C, 0x76 },
        { 0x3F, 0x0C },
        { 0x51, 0x0A },
        { 0x5F, 0x0C },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_GetViewport_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x09, 0x05 },
        { 0x14, 0x04 },
        { 0x1D, 0x08 },
        { 0x20, 0x0C },
        { 0x26, 0x10 },
        { 0x2B, 0x40 },
        { 0x2F, 0x14 },
        { 0x31, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetBackBufferScale
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetBackBufferScale_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x24 },
        { 0x3E, 0x06 },
        { 0x62, 0xD9 },
        { 0x7E, 0xC4 },
        { 0x9E, 0x81 },
        { 0xBE, 0xFF },
        { 0xDE, 0xC0 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetBackBuffer2_1_0_5849 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer2+0x18 : mov esi, [ecx+eax*4+0x21C0]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0x14 }, // (Offset,Value)-Pair #9
        { 0x19, 0x1A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetDepthStencilSurface2_1_0_5849 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_GetDepthStencilSurface2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetDepthStencilSurface2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetDepthStencilSurface2+0x06 : mov esi, [eax + 0x1A08]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0x08 }, // (Offset,Value)-Pair #5
        { 0x09, 0x1A }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetDepthStencilSurface2+0x0E : jnz +0x04
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetDepthStencilSurface2+0x15 : call [addr]
        { 0x15, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetDepthStencilSurface2+0x1D : retn
        { 0x1D, 0xC3 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CreateVertexShader_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x85 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x75 },
        { 0x7E, 0x04 },
        { 0x9E, 0x24 },
        { 0xBE, 0x24 },
        { 0xDE, 0x83 },
        { 0xFE, 0xC7 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreatePixelShader
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CreatePixelShader_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x07, 0x00 },
        { 0x10, 0xC0 },
        { 0x19, 0x08 },
        { 0x22, 0x78 },
        { 0x2B, 0x00 },
        { 0x34, 0xB9 },
        { 0x3D, 0x24 },
        { 0x46, 0x08 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexData4f
// ******************************************************************
SOOVPA<17> IDirect3DDevice8_SetVertexData4f_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x0A, 0x46 },
        { 0x16, 0x08 },
        { 0x22, 0x09 },
        { 0x2E, 0x24 },
        { 0x3A, 0x24 },
        { 0x46, 0x8B },
        { 0x52, 0x14 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexDataColor
// ******************************************************************
SOOVPA<17> IDirect3DDevice8_SetVertexDataColor_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x08, 0x06 },
        { 0x14, 0x8B },
        { 0x1C, 0x19 },
        { 0x26, 0xB6 },
        { 0x30, 0x00 },
        { 0x3A, 0xFF },
        { 0x44, 0x08 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x09, 0x84 },
        { 0x14, 0x46 },
        { 0x1F, 0x24 },
        { 0x2A, 0xBA },
        { 0x35, 0x4A },
        { 0x40, 0x5E },
        { 0x4D, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_StencilEnable_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // XRef Not Used
    {
        { 0x12, 0x8B },
        { 0x24, 0x33 },
        { 0x37, 0x74 },
        { 0x4A, 0x1E },
        { 0x5D, 0x74 },
        { 0x70, 0xB9 },
        { 0x83, 0x40 },
        { 0x96, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetIndices_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
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

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawIndexedVertices_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x1E, 0x5E },
        { 0x3E, 0x17 },
        { 0x5E, 0x00 },
        { 0x7E, 0x8B },
        { 0x9E, 0x89 },
        { 0xBE, 0x2B },
        { 0xDE, 0x00 },
        { 0xFE, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x1F, 0x05 },
        { 0x3A, 0x89 },
        { 0x64, 0xF0 },
        { 0x73, 0x13 },
        { 0x94, 0xAF },
        { 0xAB, 0x12 },
        { 0xE4, 0xDB },
        { 0xFF, 0x0C },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetMaterial_1_0_5849 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
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

// ******************************************************************
// * IDirect3DDevice8_BeginStateBig
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_BeginStateBig_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x07, 0x01 },
        { 0x10, 0x8D },
        { 0x19, 0x3B },
        { 0x22, 0x8B },
        { 0x2B, 0x00 },
        { 0x34, 0xCA },
        { 0x40, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_CreateVertexBuffer2_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
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

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
SOOVPA<8> IDirect3DVertexBuffer8_Lock_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x01, 0x51 },
        { 0x07, 0x8B },
        { 0x08, 0x4C },
        { 0x0A, 0x04 },
        { 0x0D, 0x24 },
        { 0x10, 0xC1 },
        { 0x13, 0x33 },
        { 0x16, 0x08 },
    }
};

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
SOOVPA<7> IDirect3DVertexBuffer8_Lock2_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x08, 0x56 },
        { 0x12, 0x06 },
        { 0x1D, 0xC7 },
        { 0x26, 0x00 },
        { 0x30, 0xC3 },
        { 0x3E, 0x8B },
        { 0x44, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_LightEnable_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x8D },
        { 0x3E, 0xC7 },
        { 0x5E, 0xC7 },
        { 0x7E, 0x00 },
        { 0x9E, 0x85 },
        { 0xBE, 0x01 },
        { 0xDE, 0x00 },
        { 0xFE, 0x83 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_DrawVertices_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x12, 0x8B },
        { 0x26, 0x8B },
        { 0x37, 0x00 },
        { 0x4A, 0x27 },
        { 0x5D, 0x89 },
        { 0x70, 0x75 },
        { 0x83, 0x40 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderTarget_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x44 },
        { 0x3E, 0x2C },
        { 0x5E, 0x8B },
        { 0x7E, 0xDE },
        { 0xA0, 0x89 },
        { 0xBE, 0x09 },
        { 0xDE, 0x8B },
        { 0xFE, 0x8B },
    }
};

// ******************************************************************
// * IDirect3D8_AllocContiguousMemory
// ******************************************************************
SOOVPA<7> IDirect3D8_AllocContiguousMemory_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x05, 0x10 },
        { 0x0C, 0x00 },
        { 0x13, 0x00 },
        { 0x1A, 0x08 },
        { 0x21, 0xBC },
        { 0x28, 0x0D },
        { 0x2F, 0x24 },
    }
};

// ******************************************************************
// * IDirect3DTexture8_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
SOOVPA<10> Get2DSurfaceDesc_1_0_5849 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CreateTexture2_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved, // ?? XREF_D3DDevice_CreateTexture2,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x14, 0x4C },
        { 0x2A, 0x8B },
        { 0x42, 0x8B },
        { 0x56, 0x80 },
        { 0x70, 0x85 },
        { 0x82, 0x5E },
        { 0x98, 0x89 },
        { 0xAE, 0x1C },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexShaderConstant1_1_0_5849 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1+0x0E : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1+0x28 : mov [eax-0x18], ecx
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #6
        { 0x1E, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1+0x3D : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1+0x47 : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexData2f
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetVertexData2f_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x0D, 0x05 },
        { 0x17, 0x8D },
        { 0x19, 0xCD },
        { 0x1F, 0x4C },
        { 0x25, 0x54 },
        { 0x27, 0x10 },
        { 0x30, 0x0C },
        { 0x35, 0x0C },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawVerticesUP_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x72 },
        { 0x3E, 0xC0 },
        { 0x5E, 0xC6 },
        { 0x7E, 0x14 },
        { 0x9E, 0x00 },
        { 0xBE, 0x81 },
        { 0xDE, 0x8B },
        { 0xFE, 0x7D },
    }
};

// ******************************************************************
// * IDirect3DTexture8_LockRect
// ******************************************************************
SOOVPA<7> IDirect3DTexture8_LockRect_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x03, 0x14 },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x8B },
        { 0x17, 0x50 },
        { 0x20, 0xC2 },
        { 0x21, 0x14 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_BeginPushBuffer
// ******************************************************************
SOOVPA<6> IDirect3DDevice8_BeginPushBuffer_1_0_5849 =
{
    0,  // Large == 0
    6,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x09, 0x00 },
        { 0x11, 0x8B },
        { 0x2F, 0x08 },
        { 0x43, 0x57 },
        { 0x58, 0xC8 },
        { 0x63, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_EndPushBuffer
// ******************************************************************
SOOVPA<6> IDirect3DDevice8_EndPushBuffer_1_0_5849 =
{
    0,  // Large == 0
    6,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x09, 0x00 },
        { 0x15, 0x50 },
        { 0x47, 0x7B },
        { 0x61, 0x78 },
        { 0x77, 0x07 },
        { 0x8C, 0xC3 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_RunPushBuffer
// ******************************************************************
SOOVPA<6> IDirect3DDevice8_RunPushBuffer_1_0_5849 =
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    0,  // XRef Not Used

    {
        { 0x0E, 0xED },
        { 0x42, 0x18 },
        { 0x62, 0x04 },
        { 0x8D, 0x1C },
        { 0xAF, 0x0F },
        { 0xF6, 0x41 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetVerticalBlankCallback_1_0_5849 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        { 0x06, XREF_D3DDEVICE },

        { 0x01, 0x44 },
        { 0x03, 0x04 },
        { 0x05, 0x0D },
        { 0x0C, 0xB8 },
        { 0x0D, 0x1D },
        { 0x11, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
SOOVPA<8> D3DDevice_SetViewport_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x86 },
        { 0x3E, 0x1B },
        { 0x5E, 0x8B },
        { 0x7E, 0x6D },
        { 0x9E, 0x81 },
        { 0xBE, 0x0C },
        { 0xDE, 0x75 },
        { 0xFE, 0x85 },
    }
};

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
SOOVPA<7> D3DDevice_BlockUntilVerticalBlank_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xC0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
    }
};

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
SOOVPA<9> D3DDevice_SetBackMaterial_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x44 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
SOOVPA<8> D3DDevice_SetRenderState_TwoSidedLighting_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0E, 0x05 },
        { 0x1E, 0x89 },
        { 0x30, 0x00 },
        { 0x40, 0x89 },
        { 0x52, 0x8B },
        { 0x60, 0x75 },
        { 0x6E, 0x48 },
        { 0x7E, 0x04 },
    }
};

// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_Release_1_0_5849 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x28 },
        { 0x21, 0x05 },  
        { 0x28, 0x00 }, 
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
    }
};

// ******************************************************************
// * D3D::KickOffAndWaitForIdle
// ******************************************************************
/*SOOVPA<5> D3D_KickOffAndWaitForIdle_1_0_5849 = 
{
    0,  // Large == 0
    5,  // Count == 5

    XREF_D3D_KickOffAndWaitForIdle,  // Xref Is Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x2C },
        { 0x0A, 0x51 },
        { 0x10, 0xC3 },
    }
};*/

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
SOOVPA<10> D3DDevice_SetTileNoWait_1_0_5849 = 
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    0,  // Xref Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
SOOVPA<7> D3DDevice_SetSwapCallback_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xB4 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
SOOVPA<8> D3DDevice_SetRenderState_StencilFail_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0C, 0x89 },
        { 0x1E, 0x8B },
        { 0x28, 0x74 },
        { 0x36, 0xD2 },
        { 0x44, 0x3D },
        { 0x52, 0xC7 },
        { 0x60, 0x04 },
        { 0x6E, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_VertexBlend_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
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
// * D3DDevice8::SetRenderState_LineWidth
// ******************************************************************
SOOVPA<13> D3DDevice_SetRenderState_LineWidth_1_0_5849 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_LineWidth+0x00 : push ebx
        { 0x00, 0x53 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_LineWidth+0x0C : mov [esp+8+argo_0], ebx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x5C }, // (Offset,Value)-Pair #3
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_LineWidth+0x28 : fstp [esp+10h+var_10]
        { 0x28, 0xD9 }, // (Offset,Value)-Pair #6
        { 0x29, 0x1C }, // (Offset,Value)-Pair #7
        { 0x2A, 0x24 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_LineWidth+0x30 : mov edi, eax
        { 0x30, 0x8B }, // (Offset,Value)-Pair #9
        { 0x31, 0xF8 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_LineWidth+0x41 : cmp eax, [esi+4]
        { 0x41, 0x3B }, // (Offset,Value)-Pair #11
        { 0x42, 0x46 }, // (Offset,Value)-Pair #12
        { 0x43, 0x04 }, // (Offset,Value)-Pair #13
    }
};


// ******************************************************************
// * D3DDevice8::SetDepthClipPlanes
// ******************************************************************
SOOVPA<13> D3DDevice_SetDepthClipPlanes_1_0_5849 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
SOOVPA<7> D3DDevice_SetPalette_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x10, 0x98 },
        { 0x22, 0x0F },
        { 0x34, 0x00 },
        { 0x46, 0x9C },
        { 0x58, 0xC5 },
        { 0x6E, 0x8B },
        { 0x7C, 0x89 },
    }
};

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
SOOVPA<7> D3DDevice_LoadVertexShaderProgram_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0xE0 },
        { 0x32, 0x5F },
        { 0x3F, 0x24 },
        { 0x4C, 0x83 },
        { 0x5C, 0x89 },
    }
};

// ******************************************************************
// * D3DDevice_GetPushBufferOffset
// ******************************************************************
SOOVPA<7> D3DDevice_GetPushBufferOffset_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x13, 0xCA },
        { 0x28, 0xFF },
        { 0x3D, 0x89 },
        { 0x52, 0x12 },
        { 0x67, 0xAB },
        { 0x7C, 0x51 },
        { 0x91, 0x5F },
    }
};

// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
SOOVPA<9> D3D_MakeRequestedSpace_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

	XREF_D3D_MakeRequestedSpace, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x32, 0xC2 },
        { 0x33, 0x08 },
        { 0x8E, 0x83 },
        { 0x8F, 0xC0 },
        { 0x90, 0x04 },
        { 0xBD, 0xD1 },
        { 0xBE, 0xEB },
		{ 0xBF, 0x03 },
		{ 0xC0, 0xDF },
    }
};

// ******************************************************************
// * D3D::Unknown (D3D::MakeSpace?)
// ******************************************************************
// * FOR DEBUGGING ONLY!
// ******************************************************************
SOOVPA<6> D3D_Unknown_1_0_5849 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    1,  // Xref Is Used

    {
		{ 0x0A, XREF_D3D_MakeRequestedSpace },

        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
		{ 0x0E, 0xC3 },
    }
};

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
SOOVPA<10> D3DDevice_FlushVertexCache_1_0_5849 = 
{
    0,  // Large == 0
    10,  // Count == 10

    XRefNotSaved,
    0,  // Xref Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
SOOVPA<7> D3DDevice_AddRef_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x38 },
        { 0x0A, 0x00 },
        { 0x0D, 0x88 },
        { 0x10, 0x00 },
        { 0x13, 0xC1 },
    }
};

// ******************************************************************
// * D3D::ClearStateBlockFlags
// ******************************************************************
SOOVPA<9> D3D_ClearStateBlockFlags_1_0_5849 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_CLEARSTATEBLOCKFLAGS, // XRef Is  Saved
    0,                         // XRef Not Used

    {
        // IDirect3D_ClearStateBlockFlags+0x0A : mov ecx, 0x87
        { 0x0A, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x87 }, // (Offset,Value)-Pair #2

        // IDirect3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x07A4]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #3
        { 0x17, 0x8A }, // (Offset,Value)-Pair #4
        { 0x18, 0xA4 }, // (Offset,Value)-Pair #5
        { 0x19, 0x07 }, // (Offset,Value)-Pair #6

        // IDirect3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x31, 0x90 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * D3D::RecordStateBlock
// ******************************************************************
SOOVPA<10> D3D_RecordStateBlock_1_0_5849 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_RECORDSTATEBLOCK,  // XRef Is  Saved
    0,                      // XRef Not Used

    {
        // IDirect3D_RecordStateBlock+0x0F : mov eax, [edi+0x07A4]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x87 }, // (Offset,Value)-Pair #2
        { 0x11, 0xA4 }, // (Offset,Value)-Pair #3
        { 0x12, 0x07 }, // (Offset,Value)-Pair #4

        // IDirect3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 }, // (Offset,Value)-Pair #5
        { 0x96, 0xD9 }, // (Offset,Value)-Pair #6

        // IDirect3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xD6, 0x46 }, // (Offset,Value)-Pair #8
        { 0xD7, 0x0C }, // (Offset,Value)-Pair #9
        { 0xD8, 0x01 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_BackFillMode_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
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
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
SOOVPA<8> D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0D, 0x8B },
        { 0x1D, 0x80 },
        { 0x2B, 0x08 },
        { 0x3A, 0x01 },
        { 0x49, 0xC7 },
        { 0x58, 0x05 },
        { 0x67, 0xC0 },
        { 0x76, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
SOOVPA<7> D3DDevice_GetTile_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x06, 0x8B },
        { 0x0E, 0x94 },
        { 0x16, 0x8B },
        { 0x1E, 0x04 },
        { 0x26, 0x48 },
        { 0x2E, 0x8B },
        { 0x36, 0x14 },
    }
};

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
SOOVPA<7> D3DDevice_Swap_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0x3F },
        { 0x8F, 0x8E },
        { 0xAC, 0xC6 },
        { 0xC9, 0xC0 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap (2)
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap2_1_0_5849 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_Swap+0x2A : mov ebx, 5
        { 0x2A, 0xBB }, // (Offset,Value)-Pair #1
        { 0x2B, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x37 : test bl, 3
        { 0x37, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x38, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x39, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x60 : inc dword ptr [esi+0x2478]
        { 0x60, 0xFF }, // (Offset,Value)-Pair #6
        { 0x61, 0x86 }, // (Offset,Value)-Pair #7
        { 0x62, 0x78 }, // (Offset,Value)-Pair #8
        { 0x63, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xE0 : retn 4
        { 0xE0, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xE1, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
SOOVPA<13> D3D_CDevice_KickOff_1_0_5849 = 
{
	0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    0,  // Xref Is  Used

    {
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
	}
};

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
SOOVPA<7> D3DDevice_SetSoftDisplayFilter_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0D, 0x56 },
        { 0x14, 0x8B },
        { 0x1F, 0x95 },
        { 0x2A, 0x00 },
        { 0x37, 0x33 },
        { 0x44, 0x01 },
        { 0x4D, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
SOOVPA<7> D3DDevice_SetFlickerFilter_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x07, 0xC9 },
        { 0x10, 0x24 },
        { 0x1A, 0x74 },
        { 0x22, 0x6A },
        { 0x2E, 0xC7 },
        { 0x34, 0x01 },
        { 0x3E, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
SOOVPA<9> D3DDevice_SetRenderState_MultiSampleMode_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
SOOVPA<9> D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x74 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0D, 0xCE },
        { 0x1E, 0x8B },
        { 0x2B, 0xF9 },
        { 0x3A, 0xCF },
        { 0x49, 0x00 },
        { 0x58, 0xC1 },
        { 0x6B, 0x5F },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
SOOVPA<8> D3DDevice_SetRenderState_MultiSampleMask_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0A, 0xA3 },
        { 0x12, 0x56 },
        { 0x1C, 0x8B },
        { 0x2A, 0x85 },
        { 0x30, 0x01 },
        { 0x3D, 0xC7 },
        { 0x44, 0x78 },
        { 0x4E, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
SOOVPA<7> D3DDevice_SetPixelShaderConstant_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x85 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC0 },
        { 0x7E, 0x0F },
        { 0x9F, 0x8B },
        { 0xBE, 0x00 },
        { 0xDE, 0xC0 },
    }
};

// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
SOOVPA<7> D3DDevice_GetDisplayFieldStatus_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_GetScissors
// ******************************************************************
SOOVPA<7> D3DDevice_GetScissors_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x0B, 0x74 },
        { 0x14, 0x11 },
        { 0x1F, 0x04 },
        { 0x2A, 0x85 },
        { 0x35, 0xE1 },
        { 0x40, 0xC1 },
        { 0x4B, 0xA4 },
    }
};

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
SOOVPA<8> D3DDevice_SetScissors_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1D, 0x44 },
        { 0x3C, 0x8B },
        { 0x5D, 0xD9 },
        { 0x7D, 0xD8 },
        { 0x99, 0x0E },
        { 0xB8, 0xE8 },
        { 0xD7, 0x24 },
        { 0xF6, 0x8B },
    }
};

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
SOOVPA<8> D3DDevice_GetBackMaterial_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x44 },
        { 0x0E, 0x0F },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
SOOVPA<8> D3DDevice_CaptureStateBlock_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x1E, 0x3C },
        { 0x3E, 0xC0 },
        { 0x5E, 0x7E },
        { 0x7E, 0x00 },
        { 0x9E, 0x74 },
        { 0xC0, 0x8B },
        { 0xDE, 0x83 },
        { 0xFE, 0x45 },
    }
};

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
SOOVPA<9> D3DDevice_DeleteStateBlock_1_0_5849 = 
{
    0,  // Large == 0
    9,  // Count == 7

    XRefNotSaved,
    0,  // Xref Not Used

    {
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
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_RopZCmpAlwaysRead_1_0_5849 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    1,  // Xref Is Used

    {
		{ 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD },

        { 0x01, 0x44 },
        { 0x04, 0xA3 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_GetTexture2
// ******************************************************************
SOOVPA<8> D3DDevice_GetTexture2_1_0_5849 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    0,  // Xref Not Used

    {
        { 0x03, 0x04 },
        { 0x0A, 0x56 },
        { 0x0D, 0x81 },
        { 0x0E, 0x88 },
        { 0x12, 0x8B },
        { 0x17, 0x06 },
        { 0x1E, 0x8B },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * D3D8_1_0_5849
// ******************************************************************
OOVPATable D3D8_1_0_5849[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_4627,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
    // IDirect3DDevice8::BeginVisibilityTest (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_BeginVisibilityTest_1_0_4627,

        XTL::EmuIDirect3DDevice8_BeginVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginVisibilityTest"
        #endif
    },
    // IDirect3DDevice8::EndVisibilityTest
    {
        (OOVPA*)&IDirect3DDevice8_EndVisibilityTest_1_0_5849,

        XTL::EmuIDirect3DDevice8_EndVisibilityTest,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndVisibilityTest"
        #endif
    },
    // IDirect3DDevice8::GetVisibilityTestResult
    {
        (OOVPA*)&IDirect3DDevice8_GetVisibilityTestResult_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetVisibilityTestResult,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetVisibilityTestResult"
        #endif
    },
    // IDirect3DDevice8::GetViewport
    {
        (OOVPA*)&IDirect3DDevice8_GetViewport_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetViewport"
        #endif
    },
    // IDirect3DDevice8::SetBackBufferScale
    {
        (OOVPA*)&IDirect3DDevice8_SetBackBufferScale_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetBackBufferScale,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetBackBufferScale"
        #endif
    },
    // IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetCreationParameters_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetCreationParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetCreationParameters"
        #endif
    },
    // IDirect3DDevice8::Clear (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_5233,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&D3DDevice_Swap_1_0_5849,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
	// IDirect3DDevice8::Swap (2)
    {
        (OOVPA*)&IDirect3DDevice8_Swap2_1_0_5849,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_Swap2"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer2
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer2_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetBackBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer2"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface2
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface2_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface2"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_5849,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::CreatePixelShader
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_5849,

        XTL::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
        #endif
    },
    // IDirect3DDevice8::SetPixelShader (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
    // IDirect3DDevice8::GetDisplayMode (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayMode_1_0_4627,

        XTL::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
        #endif
    },
    // IDirect3DDevice8::SetVertexData2f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData2f_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetVertexData2f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2f"
        #endif
    },
    // IDirect3DDevice8::SetVertexData4f
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData4f_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetVertexData4f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4f"
        #endif
    },
    // IDirect3DDevice8::SetVertexDataColor
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexDataColor_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetVertexDataColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexDataColor"
        #endif
    },
    // IDirect3DDevice8::End (* unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_End_1_0_5558,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
        #endif
    },
    // IDirect3DDevice8::SetVerticalBlankCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetVerticalBlankCallback_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVerticalBlankCallback"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BumpEnv"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_BorderColor (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BorderColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_NormalizeNormals (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
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
    // IDirect3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader (* unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
        #endif
    },
    // IDirect3DDevice8::SetTexture (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // D3D::CDevice::LazySetStateVB (XREF) (* unchanged since 5558 *)
    {
        (OOVPA*)&D3D_CDevice_LazySetStateVB_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "D3D::CDevice::LazySetStateVB (XREF)"
        #endif
    },
    // IDirect3DDevice8::BeginPush (* unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_BeginPush_1_0_5558,

        XTL::EmuIDirect3DDevice8_BeginPush,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginPush"
        #endif
    },
    // IDirect3DDevice8::EndPush (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_EndPush_1_0_4627,

        XTL::EmuIDirect3DDevice8_EndPush,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndPush"
        #endif
    },
    // /* Not implemented yet.
    // IDirect3DDevice8::BeginPushBuffer
    {
        (OOVPA*)&IDirect3DDevice8_BeginPushBuffer_1_0_5849,

        XTL::EmuIDirect3DDevice8_BeginPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginPushBuffer"
        #endif
    },
    // IDirect3DDevice8::EndPushBuffer
    {
        (OOVPA*)&IDirect3DDevice8_EndPushBuffer_1_0_5849,

        XTL::EmuIDirect3DDevice8_EndPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndPushBuffer"
        #endif
    },
   // */
    // IDirect3DDevice8::RunPushBuffer
    {
        (OOVPA*)&IDirect3DDevice8_RunPushBuffer_1_0_5849,

        XTL::EmuIDirect3DDevice8_RunPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_RunPushBuffer"
        #endif
    },
    // IDirect3DDevice8::Begin (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_Begin_1_0_5233,

        XTL::EmuIDirect3DDevice8_Begin,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Begin"
        #endif
    },
    // IDirect3DDevice8::BeginStateBig
    {
        (OOVPA*)&IDirect3DDevice8_BeginStateBig_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"IDirect3DDevice8_BeginStateBig (XRef)"
        #endif

    },
    // IDirect3DDevice8::GetTransform (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetTransform_1_0_4361,

        XTL::EmuIDirect3DDevice8_GetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTransform"
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
    // IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_5849,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5849,

        XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVerticesUP"
        #endif
    },
    // IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::SetStreamSource (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DResource8::Register (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DResource8_Register_1_0_3925,

        XTL::EmuIDirect3DResource8_Register,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Register"
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
    // IDirect3DResource8::Release (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_3925,

        XTL::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release"
        #endif
    },
    // IDirect3DResource8::IsBusy (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DResource8_IsBusy_1_0_4361,

        XTL::EmuIDirect3DResource8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_IsBusy"
        #endif
    },
    // IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DSurface8_GetDesc_1_0_4361,

        XTL::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc"
        #endif
    },
    // IDirect3DDevice8::CopyRects (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_CopyRects_1_0_5233,

        XTL::EmuIDirect3DDevice8_CopyRects,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CopyRects"
        #endif
    },
    // IDirect3DPalette8::Lock2 (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DPalette8_Lock2_1_0_5233,

        XTL::EmuIDirect3DPalette8_Lock2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DPalette8_Lock2"
        #endif
    },    
    // IDirect3DTexture8::LockRect
    {
        (OOVPA*)&IDirect3DTexture8_LockRect_1_0_5849,

        XTL::EmuIDirect3DTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_LockRect"
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
    // IDirect3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DBaseTexture8_GetLevelCount_1_0_4361,

        XTL::EmuIDirect3DBaseTexture8_GetLevelCount,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DBaseTexture8_GetLevelCount"
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel2 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel2_1_0_4627,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel2"
        #endif
    },
    // IDirect3DDevice8::SetLight
    {

        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer2
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer2_1_0_5849,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer2"
        #endif
    },
    // IDirect3DVertexBuffer8_Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_5849,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock2
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock2_1_0_5849,

        XTL::EmuIDirect3DVertexBuffer8_Lock2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock2"
        #endif
    },
    // IDirect3DDevice8_LightEnable
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_5849,

        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
    // IDirect3DDevice8_DrawVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_5849,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3D8_AllocContiguousMemory
    {
        (OOVPA*)&IDirect3D8_AllocContiguousMemory_1_0_5849,
        XTL::EmuIDirect3D8_AllocContiguousMemory,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_AllocContiguousMemory"
        #endif
    },
    // IDirect3DTexture8_GetLevelDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_5849,
        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetLevelDesc"
        #endif
    },
    // IDirect3DDevice8::CreateTexture2
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture2_1_0_5849,

        XTL::EmuIDirect3DDevice8_CreateTexture2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture2"
        #endif
    },
    // Lock2DSurface
    {
        (OOVPA*)&Lock2DSurface_1_0_3925,

        XTL::EmuLock2DSurface,

        #ifdef _DEBUG_TRACE
        "EmuLock2DSurface"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant4
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant4"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant1
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant1_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant1"
        #endif
    },
    // IDirect3DDevice8::DrawVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawVerticesUP_1_0_5849,

        XTL::EmuIDirect3DDevice8_DrawVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget"
        #endif
    },
	// IDirect3DDevice8::SetShaderConstantMode (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
	// IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&D3DDevice_SetViewport_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
	// IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&D3DDevice_BlockUntilVerticalBlank_1_0_5849,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FrontFace"
        #endif
    },
	// IDirect3DDevice8::SetBackMaterial
    {
        (OOVPA*)&D3DDevice_SetBackMaterial_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetBackMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetBackMaterial"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_TwoSidedLighting
    {
        (OOVPA*)&D3DDevice_SetRenderState_TwoSidedLighting_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TwoSidedLighting"
        #endif
    },
	// IDirect3D8::KickOffAndWaitForIdle (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3D8_KickOffAndWaitForIdle_1_0_5233,

        XTL::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle"
        #endif
    },
	// D3D::SetTileNoWait
    {
        (OOVPA*)&D3DDevice_SetTileNoWait_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetTileNoWait,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTileNoWait"
        #endif
    },
	// IDirect3DDevice8::SetGammaRamp (* unchanged since 4928 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetGammaRamp_1_0_4928,

        XTL::EmuIDirect3DDevice8_SetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetGammaRamp"
        #endif
    },
	// IDirect3DDevice8::GetGammaRamp (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetGammaRamp_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetGammaRamp"
        #endif
    },
	// D3DDevice_SetSwapCallback
    {
        (OOVPA*)&D3DDevice_SetSwapCallback_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetSwapCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSwapCallback"
        #endif
    },
	// IDirect3DDevice8::GetRenderTarget2 (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget2_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetRenderTarget2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget2"
        #endif
    },
	// D3DDevice_SetRenderState_StencilFail
    {
        (OOVPA*)&D3DDevice_SetRenderState_StencilFail_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
        #endif
    },
	// D3DDevice_SetRenderState_VertexBlend
    {
        (OOVPA*)&D3DDevice_SetRenderState_VertexBlend_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
	// D3DDevice_SetRenderState_VertexBlend
    {
        (OOVPA*)&D3DDevice_SetRenderState_VertexBlend_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
	// IDirect3DDevice8::Reset (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_Reset_1_0_4134,

        XTL::EmuIDirect3DDevice8_Reset,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Reset"
        #endif
    },
	// IDirect3DDevice::Release
    {
        (OOVPA*)&IDirect3DDevice8_Release_1_0_5849,
        XTL::EmuIDirect3DDevice8_Release,

        #ifdef _DEBUG_TRACE
        "D3DDevice_Release"
        #endif
    },
	// IDirect3DDevice8_InsertCallback (* unchanged since 5558 *)
	{
		(OOVPA*)&D3DDevice_InsertCallback_1_0_5558,

		XTL::EmuIDirect3DDevice8_InsertCallback,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_InsertCallback"
		#endif
	},
	// D3DDevice::SetRenderState_LineWidth
    {
        (OOVPA*)&D3DDevice_SetRenderState_LineWidth_1_0_5849,  
        XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth,
            
        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LineWidth"
        #endif
    },
	// D3DDevice_SetRenderState_SampleAlpha (* unchanged since 5233 *)
	{
		(OOVPA*)&D3DDevice_SetRenderState_SampleAlpha_1_0_5233,

		XTL::EmuIDirect3DDevice8_SetRenderState_SampleAlpha,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_SampleAlpha"
		#endif
	},
    // IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
	// IDirect3DDevice8::EnableOverlay (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_EnableOverlay_1_0_4361,

        XTL::EmuIDirect3DDevice8_EnableOverlay,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EnableOverlay"
        #endif
    },
	// D3DDevice_SetRenderState_LogicOp (* unchanged since 5558 *)
	{
		(OOVPA*)&D3DDevice_SetRenderState_LogicOp_1_0_5558,

		XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_LogicOp"
		#endif
	},
	// D3DDevice_SetRenderState_MultiSampleMask
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleMask_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMask"
        #endif
    },
	// D3DDevice_SetPalette
    {
        (OOVPA*)&D3DDevice_SetPalette_1_0_5849,
        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette"
        #endif
    },
	// D3DDevice_LoadVertexShaderProgram
    {
        (OOVPA*)&D3DDevice_LoadVertexShaderProgram_1_0_5849,
        XTL::EmuIDirect3DDevice8_LoadVertexShaderProgram,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShaderProgram"
        #endif
    },
	// D3DDevice_GetPushBufferOffset
	{
		(OOVPA*)&D3DDevice_GetPushBufferOffset_1_0_5849,
		XTL::EmuIDirect3DDevice8_GetPushBufferOffset,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_GetPushBufferOffset"
		#endif
	},
	// D3D::MakeRequestedSpace
	{
		(OOVPA*)&D3D_MakeRequestedSpace_1_0_5849, 0,

		#ifdef _DEBUG_TRACE
		"D3D::MakeRequestedSpace (XRef)"
		#endif
	},
	// D3D::Unknown
	{
		(OOVPA*)&D3D_Unknown_1_0_5849, 0,

		#ifdef _DEBUG_TRACE
		"D3D::Unknown (XRef)"
		#endif
	},
	// IDirect3DDevice8::SetVertexShaderConstantNotInline (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstantNotInline"
        #endif
    },
	// D3DDevice_FlushVertexCache
	{
		(OOVPA*)&D3DDevice_FlushVertexCache_1_0_5849,
		XTL::EmuIDirect3DDevice8_FlushVertexCache,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_FlushVertexCache"
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
	// IDirect3DDevice8::LoadVertexShader (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_5233,

        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader"
        #endif
    },
	// IDirect3DDevice8::SelectVertexShader (* unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_SelectVertexShader_1_0_5558,

        XTL::EmuIDirect3DDevice8_SelectVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShader"
        #endif
    },
	// D3DDevice_RunVertexStateShader (* unchanged since 4627 *)
    {
       (OOVPA*)&X_D3DDevice_RunVertexStateShader_1_0_4627,

       XTL::EmuIDirect3DDevice8_RunVertexStateShader,

       #ifdef _DEBUG_TRACE
       "D3DDevice_RunVertexStateShader"
       #endif
    },
	// IDirect3DDevice8::AddRef
    {
        (OOVPA*)&D3DDevice_AddRef_1_0_5849,

        XTL::EmuIDirect3DDevice8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_AddRef"
        #endif
    },
	// D3D::ClearStateBlockFlags
    {
        (OOVPA*)&D3D_ClearStateBlockFlags_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"D3D::ClearStateBlockFlags (XREF)"
        #endif
    },
	// D3D::RecordStateBlock
    {
        (OOVPA*)&D3D_RecordStateBlock_1_0_5849, 0,

        #ifdef _DEBUG_TRACE
		"D3D::RecordStateBlock (XREF)"
        #endif
    },
    // IDirect3DDevice8::BeginStateBlock
    {
        (OOVPA*)&IDirect3DDevice8_BeginStateBlock_1_0_4361,

        XTL::EmuIDirect3DDevice8_BeginStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginStateBlock"
        #endif
    },
	// IDirect3DDevice8::EndStateBlock
    {
        (OOVPA*)&IDirect3DDevice8_EndStateBlock_1_0_4361,

        XTL::EmuIDirect3DDevice8_EndStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndStateBlock"
        #endif
    },
	// D3DDevice_SetRenderState_BackFillMode
	{
        (OOVPA*)&D3DDevice_SetRenderState_BackFillMode_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_BackFillMode"
        #endif
    },
	// D3DDevice_SetRenderState_Dxt1NoiseEnable
	{
        (OOVPA*)&D3DDevice_SetRenderState_Dxt1NoiseEnable_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable"
        #endif
    },
	// D3DDevice_GetTile
	{
        (OOVPA*)&D3DDevice_GetTile_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetTile,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTile"
        #endif
    },
	// D3D::CDevice::KickOff
	{
		(OOVPA*)&D3D_CDevice_KickOff_1_0_5849, 
			
		XTL::EmuD3DDevice_KickOff,

		#ifdef _DEBUG_TRACE
		"D3D::CDevice::KickOff (D3DDevice::KickPushBuffer)"
		#endif
	},
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed"
        #endif
    },
	// IDirect3DDevice8::SetSoftDisplayFilter
    {
        (OOVPA*)&D3DDevice_SetSoftDisplayFilter_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSoftDisplayFilter"
        #endif
    },
	// IDirect3DDevice8::SetFlickerFilter
    {
        (OOVPA*)&D3DDevice_SetFlickerFilter_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetFlickerFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetFlickerFilter"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleMode_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMode"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleMask
    {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleMask_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMode"
        #endif
    },
	// IDirect3DDevice8::SetPixelShaderConstant
    {
        (OOVPA*)&D3DDevice_SetPixelShaderConstant_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetPixelShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderConstant"
        #endif
    },
	// D3DDevice_GetDisplayFieldStatus
    {
        (OOVPA*)&D3DDevice_GetDisplayFieldStatus_1_0_5849,
		XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayFieldStatus"
        #endif
   },
   // IDirect3DCubeTexture8::GetCubeMapSurface2 (* Unchanged since 4627 *)
    {
        (OOVPA*)&D3DCubeTexture_GetCubeMapSurface2_1_0_4627,

        XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DCubeTexture8_GetCubeMapSurface2"
        #endif
    }, 
	// IDirect3DDevice8::SetVertexShaderConstant1Fast (* Unchanged since 5558 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant1Fast_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant1Fast"
        #endif
    },
	// D3DDevice_SetPixelShaderProgram (* Unchanged since 5558 *)
    {
        (OOVPA*)&D3DDevice_SetPixelShaderProgram_1_0_5558,
        XTL::EmuIDirect3DDevice8_SetPixelShaderProgram,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderProgram"
        #endif
    },
	// IDirect3DDevice8::SetScissors
    {
        (OOVPA*)&D3DDevice_SetScissors_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetScissors,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetScissors"
        #endif
    },
	// IDirect3DDevice8::GetScissors
    {
        (OOVPA*)&D3DDevice_GetScissors_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetScissors,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_GetScissors"
        #endif
    },
	// IDirect3DDevice8::GetBackMaterial
    {
        (OOVPA*)&D3DDevice_GetBackMaterial_1_0_5849,

        XTL::EmuIDirect3DDevice8_GetBackMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackMaterial"
        #endif
    },
	// IDirect3DDevice8::ApplyStateBlock (* unchanged since 4627 *)
    {
        (OOVPA*)&D3DDevice_ApplyStateBlock_1_0_4627,

        XTL::EmuIDirect3DDevice8_ApplyStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_ApplyStateBlock"
        #endif
    },
	// IDirect3DDevice8::CaptureStateBlock
    {
        (OOVPA*)&D3DDevice_CaptureStateBlock_1_0_5849,

        XTL::EmuIDirect3DDevice8_CaptureStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CaptureStateBlock"
        #endif
    },
	// IDirect3DDevice8::DeleteStateBlock
    {
        (OOVPA*)&D3DDevice_DeleteStateBlock_1_0_5849,

        XTL::EmuIDirect3DDevice8_DeleteStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteStateBlock"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilCullEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead
    {
        (OOVPA*)&D3DDevice_SetRenderState_RopZCmpAlwaysRead_1_0_5849,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed"
        #endif
    },
	// D3DDevice_GetTexture2
	{
       (OOVPA*)&D3DDevice_GetTexture2_1_0_5849,
       XTL::EmuIDirect3DDevice8_GetTexture2,

       #ifdef _DEBUG_TRACE
       "EmuIDirect3DDevice8_GetTexture2"
       #endif
    },
	// IDirect3DDevice8::SetRenderTargetFast (* unchanged since 5344 *)
    {
        (OOVPA*)&D3DDevice_SetRenderTargetFast_1_0_5344,

        XTL::EmuIDirect3DDevice8_SetRenderTargetFast,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderTargetFast"
        #endif
    },
	// D3DDevice_SetDepthClipPlanes
	{
       (OOVPA*)&D3DDevice_SetDepthClipPlanes_1_0_5849,
       XTL::EmuIDirect3DDevice8_SetDepthClipPlanes,

       #ifdef _DEBUG_TRACE
       "EmuIDirect3DDevice8_SetDepthClipPlanes"
       #endif
    },
};

// ******************************************************************
// * D3D8_1_0_5849_SIZE
// ******************************************************************
uint32 D3D8_1_0_5849_SIZE = sizeof(D3D8_1_0_5849);
