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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_IsBusy
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_IsBusy_1_0_3925 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x90 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x04 }, // (Offset,Value)-Pair #3
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #4

        { 0x1E, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1F, 0x06 }, // (Offset,Value)-Pair #6

        { 0x25, 0xC3 }, // (Offset,Value)-Pair #7

        { 0x33, 0xC3 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDeviceCaps
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetDeviceCaps_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetDeviceCaps+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1
        { 0x01, 0x57 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetDeviceCaps+0x06 : mov ecx, 0x35
        { 0x06, 0xB9 }, // (Offset,Value)-Pair #3
        { 0x07, 0x35 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetDeviceCaps+0x10 : rep movsd
        { 0x10, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x11, 0xA5 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetDeviceCaps+0x14 : retn 0x04
        { 0x14, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x15, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_BeginVisibilityTest
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_BeginVisibilityTest_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_BeginVisibilityTest+0x0D : mov ecx, 1
        { 0x0D, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0E, 0x01 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_BeginVisibilityTest+0x12 : mov dword ptr [eax], 0x000817C8
        { 0x12, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x13, 0x00 }, // (Offset,Value)-Pair #5
        { 0x14, 0xC8 }, // (Offset,Value)-Pair #6
        { 0x15, 0x17 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_BeginVisibilityTest+0x1E : add eax, 0x0C
        { 0x1E, 0x83 }, // (Offset,Value)-Pair #9
        { 0x1F, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x20, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
SOOVPA<9> D3DDevice_GetMaterial_1_0_3925 = 
{
    0,  // Large == 0
    9,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0C, 0xB0 },
        { 0x0D, 0x18 },
        { 0x0E, 0x0B },
        { 0x11, 0xB9 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_EndVisibilityTest
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_EndVisibilityTest_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_EndVisibilityTest+0x11 : mov eax, 0x8007000E
        { 0x11, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x12, 0x0E }, // (Offset,Value)-Pair #2
        { 0x13, 0x00 }, // (Offset,Value)-Pair #3
        { 0x14, 0x07 }, // (Offset,Value)-Pair #4
        { 0x15, 0x80 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_EndVisibilityTest+0x2D : mov dword ptr [eax], 0x000817CC
        { 0x2D, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x2E, 0x00 }, // (Offset,Value)-Pair #7
        { 0x2F, 0xCC }, // (Offset,Value)-Pair #8
        { 0x30, 0x17 }, // (Offset,Value)-Pair #9
        { 0x31, 0x08 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_EndVisibilityTest+0x43 : add eax, 0x0C
        { 0x43, 0x83 }, // (Offset,Value)-Pair #11
        { 0x44, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x45, 0x0C }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetVisibilityTestResult
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_GetVisibilityTestResult_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetVisibilityTestResult+0x0C : shr ecx, 8
        { 0x0C, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xE9 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetVisibilityTestResult+0x0F : and eax, 0xFF
        { 0x0F, 0x25 }, // (Offset,Value)-Pair #4
        { 0x10, 0xFF }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetVisibilityTestResult+0x2B : mov eax, 0x88760828
        { 0x2B, 0xB8 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x28 }, // (Offset,Value)-Pair #8
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #9
        { 0x2E, 0x76 }, // (Offset,Value)-Pair #10
        { 0x2F, 0x88 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
// * Actually, this is D3D::CDevice::KickOff
// ******************************************************************
SOOVPA<10> IDirect3D8_KickOffAndWaitForIdle_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x01, 0xA1 }, // (Offset,Value)-Pair #1

        { 0x32, 0x81 }, // (Offset,Value)-Pair #2
        { 0x33, 0xCA }, // (Offset,Value)-Pair #3
        { 0x36, 0x01 }, // (Offset,Value)-Pair #4

        { 0x5E, 0x89 }, // (Offset,Value)-Pair #5
        { 0x5F, 0x50 }, // (Offset,Value)-Pair #6
        { 0x60, 0x40 }, // (Offset,Value)-Pair #7

        { 0x90, 0x83 }, // (Offset,Value)-Pair #8
        { 0x91, 0xEA }, // (Offset,Value)-Pair #9
        { 0x92, 0x02 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_LoadVertexShader_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_LoadVertexShader+0x07 : mov al, [ebx+0x0C]
        { 0x07, 0x8A }, // (Offset,Value)-Pair #1
        { 0x08, 0x43 }, // (Offset,Value)-Pair #2
        { 0x09, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_LoadVertexShader+0x3D : mov dword ptr [eax], 0x00041E9C
        { 0x3D, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x3E, 0x00 }, // (Offset,Value)-Pair #5
        { 0x3F, 0x9C }, // (Offset,Value)-Pair #6
        { 0x40, 0x1E }, // (Offset,Value)-Pair #7
        { 0x41, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_LoadVertexShader+0x4E : mov [ebx], ebx
        { 0x4E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x13 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SelectVertexShader
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SelectVertexShader_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SelectVertexShader+0x16 : or ecx, 0x3A0
        { 0x16, 0x81 }, // (Offset,Value)-Pair #1
        { 0x17, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x18, 0xA0 }, // (Offset,Value)-Pair #3
        { 0x19, 0x03 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SelectVertexShader+0x3F : mov dword ptr [eax], 0x00081E94
        { 0x3F, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x40, 0x00 }, // (Offset,Value)-Pair #6
        { 0x41, 0x94 }, // (Offset,Value)-Pair #7
        { 0x42, 0x1E }, // (Offset,Value)-Pair #8
        { 0x43, 0x08 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SelectVertexShader+0x51 : mov [esi], eax
        { 0x51, 0x89 }, // (Offset,Value)-Pair #10
        { 0x52, 0x06 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_BlockUntilVerticalBlank_1_0_3925 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0;push 0;push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x0B : mov dword ptr [eax+0x24F4],
        { 0x0B, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x0C, 0x80 }, // (Offset,Value)-Pair #8
        { 0x0D, 0xF4 }, // (Offset,Value)-Pair #9
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #10
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : and eax, 0x24F0
        { 0x17, 0x05 }, // (Offset,Value)-Pair #12
        { 0x18, 0xF0 }, // (Offset,Value)-Pair #13
        { 0x19, 0x24 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetVerticalBlankCallback_1_0_3925 =
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

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x24EC], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #6
        { 0x0C, 0xEC }, // (Offset,Value)-Pair #7
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x11, 0x04 }, // (Offset,Value)-Pair #11
        { 0x12, 0x00 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderTarget_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderTarget_1_0_3948 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderTarget+0x00 : sub esp, 0x30
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x30 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderTarget+0x32 : and ecx, 0x0F
        { 0x32, 0x83 }, // (Offset,Value)-Pair #4
        { 0x33, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x34, 0x0F }, // (Offset,Value)-Pair #6

		// IDirect3DDevice8_SetRenderTarget+0x69 : push ebx
        { 0x69, 0x53 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderTarget+0xDA : push edx
        { 0xDA, 0x52 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * D3DSurface_GetDesc
// ******************************************************************
SOOVPA<7> D3DSurface_GetDesc_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x04 },
        { 0x0A, 0x00 },
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
        { 0x13, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetTransform
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_GetTransform_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetTransform+0x0A : add esi, 0x22
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0B, 0xC6 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x22 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetTransform+0x12 : shl esi, 6; add esi, eax
        { 0x12, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x13, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x14, 0x06 }, // (Offset,Value)-Pair #6
        { 0x15, 0x03 }, // (Offset,Value)-Pair #7
        { 0x16, 0xF0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetTransform+0x17 : mov ecx, 0x10
        { 0x17, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x18, 0x10 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetTransform+0x20 : retn 0x08
        { 0x20, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x21, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetStreamSource
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetStreamSource_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetVertexShader
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetVertexShader_1_0_3925 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexShader+0x05 : test bl, 1
        { 0x05, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x07, 0x01 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShader+0x10 : mov eax, [esi+0x0470]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #4
        { 0x11, 0x86 }, // (Offset,Value)-Pair #5
        { 0x12, 0x70 }, // (Offset,Value)-Pair #6
        { 0x13, 0x04 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetVertexShader+0x6D : retn 0x04
        { 0x6D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShader+0x8C : mov dword ptr [eax], 0x81E94
        { 0x8C, 0xC7 }, // (Offset,Value)-Pair #10
        { 0x8D, 0x40 }, // (Offset,Value)-Pair #11
        { 0x8E, 0x08 }, // (Offset,Value)-Pair #12
        { 0x8F, 0x94 }, // (Offset,Value)-Pair #13
        { 0x90, 0x1E }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_CreatePixelShader
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_CreatePixelShader_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetPixelShader_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetPixelShader+0x0E : mov esi, [ebx+0x0414]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xB3 }, // (Offset,Value)-Pair #2
        { 0x10, 0x14 }, // (Offset,Value)-Pair #3
        { 0x11, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetPixelShader+0x34 : or eax, 0x40
        { 0x35, 0xC8 }, // (Offset,Value)-Pair #5
        { 0x36, 0x40 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetPixelShader+0x68 : retn 0x04
        { 0x68, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x69, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xE4, 0xA5 }, // (Offset,Value)-Pair #10

    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetIndices_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_DrawIndexedVertices_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_DrawIndexedVertices+0x0E : mov eax, [esi+0x0478]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x86 }, // (Offset,Value)-Pair #2
        { 0x10, 0x78 }, // (Offset,Value)-Pair #3
        { 0x11, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawIndexedVertices+0x44 : mov dword ptr [ebx-8], 0x000417FC
        { 0x44, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x45, 0x43 }, // (Offset,Value)-Pair #6
        { 0x46, 0xF8 }, // (Offset,Value)-Pair #7
        { 0x47, 0xFC }, // (Offset,Value)-Pair #8
        { 0x48, 0x17 }, // (Offset,Value)-Pair #9
        { 0x49, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_DrawIndexedVertices+0xDA : lea ebx, [ebx+0]
        { 0xDA, 0x8D }, // (Offset,Value)-Pair #11
        { 0xDB, 0x9B }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_Begin
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_Begin_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_Begin+0x20 : mov dword ptr [eax], 0x417FC
        { 0x20, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x21, 0x00 }, // (Offset,Value)-Pair #2
        { 0x22, 0xFC }, // (Offset,Value)-Pair #3
        { 0x23, 0x17 }, // (Offset,Value)-Pair #4
        { 0x24, 0x04 }, // (Offset,Value)-Pair #5
        { 0x25, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_Begin+0x2E : shl edx, 6
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #7
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #8
        { 0x30, 0x08 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_Begin+0x38 : retn 0x08
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x39, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexData2f
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetVertexData2f_1_0_3925 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexData2f+0x11 : lea edx, ds:abs[ecx*8]
        { 0x11, 0x8D }, // (Offset,Value)-Pair #1
        { 0x12, 0x14 }, // (Offset,Value)-Pair #2
        { 0x13, 0xCD }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexData2f+0x1C : mov [eax], edx; mov edx, [esp+0x10]
        { 0x1C, 0x89 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x10 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x8B }, // (Offset,Value)-Pair #6
        { 0x1F, 0x54 }, // (Offset,Value)-Pair #7
        { 0x20, 0x24 }, // (Offset,Value)-Pair #8
        { 0x21, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData2f+0x28 : add eax, 0x0C
        { 0x28, 0x83 }, // (Offset,Value)-Pair #10
        { 0x29, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x2A, 0x0C }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_SetVertexData2f+0x2E : retn 0x0C
        { 0x2E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x2F, 0x0C }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexData2s
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexData2s_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexData2s+0x11 : lea edx, ds:abs[ecx*4]
        { 0x11, 0x8D }, // (Offset,Value)-Pair #1
        { 0x12, 0x14 }, // (Offset,Value)-Pair #2
        { 0x13, 0x8D }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexData2s+0x24 : shl ecx, 0x10
        { 0x24, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x25, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x26, 0x10 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVertexData2s+0x29 : mov [eax+4], ecx
        { 0x29, 0x89 }, // (Offset,Value)-Pair #7
        { 0x2A, 0x48 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData2s+0x32 : retn 0x0C
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x33, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexData4f
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetVertexData4f_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexData4f+0x11 : cmp ecx, -1; jnz +0x07
        { 0x11, 0x83 }, // (Offset,Value)-Pair #1
        { 0x12, 0xF9 }, // (Offset,Value)-Pair #2
        { 0x13, 0xFF }, // (Offset,Value)-Pair #3
        { 0x14, 0x75 }, // (Offset,Value)-Pair #4
        { 0x15, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexData4f+0x1D : add ecx, 0x01A0
        { 0x1D, 0x81 }, // (Offset,Value)-Pair #6
        { 0x1E, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xA0 }, // (Offset,Value)-Pair #8
        { 0x20, 0x01 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData4f+0x50 : retn 0x14
        { 0x50, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x51, 0x14 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexDataColor
// ******************************************************************
SOOVPA<17> IDirect3DDevice8_SetVertexDataColor_1_0_3925 =
{
    0,  // Large == 0
    17, // Count == 17

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetVertexDataColor+0x1F : mov byte ptr [esp+?]
        { 0x1F, 0x0F }, // (Offset,Value)-Pair #1
        { 0x20, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x21, 0x54 }, // (Offset,Value)-Pair #3
        { 0x22, 0x24 }, // (Offset,Value)-Pair #4
        { 0x23, 0x12 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexDataColor+0x26 : add edi, 0xFF; shl edi, 0x10
        { 0x26, 0x81 }, // (Offset,Value)-Pair #6
        { 0x27, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x28, 0xFF }, // (Offset,Value)-Pair #8
        { 0x2C, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x2D, 0xE7 }, // (Offset,Value)-Pair #10
        { 0x2E, 0x10 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetVertexDataColor+0x31 : and ecx, 0xFF00FF00
        { 0x31, 0x81 }, // (Offset,Value)-Pair #12
        { 0x32, 0xE1 }, // (Offset,Value)-Pair #13
        { 0x33, 0x00 }, // (Offset,Value)-Pair #14
        { 0x34, 0xFF }, // (Offset,Value)-Pair #15
        { 0x35, 0x00 }, // (Offset,Value)-Pair #16
        { 0x36, 0xFF }, // (Offset,Value)-Pair #17
    }
};

// ******************************************************************
// * IDirect3DDevice8_End
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_End_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_End+0x17 : mov dword ptr [eax], 0x417FC
        { 0x17, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x18, 0x00 }, // (Offset,Value)-Pair #2
        { 0x19, 0xFC }, // (Offset,Value)-Pair #3
        { 0x1A, 0x17 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_End+0x2C : test ah, 0x10
        { 0x2C, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x2D, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_End+0x2F : jz +0x07
        { 0x2F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x30, 0x07 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_End+0x40 : retn
        { 0x40, 0xC3 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_Clear_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_CreateVertexShader
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_CreateVertexShader_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

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
SOOVPA<10> IDirect3DDevice8_SetVertexShaderConstant_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetTextureState_BorderColor
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetTextureState_BorderColor_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTextureState_BorderColor+0x13 : shl eax, 0x07
        { 0x13, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x14, 0xE2 }, // (Offset,Value)-Pair #2
        { 0x15, 0x06 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureState_BorderColor+0x16 : add edx, 0x41B24
        { 0x16, 0x81 }, // (Offset,Value)-Pair #4
        { 0x17, 0xC2 }, // (Offset,Value)-Pair #5
        { 0x18, 0x24 }, // (Offset,Value)-Pair #6
        { 0x19, 0x1B }, // (Offset,Value)-Pair #7
        { 0x1A, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTextureState_BorderColor+0x2A : shl ecx, 7
        { 0x2A, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x2B, 0xE1 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x07 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetTextureState_BorderColor+0x34 : retn 0x08
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x35, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_PSTextureModes
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_PSTextureModes_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_PSTextureModes+0x0C : or edx, 0x400
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xCA }, // (Offset,Value)-Pair #2
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_PSTextureModes+0x12 : mov [eax+0x420], ecx
        { 0x12, 0x89 }, // (Offset,Value)-Pair #5
        { 0x13, 0x88 }, // (Offset,Value)-Pair #6
        { 0x14, 0x20 }, // (Offset,Value)-Pair #7
        { 0x15, 0x04 }, // (Offset,Value)-Pair #8
        { 0x16, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_PSTextureModes+0x21 : retn 4
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilFail
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_StencilFail_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_StencilFail+0x19 : mov ecx, 2
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x1A, 0x02 }, // (Offset,Value)-Pair #2
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_StencilFail+0x3E : or ecx, 1
        { 0x3E, 0x83 }, // (Offset,Value)-Pair #4
        { 0x3F, 0xC9 }, // (Offset,Value)-Pair #5
        { 0x40, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_StencilFail+0x48 : mov dword ptr [eax], 0x00041D84
        { 0x48, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x49, 0x00 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x84 }, // (Offset,Value)-Pair #9
        { 0x4B, 0x1D }, // (Offset,Value)-Pair #10
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #11
        { 0x4D, 0x00 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<15> IDirect3DDevice8_SetRenderState_CullMode_1_0_3925 =
{
    0,  // Large == 0
    15, // Count == 15

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_CullMode+0x42 : mov dword ptr [eax+8], 0x4039C
        { 0x42, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x43, 0x40 }, // (Offset,Value)-Pair #2
        { 0x44, 0x08 }, // (Offset,Value)-Pair #3
        { 0x45, 0x9C }, // (Offset,Value)-Pair #4
        { 0x46, 0x03 }, // (Offset,Value)-Pair #5
        { 0x47, 0x04 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_CullMode+0x49 : add eax, 0x10
        { 0x49, 0x83 }, // (Offset,Value)-Pair #7
        { 0x4A, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x4B, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullMode+0x5F : retn 4
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x60, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_CullMode+0x4D : add edx, 0x404
        { 0x4D, 0x81 }, // (Offset,Value)-Pair #12
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #14
        { 0x50, 0x04 }, // (Offset,Value)-Pair #15
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Simple
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_Simple_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetTransform
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetTransform_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTransform+0x3A : fld dword ptr [edx+0x28]
        { 0x3A, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x3B, 0x42 }, // (Offset,Value)-Pair #2
        { 0x3C, 0x28 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTransform+0x47 : test ah, 0x44
        { 0x47, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x48, 0xC4 }, // (Offset,Value)-Pair #5
        { 0x49, 0x44 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTransform+0x55 : fnstsw ax
        { 0x55, 0xDF }, // (Offset,Value)-Pair #7
        { 0x56, 0xE0 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTransform+0x7A : fdivp st(1), st
        { 0x7A, 0xDE }, // (Offset,Value)-Pair #9
        { 0x7B, 0xF9 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FogColor
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_FogColor_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetRenderState_ZBias
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_ZBias_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_ZEnable_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DDevice8_GetRenderTarget
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_GetRenderTarget_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget+0x05 : mov eax, [eax + 0x040C]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

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
SOOVPA<10> IDirect3DDevice8_GetDepthStencilSurface_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DResource8_IsBusy
// ******************************************************************
SOOVPA<11> IDirect3DResource8_IsBusy_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DResource8_IsBusy+0x24 : test eax, 0x780000
        { 0x24, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x25, 0x00 }, // (Offset,Value)-Pair #2
        { 0x26, 0x00 }, // (Offset,Value)-Pair #3
        { 0x27, 0x78 }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_IsBusy+0x35 : jnz +0x44
        { 0x35, 0x75 }, // (Offset,Value)-Pair #5
        { 0x36, 0x44 }, // (Offset,Value)-Pair #6

        // IDirect3DResource8_IsBusy+0x4E : mov eax, [ecx+0x14]
        { 0x4E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x4F, 0x41 }, // (Offset,Value)-Pair #8
        { 0x50, 0x14 }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_IsBusy+0x79 : jnb +0x09
        { 0x79, 0x73 }, // (Offset,Value)-Pair #10
        { 0x7A, 0x09 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DSurface8_LockRect
// ******************************************************************
SOOVPA<16> IDirect3DSurface8_LockRect_1_0_3925 =
{
    0,  // Large == 0
    16, // Count == 16

    XRefNotSaved,
    XRefNotUsed,

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

        // IDirect3DSurface8_LockRect+0x11 : push ecx; push edx; push 0; push 0
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x12, 0x52 }, // (Offset,Value)-Pair #11
        { 0x13, 0x6A }, // (Offset,Value)-Pair #12
        { 0x15, 0x6A }, // (Offset,Value)-Pair #13

        // IDirect3DSurface8_LockRect+0x18 : call [abs]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #14

        // IDirect3DSurface8_LockRect+0x11 : retn 0x10
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #15
        { 0x1E, 0x10 }, // (Offset,Value)-Pair #16
    }
};

// ******************************************************************
// * IDirect3DPalette8_Lock
// ******************************************************************
SOOVPA<13> IDirect3DPalette8_Lock_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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

    XRefNotSaved,
    XRefNotUsed,

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
// * IDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
SOOVPA<11> IDirect3DTexture8_GetSurfaceLevel_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

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
// * Lock2DSurface
// ******************************************************************
SOOVPA<11> Lock2DSurface_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x20 }, // (Offset,Value)-Pair #3

        { 0x3F, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x40, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x41, 0x40 }, // (Offset,Value)-Pair #6

        { 0x71, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x72, 0xEA }, // (Offset,Value)-Pair #8
        { 0x73, 0x03 }, // (Offset,Value)-Pair #9

        { 0x98, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x99, 0x18 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
SOOVPA<10> Get2DSurfaceDesc_1_0_3925 =
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

        // Get2DSurfaceDesc+0x56 : cmp eax, [edx+0x2154]
        { 0x56, 0x3B }, // (Offset,Value)-Pair #5
        { 0x57, 0x82 }, // (Offset,Value)-Pair #6
        { 0x58, 0x54 }, // (Offset,Value)-Pair #7
        { 0x59, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * D3DDevice_GetVertexShaderSize
// ******************************************************************
SOOVPA<7> X_D3DDevice_GetVertexShaderSize_1_0_3925 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x01, 0x44 },
        { 0x04, 0x85 },
        { 0x07, 0x09 },
        { 0x0A, 0x24 },
        { 0x0D, 0x51 },
        { 0x10, 0x10 },
        { 0x13, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetGammaRamp
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetGammaRamp_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetGammaRamp+0x11 : and eax, 1; push ebx
        { 0x11, 0x83 }, // (Offset,Value)-Pair #1
        { 0x12, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x13, 0x01 }, // (Offset,Value)-Pair #3
        { 0x14, 0x53 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetGammaRamp+0x2F : rep movsd
        { 0x2F, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x30, 0xA5 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetGammaRamp+0x3E : push ebx; mov ecx, edx
        { 0x3E, 0x53 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x8B }, // (Offset,Value)-Pair #8
        { 0x40, 0xCA }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetMaterial_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetMaterial+0x0B : lea edi, [eax+0xB18]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x18 }, // (Offset,Value)-Pair #3
        { 0x0E, 0x0B }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetMaterial+0x11 : mov ecx, 0x11
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x12, 0x11 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetMaterial+0x1C : or ecx, 0x20
        { 0x1B, 0x83 }, // (Offset,Value)-Pair #7
        { 0x1C, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x1D, 0x20 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetMaterial+0x23 : retn 0x04
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x24, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_AddRef
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_AddRef_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_AddRef+0x05 : mov ecx, [eax+0x0408]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x08 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_AddRef+0x05 : mov [eax+0x0408], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_Unknown1
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_Unknown1_1_0_3925 = 
{
	0,	// Large == 0
	9,	// Count == 9

	XRefNotSaved,
	XRefNotUsed,

	{
		// IDirect3DDevice8_Unknown1+0x00 : xor eax, eax
		{ 0x00, 0x33 },
		{ 0x01, 0xC0 },
		// IDirect3DDevice8_Unknown1+0x13 : call ds:AvSendTVEncoderOption
		{ 0x13, 0xFF },
		{ 0x14, 0x15 },
		{ 0x15, 0x10 },
		{ 0x16, 0x49 },
		{ 0x17, 0x2A },
		{ 0x18, 0x00 },
		// IDirect3DDevice8_Unknown1+0x1E : retn
		{ 0x1E, 0xC3 },
	}
};

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_GetViewport_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetViewport+0x0B : lea esi, [eax+0x0B00]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x00 }, // (Offset,Value)-Pair #5
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
// * IDirect3DDevice8_GetGammaRamp
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_GetGammaRamp_1_0_3925 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

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
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
SOOVPA<14> D3DDevice_GetDisplayFieldStatus_1_0_3925 = 
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_GetDisplayFieldStatus+0x05 : mov edx, [eax+0x2B28]
		{ 0x05, 0x8B },
		{ 0x06, 0x90 },
		{ 0x07, 0x28 },
		{ 0x08, 0x2B },
		// IDirect3DDevice8_GetDisplayFieldStatus+0x12 : test [eax+0x2510], 0x1200000
		{ 0x12, 0xF7 },
		{ 0x13, 0x80 },
		{ 0x14, 0x10 },
		{ 0x15, 0x25 },
		// IDirect3DDevice8_GetDisplayFieldStatus+0x1E : mov dx, 0x80C0
		{ 0x1E, 0x66 },
		{ 0x1F, 0xBA },
		{ 0x20, 0xC0 },
		{ 0x21, 0x80 },
		// IDirect3DDevice8_GetDisplayFieldStatus+0x3C : retn 4
		{ 0x3C, 0xC2 },
		{ 0x3D, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
SOOVPA<7> D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x8B },
        { 0x1D, 0x8B },
        { 0x26, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0x0B },
        { 0x40, 0x89 },
        { 0x49, 0xC2 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_VertexBlend
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_VertexBlend_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_VertexBlend+0x0A : or ecx, 2
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0B, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x02 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x1A : mov dword ptr [eax], 0x40328
        { 0x1A, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #5
		{ 0x1C, 0x28 }, // (Offset,Value)-Pair #6
		{ 0x1D, 0x03 }, // (Offset,Value)-Pair #7
		{ 0x1E, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x23 : add eax, 8
        { 0x23, 0x83 }, // (Offset,Value)-Pair #9
        { 0x24, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x25, 0x08 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_VertexBlend+0x2F : retn 0x04
        { 0x2F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x30, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_BackFillMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_BackFillMode_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_BackFillMode+0x2B : jnz +0x2
        { 0x2B, 0x75 }, // (Offset,Value)-Pair #1
        { 0x2C, 0x02 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x32 : mov dword ptr [eax], abs
        { 0x32, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x33, 0x00 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x3B : add eax, 0x0C; mov [edi], eax; pop edi
        { 0x3B, 0x83 }, // (Offset,Value)-Pair #5
        { 0x3C, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x0C }, // (Offset,Value)-Pair #7
        { 0x3E, 0x89 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x07 }, // (Offset,Value)-Pair #9
        { 0x40, 0x5F }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_BackFillMode+0x48 : retn 0x04
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x49, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TwoSidedLighting
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x06 : or dword ptr [eax+8], 0x20
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0x48 }, // (Offset,Value)-Pair #2
        { 0x08, 0x08 }, // (Offset,Value)-Pair #3
        { 0x09, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x3B : mov dword ptr [eax], 0x8038C
        { 0x3B, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x8C }, // (Offset,Value)-Pair #7
        { 0x3E, 0x03 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x44 : add eax, 0x0C
        { 0x44, 0x83 }, // (Offset,Value)-Pair #9
        { 0x45, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x46, 0x0C }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetTextureState_TwoSidedLighting+0x51 : retn 0x04
        { 0x51, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x52, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormals
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_3925 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x1D : mov [esi], eax
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #1
        { 0x1E, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x1F : or dword ptr [esi+8], 2
        { 0x1F, 0x83 }, // (Offset,Value)-Pair #3
        { 0x20, 0x4E }, // (Offset,Value)-Pair #4
        { 0x21, 0x08 }, // (Offset,Value)-Pair #5
        { 0x22, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_NormalizeNormals+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_FrontFace
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_FrontFace_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_FrontFace+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

		// IDirect3DDevice8_SetRenderState_FrontFace+0x07 : push esi
		{ 0x07, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_FrontFace+0xD : mov ecx, [esp+0x08]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #3
        { 0x0E, 0x4C }, // (Offset,Value)-Pair #4
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #5
        { 0x10, 0x08 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_FrontFace+0x1A : add eax, 8
        { 0x1A, 0x83 }, // (Offset,Value)-Pair #7
        { 0x1B, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_TextureFactor
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_TextureFactor_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x0414]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0x14 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x2E : rep stosd
		{ 0x2E, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x2F, 0xAB }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x3C : retn 0x04
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x3D, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_TextureFactor+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_LogicOp
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_LogicOp_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_LogicOp+0x11 : test ecx, ecx
        { 0x11, 0x85 }, // (Offset,Value)-Pair #1
        { 0x12, 0xC9 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_LogicOp+0x2D : mov dword ptr [eax], 0x817BC
        { 0x2D, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x00 }, // (Offset,Value)-Pair #4
        { 0x2F, 0xBC }, // (Offset,Value)-Pair #5
        { 0x30, 0x17 }, // (Offset,Value)-Pair #6
        { 0x31, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_LogicOp+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_EdgeAntiAlias
// ******************************************************************
SOOVPA<11> IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_3925 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x17 : mov [eax+0x04], ecx
        { 0x17, 0x89 }, // (Offset,Value)-Pair #1
        { 0x18, 0x48 }, // (Offset,Value)-Pair #2
        { 0x19, 0x04 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x1A : mov [eax+0x08], ecx
        { 0x1A, 0x89 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x48 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #6

		// IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x1D : add eax, 0x0C
		{ 0x1D, 0x83 }, // (Offset,Value)-Pair #7
		{ 0x1E, 0xC0 }, // (Offset,Value)-Pair #8
		{ 0x1F, 0x0C }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_EdgeAntiAlias+0x29 : retn 0x04
        { 0x29, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x2A, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleMask
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_3925 =
{
    0,  // Large == 0
    13, // Count == 13

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x0B : mov ecx, esi
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0xCE }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x32 : and ecx, 0x0F; shl edx, 0x10
        { 0x32, 0x83 }, // (Offset,Value)-Pair #3
        { 0x33, 0xE1 }, // (Offset,Value)-Pair #4
        { 0x34, 0x0F }, // (Offset,Value)-Pair #5
        { 0x35, 0xC1 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x3A : mov dword ptr [eax], 0x41D7C
        { 0x3A, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x00 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x7C }, // (Offset,Value)-Pair #9
        { 0x3D, 0x1D }, // (Offset,Value)-Pair #10
        { 0x3E, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_MultiSampleMask+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x05 : mov ecx, [esp+4]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x4C }, // (Offset,Value)-Pair #2
        { 0x07, 0x24 }, // (Offset,Value)-Pair #3
        { 0x08, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x0F : mov edx, [eax+0x410]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #5
        { 0x10, 0x90 }, // (Offset,Value)-Pair #6
        { 0x11, 0x10 }, // (Offset,Value)-Pair #7
        { 0x12, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x15 : mov eax, [eax+0x40C]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #9
        { 0x16, 0x80 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleMode+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x23, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
SOOVPA<14> D3DDevice_PersistDisplay_1_0_3925 = 
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // D3DDevice_PersistDisplay+0x25 : mov eax, [ebx+0x408]
		{ 0x25, 0x8B },
		{ 0x26, 0x83 },
		{ 0x27, 0x08 },
		{ 0x28, 0x04 },
		{ 0x29, 0x00 },
		{ 0x2A, 0x00 },
		// D3DDevice_PersistDisplay+0x3A : lea edi, [ebx+0x2308]
		{ 0x3a, 0x8D },
		{ 0x3B, 0xBB },
		{ 0x3C, 0x08 },
		{ 0x3D, 0x23 },
		{ 0x3E, 0x00 },
		{ 0x3F, 0x00 },
		// D3DDevice_PersistDisplay+0x70 : shl edx, cl
		{ 0x70, 0xD3 },
		{ 0x71, 0xE2 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ShadowFunc
// ******************************************************************
SOOVPA<14> IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_3925 =
{
    0,  // Large == 0
    14, // Count == 14

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x11 : mov dword ptr [eax], 0x41E6C
		{ 0x11, 0xC7 }, // (Offset,Value)-Pair #2
		{ 0x12, 0x00 }, // (Offset,Value)-Pair #3
		{ 0x13, 0x6C }, // (Offset,Value)-Pair #4
		{ 0x14, 0x1E }, // (Offset,Value)-Pair #5
		{ 0x15, 0x04 }, // (Offset,Value)-Pair #6
		{ 0x16, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x17 : lea edx, [ecx-0x0200]
        { 0x17, 0x8D }, // (Offset,Value)-Pair #8
        { 0x18, 0x91 }, // (Offset,Value)-Pair #9
        { 0x19, 0x00 }, // (Offset,Value)-Pair #10
        { 0x1A, 0xFE }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_ShadowFunc+0x20 : add eax, 0x08
        { 0x20, 0x83 }, // (Offset,Value)-Pair #12
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #13
        { 0x22, 0x08 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_LineWidth_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x508]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2
        { 0x17, 0x08 }, // (Offset,Value)-Pair #3
        { 0x18, 0x05 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_LineWidth+0x5C : retn 0x04
        { 0x5C, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x5D, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_YuvEnable
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_SetRenderState_YuvEnable_1_0_3925 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x04 : push edi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x0B : push edi
        { 0x0B, 0x56 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x1F : mov [esi], eax
        { 0x1F, 0x89 }, // (Offset,Value)-Pair #7
        { 0x20, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_YuvEnable+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x23, 0x04 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_OcclusionCullEnable
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_3925 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x20 : jz +0x05
        { 0x20, 0x74 }, // (Offset,Value)-Pair #1
        { 0x21, 0x05 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x3B : cmp [abs], 0x1E00
        { 0x3B, 0x81 }, // (Offset,Value)-Pair #3
        { 0x41, 0x00 }, // (Offset,Value)-Pair #4
        { 0x42, 0x1E }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x4A : mov dword ptr [eax], 0x41D84
        { 0x4A, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x4B, 0x00 }, // (Offset,Value)-Pair #7
        { 0x4C, 0x84 }, // (Offset,Value)-Pair #8
        { 0x4D, 0x1D }, // (Offset,Value)-Pair #9
        { 0x4E, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_OcclusionCullEnable+0x59 : retn 0x04
        { 0x59, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x5A, 0x04 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilCullEnable
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_3925 =
{
	0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x56 },
        { 0x16, 0x8B },
        { 0x22, 0xB9 },
        { 0x2E, 0xD2 },
        { 0x3A, 0x0C },
        { 0x46, 0x03 },
        { 0x52, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
SOOVPA<8> D3DDevice_DrawVerticesUP_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1F, 0x8B },
        { 0x3E, 0xC0 },
        { 0x5E, 0x03 },
        { 0x7E, 0x07 },
        { 0x9E, 0xF0 },
        { 0xBE, 0x40 },
        { 0xDE, 0x75 },
        { 0xFE, 0x8B },
    }
};

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
SOOVPA<8> D3DDevice_DrawIndexedVerticesUP_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1F, 0x8B },
        { 0x3E, 0x8B },
        { 0x5E, 0xF0 },
        { 0x7E, 0x8B },
        { 0xA2, 0x8B },
        { 0xBE, 0x83 },
        { 0xDE, 0x75 },
        { 0xFE, 0x75 },
    }
};

// ******************************************************************
// * D3DDevice_DrawVertices
// ******************************************************************
SOOVPA<8> D3DDevice_DrawVertices_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x12, 0x8B },
        { 0x27, 0x8B },
        { 0x3A, 0x89 },
        { 0x4E, 0xFF },
        { 0x62, 0x81 },
        { 0x76, 0x00 },
        { 0x8A, 0x17 },
        { 0x9E, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
SOOVPA<7> D3DDevice_GetDisplayMode_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x09, 0xB4 },
        { 0x14, 0x8B },
        { 0x1F, 0x10 },
        { 0x2A, 0x1B },
        { 0x35, 0x8B },
        { 0x41, 0x89 },
        { 0x4B, 0x89 },
    }
};

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
SOOVPA<7> D3DDevice_SetTextureState_BumpEnv_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0E, 0x8B },
        { 0x16, 0x8B },
        { 0x22, 0x24 },
        { 0x2E, 0x24 },
        { 0x3A, 0x04 },
        { 0x46, 0x8B }, 
        { 0x52, 0xB5 },
    }
};

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
SOOVPA<7> D3DDevice_SetTextureState_ColorKeyColor_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x07, 0x56 },
        { 0x0D, 0x8B },
        { 0x13, 0x8D },
        { 0x1A, 0x8B },
        { 0x21, 0x83 },
        { 0x28, 0x07 },
        { 0x2F, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
SOOVPA<8> D3DDevice_SetVertexData4s_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x07, 0x56 },
        { 0x10, 0x08 },
        { 0x19, 0xBF },
        { 0x22, 0x24 },
        { 0x2B, 0x54 },
        { 0x34, 0x24 },
        { 0x3D, 0x08 },
        { 0x46, 0x00 },
    }
};

// ******************************************************************
// * Direct3D_SetPushBufferSize
// ******************************************************************
SOOVPA<7> Direct3D_SetPushBufferSize_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

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
// * D3DResource_GetType
// ******************************************************************
SOOVPA<7> D3DResource_GetType_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x13, 0x2A },
        { 0x28, 0x00 },
        { 0x3D, 0x00 },
        { 0x52, 0x74 },
        { 0x67, 0x00 },
        { 0x7C, 0x04 },
        { 0x91, 0x00 },
    }
};

// ******************************************************************
// * D3D_AllocContiguousMemory
// ******************************************************************
SOOVPA<6> D3D_AllocContiguousMemory_1_0_3925 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x04 },
        { 0x0E, 0x6A },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Deferred
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_Deferred_1_0_3925 = 
{
	0,	// Large == 0
	9,	// Count == 9

	XRefNotSaved,
	XRefNotUsed,

	{
		// IDirect3DDevice8_SetRenderState_Deferred+0x00 : mov eax, ds:(g_DirtyFromRenderState)-148[ecx*4]
		{ 0x00, 0x8B },
		{ 0x01, 0x04 },
		{ 0x02, 0x8D },
		// IDirect3DDevice8_SetRenderState_Deferred+0x07 : or dword ptr ds:g_Device+8,eax
		{ 0x07, 0x09 },
		{ 0x08, 0x05 },
		// IDirect3DDevice8_SetRenderState_Deferred+0x0D : mov dword ptr ds:D3D__RenderState[ecx*4], edx
		{ 0x0D, 0x89 },
		{ 0x0E, 0x14 },
		{ 0x0F, 0x8D },
		// IDirect3DDevice8_SetRenderState_Deferred+0x14 : retn
		{ 0x14, 0xC3 },
	}
};

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
SOOVPA<8> D3DDevice_SetLight_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1D, 0x0F },
        { 0x3C, 0xD8 },
        { 0x5B, 0x00 },
        { 0x7A, 0x89 },
        { 0x99, 0xA5 },
        { 0xB8, 0xC1 },
        { 0xD7, 0x00 },
        { 0xF6, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
SOOVPA<7> D3DDevice_LightEnable_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1A, 0x04 },
        { 0x36, 0x00 },
        { 0x52, 0x00 },
        { 0x6E, 0x75 },
        { 0x8A, 0x88 },
        { 0xA6, 0x75 },
        { 0xC2, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
SOOVPA<8> D3DDevice_CreateVertexShader_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x85 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x5D },
        { 0x7E, 0x01 },
        { 0x9E, 0x24 },
        { 0xBE, 0x24 },
        { 0xDE, 0x83 },
        { 0xFE, 0xC7 },
    }
};

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
SOOVPA<7> D3DDevice_GetProjectionViewportMatrix_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0xA0 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
    }
};

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
SOOVPA<7> D3DDevice_GetTile_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x03, 0x04 },
        { 0x0A, 0x56 },
        { 0x0D, 0x7C },
        { 0x12, 0x40 },
        { 0x17, 0x21 },
        { 0x1C, 0x00 },
        { 0x21, 0x5F },
    }
};

// ******************************************************************
// * D3DDevice_ApplyStateBlock
// ******************************************************************
SOOVPA<8> D3DDevice_ApplyStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1D, 0x83 },
        { 0x40, 0x83 },
        { 0x5B, 0x16 },
        { 0x7A, 0x01 },
        { 0x99, 0x46 },
        { 0xB8, 0x06 },
        { 0xD7, 0x39 },
        { 0xF6, 0x51 },
    }
};

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
SOOVPA<7> D3DDevice_CaptureStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x76 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x06 },
        { 0x7E, 0x26 },
        { 0x9E, 0xFF },
        { 0xBE, 0x04 },
        { 0xDE, 0xF8 },
    }
};

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
SOOVPA<7> D3DDevice_DeleteStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x11, 0x76 },
        { 0x24, 0x3B },
        { 0x37, 0xE8 },
        { 0x4A, 0x50 },
        { 0x5D, 0x74 },
        { 0x70, 0x06 },
        { 0x83, 0xEB },
    }
};

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
SOOVPA<8> D3DDevice_GetBackMaterial_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x5C },
        { 0x0E, 0x0B },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
    }
};

// ******************************************************************
// * D3DDevice_CreateStateBlock
// ******************************************************************
SOOVPA<8> D3DDevice_CreateStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x8B },
        { 0x3E, 0x89 },
        { 0x5E, 0x24 },
        { 0x7E, 0xF8 },
        { 0x9F, 0x01 },
        { 0xBE, 0xB6 },
        { 0xDE, 0xF8 },
        { 0xFE, 0x76 },
    }
};

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
SOOVPA<7> D3DDevice_DeletePixelShader_1_0_3925 = 
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
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
SOOVPA<7> D3DDevice_SetPixelShaderProgram_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x06, 0xA1 },
        { 0x0E, 0x88 },
        { 0x16, 0x00 },
        { 0x1E, 0x00 },
        { 0x26, 0x04 },
        { 0x32, 0xC7 },
        { 0x36, 0x00 },
    }
};

// ******************************************************************
// * D3D::KickOffAndWaitForIdle
// ******************************************************************
// * This is for the real D3D::KickOffAndWaitForIdle
// ******************************************************************
SOOVPA<5> D3D_KickOffAndWaitForIdle_1_0_3925 = 
{
    0,  // Large == 0
    5,  // Count == 5

    XREF_D3D_KickOffAndWaitForIdle,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x1C },
        { 0x0A, 0x51 },
        { 0x10, 0xC3 },
    }
};

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
SOOVPA<7> D3DDevice_SetModelView_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x12, 0x08 },
        { 0x26, 0x53 },
        { 0x3A, 0x8B },
        { 0x4E, 0x80 },
        { 0x62, 0x00 },
        { 0x76, 0x00 },
        { 0x8A, 0x0C },
    }
};

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
SOOVPA<8> D3DDevice_FlushVertexCache_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0x56 },
        { 0x07, 0x56 },
        { 0x08, 0xE8 },
        { 0x0D, 0xC7 },
        { 0x0F, 0x10 },
        { 0x12, 0x00 },
        { 0x17, 0x00 },
        { 0x1C, 0x08 },
    }
};

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
SOOVPA<8> D3DDevice_SetScissors_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1D, 0x08 },
        { 0x3C, 0x24 },
        { 0x5F, 0x8B },
        { 0x7A, 0x83 },
        { 0x99, 0xE0 },
        { 0xBA, 0xD9 },
        { 0xD9, 0x8B },
        { 0xF6, 0xDB },
    }
};

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
SOOVPA<8> D3DDevice_SetVertexShaderInput_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x03 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC7 },
        { 0x7E, 0xF8 },
        { 0x9E, 0xC6 },
        { 0xBE, 0x7F },
        { 0xDE, 0xCA },
        { 0xFE, 0x17 },
    }
};

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
SOOVPA<8> D3DDevice_PrimeVertexCache_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0D, 0x00 },
        { 0x1C, 0x55 },
        { 0x2B, 0x00 },
        { 0x3A, 0x24 },
        { 0x49, 0xCB },
        { 0x58, 0x74 },
        { 0x67, 0x00 },
        { 0x76, 0x08 },
    }
};

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
SOOVPA<7> D3DDevice_SetVertexData4ub_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x07, 0x56 },
        { 0x10, 0x08 },
        { 0x19, 0xC9 },
        { 0x22, 0x54 },
        { 0x2B, 0x14 },
        { 0x34, 0x24 },
        { 0x3D, 0xFC },
    }
};

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
SOOVPA<7> D3DDevice_SetPixelShaderConstant_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x4C },
        { 0x3E, 0x11 },
        { 0x61, 0xDF },
        { 0x7E, 0xF6 },
        { 0x9E, 0x05 },
        { 0xC0, 0x51 },
        { 0xDE, 0xC4 },
    }
};

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
SOOVPA<7> D3DDevice_InsertCallback_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0D, 0x8B },
        { 0x18, 0x8B },
        { 0x25, 0x50 },
        { 0x32, 0x10 },
        { 0x3F, 0x00 },
        { 0x4C, 0x00 },
        { 0x59, 0x03 },
    }
};

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
SOOVPA<7> D3DDevice_BeginPushBuffer_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0xCE },
        { 0x1B, 0x57 },
        { 0x25, 0x00 },
        { 0x32, 0x06 },
        { 0x3F, 0x03 },
        { 0x4C, 0x04 },
        { 0x59, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
SOOVPA<7> D3DDevice_EndPushBuffer_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x11, 0x8D },
        { 0x24, 0x8B },
        { 0x37, 0xFC },
        { 0x4A, 0xFF },
        { 0x5D, 0xF7 },
        { 0x70, 0x03 },
        { 0x83, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
SOOVPA<9> IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_3925 =
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
SOOVPA<9> IDirect3DDevice8_SetRenderState_RopZRead_1_0_3925 =
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
SOOVPA<9> IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_3925 =
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
// * XMETAL_StartPush
// ******************************************************************
SOOVPA<11> XMETAL_StartPush_1_0_3925 = 
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
		// XMETAL_StartPush+0x00 : mov ecx, [esp+0x4]
		{ 0x00, 0x8B },
		{ 0x01, 0x4C },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },

		// XMETAL_StartPush+0x04 : mov eax, [ecx]
		{ 0x04, 0x8B },
		{ 0x05, 0x01 },

		// XMETAL_StartPush+0x06 : cmp eax, [ecx+0x4]
		{ 0x06, 0x3B },
		{ 0x07, 0x41 },
		{ 0x08, 0x04 },

		// XMETAL_StartPush+0x10 : retn 4
		{ 0x10, 0xC2 },
		{ 0x11, 0x04 },
	}
};

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
SOOVPA<11> D3D_SetFence_1_0_3925 =
{
    0,   // Large == 0
    11,  // Count == 11

    XREF_D3D_SETFENCE, // Xref Is Saved
    0,                 // Xref Not Used

    {
		// D3D::SetFence+0x0F : mov edi, [esi+0x1C]
		{ 0x0F, 0x8B },
		{ 0x10, 0x7E },
		{ 0x11, 0x1C },

		// D3D::SetFence+0x3D : and ecx, 0x3F
		{ 0x3D, 0x83 },
		{ 0x3E, 0xE1 },
		{ 0x3F, 0x3F },

		// D3D::SetFence+0x77 : mov [esi*ebp*4], edi
		{ 0x77, 0x89 },
		{ 0x78, 0x3C },
		{ 0x79, 0xAE },

		// D3D::SetFence+0x96 : retn 4
		{ 0x96, 0xC2 },
		{ 0x97, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_InsertFence
// ******************************************************************
SOOVPA<5> D3DDevice_InsertFence_1_0_3925 = 
{
    0,  // Large == 0
    5,  // Count == 5

    XRefNotSaved,
    1,  // Xref Is Used

    {
        { 0x03, XREF_D3D_SETFENCE },

        { 0x00, 0x6A },
        { 0x01, 0x00 },
        { 0x02, 0xE8 },
        { 0x07, 0xC3 },
    }
};

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
SOOVPA<7> D3DDevice_LoadVertexShaderProgram_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0x00 },
        { 0x32, 0xA5 },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0xE8 },
    }
};

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
SOOVPA<6> X_D3DDevice_DeleteVertexShader_1_0_3925 =
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

        // D3DDevice_DeleteVertexShader+0x16 : retn 4
        { 0x16, 0xC2 },
        { 0x17, 0x04 },
    }
};

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
SOOVPA<15> D3DDevice_RunPushBuffer_1_0_3925 = 
{
    0,  // Large == 0
    15, // Count == 15

    XRefNotSaved,
    XRefNotUsed,

    {
        // D3DDevice_RunPushBuffer+0x5E : and ecx, 4
		{ 0x5E, 0x83 },
		{ 0x5F, 0xE1 },
		{ 0x60, 0x04 },

		// D3DDevice_RunPushBuffer+0x95 : push 0x308
		{ 0x95, 0x68 },
		{ 0x96, 0x08 },
		{ 0x97, 0x03 },
		{ 0x98, 0x00 },
		{ 0x99, 0x00 },

		// D3DDevice_RunPushBuffer+0xF1 : mov dword ptr [eax+8], 0x40100
		{ 0xF1, 0xC7 },
		{ 0xF2, 0x40 },
		{ 0xF3, 0x08 },
		{ 0xF4, 0x00 },
		{ 0xF5, 0x01 },
		{ 0xF6, 0x04 },
		{ 0xF7, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
SOOVPA<7> D3DDevice_BlockOnFence_1_0_3925 = 
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
// * Direct3D_EnumAdapterModes
// ******************************************************************
SOOVPA<8> Direct3D_EnumAdapterModes_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x20, 0x8B },
        { 0x3E, 0xE3 },
        { 0x5E, 0x75 },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x0F },
        { 0xE2, 0x89 },
        { 0xFE, 0x5D },
    }
};

// ******************************************************************
// * Direct3D_GetAdapterModeCount
// ******************************************************************
SOOVPA<8> Direct3D_GetAdapterModeCount_1_0_3925 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x14, 0x8B },
        { 0x26, 0xE2 },
        { 0x3A, 0x00 },
        { 0x4E, 0x00 },
        { 0x62, 0x19 },
        { 0x76, 0x75 },
        { 0x8A, 0x3B },
        { 0x9E, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
SOOVPA<7> D3DDevice_Reset_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x15, 0x8B },
        { 0x2C, 0x8B },
        { 0x43, 0x8B },
        { 0x5A, 0x08 },
        { 0x74, 0x8B },
        { 0x88, 0x50 },
        { 0x9F, 0x03 },
    }
};

// ******************************************************************
// * Direct3D_GetAdapterIdentifier
// ******************************************************************
SOOVPA<7> Direct3D_GetAdapterIdentifier_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x85 },
        { 0x0A, 0x08 },
        { 0x10, 0x56 },
        { 0x16, 0xB9 },
        { 0x20, 0xF3 },
        { 0x22, 0x5F },
        { 0x28, 0x00 },
    }
};

// ******************************************************************
// * Direct3D_GetDeviceCaps
// ******************************************************************
SOOVPA<7> Direct3D_GetDeviceCaps_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x85 },
        { 0x0A, 0x08 },
        { 0x10, 0x83 },
        { 0x16, 0x08 },
        { 0x1C, 0xC2 },
        { 0x22, 0x0C },
        { 0x29, 0x33 },
    }
};

// ******************************************************************
// * D3D::SetCommonDebugRegisters
// ******************************************************************
SOOVPA<10> D3D_SetCommonDebugRegisters_1_0_3925 = 
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
		{ 0x07, 0x8B },
		{ 0x08, 0x96 },
		{ 0x09, 0x44 },
		{ 0x0A, 0x2B },
        { 0x31, 0x81 },
        { 0x32, 0xE2 },
        { 0x33, 0xFF },
        { 0x34, 0xFF },
        { 0x35, 0xEF },
        { 0x36, 0xE7 },
    }
};

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
SOOVPA<7> D3DDevice_CreateImageSurface_1_0_3925 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x24 },
        { 0x0E, 0x44 },
        { 0x12, 0x00 },
        { 0x16, 0xE8 },
        { 0x1B, 0xC2 },
    }
};

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
SOOVPA<9> D3D_BlockOnTime_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 6

    XREF_D3D_BLOCKONTIME, // Xref Is Saved
    0,                    // Xref Not Used

    {
        // D3D::BlockOnFence+0x0A : push edi
		{ 0x0A, 0x57 },
		// D3D::BlockOnFence+0x4E : cmp eax, 0x8000
		{ 0x4E, 0x3D },
		{ 0x4F, 0x00 },
		{ 0x50, 0x80 },
		{ 0x51, 0x00 },
		// D3D::BlockOnFence+0x6E : mov ebp, 0x40100
		{ 0x6E, 0xBD },
		{ 0x6F, 0x00 },
		{ 0x70, 0x01 },
		{ 0x71, 0x04 }
    }
};

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
SOOVPA<9> D3D_BlockOnResource_1_0_3925 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_D3D_BlockOnResource, // Xref Is Saved
    XRefNotUsed,

    {
		{ 0x34, 0x8B }, // mov edx, [ecx+0x1C]
		{ 0x35, 0x51 },
		{ 0x36, 0x1C },
		{ 0x41, 0xC2 }, // retn 4
        { 0x42, 0x04 },
        { 0x43, 0x00 },
        { 0x7E, 0xC2 }, // retn 4
        { 0x7F, 0x04 },
        { 0x80, 0x00 },
    }
};

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
SOOVPA<6> D3DDevice_BeginStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x0C },
        { 0x08, 0x20 },
        { 0x09, 0xE9 },
    }
};

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
SOOVPA<6> D3DDevice_EndStateBlock_1_0_3925 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x0C },
        { 0x08, 0xDF },
        { 0x09, 0xE9 },
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
    // IDirect3DDevice8::IsBusy
    {
        (OOVPA*)&IDirect3DDevice8_IsBusy_1_0_3925,

        XTL::EmuIDirect3DDevice8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_IsBusy"
        #endif
    },
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
    // IDirect3DDevice8::GetVisibilityTestResult
    {
        (OOVPA*)&IDirect3DDevice8_GetVisibilityTestResult_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetVisibilityTestResult,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetVisibilityTestResult"
        #endif
    },
    // IDirect3D8::KickOffAndWaitForIdle
    {
        (OOVPA*)&IDirect3D8_KickOffAndWaitForIdle_1_0_3925,

        XTL::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle"
        #endif
    },
    // IDirect3DDevice8::GetMaterial
    {
        (OOVPA*)&D3DDevice_GetMaterial_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetMaterial"
        #endif
    },    
    // IDirect3DDevice8::GetBackMaterial
    {
        (OOVPA*)&D3DDevice_GetBackMaterial_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetBackMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetBackMaterial"
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
    // IDirect3DDevice8::CreatePixelShader
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
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
    // IDirect3DDevice8::SetVertexData2s
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexData2s_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData2s,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData2s"
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
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
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
    // IDirect3DDevice8::SetTransform
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTransform,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTransform"
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
    // IDirect3DDevice8::GetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetRenderTarget,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget"
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
    // IDirect3DResource8::IsBusy
    {
        (OOVPA*)&IDirect3DResource8_IsBusy_1_0_3925,

        XTL::EmuIDirect3DResource8_IsBusy,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_IsBusy"
        #endif
    },
    // IDirect3DSurface8::LockRect
    {
        (OOVPA*)&IDirect3DSurface8_LockRect_1_0_3925,

        XTL::EmuIDirect3DSurface8_LockRect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_LockRect"
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
    // Lock2DSurface
    {
        (OOVPA*)&Lock2DSurface_1_0_3925,

        XTL::EmuLock2DSurface,

        #ifdef _DEBUG_TRACE
        "EmuLock2DSurface"
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
    // D3DDevice_GetVertexShaderSize
    {
        (OOVPA*)&X_D3DDevice_GetVertexShaderSize_1_0_3925,
        XTL::EmuIDirect3DDevice8_GetVertexShaderSize,

        #ifdef _DEBUG_TRACE
        "D3DDevice_GetVertexShaderSize"
        #endif
    },
	// IDirect3DDevice8_SetGammaRamp
	{
		(OOVPA*)&IDirect3DDevice8_SetGammaRamp_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetGammaRamp,

		#ifdef _DEBUG_TRACE
		"IDirect3DDevice8_SetGammaRamp"
		#endif
	},
	// IDirect3DDevice8::SetMaterial
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
	// IDirect3DDevice8::AddRef
    {
        (OOVPA*)&IDirect3DDevice8_AddRef_1_0_3925,

        XTL::EmuIDirect3DDevice8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_AddRef"
        #endif
    },
	// IDirect3DDevice8_Unknown1
	{
		(OOVPA*)&IDirect3DDevice8_Unknown1_1_0_3925,

		XTL::EmuIDirect3DDevice8_Unknown1,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_Unknown1"
		#endif
	},
	// IDirect3DDevice8::GetViewport
    {
        (OOVPA*)&IDirect3DDevice8_GetViewport_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetViewport,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetViewport"
        #endif
    },
	// IDirect3DDevice8::GetGammaRamp
    {
        (OOVPA*)&IDirect3DDevice8_GetGammaRamp_1_0_3925,

        XTL::EmuIDirect3DDevice8_GetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetGammaRamp"
        #endif
    },
	// D3DDevice_GetDisplayFieldStatus
    {
        (OOVPA*)&D3DDevice_GetDisplayFieldStatus_1_0_3925,
		XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayFieldStatus"
        #endif
   },
   // IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
   {
        (OOVPA*)&D3DDevice_SetRenderState_MultiSampleAntiAlias_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias"
        #endif
   },
   // IDirect3DDevice8::SetRenderState_VertexBlend
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_VertexBlend_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_BackFillMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_BackFillMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_BackFillMode"
        #endif
    },
	// IDirect3DDevice8::SetTextureState_TwoSidedLighting
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TwoSidedLighting"
        #endif
    },
	// IDirect3DDevice8_SetRenderState_NormalizeNormals
	{
		(OOVPA*)&IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_3925,

		XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_NormalizeNormals"
		#endif
	},
	// IDirect3DDevice8::SetRenderState_FrontFace
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_FrontFace_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_FrontFace"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_TextureFactor
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_TextureFactor_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_TextureFactor"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_LogicOp
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LogicOp_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LogicOp"
        #endif
    },
	 // IDirect3DDevice8::SetRenderState_EdgeAntiAlias
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleMask
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMask_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMask"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_MultiSampleMode_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_MultiSampleMode"
        #endif
    },
	// D3DDevice_PersistDisplay
    {
        (OOVPA*)&D3DDevice_PersistDisplay_1_0_3925,
        XTL::EmuIDirect3DDevice8_PersistDisplay,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_PersistDisplay"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_ShadowFunc 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_ShadowFunc"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_LineWidth
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LineWidth_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LineWidth"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_YuvEnable
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_YuvEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_YuvEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_StencilCullEnable 
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_StencilCullEnable"
        #endif
    },
	// D3DDevice_DrawVerticesUP
    {
        (OOVPA*)&D3DDevice_DrawVerticesUP_1_0_3925,
        XTL::EmuIDirect3DDevice8_DrawVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVerticesUP"
        #endif
    },
    // D3DDevice_DrawIndexedVerticesUP
    {
        (OOVPA*)&D3DDevice_DrawIndexedVerticesUP_1_0_3925,
        XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVerticesUP"
        #endif
    },
    // D3DDevice_DrawVertices
    {
        (OOVPA*)&D3DDevice_DrawVertices_1_0_3925,
        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
	// D3DDevice_GetDisplayMode
    {
        (OOVPA*)&D3DDevice_GetDisplayMode_1_0_3925,
        XTL::EmuIDirect3DDevice8_GetDisplayMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDisplayMode"
        #endif
    },
	// D3DDevice_SetTextureState_BumpEnv
    {
        (OOVPA*)&D3DDevice_SetTextureState_BumpEnv_1_0_3925,
        XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_BumpEnv"
        #endif
    },
	// D3DDevice_SetTextureState_ColorKeyColor
    {
        (OOVPA*)&D3DDevice_SetTextureState_ColorKeyColor_1_0_3925,
        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
	// D3DDevice_SetVertexData4s
    {
        (OOVPA*)&D3DDevice_SetVertexData4s_1_0_3925,
        XTL::EmuIDirect3DDevice8_SetVertexData4s,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4s"
        #endif
    },
	// IDirect3D8::SetPushBufferSize
    {
        (OOVPA*)&Direct3D_SetPushBufferSize_1_0_3925,
        XTL::EmuIDirect3D8_SetPushBufferSize,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_SetPushBufferSize"
        #endif
    },
	// IDirect3DResource8::GetType
	// Probably not even necessary...
    {
        (OOVPA*)&D3DResource_GetType_1_0_3925,

        XTL::EmuIDirect3DResource8_GetType,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DResource8_GetType"
        #endif
    },
	// IDirect3D8_AllocContiguousMemory
    {
        (OOVPA*)&D3D_AllocContiguousMemory_1_0_3925,
        XTL::EmuIDirect3D8_AllocContiguousMemory,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_AllocContiguousMemory"
        #endif
    },
	// IDirect3DDevice8_SetRenderState_Deferred
	/*{
		(OOVPA*)&IDirect3DDevice8_SetRenderState_Deferred_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetRenderState_Deferred,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderState_Deferred"
		#endif
	},*/
	// D3DDevice_SetLight
    {
        (OOVPA*)&D3DDevice_SetLight_1_0_3925,
        XTL::EmuIDirect3DDevice8_SetLight,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetLight"
        #endif
    },
	// D3DDevice_LightEnable
    {
        (OOVPA*)&D3DDevice_LightEnable_1_0_3925,
        XTL::EmuIDirect3DDevice8_LightEnable,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LightEnable"
        #endif
    },
	// D3DDevice_CreateVertexShader
    {
        (OOVPA*)&D3DDevice_CreateVertexShader_1_0_3925,
        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
	// IDirect3DDevice8::SetRenderTarget
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderTarget_1_0_3948,

        XTL::EmuIDirect3DDevice8_SetRenderTarget,

        #ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetRenderTarget (3948)"
        #endif
    },
	// IDirect3DSurface8::GetDesc
    {
        (OOVPA*)&D3DSurface_GetDesc_1_0_3925,

        XTL::EmuIDirect3DSurface8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DSurface8_GetDesc"
        #endif
    },
	// IDirect3DDevice8_GetProjectionViewportMatrix
	{
		(OOVPA*)&D3DDevice_GetProjectionViewportMatrix_1_0_3925,

		XTL::EmuIDirect3DDevice8_GetProjectionViewportMatrix,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_GetProjectionViewportMatrix"
		#endif
	},
	// D3DDevice_GetTile
    {
        (OOVPA*)&D3DDevice_GetTile_1_0_3925,
        XTL::EmuIDirect3DDevice8_GetTile,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetTile"
        #endif
    },
	// D3DDevice_ApplyStateBlock
    {
        (OOVPA*)&D3DDevice_ApplyStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_ApplyStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_ApplyStateBlock"
        #endif
    },
    // D3DDevice_CaptureStateBlock
    {
        (OOVPA*)&D3DDevice_CaptureStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_CaptureStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CaptureStateBlock"
        #endif
    },
    // D3DDevice_DeleteStateBlock
    {
        (OOVPA*)&D3DDevice_DeleteStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_DeleteStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteStateBlock"
        #endif
    },
    // D3DDevice_CreateStateBlock
    {

        (OOVPA*)&D3DDevice_CreateStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_CreateStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateStateBlock"
        #endif
    },
	// D3DDevice_DeletePixelShader
    {
        (OOVPA*)&D3DDevice_DeletePixelShader_1_0_3925,
        XTL::EmuIDirect3DDevice8_DeletePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeletePixelShader"
        #endif
    },
	// D3DDevice_SetPixelShaderProgram
    {
        (OOVPA*)&D3DDevice_SetPixelShaderProgram_1_0_3925,
        XTL::EmuIDirect3DDevice8_SetPixelShaderProgram,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderProgram"
        #endif
    },
	// D3D::KickOffAndWaitForIdle
    {

        (OOVPA*)&D3D_KickOffAndWaitForIdle_1_0_3925,
	    XTL::EmuIDirect3D8_KickOffAndWaitForIdle,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_KickOffAndWaitForIdle"
        #endif
    },
	// D3DDevice_SetModelView
	{
		(OOVPA*)&D3DDevice_SetModelView_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetModelView,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetModelView"
		#endif
	},
	// D3DDevice_FlushVertexCache
	{
		(OOVPA*)&D3DDevice_FlushVertexCache_1_0_3925,
		XTL::EmuIDirect3DDevice8_FlushVertexCache,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_FlushVertexCache"
		#endif
	},
	// D3DDevice_SetScissors
	{
		(OOVPA*)&D3DDevice_SetScissors_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetScissors,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetScissors"
		#endif
	},
	// D3DDevice_SetVertexShaderInput
	{
		(OOVPA*)&D3DDevice_SetVertexShaderInput_1_0_3925,
		XTL::EmuIDirect3DDevice8_SetVertexShaderInput,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_SetVertexShaderInput"
		#endif
	},
	// D3DDevice_PrimeVertexCache
	{
		(OOVPA*)&D3DDevice_PrimeVertexCache_1_0_3925,
		XTL::EmuIDirect3DDevice8_PrimeVertexCache,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_PrimeVertexCache"
		#endif
	},
	// IDirect3DDevice8::SetVertexData4ub
    {
        (OOVPA*)&D3DDevice_SetVertexData4ub_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetVertexData4ub,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexData4ub"
        #endif
    },
	// IDirect3DDevice8::SetPixelShaderConstant
    {
        (OOVPA*)&D3DDevice_SetPixelShaderConstant_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetPixelShaderConstant,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShaderConstant"
        #endif
    },
	// IDirect3DDevice8_InsertCallback
	{
		(OOVPA*)&D3DDevice_InsertCallback_1_0_3925,

		XTL::EmuIDirect3DDevice8_InsertCallback,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3DDevice8_InsertCallback"
		#endif
	},
	// IDirect3DDevice8::BeginPushBuffer
    {
        (OOVPA*)&D3DDevice_BeginPushBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_BeginPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginPushBuffer"
        #endif
    },
    // IDirect3DDevice8::EndPushBuffer
    {
        (OOVPA*)&D3DDevice_EndPushBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_EndPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndPushBuffer"
        #endif
    },
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_RopZRead
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_RopZRead_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_RopZRead"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_DoNotCullUncompressed
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_3925,

        XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed"
        #endif
    },
	// XMETAL_StartPush
	/*{
		(OOVPA*)&XMETAL_StartPush_1_0_3925,

		XTL::EmuXMETAL_StartPush,

		#ifdef _DEBUG_TRACE
		"EmuXMETAL_StartPush"
		#endif
	}*/
	// D3D::SetFence (XREF)
    {
        (OOVPA*)&D3D_SetFence_1_0_3925, 0,

        #ifdef _DEBUG_TRACE
        "D3D::SetFence (XREF)"
        #endif
    },
	// IDirect3DDevice8::InsertFence
    {
        (OOVPA*)&D3DDevice_InsertFence_1_0_3925,

        XTL::EmuIDirect3DDevice8_InsertFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_InsertFence"
        #endif
    },
	// D3DDevice_LoadVertexShaderProgram
    {
        (OOVPA*)&D3DDevice_LoadVertexShaderProgram_1_0_3925,

        XTL::EmuIDirect3DDevice8_LoadVertexShaderProgram,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShaderProgram"
        #endif
    },
	// D3DDevice_DeleteVertexShader
    {
        (OOVPA*)&X_D3DDevice_DeleteVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_DeleteVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteVertexShader"
        #endif
    },
	// IDirect3DDevice8::RunPushBuffer
    {
        (OOVPA*)&D3DDevice_RunPushBuffer_1_0_3925,

        XTL::EmuIDirect3DDevice8_RunPushBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_RunPushBuffer"
        #endif
    },
	// IDirect3DDevice8::BlockOnFence
    {
        (OOVPA*)&D3DDevice_BlockOnFence_1_0_3925,

        XTL::EmuIDirect3DDevice8_BlockOnFence,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BlockOnFence"
        #endif
    },
	// IDirect3DDevice8::Reset
    {
        (OOVPA*)&D3DDevice_Reset_1_0_3925,

        XTL::EmuIDirect3DDevice8_Reset,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Reset"
        #endif
    },
	// Direct3D_GetAdapterIdentifier
	{
		(OOVPA*)&Direct3D_GetAdapterIdentifier_1_0_3925,
		XTL::EmuIDirect3D8_GetAdapterIdentifier,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3D8_GetAdapterIdentifier"
		#endif
    },
	// Direct3D_GetDeviceCaps
	{
		(OOVPA*)&Direct3D_GetDeviceCaps_1_0_3925,
		XTL::EmuIDirect3D8_GetDeviceCaps,

		#ifdef _DEBUG_TRACE
		"EmuIDirect3D8_GetDeviceCaps"
		#endif
    },
	// D3D::SetCommonDebugRegisters
	{
		(OOVPA*)&D3D_SetCommonDebugRegisters_1_0_3925,
		XTL::EmuD3D_SetCommonDebugRegisters,

		#ifdef _DEBUG_TRACE
		"EmuD3D::SetCommonDebugRegisters"
		#endif
	},
	// D3DDevice_CreateImageSurface
	{
		(OOVPA*)&D3DDevice_CreateImageSurface_1_0_3925,
		XTL::EmuIDirect3DDevice8_CreateImageSurface,

		#ifdef _DEBUG_TRACE
		"D3DDevice_CreateImageSurface"
		#endif
	},
	// D3D::BlockOnTime
	{
		(OOVPA*)&D3D_BlockOnTime_1_0_3925, 
		XTL::EmuD3D_BlockOnTime,

		#ifdef _DEBUG_TRACE
		"D3D::BlockOnTime"
		#endif
	},
	// D3D::BlockOnResource
    {
        (OOVPA*)&D3D_BlockOnResource_1_0_3925, 
		XTL::EmuD3D_BlockOnResource,

        #ifdef _DEBUG_TRACE
        "D3D::BlockOnResource"
        #endif
    },
	// D3DDevice_BeginStateBlock
    {
        (OOVPA*)&D3DDevice_BeginStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_BeginStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_BeginStateBlock"
        #endif
    },
	// D3DDevice_EndStateBlock
    {
        (OOVPA*)&D3DDevice_EndStateBlock_1_0_3925,
        XTL::EmuIDirect3DDevice8_EndStateBlock,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EndStateBlock"
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_3925_SIZE
// ******************************************************************
uint32 D3D8_1_0_3925_SIZE = sizeof(D3D8_1_0_3925);
