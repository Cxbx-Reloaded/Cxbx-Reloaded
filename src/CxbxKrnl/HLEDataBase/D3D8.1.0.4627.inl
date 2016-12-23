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
OOVPA_NO_XREF(IDirect3D8_CreateDevice_1_0_4627, 8)

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
OOVPA_END;

// ******************************************************************
// * IDirect3D8_CreateDeviceX (From D3D8X)
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_CreateDeviceX_1_0_4627, 8)

        // IDirect3D8_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 }, // (Offset,Value)-Pair #1
        { 0x08, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x7F : repe stosd
        { 0x7F, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x80, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x82 : mov eax, esi
        { 0x82, 0x8B }, // (Offset,Value)-Pair #5
        { 0x83, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 }, // (Offset,Value)-Pair #7
        { 0xA1, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BeginVisibilityTest_1_0_4627, 11)

        // IDirect3DDevice8_BeginVisibilityTest+0x13 : mov dword ptr [eax], 0x000817C8
        { 0x13, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x14, 0x00 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC8 }, // (Offset,Value)-Pair #3
        { 0x16, 0x17 }, // (Offset,Value)-Pair #4
        { 0x17, 0x08 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_BeginVisibilityTest+0x19 : mov ecx, 1
        { 0x19, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1A, 0x01 }, // (Offset,Value)-Pair #7
        { 0x1B, 0x00 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_BeginVisibilityTest+0x24 : add eax, 0x0C
        { 0x24, 0x83 }, // (Offset,Value)-Pair #9
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x26, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3D8_CreateDevice
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_CreateDevice_1_0_5028, 8)

        // IDirect3D8_CreateDevice+0x0A : jnz +0x0A
        { 0x0A, 0x75 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x0A }, // (Offset,Value)-Pair #2

        // IDirect3D8_CreateDevice+0x80 : repe stosd
        { 0x80, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x81, 0xAB }, // (Offset,Value)-Pair #4

        // IDirect3D8_CreateDevice+0x83 : mov eax, esi
        { 0x83, 0x8B }, // (Offset,Value)-Pair #5
        { 0x84, 0xC6 }, // (Offset,Value)-Pair #6

        // IDirect3D8_CreateDevice+0x91 : retn 0x18
        { 0x91, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x92, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_EndVisibilityTest_1_0_4627, 13)

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

        // IDirect3DDevice8_EndVisibilityTest+0x49 : add eax, 0x0C
        { 0x49, 0x83 }, // (Offset,Value)-Pair #11
        { 0x4A, 0xC0 }, // (Offset,Value)-Pair #12
        { 0x4B, 0x0C }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3D8_GetAdapterDisplayMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_GetAdapterDisplayMode_1_0_4627, 13)

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
OOVPA_END;

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_KickOffAndWaitForIdle_1_0_4627, 9)

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
OOVPA_END;

// ******************************************************************
// * IDirect3D8_KickOffAndWaitForIdle2
// ******************************************************************
OOVPA_NO_XREF(IDirect3D8_KickOffAndWaitForIdle2_1_0_4627, 11)

        // IDirect3D8_KickOffAndWaitForIdle2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3D8_KickOffAndWaitForIdle2+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x48 }, // (Offset,Value)-Pair #3
        { 0x07, 0x30 }, // (Offset,Value)-Pair #4

        // IDirect3D8_KickOffAndWaitForIdle2+0x08 : push 2
        { 0x08, 0x6A }, // (Offset,Value)-Pair #5
        { 0x09, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3D8_KickOffAndWaitForIdle2+0x0A : push ecx
        { 0x0A, 0x51 }, // (Offset,Value)-Pair #7

        // IDirect3D8_KickOffAndWaitForIdle2+0x0B : call [addr]
        { 0x0B, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3D8_KickOffAndWaitForIdle2+0x18 : call [addr]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3D8_KickOffAndWaitForIdle2+0x1D : retn 0x08
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x1E, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BeginPush
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BeginPush_1_0_4627, 7)

        { 0x08, 0x00 },
        { 0x12, 0x8B },
        { 0x1C, 0x02 },
        { 0x26, 0x72 },
        { 0x30, 0xE8 },
        { 0x3A, 0x76 },
        { 0x44, 0x52 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetViewport_1_0_4627, 9)

        // IDirect3DDevice8_SetViewport+0x0A : mov eax, [esi+0x21B4]
        { 0x0A, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0B, 0x86 }, // (Offset,Value)-Pair #2
        { 0x0C, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x21 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetViewport+0x63 : shl edi, cl
        { 0x63, 0xD3 }, // (Offset,Value)-Pair #5
        { 0x64, 0xE7 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetViewport+0xDF : and ecx, 0x0F
        { 0xDF, 0x83 }, // (Offset,Value)-Pair #7
        { 0xE0, 0xE1 }, // (Offset,Value)-Pair #8
        { 0xE1, 0x0F }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * D3D_CommonSetRenderTarget
// ******************************************************************
OOVPA_XREF(D3D_CommonSetRenderTarget_1_0_4627, 12,

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
OOVPA_XREF(D3D_CommonSetRenderTargetB_1_0_4627, 11,

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
// * IDirect3DDevice8_SetRenderTarget
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_SetRenderTarget_1_0_4627, 13,

    XRefNoSaveIndex,
    XRefOne)

        // IDirect3DDevice8_SetRenderTarget+0x10 : call [D3D_CommonSetRenderTarget]
        { 0x10, XREF_COMMONSETRENDERTARGET }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderTarget+0x00 : mov eax, [esp+0x08]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x44 }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x08 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderTarget+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #6
        { 0x05, 0x4C }, // (Offset,Value)-Pair #7
        { 0x06, 0x24 }, // (Offset,Value)-Pair #8
        { 0x07, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderTarget+0x0D : push eax; push ecx
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #10
        { 0x0E, 0x51 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderTarget+0x14 : retn 0x08
        { 0x14, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x15, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_AddRef_1_0_4627, 10)

        // IDirect3DDevice8_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_AddRef+0x05 : mov ecx, [eax+0x0500]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #2
        { 0x06, 0x88 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x05 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_AddRef+0x0C : mov [eax+0x0500], ecx
        { 0x0C, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x0E, 0x00 }, // (Offset,Value)-Pair #9
        { 0x0F, 0x05 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetGammaRamp_1_0_4928, 9)

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
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetShaderConstantMode_1_0_4627, 12)

        // IDirect3DDevice8_SetShaderConstantMode+0x26 : mov [ebx+0x20D8], eax
        { 0x26, 0x89 }, // (Offset,Value)-Pair #1
        { 0x27, 0x83 }, // (Offset,Value)-Pair #2
        { 0x28, 0xD8 }, // (Offset,Value)-Pair #3
        { 0x29, 0x20 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetShaderConstantMode+0x50 : mov dword ptr [eax+0x04], 0x3C
        { 0x50, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x51, 0x40 }, // (Offset,Value)-Pair #6
        { 0x52, 0x04 }, // (Offset,Value)-Pair #7
        { 0x53, 0x3C }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetShaderConstantMode+0xE7 : add esi, 0x0124
        { 0xE7, 0x81 }, // (Offset,Value)-Pair #9
        { 0xE8, 0xC6 }, // (Offset,Value)-Pair #10
        { 0xE9, 0x24 }, // (Offset,Value)-Pair #11
        { 0xEA, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CopyRects
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CopyRects_1_0_4627, 8)

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
// * IDirect3DDevice8_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2_1_0_4627, 12)

        // IDirect3DDevice8_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer+0x0D : jnz +0x19
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x19 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer+0x15 : mov esi, [ecx+eax*4+0x21C0]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x81 }, // (Offset,Value)-Pair #8
        { 0x18, 0xC0 }, // (Offset,Value)-Pair #9
        { 0x19, 0x21 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer+0x40 : retn 0x04
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x41, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetBackBuffer2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetBackBuffer2B_1_0_4627, 12)

        // IDirect3DDevice8_GetBackBuffer2B+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x06, 0xFF }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_GetBackBuffer2B+0x0D : jnz +0x07
        { 0x0D, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0E, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetBackBuffer2B+0x1E : mov esi, [ecx+eax*4+0x21C0]
        { 0x1E, 0x8B }, // (Offset,Value)-Pair #6
        { 0x1F, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x20, 0x81 }, // (Offset,Value)-Pair #8
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #9
        { 0x22, 0x21 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_GetBackBuffer2B+0x4C : retn 0x04
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x4D, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetRenderTarget2_1_0_4627, 10)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetRenderTarget2B_1_0_4627, 10)

        // IDirect3DDevice8_GetRenderTarget2B+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetRenderTarget2B+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetRenderTarget2B+0x06 : mov esi, [eax + 0x21B4]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB4 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetRenderTarget2B+0x2A : jz + 0x06
        { 0x2A, 0x74 }, // (Offset,Value)-Pair #7
        { 0x2B, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetRenderTarget2B+0x2D : call [addr]
        { 0x2D, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetRenderTarget2B+0x37 : retn
        { 0x37, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDepthStencilSurface2_1_0_4627, 10)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDepthStencilSurface2B_1_0_4627, 10)

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x00 : mov eax, [addr]
        { 0x00, 0xA1 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x05 : push esi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x06 : mov esi, [eax + 0x21B8]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #3
        { 0x07, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x08, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x09, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x0E : jnz +0x04
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x31 : call [addr]
        { 0x31, 0xE8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetDepthStencilSurface2B+0x3B : retn
        { 0x3B, 0xC3 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetTile
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetTile_1_0_4627, 11)

        // IDirect3DDevice8_GetTile+0x13 : lea esi, [ecx+eax*8+0x2260]
        { 0x13, 0x8D }, // (Offset,Value)-Pair #1
        { 0x14, 0xB4 }, // (Offset,Value)-Pair #2
        { 0x15, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x16, 0x60 }, // (Offset,Value)-Pair #4
        { 0x17, 0x22 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 }, // (Offset,Value)-Pair #6
        { 0x1B, 0x06 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_GetTile+0x21 : pop edi; pop esi
        { 0x21, 0x5F }, // (Offset,Value)-Pair #8
        { 0x22, 0x5E }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x24, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTileNoWait
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTileNoWait_1_0_4627, 11)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateIndexBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateIndexBuffer2_1_0_4627, 7)

        { 0x05, 0xC0 },
        { 0x0F, 0x85 },
        { 0x13, 0xC2 },
        { 0x1A, 0x89 },
        { 0x21, 0x08 },
        { 0x28, 0x00 },
        { 0x2F, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPixelShaderConstant_1_0_4928, 13)

        // IDirect3DDevice8_SetPixelShaderConstant+0x1E : test eax, eax
        { 0x1E, 0x85 }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC0 }, // (Offset,Value)-Pair #2

		// IDirect3DDevice8_SetPixelShaderConstant+0x32 : mov [ebp+0x10], eax
		{ 0x32, 0x89 }, // (Offset,Value)-Pair #3
		{ 0x33, 0x45 }, // (Offset,Value)-Pair #4
		{ 0x34, 0x10 }, // (Offset,Value)-Pair #5

		// IDirect3DDevice8_SetPixelShaderConstant+0x84 : or eax, ebx
		{ 0x84, 0x0B }, // (Offset,Value)-Pair #6
		{ 0x85, 0xC3 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetPixelShaderConstant+0xBD : mov dword ptr [eax], 0x406A0
		{ 0xBD, 0xC7 }, // (Offset,Value)-Pair #8
		{ 0xBE, 0x00 }, // (Offset,Value)-Pair #9
		{ 0xBF, 0x60 }, // (Offset,Value)-Pair #10
		{ 0xC0, 0x0A }, // (Offset,Value)-Pair #11
		{ 0xC1, 0x04 }, // (Offset,Value)-Pair #12
		{ 0xC2, 0x00 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstant1_1_0_4627, 11)

        // IDirect3DDevice8_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x08, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant1+0x0F : jnb +0x43
        { 0x0F, 0x73 }, // (Offset,Value)-Pair #4
        { 0x10, 0x43 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstant1+0x27 : shl ecx, 0x04
        { 0x27, 0xC1 }, // (Offset,Value)-Pair #6
        { 0x28, 0xE1 }, // (Offset,Value)-Pair #7
        { 0x29, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstant1+0x5D : jmp +0xA2
        { 0x5D, 0xEB }, // (Offset,Value)-Pair #10
        { 0x5E, 0xA2 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstant4
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627, 12)

        // IDirect3DDevice8_SetVertexShaderConstant4+0x05 : add eax, 0x4C
        { 0x05, 0x83 }, // (Offset,Value)-Pair #1
        { 0x06, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x07, 0x4C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstant4+0x34 : movq mm7, qword ptr [edx+0x38]
        { 0x34, 0x0F }, // (Offset,Value)-Pair #4
        { 0x35, 0x6F }, // (Offset,Value)-Pair #5
        { 0x36, 0x7A }, // (Offset,Value)-Pair #6
        { 0x37, 0x38 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetVertexShaderConstant4+0x42 : shl ecx, 0x04
        { 0x42, 0xC1 }, // (Offset,Value)-Pair #8
        { 0x43, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x44, 0x04 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVertexShaderConstant4+0x91 : emms
        { 0x91, 0x0F }, // (Offset,Value)-Pair #11
        { 0x92, 0x77 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_4627, 12)

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x02 : test byte ptr ds:[abs], 0x10
        { 0x02, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0x00 }, // (Offset,Value)-Pair #2
        { 0x08, 0x10 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x0E : jnz +0x15
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #4
        { 0x0F, 0x15 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x42 : push esi; push edi; mov edi, eax
        { 0x10, 0x56 }, // (Offset,Value)-Pair #6
        { 0x11, 0x57 }, // (Offset,Value)-Pair #7
        { 0x12, 0x8B }, // (Offset,Value)-Pair #8
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x21 : rep movsd
        { 0x21, 0xF3 }, // (Offset,Value)-Pair #10
        { 0x22, 0xA5 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x28 : call [abs]
        { 0x28, 0xE8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPixelShader_1_0_4627, 10)

        // IDirect3DDevice8_SetPixelShader+0x26 : or ecx, 0x4800
        { 0x26, 0x81 }, // (Offset,Value)-Pair #1
        { 0x27, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x28, 0x00 }, // (Offset,Value)-Pair #3
        { 0x29, 0x48 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #5
        { 0x40, 0x20 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetPixelShader+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetPixelShader+0xDE : rep movsd
        { 0xDE, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xDF, 0xA5 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateTexture2_1_0_4627, 12)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateTexture2B_1_0_4627, 12)

        // IDirect3DDevice8_CreateTexture2B+0x02 : lea eax, [esp+0x20]
        { 0x02, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x20 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateTexture2B+0x0E : setz dl
        { 0x0E, 0x0F }, // (Offset,Value)-Pair #3
        { 0x0F, 0x94 }, // (Offset,Value)-Pair #4
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateTexture2B+0x2A : push 0x00
        { 0x25, 0x6A }, // (Offset,Value)-Pair #6
        { 0x26, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateTexture2B+0x53 : call [abs]
        { 0x50, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateTexture2B+0x78 : jnz +0x0D
        { 0x75, 0x75 }, // (Offset,Value)-Pair #9
        { 0x76, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateTexture2B+0xAE : retn 0x1C
        { 0x81, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x82, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateTexture2C
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateTexture2C_1_0_4627, 12)

        // IDirect3DDevice8_CreateTexture2C+0x02 : lea eax, [esp+0x20]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x07, 0x20 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_CreateTexture2C+0x0E : setz dl
        { 0x10, 0x0F }, // (Offset,Value)-Pair #3
        { 0x11, 0x94 }, // (Offset,Value)-Pair #4
        { 0x12, 0xC2 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_CreateTexture2C+0x2A : push 0x00
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #6
        { 0x2F, 0x00 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_CreateTexture2C+0x53 : call [abs]
        { 0x57, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_CreateTexture2C+0x78 : jnz +0x0D
        { 0x7C, 0x75 }, // (Offset,Value)-Pair #9
        { 0x7D, 0x0D }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_CreateTexture2C+0xAE : retn 0x1C
        { 0x88, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x89, 0x1C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetIndicesB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetIndicesB_1_0_4627, 11)

        // IDirect3DDevice8_SetIndicesB+0x0F : jz +0x10
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #1
        { 0x10, 0x10 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetIndicesB+0x1F : jmp +0x0A
        { 0x1F, 0xEB }, // (Offset,Value)-Pair #3
        { 0x20, 0x0A }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetIndicesB+0x58 : mov [esi+0x1C], ecx
        { 0x58, 0x89 }, // (Offset,Value)-Pair #5
        { 0x59, 0x4E }, // (Offset,Value)-Pair #6
        { 0x5A, 0x1C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetIndicesB+0x78 : mov [esi+0x038C], ebx
        { 0x78, 0x89 }, // (Offset,Value)-Pair #8
        { 0x79, 0x9E }, // (Offset,Value)-Pair #9
        { 0x7A, 0x8C }, // (Offset,Value)-Pair #10
        { 0x7B, 0x03 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureB_1_0_4627, 10)

        // IDirect3DDevice8_SetTextureB+0x0F : push edi
        { 0x0F, 0x57 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetTextureB+0x21 : jz +0x1E
        { 0x21, 0x74 }, // (Offset,Value)-Pair #2
        { 0x22, 0x1E }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetTextureB+0x33 : test ecx, 0x78FFFF
        { 0x33, 0xA9 }, // (Offset,Value)-Pair #4
        { 0x34, 0xFF }, // (Offset,Value)-Pair #5
        { 0x36, 0x78 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetTextureB+0x38 : mov [edi], ecx
        { 0x38, 0x89 }, // (Offset,Value)-Pair #7
        { 0x39, 0x0F }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetTextureB+0xD9 : jz +0x4D
        { 0xD9, 0x74 }, // (Offset,Value)-Pair #9
        { 0xDA, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureC
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureC_1_0_4928, 10)

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
// * IDirect3DDevice8_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDisplayMode_1_0_4627, 12)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_RunPushBuffer_1_0_4627, 10)

        // IDirect3DDevice8_RunPushBuffer+0x3C : mov edx, [esi+30h]
        { 0x3C, 0x8B }, // (Offset,Value)-Pair #1
        { 0x3D, 0x56 }, // (Offset,Value)-Pair #2
        { 0x3E, 0x30 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_RunPushBuffer+0xCA : mov edi, ebp
        { 0xCA, 0x8B }, // (Offset,Value)-Pair #4
        { 0xCB, 0xFD }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_RunPushBuffer+0xDC : jmp +0x12
        { 0xDC, 0xEB }, // (Offset,Value)-Pair #6
        { 0xDD, 0x12 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_RunPushBuffer+0xED : lea edi, [eax+4]
        { 0xED, 0x8D }, // (Offset,Value)-Pair #8
        { 0xEE, 0x78 }, // (Offset,Value)-Pair #9
        { 0xEF, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Begin
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Begin_1_0_4627, 14)

        // IDirect3DDevice8_Begin+0x25 : mov dword ptr [eax], 0x417FC
        { 0x25, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x26, 0x00 }, // (Offset,Value)-Pair #2
        { 0x27, 0xFC }, // (Offset,Value)-Pair #3
        { 0x28, 0x17 }, // (Offset,Value)-Pair #4
        { 0x29, 0x04 }, // (Offset,Value)-Pair #5
        { 0x2A, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_Begin+0x19 : shl edx, 6
        { 0x19, 0xD1 }, // (Offset,Value)-Pair #7
        { 0x1A, 0xE8 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_Begin+0x33 : or dword ptr [esi+8], 0x800
        { 0x33, 0x81 }, // (Offset,Value)-Pair #9
        { 0x34, 0x4E }, // (Offset,Value)-Pair #10
        { 0x35, 0x08 }, // (Offset,Value)-Pair #11
        { 0x37, 0x08 }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_Begin+0x3B : retn 0x08
        { 0x3B, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexData2f_1_0_4627, 14)

        // IDirect3DDevice8_SetVertexData2f+0x20 : lea edx, ds:abs[ecx*8]
        { 0x20, 0x8D }, // (Offset,Value)-Pair #1
        { 0x21, 0x14 }, // (Offset,Value)-Pair #2
        { 0x22, 0xCD }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexData2f+0x2B : mov [eax], edx; mov edx, [esp+0x10]
        { 0x2B, 0x89 }, // (Offset,Value)-Pair #4
        { 0x2C, 0x10 }, // (Offset,Value)-Pair #5
        { 0x2D, 0x8B }, // (Offset,Value)-Pair #6
        { 0x2E, 0x54 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData2f+0x37 : add eax, 0x0C
        { 0x37, 0x83 }, // (Offset,Value)-Pair #10
        { 0x38, 0xC0 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12

        // IDirect3DDevice8_SetVertexData2f+0x3D : retn 0x0C
        { 0x3D, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x3E, 0x0C }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexData4f_1_0_4627, 11)

        // IDirect3DDevice8_SetVertexData4f+0x20 : cmp ecx, -1; jnz +0x07
        { 0x20, 0x83 }, // (Offset,Value)-Pair #1
        { 0x21, 0xF9 }, // (Offset,Value)-Pair #2
        { 0x22, 0xFF }, // (Offset,Value)-Pair #3
        { 0x23, 0x75 }, // (Offset,Value)-Pair #4
        { 0x24, 0x07 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexData4f+0x2C : add ecx, 0x01A0
        { 0x2C, 0x81 }, // (Offset,Value)-Pair #6
        { 0x2D, 0xC1 }, // (Offset,Value)-Pair #7
        { 0x2E, 0xA0 }, // (Offset,Value)-Pair #8
        { 0x2F, 0x01 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexData4f+0x5F : retn 0x14
        { 0x5F, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x60, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_End
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_End_1_0_4627, 12)

        // IDirect3DDevice8_End+0x1C : mov dword ptr [eax], 0x417FC
        { 0x1C, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1D, 0x00 }, // (Offset,Value)-Pair #2
        { 0x1E, 0xFC }, // (Offset,Value)-Pair #3
        { 0x1F, 0x17 }, // (Offset,Value)-Pair #4
        { 0x20, 0x04 }, // (Offset,Value)-Pair #5
        { 0x21, 0x00 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_End+0x31 : test ah, 0x10
        { 0x31, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x32, 0xC4 }, // (Offset,Value)-Pair #8
        { 0x33, 0x10 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_End+0x34 : jz +0x07
        { 0x34, 0x74 }, // (Offset,Value)-Pair #10
        { 0x35, 0x07 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_End+0x45 : retn
        { 0x45, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
OOVPA_NO_XREF_LARGE(IDirect3DDevice8_Clear_1_0_4627, 9)
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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_ClearB
// ******************************************************************
OOVPA_NO_XREF_LARGE(IDirect3DDevice8_ClearB_1_0_4627, 9)
        // IDirect3DDevice8_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0001, 0xEC }, // (Offset,Value)-Pair #2
        { 0x0002, 0x38 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_Clear+0x001C : mov ebx, [ebp+0x21B8]
        { 0x001C, 0x8B }, // (Offset,Value)-Pair #4
        { 0x001D, 0xB5 }, // (Offset,Value)-Pair #5
        { 0x001E, 0xB8 }, // (Offset,Value)-Pair #6
        { 0x001F, 0x21 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_Clear+0x03C6 : retn 0x18
        { 0x03C6, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x03C7, 0x18 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Swap_1_0_4627, 11)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateVertexBuffer2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateVertexBuffer2_1_0_4627, 13)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_UpdateOverlay_1_0_4627, 11)

        // IDirect3DDevice8_UpdateOverlay+0x0F : mov [eax+0x2BA8], ecx
        { 0x0F, 0x89 }, // (Offset,Value)-Pair #1
        { 0x10, 0x88 }, // (Offset,Value)-Pair #2
        { 0x11, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x12, 0x2B }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_UpdateOverlay+0x8E : and eax, 0xFFFFFFFE
        { 0x8E, 0x83 }, // (Offset,Value)-Pair #5
        { 0x8F, 0xE0 }, // (Offset,Value)-Pair #6
        { 0x90, 0xFE }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_UpdateOverlay+0xAC : mov [esi+0x8930], eax
        { 0xAC, 0x89 }, // (Offset,Value)-Pair #8
        { 0xAD, 0x86 }, // (Offset,Value)-Pair #9
        { 0xAE, 0x30 }, // (Offset,Value)-Pair #10
        { 0xAF, 0x89 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetOverlayUpdateStatus
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_4627, 12)

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x05 : mov ecx, [eax+0x2BA8]
        { 0x05, 0x8B }, // (Offset,Value)-Pair #1
        { 0x06, 0x88 }, // (Offset,Value)-Pair #2
        { 0x07, 0xA8 }, // (Offset,Value)-Pair #3
        { 0x08, 0x2B }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x0C : mov esi, [eax+0x2580]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x0D, 0xB0 }, // (Offset,Value)-Pair #6
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x25 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x16 : setnz dl
        { 0x16, 0x0F }, // (Offset,Value)-Pair #9
        { 0x17, 0x95 }, // (Offset,Value)-Pair #10
        { 0x18, 0xC2 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_GetOverlayUpdateStatus+0x1C : retn
        { 0x1C, 0xC3 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4627, 11)

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A }, // (Offset,Value)-Pair #1
        { 0x06, 0x00 }, // (Offset,Value)-Pair #2
        { 0x07, 0x6A }, // (Offset,Value)-Pair #3
        { 0x08, 0x00 }, // (Offset,Value)-Pair #4
        { 0x09, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0A, 0x01 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x17 : add eax, 0x2554
        { 0x17, 0x05 }, // (Offset,Value)-Pair #7
        { 0x18, 0x54 }, // (Offset,Value)-Pair #8
        { 0x19, 0x25 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVerticalBlankCallback_1_0_4627, 12)

        // IDirect3DDevice8_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x2550], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9
        { 0x0D, 0x25 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_GetViewport
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetViewport_1_0_4627, 10)

        // IDirect3DDevice8_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 }, // (Offset,Value)-Pair #1
        { 0x06, 0x57 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_GetViewport+0x0B : lea esi, [eax+0x0A90]
        { 0x0B, 0x8D }, // (Offset,Value)-Pair #3
        { 0x0C, 0xB0 }, // (Offset,Value)-Pair #4
        { 0x0D, 0x90 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x0A }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 }, // (Offset,Value)-Pair #7
        { 0x12, 0x06 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1B, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, 10)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_NormalizeNormalsB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_NormalizeNormalsB_1_0_4627, 8)

        // IDirect3DDevice8_SetRenderState_NormalizeNormalsB+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_NormalizeNormalsB+0x2E : or [addr], 0x200
        { 0x2E, 0x81 }, // (Offset,Value)-Pair #3
        { 0x2F, 0x0D }, // (Offset,Value)-Pair #4
        { 0x34, 0x00 }, // (Offset,Value)-Pair #5
        { 0x35, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_NormalizeNormalsB+0x3F : retn 0x04
        { 0x3F, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x40, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_EdgeAntiAliasB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_EdgeAntiAliasB_1_0_4627, 10)

        // IDirect3DDevice8_SetRenderState_EdgeAntiAliasB+0x07 : mov eax, [esi]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x06 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_EdgeAntiAliasB+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #3
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_EdgeAntiAliasB+0x1C : mov ecx, [esp+0x08]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #5
        { 0x1D, 0x4C }, // (Offset,Value)-Pair #6
        { 0x1E, 0x24 }, // (Offset,Value)-Pair #7
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_EdgeAntiAliasB+0x38 : retn 0x04
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4627, 9)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_ZEnable_1_0_4627, 13)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnableB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_ZEnableB_1_0_4627, 13)

        // IDirect3DDevice8_SetRenderState_ZEnableB+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_ZEnableB+0x1B : mov ecx, [esi+0x21B8h]
        { 0x24, 0x8B }, // (Offset,Value)-Pair #3
        { 0x25, 0x8E }, // (Offset,Value)-Pair #4
        { 0x26, 0xB8 }, // (Offset,Value)-Pair #5
        { 0x27, 0x21 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_ZEnableB+0x37 : mov dword ptr [eax], 0x4030
        { 0x37, 0xC7 }, // (Offset,Value)-Pair #7
        { 0x38, 0x00 }, // (Offset,Value)-Pair #8
        { 0x39, 0x0C }, // (Offset,Value)-Pair #9
        { 0x3A, 0x03 }, // (Offset,Value)-Pair #10
        { 0x3B, 0x04 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetRenderState_ZEnableB+0x99 : retn 0x04
        { 0x99, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x9A, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_StencilEnableB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_StencilEnableB_1_0_4627, 11)

        // IDirect3DDevice8_SetRenderState_StencilEnableB+0x0C : jb +0x0E
        { 0x0C, 0x72 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x0E }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_StencilEnableB+0x35 : jz +0x19
        { 0x35, 0x74 }, // (Offset,Value)-Pair #3
        { 0x36, 0x19 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_StencilEnableB+0x56 : mov dword ptr [eax], 0x00041D84
        { 0x56, 0xC7 }, // (Offset,Value)-Pair #5
        { 0x57, 0x00 }, // (Offset,Value)-Pair #6
        { 0x58, 0x84 }, // (Offset,Value)-Pair #7
        { 0x59, 0x1D }, // (Offset,Value)-Pair #8
        { 0x5A, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_StencilEnableB+0x8A : retn 0x04
        { 0x8A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x8B, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627, 12)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB_1_0_4627, 12)

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x04 : push esi
        { 0x04, 0x56 }, // (Offset,Value)-Pair #1

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x11 : call [abs]
        { 0x11, 0xE8 }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x25 : shl edi, 0x10
        { 0x25, 0xC1 }, // (Offset,Value)-Pair #3
        { 0x26, 0xE7 }, // (Offset,Value)-Pair #4
        { 0x27, 0x10 }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x2C : jns +0x0C
        { 0x2C, 0x79 }, // (Offset,Value)-Pair #6
        { 0x2D, 0x0C }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x37 : or edi, 0x01
        { 0x37, 0x83 }, // (Offset,Value)-Pair #8
        { 0x38, 0xCF }, // (Offset,Value)-Pair #9
        { 0x39, 0x01 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB+0x7B : retn 0x04
        { 0x7B, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x7C, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawVerticesUP_1_0_4627, 10)

        // IDirect3DDevice8_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawVerticesUP+0x2F : mov edx, [edi+0x804]
        { 0x2F, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0x97 }, // (Offset,Value)-Pair #5
        { 0x31, 0x04 }, // (Offset,Value)-Pair #6
        { 0x32, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawVerticesUP+0xC2 : cmp esi, 0x80
        { 0xC2, 0x81 }, // (Offset,Value)-Pair #8
        { 0xC3, 0xFE }, // (Offset,Value)-Pair #9
        { 0xC4, 0x80 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawVerticesUPB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawVerticesUPB_1_0_4627, 10)

        // IDirect3DDevice8_DrawVerticesUPB+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawVerticesUPB+0x3C : mov edx, [esi+0x804]
        { 0x3C, 0x8B }, // (Offset,Value)-Pair #4
        { 0x3D, 0x96 }, // (Offset,Value)-Pair #5
        { 0x3E, 0x04 }, // (Offset,Value)-Pair #6
        { 0x3F, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawVerticesUPB+0xF9 : cmp edi, 0x80
        { 0xF9, 0x81 }, // (Offset,Value)-Pair #8
        { 0xFA, 0xFF }, // (Offset,Value)-Pair #9
        { 0xFB, 0x80 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawIndexedVertices_1_0_4627, 11)

        // IDirect3DDevice8_DrawIndexedVertices+0x0E : mov eax, [esi+0x1C]
        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x46 }, // (Offset,Value)-Pair #2
        { 0x10, 0x1C }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_DrawIndexedVertices+0x26 : push 0x0209
        { 0x26, 0x68 }, // (Offset,Value)-Pair #4
        { 0x27, 0x09 }, // (Offset,Value)-Pair #5
        { 0x28, 0x02 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_DrawIndexedVertices+0x66 : dec eax
        { 0x66, 0x48 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_DrawIndexedVertices+0xFB : prefetchnta byte ptr [esi+0x3C]
        { 0xFB, 0x0F }, // (Offset,Value)-Pair #8
        { 0xFC, 0x18 }, // (Offset,Value)-Pair #9
        { 0xFD, 0x46 }, // (Offset,Value)-Pair #10
        { 0xFE, 0x3C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetMaterial_1_0_4627, 12)

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
OOVPA_END;

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DVertexBuffer8_Lock2_1_0_4627, 11)

        // IDirect3DVertexBuffer8_Lock2+0x01 : mov bl, [esp+0x0C]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x0C }, // (Offset,Value)-Pair #4

        // IDirect3DVertexBuffer8_Lock2+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #6

        // IDirect3DVertexBuffer8_Lock2+0x2F : test bl, 0xA0
        { 0x2F, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x30, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x31, 0xA0 }, // (Offset,Value)-Pair #9

        // IDirect3DVertexBuffer8_Lock2+0x48 : retn 0x08
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x49, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DVertexBuffer8_Lock2B_1_0_4627, 11)

        // IDirect3DVertexBuffer8_Lock2B+0x01 : mov bl, [esp+0x0C]
        { 0x01, 0x8A }, // (Offset,Value)-Pair #1
        { 0x02, 0x5C }, // (Offset,Value)-Pair #2
        { 0x03, 0x24 }, // (Offset,Value)-Pair #3
        { 0x04, 0x0C }, // (Offset,Value)-Pair #4

        // IDirect3DVertexBuffer8_Lock2B+0x09 : jnz +0x24
        { 0x09, 0x75 }, // (Offset,Value)-Pair #5
        { 0x0A, 0x2D }, // (Offset,Value)-Pair #6

        // IDirect3DVertexBuffer8_Lock2B+0x2F : test bl, 0xA0
        { 0x38, 0xF6 }, // (Offset,Value)-Pair #7
        { 0x39, 0xC3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xA0 }, // (Offset,Value)-Pair #9

        // IDirect3DVertexBuffer8_Lock2B+0x48 : retn 0x08
        { 0x6B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x6C, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DResource8_ReleaseB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DResource8_ReleaseB_1_0_4627, 11)

        // IDirect3DResource8_ReleaseB+0x09 : and ecx, 0xFFFF
        { 0x09, 0x81 }, // (Offset,Value)-Pair #1
        { 0x0A, 0xE1 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xFF }, // (Offset,Value)-Pair #3
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #4

        // IDirect3DResource8_ReleaseB+0x14 : and eax, 0x00070000
        { 0x14, 0x25 }, // (Offset,Value)-Pair #5
        { 0x17, 0x07 }, // (Offset,Value)-Pair #6

        // IDirect3DResource8_ReleaseB+0x19 : cmp eax, 0x00050000
        { 0x19, 0x3D }, // (Offset,Value)-Pair #7
        { 0x1C, 0x05 }, // (Offset,Value)-Pair #8

        // IDirect3DResource8_ReleaseB+0x3D : pop edi
        { 0x3D, 0x5F }, // (Offset,Value)-Pair #9

        // IDirect3DResource8_ReleaseB+0x4A : retn 0x04
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4B, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirect3DResource8_GetType
// ******************************************************************
OOVPA_NO_XREF(IDirect3DResource8_GetType_1_0_4627, 7)

        { 0x10, 0x77 },
        { 0x22, 0x83 },
        { 0x34, 0x00 },
        { 0x46, 0x74 },
        { 0x58, 0x00 },
        { 0x6A, 0x74 },
        { 0x7C, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirect3DVolumeTexture8_LockBox
// ******************************************************************
OOVPA_NO_XREF(IDirect3DVolumeTexture8_LockBox_1_0_4627, 15)

        // IDirect3DVolumeTexture8_LockBox+0x00 : mov eax, [esp+0x14]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x14 }, // (Offset,Value)-Pair #4

        // IDirect3DVolumeTexture8_LockBox+0x04 : mov ecx, [esp+0x10]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x0C }, // (Offset,Value)-Pair #8

        // IDirect3DVolumeTexture8_LockBox+0x0C : push eax
        { 0x0C, 0x50 }, // (Offset,Value)-Pair #9

        // IDirect3DVolumeTexture8_LockBox+0x11 : push ecx; push edx; push eax
        { 0x11, 0x51 }, // (Offset,Value)-Pair #10
        { 0x12, 0x52 }, // (Offset,Value)-Pair #11
        { 0x13, 0x50 }, // (Offset,Value)-Pair #12

        // IDirect3DVolumeTexture8_LockBox+0x18 : call [addr]
        { 0x18, 0xE8 }, // (Offset,Value)-Pair #13

        // IDirect3DVolumeTexture8_LockBox+0x1D : retn 0x14
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x1E, 0x14 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * IDirect3DSurface8_LockRectB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DSurface8_LockRectB_1_0_4627, 17)

        // IDirect3DSurface8_LockRectB+0x00 : mov eax, [esp+0x10]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

        // IDirect3DSurface8_LockRectB+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x4C }, // (Offset,Value)-Pair #6
        { 0x06, 0x24 }, // (Offset,Value)-Pair #7
        { 0x07, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DSurface8_LockRectB+0x08 : push esi; mov esi, [esp+0x0C]; push eax
        { 0x08, 0x56 }, // (Offset,Value)-Pair #9
        { 0x09, 0x8B }, // (Offset,Value)-Pair #10
        { 0x0A, 0x74 }, // (Offset,Value)-Pair #11
        { 0x0B, 0x24 }, // (Offset,Value)-Pair #12
        { 0x0C, 0x0C }, // (Offset,Value)-Pair #13
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #14

        // IDirect3DSurface8_LockRectB+0x17 : call [addr]
        { 0x17, 0xE8 }, // (Offset,Value)-Pair #15

        // IDirect3DSurface8_LockRectB+0x1D : retn 0x10
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #16
        { 0x1E, 0x10 }, // (Offset,Value)-Pair #17
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc_1_0_4627, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x2C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x2D, 0x57 }, // (Offset,Value)-Pair #3
        { 0x2E, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21C0]
        { 0x52, 0x8B }, // (Offset,Value)-Pair #5
        { 0x53, 0x90 }, // (Offset,Value)-Pair #6
        { 0x54, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x55, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAF, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDescB
// ******************************************************************
OOVPA_XREF(Get2DSurfaceDescB_1_0_4627, 9,

    XREF_GET2DSURFACEDESCB,
    XRefZero)

        // Get2DSurfaceDescB+0x1D : movzx ecx, byte ptr [edi+0x0D]
        { 0x1D, 0x0F }, // (Offset,Value)-Pair #1
        { 0x1E, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x1F, 0x4F }, // (Offset,Value)-Pair #3
        { 0x20, 0x0D }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDescB+0x44 : mov edx, [eax+0x21C0]
        { 0x44, 0x8B }, // (Offset,Value)-Pair #5
        { 0x45, 0x90 }, // (Offset,Value)-Pair #6
        { 0x46, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x47, 0x21 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDescB+0x9E : retn
        { 0x9E, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDescC
// ******************************************************************
OOVPA_XREF(Get2DSurfaceDescC_1_0_4627, 10,

    XRefNoSaveIndex,
    XRefOne)

        // Get2DSurfaceDescC+0x10 : call [Get2DSurfaceDescB]
        { 0x10, XREF_GET2DSURFACEDESCB }, // (Offset,Value)-Pair #1

        // Get2DSurfaceDescC+0x00 : push ebx; push esi; push edi
        { 0x00, 0x53 }, // (Offset,Value)-Pair #2
        { 0x05, 0x56 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x57 }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDescC+0x01 : mov ebx, [esp+0x0C]
        { 0x01, 0x8B }, // (Offset,Value)-Pair #5
        { 0x02, 0x5C }, // (Offset,Value)-Pair #6
        { 0x03, 0x24 }, // (Offset,Value)-Pair #7
        { 0x04, 0x0C }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDescC+0x17 : retn 0x0C
        { 0x17, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x18, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * Get2DSurfaceDescD
// ******************************************************************
OOVPA_XREF(Get2DSurfaceDescD_1_0_4627, 10,

    XRefNoSaveIndex,
    XRefOne)

        // Get2DSurfaceDescD+0x0E : call [Get2DSurfaceDescB]
        { 0x0E, XREF_GET2DSURFACEDESCB }, // (Offset,Value)-Pair #1

        // Get2DSurfaceDescD+0x00 : push ebx; push esi; push edi
        { 0x00, 0x53 }, // (Offset,Value)-Pair #2
        { 0x01, 0x56 }, // (Offset,Value)-Pair #3
        { 0x06, 0x57 }, // (Offset,Value)-Pair #4

        // Get2DSurfaceDescD+0x02 : mov esi, [esp+0x10]
        { 0x02, 0x8B }, // (Offset,Value)-Pair #5
        { 0x03, 0x74 }, // (Offset,Value)-Pair #6
        { 0x04, 0x24 }, // (Offset,Value)-Pair #7
        { 0x05, 0x10 }, // (Offset,Value)-Pair #8

        // Get2DSurfaceDescD+0x15 : retn 0x08
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DTexture8_GetSurfaceLevel2
// ******************************************************************
OOVPA_NO_XREF(IDirect3DTexture8_GetSurfaceLevel2_1_0_4627, 14)

        // IDirect3DTexture8_GetSurfaceLevel2+0x00 : sub esp, 0x0C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DTexture8_GetSurfaceLevel2+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x14 }, // (Offset,Value)-Pair #5

        // IDirect3DTexture8_GetSurfaceLevel2+0x08 : lea eax, [esp+0x14]
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
OOVPA_END;

// ******************************************************************
// * IDirect3DTexture8_GetSurfaceLevel2B
// ******************************************************************
OOVPA_NO_XREF(IDirect3DTexture8_GetSurfaceLevel2B_1_0_4627, 14)

        // IDirect3DTexture8_GetSurfaceLevel2B+0x00 : sub esp, 0x0C
        { 0x00, 0x83 }, // (Offset,Value)-Pair #1
        { 0x01, 0xEC }, // (Offset,Value)-Pair #2
        { 0x02, 0x0C }, // (Offset,Value)-Pair #3

        // IDirect3DTexture8_GetSurfaceLevel2B+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x14 }, // (Offset,Value)-Pair #5

        // IDirect3DTexture8_GetSurfaceLevel2B+0x08 : lea eax, [esp+0x14]
        { 0x08, 0x8D }, // (Offset,Value)-Pair #6
        { 0x0B, 0x18 }, // (Offset,Value)-Pair #7

        // IDirect3DTexture8_GetSurfaceLevel2B+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B }, // (Offset,Value)-Pair #8
        { 0x1A, 0x24 }, // (Offset,Value)-Pair #9

        // IDirect3DTexture8_GetSurfaceLevel2B+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D }, // (Offset,Value)-Pair #10
        { 0x1E, 0x18 }, // (Offset,Value)-Pair #11

        // IDirect3DTexture8_GetSurfaceLevel2B+0x29 : call [abs]
        { 0x29, 0xE8 }, // (Offset,Value)-Pair #12

        // IDirect3DTexture8_GetSurfaceLevel2B+0x47 : retn 0x08
        { 0x45, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x46, 0x08 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetViewportOffsetAndScale@8
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_GetViewportOffsetAndScale_1_0_4627, 8)

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
OOVPA_NO_XREF(X_D3DDevice_SelectVertexShaderDirect_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_GetShaderConstantMode_1_0_4627, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xD8 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_GetVertexShader_1_0_4627, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x84 },
        { 0x0A, 0x00 },
        { 0x0D, 0x24 },
        { 0x10, 0x0A },
        { 0x13, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_GetVertexShaderConstant_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_SetVertexShaderInputDirect_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_GetVertexShaderInput_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_RunVertexStateShader_1_0_4627, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0xC0 },
        { 0x2A, 0x40 },
        { 0x35, 0xF4 },
        { 0x40, 0xC7 },
        { 0x4B, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_EndPush
// ******************************************************************
OOVPA_XREF(IDirect3DDevice8_EndPush_1_0_4627, 12,

    XRefNoSaveIndex,
    XRefOne)

        { 0x06, XREF_D3DDEVICE }, // 1

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
OOVPA_NO_XREF(X_D3DDevice_LoadVertexShader_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_LoadVertexShaderProgram_1_0_4627, 7)

        { 0x0B, 0x2D },
        { 0x18, 0x10 },
        { 0x25, 0xE0 },
        { 0x32, 0x5F },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_SelectVertexShader_1_0_4627, 8)

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
OOVPA_NO_XREF(X_D3DDevice_GetVertexShaderType_1_0_4627, 7)

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
OOVPA_NO_XREF(X_D3DDevice_GetVertexShaderDeclaration_1_0_4627, 8)

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
OOVPA_NO_XREF(X_D3DDevice_GetVertexShaderFunction_1_0_4627, 7)

        { 0x16, 0xC0 },
        { 0x2E, 0x01 },
        { 0x46, 0x10 },
        { 0x5E, 0x07 },
        { 0x76, 0xC7 },
        { 0x8E, 0x00 },
        { 0xA6, 0xF8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_SetVertexShaderInput_1_0_4627, 8)

        { 0x1E, 0x83 },
        { 0x3E, 0x10 },
        { 0x5E, 0x00 },
        { 0x7E, 0x24 },
        { 0x9E, 0x89 },
        { 0xBE, 0x81 },
        { 0xDE, 0xC6 },
        { 0xFE, 0x89 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetStreamSource2 (Maybe same in older versions)
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_GetStreamSource2_1_0_4627, 7)

        { 0x06, 0x34 },
        { 0x12, 0x85 },
        { 0x16, 0x57 },
        { 0x22, 0x8B },
        { 0x26, 0x89 },
        { 0x2E, 0x00 },
        { 0x36, 0x89 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderB
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetVertexShaderB_1_0_4627, 14)

        // IDirect3DDevice8_SetVertexShaderB+0x06 : test bl, 1
        { 0x06, 0xF6 }, // (Offset,Value)-Pair #1
        { 0x07, 0xC3 }, // (Offset,Value)-Pair #2
        { 0x08, 0x01 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderB+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #4
        { 0x12, 0x86 }, // (Offset,Value)-Pair #5
        { 0x13, 0x80 }, // (Offset,Value)-Pair #6
        { 0x14, 0x03 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetVertexShaderB+0x95 : retn 0x04
        { 0x95, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x96, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderB+0xC2 : mov dword ptr [eax], 0x4194C
        { 0xC2, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xC3, 0x00 }, // (Offset,Value)-Pair #11
        { 0xC4, 0x4C }, // (Offset,Value)-Pair #12
        { 0xC5, 0x19 }, // (Offset,Value)-Pair #13
        { 0xC6, 0x04 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(X_D3DDevice_Release_1_0_4627, 8)

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
// * IDirect3DDevice8_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetRenderState_LineWidth_1_0_4627, 8)

        // IDirect3DDevice8_SetRenderState_LineWidth+0x15 : fmul dword ptr [esi+0xXXXX]
        { 0x15, 0xD8 }, // (Offset,Value)-Pair #1
        { 0x16, 0x8E }, // (Offset,Value)-Pair #2

        // IDirect3DDevice8_SetRenderState_LineWidth+0x32 : cmp edi, 0x1FF
        { 0x32, 0x81 }, // (Offset,Value)-Pair #5
        { 0x33, 0xFF }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetRenderState_LineWidth+0x3A : mov edi, 0x1FF
        { 0x3A, 0xBF }, // (Offset,Value)-Pair #7
        { 0x3B, 0xFF }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetRenderState_LineWidth+0x62 : retn 0x04
        { 0x62, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x63, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetBackBufferScale_1_0_4627, 6)

        { 0x1E, 0x24 },
        { 0x3E, 0x06 },
//      { 0x62, 0xEC },
        { 0x7E, 0x8B },
        { 0x9E, 0xBF },
        { 0xBE, 0x89 },
        { 0xDE, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_Reset
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_Reset_1_0_4627, 18)

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
// * IDirect3DDevice8_SetPixelShaderProgram
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_SetPixelShaderProgram_1_0_4627, 10)

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
// * IDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetDisplayFieldStatus_1_0_4627, 7)

		{ 0x0B, 0x8B },
		{ 0x0C, 0x4C },
		{ 0x0D, 0x24 },
		{ 0x0E, 0x04 },
		{ 0x1E, 0x56 },
		{ 0x25, 0x33 },
		{ 0x26, 0xD2 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_CreateStateBlock
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_CreateStateBlock_1_0_4627, 11)

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
// * IDirect3DDevice8_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_InsertCallback_1_0_4627, 7)

        { 0x0D, 0x0E },
        { 0x1C, 0x8B },
        { 0x2B, 0x48 },
        { 0x3A, 0x0C },
        { 0x49, 0x89 },
        { 0x58, 0x10 },
        { 0x67, 0x07 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawRectPatch
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawRectPatch_1_0_4627, 8)

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
// * IDirect3DDevice8_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_GetProjectionViewportMatrix_1_0_4627, 7)

        { 0x02, 0x35 },
        { 0x07, 0x57 },
        { 0x0A, 0x24 },
        { 0x0E, 0x30 },
        { 0x12, 0xB9 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
/* blueshogun96 8/7/08
   for MGS2 :)*/
OOVPA_NO_XREF(IDirect3DDevice8_SetPixelShader_1_0_4721, 10)

        // IDirect3DDevice8_SetPixelShader+0x26 : or ecx, 0x4800
        { 0x26, 0x81 }, // (Offset,Value)-Pair #1
        { 0x27, 0xC9 }, // (Offset,Value)-Pair #2
        { 0x28, 0x00 }, // (Offset,Value)-Pair #3
        { 0x29, 0x48 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetPixelShader+0x3E : or eax, 0x2000
        { 0x3E, 0x0D }, // (Offset,Value)-Pair #5
        { 0x40, 0x20 }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetPixelShader+0x7D : retn 0x04
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x7E, 0x04 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_SetPixelShader+0xDE : rep movsd
        { 0xF4, 0xF3 }, // (Offset,Value)-Pair #9
        { 0xF5, 0xA5 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetScissors@12
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScissors_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_SetPalette_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp_1_0_4627, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0x04 },
        { 0x2A, 0x0D },
        { 0x35, 0xBC },
        { 0x40, 0x89 },
        { 0x4E, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertFence
// ******************************************************************
OOVPA_XREF(D3DDevice_InsertFence_1_0_4627, 5,

    XRefNoSaveIndex,
    XRefOne)

        { 0x03, XREF_D3D_SETFENCE },

        { 0x00, 0x6A },
        { 0x01, 0x00 },
        { 0x02, 0xE8 },
        { 0x07, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3D_CDevice_KickOff_1_0_4627, 13)

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
OOVPA_XREF(D3DDevice_KickPushBuffer_1_0_4627, 4,

    XRefNoSaveIndex,
    XRefOne)

	{ 0x07, XREF_D3D_KickOff },

        { 0x00, 0x8B },
        { 0x01, 0x0D },
        { 0x06, 0xE9 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8::GetTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTexture2_1_0_4627, 8)

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
OOVPA_NO_XREF(D3D_CDevice_SetStateVB_1_0_4627, 12)

		// D3D_CDevice_SetStateVB+0x00 : sub esp, 0xC
		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x0C },

		// D3D_CDevice_SetStateVB+0x0D : and eax, 0xFFFFFFAF
		{ 0x0D, 0x83 },
		{ 0x0E, 0xE0 },
		{ 0x0F, 0xAF },

		// D3D_CDevice_SetStateVB+0x10 : test ebx, 0x3FFFFF8F
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
OOVPA_NO_XREF(D3D_CDevice_SetStateUP_1_0_4627, 11)

		// D3D_CDevice_SetStateUP+0x05 : sub esp, 0x14
		{ 0x05, 0x83 },
		{ 0x06, 0xEC },
		{ 0x07, 0x14 },

		// D3D_CDevice_SetStateUP+0x08 : test eax, 0x3FFFFF8F
		{ 0x08, 0xA9 },
		{ 0x09, 0x8F },
		{ 0x0A, 0xFF },
		{ 0x0B, 0xFF },
		{ 0x0C, 0x3F },

		// D3D_CDevice_SetStateUP+0x27 : or eax, 0x50
		{ 0x27, 0x83 },
		{ 0x28, 0xC8 },
		{ 0x29, 0x50 },
OOVPA_END;

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4627, 10)

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #1
        { 0x4B, 0xCA }, // (Offset,Value)-Pair #2
        { 0x4C, 0x00 }, // (Offset,Value)-Pair #3
        { 0x4D, 0x08 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x784]
        { 0x5C, 0x8D }, // (Offset,Value)-Pair #5
        { 0x5D, 0x97 }, // (Offset,Value)-Pair #6
        { 0x5E, 0x84 }, // (Offset,Value)-Pair #7
        { 0x5F, 0x07 }, // (Offset,Value)-Pair #8

        // IDirect3DDevice8_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 }, // (Offset,Value)-Pair #9
        { 0x6B, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetStipple
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetStipple_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_PersistDisplay_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_PersistDisplay_1_0_4928, 14)

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
// * IDirect3DDevice8_SetSwapCallback
// ******************************************************************
/* See the comment for the 4134 OOVPA signature for previous notes */
OOVPA_NO_XREF(IDirect3DDevice8_SetSwapCallback_1_0_4928, 12)

        // IDirect3DDevice8_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #1
        { 0x01, 0x44 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #5
        { 0x05, 0x0D }, // (Offset,Value)-Pair #6

        // IDirect3DDevice8_SetVerticalBlankCallback+0x0A : mov [ecx+0x254C], eax
        { 0x0A, 0x89 }, // (Offset,Value)-Pair #7
        { 0x0B, 0x81 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x4C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x25 }, // (Offset,Value)-Pair #10

        // IDirect3DDevice8_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x11, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_BeginStateBlock_1_0_4627, 6)

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
OOVPA_NO_XREF(D3DDevice_EndStateBlock_1_0_4627, 6)

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
OOVPA_NO_XREF(D3DDevice_ApplyStateBlock_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock_1_0_4627, 8)

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
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock_1_0_4627, 7)

        { 0x11, 0x76 },
        { 0x24, 0x3B },
        { 0x37, 0xE8 },
        { 0x4A, 0x50 },
        { 0x5D, 0x74 },
        { 0x70, 0x06 },
        { 0x83, 0xEB },
OOVPA_END;

// ******************************************************************
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime_1_0_4627, 6,

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
OOVPA_XREF(D3DDevice_BlockOnFence_1_0_4627, 8,

    XRefNoSaveIndex,
    XRefOne)

	{ 0x08, XREF_D3D_BLOCKONTIME },

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
OOVPA_NO_XREF(Lock3DSurface_1_0_4627, 11)

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
// * IDirect3DDevice8_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface_1_0_4627, 9)

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
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface2_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_GetPixelShader_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_CreatePalette2_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DPalette_Lock2_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_EndPushBuffer_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_GetPersistedSurface2_1_0_4627, 6)

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
OOVPA_NO_XREF(D3DDevice_SetModelView_1_0_4627, 7)

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
OOVPA_NO_XREF(D3D_LazySetPointParams_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes_1_0_4627, 7)

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
OOVPA_NO_XREF(D3DDevice_GetMaterial_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_GetBackMaterial_1_0_4627, 8)

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
OOVPA_NO_XREF(D3DDevice_SetBackMaterial_1_0_4627, 9)

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
// * D3D8_1_0_4627
// ******************************************************************
OOVPATable D3D8_1_0_4627[] = {

    // IDirect3D8::CreateDevice
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDevice_1_0_4627, XTL::EmuIDirect3D8_CreateDevice),
    // IDirect3D8::CreateDeviceX
	OOVPA_TABLE_PATCH(IDirect3D8_CreateDeviceX_1_0_4627, XTL::EmuIDirect3D8_CreateDevice),
	// IDirect3D8::CheckDeviceFormat (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3D8_CheckDeviceFormat_1_0_4361, XTL::EmuIDirect3D8_CheckDeviceFormat),
	// IDirect3DDevice8::BeginVisibilityTest
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginVisibilityTest_1_0_4627, XTL::EmuIDirect3DDevice8_BeginVisibilityTest),
	// IDirect3DDevice8::GetCreationParameters (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetCreationParameters_1_0_4034, XTL::EmuIDirect3DDevice8_GetCreationParameters),
	// IDirect3DDevice8::EndVisibilityTest
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndVisibilityTest_1_0_4627, XTL::EmuIDirect3DDevice8_EndVisibilityTest),
	// IDirect3DDevice8::GetVisibilityTestResult (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetVisibilityTestResult_1_0_3925, XTL::EmuIDirect3DDevice8_GetVisibilityTestResult),
	// IDirect3D8::KickOffAndWaitForIdle
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle_1_0_4627, XTL::EmuIDirect3D8_KickOffAndWaitForIdle),
	// IDirect3D8::KickOffAndWaitForIdle2
	OOVPA_TABLE_PATCH(IDirect3D8_KickOffAndWaitForIdle2_1_0_4627, XTL::EmuIDirect3D8_KickOffAndWaitForIdle2),
	// IDirect3DDevice8::BeginPush
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginPush_1_0_4627, XTL::EmuIDirect3DDevice8_BeginPush),
	// IDirect3DDevice8::EndPush
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndPush_1_0_4627, XTL::EmuIDirect3DDevice8_EndPush),
	// IDirect3DDevice8::CopyRects
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CopyRects_1_0_4627, XTL::EmuIDirect3DDevice8_CopyRects),
	// IDirect3DDevice8::GetGammaRamp (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetGammaRamp_1_0_4034, XTL::EmuIDirect3DDevice8_GetGammaRamp),
	// IDirect3DDevice8::GetBackBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2_1_0_4627, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::GetBackBuffer2B
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetBackBuffer2B_1_0_4627, XTL::EmuIDirect3DDevice8_GetBackBuffer2),
	// IDirect3DDevice8::SetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetViewport_1_0_4627, XTL::EmuIDirect3DDevice8_SetViewport),
    // D3D::CommonSetRenderTarget (XREF)
	OOVPA_TABLE_XREF(D3D_CommonSetRenderTarget_1_0_4627),
    // D3D::CommonSetRenderTargetB (XREF)
	OOVPA_TABLE_XREF(D3D_CommonSetRenderTargetB_1_0_4627),
	// IDirect3DDevice8::SetRenderTarget
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderTarget_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderTarget),
	// IDirect3DDevice8::AddRef
	OOVPA_TABLE_PATCH(IDirect3DDevice8_AddRef_1_0_4627, XTL::EmuIDirect3DDevice8_AddRef),
	// IDirect3DDevice8::SetGammaRamp
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetGammaRamp_1_0_4928, XTL::EmuIDirect3DDevice8_SetGammaRamp),
	// IDirect3DDevice8::SetShaderConstantMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetShaderConstantMode_1_0_4627, XTL::EmuIDirect3DDevice8_SetShaderConstantMode),
	// IDirect3DDevice8::GetRenderTarget2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetRenderTarget2_1_0_4627, XTL::EmuIDirect3DDevice8_GetRenderTarget2),
	// IDirect3DDevice8::GetRenderTarget2B
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetRenderTarget2B_1_0_4627, XTL::EmuIDirect3DDevice8_GetRenderTarget2),
	// IDirect3DDevice8::GetDepthStencilSurface2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface2_1_0_4627, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2),
	// IDirect3DDevice8::GetDepthStencilSurface2B
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDepthStencilSurface2B_1_0_4627, XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2),
	// IDirect3DDevice8::GetTile
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTile_1_0_4627, XTL::EmuIDirect3DDevice8_GetTile),
	// IDirect3DDevice8::SetTileNoWait
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTileNoWait_1_0_4627, XTL::EmuIDirect3DDevice8_SetTileNoWait),
	// IDirect3DDevice8::CreateIndexBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateIndexBuffer2_1_0_4627, XTL::EmuIDirect3DDevice8_CreateIndexBuffer2),
	// IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreateVertexShader),
	// IDirect3DDevice8::SetPixelShaderConstant (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShaderConstant_1_0_4134, XTL::EmuIDirect3DDevice8_SetPixelShaderConstant),
	// IDirect3DDevice8::SetVertexShaderConstant1
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant1_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1),
	// IDirect3DDevice8::SetVertexShaderConstant4
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4),
	// IDirect3DDevice8::SetVertexShaderConstantNotInline
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline),
	// IDirect3DDevice8::DeletePixelShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DeletePixelShader_1_0_4134, XTL::EmuIDirect3DDevice8_DeletePixelShader),
	// IDirect3DDevice8::CreatePixelShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreatePixelShader_1_0_3925, XTL::EmuIDirect3DDevice8_CreatePixelShader),
	// IDirect3DDevice8::SetPixelShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_4627, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::CreateTexture2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2_1_0_4627, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::CreateTexture2B
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2B_1_0_4627, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::CreateTexture2C
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateTexture2C_1_0_4627, XTL::EmuIDirect3DDevice8_CreateTexture2),
	// IDirect3DDevice8::SetIndices (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndices_1_0_4361, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetIndicesB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetIndicesB_1_0_4627, XTL::EmuIDirect3DDevice8_SetIndices),
	// IDirect3DDevice8::SetTexture (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTexture_1_0_4361, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SetTextureB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureB_1_0_4627, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::SetTextureC
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureC_1_0_4928, XTL::EmuIDirect3DDevice8_SetTexture),
	// IDirect3DDevice8::GetDisplayMode
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayMode_1_0_4627, XTL::EmuIDirect3DDevice8_GetDisplayMode),
	// IDirect3DDevice8::RunPushBuffer
	OOVPA_TABLE_PATCH(IDirect3DDevice8_RunPushBuffer_1_0_4627, XTL::EmuIDirect3DDevice8_RunPushBuffer),
	// IDirect3DDevice8::Begin
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Begin_1_0_4627, XTL::EmuIDirect3DDevice8_Begin),
	// IDirect3DDevice8::SetVertexData2f
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData2f_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexData2f),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexData4f_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// IDirect3DDevice8::End
	OOVPA_TABLE_PATCH(IDirect3DDevice8_End_1_0_4627, XTL::EmuIDirect3DDevice8_End),
	// IDirect3DDevice8::Clear
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Clear_1_0_4627, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::ClearB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_ClearB_1_0_4627, XTL::EmuIDirect3DDevice8_Clear),
	// IDirect3DDevice8::Swap
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Swap_1_0_4627, XTL::EmuIDirect3DDevice8_Swap),
	// IDirect3DDevice8::EnableOverlay (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EnableOverlay_1_0_4361, XTL::EmuIDirect3DDevice8_EnableOverlay),
	// IDirect3DDevice8::CreateVertexBuffer2
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateVertexBuffer2_1_0_4627, XTL::EmuIDirect3DDevice8_CreateVertexBuffer2),
	// IDirect3DDevice8::UpdateOverlay
	OOVPA_TABLE_PATCH(IDirect3DDevice8_UpdateOverlay_1_0_4627, XTL::EmuIDirect3DDevice8_UpdateOverlay),
	// IDirect3DDevice8::GetOverlayUpdateStatus
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetOverlayUpdateStatus_1_0_4627, XTL::EmuIDirect3DDevice8_GetOverlayUpdateStatus),
	// IDirect3DDevice8::BlockUntilVerticalBlank
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BlockUntilVerticalBlank_1_0_4627, XTL::EmuIDirect3DDevice8_BlockUntilVerticalBlank),
	// IDirect3DDevice8::SetVerticalBlankCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVerticalBlankCallback_1_0_4627, XTL::EmuIDirect3DDevice8_SetVerticalBlankCallback),
	// IDirect3DDevice8::GetViewport
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetViewport_1_0_4627, XTL::EmuIDirect3DDevice8_GetViewport),
	// IDirect3DDevice8::SetTextureState_TexCoordIndex
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TexCoordIndex_1_0_4627, XTL::EmuIDirect3DDevice8_SetTextureState_TexCoordIndex),
	// IDirect3DDevice8::SetTextureState_BumpEnv (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BumpEnv_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BumpEnv),
	// IDirect3DDevice8::SetTextureState_BorderColor (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_BorderColor_1_0_4361, XTL::EmuIDirect3DDevice8_SetTextureState_BorderColor),
	// IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor),
	// IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend),
	// IDirect3DDevice8::SetRenderState_CullModeB (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullModeB_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_CullMode (* unchanged since 4034 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_CullMode_1_0_4034, XTL::EmuIDirect3DDevice8_SetRenderState_CullMode),
	// IDirect3DDevice8::SetRenderState_NormalizeNormals (* unchanged since 4432 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_NormalizeNormals_1_0_4432, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// IDirect3DDevice8::SetRenderState_NormalizeNormalsB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_NormalizeNormalsB_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_NormalizeNormals),
	// IDirect3DDevice8::SetRenderState_TextureFactor (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_TextureFactor_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_TextureFactor),
	// IDirect3DDevice8::SetRenderState_ZBias (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZBias_1_0_3925, XTL::EmuIDirect3DDevice8_SetRenderState_ZBias),
	// IDirect3DDevice8::SetRenderState_EdgeAntiAlias (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_EdgeAntiAlias_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// IDirect3DDevice8::SetRenderState_EdgeAntiAliasB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_EdgeAntiAliasB_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_EdgeAntiAlias),
	// IDirect3DDevice8::SetRenderState_FillMode (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FillMode_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_FillMode),
	// IDirect3DDevice8::SetRenderState_BackFillMode (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_BackFillMode_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_BackFillMode),
	// IDirect3DDevice8::SetRenderState_FogColor (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FogColor_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FogColor),
	// IDirect3DDevice8::SetRenderState_Dxt1NoiseEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Dxt1NoiseEnable_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_Dxt1NoiseEnable),
	// IDirect3DDevice8::SetRenderState_Simple (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_Simple_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_Simple),
	// IDirect3DDevice8::SetRenderState_ZEnable
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnable_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::SetRenderState_ZEnableB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ZEnableB_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_ZEnable),
	// IDirect3DDevice8::SetRenderState_StencilEnable (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnable_1_0_4361, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_StencilEnableB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilEnableB_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_StencilEnable),
	// IDirect3DDevice8::SetRenderState_LineWidth
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_LineWidth_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth),
	// IDirect3DDevice8::SetRenderState_OcclusionCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_OcclusionCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_OcclusionCullEnable),
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAlias
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleAntiAlias_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// IDirect3DDevice8::SetRenderState_MultiSampleAntiAliasB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_MultiSampleAntiAliasB_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleAntiAlias),
	// IDirect3DDevice8::SetRenderState_ShadowFunc (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_ShadowFunc_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_ShadowFunc),
	// IDirect3DDevice8::SwitchTexture (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SwitchTexture_1_0_4361, XTL::EmuIDirect3DDevice8_SwitchTexture),
	// IDirect3DDevice8::SetRenderState_YuvEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_YuvEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_YuvEnable),
	// IDirect3DDevice8::SetTransform (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTransform_1_0_4134, XTL::EmuIDirect3DDevice8_SetTransform),
	// IDirect3DDevice8::GetTransform (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetTransform_1_0_4361, XTL::EmuIDirect3DDevice8_GetTransform),
    // D3DDevice_GetStreamSource2
	OOVPA_TABLE_PATCH(X_D3DDevice_GetStreamSource2_1_0_4627, XTL::EmuIDirect3DDevice8_GetStreamSource2),
	// IDirect3DDevice8::SetStreamSource (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetStreamSource_1_0_4361, XTL::EmuIDirect3DDevice8_SetStreamSource),
	// IDirect3DDevice8::SetVertexShader (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShader_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::SetVertexShaderB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetVertexShaderB_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShader),
	// IDirect3DDevice8::DrawVertices (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVertices_1_0_4361, XTL::EmuIDirect3DDevice8_DrawVertices),
	// IDirect3DDevice8::DrawVerticesUP
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVerticesUP_1_0_4627, XTL::EmuIDirect3DDevice8_DrawVerticesUP),
	// IDirect3DDevice8::DrawVerticesUPB
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawVerticesUPB_1_0_4627, XTL::EmuIDirect3DDevice8_DrawVerticesUP),
	// IDirect3DDevice8::SetLight (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetLight_1_0_4361, XTL::EmuIDirect3DDevice8_SetLight),
	// IDirect3DDevice8::DrawIndexedVertices
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVertices_1_0_4627, XTL::EmuIDirect3DDevice8_DrawIndexedVertices),
	// IDirect3DDevice8::SetMaterial
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetMaterial_1_0_4627, XTL::EmuIDirect3DDevice8_SetMaterial),
	// IDirect3DDevice8::LightEnable (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_LightEnable_1_0_4361, XTL::EmuIDirect3DDevice8_LightEnable),
	// IDirect3DVertexBuffer8::Lock2
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock2_1_0_4627, XTL::EmuIDirect3DVertexBuffer8_Lock2),
	// IDirect3DVertexBuffer8::Lock2B
	OOVPA_TABLE_PATCH(IDirect3DVertexBuffer8_Lock2B_1_0_4627, XTL::EmuIDirect3DVertexBuffer8_Lock2),
	// IDirect3DResource8::Register (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Register_1_0_3925, XTL::EmuIDirect3DResource8_Register),
	// IDirect3DResource8::AddRef (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_AddRef_1_0_3925, XTL::EmuIDirect3DResource8_AddRef),
	// IDirect3DResource8::Release (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_Release_1_0_3925, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DResource8::ReleaseB
	OOVPA_TABLE_PATCH(IDirect3DResource8_ReleaseB_1_0_4627, XTL::EmuIDirect3DResource8_Release),
	// IDirect3DResource8::IsBusy (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DResource8_IsBusy_1_0_4361, XTL::EmuIDirect3DResource8_IsBusy), /* I forget why I had this in here...
    // IDirect3DResource8::GetType
	OOVPA_TABLE_PATCH(IDirect3DResource8_GetType_1_0_4627, XTL::EmuIDirect3DResource8_GetType),*/
    // Get2DSurfaceDesc
	OOVPA_TABLE_PATCH(Get2DSurfaceDesc_1_0_4627, XTL::EmuGet2DSurfaceDesc),
    // Get2DSurfaceDescB
	OOVPA_TABLE_XREF(Get2DSurfaceDescB_1_0_4627),
	// Get2DSurfaceDescC
	OOVPA_TABLE_PATCH(Get2DSurfaceDescC_1_0_4627, XTL::EmuGet2DSurfaceDesc),
	// Get2DSurfaceDescD
	OOVPA_TABLE_PATCH(Get2DSurfaceDescD_1_0_4627, XTL::EmuGet2DSurfaceDescD),
	// IDirect3DSurface8::GetDesc (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_GetDesc_1_0_4361, XTL::EmuIDirect3DSurface8_GetDesc),
	// IDirect3DSurface8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRect_1_0_3925, XTL::EmuIDirect3DSurface8_LockRect),
	// IDirect3DSurface8::LockRectB
	OOVPA_TABLE_PATCH(IDirect3DSurface8_LockRectB_1_0_4627, XTL::EmuIDirect3DSurface8_LockRect),
	// IDirect3DBaseTexture8::GetLevelCount (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(IDirect3DBaseTexture8_GetLevelCount_1_0_4361, XTL::EmuIDirect3DBaseTexture8_GetLevelCount),
	// IDirect3DTexture8::GetSurfaceLevel2
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel2_1_0_4627, XTL::EmuIDirect3DTexture8_GetSurfaceLevel2),
	// IDirect3DTexture8::GetSurfaceLevel2B
	OOVPA_TABLE_PATCH(IDirect3DTexture8_GetSurfaceLevel2B_1_0_4627, XTL::EmuIDirect3DTexture8_GetSurfaceLevel2),
	// IDirect3DTexture8::LockRect (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DTexture8_LockRect),
	// IDirect3DVolumeTexture8::LockBox
	OOVPA_TABLE_PATCH(IDirect3DVolumeTexture8_LockBox_1_0_4627, XTL::EmuIDirect3DVolumeTexture8_LockBox),
    // X_D3DDevice_GetViewportOffsetAndScale
	OOVPA_TABLE_PATCH(X_D3DDevice_GetViewportOffsetAndScale_1_0_4627, XTL::EmuIDirect3DDevice8_GetViewportOffsetAndScale),
    // D3DDevice_DeleteVertexShader (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_DeleteVertexShader_1_0_3925, XTL::EmuIDirect3DDevice8_DeleteVertexShader),
    // D3DDevice_SelectVertexShaderDirect
	OOVPA_TABLE_PATCH(X_D3DDevice_SelectVertexShaderDirect_1_0_4627, XTL::EmuIDirect3DDevice8_SelectVertexShaderDirect),
    // D3DDevice_GetShaderConstantMode
	OOVPA_TABLE_PATCH(X_D3DDevice_GetShaderConstantMode_1_0_4627, XTL::EmuIDirect3DDevice8_GetShaderConstantMode),
    // D3DDevice_GetVertexShader
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShader_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShader),
    // D3DDevice_GetVertexShaderConstant
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderConstant_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShaderConstant),
    // D3DDevice_SetVertexShaderInputDirect
	OOVPA_TABLE_PATCH(X_D3DDevice_SetVertexShaderInputDirect_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderInputDirect),
    // D3DDevice_GetVertexShaderInput
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderInput_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShaderInput),
    // D3DDevice_RunVertexStateShader
	OOVPA_TABLE_PATCH(X_D3DDevice_RunVertexStateShader_1_0_4627, XTL::EmuIDirect3DDevice8_RunVertexStateShader),
    // D3DDevice_LoadVertexShader
	OOVPA_TABLE_PATCH(X_D3DDevice_LoadVertexShader_1_0_4627, XTL::EmuIDirect3DDevice8_LoadVertexShader),
    // D3DDevice_LoadVertexShaderProgram
	OOVPA_TABLE_PATCH(X_D3DDevice_LoadVertexShaderProgram_1_0_4627, XTL::EmuIDirect3DDevice8_LoadVertexShaderProgram),
    // D3DDevice_SelectVertexShader
	OOVPA_TABLE_PATCH(X_D3DDevice_SelectVertexShader_1_0_4627, XTL::EmuIDirect3DDevice8_SelectVertexShader),
    // D3DDevice_GetVertexShaderSize (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderSize_1_0_3925, XTL::EmuIDirect3DDevice8_GetVertexShaderSize),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BeginStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_EndStateBlock_1_0_4361, XTL::EmuIDirect3DDevice8_EndStateBlock),
    // D3DDevice_GetVertexShaderType
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderType_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShaderType),
    // D3DDevice_GetVertexShaderDeclaration
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderDeclaration_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShaderDeclaration),
    // D3DDevice_GetVertexShaderFunction
	OOVPA_TABLE_PATCH(X_D3DDevice_GetVertexShaderFunction_1_0_4627, XTL::EmuIDirect3DDevice8_GetVertexShaderFunction),
    // D3DDevice_SetVertexShaderInput
	OOVPA_TABLE_PATCH(X_D3DDevice_SetVertexShaderInput_1_0_4627, XTL::EmuIDirect3DDevice8_SetVertexShaderInput),
    // D3DDevice_Release
	OOVPA_TABLE_PATCH(X_D3DDevice_Release_1_0_4627, XTL::EmuIDirect3DDevice8_Release),
	// IDirect3DDevice8::SetRenderState_StencilFail (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilFail_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilFail),
	// IDirect3DDevice8::SetPixelShader
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShader_1_0_4721, XTL::EmuIDirect3DDevice8_SetPixelShader),
	// IDirect3DDevice8::Reset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Reset_1_0_4134, XTL::EmuIDirect3DDevice8_Reset),
	// IDirect3DDevice8::SetScissors
	OOVPA_TABLE_PATCH(D3DDevice_SetScissors_1_0_4627, XTL::EmuIDirect3DDevice8_SetScissors),
	// IDirect3DDevice8::SetPalette
	OOVPA_TABLE_PATCH(D3DDevice_SetPalette_1_0_4627, XTL::EmuIDirect3DDevice8_SetPalette),
	// IDirect3DDevice8::SetBackBufferScale
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetBackBufferScale_1_0_4627, XTL::EmuIDirect3DDevice8_SetBackBufferScale),
	// IDirect3DDevice8::Reset
	OOVPA_TABLE_PATCH(IDirect3DDevice8_Reset_1_0_4627, XTL::EmuIDirect3DDevice8_Reset),
	// IDirect3DDevice8_SetPixelShaderProgram
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShaderProgram_1_0_4627, XTL::EmuIDirect3DDevice8_SetPixelShaderProgram),
	// IDirect3DDevice8_GetDisplayFieldStatus
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDisplayFieldStatus_1_0_4627, XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus),
	// IDirect3DDevice8_SetScreenSpaceOffset (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetScreenSpaceOffset_1_0_4134, XTL::EmuIDirect3DDevice8_SetScreenSpaceOffset),
	// IDirect3DDevice8_CreateStateBlock
	OOVPA_TABLE_PATCH(IDirect3DDevice8_CreateStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_CreateStateBlock),
	// IDirect3DDevice8_InsertCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_InsertCallback_1_0_4627, XTL::EmuIDirect3DDevice8_InsertCallback),
	// IDirect3DDevice8_DrawRectPatch
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawRectPatch_1_0_4627, XTL::EmuIDirect3DDevice8_DrawRectPatch),
	// IDirect3DDevice8_GetProjectionViewportMatrix
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetProjectionViewportMatrix_1_0_4627, XTL::EmuIDirect3DDevice8_GetProjectionViewportMatrix),
	// IDirect3DDevice8_BackFillMode (* unchanged since 4531 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_BackFillMode_1_0_4531, XTL::EmuIDirect3DDevice8_BackFillMode),
	// IDirect3DDevice8_GetDeviceCaps (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_GetDeviceCaps_1_0_3925, XTL::EmuIDirect3DDevice8_GetDeviceCaps),
	// D3DDevice_SetRenderState_MultiSampleMask
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMask_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMask),
	// D3DDevice_SetRenderState_LogicOp
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_LogicOp_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_LogicOp),
	// D3D::SetFence (XREF) (* unchanged since 4134 *)
	OOVPA_TABLE_XREF(D3D_SetFence_1_0_4134),
	// IDirect3DDevice8::InsertFence
	OOVPA_TABLE_PATCH(D3DDevice_InsertFence_1_0_4627, XTL::EmuIDirect3DDevice8_InsertFence),
	// D3D::CDevice::KickOff
	OOVPA_TABLE_PATCH(D3D_CDevice_KickOff_1_0_4627, XTL::EmuD3DDevice_KickOff),
	// IDirect3DDevice8::SetPixelShaderConstant
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetPixelShaderConstant_1_0_4928, XTL::EmuIDirect3DDevice8_SetPixelShaderConstant),
	// IDirect3DDevice8::SetRenderState_TwoSidedLighting (* unchanged since 4134 *)
	// Beware of the typo...
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetTextureState_TwoSidedLighting_1_0_4134, XTL::EmuIDirect3DDevice8_SetTextureState_TwoSidedLighting),
	// D3DDevice_GetTexture2
	OOVPA_TABLE_PATCH(D3DDevice_GetTexture2_1_0_4627, XTL::EmuIDirect3DDevice8_GetTexture2),
	// D3D::CDevice::SetStateVB
	/*
	OOVPA_TABLE_PATCH(D3D_CDevice_SetStateVB_1_0_4627, XTL::EmuD3DDevice_SetStateVB),
	*/
	// D3D::CDevice::SetStateUP
	/*
	OOVPA_TABLE_PATCH(D3D_CDevice_SetStateUP_1_0_4627, XTL::EmuD3DDevice_SetStateUP),
    */
	// IDirect3DDevice8::DrawIndexedVerticesUP
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DrawIndexedVerticesUP_1_0_4627, XTL::EmuIDirect3DDevice8_DrawIndexedVerticesUP),
	// IDirect3DDevice8::SetStipple
	OOVPA_TABLE_PATCH(D3DDevice_SetStipple_1_0_4627, XTL::EmuIDirect3DDevice8_SetStipple),
	// IDirect3DDevice8::SetRenderState_RopZCmpAlwaysRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZCmpAlwaysRead),
	// IDirect3DDevice8::SetRenderState_RopZRead (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_RopZRead_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_RopZRead),
	// IDirect3DDevice8::SetRenderState_DoNotCullUncompressed (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_DoNotCullUncompressed_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_DoNotCullUncompressed),
	// IDirect3DDevice8::DeletePixelShader (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_DeletePixelShader_1_0_4134, XTL::EmuIDirect3DDevice8_DeletePixelShader),
	// D3DDevice_PersistDisplay
	OOVPA_TABLE_PATCH(D3DDevice_PersistDisplay_1_0_4627, XTL::EmuIDirect3DDevice8_PersistDisplay),
	// D3DDevice_PersistDisplay
	OOVPA_TABLE_PATCH(D3DDevice_PersistDisplay_1_0_4928, XTL::EmuIDirect3DDevice8_PersistDisplay),
	// IDirect3DDevice8::SetSwapCallback
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetSwapCallback_1_0_4928, XTL::EmuIDirect3DDevice8_SetSwapCallback),
	// IDirect3DDevice8::SetRenderState_FrontFace (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_FrontFace_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_FrontFace),
	// IDirect3DDevice8::SetRenderState_MultiSampleMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleMode_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleMode),
	// IDirect3DDevice8::SetRenderState_MultiSampleRenderTargetMode
	OOVPA_TABLE_PATCH(D3DDevice_SetRenderState_MultiSampleRenderTargetMode_1_0_4627, XTL::EmuIDirect3DDevice8_SetRenderState_MultiSampleRenderTargetMode),
	// IDirect3DDevice8::SetRenderState_StencilCullEnable (* unchanged since 4134 *)
	OOVPA_TABLE_PATCH(IDirect3DDevice8_SetRenderState_StencilCullEnable_1_0_4134, XTL::EmuIDirect3DDevice8_SetRenderState_StencilCullEnable),
	// IDirect3DDevice8::BeginStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_BeginStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_BeginStateBlock),
	// IDirect3DDevice8::EndStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_EndStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_EndStateBlock),
	// IDirect3DDevice8::ApplyStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_ApplyStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_ApplyStateBlock),
	// IDirect3DDevice8::CaptureStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_CaptureStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_CaptureStateBlock),
	// IDirect3DDevice8::DeleteStateBlock
	OOVPA_TABLE_PATCH(D3DDevice_DeleteStateBlock_1_0_4627, XTL::EmuIDirect3DDevice8_DeleteStateBlock),
	// IDirect3DDevice8::SetVertexData4ub (* unchanged since 4361 *)
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4ub_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4ub),
	// IDirect3DDevice8::SetVertexData4f
	OOVPA_TABLE_PATCH(D3DDevice_SetVertexData4f_1_0_4361, XTL::EmuIDirect3DDevice8_SetVertexData4f),
	// D3D_BlockOnTime
	OOVPA_TABLE_XREF(D3D_BlockOnTime_1_0_4627),
	// D3DDevice_BlockOnFence
	OOVPA_TABLE_PATCH(D3DDevice_BlockOnFence_1_0_4627, XTL::EmuIDirect3DDevice8_BlockOnFence),
	// Lock3DSurface
	OOVPA_TABLE_PATCH(Lock3DSurface_1_0_4627, XTL::EmuLock3DSurface),
	// Lock2DSurface (* unchanged since 3925 *)
	OOVPA_TABLE_PATCH(Lock2DSurface_1_0_3925, XTL::EmuLock2DSurface),
	// IDirect3DDevice8::CreateImageSurface
	OOVPA_TABLE_PATCH(D3DDevice_CreateImageSurface_1_0_4627, XTL::EmuIDirect3DDevice8_CreateImageSurface),
	// IDirect3DCubeTexture8::GetCubeMapSurface2
	OOVPA_TABLE_PATCH(D3DCubeTexture_GetCubeMapSurface2_1_0_4627, XTL::EmuIDirect3DCubeTexture8_GetCubeMapSurface2),
	// IDirect3DDevice8::SetSoftDisplayFilter
	OOVPA_TABLE_PATCH(D3DDevice_SetSoftDisplayFilter_1_0_4627, XTL::EmuIDirect3DDevice8_SetSoftDisplayFilter),
	// IDirect3DDevice8::SetFlickerFilter
	OOVPA_TABLE_PATCH(D3DDevice_SetFlickerFilter_1_0_4627, XTL::EmuIDirect3DDevice8_SetFlickerFilter),
	// IDirect3DDevice8::GetPixelShader
	OOVPA_TABLE_PATCH(D3DDevice_GetPixelShader_1_0_4627, XTL::EmuIDirect3DDevice8_GetPixelShader),
	// IDirect3DDevice8::CreatePalette2
	OOVPA_TABLE_PATCH(D3DDevice_CreatePalette2_1_0_4627, XTL::EmuIDirect3DDevice8_CreatePalette2),
	// IDirect3DPalette8::Lock2
	OOVPA_TABLE_PATCH(D3DPalette_Lock2_1_0_4627, XTL::EmuIDirect3DPalette8_Lock2),
	// IDirect3DDevice8::BeginPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_BeginPushBuffer_1_0_4627, XTL::EmuIDirect3DDevice8_BeginPushBuffer),
	// IDirect3DDevice8::EndPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_EndPushBuffer_1_0_4627, XTL::EmuIDirect3DDevice8_EndPushBuffer),
	// D3DDevice_GetPersistedSurface2
	OOVPA_TABLE_PATCH(D3DDevice_GetPersistedSurface2_1_0_4627, XTL::EmuIDirect3DDevice8_GetPersistedSurface2),
	// D3DDevice_SetModelView
	OOVPA_TABLE_PATCH(D3DDevice_SetModelView_1_0_4627, XTL::EmuIDirect3DDevice8_SetModelView),
	// D3D_LazySetPointParams_1_0_4627
	OOVPA_TABLE_PATCH(D3D_LazySetPointParams_1_0_4627, XTL::EmuD3D_LazySetPointParams),
	// D3DDevice_SetDepthClipPlanes
	OOVPA_TABLE_PATCH(D3DDevice_SetDepthClipPlanes_1_0_4627, XTL::EmuIDirect3DDevice8_SetDepthClipPlanes),
	// IDirect3DDevice8::GetMaterial
	OOVPA_TABLE_PATCH(D3DDevice_GetMaterial_1_0_4627, XTL::EmuIDirect3DDevice8_GetMaterial),
	// IDirect3DDevice8::KickPushBuffer
	OOVPA_TABLE_PATCH(D3DDevice_KickPushBuffer_1_0_4627, XTL::EmuD3DDevice_KickOff),
	// IDirect3DDevice8::GetBackMaterial
	OOVPA_TABLE_PATCH(D3DDevice_GetBackMaterial_1_0_4627, XTL::EmuIDirect3DDevice8_GetBackMaterial),
	// IDirect3DDevice8::SetBackMaterial
	OOVPA_TABLE_PATCH(D3DDevice_SetBackMaterial_1_0_4627, XTL::EmuIDirect3DDevice8_SetBackMaterial),
	// IDirect3DCubeTexture8::LockRect
	// TODO: This needs to be verified on 4361, not just 4242!
    /*
	OOVPA_TABLE_PATCH(IDirect3DCubeTexture8_LockRect_1_0_3925, XTL::EmuIDirect3DCubeTexture8_LockRect),
	*/
};

// ******************************************************************
// * D3D8_1_0_4627_SIZE
// ******************************************************************
uint32 D3D8_1_0_4627_SIZE = sizeof(D3D8_1_0_4627);
