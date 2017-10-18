// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.4134.cpp
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

#if 0 // Moved to 4034
// ******************************************************************
// * D3DDevice_SetTextureState_TexCoordIndex
// ******************************************************************
OOVPA_XREF(D3DDevice_SetTextureState_TexCoordIndex, 4134, 1+10,

	XRefNoSaveIndex,
	XRefOne)

		XREF_ENTRY(0x18, XREF_D3DTSS_TEXCOORDINDEX), // Derived

        // D3DDevice_SetTextureState_TexCoordIndex+0x13 : shl eax, 0x07
        { 0x13, 0xC1 },
        { 0x14, 0xE0 },
        { 0x15, 0x07 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x25 : cmp eax, ecx
        { 0x25, 0x3B },
        { 0x26, 0xC1 },

        // D3DDevice_SetTextureState_TexCoordIndex+0x6C : mov edi, 0x2400
        { 0x6C, 0xBF },
        { 0x6E, 0x24 },

        // D3DDevice_SetTextureState_TexCoordIndex+0xB3 : shl edx, 0x04
        { 0xB4, 0xC1 },
        { 0xB5, 0xE2 },
        { 0xB6, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetTransform
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTransform, 4134, 11)

        // D3DDevice_SetTransform+0x00 : mov eax, [esp+arg_0]
        { 0x00, 0x8B },

        // D3DDevice_SetTransform+0x16 : lea edi, [ecx+ebx+0x750]
        { 0x16, 0x8D },
        { 0x17, 0xBC },
        { 0x18, 0x19 },

        // D3DDevice_SetTransform+0x24 : repe movsd
        { 0x24, 0xF3 },
        { 0x25, 0xA5 },

        // D3DDevice_SetTransform+0x78 : fld [edx+0x3C]
        { 0x78, 0xD9 },
        { 0x79, 0x42 },
        { 0x7A, 0x3C },

        // D3DDevice_SetTransform+0x92 : jmp +0x10
        { 0x92, 0xEB },
        { 0x93, 0x10 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D_CheckDeviceFormat
// ******************************************************************
OOVPA_NO_XREF(D3D_CheckDeviceFormat, 4134, 8)

        // D3D_CheckDeviceFormat+0x0D : retn 0x18
        { 0x0D, 0xC2 },
        { 0x0E, 0x18 },

        // D3D_CheckDeviceFormat+0x15 : jz +0x08
        { 0x15, 0x74 },
        { 0x16, 0x08 },

        // D3D_CheckDeviceFormat+0x29 : mov cl, 0x80
        { 0x29, 0xB1 },
        { 0x2A, 0x80 },

        // D3D_CheckDeviceFormat+0x61 : retn 0x18
        { 0x61, 0xC2 },
        { 0x62, 0x18 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D_GetAdapterModeCount
// ******************************************************************
OOVPA_NO_XREF(D3D_GetAdapterModeCount, 4134, 12)

        // D3D_GetAdapterModeCount+0x25 : add edx, 0xFF
        { 0x25, 0x81 },
        { 0x26, 0xE2 },
        { 0x27, 0xFF },

        // D3D_GetAdapterModeCount+0x47 : cmp edi, 0x04
        { 0x47, 0x83 },
        { 0x48, 0xFF },
        { 0x49, 0x04 },

        // D3D_GetAdapterModeCount+0x95 : shl eax, 0x02
        { 0x95, 0xC1 },
        { 0x96, 0xE0 },
        { 0x97, 0x02 },

        // D3D_GetAdapterModeCount+0x99 : add esp, 0x08
        { 0x99, 0x83 },
        { 0x9A, 0xC4 },
        { 0x9B, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D_EnumAdapterModes
// ******************************************************************
OOVPA_NO_XREF(D3D_EnumAdapterModes, 4134, 10)

        // D3D_EnumAdapterModes+0x00 : sub esp, 0x08
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x08 },

        // D3D_EnumAdapterModes+0xD1 : and edx, 0x0A
        { 0xD1, 0x83 },
        { 0xD2, 0xE2 },
        { 0xD3, 0x0A },

        // D3D_EnumAdapterModes+0xF5 : mov dword ptr[edi+0x10], 0x1E
        { 0xF5, 0xC7 },
        { 0xF6, 0x47 },
        { 0xF7, 0x10 },
        { 0xF8, 0x1E },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_FogColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FogColor, 4134, 10)

        // D3DDevice_SetRenderState_FogColor+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_FogColor+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },

        // D3DDevice_SetRenderState_FogColor+0x25 : shl edi, 0x10
        { 0x25, 0xC1 },
        { 0x26, 0xE7 },
        { 0x27, 0x10 },

        // D3DDevice_SetRenderState_FogColor+0x40 : mov [esi], eax
        { 0x40, 0x89 },
        { 0x41, 0x06 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_Swap
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Swap, 4134, 11)

        // D3DDevice_Swap+0x10 : mov ebx, 5
        { 0x10, 0xBB },
        { 0x11, 0x05 },

        // D3DDevice_Swap+0x1D : test bl, 3
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },

        // D3DDevice_Swap+0x46 : inc dword ptr [esi+0x2ABC]
        { 0x46, 0xFF },
        { 0x47, 0x86 },
        { 0x48, 0xBC },
        { 0x49, 0x2A },

        // D3DDevice_Swap+0xAE : retn 4
        { 0xAE, 0xC2 },
        { 0xAF, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_Clear
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Clear, 4134, 9)
        // D3DDevice_Clear+0x0000 : sub esp, 38h
        { 0x0000, 0x83 },
        { 0x0001, 0xEC },
        { 0x0002, 0x38 },

        // D3DDevice_Clear+0x000B : mov ebx, [ebp+0x2074]
        { 0x000B, 0x8B },
        { 0x000C, 0x9D },
        { 0x000D, 0x74 },
        { 0x000E, 0x20 },

        // D3DDevice_Clear+0x0399 : retn 0x18
        { 0x0399, 0xC2 },
        { 0x039A, 0x18 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_Begin
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Begin, 4134, 11)
        // D3DDevice_Begin : mov dword ptr [eax], 0x417FC
        { 0x1C, 0xC7 },
        { 0x1D, 0x00 },
        { 0x1E, 0xFC },
        { 0x1F, 0x17 },
        { 0x20, 0x04 },
        { 0x21, 0x00 },

        // D3DDevice_Begin : mov     [eax+4], ecx
        { 0x22, 0x89 },
        { 0x23, 0x48 },
        { 0x24, 0x04 },

        // D3DDevice_Begin : retn 0x04
        { 0x32, 0xC2 },
        { 0x33, 0x04 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_End
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_End, 4134, 7)
	{ 0x07, 0x8B },
	{ 0x0E, 0xE8 },
	{ 0x16, 0x17 },
	{ 0x1E, 0x00 },
	{ 0x26, 0x46 },
	{ 0x2E, 0x01 },
	{ 0x36, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetBackBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackBuffer, 4134, 12)

        // D3DDevice_GetBackBuffer+0x04 : cmp eax, 0xFFFFFFFF
        { 0x04, 0x83 },
        { 0x05, 0xF8 },
        { 0x06, 0xFF },

        // D3DDevice_GetBackBuffer+0x0D : jnz +0x07
        { 0x0D, 0x75 },
        { 0x0E, 0x07 },

        // D3DDevice_GetBackBuffer+0x18 : sbb eax, eax
        { 0x18, 0x1B },
        { 0x19, 0xC0 },

        // D3DDevice_GetBackBuffer+0x1D : mov eax, [ecx+eax*4+0x207C]
        { 0x1D, 0x8B },
        { 0x1E, 0x84 },
        { 0x1F, 0x81 },
        { 0x20, 0x7C },
        { 0x21, 0x20 },
OOVPA_END;
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetDepthStencilSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface, 4134, 9)

        // D3DDevice_GetDepthStencilSurface+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetDepthStencilSurface+0x05 : mov eax, [eax + 0x2074]
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x07, 0x74 },
        { 0x08, 0x20 },

        // D3DDevice_GetDepthStencilSurface+0x11 : mov [ecx], eax
        { 0x11, 0x89 },
        { 0x12, 0x01 },

        // D3DDevice_GetDepthStencilSurface+0x1D : retn 0x04
        { 0x1D, 0xC2 },
        { 0x1E, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderTarget, 4134, 9)

        // D3DDevice_SetRenderTarget+0x00 : sub esp, 0x3C
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x3C },

        // D3DDevice_SetRenderTarget+014 : jnz +0x0C
        { 0x14, 0x75 },
        { 0x15, 0x0C },

        // D3DDevice_SetRenderTarget+0x44 : push ebp
        { 0x44, 0x55 },

        // D3DDevice_SetRenderTarget+0x79 : shr ecx, 0x14
        { 0x79, 0xC1 },
        { 0x7A, 0xE9 },
        { 0x7B, 0x14 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SelectVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SelectVertexShader, 4134, 11)

        // D3DDevice_SelectVertexShader+0x13 : mov [esi+0x384], eax
        { 0x13, 0x89 },
        { 0x14, 0x86 },
        { 0x15, 0x84 },
        { 0x16, 0x03 },

        // D3DDevice_SelectVertexShader+0x3B : mov dword ptr [eax], 0x00081E94
        { 0x3B, 0xC7 },
        { 0x3C, 0x00 },
        { 0x3D, 0x94 },
        { 0x3E, 0x1E },
        { 0x3F, 0x08 },

        // D3DDevice_SelectVertexShader+0x55 : mov [esi], eax
        { 0x55, 0x89 },
        { 0x56, 0x06 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CopyRects, 4134, 10)

        // D3DDevice_CopyRects+0x00 : sub esp, 0x01A0
        { 0x00, 0x81 },
        { 0x01, 0xEC },
        { 0x02, 0xA0 },
        { 0x03, 0x01 },

        // D3DDevice_CopyRects+0x6E : mov esi, [eax+0x201C]
        { 0x6E, 0x8B },
        { 0x6F, 0xB0 },
        { 0x70, 0x1C },
        { 0x71, 0x20 },

        // D3DDevice_CopyRects+0xDD : jnz +0x22
        { 0xDD, 0x75 },
        { 0xDE, 0x22 },
OOVPA_END;
#endif

// ******************************************************************
// * D3D::CreateStandAloneSurface
// ******************************************************************
OOVPA_XREF(D3D_CreateStandAloneSurface, 4134, 12,

    XREF_D3D_CreateStandAloneSurface,
    XRefZero)

        { 0x02, 0x24 },
        { 0x17, 0x14 },
        { 0x18, 0x6A },
        { 0x19, 0x00 },
        { 0x1A, 0x6A },
        { 0x1B, 0x00 },
        { 0x1C, 0x6A },
        { 0x1D, 0x00 },
        { 0x1E, 0x52 },
        { 0x1F, 0x6A },

        { 0x36, 0xF0 },
        { 0x4B, 0x15 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_XREF(D3DDevice_CreateImageSurface, 4134, 2,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x01, XREF_D3D_CreateStandAloneSurface),

        { 0x00, 0xE9 },
OOVPA_END;

#if 0 // Probably ?Lock3DSurface@PixelJar@D3D@@YGXPAUD3DPixelContainer@@IPAU_D3DLOCKED_BOX@@PBU_D3DBOX@@K@Z
// ******************************************************************
// * D3DDevice_CreateImageSurface
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CreateImageSurface, 4134, 28)
	{ 0x00, 0x83 }, 
	{ 0x01, 0xEC },
	{ 0x02, 0x08 },
	{ 0x03, 0x53 },
	{ 0x04, 0x8A },
	{ 0x05, 0x5C },
	{ 0x06, 0x24 },
	{ 0x07, 0x20 },
	{ 0x08, 0xF6 },
	{ 0x09, 0xC3 },
	{ 0x0A, 0x20 },
	{ 0x0B, 0x55 },
	{ 0x0C, 0x8B },
	{ 0x0D, 0x6C },
	{ 0x0E, 0x24 },
	{ 0x0F, 0x14 },
	{ 0x10, 0x56 },
	{ 0x11, 0x57 },
	{ 0x12, 0x75 },
	{ 0x13, 0x06 },
	{ 0x14, 0x55 },
	{ 0x15, 0xE8 },
	{ 0x1A, 0x8D },
	{ 0x1B, 0x44 },
	{ 0x1C, 0x24 },
	{ 0x1D, 0x10 },
	{ 0x1E, 0x50 },
	{ 0x1F, 0x8D },

	OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShader, 4134, 10)

        // D3DDevice_SetPixelShader+0x0E : mov esi, [ebx+0x0370]
        { 0x0E, 0x8B },
        { 0x0F, 0xB3 },
        { 0x10, 0x70 },
        { 0x11, 0x03 },

        // D3DDevice_SetPixelShader+0x3A : or eax, 0x2000
        { 0x3A, 0x0D },
        { 0x3C, 0x20 },

        // D3DDevice_SetPixelShader+0x78 : retn 0x04
        { 0x78, 0xC2 },
        { 0x79, 0x04 },

        // D3DDevice_SetPixelShader+0xE3 : rep movsd
        { 0xE3, 0xF3 },
        { 0xE4, 0xA5 },

OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_SetTextureState_BumpEnv
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_BumpEnv, 4134, 12)

        // D3DDevice_SetTextureState_BumpEnv+0x18 : jnz +0x03
        { 0x18, 0x75 },
        { 0x19, 0x03 },

        // D3DDevice_SetTextureState_BumpEnv+0x1D : test bl, 3
        { 0x1D, 0xF6 },
        { 0x1E, 0xC3 },
        { 0x1F, 0x03 },

        // D3DDevice_SetTextureState_BumpEnv+0x32 : mov ecx, [esp+0x14]
        { 0x32, 0x8B },
        { 0x33, 0x4C },
        { 0x34, 0x24 },
        { 0x35, 0x18 },

        // D3DDevice_SetTextureState_BumpEnv+0x50 : shl esi, 5
        { 0x50, 0xC1 },
        { 0x51, 0xE6 },
        { 0x52, 0x05 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetIndices
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetIndices, 4134, 11)

        // D3DDevice_SetIndices+0x0E : jz +0x10
        { 0x0E, 0x74 },
        { 0x0F, 0x10 },

        // D3DDevice_SetIndices+0x1E : jmp +0x0A
        { 0x1E, 0xEB },
        { 0x1F, 0x0A },

        // D3DDevice_SetIndices+0x59 : mov [esi+0x1C], ecx
        { 0x59, 0x89 },
        { 0x5A, 0x4E },
        { 0x5B, 0x1C },

        // D3DDevice_SetIndices+0x76 : mov [esi+0x038C], edi
        { 0x76, 0x89 },
        { 0x77, 0xBE },
        { 0x78, 0x8C },
        { 0x79, 0x03 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_VertexBlend
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_VertexBlend, 4134, 11)

        // D3DDevice_SetRenderState_VertexBlend+0x06 : or edx, 0x200
        { 0x06, 0x81 },
        { 0x07, 0xCA },
        { 0x08, 0x00 },
        { 0x09, 0x02 },

        // D3DDevice_SetRenderState_VertexBlend+0x1E : jb +0x05
        { 0x1E, 0x72 },
        { 0x1F, 0x05 },

        // D3DDevice_SetRenderState_VertexBlend+0x2F : mov [eax+4], ecx
        { 0x2F, 0x89 },
        { 0x30, 0x48 },
        { 0x31, 0x04 },

        // D3DDevice_SetRenderState_VertexBlend+0x3E : retn 0x04
        { 0x3E, 0xC2 },
        { 0x3F, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_TextureFactor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TextureFactor, 4134, 9)

        // D3DDevice_SetRenderState_TextureFactor+0x07 : mov eax, [esi+0x0370]
        { 0x07, 0x8B },
        { 0x08, 0x86 },
        { 0x09, 0x70 },
        { 0x0A, 0x03 },

        // D3DDevice_SetRenderState_TextureFactor+0x2F : nop
        { 0x2F, 0x90 },

        // D3DDevice_SetRenderState_TextureFactor+0x41 : retn 0x04
        { 0x41, 0xC2 },
        { 0x42, 0x04 },

        // D3DDevice_SetRenderState_TextureFactor+0x4E : retn 0x04
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_EdgeAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_EdgeAntiAlias, 4134, 10)

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B },
        { 0x14, 0x4C },
        { 0x15, 0x24 },
        { 0x16, 0x08 },

        // D3DDevice_SetRenderState_EdgeAntiAlias+0x2F : retn 0x04
        { 0x2F, 0xC2 },
        { 0x30, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_ZEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ZEnable, 4134, 13)

        // D3DDevice_SetRenderState_ZEnable+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_ZEnable+0x1B : mov edx, [esi+0x2074h]
        { 0x1B, 0x8B },
        { 0x1C, 0x96 },
        { 0x1D, 0x74 },
        { 0x1E, 0x20 },

        // D3DDevice_SetRenderState_ZEnable+0x31 : mov dword ptr [eax], 0x4030C
        { 0x31, 0xC7 },
        { 0x32, 0x00 },
        { 0x33, 0x0C },
        { 0x34, 0x03 },
        { 0x35, 0x04 },

        // D3DDevice_SetRenderState_ZEnable+0x98 : retn 0x04
        { 0x98, 0xC2 },
        { 0x99, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_StencilEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilEnable, 4134, 11)

        // D3DDevice_SetRenderState_StencilEnable+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_StencilEnable+0x2C : jz +0x19
        { 0x2C, 0x74 },
        { 0x2D, 0x19 },

        // D3DDevice_SetRenderState_StencilEnable+0x4D : mov dword ptr [eax], 0x00041D84
        { 0x4D, 0xC7 },
        { 0x4E, 0x00 },
        { 0x4F, 0x84 },
        { 0x50, 0x1D },
        { 0x51, 0x04 },

        // D3DDevice_SetRenderState_StencilEnable+0x81 : retn 0x04
        { 0x81, 0xC2 },
        { 0x82, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleAntiAlias
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x04 : push esi
        { 0x04, 0x56 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x05 : mov esi, dword ptr [addr]
        { 0x05, 0x8B },
        { 0x06, 0x35 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x0B : mov ecx, esi
        { 0x0B, 0x8B },
        { 0x0C, 0xCE },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x1C : jb +0x05
        { 0x1C, 0x72 },
        { 0x1D, 0x05 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x38 : shl ecx, 0x10
        { 0x38, 0xC1 },
        { 0x39, 0xE1 },
        { 0x3A, 0x10 },

        // D3DDevice_SetRenderState_MultiSampleAntiAlias+0x4F : retn 0x04
        { 0x4F, 0xC2 },
        { 0x50, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShader, 4134, 14)

        // D3DDevice_SetVertexShader+0x06 : test bl, 1
        { 0x06, 0xF6 },
        { 0x07, 0xC3 },
        { 0x08, 0x01 },

        // D3DDevice_SetVertexShader+0x11 : mov eax, [esi+0x0380]
        { 0x11, 0x8B },
        { 0x12, 0x86 },
        { 0x13, 0x80 },
        { 0x14, 0x03 },

        // D3DDevice_SetVertexShader+0x90 : retn 0x04
        { 0x90, 0xC2 },
        { 0x91, 0x04 },

        // D3DDevice_SetVertexShader+0xB0 : mov dword ptr [eax], 0x4194C
        { 0xB0, 0xC7 },
        { 0xB1, 0x00 },
        { 0xB2, 0x4C },
        { 0xB3, 0x19 },
        { 0xB4, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_DrawVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawVerticesUP, 4134, 10)

        // D3DDevice_DrawVerticesUP+0x03 : sub esp, 0x14
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        // D3DDevice_DrawVerticesUP+0x2F : mov edx, [edi+0x744]
        { 0x2F, 0x8B },
        { 0x30, 0x97 },
        { 0x31, 0x44 },
        { 0x32, 0x07 },

        // D3DDevice_DrawVerticesUP+0xC1 : cmp esi, 0x80
        { 0xC1, 0x81 },
        { 0xC2, 0xFE },
        { 0xC3, 0x80 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetLight
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetLight, 4134, 12)

        // D3DDevice_SetLight+0x11 : mov edi, [ebp+0x394]
        { 0x11, 0x8B },
        { 0x12, 0xBD },
        { 0x13, 0x94 },
        { 0x14, 0x03 },

        // D3DDevice_SetLight+0x30 : shl eax, 0x04
        { 0x30, 0xC1 },
        { 0x31, 0xE0 },
        { 0x32, 0x04 },

        // D3DDevice_SetLight+0xBD : rep stosb
        { 0xBD, 0xF3 },
        { 0xBE, 0xAB },

        // D3DDevice_SetLight+0xFD : mov ecx, [ebx+0x68]
        { 0xFD, 0x8B },
        { 0xFE, 0x4B },
        { 0xFF, 0x68 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 4134, 12)

        // D3DDevice_SetMaterial+0x0C : add edi, 0x0940
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0xF0 },
        { 0x0F, 0x09 },

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
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 4134, 11)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },
        { 0x0D, 0xF0 },
        { 0x0E, 0x09 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0xB9 },
        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 4134, 12)

        // D3DDevice_SetBackMaterial+0x0C : add edi, 0x0A34
        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x34 },
        { 0x0F, 0x0A },

        // D3DDevice_SetBackMaterial+0x12 : mov ecx, 0x11
        { 0x12, 0xB9 },
        { 0x13, 0x11 },

        // D3DDevice_SetBackMaterial+0x1F : or ecx, 0x1000
        { 0x1F, 0x81 },
        { 0x20, 0xC9 },
        { 0x21, 0x00 },
        { 0x22, 0x10 },

        // D3DDevice_SetBackMaterial+0x2D : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 4134, 11)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0x34 },
        { 0x0E, 0x0A },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0xB9 },
        { 0x12, 0x11 },

        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

#if 0 // Used 3911
// ******************************************************************
// * D3DDevice_LightEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LightEnable, 4134, 13)

        // D3DDevice_LightEnable+0x0F : cmp ebx, [esi+0x0394]
        { 0x0F, 0x3B },
        { 0x10, 0x9E },
        { 0x11, 0x94 },
        { 0x12, 0x03 },

        // D3DDevice_LightEnable+0x6B : test eax, eax
        { 0x6B, 0x85 },
        { 0x6C, 0xC0 },

        // D3DDevice_LightEnable+0x9E : mov eax, [eax+0x8C]
        { 0x9E, 0x8B },
        { 0x9F, 0x80 },
        { 0xA0, 0x8C },

        // D3DDevice_LightEnable+0xBC : jz +0x12
        { 0xBC, 0x74 },
        { 0xBD, 0x12 },

        // D3DDevice_LightEnable+0xE1 : retn 0x08
        { 0xE1, 0xC2 },
        { 0xE2, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_NormalizeNormals
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_NormalizeNormals, 4134, 8)

        // D3DDevice_SetRenderState_NormalizeNormals+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_NormalizeNormals+0x25 : or [addr], 0x200
        { 0x25, 0x81 },
        { 0x26, 0x0D },
        { 0x2B, 0x00 },
        { 0x2C, 0x02 },

        // D3DDevice_SetRenderState_NormalizeNormals+0x36 : retn 0x04
        { 0x36, 0xC2 },
        { 0x37, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * Get2DSurfaceDesc
// ******************************************************************
OOVPA_NO_XREF(Get2DSurfaceDesc, 4134, 10)

        // Get2DSurfaceDesc+0x2B : movzx edx, byte ptr [edi+0x0D]
        { 0x2B, 0x0F },
        { 0x2C, 0xB6 },
        { 0x2D, 0x57 },
        { 0x2E, 0x0D },

        // Get2DSurfaceDesc+0x52 : mov edx, [eax+0x21C0]
        { 0x52, 0x8B },
        { 0x53, 0x90 },
        { 0x54, 0x7C },
        { 0x55, 0x20 },

        // Get2DSurfaceDesc+0xAE : retn 0x0C
        { 0xAE, 0xC2 },
        { 0xAF, 0x0C },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_Reset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Reset, 4134, 10)

        // D3DDevice_Reset+0x15 : mov ecx, ebp
        { 0x15, 0x8B },
        { 0x16, 0xCD },

        // D3DDevice_Reset+0x1E : jnz +0xF5
        { 0x1E, 0x75 },
        { 0x1F, 0xF5 },

        // D3DDevice_Reset+0x27 : mov esi, [esp+0x14]
        { 0x27, 0x8B },
        { 0x28, 0x74 },
        { 0x29, 0x24 },
        { 0x2A, 0x14 },

        // D3DDevice_Reset+0x37 : jge +0x10
        { 0x37, 0x7D },
        { 0x38, 0x10 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_TwoSidedLighting
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_TwoSidedLighting, 4134, 13)

        // D3DDevice_SetRenderState_TwoSidedLighting+0x11 : or edx, 0x1000
        { 0x11, 0x81 },
        { 0x12, 0xCA },
        { 0x13, 0x00 },
        { 0x14, 0x10 },

        // D3DDevice_SetRenderState_TwoSidedLighting+0x24 : cmp eax, [esi+4]; push edi
        { 0x24, 0x3B },
        { 0x25, 0x46 },
        { 0x26, 0x04 },
        { 0x27, 0x57 },

        // D3DDevice_SetRenderState_TwoSidedLighting+0x53 : add eax, 0x0C
        { 0x53, 0x83 },
        { 0x54, 0xC0 },
        { 0x55, 0x0C },

        // D3DDevice_SetRenderState_TwoSidedLighting+0x60 : retn 0x04
        { 0x60, 0xC2 },
        { 0x61, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_BackFillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_BackFillMode, 4134, 13)

        // D3DDevice_SetRenderState_BackFillMode+0x1C : jb +0x05
        { 0x1C, 0x72 },
        { 0x1D, 0x05 },

        // D3DDevice_SetRenderState_BackFillMode+0x35 : mov dword ptr [eax], abs
        { 0x35, 0xC7 },
        { 0x36, 0x00 },

        // D3DDevice_SetRenderState_BackFillMode+0x3B : add eax, 0x0C; mov [esi], eax; mov [abs], edi
        { 0x41, 0x83 },
        { 0x42, 0xC0 },
        { 0x43, 0x0C },
        { 0x44, 0x89 },
        { 0x45, 0x06 },
        { 0x46, 0x89 },
        { 0x47, 0x3D },

        // D3DDevice_SetRenderState_BackFillMode+0x4E : retn 0x04
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetTextureState_ColorKeyColor
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTextureState_ColorKeyColor, 4134, 12)

        // D3DDevice_SetTextureState_ColorKeyColor+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetTextureState_ColorKeyColor+0x27 : add eax, 8; mov [esi], eax; shl ecx, 7
        { 0x27, 0x83 },
        { 0x28, 0xC0 },
        { 0x29, 0x08 },
        { 0x2A, 0x89 },
        { 0x2B, 0x06 },
        { 0x2C, 0xC1 },
        { 0x2D, 0xE1 },
        { 0x2E, 0x07 },

        // D3DDevice_SetTextureState_ColorKeyColor+0x36 : retn 0x08
        { 0x36, 0xC2 },
        { 0x37, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_FrontFace
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_FrontFace, 4134, 11,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_FrontFace+0x36 : jmp D3DDevice_SetRenderState_CullMode
        XREF_ENTRY( 0x36, XREF_D3DDevice_SetRenderState_CullMode),

        // D3DDevice_SetRenderState_FrontFace+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },

        // D3DDevice_SetRenderState_FrontFace+0x13 : mov ecx, [esp+0x08]
        { 0x13, 0x8B },
        { 0x14, 0x4C },
        { 0x15, 0x24 },
        { 0x16, 0x08 },

        // D3DDevice_SetRenderState_FrontFace+0x20 : add eax, 8
        { 0x20, 0x83 },
        { 0x21, 0xC0 },
        { 0x22, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_LogicOp
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LogicOp, 4134, 9)

        // D3DDevice_SetRenderState_LogicOp+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_LogicOp+0x1B : mov dword ptr [eax], 0x417BC
        { 0x1B, 0xC7 },
        { 0x1C, 0x00 },
        { 0x1D, 0xBC },
        { 0x1E, 0x17 },
        { 0x1F, 0x04 },

        // D3DDevice_SetRenderState_LogicOp+0x4F : retn 0x04
        { 0x4F, 0xC2 },
        { 0x50, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_StencilFail
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_StencilFail, 4134, 12)

        // D3DDevice_SetRenderState_StencilFail+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_StencilFail+0x38 : cmp [abs], 0x1E00
        { 0x38, 0x81 },
        { 0x3E, 0x00 },
        { 0x3F, 0x1E },

        // D3DDevice_SetRenderState_StencilFail+0x47 : mov dword ptr [eax], 0x41D84
        { 0x47, 0xC7 },
        { 0x48, 0x00 },
        { 0x49, 0x84 },
        { 0x4A, 0x1D },
        { 0x4B, 0x04 },

        // D3DDevice_SetRenderState_StencilFail+0x6A : retn 0x04
        { 0x6A, 0xC2 },
        { 0x6B, 0x04 },
OOVPA_END;
#endif
#if 0 // Moved to 4034
// ******************************************************************
// * D3DDevice_SetRenderState_OcclusionCullEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_OcclusionCullEnable, 4134, 12)

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 },
        { 0x16, 0x05 },

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 },
        { 0x47, 0x00 },
        { 0x48, 0x1E },

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 },
        { 0x51, 0x00 },
        { 0x52, 0x84 },
        { 0x53, 0x1D },
        { 0x54, 0x04 },

        // D3DDevice_SetRenderState_OcclusionCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 },
        { 0x60, 0x04 },
OOVPA_END;
#endif
#if 0 // Moved to 4034
// ******************************************************************
// * D3DDevice_SetRenderState_StencilCullEnable
// ******************************************************************
OOVPA_XREF(D3DDevice_SetRenderState_StencilCullEnable, 4134, 13,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_SetRenderState_StencilCullEnable+0x0C : D3D__RenderState[D3DRS_STENCILCULLENABLE]
        XREF_ENTRY( 0x0C, XREF_D3DRS_STENCILCULLENABLE ),

        // D3DDevice_SetRenderState_StencilCullEnable+0x15 : jb +0x05
        { 0x15, 0x72 },
        { 0x16, 0x05 },

        // D3DDevice_SetRenderState_StencilCullEnable+0x41 : cmp [abs], 0x1E00
        { 0x41, 0x81 },
        { 0x47, 0x00 },
        { 0x48, 0x1E },

        // D3DDevice_SetRenderState_StencilCullEnable+0x50 : mov dword ptr [eax], 0x41D84
        { 0x50, 0xC7 },
        { 0x51, 0x00 },
        { 0x52, 0x84 },
        { 0x53, 0x1D },
        { 0x54, 0x04 },

        // D3DDevice_SetRenderState_StencilCullEnable+0x5F : retn 0x04
        { 0x5F, 0xC2 },
        { 0x60, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_FillMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_FillMode, 4134, 11)

        // D3DDevice_SetRenderState_FillMode+0x07 : mov eax, [esi]
        { 0x07, 0x8B },
        { 0x08, 0x06 },

        // D3DDevice_SetRenderState_FillMode+0x21 : mov ecx, [esp+0x08]
        { 0x21, 0x8B },
        { 0x22, 0x4C },
        { 0x23, 0x24 },
        { 0x24, 0x08 },

        // D3DDevice_SetRenderState_FillMode+0x25 : jnz +0x02
        { 0x25, 0x75 },
        { 0x26, 0x02 },

        // D3DDevice_SetRenderState_FillMode+0x35 : add eax, 0x0C
        { 0x35, 0x83 },
        { 0x36, 0xC0 },
        { 0x37, 0x0C },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_Dxt1NoiseEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4134, 9)

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x0C : mov eax, [edi+0x2070]
        { 0x0C, 0x8B },
        { 0x0D, 0x86 },
        { 0x0E, 0x70 },
        { 0x0F, 0x20 },

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x6E : add eax, 0x18
        { 0x6E, 0x83 },
        { 0x6F, 0xC0 },
        { 0x70, 0x18 },

        // D3DDevice_SetRenderState_Dxt1NoiseEnable+0x7C : retn 0x04
        { 0x7C, 0xC2 },
        { 0x7D, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMask
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMask, 4134, 13)

        // D3DDevice_SetRenderState_MultiSampleMask+0x1C : jb +0x05
        { 0x1C, 0x72 },
        { 0x1D, 0x05 },

        // D3DDevice_SetRenderState_MultiSampleMask+0x38 : and ecx, 0x0F; shl edx, 0x10
        { 0x38, 0x83 },
        { 0x39, 0xE1 },
        { 0x3A, 0x0F },
        { 0x3B, 0xC1 },

        // D3DDevice_SetRenderState_MultiSampleMask+0x3E : mov dword ptr [eax], 0x41D7C
        { 0x3E, 0xC7 },
        { 0x3F, 0x00 },
        { 0x40, 0x7C },
        { 0x41, 0x1D },
        { 0x42, 0x04 },

        // D3DDevice_SetRenderState_MultiSampleMask+0x4F : retn 0x04
        { 0x4F, 0xC2 },
        { 0x50, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleMode, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B },
        { 0x10, 0x88 },
        { 0x11, 0x70 },
        { 0x12, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B },
        { 0x16, 0x88 },
        { 0x17, 0x7C },
        { 0x18, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleMode+0x1B : jnz +0x0D
        { 0x1B, 0x75 },
        { 0x1C, 0x0D },

        // D3DDevice_SetRenderState_MultiSampleMode+0x2A : retn 0x04
        { 0x2A, 0xC2 },
        { 0x2B, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetRenderState_MultiSampleRenderTargetMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, 12)

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x0F : mov ecx, [eax+0x2070]
        { 0x0F, 0x8B },
        { 0x10, 0x88 },
        { 0x11, 0x70 },
        { 0x12, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x15 : cmp ecx, [eax+0x207C]
        { 0x15, 0x3B },
        { 0x16, 0x88 },
        { 0x17, 0x7C },
        { 0x18, 0x20 },

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x1B : jz +0x0D
        { 0x1B, 0x74 },
        { 0x1C, 0x0D },

        // D3DDevice_SetRenderState_MultiSampleRenderTargetMode+0x2A : retn 0x04
        { 0x2A, 0xC2 },
        { 0x2B, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_ShadowFunc
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_ShadowFunc, 4134, 13)

        // D3DDevice_SetRenderState_ShadowFunc+0x00 : push esi
        { 0x00, 0x56 },

        // D3DDevice_SetRenderState_ShadowFunc+0x09 : cmp eax, [esi+4]
        { 0x09, 0x3B },
        { 0x0A, 0x46 },
        { 0x0B, 0x04 },

        // D3DDevice_SetRenderState_ShadowFunc+0x0C : jb +0x05
        { 0x0C, 0x72 },
        { 0x0D, 0x05 },

        // D3DDevice_SetRenderState_ShadowFunc+0x1D : lea edx, [ecx-0x0200]
        { 0x1D, 0x8D },
        { 0x1E, 0x91 },
        { 0x1F, 0x00 },
        { 0x20, 0xFE },

        // D3DDevice_SetRenderState_ShadowFunc+0x26 : add eax, 0x08
        { 0x26, 0x83 },
        { 0x27, 0xC0 },
        { 0x28, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_YuvEnable
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_YuvEnable, 4134, 13)

        // D3DDevice_SetRenderState_YuvEnable+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetRenderState_YuvEnable+0x10 : mov eax, [esi]
        { 0x10, 0x8B },
        { 0x11, 0x06 },

        // D3DDevice_SetRenderState_YuvEnable+0x12 : cmp eax, [esi+4]
        { 0x12, 0x3B },
        { 0x13, 0x46 },
        { 0x14, 0x04 },

        // D3DDevice_SetRenderState_YuvEnable+0x15 : jb +0x05
        { 0x15, 0x72 },
        { 0x16, 0x05 },

        // D3DDevice_SetRenderState_YuvEnable+0x28 : retn 0x04
        { 0x28, 0xC2 },
        { 0x29, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_DrawIndexedVerticesUP
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DrawIndexedVerticesUP, 4134, 10)

        // D3DDevice_DrawIndexedVerticesUP+0x4A : or edx, 0x800
        { 0x4A, 0x81 },
        { 0x4B, 0xCA },
        { 0x4C, 0x00 },
        { 0x4D, 0x08 },

        // D3DDevice_DrawIndexedVerticesUP+0x5C : lea edx, [edi+0x6C4]
        { 0x5C, 0x8D },
        { 0x5D, 0x97 },
        { 0x5E, 0xC4 },
        { 0x5F, 0x06 },

        // D3DDevice_DrawIndexedVerticesUP+0x6A : mov eax, 0x10
        { 0x6A, 0xB8 },
        { 0x6B, 0x10 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 4134, 10)

        // D3DDevice_SetPixelShaderConstant+0x17 : test eax, eax
        { 0x17, 0x85 },
        { 0x18, 0xC0 },

        // D3DDevice_SetPixelShaderConstant+0x32 : add edi, 0x04
        { 0x32, 0x83 },
        { 0x33, 0xC7 },
        { 0x34, 0x04 },

        // D3DDevice_SetPixelShaderConstant+0x47 : jmp +0x07
        { 0x47, 0xEB },
        { 0x48, 0x07 },

        // D3DDevice_SetPixelShaderConstant+0xC7 : fstp [esp+??]
        { 0xC7, 0xD9 },
        { 0xC8, 0x1C },
        { 0xC9, 0x24 },
OOVPA_END;
#endif
#if 0 // Used 3925
// ******************************************************************
// * D3DDevice_DeleteVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_DeleteVertexShader, 4134, 6)

        { 0x02, 0x24 },
        { 0x06, 0xFF },
        { 0x0A, 0x08 },
        { 0x0E, 0x44 },
        { 0x16, 0xC2 },
        { 0x17, 0x04 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_SetScreenSpaceOffset
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetScreenSpaceOffset, 4134, 7)

	{ 0x06, 0x56 },
	{ 0x07, 0xD8 },
	{ 0x08, 0x44 },
	{ 0x09, 0x24 },
	{ 0x0A, 0x08 },
	{ 0x2E, 0x8B },
	{ 0x2F, 0x06 },
OOVPA_END;
#endif

// ******************************************************************
// * D3D::SetFence
// ******************************************************************
OOVPA_XREF(D3D_SetFence, 4134, 12,

    XREF_D3D_SetFence,
    XRefZero)

        { 0x0D, 0x72 },
        { 0x0E, 0x0E },
        { 0x31, 0xBA },
        { 0x32, 0x90 },
        { 0x33, 0x1D },
        { 0x34, 0x04 },
        { 0x35, 0x00 },
        { 0x45, 0x83 },
        { 0x46, 0xE1 },
        { 0x47, 0x3F },
        { 0xAA, 0xC2 },
        { 0xAB, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetRenderState_LineWidth
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_LineWidth, 4134, 7)

        { 0x0C, 0x89 },
        { 0x18, 0x04 },
        { 0x28, 0xD9 },
        { 0x32, 0x81 },
        { 0x3F, 0x8B },
        { 0x4C, 0x00 },
        { 0x59, 0x06 },
OOVPA_END;
#endif
#if 0 // Moved to 4034
// ******************************************************************
// * D3DDevice_SetRenderState_PSTextureModes
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetRenderState_PSTextureModes, 4134, 10)
		{ 0x0A, 0x89 },
		{ 0x0B, 0x81 },
		{ 0x0E, 0x00 },
		{ 0x0F, 0x00 },
		{ 0x10, 0x81 },
		{ 0x11, 0x0D },
		{ 0x16, 0x00 },
		{ 0x17, 0x40 },
		{ 0x1A, 0xA3 },
		{ 0x1F, 0xC2 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_BlockUntilVerticalBlank
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockUntilVerticalBlank, 4134, 11)

        // D3DDevice_BlockUntilVerticalBlank+0x05 : push 0; push 0; push 1
        { 0x05, 0x6A },
        { 0x06, 0x00 },
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x01 },

        // D3DDevice_BlockUntilVerticalBlank+0x17 : add eax, 0x2430
        { 0x17, 0x05 },
        { 0x18, 0x30 },
        { 0x19, 0x24 },

        // D3DDevice_BlockUntilVerticalBlank+0x1D : call [KrnlImport]
        { 0x1D, 0xFF },

        // D3DDevice_BlockUntilVerticalBlank+0x23 : retn
        { 0x23, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
/* NOTE: This may look the similar as SetSwapCallback. 
   It might be a Offset 0Ch is increment +4. */
OOVPA_NO_XREF(D3DDevice_SetVerticalBlankCallback, 4134, 12)

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x242C], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x2C }, // 28 vs 2C
        { 0x0D, 0x24 },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
/* This may look the same as 4361, but accuracy is not gauranteed.
   It might be a SetVerticalBlankCallback that has an identical signature... */
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 4134, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x2428], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0x28 }, // 28 vs 2C
        { 0x0D, 0x24 },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetTile, 4134, 11)

        // D3DDevice_GetTile+0x13 : lea esi, [ecx+eax*8+0x211C]
        { 0x13, 0x8D },
        { 0x14, 0xB4 },
        { 0x15, 0xC1 },
        { 0x16, 0x1C },
        { 0x17, 0x21 },

        // D3DDevice_GetTile+0x1A : mov ecx, 0x06
        { 0x1A, 0xB9 },
        { 0x1B, 0x06 },

        // D3DDevice_GetTile+0x21 : pop edi; pop esi
        { 0x21, 0x5F },
        { 0x22, 0x5E },

        // D3DDevice_GetTile+0x23 : retn 0x08
        { 0x23, 0xC2 },
        { 0x24, 0x08 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetTile
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetTile, 4134, 11)

        // D3DDevice_SetTile+0x00 : sub esp, 0x18
        { 0x00, 0x83 },
        { 0x01, 0xEC },
        { 0x02, 0x18 },

        // D3DDevice_SetTile+0x16 : cmp [esi+4], eax
        { 0x16, 0x39 },
        { 0x17, 0x46 },
        { 0x18, 0x04 },

        // D3DDevice_SetTile+0x3D : lea edi, [edx+ecx*8+0x211C]
        { 0x3D, 0x8D },
        { 0x3E, 0xBC },
        { 0x3F, 0xCB },
        { 0x40, 0x1C },
        { 0x41, 0x21 },
OOVPA_END;
#endif
#if 0 // Moved to 4034
// ******************************************************************
// * D3DDevice_IsBusy
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_IsBusy, 4134, 7)

        { 0x05, 0x8B },
        { 0x0C, 0x90 },
        { 0x13, 0x44 },
        { 0x1A, 0xFF },
        { 0x21, 0x01 },
        { 0x28, 0x00 },
        { 0x2F, 0xD2 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3D::CDevice::KickOff
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_KickOff, 4134, 12)

	// D3D::CDevice::KickOff+0x0D : test al, 4
	{ 0x0D, 0xA8 },
	{ 0x0E, 0x04 },
	// D3D::CDevice::KickOff+0x11 : mov ecx, [ecx+0x35C]
	{ 0x11, 0x8B },
	{ 0x12, 0x89 },
	{ 0x13, 0x5C },
	{ 0x14, 0x03 },
	{ 0x15, 0x00 },
	{ 0x16, 0x00 },
	// D3D::CDevice::KickOff+0x26 : sfence
	{ 0x26, 0x0F },
	{ 0x27, 0xAE },
	{ 0x28, 0xF8 },
	// D3D::CDevice::KickOff+0xE3 : retn
	{ 0xE3, 0xC3 }
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_BlockOnFence
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BlockOnFence, 4134, 7)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x6A },
        { 0x06, 0x50 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetRenderTarget
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetRenderTarget, 4134, 11)

        // D3DDevice_GetRenderTarget+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_GetRenderTarget+0x05 : mov eax, [eax + 0x2070]
        { 0x05, 0x8B },
        { 0x06, 0x80 },
        { 0x07, 0x70 },
        { 0x08, 0x20 },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },

        // D3DDevice_GetRenderTarget+0x0B : test    eax, eax
        { 0x0B, 0x85 },
        { 0x0C, 0xC0 },

        // D3DDevice_GetRenderTarget+0x1D : retn 0x04
        { 0x1D, 0xC2 },
        { 0x1E, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_Release
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_Release, 4134, 11)

        // D3DDevice_Release+0x07 : mov eax, [edi+0x043C]
        { 0x07, 0x8B },
        { 0x08, 0x87 },
        { 0x09, 0x3C },
        { 0x0A, 0x04 },

        // D3DDevice_Release+0x0D : cmp eax, 1
        { 0x0D, 0x83 },
        { 0x0E, 0xF8 },
        { 0x0F, 0x01 },

        // D3DDevice_Release+0x1B : mov ecx, 0x0AB8
        { 0x1B, 0xB9 },
        { 0x1C, 0xB8 },
        { 0x1D, 0x0A },

        // D3DDevice_Release+0x2E : dec eax
        { 0x2E, 0x48 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_GetDisplayMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetDisplayMode, 4134, 13)

        // D3DDevice_GetDisplayMode+0x01 : mov esi, ds:[*]
        { 0x01, 0x8B },
        { 0x02, 0x35 },

        // D3DDevice_GetDisplayMode+0x18 : shr ecx, 0x14
        { 0x18, 0xC1 },
        { 0x19, 0xE9 },
        { 0x1A, 0x14 },

        // D3DDevice_GetDisplayMode+0x50 : shr eax, 0x0C
        { 0x50, 0xC1 },
        { 0x51, 0xE8 },
        { 0x52, 0x0C },

        // D3DDevice_GetDisplayMode+0x6B : and eax, 0x0A
        { 0x6B, 0x83 },
        { 0x6C, 0xE0 },
        { 0x6D, 0x0A },

        // D3DDevice_GetDisplayMode+0x92 : retn 0x04
        { 0x92, 0xC2 },
        { 0x93, 0x04 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetShaderConstantMode, 4134, 7)

        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0xC7 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3D::LazySetPointParams
// ******************************************************************
OOVPA_NO_XREF(D3D_LazySetPointParams, 4134, 7)
	{ 0x00, 0x83 },
	{ 0x01, 0xEC },
	{ 0x02, 0x14 },
	{ 0x4E, 0xE0 },
	{ 0x73, 0xF6 },
	{ 0x74, 0xC4 },
	{ 0x75, 0x41 },
OOVPA_END;
#endif
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_SetVertexData2f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2f, 4134, 14)
/*	{ 0x07, 0x8B },
	{ 0x0C, 0x72 },
	{ 0x13, 0x8B },
	{ 0x1A, 0x80 },
	{ 0x21, 0x0C },
	{ 0x28, 0x89 },
	{ 0x2F, 0xC0 },
	{ 0x36, 0x00 },*/
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4039 version
// ******************************************************************
// * D3DDevice_SetVertexData4f
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4f, 4134, 7)
	{ 0x0A, 0x46 },
	{ 0x16, 0x08 },
	{ 0x22, 0x09 },
	{ 0x2E, 0x24 },
	{ 0x3A, 0x24 },
	{ 0x46, 0x8B },
	{ 0x52, 0x14 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4134, 18)

        { 0x01, 0x0D },

        { 0x0D, 0x56 },
        { 0x0E, 0x8B },
        { 0x0F, 0x74 },
        { 0x10, 0x24 },
        { 0x11, 0x08 },
        { 0x12, 0x74 },
        { 0x13, 0x08 },
        { 0x14, 0x39 },
        { 0x15, 0x35 },

        { 0x22, 0x6A },
        { 0x23, 0x00 },
        { 0x24, 0x56 },
        { 0x25, 0x6A },
        { 0x26, 0x0B },
        { 0x27, 0x50 },

        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_SetFlickerFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetFlickerFilter, 4134, 11)

        // D3DDevice_SetFlickerFilter+0x1C : mov eax, [eax+0x2268]
        { 0x1D, 0x80 },
        { 0x1E, 0x68 },
        { 0x1F, 0x22 },

        // D3DDevice_SetFlickerFilter+0x22 : push 0; push esi; push 0x0B; push eax
        { 0x22, 0x6A },
        { 0x23, 0x00 },
        { 0x24, 0x56 },
        { 0x25, 0x6A },
        { 0x26, 0x0B },
        { 0x27, 0x50 },

        // D3DDevice_SetFlickerFilter+0x3F : retn 0x04
        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;
#endif
// ******************************************************************
// * D3DDevice_SetSoftDisplayFilter
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSoftDisplayFilter, 4134, 13)

        { 0x01, 0x0D },

        { 0x23, 0x74 },
        { 0x24, 0x28 },
        { 0x25, 0x8B },
        { 0x26, 0x90 },

        { 0x2B, 0x6A },
        { 0x2C, 0x00 },
        { 0x2D, 0x56 },
        { 0x2E, 0x6A },
        { 0x2F, 0x0E },
        { 0x30, 0x52 },

        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },

/*
        // D3DDevice_SetSoftDisplayFilter+0x25 : mov edx, [eax+0x2268]
        { 0x25, 0x8B },
        { 0x26, 0x90 },
        { 0x27, 0x68 },
        { 0x28, 0x22 },*/

OOVPA_END;

#if 0 // No longer used, replaced by generic 4034 version
// ******************************************************************
// * D3DDevice_SetPalette
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetPalette, 4134, 12)

        // D3DDevice_SetPalette+0x0D : mov eax, [edi+esi*4+0x0A88]
        { 0x0D, 0x8B },
        { 0x0E, 0x84 },
        { 0x0F, 0xBE },
        { 0x10, 0x88 },
        { 0x11, 0x0A },

        // D3DDevice_SetPalette+0x76 : add edi, 0x41B20
        { 0x76, 0x81 },
        { 0x77, 0xC7 },
        { 0x78, 0x20 },
        { 0x79, 0x1B },
        { 0x7A, 0x04 },

        // D3DDevice_SetPalette+0x8A : retn 0x08
        { 0x8A, 0xC2 },
        { 0x8B, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_EnableOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EnableOverlay, 4134, 11)

        // D3DDevice_EnableOverlay+0x0B : mov ecx, [eax+0x8700]
        { 0x0B, 0x8B },
        { 0x0C, 0x88 },
        { 0x0E, 0x87 },

        // D3DDevice_EnableOverlay+0x15 : jz +0x0A
        { 0x15, 0x74 },
        { 0x16, 0x0A },

        // D3DDevice_EnableOverlay+0x5A : mov [eax+0x8918], ecx
        { 0x5A, 0x89 },
        { 0x5B, 0x88 },
        { 0x5C, 0x18 },
        { 0x5D, 0x89 },

        // D3DDevice_EnableOverlay+0x60 : retn 0x04
        { 0x60, 0xC2 },
        { 0x61, 0x04 },
OOVPA_END;

#if 0 // Moved to 3911
// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4134, 11)

        { 0x02, 0x08 },
        { 0x1F, 0x7C },
        { 0x6F, 0x8B },

        { 0x80, 0x8B },
        { 0x81, 0x54 },
        { 0x82, 0x24 },
        { 0x83, 0x20 },
        { 0x84, 0x8B },
        { 0x85, 0x3F },
        { 0x86, 0x83 },
        { 0x87, 0xE1 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 4134, 11)

        // D3DDevice_UpdateOverlay+0x0F : mov [eax+0x2A88], ecx
        { 0x0F, 0x89 },
        { 0x10, 0x88 },
        { 0x11, 0x88 },
        { 0x12, 0x2A },

        // D3DDevice_UpdateOverlay+0x86 : and ecx, 0xFFFFFFFE
        { 0x86, 0x83 },
        { 0x87, 0xE1 },
        { 0x88, 0xFE },

        // D3DDevice_UpdateOverlay+0xA2 : mov [esi+0x8920], ecx
        { 0xA2, 0x89 },
        { 0xA3, 0x8E },
        { 0xA4, 0x20 },
        { 0xA5, 0x89 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetModelView, 4134, 7)

        { 0x05, 0x57 },
        { 0x0A, 0x85 },
        { 0x10, 0xB0 },
        { 0x16, 0x10 },
        { 0x1C, 0x5E },
        { 0x22, 0xC1 },
        { 0x28, 0xE0 },
OOVPA_END;


// ******************************************************************
// * D3D_MakeRequestedSpace
// ******************************************************************
OOVPA_XREF(D3D_MakeRequestedSpace, 4134, 28, // Also for 4361, 4627

	XREF_D3D_MakeRequestedSpace,
	XRefZero)

		{ 0x00, 0x83 },
		{ 0x01, 0xEC },
		{ 0x02, 0x08 },
		{ 0x03, 0x56 },
		{ 0x04, 0x8B },
		{ 0x05, 0x35 },

		{ 0x0A, 0xF6 },
		{ 0x0B, 0x46 },
		{ 0x0C, 0x08 },
		{ 0x0D, 0x04 },
		{ 0x0E, 0x8B },
		{ 0x0F, 0x0E },
		{ 0x10, 0x57 },
		{ 0x11, 0x74 },
		{ 0x12, 0x26 },
		{ 0x13, 0x8B },
		{ 0x14, 0x86 },
		{ 0x15, 0x50 },
		{ 0x16, 0x03 },
		{ 0x17, 0x00 },
		{ 0x18, 0x00 },
		{ 0x19, 0x8B },
		{ 0x1A, 0x78 },
		{ 0x1B, 0x04 },
		{ 0x1C, 0x8B },
		{ 0x1D, 0x96 },
		{ 0x1E, 0x54 },
		{ 0x1F, 0x03 },
OOVPA_END;
#if 0 // Moved to 4039
// ******************************************************************
// * D3DDevice_RunPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunPushBuffer, 4134, 8)

        { 0x1E, 0x07 },
        { 0x3E, 0x00 },
        { 0x5E, 0x46 },
        { 0x7E, 0x24 },
        { 0x9E, 0x18 },
        { 0xBE, 0x74 },
        { 0xE2, 0x8B },
        { 0xFE, 0x24 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4134, 10)

        { 0x00, 0x56 },
        { 0x07, 0x6A },

        { 0x10, 0x8B },
        { 0x11, 0x44 },
        { 0x12, 0x24 },
        { 0x13, 0x08 },
        { 0x14, 0x40 },
        { 0x15, 0x50 },
        { 0x16, 0x56 },
        { 0x17, 0xE8 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_BeginPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPush, 4134, 7)

        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x10, 0x8B },
        { 0x12, 0x24 },
        { 0x17, 0xE8 },
        { 0x1C, 0x8B },
        { 0x21, 0x01 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_XREF(D3DDevice_EndPush, 4134, 1+11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE ),

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x8B },
        { 0x05, 0x0D },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_EndPush
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPush, 4134, 8)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x0A, 0x89 },
        { 0x0B, 0x01 },
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
        { 0x0E, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BeginPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_BeginPushBuffer, 4134, 10)

        { 0x07, 0x57 },
        { 0x1C, 0xBE },

        { 0x50, 0xFC },
        { 0x51, 0xFD },
        { 0x52, 0xFF },
        { 0x53, 0xFF },
        { 0x54, 0x8B },
        { 0x55, 0x46 },
        { 0x56, 0x08 },
        { 0x57, 0x83 },
OOVPA_END;

#if 0 // Moved to 3911
// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 4134, 13) // Up to 5849

        { 0x01, 0x56 },
        { 0x11, 0x8D },

        { 0x2E, 0x8D },
        { 0x2F, 0x4C },
        { 0x30, 0x11 },
        { 0x31, 0x04 },
        { 0x32, 0x89 },
        { 0x33, 0x48 },
        { 0x34, 0x0C },
        { 0x35, 0x8B },
        { 0x36, 0x86 },

        { 0x6D, 0x8B },
        { 0x6E, 0x86 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 4134, 7)

        { 0x11, 0x8D },
        { 0x24, 0x8B },
        { 0x37, 0x58 },
        { 0x4A, 0xFF },
        { 0x5D, 0xF7 },
        { 0x70, 0x03 },
        { 0x83, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetModelView
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetModelView, 4134, 7)

        { 0x15, 0x25 },
        { 0x2F, 0x81 },
        { 0x43, 0x43 },
        { 0x5A, 0x04 },
        { 0x72, 0x8D },
        { 0x88, 0x00 },
        { 0x9F, 0x75 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexData4s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4s, 4134, 10)

        { 0x00, 0x56 },
        { 0x08, 0x06 },

        { 0x15, 0x24 },
        { 0x16, 0x08 },
        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0xCD },
        { 0x1A, 0x80 },
        { 0x1B, 0x19 },
        { 0x1F, 0xBF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexShaderInput, 4134, 16)

        { 0x03, 0x04 },
        { 0x04, 0x83 },
        { 0x05, 0xEC },
        { 0x06, 0x40 },
        { 0x07, 0x85 },
        { 0x08, 0xC0 },
        { 0x09, 0x53 },
        { 0x0A, 0x8B },
        { 0x0B, 0x1D },

        { 0x19, 0x25 },
        { 0x1A, 0xFF },
        { 0x1B, 0xFF },
        { 0x1C, 0xFF },
        { 0x1D, 0xBF },
        { 0x1E, 0x83 },
        { 0x1F, 0xC8 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PrimeVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_PrimeVertexCache, 4134, 15)

        { 0x02, 0x2D },

        { 0x07, 0x8B },
        { 0x08, 0x45 },
        { 0x09, 0x1C },
        { 0x0A, 0x56 },
        { 0x0B, 0x50 },
        { 0x0C, 0x8B },
        { 0x0D, 0xCD },

        { 0x19, 0x00 },
        { 0x1A, 0x8B },
        { 0x1B, 0x4D },
        { 0x1C, 0x04 },
        { 0x1D, 0xD1 },
        { 0x1E, 0xEE },
        { 0x1F, 0x3B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_InsertCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_InsertCallback, 4134, 13)

        { 0x02, 0x35 },

        { 0x07, 0x8B },
        { 0x0E, 0xA1 },

        { 0x1F, 0x0C },
        { 0x20, 0x8B },
        { 0x21, 0x54 },
        { 0x22, 0x24 },
        { 0x23, 0x10 },
        { 0x24, 0xC7 },
        { 0x25, 0x00 },
        { 0x26, 0x8C },
        { 0x27, 0x1D },
        { 0x28, 0x08 },
OOVPA_END;

#if 0 // Moved to 3911
// ******************************************************************
// * D3DDevice_LoadVertexShaderProgram
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_LoadVertexShaderProgram, 4134, 16)

        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x04, 0x53 },
        { 0x05, 0x0F },
        { 0x06, 0xB7 },
        { 0x07, 0x58 },
        { 0x08, 0x02 },
        { 0x09, 0x55 },
        { 0x0A, 0x8B },
        { 0x0B, 0x2D },

        { 0x14, 0xE3 },
        { 0x15, 0x02 },
        { 0x16, 0xF6 },
        { 0x17, 0xC1 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetShaderConstantMode
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetShaderConstantMode, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x18 },
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

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetVertexShaderInput
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderInput, 4134, 15)

        { 0x05, 0xC9 },

        { 0x0B, 0x74 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x89 },
        { 0x14, 0x11 },
        { 0x15, 0x8B },
        { 0x16, 0x4C },
        { 0x17, 0x24 },
        { 0x18, 0x04 },
        { 0x19, 0x85 },
        { 0x1A, 0xC9 },
        { 0x1B, 0x74 },
        { 0x1C, 0x08 },
        { 0x1D, 0x8B },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_RunVertexStateShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_RunVertexStateShader, 4134, 7)

        { 0x09, 0x3B },
        { 0x14, 0x4C },
        { 0x1F, 0xC0 },
        { 0x2A, 0x40 },
        { 0x35, 0xF4 },
        { 0x40, 0xC7 },
        { 0x4B, 0x08 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetVertexShaderDeclaration
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderDeclaration, 4134, 14)

        { 0x04, 0x8A },
        { 0x13, 0xC7 },
        { 0x14, 0x01 },
        { 0x15, 0x00 },
        { 0x16, 0x00 },
        { 0x17, 0x00 },
        { 0x18, 0x00 },
        { 0x19, 0x33 },
        { 0x1A, 0xC0 },
        { 0x1B, 0x83 },
        { 0x1C, 0xC4 },
        { 0x1D, 0x10 },
        { 0x1E, 0xC2 },
        { 0x1F, 0x0C },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetVertexShaderFunction
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShaderFunction, 4134, 12)

        { 0x01, 0x8B },
        { 0x09, 0x75 },
        { 0x0A, 0x10 },
        { 0x0B, 0x8B },
        { 0x0C, 0x44 },
        { 0x0D, 0x24 },
        { 0x0E, 0x10 },
        { 0x0F, 0xC7 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x1C, 0x56 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_GetProjectionViewportMatrix
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetProjectionViewportMatrix, 4134, 16) // Up to 4531

        { 0x02, 0x35 },

        { 0x07, 0x57 },

        { 0x0E, 0x70 }, // 4361 0x7004
        { 0x0F, 0x04 },
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
// * D3DDevice_SetVertexData4ub
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData4ub, 4134, 7)

        { 0x08, 0x06 },
        { 0x13, 0x8B },
        { 0x1C, 0x04 },
        { 0x26, 0x0F },
        { 0x30, 0x24 },
        { 0x3A, 0x24 },
        { 0x44, 0x89 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DCubeTexture_GetCubeMapSurface
// ******************************************************************
OOVPA_NO_XREF(D3DCubeTexture_GetCubeMapSurface, 4134, 7)

        { 0x09, 0x44 },
        { 0x14, 0x24 },
        { 0x1F, 0x50 },
        { 0x2A, 0x50 },
        { 0x35, 0x8B },
        { 0x40, 0x24 },
        { 0x4B, 0x5E },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3DDevice_GetViewport
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetViewport, 4134, 10)

        // D3DDevice_GetViewport+0x05 : push esi; push edi
        { 0x05, 0x56 },
        { 0x06, 0x57 },

        // D3DDevice_GetViewport+0x0B : lea esi, [eax+0x9D0]
        { 0x0B, 0x8D },
        { 0x0C, 0xB0 },
        { 0x0D, 0xD0 },
        { 0x0E, 0x09 },

        // D3DDevice_GetViewport+0x11 : mov ecx, 6
        { 0x11, 0xB9 },
        { 0x12, 0x06 },

        // D3DDevice_GetViewport+0x1A : retn 0x04
        { 0x1A, 0xC2 },
        { 0x1B, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4134, 12)

        { 0x14, 0x3D },
        { 0x36, 0x8B },

        { 0x50, 0x83 },
        { 0x51, 0xC3 },
        { 0x52, 0x08 },
        { 0x53, 0xFF },
        { 0x54, 0x45 },
        { 0x55, 0xF8 },
        { 0x56, 0x8B },
        { 0x57, 0x45 },
        { 0x58, 0xF8 },

        { 0x6A, 0xE8 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4134 version
// ******************************************************************
// * D3DDevice_CaptureStateBlock
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_CaptureStateBlock, 4134, 9)

        // D3DDevice_CaptureStateBlock+0x36 : mov eax, [edi+eax*4+0x0A78]
        { 0x36, 0x8B },
        { 0x37, 0x84 },
        { 0x38, 0x87 },
        { 0x39, 0x78 },
        { 0x3A, 0x0A },

        // D3DDevice_CaptureStateBlock+0xA9 : cmp dword ptr [esi+0x0C], 0
        { 0xA9, 0x83 },
        { 0xAA, 0x7E },
        { 0xAB, 0x0C },
        { 0xAC, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_BeginStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_BeginStateBlock, 4134, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_BeginStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_ClearStateBlockFlags ),

        // D3DDevice_BeginStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_BeginStateBlock+0x05 : mov [eax+8], 0x20
        { 0x05, 0x83 },
        { 0x06, 0x48 },
        { 0x07, 0x08 },
        { 0x08, 0x20 },
OOVPA_END;
#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * D3D_ClearStateBlockFlags
// ******************************************************************
OOVPA_XREF(D3D_ClearStateBlockFlags, 4134, 9,//OK

    XREF_D3D_ClearStateBlockFlags,
    XRefZero)

        // D3D_ClearStateBlockFlags+0x0A : movzx ecx, 0x82
        { 0x0A, 0xB9 },
        { 0x0B, 0x82 },

        // D3D_ClearStateBlockFlags+0x16 : mov ecx, [edx+0x0390]
        { 0x16, 0x8B },
        { 0x17, 0x8A },
        { 0x18, 0x90 },
        { 0x19, 0x03 },

        // D3D_ClearStateBlockFlags+0x2F : add ecx, 0x90
        { 0x2F, 0x81 },
        { 0x30, 0xC1 },
        { 0x31, 0x90 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_EndStateBlock
// ******************************************************************
OOVPA_XREF(D3DDevice_EndStateBlock, 4134, 6,

    XRefNoSaveIndex,
    XRefOne)

        // D3DDevice_EndStateBlock+0x0F : call [ClearStateBlockFlags]
        XREF_ENTRY( 0x0A, XREF_D3D_RecordStateBlock ),

        // D3DDevice_EndStateBlock+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_EndStateBlock+0x05 : and [eax+8], 0xFFFFFFDF
        { 0x05, 0x83 },
        { 0x06, 0x60 },
        { 0x07, 0x08 },
        { 0x08, 0xDF },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetVertexShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetVertexShader, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x84 },
        { 0x08, 0x03 },
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
// * D3DDevice_GetPixelShader
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetPixelShader, 4134, 16)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x70 },
        { 0x08, 0x03 },
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
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4134, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x043C]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x3C },
        { 0x08, 0x04 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x043C], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x3C },
        { 0x0F, 0x04 },
OOVPA_END;
#if 0 // No longer used, replaced by another 4134 version
// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 4134, 10)

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        { 0x09, 0x00 },
        { 0x0D, 0x88 },

        { 0x0F, 0x04 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x8B },
        { 0x13, 0xC1 },
        { 0x14, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * D3DDevice_SetVertexData2s
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetVertexData2s, 4134, 11)

        { 0x02, 0x35 },

        { 0x17, 0x8D },
        { 0x18, 0x14 },
        { 0x19, 0x8D },
        { 0x1A, 0x00 },
        { 0x1B, 0x19 },
        { 0x1C, 0x04 },
        { 0x1D, 0x00 },
        { 0x1E, 0x0F },
        { 0x1F, 0xBF },
        { 0x25, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_FlushVertexCache
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_FlushVertexCache, 4134, 10)

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x08, 0x06 },
        { 0x0D, 0x05 },
        { 0x13, 0xC7 },
        { 0x15, 0x10 },
        { 0x17, 0x04 },
        { 0x1C, 0x00 },
        { 0x21, 0xC0 },
        { 0x26, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackBufferScale
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackBufferScale, 4134, 16)

        { 0x09, 0x86 },

        { 0x10, 0xC1 },
        { 0x11, 0xE9 },
        { 0x12, 0x04 },
        { 0x13, 0x83 },
        { 0x14, 0xE1 },
        { 0x15, 0x0F },
        { 0x16, 0x85 },
        { 0x17, 0xC9 },

        { 0x28, 0xD8 },
        { 0x29, 0x4C },
        { 0x2A, 0x24 },
        { 0x2B, 0x0C },
        { 0x2C, 0x83 },
        { 0x2D, 0xE0 },
        { 0x2E, 0x0F },
OOVPA_END;

// ******************************************************************
// * D3DDevice_MakeSpace
// ******************************************************************
OOVPA_XREF(D3DDevice_MakeSpace, 4134, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0A, XREF_D3D_MakeRequestedSpace ),

        { 0x00, 0xA1 },
        { 0x05, 0x50 },
        { 0x06, 0xD1 },
        { 0x07, 0xE8 },
        { 0x08, 0x50 },
        { 0x09, 0xE8 },
        { 0x0E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * Rollback support signature(s)
// ******************************************************************
#define D3DDevice_BeginVisibilityTest_4134 D3DDevice_BeginVisibilityTest_3925
#define D3DDevice_EndVisibilityTest_4134 D3DDevice_EndVisibilityTest_3925

// ******************************************************************
// * D3D8_4134
// ******************************************************************
OOVPATable D3D8_4134[] = {

	REGISTER_OOVPA(CMiniport_InitHardware, 4034, PATCH),
	REGISTER_OOVPA(CMiniport_CreateCtxDmaObject, 4034, PATCH),
	REGISTER_OOVPA(D3D_CMiniport_GetDisplayCapabilities, 3911, PATCH),

	REGISTER_OOVPA(Direct3D_CreateDevice, 3911, PATCH),
	REGISTER_OOVPA(D3D_CheckDeviceFormat, 3911, DISABLED),
	REGISTER_OOVPA(D3D_MakeRequestedSpace, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_TexCoordIndex, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_CullMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTransform, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FogColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Swap, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Clear, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginVisibilityTest, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_EndVisibilityTest, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDeviceCaps, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_LoadVertexShader, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetViewport, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Begin, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexDataColor, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_End, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePalette, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPalette, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateTexture, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVolumeTexture, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateCubeTexture, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateIndexBuffer, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_CreateVertexShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetFlickerFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSoftDisplayFilter, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BorderColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_PSTextureModes, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZBias, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_Present, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateVertexBuffer, 3911, DISABLED),
	REGISTER_OOVPA(D3DResource_Register, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_AddRef, 3911, PATCH),
	REGISTER_OOVPA(D3DResource_IsBusy, 4039, PATCH),
	REGISTER_OOVPA(D3DPalette_Lock, 3911, PATCH),
	REGISTER_OOVPA(D3DTexture_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DVolumeTexture_LockBox, 3911, PATCH),	// Just calls Lock3DSurface
	REGISTER_OOVPA(D3DCubeTexture_LockRect, 3911, PATCH),	// Just calls Lock2DSurface
	REGISTER_OOVPA(D3DTexture_GetSurfaceLevel, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4f, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDepthStencilSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_CreatePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DSurface_LockRect, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteVertexShader, 3911, PATCH),
	REGISTER_OOVPA(Lock3DSurface, 3911, PATCH),
	REGISTER_OOVPA(Lock2DSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetGammaRamp, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SelectVertexShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_CopyRects, 4034, PATCH),
	REGISTER_OOVPA(D3D_CreateStandAloneSurface, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_CreateImageSurface, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderConstant, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShader, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_BumpEnv, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetIndices, 4034, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetTexture, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_VertexBlend, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TextureFactor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_EdgeAntiAlias, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Simple, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ZEnable, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleAntiAlias, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTransform, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetStreamSource, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVertices, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawVerticesUP, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVertices, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetLightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetLight, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LightEnable, 3911, PATCH),
	REGISTER_OOVPA(D3DVertexBuffer_Lock, 4034, PATCH),
	REGISTER_OOVPA(Get2DSurfaceDesc, 4034, PATCH),
	REGISTER_OOVPA(D3DSurface_GetDesc, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilFail, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_NormalizeNormals, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_Reset, 3911, PATCH),
	REGISTER_OOVPA(D3D_KickOffAndWaitForIdle, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_TwoSidedLighting, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_BackFillMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTextureState_ColorKeyColor, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FrontFace, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LogicOp, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_OcclusionCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_StencilCullEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_FillMode, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZCmpAlwaysRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_RopZRead, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_DoNotCullUncompressed, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_Dxt1NoiseEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMask, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_MultiSampleRenderTargetMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_ShadowFunc, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_YuvEnable, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_DrawIndexedVerticesUP, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderConstant, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_DeletePixelShader, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScreenSpaceOffset, 4039, PATCH),
	REGISTER_OOVPA(D3D_SetFence, 4134, XREF),
	REGISTER_OOVPA(D3DDevice_InsertFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetRenderState_LineWidth, 4034, PATCH),
	REGISTER_OOVPA(D3DDevice_GetGammaRamp, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockUntilVerticalBlank, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetTile, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetTile, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_IsBusy, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderSize, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_KickOff, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_BlockOnFence, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetRenderTarget, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_Release, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayMode, 4039, PATCH),
	REGISTER_OOVPA(D3D_GetAdapterIdentifier, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_SetShaderConstantMode, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetCreationParameters, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVisibilityTestResult, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVerticalBlankCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetSwapCallback, 4134, PATCH),
	REGISTER_OOVPA(D3D_LazySetPointParams, 4039, PATCH),
	REGISTER_OOVPA(D3D_SetPushBufferSize, 3911, DISABLED),
	REGISTER_OOVPA(D3DDevice_EnableOverlay, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_UpdateOverlay, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_FlushVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3D_BlockOnTime, 3911, PATCH),
	REGISTER_OOVPA(D3D_BlockOnResource, 4034, PATCH),
	REGISTER_OOVPA(D3DResource_BlockUntilNotBusy, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPushBuffer, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPushBuffer, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_RunPushBuffer, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPushBufferOffset, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackBufferScale, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetDisplayFieldStatus, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_PersistDisplay, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexShaderInput, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_PrimeVertexCache, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderType, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_IsFencePending, 4039, PATCH),
	REGISTER_OOVPA(D3D_ClearStateBlockFlags, 3911, XREF),
	REGISTER_OOVPA(D3D_RecordStateBlock, 3911, XREF),
	REGISTER_OOVPA(D3DDevice_BeginStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetModelView, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData2s, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4s, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetVertexData4ub, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_InsertCallback, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_LoadVertexShaderProgram, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetShaderConstantMode, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderInput, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_RunVertexStateShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderDeclaration, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderFunction, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetProjectionViewportMatrix, 4134, PATCH),
	REGISTER_OOVPA(D3DBaseTexture_GetLevelCount, 3911, PATCH),
	REGISTER_OOVPA(D3DCubeTexture_GetCubeMapSurface, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_ApplyStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CaptureStateBlock, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_DeleteStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_CreateStateBlock, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetPixelShaderProgram, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SetScissors, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_SwitchTexture, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetVertexShaderConstant, 4039, PATCH),
	REGISTER_OOVPA(D3DDevice_GetPixelShader, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_BeginPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_EndPush, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_SetBackMaterial, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_AddRef, 4134, PATCH),
	REGISTER_OOVPA(D3DDevice_GetViewport, 3911, PATCH),
	REGISTER_OOVPA(D3DDevice_GetOverlayUpdateStatus, 3911, PATCH),

	// ******************************************************************
	// Provisional registration functions in XDK 4134
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(D3DDevice_DrawRectPatch, 3911, PATCH),
//	REGISTER_OOVPA(D3DDevice_SelectVertexShaderDirect, 4361, PATCH),
//	REGISTER_OOVPA(D3DDevice_SetVertexShaderInputDirect, 4361, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * D3D8_4134_SIZE
// ******************************************************************
uint32 D3D8_4134_SIZE = sizeof(D3D8_4134);
