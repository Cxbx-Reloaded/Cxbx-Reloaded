// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4361.cpp
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
SOOVPA<8> IDirect3D8_CreateDevice_1_0_4361 =
{
    0,  // Large == 0
    8,  // Count == 8

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
// * IDirect3D8_GetAdapterDisplayMode
// ******************************************************************
SOOVPA<13> IDirect3D8_GetAdapterDisplayMode_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3D8_GetAdapterDisplayMode+0x08 : mov eax, 0x8876086C
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x09, 0x6C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x76 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #5

        // IDirect3D8_GetAdapterDisplayMode+0x18 : jnz +0x17
        { 0x18, 0x75 }, // (Offset,Value)-Pair #6
        { 0x19, 0x17 }, // (Offset,Value)-Pair #7

        // IDirect3D8_GetAdapterDisplayMode+0x31 : mov ecx, [edx+0x2080]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #8
        { 0x32, 0x8A }, // (Offset,Value)-Pair #9
        { 0x33, 0x80 }, // (Offset,Value)-Pair #10
        { 0x34, 0x20 }, // (Offset,Value)-Pair #11

        // IDirect3D8_GetAdapterDisplayMode+0xBD : retn 0x08
        { 0xBD, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xBE, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_CopyRects
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_CopyRects_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

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
SOOVPA<9> IDirect3DDevice8_CreateImageSurface_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

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
SOOVPA<12> IDirect3DDevice8_GetBackBuffer_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

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
// * IDirect3DDevice8_GetRenderTarget
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_GetRenderTarget_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    {
        // IDirect3DDevice8_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x70 }, // (Offset,Value)-Pair #4
        { 0x08, 0x20 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetRenderTarget+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_GetDepthStencilSurface_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

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
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreateVertexShader_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

    {
        // IDirect3DDevice8_CreateVertexShader+0x00 : push ecx; push ebx; push ebp
        { 0x00, 0x51 }, // (Offset,Value)-Pair #1
        { 0x01, 0x53 }, // (Offset,Value)-Pair #2
        { 0x02, 0x55 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_CreateVertexShader+0x0A : jz +0x10
        { 0x0A, 0x74 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x10 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateVertexShader+0x43 : shl eax, 0x02
        { 0x43, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x44, 0xE0 }, // (Offset,Value)-Pair #7
        { 0x45, 0x02 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateVertexShader+0x66 : retn 0x10
        { 0x66, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x67, 0x10 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateVertexShader+0x73 : rep stosb
        { 0x73, 0xF3 }, // (Offset,Value)-Pair #11
        { 0x74, 0xAB }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetVertexShaderConstant_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

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
// * IDirect3DDevice8_CreatePixelShader
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_CreatePixelShader_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_CreatePixelShader+0x00 : push 0xFC
        { 0x00, 0x68 }, // (Offset,Value)-Pair #1
        { 0x01, 0xFC }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreatePixelShader+0x10 : mov eax, 0x8007000E
        { 0x10, 0xB8 }, // (Offset,Value)-Pair #3
        { 0x11, 0x0E }, // (Offset,Value)-Pair #4
        { 0x12, 0x00 }, // (Offset,Value)-Pair #5
        { 0x13, 0x07 }, // (Offset,Value)-Pair #6
        { 0x14, 0x80 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreatePixelShader+0x31 : mov ecx, 0x3C
        { 0x31, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x32, 0x3C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreatePixelShader+0x42 : retn 0x08
        { 0x42, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x43, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateTexture
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_CreateTexture_1_0_4361 =
{
    0,  // Large == 0
    14, // Count == 14

    {
        // IDirect3DDevice8_CreateTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x1C }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CreateTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateTexture+0x11 : push 0
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateTexture+0x13 : push 0
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreateTexture+0x15 : push ecx
        { 0x15, 0x51 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateTexture+0x22 : push ecx
        { 0x22, 0x51 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_CreateTexture+0x23 : push edx
        { 0x23, 0x52 }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_CreateTexture+0x29 : retn 0x1C
        { 0x29, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x2A, 0x1C }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTexture_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

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
// * IDirect3DDevice8_SwitchTexture
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SwitchTexture_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_SwitchTexture+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SwitchTexture+0x05 : add eax, 0x0C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #2
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SwitchTexture+0x0E : jnb +0x15
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #5
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SwitchTexture+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x23, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SwitchTexture+0x2E : jmp +0xD0
        { 0x2E, 0xEB }, // (Offset,Value)-Pair #9
        { 0x2F, 0xD0 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDisplayMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_GetDisplayMode_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

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
// * IDirect3DDevice8_Clear
// ******************************************************************
LOOVPA<9> IDirect3DDevice8_Clear_1_0_4361 =
{
    1,  // Large == 1
    9,  // Count == 9

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
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB }, // (Offset,Value)-Pair #1
        { 0x11, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x1E, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x1F, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x46 : inc dword ptr [esi+0x2AC4]
        { 0x46, 0xFF }, // (Offset,Value)-Pair #6
        { 0x47, 0x86 }, // (Offset,Value)-Pair #7
        { 0x48, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x49, 0x2A }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xAE : retn 4
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_CreateVertexBuffer_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DDevice8_CreateVertexBuffer+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #1
        { 0x04, 0x40 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateVertexBuffer+0x10 : mov eax, [esp+8]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #3
        { 0x11, 0x44 }, // (Offset,Value)-Pair #4
        { 0x12, 0x24 }, // (Offset,Value)-Pair #5
        { 0x13, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_CreateVertexBuffer+0x48 : mov dword ptr [esi], 0x01000001
        { 0x48, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x49, 0x06 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x01 }, // (Offset,Value)-Pair #9
        { 0x4B, 0x00 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x01 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_CreateVertexBuffer+0x53 : retn 0x14
        { 0x53, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x54, 0x14 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x0D : shl eax, 0x07
        { 0x0D, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0E, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x24 : cmp eax, ecx
        { 0x24, 0x3B }, // (Offset,Value)-Pair #4
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x6B : mov esi, 0x2400
        { 0x6B, 0xBE }, // (Offset,Value)-Pair #6
        { 0x6D, 0x24 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB3, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xB4, 0xE2 }, // (Offset,Value)-Pair #9
        { 0xB5, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_CullMode_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
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
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DDevice8_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x96 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x20 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnable+0x31 : mov dword ptr [eax], 0x4030
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
// * IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

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
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTransform_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

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
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetStreamSource_1_0_4361 =
{
    0,  // Large == 0
    14, // Count == 14

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
SOOVPA<14> IDirect3DDevice8_SetVertexShader_1_0_4361 =
{
    0,  // Large == 0
    14, // Count == 14

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

        // IDirect3DDevice8_SetVertexShader+0xB0 : mov dword ptr [eaax], 0x4194C
        { 0xB0, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB1, 0x00 }, // (Offset,Value)-Pair #11
        { 0xB2, 0x4C }, // (Offset,Value)-Pair #12
        { 0xB3, 0x19 }, // (Offset,Value)-Pair #13
        { 0xB4, 0x04 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_DrawVertices_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

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
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_DrawVerticesUP_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

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
SOOVPA<12> IDirect3DDevice8_SetLight_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

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
SOOVPA<12> IDirect3DDevice8_SetMaterial_1_0_4361 =
{
    0,  // Large == 0
    12, // Count == 12

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
SOOVPA<13> IDirect3DDevice8_LightEnable_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

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
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
SOOVPA<11> IDirect3DVertexBuffer8_Lock_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

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
// * IDirect3DResource8_Register
// ******************************************************************
SOOVPA<19> IDirect3DResource8_Register_1_0_4361 =
{
    0,  // Large == 0
    19, // Count == 19

    {
        // IDirect3DResource8_Register+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x4C }, // (Offset,Value)-Pair #2
        { 0x03, 0x04 }, // (Offset,Value)-Pair #3

        // IDirect3DResource8_Register+0x04 : mov edx, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x05, 0x54 }, // (Offset,Value)-Pair #5
        { 0x07, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DResource8_Register+0x08 : mov eax, [ecx+0x04]
        { 0x08, 0x8B }, // (Offset,Value)-Pair #7
        { 0x09, 0x41 }, // (Offset,Value)-Pair #8
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Register+0x0F : and edx, 0x00070000
        { 0x0F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x10, 0xE2 }, // (Offset,Value)-Pair #11
        { 0x13, 0x07 }, // (Offset,Value)-Pair #12

        // IDirect3DResource8_Register+0x15 : cmp edx, 0x00020000
        { 0x15, 0x81 }, // (Offset,Value)-Pair #13
        { 0x16, 0xFA }, // (Offset,Value)-Pair #14
        { 0x19, 0x02 }, // (Offset,Value)-Pair #15

        // IDirect3DResource8_Register+0x1B : jnz +0x05
        { 0x1B, 0x74 }, // (Offset,Value)-Pair #16
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #17

        // IDirect3DResource8_Register+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #18
        { 0x26, 0x08 }, // (Offset,Value)-Pair #19
    }
};

// ******************************************************************
// * IDirect3DResource8_Release
// ******************************************************************
SOOVPA<13> IDirect3DResource8_Release_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DResource8_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_Release+0x0F : cmp ecx, 1
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #5
        { 0x10, 0xF9 }, // (Offset,Value)-Pair #6
        { 0x11, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DResource8_Release+0x12 : jnz +0x2E
        { 0x12, 0x75 }, // (Offset,Value)-Pair #8
        { 0x13, 0x2E }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Release+0x2F : test eax, 0x780000
        { 0x2F, 0xA9 }, // (Offset,Value)-Pair #10
        { 0x32, 0x78 }, // (Offset,Value)-Pair #11

        // IDirect3DResource8_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DSurface8_GetDesc
// ******************************************************************
SOOVPA<15> IDirect3DSurface8_GetDesc_1_0_4361 =
{
    0,  // Large == 0
    15, // Count == 15

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
// * IDirect3DSurface8_LockRect
// ******************************************************************
SOOVPA<16> IDirect3DSurface8_LockRect_1_0_4361 =
{
    0,  // Large == 0
    16, // Count == 16

    {
        // IDirect3DSurface8_LockRect+0x00 : mov eax, [esp+0x10]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

        // IDirect3DSurface8_LockRect+0x04 : mov ecx, [esp+0x0C]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0C }, // (Offset,Value)-Pair #8

        // IDirect3DSurface8_LockRect+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9

        // IDirect3DSurface8_LockRect+0x11 : push ecx
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10

        // IDirect3DSurface8_LockRect+0x12 : push edx
        { 0x12, 0x52 }, // (Offset,Value)-Pair #11

        // IDirect3DSurface8_LockRect+0x13 : push 0
        { 0x13, 0x6A }, // (Offset,Value)-Pair #12

        // IDirect3DSurface8_LockRect+0x15 : push 0
        { 0x15, 0x6A }, // (Offset,Value)-Pair #13

        // IDirect3DSurface8_LockRect+0x18 : call [abs]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #14

        // IDirect3DSurface8_LockRect+0x11 : retn 0x08
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #15
        { 0x1E, 0x10 }, // (Offset,Value)-Pair #16
    }
};


// ******************************************************************
// * IDirect3DBaseTexture8_GetLevelCount
// ******************************************************************
SOOVPA<13> IDirect3DBaseTexture8_GetLevelCount_1_0_4361 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DBaseTexture8_GetLevelCount+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DBaseTexture8_GetLevelCount+0x04 : movzx eax, [eax+0x0E]
        { 0x04, 0x0F }, // (Offset,Value)-Pair #5
        { 0x05, 0xB6 }, // (Offset,Value)-Pair #6
        { 0x06, 0x40 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0E }, // (Offset,Value)-Pair #8

        // IDirect3DBaseTexture8_GetLevelCount+0x08 : and eax, 0x0F
        { 0x08, 0x83 }, // (Offset,Value)-Pair #9
        { 0x09, 0xE0 }, // (Offset,Value)-Pair #10
        { 0x0A, 0x0F }, // (Offset,Value)-Pair #11

        // IDirect3DBaseTexture8_GetLevelCount+0x0B : retn 0x04
        { 0x0B, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x0C, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
SOOVPA<11> IDirect3DTexture8_GetSurfaceLevel_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DTexture8_GetSurfaceLevel+0x08 : lea [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0B, 0x14 }, // (Offset,Value)-Pair #2

        // IDirect3DTexture8_GetSurfaceLevel+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #4

        // IDirect3DTexture8_GetSurfaceLevel+0x20 : lea ecx, [esp+0x28]
        { 0x23, 0x28 }, // (Offset,Value)-Pair #4

        // IDirect3DTexture8_GetSurfaceLevel+0x24 : push ecx
        { 0x24, 0x51 }, // (Offset,Value)-Pair #6

        // IDirect3DTexture8_GetSurfaceLevel+0x25 : push edx
        { 0x25, 0x52 }, // (Offset,Value)-Pair #7

        // IDirect3DTexture8_GetSurfaceLevel+0x26 : push 0
        { 0x26, 0x6A }, // (Offset,Value)-Pair #8

        // IDirect3DTexture8_GetSurfaceLevel+0x43 : call [abs]
        { 0x43, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DTexture8_GetSurfaceLevel+0x4C : retn 0x0C
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3D8_1_0_4361
// ******************************************************************
OOVPATable D3D8_1_0_4361[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_4361,

        xd3d8::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
        #endif
    },
    // IDirect3D8::GetAdapterDisplayMode
    {
        (OOVPA*)&IDirect3D8_GetAdapterDisplayMode_1_0_4361,

        xd3d8::EmuIDirect3D8_GetAdapterDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_GetAdapterDisplayMode" 
        #endif
    },
    // IDirect3DDevice8::CopyRects
    {
        (OOVPA*)&IDirect3DDevice8_CopyRects_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CopyRects,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CopyRects"
        #endif
    },
    // IDirect3DDevice8::CreateImageSurface
    {
        (OOVPA*)&IDirect3DDevice8_CreateImageSurface_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreateImageSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateImageSurface"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_GetBackBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer"
        #endif
    },
    // IDirect3DDevice8::GetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_GetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_GetDepthStencilSurface,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant"
        #endif
    },
    // IDirect3DDevice8::CreatePixelShader
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
        #endif
    },
    // IDirect3DDevice8::CreateTexture
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreateTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture"
        #endif
    },
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // IDirect3DDevice8::SwitchTexture
    {
        (OOVPA*)&IDirect3DDevice8_SwitchTexture_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SwitchTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SwitchTexture"
        #endif
    },
    // IDirect3DDevice8::GetDisplayMode
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayMode_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear" 
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap" 
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer" 
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias" 
        #endif
    },
    // IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform" 
        #endif
    },
    // IDirect3DDevice8::SetStreamSource
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::DrawVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3DDevice8::DrawVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawVerticesUP_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_DrawVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // IDirect3DDevice8::SetLight
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::LightEnable
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock_1_0_4361,

        xd3d8::EmuIDirect3DVertexBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock" 
        #endif
    },
    // IDirect3DResource8::Register
    {
        (OOVPA*)&IDirect3DResource8_Register_1_0_4361,

        xd3d8::EmuIDirect3DResource8_Register,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Register" 
        #endif
    },
    // IDirect3DResource8::Release
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_4361,

        xd3d8::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release" 
        #endif
    },
    // IDirect3DSurface8::GetDesc
    {
        (OOVPA*)&IDirect3DSurface8_GetDesc_1_0_4361,

        xd3d8::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc" 
        #endif
    },
    // IDirect3DSurface8::LockRect
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_4361,

        xd3d8::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect" 
        #endif
    },
    // IDirect3DBaseTexture8::GetLevelCount
    {
        (OOVPA*)&IDirect3DBaseTexture8_GetLevelCount_1_0_4361,

        xd3d8::EmuIDirect3DBaseTexture8_GetLevelCount,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DBaseTexture8_GetLevelCount" 
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel_1_0_4361,

        xd3d8::EmuIDirect3DTexture8_GetSurfaceLevel,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel" 
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_4361_SIZE
// ******************************************************************
uint32 D3D8_1_0_4361_SIZE = sizeof(D3D8_1_0_4361);
