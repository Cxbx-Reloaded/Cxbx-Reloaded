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
// * Direct3D_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(Direct3D_CreateDevice, 4627, 8)

        // Direct3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // Direct3D_CreateDevice+0x80 : repe stosd
        { 0x80, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x81, 0xAB }, // (Offset,Value)-Pair #4

        // Direct3D_CreateDevice+0x83 : mov eax, esi
        { 0x83, 0x8B }, // (Offset,Value)-Pair #5
        { 0x84, 0xC6 }, // (Offset,Value)-Pair #6

        // Direct3D_CreateDevice+0x90 : retn 0x18
        { 0x90, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x91, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3D_CreateDeviceX (Direct3D_CreateDevice from D3D8LTCG)
// ******************************************************************
OOVPA_NO_XREF(D3D_CreateDeviceX, 4627, 8)

        // Direct3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // Direct3D_CreateDevice+0x7F : repe stosd
        { 0x7F, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x80, 0xAB }, // (Offset,Value)-Pair #4

        // Direct3D_CreateDevice+0x82 : mov eax, esi
        { 0x82, 0x8B }, // (Offset,Value)-Pair #5
        { 0x83, 0xC6 }, // (Offset,Value)-Pair #6

        // Direct3D_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 }, // (Offset,Value)-Pair #7
        { 0xA1, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 4627, 11)

        // D3DDevice_BeginVisibilityTest+0x13 : mov dword ptr [eax], 0x000817C8
        { 0x13, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x14, 0x00 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x17 }, // (Offset,Value)-Pair #4
        { 0x17, 0x08 }, // (Offset,Value)-Pair #5

        // D3DDevice_BeginVisibilityTest+0x19 : mov ecx, 1
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1A, 0x01 }, // (Offset,Value)-Pair #7
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #8

        // D3DDevice_BeginVisibilityTest+0x24 : add eax, 0x0C
        { 0x24, 0x83 }, // (Offset,Value)-Pair #9
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x26, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 4627, 11)

        // D3DDevice_EndVisibilityTest+0x0B : mov esi, eax
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0xF0 }, // (Offset,Value)-Pair #2

        // D3DDevice_EndVisibilityTest+0x0D : test esi, esi
        { 0x0D, 0x85 }, // (Offset,Value)-Pair #3
        { 0x0E, 0xF6 }, // (Offset,Value)-Pair #4

        // D3DDevice_EndVisibilityTest+0x0D : jnz +0x09
        { 0x0F, 0x75 }, // (Offset,Value)-Pair #5
        { 0x10, 0x09 }, // (Offset,Value)-Pair #6

        // D3DDevice_EndVisibilityTest+0x11 : mov eax, 0x8007000E
        { 0x11, 0xB8 }, // (Offset,Value)-Pair #7
        { 0x12, 0x0E }, // (Offset,Value)-Pair #8
        { 0x13, 0x00 }, // (Offset,Value)-Pair #9
        { 0x14, 0x07 }, // (Offset,Value)-Pair #10
        { 0x15, 0x80 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterDisplayMode, 4627, 13)

        // D3D_GetAdapterDisplayMode+0x08 : mov eax, 0x8876086C
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x09, 0x6C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x76 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x88 }, // (Offset,Value)-Pair #5

        // D3D_GetAdapterDisplayMode+0x19 : jnz +0x18
        { 0x19, 0x75 }, // (Offset,Value)-Pair #6
        { 0x1A, 0x18 }, // (Offset,Value)-Pair #7

        // D3D_GetAdapterDisplayMode+0x33 : mov ecx, [esi+0x21C4]
        { 0x33, 0x8B }, // (Offset,Value)-Pair #8
        { 0x34, 0x8E }, // (Offset,Value)-Pair #9
        { 0x35, 0xC4 }, // (Offset,Value)-Pair #10
        { 0x36, 0x21 }, // (Offset,Value)-Pair #11

        // D3D_GetAdapterDisplayMode+0xB6 : retn 0x08
        { 0xB6, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xB7, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle, 4627, 9)

        // D3D_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3D_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // D3D_KickOffAndWaitForIdle+0x08 : push 2
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x02 }, // (Offset,Value)-Pair #6

        // D3D_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // D3D_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // D3D_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle2
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle2, 4627, 11)

        // D3D_KickOffAndWaitForIdle2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3D_KickOffAndWaitForIdle2+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // D3D_KickOffAndWaitForIdle2+0x08 : push 2
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x02 }, // (Offset,Value)-Pair #6

        // D3D_KickOffAndWaitForIdle2+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // D3D_KickOffAndWaitForIdle2+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // D3D_KickOffAndWaitForIdle2+0x18 : call [addr]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #9

        // D3D_KickOffAndWaitForIdle2+0x1D : retn 0x08
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x1E, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4627, 7)

        { 0x08, 0x00 },
        { 0x12, 0x8B },
        { 0x1C, 0x02 },
        { 0x26, 0x72 },
        { 0x30, 0xE8 },
        { 0x3A, 0x76 },
        { 0x44, 0x52 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetViewport, 4627, 9)

        // D3DDevice_SetViewport+0x0A : mov eax, [esi+0x21B4]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0C, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x21 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetViewport+0x63 : shl edi, cl
        { 0x63, 0xD3 }, // (Offset,Value)-Pair #5
        { 0x64, 0xE7 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetViewport+0xDF : and ecx, 0x0F
        { 0xDF, 0x83 }, // (Offset,Value)-Pair #7
        { 0xE0, 0xE1 }, // (Offset,Value)-Pair #8
        { 0xE1, 0x0F }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3D_CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget, 4627, 12,

    XREF_COMMONSETRENDERTARGET,
    XRefZero)

        // D3D_CommonSetRenderTarget+0x00 : sub esp, 0x20
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x20 }, // (Offset,Value)-Pair #3

        // D3D_CommonSetRenderTarget+0x86 : jmp +0x06
        { 0x86, 0xEB }, // (Offset,Value)-Pair #4
        { 0x87, 0x06 }, // (Offset,Value)-Pair #5

        // D3D_CommonSetRenderTarget+0x88 : and eax, 0x0FFF
        { 0x88, 0x25 }, // (Offset,Value)-Pair #6
        { 0x89, 0xFF }, // (Offset,Value)-Pair #7
        { 0x8A, 0x0F }, // (Offset,Value)-Pair #8
        { 0x8B, 0x00 }, // (Offset,Value)-Pair #9

        // D3D_CommonSetRenderTarget+0xC6 : and ecx, 0x0F
        { 0xC6, 0x83 }, // (Offset,Value)-Pair #10
        { 0xC7, 0xE1 }, // (Offset,Value)-Pair #11
        { 0xC8, 0x0F }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3D_CommonSetRenderTargetB
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTargetB, 4627, 11,

    XREF_COMMONSETRENDERTARGET,
    XRefZero)

        // D3D_CommonSetRenderTargetB+0x00 : sub esp, 0x24
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3

        // D3D_CommonSetRenderTargetB+0x86 : jmp +0x06
        { 0x8D, 0xEB }, // (Offset,Value)-Pair #4
        { 0x8E, 0x04 }, // (Offset,Value)-Pair #5

        // D3D_CommonSetRenderTargetB+0xE4 : and eax, 0x0FFF
        { 0xE4, 0x25 }, // (Offset,Value)-Pair #6
        { 0xE5, 0xFF }, // (Offset,Value)-Pair #7
        { 0xE6, 0x0F }, // (Offset,Value)-Pair #8
        { 0xE7, 0x00 }, // (Offset,Value)-Pair #9

        // D3D_CommonSetRenderTargetB+0xF4 : jge +0x06
        { 0xF4, 0x7D }, // (Offset,Value)-Pair #10
        { 0xF5, 0x06 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderTarget, 4627, 13,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderTarget+0x10 : call [D3D_CommonSetRenderTarget]
        XREF_ENTRY( 0x10, XREF_COMMONSETRENDERTARGET ), // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderTarget+0x00 : mov eax, [esp+0x08]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x08 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderTarget+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #6
        { 0x05, 0x4C }, // (Offset,Value)-Pair #7
        { 0x06, 0x24 }, // (Offset,Value)-Pair #8
        { 0x07, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderTarget+0x0D : push eax; push ecx
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #10
        { 0x0E, 0x51 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetRenderTarget+0x14 : retn 0x08
        { 0x14, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x15, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4627, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0500]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x05 }, // (Offset,Value)-Pair #5

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // D3DDevice_AddRef+0x0C : mov [eax+0x0500], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x05 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 4928, 9)

        { 0x25, 0x57 }, // (Offset,Value)-Pair #1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #2
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #3
        { 0x28, 0x01 }, // (Offset,Value)-Pair #4

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #5
        { 0x3A, 0xA7 }, // (Offset,Value)-Pair #6

        { 0x5A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x5B, 0x82 }, // (Offset,Value)-Pair #8
        { 0x5C, 0xE4 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4627, 12)

        // D3DDevice_SetShaderConstantMode+0x26 : mov [ebx+0x20D8], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x83 }, // (Offset,Value)-Pair #2
        { 0x28, 0xD8 }, // (Offset,Value)-Pair #3
        { 0x29, 0x20 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetShaderConstantMode+0x50 : mov dword ptr [eax+0x04], 0x3C
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x51, 0x40 }, // (Offset,Value)-Pair #6
        { 0x52, 0x04 }, // (Offset,Value)-Pair #7
        { 0x53, 0x3C }, // (Offset,Value)-Pair #8

        // D3DDevice_SetShaderConstantMode+0xE7 : add esi, 0x0124
        { 0xE7, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE8, 0xC6 }, // (Offset,Value)-Pair #10
        { 0xE9, 0x24 }, // (Offset,Value)-Pair #11
        { 0xEA, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 4627, 8)

        { 0x1E, 0x57 },
        { 0x3E, 0xF6 },
        { 0x5E, 0x00 },
        { 0x7E, 0x01 },
        { 0x9E, 0x00 },
        { 0xBE, 0x0C },
        { 0xDE, 0xF7 },
        { 0xFE, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 4627, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetBackBuffer+0x12 : mov eax, 1
        { 0x0F, 0xB8 }, // (Offset,Value)-Pair #6
        { 0x13, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetBackBuffer+0x15 : mov esi, [ecx+eax*4+0xXXXX]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #8
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #9
        { 0x17, 0x81 }, // (Offset,Value)-Pair #10

        // D3DDevice_GetBackBuffer+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget2, 4627, 10)

        // D3DDevice_GetRenderTarget2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetRenderTarget2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetRenderTarget2+0x06 : mov esi, [eax + 0x21B4]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB4 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

        // D3DDevice_GetRenderTarget2+0x0E : jz + 0x06
        { 0x0E, 0x74 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetRenderTarget2+0x11 : call [addr]
        { 0x11, 0xE8 }, // (Offset,Value)-Pair #9

        // D3DDevice_GetRenderTarget2+0x19 : retn
        { 0x19, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2, 4627, 10)

        // D3DDevice_GetDepthStencilSurface2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // D3DDevice_GetDepthStencilSurface2+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetDepthStencilSurface2+0x06 : mov esi, [eax + 0x21B8]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

        // D3DDevice_GetDepthStencilSurface2+0x0E : jnz +0x04
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetDepthStencilSurface2+0x15 : call [addr]
        { 0x15, 0xE8 }, // (Offset,Value)-Pair #9

        // D3DDevice_GetDepthStencilSurface2+0x1D : retn
        { 0x1D, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 4627, 11)

        // D3DDevice_GetTile+0x13 : lea esi, [ecx+eax*8+0x2260]
        { 0x13, 0x8D }, // (Offset,Value)-Pair #1
        { 0x14, 0xB4 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x16, 0x60 }, // (Offset,Value)-Pair #4
        { 0x17, 0x22 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #7

        // D3DDevice_GetTile+0x21 : pop edi; pop esi
        { 0x21, 0x5F }, // (Offset,Value)-Pair #8
        { 0x22, 0x5E }, // (Offset,Value)-Pair #9

        // D3DDevice_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x24, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 4627, 11)

        // D3DDevice_SetTile+0x06 : sub esp, 0x18
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xEC }, // (Offset,Value)-Pair #2
        { 0x08, 0x18 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTile+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 }, // (Offset,Value)-Pair #4
        { 0x16, 0x46 }, // (Offset,Value)-Pair #5
        { 0x17, 0x04 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTile+0x3D : lea edi, [edx+ecx*8+0x2260]
        { 0x3D, 0x8D }, // (Offset,Value)-Pair #7
        { 0x3E, 0xBC }, // (Offset,Value)-Pair #8
        { 0x3F, 0xCA }, // (Offset,Value)-Pair #9
        { 0x40, 0x60 }, // (Offset,Value)-Pair #10
        { 0x41, 0x22 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateIndexBuffer2, 4627, 7)

        { 0x05, 0xC0 },
        { 0x0F, 0x85 },
        { 0x13, 0xC2 },
        { 0x1A, 0x89 },
        { 0x21, 0x08 },
        { 0x28, 0x00 },
        { 0x2F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 4928, 13)

        // D3DDevice_SetPixelShaderConstant+0x1E : test eax, eax
        { 0x1E, 0x85 }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC0 }, // (Offset,Value)-Pair #2

		// D3DDevice_SetPixelShaderConstant+0x32 : mov [ebp+0x10], eax
		{ 0x32, 0x89 }, // (Offset,Value)-Pair #3
		{ 0x33, 0x45 }, // (Offset,Value)-Pair #4
		{ 0x34, 0x10 }, // (Offset,Value)-Pair #5

		// D3DDevice_SetPixelShaderConstant+0x84 : or eax, ebx
		{ 0x84, 0x0B }, // (Offset,Value)-Pair #6
		{ 0x85, 0xC3 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetPixelShaderConstant+0xBD : mov dword ptr [eax], 0x406A0
		{ 0xBD, 0xC7 }, // (Offset,Value)-Pair #8
		{ 0xBE, 0x00 }, // (Offset,Value)-Pair #9
		{ 0xBF, 0x60 }, // (Offset,Value)-Pair #10
		{ 0xC0, 0x0A }, // (Offset,Value)-Pair #11
		{ 0xC1, 0x04 }, // (Offset,Value)-Pair #12
		{ 0xC2, 0x00 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1, 4627, 11)

        // D3DDevice_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant1+0x0F : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstant1+0x27 : shl ecx, 0x04
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x28, 0xE1 }, // (Offset,Value)-Pair #7
        { 0x29, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstant1+0x5D : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant4
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant4, 4627, 12)

        // D3DDevice_SetVertexShaderConstant4+0x05 : add eax, 0x4C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x4C }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstant4+0x34 : movq mm7, qword ptr [edx+0x38]
        { 0x34, 0x0F }, // (Offset,Value)-Pair #4
        { 0x35, 0x6F }, // (Offset,Value)-Pair #5
        { 0x36, 0x7A }, // (Offset,Value)-Pair #6
        { 0x37, 0x38 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetVertexShaderConstant4+0x42 : shl ecx, 0x04
        { 0x42, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x43, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x44, 0x04 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVertexShaderConstant4+0x91 : emms
        { 0x91, 0x0F }, // (Offset,Value)-Pair #11
        { 0x92, 0x77 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInline, 4627, 12)

        // D3DDevice_SetVertexShaderConstantNotInline+0x02 : test byte ptr ds:[abs], 0x10
        { 0x02, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0x00 }, // (Offset,Value)-Pair #2
        { 0x08, 0x10 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShaderConstantNotInline+0x0E : jnz +0x15
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexShaderConstantNotInline+0x42 : push esi; push edi; mov edi, eax
        { 0x10, 0x56 }, // (Offset,Value)-Pair #6
        { 0x11, 0x57 }, // (Offset,Value)-Pair #7
        { 0x12, 0x8B }, // (Offset,Value)-Pair #8
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShaderConstantNotInline+0x21 : rep movsd
        { 0x21, 0xF3 }, // (Offset,Value)-Pair #10
        { 0x22, 0xA5 }, // (Offset,Value)-Pair #11

        // D3DDevice_SetVertexShaderConstantNotInline+0x28 : call [abs]
        { 0x28, 0xE8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4627, 10)

        // D3DDevice_SetPixelShader+0x26 : or ecx, 0x4800
        { 0x26, 0x81 }, // (Offset,Value)-Pair #1
        { 0x27, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x28, 0x00 }, // (Offset,Value)-Pair #3
        { 0x29, 0x48 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #5
        { 0x40, 0x20 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetPixelShader+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetPixelShader+0xDE : rep movsd
        { 0xDE, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xDF, 0xA5 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 4627, 13)

        // D3DDevice_CreateTexture2+0x02 : lea eax, [esp+0x20]
        { 0x02, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x20 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateTexture2+0x0E : setz dl
        { 0x0E, 0x0F }, // (Offset,Value)-Pair #3
        { 0x0F, 0x94 }, // (Offset,Value)-Pair #4
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateTexture2+0x11 : cmp eax, 5
        { 0x11, 0x83 }, // (Offset,Value)-Pair #6
        { 0x12, 0xF8 }, // (Offset,Value)-Pair #7
        { 0x13, 0x05 }, // (Offset,Value)-Pair #8

        // D3DDevice_CreateTexture2+0x53 : call [abs]
        { 0x53, 0xE8 }, // (Offset,Value)-Pair #9

        // D3DDevice_CreateTexture2+0x78 : jnz +0x0D
        { 0x78, 0x75 }, // (Offset,Value)-Pair #10
        { 0x79, 0x0D }, // (Offset,Value)-Pair #11

        // D3DDevice_CreateTexture2+0xAE : retn 0x1C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #12
        { 0xAF, 0x1C }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 4831, 12)

        // D3DDevice_CreateTexture2+0x02 : lea eax, [esp+0x20]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x07, 0x20 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateTexture2+0x0E : setz dl
        { 0x10, 0x0F }, // (Offset,Value)-Pair #3
        { 0x11, 0x94 }, // (Offset,Value)-Pair #4
        { 0x12, 0xC2 }, // (Offset,Value)-Pair #5

        // D3DDevice_CreateTexture2+0x2A : push 0x00
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #6
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #7

        // D3DDevice_CreateTexture2+0x53 : call [abs]
        { 0x57, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_CreateTexture2+0x78 : jnz +0x0D
        { 0x7C, 0x75 }, // (Offset,Value)-Pair #9
        { 0x7D, 0x0D }, // (Offset,Value)-Pair #10

        // D3DDevice_CreateTexture2+0xAE : retn 0x1C
        { 0x88, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x89, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 4627, 11)

        // D3DDevice_SetIndices+0x0F : jz +0x10
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #1
        { 0x10, 0x10 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetIndices+0x1F : jmp +0x0A
        { 0x1F, 0xEB }, // (Offset,Value)-Pair #3
        { 0x20, 0x0A }, // (Offset,Value)-Pair #4

        // D3DDevice_SetIndices+0x58 : mov [esi+0x1C], ecx
        { 0x58, 0x89 }, // (Offset,Value)-Pair #5
        { 0x59, 0x4E }, // (Offset,Value)-Pair #6
        { 0x5A, 0x1C }, // (Offset,Value)-Pair #7

        // D3DDevice_SetIndices+0x78 : mov [esi+0x038C], ebx
        { 0x78, 0x89 }, // (Offset,Value)-Pair #8
        { 0x79, 0x9E }, // (Offset,Value)-Pair #9
        { 0x7A, 0x8C }, // (Offset,Value)-Pair #10
        { 0x7B, 0x03 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture, 4627, 10)

        // D3DDevice_SetTexture+0x0F : push edi
        { 0x0F, 0x57 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetTexture+0x21 : jz +0x1E
        { 0x21, 0x74 }, // (Offset,Value)-Pair #2
        { 0x22, 0x1E }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTexture+0x33 : test ecx, 0x78FFFF
        { 0x33, 0xA9 }, // (Offset,Value)-Pair #4
        { 0x34, 0xFF }, // (Offset,Value)-Pair #5
        { 0x36, 0x78 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetTexture+0x38 : mov [edi], ecx
        { 0x38, 0x89 }, // (Offset,Value)-Pair #7
        { 0x39, 0x0F }, // (Offset,Value)-Pair #8

        // D3DDevice_SetTexture+0xD9 : jz +0x4D
        { 0xD9, 0x74 }, // (Offset,Value)-Pair #9
        { 0xDA, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTexture, 4928, 10)

        { 0x0B, 0x56 }, // (Offset,Value)-Pair #1

        { 0x21, 0x74 }, // (Offset,Value)-Pair #2
        { 0x22, 0x20 }, // (Offset,Value)-Pair #3

        { 0x33, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x34, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x36, 0xFF }, // (Offset,Value)-Pair #6

        { 0x39, 0x89 }, // (Offset,Value)-Pair #7
        { 0x3A, 0x10 }, // (Offset,Value)-Pair #8

        { 0x50, 0x75 }, // (Offset,Value)-Pair #9
        { 0x51, 0x47 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4627, 12)

        // D3DDevice_GetDisplayMode+0x01 : mov esi, ds:[*]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #1
        { 0x02, 0x35 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetDisplayMode+0x18 : shr ecx, 0x14
        { 0x18, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x19, 0xE9 }, // (Offset,Value)-Pair #4
        { 0x1A, 0x14 }, // (Offset,Value)-Pair #5

        // D3DDevice_GetDisplayMode+0x50 : shr eax, 0x0C
        { 0x50, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x51, 0xE8 }, // (Offset,Value)-Pair #7
        { 0x52, 0x0C }, // (Offset,Value)-Pair #8

        // D3DDevice_GetDisplayMode+0x6A : or edx, eax
        { 0x6A, 0x0B }, // (Offset,Value)-Pair #9
        { 0x6B, 0xD0 }, // (Offset,Value)-Pair #10

        // D3DDevice_GetDisplayMode+0x7F : jmp +0x1C
        { 0x7F, 0xEB }, // (Offset,Value)-Pair #11
        { 0x80, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 4627, 10)

        // D3DDevice_RunPushBuffer+0x3C : mov edx, [esi+30h]
        { 0x3C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x3D, 0x56 }, // (Offset,Value)-Pair #2
        { 0x3E, 0x30 }, // (Offset,Value)-Pair #3

        // D3DDevice_RunPushBuffer+0xCA : mov edi, ebp
        { 0xCA, 0x8B }, // (Offset,Value)-Pair #4
        { 0xCB, 0xFD }, // (Offset,Value)-Pair #5

        // D3DDevice_RunPushBuffer+0xDC : jmp +0x12
        { 0xDC, 0xEB }, // (Offset,Value)-Pair #6
        { 0xDD, 0x12 }, // (Offset,Value)-Pair #7

        // D3DDevice_RunPushBuffer+0xED : lea edi, [eax+4]
        { 0xED, 0x8D }, // (Offset,Value)-Pair #8
        { 0xEE, 0x78 }, // (Offset,Value)-Pair #9
        { 0xEF, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 4627, 14)

        // D3DDevice_Begin+0x19 : shl edx, 6
        { 0x19, 0xD1 }, // (Offset,Value)-Pair #7
        { 0x1A, 0xE8 }, // (Offset,Value)-Pair #8

        // D3DDevice_Begin+0x25 : mov dword ptr [eax], 0x417FC
        { 0x25, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x26, 0x00 }, // (Offset,Value)-Pair #2
        { 0x27, 0xFC }, // (Offset,Value)-Pair #3
        { 0x28, 0x17 }, // (Offset,Value)-Pair #4
        { 0x29, 0x04 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_Begin+0x33 : or dword ptr [esi+8], 0x800
        { 0x33, 0x81 }, // (Offset,Value)-Pair #9
        { 0x34, 0x4E }, // (Offset,Value)-Pair #10
        { 0x35, 0x08 }, // (Offset,Value)-Pair #11
        { 0x37, 0x08 }, // (Offset,Value)-Pair #12

        // D3DDevice_Begin+0x3B : retn 0x08
        { 0x3B, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 4627, 14)

        // D3DDevice_SetVertexData2f+0x20 : lea edx, ds:abs[ecx*8]
        { 0x20, 0x8D }, // (Offset,Value)-Pair #1
        { 0x21, 0x14 }, // (Offset,Value)-Pair #2
        { 0x22, 0xCD }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexData2f+0x2B : mov [eax], edx; mov edx, [esp+0x10]
        { 0x2B, 0x89 }, // (Offset,Value)-Pair #4
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #6
        { 0x2E, 0x54 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData2f+0x37 : add eax, 0x0C
        { 0x37, 0x83 }, // (Offset,Value)-Pair #10
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12

        // D3DDevice_SetVertexData2f+0x3D : retn 0x0C
        { 0x3D, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3E, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 4627, 11)

        // D3DDevice_SetVertexData4f+0x20 : cmp ecx, -1; jnz +0x07
        { 0x20, 0x83 }, // (Offset,Value)-Pair #1
        { 0x21, 0xF9 }, // (Offset,Value)-Pair #2
        { 0x22, 0xFF }, // (Offset,Value)-Pair #3
        { 0x23, 0x75 }, // (Offset,Value)-Pair #4
        { 0x24, 0x07 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetVertexData4f+0x2C : add ecx, 0x01A0
        { 0x2C, 0x81 }, // (Offset,Value)-Pair #6
        { 0x2D, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x2E, 0xA0 }, // (Offset,Value)-Pair #8
        { 0x2F, 0x01 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexData4f+0x5F : retn 0x14
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x60, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 4627, 12)

        // D3DDevice_End+0x1C : mov dword ptr [eax], 0x417FC
        { 0x1C, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x00 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xFC }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4
        { 0x20, 0x04 }, // (Offset,Value)-Pair #5
        { 0x21, 0x00 }, // (Offset,Value)-Pair #6

        // D3DDevice_End+0x31 : test ah, 0x10
        { 0x31, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x32, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x33, 0x10 }, // (Offset,Value)-Pair #9

        // D3DDevice_End+0x34 : jz +0x07
        { 0x34, 0x74 }, // (Offset,Value)-Pair #10
        { 0x35, 0x07 }, // (Offset,Value)-Pair #11

        // D3DDevice_End+0x45 : retn
        { 0x45, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 4627, 9)

        { 0x0C, 0x9D },
        { 0x1A, 0xB6 },
        { 0x1B, 0x47 },
        { 0x1C, 0x0D },
        { 0x1D, 0x8A },
        { 0x1E, 0x88 },
        { 0x54, 0x05 },
        { 0x81, 0xB6 },
        { 0xF8, 0x95 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4627, 11)

        // D3DDevice_Swap+0x2A : mov ebx, 5
        { 0x2A, 0xBB }, // (Offset,Value)-Pair #1
        { 0x2B, 0x05 }, // (Offset,Value)-Pair #2

        // D3DDevice_Swap+0x37 : test bl, 3
        { 0x37, 0xF6 }, // (Offset,Value)-Pair #3
        { 0x38, 0xC3 }, // (Offset,Value)-Pair #4
        { 0x39, 0x03 }, // (Offset,Value)-Pair #5

        // D3DDevice_Swap+0x60 : inc dword ptr [esi+0x2C10]
        { 0x60, 0xFF }, // (Offset,Value)-Pair #6
        { 0x61, 0x86 }, // (Offset,Value)-Pair #7
        { 0x62, 0x10 }, // (Offset,Value)-Pair #8
        { 0x63, 0x2C }, // (Offset,Value)-Pair #9

        // D3DDevice_Swap+0xD8 : retn 4
        { 0xD8, 0xC2 }, // (Offset,Value)-Pair #10
        { 0xD9, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVertexBuffer2, 4627, 13)

        // D3DDevice_CreateVertexBuffer2+0x03 : push 0x40
        { 0x03, 0x6A }, // (Offset,Value)-Pair #1
        { 0x04, 0x40 }, // (Offset,Value)-Pair #2

        // D3DDevice_CreateVertexBuffer2+0x10 : mov eax, [esp+8]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #3
        { 0x11, 0x44 }, // (Offset,Value)-Pair #4
        { 0x12, 0x24 }, // (Offset,Value)-Pair #5
        { 0x13, 0x08 }, // (Offset,Value)-Pair #6

        // D3DDevice_CreateVertexBuffer2+0x41 : mov dword ptr [esi], 0x01000001
        { 0x41, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x42, 0x06 }, // (Offset,Value)-Pair #8
        { 0x43, 0x01 }, // (Offset,Value)-Pair #9
        { 0x44, 0x00 }, // (Offset,Value)-Pair #10
        { 0x46, 0x01 }, // (Offset,Value)-Pair #11

        // D3DDevice_CreateVertexBuffer2+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4627, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2BA8], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x12, 0x2B }, // (Offset,Value)-Pair #4

        // D3DDevice_UpdateOverlay+0x8E : and eax, 0xFFFFFFFE
        { 0x8E, 0x83 }, // (Offset,Value)-Pair #5
        { 0x8F, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x90, 0xFE }, // (Offset,Value)-Pair #7

        // D3DDevice_UpdateOverlay+0xAC : mov [esi+0x8930], eax
        { 0xAC, 0x89 }, // (Offset,Value)-Pair #8
        { 0xAD, 0x86 }, // (Offset,Value)-Pair #9
        { 0xAE, 0x30 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetOverlayUpdateStatus, 4627, 12)

        // D3DDevice_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x2BA8]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x07, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x08, 0x2B }, // (Offset,Value)-Pair #4

        // D3DDevice_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x2580]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x25 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // D3DDevice_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4627, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2554
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x54 }, // (Offset,Value)-Pair #8
        { 0x19, 0x25 }, // (Offset,Value)-Pair #9

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4627, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x2550], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x25 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 4627, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x0A90]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x90 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x0A }, // (Offset,Value)-Pair #6

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4627, 1+10,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x19, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x14 : shl eax, 0x07
        { 0x14, 0xC1 }, // (Offset,Value)-Pair #1
        { 0x15, 0xE0 }, // (Offset,Value)-Pair #2
        { 0x16, 0x07 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetTextureState_TexCoordIndex+0x26 : cmp eax, ecx
        { 0x26, 0x3B }, // (Offset,Value)-Pair #4
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetTextureState_TexCoordIndex+0x71 : mov ebp, 0x2400
        { 0x71, 0xBD }, // (Offset,Value)-Pair #6
        { 0x73, 0x24 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetTextureState_TexCoordIndex+0xAA : shl ebx, 0x04
        { 0xAA, 0xC1 }, // (Offset,Value)-Pair #8
        { 0xAB, 0xE3 }, // (Offset,Value)-Pair #9
        { 0xAC, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 4627, 8)

        // D3DDevice_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_NormalizeNormals+0x2E : or [addr], 0x200
        { 0x2E, 0x81 }, // (Offset,Value)-Pair #3
        { 0x2F, 0x0D }, // (Offset,Value)-Pair #4
        { 0x34, 0x00 }, // (Offset,Value)-Pair #5
        { 0x35, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_NormalizeNormals+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x40, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 4627, 10)

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x1C : mov ecx, [esp+0x08]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x1D, 0x4C }, // (Offset,Value)-Pair #6
        { 0x1E, 0x24 }, // (Offset,Value)-Pair #7
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x38 : retn 0x04
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4627, 9)

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x0D : mov eax, [edi+0x21B4]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0E, 0x87 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x10, 0x21 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x56 : shl esi, 0x05
        { 0x56, 0xC1 }, // (Offset,Value)-Pair #5
        { 0x57, 0xE6 }, // (Offset,Value)-Pair #6
        { 0x58, 0x05 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x74 : retn 0x04
        { 0x74, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x75, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4627, 9)

        { 0x08, 0x06 },
        { 0x1D, 0xB8 },
        { 0x48, 0xC0 },
        { 0x49, 0x10 },
        { 0x4A, 0x89 },
        { 0x4B, 0x06 },
        { 0x4C, 0xA1 },
        { 0x5A, 0x74 },
        { 0x60, 0x2A },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 4627, 11)

        // D3DDevice_SetRenderState_StencilEnable+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_StencilEnable+0x35 : jz +0x19
        { 0x35, 0x74 }, // (Offset,Value)-Pair #3
        { 0x36, 0x19 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetRenderState_StencilEnable+0x56 : mov dword ptr [eax], 0x00041D84
        { 0x56, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x57, 0x00 }, // (Offset,Value)-Pair #6
        { 0x58, 0x84 }, // (Offset,Value)-Pair #7
        { 0x59, 0x1D }, // (Offset,Value)-Pair #8
        { 0x5A, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetRenderState_StencilEnable+0x8A : retn 0x04
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x8B, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, 12)

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x13 : call [abs]
        { 0x13, 0xE8 }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x27 : shl edi, 0x10
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x28, 0xE7 }, // (Offset,Value)-Pair #4
        { 0x29, 0x10 }, // (Offset,Value)-Pair #5

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x2E : jns +0x0C
        { 0x2E, 0x79 }, // (Offset,Value)-Pair #6
        { 0x2F, 0x0C }, // (Offset,Value)-Pair #7

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x39 : or edi, 0x01
        { 0x39, 0x83 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xCF }, // (Offset,Value)-Pair #9
        { 0x3B, 0x01 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x6F : retn 0x04
        { 0x6F, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x70, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 4627, 10)

        // D3DDevice_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // D3DDevice_DrawVerticesUP+0x2F : mov edx, [edi+0x804]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x97 }, // (Offset,Value)-Pair #5
        { 0x31, 0x04 }, // (Offset,Value)-Pair #6
        { 0x32, 0x08 }, // (Offset,Value)-Pair #7

        // D3DDevice_DrawVerticesUP+0x8A : mov [ebp+arg_C], eax
        { 0x8A, 0x89 }, // (Offset,Value)-Pair #8
        { 0x8B, 0x45 }, // (Offset,Value)-Pair #9
        { 0x8C, 0x14 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 4627, 11)

        // D3DDevice_DrawIndexedVertices+0x0E : mov eax, [esi+0x1C]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x46 }, // (Offset,Value)-Pair #2
        { 0x10, 0x1C }, // (Offset,Value)-Pair #3

        // D3DDevice_DrawIndexedVertices+0x26 : push 0x0209
        { 0x26, 0x68 }, // (Offset,Value)-Pair #4
        { 0x27, 0x09 }, // (Offset,Value)-Pair #5
        { 0x28, 0x02 }, // (Offset,Value)-Pair #6

        // D3DDevice_DrawIndexedVertices+0x66 : dec eax
        { 0x66, 0x48 }, // (Offset,Value)-Pair #7

        // D3DDevice_DrawIndexedVertices+0xFB : prefetchnta byte ptr [esi+0x3C]
        { 0xFB, 0x0F }, // (Offset,Value)-Pair #8
        { 0xFC, 0x18 }, // (Offset,Value)-Pair #9
        { 0xFD, 0x46 }, // (Offset,Value)-Pair #10
        { 0xFE, 0x3C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4627, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0AB0
        { 0x0C, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xC7 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x0A }, // (Offset,Value)-Pair #4

        // D3DDevice_SetMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 }, // (Offset,Value)-Pair #5
        { 0x13, 0x11 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 }, // (Offset,Value)-Pair #7
        { 0x20, 0xC9 }, // (Offset,Value)-Pair #8
        { 0x21, 0x00 }, // (Offset,Value)-Pair #9
        { 0x22, 0x10 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2E, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock2, 4627, 12)

        // D3DVertexBuffer_Lock2B+0x00 : push ebx
        { 0x00, 0x53 }, // (Offset,Value)-Pair #1

        // D3DVertexBuffer_Lock2+0x01 : mov bl, [esp+0x0C]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #2
        { 0x02, 0x5C }, // (Offset,Value)-Pair #3
        { 0x03, 0x24 }, // (Offset,Value)-Pair #4
        { 0x04, 0x0C }, // (Offset,Value)-Pair #5

        // D3DVertexBuffer_Lock2B+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #7

        // D3DVertexBuffer_Lock2+0x3E : mov eax, [esi+4]
        { 0x3E, 0x8B }, // (Offset,Value)-Pair #8
        { 0x3F, 0x46 }, // (Offset,Value)-Pair #9
        { 0x40, 0x04 }, // (Offset,Value)-Pair #10

        // D3DVertexBuffer_Lock2+0x48 : retn 0x08
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x49, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DResource_Release
// ******************************************************************
OOVPA_NO_XREF(D3DResource_Release, 4627, 11)

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

        // D3DResource_Release+0x3D : pop edi
        { 0x3D, 0x5F }, // (Offset,Value)-Pair #9

        // D3DResource_Release+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DResource_GetType
// ******************************************************************
OOVPA_NO_XREF(D3DResource_GetType, 4627, 7)

        { 0x10, 0x77 },
        { 0x22, 0x83 },
        { 0x34, 0x00 },
        { 0x46, 0x74 },
        { 0x58, 0x00 },
        { 0x6A, 0x74 },
        { 0x7C, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DVolumeTexture_LockBox
// ******************************************************************
OOVPA_NO_XREF(D3DVolumeTexture_LockBox, 4627, 15)

        // D3DVolumeTexture_LockBox+0x00 : mov eax, [esp+0x14]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x14 }, // (Offset,Value)-Pair #4

        // D3DVolumeTexture_LockBox+0x04 : mov ecx, [esp+0x10]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0C }, // (Offset,Value)-Pair #8

        // D3DVolumeTexture_LockBox+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9

        // D3DVolumeTexture_LockBox+0x11 : push ecx; push edx; push eax
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x12, 0x52 }, // (Offset,Value)-Pair #11
        { 0x13, 0x50 }, // (Offset,Value)-Pair #12

        // D3DVolumeTexture_LockBox+0x18 : call [addr]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #13

        // D3DVolumeTexture_LockBox+0x1D : retn 0x14
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x1E, 0x14 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * D3DSurface_LockRect
// ******************************************************************
OOVPA_NO_XREF(D3DSurface_LockRect, 4627, 17)

        // D3DSurface_LockRect+0x00 : mov eax, [esp+0x10]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

        // D3DSurface_LockRect+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x04 }, // (Offset,Value)-Pair #8

        // D3DSurface_LockRect+0x08 : push esi; mov esi, [esp+0x0C]; push eax
        { 0x08, 0x56 }, // (Offset,Value)-Pair #9
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x74 }, // (Offset,Value)-Pair #11
        { 0x0B, 0x24 }, // (Offset,Value)-Pair #12
        { 0x0C, 0x0C }, // (Offset,Value)-Pair #13
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #14

        // D3DSurface_LockRect+0x17 : call [addr]
        { 0x17, 0xE8 }, // (Offset,Value)-Pair #15

        // D3DSurface_LockRect+0x1D : retn 0x10
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1E, 0x10 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 4627, 10)

        { 0x10, 0x0D },
        { 0x48, 0x08 },
        { 0x49, 0x02 },
        { 0x4A, 0x00 },
        { 0x4B, 0x00 },
        { 0x4C, 0x00 },
        { 0x4D, 0xA1 },
        { 0x58, 0x53 },
        { 0x63, 0x80 },
        { 0xAE, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetSurfaceLevel2
// ******************************************************************
OOVPA_NO_XREF(D3DTexture_GetSurfaceLevel2, 4627, 12)

        // D3DTexture_GetSurfaceLevel2+0x00 : sub esp, 0x0C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x0C }, // (Offset,Value)-Pair #3

        // D3DTexture_GetSurfaceLevel2+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x14 }, // (Offset,Value)-Pair #5

        // D3DTexture_GetSurfaceLevel2+0x08 : lea eax, [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #6
        { 0x0B, 0x14 }, // (Offset,Value)-Pair #7

        // D3DTexture_GetSurfaceLevel2+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #9

        // D3DTexture_GetSurfaceLevel2+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D }, // (Offset,Value)-Pair #10
        { 0x1E, 0x18 }, // (Offset,Value)-Pair #11

        // D3DTexture_GetSurfaceLevel2+0x3E : call [abs]
        { 0x3E, 0xE8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewportOffsetAndScale@8
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewportOffsetAndScale, 4627, 8)

        { 0x1E, 0xD8 },
        { 0x40, 0xD8 },
        { 0x5E, 0xC9 },
        { 0x7E, 0x05 },
        { 0x9E, 0x0A },
        { 0xBE, 0x00 },
        { 0xDE, 0x14 },
        { 0xFE, 0xE0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShaderDirect
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShaderDirect, 4627, 7)

        { 0x05, 0x85 },
        { 0x0C, 0x00 },
        { 0x14, 0xF3 },
        { 0x1D, 0x83 },
        { 0x21, 0x4C },
        { 0x29, 0x5E },
        { 0x2F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 4627, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xD8 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderConstant, 4627, 7)

        { 0x05, 0x15 },
        { 0x0C, 0x24 },
        { 0x13, 0xE0 },
        { 0x1A, 0x8D },
        { 0x21, 0x8B },
        { 0x28, 0x8B },
        { 0x2F, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInputDirect
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInputDirect, 4627, 7)

        { 0x05, 0x85 },
        { 0x0C, 0x00 },
        { 0x14, 0xF3 },
        { 0x1D, 0x83 },
        { 0x21, 0x54 },
        { 0x29, 0x5E },
        { 0x2F, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderInput, 4627, 7)

        { 0x0B, 0x74 },
        { 0x18, 0x04 },
        { 0x25, 0x57 },
        { 0x32, 0x00 },
        { 0x3F, 0x00 },
        { 0x4C, 0x03 },
        { 0x59, 0xD0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunVertexStateShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunVertexStateShader, 4627, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0xC0 },
        { 0x2A, 0x40 },
        { 0x35, 0xF4 },
        { 0x40, 0xC7 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_XREF(D3DDevice_EndPush, 4627, 12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE ), // 1 

        { 0x00, 0x8B }, // 2
        { 0x01, 0x44 }, // 3
        { 0x02, 0x24 }, // 4
        { 0x03, 0x04 }, // 5
        { 0x04, 0x8B }, // 6
        { 0x05, 0x0D }, // 7
        { 0x0A, 0x89 }, // 8
        { 0x0B, 0x01 }, // 9
        { 0x0C, 0xC2 }, // 10
        { 0x0D, 0x04 }, // 11
        { 0x0E, 0x00 }, // 12
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShader, 4627, 7)

        { 0x09, 0x08 },
        { 0x14, 0x75 },
        { 0x21, 0x8B },
        { 0x2A, 0xE8 },
        { 0x35, 0x9C },
        { 0x40, 0x14 },
        { 0x4B, 0x8D },
OOVPA_END;

// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 4627, 8)

        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x19, 0x75 },
        { 0x1A, 0x19 },
        { 0x1B, 0x56 },
        { 0x1C, 0x8D },
        { 0x25, 0xE0 },
        { 0x32, 0x5F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 4627, 8)

        { 0x0E, 0x49 },
        { 0x1E, 0x00 },
        { 0x2E, 0x72 },
        { 0x3E, 0x1E },
        { 0x4E, 0x8B },
        { 0x5E, 0x05 },
        { 0x6E, 0x89 },
        { 0x7E, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderType@8
// ******************************************************************
#define D3DDevice_GetVertexShaderType_4627 D3DDevice_GetVertexShaderType_3925

// ******************************************************************
// * D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderDeclaration, 4627, 8)

        { 0x1E, 0xC2 },
        { 0x3E, 0x00 },
        { 0x5E, 0x00 },
        { 0x7E, 0x02 },
        { 0x9E, 0x24 },
        { 0xBE, 0x2B },
        { 0xDE, 0x9A },
        { 0xFE, 0x10 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderFunction
// ******************************************************************
#define D3DDevice_GetVertexShaderFunction_4627 D3DDevice_GetVertexShaderFunction_3925


// ******************************************************************
// * D3DDevice_GetStreamSource2 (Maybe same in older versions)
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetStreamSource2, 4627, 7)

        { 0x06, 0x34 },
        { 0x12, 0x85 },
        { 0x16, 0x57 },
        { 0x22, 0x8B },
        { 0x26, 0x89 },
        { 0x2E, 0x00 },
        { 0x36, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4627, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0x80 }, // (Offset,Value)-Pair #6
        { 0x14, 0x03 }, // (Offset,Value)-Pair #7

        // D3DDevice_SetVertexShader+0x95 : retn 0x04
        { 0x95, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x96, 0x04 }, // (Offset,Value)-Pair #9

        // D3DDevice_SetVertexShader+0xC2 : mov dword ptr [eax], 0x4194C
        { 0xC2, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xC3, 0x00 }, // (Offset,Value)-Pair #11
        { 0xC4, 0x4C }, // (Offset,Value)-Pair #12
        { 0xC5, 0x19 }, // (Offset,Value)-Pair #13
        { 0xC6, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 4627, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x0C },
        { 0x21, 0x05 },
        { 0x28, 0x00 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 4627, 8)

        // D3DDevice_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0xXXXX]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2

        // D3DDevice_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // D3DDevice_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // D3DDevice_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x63, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale, 4627, 6)

        { 0x1E, 0x24 },
        { 0x3E, 0x06 },
//      { 0x62, 0xEC },
        { 0x7E, 0x8B },
        { 0x9E, 0xBF },
        { 0xBE, 0x89 },
        { 0xDE, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 4627, 18)

		{ 0x00, 0x8B },
		{ 0x01, 0x81 },
		{ 0x02, 0xBC },
		{ 0x03, 0x01 },
		{ 0x04, 0x00 },
		{ 0x05, 0x00 },
		{ 0x06, 0x83 },
		{ 0x07, 0xE0 },
		{ 0x08, 0x01 },
		{ 0x09, 0x83 },
		{ 0x0A, 0xC0 },
		{ 0x0B, 0x1F },
		{ 0x0C, 0x8D },
		{ 0x0D, 0x04 },
		{ 0x0E, 0x40 },
		{ 0x0F, 0x8B },
		{ 0x10, 0x04 },
		{ 0x11, 0x81 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderProgram, 4627, 10)

		{ 0x00, 0x8B },
                { 0x01, 0x54 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },
		{ 0x04, 0x85 },
		{ 0x05, 0xD2 },
		{ 0x29, 0x89 },
		{ 0x2A, 0x4C },
		{ 0x2B, 0x24 },
		{ 0x2C, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 4627, 7)

		{ 0x0B, 0x8B },
		{ 0x0C, 0x4C },
		{ 0x0D, 0x24 },
		{ 0x0E, 0x04 },
		{ 0x1E, 0x56 },
		{ 0x25, 0x33 },
		{ 0x26, 0xD2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateStateBlock, 4627, 11)

		{ 0x0C, 0x57 },
		{ 0x1C, 0xF3 },
		{ 0x1D, 0xAB },
		{ 0x41, 0x81 },
		{ 0x42, 0xC1 },
		{ 0x43, 0x90 },
		{ 0x44, 0x00 },
		{ 0x45, 0x00 },
		{ 0x46, 0x00 },
		{ 0x8B, 0x33 },
		{ 0x8C, 0xC0 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback, 4627, 7)

        { 0x0D, 0x0E },
        { 0x1C, 0x8B },
        { 0x2B, 0x48 },
        { 0x3A, 0x0C },
        { 0x49, 0x89 },
        { 0x58, 0x10 },
        { 0x67, 0x07 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawRectPatch
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawRectPatch, 4627, 8)

        { 0x1E, 0x6C },
        { 0x3E, 0x3C },
        { 0x5E, 0x54 },
        { 0x80, 0x89 },
        { 0x9E, 0xF0 },
        { 0xBE, 0x00 },
        { 0xDE, 0xD8 },
        { 0xFE, 0xD9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 4627, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x30 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
/* blueshogun96 8/7/08
   for MGS2 :)*/
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4721, 10)

        // D3DDevice_SetPixelShader+0x26 : or ecx, 0x4800
        { 0x26, 0x81 }, // (Offset,Value)-Pair #1
        { 0x27, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x28, 0x00 }, // (Offset,Value)-Pair #3
        { 0x29, 0x48 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #5
        { 0x40, 0x20 }, // (Offset,Value)-Pair #6

        // D3DDevice_SetPixelShader+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #8

        // D3DDevice_SetPixelShader+0xDE : rep movsd
        { 0xF4, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xF5, 0xA5 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors@12
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors, 4627, 8)

        { 0x1E, 0x57 },
        { 0x3E, 0x44 },
        { 0x5E, 0x5C },
        { 0x7E, 0x24 },
        { 0x9E, 0xBE },
        { 0xBE, 0x04 },
        { 0xDE, 0x24 },
        { 0xFE, 0x4C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPalette@8
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4627, 7)

        { 0x10, 0x48 },
        { 0x22, 0x0B },
        { 0x34, 0x00 },
        { 0x46, 0x9C },
        { 0x58, 0xC5 },
        { 0x6E, 0x8B },
        { 0x7C, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask, 4627, 8)

        { 0x0A, 0xA3 },
        { 0x12, 0x56 },
        { 0x1C, 0x8B },
        { 0x2A, 0x85 },
        { 0x30, 0x01 },
        { 0x3D, 0xC7 },
        { 0x44, 0x78 },
        { 0x4E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 4627, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 4627, 13)

		// D3D::CDevice::KickOff+0x0B : mov edx, [ecx+0x35C]
		{ 0x0B, 0x8B },
		{ 0x0C, 0x91 },
		{ 0x0D, 0x5C },
		{ 0x0E, 0x03 },
		{ 0x0F, 0x00 },
		{ 0x10, 0x00 },

		// D3D::CDevice::KickOff+0x1F : test ch, 0x20
		{ 0x1F, 0xF6 },
		{ 0x20, 0xC5 },
		{ 0x21, 0x20 },

		// D3D::CDevice::KickOff+0x2F : sfence
		{ 0x2F, 0x0F },
		{ 0x30, 0xAE },
		{ 0x31, 0xF8 },

		// D3D::CDevice::KickOff+0xE2 : retn
		{ 0xE2, 0xC3 }
OOVPA_END;

// ******************************************************************
// * D3DDevice_KickPushBuffer
// ******************************************************************
OOVPA_XREF(D3DDevice_KickPushBuffer, 4627, 4,

    XRefNoSaveIndex,
    XRefOne)

	XREF_ENTRY( 0x07, XREF_D3D_KickOff ),

        { 0x00, 0x8B },
        { 0x01, 0x0D },
        { 0x06, 0xE9 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8::GetTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTexture2, 4627, 8)

        { 0x03, 0x04 },
        { 0x0A, 0x56 },
        { 0x0D, 0x81 },
        { 0x0E, 0x38 },
        { 0x12, 0x8B },
        { 0x17, 0x06 },
        { 0x1E, 0x8B },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateVB
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateVB, 4627, 12)

		// D3DDevice_SetStateVB+0x00 : sub esp, 0xC
		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x0C },

		// D3DDevice_SetStateVB+0x0D : and eax, 0xFFFFFFAF
		{ 0x0D, 0x83 },
		{ 0x0E, 0xE0 },
		{ 0x0F, 0xAF },

		// D3DDevice_SetStateVB+0x10 : test ebx, 0x3FFFFF8F
		{ 0x10, 0xF7 },
		{ 0x11, 0xC3 },
		{ 0x12, 0x8F },
		{ 0x13, 0xFF },
		{ 0x14, 0xFF },
		{ 0x15, 0x3F },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::SetStateUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStateUP, 4627, 11)

		// D3DDevice_SetStateUP+0x05 : sub esp, 0x14
		{ 0x05, 0x83 },
		{ 0x06, 0xEC },
		{ 0x07, 0x14 },

		// D3DDevice_SetStateUP+0x08 : test eax, 0x3FFFFF8F
		{ 0x08, 0xA9 },
		{ 0x09, 0x8F },
		{ 0x0A, 0xFF },
		{ 0x0B, 0xFF },
		{ 0x0C, 0x3F },

		// D3DDevice_SetStateUP+0x27 : or eax, 0x50
		{ 0x27, 0x83 },
		{ 0x28, 0xC8 },
		{ 0x29, 0x50 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 4627, 10)

        // D3DDevice_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // D3DDevice_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x784]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x97 }, // (Offset,Value)-Pair #6
        { 0x5E, 0x84 }, // (Offset,Value)-Pair #7
        { 0x5F, 0x07 }, // (Offset,Value)-Pair #8

        // D3DDevice_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStipple
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStipple, 4627, 7)

        { 0x08, 0x03 },
        { 0x15, 0xF6 },
        { 0x1C, 0x10 },
        { 0x26, 0x20 },
        { 0x30, 0x80 },
        { 0x3A, 0x00 },
        { 0x44, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 4627, 8)

        { 0x21, 0x8B },
        { 0x3C, 0x8D },
        { 0x5B, 0x74 },
        { 0x7A, 0x42 },
        { 0x99, 0x56 },
        { 0xB8, 0x00 },
        { 0xD7, 0x8B },
        { 0xF6, 0xF9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 4928, 14)

        // D3DDevice_PersistDisplay+0x23 : mov eax, [ebx+0x500]
		{ 0x23, 0x8B },
		{ 0x24, 0x83 },
		{ 0x25, 0x00 },
		{ 0x26, 0x05 },
		{ 0x27, 0x00 },
		{ 0x28, 0x00 },
		// D3DDevice_PersistDisplay+0x39 : lea ebp, [ebx+0x23C0]
		{ 0x39, 0x8D },
		{ 0x3A, 0xAB },
		{ 0x3B, 0xC0 },
		{ 0x3C, 0x23 },
		{ 0x3D, 0x00 },
		{ 0x3E, 0x00 },
		// D3DDevice_PersistDisplay+0x72 : shl edx, cl
		{ 0x72, 0xD3 },
		{ 0x73, 0xE2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
/* See the comment for the 4134 OOVPA signature for previous notes */
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4928, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x254C], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x4C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x25 }, // (Offset,Value)-Pair #10

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 4627, 8)

        { 0x09, 0x89 },
        { 0x0F, 0x8B },
        { 0x13, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x75 },
        { 0x21, 0x00 },
        { 0x28, 0x00 },
        { 0x31, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4627, 8)

        { 0x09, 0x89 },
        { 0x0F, 0x8B },
        { 0x13, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x74 },
        { 0x21, 0x00 },
        { 0x28, 0x00 },
        { 0x31, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBlock, 4627, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndStateBlock, 4627, 6)

        { 0x00, 0xA1 },
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x08 },
        { 0x08, 0xDF },
        { 0x09, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_ApplyStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_ApplyStateBlock, 4627, 8)

        { 0x1E, 0xC6 },
        { 0x40, 0x83 },
        { 0x5E, 0x04 },
        { 0x7E, 0x50 },
        { 0xA0, 0x83 },
        { 0xBE, 0x51 },
        { 0xDE, 0xE9 },
        { 0xFE, 0x33 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4627, 8)

        { 0x1D, 0x5E },
        { 0x3C, 0x00 },
        { 0x5B, 0x72 },
        { 0x7A, 0x00 },
        { 0x99, 0x01 },
        { 0xBC, 0x8B },
        { 0xD7, 0x89 },
        { 0xF9, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 4627, 6,

    XREF_D3D_BLOCKONTIME,
    XRefZero)

        { 0x09, 0x34 },
        { 0x27, 0x07 },
        { 0x55, 0x7E },
        { 0x7B, 0x58 },
        { 0xE3, 0x80 },
        { 0xF5, 0x30 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_XREF(D3DDevice_BlockOnFence, 4627, 8,

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
// * Lock3DSurface
// ******************************************************************
OOVPA_NO_XREF(Lock3DSurface, 4627, 11)

		// D3D::PixelJar::Lock3DSurface + 0x08: test bl, 0x20
        { 0x08, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x09, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x0A, 0x20 }, // (Offset,Value)-Pair #3

		// D3D::PixelJar::Lock3DSurface + 0x40: test bl, 0x40
        { 0x40, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x41, 0xC3 }, // (Offset,Value)-Pair #5
        { 0x42, 0x40 }, // (Offset,Value)-Pair #6

		// D3D::PixelJar::Lock3DSurface + 0x6D: and edx, 0x3C
        { 0x6D, 0x83 }, // (Offset,Value)-Pair #7
        { 0x6E, 0xE2 }, // (Offset,Value)-Pair #8
        { 0x6F, 0x3C }, // (Offset,Value)-Pair #9

		// D3D::PixelJar::Lock3DSurface + 0x99: ret 0x14
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x9A, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 4627, 9)

        { 0x04, 0x8B },
        { 0x0A, 0x24 },
        { 0x0E, 0x00 },
        { 0x10, 0x52 },
        { 0x16, 0x8B },
        { 0x1C, 0x85 },
        { 0x22, 0x01 },
        { 0x28, 0x07 },
        { 0x2E, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DCubeTexture_GetCubeMapSurface2
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface2, 4627, 7)

        { 0x08, 0x8D },
        { 0x12, 0x8D },
        { 0x1C, 0x44 },
        { 0x26, 0x24 },
        { 0x31, 0x8B },
        { 0x3A, 0x44 },
        { 0x46, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4627, 7)

        { 0x0D, 0x56 },
        { 0x14, 0x8B },
        { 0x1F, 0x95 },
        { 0x2A, 0x00 },
        { 0x37, 0x33 },
        { 0x44, 0x01 },
        { 0x4D, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 4627, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x70 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreatePalette2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePalette2, 4627, 8)

        { 0x0A, 0x8B },
        { 0x16, 0x74 },
        { 0x22, 0x04 },
        { 0x2E, 0x50 },
        { 0x3A, 0xE8 },
        { 0x46, 0xC1 },
        { 0x52, 0xFF },
        { 0x5E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock2
// ******************************************************************
OOVPA_NO_XREF(D3DPalette_Lock2, 4627, 7)

        { 0x02, 0x24 },
        { 0x06, 0x8B },
        { 0x0A, 0x75 },
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4627, 7)

        { 0x07, 0xC9 },
        { 0x10, 0x24 },
        { 0x1A, 0x74 },
        { 0x22, 0x6A },
        { 0x2E, 0xC7 },
        { 0x34, 0x01 },
        { 0x3E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 4627, 7)

        { 0x0B, 0xCE },
        { 0x1B, 0x89 },
        { 0x25, 0x00 },
        { 0x32, 0x06 },
        { 0x3F, 0x03 },
        { 0x4C, 0x04 },
        { 0x59, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 4627, 7)

        { 0x11, 0x8D },
        { 0x22, 0x50 },
        { 0x34, 0x0C },
        { 0x46, 0x08 },
        { 0x58, 0x00 },
        { 0x6D, 0x8B },
        { 0x7C, 0x88 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPersistedSurface2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPersistedSurface2, 4627, 6)

        { 0x00, 0xFF },
        { 0x01, 0x25 },
        { 0x02, 0x04 },
        { 0x03, 0xC1 },
        { 0x04, 0x3A },
        { 0x05, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView, 4627, 7)

        { 0x15, 0xFF },
        { 0x2D, 0x81 },
        { 0x43, 0x3B },
        { 0x5A, 0xC1 },
        { 0x71, 0x53 },
        { 0x8A, 0x8B },
        { 0x9F, 0x30 },
OOVPA_END;

// ******************************************************************
// * D3D::LazySetPointParams
// ******************************************************************
OOVPA_NO_XREF(D3D_LazySetPointParams, 4627, 7)

        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x14 },
        { 0x4E, 0xE0 },
        { 0x73, 0xF6 },
        { 0x74, 0xC4 },
        { 0x75, 0x41 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes, 4627, 7)

        { 0x12, 0x24 },
        { 0x26, 0x8B },
        { 0x3A, 0x0C },
        { 0x4E, 0x17 },
        { 0x62, 0x86 },
        { 0x76, 0x05 },
        { 0x8A, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 4627, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0xB0 },
        { 0x0E, 0x0A },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 4627, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0xF4 },
        { 0x0E, 0x0A },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 4627, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xF4 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x10 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache, 4627, 8)

        { 0x1A, 0x8B },
        { 0x1B, 0x4D },
        { 0x1C, 0x04 },
        { 0x1D, 0xD1 },
        { 0x1E, 0xEE },
        { 0x2E, 0xC7 },
        { 0x3E, 0x00 },
        { 0x4E, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnResource
// ******************************************************************
OOVPA_XREF(D3D_BlockOnResource, 4627, 9,

    XREF_D3D_BlockOnResource,
    XRefZero)

		{ 0x35, 0x8B }, // mov edx, [ecx+0x1C]
		{ 0x36, 0x42 },
		{ 0x37, 0x30 },
		{ 0x41, 0xC2 }, // retn 4
		{ 0x42, 0x04 },
		{ 0x43, 0x00 },
		{ 0x65, 0xC2 }, // retn 4
		{ 0x66, 0x04 },
		{ 0x67, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3D8_4627
// ******************************************************************
OOVPATable D3D8_4627[] = {

	REGISTER_OOVPA(Direct3D_CreateDevice, 4627, PATCH),
	REGISTER_OOVPA(D3D_CreateDeviceX, 4627, DISABLED),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3925, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4627, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4627, PATCH),
	REGISTER_OOVPA(D3D_CommonSetRenderTarget, 4627, XREF),
	REGISTER_OOVPA(D3D_CommonSetRenderTargetB, 4627, XREF),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant1, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant4, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstantNotInline, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture2, 4831, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4134, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer2, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4134, PATCH),
REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetStreamSource2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_Register, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3925, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4361, PATCH),
	REGISTER_OOVPA(D3DResource_GetType, 4627, DISABLED),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4627, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 4627, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 4361, PATCH),
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel2, 4627, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3925, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewportOffsetAndScale, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4721, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4531, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4627, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4134, PATCH), // Beware of the typo...
	REGISTER_OOVPA(D3DDevice_GetTexture2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStateVB, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetStateUP, 4627, DISABLED),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStipple, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4928, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4361, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4134, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 4627, XREF),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 4627, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 4627, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3925, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4627, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette2, 4627, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPersistedSurface2, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4627, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetDepthClipPlanes, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_KickPushBuffer, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4627, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3925, PATCH), // TODO: This needs to be verified on 4361, not just 4242!
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3925, DISABLED),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4627, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4627, PATCH),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3925, PATCH),
};

// ******************************************************************
// * D3D8_4627_SIZE
// ******************************************************************
uint32 D3D8_4627_SIZE = sizeof(D3D8_4627);
