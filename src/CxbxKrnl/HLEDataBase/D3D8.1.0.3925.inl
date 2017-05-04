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
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 3925, 8)
	// Direct3D_CreateDevice+0x07 : jnz +0x0A
	{ 0x07, 0x75 }, // (Offset,Value)-Pair #1
	{ 0x08, 0x0A }, // (Offset,Value)-Pair #2

	// Direct3D_CreateDevice+0x79 : mov eax, esi
	{ 0x79, 0x8B }, // (Offset,Value)-Pair #3
	{ 0x7A, 0xC6 }, // (Offset,Value)-Pair #4

	// Direct3D_CreateDevice+0x7C : retn 0x18
	{ 0x7C, 0xC2 }, // (Offset,Value)-Pair #5
	{ 0x7D, 0x18 }, // (Offset,Value)-Pair #6

	// Direct3D_CreateDevice+0x90 : retn 0x18
	{ 0x90, 0xC2 }, // (Offset,Value)-Pair #7
	{ 0x91, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 3925, 8)
	{ 0x0B, 0x8B }, // (Offset,Value)-Pair #1
	{ 0x0C, 0x90 }, // (Offset,Value)-Pair #2
	{ 0x0D, 0x04 }, // (Offset,Value)-Pair #3
	{ 0x0E, 0x04 }, // (Offset,Value)-Pair #4

	{ 0x1E, 0x74 }, // (Offset,Value)-Pair #5
	{ 0x1F, 0x06 }, // (Offset,Value)-Pair #6

	{ 0x25, 0xC3 }, // (Offset,Value)-Pair #7

	{ 0x33, 0xC3 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDeviceCaps
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDeviceCaps, 3925, 11)
	// D3DDevice_GetDeviceCaps+0x00 : push esi; push edi
	{ 0x00, 0x56 }, // (Offset,Value)-Pair #1
	{ 0x01, 0x57 }, // (Offset,Value)-Pair #2

	// D3DDevice_GetDeviceCaps+0x06 : mov ecx, 0x35
	{ 0x06, 0xB9 }, // (Offset,Value)-Pair #3
	{ 0x07, 0x35 }, // (Offset,Value)-Pair #4
	{ 0x08, 0x00 }, // (Offset,Value)-Pair #5
	{ 0x09, 0x00 }, // (Offset,Value)-Pair #6
	{ 0x0A, 0x00 }, // (Offset,Value)-Pair #7

	// D3DDevice_GetDeviceCaps+0x10 : rep movsd
	{ 0x10, 0xF3 }, // (Offset,Value)-Pair #8
	{ 0x11, 0xA5 }, // (Offset,Value)-Pair #9

	// D3DDevice_GetDeviceCaps+0x14 : retn 0x04
	{ 0x14, 0xC2 }, // (Offset,Value)-Pair #10
	{ 0x15, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;


// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 3925, 11)
        // D3DDevice_BeginVisibilityTest+0x0D : mov ecx, 1
        { 0x0D, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x0E, 0x01 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #3

        // D3DDevice_BeginVisibilityTest+0x12 : mov dword ptr [eax], 0x000817C8
        { 0x12, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x13, 0x00 }, // (Offset,Value)-Pair #5
        { 0x14, 0xC8 }, // (Offset,Value)-Pair #6
        { 0x15, 0x17 }, // (Offset,Value)-Pair #7
        { 0x16, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_BeginVisibilityTest+0x1E : add eax, 0x0C
        { 0x1E, 0x83 }, // (Offset,Value)-Pair #9
        { 0x1F, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x20, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;


// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 3925, 9)
        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0C, 0xB0 },
        { 0x0D, 0x18 },
        { 0x0E, 0x0B },
        { 0x11, 0xB9 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 3925, 13)
        // D3DDevice_EndVisibilityTest+0x11 : mov eax, 0x8007000E
        { 0x11, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x12, 0x0E }, // (Offset,Value)-Pair #2
        { 0x13, 0x00 }, // (Offset,Value)-Pair #3
        { 0x14, 0x07 }, // (Offset,Value)-Pair #4
        { 0x15, 0x80 }, // (Offset,Value)-Pair #5

        // D3DDevice_EndVisibilityTest+0x2D : mov dword ptr [eax], 0x000817CC
        { 0x2D, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x2E, 0x00 }, // (Offset,Value)-Pair #7
        { 0x2F, 0xCC }, // (Offset,Value)-Pair #8
        { 0x30, 0x17 }, // (Offset,Value)-Pair #9
        { 0x31, 0x08 }, // (Offset,Value)-Pair #10

        // D3DDevice_EndVisibilityTest+0x43 : add eax, 0x0C
        { 0x43, 0x83 }, // (Offset,Value)-Pair #11
        { 0x44, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x45, 0x0C }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVisibilityTestResult
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVisibilityTestResult, 3925, 11)
        // D3DDevice_GetVisibilityTestResult+0x0C : shr ecx, 8
        { 0x0C, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xE9 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_GetVisibilityTestResult+0x0F : and eax, 0xFF
        { 0x0F, 0x25 }, // (Offset,Value)-Pair #4
        { 0x10, 0xFF }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_GetVisibilityTestResult+0x2B : mov eax, 0x88760828
        { 0x2B, 0xB8 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x28 }, // (Offset,Value)-Pair #8
        { 0x2D, 0x08 }, // (Offset,Value)-Pair #9
        { 0x2E, 0x76 }, // (Offset,Value)-Pair #10
        { 0x2F, 0x88 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 3925, 10)
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
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShader, 3925, 10)
        // D3DDevice_LoadVertexShader+0x07 : mov al, [ebx+0x0C]
        { 0x07, 0x8A }, // (Offset,Value)-Pair #1
        { 0x08, 0x43 }, // (Offset,Value)-Pair #2
        { 0x09, 0x0C }, // (Offset,Value)-Pair #3

        // D3DDevice_LoadVertexShader+0x3D : mov dword ptr [eax], 0x00041E9C
        { 0x3D, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x3E, 0x00 }, // (Offset,Value)-Pair #5
        { 0x3F, 0x9C }, // (Offset,Value)-Pair #6
        { 0x40, 0x1E }, // (Offset,Value)-Pair #7
        { 0x41, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_LoadVertexShader+0x4E : mov [ebx], ebx
        { 0x4E, 0x89 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x13 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 3925, 11)
        // D3DDevice_SelectVertexShader+0x16 : or ecx, 0x3A0
        { 0x16, 0x81 }, // (Offset,Value)-Pair #1
        { 0x17, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x18, 0xA0 }, // (Offset,Value)-Pair #3
        { 0x19, 0x03 }, // (Offset,Value)-Pair #4

        // D3DDevice_SelectVertexShader+0x3F : mov dword ptr [eax], 0x00081E94
        { 0x3F, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x40, 0x00 }, // (Offset,Value)-Pair #6
        { 0x41, 0x94 }, // (Offset,Value)-Pair #7
        { 0x42, 0x1E }, // (Offset,Value)-Pair #8
        { 0x43, 0x08 }, // (Offset,Value)-Pair #9

        // D3DDevice_SelectVertexShader+0x51 : mov [esi], eax
        { 0x51, 0x89 }, // (Offset,Value)-Pair #10
        { 0x52, 0x06 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 3925, 14)
        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0;push 0;push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x0B : mov dword ptr [eax+0x24F4],
        { 0x0B, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x0C, 0x80 }, // (Offset,Value)-Pair #8
        { 0x0D, 0xF4 }, // (Offset,Value)-Pair #9
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #10
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #11

        // D3DDevice_BlockUntilVerticalBlank+0x17 : and eax, 0x24F0
        { 0x17, 0x05 }, // (Offset,Value)-Pair #12
        { 0x18, 0xF0 }, // (Offset,Value)-Pair #13
        { 0x19, 0x24 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 3925, 12)
        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x24EC], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #6
        { 0x0C, 0xEC }, // (Offset,Value)-Pair #7
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x11, 0x04 }, // (Offset,Value)-Pair #11
        { 0x12, 0x00 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 3925, 11)
        // D3DDevice_SetRenderTarget+0x00 : sub esp, 0x20
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x20 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderTarget+0x32 : mov [esp+0x34], ebx
        { 0x32, 0x89 }, // (Offset,Value)-Pair #4
        { 0x33, 0x5C }, // (Offset,Value)-Pair #5
        { 0x34, 0x24 }, // (Offset,Value)-Pair #6
        { 0x35, 0x34 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderTarget+0x55 : and ecx, 0x0F
        { 0x55, 0x83 }, // (Offset,Value)-Pair #8
        { 0x56, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x57, 0x0F }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderTarget+0xDC : push edx
        { 0xDC, 0x52 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 3948, 8)
        // D3DDevice_SetRenderTarget+0x00 : sub esp, 0x30
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x30 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderTarget+0x32 : and ecx, 0x0F
        { 0x32, 0x83 }, // (Offset,Value)-Pair #4
        { 0x33, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x34, 0x0F }, // (Offset,Value)-Pair #6

		// D3DDevice_SetRenderTarget+0x69 : push ebx
        { 0x69, 0x53 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderTarget+0xDA : push edx
        { 0xDA, 0x52 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DSurface_GetDesc
// ******************************************************************
OOVPA_NO_XREF(D3DSurface_GetDesc, 3925, 7)
        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x04 },
        { 0x0A, 0x00 },
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTransform, 3925, 12)

        // D3DDevice_GetTransform+0x06 : mov esi, [esp+4+arg_0]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #1
        { 0x07, 0x74 }, // (Offset,Value)-Pair #2
        { 0x08, 0x24 }, // (Offset,Value)-Pair #3

        // D3DDevice_GetTransform+0x12 : shl esi, 6; add esi, eax
        { 0x12, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x13, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x14, 0x06 }, // (Offset,Value)-Pair #6
        { 0x15, 0x03 }, // (Offset,Value)-Pair #7
        { 0x16, 0xF0 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetTransform+0x17 : mov ecx, 0x10
        { 0x17, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x18, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetTransform+0x20 : retn 0x08
        { 0x20, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x21, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStreamSource
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStreamSource, 3925, 12)
        // D3DDevice_SetStreamSource+0x39 : add edx, 0xFFF80000
        { 0x39, 0x81 }, // (Offset,Value)-Pair #1
        { 0x3A, 0xC2 }, // (Offset,Value)-Pair #2
        { 0x3B, 0x00 }, // (Offset,Value)-Pair #3
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #4
        { 0x3D, 0xF8 }, // (Offset,Value)-Pair #5
        { 0x3E, 0xFF }, // (Offset,Value)-Pair #6

        // D3DDevice_SetStreamSource+0x49 : jnz +0x06
        { 0x49, 0x75 }, // (Offset,Value)-Pair #7
        { 0x4A, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetStreamSource+0x6A : or ecx, 0x280
        { 0x6A, 0x81 }, // (Offset,Value)-Pair #9
        { 0x6B, 0xC9 }, // (Offset,Value)-Pair #10
        { 0x6C, 0x80 }, // (Offset,Value)-Pair #11
        { 0x6D, 0x02 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 3925, 14)
        // D3DDevice_SetVertexShader+0x05 : test bl, 1
        { 0x05, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x07, 0x01 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShader+0x10 : mov eax, [esi+0x0470]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #4
        { 0x11, 0x86 }, // (Offset,Value)-Pair #5
        { 0x12, 0x70 }, // (Offset,Value)-Pair #6
        { 0x13, 0x04 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetVertexShader+0x6D : retn 0x04
        { 0x6D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x6E, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShader+0x8C : mov dword ptr [eax], 0x81E94
        { 0x8C, 0xC7 }, // (Offset,Value)-Pair #10
        { 0x8D, 0x40 }, // (Offset,Value)-Pair #11
        { 0x8E, 0x08 }, // (Offset,Value)-Pair #12
        { 0x8F, 0x94 }, // (Offset,Value)-Pair #13
        { 0x90, 0x1E }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePixelShader, 3925, 11)
        // D3DDevice_CreatePixelShader+0x00 : push 0xFC
        { 0x00, 0x68 }, // (Offset,Value)-Pair #1
        { 0x01, 0xFC }, // (Offset,Value)-Pair #2

        // D3DDevice_CreatePixelShader+0x10 : mov eax, 0x8007000E
        { 0x10, 0xB8 }, // (Offset,Value)-Pair #3
        { 0x11, 0x0E }, // (Offset,Value)-Pair #4
        { 0x12, 0x00 }, // (Offset,Value)-Pair #5
        { 0x13, 0x07 }, // (Offset,Value)-Pair #6
        { 0x14, 0x80 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreatePixelShader+0x31 : mov ecx, 0x3C
        { 0x31, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x32, 0x3C }, // (Offset,Value)-Pair #9

        // D3DDevice_CreatePixelShader+0x42 : retn 0x08
        { 0x42, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x43, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 3925, 10)
        // D3DDevice_SetPixelShader+0x0E : mov esi, [ebx+0x0414]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xB3 }, // (Offset,Value)-Pair #2
        { 0x10, 0x14 }, // (Offset,Value)-Pair #3
        { 0x11, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetPixelShader+0x34 : or eax, 0x40
        { 0x35, 0xC8 }, // (Offset,Value)-Pair #5
        { 0x36, 0x40 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetPixelShader+0x68 : retn 0x04
        { 0x68, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x69, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xE4, 0xA5 }, // (Offset,Value)-Pair #10

OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 3925, 11)
        // D3DDevice_SetIndices+0x0E : jz +0x10
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #1
        { 0x0F, 0x10 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetIndices+0x3E : test ecx, 0x78FFFF
        { 0x3E, 0xF7 }, // (Offset,Value)-Pair #3
        { 0x3F, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x40, 0xFF }, // (Offset,Value)-Pair #5
        { 0x41, 0xFF }, // (Offset,Value)-Pair #6
        { 0x42, 0x78 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetIndices+0x67 : mov [esi+0x047C], edi
        { 0x67, 0x89 }, // (Offset,Value)-Pair #8
        { 0x68, 0xBE }, // (Offset,Value)-Pair #9
        { 0x69, 0x7C }, // (Offset,Value)-Pair #10
        { 0x6A, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 3925, 10)
        // D3DDevice_SetViewport+0x00 : sub esp, 0x08
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetViewport+0x2F : mov ebx, [esp+0x1C]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x5C }, // (Offset,Value)-Pair #5
        { 0x31, 0x24 }, // (Offset,Value)-Pair #6
        { 0x32, 0x1C }, // (Offset,Value)-Pair #7

        // D3DDevice_SetViewport+0x55 : jnz +0x12
        { 0x55, 0x75 }, // (Offset,Value)-Pair #8
        { 0x56, 0x12 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetViewport+0x9D : inc edx
        { 0x9D, 0x42 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture, 3925, 12)
        // D3DDevice_SetTexture+0x0E : sub esp, 0x08
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x84 }, // (Offset,Value)-Pair #2
        { 0x10, 0x9F }, // (Offset,Value)-Pair #3
        { 0x11, 0xA0 }, // (Offset,Value)-Pair #4
        { 0x12, 0x0B }, // (Offset,Value)-Pair #5
        { 0x13, 0x00 }, // (Offset,Value)-Pair #6
        { 0x14, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetTexture+0x57 : shl edx, 6
        { 0x57, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x58, 0xE2 }, // (Offset,Value)-Pair #9
        { 0x59, 0x06 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetTexture+0x7D : retn 0x08
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x7E, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 3925, 12)
        // D3DDevice_DrawIndexedVertices+0x0E : mov eax, [esi+0x0478]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x86 }, // (Offset,Value)-Pair #2
        { 0x10, 0x78 }, // (Offset,Value)-Pair #3
        { 0x11, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_DrawIndexedVertices+0x44 : mov dword ptr [ebx-8], 0x000417FC
        { 0x44, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x45, 0x43 }, // (Offset,Value)-Pair #6
        { 0x46, 0xF8 }, // (Offset,Value)-Pair #7
        { 0x47, 0xFC }, // (Offset,Value)-Pair #8
        { 0x48, 0x17 }, // (Offset,Value)-Pair #9
        { 0x49, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_DrawIndexedVertices+0xDA : lea ebx, [ebx+0]
        { 0xDA, 0x8D }, // (Offset,Value)-Pair #11
        { 0xDB, 0x9B }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 3925, 11)
        // D3DDevice_Begin+0x20 : mov dword ptr [eax], 0x417FC
        { 0x20, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x21, 0x00 }, // (Offset,Value)-Pair #2
        { 0x22, 0xFC }, // (Offset,Value)-Pair #3
        { 0x23, 0x17 }, // (Offset,Value)-Pair #4
        { 0x24, 0x04 }, // (Offset,Value)-Pair #5
        { 0x25, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_Begin+0x2E : shl edx, 6
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #7
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #8
        { 0x30, 0x08 }, // (Offset,Value)-Pair #9

        // D3DDevice_Begin+0x38 : retn 0x08
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x39, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 3925, 14)
        // D3DDevice_SetVertexData2f+0x11 : lea edx, ds:abs[ecx*8]
        { 0x11, 0x8D }, // (Offset,Value)-Pair #1
        { 0x12, 0x14 }, // (Offset,Value)-Pair #2
        { 0x13, 0xCD }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexData2f+0x1C : mov [eax], edx; mov edx, [esp+0x10]
        { 0x1C, 0x89 }, // (Offset,Value)-Pair #4
        { 0x1D, 0x10 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x8B }, // (Offset,Value)-Pair #6
        { 0x1F, 0x54 }, // (Offset,Value)-Pair #7
        { 0x20, 0x24 }, // (Offset,Value)-Pair #8
        { 0x21, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData2f+0x28 : add eax, 0x0C
        { 0x28, 0x83 }, // (Offset,Value)-Pair #10
        { 0x29, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x2A, 0x0C }, // (Offset,Value)-Pair #12

        // D3DDevice_SetVertexData2f+0x2E : retn 0x0C
        { 0x2E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x2F, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 3925, 11)
        // D3DDevice_SetVertexData2s+0x11 : lea edx, ds:abs[ecx*4]
        { 0x11, 0x8D }, // (Offset,Value)-Pair #1
        { 0x12, 0x14 }, // (Offset,Value)-Pair #2
        { 0x13, 0x8D }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexData2s+0x24 : shl ecx, 0x10
        { 0x24, 0xC1 }, // (Offset,Value)-Pair #4
        { 0x25, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x26, 0x10 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVertexData2s+0x29 : mov [eax+4], ecx
        { 0x29, 0x89 }, // (Offset,Value)-Pair #7
        { 0x2A, 0x48 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData2s+0x32 : retn 0x0C
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x33, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 3925, 11)
        // D3DDevice_SetVertexData4f+0x11 : cmp ecx, -1; jnz +0x07
        { 0x11, 0x83 }, // (Offset,Value)-Pair #1
        { 0x12, 0xF9 }, // (Offset,Value)-Pair #2
        { 0x13, 0xFF }, // (Offset,Value)-Pair #3
        { 0x14, 0x75 }, // (Offset,Value)-Pair #4
        { 0x15, 0x07 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexData4f+0x1D : add ecx, 0x01A0
        { 0x1D, 0x81 }, // (Offset,Value)-Pair #6
        { 0x1E, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x1F, 0xA0 }, // (Offset,Value)-Pair #8
        { 0x20, 0x01 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData4f+0x50 : retn 0x14
        { 0x50, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x51, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexDataColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexDataColor, 3925, 17)
        // D3DDevice_SetVertexDataColor+0x1F : mov byte ptr [esp+?]
        { 0x1F, 0x0F }, // (Offset,Value)-Pair #1
        { 0x20, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x21, 0x54 }, // (Offset,Value)-Pair #3
        { 0x22, 0x24 }, // (Offset,Value)-Pair #4
        { 0x23, 0x12 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexDataColor+0x26 : add edi, 0xFF; shl edi, 0x10
        { 0x26, 0x81 }, // (Offset,Value)-Pair #6
        { 0x27, 0xE7 }, // (Offset,Value)-Pair #7
        { 0x28, 0xFF }, // (Offset,Value)-Pair #8
        { 0x2C, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x2D, 0xE7 }, // (Offset,Value)-Pair #10
        { 0x2E, 0x10 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetVertexDataColor+0x31 : and ecx, 0xFF00FF00
        { 0x31, 0x81 }, // (Offset,Value)-Pair #12
        { 0x32, 0xE1 }, // (Offset,Value)-Pair #13
        { 0x33, 0x00 }, // (Offset,Value)-Pair #14
        { 0x34, 0xFF }, // (Offset,Value)-Pair #15
        { 0x35, 0x00 }, // (Offset,Value)-Pair #16
        { 0x36, 0xFF }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 3925, 12)
        // D3DDevice_End+0x17 : mov dword ptr [eax], 0x417FC
        { 0x17, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x18, 0x00 }, // (Offset,Value)-Pair #2
        { 0x19, 0xFC }, // (Offset,Value)-Pair #3
        { 0x1A, 0x17 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_End+0x2C : test ah, 0x10
        { 0x2C, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x2D, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_End+0x2F : jz +0x07
        { 0x2F, 0x74 }, // (Offset,Value)-Pair #10
        { 0x30, 0x07 }, // (Offset,Value)-Pair #11

        // D3DDevice_End+0x40 : retn
        { 0x40, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 3925, 10)
        // D3DDevice_Clear+0x00 : sub esp, 0x38
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x38 }, // (Offset,Value)-Pair #3

        // D3DDevice_Clear+0x25 : test cl, 1
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x27, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_Clear+0x5D : mov [esi], eax; xor ebp, ebp
        { 0x5D, 0x89 }, // (Offset,Value)-Pair #7
        { 0x5E, 0x06 }, // (Offset,Value)-Pair #8
        { 0x5F, 0x33 }, // (Offset,Value)-Pair #9
        { 0x60, 0xED }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePalette, 3925, 12)
        // D3DDevice_CreatePalette+0x01 : push 0x0C
        { 0x01, 0x6A }, // (Offset,Value)-Pair #1
        { 0x02, 0x0C }, // (Offset,Value)-Pair #2

        // D3DDevice_CreatePalette+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #3
        { 0x04, 0x40 }, // (Offset,Value)-Pair #4

        // D3DDevice_CreatePalette+0x10 : mov eax, 0x8007000E
        { 0x10, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x11, 0x0E }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7
        { 0x13, 0x07 }, // (Offset,Value)-Pair #8
        { 0x14, 0x80 }, // (Offset,Value)-Pair #9

        // D3DDevice_CreatePalette+0x4E : shl esi, 0x1E
        { 0x4E, 0xC1 }, // (Offset,Value)-Pair #10
        { 0x4F, 0xE6 }, // (Offset,Value)-Pair #11
        { 0x50, 0x1E }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 3925, 12)
        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0BB0]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xB7 }, // (Offset,Value)-Pair #3
        { 0x10, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x11, 0x0B }, // (Offset,Value)-Pair #5

        // D3DDevice_SetPalette+0x68 : add esi, 0x41B20
        { 0x68, 0x81 }, // (Offset,Value)-Pair #6
        { 0x69, 0xC6 }, // (Offset,Value)-Pair #7
        { 0x6A, 0x20 }, // (Offset,Value)-Pair #8
        { 0x6B, 0x1B }, // (Offset,Value)-Pair #9
        { 0x6C, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetPalette+0x7C : retn 0x08
        { 0x7C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x7D, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture, 3925, 14)
        // D3DDevice_CreateTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x1C }, // (Offset,Value)-Pair #4

        // D3DDevice_CreateTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateTexture+0x11 : push 0
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateTexture+0x13 : push 0
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_CreateTexture+0x15 : push ecx
        { 0x15, 0x51 }, // (Offset,Value)-Pair #10

        // D3DDevice_CreateTexture+0x22 : push ecx
        { 0x22, 0x51 }, // (Offset,Value)-Pair #11

        // D3DDevice_CreateTexture+0x23 : push edx
        { 0x23, 0x52 }, // (Offset,Value)-Pair #12

        // D3DDevice_CreateTexture+0x29 : retn 0x1C
        { 0x29, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x2A, 0x1C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVolumeTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVolumeTexture, 3925, 15)
        // D3DDevice_CreateVolumeTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_CreateVolumeTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateVolumeTexture+0x11 : push 1
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateVolumeTexture+0x13 : push 0
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_CreateVolumeTexture+0x16 : mov ecx, [esp+0x1C]
        { 0x16, 0x8B }, // (Offset,Value)-Pair #10
        { 0x17, 0x4C }, // (Offset,Value)-Pair #11
        { 0x18, 0x24 }, // (Offset,Value)-Pair #12
        { 0x19, 0x1C }, // (Offset,Value)-Pair #13

        // D3DDevice_CreateVolumeTexture+0x2C : retn 0x20
        { 0x2C, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x2D, 0x20 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateCubeTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateCubeTexture, 3925, 15)
        // D3DDevice_CreateCubeTexture+0x00 : mov eax, [esp+0x1C]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x18 }, // (Offset,Value)-Pair #4

        // D3DDevice_CreateCubeTexture+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateCubeTexture+0x11 : push 0
        { 0x11, 0x6A }, // (Offset,Value)-Pair #6
        { 0x12, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateCubeTexture+0x13 : push 1
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x01 }, // (Offset,Value)-Pair #9

        // D3DDevice_CreateCubeTexture+0x18 : mov eax, [esp+0x1C]
        { 0x18, 0x8B }, // (Offset,Value)-Pair #10
        { 0x19, 0x44 }, // (Offset,Value)-Pair #11
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #12
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #13

        // D3DDevice_CreateCubeTexture+0x25 : retn 0x18
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x26, 0x18 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateIndexBuffer, 3925, 13)
        // D3DDevice_CreateIndexBuffer+0x13 : mov eax, 0x8007000E
        { 0x13, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x14, 0x0E }, // (Offset,Value)-Pair #2
        { 0x15, 0x00 }, // (Offset,Value)-Pair #3
        { 0x16, 0x07 }, // (Offset,Value)-Pair #4
        { 0x17, 0x80 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateIndexBuffer+0x18 : retn 0x14
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x19, 0x14 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateIndexBuffer+0x2E : mov dword ptr [eax], 0x1010001
        { 0x2E, 0xC7 }, // (Offset,Value)-Pair #8
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #9
        { 0x30, 0x01 }, // (Offset,Value)-Pair #10
        { 0x31, 0x00 }, // (Offset,Value)-Pair #11
        { 0x32, 0x01 }, // (Offset,Value)-Pair #12
        { 0x33, 0x01 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexShader
// ******************************************************************
// Based on Dxbx patterns for 3911, 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933
OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 3925, 38)
		// IDirect3DDevice8_CreateVertexShader+0x00 : push ecx; push ebx; push ebp
		{ 0x00, 0x51 },
		{ 0x01, 0x53 },
		{ 0x02, 0x55 },

		{ 0x03, 0x8B },
		{ 0x04, 0x6C },
		{ 0x05, 0x24 },
		{ 0x06, 0x14 },
		{ 0x07, 0x85 },
		{ 0x08, 0xED },
		{ 0x09, 0x56 },

		// IDirect3DDevice8_CreateVertexShader+0x0A : jz +0x10
		{ 0x0A, 0x74 },
		{ 0x0B, 0x10 },

		{ 0x0C, 0x0F },
		{ 0x0D, 0xB7 },
		{ 0x0E, 0x45 },
		{ 0x0F, 0x00 },
		{ 0x10, 0x0F },
		{ 0x11, 0xB7 },
		{ 0x12, 0x4D },
		{ 0x13, 0x02 },
		{ 0x14, 0x89 },
		{ 0x15, 0x44 },
		{ 0x16, 0x24 },
		{ 0x17, 0x0C },
		{ 0x18, 0x89 },
		{ 0x19, 0x4C },
		{ 0x1A, 0x24 },
		{ 0x1B, 0x18 },
		{ 0x1C, 0x33 },
		{ 0x1D, 0xF6 },
		{ 0x1E, 0x85 },
		{ 0x1F, 0xED },

		// D3DDevice_CreateVertexShader+0x43 : shl eax, 0x02
		{ 0x43, 0xC1 },
		{ 0x44, 0xE0 },
		{ 0x45, 0x02 },

		{ 0x46, 0x8D },
		{ 0x47, 0x1C },
		{ 0x48, 0x30 }, 
/*
	OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5558)

		// D3DDevice_CreateVertexShader+0x73 : mov eax, 0x8007000E
		{ 0x65, 0x07 }, // (Offset,Value)-Pair #11
		{ 0x66, 0x80 }, // (Offset,Value)-Pair #12

		// D3DDevice_CreateVertexShader+0x69 : retn 0x10
		{ 0x69, 0xC2 }, // (Offset,Value)-Pair #9
		{ 0x6A, 0x10 }, // (Offset,Value)-Pair #10
*/
/*
	OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5788)

		{ 0x3E, 0xE8 },
		{ 0x5E, 0x75 },
		{ 0x7E, 0x04 },
		{ 0x9E, 0x24 },
		{ 0xBE, 0x24 },
		{ 0xDE, 0x83 },
		{ 0xFE, 0xC7 },
*/
/*
	OOVPA_NO_XREF(D3DDevice_CreateVertexShader, 5849)

		{ 0x3E, 0xE8 },
		{ 0x5E, 0x75 },
		{ 0x7E, 0x04 },
		{ 0x9E, 0x24 },
		{ 0xBE, 0x24 },
		{ 0xDE, 0x83 },
		{ 0xFE, 0xC7 },
*/
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant, 3925, 10)
        // D3DDevice_SetVertexShaderConstant+0x11 : mov cl, [ebx+0x0C]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x4B }, // (Offset,Value)-Pair #2
        { 0x13, 0x0C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant+0x1A : test cl, 0x10
        { 0x1A, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x1B, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x10 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVertexShaderConstant+0x63 : jle +0x19
        { 0x63, 0x7E }, // (Offset,Value)-Pair #7
        { 0x64, 0x19 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant+0x9B : retn 0x0C
        { 0x9B, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x9C, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 3925, 12)
        // D3DDevice_SetFlickerFilter+0x0A : mov edx, [ecx+0x2308]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetFlickerFilter+0x10 : push 0; push eax; push 0x0B; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0B }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetFlickerFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 3925, 12)
        // D3DDevice_SetSoftDisplayFilter+0x0A : mov edx, [ecx+0x2308]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x91 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetSoftDisplayFilter+0x10 : push 0; push eax; push 0x0E; push edx
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x50 }, // (Offset,Value)-Pair #7
        { 0x13, 0x6A }, // (Offset,Value)-Pair #8
        { 0x14, 0x0E }, // (Offset,Value)-Pair #9
        { 0x15, 0x52 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetSoftDisplayFilter+0x1C : retn 0x08
        { 0x1C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 3925, 1+11,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x11, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x0B : shl eax, 0x07
        { 0x0B, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x0C, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x07 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_TexCoordIndex+0x24 : cmp eax, 0x20000
        { 0x24, 0x81 }, // (Offset,Value)-Pair #4
        { 0x25, 0xF9 }, // (Offset,Value)-Pair #5
        { 0x28, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTextureState_TexCoordIndex+0x3B : mov edi, 0x8511
        { 0x3B, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3C, 0x11 }, // (Offset,Value)-Pair #8
        { 0x3D, 0x85 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetTextureState_TexCoordIndex+0x97 : shl eax, cl
        { 0x97, 0xD3 }, // (Offset,Value)-Pair #10
        { 0x98, 0xE0 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BorderColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BorderColor, 3925, 13)
        // D3DDevice_SetTextureState_BorderColor+0x13 : shl eax, 0x07
        { 0x13, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x14, 0xE2 }, // (Offset,Value)-Pair #2
        { 0x15, 0x06 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_BorderColor+0x16 : add edx, 0x41B24
        { 0x16, 0x81 }, // (Offset,Value)-Pair #4
        { 0x17, 0xC2 }, // (Offset,Value)-Pair #5
        { 0x18, 0x24 }, // (Offset,Value)-Pair #6
        { 0x19, 0x1B }, // (Offset,Value)-Pair #7
        { 0x1A, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTextureState_BorderColor+0x2A : shl ecx, 7
        { 0x2A, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x2B, 0xE1 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x07 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetTextureState_BorderColor+0x34 : retn 0x08
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x35, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_PSTextureModes, 3925, 11)
        // D3DDevice_SetRenderState_PSTextureModes+0x0C : or edx, 0x400
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xCA }, // (Offset,Value)-Pair #2
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_PSTextureModes+0x12 : mov [eax+0x420], ecx
        { 0x12, 0x89 }, // (Offset,Value)-Pair #5
        { 0x13, 0x88 }, // (Offset,Value)-Pair #6
        { 0x14, 0x20 }, // (Offset,Value)-Pair #7
        { 0x15, 0x04 }, // (Offset,Value)-Pair #8
        { 0x16, 0x00 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_PSTextureModes+0x21 : retn 4
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 3925, 12)
        // D3DDevice_SetRenderState_StencilFail+0x19 : mov ecx, 2
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #1
        { 0x1A, 0x02 }, // (Offset,Value)-Pair #2
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_StencilFail+0x3E : or ecx, 1
        { 0x3E, 0x83 }, // (Offset,Value)-Pair #4
        { 0x3F, 0xC9 }, // (Offset,Value)-Pair #5
        { 0x40, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_StencilFail+0x48 : mov dword ptr [eax], 0x00041D84
        { 0x48, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x49, 0x00 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x84 }, // (Offset,Value)-Pair #9
        { 0x4B, 0x1D }, // (Offset,Value)-Pair #10
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #11
        { 0x4D, 0x00 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_CullMode
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_CullMode, 3925, 2+15,

	XRefNoSaveIndex,
	XRefTwo)

		XREF_ENTRY(0x03, XREF_D3DDEVICE), // Derived
		XREF_ENTRY(0x25, XREF_D3DRS_CULLMODE), // Derived

		// D3DDevice_SetRenderState_CullMode+0x42 : mov dword ptr [eax+8], 0x4039C
        { 0x42, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x43, 0x40 }, // (Offset,Value)-Pair #2
        { 0x44, 0x08 }, // (Offset,Value)-Pair #3
        { 0x45, 0x9C }, // (Offset,Value)-Pair #4
        { 0x46, 0x03 }, // (Offset,Value)-Pair #5
        { 0x47, 0x04 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_CullMode+0x49 : add eax, 0x10
        { 0x49, 0x83 }, // (Offset,Value)-Pair #7
        { 0x4A, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x4B, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_CullMode+0x5F : retn 4
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x60, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_CullMode+0x4D : add edx, 0x404
        { 0x4D, 0x81 }, // (Offset,Value)-Pair #12
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #14
        { 0x50, 0x04 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Simple
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Simple, 3925, 13)
        // D3DDevice_SetRenderState_Simple+0x05 : add eax, 0x08
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x08 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_Simple+0x0E : jnb +0x0C
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_Simple+0x15 : mov [eax-8], ecx
        { 0x15, 0x89 }, // (Offset,Value)-Pair #6
        { 0x16, 0x48 }, // (Offset,Value)-Pair #7
        { 0x17, 0xF8 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_Simple+0x18 : mov [eax-4], ecx
        { 0x18, 0x89 }, // (Offset,Value)-Pair #9
        { 0x19, 0x50 }, // (Offset,Value)-Pair #10
        { 0x1A, 0xFC }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_Simple+0x2B : jmp +0xD3
        { 0x2B, 0xEB }, // (Offset,Value)-Pair #12
        { 0x2C, 0xD3 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 3925, 10)
        // D3DDevice_SetTransform+0x3A : fld dword ptr [edx+0x28]
        { 0x3A, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x3B, 0x42 }, // (Offset,Value)-Pair #2
        { 0x3C, 0x28 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTransform+0x47 : test ah, 0x44
        { 0x47, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x48, 0xC4 }, // (Offset,Value)-Pair #5
        { 0x49, 0x44 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTransform+0x55 : fnstsw ax
        { 0x55, 0xDF }, // (Offset,Value)-Pair #7
        { 0x56, 0xE0 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTransform+0x7A : fdivp st(1), st
        { 0x7A, 0xDE }, // (Offset,Value)-Pair #9
        { 0x7B, 0xF9 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 3925, 11)
        // D3DDevice_SetRenderState_FogColor+0x2E : mov dword ptr [eax], 0x402A8
        { 0x2E, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #2
        { 0x30, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x31, 0x02 }, // (Offset,Value)-Pair #4
        { 0x32, 0x04 }, // (Offset,Value)-Pair #5
        { 0x33, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_FogColor+0x37 : add eax, 0x08
        { 0x37, 0x83 }, // (Offset,Value)-Pair #7
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x39, 0x08 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_FogColor+0x44 : retn 0x04
        { 0x44, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x45, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 3925, 11)
        // D3DDevice_SetRenderState_FillMode+0x23 : mov dword ptr [eax], 0x8038C
        { 0x23, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x24, 0x00 }, // (Offset,Value)-Pair #2
        { 0x25, 0x8C }, // (Offset,Value)-Pair #3
        { 0x26, 0x03 }, // (Offset,Value)-Pair #4
        { 0x27, 0x08 }, // (Offset,Value)-Pair #5
        { 0x28, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_FillMode+0x2F : add eax, 0x0C
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x31, 0x0C }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_FillMode+0x3B : retn 0x04
        { 0x3B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 3925, 11)
        // D3DDevice_SetRenderState_StencilEnable+0x45 : mov dword ptr [eax], 0x41D84
        { 0x45, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x46, 0x00 }, // (Offset,Value)-Pair #2
        { 0x47, 0x84 }, // (Offset,Value)-Pair #3
        { 0x48, 0x1D }, // (Offset,Value)-Pair #4
        { 0x49, 0x04 }, // (Offset,Value)-Pair #5
        { 0x4A, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_StencilEnable+0x71 : add eax, 0x08
        { 0x71, 0x83 }, // (Offset,Value)-Pair #7
        { 0x72, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x73, 0x08 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_StencilEnable+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 3925, 11)
        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x0C : mov eax, [esi+0x040C]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0D, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #3
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x22 : cmp dl, 0x20
        { 0x22, 0x80 }, // (Offset,Value)-Pair #7
        { 0x23, 0xFA }, // (Offset,Value)-Pair #8
        { 0x24, 0x20 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x74 : retn 0x04
        { 0x74, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x75, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZBias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZBias, 3925, 10)
        // D3DDevice_SetRenderState_ZBias+0x0B : setnz al
        { 0x0B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0C, 0x95 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_ZBias+0x14 : fild [esp+0x10]
        { 0x14, 0xDB }, // (Offset,Value)-Pair #4
        { 0x15, 0x44 }, // (Offset,Value)-Pair #5
        { 0x16, 0x24 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_ZBias+0x45 : push 0x4E
        { 0x45, 0x6A }, // (Offset,Value)-Pair #7
        { 0x46, 0x4E }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_ZBias+0x45 : retn 0x04
        { 0x71, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x72, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 3925, 13)
        // D3DDevice_SetRenderState_ZEnable+0x13 : jz +0x11
        { 0x13, 0x74 }, // (Offset,Value)-Pair #1
        { 0x14, 0x11 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_ZEnable+0x15 : mov edx, [esi+0x0410]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #3
        { 0x16, 0x96 }, // (Offset,Value)-Pair #4
        { 0x17, 0x10 }, // (Offset,Value)-Pair #5
        { 0x18, 0x04 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_ZEnable+0x28 : mov dword ptr [eax], 0x4030
        { 0x28, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x29, 0x00 }, // (Offset,Value)-Pair #8
        { 0x2A, 0x0C }, // (Offset,Value)-Pair #9
        { 0x2B, 0x03 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_ZEnable+0x69 : retn 0x04
        { 0x69, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x6A, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 3925, 11)
        // D3DDevice_Release+0x07 : mov eax, [edi+0x0408]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x87 }, // (Offset,Value)-Pair #2
        { 0x09, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_Release+0x0D : cmp eax, 1
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0xF8 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_Release+0x1B : mov ecx, 0x0AE4
        { 0x1B, 0xB9 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xE4 }, // (Offset,Value)-Pair #9
        { 0x1D, 0x0A }, // (Offset,Value)-Pair #10

        // D3DDevice_Release+0x2E : dec eax
        { 0x2E, 0x48 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_Present
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Present, 3925, 10)
        // D3DDevice_Present+0x0A : push 2
        { 0x0A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x0B, 0x02 }, // (Offset,Value)-Pair #2

        // D3DDevice_Present+0x11 : mov eax, [esi+2518]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #3
        { 0x12, 0x86 }, // (Offset,Value)-Pair #4
        { 0x13, 0x18 }, // (Offset,Value)-Pair #5
        { 0x14, 0x25 }, // (Offset,Value)-Pair #6

        // D3DDevice_Present+0xBE : shr ebx, 1
        { 0xBE, 0xD1 }, // (Offset,Value)-Pair #7
        { 0xBF, 0xEB }, // (Offset,Value)-Pair #8

        // D3DDevice_Present+0xC0 : xor edi, edi
        { 0xC0, 0x33 }, // (Offset,Value)-Pair #9
        { 0xC1, 0xFF }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 3925, 12)

        // D3DDevice_SetShaderConstantMode+0x26 : mov [ebp+0x2140], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x85 }, // (Offset,Value)-Pair #2
        { 0x28, 0x40 }, // (Offset,Value)-Pair #3
        { 0x29, 0x21 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetShaderConstantMode+0x51 : mov dword ptr [ebx+0x04], 0x3C
        { 0x51, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x52, 0x43 }, // (Offset,Value)-Pair #6
        { 0x53, 0x04 }, // (Offset,Value)-Pair #7
        { 0x54, 0x3C }, // (Offset,Value)-Pair #8

        // D3DDevice_SetShaderConstantMode+0xE4 : add ebx, 0x0124
        { 0xE4, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE5, 0xC3 }, // (Offset,Value)-Pair #10
        { 0xE6, 0x24 }, // (Offset,Value)-Pair #11
        { 0xE7, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 3925, 12)

        // D3DDevice_GetBackBuffer+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x07
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x07 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer+0x1E : lea eax, [ecx+eax*8+0x2150]
        { 0x1E, 0x8D }, // (Offset,Value)-Pair #6
        { 0x1F, 0x84 }, // (Offset,Value)-Pair #7
        { 0x20, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x21, 0x50 }, // (Offset,Value)-Pair #9
        { 0x22, 0x21 }, // (Offset,Value)-Pair #10

        // D3DDevice_GetBackBuffer+0x31 : retn 0x0C
        { 0x31, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x32, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 3925, 9)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x040C]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetRenderTarget+0x11 : mov [ecx], eax
        { 0x11, 0x89 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x1E, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface, 3925, 10)

        // D3DDevice_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetDepthStencilSurface+0x06 : mov eax, [eax + 0x0410]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x10 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetDepthStencilSurface+0x13 : jnz +0x04
        { 0x13, 0x74 }, // (Offset,Value)-Pair #6
        { 0x14, 0x0B }, // (Offset,Value)-Pair #7

        // D3DDevice_GetDepthStencilSurface+0x16 : call [addr]
        { 0x16, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetDepthStencilSurface+0x25 : retn
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x26, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer, 3925, 13)

        // D3DDevice_CreateVertexBuffer+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #1
        { 0x04, 0x40 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateVertexBuffer+0x10 : mov eax, [esp+8]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #3
        { 0x11, 0x44 }, // (Offset,Value)-Pair #4
        { 0x12, 0x24 }, // (Offset,Value)-Pair #5
        { 0x13, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_CreateVertexBuffer+0x48 : mov dword ptr [esi], 0x01000001
        { 0x48, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x49, 0x06 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x01 }, // (Offset,Value)-Pair #9
        { 0x4B, 0x00 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x01 }, // (Offset,Value)-Pair #11

        // D3DDevice_CreateVertexBuffer+0x53 : retn 0x14
        { 0x53, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x54, 0x14 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock, 3925, 11)

        // D3DVertexBuffer_Lock+0x01 : mov bl, [esp+0x18]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x18 }, // (Offset,Value)-Pair #4

        // D3DVertexBuffer_Lock+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x1E }, // (Offset,Value)-Pair #6

        // D3DVertexBuffer_Lock+0x29 : test bl, 0xA0
        { 0x29, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x2A, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x2B, 0xA0 }, // (Offset,Value)-Pair #9

        // D3DVertexBuffer_Lock+0x4A : retn 0x14
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DResource_Register
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Register, 3925, 19)

        // D3DResource_Register+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x4C }, // (Offset,Value)-Pair #2
        { 0x03, 0x04 }, // (Offset,Value)-Pair #3

        // D3DResource_Register+0x04 : mov edx, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x05, 0x54 }, // (Offset,Value)-Pair #5
        { 0x07, 0x08 }, // (Offset,Value)-Pair #6

        // D3DResource_Register+0x08 : mov eax, [ecx+0x04]
        { 0x08, 0x8B }, // (Offset,Value)-Pair #7
        { 0x09, 0x41 }, // (Offset,Value)-Pair #8
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #9

        // D3DResource_Register+0x0F : and edx, 0x00070000
        { 0x0F, 0x81 }, // (Offset,Value)-Pair #10
        { 0x10, 0xE2 }, // (Offset,Value)-Pair #11
        { 0x13, 0x07 }, // (Offset,Value)-Pair #12

        // D3DResource_Register+0x15 : cmp edx, 0x00020000
        { 0x15, 0x81 }, // (Offset,Value)-Pair #13
        { 0x16, 0xFA }, // (Offset,Value)-Pair #14
        { 0x19, 0x02 }, // (Offset,Value)-Pair #15

        // D3DResource_Register+0x1B : jnz +0x05
        { 0x1B, 0x74 }, // (Offset,Value)-Pair #16
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #17

        // D3DResource_Register+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #18
        { 0x26, 0x08 }, // (Offset,Value)-Pair #19
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Release, 3925, 11)

        // D3DResource_Release+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // D3DResource_Release+0x14 : and eax, 0x00070000
        { 0x14, 0x25 }, // (Offset,Value)-Pair #5
        { 0x17, 0x07 }, // (Offset,Value)-Pair #6

        // D3DResource_Release+0x19 : cmp eax, 0x00050000
        { 0x19, 0x3D }, // (Offset,Value)-Pair #7
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #8

        // D3DResource_Release+0x3E : pop esi
        { 0x3E, 0x5E }, // (Offset,Value)-Pair #9

        // D3DResource_Release+0x4B : retn 0x04
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DResource_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DResource_AddRef, 3925, 10)

        // D3DResource_AddRef+0x07 : test eax, 0xFFFF
        { 0x07, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x08, 0xFF }, // (Offset,Value)-Pair #2
        { 0x09, 0xFF }, // (Offset,Value)-Pair #3

        // D3DResource_AddRef+0x0E : and eax, 0x00070000
        { 0x0E, 0x25 }, // (Offset,Value)-Pair #4
        { 0x11, 0x07 }, // (Offset,Value)-Pair #5

        // D3DResource_AddRef+0x13 : cmp eax, 0x00050000
        { 0x13, 0x3D }, // (Offset,Value)-Pair #6
        { 0x16, 0x05 }, // (Offset,Value)-Pair #7

        // D3DResource_AddRef+0x33 : pop esi
        { 0x33, 0x5E }, // (Offset,Value)-Pair #8

        // D3DResource_AddRef+0x34 : retn 0x04
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x35, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DResource_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DResource_IsBusy, 3925, 11)

        // D3DResource_IsBusy+0x24 : test eax, 0x780000
        { 0x24, 0xA9 }, // (Offset,Value)-Pair #1
        { 0x25, 0x00 }, // (Offset,Value)-Pair #2
        { 0x26, 0x00 }, // (Offset,Value)-Pair #3
        { 0x27, 0x78 }, // (Offset,Value)-Pair #4

        // D3DResource_IsBusy+0x35 : jnz +0x44
        { 0x35, 0x75 }, // (Offset,Value)-Pair #5
        { 0x36, 0x44 }, // (Offset,Value)-Pair #6

        // D3DResource_IsBusy+0x4E : mov eax, [ecx+0x14]
        { 0x4E, 0x8B }, // (Offset,Value)-Pair #7
        { 0x4F, 0x41 }, // (Offset,Value)-Pair #8
        { 0x50, 0x14 }, // (Offset,Value)-Pair #9

        // D3DResource_IsBusy+0x79 : jnb +0x09
        { 0x79, 0x73 }, // (Offset,Value)-Pair #10
        { 0x7A, 0x09 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DSurface_LockRect
// ******************************************************************
OOVPA_NO_XREF(D3DSurface_LockRect, 3925, 16)

        // D3DSurface_LockRect+0x00 : mov eax, [esp+0x10]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

        // D3DSurface_LockRect+0x04 : mov ecx, [esp+0x0C]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0C }, // (Offset,Value)-Pair #8

        // D3DSurface_LockRect+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9

        // D3DSurface_LockRect+0x11 : push ecx; push edx; push 0; push 0
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x12, 0x52 }, // (Offset,Value)-Pair #11
        { 0x13, 0x6A }, // (Offset,Value)-Pair #12
        { 0x15, 0x6A }, // (Offset,Value)-Pair #13

        // D3DSurface_LockRect+0x18 : call [abs]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #14

        // D3DSurface_LockRect+0x11 : retn 0x10
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #15
        { 0x1E, 0x10 }, // (Offset,Value)-Pair #16
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DPalette_Lock, 3925, 13)

        // D3DPalette_Lock+0x00 : test [esp+0x0C], 0xA0
        { 0x00, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x0C }, // (Offset,Value)-Pair #4
        { 0x04, 0xA0 }, // (Offset,Value)-Pair #5

        // D3DPalette_Lock+0x0A : jnz +0x06
        { 0x0A, 0x75 }, // (Offset,Value)-Pair #6
        { 0x0B, 0x06 }, // (Offset,Value)-Pair #7

        // D3DPalette_Lock+0x18 : mov ecx, [esp+0x0C]
        { 0x18, 0x8B }, // (Offset,Value)-Pair #8
        { 0x19, 0x4C }, // (Offset,Value)-Pair #9
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #10
        { 0x1B, 0x0C }, // (Offset,Value)-Pair #11

        // D3DPalette_Lock+0x1C : mov [ecx], eax
        { 0x1C, 0x89 }, // (Offset,Value)-Pair #12
        { 0x1D, 0x01 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DTexture_LockRect
// ******************************************************************
OOVPA_NO_XREF(D3DTexture_LockRect, 3925, 17)

        // D3DTexture_LockRect+0x00 : mov eax, [esp+0x14]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x14 }, // (Offset,Value)-Pair #4

        // D3DTexture_LockRect+0x04 : mov ecx, [esp+0x10]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x10 }, // (Offset,Value)-Pair #8

        // D3DTexture_LockRect+0x0C : push eax, push ecx, push edx, push eax, push 0
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x16, 0x52 }, // (Offset,Value)-Pair #11
        { 0x17, 0x50 }, // (Offset,Value)-Pair #12
        { 0x18, 0x6A }, // (Offset,Value)-Pair #13
        { 0x19, 0x00 }, // (Offset,Value)-Pair #14

        // D3DTexture_LockRect+0x1B : call [addr]
        { 0x1B, 0xE8 }, // (Offset,Value)-Pair #15

        // D3DTexture_LockRect+0x20 : retn 0x14
        { 0x20, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x21, 0x14 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * D3DVolumeTexture_LockBox
// ******************************************************************
OOVPA_NO_XREF(D3DVolumeTexture_LockBox, 3925, 11)

        // D3DVolumeTexture_LockBox+0x08 : test bl, 0x20
        { 0x08, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x09, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x0A, 0x20 }, // (Offset,Value)-Pair #3

        // D3DVolumeTexture_LockBox+0x40 : test bl, 0x40
        { 0x40, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x41, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x42, 0x40 }, // (Offset,Value)-Pair #6

        // D3DVolumeTexture_LockBox+0x83 : lea ecx, [eax+edx]
        { 0x83, 0x8D }, // (Offset,Value)-Pair #7
        { 0x84, 0x0C }, // (Offset,Value)-Pair #8
        { 0x85, 0x10 }, // (Offset,Value)-Pair #9

        // D3DVolumeTexture_LockBox+0x99 : retn 0x14
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x9A, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DCubeTexture_LockRect
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_LockRect, 3925, 11)

        // D3DCubeTexture_LockRect+0x06 : test bl, 0x20
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x20 }, // (Offset,Value)-Pair #3

        // D3DCubeTexture_LockRect+0x3F : test bl, 0x40
        { 0x3F, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x40, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x41, 0x40 }, // (Offset,Value)-Pair #6

        // D3DCubeTexture_LockRect+0x71 : shr edx, 0x03
        { 0x71, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x72, 0xEA }, // (Offset,Value)-Pair #8
        { 0x73, 0x03 }, // (Offset,Value)-Pair #9

        // D3DCubeTexture_LockRect+0x98 : retn 0x18
        { 0x98, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x99, 0x18 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetSurfaceLevel
// ******************************************************************
OOVPA_NO_XREF(D3DTexture_GetSurfaceLevel, 3925, 11)

        // D3DTexture_GetSurfaceLevel+0x08 : lea [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0B, 0x14 }, // (Offset,Value)-Pair #2

        // D3DTexture_GetSurfaceLevel+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #3
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #4

        // D3DTexture_GetSurfaceLevel+0x20 : lea ecx, [esp+0x28]
        { 0x23, 0x28 }, // (Offset,Value)-Pair #4

        // D3DTexture_GetSurfaceLevel+0x24 : push ecx
        { 0x24, 0x51 }, // (Offset,Value)-Pair #6

        // D3DTexture_GetSurfaceLevel+0x25 : push edx
        { 0x25, 0x52 }, // (Offset,Value)-Pair #7

        // D3DTexture_GetSurfaceLevel+0x26 : push 0
        { 0x26, 0x6A }, // (Offset,Value)-Pair #8

        // D3DTexture_GetSurfaceLevel+0x43 : call [abs]
        { 0x43, 0xE8 }, // (Offset,Value)-Pair #9

        // D3DTexture_GetSurfaceLevel+0x4C : retn 0x0C
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4D, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * Lock2DSurface
// ******************************************************************
OOVPA_NO_XREF(Lock2DSurface, 3925, 11)

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
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 3925, 10)

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
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderSize
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderSize, 3925, 7)

        { 0x01, 0x44 },
        { 0x04, 0x85 },
        { 0x07, 0x09 },
        { 0x0A, 0x24 },
        { 0x0D, 0x51 },
        { 0x10, 0x10 },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 3925, 9)

        // D3DDevice_SetGammaRamp+0x11 : and eax, 1; push ebx
        { 0x11, 0x83 }, // (Offset,Value)-Pair #1
        { 0x12, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x13, 0x01 }, // (Offset,Value)-Pair #3
        { 0x14, 0x53 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetGammaRamp+0x2F : rep movsd
        { 0x2F, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x30, 0xA5 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetGammaRamp+0x3E : push ebx; mov ecx, edx
        { 0x3E, 0x53 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x8B }, // (Offset,Value)-Pair #8
        { 0x40, 0xCA }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 3925, 11)

        // D3DDevice_SetMaterial+0x0B : lea edi, [eax+0xB18]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x0D, 0x18 }, // (Offset,Value)-Pair #3
        { 0x0E, 0x0B }, // (Offset,Value)-Pair #4

        // D3DDevice_SetMaterial+0x11 : mov ecx, 0x11
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x12, 0x11 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetMaterial+0x1C : or ecx, 0x20
        { 0x1B, 0x83 }, // (Offset,Value)-Pair #7
        { 0x1C, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x1D, 0x20 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetMaterial+0x23 : retn 0x04
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x24, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 3925, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0408]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x08 }, // (Offset,Value)-Pair #4
        { 0x08, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // D3DDevice_AddRef+0x05 : mov [eax+0x0408], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_Unknown1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Unknown1, 3925, 9)

		// D3DDevice_Unknown1+0x00 : xor eax, eax
		{ 0x00, 0x33 },
		{ 0x01, 0xC0 },
		// D3DDevice_Unknown1+0x13 : call ds:AvSendTVEncoderOption
		{ 0x13, 0xFF },
		{ 0x14, 0x15 },
		{ 0x15, 0x10 },
		{ 0x16, 0x49 },
		{ 0x17, 0x2A },
		{ 0x18, 0x00 },
		// D3DDevice_Unknown1+0x1E : retn
		{ 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 3925, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x0B00]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x00 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x0B }, // (Offset,Value)-Pair #6

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetGammaRamp, 3925, 7)

        { 0x05, 0x8B },
        { 0x0A, 0x00 },
        { 0x10, 0xE1 },
        { 0x16, 0x0C },
        { 0x1C, 0x00 },
        { 0x22, 0x00 },
        { 0x28, 0x04 },
OOVPA_END;
// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 3925, 14)

        // D3DDevice_GetDisplayFieldStatus+0x05 : mov edx, [eax+0x2B28]
		{ 0x05, 0x8B },
		{ 0x06, 0x90 },
		{ 0x07, 0x28 },
		{ 0x08, 0x2B },
		// D3DDevice_GetDisplayFieldStatus+0x12 : test [eax+0x2510], 0x1200000
		{ 0x12, 0xF7 },
		{ 0x13, 0x80 },
		{ 0x14, 0x10 },
		{ 0x15, 0x25 },
		// D3DDevice_GetDisplayFieldStatus+0x1E : mov dx, 0x80C0
		{ 0x1E, 0x66 },
		{ 0x1F, 0xBA },
		{ 0x20, 0xC0 },
		{ 0x21, 0x80 },
		// D3DDevice_GetDisplayFieldStatus+0x3C : retn 4
		{ 0x3C, 0xC2 },
		{ 0x3D, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 3925, 7)

        { 0x0B, 0x8B },
        { 0x1D, 0x8B },
        { 0x26, 0x8B },
        { 0x2C, 0x8B },
        { 0x38, 0x0B },
        { 0x40, 0x89 },
        { 0x49, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 3925, 13)

        // D3DDevice_SetRenderState_VertexBlend+0x0A : or ecx, 2
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0B, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x02 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_VertexBlend+0x1A : mov dword ptr [eax], 0x40328
        { 0x1A, 0xC7 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #5
		{ 0x1C, 0x28 }, // (Offset,Value)-Pair #6
		{ 0x1D, 0x03 }, // (Offset,Value)-Pair #7
		{ 0x1E, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_VertexBlend+0x23 : add eax, 8
        { 0x23, 0x83 }, // (Offset,Value)-Pair #9
        { 0x24, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x25, 0x08 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_VertexBlend+0x2F : retn 0x04
        { 0x2F, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x30, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 3925, 12)

        // D3DDevice_SetRenderState_BackFillMode+0x2B : jnz +0x2
        { 0x2B, 0x75 }, // (Offset,Value)-Pair #1
        { 0x2C, 0x02 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_BackFillMode+0x32 : mov dword ptr [eax], abs
        { 0x32, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x33, 0x00 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_BackFillMode+0x3B : add eax, 0x0C; mov [edi], eax; pop edi
        { 0x3B, 0x83 }, // (Offset,Value)-Pair #5
        { 0x3C, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x0C }, // (Offset,Value)-Pair #7
        { 0x3E, 0x89 }, // (Offset,Value)-Pair #8
        { 0x3F, 0x07 }, // (Offset,Value)-Pair #9
        { 0x40, 0x5F }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_BackFillMode+0x48 : retn 0x04
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x49, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 3925, 13)

        // D3DDevice_SetRenderState_TwoSidedLighting+0x06 : or dword ptr [eax+8], 0x20
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0x48 }, // (Offset,Value)-Pair #2
        { 0x08, 0x08 }, // (Offset,Value)-Pair #3
        { 0x09, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_TwoSidedLighting+0x3B : mov dword ptr [eax], 0x8038C
        { 0x3B, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x3C, 0x00 }, // (Offset,Value)-Pair #6
        { 0x3D, 0x8C }, // (Offset,Value)-Pair #7
        { 0x3E, 0x03 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_TwoSidedLighting+0x44 : add eax, 0x0C
        { 0x44, 0x83 }, // (Offset,Value)-Pair #9
        { 0x45, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x46, 0x0C }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_TwoSidedLighting+0x51 : retn 0x04
        { 0x51, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x52, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 3925, 8)

        // D3DDevice_SetRenderState_NormalizeNormals+0x1D : mov [esi], eax
        { 0x1D, 0x89 }, // (Offset,Value)-Pair #1
        { 0x1E, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_NormalizeNormals+0x1F : or dword ptr [esi+8], 2
        { 0x1F, 0x83 }, // (Offset,Value)-Pair #3
        { 0x20, 0x4E }, // (Offset,Value)-Pair #4
        { 0x21, 0x08 }, // (Offset,Value)-Pair #5
        { 0x22, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_NormalizeNormals+0x2A : retn 0x04
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x2B, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FrontFace, 3925, 9)

        // D3DDevice_SetRenderState_FrontFace+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

		// D3DDevice_SetRenderState_FrontFace+0x07 : push esi
		{ 0x07, 0x56 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_FrontFace+0xD : mov ecx, [esp+0x08]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #3
        { 0x0E, 0x4C }, // (Offset,Value)-Pair #4
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #5
        { 0x10, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_FrontFace+0x1A : add eax, 8
        { 0x1A, 0x83 }, // (Offset,Value)-Pair #7
        { 0x1B, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 3925, 10)

        // D3DDevice_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x0414]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x86 }, // (Offset,Value)-Pair #2
        { 0x09, 0x14 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_TextureFactor+0x2E : rep stosd
		{ 0x2E, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x2F, 0xAB }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_TextureFactor+0x3C : retn 0x04
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x3D, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_TextureFactor+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 3925, 9)

        // D3DDevice_SetRenderState_LogicOp+0x11 : test ecx, ecx
        { 0x11, 0x85 }, // (Offset,Value)-Pair #1
        { 0x12, 0xC9 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_LogicOp+0x2D : mov dword ptr [eax], 0x817BC
        { 0x2D, 0xC7 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x00 }, // (Offset,Value)-Pair #4
        { 0x2F, 0xBC }, // (Offset,Value)-Pair #5
        { 0x30, 0x17 }, // (Offset,Value)-Pair #6
        { 0x31, 0x08 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_LogicOp+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 3925, 11)

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x17 : mov [eax+0x04], ecx
        { 0x17, 0x89 }, // (Offset,Value)-Pair #1
        { 0x18, 0x48 }, // (Offset,Value)-Pair #2
        { 0x19, 0x04 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x1A : mov [eax+0x08], ecx
        { 0x1A, 0x89 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x48 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #6

		// D3DDevice_SetRenderState_EdgeAntiAlias+0x1D : add eax, 0x0C
		{ 0x1D, 0x83 }, // (Offset,Value)-Pair #7
		{ 0x1E, 0xC0 }, // (Offset,Value)-Pair #8
		{ 0x1F, 0x0C }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x29 : retn 0x04
        { 0x29, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x2A, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask, 3925, 13)

        // D3DDevice_SetRenderState_MultiSampleMask+0x0B : mov ecx, esi
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0xCE }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_MultiSampleMask+0x32 : and ecx, 0x0F; shl edx, 0x10
        { 0x32, 0x83 }, // (Offset,Value)-Pair #3
        { 0x33, 0xE1 }, // (Offset,Value)-Pair #4
        { 0x34, 0x0F }, // (Offset,Value)-Pair #5
        { 0x35, 0xC1 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_MultiSampleMask+0x3A : mov dword ptr [eax], 0x41D7C
        { 0x3A, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x3B, 0x00 }, // (Offset,Value)-Pair #8
        { 0x3C, 0x7C }, // (Offset,Value)-Pair #9
        { 0x3D, 0x1D }, // (Offset,Value)-Pair #10
        { 0x3E, 0x04 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_MultiSampleMask+0x49 : retn 0x04
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4A, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 3925, 12)

        // D3DDevice_SetRenderState_MultiSampleMode+0x05 : mov ecx, [esp+4]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x4C }, // (Offset,Value)-Pair #2
        { 0x07, 0x24 }, // (Offset,Value)-Pair #3
        { 0x08, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov edx, [eax+0x410]
        { 0x0F, 0x8B }, // (Offset,Value)-Pair #5
        { 0x10, 0x90 }, // (Offset,Value)-Pair #6
        { 0x11, 0x10 }, // (Offset,Value)-Pair #7
        { 0x12, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : mov eax, [eax+0x40C]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #9
        { 0x16, 0x80 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleMode+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x23, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 3925, 14)

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
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 3925, 14)

        // D3DDevice_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_ShadowFunc+0x11 : mov dword ptr [eax], 0x41E6C
		{ 0x11, 0xC7 }, // (Offset,Value)-Pair #2
		{ 0x12, 0x00 }, // (Offset,Value)-Pair #3
		{ 0x13, 0x6C }, // (Offset,Value)-Pair #4
		{ 0x14, 0x1E }, // (Offset,Value)-Pair #5
		{ 0x15, 0x04 }, // (Offset,Value)-Pair #6
		{ 0x16, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_ShadowFunc+0x17 : lea edx, [ecx-0x0200]
        { 0x17, 0x8D }, // (Offset,Value)-Pair #8
        { 0x18, 0x91 }, // (Offset,Value)-Pair #9
        { 0x19, 0x00 }, // (Offset,Value)-Pair #10
        { 0x1A, 0xFE }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderState_ShadowFunc+0x20 : add eax, 0x08
        { 0x20, 0x83 }, // (Offset,Value)-Pair #12
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #13
        { 0x22, 0x08 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 3925, 10)

        // D3DDevice_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0x508]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2
        { 0x17, 0x08 }, // (Offset,Value)-Pair #3
        { 0x18, 0x05 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_LineWidth+0x5C : retn 0x04
        { 0x5C, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x5D, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable, 3925, 10)

        // D3DDevice_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_YuvEnable+0x04 : push edi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_YuvEnable+0x0B : push edi
        { 0x0B, 0x56 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_YuvEnable+0x1F : mov [esi], eax
        { 0x1F, 0x89 }, // (Offset,Value)-Pair #7
        { 0x20, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_YuvEnable+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x23, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_OcclusionCullEnable, 3925, 12)

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x20 : jz +0x05
        { 0x20, 0x74 }, // (Offset,Value)-Pair #1
        { 0x21, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x3B : cmp [abs], 0x1E00
        { 0x3B, 0x81 }, // (Offset,Value)-Pair #3
        { 0x41, 0x00 }, // (Offset,Value)-Pair #4
        { 0x42, 0x1E }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x4A : mov dword ptr [eax], 0x41D84
        { 0x4A, 0xC7 }, // (Offset,Value)-Pair #6
        { 0x4B, 0x00 }, // (Offset,Value)-Pair #7
        { 0x4C, 0x84 }, // (Offset,Value)-Pair #8
        { 0x4D, 0x1D }, // (Offset,Value)-Pair #9
        { 0x4E, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x59 : retn 0x04
        { 0x59, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x5A, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_StencilCullEnable, 3925, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_D3DRS_STENCILCULLENABLE ),

        { 0x0B, 0x56 },
        { 0x16, 0x8B },
        { 0x22, 0xB9 },
        { 0x2E, 0xD2 },
        { 0x3A, 0x0C },
        { 0x46, 0x03 },
        { 0x52, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 3925, 8)

        { 0x1F, 0x8B },
        { 0x3E, 0xC0 },
        { 0x5E, 0x03 },
        { 0x7E, 0x07 },
        { 0x9E, 0xF0 },
        { 0xBE, 0x40 },
        { 0xDE, 0x75 },
        { 0xFE, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 3925, 8)

        { 0x1F, 0x8B },
        { 0x3E, 0x8B },
        { 0x5E, 0xF0 },
        { 0x7E, 0x8B },
        { 0xA2, 0x8B },
        { 0xBE, 0x83 },
        { 0xDE, 0x75 },
        { 0xFE, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVertices, 3925, 8)

        { 0x12, 0x8B },
        { 0x27, 0x8B },
        { 0x3A, 0x89 },
        { 0x4E, 0xFF },
        { 0x62, 0x81 },
        { 0x76, 0x00 },
        { 0x8A, 0x17 },
        { 0x9E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 3925, 7)

        { 0x09, 0xB4 },
        { 0x14, 0x8B },
        { 0x1F, 0x10 },
        { 0x2A, 0x1B },
        { 0x35, 0x8B },
        { 0x41, 0x89 },
        { 0x4B, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 3925, 7)

        { 0x0E, 0x8B },
        { 0x16, 0x8B },
        { 0x22, 0x24 },
        { 0x2E, 0x24 },
        { 0x3A, 0x04 },
        { 0x46, 0x8B },
        { 0x52, 0xB5 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor, 3925, 7)

        { 0x07, 0x56 },
        { 0x0D, 0x8B },
        { 0x13, 0x8D },
        { 0x1A, 0x8B },
        { 0x21, 0x83 },
        { 0x28, 0x07 },
        { 0x2F, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 3925, 8)

        { 0x07, 0x56 },
        { 0x10, 0x08 },
        { 0x19, 0xBF },
        { 0x22, 0x24 },
        { 0x2B, 0x54 },
        { 0x34, 0x24 },
        { 0x3D, 0x08 },
        { 0x46, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_SetPushBufferSize
// ******************************************************************
OOVPA_NO_XREF(D3D_SetPushBufferSize, 3925, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0D, 0x89 },
        { 0x0E, 0x0D },
        { 0x13, 0xC2 },
        { 0x14, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DResource_GetType
// ******************************************************************
OOVPA_NO_XREF(D3DResource_GetType, 3925, 7)

        { 0x13, 0x2A },
        { 0x28, 0x00 },
        { 0x3D, 0x00 },
        { 0x52, 0x74 },
        { 0x67, 0x00 },
        { 0x7C, 0x04 },
        { 0x91, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_AllocContiguousMemory
// ******************************************************************
OOVPA_NO_XREF(D3D_AllocContiguousMemory, 3925, 6)

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x04 },
        { 0x0E, 0x6A },
        { 0x16, 0xC2 },
        { 0x17, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Deferred
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Deferred, 3925, 9)

		// D3DDevice_SetRenderState_Deferred+0x00 : mov eax, ds:(g_DirtyFromRenderState)-148[ecx*4]
		{ 0x00, 0x8B },
		{ 0x01, 0x04 },
		{ 0x02, 0x8D },
		// D3DDevice_SetRenderState_Deferred+0x07 : or dword ptr ds:g_Device+8,eax
		{ 0x07, 0x09 },
		{ 0x08, 0x05 },
		// D3DDevice_SetRenderState_Deferred+0x0D : mov dword ptr ds:D3D__RenderState[ecx*4], edx
		{ 0x0D, 0x89 },
		{ 0x0E, 0x14 },
		{ 0x0F, 0x8D },
		// D3DDevice_SetRenderState_Deferred+0x14 : retn
		{ 0x14, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetLight, 3925, 10)
// Based on Dxbx patterns for 3911, 4361, 4627, 5344, 5558, 5659, 5788, 5849, 5933

        // D3DDevice_GetLight+0x0B : mov esi, [ecx+XXXh]
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB1 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #3
        { 0x10, 0x00 }, // (Offset,Value)-Pair #4

        // D3DDevice_GetLight+0x12 : mov edi, [esp+8+arg_4]
        { 0x12, 0x8B }, // (Offset,Value)-Pair #5
        { 0x13, 0x7C }, // (Offset,Value)-Pair #6
        { 0x14, 0x24 }, // (Offset,Value)-Pair #7
        { 0x15, 0x10 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetLight+0x1F : mov ecx, 1Ah
        { 0x1E, 0xB9 }, // (Offset,Value)-Pair #9
        { 0x1F, 0x1A }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 3925, 8)

        { 0x1D, 0x0F },
        { 0x3C, 0xD8 },
        { 0x5B, 0x00 },
        { 0x7A, 0x89 },
        { 0x99, 0xA5 },
        { 0xB8, 0xC1 },
        { 0xD7, 0x00 },
        { 0xF6, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 3925, 7)

        { 0x1A, 0x04 },
        { 0x36, 0x00 },
        { 0x52, 0x00 },
        { 0x6E, 0x75 },
        { 0x8A, 0x88 },
        { 0xA6, 0x75 },
        { 0xC2, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 3925, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0xA0 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 3925, 7)

        { 0x03, 0x04 },
        { 0x0A, 0x56 },
        { 0x0D, 0x7C },
        { 0x12, 0x40 },
        { 0x17, 0x21 },
        { 0x1C, 0x00 },
        { 0x21, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_ApplyStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_ApplyStateBlock, 3925, 8)

        { 0x1D, 0x83 },
        { 0x40, 0x83 },
        { 0x5B, 0x16 },
        { 0x7A, 0x01 },
        { 0x99, 0x46 },
        { 0xB8, 0x06 },
        { 0xD7, 0x39 },
        { 0xF6, 0x51 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 3925, 7)

        { 0x1E, 0x76 },
        { 0x3E, 0xE8 },
        { 0x5E, 0x06 },
        { 0x7E, 0x26 },
        { 0x9E, 0xFF },
        { 0xBE, 0x04 },
        { 0xDE, 0xF8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock, 3925, 7)

        { 0x11, 0x76 },
        { 0x24, 0x3B },
        { 0x37, 0xE8 },
        { 0x4A, 0x50 },
        { 0x5D, 0x74 },
        { 0x70, 0x06 },
        { 0x83, 0xEB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 3925, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x5C },
        { 0x0E, 0x0B },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateStateBlock, 3925, 8)

        { 0x1E, 0x8B },
        { 0x3E, 0x89 },
        { 0x5E, 0x24 },
        { 0x7E, 0xF8 },
        { 0x9F, 0x01 },
        { 0xBE, 0xB6 },
        { 0xDE, 0xF8 },
        { 0xFE, 0x76 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeletePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeletePixelShader, 3925, 7)

        { 0x02, 0x24 },
        { 0x06, 0x75 },
        { 0x0A, 0x04 },
        { 0x0E, 0x09 },
        { 0x12, 0x04 },
        { 0x18, 0xC2 },
        { 0x1A, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram, 3925, 7)

        { 0x06, 0xA1 },
        { 0x0E, 0x88 },
        { 0x16, 0x00 },
        { 0x1E, 0x00 },
        { 0x26, 0x04 },
        { 0x32, 0xC7 },
        { 0x36, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
// * This is for the real D3D::KickOffAndWaitForIdle
// ******************************************************************
OOVPA_XREF(D3D_KickOffAndWaitForIdle, 3925, 5,

    XREF_D3D_KickOffAndWaitForIdle,
    XRefZero)

        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x1C },
        { 0x0A, 0x51 },
        { 0x10, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView, 3925, 7)

        { 0x12, 0x08 },
        { 0x26, 0x53 },
        { 0x3A, 0x8B },
        { 0x4E, 0x80 },
        { 0x62, 0x00 },
        { 0x76, 0x00 },
        { 0x8A, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache, 3925, 8)

        { 0x00, 0x56 },
        { 0x07, 0x56 },
        { 0x08, 0xE8 },
        { 0x0D, 0xC7 },
        { 0x0F, 0x10 },
        { 0x12, 0x00 },
        { 0x17, 0x00 },
        { 0x1C, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 3925, 8)

        { 0x1D, 0x08 },
        { 0x3C, 0x24 },
        { 0x5F, 0x8B },
        { 0x7A, 0x83 },
        { 0x99, 0xE0 },
        { 0xBA, 0xD9 },
        { 0xD9, 0x8B },
        { 0xF6, 0xDB },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInput, 3925, 8)

        { 0x1E, 0x03 },
        { 0x3E, 0x8B },
        { 0x5E, 0xC7 },
        { 0x7E, 0xF8 },
        { 0x9E, 0xC6 },
        { 0xBE, 0x7F },
        { 0xDE, 0xCA },
        { 0xFE, 0x17 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache, 3925, 8)

        { 0x0D, 0x00 },
        { 0x1C, 0x55 },
        { 0x2B, 0x00 },
        { 0x3A, 0x24 },
        { 0x49, 0xCB },
        { 0x58, 0x74 },
        { 0x67, 0x00 },
        { 0x76, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4ub
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 3925, 7)

        { 0x07, 0x56 },
        { 0x10, 0x08 },
        { 0x19, 0xC9 },
        { 0x22, 0x54 },
        { 0x2B, 0x14 },
        { 0x34, 0x24 },
        { 0x3D, 0xFC },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 3925, 7)

        { 0x1E, 0x4C },
        { 0x3E, 0x11 },
        { 0x61, 0xDF },
        { 0x7E, 0xF6 },
        { 0x9E, 0x05 },
        { 0xC0, 0x51 },
        { 0xDE, 0xC4 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback, 3925, 7)

        { 0x0D, 0x8B },
        { 0x18, 0x8B },
        { 0x25, 0x50 },
        { 0x32, 0x10 },
        { 0x3F, 0x00 },
        { 0x4C, 0x00 },
        { 0x59, 0x03 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 3925, 7)

        { 0x0B, 0xCE },
        { 0x1B, 0x57 },
        { 0x25, 0x00 },
        { 0x32, 0x06 },
        { 0x3F, 0x03 },
        { 0x4C, 0x04 },
        { 0x59, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 3925, 7)

        { 0x11, 0x8D },
        { 0x24, 0x8B },
        { 0x37, 0xFC },
        { 0x4A, 0xFF },
        { 0x5D, 0xF7 },
        { 0x70, 0x03 },
        { 0x83, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, 9,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_RopZCmpAlwaysRead+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        XREF_ENTRY( 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD ), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_RopZCmpAlwaysRead+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_RopZCmpAlwaysRead+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_RopZCmpAlwaysRead+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_RopZRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZRead, 3925, 9,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_RopZRead+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        XREF_ENTRY( 0x05, XREF_D3DRS_ROPZREAD ), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_RopZRead+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_RopZRead+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_RopZRead+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_DoNotCullUncompressed
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, 9,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_DoNotCullUncompressed+0x05 : D3D__RenderState[D3DRS_ROPZCMPALWAYSREAD]
        XREF_ENTRY( 0x05, XREF_D3DRS_DONOTCULLUNCOMPRESSED ), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_DoNotCullUncompressed+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_DoNotCullUncompressed+0x09 : call [abs]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_DoNotCullUncompressed+0x0E : retn 0x04
        { 0x0E, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8
        { 0x10, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * XMETAL_StartPush
// ******************************************************************
OOVPA_NO_XREF(XMETAL_StartPush, 3925, 11)

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
OOVPA_END;

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 3925, 11,

    XREF_D3D_SETFENCE,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertFence
// ******************************************************************
OOVPA_XREF(D3DDevice_InsertFence, 3925, 5,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x03, XREF_D3D_SETFENCE ),

        { 0x00, 0x6A },
        { 0x01, 0x00 },
        { 0x02, 0xE8 },
        { 0x07, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 3925, 7)

        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0x00 },
        { 0x32, 0xA5 },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteVertexShader, 3925, 6)

        { 0x02, 0x24 },
        { 0x06, 0xFF },
        { 0x0A, 0x08 },
        { 0x0E, 0x44 },

        // D3DDevice_DeleteVertexShader+0x16 : retn 4
        { 0x16, 0xC2 },
        { 0x17, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 3925, 15)

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
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_XREF(D3DDevice_BlockOnFence, 3925, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_D3D_BLOCKONTIME ),

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(D3D_EnumAdapterModes, 3925, 8)

        { 0x20, 0x8B },
        { 0x3E, 0xE3 },
        { 0x5E, 0x75 },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x0F },
        { 0xE2, 0x89 },
        { 0xFE, 0x5D },
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterModeCount
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterModeCount, 3925, 8)

        { 0x14, 0x8B },
        { 0x26, 0xE2 },
        { 0x3A, 0x00 },
        { 0x4E, 0x00 },
        { 0x62, 0x19 },
        { 0x76, 0x75 },
        { 0x8A, 0x3B },
        { 0x9E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 3925, 7)

        { 0x15, 0x8B },
        { 0x2C, 0x8B },
        { 0x43, 0x8B },
        { 0x5A, 0x08 },
        { 0x74, 0x8B },
        { 0x88, 0x50 },
        { 0x9F, 0x03 },
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterIdentifier
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterIdentifier, 3925, 7)

        { 0x04, 0x85 },
        { 0x0A, 0x08 },
        { 0x10, 0x56 },
        { 0x16, 0xB9 },
        { 0x20, 0xF3 },
        { 0x22, 0x5F },
        { 0x28, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D_GetDeviceCaps
// ******************************************************************
OOVPA_NO_XREF(D3D_GetDeviceCaps, 3925, 7)

        { 0x04, 0x85 },
        { 0x0A, 0x08 },
        { 0x10, 0x83 },
        { 0x16, 0x08 },
        { 0x1C, 0xC2 },
        { 0x22, 0x0C },
        { 0x29, 0x33 },
OOVPA_END;

// ******************************************************************
// * D3D::SetCommonDebugRegisters
// ******************************************************************
OOVPA_NO_XREF(D3D_SetCommonDebugRegisters, 3925, 10)

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
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 3925, 7)

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x24 },
        { 0x0E, 0x44 },
        { 0x12, 0x00 },
        { 0x16, 0xE8 },
        { 0x1B, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 3925, 9,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
OOVPA_XREF(D3D_BlockOnResource, 3925, 9,

    XREF_D3D_BlockOnResource,
    XRefZero)

	{ 0x34, 0x8B }, // mov edx, [ecx+0x1C]
	{ 0x35, 0x51 },
	{ 0x36, 0x1C },
	{ 0x41, 0xC2 }, // retn 4
        { 0x42, 0x04 },
        { 0x43, 0x00 },
        { 0x7E, 0xC2 }, // retn 4
        { 0x7F, 0x04 },
        { 0x80, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DResource_BlockUntilNotBusy
// ******************************************************************
OOVPA_XREF(D3DResource_BlockUntilNotBusy, 3925, 2,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3D_BlockOnResource ),

        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBlock, 3925, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x0C },
        { 0x08, 0x20 },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndStateBlock, 3925, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x0C },
        { 0x08, 0xDF },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 3925, 9)

        // D3DDevice_SetTile+0x06 : sub esp, 0x18
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xEC }, // (Offset,Value)-Pair #2
        { 0x08, 0x18 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTile+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 }, // (Offset,Value)-Pair #4
        { 0x16, 0x46 }, // (Offset,Value)-Pair #5
        { 0x17, 0x04 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTile+0x7E : lea edi, [edx+ecx*8+21BCh]
        { 0x7E, 0x83 }, // (Offset,Value)-Pair #7
        { 0x7F, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x80, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 3925, 9)

        { 0x0A, 0x57 },
        { 0x0B, 0x8D },
        { 0x0C, 0xB8 },
        { 0x0D, 0x5C },
        { 0x0E, 0x0B },
        { 0x0F, 0x00 },
        { 0x14, 0x00 },
        { 0x18, 0x8B },
        { 0x1E, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SwitchTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SwitchTexture, 3925, 10)

        // D3DDevice_SwitchTexture+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_SwitchTexture+0x05 : add eax, 0x0C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #2
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x07, 0x0C }, // (Offset,Value)-Pair #4

        // D3DDevice_SwitchTexture+0x08 : cmp eax, ds:dword_XXXXX
        { 0x08, 0x3B }, // (Offset,Value)-Pair #5
        { 0x09, 0x05 }, // (Offset,Value)-Pair #6

        // D3DDevice_SwitchTexture+0x0E : jnb +0x15
        { 0x0E, 0x73 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #8

        // D3DDevice_SwitchTexture+0x22 : retn 0x04
        { 0x22, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x23, 0x04 }, // (Offset,Value)-Pair #10

OOVPA_END;

// ******************************************************************
// * D3DDevice_GetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetModelView, 3925, 10)

        { 0x05, 0x57 },
        { 0x0A, 0x85 },
        { 0x10, 0xB0 },
        { 0x11, 0xE0 },
        { 0x12, 0x05 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0xB9 },
        { 0x16, 0x10 },
        { 0x1C, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 3925, 9)

        { 0x10, 0x0F },
        { 0x11, 0xB6 },
        { 0x12, 0x75 },
        { 0x13, 0x0D },
        { 0x14, 0x8A },
        { 0x15, 0x9E },
        { 0x40, 0xF6 },
        { 0x4F, 0x84 },
        { 0x75, 0x15 },
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_GetDesc
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_GetDesc, 3925, 7)

        { 0x02, 0x24 },
        { 0x06, 0x74 },
        { 0x0A, 0xC7 },
        { 0x0E, 0x00 },
        { 0x15, 0x89 },
        { 0x16, 0x46 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 3925, 8)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x40 },
        { 0x08, 0x21 },
        { 0x0B, 0x8B },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 3925, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x74 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderConstant, 3925, 9)

        { 0x05, 0x8B },
        { 0x06, 0x74 },
        { 0x07, 0x24 },
        { 0x08, 0x08 },
        { 0x09, 0x57 },
        { 0x0A, 0x8B },
        { 0x0B, 0x3D },
        { 0x18, 0x00 },
        { 0x1F, 0xF7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderInput, 3925, 8)

        { 0x00, 0x8B },
        { 0x03, 0x08 },
        { 0x04, 0x85 },
        { 0x05, 0xC9 },
        { 0x06, 0xA1 },
        { 0x10, 0x07 },
        { 0x18, 0x04 },
        { 0x1F, 0x20 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunVertexStateShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunVertexStateShader, 3925, 9)

        { 0x0D, 0x8B },
        { 0x10, 0x0C },
        { 0x11, 0x85 },
        { 0x12, 0xC9 },
        { 0x13, 0x74 },
        { 0x14, 0x21 },
        { 0x15, 0xD9 },
        { 0x16, 0x41 },
        { 0x1F, 0x41 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderType
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderType, 3925, 7)

        { 0x04, 0x8B },
        { 0x0A, 0x07 },
        { 0x10, 0xEB },
        { 0x16, 0x00 },
        { 0x1C, 0x40 },
        { 0x22, 0xC9 },
        { 0x28, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderDeclaration, 3925, 9)

        { 0x00F, 0x8B },
        { 0x010, 0x4C },
        { 0x011, 0x24 },
        { 0x012, 0x1C },
        { 0x013, 0xC7 },
        { 0x014, 0x01 },
        { 0x015, 0x00 },
        { 0x11F, 0x83 },
        { 0x129, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderFunction
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderFunction, 3925, 7)

        { 0x16, 0xC0 },
        { 0x2E, 0x01 },
        { 0x46, 0x10 },
        { 0x5E, 0x07 },
        { 0x76, 0xC7 },
        { 0x8E, 0x00 },
        { 0xA6, 0xF8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 3925, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x14 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_IsFencePending
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsFencePending, 3925, 11)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xF0 },
        { 0x08, 0x03 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x40 },
        { 0x0D, 0x1C },
        { 0x1A, 0x1B },
        { 0x1F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3D8_3925
// ******************************************************************
OOVPATable D3D8_3925[] = {

	REGISTER_OOVPA(Direct3D_CreateDevice, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Present, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3925, DISABLED),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3925, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3925, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Unknown1, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 3925, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3DResource_GetType, 3925, DISABLED), // Probably not even necessary...
	REGISTER_OOVPA(D3D_AllocContiguousMemory, 3925, DISABLED), // Just calls MmAllocateContiguousMemory. Was PATCH
	REGISTER_OOVPA(D3DDevice_SetRenderState_Deferred, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 3948, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(XMETAL_StartPush, 3925, DISABLED),
	REGISTER_OOVPA(D3D_SetFence, 3925, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 3925, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3925, DISABLED),
	REGISTER_OOVPA(D3D_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3D_SetCommonDebugRegisters, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 3925, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 3925, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 3925, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_GetDesc, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 3925, PATCH),
};

// ******************************************************************
// * D3D8_3925_SIZE
// ******************************************************************
uint32 D3D8_3925_SIZE = sizeof(D3D8_3925);
