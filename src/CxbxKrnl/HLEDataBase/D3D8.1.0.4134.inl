// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4134.cpp
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
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB4, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB5, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB6, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTransform_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTransform+0x16 : lea edi, [ecx+ebx+0x750]
        { 0x16, 0x8D }, // (Offset,Value)-Pair #1
        { 0x17, 0xBC }, // (Offset,Value)-Pair #2
        { 0x18, 0x19 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTransform+0x24 : repe movsd
        { 0x24, 0xF3 }, // (Offset,Value)-Pair #4
        { 0x25, 0xA5 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTransform+0x78 : fld [edx+0x3C]
        { 0x78, 0xD9 }, // (Offset,Value)-Pair #6
        { 0x79, 0x42 }, // (Offset,Value)-Pair #7
        { 0x7A, 0x3C }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTransform+0x92 : jmp +0x10
        { 0x92, 0xEB }, // (Offset,Value)-Pair #9
        { 0x93, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3D8_CheckDeviceFormat
// ******************************************************************
SOOVPA<8> IDirect3D8_CheckDeviceFormat_1_0_4134 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3D8_CheckDeviceFormat+0x0D : retn 0x18
        { 0x0D, 0xC2 }, // (Offset,Value)-Pair #1
        { 0x0E, 0x18 }, // (Offset,Value)-Pair #2

        // IDirect3D8_CheckDeviceFormat+0x15 : jz +0x08
        { 0x15, 0x74 }, // (Offset,Value)-Pair #3
        { 0x16, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3D8_CheckDeviceFormat+0x29 : mov cl, 0x80
        { 0x29, 0xB1 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x80 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CheckDeviceFormat+0x61 : retn 0x18
        { 0x61, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x62, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3D8_GetAdapterModeCount
// ******************************************************************
SOOVPA<12> IDirect3D8_GetAdapterModeCount_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3D8_GetAdapterModeCount+0x25 : add edx, 0xFF
        { 0x25, 0x81 }, // (Offset,Value)-Pair #1
        { 0x26, 0xE2 }, // (Offset,Value)-Pair #2
        { 0x27, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3D8_GetAdapterModeCount+0x47 : cmp edi, 0x04
        { 0x47, 0x83 }, // (Offset,Value)-Pair #4
        { 0x48, 0xFF }, // (Offset,Value)-Pair #5
        { 0x49, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3D8_GetAdapterModeCount+0x95 : shl eax, 0x02
        { 0x95, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x96, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x97, 0x02 }, // (Offset,Value)-Pair #9

        // IDirect3D8_GetAdapterModeCount+0x99 : add esp, 0x08
        { 0x99, 0x83 }, // (Offset,Value)-Pair #10
        { 0x9A, 0xC4 }, // (Offset,Value)-Pair #11
        { 0x9B, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3D8_EnumAdapterModes
// ******************************************************************
SOOVPA<10> IDirect3D8_EnumAdapterModes_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3D8_EnumAdapterModes+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3D8_EnumAdapterModes+0xD1 : and edx, 0x0A
        { 0xD1, 0x83 }, // (Offset,Value)-Pair #4
        { 0xD2, 0xE2 }, // (Offset,Value)-Pair #5
        { 0xD3, 0x0A }, // (Offset,Value)-Pair #6

        // IDirect3D8_EnumAdapterModes+0xF5 : mov dword ptr[edi+0x10], 0x1E
        { 0xF5, 0xC7 }, // (Offset,Value)-Pair #7
        { 0xF6, 0x47 }, // (Offset,Value)-Pair #8
        { 0xF7, 0x10 }, // (Offset,Value)-Pair #9
        { 0xF8, 0x1E }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_LoadVertexShader_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_FogColor_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_FogColor+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_FogColor+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #3
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_FogColor+0x25 : shl edi, 0x10
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x26, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x27, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_FogColor+0x40 : mov [esi], eax
        { 0x40, 0x89 }, // (Offset,Value)-Pair #9
        { 0x41, 0x06 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap_1_0_4134 =
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

        // IDirect3DDevice8_Swap+0x46 : inc dword ptr [esi+0x2ABC]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xBC }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
LOOVPA<9> IDirect3DDevice8_Clear_1_0_4134 =
{
    1,  // Large == 1
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_Clear+0x000B : mov ebx, [ebp+0x2074]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0x74 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x20 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x039A, 0x18 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetBackBuffer_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

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

        // IDirect3DDevice8_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x207C]
        { 0x1D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1E, 0x84 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x20, 0x7C }, // (Offset,Value)-Pair #11
        { 0x21, 0x20 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_GetDepthStencilSurface_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

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

        // IDirect3DDevice8_GetDepthStencilSurface+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetDepthStencilSurface+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderTarget_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderTarget+0x00 : sub esp, 0x3C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x3C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderTarget+014 : jnz +0x0C
        { 0x14, 0x75 }, // (Offset,Value)-Pair #4
        { 0x15, 0x0C }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderTarget+0x44 : push ebp
        { 0x44, 0x55 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderTarget+0x79 : shr ecx, 0x14
        { 0x79, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x7A, 0xE9 }, // (Offset,Value)-Pair #8
        { 0x7B, 0x14 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SelectVertexShader
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SelectVertexShader_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SelectVertexShader+0x13 : mov [esi+0x384], eax
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x86 }, // (Offset,Value)-Pair #2
        { 0x15, 0x84 }, // (Offset,Value)-Pair #3
        { 0x16, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SelectVertexShader+0x3B : mov dword ptr [eax], 0x00081E94
        { 0x3B, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x94 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x1E }, // (Offset,Value)-Pair #8
        { 0x3F, 0x08 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SelectVertexShader+0x55 : mov [esi], eax
        { 0x55, 0x89 }, // (Offset,Value)-Pair #10
        { 0x56, 0x06 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_CopyRects
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_CopyRects_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_CopyRects+0x00 : sub esp, 0x01A0
        { 0x00, 0x81 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0xA0 }, // (Offset,Value)-Pair #3
        { 0x03, 0x01 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CopyRects+0x6E : mov esi, [eax+0x201C]
        { 0x6E, 0x8B }, // (Offset,Value)-Pair #5
        { 0x6F, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x70, 0x1C }, // (Offset,Value)-Pair #7
        { 0x71, 0x20 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CopyRects+0xDD : jnz +0x22
        { 0xDD, 0x75 }, // (Offset,Value)-Pair #9
        { 0xDE, 0x22 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateImageSurface
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_CreateImageSurface_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetVertexShaderConstant_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexShaderConstant+0x11 : mov cl, [ebx+0x08]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x4B }, // (Offset,Value)-Pair #2
        { 0x13, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant+0x1A : test cl, 0x10
        { 0x1A, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x1B, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x10 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVertexShaderConstant+0x60 : jle +0x19
        { 0x60, 0x7E }, // (Offset,Value)-Pair #7
        { 0x61, 0x19 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant+0x98 : retn 0x0C
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x99, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetPixelShader_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetPixelShader+0x0E : mov esi, [ebx+0x0370]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xB3 }, // (Offset,Value)-Pair #2
        { 0x10, 0x70 }, // (Offset,Value)-Pair #3
        { 0x11, 0x03 }, // (Offset,Value)-Pair #4

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
// * IDirect3DDevice8_SetTextureState_BumpEnv
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTextureState_BumpEnv+0x18 : jnz +0x03
        { 0x18, 0x75 }, // (Offset,Value)-Pair #1
        { 0x19, 0x03 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x32 : mov ecx, [esp+0x14]
        { 0x32, 0x8B }, // (Offset,Value)-Pair #6
        { 0x33, 0x4C }, // (Offset,Value)-Pair #7
        { 0x34, 0x24 }, // (Offset,Value)-Pair #8
        { 0x35, 0x18 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetTextureState_BumpEnv+0x50 : shl esi, 5
        { 0x50, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x51, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x52, 0x05 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetIndices_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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

        // IDirect3DDevice8_SetIndices+0x76 : mov [esi+0x038C], edi
        { 0x76, 0x89 }, // (Offset,Value)-Pair #8
        { 0x77, 0xBE }, // (Offset,Value)-Pair #9
        { 0x78, 0x8C }, // (Offset,Value)-Pair #10
        { 0x79, 0x03 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_VertexBlend
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_VertexBlend+0x06 : or edx, 0x200
        { 0x06, 0x81 }, // (Offset,Value)-Pair #1
        { 0x07, 0xCA }, // (Offset,Value)-Pair #2
        { 0x08, 0x00 }, // (Offset,Value)-Pair #3
        { 0x09, 0x02 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x1E : jb +0x05
        { 0x1E, 0x72 }, // (Offset,Value)-Pair #5
        { 0x1F, 0x05 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x2F : mov [eax+4], ecx
        { 0x2F, 0x89 }, // (Offset,Value)-Pair #7
        { 0x30, 0x48 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x3E : retn 0x04
        { 0x3E, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x3F, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x0370]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x03 }, // (Offset,Value)-Pair #4

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
// * IDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B }, // (Offset,Value)-Pair #5
        { 0x14, 0x4C }, // (Offset,Value)-Pair #6
        { 0x15, 0x24 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x2F : retn 0x04
        { 0x2F, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x30, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x96 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x20 }, // (Offset,Value)-Pair #6

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
// * IDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,
    {
        // IDirect3DDevice8_SetRenderState_StencilEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_StencilEnable+0x2C : jz +0x19
        { 0x2C, 0x74 }, // (Offset,Value)-Pair #3
        { 0x2D, 0x19 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_StencilEnable+0x4D : mov dword ptr [eax], 0x00041D84
        { 0x4D, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x4E, 0x00 }, // (Offset,Value)-Pair #6
        { 0x4F, 0x84 }, // (Offset,Value)-Pair #7
        { 0x50, 0x1D }, // (Offset,Value)-Pair #8
        { 0x51, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_StencilEnable+0x81 : retn 0x04
        { 0x81, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x82, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x05 : mov esi, dword ptr [addr]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x35 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x0B : mov ecx, esi
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x0C, 0xCE }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #6
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x38 : shl ecx, 0x10
        { 0x38, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x39, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x50, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetTransform
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetTransform_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetTransform+0x0A : shl ecx, 0x06
        { 0x0A, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0B, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x06 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetTransform+0x12 : lea esi, [ecx+eax+*numb*]
        { 0x12, 0x8D }, // (Offset,Value)-Pair #4
        { 0x13, 0xB4 }, // (Offset,Value)-Pair #5
        { 0x14, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetTransform+0x19 : mov ecx, 0x10
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x1A, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetTransform+0x20 : pop edi, pop esi
        { 0x20, 0x5F }, // (Offset,Value)-Pair #9
        { 0x21, 0x5E }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetTransform+0x22 : retn 0x08
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x23, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetStreamSource_1_0_4134 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetVertexShader_1_0_4134 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShader+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0x80 }, // (Offset,Value)-Pair #6
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_DrawVerticesUP_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawVerticesUP+0x2F : mov edx, [edi+0x744]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x97 }, // (Offset,Value)-Pair #5
        { 0x31, 0x44 }, // (Offset,Value)-Pair #6
        { 0x32, 0x07 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawVerticesUP+0xC1 : cmp esi, 0x80
        { 0xC1, 0x81 }, // (Offset,Value)-Pair #8
        { 0xC2, 0xFE }, // (Offset,Value)-Pair #9
        { 0xC3, 0x80 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetLight_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetLight+0x11 : mov edi, [ebp+0x394]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #1
        { 0x12, 0xBD }, // (Offset,Value)-Pair #2
        { 0x13, 0x94 }, // (Offset,Value)-Pair #3
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetMaterial_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetMaterial+0x0C : add edi, 0x0940
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xF0 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x09 }, // (Offset,Value)-Pair #4

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
// * IDirect3DDevice8_LightEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_LightEnable_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_LightEnable+0x0F : cmp ebx, [esi+0x0394]
        { 0x0F, 0x3B }, // (Offset,Value)-Pair #1
        { 0x10, 0x9E }, // (Offset,Value)-Pair #2
        { 0x11, 0x94 }, // (Offset,Value)-Pair #3
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
    }
};

// ******************************************************************
// * IDirect3DSurface8_GetDesc
// ******************************************************************
SOOVPA<15> IDirect3DSurface8_GetDesc_1_0_4134 =
{
    0,  // Large == 0
    15, // Count == 15

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DSurface8_GetDesc+0x00 : mov eax, [esp+0x08]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3DSurface8_GetDesc+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DSurface8_GetDesc+0x08 : push eax
        { 0x08, 0x50 }, // (Offset,Value)-Pair #9

        // IDirect3DSurface8_GetDesc+0x09 : push 0
        { 0x09, 0x6A }, // (Offset,Value)-Pair #10
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #11

        // IDirect3DSurface8_GetDesc+0x0B : push ecx
        { 0x0B, 0x51 }, // (Offset,Value)-Pair #12

        // IDirect3DSurface8_GetDesc+0x0C : call [abs]
        { 0x0C, 0xE8 }, // (Offset,Value)-Pair #13

        // IDirect3DSurface8_GetDesc+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x12, 0x08 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4134 =
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
// * Get2DSurfaceDesc
// ******************************************************************
SOOVPA<10> Get2DSurfaceDesc_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21C0]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0x7C }, // (Offset,Value)-Pair #7
        { 0x55, 0x20 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_Reset
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_Reset_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Reset+0x15 : mov ecx, ebp
        { 0x15, 0x8B }, // (Offset,Value)-Pair #1
        { 0x16, 0xCD }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Reset+0x1E : jnz +0xF5
        { 0x1E, 0x75 }, // (Offset,Value)-Pair #3
        { 0x1F, 0xF5 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_Reset+0x27 : mov esi, [esp+0x14]
        { 0x27, 0x8B }, // (Offset,Value)-Pair #5
        { 0x28, 0x74 }, // (Offset,Value)-Pair #6
        { 0x29, 0x24 }, // (Offset,Value)-Pair #7
        { 0x2A, 0x14 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_Reset+0x37 : jge +0x10
        { 0x37, 0x7D }, // (Offset,Value)-Pair #9
        { 0x38, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4134 =
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
// * IDirect3DDevice8_SetRenderState_BackFillMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_BackFillMode+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x35 : mov dword ptr [eax], abs
        { 0x35, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x36, 0x00 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x3B : add eax, 0x0C; mov [esi], eax; mov [abs], edi
        { 0x41, 0x83 }, // (Offset,Value)-Pair #5
        { 0x42, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x43, 0x0C }, // (Offset,Value)-Pair #7
        { 0x44, 0x89 }, // (Offset,Value)-Pair #8
        { 0x45, 0x06 }, // (Offset,Value)-Pair #9
        { 0x46, 0x89 }, // (Offset,Value)-Pair #10
        { 0x47, 0x3D }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x4E : retn 0x04
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_ColorKeyColor
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134 =
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
// * IDirect3DDevice8_SetRenderState_FrontFace
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        // IDirect3DDevice8_SetRenderState_FrontFace+0x36 : jmp IDirect3DDevice8_SetRenderState_CullMode
        { 0x36, XREF_DXSRSCULLMODE}, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_FrontFace+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_FrontFace+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B }, // (Offset,Value)-Pair #5
        { 0x14, 0x4C }, // (Offset,Value)-Pair #6
        { 0x15, 0x24 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_FrontFace+0x20 : add eax, 8
        { 0x20, 0x83 }, // (Offset,Value)-Pair #9
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x22, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_LogicOp
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_LogicOp_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_LogicOp+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_LogicOp+0x1B : mov dword ptr [eax], 0x417BC
        { 0x1B, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x1C, 0x00 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xBC }, // (Offset,Value)-Pair #5
        { 0x1E, 0x17 }, // (Offset,Value)-Pair #6
        { 0x1F, 0x04 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_LogicOp+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x50, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilFail
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_StencilFail+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_StencilFail+0x38 : cmp [abs], 0x1E00
        { 0x38, 0x81 }, // (Offset,Value)-Pair #3
        { 0x3E, 0x00 }, // (Offset,Value)-Pair #4
        { 0x3F, 0x1E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_StencilFail+0x47 : mov dword ptr [eax], 0x41D84
        { 0x47, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x48, 0x00 }, // (Offset,Value)-Pair #7
        { 0x49, 0x84 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x1D }, // (Offset,Value)-Pair #9
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_StencilFail+0x6A : retn 0x04
        { 0x6A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6B, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #1
        { 0x16, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 }, // (Offset,Value)-Pair #3
        { 0x47, 0x00 }, // (Offset,Value)-Pair #4
        { 0x48, 0x1E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x51, 0x00 }, // (Offset,Value)-Pair #7
        { 0x52, 0x84 }, // (Offset,Value)-Pair #8
        { 0x53, 0x1D }, // (Offset,Value)-Pair #9
        { 0x54, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x60, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilCullEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        // IDirect3DDevice8_SetRenderState_StencilCullEnable+0x0C : D3D__RenderState[D3DRS_STENCILCULLENABLE]
        { 0x0C, XREF_D3DRS_STENCILCULLENABLE }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_StencilCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #2
        { 0x16, 0x05 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_StencilCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 }, // (Offset,Value)-Pair #4
        { 0x47, 0x00 }, // (Offset,Value)-Pair #5
        { 0x48, 0x1E }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_StencilCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x51, 0x00 }, // (Offset,Value)-Pair #8
        { 0x52, 0x84 }, // (Offset,Value)-Pair #9
        { 0x53, 0x1D }, // (Offset,Value)-Pair #10
        { 0x54, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_StencilCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x60, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FillMode_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_FillMode+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_FillMode+0x21 : mov ecx, [esp+0x08]
        { 0x21, 0x8B }, // (Offset,Value)-Pair #3
        { 0x22, 0x4C }, // (Offset,Value)-Pair #4
        { 0x23, 0x24 }, // (Offset,Value)-Pair #5
        { 0x24, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_FillMode+0x25 : jnz +0x02
        { 0x25, 0x75 }, // (Offset,Value)-Pair #7
        { 0x26, 0x02 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_FillMode+0x35 : add eax, 0x0C
        { 0x35, 0x83 }, // (Offset,Value)-Pair #9
        { 0x36, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x37, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134 =
{
    0, // Large == 0
    9, // Count == 9

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        // IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        { 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_RopZRead
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134 =
{
    0, // Large == 0
    9, // Count == 9

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        // IDirect3DDevice8_SetRenderState_RopZRead+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        { 0x05, XREF_D3DRS_ROPZREAD }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_RopZRead+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_RopZRead+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_RopZRead+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_DoNotCullUncompressed
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134 =
{
    0, // Large == 0
    9, // Count == 9

    XRefNotSaved,
    1,  // XRef Is  Used

    {
        // IDirect3DDevice8_SetRenderState_DoNotCullUncompressed+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        { 0x05, XREF_D3DRS_DONOTCULLUNCOMPRESSED }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_DoNotCullUncompressed+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_DoNotCullUncompressed+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_DoNotCullUncompressed+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x0C : mov eax, [edi+0x2070]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0D, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x70 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x6E : add eax, 0x18
        { 0x6E, 0x83 }, // (Offset,Value)-Pair #5
        { 0x6F, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x70, 0x18 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x7C : retn 0x04
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x7D, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleMask
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x1C : jb +0x05
        { 0x1C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x38 : and ecx, 0x0F; shl edx, 0x10
        { 0x38, 0x83 }, // (Offset,Value)-Pair #3
        { 0x39, 0xE1 }, // (Offset,Value)-Pair #4
        { 0x3A, 0x0F }, // (Offset,Value)-Pair #5
        { 0x3B, 0xC1 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x3E : mov dword ptr [eax], 0x41D7C
        { 0x3E, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x00 }, // (Offset,Value)-Pair #8
        { 0x40, 0x7C }, // (Offset,Value)-Pair #9
        { 0x41, 0x1D }, // (Offset,Value)-Pair #10
        { 0x42, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x4F : retn 0x04
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x50, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x70 }, // (Offset,Value)-Pair #3
        { 0x12, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0x7C }, // (Offset,Value)-Pair #7
        { 0x18, 0x20 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x1B : jnz +0x0D
        { 0x1B, 0x75 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_4134 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0x70 }, // (Offset,Value)-Pair #3
        { 0x12, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B }, // (Offset,Value)-Pair #5
        { 0x16, 0x88 }, // (Offset,Value)-Pair #6
        { 0x17, 0x7C }, // (Offset,Value)-Pair #7
        { 0x18, 0x20 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode+0x1B : jz +0x0D
        { 0x1B, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1C, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x46 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #5
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x1D : lea edx, [ecx-0x0200]
        { 0x1D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x1E, 0x91 }, // (Offset,Value)-Pair #8
        { 0x1F, 0x00 }, // (Offset,Value)-Pair #9
        { 0x20, 0xFE }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x26 : add eax, 0x08
        { 0x26, 0x83 }, // (Offset,Value)-Pair #11
        { 0x27, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x28, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

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

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x15 : jb +0x05
        { 0x15, 0x72 }, // (Offset,Value)-Pair #10
        { 0x16, 0x05 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x28 : retn 0x04
        { 0x28, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x29, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x6C4]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x97 }, // (Offset,Value)-Pair #6
        { 0x5E, 0xC4 }, // (Offset,Value)-Pair #7
        { 0x5F, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPixelShaderConstant
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetPixelShaderConstant_1_0_4134 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetPixelShaderConstant+0x17 : test eax, eax
        { 0x17, 0x85 }, // (Offset,Value)-Pair #1
        { 0x18, 0xC0 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetPixelShaderConstant+0x32 : add edi, 0x04
        { 0x32, 0x83 }, // (Offset,Value)-Pair #3
        { 0x33, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x34, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPixelShaderConstant+0x47 : jmp +0x07
        { 0x47, 0xEB }, // (Offset,Value)-Pair #6
        { 0x48, 0x07 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetPixelShaderConstant+0xC7 : fstp [esp+??]
        { 0xC7, 0xD9 }, // (Offset,Value)-Pair #8
        { 0xC8, 0x1C }, // (Offset,Value)-Pair #9
        { 0xC9, 0x24 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_DeletePixelShader_1_0_4134 =
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
// * IDirect3DDevice8_SetRenderState_CullModeB
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_CullModeB_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_DXSRSCULLMODE, // XRef Is Saved
    0,                  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_CullModeB+0x22 : mov dword ptr [eax], 0x40308
        { 0x22, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x24, 0x08 }, // (Offset,Value)-Pair #2
        { 0x25, 0x03 }, // (Offset,Value)-Pair #3
        { 0x26, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_CullModeB+0x2D : add eax, 8
        { 0x2D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x2E, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x2F, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_CullModeB+0x39 : retn 4
        { 0x39, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x3A, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullModeB+0x5C : add edx, 0x404
        { 0x5C, 0x81 }, // (Offset,Value)-Pair #10
        { 0x5D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x5E, 0x04 }, // (Offset,Value)-Pair #12
        { 0x5F, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
SOOVPA<6> X_D3DDevice_DeleteVertexShader_1_0_4134 =
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0xFF },
        { 0x0A, 0x08 },
        { 0x0E, 0x44 },
        { 0x16, 0xC2 },
        { 0x17, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetScreenSpaceOffset
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetScreenSpaceOffset_1_0_4134 = 
{
	0,	// Large == 0
	7,	// Count == 7

	XRefNotSaved,
	XRefNotUsed,

	{
		{ 0x06, 0x56 },
		{ 0x07, 0xD8 },
		{ 0x08, 0x44 },
		{ 0x09, 0x24 },
		{ 0x0A, 0x08 },
		{ 0x2E, 0x8B },
		{ 0x2F, 0x06 },
	}
};

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
SOOVPA<12> D3D_SetFence_1_0_4134 =
{
    0,   // Large == 0
    12,  // Count == 12

    XREF_D3D_SETFENCE, // Xref Is Saved
    0,                 // Xref Not Used

    {
		{ 0x0D, 0x72 },
		{ 0x0E, 0x0E },
		{ 0x31, 0xBA },
		{ 0x32, 0x90 },
		{ 0x33, 0x1D },
		{ 0x34, 0x04 },
		{ 0x35, 0x00 },
		{ 0x45, 0x83 },
		{ 0x46, 0xE1 },
		{ 0x47, 0x3F },
        { 0xAA, 0xC2 },
		{ 0xAB, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_LineWidth_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0C, 0x89 },
        { 0x18, 0x04 },
        { 0x28, 0xD9 },
        { 0x32, 0x81 },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0x06 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4134 =
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

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x2430
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x30 }, // (Offset,Value)-Pair #8
        { 0x19, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetSwapCallback
// ******************************************************************
/* This may look the same as 4361, but accuracy is not gauranteed.
   It might be a SetVerticalBlankCallback that has an identical signature... */
SOOVPA<12> IDirect3DDevice8_SetSwapCallback_1_0_4134 =
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

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x2C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetTile
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetTile_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetTile+0x13 : lea esi, [ecx+eax*8+0x211C]
        { 0x13, 0x8D }, // (Offset,Value)-Pair #1
        { 0x14, 0xB4 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x16, 0x1C }, // (Offset,Value)-Pair #4
        { 0x17, 0x21 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetTile+0x21 : pop edi; pop esi
        { 0x21, 0x5F }, // (Offset,Value)-Pair #8
        { 0x22, 0x5E }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x24, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTile
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetTile_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTileNoWait+0x00 : sub esp, 0x18
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x18 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTileNoWait+0x16 : cmp [esi+4], eax
        { 0x16, 0x39 }, // (Offset,Value)-Pair #4
        { 0x17, 0x46 }, // (Offset,Value)-Pair #5
        { 0x18, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTileNoWait+0x3D : lea edi, [edx+ecx*8+0x211C]
        { 0x3D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x3E, 0xBC }, // (Offset,Value)-Pair #8
        { 0x3F, 0xCB }, // (Offset,Value)-Pair #9
        { 0x40, 0x1C }, // (Offset,Value)-Pair #10
        { 0x41, 0x21 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
SOOVPA<7> D3DDevice_IsBusy_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x05, 0x8B },
        { 0x0C, 0x90 },
        { 0x13, 0x44 },
        { 0x1A, 0xFF },
        { 0x21, 0x01 },
        { 0x28, 0x00 },
        { 0x2F, 0xD2 },
    }
};

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
SOOVPA<12> D3D_CDevice_KickOff_1_0_4134 = 
{
	0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    0,  // Xref Is  Used

    {
		// D3D::CDevice::KickOff+0x0D : test al, 4
		{ 0x0D, 0xA8 },
		{ 0x0E, 0x04 },
		// D3D::CDevice::KickOff+0x11 : mov ecx, [ecx+0x35C]
		{ 0x11, 0x8B },
		{ 0x12, 0x89 },
		{ 0x13, 0x5C },
		{ 0x14, 0x03 },
		{ 0x15, 0x00 },
		{ 0x16, 0x00 },
		// D3D::CDevice::KickOff+0x26 : sfence
		{ 0x26, 0x0F },
		{ 0x27, 0xAE },
		{ 0x28, 0xF8 },
		// D3D::CDevice::KickOff+0xE3 : retn
		{ 0xE3, 0xC3 }
	}
};

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
SOOVPA<7> D3DDevice_BlockOnFence_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
SOOVPA<8> D3DDevice_GetRenderTarget_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x0A, 0x00 },
        { 0x0E, 0x4C },
        { 0x12, 0x01 },
        { 0x16, 0xE8 },
        { 0x1B, 0x33 },
        { 0x1E, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Release
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Release_1_0_4134 =
{
    0,  // Large == 0
    11, // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Release+0x07 : mov eax, [edi+0x043C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x87 }, // (Offset,Value)-Pair #2
        { 0x09, 0x3C }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_Release+0x0D : cmp eax, 1
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Release+0x1B : mov ecx, 0x0AB8
        { 0x1B, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x1D, 0x0A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_Release+0x2E : dec eax
        { 0x2E, 0x48 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDisplayMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_GetDisplayMode_1_0_4134 =
{
    0,  // Large == 0
    13, // Count == 13

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
        { 0x6D, 0x0A }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_GetDisplayMode+0x92 : retn 0x04
        { 0x92, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x93, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
SOOVPA<7> D3DDevice_SetShaderConstantMode_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0xC7 },
    }
};

// ******************************************************************
// * D3D8_1_0_4134
// ******************************************************************
OOVPATable D3D8_1_0_4134[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 1.0.3925 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
	// IDirect3D8::CheckDeviceFormat
    {
        (OOVPA*)&IDirect3D8_CheckDeviceFormat_1_0_4134,

        XTL::EmuIDirect3D8_CheckDeviceFormat,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CheckDeviceFormat"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex"
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
    // IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_4134,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_4134,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
        #endif
    },


    // ********************** BEG WARNING UNTESTED!!! *******************


    // IDirect3DDevice8::GetDeviceCaps
    {
        (OOVPA*)&IDirect3DDevice8_GetDeviceCaps_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetDeviceCaps,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDeviceCaps"
        #endif
    },
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
    // IDirect3DDevice8::LoadVertexShader (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_4034,

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
    /*{
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_3925,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },*/
    // IDirect3DDevice8::SetVerticalBlankCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetVerticalBlankCallback_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVerticalBlankCallback"
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
   /* {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },*/
    // IDirect3DDevice8::SetPixelShader
    /*{
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },*/
    // IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
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
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
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
    // IDirect3DDevice8::CreateTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture"
        #endif
    },
    // IDirect3DDevice8::CreateVolumeTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateVolumeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVolumeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVolumeTexture"
        #endif
    },
    // IDirect3DDevice8::CreateCubeTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateCubeTexture_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateCubeTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateCubeTexture"
        #endif
    },
    // IDirect3DDevice8::CreateIndexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateIndexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateIndexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateIndexBuffer"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
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
    /*{
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
        #endif
    },*/
    // IDirect3DDevice8::SetRenderState_Simple
    /*{
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple"
        #endif
    },*/
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode
    /*{
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },*/
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
    // IDirect3DDevice8::SetRenderState_ZBias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZBias_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZBias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZBias"
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
    /*{
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },*/
    // IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock
   /* {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_3925,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },*/
    // IDirect3DResource8::Register
    {
        (OOVPA*)&IDirect3DResource8_Register_1_0_3925,

        XTL::EmuIDirect3DResource8_Register,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Register"
        #endif
    },
    // IDirect3DResource8::Release
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_3925,

        XTL::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release"
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
    // IDirect3DResource8::IsBusy
    {
        (OOVPA*)&IDirect3DResource8_IsBusy_1_0_3925,

        XTL::EmuIDirect3DResource8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_IsBusy"
        #endif
    },
    // IDirect3DPalette8::Lock
    {
        (OOVPA*)&IDirect3DPalette8_Lock_1_0_3925,

        XTL::EmuIDirect3DPalette8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DPalette8_Lock"
        #endif
    },
    // IDirect3DTexture8::LockRect
    {
        (OOVPA*)&IDirect3DTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_LockRect"
        #endif
    },
    // IDirect3DVolumeTexture8::LockBox
    {
        (OOVPA*)&IDirect3DVolumeTexture8_LockBox_1_0_3925,

        XTL::EmuIDirect3DVolumeTexture8_LockBox,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVolumeTexture8_LockBox"
        #endif
    },
    // IDirect3DCubeTexture8::LockRect
    {
        (OOVPA*)&IDirect3DCubeTexture8_LockRect_1_0_3925,

        XTL::EmuIDirect3DCubeTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DCubeTexture8_LockRect"
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_3925,

        XTL::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel"
        #endif
    },
    // Get2DSurfacDesc
    /*{
        (OOVPA*)&Get2DSurfaceDesc_1_0_3925,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },*/

    // ********************** END WARNING UNTESTED!!! *******************

	// IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface"
        #endif
    },
	
    // IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget"
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
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_3925,

        XTL::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect"
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
	// IDirect3DDevice8::LoadVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader"
        #endif
    },
    // IDirect3DDevice8::SelectVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SelectVertexShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_SelectVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShader"
        #endif
    },
	// IDirect3DDevice8::CopyRects
    {
        (OOVPA*)&IDirect3DDevice8_CopyRects_1_0_4134,

        XTL::EmuIDirect3DDevice8_CopyRects,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CopyRects"
        #endif
    },
	// IDirect3DDevice8::CreateImageSurface 
    {
        (OOVPA*)&IDirect3DDevice8_CreateImageSurface_1_0_4034,

        XTL::EmuIDirect3DDevice8_CreateImageSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateImageSurface"
        #endif
    },
	// IDirect3DDevice8::SetVertexShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant"
        #endif
    },
	// IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
	 // IDirect3DDevice8::SetTextureState_BumpEnv
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BumpEnv"
        #endif
    },
	// IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
        #endif
    },
    // IDirect3DDevice8::SetTexture (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_VertexBlend
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_EdgeAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_4034,

        XTL::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
    },
    // IDirect3DDevice8::GetTransform
    {
        (OOVPA*)&IDirect3DDevice8_GetTransform_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTransform"
        #endif
    },
    // IDirect3DDevice8::SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::DrawVertices (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_4034,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3DDevice8::DrawVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawVerticesUP_1_0_4134,

        XTL::EmuIDirect3DDevice8_DrawVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_4034,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::SetLight
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::LightEnable
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock (* unchanged since 4034 *)
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_4034,

        XTL::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock"
        #endif
    },
	// Get2DSurfacDesc
    {
        (OOVPA*)&Get2DSurfaceDesc_1_0_4134,

        XTL::EmuGet2DSurfaceDesc,

        #ifdef _DEBUG_TRACE
        "EmuGet2DSurfaceDesc"
        #endif
    },
    // IDirect3DSurface8::GetDesc
    {
        (OOVPA*)&IDirect3DSurface8_GetDesc_1_0_4134,

        XTL::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc"
        #endif
    },
	// IDirect3DDevice8_SetRenderState_StencilFail
	{
		(OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134,

		XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_StencilFail"
		#endif
	},
	// IDirect3DDevice8_SetRenderState_NormalizeNormals
	{
		(OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4134,

		XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
		#endif
	},
	// IDirect3DDevice8::Reset
    {
        (OOVPA*)&IDirect3DDevice8_Reset_1_0_4134,

        XTL::EmuIDirect3DDevice8_Reset,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Reset"
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
	// IDirect3DDevice8::SetTextureState_TwoSidedLighting
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
	// IDirect3DDevice8::SetTextureState_ColorKeyColor
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_FrontFace
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FrontFace"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_LogicOp
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LogicOp_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LogicOp"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_StencilFail 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_StencilCullEnable 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilCullEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_FillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_RopZRead
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_DoNotCullUncompressed
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleMask
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMask"
        #endif
    },
	 // IDirect3DDevice8::SetRenderState_MultiSampleMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMode"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_ShadowFunc
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_YuvEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
	// IDirect3DDevice8::DrawIndexedVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4134,

        XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVerticesUP"
        #endif
    },
	// IDirect3DDevice8::SetPixelShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShaderConstant_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetPixelShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderConstant"
        #endif
    },
	// IDirect3DDevice8::DeletePixelShader
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
	// D3DDevice_DeleteVertexShader (* unchanged since 4134 *)
    {
        (OOVPA*)&X_D3DDevice_DeleteVertexShader_1_0_4134,

        XTL::EmuIDirect3DDevice8_DeleteVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteVertexShader"
        #endif
    },
	// IDirect3DDevice8_SetScreenSpaceOffset
	{
		(OOVPA*)&IDirect3DDevice8_SetScreenSpaceOffset_1_0_4134,

		XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetScreenSpaceOffset"
		#endif
	},
	// D3D::SetFence (XREF)
    {
        (OOVPA*)&D3D_SetFence_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "D3D::SetFence (XREF)"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_LineWidth
    {
        (OOVPA*)&D3DDevice_SetRenderState_LineWidth_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LineWidth"
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
	// IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4134,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },
	// IDirect3DDevice8::SetSwapCallback
    {
        (OOVPA*)&IDirect3DDevice8_SetSwapCallback_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetSwapCallback,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetSwapCallback"
        #endif
    },
	// IDirect3DDevice8::GetTile
    {
        (OOVPA*)&IDirect3DDevice8_GetTile_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetTile,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTile"
        #endif
    },
	// IDirect3DDevice8::SetTile
    {
        (OOVPA*)&IDirect3DDevice8_SetTile_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetTileNoWait,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTile"
        #endif
    },
	// IDirect3DDevice8::IsBusy
    {
        (OOVPA*)&D3DDevice_IsBusy_1_0_4134,

        XTL::EmuIDirect3DDevice8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_IsBusy"
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
	// D3D::CDevice::KickOff
	{
		(OOVPA*)&D3D_CDevice_KickOff_1_0_4134, 
			
		XTL::EmuD3DDevice_KickOff,

		#ifdef _DEBUG_TRACE
		"D3D::CDevice::KickOff"
		#endif
	},
	// IDirect3DDevice8::InsertFence (* unchanged since 3925 *)
    {
        (OOVPA*)&D3DDevice_InsertFence_1_0_3925,

        XTL::EmuIDirect3DDevice8_InsertFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_InsertFence"
        #endif
    },
	// IDirect3DDevice8::BlockOnFence
    {
        (OOVPA*)&D3DDevice_BlockOnFence_1_0_4134,

        XTL::EmuIDirect3DDevice8_BlockOnFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockOnFence"
        #endif
    },
	// IDirect3DDevice8::GetRenderTarget
    {
		(OOVPA*)&D3DDevice_GetRenderTarget_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget"
        #endif
    },
	// IDirect3DDevice8::Release
    {
        (OOVPA*)&IDirect3DDevice8_Release_1_0_4134,

        XTL::EmuIDirect3DDevice8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Release"
        #endif
    },
	// IDirect3DDevice8::GetDisplayMode
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
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
	// D3DDevice_SetShaderConstantMode
	{
        (OOVPA*)&D3DDevice_SetShaderConstantMode_1_0_4134,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
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
};

// ******************************************************************
// * D3D8_1_0_4134_SIZE
// ******************************************************************
uint32 D3D8_1_0_4134_SIZE = sizeof(D3D8_1_0_4134);
