// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.4627.inl
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
// * D3D_CreateDeviceX (Direct3D_CreateDevice from D3D8LTCG)
// ******************************************************************
OOVPA_NO_XREF(D3D_CreateDeviceX, 4627, 8)

        // Direct3D_CreateDevice+0x07 : jnz +0x0A
        { 0x07, 0x75 },
        { 0x08, 0x0A },

        // Direct3D_CreateDevice+0x7F : repe stosd
        { 0x7F, 0xF3 },
        { 0x80, 0xAB },

        // Direct3D_CreateDevice+0x82 : mov eax, esi
        { 0x82, 0x8B },
        { 0x83, 0xC6 },

        // Direct3D_CreateDevice+0xA0 : retn 0x18
        { 0xA0, 0xC2 },
        { 0xA1, 0x18 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BeginVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginVisibilityTest, 4627, 11)

        // D3DDevice_BeginVisibilityTest+0x13 : mov dword ptr [eax], 0x000817C8
        { 0x13, 0xC7 },
        { 0x14, 0x00 },
        { 0x15, 0xC8 },
        { 0x16, 0x17 },
        { 0x17, 0x08 },

        // D3DDevice_BeginVisibilityTest+0x19 : mov ecx, 1
        { 0x19, 0xB9 },
        { 0x1A, 0x01 },
        { 0x1B, 0x00 },

        // D3DDevice_BeginVisibilityTest+0x24 : add eax, 0x0C
        { 0x24, 0x83 },
        { 0x25, 0xC0 },
        { 0x26, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3D_GetAdapterDisplayMode
// ******************************************************************
// Generic OOVPA as of 4627 and newer.
OOVPA_NO_XREF(D3D_GetAdapterDisplayMode, 4627, 13)

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x0A, 0x08 },
        { 0x0B, 0x76 },
        { 0x0C, 0x88 },
        { 0x0D, 0xC2 },
        { 0x0E, 0x08 },
        { 0x0F, 0x00 },
        { 0x10, 0x56 },
        { 0x11, 0x8B },
        { 0x12, 0x35 },

        { 0x30, 0xC2 },
        { 0x31, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle, 4627, 9)

        // D3D_KickOffAndWaitForIdle+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3D_KickOffAndWaitForIdle+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x30 },

        // D3D_KickOffAndWaitForIdle+0x08 : push 2
        { 0x08, 0x6A },
        { 0x09, 0x02 },

        // D3D_KickOffAndWaitForIdle+0x0A : push ecx
        { 0x0A, 0x51 },

        // D3D_KickOffAndWaitForIdle+0x0B : call [addr]
        { 0x0B, 0xE8 },

        // D3D_KickOffAndWaitForIdle+0x10 : retn
        { 0x10, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3D_KickOffAndWaitForIdle2 (D3D_KickOffAndWaitForIdle from D3D8LTCG)
// ******************************************************************
OOVPA_NO_XREF(D3D_KickOffAndWaitForIdle2, 4627, 11)

        // D3D_KickOffAndWaitForIdle2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3D_KickOffAndWaitForIdle2+0x05 : mov ecx, [eax+0x30]
        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x30 },

        // D3D_KickOffAndWaitForIdle2+0x08 : push 2
        { 0x08, 0x6A },
        { 0x09, 0x02 },

        // D3D_KickOffAndWaitForIdle2+0x0A : push ecx
        { 0x0A, 0x51 },

        // D3D_KickOffAndWaitForIdle2+0x0B : call [addr]
        { 0x0B, 0xE8 },

        // D3D_KickOffAndWaitForIdle2+0x18 : call [addr]
        { 0x18, 0xE8 },

        // D3D_KickOffAndWaitForIdle2+0x1D : retn 0x08
        { 0x1D, 0xC2 },
        { 0x1E, 0x08 },
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
// * D3D_CommonSetRenderTarget
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3D_CommonSetRenderTarget, 4627, 12)
#else
OOVPA_XREF(D3D_CommonSetRenderTarget, 4627, 1+12,

    XREF_D3D_CommonSetRenderTarget,
    XRefOne)

        XREF_ENTRY( 0x18, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived
#endif
        // D3D_CommonSetRenderTarget+0x00 : sub esp, 0x20
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x20 },

        // D3D_CommonSetRenderTarget+0x86 : jmp +0x06
        { 0x86, 0xEB },
        { 0x87, 0x06 },

        // D3D_CommonSetRenderTarget+0x88 : and eax, 0x0FFF
        { 0x88, 0x25 },
        { 0x89, 0xFF },
        { 0x8A, 0x0F },
        { 0x8B, 0x00 },

        // D3D_CommonSetRenderTarget+0xC6 : and ecx, 0x0F
        { 0xC6, 0x83 },
        { 0xC7, 0xE1 },
        { 0xC8, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3D_CommonSetRenderTargetB (D3D_CommonSetRenderTarget from D3D8LTCG)
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_XREF(D3D_CommonSetRenderTargetB, 4627, 11,
    XREF_D3D_CommonSetRenderTarget,
    XRefZero)
#else
OOVPA_XREF(D3D_CommonSetRenderTargetB, 4627, 1+11,

    XREF_D3D_CommonSetRenderTarget,
    XRefOne)

        XREF_ENTRY( 0x18, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived TODO : Verify offset
#endif
        // D3D_CommonSetRenderTargetB+0x00 : sub esp, 0x24
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x24 },

        // D3D_CommonSetRenderTargetB+0x86 : jmp +0x06
        { 0x8D, 0xEB },
        { 0x8E, 0x04 },

        // D3D_CommonSetRenderTargetB+0xE4 : and eax, 0x0FFF
        { 0xE4, 0x25 },
        { 0xE5, 0xFF },
        { 0xE6, 0x0F },
        { 0xE7, 0x00 },

        // D3D_CommonSetRenderTargetB+0xF4 : jge +0x06
        { 0xF4, 0x7D },
        { 0xF5, 0x06 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderTarget, 4627, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderTarget+0x10 : call [D3D_CommonSetRenderTarget]
        XREF_ENTRY( 0x10, XREF_D3D_CommonSetRenderTarget ),

        // D3DDevice_SetRenderTarget+0x00 : mov eax, [esp+0x08]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },

        // D3DDevice_SetRenderTarget+0x04 : mov ecx, [esp+0x04]
        { 0x04, 0x8B },
        { 0x05, 0x4C },
        { 0x06, 0x24 },
        { 0x07, 0x04 },

        // D3DDevice_SetRenderTarget+0x0D : push eax; push ecx
        { 0x0D, 0x50 },
        { 0x0E, 0x51 },

        // D3DDevice_SetRenderTarget+0x14 : retn 0x08
        { 0x14, 0xC2 },
        { 0x15, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4627, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0500]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x00 },
        { 0x08, 0x05 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x0500], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x00 },
        { 0x0F, 0x05 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetGammaRamp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetGammaRamp, 4627, 17) // Up to 5849

        { 0x25, 0x57 },
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        { 0x39, 0xF3 },
        { 0x3A, 0xA7 },

        { 0x5A, 0x89 },
        { 0x5B, 0x82 },
        { 0x5C, 0xE4 },

        { 0x70, 0x00 },
        { 0x71, 0xC7 },
        { 0x72, 0x84 },
        { 0x73, 0x82 },
        { 0x74, 0xDC },
        { 0x75, 0x07 },
        { 0x76, 0x00 },
        { 0x77, 0x00 },
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
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_XREF(D3DDevice_GetBackBuffer, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDevice_GetBackBuffer2 ),

        { 0x03, 0x04 },
        { 0x05, 0xE8 },

        { 0x0A, 0x8B },
        { 0x0B, 0x4C },
        { 0x0C, 0x24 },
        { 0x0D, 0x0C },
        { 0x0E, 0x89 },
        { 0x0F, 0x01 },
        { 0x10, 0xC2 },
        { 0x11, 0x0C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_XREF(D3DDevice_GetBackBuffer2, 4627, 12,

    XREF_D3DDevice_GetBackBuffer2,
    XRefZero)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0xFF },

        // D3DDevice_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 },
        { 0x0E, 0x19 },

        // D3DDevice_GetBackBuffer2+0x12 : mov eax, 1
        { 0x0F, 0xB8 },
        { 0x13, 0x00 },

        // D3DDevice_GetBackBuffer2+0x15 : mov esi, [ecx+eax*4+0xXXXX]
        { 0x15, 0x8B },
        { 0x16, 0xB4 },
        { 0x17, 0x81 },

        // D3DDevice_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 },
        { 0x41, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_XREF(D3DDevice_GetRenderTarget, 4627, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3DDevice_GetRenderTarget2 ),

        // D3DDevice_GetRenderTarget+0x00 : call addr
        { 0x00, 0xE8 },

        // D3DDevice_GetRenderTarget+0x05 : mov ecx, [esp + 0x04]
        { 0x05, 0x8B },
        { 0x06, 0x4C },
        { 0x07, 0x24 },
        { 0x08, 0x04 },

        // D3DDevice_GetRenderTarget+0x09 : mov [ecx], eax
        { 0x09, 0x89 },
        { 0x0A, 0x01 },

        // D3DDevice_GetRenderTarget+0x0B : xor eax, eax
        { 0x0B, 0x33 },
        { 0x0C, 0xC0 },

        // D3DDevice_GetRenderTarget+0x0D : ret 0x0004
        { 0x0D, 0xC2 },
        { 0x0E, 0x04 },
        { 0x0F, 0x00 },

OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget2
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_XREF(D3DDevice_GetRenderTarget2, 4627, 2+8,

    XREF_D3DDevice_GetRenderTarget2,
    XRefTwo)
#else
OOVPA_XREF(D3DDevice_GetRenderTarget2, 4627, 3+8,

    XREF_D3DDevice_GetRenderTarget2,
    XRefThree)
#endif
        XREF_ENTRY( 0x01, XREF_D3DDEVICE ), // Derived
#ifdef WIP_LessVertexPatching
        XREF_ENTRY( 0x08, XREF_OFFSET_D3DDEVICE_M_RENDERTARGET ), // Derived
#endif
        XREF_ENTRY( 0x12, XREF_D3DResource_AddRef ),

        // D3DDevice_GetRenderTarget2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetRenderTarget2+0x05 : push esi
        { 0x05, 0x56 },

        // D3DDevice_GetRenderTarget2+0x06 : mov esi, [eax + 0xXXXX]
        { 0x06, 0x8B },
        { 0x07, 0xB0 },
        //{ 0x08, 0xB4 }, // disabled. part of an offset
        //{ 0x09, 0x21 },

        // D3DDevice_GetRenderTarget2+0x0E : jz + 0x06
        { 0x0E, 0x74 },
        { 0x0F, 0x06 },

        // D3DDevice_GetRenderTarget2+0x11 : call [addr]
        { 0x11, 0xE8 },

        // D3DDevice_GetRenderTarget2+0x19 : retn
        { 0x19, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_XREF(D3DDevice_GetDepthStencilSurface, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3DDevice_GetDepthStencilSurface2 ),

        { 0x00, 0xE8 },

        { 0x06, 0x4C },
        { 0x15, 0x66 },
        { 0x16, 0x08 },
        { 0x17, 0x76 },
        { 0x18, 0x88 },
        { 0x19, 0x8B },
        { 0x1A, 0xC2 },
        { 0x1B, 0xC2 },
        { 0x1C, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2, 4627, 20)
#else
OOVPA_XREF(D3DDevice_GetDepthStencilSurface2, 4627, 1+20,

    XREF_D3DDevice_GetDepthStencilSurface2,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_OFFSET_D3DDEVICE_M_DEPTHSTENCIL ), // Derived
#endif
        { 0x00, 0xA1 },

        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x07, 0xB0 },
        //{ 0x08, 0xB8 },
        //{ 0x09, 0x21 },
        { 0x0A, 0x00 },
        { 0x0B, 0x00 },
        { 0x0C, 0x85 },
        { 0x0D, 0xF6 },
        { 0x0E, 0x75 },
        { 0x0F, 0x04 },
        { 0x10, 0x33 },
        { 0x11, 0xC0 },
        { 0x12, 0x5E },
        { 0x13, 0xC3 },
        { 0x14, 0x56 },
        { 0x15, 0xE8 },

        { 0x1A, 0x8B },
        { 0x1B, 0xC6 },
        { 0x1C, 0x5E },
        { 0x1D, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3D_SetTileNoWait
// ******************************************************************
OOVPA_XREF(D3D_SetTileNoWait, 4627, 11,

    XREF_D3D_SetTileNoWait,
    XRefZero)

        // D3D_SetTileNoWait+0x06 : sub esp, 0x18
        { 0x06, 0x83 },
        { 0x07, 0xEC },
        { 0x08, 0x18 },

        // D3D_SetTileNoWait+0x15 : cmp [esi+4], eax
        { 0x15, 0x39 },
        { 0x16, 0x46 },
        { 0x17, 0x04 },

        // D3D_SetTileNoWait+0x3D : lea edi, [edx+ecx*8+0xXXXX]
        { 0x3D, 0x8D },
        { 0x3E, 0xBC },
        { 0x3F, 0xCA },
        //{ 0x40, 0x60 },
        //{ 0x41, 0x22 },
        { 0x42, 0x00 },
        { 0x43, 0x00 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTile, 4627, 1+2,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3D_SetTileNoWait ),

        { 0x00, 0xE8 },
        { 0x05, 0xE9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateIndexBuffer2, 4627, 7,

    XREF_D3DDevice_CreateIndexBuffer2,
    XRefZero)

        { 0x05, 0xC0 },
        { 0x0F, 0x85 },
        { 0x13, 0xC2 },
        { 0x1A, 0x89 },
        { 0x21, 0x08 },
        { 0x28, 0x00 },
        { 0x2F, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateIndexBuffer
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateIndexBuffer, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDevice_CreateIndexBuffer2 ),

        { 0x00, 0x8B },

        { 0x0A, 0x8B },
        { 0x0B, 0x4C },
        { 0x0C, 0x24 },
        { 0x0D, 0x14 },
        { 0x0E, 0x33 },
        { 0x0F, 0xD2 },
        { 0x10, 0x85 },
        { 0x11, 0xC0 },

        { 0x1F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant, 4627, 13)

        { 0x00, 0x83 },
        { 0x01, 0xC1 },
        { 0x02, 0x60 },
        { 0x03, 0x83 },
        { 0x04, 0xF8 },
        { 0x05, 0x01 },
        { 0x06, 0x75 },
        { 0x07, 0x05 },
        { 0x08, 0xE9 },

        { 0x17, 0xC1 },
        { 0x18, 0xE0 },
        { 0x19, 0x02 },

        { 0x20, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1, 4627, 11)

        // D3DDevice_SetVertexShaderConstant1+0x06 : add eax, 0x1C
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0x1C },

        // D3DDevice_SetVertexShaderConstant1+0x0F : jnb +0x43
        { 0x0F, 0x73 },
        { 0x10, 0x43 },

        // D3DDevice_SetVertexShaderConstant1+0x27 : shl ecx, 0x04
        { 0x27, 0xC1 },
        { 0x28, 0xE1 },
        { 0x29, 0x04 },

        // D3DDevice_SetVertexShaderConstant1+0x53 : retn
        { 0x53, 0xC3 },

        // D3DDevice_SetVertexShaderConstant1+0x5D : jmp +0xA2
        { 0x5D, 0xEB },
        { 0x5E, 0xA2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant4
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant4, 4627, 12)

        // D3DDevice_SetVertexShaderConstant4+0x05 : add eax, 0x4C
        { 0x05, 0x83 },
        { 0x06, 0xC0 },
        { 0x07, 0x4C },

        // D3DDevice_SetVertexShaderConstant4+0x34 : movq mm7, qword ptr [edx+0x38]
        { 0x34, 0x0F },
        { 0x35, 0x6F },
        { 0x36, 0x7A },
        { 0x37, 0x38 },

        // D3DDevice_SetVertexShaderConstant4+0x42 : shl ecx, 0x04
        { 0x42, 0xC1 },
        { 0x43, 0xE1 },
        { 0x44, 0x04 },

        // D3DDevice_SetVertexShaderConstant4+0x91 : emms
        { 0x91, 0x0F },
        { 0x92, 0x77 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInline
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInline, 4627, 12) // (NOT 5344!)

        // D3DDevice_SetVertexShaderConstantNotInline+0x02 : test byte ptr ds:[abs], 0x10
        { 0x02, 0xF6 },
        { 0x07, 0x00 },
        { 0x08, 0x10 },

        // D3DDevice_SetVertexShaderConstantNotInline+0x0E : jnz +0x15
        { 0x0E, 0x75 },
        { 0x0F, 0x15 },

        // D3DDevice_SetVertexShaderConstantNotInline+0x42 : push esi; push edi; mov edi, eax
        { 0x10, 0x56 },
        { 0x11, 0x57 },
        { 0x12, 0x8B },
        { 0x13, 0xF8 },

        // D3DDevice_SetVertexShaderConstantNotInline+0x21 : rep movsd
        { 0x21, 0xF3 },
        { 0x22, 0xA5 },

        // D3DDevice_SetVertexShaderConstantNotInline+0x28 : call [abs]
        { 0x28, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstantNotInlineFast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstantNotInlineFast, 4627, 13)

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x00 : push esi
        { 0x00, 0x56 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x26 : cmp eax, 0x10
        { 0x26, 0x83 },
        { 0x27, 0xF8 },
        { 0x28, 0x10 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x29 : jnb +0x1F
        { 0x29, 0x73 },
        { 0x2A, 0x1F },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x33 : or eax, 0x0B80
        { 0x33, 0x0D },
        { 0x34, 0x80 },
        { 0x35, 0x0B },
        { 0x36, 0x00 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x3B : rep movsd
        { 0x3B, 0xF3 },
        { 0x3C, 0xA5 },

        // D3DDevice_SetVertexShaderConstantNotInlineFast+0x43 : emms
        { 0x43, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1Fast
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1Fast, 4627, 11)

        // D3DDevice_SetVertexShaderConstant1Fast+0x05 : add eax, 0x1C
        { 0x05, 0x83 },
        { 0x06, 0xC0 },
        { 0x07, 0x1C },

        // D3DDevice_SetVertexShaderConstant1Fast+0x0E : jnb +0x2E
        { 0x0E, 0x73 },
        { 0x0F, 0x2E },

        // D3DDevice_SetVertexShaderConstant1Fast+0x28 : mov [eax-0x10], ecx
        { 0x28, 0x89 },
        { 0x29, 0x48 },
        { 0x2A, 0xF0 },

        // D3DDevice_SetVertexShaderConstant1Fast+0x3D : retn
        { 0x3D, 0xC3 },

        // D3DDevice_SetVertexShaderConstant1Fast+0x47 : jmp +0xB7
        { 0x47, 0xEB },
        { 0x48, 0xB7 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4627, 16)
#else
OOVPA_XREF(D3DDevice_SetPixelShader, 4627, 1+16,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0A, XREF_OFFSET_D3DDEVICE_M_PIXELSHADER ), // Derived
#endif
        { 0x03, 0x1D },

        { 0x0C, 0x00 },
        { 0x0D, 0x00 },
        { 0x0E, 0x89 },
        { 0x0F, 0x44 },
        { 0x10, 0x24 },
        { 0x11, 0x04 },
        { 0x12, 0x8B },
        { 0x13, 0x44 },
        { 0x14, 0x24 },
        { 0x15, 0x0C },
        { 0x16, 0x85 },
        { 0x17, 0xC0 },
        { 0x18, 0x89 },

        { 0x69, 0xC7 },
        { 0x76, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 4627, 13)

        // D3DDevice_CreateTexture2+0x02 : lea eax, [esp+0x20]
        { 0x02, 0x8D },
        { 0x05, 0x20 },

        // D3DDevice_CreateTexture2+0x0E : setz dl
        { 0x0E, 0x0F },
        { 0x0F, 0x94 },
        { 0x10, 0xC2 },

        // D3DDevice_CreateTexture2+0x11 : cmp eax, 5
        { 0x11, 0x83 },
        { 0x12, 0xF8 },
        { 0x13, 0x05 },

        // D3DDevice_CreateTexture2+0x53 : call [abs]
        { 0x53, 0xE8 },

        // D3DDevice_CreateTexture2+0x78 : jnz +0x0D
        { 0x78, 0x75 },
        { 0x79, 0x0D },

        // D3DDevice_CreateTexture2+0xAE : retn 0x1C
        { 0xAE, 0xC2 },
        { 0xAF, 0x1C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateTexture, 4627, 11)

        { 0x03, 0x14 },

        { 0x13, 0x51 },
        { 0x14, 0x8B },
        { 0x15, 0x4C },
        { 0x16, 0x24 },
        { 0x17, 0x10 },
        { 0x18, 0x52 },
        { 0x19, 0x6A },
        { 0x1A, 0x01 },

        { 0x23, 0x54 },
        { 0x38, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 4627, 10)

        // D3DDevice_RunPushBuffer+0x3C : mov edx, [esi+30h]
        { 0x3C, 0x8B },
        { 0x3D, 0x56 },
        { 0x3E, 0x30 },

        // D3DDevice_RunPushBuffer+0xCA : mov edi, ebp
        { 0xCA, 0x8B },
        { 0xCB, 0xFD },

        // D3DDevice_RunPushBuffer+0xDC : jmp +0x12
        { 0xDC, 0xEB },
        { 0xDD, 0x12 },

        // D3DDevice_RunPushBuffer+0xED : lea edi, [eax+4]
        { 0xED, 0x8D },
        { 0xEE, 0x78 },
        { 0xEF, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4627, 13)

        { 0x2A, 0xBB },
        { 0x2B, 0x05 },
        { 0x37, 0xF6 },
        { 0x38, 0xC3 },
        { 0x39, 0x03 },

        { 0x73, 0xEB },
        { 0x74, 0x36 },
        { 0x75, 0x8B },
        { 0x76, 0xFB },
        { 0x77, 0x83 },
        { 0x78, 0xE7 },
        { 0x79, 0x04 },
        { 0x7A, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateVertexBuffer, 4627, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDevice_CreateVertexBuffer2 ),

        { 0x00, 0x8B },

        { 0x0A, 0x8B },
        { 0x0B, 0x4C },
        { 0x0C, 0x24 },
        { 0x0D, 0x14 },
        { 0x0E, 0x33 },
        { 0x0F, 0xD2 },
        { 0x10, 0x85 },
        { 0x11, 0xC0 },
        { 0x12, 0x0F },

        { 0x1E, 0x8B },
        { 0x1F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVertexBuffer2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateVertexBuffer2, 4627, 13,

    XREF_D3DDevice_CreateVertexBuffer2,
    XRefZero)

        // D3DDevice_CreateVertexBuffer2+0x03 : push 0x40
        { 0x03, 0x6A },
        { 0x04, 0x40 },

        // D3DDevice_CreateVertexBuffer2+0x10 : mov eax, [esp+8]
        { 0x10, 0x8B },
        { 0x11, 0x44 },
        { 0x12, 0x24 },
        { 0x13, 0x08 },

        // D3DDevice_CreateVertexBuffer2+0x41 : mov dword ptr [esi], 0x01000001
        { 0x41, 0xC7 },
        { 0x42, 0x06 },
        { 0x43, 0x01 },
        { 0x44, 0x00 },
        { 0x46, 0x01 },

        // D3DDevice_CreateVertexBuffer2+0x4A : retn 0x04
        { 0x4A, 0xC2 },
        { 0x4B, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4627, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2554
        { 0x17, 0x05 },
        { 0x18, 0x54 },
        { 0x19, 0x25 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4627, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x2550], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x50 },
        { 0x0D, 0x25 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_D3DTSS_TEXCOORDINDEX ), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x14 : shl eax, 0x07
        { 0x14, 0xC1 },
        { 0x15, 0xE0 },
        { 0x16, 0x07 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x26 : cmp eax, ecx
        { 0x26, 0x3B },
        { 0x27, 0xC1 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x71 : mov ebp, 0x2400
        { 0x71, 0xBD },
        { 0x73, 0x24 },

        // D3DDevice_SetTextureState_TexCoordIndex+0xAA : shl ebx, 0x04
        { 0xAA, 0xC1 },
        { 0xAB, 0xE3 },
        { 0xAC, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4627, 12)

        { 0x0D, 0x8B },
        { 0x0E, 0x87 },

        { 0x50, 0x40 },
        { 0x51, 0x04 },
        { 0x52, 0x00 },
        { 0x53, 0x00 },
        { 0x54, 0x00 },
        { 0x55, 0x00 },
        { 0x56, 0xC1 },
        { 0x57, 0xE6 },

        { 0x74, 0xC2 },
        { 0x75, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4627, 17)

        { 0x04, 0x56 },

        { 0x13, 0xE8 },

        { 0x48, 0xC7 },
        { 0x50, 0x04 },
        { 0x51, 0x8B },
        { 0x52, 0x4E },
        { 0x53, 0x04 },
        { 0x54, 0x83 },
        { 0x55, 0xC0 },
        { 0x56, 0x08 },
        { 0x57, 0x3B },
        { 0x58, 0xC1 },
        { 0x59, 0x89 },
        { 0x5A, 0x06 },
        { 0x5B, 0x72 },
        { 0x5C, 0x05 },
        { 0x5D, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_DrawIndexedVertices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVertices, 4627, 11)

        // D3DDevice_DrawIndexedVertices+0x0E : mov eax, [esi+0x1C]
        { 0x0E, 0x8B },
        { 0x0F, 0x46 },
        { 0x10, 0x1C },

        // D3DDevice_DrawIndexedVertices+0x26 : push 0x0209
        { 0x26, 0x68 },
        { 0x27, 0x09 },
        { 0x28, 0x02 },

        // D3DDevice_DrawIndexedVertices+0x66 : dec eax
        { 0x66, 0x48 },

        // D3DDevice_DrawIndexedVertices+0xFB : prefetchnta byte ptr [esi+0x3C]
        { 0xFB, 0x0F },
        { 0xFC, 0x18 },
        { 0xFD, 0x46 },
        { 0xFE, 0x3C },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4627, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0AB0
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0xB0 },
        { 0x0F, 0x0A },

        // D3DDevice_SetMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 },
        { 0x13, 0x11 },

        // D3DDevice_SetMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 },
        { 0x20, 0xC9 },
        { 0x21, 0x00 },
        { 0x22, 0x10 },

        // D3DDevice_SetMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
OOVPA_XREF(D3DVertexBuffer_Lock2, 4627, 12,

    XREF_D3DVertexBuffer_Lock2,
    XRefZero)

        // D3DVertexBuffer_Lock2B+0x00 : push ebx
        { 0x00, 0x53 },

        // D3DVertexBuffer_Lock2+0x01 : mov bl, [esp+0x0C]
        { 0x01, 0x8A },
        { 0x02, 0x5C },
        { 0x03, 0x24 },
        { 0x04, 0x0C },

        // D3DVertexBuffer_Lock2B+0x09 : jnz +0x24
        { 0x09, 0x75 },
        { 0x0A, 0x24 },

        // D3DVertexBuffer_Lock2+0x3E : mov eax, [esi+4]
        { 0x3E, 0x8B },
        { 0x3F, 0x46 },
        { 0x40, 0x04 },

        // D3DVertexBuffer_Lock2+0x48 : retn 0x08
        { 0x48, 0xC2 },
        { 0x49, 0x08 },
OOVPA_END;

// ******************************************************************
// * D3DVertexBuffer_Lock
// ******************************************************************
OOVPA_XREF(D3DVertexBuffer_Lock, 4627, 1+25,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DVertexBuffer_Lock2 ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x14 },
        { 0x04, 0x8B },
        { 0x05, 0x4C },
        { 0x06, 0x24 },
        { 0x07, 0x04 },
        { 0x08, 0x50 },
        { 0x09, 0x51 },
        { 0x0A, 0xE8 },

        { 0x0F, 0x8B },
        { 0x10, 0x4C },
        { 0x11, 0x24 },
        { 0x12, 0x08 },
        { 0x13, 0x8B },
        { 0x14, 0x54 },
        { 0x15, 0x24 },
        { 0x16, 0x10 },
        { 0x17, 0x03 },
        { 0x18, 0xC1 },
        { 0x19, 0x89 },
        { 0x1A, 0x02 },
        { 0x1B, 0xC2 },
        { 0x1C, 0x14 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetSurfaceLevel2
// ******************************************************************
OOVPA_XREF(D3DTexture_GetSurfaceLevel2, 4627, 12,

    XREF_D3DTexture_GetSurfaceLevel2,
    XRefZero)

        // D3DTexture_GetSurfaceLevel2+0x00 : sub esp, 0x0C
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x0C },

        // D3DTexture_GetSurfaceLevel2+0x04 : mov esi, [esp+0x14]
        { 0x04, 0x8B },
        { 0x07, 0x14 },

        // D3DTexture_GetSurfaceLevel2+0x08 : lea eax, [esp+0x14]
        { 0x08, 0x8D },
        { 0x0B, 0x14 },

        // D3DTexture_GetSurfaceLevel2+0x17 : mov edx, [esp+0x24]
        { 0x17, 0x8B },
        { 0x1A, 0x24 },

        // D3DTexture_GetSurfaceLevel2+0x1B : lea eax, [esp+0x18]
        { 0x1B, 0x8D },
        { 0x1E, 0x18 },

        // D3DTexture_GetSurfaceLevel2+0x3E : call [abs]
        { 0x3E, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetSurfaceLevel
// ******************************************************************
OOVPA_XREF(D3DTexture_GetSurfaceLevel, 4627, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DTexture_GetSurfaceLevel2 ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },

        { 0x0A, 0xE8 },

        { 0x0F, 0x8B },
        { 0x10, 0x54 },
        { 0x11, 0x24 },
        { 0x12, 0x0C },
        { 0x13, 0x33 },
        { 0x14, 0xC9 },
        { 0x15, 0x85 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 4627, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0xD8 },
        { 0x08, 0x20 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
        { 0x0E, 0x04 },
        { 0x0F, 0x89 },
        { 0x10, 0x0A },
        { 0x11, 0xC2 },
        { 0x12, 0x04 },
        { 0x13, 0x00 },
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
// * CMiniport_IsFlipPending
// ******************************************************************
OOVPA_NO_XREF(CMiniport_IsFlipPending, 4627, 18) // Was D3DDevice_Reset

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
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 4627, 16) // Up to 5233

        { 0x02, 0x35 },

        { 0x07, 0x57 },

        { 0x0E, 0x30 }, // 4361 0x7004
        { 0x0F, 0x05 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0xB9 },
        { 0x13, 0x10 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0xF3 },
        { 0x18, 0xA5 },
        { 0x19, 0x5F },
        { 0x1A, 0x5E },
        { 0x1B, 0xC2 },
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
// * D3DDevice_GetTexture, named with 2 suffix to match EMUPATCH(D3DDevice_GetTexture2)
// ******************************************************************
#ifndef WIP_LessVertexPatching
OOVPA_NO_XREF(D3DDevice_GetTexture2, 4627, 23)
#else
OOVPA_XREF(D3DDevice_GetTexture2, 4627, 1+23, // Up to 5233. Also for 5344, 5558, 5659, 5788, 5849, 5933

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0E, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived
#endif
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        { 0x0A, 0x56 },
        { 0x0B, 0x8D },
        { 0x0C, 0xB4 },
        { 0x0D, 0x81 },
        { 0x0E, 0x38 }, // GetTexture2 380B vs GetPalette2 480B
        { 0x0F, 0x0B },

        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x8B },
        { 0x13, 0x06 },
        { 0x14, 0x85 },
        { 0x15, 0xC0 },
        { 0x16, 0x74 },

        { 0x18, 0x50 },
        { 0x19, 0xE8 },

        { 0x1E, 0x8B },
        { 0x1F, 0x06 },
//        { 0x21, 0xC2 },
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
        { 0x4A, 0x81 },
        { 0x4B, 0xCA },
        { 0x4C, 0x00 },
        { 0x4D, 0x08 },

        // D3DDevice_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x784]
        { 0x5C, 0x8D },
        { 0x5D, 0x97 },
        { 0x5E, 0x84 },
        { 0x5F, 0x07 },

        // D3DDevice_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 },
        { 0x6B, 0x10 },
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
OOVPA_NO_XREF(D3DDevice_PersistDisplay, 4627, 13)

        { 0x00, 0x51 },
        { 0x01, 0x53 },
        { 0x02, 0x8B },
        { 0x03, 0x1D },

        { 0x0E, 0x85 },
        { 0x0F, 0xC0 },
        { 0x10, 0x74 },
        { 0x11, 0x0F },
        { 0x12, 0x50 },
        { 0x13, 0xFF },
        { 0x14, 0x15 },

        { 0x21, 0x8B },
        { 0x42, 0xE8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
/* See the comment for the 4134 OOVPA signature for previous notes */
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4627, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x254C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x4C },
        { 0x0D, 0x25 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
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
OOVPA_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DRS_MULTISAMPLERENDERTARGETMODE ), // Derived

        { 0x03, 0x04 },

        { 0x19, 0x00 },
        { 0x1A, 0x00 },
        { 0x1B, 0x74 },
        { 0x1C, 0x14 },
        { 0x1D, 0x8D },
        { 0x1E, 0x90 },
        { 0x1F, 0x90 },
        { 0x20, 0x0A },

        { 0x31, 0xC2 },
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
// * D3D::BlockOnTime
// ******************************************************************
OOVPA_XREF(D3D_BlockOnTime, 4627, 6,

    XREF_D3D_BlockOnTime,
    XRefZero)

        { 0x09, 0x34 },
        { 0x27, 0x07 },
        { 0x55, 0x7E },
        { 0x7B, 0x58 },
        { 0xE3, 0x80 },
        { 0xF5, 0x30 },
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
// * D3DCubeTexture_GetCubeMapSurface
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface, 4627, 16)

        { 0x02, 0x24 },
        { 0x0D, 0x51 },
        { 0x0E, 0x52 },
        { 0x0F, 0xE8 },

        { 0x14, 0x8B },
        { 0x15, 0x4C },
        { 0x16, 0x24 },
        { 0x17, 0x10 },
        { 0x18, 0x33 },
        { 0x19, 0xD2 },
        { 0x1A, 0x85 },
        { 0x1B, 0xC0 },
        { 0x1C, 0x0F },
        { 0x1D, 0x95 },
        { 0x2A, 0xC2 },
        { 0x2B, 0x10 },
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
// * D3DDevice_CreatePalette2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreatePalette2, 4627, 8,

    XREF_D3DDevice_CreatePalette2,
    XRefZero)

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
// * D3DDevice_CreatePalette
// ******************************************************************
OOVPA_XREF(D3DDevice_CreatePalette, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDevice_CreatePalette2 ),

        { 0x00, 0x8B },
        { 0x0A, 0x8B },

        { 0x13, 0x95 },
        { 0x14, 0xC2 },
        { 0x15, 0x89 },
        { 0x16, 0x01 },
        { 0x17, 0x4A },
        { 0x18, 0x81 },
        { 0x19, 0xE2 },
        { 0x1A, 0x0E },
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock2
// ******************************************************************
OOVPA_XREF(D3DPalette_Lock2, 4627, 7,

    XREF_D3DPalette_Lock2,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x8B },
        { 0x0A, 0x75 },
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
OOVPA_END;

// ******************************************************************
// * D3DPalette_Lock
// ******************************************************************
OOVPA_XREF(D3DPalette_Lock, 4627, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0B, XREF_D3DPalette_Lock2 ),

        { 0x01, 0x44 },
        { 0x08, 0x50 },
        { 0x0A, 0xE8 },
        { 0x0F, 0x8B },
        { 0x10, 0x54 },
        { 0x13, 0x89 },
        { 0x14, 0x02 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetPushBufferOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPushBufferOffset, 4627, 10)

        { 0x02, 0x35 },
        { 0x08, 0x86 },
        { 0x18, 0x00 },
        { 0x19, 0x2B },
        { 0x1A, 0xC1 },
        { 0x1B, 0x99 },
        { 0x1C, 0x83 },
        { 0x1D, 0xE2 },
        { 0x1E, 0x07 },
        { 0x1F, 0x03 },
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
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 4627, 10)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0xF4 },
        { 0x0F, 0x0A },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x10 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
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
// * D3DDevice_SetRenderState_SampleAlpha
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_SampleAlpha, 4627, 7)

        { 0x0B, 0xC1 },
        { 0x15, 0x0B },
        { 0x1C, 0x8B },
        { 0x28, 0x85 },
        { 0x30, 0x07 },
        { 0x3B, 0xC7 },
        { 0x44, 0x83 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateVolumeTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateVolumeTexture, 4627, 14)

        { 0x03, 0x18 },
        { 0x0B, 0x10 },
        { 0x0C, 0x6A },
        { 0x0D, 0x04 },
        { 0x0E, 0x50 },
        { 0x0F, 0x8B },
        { 0x10, 0x44 },
        { 0x11, 0x24 },
        { 0x12, 0x14 },
        { 0x13, 0x51 },
        { 0x14, 0x8B },
        { 0x15, 0x4C },

        { 0x28, 0x20 },
        { 0x3B, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateCubeTexture
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateCubeTexture, 4627, 11)

        { 0x03, 0x10 },
        { 0x10, 0x44 },
        { 0x11, 0x24 },
        { 0x12, 0x0C },
        { 0x13, 0x51 },
        { 0x14, 0x52 },
        { 0x15, 0x6A },
        { 0x16, 0x01 },
        { 0x17, 0x50 },
        { 0x18, 0x50 },

        { 0x1F, 0x4C },
OOVPA_END;

// ******************************************************************
// * IDirect3DVertexBuffer8_Lock
// ******************************************************************
OOVPA_XREF(IDirect3DVertexBuffer8_Lock, 4627, 1+10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x03, XREF_D3DVertexBuffer_Lock2 ),

        { 0x00, 0x50 },
        { 0x01, 0x51 },
        { 0x02, 0xE8 },

        { 0x07, 0x8B },
        { 0x08, 0x4C },
        { 0x09, 0x24 },
        { 0x0A, 0x04 },
        { 0x0B, 0x8B },
        { 0x0C, 0x54 },
        { 0x0D, 0x24 },
OOVPA_END;
