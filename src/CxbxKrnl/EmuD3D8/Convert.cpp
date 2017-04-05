// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->Convert.cpp
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"

enum _FormatStorage {
	Undefined = 0,
	Linear,
	Swizzled,
	Compressed
};

typedef struct _FormatInfo {
	uint8_t bits_per_pixel;
	_FormatStorage stored;
	XTL::D3DFORMAT pc;
	char *warning;
} FormatInfo;

const FormatInfo FormatInfos[] = {
	// X_D3DFMT_L8 = 0x00,
	{ 8, Swizzled, XTL::D3DFMT_L8 },
	// X_D3DFMT_AL8 = 0x01,
	{ 8, Swizzled, XTL::D3DFMT_A4L4, "X_D3DFMT_AL8 -> D3DFMT_A4L4" }, // Was : D3DFMT_A8 // Cxbx NOTE: Hack: Alpha ignored, basically
	// X_D3DFMT_A1R5G5B5 = 0x02,
	{ 16, Swizzled, XTL::D3DFMT_A1R5G5B5 },
	// X_D3DFMT_X1R5G5B5 = 0x03,
	{ 16, Swizzled, XTL::D3DFMT_X1R5G5B5 },
	// X_D3DFMT_A4R4G4B4 = 0x04,
	{ 16, Swizzled, XTL::D3DFMT_A4R4G4B4 },
	// X_D3DFMT_R5G6B5 = 0x05,
	{ 16, Swizzled, XTL::D3DFMT_R5G6B5 },
	// X_D3DFMT_A8R8G8B8 = 0x06,
	{ 32, Swizzled, XTL::D3DFMT_A8R8G8B8 },
	// X_D3DFMT_X8R8G8B8 = 0x07, // Alias : X_D3DFMT_X8L8V8U8
	{ 32, Swizzled, XTL::D3DFMT_X8R8G8B8 },
	// undefined : 0x08,
	{ },
	// undefined : 0x09,
	{ },
	// undefined : 0x0A,
	{ },
	// X_D3DFMT_P8 = 0x0B, // 8-bit Palletized
	{ 8, Swizzled, XTL::D3DFMT_P8 },
	// X_D3DFMT_DXT1 = 0x0C, // opaque/one-bit alpha
	{ 4, Compressed, XTL::D3DFMT_DXT1 }, // NOTE : DXT1 is half byte per pixel, so divide Size and Pitch calculations by two!
	// undefined : 0x0D,
	{ },
	// X_D3DFMT_DXT3 = 0x0E, // linear alpha. Alias : X_D3DFMT_DXT2
	{ 8, Compressed, XTL::D3DFMT_DXT3 },
	// X_D3DFMT_DXT5 = 0x0F, // interpolated alpha. Alias : X_D3DFMT_DXT4
	{ 8, Compressed, XTL::D3DFMT_DXT5 },
	// X_D3DFMT_LIN_A1R5G5B5 = 0x10,
	{ 16, Linear, XTL::D3DFMT_A1R5G5B5 },
	// X_D3DFMT_LIN_R5G6B5 = 0x11,
	{ 16, Linear, XTL::D3DFMT_R5G6B5 },
	// X_D3DFMT_LIN_A8R8G8B8 = 0x12,
	{ 32, Linear, XTL::D3DFMT_A8R8G8B8 },
	// X_D3DFMT_LIN_L8 = 0x13,
	{ 8, Linear, XTL::D3DFMT_L8 },
	// undefined : 0x14,
	{ },
	// undefined : 0x15,
	{ },
	// X_D3DFMT_LIN_R8B8 = 0x16,
	{ 16, Linear, XTL::D3DFMT_R5G6B5, "X_D3DFMT_LIN_R8B8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong :)
	// X_D3DFMT_LIN_G8B8 = 0x17, // Alias : X_D3DFMT_LIN_V8U8
	{ 16, Linear, XTL::D3DFMT_R5G6B5, "X_D3DFMT_LIN_G8B8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong :)
	// undefined : 0x18,
	{ },
	// X_D3DFMT_A8 = 0x19,
	{ 8, Swizzled, XTL::D3DFMT_A8 },
	// X_D3DFMT_A8L8 = 0x1A,
	{ 16, Swizzled, XTL::D3DFMT_R5G6B5, "X_D3DFMT_A8L8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong :)
	// TODO : { 16, Swizzled, XTL::D3DFMT_A8L8 },
	// X_D3DFMT_LIN_AL8 = 0x1B,
	{ 8, Linear, XTL::D3DFMT_A4L4, "X_D3DFMT_LIN_AL8 -> D3DFMT_A4L4" },
	// X_D3DFMT_LIN_X1R5G5B5 = 0x1C,
	{ 16, Linear, XTL::D3DFMT_X1R5G5B5 },
	// X_D3DFMT_LIN_A4R4G4B4 = 0x1D,
	{ 16, Linear, XTL::D3DFMT_A4R4G4B4 },
	// X_D3DFMT_LIN_X8R8G8B8 = 0x1E, // Alias : X_D3DFMT_LIN_X8L8V8U8
	{ 32, Linear, XTL::D3DFMT_X8R8G8B8 },
	// X_D3DFMT_LIN_A8 = 0x1F,
	{ 8, Linear, XTL::D3DFMT_A8 },
	// X_D3DFMT_LIN_A8L8 = 0x20,
	{ 16, Linear, XTL::D3DFMT_A8L8 },
	// undefined : 0x21,
	{ },
	// undefined : 0x22,
	{ },
	// undefined : 0x23,
	{ },
	// X_D3DFMT_YUY2 = 0x24,
	{ 8, Undefined, XTL::D3DFMT_YUY2 },
	// X_D3DFMT_UYVY = 0x25,
	// undefined : 0x26,
	{ },
	{ 8, Undefined, XTL::D3DFMT_UYVY },
	// X_D3DFMT_L6V5U5 = 0x27, // Alias : X_D3DFMT_R6G5B5
	{ 16, Swizzled, XTL::D3DFMT_L6V5U5 }, // XQEMU NOTE: This might be signed
	// X_D3DFMT_V8U8 = 0x28, // Alias : X_D3DFMT_G8B8
	{ 16, Swizzled, XTL::D3DFMT_V8U8 }, // XQEMU NOTE: This might be signed
	// X_D3DFMT_R8B8 = 0x29,
	{ 16, Swizzled, }, // XQEMU NOTE : This might be signed
	// X_D3DFMT_D24S8 = 0x2A,
	{ 32, Swizzled, XTL::D3DFMT_D24S8 },
	// X_D3DFMT_F24S8 = 0x2B,
	{ 32, Swizzled, XTL::D3DFMT_D24S8, "X_D3DFMT_F24S8 -> D3DFMT_D24S8" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F24S8 (Float vs Int)
	// X_D3DFMT_D16 = 0x2C, // Alias : X_D3DFMT_D16_LOCKABLE
	{ 16, Swizzled, XTL::D3DFMT_D16 }, // TODO -oCXBX: XTL::D3DFMT_D16 on Xbox is always lockable - Should we use XTL::D3DFMT_D16_LOCKABLE instead? Needs testcase...
	// X_D3DFMT_F16 = 0x2D,
	{ 16, Swizzled, XTL::D3DFMT_D16, "X_D3DFMT_F16 -> D3DFMT_D16" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F16 (Float vs Int)
	// X_D3DFMT_LIN_D24S8 = 0x2E,
	{ 32, Linear, XTL::D3DFMT_D24S8 },
	// X_D3DFMT_LIN_F24S8 = 0x2F,
	{ 32, Linear, XTL::D3DFMT_D24S8, "X_D3DFMT_LIN_F24S8 -> D3DFMT_D24S8" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F24S8 (Float vs Int)
	// X_D3DFMT_LIN_D16 = 0x30,
	{ 16, Linear, XTL::D3DFMT_D16 }, // TODO -oCXBX: XTL::D3DFMT_D16 on Xbox is always lockable - Should we use XTL::D3DFMT_D16_LOCKABLE instead? Needs testcase...
	// X_D3DFMT_LIN_F16 = 0x31,
	{ 16, Linear, XTL::D3DFMT_D16, "X_D3DFMT_LIN_F16 -> D3DFMT_D16" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F16 (Float vs Int)
	// X_D3DFMT_L16 = 0x32,
	{ 16, Swizzled, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_V16U16 = 0x33,
	{ 32, Swizzled, XTL::D3DFMT_V16U16 },
	// undefined : 0x34,
	{ },
	// X_D3DFMT_LIN_L16 = 0x35,
	{ 16, Linear, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_V16U16 = 0x36,
	{ 32, Linear, XTL::D3DFMT_V16U16 },
	// X_D3DFMT_LIN_L6V5U5 = 0x37, // Alias : X_D3DFMT_LIN_R6G5B5
	{ 16, Linear, XTL::D3DFMT_L6V5U5 }, // Untested...
	// X_D3DFMT_R5G5B5A1 = 0x38,
	{ 16, Swizzled, XTL::D3DFMT_A1R5G5B5, "X_D3DFMT_R5G5B5A1 -> D3DFMT_A1R5G5B5" }, // TODO : Requires conversion
	// X_D3DFMT_R4G4B4A4 = 0x39,
	{ 16, Swizzled, XTL::D3DFMT_A4R4G4B4, "X_D3DFMT_R4G4B4A4 -> D3DFMT_A4R4G4B4" }, // TODO : Requires conversion
	// X_D3DFMT_A8B8G8R8 = 0x3A, // Alias : X_D3DFMT_Q8W8V8U8
	{ 32, Swizzled, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_A8B8G8R8 -> D3DFMT_A8R8G8B8" }, // Cxbx NOTE: HACK: R<->B Swapped!
	// TODO : // X_D3DFMT_Q8W8V8U8 = 0x3A, // Alias : X_D3DFMT_A8B8G8R8
	// TODO : { 32, Swizzled, XTL::D3DFMT_Q8W8V8U8 }, // Untested...
	// X_D3DFMT_B8G8R8A8 = 0x3B,
	{ 32, Swizzled, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_B8G8R8A8 -> D3DFMT_A8R8G8B8" }, // TODO : Requires conversion
	// X_D3DFMT_R8G8B8A8 = 0x3C,
	{ 32, Swizzled, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_R5G5B5A1 = 0x3D,
	{ 16, Linear, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_R4G4B4A4 = 0x3E,
	{ 16, Linear, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_A8B8G8R8 = 0x3F,
	{ 32, Linear, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_LIN_A8B8G8R8 -> D3DFMT_A8R8G8B8" }, // Cxbx NOTE: HACK: R<->B Swapped!
	// X_D3DFMT_LIN_B8G8R8A8 = 0x40,
	{ 32, Linear, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_R8G8B8A8 = 0x41,
	{ 32, Linear, XTL::D3DFMT_UNKNOWN },
/*
	// undefined : 0x42-0x63
	{},
	// X_D3DFMT_VERTEXDATA = 0x64,
	{ 8, Linear, XTL::D3DFMT_VERTEXDATA },
	// Dxbx addition : X_D3DFMT_INDEX16 is not an Xbox format, but used internally
	// X_D3DFMT_INDEX16 = 0x65 
	{ 16, Linear, XTL::D3DFMT_INDEX16 },
*/
};

DWORD XTL::EmuXBFormatBitsPerPixel(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		return FormatInfos[Format].bits_per_pixel;

	return 8;
}

DWORD XTL::EmuXBFormatBytesPerPixel(X_D3DFORMAT Format)
{
	return ((EmuXBFormatBitsPerPixel(Format) + 4) / 8);
}

BOOL XTL::EmuXBFormatIsCompressed(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Compressed);

	return false;
}

BOOL XTL::EmuXBFormatIsLinear(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Linear);

	return false;
}

BOOL XTL::EmuXBFormatIsSwizzled(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		return (FormatInfos[Format].stored == Swizzled);

	return false;
}

XTL::D3DFORMAT XTL::EmuXB2PC_D3DFormat(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8) {
		const FormatInfo info = FormatInfos[Format];
		if (info.warning != nullptr)
			EmuWarning(info.warning);

		return info.pc;
	}

	switch (Format) {
	case X_D3DFMT_VERTEXDATA:
		return D3DFMT_VERTEXDATA;
	case ((X_D3DFORMAT)0xffffffff):
		return D3DFMT_UNKNOWN; // TODO -oCXBX: Not sure if this counts as swizzled or not...
	default:
		CxbxKrnlCleanup("EmuXB2PC_D3DFormat: Unknown Format (0x%.08X)", Format);
	}

	return D3DFMT_UNKNOWN;
}

XTL::X_D3DFORMAT XTL::EmuPC2XB_D3DFormat(D3DFORMAT Format)
{
	X_D3DFORMAT result;
    switch(Format)
    {
	case D3DFMT_YUY2:
		result = X_D3DFMT_YUY2;
		break;
	case D3DFMT_UYVY:
		result = X_D3DFMT_UYVY;
		break;
	case D3DFMT_R5G6B5:
		result = X_D3DFMT_LIN_R5G6B5;
		break; // Linear
			   //      Result := X_D3DFMT_R5G6B5; // Swizzled

	case D3DFMT_D24S8:
		result = X_D3DFMT_D24S8;
		break; // Swizzled

	case D3DFMT_DXT5:
		result = X_D3DFMT_DXT5;
		break; // Compressed

	case D3DFMT_DXT4:
		result = X_D3DFMT_DXT4; // Same as X_D3DFMT_DXT5
		break; // Compressed

	case D3DFMT_DXT3:
		result = X_D3DFMT_DXT3;
		break; // Compressed

	case D3DFMT_DXT2:
		result = X_D3DFMT_DXT2; // Same as X_D3DFMT_DXT3
		break; // Compressed

	case D3DFMT_DXT1:
		result = X_D3DFMT_DXT1;
		break; // Compressed

	case D3DFMT_A1R5G5B5:
		result = X_D3DFMT_LIN_A1R5G5B5;
		break; // Linear

	case D3DFMT_X8R8G8B8:
		result = X_D3DFMT_LIN_X8R8G8B8;
		break; // Linear
			   //      Result := X_D3DFMT_X8R8G8B8; // Swizzled

	case D3DFMT_A8R8G8B8:
		//      Result := X_D3DFMT_LIN_A8R8G8B8; // Linear
		result = X_D3DFMT_A8R8G8B8;
		break;
	case D3DFMT_A4R4G4B4:
		result = X_D3DFMT_LIN_A4R4G4B4;
		break; // Linear
			   //      Result := X_D3DFMT_A4R4G4B4; // Swizzled
	case D3DFMT_X1R5G5B5:	// Linear
		result = X_D3DFMT_LIN_X1R5G5B5;
		break;
	case D3DFMT_A8:
		result = X_D3DFMT_A8;
		break;
	case D3DFMT_L8:
		result = X_D3DFMT_LIN_L8;
		break; // Linear
			   //        Result := X_D3DFMT_L8; // Swizzled

	case D3DFMT_D16: case D3DFMT_D16_LOCKABLE:
		result = X_D3DFMT_D16_LOCKABLE;
		break; // Swizzled

	case D3DFMT_UNKNOWN:
		result = ((X_D3DFORMAT)0xffffffff);
		break;

		// Dxbx additions :

	case D3DFMT_L6V5U5:
		result = X_D3DFMT_L6V5U5;
		break; // Swizzled

	case D3DFMT_V8U8:
		result = X_D3DFMT_V8U8;
		break; // Swizzled

	case D3DFMT_V16U16:
		result = X_D3DFMT_V16U16;
		break; // Swizzled

	case D3DFMT_VERTEXDATA:
		result = X_D3DFMT_VERTEXDATA;
		break;

	default:
		CxbxKrnlCleanup("EmuPC2XB_D3DFormat: Unknown Format (%d)", Format);
    }

    return result;
}

DWORD XTL::EmuXB2PC_D3DLock(DWORD Flags)
{
    DWORD NewFlags = 0;

    // Need to convert the flags, TODO: fix the xbox extensions
//    if(Flags & X_D3DLOCK_NOFLUSH)
//        NewFlags ^= 0;

	if(Flags & X_D3DLOCK_NOOVERWRITE)
        NewFlags |= D3DLOCK_NOOVERWRITE;

//	if(Flags & X_D3DLOCK_TILED)
//        NewFlags ^= 0;

	if(Flags & X_D3DLOCK_READONLY)
        NewFlags |= D3DLOCK_READONLY;

    return NewFlags;
}

// convert from xbox to pc multisample formats
XTL::D3DMULTISAMPLE_TYPE XTL::EmuXB2PC_D3DMultiSampleFormat(DWORD Type)
{
	D3DMULTISAMPLE_TYPE result;
	switch (Type & 0xFFFF)
	{
	case X_D3DMULTISAMPLE_NONE:
		result = D3DMULTISAMPLE_NONE;
		break;
	case X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR: 
	case X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX: 
	case X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR: 
	case X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
		result = D3DMULTISAMPLE_2_SAMPLES;
		break;
	case X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR: 
	case X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN: 
	case X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR: 
	case X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
		result = D3DMULTISAMPLE_4_SAMPLES;
		break;
	case X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN: 
	case X_D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
		result = D3DMULTISAMPLE_9_SAMPLES;
		break;
	default:
		EmuWarning("Unknown Multisample Type (0x%X)!\x0d\x0a. If this value is greater than 0xFFFF contact blueshogun!", Type);
		result = D3DMULTISAMPLE_NONE;
	}
	return result;
}

// lookup table for converting vertex count to primitive count
UINT XTL::EmuD3DVertexToPrimitive[11][2] =
{
    {0, 0}, // NULL
    {1, 0}, // X_D3DPT_POINTLIST
    {2, 0}, // X_D3DPT_LINELIST
    {1, 1}, // X_D3DPT_LINELOOP
    {1, 1}, // X_D3DPT_LINESTRIP
    {3, 0}, // X_D3DPT_TRIANGLELIST
    {1, 2}, // X_D3DPT_TRIANGLESTRIP
    {1, 2}, // X_D3DPT_TRIANGLEFAN
    {4, 0}, // X_D3DPT_QUADLIST
    {2, 2}, // X_D3DPT_QUADSTRIP
    {1, 0}, // X_D3DPT_POLYGON
};

// conversion table for xbox->pc primitive types
XTL::D3DPRIMITIVETYPE XTL::EmuPrimitiveTypeLookup[] =
{
    /* NULL                 = 0         */ (XTL::D3DPRIMITIVETYPE)0,
    /* D3DPT_POINTLIST      = 1,        */ XTL::D3DPT_POINTLIST,
    /* D3DPT_LINELIST       = 2,        */ XTL::D3DPT_LINELIST,
    /* D3DPT_LINELOOP       = 3,  Xbox  */ XTL::D3DPT_LINESTRIP,
    /* D3DPT_LINESTRIP      = 4,        */ XTL::D3DPT_LINESTRIP,
    /* D3DPT_TRIANGLELIST   = 5,        */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_TRIANGLESTRIP  = 6,        */ XTL::D3DPT_TRIANGLESTRIP,
    /* D3DPT_TRIANGLEFAN    = 7,        */ XTL::D3DPT_TRIANGLEFAN,
    /* D3DPT_QUADLIST       = 8,  Xbox  */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_QUADSTRIP      = 9,  Xbox  */ XTL::D3DPT_TRIANGLESTRIP,
    /* D3DPT_POLYGON        = 10, Xbox  */ XTL::D3DPT_TRIANGLEFAN,
    /* D3DPT_MAX            = 11,       */ (XTL::D3DPRIMITIVETYPE)11
};

// render state conversion table
CONST DWORD XTL::EmuD3DRenderStateSimpleEncoded[174] =
{
    // WARNING: This lookup table strongly binds us to an SDK with these
    // specific #define values for D3DRS_*. Make VERY sure that you have
    // the correct lookup values;
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 0
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 2
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 4
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 6
    X_D3DRSSE_UNK,  0x0004037c,     // 8  - , D3DRS_SHADEMODE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 10
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 12
    0x0004035c,     0x00040300,     // 14 - D3DRS_ZWRITEENABLE, D3DRS_ALPHATESTENABLE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 16
    X_D3DRSSE_UNK,  0x00040344,     // 18 - , D3DRS_SRCBLEND
    0x00040348,     X_D3DRSSE_UNK,  // 20 - D3DRS_DESTBLEND
    X_D3DRSSE_UNK,  0x00040354,     // 22 - , D3DRS_ZFUNC
    0x00040340,     0x0004033c,     // 24 - D3DRS_ALPHAREF, D3DRS_ALPHAFUNC
    0x00040310,     0x00040304,     // 26 - D3DRS_DITHERENABLE, D3DRS_ALPHABLENDENABLE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 28
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 30
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 32
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 34
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 36
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 38
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 40
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 42
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 44
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 46
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 48
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 50
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 52
    0x00040374,     0x00040378,     // 54 - D3DRS_STENCILZFAIL, D3DRS_STENCILPASS
    0x00040364,     0x00040368,     // 56 - D3DRS_STENCILFUNC, D3DRS_STENCILREF
    0x0004036c,     0x00040360,		// 58 - D3DRS_STENCILMASK, D3DRS_STENCILWRITEMASK
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 60
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 62
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 64
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 66
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 68
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 70
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 72
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 74
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 76
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 78
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 80
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 82
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 84
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 86
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 88
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 90
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 92
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 94
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 96
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 98
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 100
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 102
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 104
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 106
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 108
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 110
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 112
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 114
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 116
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 118
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 120
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 122
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 124
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 126
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 128
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 130
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 132
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 134
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 136
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 138
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 140
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 142
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 144
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 146
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 148
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 150
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 152
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 154
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 156
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 158
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 160
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 162
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 164
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 166
    0x00040358,     X_D3DRSSE_UNK,  // 168 - D3DRS_COLORWRITEENABLE
    X_D3DRSSE_UNK,  0x00040350,     // 170
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 172
};

void XTL::EmuUnswizzleRect
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
) // Source : Dxbx
{
	// TODO : The following could be done using a lookup table :
	DWORD dwMaskX = 0, dwMaskY = 0, dwMaskZ = 0;
	for (uint i=1, j=1; (i <= dwWidth) || (i <= dwHeight) || (i <= dwDepth); i <<= 1) {
		if (i < dwWidth) {
			dwMaskX = dwMaskX | j;
			j <<= 1;
		};

		if (i < dwHeight) {
			dwMaskY = dwMaskY | j;
			j <<= 1;
		}

		if (i < dwDepth) {
			dwMaskZ = dwMaskZ | j;
			j <<= 1;
		}
	}

	// get the biggest mask
	DWORD dwMaskMax;
	if (dwMaskX > dwMaskY)
		dwMaskMax = dwMaskX;
	else
		dwMaskMax = dwMaskY;

	if (dwMaskZ > dwMaskMax)
		dwMaskMax = dwMaskZ;

	DWORD dwStartX = 0, dwOffsetX = 0;
	DWORD dwStartY = 0, dwOffsetY = 0;
	DWORD dwStartZ = 0, dwOffsetW = 0;
	/* TODO : Use values from poDst and rSrc to initialize above values, after which the following makes more sense:
	for (uint i=1; i <= dwMaskMax; i <<= 1) {
		if (i <= dwMaskX) {
			if (dwMaskX & i)
				dwStartX |= (dwOffsetX & i);
			else
				dwOffsetX <<= 1;
		}

		if (i <= dwMaskY) {
			if (dwMaskY & i)
				dwStartY |= dwOffsetY & i;
			else
				dwOffsetY <<= 1;
		}

		if (i <= dwMaskZ) {
			if (dwMaskZ & i)
				dwStartZ |= dwOffsetZ & i;
			else
				dwOffsetZ <<= 1;
		}
	}*/

	DWORD dwZ = dwStartZ;
	for (uint z = 0; z < dwDepth; z++) {
		DWORD dwY = dwStartY;
		for (uint y = 0; y < dwHeight; y++) {
			DWORD dwX = dwStartX;
			for (uint x = 0; x < dwWidth; x++) {
				int delta = ((dwX | dwY | dwZ) * dwBPP);
				memcpy(pDstBuff, (PBYTE)pSrcBuff + delta, dwBPP); // copy one pixel
				pDstBuff = (PBYTE)pDstBuff + dwBPP; // Step to next pixel in destination
				dwX = (dwX - dwMaskX) & dwMaskX; // step to next pixel in source
			}

			pDstBuff = (PBYTE)pDstBuff + dwPitch - (dwWidth * dwBPP); // step to next line in destination
			dwY = (dwY - dwMaskY) & dwMaskY; // step to next line in source
		}

		// TODO : How to step to next level in destination? Should X and Y be recalculated per level?
		dwZ = (dwZ - dwMaskZ) & dwMaskZ; // step to next level in source
	}
} // EmuUnswizzleRect NOPATCH
