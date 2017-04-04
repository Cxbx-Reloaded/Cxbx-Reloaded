// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->Convert.h
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
#ifndef CONVERT_H
#define CONVERT_H

#include "CxbxKrnl.h"


// simple render state encoding lookup table
#define X_D3DRSSE_UNK 0x7fffffff
extern CONST DWORD EmuD3DRenderStateSimpleEncoded[174];

typedef struct _ComponentEncodingInfo
{
	uint8_t ABits, RBits, GBits, BBits;
	uint8_t AShift, RShift, GShift, BShift;
} ComponentEncodingInfo;

extern const ComponentEncodingInfo *EmuXBFormatComponentEncodingInfo(X_D3DFORMAT Format);

extern D3DCOLOR DecodeUInt32ToColor(const ComponentEncodingInfo * encoding, const uint32 value);

// how many bits does this format use per pixel?
extern DWORD EmuXBFormatBitsPerPixel(X_D3DFORMAT Format);

// how many bytes does this format use per pixel?
extern DWORD EmuXBFormatBytesPerPixel(X_D3DFORMAT Format);

// is this format compressed?
extern BOOL EmuXBFormatIsCompressed(X_D3DFORMAT Format);

// is this format linear?
extern BOOL EmuXBFormatIsLinear(X_D3DFORMAT Format);

// is this format swizzled?
extern BOOL EmuXBFormatIsSwizzled(X_D3DFORMAT Format);

// convert from xbox to pc color formats
extern D3DFORMAT EmuXB2PC_D3DFormat(X_D3DFORMAT Format);

// convert from pc to xbox color formats
extern X_D3DFORMAT EmuPC2XB_D3DFormat(D3DFORMAT Format);

// convert from xbox to pc d3d lock flags
extern DWORD EmuXB2PC_D3DLock(DWORD Flags);

// convert from xbox to pc multisample formats
extern D3DMULTISAMPLE_TYPE EmuXB2PC_D3DMultiSampleFormat(DWORD Type);

/**
// convert from pc to xbox texture transform state types (unnecessary so far)
if((uint32)State < 4)
    State = (D3DTRANSFORMSTATETYPE)(State - 2);
else if((uint32)State < 20)
    State = (D3DTRANSFORMSTATETYPE)(State - 14);
else if((uint32)State > 255)
    State = (D3DTRANSFORMSTATETYPE)(State - 250);
else
    CxbxKrnlCleanup("Unknown Transform State Type (%d)", State);
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
    else if((uint32)State == 10) // Max
        return (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE7 + 1);

    CxbxKrnlCleanup("Unknown Transform State Type (%d)", State);

    return State;
}

// convert from xbox to pc blend ops
inline D3DBLENDOP EmuXB2PC_D3DBLENDOP(X_D3DBLENDOP Value)
{
    switch(Value)
    {
        case 0x8006:
            return D3DBLENDOP_ADD;
		case 0x800a:
			return D3DBLENDOP_SUBTRACT;
		case 0x800b:
			return D3DBLENDOP_REVSUBTRACT;
		case 0x8007:
			return D3DBLENDOP_MIN;
		case 0x8008:
			return D3DBLENDOP_MAX;
		case 0xF006:
			{
				CxbxKrnlCleanup("D3DBLENDOP_ADDSIGNED is not supported!");
				return D3DBLENDOP_ADD;
			};
		case 0xF005:
			{
				CxbxKrnlCleanup("D3DBLENDOP_REVSUBTRACTSIGNED is not supported!");
				return D3DBLENDOP_REVSUBTRACT;
			}
    }

    CxbxKrnlCleanup("Unknown D3DBLENDOP (0x%.08X)", Value);

    return (D3DBLENDOP)Value;
}

// convert from xbox to pc blend types 
inline D3DBLEND EmuXB2PC_D3DBLEND(X_D3DBLEND Value)
{
    if(Value < 2)
        return (D3DBLEND)(Value + 1);
    else if(Value < 0x309)
        return (D3DBLEND)((Value & 0xF) + 3);

    EmuWarning("Unknown Xbox D3DBLEND Extension (0x%.08X)", Value);
	return D3DBLEND_ONE;
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

// convert from xbox to pc shade modes
inline D3DSHADEMODE EmuXB2PC_D3DSHADEMODE(X_D3DSHADEMODE Value)
{
    return (D3DSHADEMODE)((Value & 0x3) + 1);
}

// convert from xbox to pc stencilop modes
inline D3DSTENCILOP EmuXB2PC_D3DSTENCILOP(X_D3DSTENCILOP Value)
{
	switch(Value)
	{
	case 0x1e00:
		return D3DSTENCILOP_KEEP;
	case 0:
		return D3DSTENCILOP_ZERO;
	case 0x1e01:
		return D3DSTENCILOP_REPLACE;
	case 0x1e02:
		return D3DSTENCILOP_INCRSAT;
	case 0x1e03:
		return D3DSTENCILOP_DECRSAT;
	case 0x150a:
		return D3DSTENCILOP_INVERT;
	case 0x8507:
		return D3DSTENCILOP_INCR;
	case 0x8508:
		return D3DSTENCILOP_DECR;

	default:
		CxbxKrnlCleanup("Unknown D3DSTENCILOP (0x%.08X)", Value);
	}

	return (D3DSTENCILOP) Value;
}

// table used for vertex->primitive count conversion
extern UINT EmuD3DVertexToPrimitive[11][2];

// convert from vertex count to primitive count (Xbox)
inline int EmuD3DVertex2PrimitiveCount(X_D3DPRIMITIVETYPE PrimitiveType, int VertexCount)
{
    return (VertexCount - EmuD3DVertexToPrimitive[PrimitiveType][1]) / EmuD3DVertexToPrimitive[PrimitiveType][0];
}

// convert from primitive count to vertex count (Xbox)
inline int EmuD3DPrimitive2VertexCount(X_D3DPRIMITIVETYPE PrimitiveType, int PrimitiveCount)
{
    return (PrimitiveCount * EmuD3DVertexToPrimitive[PrimitiveType][0]) + EmuD3DVertexToPrimitive[PrimitiveType][1];
}

// conversion table for xbox->pc primitive types
extern D3DPRIMITIVETYPE EmuPrimitiveTypeLookup[];

// convert xbox->pc primitive type
inline D3DPRIMITIVETYPE EmuXB2PC_D3DPrimitiveType(X_D3DPRIMITIVETYPE PrimitiveType)
{
    if((DWORD)PrimitiveType == 0x7FFFFFFF)
        return D3DPT_FORCE_DWORD;

    return EmuPrimitiveTypeLookup[PrimitiveType];
}

extern void EmuUnswizzleRect
(
	PVOID pSrcBuff,
	DWORD dwWidth,
	DWORD dwHeight,
	DWORD dwDepth,
	PVOID pDstBuff,
	DWORD dwPitch,
	RECT rSrc, // Unused
	POINT poDst, // Unused
	DWORD dwBPP // expressed in Bytes Per Pixel
); // NOPATCH

#endif