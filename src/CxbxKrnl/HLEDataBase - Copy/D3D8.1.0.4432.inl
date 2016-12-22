// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4432.cpp
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
SOOVPA<8> IDirect3D8_CreateDevice_1_0_4432 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x86 : repe stosd
        { 0x86, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x87, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x89 : mov eax, esi
        { 0x89, 0x8B }, // (Offset,Value)-Pair #5
        { 0x8A, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 }, // (Offset,Value)-Pair #7
        { 0xA1, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetRenderTarget_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x70 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetRenderTarget+0x15 : push eax
        { 0x15, 0x50 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetRenderTarget+0x13 : jz + 0x06
        { 0x13, 0x74 }, // (Offset,Value)-Pair #7
        { 0x14, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetRenderTarget+0x16 : call [addr]
        { 0x16, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetDepthStencilSurface_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetDepthStencilSurface+0x05 : mov eax, [eax + 0x2074]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x74 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetDepthStencilSurface+0x15 : push eax
        { 0x15, 0x50 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetDepthStencilSurface+0x13 : jnz +0x0B
        { 0x13, 0x74 }, // (Offset,Value)-Pair #7
        { 0x14, 0x0B }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetDepthStencilSurface+0x16 : call [addr]
        { 0x16, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetDepthStencilSurface+0x25 : retn 0x04
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x26, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetPixelShader_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetPixelShader+0x22 : or ecx, 0x4800
        { 0x22, 0x81 }, // (Offset,Value)-Pair #1
        { 0x23, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x24, 0x00 }, // (Offset,Value)-Pair #3
        { 0x25, 0x48 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetPixelShader+0x3A : or eax, 0x2000
        { 0x3A, 0x0D }, // (Offset,Value)-Pair #5
        { 0x3C, 0x20 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetPixelShader+0x78 : retn 0x04
        { 0x78, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x79, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xE4, 0xA5 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4432 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x11 : or edx, 0x1000
        { 0x11, 0x81 }, // (Offset,Value)-Pair #1
        { 0x12, 0xCA }, // (Offset,Value)-Pair #2
        { 0x13, 0x00 }, // (Offset,Value)-Pair #3
        { 0x14, 0x10 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x24 : cmp eax, [esi+4]; push edi
        { 0x24, 0x3B }, // (Offset,Value)-Pair #5
        { 0x25, 0x46 }, // (Offset,Value)-Pair #6
        { 0x26, 0x04 }, // (Offset,Value)-Pair #7
        { 0x27, 0x57 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x53 : add eax, 0x0C
        { 0x53, 0x83 }, // (Offset,Value)-Pair #9
        { 0x54, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x55, 0x0C }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x60 : retn 0x04
        { 0x60, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x61, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_ColorKeyColor
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x27 : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x27, 0x83 }, // (Offset,Value)-Pair #3
        { 0x28, 0xC0 }, // (Offset,Value)-Pair #4
        { 0x29, 0x08 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x89 }, // (Offset,Value)-Pair #6
        { 0x2B, 0x06 }, // (Offset,Value)-Pair #7
        { 0x2C, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x2D, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x2E, 0x07 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetTextureState_ColorKeyColor+0x36 : retn 0x08
        { 0x36, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x37, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x25 : or [addr], 0x200
        { 0x25, 0x81 }, // (Offset,Value)-Pair #3
        { 0x26, 0x0D }, // (Offset,Value)-Pair #4
        { 0x2B, 0x00 }, // (Offset,Value)-Pair #5
        { 0x2C, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x36 : retn 0x04
        { 0x36, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x37, 0x04 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_4432 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x1B : mov ecx, [esi+0x2074]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x8E }, // (Offset,Value)-Pair #4
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x20 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnable+0x2E : mov dword ptr [eax], 0x4030C
        { 0x2E, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #8
        { 0x30, 0x0C }, // (Offset,Value)-Pair #9
        { 0x31, 0x03 }, // (Offset,Value)-Pair #10
        { 0x32, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_ZEnable+0x8C : retn 0x04
        { 0x8C, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x8D, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4432 =
{
    0,  // Large == 0
    9, // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x12 : call [abs]
        { 0x12, 0xE8 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x38 : shl ecx, 0x10
        { 0x38, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x39, 0xE1 }, // (Offset,Value)-Pair #4
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x44 : or ecx, edx
        { 0x44, 0x0B }, // (Offset,Value)-Pair #6
        { 0x45, 0xCA }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x50, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_LineWidth_1_0_4432 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x460]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2
        { 0x17, 0x60 }, // (Offset,Value)-Pair #3
        { 0x18, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x63, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetPalette_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0A }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x76 : add esi, 0x41B20
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
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x46 : inc dword ptr [esi+0x2AD8]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xD8 }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap_1_0_4531 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x46 : inc dword ptr [esi+0x2AD8]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xD8 }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xAE : retn 4
        { 0xB9, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xBA, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_UpdateOverlay_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_UpdateOverlay+0x0F : mov [eax+0x2AA4], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0xA4 }, // (Offset,Value)-Pair #3
        { 0x12, 0x2A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x86 : and ecx, 0xFFFFFFFE
        { 0x86, 0x83 }, // (Offset,Value)-Pair #5
        { 0x87, 0xE1 }, // (Offset,Value)-Pair #6
        { 0x88, 0xFE }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0xA2 : mov [esi+0x8920], ecx
        { 0xA2, 0x89 }, // (Offset,Value)-Pair #8
        { 0xA3, 0x8E }, // (Offset,Value)-Pair #9
        { 0xA4, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA5, 0x89 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x2448
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x48 }, // (Offset,Value)-Pair #8
        { 0x19, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetVerticalBlankCallback_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x2444], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x44 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_BackFillMode
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_BackFillMode_1_0_4531 = 
{
    0,  // Large == 0
    9,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x56 },
		{ 0x10, 0x8B },
		{ 0x11, 0x06 },
		{ 0x12, 0x3B },
		{ 0x13, 0x46 },
		{ 0x14, 0x04 },
		{ 0x41, 0x83 },
		{ 0x42, 0xC0 },
		{ 0x43, 0x0C },
    }
};

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
SOOVPA<6> D3DDevice_BeginPush_1_0_4432 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x09, 0x8B },
        { 0x0A, 0xCE },
        { 0x10, 0x8B },
        { 0x11, 0x44 },
        { 0x17, 0xE8 },
        { 0x1D, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
SOOVPA<8> D3DDevice_EndPush_1_0_4432 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
// TODO: Add
SOOVPA<7> IDirect3DDevice8_DeletePixelShader_1_0_4531 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x75 },
        { 0x0A, 0x04 },
        { 0x0E, 0x09 },
        { 0x12, 0x04 },
        { 0x18, 0xC2 },
        { 0x1A, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Release
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_Release_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
		{ 0x07, 0x8B },
		{ 0x08, 0x87 },
		{ 0x09, 0x40 },
		{ 0x0A, 0x04 },
		{ 0x0B, 0x00 },
		{ 0x0C, 0x00 },
		{ 0x1B, 0xB9 },
		{ 0x1C, 0xC0 },
		{ 0x1D, 0x0A },
		{ 0x1E, 0x00 },
		{ 0x1F, 0x00 }
	}
};

// ******************************************************************
// * IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
SOOVPA<14> IDirect3DTexture8_GetSurfaceLevel_1_0_4432 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DTexture8_GetSurfaceLevel+0x00 : sub esp, 0x0C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DTexture8_GetSurfaceLevel+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x14 }, // (Offset,Value)-Pair #5

        // IDirect3DTexture8_GetSurfaceLevel+0x04 : lea eax, [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #6
        { 0x0B, 0x14 }, // (Offset,Value)-Pair #7

        // IDirect3DTexture8_GetSurfaceLevel+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DTexture8_GetSurfaceLevel+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D }, // (Offset,Value)-Pair #10
        { 0x1E, 0x18 }, // (Offset,Value)-Pair #11

        // IDirect3DTexture8_GetSurfaceLevel+0x29 : call [abs]
        { 0x29, 0xE8 }, // (Offset,Value)-Pair #12

        // IDirect3DTexture8_GetSurfaceLevel+0x4C : retn 0x0C
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x4D, 0x0C }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDisplayMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetDisplayMode_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetDisplayMode+0x01 : mov esi, ds:[*]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #1
        { 0x02, 0x35 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetDisplayMode+0x18 : shr ecx, 0x14
        { 0x18, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x19, 0xE9 }, // (Offset,Value)-Pair #4
        { 0x1A, 0x14 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetDisplayMode+0x50 : shr eax, 0x0C
        { 0x50, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x51, 0xE8 }, // (Offset,Value)-Pair #7
        { 0x52, 0x0C }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetDisplayMode+0x6B : and eax, 0x0A
        { 0x6B, 0x83 }, // (Offset,Value)-Pair #9
        { 0x6C, 0xE0 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetDisplayMode+0x92 : retn 4
        { 0x92, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x93, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetFlickerFilter_1_0_4432 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetFlickerFilter+0x1C : mov eax, [eax+0x227C]
        { 0x1D, 0x80 }, // (Offset,Value)-Pair #2
        { 0x1E, 0x7C }, // (Offset,Value)-Pair #3
        { 0x1F, 0x22 }, // (Offset,Value)-Pair #4

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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetSoftDisplayFilter_1_0_4432 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x227C]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x90 }, // (Offset,Value)-Pair #2
        { 0x27, 0x7C }, // (Offset,Value)-Pair #3
        { 0x28, 0x22 }, // (Offset,Value)-Pair #4

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
    }
};

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
SOOVPA<11> IDirect3DVertexBuffer8_Lock_1_0_4531 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x55, 0x14 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
SOOVPA<7> D3DDevice_GetProjectionViewportMatrix_1_0_4432 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x70 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
SOOVPA<8> D3DDevice_SetVertexShaderInput_1_0_4432 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x83 },
        { 0x3E, 0x10 },
        { 0x5E, 0x00 },
        { 0x7E, 0x24 },
        { 0x9E, 0x89 },
        { 0xBE, 0x81 },
        { 0xDE, 0xC6 },
        { 0xFE, 0x89 },
    }
};

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
SOOVPA<7> D3DDevice_SetModelView_1_0_4432 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x15, 0x25 },
        { 0x2F, 0x81 },
        { 0x43, 0x43 },
        { 0x5A, 0x04 },
        { 0x72, 0x8D },
        { 0x88, 0x00 },
        { 0x9F, 0x75 },
    }
};

// ******************************************************************
// * D3D8_1_0_4432
// ******************************************************************
OOVPATable D3D8_1_0_4432[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_4432,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
    // IDirect3D8::CheckDeviceFormat (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3D8_CheckDeviceFormat_1_0_4361,

        XTL::EmuIDirect3D8_CheckDeviceFormat,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CheckDeviceFormat"
        #endif
    },    
    // IDirect3DDevice8::SetRenderTarget (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget"
        #endif
    },
    // IDirect3DDevice8::AddRef (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_AddRef_1_0_4361,

        XTL::EmuIDirect3DDevice8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_AddRef"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
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
    // IDirect3DDevice8::GetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget_1_0_4432,

        XTL::EmuIDirect3DDevice8_GetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface_1_0_4432,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface"
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
    // IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
    // IDirect3D8::KickOffAndWaitForIdle (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3D8_KickOffAndWaitForIdle_1_0_4034,

        XTL::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle"
        #endif
    },
    // IDirect3DDevice8::SetViewport (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
    // IDirect3DDevice8::SetTexture (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
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
    // IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BumpEnv"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TwoSidedLighting (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_BackFillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_BackFillMode"
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
    // IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
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
    // IDirect3DDevice8::SetRenderState_LogicOp (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LogicOp_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LogicOp"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_NormalizeNormals
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_TextureFactor (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
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
    // IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetCreationParameters_1_0_4034,

        XTL::EmuIDirect3DDevice8_GetCreationParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetCreationParameters"
        #endif
    },    
    // IDirect3DDevice8::GetVisibilityTestResult (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetVisibilityTestResult_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetVisibilityTestResult,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetVisibilityTestResult"
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
    // IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_LineWidth
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LineWidth_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LineWidth"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
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
    // IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilCullEnable"
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
    // IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleMask (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMask"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleMode (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode"
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
    // IDirect3DDevice8::SwitchTexture (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SwitchTexture_1_0_4361,

        XTL::EmuIDirect3DDevice8_SwitchTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SwitchTexture"
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
    // IDirect3DDevice8::SetTransform (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
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
    // IDirect3DDevice8::Clear (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_4134,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
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
    // IDirect3DDevice8::SetPalette
    {
        (OOVPA*)&IDirect3DDevice8_SetPalette_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette"
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
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_4432,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
	// IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_4531,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_Swap (4531)"
        #endif
    },
    // IDirect3DDevice8::UpdateOverlay
    {
        (OOVPA*)&IDirect3DDevice8_UpdateOverlay_1_0_4432,

        XTL::EmuIDirect3DDevice8_UpdateOverlay,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_UpdateOverlay"
        #endif
    },
    // IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4432,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },
    // IDirect3DDevice8::SetVerticalBlankCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetVerticalBlankCallback_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVerticalBlankCallback"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVerticesUP (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4134,

        XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVerticesUP"
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
    // IDirect3DDevice8::SetVertexShader (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::DrawVerticesUP (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawVerticesUP_1_0_4361,

        XTL::EmuIDirect3DDevice8_DrawVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // IDirect3DDevice8::SetLight (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::LightEnable (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_4361,

        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
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
    // IDirect3DTexture8::GetSurfaceLevel
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_4432,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel"
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
    // IDirect3DSurface8::LockRect (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_3925,

        XTL::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect"
        #endif
    },
	// IDirect3DDevice8_Release
	{
		(OOVPA*)&IDirect3DDevice8_Release_1_0_4432,

		XTL::EmuIDirect3DDevice8_Release,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_Release"
		#endif
	},
	// IDirect3DDevice8::SetMaterial (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
	// IDirect3DDevice8_GetDeviceCaps (* unchanged since 3925 *)
	{
		(OOVPA*)&IDirect3DDevice8_GetDeviceCaps_1_0_3925, 
			
		XTL::EmuIDirect3DDevice8_GetDeviceCaps,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_GetDeviceCaps"
		#endif
	},
	// IDirect3DDevice8_BackFillMode_4531
	{
		(OOVPA*)&IDirect3DDevice8_BackFillMode_1_0_4531,

		XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_BackFillMode (4531)"
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
	// Get2DSurfacDesc (* unchanged since 4361 *)
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_4361,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },
	// IDirect3DDevice8::SetPixelShaderConstant (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShaderConstant_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetPixelShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderConstant"
        #endif
    },
	// IDirect3DDevice8::DeletePixelShader (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_DeletePixelShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_DeletePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeletePixelShader"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_CullModeB (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullModeB_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullModeB"
        #endif
    },
	// D3DDevice_DeleteVertexShader (* unchanged since 3925 *)
    {
        (OOVPA*)&X_D3DDevice_DeleteVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_DeleteVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteVertexShader"
        #endif
    },
	// IDirect3DDevice8_SetScreenSpaceOffset (* unchanged since 4134 *)
	{
		(OOVPA*)&IDirect3DDevice8_SetScreenSpaceOffset_1_0_4134,

		XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetScreenSpaceOffset"
		#endif
	},
	// D3D::SetFence (XREF) (* unchanged since 4134 *)
    {
        (OOVPA*)&D3D_SetFence_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "D3D::SetFence (XREF)"
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
	// IDirect3DDevice8::GetDisplayMode
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayMode_1_0_4432,

        XTL::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
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
	// IDirect3DVertexBuffer8::Lock (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_4361,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },
	// IDirect3DVertexBuffer8::Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_4531,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DVertexBuffer8_Lock (4531)"
        #endif
    },
	// IDirect3DDevice8::SetShaderConstantMode (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
	// IDirect3DDevice8::SetVertexShaderConstant (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant"
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
	// IDirect3DDevice8::GetViewport (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetViewport_1_0_4361,

        XTL::EmuIDirect3DDevice8_GetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetViewport"
        #endif
    },
	// IDirect3DDevice8::DrawVertices (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_4361,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
	// IDirect3DDevice8::SetFlickerFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetFlickerFilter_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetFlickerFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetFlickerFilter"
        #endif
    },
	// IDirect3DDevice8::SetSoftDisplayFilter
    {
        (OOVPA*)&IDirect3DDevice8_SetSoftDisplayFilter_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSoftDisplayFilter"
        #endif
    },
	// IDirect3DDevice8::SetGammaRamp (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetGammaRamp_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetGammaRamp"
        #endif
    },
	// IDirect3DDevice8::DrawIndexedVertices (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_4361,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
	// IDirect3DDevice8::Begin (* unchanged since 4361 *)
    {
        (OOVPA*)&D3DDevice_Begin_1_0_4361,

        XTL::EmuIDirect3DDevice8_Begin,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Begin"
        #endif
    },
	// IDirect3DDevice8::End (* unchanged since 4361 *)
    {
        (OOVPA*)&D3DDevice_End_1_0_4361,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
        #endif
    },
	// IDirect3DDevice8::SetVertexData2f (* unchanged since 4361 *)
    {
        (OOVPA*)&D3DDevice_SetVertexData2f_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetVertexData2f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2f"
        #endif
    },
    // IDirect3DDevice8::SetVertexData4f (* unchanged since 4361 *)
    {
        (OOVPA*)&D3DDevice_SetVertexData4f_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetVertexData4f,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4f"
        #endif
    },
	// IDirect3DDevice8::SetVertexData4ub (* un`anged since 4361 *)
    {
        (OOVPA*)&D3DDevice_SetVertexData4ub_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetVertexData4ub,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4ub"
        #endif
    },
	// IDirect3DDevice8::SetIndices (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
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
	// Lock2DSurface (* unchanged since 3925 *)
    {
        (OOVPA*)&Lock2DSurface_1_0_3925,

        XTL::EmuLock2DSurface,

        #ifdef _DEBUG_TRACE
        "EmuLock2DSurface"
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
	// IDirect3DDevice8_GetProjectionViewportMatrix
	{
		(OOVPA*)&D3DDevice_GetProjectionViewportMatrix_1_0_4432,

		XTL::EmuIDirect3DDevice8_GetProjectionViewportMatrix,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_GetProjectionViewportMatrix"
		#endif
	},
	// D3DDevice_SetVertexShaderInputDirect
    {
        (OOVPA*)&D3DDevice_SetVertexShaderInput_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetVertexShaderInput,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderInput"
        #endif
    },
	// D3DDevice_SetModelView
	{
		(OOVPA*)&D3DDevice_SetModelView_1_0_4432,
		XTL::EmuIDirect3DDevice8_SetModelView,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetModelView"
		#endif
	},
	// IDirect3DDevice8::BeginPush
    {
        (OOVPA*)&D3DDevice_BeginPush_1_0_4432,

        XTL::EmuIDirect3DDevice8_BeginPush,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginPush"
        #endif
    },
	// IDirect3DDevice8::EndPush
    {
        (OOVPA*)&D3DDevice_EndPush_1_0_4432,

        XTL::EmuIDirect3DDevice8_EndPush,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndPush"
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_4432_SIZE
// ******************************************************************
uint32 D3D8_1_0_4432_SIZE = sizeof(D3D8_1_0_4432);
