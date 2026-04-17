// ******************************************************************
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
#ifndef XBCONVERT_H
#define XBCONVERT_H

#include "common/cxbxr.hpp"
#include "core\kernel\init\CxbxKrnl.h"

#include "core\hle\D3D8\XbD3D8Types.h"

#define VERTICES_PER_DOT 1
#define VERTICES_PER_LINE 2
#define VERTICES_PER_TRIANGLE 3
#define VERTICES_PER_QUAD 4
#define TRIANGLES_PER_QUAD 2

#define LOG_PREFIX_D3DCVT CXBXR_MODULE::D3DCVT

// simple render state encoding lookup table
#define X_D3DRSSE_UNK 0x7fffffff

enum _ComponentEncoding {
	NoCmpnts = 0, // Format doesn't contain any component (ARGB/QWVU)
	A1R5G5B5,
	X1R5G5B5, // NOTE : A=255
	A4R4G4B4,
	__R5G6B5, // NOTE : A=255
	A8R8G8B8,
	X8R8G8B8, // NOTE : A=255
	____R8B8, // NOTE : A takes R, G takes B
	____G8B8, // NOTE : A takes G, R takes B
	______A8, // TEST : R=G=B= 255
	__R6G5B5, // NOTE : A=255
	__L6V5U5, // NOTE : A=255
	R5G5B5A1,
	R4G4B4A4,
	A8B8G8R8,
	B8G8R8A8,
	R8G8B8A8,
	______L8, // NOTE : A=255, R=G=B= L
	_____AL8, // NOTE : A=R=G=B= L
	_____L16, // NOTE : Actually G8B8, with A=R=255
	____A8L8, // NOTE : R=G=B= L
	____DXT1,
	____DXT3,
	____DXT5,
	______P8,
	____YUY2,
	____UYVY,
};

typedef void(*FormatToARGBRow)(const uint8_t* src, uint8_t* dst_argb, int width);

extern void __L6V5U5ToX8L8V8U8Row_C(const uint8_t* src_l6v5u5, uint8_t* dst_xlvu, int width);
extern const FormatToARGBRow ComponentConverters[];
extern const FormatToARGBRow EmuXBFormatComponentConverter(xbox::X_D3DFORMAT Format);

bool EmuXBFormatCanBeConverted(xbox::X_D3DFORMAT Format, EMUFORMAT &PCFormat);

bool EmuXBFormatRequiresConversion(xbox::X_D3DFORMAT Format, EMUFORMAT &PCFormat);

// how many bits does this format use per pixel?
extern DWORD EmuXBFormatBitsPerPixel(xbox::X_D3DFORMAT Format);

// how many bytes does this format use per pixel?
extern DWORD EmuXBFormatBytesPerPixel(xbox::X_D3DFORMAT Format);

// is this format compressed?
extern BOOL EmuXBFormatIsCompressed(xbox::X_D3DFORMAT Format);

// is this format linear?
extern BOOL EmuXBFormatIsLinear(xbox::X_D3DFORMAT Format);

// is this format swizzled?
extern BOOL EmuXBFormatIsSwizzled(xbox::X_D3DFORMAT Format);

// is this format a valid render target?
extern BOOL EmuXBFormatIsRenderTarget(xbox::X_D3DFORMAT Format);

// is this format a valid depth buffer?
extern BOOL EmuXBFormatIsDepthBuffer(xbox::X_D3DFORMAT Format);

// convert from xbox to pc color formats
extern EMUFORMAT EmuXB2PC_D3DFormat(xbox::X_D3DFORMAT Format);

// convert from xbox to pc d3d lock flags
#ifndef CXBX_USE_D3D11
extern DWORD EmuXB2PC_D3DLock(DWORD Flags);
#endif

// convert from xbox to pc multisample formats
#ifndef CXBX_USE_D3D11
extern D3DMULTISAMPLE_TYPE EmuXB2PC_D3DMultiSampleFormat(DWORD Type);
#endif

/**
// convert from pc to xbox texture transform state types (unnecessary so far)
if((uint32)State < 4)
    State = (D3DTRANSFORMSTATETYPE)(State - 2);
else if((uint32)State < 20)
    State = (D3DTRANSFORMSTATETYPE)(State - 14);
else if((uint32)State > 255)
    State = (D3DTRANSFORMSTATETYPE)(State - 250);
else
    CxbxrAbortEx(LOG_PREFIX_D3DCVT, "Unknown Transform State Type (%d)", State);
//*/

// convert from xbox to pc blend ops
inline D3DBLENDOP EmuXB2PC_D3DBLENDOP(xbox::X_D3DBLENDOP Value)
{
	switch(Value)
    {
        case 0x8006: return D3DBLENDOP_ADD;
        case 0x800a: return D3DBLENDOP_SUBTRACT;
        case 0x800b: return D3DBLENDOP_REVSUBTRACT;
        case 0x8007: return D3DBLENDOP_MIN;
        case 0x8008: return D3DBLENDOP_MAX;
        case 0xF006: {
            EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "D3DBLENDOP_ADDSIGNED is not supported!");
            return D3DBLENDOP_ADD;
        };
        case 0xF005: {
            EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "D3DBLENDOP_REVSUBTRACTSIGNED is not supported!");
            return D3DBLENDOP_REVSUBTRACT;
        }
    }

    EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown D3DBLENDOP (0x%.08X)", Value);

    return (D3DBLENDOP)D3DBLENDOP_ADD;
}

// convert from xbox to pc blend types 
inline D3DBLEND EmuXB2PC_D3DBLEND(xbox::X_D3DBLEND Value)
{
	switch (Value) {
        case 0x000: return D3DBLEND_ZERO;
        case 0x001: return D3DBLEND_ONE;
        case 0x300: return D3DBLEND_SRCCOLOR;
        case 0x301: return D3DBLEND_INVSRCCOLOR;
        case 0x302: return D3DBLEND_SRCALPHA;
        case 0x303: return D3DBLEND_INVSRCALPHA;
        case 0x304: return D3DBLEND_DESTALPHA;
        case 0x305: return D3DBLEND_INVDESTALPHA;
        case 0x306: return D3DBLEND_DESTCOLOR;
        case 0x307: return D3DBLEND_INVDESTCOLOR;
        case 0x308: return D3DBLEND_SRCALPHASAT;
        case 0x8001:return D3DBLEND_BLENDFACTOR; // Maps Xbox D3DBLEND_CONSTANTCOLOR
        case 0x8002:return D3DBLEND_INVBLENDFACTOR; // Maps Xbox D3DBLEND_INVCONSTANTCOLOR
        case 0x8003: EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unsupported Xbox D3DBlend Extension: D3DBLEND_CONSTANTALPHA"); return D3DBLEND_ONE;
        case 0x8004: EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unsupported Xbox D3DBlend Extension: D3DBLEND_INVCONSTANTALPHA"); return D3DBLEND_ONE;
    }

    EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown Xbox D3DBLEND Extension (0x%.08X)", Value);
	return D3DBLEND_ONE;
}

// convert from xbox to pc comparison functions
inline D3DCMPFUNC EmuXB2PC_D3DCMPFUNC(xbox::X_D3DCMPFUNC Value)
{
	switch (Value) {
        case 0x200: return D3DCMP_NEVER;
        case 0x201: return D3DCMP_LESS;
        case 0x202: return D3DCMP_EQUAL;
        case 0x203: return D3DCMP_LESSEQUAL;
        case 0x204: return D3DCMP_GREATER;
        case 0x205: return D3DCMP_NOTEQUAL;
        case 0x206: return D3DCMP_GREATEREQUAL;
        case 0x207: return D3DCMP_ALWAYS;
    }

    EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown Xbox D3DCMPFUNC Extension (0x%.08X)", Value);
    return D3DCMP_NEVER;
}

// convert from xbox to pc fill modes
inline D3DFILLMODE EmuXB2PC_D3DFILLMODE(xbox::X_D3DFILLMODE Value)
{
	switch (Value) {
        case 0x1B00: return D3DFILL_POINT;
        case 0x1B01: return D3DFILL_WIREFRAME;
        case 0x1B02: return D3DFILL_SOLID;
    }

    EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown Xbox D3DFILLMODE Extension (0x%.08X)", Value);
    return D3DFILL_SOLID;
}

// convert from xbox to pc shade modes
inline D3DSHADEMODE EmuXB2PC_D3DSHADEMODE(xbox::X_D3DSHADEMODE Value)
{
	switch (Value) {
        case 0x1D00: return D3DSHADE_FLAT;
        case 0x1D01: return D3DSHADE_GOURAUD;
    }

    EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown Xbox D3DSHADEMODE Extension (0x%.08X)", Value);
    return D3DSHADE_GOURAUD;
}

// convert from xbox to pc stencilop modes
inline D3DSTENCILOP EmuXB2PC_D3DSTENCILOP(xbox::X_D3DSTENCILOP Value)
{
    switch(Value)
    {
        case 0x1e00: return D3DSTENCILOP_KEEP;
        case 0x0000: return D3DSTENCILOP_ZERO;
        case 0x1e01: return D3DSTENCILOP_REPLACE;
        case 0x1e02: return D3DSTENCILOP_INCRSAT;
        case 0x1e03: return D3DSTENCILOP_DECRSAT;
        case 0x150a: return D3DSTENCILOP_INVERT;
        case 0x8507: return D3DSTENCILOP_INCR;
        case 0x8508: return D3DSTENCILOP_DECR;
        default:
            EmuLogEx(LOG_PREFIX_D3DCVT, LOG_LEVEL::WARNING, "Unknown D3DSTENCILOP (0x%.08X)", Value);
    }

    return (D3DSTENCILOP) Value;
}

// table used for vertex->primitive count conversion
extern const UINT g_XboxPrimitiveTypeInfo[xbox::X_D3DPT_POLYGON + 1][2];

inline bool IsValidXboxVertexCount(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, UINT VertexCount)
{
	assert(XboxPrimitiveType < xbox::X_D3DPT_MAX);

	// Are there more vertices than required for setup?
	if (VertexCount > g_XboxPrimitiveTypeInfo[XboxPrimitiveType][0])
		// Are the additional vertices exact multiples of the required additional vertices per primitive?
		if (0 == ((VertexCount - g_XboxPrimitiveTypeInfo[XboxPrimitiveType][0]) % g_XboxPrimitiveTypeInfo[XboxPrimitiveType][1]))
			return true;

	return false;
}

// convert from vertex count to primitive count (Xbox)
inline unsigned ConvertXboxVertexCountToPrimitiveCount(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType, unsigned VertexCount)
{
	assert(XboxPrimitiveType < xbox::X_D3DPT_MAX);

	return (VertexCount - g_XboxPrimitiveTypeInfo[XboxPrimitiveType][0]) / g_XboxPrimitiveTypeInfo[XboxPrimitiveType][1];
}

// conversion table for xbox->pc primitive types
extern const D3DPRIMITIVETYPE g_XboxPrimitiveTypeToHost[];

// convert xbox->pc primitive type
inline D3DPRIMITIVETYPE EmuXB2PC_D3DPrimitiveType(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType)
{
	if (XboxPrimitiveType >= xbox::X_D3DPT_MAX) {
		LOG_TEST_CASE("XboxPrimitiveType too large");
		return D3DPT_FORCE_DWORD;
	}

    return g_XboxPrimitiveTypeToHost[XboxPrimitiveType];
}

#ifdef CXBX_USE_D3D11
extern const D3D_PRIMITIVE_TOPOLOGY g_XboxPrimitiveTypeToD3D11Topology[];

// convert xbox primitive type to D3D11 primitive topology
inline D3D_PRIMITIVE_TOPOLOGY EmuXB2PC_D3D11PrimitiveTopology(xbox::X_D3DPRIMITIVETYPE XboxPrimitiveType)
{
	if (XboxPrimitiveType >= xbox::X_D3DPT_MAX) {
		LOG_TEST_CASE("XboxPrimitiveType too large");
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}

	return g_XboxPrimitiveTypeToD3D11Topology[XboxPrimitiveType];
}
#endif

extern void EmuUnswizzleBox
(
	CONST PVOID pSrcBuff,
	CONST DWORD dwWidth,
	CONST DWORD dwHeight,
	CONST DWORD dwDepth,
	CONST DWORD dwBytesPerPixel,
	CONST PVOID pDstBuff,
	CONST DWORD dwDstRowPitch,
	CONST DWORD dwDstSlicePitch
); // NOPATCH

#include "devices/video/nv2a_methods.h"


typedef enum _TXBType {
	xt_Unknown = 0, // Defined as zero, to coincide with default value of DxbxRenderStateInfo.T and DxbxTextureStageStateInfo.T

	xtBOOL,
	xtBYTE,
	xtD3DBLEND,
	xtD3DBLENDOP,
	xtD3DCLEAR,
	xtD3DCMPFUNC,
	xtD3DCOLOR,
	xtD3DCOLORWRITEENABLE,
	xtD3DCUBEMAP_FACES,
	xtD3DCULL,
	xtD3DDCC,
	xtD3DFILLMODE,
	xtD3DFOGMODE,
	xtD3DFORMAT,
	xtD3DFRONT,
	xtD3DLOGICOP,
	xtD3DMCS,
	xtD3DMULTISAMPLE_TYPE,
	xtD3DMULTISAMPLEMODE,
	xtD3DPRIMITIVETYPE,
	xtD3DRESOURCETYPE,
	xtD3DSAMPLEALPHA,
	xtD3DSHADEMODE,
	xtD3DSTENCILOP,
	xtD3DSWATH,
	xtD3DTEXTUREADDRESS, // Used for TextureStageState X_D3DTSS_ADDRESSU, X_D3DTSS_ADDRESSV and X_D3DTSS_ADDRESSW
	xtD3DTEXTUREFILTERTYPE, // Used for TextureStageState X_D3DTSS_MAGFILTER, X_D3DTSS_MINFILTER and X_D3DTSS_MIPFILTER
	xtD3DTEXTUREOP, // Used for TextureStageState X_D3DTSS_COLOROP and X_D3DTSS_ALPHAOP
	xtD3DTEXTURESTAGESTATETYPE,
	xtD3DTRANSFORMSTATETYPE,
	xtD3DVERTEXBLENDFLAGS, // Used for X_D3DRS_VERTEXBLEND
	xtD3DVSDE,
	xtD3DWRAP,
	xtDWORD,
	xtFloat,
	xtLONG
} TXBType;

typedef struct _RenderStateInfo {
	const char *S;   // String representation.
	WORD V;    // The XDK version since which a render state was introduced (using the 5911 declarations as a base).
	TXBType T = xt_Unknown; // The Xbox data type. Defaults to xt_Unknown.
	xbox::NV2AMETHOD M; // The related push buffer method. Not always a 1-to-1 mapping. Needs push-buffer interpretation & conversion code.
	D3DRENDERSTATETYPE PC = (D3DRENDERSTATETYPE)0; // Map XBox to D3D9 render state
	const char *N;   // XDK notes. Defaults to ''.
	WORD R; // The XDK version since which a render state was removed
}
RenderStateInfo;

#define D3DRS_UNSUPPORTED ((D3DRENDERSTATETYPE)0)

extern const RenderStateInfo& GetDxbxRenderStateInfo(int State);

extern xbox::X_D3DFORMAT GetXboxPixelContainerFormat(const xbox::dword_xt XboxPixelContainer_Format);

extern xbox::X_D3DFORMAT GetXboxPixelContainerFormat(const xbox::X_D3DPixelContainer* pXboxPixelContainer);

extern bool ConvertD3DTextureToARGBBuffer(
	xbox::X_D3DFORMAT X_Format,
	uint8_t* pSrc,
	int SrcWidth, int SrcHeight, int SrcRowPitch, int SrcSlicePitch,
	uint8_t* pDst, int DstRowPitch, int DstSlicePitch,
	unsigned int uiDepth = 1,
	xbox::PVOID pPalleteData = xbox::zeroptr
);

extern void CxbxSetPixelContainerHeader
(
	xbox::X_D3DPixelContainer* pPixelContainer,
	DWORD                      Common,
	UINT                       Width,
	UINT                       Height,
	UINT                       Levels,
	xbox::X_D3DFORMAT          Format,
	UINT                       Dimensions,
	UINT                       Pitch
);

extern uint8_t* ConvertD3DTextureToARGB(
	xbox::X_D3DPixelContainer* pXboxPixelContainer,
	uint8_t* pSrc,
	int* pWidth, int* pHeight,
	xbox::PVOID pPalleteData = xbox::zeroptr
);

#endif
