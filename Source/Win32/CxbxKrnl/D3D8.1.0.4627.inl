// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4627.cpp
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
SOOVPA<8> IDirect3D8_CreateDevice_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

    {
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x80 : repe stosd
        { 0x80, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x81, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x83 : mov eax, esi
        { 0x83, 0x8B }, // (Offset,Value)-Pair #5
        { 0x84, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x90 : retn 0x18
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x91, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3D8_GetAdapterDisplayMode
// ******************************************************************
SOOVPA<13> IDirect3D8_GetAdapterDisplayMode_1_0_4627 =
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

        // IDirect3D8_GetAdapterDisplayMode+0x19 : jnz +0x18
        { 0x19, 0x75 }, // (Offset,Value)-Pair #6
        { 0x1A, 0x18 }, // (Offset,Value)-Pair #7

        // IDirect3D8_GetAdapterDisplayMode+0x33 : mov ecx, [esi+0x21C4]
        { 0x33, 0x8B }, // (Offset,Value)-Pair #8
        { 0x34, 0x8E }, // (Offset,Value)-Pair #9
        { 0x35, 0xC4 }, // (Offset,Value)-Pair #10
        { 0x36, 0x21 }, // (Offset,Value)-Pair #11

        // IDirect3D8_GetAdapterDisplayMode+0xB6 : retn 0x08
        { 0xB6, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xB7, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
SOOVPA<9> IDirect3D8_KickOffAndWaitForIdle_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    {
        // IDirect3D8_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3D8_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // IDirect3D8_KickOffAndWaitForIdle+0x08 : push 2
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3D8_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // IDirect3D8_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3D8_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget2
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetRenderTarget2_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_GetRenderTarget2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetRenderTarget2+0x06 : mov esi, [eax + 0x21B4]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB4 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetRenderTarget2+0x0E : jz + 0x06
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetRenderTarget2+0x11 : call [addr]
        { 0x11, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetRenderTarget2+0x19 : retn
        { 0x19, 0xC3 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetDepthStencilSurface2_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_GetDepthStencilSurface2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetDepthStencilSurface2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetDepthStencilSurface2+0x06 : mov esi, [eax + 0x21B8]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

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
// * IDirect3DDevice8_GetTile
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetTile_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_GetTile+0x13 : lea esi, [ecx+eax*8+0x2260]
        { 0x13, 0x8D }, // (Offset,Value)-Pair #1
        { 0x14, 0xB4 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x16, 0x60 }, // (Offset,Value)-Pair #4
        { 0x17, 0x22 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetTile+0x5F : pop edi; pop esi
        { 0x21, 0x5F }, // (Offset,Value)-Pair #8
        { 0x22, 0x5E }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x24, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTileNoWait
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetTileNoWait_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_SetTileNoWait+0x06 : sub esp, 0x18
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xEC }, // (Offset,Value)-Pair #2
        { 0x08, 0x18 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTileNoWait+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 }, // (Offset,Value)-Pair #4
        { 0x16, 0x46 }, // (Offset,Value)-Pair #5
        { 0x17, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTileNoWait+0x3D : lea edi, [edx+ecx*8+0x2260]
        { 0x3D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x3E, 0xBC }, // (Offset,Value)-Pair #8
        { 0x3F, 0xCA }, // (Offset,Value)-Pair #9
        { 0x40, 0x60 }, // (Offset,Value)-Pair #10
        { 0x41, 0x22 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexShaderConstant_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_SetVertexShaderConstant+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant+0x0F : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant+0x27 : shl ecx, 0x04
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x28, 0xE1 }, // (Offset,Value)-Pair #7
        { 0x29, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant+0x5D : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreateTexture2_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    {
        // IDirect3DDevice8_CreateTexture2+0x02 : lea eax, [esp+0x20]
        { 0x02, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x20 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateTexture2+0x0E : setz dl
        { 0x0E, 0x0F }, // (Offset,Value)-Pair #3
        { 0x0F, 0x94 }, // (Offset,Value)-Pair #4
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateTexture2+0x2A : push 0x00
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #6
        { 0x2B, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateTexture2+0x53 : call [abs]
        { 0x53, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateTexture2+0x78 : jnz +0x0D
        { 0x78, 0x75 }, // (Offset,Value)-Pair #9
        { 0x79, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateTexture2+0xAE : retn 0x1C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #11
        { 0xAF, 0x1C }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDisplayMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetDisplayMode_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

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

        // IDirect3DDevice8_GetDisplayMode+0x6A : or edx, eax
        { 0x6A, 0x0B }, // (Offset,Value)-Pair #9
        { 0x6B, 0xD0 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetDisplayMode+0x7F : jmp +0x1C
        { 0x7F, 0xEB }, // (Offset,Value)-Pair #11
        { 0x80, 0x1C }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
LOOVPA<9> IDirect3DDevice8_Clear_1_0_4627 =
{
    1,  // Large == 1
    9,  // Count == 9

    {
        // IDirect3DDevice8_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_Clear+0x000B : mov ebx, [ebp+0x21B8]
        { 0x000B, 0x8B }, // (Offset,Value)-Pair #4
        { 0x000C, 0x9D }, // (Offset,Value)-Pair #5
        { 0x000D, 0xB8 }, // (Offset,Value)-Pair #6
        { 0x000E, 0x21 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x039A, 0x18 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Swap_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DDevice8_Swap+0x2A : mov ebx, 5
        { 0x2A, 0xBB }, // (Offset,Value)-Pair #1
        { 0x2B, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Swap+0x37 : test bl, 3
        { 0x37, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x38, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x39, 0x03 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_Swap+0x60 : inc dword ptr [esi+0x2C10]
        { 0x60, 0xFF }, // (Offset,Value)-Pair #6
        { 0x61, 0x86 }, // (Offset,Value)-Pair #7
        { 0x62, 0x10 }, // (Offset,Value)-Pair #8
        { 0x63, 0x2C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Swap+0xD8 : retn 4
        { 0xD8, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xD9, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_CreateVertexBuffer2_1_0_4627 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DDevice8_CreateVertexBuffer2+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #1
        { 0x04, 0x40 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateVertexBuffer2+0x10 : mov eax, [esp+8]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #3
        { 0x11, 0x44 }, // (Offset,Value)-Pair #4
        { 0x12, 0x24 }, // (Offset,Value)-Pair #5
        { 0x13, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_CreateVertexBuffer2+0x41 : mov dword ptr [esi], 0x01000001
        { 0x41, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x42, 0x06 }, // (Offset,Value)-Pair #8
        { 0x43, 0x01 }, // (Offset,Value)-Pair #9
        { 0x44, 0x00 }, // (Offset,Value)-Pair #10
        { 0x46, 0x01 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_CreateVertexBuffer2+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x14 : shl eax, 0x07
        { 0x14, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x15, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x16, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x26 : cmp eax, ecx
        { 0x26, 0x3B }, // (Offset,Value)-Pair #4
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x71 : mov ebp, 0x2400
        { 0x71, 0xBD }, // (Offset,Value)-Pair #6
        { 0x73, 0x24 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0xAA : shl ebx, 0x04
        { 0xAA, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xAB, 0xE3 }, // (Offset,Value)-Pair #9
        { 0xAC, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4627 =
{
    0,  // Large == 0
    8,  // Count == 8

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
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

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
// * IDirect3DDevice8_SetRenderState_ZBias
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_ZBias_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    {
        // IDirect3DDevice8_SetRenderState_ZBias+0x0B : setnz al
        { 0x0B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0C, 0x95 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_ZBias+0x14 : fild [esp+0x10]
        { 0x14, 0xDB }, // (Offset,Value)-Pair #4
        { 0x15, 0x44 }, // (Offset,Value)-Pair #5
        { 0x16, 0x24 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZBias+0x45 : push 0x4E
        { 0x45, 0x6A }, // (Offset,Value)-Pair #7
        { 0x46, 0x4E }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_ZBias+0x45 : retn 0x04
        { 0x71, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x72, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

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
// * IDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FillMode_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

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
// * IDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_FogColor_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

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
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    {
        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x0D : mov eax, [edi+0x21B4]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x87 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x10, 0x21 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x56 : shl esi, 0x05
        { 0x56, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x57, 0xE6 }, // (Offset,Value)-Pair #6
        { 0x58, 0x05 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x74 : retn 0x04
        { 0x74, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x75, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_Simple_1_0_4627 =
{
    0,  // Large == 0
    13, // Count == 13

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
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_4627 =
{
    0,  // Large == 0
    13, // Count == 13

    {
        // IDirect3DDevice8_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x1B : mov ecx, [esi+0x21B8h]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1C, 0x8E }, // (Offset,Value)-Pair #4
        { 0x1D, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnable+0x2E : mov dword ptr [eax], 0x4030
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
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x13 : call [abs]
        { 0x13, 0xE8 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x27 : shl edi, 0x10
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x28, 0xE7 }, // (Offset,Value)-Pair #4
        { 0x29, 0x10 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x2E : jns +0x0C
        { 0x2E, 0x79 }, // (Offset,Value)-Pair #6
        { 0x2F, 0x0C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x39 : or edi, 0x01
        { 0x39, 0x83 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xCF }, // (Offset,Value)-Pair #9
        { 0x3B, 0x01 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias+0x6F : retn 0x04
        { 0x6F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x70, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetMaterial_1_0_4627 =
{
    0,  // Large == 0
    12, // Count == 12

    {
        // IDirect3DDevice8_SetMaterial+0x0C : add edi, 0x0AB0
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x0A }, // (Offset,Value)-Pair #4

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
// * IDirect3DVertexBuffer8_Lock2
// ******************************************************************
SOOVPA<11> IDirect3DVertexBuffer8_Lock2_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    {
        // IDirect3DVertexBuffer8_Lock+0x01 : mov bl, [esp+0x0C]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x0C }, // (Offset,Value)-Pair #4

        // IDirect3DVertexBuffer8_Lock+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #6

        // IDirect3DVertexBuffer8_Lock+0x2F : test bl, 0xA0
        { 0x2F, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x31, 0xA0 }, // (Offset,Value)-Pair #9

        // IDirect3DVertexBuffer8_Lock+0x48 : retn 0x08
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x49, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DResource8_Release
// ******************************************************************
SOOVPA<11> IDirect3DResource8_Release_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

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

        // IDirect3DResource8_Release+0x3E : pop esi
        { 0x3E, 0x5E }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
SOOVPA<14> IDirect3DTexture8_GetSurfaceLevel2_1_0_4627 =
{
    0,  // Large == 0
    14, // Count == 14

    {
        // IDirect3DTexture8_GetSurfaceLevel2+0x00 : sub esp, 0x0C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DTexture8_GetSurfaceLevel2+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x14 }, // (Offset,Value)-Pair #5

        // IDirect3DTexture8_GetSurfaceLevel2+0x04 : lea eax, [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #6
        { 0x0B, 0x14 }, // (Offset,Value)-Pair #7

        // IDirect3DTexture8_GetSurfaceLevel2+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DTexture8_GetSurfaceLevel2+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D }, // (Offset,Value)-Pair #10
        { 0x1E, 0x18 }, // (Offset,Value)-Pair #11

        // IDirect3DTexture8_GetSurfaceLevel2+0x29 : call [abs]
        { 0x29, 0xE8 }, // (Offset,Value)-Pair #12

        // IDirect3DTexture8_GetSurfaceLevel2+0x47 : retn 0x08
        { 0x47, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x48, 0x08 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * D3D8_1_0_4627
// ******************************************************************
OOVPATable D3D8_1_0_4627[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_4627,

        xd3d8::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
        #endif
    },
    // IDirect3D8::GetAdapterDisplayMode
    {
        (OOVPA*)&IDirect3D8_GetAdapterDisplayMode_1_0_4627,

        xd3d8::EmuIDirect3D8_GetAdapterDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_GetAdapterDisplayMode" 
        #endif
    },
    // IDirect3D8::KickOffAndWaitForIdle
    {
        (OOVPA*)&IDirect3D8_KickOffAndWaitForIdle_1_0_4627,

        xd3d8::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle" 
        #endif
    },
    // IDirect3DDevice8::GetRenderTarget2
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget2_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_GetRenderTarget2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget2"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface2
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface2_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_GetDepthStencilSurface2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface2"
        #endif
    },
    // IDirect3DDevice8::GetTile
    {
        (OOVPA*)&IDirect3DDevice8_GetTile_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_GetTile,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTile"
        #endif
    },
    // IDirect3DDevice8::SetTileNoWait
    {
        (OOVPA*)&IDirect3DDevice8_SetTileNoWait_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetTileNoWait,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTileNoWait"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetVertexShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant"
        #endif
    },
    // IDirect3DDevice8::CreatePixelShader (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
        #endif
    },
    // IDirect3DDevice8::CreateTexture2
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture2_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_CreateTexture2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture2"
        #endif
    },
    // IDirect3DDevice8::SetTexture (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // IDirect3DDevice8::GetDisplayMode
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayMode_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear" 
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap" 
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer2
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer2_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_CreateVertexBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer2" 
        #endif
    },
    // IDirect3DDevice8::SetTextureState_TexCoordIndex
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_TexCoordIndex" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_NormalizeNormals
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_NormalizeNormals" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZBias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZBias_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_ZBias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZBias" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_EdgeAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FillMode_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_FillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FillMode" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_FogColor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FogColor_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_FogColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FogColor" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_Simple
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_Simple_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_Simple,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_Simple" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_ZEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ZEnable_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_ZEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ZEnable" 
        #endif
    },
    // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias" 
        #endif
    },
    // IDirect3DDevice8::SetTransform (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform" 
        #endif
    },
    // IDirect3DDevice8::GetTransform (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetTransform_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_GetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTransform" 
        #endif
    },
    // IDirect3DDevice8::SetStreamSource (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetStreamSource_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetStreamSource,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetStreamSource"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::DrawVertices (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3DDevice8::SetLight (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetLight_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
    // IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4627,

        xd3d8::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::LightEnable (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_4361,

        xd3d8::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock2
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock2_1_0_4627,

        xd3d8::EmuIDirect3DVertexBuffer8_Lock2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock2" 
        #endif
    },
    // IDirect3DResource8::Release
    {
        (OOVPA*)&IDirect3DResource8_Release_1_0_4627,

        xd3d8::EmuIDirect3DResource8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_Release" 
        #endif
    },
    // IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DSurface8_GetDesc_1_0_4361,

        xd3d8::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc" 
        #endif
    },
    // IDirect3DSurface8::LockRect (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_4361,

        xd3d8::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect" 
        #endif
    },
    // IDirect3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DBaseTexture8_GetLevelCount_1_0_4361,

        xd3d8::EmuIDirect3DBaseTexture8_GetLevelCount,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DBaseTexture8_GetLevelCount" 
        #endif
    },
    // IDirect3DTexture8::GetSurfaceLevel2
    {
        (OOVPA*)&IDirect3DTexture8_GetSurfaceLevel2_1_0_4627,

        xd3d8::EmuIDirect3DTexture8_GetSurfaceLevel2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_GetSurfaceLevel2" 
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_4627_SIZE
// ******************************************************************
uint32 D3D8_1_0_4627_SIZE = sizeof(D3D8_1_0_4627);
