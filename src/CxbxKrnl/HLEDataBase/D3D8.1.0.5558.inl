// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5558.cpp
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
// * D3D_CDevice_LazySetStateVB
// ******************************************************************
SOOVPA<12> D3D_CDevice_LazySetStateVB_1_0_5558 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_D3DCD_LAZYSETSTATEVB, // Xref Is Saved
    0,                         // Xref Not Used

    {
        { 0x00, 0x83 }, // 1
        { 0x01, 0xEC }, // 2
        { 0x02, 0x0C }, // 3

        { 0x34, 0xF6 }, // 4
        { 0x35, 0xC3 }, // 5
        { 0x36, 0x40 }, // 6

        { 0x57, 0x83 }, // 7
        { 0x58, 0xE5 }, // 8
        { 0x59, 0x10 }, // 9

        { 0x79, 0x17 }, // 10
        { 0x7A, 0x40 }, // 11
        { 0x7B, 0x00 }, // 12
    }
};

// ******************************************************************
// * IDirect3DDevice8_BeginPush
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_BeginPush_1_0_5558 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // Xref Not Saved
    1,  // Xref Is  Used

    {
        { 0x09, XREF_D3DCD_LAZYSETSTATEVB }, // 1

        { 0x00, 0x8B }, // 2
        { 0x01, 0x0D }, // 3

        { 0x06, 0x6A }, // 4
        { 0x07, 0x00 }, // 5

        { 0x11, 0x40 }, // 6

        { 0x12, 0x89 }, // 7
        { 0x13, 0x44 }, // 8
        { 0x14, 0x24 }, // 9
        { 0x15, 0x04 }, // 10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SelectVertexShader
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SelectVertexShader_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x12, 0x07 },
        { 0x26, 0x00 },
        { 0x3A, 0x0D },
        { 0x4F, 0x8B },
        { 0x62, 0xE1 },
        { 0x76, 0x3B },
        { 0x8A, 0x89 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreateVertexShader_1_0_5558 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

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

        // IDirect3DDevice8_CreateVertexShader+0x73 : mov eax, 0x8007000E
        { 0x65, 0x07 }, // (Offset,Value)-Pair #11
        { 0x66, 0x80 }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_CreateVertexShader+0x69 : retn 0x10
        { 0x69, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x6A, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexShaderConstant1_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1+0x0E : jnb +0x2E
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x2E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 }, // (Offset,Value)-Pair #6
        { 0x29, 0x48 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xF0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1+0x3D : retn
        { 0x3D, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1+0x47 : jmp +0xB7
        { 0x47, 0xEB }, // (Offset,Value)-Pair #10
        { 0x48, 0xB7 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetViewport_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
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
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
SOOVPA<17> IDirect3DDevice8_CreateVertexBuffer2_1_0_5558 =
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

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_UpdateOverlay_1_0_5558 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_UpdateOverlay+0x13 : mov [eax+0x1C0C], ecx
        { 0x13, 0x89 }, // (Offset,Value)-Pair #1
        { 0x14, 0x88 }, // (Offset,Value)-Pair #2
        { 0x15, 0x0C }, // (Offset,Value)-Pair #3
        { 0x16, 0x1C }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x85 : and eax, 0x0FFFFFFF
        { 0x85, 0x25 }, // (Offset,Value)-Pair #5
        { 0x86, 0xFF }, // (Offset,Value)-Pair #6
        { 0x89, 0x0F }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0x9D : mov [esi+0x8920], ecx
        { 0x9D, 0x89 }, // (Offset,Value)-Pair #8
        { 0x9E, 0x8E }, // (Offset,Value)-Pair #9
        { 0x9F, 0x20 }, // (Offset,Value)-Pair #10
        { 0xA0, 0x89 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5558 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x2400]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x07, 0x00 }, // (Offset,Value)-Pair #3
        { 0x08, 0x24 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x1DD8]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xD8 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x1D }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreatePalette2
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreatePalette2_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0D, 0x8B },
        { 0x16, 0x00 },
        { 0x23, 0x68 },
        { 0x2E, 0x85 },
        { 0x3D, 0x5E },
        { 0x46, 0x1E },
        { 0x52, 0x89 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CreateTexture2_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

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
// * IDirect3DDevice8_End
// ******************************************************************
SOOVPA<17> IDirect3DDevice8_End_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x07, 0x8B },
        { 0x13, 0xC7 },
        { 0x19, 0xC7 },
        { 0x22, 0x08 },
        { 0x2B, 0xE1 },
        { 0x34, 0x4E },
        { 0x40, 0xC3 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetLight
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetLight_1_0_5558 =
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
        { 0xFE, 0xC1 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetMaterial_1_0_5558 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xF0 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_LightEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_LightEnable_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
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
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetVertexShader_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x89 },
        { 0x3E, 0x5F },
        { 0x61, 0x3B },
        { 0x81, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x8B },
        { 0xDE, 0x04 },
        { 0xFE, 0xC1 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPalette
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetPalette_1_0_5558 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xBE }, // (Offset,Value)-Pair #3
        { 0x10, 0x88 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0F }, // (Offset,Value)-Pair #5

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
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xB0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5558 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0xBC4]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x96 }, // (Offset,Value)-Pair #6
        { 0x5E, 0xC4 }, // (Offset,Value)-Pair #7
        { 0x5F, 0x0B }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetTransform_1_0_5558 =
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

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetBackBuffer2_1_0_5558 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
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
        { 0x18, 0x04 }, // (Offset,Value)-Pair #9
        { 0x19, 0x1A }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DTexture8_LockRect
// ******************************************************************
SOOVPA<17> IDirect3DTexture8_LockRect_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
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
// * IDirect3D8_CheckDeviceMultiSampleType
// ******************************************************************
SOOVPA<7> IDirect3D8_CheckDeviceMultiSampleType_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0E, 0x00 },
        { 0x1E, 0x54 },
        { 0x2E, 0xC9 },
        { 0x3E, 0x8B },
        { 0x4E, 0x08 },
        { 0x5E, 0x72 },
        { 0x6E, 0x03 },
    }
};

// ******************************************************************
// * IDirect3D8_SetPushBufferSize
// ******************************************************************
SOOVPA<7> IDirect3D8_SetPushBufferSize_1_0_5558 =
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
// * D3D::SetFence
// ******************************************************************
SOOVPA<14> D3D_SetFence_1_0_5558 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_D3D_SETFENCE, // Xref Is Saved
    0,                 // Xref Not Used

    {
        { 0x14, 0x8B },
        { 0x15, 0x7E },
        { 0x16, 0x2C },
        { 0x23, 0x8A },
        { 0x24, 0x54 },
        { 0x25, 0x24 },
        { 0x26, 0x0C },
        { 0x66, 0x89 },
        { 0x67, 0x44 },
        { 0x68, 0xCE },
        { 0x69, 0x64 },
        { 0x91, 0xC7 },
        { 0x92, 0x46 },
        { 0x93, 0x3C },
    }
};

// ******************************************************************
// * IDirect3DDevice8_BlockOnFence
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_BlockOnFence_1_0_5558 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x00, 0x8B }, // FIXME: Very weak.
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
    }
};

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
SOOVPA<16> D3D_BlockOnResource_1_0_5558 =
{
    0,  // Large == 0
    16,  // Count == 16

    XREF_D3D_BlockOnResource, // Xref Is Saved
    0,  // Xref Not Used

    {
        { 0x09, 0x8B }, // mov eax, [esp+arg_0]
        { 0x0A, 0x44 },
        { 0x0B, 0x24 },
        { 0x0C, 0x04 },
        { 0x0F, 0x81 }, // and ecx, 0x00070000
        { 0x10, 0xE1 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x07 },
        { 0x14, 0x00 },
        { 0x46, 0x8B }, // mov esi, [eax+8]
        { 0x47, 0x70 },
        { 0x48, 0x08 },
        { 0x75, 0xC2 }, // retn 4
        { 0x76, 0x04 },
        { 0x77, 0x00 },

    }
};

// ******************************************************************
// * IDirect3DResource8_BlockUntilNotBusy
// ******************************************************************
SOOVPA<2> IDirect3DResource8_BlockUntilNotBusy_1_0_5558 =
{
    0,  // Large == 0
    2,  // Count == 2

    -1, // Xref Not Saved
    1,  // Xref Is Used

    {
        { 0x01, XREF_D3D_BlockOnResource },

        { 0x00, 0xE9 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetScissors
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetScissors_1_0_5558 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
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
// * D3D8_1_0_5558
// ******************************************************************
OOVPATable D3D8_1_0_5558[] =
{
    // IDirect3D8::CreateDevice (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3D8_CreateDevice_1_0_4627,

        XTL::EmuIDirect3D8_CreateDevice,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CreateDevice"
        #endif
    },
    // IDirect3DDevice8::GetDisplayFieldStatus (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus,

        #ifdef _DEBUG_TRACE
        "IDirect3DDevice8_GetDisplayFieldStatus"
        #endif
    },
    // D3D::CDevice::LazySetStateVB (XREF)
    {
        (OOVPA*)&D3D_CDevice_LazySetStateVB_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "D3D::CDevice::LazySetStateVB (XREF)"
        #endif
    },
    // IDirect3DDevice8::BeginPush
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
    // D3DDevice_RunVertexStateShader (* unchanged since 4627 *)
    {
       (OOVPA*)&X_D3DDevice_RunVertexStateShader_1_0_4627,

       XTL::EmuIDirect3DDevice8_RunVertexStateShader,

       #ifdef _DEBUG_TRACE
       "D3DDevice_RunVertexStateShader"
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
    // IDirect3DDevice8::SelectVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SelectVertexShader_1_0_5558,

        XTL::EmuIDirect3DDevice8_SelectVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShader"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_5558,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant1
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant1_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant1"
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
    // IDirect3DDevice8::CreateVertexBuffer2
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer2_1_0_5558,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer2"
        #endif
    },
    // IDirect3DDevice8::UpdateOverlay
    {
        (OOVPA*)&IDirect3DDevice8_UpdateOverlay_1_0_5558,

        XTL::EmuIDirect3DDevice8_UpdateOverlay,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_UpdateOverlay"
        #endif
    },
    // IDirect3DDevice8::GetOverlayUpdateStatus
    {
        (OOVPA*)&IDirect3DDevice8_GetOverlayUpdateStatus_1_0_5558,

        XTL::EmuIDirect3DDevice8_GetOverlayUpdateStatus,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetOverlayUpdateStatus"
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
    // IDirect3DDevice8::CreatePalette2
    {
        (OOVPA*)&IDirect3DDevice8_CreatePalette2_1_0_5558,

        XTL::EmuIDirect3DDevice8_CreatePalette2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePalette2"
        #endif
    },
    // IDirect3DDevice8::SetPalette
    {
        (OOVPA*)&IDirect3DDevice8_SetPalette_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetPalette,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPalette"
        #endif
    },
    // IDirect3DDevice8::Swap (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_5233,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
    // IDirect3DDevice8::BlockUntilVerticalBlank
    {
        (OOVPA*)&IDirect3DDevice8_BlockUntilVerticalBlank_1_0_5558,

        XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockUntilVerticalBlank"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_5233,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVerticesUP
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVerticesUP_1_0_5558,

        XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVerticesUP"
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
    // IDirect3DDevice8::CreateTexture2
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture2_1_0_5558,

        XTL::EmuIDirect3DDevice8_CreateTexture2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture2"
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
        (OOVPA*)&IDirect3DDevice8_End_1_0_5558,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
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
    // IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilEnable"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilFail_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilFail"
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
    // IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DDevice8::LightEnable
    {
        (OOVPA*)&IDirect3DDevice8_LightEnable_1_0_5558,

        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
    // IDirect3DVertexBuffer8::Lock2 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DVertexBuffer8_Lock2_1_0_4627,

        XTL::EmuIDirect3DVertexBuffer8_Lock2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_Lock2"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4432,

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
    // IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
    // IDirect3DDevice8::SetVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShader_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetIndices (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_5233,

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
    // IDirect3DDevice8::SwitchTexture (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SwitchTexture_1_0_4361,

        XTL::EmuIDirect3DDevice8_SwitchTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SwitchTexture"
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
    // IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
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
    // IDirect3DDevice8::SetViewport
    {
        (OOVPA*)&IDirect3DDevice8_SetViewport_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetViewport"
        #endif
    },
    // IDirect3DDevice8::GetBackBuffer2
    {
        (OOVPA*)&IDirect3DDevice8_GetBackBuffer2_1_0_5558,

        XTL::EmuIDirect3DDevice8_GetBackBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackBuffer2"
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
    // IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DSurface8_GetDesc_1_0_4361,

        XTL::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc"
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
        (OOVPA*)&IDirect3DTexture8_LockRect_1_0_5558,

        XTL::EmuIDirect3DTexture8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DTexture8_LockRect"
        #endif
    },
    // IDirect3DSurface8::LockRect (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_4361,

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
    // IDirect3DDevice8::GetRenderTarget2 (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget2_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetRenderTarget2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget2"
        #endif
    },
    // IDirect3D8::CheckDeviceMultiSampleType
    {
        (OOVPA*)&IDirect3D8_CheckDeviceMultiSampleType_1_0_5558,

        XTL::EmuIDirect3D8_CheckDeviceMultiSampleType,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CheckDeviceMultiSampleType"
        #endif
    },
    // IDirect3D8::GetDeviceCaps (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3D8_GetDeviceCaps_1_0_5233,

        XTL::EmuIDirect3D8_GetDeviceCaps,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_GetDeviceCaps"
        #endif
    },
    // IDirect3D8::SetPushBufferSize
    {
        (OOVPA*)&IDirect3D8_SetPushBufferSize_1_0_5558,
        XTL::EmuIDirect3D8_SetPushBufferSize,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_SetPushBufferSize"
        #endif
    },
    // IDirect3DDevice8::DrawVertices (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_5233,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // D3D::SetFence (XREF)
    {
        (OOVPA*)&D3D_SetFence_1_0_5558,0,

        #ifdef _DEBUG_TRACE
        "D3D::SetFence (XREF)"
        #endif
    },
    // IDirect3DDevice8::InsertFence (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_InsertFence_1_0_5233,

        XTL::EmuIDirect3DDevice8_InsertFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_InsertFence"
        #endif
    },
    // IDirect3DDevice8::BlockOnFence
    {
        (OOVPA*)&IDirect3DDevice8_BlockOnFence_1_0_5558,

        XTL::EmuIDirect3DDevice8_BlockOnFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockOnFence"
        #endif
    },
    // D3D::BlockOnResource (XREF)
    {
        (OOVPA*)&D3D_BlockOnResource_1_0_5558, 0,

        #ifdef _DEBUG_TRACE
        "D3D::BlockOnResource (XREF)"
        #endif
    },
    // IDirect3DResource8::BlockUntilNotBusy
    {
        (OOVPA*)&IDirect3DResource8_BlockUntilNotBusy_1_0_5558,

        XTL::EmuIDirect3DResource8_BlockUntilNotBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_BlockUntilNotBusy"
        #endif
    },
    // IDirect3DVertexBuffer8::GetDesc (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DVertexBuffer8_GetDesc_1_0_5233,

        XTL::EmuIDirect3DVertexBuffer8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_GetDesc"
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
    // IDirect3DDevice8::SetScissors
    {
        (OOVPA*)&IDirect3DDevice8_SetScissors_1_0_5558,

        XTL::EmuIDirect3DDevice8_SetScissors,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetScissors"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface2 (* unchanged since 5233 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface2"
        #endif
    }
};

// ******************************************************************
// * D3D8_1_0_5558_SIZE
// ******************************************************************
uint32 D3D8_1_0_5558_SIZE = sizeof(D3D8_1_0_5558);
