// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.3925.cpp
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
SOOVPA<8> IDirect3D8_CreateDevice_1_0_3925 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x79 : mov eax, esi
        { 0x79, 0x8B }, // (Offset,Value)-Pair #3
        { 0x7A, 0xC6 }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x7C : retn 0x18
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #5
        { 0x7D, 0x18 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x90 : retn 0x18
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x91, 0x18 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderTarget_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderTarget+0x00 : sub esp, 0x20
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x20 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderTarget+0x32 : mov [esp+0x34], ebx
        { 0x32, 0x89 }, // (Offset,Value)-Pair #4
        { 0x33, 0x5C }, // (Offset,Value)-Pair #5
        { 0x34, 0x24 }, // (Offset,Value)-Pair #6
        { 0x35, 0x34 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderTarget+0x55 : and ecx, 0x0F
        { 0x55, 0x83 }, // (Offset,Value)-Pair #8
        { 0x56, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x57, 0x0F }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderTarget+0xDC : push edx
        { 0xDC, 0x52 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetStreamSource_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetStreamSource+0x39 : add edx, 0xFFF80000
        { 0x39, 0x81 }, // (Offset,Value)-Pair #1
        { 0x3A, 0xC2 }, // (Offset,Value)-Pair #2
        { 0x3B, 0x00 }, // (Offset,Value)-Pair #3
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #4
        { 0x3D, 0xF8 }, // (Offset,Value)-Pair #5
        { 0x3E, 0xFF }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetStreamSource+0x49 : jnz +0x06
        { 0x49, 0x75 }, // (Offset,Value)-Pair #7
        { 0x4A, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetStreamSource+0x6A : or ecx, 0x280
        { 0x6A, 0x81 }, // (Offset,Value)-Pair #9
        { 0x6B, 0xC9 }, // (Offset,Value)-Pair #10
        { 0x6C, 0x80 }, // (Offset,Value)-Pair #11
        { 0x6D, 0x02 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetIndices_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetIndices+0x0E : jz +0x10
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #1
        { 0x0F, 0x10 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetIndices+0x3E : test ecx, 0x78FFFF
        { 0x3E, 0xF7 }, // (Offset,Value)-Pair #3
        { 0x3F, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x40, 0xFF }, // (Offset,Value)-Pair #5
        { 0x41, 0xFF }, // (Offset,Value)-Pair #6
        { 0x42, 0x78 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetIndices+0x67 : mov [esi+0x047C], edi
        { 0x67, 0x89 }, // (Offset,Value)-Pair #8
        { 0x68, 0xBE }, // (Offset,Value)-Pair #9
        { 0x69, 0x7C }, // (Offset,Value)-Pair #10
        { 0x6A, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetViewport_1_0_3925 =
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

        // IDirect3DDevice8_SetViewport+0x2F : mov ebx, [esp+0x1C]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x5C }, // (Offset,Value)-Pair #5
        { 0x31, 0x24 }, // (Offset,Value)-Pair #6
        { 0x32, 0x1C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetViewport+0x55 : jnz +0x12
        { 0x55, 0x75 }, // (Offset,Value)-Pair #8
        { 0x56, 0x12 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetViewport+0x9D : inc edx
        { 0x9D, 0x42 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetTexture_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetTexture+0x0E : sub esp, 0x08
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x84 }, // (Offset,Value)-Pair #2
        { 0x10, 0x9F }, // (Offset,Value)-Pair #3
        { 0x11, 0xA0 }, // (Offset,Value)-Pair #4
        { 0x12, 0x0B }, // (Offset,Value)-Pair #5
        { 0x13, 0x00 }, // (Offset,Value)-Pair #6
        { 0x14, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetTexture+0x57 : shl edx, 6
        { 0x57, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x58, 0xE2 }, // (Offset,Value)-Pair #9
        { 0x59, 0x06 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetTexture+0x7D : retn 0x08
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x7E, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_Clear_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_Clear+0x00 : sub esp, 0x38
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x38 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_Clear+0x25 : test cl, 1
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x27, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_Clear+0x5D : mov [esi], eax; xor ebp, ebp
        { 0x5D, 0x89 }, // (Offset,Value)-Pair #7
        { 0x5E, 0x06 }, // (Offset,Value)-Pair #8
        { 0x5F, 0x33 }, // (Offset,Value)-Pair #9
        { 0x60, 0xED }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreatePalette
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreatePalette_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_CreatePalette+0x01 : push 0x0C
        { 0x01, 0x6A }, // (Offset,Value)-Pair #1
        { 0x02, 0x0C }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreatePalette+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #3
        { 0x04, 0x40 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CreatePalette+0x10 : mov eax, 0x8007000E
        { 0x10, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x11, 0x0E }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7
        { 0x13, 0x07 }, // (Offset,Value)-Pair #8
        { 0x14, 0x80 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreatePalette+0x4E : shl esi, 0x1E
        { 0x4E, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x4F, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x50, 0x1E }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetPalette_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0BB0]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xB7 }, // (Offset,Value)-Pair #3
        { 0x10, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0B }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetPalette+0x68 : add esi, 0x41B20
        { 0x68, 0x81 }, // (Offset,Value)-Pair #6
        { 0x69, 0xC6 }, // (Offset,Value)-Pair #7
        { 0x6A, 0x20 }, // (Offset,Value)-Pair #8
        { 0x6B, 0x1B }, // (Offset,Value)-Pair #9
        { 0x6C, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetPalette+0x7C : retn 0x08
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x7D, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateTexture
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_CreateTexture_1_0_3925 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

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
// * IDirect3DDevice8_CreateVolumeTexture
// ******************************************************************
SOOVPA<15> IDirect3DDevice8_CreateVolumeTexture_1_0_3925 =
{
    0,  // Large == 0
    15, // Count == 15

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_CreateVolumeTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CreateVolumeTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateVolumeTexture+0x11 : push 1
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateVolumeTexture+0x13 : push 0
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreateVolumeTexture+0x16 : mov ecx, [esp+0x1C]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #10
        { 0x17, 0x4C }, // (Offset,Value)-Pair #11
        { 0x18, 0x24 }, // (Offset,Value)-Pair #12
        { 0x19, 0x1C }, // (Offset,Value)-Pair #13

        // IDirect3DDevice8_CreateVolumeTexture+0x2C : retn 0x20
        { 0x2C, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x2D, 0x20 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateCubeTexture
// ******************************************************************
SOOVPA<15> IDirect3DDevice8_CreateCubeTexture_1_0_3925 =
{
    0,  // Large == 0
    15, // Count == 15

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_CreateCubeTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x18 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_CreateCubeTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateCubeTexture+0x11 : push 0
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateCubeTexture+0x13 : push 1
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x01 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_CreateCubeTexture+0x18 : mov eax, [esp+0x1C]
        { 0x18, 0x8B }, // (Offset,Value)-Pair #10
        { 0x19, 0x44 }, // (Offset,Value)-Pair #11
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #12
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #13

        // IDirect3DDevice8_CreateCubeTexture+0x25 : retn 0x18
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x26, 0x18 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateIndexBuffer
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_CreateIndexBuffer_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_CreateIndexBuffer+0x13 : mov eax, 0x8007000E
        { 0x13, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x14, 0x0E }, // (Offset,Value)-Pair #2
        { 0x15, 0x00 }, // (Offset,Value)-Pair #3
        { 0x16, 0x07 }, // (Offset,Value)-Pair #4
        { 0x17, 0x80 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateIndexBuffer+0x18 : retn 0x14
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x19, 0x14 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateIndexBuffer+0x2E : mov dword ptr [eax], 0x1010001
        { 0x2E, 0xC7 }, // (Offset,Value)-Pair #8
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #9
        { 0x30, 0x01 }, // (Offset,Value)-Pair #10
        { 0x31, 0x00 }, // (Offset,Value)-Pair #11
        { 0x32, 0x01 }, // (Offset,Value)-Pair #12
        { 0x33, 0x01 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetVertexShaderConstant_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexShaderConstant+0x11 : mov cl, [ebx+0x0C]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x4B }, // (Offset,Value)-Pair #2
        { 0x13, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant+0x1A : test cl, 0x10
        { 0x1A, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x1B, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x10 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVertexShaderConstant+0x63 : jle +0x19
        { 0x63, 0x7E }, // (Offset,Value)-Pair #7
        { 0x64, 0x19 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant+0x9B : retn 0x0C
        { 0x9B, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x9C, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetFlickerFilter
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetFlickerFilter_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetFlickerFilter+0x0A : mov edx, [ecx+0x2308]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetFlickerFilter+0x10 : push 0; push eax; push 0x0B; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0B }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetFlickerFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetSoftDisplayFilter
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetSoftDisplayFilter_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetSoftDisplayFilter+0x0A : mov edx, [ecx+0x2308]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetSoftDisplayFilter+0x10 : push 0; push eax; push 0x0E; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0E }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetSoftDisplayFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x0B : shl eax, 0x07
        { 0x0B, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0C, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x07 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x24 : cmp eax, 0x20000
        { 0x24, 0x81 }, // (Offset,Value)-Pair #4
        { 0x25, 0xF9 }, // (Offset,Value)-Pair #5
        { 0x28, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x3B : mov edi, 0x8511
        { 0x3B, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3C, 0x11 }, // (Offset,Value)-Pair #8
        { 0x3D, 0x85 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetTextureState_TexCoordIndex+0x97 : shl eax, cl
        { 0x97, 0xD3 }, // (Offset,Value)-Pair #10
        { 0x98, 0xE0 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_Simple_1_0_3925 =
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

        // IDirect3DDevice8_SetRenderState_Simple+0x2B : jmp +0xD3
        { 0x2B, 0xEB }, // (Offset,Value)-Pair #12
        { 0x2C, 0xD3 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FogColor_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_FogColor+0x2E : mov dword ptr [eax], 0x402A8
        { 0x2E, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #2
        { 0x30, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x31, 0x02 }, // (Offset,Value)-Pair #4
        { 0x32, 0x04 }, // (Offset,Value)-Pair #5
        { 0x33, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_FogColor+0x37 : add eax, 0x08
        { 0x37, 0x83 }, // (Offset,Value)-Pair #7
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x39, 0x08 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_FogColor+0x44 : retn 0x04
        { 0x44, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x45, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FillMode
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FillMode_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_FillMode+0x23 : mov dword ptr [eax], 0x8038C
        { 0x23, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x24, 0x00 }, // (Offset,Value)-Pair #2
        { 0x25, 0x8C }, // (Offset,Value)-Pair #3
        { 0x26, 0x03 }, // (Offset,Value)-Pair #4
        { 0x27, 0x08 }, // (Offset,Value)-Pair #5
        { 0x28, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_FillMode+0x2F : add eax, 0x0C
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x31, 0x0C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_FillMode+0x3B : retn 0x04
        { 0x3B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilEnable
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_StencilEnable_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_StencilEnable+0x45 : mov dword ptr [eax], 0x41D84
        { 0x45, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x46, 0x00 }, // (Offset,Value)-Pair #2
        { 0x47, 0x84 }, // (Offset,Value)-Pair #3
        { 0x48, 0x1D }, // (Offset,Value)-Pair #4
        { 0x49, 0x04 }, // (Offset,Value)-Pair #5
        { 0x4A, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_StencilEnable+0x71 : add eax, 0x08
        { 0x71, 0x83 }, // (Offset,Value)-Pair #7
        { 0x72, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x73, 0x08 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_StencilEnable+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x0C : mov eax, [esi+0x040C]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0D, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #3
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x22 : cmp dl, 0x20
        { 0x22, 0x80 }, // (Offset,Value)-Pair #7
        { 0x23, 0xFA }, // (Offset,Value)-Pair #8
        { 0x24, 0x20 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable+0x74 : retn 0x04
        { 0x74, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x75, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_ZEnable+0x13 : jz +0x11
        { 0x13, 0x74 }, // (Offset,Value)-Pair #1
        { 0x14, 0x11 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnable+0x15 : mov edx, [esi+0x0410]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #3
        { 0x16, 0x96 }, // (Offset,Value)-Pair #4
        { 0x17, 0x10 }, // (Offset,Value)-Pair #5
        { 0x18, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnable+0x28 : mov dword ptr [eax], 0x4030
        { 0x28, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x29, 0x00 }, // (Offset,Value)-Pair #8
        { 0x2A, 0x0C }, // (Offset,Value)-Pair #9
        { 0x2B, 0x03 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_ZEnable+0x69 : retn 0x04
        { 0x69, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x6A, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_Release
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Release_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_Release+0x07 : mov eax, [edi+0x0408]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x87 }, // (Offset,Value)-Pair #2
        { 0x09, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_Release+0x0D : cmp eax, 1
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x01 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Release+0x1B : mov ecx, 0x0AE4
        { 0x1B, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xE4 }, // (Offset,Value)-Pair #9
        { 0x1D, 0x0A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_Release+0x2E : dec eax
        { 0x2E, 0x48 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_Present
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_Present_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_Present+0x0A : push 2
        { 0x0A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x0B, 0x02 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_Present+0x11 : mov eax, [esi+2518]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #3
        { 0x12, 0x86 }, // (Offset,Value)-Pair #4
        { 0x13, 0x18 }, // (Offset,Value)-Pair #5
        { 0x14, 0x25 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_Present+0xBE : shr ebx, 1
        { 0xBE, 0xD1 }, // (Offset,Value)-Pair #7
        { 0xBF, 0xEB }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_Present+0xC0 : xor edi, edi
        { 0xC0, 0x33 }, // (Offset,Value)-Pair #9
        { 0xC1, 0xFF }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetShaderConstantMode_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetShaderConstantMode+0x26 : mov [ebp+0x2140], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x85 }, // (Offset,Value)-Pair #2
        { 0x28, 0x40 }, // (Offset,Value)-Pair #3
        { 0x29, 0x21 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetShaderConstantMode+0x51 : mov dword ptr [ebx+0x04], 0x3C
        { 0x51, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x52, 0x43 }, // (Offset,Value)-Pair #6
        { 0x53, 0x04 }, // (Offset,Value)-Pair #7
        { 0x54, 0x3C }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetShaderConstantMode+0xE4 : add ebx, 0x0124
        { 0xE4, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE5, 0xC3 }, // (Offset,Value)-Pair #10
        { 0xE6, 0x24 }, // (Offset,Value)-Pair #11
        { 0xE7, 0x01 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetBackBuffer_1_0_3925 =
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

        // IDirect3DDevice8_GetBackBuffer+0x1E : lea eax, [ecx+eax*8+0x2150]
        { 0x1E, 0x8D }, // (Offset,Value)-Pair #6
        { 0x1F, 0x84 }, // (Offset,Value)-Pair #7
        { 0x20, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x21, 0x50 }, // (Offset,Value)-Pair #9
        { 0x22, 0x21 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer+0x31 : retn 0x0C
        { 0x31, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x32, 0x0C }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetDepthStencilSurface_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetDepthStencilSurface+0x06 : mov eax, [eax + 0x0410]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x10 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetDepthStencilSurface+0x13 : jnz +0x04
        { 0x13, 0x74 }, // (Offset,Value)-Pair #6
        { 0x14, 0x0B }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetDepthStencilSurface+0x16 : call [addr]
        { 0x16, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetDepthStencilSurface+0x25 : retn
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x26, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_CreateVertexBuffer_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

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
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
SOOVPA<11> IDirect3DVertexBuffer8_Lock_1_0_3925 =
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
        { 0x0A, 0x1E }, // (Offset,Value)-Pair #6

        // IDirect3DVertexBuffer8_Lock+0x29 : test bl, 0xA0
        { 0x29, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x2B, 0xA0 }, // (Offset,Value)-Pair #9

        // IDirect3DVertexBuffer8_Lock+0x4A : retn 0x14
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x14 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DResource8_Register
// ******************************************************************
SOOVPA<19> IDirect3DResource8_Register_1_0_3925 =
{
    0,  // Large == 0
    19, // Count == 19

    -1, // XRef Not Saved
    0,  // XRef Not Used

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
SOOVPA<11> IDirect3DResource8_Release_1_0_3925 =
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

        // IDirect3DResource8_Release+0x3E : pop esi
        { 0x3E, 0x5E }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DResource8_AddRef
// ******************************************************************
SOOVPA<10> IDirect3DResource8_AddRef_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DResource8_AddRef+0x07 : test eax, 0xFFFF
        { 0x07, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x08, 0xFF }, // (Offset,Value)-Pair #2
        { 0x09, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DResource8_AddRef+0x0E : and eax, 0x00070000
        { 0x0E, 0x25 }, // (Offset,Value)-Pair #4
        { 0x11, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DResource8_AddRef+0x13 : cmp eax, 0x00050000
        { 0x13, 0x3D }, // (Offset,Value)-Pair #6
        { 0x16, 0x05 }, // (Offset,Value)-Pair #7

        // IDirect3DResource8_AddRef+0x33 : pop esi
        { 0x33, 0x5E }, // (Offset,Value)-Pair #8

        // IDirect3DResource8_AddRef+0x34 : retn 0x04
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x35, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DPalette8_Lock
// ******************************************************************
SOOVPA<13> IDirect3DPalette8_Lock_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DPalette8_Lock+0x00 : test [esp+0x0C], 0xA0
        { 0x00, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x0C }, // (Offset,Value)-Pair #4
        { 0x04, 0xA0 }, // (Offset,Value)-Pair #5

        // IDirect3DPalette8_Lock+0x0A : jnz +0x06
        { 0x0A, 0x75 }, // (Offset,Value)-Pair #6
        { 0x0B, 0x06 }, // (Offset,Value)-Pair #7

        // IDirect3DPalette8_Lock+0x18 : mov ecx, [esp+0x0C]
        { 0x18, 0x8B }, // (Offset,Value)-Pair #8
        { 0x19, 0x4C }, // (Offset,Value)-Pair #9
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #10
        { 0x1B, 0x0C }, // (Offset,Value)-Pair #11

        // IDirect3DPalette8_Lock+0x1C : mov [ecx], eax
        { 0x1C, 0x89 }, // (Offset,Value)-Pair #12
        { 0x1D, 0x01 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DTexture8_LockRect
// ******************************************************************
SOOVPA<17> IDirect3DTexture8_LockRect_1_0_3925 =
{
    0,  // Large == 0
    17, // Count == 17

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DTexture8_LockRect+0x00 : mov eax, [esp+0x14]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x14 }, // (Offset,Value)-Pair #4

        // IDirect3DTexture8_LockRect+0x04 : mov ecx, [esp+0x10]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DTexture8_LockRect+0x0C : push eax, push ecx, push edx, push eax, push 0
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x16, 0x52 }, // (Offset,Value)-Pair #11
        { 0x17, 0x50 }, // (Offset,Value)-Pair #12
        { 0x18, 0x6A }, // (Offset,Value)-Pair #13
        { 0x19, 0x00 }, // (Offset,Value)-Pair #14

        // IDirect3DTexture8_LockRect+0x1B : call [addr]
        { 0x1B, 0xE8 }, // (Offset,Value)-Pair #15

        // IDirect3DTexture8_LockRect+0x20 : retn 0x14
        { 0x20, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x21, 0x14 }, // (Offset,Value)-Pair #17
    }
};

// ******************************************************************
// * IDirect3DVolumeTexture8_LockBox
// ******************************************************************
SOOVPA<11> IDirect3DVolumeTexture8_LockBox_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DVolumeTexture8_LockBox+0x08 : test bl, 0x20
        { 0x08, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x09, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x0A, 0x20 }, // (Offset,Value)-Pair #3

        // IDirect3DVolumeTexture8_LockBox+0x40 : test bl, 0x40
        { 0x40, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x41, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x42, 0x40 }, // (Offset,Value)-Pair #6

        // IDirect3DVolumeTexture8_LockBox+0x83 : lea ecx, [eax+edx]
        { 0x83, 0x8D }, // (Offset,Value)-Pair #7
        { 0x84, 0x0C }, // (Offset,Value)-Pair #8
        { 0x85, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DVolumeTexture8_LockBox+0x99 : retn 0x14
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x9A, 0x14 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DCubeTexture8_LockRect
// ******************************************************************
SOOVPA<11> IDirect3DCubeTexture8_LockRect_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DCubeTexture8_LockRect+0x06 : test bl, 0x20
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x20 }, // (Offset,Value)-Pair #3

        // IDirect3DCubeTexture8_LockRect+0x3F : test bl, 0x40
        { 0x3F, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x40, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x41, 0x40 }, // (Offset,Value)-Pair #6

        // IDirect3DCubeTexture8_LockRect+0x71 : shr edx, 0x03
        { 0x71, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x72, 0xEA }, // (Offset,Value)-Pair #8
        { 0x73, 0x03 }, // (Offset,Value)-Pair #9

        // IDirect3DCubeTexture8_LockRect+0x98 : retn 0x18
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x99, 0x18 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3D8_1_0_3925
// ******************************************************************
OOVPATable D3D8_1_0_3925[] =
{
    // IDirect3D8::CreateDevice
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_3925,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice" 
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
    // IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderTarget" 
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
    // IDirect3DDevice8::CreateVertexBuffer
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer" 
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
};

// ******************************************************************
// * D3D8_1_0_3925_SIZE
// ******************************************************************
uint32 D3D8_1_0_3925_SIZE = sizeof(D3D8_1_0_3925);
