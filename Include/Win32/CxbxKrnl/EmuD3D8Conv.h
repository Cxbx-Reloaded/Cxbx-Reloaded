// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8Conv.h
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
#ifndef EMUD3D8CONV_H
#define EMUD3D8CONV_H

// fixup xbox extensions to be compatible with PC direct3d
extern UINT EmuFixupVerticesA
(
    DWORD                           PrimitiveType,
    UINT                           &PrimitiveCount,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8,
    UINT                            dwOffset,
    PVOID                           pVertexStreamZeroData,
    UINT                            uiVertexStreamZeroStride, 
    PVOID                          *ppNewVertexStreamZeroData
);

// fixup xbox extensions to be compatible with PC direct3d
extern VOID EmuFixupVerticesB
(
    UINT                            nStride,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8
);

// table used to vertex->primitive count conversion
extern UINT EmuD3DVertexToPrimitive[11][2];

// conversion table for xbox->pc primitive types
extern D3DPRIMITIVETYPE EmuPrimitiveTypeLookup[];

// simple render state encoding lookup table
#define X_D3DRSSE_UNK 0x7fffffff
extern CONST DWORD EmuD3DRenderStateSimpleEncoded[174];

// convert from xbox to pc color formats
inline D3DFORMAT EmuXB2PC_D3DFormat(X_D3DFORMAT Format)
{
    switch(Format)
    {
        case 0x00: // Swizzled   (X_D3DFMT_L8)
        case 0x01: // Swizzled   (X_D3DFMT_AL8) // NOTE: Hack: Alpha ignored, basically
            return D3DFMT_L8;

        case 0x02: // Swizzled   (X_D3DFMT_A1R5G5B5)
            return D3DFMT_A1R5G5B5;

        case 0x1A: // Swizzled   (X_D3DFMT_A8L8)
            return D3DFMT_R5G6B5;   // NOTE: HACK: Totally and utterly wrong :)

        case 0x1D: // Linear     (X_D3DFMT_LIN_A4R4G4B4)
        case 0x04: // Swizzled   (X_D3DFMT_A4R4G4B4)
            return D3DFMT_A4R4G4B4;

        case 0x11: // Linear     (X_D3DFMT_LIN_R5G6B5)
        case 0x05: // Swizzled   (X_D3DFMT_R5G6B5)
            return D3DFMT_R5G6B5;

        case 0x12: // Linear     (X_D3DFMT_LIN_A8R8G8B8)
        case 0x06: // Swizzled   (X_D3DFMT_A8R8G8B8)
            return D3DFMT_A8R8G8B8;

        case 0x3F: // Linear     (X_D3DFMT_LIN_A8B8G8R8)
            return D3DFMT_A8R8G8B8; // NOTE: HACK: R<->B Swapped!

        case 0x1E: // Linear     (X_D3DFMT_LIN_X8R8G8B8)
        case 0x07: // Swizzled   (X_D3DFMT_X8R8G8B8)
            return D3DFMT_X8R8G8B8;

        case 0x0B: // Swizzled   (X_D3DFMT_P8)
            return D3DFMT_P8;

        case 0x0C: // Compressed (X_D3DFMT_DXT1)
            return D3DFMT_DXT1;

        case 0x0E: // Compressed (X_D3DFMT_DXT2)
            return D3DFMT_DXT2;

        case 0x0F: // Compressed (X_D3DFMT_DXT3)
            return D3DFMT_DXT3;

        case 0x24: // Swizzled   (X_D3DFMT_YUV2)
            return D3DFMT_YUY2;

        case 0x2E: // Linear     (X_D3DFMT_LIN_D24S8)
        case 0x2A: // Swizzled   (X_D3DFMT_D24S8)
            return D3DFMT_D24S8;

        case 0x30: // Linear     (X_D3DFMT_LIN_D16)
        case 0x2C: // Swizzled   (X_D3DFMT_D16)
            return D3DFMT_D16;

        case 0x28: // Swizzled   (X_D3DFMT_V8U8)
            return D3DFMT_V8U8;
    }

    EmuCleanup("EmuXB2PC_D3DFormat: Unknown Format (0x%.08X)", Format);

    return (D3DFORMAT)Format;
}

// convert from pc to xbox color formats
inline X_D3DFORMAT EmuPC2XB_D3DFormat(D3DFORMAT Format)
{
    switch(Format)
    {
        case D3DFMT_YUY2:
            return 0x24;
        case D3DFMT_R5G6B5:
            return 0x05;
        case D3DFMT_D24S8:
            return 0x2A;
        case D3DFMT_X8R8G8B8:
//            return 0x1E;    // Linear (X_D3DFMT_LIN_X8R8G8B8)
            return 0x07;
        case D3DFMT_A8R8G8B8:
//            return 0x12;    // Linear (X_D3DFMT_LIN_A8R8G8B8)
            return 0x06;
    }

    EmuCleanup("EmuPC2XB_D3DFormat: Unknown Format (%d)", Format);

    return Format;
}

/**
// convert from pc to xbox texture transform state types (unnecessary so far)
if((uint32)State < 4)
    State = (D3DTRANSFORMSTATETYPE)(State - 2);
else if((uint32)State < 20)
    State = (D3DTRANSFORMSTATETYPE)(State - 14);
else if((uint32)State > 255)
    State = (D3DTRANSFORMSTATETYPE)(State - 250);
else
    EmuCleanup("Unknown Transform State Type (%d)", State);
//*/

// convert from xbox to pc texture transform state types
inline D3DTRANSFORMSTATETYPE EmuXB2PC_D3DTS(D3DTRANSFORMSTATETYPE State)
{
    if((uint32)State < 2)
        return (D3DTRANSFORMSTATETYPE)(State + 2);
    else if((uint32)State < 6)
        return (D3DTRANSFORMSTATETYPE)(State + 14);
    else if((uint32)State < 10)
        return D3DTS_WORLDMATRIX(State-6);

    EmuCleanup("Unknown Transform State Type (%d)", State);

    return State;
}

// convert from xbox to pc blend ops
inline D3DBLENDOP EmuXB2PC_D3DBLENDOP(X_D3DBLENDOP Value)
{
    switch(Value)
    {
        case 0x8006:
            return D3DBLENDOP_ADD;
    }

    EmuCleanup("Unknown D3DBLENDOP (0x%.08X)", Value);

    return (D3DBLENDOP)Value;
}

// convert from xbox to pc blend types
inline D3DBLEND EmuXB2PC_D3DBLEND(X_D3DBLEND Value)
{
    if(Value < 2)
        return (D3DBLEND)(Value + 1);
    else if(Value < 0x309)
        return (D3DBLEND)((Value & 0xF) + 3);

    EmuCleanup("Unknown Xbox D3DBLEND Extension (0x%.08X)", Value);

    return (D3DBLEND)Value;
}

// convert from xbox to pc comparison functions
inline D3DCMPFUNC EmuXB2PC_D3DCMPFUNC(X_D3DCMPFUNC Value)
{
    return (D3DCMPFUNC)((Value & 0xF) + 1);
}

// convert from xbox to pc fill modes
inline D3DFILLMODE EmuXB2PC_D3DFILLMODE(X_D3DFILLMODE Value)
{
    return (D3DFILLMODE)((Value & 0xF) + 1);
}

// convert from vertex count to primitive count (Xbox)
inline int EmuD3DVertex2PrimitiveCount(int PrimitiveType, int VertexCount)
{
    return (VertexCount - EmuD3DVertexToPrimitive[PrimitiveType][1]) / EmuD3DVertexToPrimitive[PrimitiveType][0];
}

// convert from primitive count to vertex count (Xbox)
inline int EmuD3DPrimitive2VertexCount(int PrimitiveType, int PrimitiveCount)
{
    return (((PrimitiveCount)*EmuD3DVertexToPrimitive[PrimitiveType][0])+EmuD3DVertexToPrimitive[PrimitiveType][1]);
}

// convert from xbox to d3d primitive type
inline D3DPRIMITIVETYPE EmuPrimitiveType(X_D3DPRIMITIVETYPE PrimitiveType)
{
    if((DWORD)PrimitiveType == 0x7FFFFFFF)
        return D3DPT_FORCE_DWORD;

    return EmuPrimitiveTypeLookup[PrimitiveType];
}

#endif