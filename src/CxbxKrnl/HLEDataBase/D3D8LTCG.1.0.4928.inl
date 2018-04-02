// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8LTCG.1.0.4928.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * D3DDevice_CopyRects
// ******************************************************************
//81EC940100005355568B
OOVPA_NO_XREF(D3DDevice_CopyRects, 1048, 10)

        { 0x00, 0x81 },
        { 0x01, 0xEC },
        { 0x02, 0x94 },
        { 0x03, 0x01 },
        { 0x04, 0x00 },
        { 0x05, 0x00 },
        { 0x06, 0x53 },
        { 0x07, 0x55 },
        { 0x08, 0x56 },
        { 0x09, 0x8B },
OOVPA_END;

// ******************************************************************
// * D3DDevice_CreateTexture2
// ******************************************************************
//F744241C0000010074
OOVPA_NO_XREF(D3DDevice_CreateTexture2, 1024, 11)

        { 0x00, 0x56 },
        { 0x01, 0x57 },

        { 0x42, 0xF7 },
        { 0x43, 0x44 },
        { 0x44, 0x24 },
        { 0x45, 0x1C },
        { 0x46, 0x00 },
        { 0x47, 0x00 },
        { 0x48, 0x01 },
        { 0x49, 0x00 },
        { 0x4A, 0x74 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_EndPushBuffer
// ******************************************************************
//B82908768859
OOVPA_NO_XREF(D3DDevice_EndPushBuffer, 1024, 8)

        { 0x00, 0x51 },
        { 0x01, 0x56 },

        { 0x7C, 0xB8 },
        { 0x7D, 0x29 },
        { 0x7E, 0x08 },
        { 0x7F, 0x76 },
        { 0x80, 0x88 },
        { 0x81, 0x59 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackBuffer2
// ******************************************************************
//7507B801000000EB07F7
OOVPA_NO_XREF(D3DDevice_GetBackBuffer2, 1024, 14)

        { 0x00, 0x8B },
        { 0x01, 0x44 },

        { 0x0D, 0x75 },
        { 0x0E, 0x07 },
        { 0x0F, 0xB8 },
        { 0x10, 0x01 },
        { 0x11, 0x00 },
        { 0x12, 0x00 },
        { 0x13, 0x00 },
        { 0x14, 0xEB },
        { 0x15, 0x07 },
        { 0x16, 0xF7 },

        { 0x4A, 0xC2 },
        { 0x4B, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetDepthStencilSurface2
// ******************************************************************
//000085C9750333C0C38B01A9
OOVPA_NO_XREF(D3DDevice_GetDepthStencilSurface2, 1024, 14)

        { 0x00, 0xA1 },
        { 0x11, 0xC3 },

        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x85 },
        { 0x0C, 0xC9 },
        { 0x0D, 0x75 },
        { 0x0E, 0x03 },
        { 0x0F, 0x33 },
        { 0x10, 0xC0 },
        { 0x11, 0xC3 },
        { 0x12, 0x8B },
        { 0x13, 0x01 },
        { 0x14, 0xA9 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetRenderTarget2
// ******************************************************************
//8B88041A000085
OOVPA_XREF(D3DDevice_GetRenderTarget2, 1024, 1+7,

	XREF_D3DDevice_GetRenderTarget2,
	XRefOne)

        XREF_ENTRY( 0x2D, XREF_D3DResource_AddRef ),

        { 0x00, 0xA1 },

        { 0x05, 0x8B },
        //{ 0x06, 0x88 },
        //{ 0x07, 0x04 },
        //{ 0x08, 0x1A },
        { 0x09, 0x00 },
        { 0x0A, 0x00 },
        { 0x0B, 0x85 },

        { 0x34, 0xC1 },
        { 0x35, 0xC3 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_PersistDisplay
// ******************************************************************
//85C0740F50FF15 ...C3
OOVPA_NO_XREF(D3DDevice_PersistDisplay_0, 2060, 9)

        { 0x00, 0x51 },
        { 0x01, 0x55 },

        { 0x0E, 0x85 },
        { 0x0F, 0xC0 },
        { 0x10, 0x74 },
        { 0x11, 0x0F },
        { 0x12, 0x50 },
        { 0x13, 0xFF },
        { 0x14, 0x15 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetPixelShaderConstant
// ******************************************************************
//450C8B451085
OOVPA_NO_XREF(D3DDevice_SetPixelShaderConstant, 1024, 8)

        { 0x00, 0x55 },
        { 0x07, 0x15 },

        { 0x19, 0x45 },
        { 0x1A, 0x0C },
        { 0x1B, 0x8B },
        { 0x1C, 0x45 },
        { 0x1D, 0x10 },
        { 0x1E, 0x85 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_UpdateOverlay
// ******************************************************************
//7707B800001000EB
OOVPA_NO_XREF(D3DDevice_UpdateOverlay, 1048, 9)

        { 0x01, 0xA1 },

        { 0xFD, 0x77 },
        { 0xFE, 0x07 },
        { 0xFF, 0xB8 },
        { 0x100, 0x00 },
        { 0x101, 0x00 },
        { 0x102, 0x10 },
        { 0x103, 0x00 },
        { 0x104, 0xEB },
OOVPA_END;

// ******************************************************************
// * D3DTexture_GetSurfaceLevel2
// ******************************************************************
//7C24148D442418508D
OOVPA_NO_XREF(D3DTexture_GetSurfaceLevel2, 1024, 12)

        { 0x00, 0x83 },

        { 0x05, 0x7C },
        { 0x06, 0x24 },
        { 0x07, 0x14 },
        { 0x08, 0x8D },
        { 0x09, 0x44 },
        { 0x0A, 0x24 },
        { 0x0B, 0x18 },
        { 0x0C, 0x50 },
        { 0x0D, 0x8D },

        { 0x45, 0xC2 },
        { 0x46, 0x08 },
OOVPA_END;
