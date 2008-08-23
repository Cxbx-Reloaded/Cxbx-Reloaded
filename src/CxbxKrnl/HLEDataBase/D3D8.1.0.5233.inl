// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5233.cpp
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
// * IDirect3DDevice8_GetDisplayFieldStatus
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x06, 0x90 },
        { 0x0E, 0x04 },
        { 0x16, 0x00 },
        { 0x1E, 0x56 },
        { 0x26, 0xD2 },
        { 0x2E, 0x89 },
        { 0x36, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_LoadVertexShader
// ******************************************************************
SOOVPA<10> IDirect3DDevice8_LoadVertexShader_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x09, 0x45 },
        { 0x14, 0x75 },
        { 0x21, 0x8B },
        { 0x2D, 0x8B },
        { 0x35, 0x04 },
        { 0x40, 0x00 },
        { 0x4B, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetVertexShaderConstantNotInline
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x26 : cmp eax, 0x10
        { 0x26, 0x83 }, // (Offset,Value)-Pair #1
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #2
        { 0x28, 0x10 }, // (Offset,Value)-Pair #3

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x29 : jnb +0x1F
        { 0x29, 0x73 }, // (Offset,Value)-Pair #4
        { 0x2A, 0x1F }, // (Offset,Value)-Pair #5

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x33 : or eax, 0x0B80
        { 0x33, 0x0D }, // (Offset,Value)-Pair #6
        { 0x34, 0x80 }, // (Offset,Value)-Pair #7
        { 0x35, 0x0B }, // (Offset,Value)-Pair #8
        { 0x36, 0x00 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x3B : rep movsd
        { 0x3B, 0xF3 }, // (Offset,Value)-Pair #10
        { 0x3C, 0xA5 }, // (Offset,Value)-Pair #11

        // IDirect3DDevice8_SetVertexShaderConstantNotInline+0x43 : emms
        { 0x43, 0x0F }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * IDirect3DDevice8_Clear
// ******************************************************************
LOOVPA<8> IDirect3DDevice8_Clear_1_0_5233 =
{
    1,  // Large == 1
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1D, 0x8A },
        { 0x3C, 0x8B },
        { 0x5B, 0x00 },
        { 0x7A, 0xFD },
        { 0x99, 0x25 },
        { 0xB8, 0x00 },
        { 0xD7, 0x75 },
        { 0xF6, 0xFF },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetPixelShader
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetPixelShader_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x75 },
        { 0x3E, 0x0D },
        { 0x60, 0x8B },
        { 0x7E, 0x04 },
        { 0x9E, 0x20 },
        { 0xBE, 0x08 },
        { 0xE1, 0xF6 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
SOOVPA<13> IDirect3DDevice8_SetRenderState_CullMode_1_0_5233 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_DXSRSCULLMODE, // XRef Is Saved
    0,                  // XRef Not Used

    {
        // IDirect3DDevice8_SetRenderState_CullMode+0x19 : mov dword ptr [eax], 0x40308
        { 0x19, 0xC7 }, // (Offset,Value)-Pair #1
        { 0x1B, 0x08 }, // (Offset,Value)-Pair #2
        { 0x1C, 0x03 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x04 }, // (Offset,Value)-Pair #4

        // IDirect3DDevice8_SetRenderState_CullMode+0x24 : add eax, 8
        { 0x24, 0x83 }, // (Offset,Value)-Pair #5
        { 0x25, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x26, 0x08 }, // (Offset,Value)-Pair #7

        // IDirect3DDevice8_SetRenderState_CullMode+0x30 : retn 4
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x31, 0x04 }, // (Offset,Value)-Pair #9

        // IDirect3DDevice8_SetRenderState_CullMode+0x53 : add edx, 0x404
        { 0x53, 0x81 }, // (Offset,Value)-Pair #10
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x04 }, // (Offset,Value)-Pair #12
        { 0x56, 0x04 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetRenderState_ZEnable_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x13, 0x8B },
        { 0x22, 0xC9 },
        { 0x34, 0x89 },
        { 0x46, 0x0C },
        { 0x5A, 0x74 },
        { 0x6D, 0x8B },
        { 0x7C, 0xCE },
        { 0x8E, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetIndices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_SetIndices_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x0F, 0x10 },
        { 0x20, 0xC7 },
        { 0x31, 0xC0 },
        { 0x42, 0x78 },
        { 0x53, 0xBE },
        { 0x64, 0x89 },
        { 0x75, 0x10 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetTexture
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_SetTexture_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1E, 0x44 },
        { 0x3E, 0xE8 },
        { 0x5F, 0x8B },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0x89 },
        { 0xDE, 0xFF },
        { 0xFE, 0xE2 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Begin
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_Begin_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x07, 0xE8 },
        { 0x0C, 0x8B },
        { 0x13, 0xE8 },
        { 0x1A, 0x24 },
        { 0x21, 0x00 },
        { 0x28, 0x89 },
        { 0x2F, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_Swap
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_Swap_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        { 0x1B, 0xE8 },
        { 0x38, 0xC3 },
        { 0x55, 0xC0 },
        { 0x72, 0x3F },
        { 0x8F, 0x8E },
        { 0xAC, 0xC6 },
        { 0xC9, 0xC0 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_DrawIndexedVertices
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_DrawIndexedVertices_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0x5E },
        { 0x3E, 0x17 },
        { 0x5E, 0x00 },
        { 0x7E, 0x8B },
        { 0x9E, 0x89 },
        { 0xBE, 0x2B },
        { 0xDE, 0x00 },
        { 0xFE, 0x04 },
    }
};

// ******************************************************************
// * IDirect3DPalette8_Lock2
// ******************************************************************
SOOVPA<13> IDirect3DPalette8_Lock2_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x02, 0x24 },
        { 0x06, 0x8B },
        { 0x0A, 0x75 },
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x16, 0x00 },
        { 0x1A, 0x5E },
    }
};

// ******************************************************************
// * IDirect3DDevice8_SetShaderConstantMode
// ******************************************************************
SOOVPA<12> IDirect3DDevice8_SetShaderConstantMode_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1D, 0xFD },
        { 0x3C, 0x8B },
        { 0x5B, 0x0B },
        { 0x7A, 0x56 },
        { 0x99, 0xC0 },
        { 0xB8, 0x00 },
        { 0xD7, 0x89 },
        { 0xF6, 0x00 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetRenderTarget2
// ******************************************************************
SOOVPA<6> IDirect3DDevice8_GetRenderTarget2_1_0_5233 =
{
    0,  // Large == 0
    6,  // Count == 6

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x74 },
        { 0x16, 0x8B },
        { 0x17, 0xC6 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_GetDepthStencilSurface2
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x56 },
        { 0x06, 0x8B },
        { 0x0A, 0x00 },
        { 0x0E, 0x75 },
        { 0x12, 0x5E },
        { 0x1A, 0x8B },
        { 0x1B, 0xC6 },
    }
};

// ******************************************************************
// * IDirect3D8_GetDeviceCaps
// ******************************************************************
SOOVPA<7> IDirect3D8_GetDeviceCaps_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

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
// * IDirect3DDevice8_DrawVertices
// ******************************************************************
SOOVPA<7> IDirect3DDevice8_DrawVertices_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x12, 0x8B },
        { 0x26, 0x8B },
        { 0x37, 0x00 },
        { 0x4A, 0x27 },
        { 0x5D, 0x89 },
        { 0x70, 0x75 },
        { 0x83, 0x40 },
    }
};

// ******************************************************************
// * IDirect3DVertexBuffer8_GetDesc
// ******************************************************************
SOOVPA<7> IDirect3DVertexBuffer8_GetDesc_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x02, 0x24 },
        { 0x06, 0x74 },
        { 0x0A, 0xC7 },
        { 0x0E, 0x00 },
        { 0x15, 0x89 },
        { 0x16, 0x46 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * IDirect3DDevice8_CopyRects
// ******************************************************************
SOOVPA<8> IDirect3DDevice8_CopyRects_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x1E, 0xE1 },
        { 0x42, 0x84 },
        { 0x5E, 0x24 },
        { 0x7E, 0x8B },
        { 0x9E, 0x1C },
        { 0xBE, 0x00 },
        { 0xDE, 0xBD },
        { 0xFE, 0x4C },
    }
};

// ******************************************************************
// * D3D8_1_0_5233
// ******************************************************************
OOVPATable D3D8_1_0_5233[] =
{
    // IDirect3DDevice8::GetDisplayFieldStatus
    {
        (OOVPA*)&IDirect3DDevice8_GetDisplayFieldStatus_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetDisplayFieldStatus,

        #ifdef _DEBUG_TRACE
        "IDirect3DDevice8_GetDisplayFieldStatus"
        #endif
    },
    // IDirect3D8::CheckDeviceFormat (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3D8_CheckDeviceFormat_1_0_4361,

        XTL::EmuIDirect3D8_CheckDeviceFormat,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_CheckDeviceFormat"
        #endif
    },
    // IDirect3DDevice8::CopyRects
    {
        (OOVPA*)&IDirect3DDevice8_CopyRects_1_0_5233,

        XTL::EmuIDirect3DDevice8_CopyRects,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CopyRects"
        #endif
    },
    // IDirect3DDevice8::GetGammaRamp (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_GetGammaRamp_1_0_4627,

        XTL::EmuIDirect3DDevice8_GetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetGammaRamp"
        #endif
    },
    // IDirect3DDevice8::SetGammaRamp (* unchanged since 4928 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetGammaRamp_1_0_4928,

        XTL::EmuIDirect3DDevice8_SetGammaRamp,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetGammaRamp"
        #endif
    },
    // IDirect3DDevice8::SetShaderConstantMode
    {
        (OOVPA*)&IDirect3DDevice8_SetShaderConstantMode_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetShaderConstantMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetShaderConstantMode"
        #endif
    },
    // IDirect3DDevice8::GetRenderTarget2
    {
        (OOVPA*)&IDirect3DDevice8_GetRenderTarget2_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetRenderTarget2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetRenderTarget2"
        #endif
    },
    // IDirect3DDevice8::GetDepthStencilSurface2
    {
        (OOVPA*)&IDirect3DDevice8_GetDepthStencilSurface2_1_0_5233,

        XTL::EmuIDirect3DDevice8_GetDepthStencilSurface2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetDepthStencilSurface2"
        #endif
    },
    // IDirect3DDevice8::CreateIndexBuffer2 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateIndexBuffer2_1_0_4627,

        XTL::EmuIDirect3DDevice8_CreateIndexBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateIndexBuffer2"
        #endif
    },
    // IDirect3DDevice8::CreateVertexShader (* unchanged since 3925 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexShader_1_0_3925,

        XTL::EmuIDirect3DDevice8_CreateVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexShader"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant1 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant1_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant1,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant1"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstant4 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstant4_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstant4,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstant4"
        #endif
    },
    // IDirect3DDevice8::SetVertexShaderConstantNotInline
    {
        (OOVPA*)&IDirect3DDevice8_SetVertexShaderConstantNotInline_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetVertexShaderConstantNotInline,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetVertexShaderConstantNotInline"
        #endif
    },
    // IDirect3DDevice8::DeletePixelShader (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_DeletePixelShader_1_0_4627,

        XTL::EmuIDirect3DDevice8_DeletePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeletePixelShader"
        #endif
    },
    // IDirect3DDevice8::CreatePixelShader (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreatePixelShader_1_0_4361,

        XTL::EmuIDirect3DDevice8_CreatePixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreatePixelShader"
        #endif
    },
    // IDirect3DDevice8::SetPixelShader
    {
        (OOVPA*)&IDirect3DDevice8_SetPixelShader_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetPixelShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetPixelShader"
        #endif
    },
    // IDirect3DDevice8::CreateTexture2C(* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateTexture2C_1_0_4627,

        XTL::EmuIDirect3DDevice8_CreateTexture2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateTexture2C"
        #endif
    },
    // IDirect3DDevice8::SetIndices
    {
        (OOVPA*)&IDirect3DDevice8_SetIndices_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetIndices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetIndices"
        #endif
    },
    // IDirect3DDevice8::SetTexture
    {
        (OOVPA*)&IDirect3DDevice8_SetTexture_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTexture"
        #endif
    },
    // IDirect3DDevice8::SetTextureC (* unchanged since 4928 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureC_1_0_4928,

        XTL::EmuIDirect3DDevice8_SetTexture,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureC"
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
    // IDirect3DDevice8::Begin
    {
        (OOVPA*)&IDirect3DDevice8_Begin_1_0_5233,

        XTL::EmuIDirect3DDevice8_Begin,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Begin"
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
    // IDirect3DDevice8::End (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_End_1_0_4627,

        XTL::EmuIDirect3DDevice8_End,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_End"
        #endif
    },
    // IDirect3DDevice8::Clear
    {
        (OOVPA*)&IDirect3DDevice8_Clear_1_0_5233,

        XTL::EmuIDirect3DDevice8_Clear,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Clear"
        #endif
    },
    // IDirect3DDevice8::Swap
    {
        (OOVPA*)&IDirect3DDevice8_Swap_1_0_5233,

        XTL::EmuIDirect3DDevice8_Swap,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_Swap"
        #endif
    },
    // IDirect3DDevice8::EnableOverlay (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_EnableOverlay_1_0_4361,

        XTL::EmuIDirect3DDevice8_EnableOverlay,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_EnableOverlay"
        #endif
    },
    // IDirect3DDevice8::CreateVertexBuffer2 (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_CreateVertexBuffer2_1_0_4627,

        XTL::EmuIDirect3DDevice8_CreateVertexBuffer2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_CreateVertexBuffer2"
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
    // IDirect3DDevice8::SetTextureState_ColorKeyColor (* unchanged since 4432 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTextureState_ColorKeyColor_1_0_4432,

        XTL::EmuIDirect3DDevice8_SetTextureState_ColorKeyColor,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetTextureState_ColorKeyColor"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_VertexBlend (* unchanged since 4361 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_VertexBlend_1_0_4361,

        XTL::EmuIDirect3DDevice8_SetRenderState_VertexBlend,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_VertexBlend"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullMode
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullMode_1_0_5233,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullMode"
        #endif
    },
    // IDirect3DDevice8::SetRenderState_CullModeB (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_CullModeB_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetRenderState_CullMode,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_CullModeB"
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
    // IDirect3DDevice8::SetRenderState_ZEnable
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
    // IDirect3DDevice8::SetRenderState_LineWidth (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetRenderState_LineWidth_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetRenderState_LineWidth,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetRenderState_LineWidth"
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
    // IDirect3DDevice8::SetTransform (* unchanged since 4134 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetTransform_1_0_4134,

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
    // IDirect3D8::GetDeviceCaps
    {
        (OOVPA*)&IDirect3D8_GetDeviceCaps_1_0_5233,

        XTL::EmuIDirect3D8_GetDeviceCaps,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3D8_GetDeviceCaps"
        #endif
    },
    // IDirect3DDevice8::DrawIndexedVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawIndexedVertices_1_0_5233,

        XTL::EmuIDirect3DDevice8_DrawIndexedVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawIndexedVertices"
        #endif
    },
    // IDirect3DDevice8::SetMaterial (* unchanged since 4627 *)
    {
        (OOVPA*)&IDirect3DDevice8_SetMaterial_1_0_4627,

        XTL::EmuIDirect3DDevice8_SetMaterial,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SetMaterial"
        #endif
    },
    // IDirect3DVertexBuffer8::GetDesc
    {
        (OOVPA*)&IDirect3DVertexBuffer8_GetDesc_1_0_5233,

        XTL::EmuIDirect3DVertexBuffer8_GetDesc,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DVertexBuffer8_GetDesc"
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
    // IDirect3DPalette8::Lock2
    {
        (OOVPA*)&IDirect3DPalette8_Lock2_1_0_5233,

        XTL::EmuIDirect3DPalette8_Lock2,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DPalette8_Lock2"
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
    // IDirect3DDevice8::DrawVertices
    {
        (OOVPA*)&IDirect3DDevice8_DrawVertices_1_0_5233,

        XTL::EmuIDirect3DDevice8_DrawVertices,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DrawVertices"
        #endif
    },
    // IDirect3DDevice8::GetViewportOffsetAndScale (* unchanged since 4627 *)
    {
        (OOVPA*)&X_D3DDevice_GetViewportOffsetAndScale_1_0_4627,

        XTL::EmuIDirect3DDevice8_GetViewportOffsetAndScale,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_GetViewportOffsetAndScale"
        #endif
    },
    // IDirect3DDevice8::DeleteVertexShader (* unchanged since 4627 *)
    {
        (OOVPA*)&X_D3DDevice_DeleteVertexShader_1_0_4627,

        XTL::EmuIDirect3DDevice8_DeleteVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_DeleteVertexShader"
        #endif
    },
    // IDirect3DDevice8::SelectVertexShaderDirect (* unchanged since 4627 *)
    {
        (OOVPA*)&X_D3DDevice_SelectVertexShaderDirect_1_0_4627,

        XTL::EmuIDirect3DDevice8_SelectVertexShaderDirect,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_SelectVertexShaderDirect"
        #endif
    },
    // IDirect3DDevice8::LoadVertexShader
    {
        (OOVPA*)&IDirect3DDevice8_LoadVertexShader_1_0_5233,

        XTL::EmuIDirect3DDevice8_LoadVertexShader,

        #ifdef _DEBUG_TRACE
        "EmuIDirect3DDevice8_LoadVertexShader"
        #endif
    },
};

// ******************************************************************
// * D3D8_1_0_5233_SIZE
// ******************************************************************
uint32 D3D8_1_0_5233_SIZE = sizeof(D3D8_1_0_5233);
