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

// About format color components:
// A = alpha, byte : 0 = fully opaque, 255 = fully transparent
// X = ignore these component bits
// R = red
// G = green
// B = blue
// L = luminance, byte : 0 = pure black ARGB(1, 0,0,0) to 255 = pure white ARGB(1,255,255,255)
// P = pallete
enum _ComponentEncoding {
	NoComponents = 0, // Format doesn't contain any component (ARGB/QWVU)
	A1R5G5B5,
	X1R5G5B5,
	A4R4G4B4,
	__R5G6B5, // NOTE : A=255
	A8R8G8B8,
	X8R8G8B8,
	____R8B8, // NOTE : A takes R, G takes B
	____G8B8, // NOTE : A takes G, R takes B
	______A8,
	__R6G5B5,
	R5G5B5A1,
	R4G4B4A4,
	A8B8G8R8,
	B8G8R8A8,
	R8G8B8A8,
	______L8, // NOTE : A=255, R=G=B= L
	_____AL8, // NOTE : A=R=G=B= L
	_____L16, // NOTE : Actually G8B8, with A=R=255
	____A8L8, // NOTE : R=G=B= L
};

static const XTL::ComponentEncodingInfo ComponentEncodingInfos[] = {
	{ }, // NoComponents
	// AB  RB  GB  BB ASh RSh GSh BSh
	//its its its its ift ift ift ift
	{  1,  5,  5,  5, 15, 10,  5,  0 }, // A1R5G5B5
	{  0,  5,  5,  5,  0, 10,  5,  0 }, // X1R5G5B5
	{  4,  4,  4,  4, 12,  8,  4,  0 }, // A4R4G4B4
	{  0,  5,  6,  5, -1, 11,  6,  0 }, // __R5G6B5 // Shift=-1 turns A into 255
	{  8,  8,  8,  8, 24, 16,  8,  0 }, // A8R8G8B8
	{  0,  8,  8,  8,  0, 16,  8,  0 }, // X8R8G8B8
	{  8,  8,  8,  8,  8,  8,  0,  0 }, // ____R8B8 // A takes R, G takes B
	{  8,  8,  8,  8,  8,  0,  8,  0 }, // ____G8B8 // A takes G, R takes B
	{  8,  0,  0,  0,  0,  0,  0,  0 }, // ______A8
	{  0,  6,  5,  5,  0, 10,  5,  0 }, // __R6G5B5
	{  1,  5,  5,  5,  0, 11,  6,  1 }, // R5G5B5A1
	{  4,  4,  4,  4,  0, 12,  8,  4 }, // R4G4B4A4
	{  8,  8,  8,  8, 24,  0,  8, 16 }, // A8B8G8R8
	{  8,  8,  8,  8,  0,  8, 16, 24 }, // B8G8R8A8
	{  8,  8,  8,  8,  0, 24, 16,  8 }, // R8G8B8A8	
	{  8,  8,  8,  8, -1,  0,  0,  0 }, // ______L8	// Shift=-1 turns A into 255
	{  8,  8,  8,  8,  0,  0,  0,  0 }, // _____AL8	// A,R,G,B take L
	{  8,  8,  8,  8, -1, -1,  8,  0 }, // _____L16	// Shift=-1 turns A,R into 255
	{  8,  8,  8,  8,  8,  0,  0,  0 }, // ____A8L8	// R,G,B take L
    // Note : For formats that copy one components into another, the above bit-
	// counts per component won't sum up to these format's byte-count per pixel!
};

enum _FormatStorage {
	Undefined = 0,
	Linear,
	Swizzled,
	Compressed
};

typedef struct _FormatInfo {
	uint8_t bits_per_pixel;
	_FormatStorage stored;
	_ComponentEncoding components;
	XTL::D3DFORMAT pc;
	char *warning;
} FormatInfo;

static const FormatInfo FormatInfos[] = {
	// X_D3DFMT_L8 = 0x00,
	{ 8, Swizzled, ______L8, XTL::D3DFMT_L8 },
	// X_D3DFMT_AL8 = 0x01,
	{ 8, Swizzled, _____AL8, XTL::D3DFMT_L8, "X_D3DFMT_AL8 -> D3DFMT_L8" }, // Cxbx NOTE: Hack: Alpha ignored, basically // TODO : Requires conversion
	// X_D3DFMT_A1R5G5B5 = 0x02,
	{ 16, Swizzled, A1R5G5B5, XTL::D3DFMT_A1R5G5B5 },
	// X_D3DFMT_X1R5G5B5 = 0x03,
	{ 16, Swizzled, X1R5G5B5, XTL::D3DFMT_X1R5G5B5 },
	// X_D3DFMT_A4R4G4B4 = 0x04,
	{ 16, Swizzled, A4R4G4B4, XTL::D3DFMT_A4R4G4B4 },
	// X_D3DFMT_R5G6B5 = 0x05,
	{ 16, Swizzled, __R5G6B5, XTL::D3DFMT_R5G6B5 },
	// X_D3DFMT_A8R8G8B8 = 0x06,
	{ 32, Swizzled, A8R8G8B8, XTL::D3DFMT_A8R8G8B8 },
	// X_D3DFMT_X8R8G8B8 = 0x07, // Alias : X_D3DFMT_X8L8V8U8
	{ 32, Swizzled, X8R8G8B8, XTL::D3DFMT_X8R8G8B8 },
	// undefined : 0x08,
	{ },
	// undefined : 0x09,
	{ },
	// undefined : 0x0A,
	{ },
	// X_D3DFMT_P8 = 0x0B, // 8-bit Palletized
	{ 8, Swizzled, NoComponents, XTL::D3DFMT_P8 },
	// X_D3DFMT_DXT1 = 0x0C, // opaque/one-bit alpha
	{ 4, Compressed, NoComponents, XTL::D3DFMT_DXT1 }, // NOTE : DXT1 is half byte per pixel, so divide Size and Pitch calculations by two!
	// undefined : 0x0D,
	{ },
	// X_D3DFMT_DXT3 = 0x0E, // linear alpha. Alias : X_D3DFMT_DXT2
	{ 8, Compressed, NoComponents, XTL::D3DFMT_DXT3 },
	// X_D3DFMT_DXT5 = 0x0F, // interpolated alpha. Alias : X_D3DFMT_DXT4
	{ 8, Compressed, NoComponents, XTL::D3DFMT_DXT5 },
	// X_D3DFMT_LIN_A1R5G5B5 = 0x10,
	{ 16, Linear, A1R5G5B5, XTL::D3DFMT_A1R5G5B5 },
	// X_D3DFMT_LIN_R5G6B5 = 0x11,
	{ 16, Linear, __R5G6B5, XTL::D3DFMT_R5G6B5 },
	// X_D3DFMT_LIN_A8R8G8B8 = 0x12,
	{ 32, Linear, A8R8G8B8, XTL::D3DFMT_A8R8G8B8 },
	// X_D3DFMT_LIN_L8 = 0x13,
	{ 8, Linear, ______L8, XTL::D3DFMT_L8 },
	// undefined : 0x14,
	{ },
	// undefined : 0x15,
	{ },
	// X_D3DFMT_LIN_R8B8 = 0x16,
	{ 16, Linear, ____R8B8, XTL::D3DFMT_R5G6B5, "X_D3DFMT_LIN_R8B8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong :) // TODO : Requires conversion
	// X_D3DFMT_LIN_G8B8 = 0x17, // Alias : X_D3DFMT_LIN_V8U8
	{ 16, Linear, ____G8B8, XTL::D3DFMT_R5G6B5, "X_D3DFMT_LIN_G8B8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong :)
	// undefined : 0x18,
	{ },
	// X_D3DFMT_A8 = 0x19,
	{ 8, Swizzled, ______A8, XTL::D3DFMT_A8 },
	// X_D3DFMT_A8L8 = 0x1A,
	{ 16, Swizzled, ____A8L8, XTL::D3DFMT_A8L8 },
	// X_D3DFMT_LIN_AL8 = 0x1B,
	{ 8, Linear, _____AL8, XTL::D3DFMT_L8, "X_D3DFMT_LIN_AL8 -> D3DFMT_L8" }, // Cxbx NOTE: Hack: Alpha ignored, basically // TODO : Requires conversion
	// X_D3DFMT_LIN_X1R5G5B5 = 0x1C,
	{ 16, Linear, X1R5G5B5, XTL::D3DFMT_X1R5G5B5 },
	// X_D3DFMT_LIN_A4R4G4B4 = 0x1D,
	{ 16, Linear, A4R4G4B4, XTL::D3DFMT_A4R4G4B4 },
	// X_D3DFMT_LIN_X8R8G8B8 = 0x1E, // Alias : X_D3DFMT_LIN_X8L8V8U8
	{ 32, Linear, X8R8G8B8, XTL::D3DFMT_X8R8G8B8 },
	// X_D3DFMT_LIN_A8 = 0x1F,
	{ 8, Linear, ______A8, XTL::D3DFMT_L8 },
	// TODO : { 8, Linear, ______A8, XTL::D3DFMT_A8 },
	// X_D3DFMT_LIN_A8L8 = 0x20,
	{ 16, Linear, ____A8L8, XTL::D3DFMT_A8L8 },
	// undefined : 0x21,
	{ },
	// undefined : 0x22,
	{ },
	// undefined : 0x23,
	{ },
	// X_D3DFMT_YUY2 = 0x24,
	{ 16, Undefined, NoComponents, XTL::D3DFMT_YUY2 },
	// X_D3DFMT_UYVY = 0x25,
	{ 16, Undefined, NoComponents, XTL::D3DFMT_UYVY },
	// undefined : 0x26,
	{ },
	// X_D3DFMT_L6V5U5 = 0x27, // Alias : X_D3DFMT_R6G5B5
	{ 16, Swizzled, __R6G5B5, XTL::D3DFMT_L6V5U5 }, // XQEMU NOTE: This might be signed
	// X_D3DFMT_V8U8 = 0x28, // Alias : X_D3DFMT_G8B8
	{ 16, Swizzled, ____G8B8, XTL::D3DFMT_V8U8 }, // XQEMU NOTE: This might be signed
	// X_D3DFMT_R8B8 = 0x29,
	{ 16, Swizzled, ____R8B8, XTL::D3DFMT_UNKNOWN },
	// TODO : { 16, Swizzled, ____R8B8, XTL::D3DFMT_R5G6B5, "X_D3DFMT_R8B8 -> D3DFMT_R5G6B5" }, // Cxbx NOTE: HACK: Totally and utterly wrong, // XQEMU NOTE : This might be signed
	// X_D3DFMT_D24S8 = 0x2A,
	{ 32, Swizzled, NoComponents, XTL::D3DFMT_D24S8 },
	// X_D3DFMT_F24S8 = 0x2B,
	{ 32, Swizzled, NoComponents, XTL::D3DFMT_D24S8, "X_D3DFMT_F24S8 -> D3DFMT_D24S8" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F24S8 (Float vs Int)
	// X_D3DFMT_D16 = 0x2C, // Alias : X_D3DFMT_D16_LOCKABLE
	{ 16, Swizzled, NoComponents, XTL::D3DFMT_D16 }, // TODO -oCXBX: XTL::D3DFMT_D16 on Xbox is always lockable - Should we use XTL::D3DFMT_D16_LOCKABLE instead? Needs testcase...
	// X_D3DFMT_F16 = 0x2D,
	{ 16, Swizzled, NoComponents, XTL::D3DFMT_D16, "X_D3DFMT_F16 -> D3DFMT_D16" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F16 (Float vs Int)
	// X_D3DFMT_LIN_D24S8 = 0x2E,
	{ 32, Linear, NoComponents, XTL::D3DFMT_D24S8 },
	// X_D3DFMT_LIN_F24S8 = 0x2F,
	{ 32, Linear, NoComponents, XTL::D3DFMT_D24S8, "X_D3DFMT_LIN_F24S8 -> D3DFMT_D24S8" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F24S8 (Float vs Int)
	// X_D3DFMT_LIN_D16 = 0x30,
	{ 16, Linear, NoComponents, XTL::D3DFMT_D16 }, // TODO -oCXBX: XTL::D3DFMT_D16 on Xbox is always lockable - Should we use XTL::D3DFMT_D16_LOCKABLE instead? Needs testcase...
	// X_D3DFMT_LIN_F16 = 0x31,
	{ 16, Linear, NoComponents, XTL::D3DFMT_D16, "X_D3DFMT_LIN_F16 -> D3DFMT_D16" }, // NOTE: Hack!! PC does not have XTL::D3DFMT_F16 (Float vs Int)
	// X_D3DFMT_L16 = 0x32,
	{ 16, Swizzled, _____L16, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_V16U16 = 0x33,
	{ 32, Swizzled, NoComponents, XTL::D3DFMT_V16U16 },
	// undefined : 0x34,
	{ },
	// X_D3DFMT_LIN_L16 = 0x35,
	{ 16, Linear, _____L16, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_V16U16 = 0x36,
	{ 32, Linear, NoComponents, XTL::D3DFMT_V16U16 },
	// X_D3DFMT_LIN_L6V5U5 = 0x37, // Alias : X_D3DFMT_LIN_R6G5B5
	{ 16, Linear, __R6G5B5, XTL::D3DFMT_L6V5U5 },
	// X_D3DFMT_R5G5B5A1 = 0x38,
	{ 16, Swizzled, R5G5B5A1, XTL::D3DFMT_UNKNOWN },
	// TODO : { 16, Swizzled, R5G5B5A1, XTL::D3DFMT_A1R5G5B5, "X_D3DFMT_R5G5B5A1 -> D3DFMT_A1R5G5B5" }, // TODO : Requires conversion
	// X_D3DFMT_R4G4B4A4 = 0x39,
	{ 16, Swizzled, R4G4B4A4, XTL::D3DFMT_UNKNOWN },
	// TODO : { 16, Swizzled, R4G4B4A4, XTL::D3DFMT_A4R4G4B4, "X_D3DFMT_R4G4B4A4 -> D3DFMT_A4R4G4B4" }, // TODO : Requires conversion
	// X_D3DFMT_Q8W8V8U8 = 0x3A, // Alias : X_D3DFMT_A8B8G8R8
	{ 32, Swizzled, A8B8G8R8, XTL::D3DFMT_Q8W8V8U8 }, // Was : D3DFMT_A8R8G8B8 // TODO : Test
	// X_D3DFMT_B8G8R8A8 = 0x3B,
	{ 32, Swizzled, B8G8R8A8, XTL::D3DFMT_UNKNOWN },
	// TODO : { 32, Swizzled, B8G8R8A8, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_B8G8R8A8 -> D3DFMT_A8R8G8B8" }, // TODO : Requires conversion
	// X_D3DFMT_R8G8B8A8 = 0x3C,
	{ 32, Swizzled, R8G8B8A8, XTL::D3DFMT_UNKNOWN },
	// X_D3DFMT_LIN_R5G5B5A1 = 0x3D,
	{ 16, Linear, R5G5B5A1, XTL::D3DFMT_UNKNOWN },
	// TODO : { 16, Linear, R5G5B5A1, XTL::D3DFMT_A1R5G5B5, "X_D3DFMT_LIN_R5G5B5A1 -> D3DFMT_A1R5G5B5" }, // TODO : Requires conversion
	// X_D3DFMT_LIN_R4G4B4A4 = 0x3E,
	{ 16, Linear, R4G4B4A4, XTL::D3DFMT_UNKNOWN },
	// TODO : { 16, Linear, R4G4B4A4, XTL::D3DFMT_A4R4G4B4, "X_D3DFMT_R4G4B4A4 -> D3DFMT_A4R4G4B4" }, // TODO : Requires conversion
	// X_D3DFMT_LIN_A8B8G8R8 = 0x3F,
	{ 32, Linear, A8B8G8R8, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_LIN_A8B8G8R8 -> D3DFMT_A8R8G8B8" }, // Cxbx NOTE: HACK: R<->B Swapped! // TODO : Requires conversion
	// X_D3DFMT_LIN_B8G8R8A8 = 0x40,
	{ 32, Linear, B8G8R8A8, XTL::D3DFMT_UNKNOWN },
	// TODO : { 32, Linear, B8G8R8A8, XTL::D3DFMT_A8R8G8B8, "X_D3DFMT_B8G8R8A8 -> D3DFMT_A8R8G8B8" }, // TODO : Requires conversion
	// X_D3DFMT_LIN_R8G8B8A8 = 0x41,
	{ 32, Linear, R8G8B8A8, XTL::D3DFMT_UNKNOWN },
/*
	// undefined : 0x42-0x63
	{},
	// X_D3DFMT_VERTEXDATA = 0x64,
	{ 8, Linear, NoComponents, XTL::D3DFMT_VERTEXDATA },
	// Dxbx addition : X_D3DFMT_INDEX16 is not an Xbox format, but used internally
	// X_D3DFMT_INDEX16 = 0x65 
	{ 16, Linear, NoComponents, XTL::D3DFMT_INDEX16 },
*/
};

XTL::D3DCOLOR XTL::DecodeUInt32ToColor(const ComponentEncodingInfo * encoding, const uint32 value)
{
	return D3DCOLOR_ARGB(
		((encoding->AShift < 0) ? 255 : (value >> encoding->AShift)) << (8 - encoding->ABits),
		((encoding->RShift < 0) ? 255 : (value >> encoding->RShift)) << (8 - encoding->RBits),
		((encoding->GShift < 0) ? 255 : (value >> encoding->GShift)) << (8 - encoding->GBits),
		((encoding->BShift < 0) ? 255 : (value >> encoding->BShift)) << (8 - encoding->BBits)
	);
};

const XTL::ComponentEncodingInfo *XTL::EmuXBFormatComponentEncodingInfo(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		if (FormatInfos[Format].components != NoComponents)
			return &(ComponentEncodingInfos[FormatInfos[Format].components]);

	return nullptr;
}

bool XTL::EmuXBFormatRequiresConversionToARGB(X_D3DFORMAT Format)
{
	const ComponentEncodingInfo *info = EmuXBFormatComponentEncodingInfo(Format);
	// Conversion is required if there's ARGB conversion info present, and the format has a warning message
	if (info != nullptr)
		if (FormatInfos[Format].warning != nullptr)
			return true;

	return false;
}

DWORD XTL::EmuXBFormatBitsPerPixel(X_D3DFORMAT Format)
{
	if (Format <= X_D3DFMT_LIN_R8G8B8A8)
		if (FormatInfos[Format].bits_per_pixel > 0) // TODO : Remove this
			return FormatInfos[Format].bits_per_pixel;

	return 16; // TODO : 8
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

	return (Format == X_D3DFMT_VERTEXDATA); // TODO : false;
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
		const FormatInfo *info = &FormatInfos[Format];
		if (info->warning != nullptr)
			EmuWarning(info->warning);

		return info->pc;
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
