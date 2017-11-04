// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.1.0.5788.inl
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

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_EndVisibilityTest
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndVisibilityTest, 5788, 7)

        { 0x10, 0x09 },
        { 0x12, 0x0E },
        { 0x18, 0x04 },
        { 0x34, 0xCC },
        { 0x3D, 0x0E },
        { 0x50, 0x0C },
        { 0x58, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5455 version
// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 5788, 8)

        { 0x09, 0x05 },
        { 0x14, 0x04 },
        { 0x1D, 0x08 },
        { 0x20, 0x0C },
        { 0x26, 0x10 },
        { 0x2B, 0x40 },
        { 0x2F, 0x14 },
        { 0x31, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale, 5788, 7)

        { 0x1E, 0x24 },
        { 0x3E, 0x06 },
        { 0x62, 0xD9 },
        { 0x7E, 0xC4 },
        { 0x9E, 0x81 },
        { 0xBE, 0xFF },
        { 0xDE, 0xC0 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 5788, 12)

        // D3DDevice_GetBackBuffer2+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0xFF },

        // D3DDevice_GetBackBuffer2+0x0D : jnz +0x19
        { 0x0D, 0x75 },
        { 0x0E, 0x19 },

        // D3DDevice_GetBackBuffer2+0x18 : mov esi, [ecx+eax*4+0x21C0]
        { 0x15, 0x8B },
        { 0x16, 0xB4 },
        { 0x17, 0x81 },
        { 0x18, 0x14 },
        { 0x19, 0x1A },

        // D3DDevice_GetBackBuffer2+0x40 : retn 0x04
        { 0x40, 0xC2 },
        { 0x41, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
OOVPA_XREF(D3DDevice_GetDepthStencilSurface2, 5788, 1+10,
	
	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x08, XREF_OFFSET_D3DDEVICE_M_DEPTHSTENCIL ), // Derived

        // D3DDevice_GetDepthStencilSurface2+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetDepthStencilSurface2+0x05 : push esi
        { 0x05, 0x56 },

        // D3DDevice_GetDepthStencilSurface2+0x06 : mov esi, [eax + 0x1A08]
        { 0x06, 0x8B },
        { 0x07, 0xB0 },
        { 0x08, 0x08 },
        { 0x09, 0x1A },

        // D3DDevice_GetDepthStencilSurface2+0x0E : jnz +0x04
        { 0x0E, 0x75 },
        { 0x0F, 0x04 },

        // D3DDevice_GetDepthStencilSurface2+0x15 : call [addr]
        { 0x15, 0xE8 },

        // D3DDevice_GetDepthStencilSurface2+0x1D : retn
        { 0x1D, 0xC3 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_CreatePixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreatePixelShader, 5788, 9)

        { 0x00, 0x68 },
        { 0x07, 0x00 },
        { 0x10, 0xC0 },
        { 0x19, 0x08 },
        { 0x22, 0x78 },
        { 0x2B, 0x00 },
        { 0x34, 0xB9 },
        { 0x3D, 0x24 },
        { 0x46, 0x08 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 5788, 7)

        { 0x09, 0x84 },
        { 0x14, 0x46 },
        { 0x1F, 0x24 },
        { 0x2A, 0xBA },
        { 0x35, 0x4A },
        { 0x40, 0x5E },
        { 0x4D, 0x5E },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 5788, 8)

        { 0x1F, 0x05 },
        { 0x3A, 0x89 },
        { 0x64, 0xF0 },
        { 0x73, 0x13 },
        { 0x94, 0xAF },
        { 0xAB, 0x12 },
        { 0xE4, 0xDB },
        { 0xFF, 0x0C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5788, 9)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x00 },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 5788, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x00 },
        { 0x0E, 0x0F },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;
#endif

#if 0 // Moved to 5028
// ******************************************************************
// * D3DDevice_BeginStateBig
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginStateBig, 5788, 15)

        { 0x01, 0x0D },
        { 0x07, 0x01 },
        { 0x10, 0x8D },
        { 0x16, 0x02 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x3B },
        { 0x1A, 0xD1 },
        { 0x1B, 0x72 },
        { 0x1C, 0x23 },
        { 0x1D, 0xA1 },
        { 0x22, 0x8B },
        { 0x2B, 0x00 },
        { 0x34, 0xCA },
        { 0x40, 0x5E },
OOVPA_END;
#endif

#if 0 // This one _IDirect3DVertexBuffer8_Lock@20, replaced by generic 4627 version
// ******************************************************************
// * D3DVertexBuffer8_Lock
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock, 5788, 8)

        { 0x01, 0x51 },
        { 0x07, 0x8B },
        { 0x08, 0x4C },
        { 0x0A, 0x04 },
        { 0x0D, 0x24 },
        { 0x10, 0xC1 },
        { 0x13, 0x33 },
        { 0x16, 0x08 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DVertexBuffer_Lock2
// ******************************************************************
OOVPA_NO_XREF(D3DVertexBuffer_Lock2, 5788, 7)

        { 0x08, 0x56 },
        { 0x12, 0x06 },
        { 0x1D, 0xC7 },
        { 0x26, 0x00 },
        { 0x30, 0xC3 },
        { 0x3E, 0x8B },
        { 0x44, 0x00 },
OOVPA_END;
#endif

#if 0 // Moved to 5455
// ******************************************************************
// * D3D_AllocContiguousMemory
// ******************************************************************
OOVPA_NO_XREF(D3D_AllocContiguousMemory, 5788, 7)

        { 0x05, 0x10 },
        { 0x0C, 0x00 },
        { 0x13, 0x00 },
        { 0x1A, 0x08 },
        { 0x21, 0xBC },
        { 0x28, 0x0D },
        { 0x2F, 0x24 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DTexture8_GetLevelDesc
// ******************************************************************
// * NOTE: We are actually intercepting Get2DSurfaceDesc, because
// *       GetLevelDesc Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 5788, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F },
        { 0x2C, 0xB6 },
        { 0x2D, 0x57 },
        { 0x2E, 0x0D },

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x1A14]
        { 0x52, 0x8B },
        { 0x53, 0x90 },
        { 0x54, 0x14 },
        { 0x55, 0x1A },

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 },
        { 0xAF, 0x0C },
OOVPA_END;
#endif

#if 0 // Used 5028
// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateTexture2, 5788, 8,

    XRefNoSaveIndex, // ?? XREF_D3DDevice_CreateTexture2,
    XRefZero)

        { 0x14, 0x4C },
        { 0x2A, 0x8B },
        { 0x42, 0x8B },
        { 0x56, 0x80 },
        { 0x70, 0x85 },
        { 0x82, 0x5E },
        { 0x98, 0x89 },
        { 0xAE, 0x1C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_SetVertexShaderConstant1
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderConstant1, 5788, 11)

        // D3DDevice_SetVertexShaderConstant1+0x05 : add eax, 0x1C
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0x1C },

        // D3DDevice_SetVertexShaderConstant1+0x0E : jnb +0x43
        { 0x0F, 0x73 },
        { 0x10, 0x43 },

        // D3DDevice_SetVertexShaderConstant1+0x28 : mov [eax-0x18], ecx
        { 0x1D, 0x89 },
        { 0x1E, 0x48 },
        { 0x1F, 0xE8 },

        // D3DDevice_SetVertexShaderConstant1+0x3D : retn
        { 0x53, 0xC3 },

        // D3DDevice_SetVertexShaderConstant1+0x47 : jmp +0xA2
        { 0x5D, 0xEB },
        { 0x5E, 0xA2 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 5788, 8)

        { 0x0D, 0x05 },
        { 0x17, 0x8D },
        { 0x19, 0xCD },
        { 0x1F, 0x4C },
        { 0x25, 0x54 },
        { 0x27, 0x10 },
        { 0x30, 0x0C },
        { 0x35, 0x0C },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 5788, 8)

        { 0x1E, 0x72 },
        { 0x3E, 0xC0 },
        { 0x5E, 0xC6 },
        { 0x7E, 0x14 },
        { 0x9E, 0x00 },
        { 0xBE, 0x81 },
        { 0xDE, 0x8B },
        { 0xFE, 0x7D },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 5788, 6)

        { 0x09, 0x00 },
        { 0x11, 0x8B },
        { 0x2F, 0x08 },
        { 0x43, 0x57 },
        { 0x58, 0xC8 },
        { 0x63, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 5788, 6)

        { 0x09, 0x00 },
        { 0x15, 0x50 },
        { 0x47, 0x7B },
        { 0x61, 0x78 },
        { 0x77, 0x07 },
        { 0x8C, 0xC3 },
OOVPA_END;
#endif

#if 0 // Moved to 5558
// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 5788, 6)

        { 0x0E, 0xED },
        { 0x42, 0x18 },
        { 0x62, 0x04 },
        { 0x8D, 0x1C },
        { 0xAF, 0x0F },
        { 0xF6, 0x41 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_XREF(D3DDevice_SetVerticalBlankCallback, 5788, 1+6,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE ), // Derived

        { 0x01, 0x44 },
        { 0x03, 0x04 },
        { 0x05, 0x0D },
        { 0x0C, 0xB8 },
        { 0x0D, 0x1D },
        { 0x11, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 5788, 7)

        { 0x05, 0x6A },
        { 0x08, 0x00 },
        { 0x0D, 0xC0 },
        { 0x12, 0x00 },
        { 0x17, 0x05 },
        { 0x1C, 0x50 },
        { 0x23, 0xC3 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5788, 10)

        { 0x04, 0x08 },
        { 0x0C, 0x81 },
        { 0x0E, 0x44 },
        { 0x0F, 0x0F },
        { 0x10, 0x00 },
        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * IDirectD3DDevice8_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 5788, 8)

        { 0x07, 0x8B },
        { 0x0C, 0x00 },
        { 0x13, 0xCF },
        { 0x1A, 0x28 },
        { 0x21, 0x05 },
        { 0x28, 0x00 },
        { 0x2F, 0x89 },
        { 0x36, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3D::KickOffAndWaitForIdle
// ******************************************************************
/*OOVPA_XREF(D3D_KickOffAndWaitForIdle, 5788, 5,

    XREF_D3D_KickOffAndWaitForIdle,
    XRefZero)

        { 0x05, 0x8B },
        { 0x06, 0x48 },
        { 0x07, 0x2C },
        { 0x0A, 0x51 },
        { 0x10, 0xC3 },
OOVPA_END;
*/

#if 0 // No longer used, replaced by generic 5455 version
// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
// Accurately this one ?SetTileNoWait@D3D@@YGXKPBU_D3DTILE@@@Z
OOVPA_NO_XREF(D3DDevice_SetTile, 5788, 10)

	// D3DDevice_SetTile+0x00: sub esp, 0x1C
	{ 0x00, 0x83 },
	{ 0x01, 0xEC },
	{ 0x02, 0x1C },
	// D3DDevice_SetTile+0x45: lea ebx, [ebx+ebp*8+0x1AC0]
	{ 0x45, 0x8D },
	{ 0x46, 0x9C },
	{ 0x47, 0xEB },
	{ 0x48, 0xC0 },
	{ 0x49, 0x1A },
	{ 0x4A, 0x00 },
	{ 0x4B, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5788, 7)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xB4 },
        { 0x0D, 0x1D },
        { 0x10, 0xC2 },
OOVPA_END;
#endif

#if 0 // Moved to 5849
// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 5788, 8)

        { 0x0C, 0x89 },
        { 0x1E, 0x8B },
        { 0x28, 0x74 },
        { 0x36, 0xD2 },
        { 0x44, 0x3D },
        { 0x52, 0xC7 },
        { 0x60, 0x04 },
        { 0x6E, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice8::SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 5788, 13)

        // D3DDevice_SetRenderState_LineWidth+0x00 : push ebx
        { 0x00, 0x53 },

        // D3DDevice_SetRenderState_LineWidth+0x0C : mov [esp+8+argo_0], ebx
        { 0x0C, 0x89 },
        { 0x0D, 0x5C },
        { 0x0E, 0x24 },
        { 0x0F, 0x0C },

        // D3DDevice_SetRenderState_LineWidth+0x28 : fstp [esp+10h+var_10]
        { 0x28, 0xD9 },
        { 0x29, 0x1C },
        { 0x2A, 0x24 },

        // D3DDevice_SetRenderState_LineWidth+0x30 : mov edi, eax
        { 0x30, 0x8B },
        { 0x31, 0xF8 },

        // D3DDevice_SetRenderState_LineWidth+0x41 : cmp eax, [esi+4]
        { 0x41, 0x3B },
        { 0x42, 0x46 },
        { 0x43, 0x04 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4432 version
// ******************************************************************
// * D3DDevice8::SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes, 5788, 18)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x0C },
        { 0x04, 0x48 },
        { 0x05, 0x83 },
        { 0x06, 0xF8 },
        { 0x07, 0x03 },
        { 0x08, 0x56 },
        { 0x09, 0x8B },
        { 0x0A, 0x35 },

        { 0x0F, 0x77 },
        { 0x10, 0x5F },

        { 0x11, 0xFF },
        { 0x12, 0x24 },
        { 0x13, 0x85 },

        { 0x7C, 0x8B },
        { 0x80, 0xE8 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5788 version
// ******************************************************************
// * D3DDevice8::SetDepthClipPlanes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetDepthClipPlanes, 5788, 13)

        // _D3DDevice_SetDepthClipPlanes+0x00 : mov eax, [esp+Flags]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x0C },

        // _D3DDevice_SetDepthClipPlanes+0x00 : ja short loc_27ABD0 ; jumptable 0027AB71 default case
        { 0x0F, 0x77 },
        { 0x10, 0x5F },

        // _D3DDevice_SetDepthClipPlanes+0x00 : jmp ds:off_27ABEC[eax*4] ; switch jump
        { 0x11, 0xFF },
        { 0x12, 0x24 },
        { 0x13, 0x85 },
        { 0x14, 0xEC },
        { 0x15, 0xAB },
        { 0x16, 0x27 },
        { 0x17, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_XREF(D3DDevice_SetPalette, 5788, 1+7,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x10, XREF_OFFSET_D3DDEVICE_M_PALETTES ), // Derived

        { 0x10, 0x98 },
        { 0x22, 0x0F },
        { 0x34, 0x00 },
        { 0x46, 0x9C },
        { 0x58, 0xC5 },
        { 0x6E, 0x8B },
        { 0x7C, 0x89 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_GetPushBufferOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPushBufferOffset, 5788, 7)

        { 0x13, 0xCA },
        { 0x28, 0xFF },
        { 0x3D, 0x89 },
        { 0x52, 0x12 },
        { 0x67, 0xAB },
        { 0x7C, 0x51 },
        { 0x91, 0x5F },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5558 version
// ******************************************************************
// * D3D::MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 5788, 28, // Also for 5849, 5933

	XREF_D3D_MakeRequestedSpace,
	XRefZero)
	
		{ 0x00, 0x51 },
		{ 0x01, 0x56 },
		{ 0x02, 0x8B },
		{ 0x03, 0x35 },

		{ 0x08, 0xF6 },
		{ 0x09, 0x46 },
		{ 0x0A, 0x08 },
		{ 0x0B, 0x04 },
		{ 0x0C, 0x57 },
		{ 0x0D, 0x74 },
		{ 0x0E, 0x26 },
		{ 0x0F, 0x8B },
		{ 0x10, 0x86 },
		{ 0x11, 0x64 },
		{ 0x12, 0x07 },
		{ 0x13, 0x00 },
		{ 0x14, 0x00 },
		{ 0x15, 0x8B },
		{ 0x16, 0x78 },
		{ 0x17, 0x04 },
		{ 0x18, 0x8B },
		{ 0x19, 0x0E },
		{ 0x1A, 0x8B },
		{ 0x1B, 0x96 },
		{ 0x1C, 0x68 },
		{ 0x1D, 0x07 },
		{ 0x1E, 0x00 },
		{ 0x1F, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic D3DDevice_MakeSpace 4134 version
// ******************************************************************
// * D3D::Unknown (D3D::MakeSpace?)
// ******************************************************************
// * FOR DEBUGGING ONLY!
// ******************************************************************
OOVPA_XREF(D3D_Unknown, 5788, 1+5,

    XRefNoSaveIndex,
    XRefOne)

	XREF_ENTRY( 0x0A, XREF_D3D_MakeRequestedSpace ),

        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
	{ 0x0E, 0xC3 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5788, 7)

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x38 },
        { 0x0A, 0x00 },
        { 0x0D, 0x88 },
        { 0x10, 0x00 },
        { 0x13, 0xC1 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D::ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags, 5788, 9,

    XREF_D3D_ClearStateBlockFlags,
    XRefZero)

        // D3D_ClearStateBlockFlags+0x0A : mov ecx, 0x87
        { 0x0A, 0xB9 },
        { 0x0B, 0x87 },

        // D3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x07A4]
        { 0x16, 0x8B },
        { 0x17, 0x8A },
        { 0x18, 0xA4 },
        { 0x19, 0x07 },

        // D3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 },
        { 0x30, 0xC1 },
        { 0x31, 0x90 },
OOVPA_END;
#endif

// ******************************************************************
// * D3D::RecordStateBlock
// ******************************************************************
OOVPA_XREF(D3D_RecordStateBlock, 5788, 10,

    XREF_D3D_RecordStateBlock,
    XRefZero)

        // D3D_RecordStateBlock+0x0F : mov eax, [edi+0x07A4]
        { 0x0F, 0x8B },
        { 0x10, 0x87 },
        { 0x11, 0xA4 },
        { 0x12, 0x07 },

        // D3D_RecordStateBlock+0x95 : add ebx, ecx
        { 0x95, 0x03 },
        { 0x96, 0xD9 },

        // D3D_RecordStateBlock+0xD5 : mov dword ptr [esi+0x0C], 1
        { 0xD5, 0xC7 },
        { 0xD6, 0x46 },
        { 0xD7, 0x0C },
        { 0xD8, 0x01 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 5788, 7)

        { 0x0B, 0xA3 },
        { 0x14, 0x04 },
        { 0x23, 0x8B },
        { 0x2A, 0xC9 },
        { 0x35, 0xC7 },
        { 0x40, 0x08 },
        { 0x4C, 0x5F },
OOVPA_END;
#endif

#if 0 // Moved to 5558
// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 5788, 7)

        { 0x06, 0x8B },
        { 0x0E, 0x94 },
        { 0x16, 0x8B },
        { 0x1E, 0x04 },
        { 0x26, 0x48 },
        { 0x2E, 0x8B },
        { 0x36, 0x14 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5455 version
// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 5788, 13)

	// D3D::CDevice::KickOff+0x0A : mov edx, [ecx+0x35C]
	{ 0x0A, 0x8B },
	{ 0x0B, 0x96 },
	{ 0x0C, 0x70 },
	{ 0x0D, 0x07 },
	{ 0x0E, 0x00 },
	{ 0x0F, 0x00 },

	// D3D::CDevice::KickOff+0x14 : test ch, 0x20
	{ 0x14, 0xF6 },
	{ 0x15, 0xC4 },
	{ 0x16, 0x20 },

	// D3D::CDevice::KickOff+0x24 : sfence
	{ 0x24, 0x0F },
	{ 0x25, 0xAE },
	{ 0x26, 0xF8 },

	// D3D::CDevice::KickOff+0xAF : retn
	{ 0xAF, 0xC3 }
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5233 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 5788, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x75 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5344 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 5788, 9)

        { 0x03, 0x04 },
        { 0x0A, 0xA3 },
        { 0x0F, 0x8B },
        { 0x12, 0x1A },
        { 0x17, 0x14 },
        { 0x1B, 0x74 },
        { 0x1C, 0x07 },
        { 0x24, 0xC2 },
        { 0x26, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 5788, 7)

        { 0x0D, 0xCE },
        { 0x1E, 0x8B },
        { 0x2B, 0xF9 },
        { 0x3A, 0xCF },
        { 0x49, 0x00 },
        { 0x58, 0xC1 },
        { 0x6B, 0x5F },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetDisplayFieldStatus
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayFieldStatus, 5788, 7)

        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetScissors
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetScissors, 5788, 7)

        { 0x0B, 0x74 },
        { 0x14, 0x11 },
        { 0x1F, 0x04 },
        { 0x2A, 0x85 },
        { 0x35, 0xE1 },
        { 0x40, 0xC1 },
        { 0x4B, 0xA4 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 5659 version
// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 5788, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0x44 },
        { 0x0E, 0x0F },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;
#endif

#if 0 // No longer used, replaced by generic 3925 version
// ******************************************************************
// * D3DDevice_DeleteStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteStateBlock, 5788, 9)

        // D3DDevice_DeleteStateBlock(x)+26: mov eax, [esi+4]
        { 0x13, 0x8B },
        { 0x14, 0x46 },
        { 0x15, 0x04 },

        // D3DDevice_DeleteStateBlock(x)+1B:
        { 0x23, 0x43 }, // inc ebx
        { 0x24, 0x3B }, // cmp ebx, [edi]
        { 0x25, 0x1F },

        // D3DDevice_DeleteStateBlock(x)+34: cmp [edi+8], ebp
        { 0x3C, 0x39 },
        { 0x3D, 0x6F },
        { 0x3E, 0x08 },
OOVPA_END;
#endif

#if 0 // Used 3925 instead in 5788, 5849
// ******************************************************************
// * D3DDevice_SetRenderState_RopZCmpAlwaysRead
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 5788, 1+6,

    XRefNoSaveIndex,
    XRefOne)

		XREF_ENTRY( 0x05, XREF_D3DRS_ROPZCMPALWAYSREAD ),

        { 0x01, 0x44 },
        { 0x04, 0xA3 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC2 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },
OOVPA_END;
#endif

#if 0 //Used 3925
// ******************************************************************
// * D3DDevice_GetVertexShaderSize
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderSize, 5788, 13)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x08 },

        { 0x08, 0x8B },
        { 0x09, 0x4C },
        { 0x0A, 0x24 },
        { 0x0B, 0x04 },

        { 0x0C, 0x8B },
        { 0x0D, 0x51 },
        { 0x0E, 0x07 },

        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;
#endif

#if 0 // Moved to 5659
// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 5788, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x28 },
        { 0x08, 0x19 },
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
#endif

#if 0 // Moved to 5659
// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 5788, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x98 },
        { 0x08, 0x07 },
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
#endif

#if 0 // Moved to 5659
// ******************************************************************
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 5788, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x84 },
        { 0x08, 0x07 },
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
#endif

#if 0 // Moved to 5659
// ******************************************************************
// * D3DDevice_GetTexture, named with 2 suffix to match EMUPATCH(D3DDevice_GetTexture2)
// ******************************************************************
OOVPA_XREF(D3DDevice_GetTexture2, 5788, 1+23, // Up to 5849

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY( 0x0E, XREF_OFFSET_D3DDEVICE_M_TEXTURES ), // Derived

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
		{ 0x0E, 0x88 }, // GetTexture2 880F vs GetPalette2 980F
		{ 0x0F, 0x0F },

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
#endif
