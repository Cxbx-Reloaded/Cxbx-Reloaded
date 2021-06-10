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
#ifndef XBD3D8TYPES_H
#define XBD3D8TYPES_H

#undef UNICODE // make sure dxerr.h DXGetErrorString is aliassed to *A, not *W

// NOTE: required because XbD3D8Types.h is included in places where xbox_types.h is not, which causes compiler errors because of the undefined types
#include "xbox_types.h"

#ifdef CXBX_USE_D3D11
// include direct3d 11 headers
#include <d3d11.h>
#include <DirectXMath.h> // XMVECTORF32
#else
// include direct3d 9x headers
#define DIRECT3D_VERSION 0x0900
#include <d3d9.h>
//implies #include <d3d9types.h> // for D3DFORMAT, D3DLIGHT9, etc
//implies #include <d3d9caps.h>
#include <d3dx9math.h> // for D3DXVECTOR4, etc
#include <d3dx9tex.h>

#include <dxerr9.h>
//#pragma comment(lib, "dxerr.lib") // See https://blogs.msdn.microsoft.com/chuckw/2012/04/24/wheres-dxerr-lib/
#endif

// If the above doesn't compile, install the June 2010 DirectX SDK
// from https://www.microsoft.com/en-us/download/details.aspx?id=6812
// and select the Direct3D 9 include & library path (TODO : how?)

// We're going to use the approach detailed in :
// https://blogs.msdn.microsoft.com/chuckw/2015/03/23/the-zombie-directx-sdk/


// For transforming code that's written for Direct3D 8 into Direct3D 9,
// See "Converting to Direct3D 9" https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx

// For transforming code that's written for Direct3D 9 into Direct3D 11,
// See "DirectX 9 to DirectX 11" https://docs.microsoft.com/en-us/windows/uwp/gaming/porting-considerations

// See https://msdn.microsoft.com/en-us/library/windows/desktop/bb204851(v=vs.85).aspx#D3DENUM_NO_WHQL_LEVEL_Changes
#define D3DENUM_NO_WHQL_LEVEL 0 // default in Direct3D 9

#ifdef CXBX_USE_D3D11
#define _9_11(_9, _11) _11
#else
#define _9_11(_9, _11) _9
#endif

// Alias all host Direct3D 9 symbols to generic symbols
#define DXGetErrorString                DXGetErrorString9A
#define DXGetErrorDescription           DXGetErrorDescription9A
#define D3DLockData                     void

#define D3DADAPTER_IDENTIFIER           D3DADAPTER_IDENTIFIER9
#define D3DCAPS                         D3DCAPS9
#define D3DVERTEXELEMENT                D3DVERTEXELEMENT9
#define D3DVIEWPORT                     _9_11(D3DVIEWPORT9,                D3D11_VIEWPORT)

#define IDirect3DDevice                 _9_11(IDirect3DDevice9Ex,          ID3D11Device)
#define IDirect3DVertexDeclaration      _9_11(IDirect3DVertexDeclaration9, ID3D11InputLayout)
#define IDirect3DVertexShader           _9_11(IDirect3DVertexShader9,      ID3D11VertexShader)
#define IDirect3DPixelShader            _9_11(IDirect3DPixelShader9,       ID3D11PixelShader)
#define IDirect3DResource               _9_11(IDirect3DResource9,          ID3D11Resource)
#define IDirect3DBaseTexture            _9_11(IDirect3DBaseTexture9,       ID3D11Resource)
#define IDirect3DTexture                _9_11(IDirect3DTexture9,           ID3D11Texture2D)
#define IDirect3DVolumeTexture          _9_11(IDirect3DVolumeTexture9,     ID3D11Texture3D)
#define IDirect3DCubeTexture            _9_11(IDirect3DCubeTexture9,       ID3D11Texture2D) // array of six 2D textures (one for each face)
#define IDirect3DVertexBuffer           _9_11(IDirect3DVertexBuffer9,      ID3D11Buffer)
#define IDirect3DIndexBuffer            _9_11(IDirect3DIndexBuffer9,       ID3D11Buffer) // or ID3D11ShaderResourceView ?
#define IDirect3DSurface                IDirect3DSurface9
#define IDirect3DVolume                 IDirect3DVolume9
#define IDirect3DQuery                  IDirect3DQuery9

typedef xbox::word_xt INDEX16; // TODO: Move INDEX16 into xbox namespace

namespace xbox {

// Xbox D3D types, probably just copies of the Windows Direct3D 9 counterparts :

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DVECTOR {
	float x;
	float y;
	float z;
} X_D3DVECTOR;
#else
typedef D3DVECTOR X_D3DVECTOR;
#endif

typedef struct _X_D3DCOLORVALUE {
	float r;
	float g;
	float b;
	float a;
} X_D3DCOLORVALUE;

typedef struct _X_D3DMATERIAL8 {
	X_D3DCOLORVALUE Diffuse;        /* Diffuse color RGBA */
	X_D3DCOLORVALUE Ambient;        /* Ambient color RGB */
	X_D3DCOLORVALUE Specular;       /* Specular 'shininess' */
	X_D3DCOLORVALUE Emissive;       /* Emissive color RGB */
	float           Power;          /* Sharpness if specular highlight */
} X_D3DMATERIAL8;

typedef enum _X_D3DLIGHTTYPE {
	D3DLIGHT_POINT = 1,
	D3DLIGHT_SPOT = 2,
	D3DLIGHT_DIRECTIONAL = 3,
	D3DLIGHT_FORCE_DWORD = 0x7fffffff, /* force 32-bit size enum */
} X_D3DLIGHTTYPE;

typedef struct _D3DLIGHT8 {
	X_D3DLIGHTTYPE  Type;            /* Type of light source */
	X_D3DCOLORVALUE Diffuse;         /* Diffuse color of light */
	X_D3DCOLORVALUE Specular;        /* Specular color of light */
	X_D3DCOLORVALUE Ambient;         /* Ambient color of light */
	X_D3DVECTOR     Position;         /* Position in world space */
	X_D3DVECTOR     Direction;        /* Direction in world space */
	float           Range;            /* Cutoff range */
	float           Falloff;          /* Falloff */
	float           Attenuation0;     /* Constant attenuation */
	float           Attenuation1;     /* Linear attenuation */
	float           Attenuation2;     /* Quadratic attenuation */
	float           Theta;            /* Inner angle of spotlight cone */
	float           Phi;              /* Outer angle of spotlight cone */
} X_D3DLIGHT8;

typedef struct _X_D3DVIEWPORT8 {
	DWORD       X;
	DWORD       Y;            /* Viewport Top left */
	DWORD       Width;
	DWORD       Height;       /* Viewport Dimensions */
	float       MinZ;         /* Min/max of clip Volume */
	float       MaxZ;
} X_D3DVIEWPORT8;

/* SwapEffects */
typedef enum _X_D3DSWAPEFFECT
{
	D3DSWAPEFFECT_DISCARD = 1,
	D3DSWAPEFFECT_FLIP = 2,
	D3DSWAPEFFECT_COPY = 3,
	D3DSWAPEFFECT_COPY_XBOX = 4, // Unknown, probably copy vsync, collides with Windows Direct3D9Ex D3DSWAPEFFECT_OVERLAY

	D3DSWAPEFFECT_FORCE_DWORD = 0x7fffffff
} X_D3DSWAPEFFECT;


// TODO: fill out these enumeration tables for convienance
#ifdef CXBX_USE_D3D11
typedef XMVECTORF32 X_D3DXVECTOR4;
#else
typedef D3DXVECTOR4 X_D3DXVECTOR4;
#endif
typedef DWORD X_D3DBLENDOP;
typedef DWORD X_D3DBLEND;
typedef DWORD X_D3DCMPFUNC;
typedef DWORD X_D3DFILLMODE;
typedef DWORD X_D3DMULTISAMPLE_TYPE;
typedef DWORD X_D3DSHADEMODE;
typedef DWORD X_D3DSTENCILOP;
typedef DWORD X_D3DTEXTURESTAGESTATETYPE;

typedef enum _X_D3DCULL
{
	X_D3DCULL_NONE = 0,
	X_D3DCULL_CW = 0x900,
	X_D3DCULL_CCW = 0x901,
	X_D3DCULL_FORCE_DWORD = 0x7fffffff
}
X_D3DCULL;

typedef enum _X_D3DFORMAT
{
/*
	Xbox1 D3DFORMAT notes
	---------------------

	The Xbox1 D3DFORMAT type consists of 4 different format categories :
	1. Swizzled (improves data locality, incompatible with native Direct3D)
	2. Compressed (DXT compression, giving 4:1 reduction on 4x4 pixel blocks)
	3. Linear (compatible with native Direct3D)
	4. Depth (Fixed or Floating point, stored Linear or Swizzled)

	Requirements\Format      Swizzled  Compressed  Linear  Depth   Notes

	Power-of-two required ?  YES       YES         NO      NO
	Mipmap supported ?       YES       YES         NO      YES     Linear has MipmapLevels = 1
	CubeMaps supported ?     YES       YES         NO      NO      Cubemaps have 6 faces
	Supports volumes ?       YES       YES         NO      NO      Volumes have 3 dimensions, Textures have 2
	Can be a rendertarget ?  YES       YES         YES     LINEAR  Depth buffers can only be rendered to if stored Linear

	Implications :
	- CubeMaps must be square
	- Volumes cannot be cube mapped and vice versa

	Maximum dimensions :
	2D : 4096 x 4096 (12 mipmap levels)
	3D : 512 x 512 x 512 (9 mipmap levels)

*/

	// Xbox D3DFORMAT types :
	// See https://wiki.beyondunreal.com/Legacy:Texture_Format

	// Swizzled Formats

	X_D3DFMT_L8 = 0x00,
	X_D3DFMT_AL8 = 0x01,
	X_D3DFMT_A1R5G5B5 = 0x02,
	X_D3DFMT_X1R5G5B5 = 0x03,
	X_D3DFMT_A4R4G4B4 = 0x04,
	X_D3DFMT_R5G6B5 = 0x05,
	X_D3DFMT_A8R8G8B8 = 0x06,
	X_D3DFMT_X8R8G8B8 = 0x07,
	X_D3DFMT_X8L8V8U8 = 0x07, // Alias

	X_D3DFMT_P8 = 0x0b, // 8-bit Palletized

	X_D3DFMT_A8 = 0x19,
	X_D3DFMT_A8L8 = 0x1a,
	X_D3DFMT_R6G5B5 = 0x27,
	X_D3DFMT_L6V5U5 = 0x27, // Alias

	X_D3DFMT_G8B8 = 0x28,
	X_D3DFMT_V8U8 = 0x28, // Alias

	X_D3DFMT_R8B8 = 0x29,
	X_D3DFMT_D24S8 = 0x2a,
	X_D3DFMT_F24S8 = 0x2b,
	X_D3DFMT_D16 = 0x2c,
	X_D3DFMT_D16_LOCKABLE = 0x2c, // Alias

	X_D3DFMT_F16 = 0x2d,
	X_D3DFMT_L16 = 0x32,
	X_D3DFMT_V16U16 = 0x33,
	X_D3DFMT_R5G5B5A1 = 0x38,
	X_D3DFMT_R4G4B4A4 = 0x39,
	X_D3DFMT_A8B8G8R8 = 0x3A,
	X_D3DFMT_Q8W8V8U8 = 0x3A, // Alias

	X_D3DFMT_B8G8R8A8 = 0x3B,
	X_D3DFMT_R8G8B8A8 = 0x3C,

	// YUV Formats

	X_D3DFMT_YUY2 = 0x24,
	X_D3DFMT_UYVY = 0x25,

	// Compressed Formats

	X_D3DFMT_DXT1 = 0x0C, // opaque/one-bit alpha
	X_D3DFMT_DXT2 = 0x0E, // Alias for X_D3DFMT_DXT3
	X_D3DFMT_DXT3 = 0x0E, // linear alpha
	X_D3DFMT_DXT4 = 0x0F, // Alias for X_D3DFMT_DXT5
	X_D3DFMT_DXT5 = 0x0F, // interpolated alpha

	// Linear Formats

	X_D3DFMT_LIN_A1R5G5B5 = 0x10,
	X_D3DFMT_LIN_R5G6B5 = 0x11,
	X_D3DFMT_LIN_A8R8G8B8 = 0x12,
	X_D3DFMT_LIN_L8 = 0x13,
	X_D3DFMT_LIN_R8B8 = 0x16,
	X_D3DFMT_LIN_G8B8 = 0x17,
	X_D3DFMT_LIN_V8U8 = 0x17, // Alias

	X_D3DFMT_LIN_AL8 = 0x1b,
	X_D3DFMT_LIN_X1R5G5B5 = 0x1c,
	X_D3DFMT_LIN_A4R4G4B4 = 0x1d,
	X_D3DFMT_LIN_X8R8G8B8 = 0x1e,
	X_D3DFMT_LIN_X8L8V8U8 = 0x1e, // Alias

	X_D3DFMT_LIN_A8 = 0x1f,
	X_D3DFMT_LIN_A8L8 = 0x20,
	X_D3DFMT_LIN_D24S8 = 0x2E,
	X_D3DFMT_LIN_F24S8 = 0x2f,
	X_D3DFMT_LIN_D16 = 0x30,
	X_D3DFMT_LIN_F16 = 0x31,
	X_D3DFMT_LIN_L16 = 0x35,
	X_D3DFMT_LIN_V16U16 = 0x36,
	X_D3DFMT_LIN_R6G5B5 = 0x37,
	X_D3DFMT_LIN_L6V5U5 = 0x37, // Alias

	X_D3DFMT_LIN_R5G5B5A1 = 0x3D,
	X_D3DFMT_LIN_R4G4B4A4 = 0x3e,
	X_D3DFMT_LIN_A8B8G8R8 = 0x3f,
	X_D3DFMT_LIN_B8G8R8A8 = 0x40,
	X_D3DFMT_LIN_R8G8B8A8 = 0x41,

	X_D3DFMT_VERTEXDATA = 0x64,

	X_D3DFMT_INDEX16 = 101/*=D3DFMT_INDEX16*/, // Dxbx addition : Not an Xbox format, used internally

	X_D3DFMT_UNKNOWN = 0xFFFFFFFF - 3,  // Unique declaration to make overloads possible
}
X_D3DFORMAT, *PX_D3DFORMAT;

// Primitives supported by draw-primitive API
typedef enum _X_D3DPRIMITIVETYPE
{
    X_D3DPT_NONE = 0, // Dxbx addition

    X_D3DPT_POINTLIST             = 1,
    X_D3DPT_LINELIST              = 2,
    X_D3DPT_LINELOOP              = 3,    // Xbox only
    X_D3DPT_LINESTRIP             = 4,
    X_D3DPT_TRIANGLELIST          = 5,
    X_D3DPT_TRIANGLESTRIP         = 6,
    X_D3DPT_TRIANGLEFAN           = 7,
    X_D3DPT_QUADLIST              = 8,    // Xbox only
    X_D3DPT_QUADSTRIP             = 9,    // Xbox only
    X_D3DPT_POLYGON               = 10,   // Xbox only

    X_D3DPT_MAX                   = 11,
    X_D3DPT_INVALID               = 0x7fffffff, /* force 32-bit size enum */
}
X_D3DPRIMITIVETYPE;

typedef enum _X_D3DRESOURCETYPE
{
    X_D3DRTYPE_NONE               =  0,
    X_D3DRTYPE_SURFACE            =  1, // = D3DRESOURCETYPE.D3DRTYPE_SURFACE
    X_D3DRTYPE_VOLUME             =  2, // = D3DRESOURCETYPE.D3DRTYPE_VOLUME
    X_D3DRTYPE_TEXTURE            =  3, // = D3DRESOURCETYPE.D3DRTYPE_TEXTURE
    X_D3DRTYPE_VOLUMETEXTURE      =  4, // = D3DRESOURCETYPE.D3DRTYPE_VOLUMETEXTURE
    X_D3DRTYPE_CUBETEXTURE        =  5, // = D3DRESOURCETYPE.D3DRTYPE_CUBETEXTURE
    X_D3DRTYPE_VERTEXBUFFER       =  6, // = D3DRESOURCETYPE.D3DRTYPE_VERTEXBUFFER
    X_D3DRTYPE_INDEXBUFFER        =  7, // = D3DRESOURCETYPE.D3DRTYPE_INDEXBUFFER
    X_D3DRTYPE_PUSHBUFFER         =  8,
    X_D3DRTYPE_PALETTE            =  9,
    X_D3DRTYPE_FIXUP              =  10,

    X_D3DRTYPE_FORCE_DWORD        = 0x7fffffff
}
X_D3DRESOURCETYPE;

// D3DUSAGE values (all but the Xbox extensions match the PC versions) :
#define X_D3DUSAGE_RENDERTARGET           0x00000001
#define X_D3DUSAGE_DEPTHSTENCIL           0x00000002
// for Vertex/Index buffers
#define X_D3DUSAGE_WRITEONLY              0x00000008
#define X_D3DUSAGE_POINTS                 0x00000040
#define X_D3DUSAGE_RTPATCHES              0x00000080
#define X_D3DUSAGE_DYNAMIC                0x00000200
// for CreateVertexShader
#define X_D3DUSAGE_PERSISTENTDIFFUSE      0x00000400L   // Xbox-only
#define X_D3DUSAGE_PERSISTENTSPECULAR     0x00000800L   // Xbox-only
#define X_D3DUSAGE_PERSISTENTBACKDIFFUSE  0x00001000L   // Xbox-only
#define X_D3DUSAGE_PERSISTENTBACKSPECULAR 0x00002000L   // Xbox-only
// for CreateTexture/CreateImageSurface
#define X_D3DUSAGE_BORDERSOURCE_COLOR     0x00000000L   // Xbox-only
#define X_D3DUSAGE_BORDERSOURCE_TEXTURE   0x00010000L   // Xbox-only

#define X_D3D_RENDER_MEMORY_ALIGNMENT     64

#define X_D3DSURFACE_ALIGNMENT            X_D3D_RENDER_MEMORY_ALIGNMENT
#define X_D3DTEXTURE_ALIGNMENT            (2 * X_D3D_RENDER_MEMORY_ALIGNMENT)
#define X_D3DTEXTURE_CUBEFACE_ALIGNMENT   (2 * X_D3D_RENDER_MEMORY_ALIGNMENT)
#define X_D3DTEXTURE_PITCH_ALIGNMENT      X_D3D_RENDER_MEMORY_ALIGNMENT
#define X_D3DTEXTURE_PITCH_MIN            X_D3DTEXTURE_PITCH_ALIGNMENT

typedef enum _X_D3DSET_DEPTH_CLIP_PLANES_FLAGS
{
    X_D3DSDCP_SET_VERTEXPROGRAM_PLANES         = 1,
    X_D3DSDCP_SET_FIXEDFUNCTION_PLANES         = 2,
    X_D3DSDCP_USE_DEFAULT_VERTEXPROGRAM_PLANES = 3,
    X_D3DSDCP_USE_DEFAULT_FIXEDFUNCTION_PLANES = 4,
} 
X_D3DSET_DEPTH_CLIP_PLANES_FLAGS;

#define X_D3DPRESENTFLAG_LOCKABLE_BACKBUFFER      0x00000001
#define X_D3DPRESENTFLAG_WIDESCREEN               0x00000010
#define X_D3DPRESENTFLAG_INTERLACED               0x00000020
#define X_D3DPRESENTFLAG_PROGRESSIVE              0x00000040
#define X_D3DPRESENTFLAG_FIELD                    0x00000080
#define X_D3DPRESENTFLAG_10X11PIXELASPECTRATIO    0x00000100

typedef struct _X_D3DDISPLAYMODE
{
    UINT        Width;
    UINT        Height;
    UINT        RefreshRate;
    DWORD       Flags;
    X_D3DFORMAT Format;
}
X_D3DDISPLAYMODE;

typedef struct _X_D3DVERTEXBUFFER_DESC
{
	X_D3DFORMAT           Format;
	X_D3DRESOURCETYPE     Type;
}
X_D3DVERTEXBUFFER_DESC;

typedef struct _X_D3DINDEXBUFFER_DESC
{
	X_D3DFORMAT           Format;
	X_D3DRESOURCETYPE     Type;
}
X_D3DINDEXBUFFER_DESC;

typedef struct _X_D3DSURFACE_DESC
{
    X_D3DFORMAT         Format;
    X_D3DRESOURCETYPE   Type;
    DWORD               Usage;
    UINT                Size;
    X_D3DMULTISAMPLE_TYPE MultiSampleType;
    UINT                Width;
    UINT                Height;
}
X_D3DSURFACE_DESC;

struct X_D3DSurface; // forward
typedef struct _X_D3DPRESENT_PARAMETERS
{
    UINT                BackBufferWidth;
    UINT                BackBufferHeight;
    X_D3DFORMAT         BackBufferFormat;
    UINT                BackBufferCount;

    X_D3DMULTISAMPLE_TYPE MultiSampleType;

    X_D3DSWAPEFFECT     SwapEffect;
    HWND                hDeviceWindow;
    BOOL                Windowed;
    BOOL                EnableAutoDepthStencil;
    X_D3DFORMAT         AutoDepthStencilFormat;
    DWORD               Flags;

    UINT                FullScreen_RefreshRateInHz;
    UINT                FullScreen_PresentationInterval;
    // The Windows DirectX8 variant ends here
    // This check guarantees identical layout, compared to Direct3D8._D3DPRESENT_PARAMETERS_:
    // assert(offsetof(X_D3DPRESENT_PARAMETERS, BufferSurfaces) == sizeof(_D3DPRESENT_PARAMETERS_));
    X_D3DSurface       *BufferSurfaces[3];
    X_D3DSurface       *DepthStencilSurface;
}
X_D3DPRESENT_PARAMETERS;

typedef struct _X_D3DGAMMARAMP
{
    BYTE    red[256];
    BYTE    green[256];
    BYTE    blue[256];
}
X_D3DGAMMARAMP;

typedef struct _X_D3DPIXELSHADERDEF	// <- blueshogun 10/1/07
{
   DWORD    PSAlphaInputs[8];          // X_D3DRS_PSALPHAINPUTS0..X_D3DRS_PSALPHAINPUTS7 : Alpha inputs for each stage
   DWORD    PSFinalCombinerInputsABCD; // X_D3DRS_PSFINALCOMBINERINPUTSABCD : Final combiner inputs
   DWORD    PSFinalCombinerInputsEFG;  // X_D3DRS_PSFINALCOMBINERINPUTSEFG : Final combiner inputs (continued)
   DWORD    PSConstant0[8];            // X_D3DRS_PSCONSTANT0_0..X_D3DRS_PSCONSTANT0_7 : C0 for each stage
   DWORD    PSConstant1[8];            // X_D3DRS_PSCONSTANT1_0..X_D3DRS_PSCONSTANT1_7 : C1 for each stage
   DWORD    PSAlphaOutputs[8];         // X_D3DRS_PSALPHAOUTPUTS0..X_D3DRS_PSALPHAOUTPUTS7 : Alpha output for each stage
   DWORD    PSRGBInputs[8];            // X_D3DRS_PSRGBINPUTS0..X_D3DRS_PSRGBINPUTS7 : RGB inputs for each stage
   DWORD    PSCompareMode;             // X_D3DRS_PSCOMPAREMODE : Compare modes for clipplane texture mode
   DWORD    PSFinalCombinerConstant0;  // X_D3DRS_PSFINALCOMBINERCONSTANT0 : C0 in final combiner
   DWORD    PSFinalCombinerConstant1;  // X_D3DRS_PSFINALCOMBINERCONSTANT1 : C1 in final combiner
   DWORD    PSRGBOutputs[8];           // X_D3DRS_PSRGBOUTPUTS0..X_D3DRS_PSRGBOUTPUTS7 : Stage 0 RGB outputs
   DWORD    PSCombinerCount;           // X_D3DRS_PSCOMBINERCOUNT : Active combiner count (Stages 0-7)
   DWORD    PSTextureModes;            // X_D3DRS_PS_RESERVED (copied from out-of-range X_D3DRS_PSTEXTUREMODES) : Texture addressing modes
   DWORD    PSDotMapping;              // X_D3DRS_PSDOTMAPPING : Input mapping for dot product modes
   DWORD    PSInputTexture;            // X_D3DRS_PSINPUTTEXTURE : Texture source for some texture modes

    // These last three DWORDs are used to define how Direct3D8 pixel shader constants map to the constant
    // registers in each combiner stage. They are used by the Direct3D run-time software but not by the hardware.
   DWORD    PSC0Mapping;               // Mapping of c0 regs to D3D constants
   DWORD    PSC1Mapping;               // Mapping of c1 regs to D3D constants
   DWORD    PSFinalCombinerConstants;  // Final combiner constant mapping
}
X_D3DPIXELSHADERDEF;

typedef void X_D3DDevice;

typedef struct _X_PixelShader
{
	DWORD RefCount;
	DWORD D3DOwned;
	X_D3DPIXELSHADERDEF *pPSDef;
} X_PixelShader;

struct X_D3DResource
{
    DWORD Common;
	DWORD Data;
	DWORD Lock;
};

// D3D resource "common" masks
#define X_D3DCOMMON_REFCOUNT_MASK      0x0000FFFF
#define X_D3DCOMMON_TYPE_MASK          0x00070000
#define X_D3DCOMMON_TYPE_SHIFT         16
#define X_D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define X_D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define X_D3DCOMMON_TYPE_PUSHBUFFER    0x00020000
#define X_D3DCOMMON_TYPE_PALETTE       0x00030000
#define X_D3DCOMMON_TYPE_TEXTURE       0x00040000
#define X_D3DCOMMON_TYPE_SURFACE       0x00050000 // Also covers Volume resources
#define X_D3DCOMMON_TYPE_FIXUP         0x00060000
#define X_D3DCOMMON_INTREFCOUNT_MASK   0x00780000
#define X_D3DCOMMON_INTREFCOUNT_SHIFT  19
#define X_D3DCOMMON_INTREFCOUNT_1      (1 << X_D3DCOMMON_INTREFCOUNT_SHIFT) // Dxbx addition
#define X_D3DCOMMON_VIDEOMEMORY        0x00000000 // Was 0x00800000, but Xbox doesn't have this flag!
#define X_D3DCOMMON_D3DCREATED         0x01000000
#define X_D3DCOMMON_ISLOCKED           0x02000010 // Surface is currently locked (potential unswizzle candidate)
#define X_D3DCOMMON_UNUSED_MASK        0xFE000000
#define X_D3DCOMMON_UNUSED_SHIFT       25

// d3d palette common
#define X_D3DPALETTE_COMMON_PALETTESIZE_MASK       0xC0000000
#define X_D3DPALETTE_COMMON_PALETTESIZE_SHIFT      30

// special resource lock flags
#define X_D3DRESOURCE_LOCK_FLAG_NOSIZE  0xEFFFFFFF

// Lock flags
#define X_D3DLOCK_NOFLUSH               0x00000010 // Xbox extension
#define X_D3DLOCK_NOOVERWRITE           0x00000020
#define X_D3DLOCK_TILED                 0x00000040 // Xbox extension
#define X_D3DLOCK_READONLY              0x00000080

// Multisample modes
const int X_D3DMULTISAMPLE_NONE                                    = 0x0011;
const int X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR            = 0x1021;
const int X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX          = 0x1121;
const int X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR = 0x2021;
const int X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR   = 0x2012;
const int X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR            = 0x1022;
const int X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN          = 0x1222;
const int X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR            = 0x2022;
const int X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN          = 0x2222;
const int X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN          = 0x1233;
const int X_D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN          = 0x2233;

// Multisample masks (Cxbx additions)
const int X_D3DMULTISAMPLE_YSCALE_MASK                             = 0x000F;
const int X_D3DMULTISAMPLE_YSCALE_SHIFT = 0;

const int X_D3DMULTISAMPLE_XSCALE_MASK                             = 0x00F0;
const int X_D3DMULTISAMPLE_XSCALE_SHIFT = 4;

const int X_D3DMULTISAMPLE_ALGO_LINEAR                           = 0x0000;
const int X_D3DMULTISAMPLE_ALGO_QUINCUNX                         = 0x0100;
const int X_D3DMULTISAMPLE_ALGO_GAUSSIAN                         = 0x0200;
const int X_D3DMULTISAMPLE_ALGO_MASK                             = 0x0300;

const int X_D3DMULTISAMPLE_SAMPLING_NONE                           = 0x0000;
const int X_D3DMULTISAMPLE_SAMPLING_MULTI                          = 0x1000;
const int X_D3DMULTISAMPLE_SAMPLING_SUPER                          = 0x2000;
const int X_D3DMULTISAMPLE_SAMPLING_MASK                           = 0x3000;

const int X_D3DMULTISAMPLE_KNOWN_MASK = 0
	| X_D3DMULTISAMPLE_YSCALE_MASK
	| X_D3DMULTISAMPLE_XSCALE_MASK
	| X_D3DMULTISAMPLE_ALGO_MASK
	| X_D3DMULTISAMPLE_SAMPLING_MASK
	;


struct X_D3DVertexBuffer : public X_D3DResource
{

};
struct X_D3DIndexBuffer : public X_D3DResource
{

};

struct X_D3DPushBuffer : public X_D3DResource
{
    ULONG Size;
    ULONG AllocationSize;
};

struct X_D3DFixup : public X_D3DResource
{
    ULONG Run;
    ULONG Next;
    ULONG Size;
};

struct X_D3DPalette : public X_D3DResource
{
};

typedef enum _X_D3DPALETTESIZE
{
    D3DPALETTE_256              = 0,
    D3DPALETTE_128              = 1,
    D3DPALETTE_64               = 2,
    D3DPALETTE_32               = 3,
    D3DPALETTE_MAX              = 4,
    D3DPALETTE_FORCE_DWORD      = 0x7fffffff, /* force 32-bit size enum */
}
X_D3DPALETTESIZE;

struct X_D3DPixelContainer : public X_D3DResource
{
    DWORD		Format; // Format information about the texture.
    DWORD       Size; // Size of a non power-of-2 texture, must be zero otherwise
};

// pixel container "format" masks
#define X_D3DFORMAT_RESERVED1_MASK        0x00000003      // Must be zero
#define X_D3DFORMAT_DMACHANNEL_MASK       0x00000003
#define X_D3DFORMAT_DMACHANNEL_A          0x00000001      // DMA channel A - the default for all system memory
#define X_D3DFORMAT_DMACHANNEL_B          0x00000002      // DMA channel B - unused
#define X_D3DFORMAT_CUBEMAP               0x00000004      // Set if the texture if a cube map
#define X_D3DFORMAT_BORDERSOURCE_COLOR    0x00000008      // If set, uses D3DTSS_BORDERCOLOR as a border
#define X_D3DFORMAT_DIMENSION_MASK        0x000000F0      // # of dimensions, must be 2 or 3
#define X_D3DFORMAT_DIMENSION_SHIFT       4
#define X_D3DFORMAT_FORMAT_MASK           0x0000FF00      // D3DFORMAT - See X_D3DFMT_* above
#define X_D3DFORMAT_FORMAT_SHIFT          8
#define X_D3DFORMAT_MIPMAP_MASK           0x000F0000      // # mipmap levels (always 1 for surfaces)
#define X_D3DFORMAT_MIPMAP_SHIFT          16
#define X_D3DFORMAT_USIZE_MASK            0x00F00000      // Log 2 of the U size of the base texture (only set for swizzled or compressed)
#define X_D3DFORMAT_USIZE_SHIFT           20
#define X_D3DFORMAT_VSIZE_MASK            0x0F000000      // Log 2 of the V size of the base texture (only set for swizzled or compressed)
#define X_D3DFORMAT_VSIZE_SHIFT           24
#define X_D3DFORMAT_PSIZE_MASK            0xF0000000      // Log 2 of the P size of the base texture (only set for swizzled or compressed)
#define X_D3DFORMAT_PSIZE_SHIFT           28

// pixel container "size" masks
// The layout of the size field, used for non swizzled or compressed textures.
//
// The Size field of a container will be zero if the texture is swizzled or compressed.
// It is guarenteed to be non-zero otherwise because either the height/width will be
// greater than one or the pitch adjust will be nonzero because the minimum texture
// pitch is 8 bytes.
#define X_D3DSIZE_WIDTH_MASK              0x00000FFF   // Width of the texture - 1, in texels
//#define X_D3DSIZE_WIDTH_SHIFT           0
#define X_D3DSIZE_HEIGHT_MASK             0x00FFF000   // Height of the texture - 1, in texels
#define X_D3DSIZE_HEIGHT_SHIFT            12
#define X_D3DSIZE_PITCH_MASK              0xFF000000   // Pitch / 64 - 1
#define X_D3DSIZE_PITCH_SHIFT             24


struct X_D3DBaseTexture : public X_D3DPixelContainer
{

};

struct X_D3DTexture : public X_D3DBaseTexture
{

};

struct X_D3DVolumeTexture : public X_D3DBaseTexture
{

};

struct X_D3DCubeTexture : public X_D3DBaseTexture
{

};

struct X_D3DVolume : public X_D3DPixelContainer
{
	X_D3DBaseTexture *Parent;
};

struct X_D3DSurface : public X_D3DPixelContainer
{
	X_D3DBaseTexture *Parent;
};

struct X_D3DTILE
{
    DWORD   Flags;
    PVOID   pMemory;
    DWORD   Size;
    DWORD   Pitch;
    DWORD   ZStartTag;
    DWORD   ZOffset;
};

typedef enum _X_D3DCALLBACKTYPE	// blueshogun96 10/1/07
{
	X_D3DCALLBACK_READ		= 0, // Fixed PatrickvL 10/7/22
	X_D3DCALLBACK_WRITE		= 1
}
X_D3DCALLBACKTYPE;

typedef enum _X_D3DFIELDTYPE
{
    X_D3DFIELD_ODD            = 1,
    X_D3DFIELD_EVEN           = 2,
    X_D3DFIELD_PROGRESSIVE    = 3,
    X_D3DFIELD_FORCE_DWORD    = 0x7fffffff
}
X_D3DFIELDTYPE;

typedef struct _X_D3DFIELD_STATUS
{
    X_D3DFIELDTYPE Field;
    UINT           VBlankCount;
}
X_D3DFIELD_STATUS;

typedef struct _D3DVBLANKDATA
{
    DWORD           VBlank;
    DWORD           Swap;
    DWORD           Flags;
}
X_D3DVBLANKDATA;

typedef struct _D3DSWAPDATA 
{
    DWORD           Swap;
    DWORD           SwapVBlank;
    DWORD           MissedVBlanks;
    DWORD           TimeUntilSwapVBlank;
    DWORD           TimeBetweenSwapVBlanks;
} 
X_D3DSWAPDATA;

typedef enum _X_D3DSWAP
{
	X_D3DSWAP_DEFAULT = 0,
	X_D3DSWAP_COPY = 1,
	X_D3DSWAP_BYPASSCOPY = 2,
	X_D3DSWAP_FINISH = 4,
}
X_D3DSWAP;

// D3DVBLANKCALLBACK
typedef void (__cdecl * X_D3DVBLANKCALLBACK)(X_D3DVBLANKDATA *pData);

// D3DSWAPCALLBACK
typedef void (__cdecl * X_D3DSWAPCALLBACK)(X_D3DSWAPDATA *pData);

// D3DCALLBACK
typedef void (__cdecl * X_D3DCALLBACK)(DWORD Context);

// X_D3DRENDERSTATETYPE values
typedef enum _X_D3DRENDERSTATETYPE {

	// Dxbx note : These declarations are from XDK version 5933, the most recent and complete version.
	// Older versions are slightly different (some members are missing), so we use a mapping table to
	// cater for the differences (see XboxRenderStateConverter::BuildRenderStateMappingTable). This enables to ignore these
	// version-differences in the rest of our code (unless it matters somehow); We write like this :
	//   XboxRenderStates.SetXboxRenderState(X_D3DRENDERSTATETYPE, Value);
	//
	// And we read like this (do note, that missing elements all point to the same dummy) :
	//   Result = XboxRenderStates.GetXboxRenderState(X_D3DRENDERSTATETYPE);

	// Dxbx note : The PS* render states map 1-on-1 to the X_D3DPIXELSHADERDEF record,
	// SetPixelShader actually pushes the definition into these render state slots.
	// See DxbxUpdateActivePixelShader for how this is employed.

	// The set starts out with "pixel-shader" render states (all Xbox extensions) :
	X_D3DRS_PSALPHAINPUTS0 = 0,
	X_D3DRS_PSALPHAINPUTS1 = 1,
	X_D3DRS_PSALPHAINPUTS2 = 2,
	X_D3DRS_PSALPHAINPUTS3 = 3,
	X_D3DRS_PSALPHAINPUTS4 = 4,
	X_D3DRS_PSALPHAINPUTS5 = 5,
	X_D3DRS_PSALPHAINPUTS6 = 6,
	X_D3DRS_PSALPHAINPUTS7 = 7,
	X_D3DRS_PSFINALCOMBINERINPUTSABCD = 8,
	X_D3DRS_PSFINALCOMBINERINPUTSEFG = 9,
	X_D3DRS_PSCONSTANT0_0 = 10,
	X_D3DRS_PSCONSTANT0_1 = 11,
	X_D3DRS_PSCONSTANT0_2 = 12,
	X_D3DRS_PSCONSTANT0_3 = 13,
	X_D3DRS_PSCONSTANT0_4 = 14,
	X_D3DRS_PSCONSTANT0_5 = 15,
	X_D3DRS_PSCONSTANT0_6 = 16,
	X_D3DRS_PSCONSTANT0_7 = 17,
	X_D3DRS_PSCONSTANT1_0 = 18,
	X_D3DRS_PSCONSTANT1_1 = 19,
	X_D3DRS_PSCONSTANT1_2 = 20,
	X_D3DRS_PSCONSTANT1_3 = 21,
	X_D3DRS_PSCONSTANT1_4 = 22,
	X_D3DRS_PSCONSTANT1_5 = 23,
	X_D3DRS_PSCONSTANT1_6 = 24,
	X_D3DRS_PSCONSTANT1_7 = 25,
	X_D3DRS_PSALPHAOUTPUTS0 = 26,
	X_D3DRS_PSALPHAOUTPUTS1 = 27,
	X_D3DRS_PSALPHAOUTPUTS2 = 28,
	X_D3DRS_PSALPHAOUTPUTS3 = 29,
	X_D3DRS_PSALPHAOUTPUTS4 = 30,
	X_D3DRS_PSALPHAOUTPUTS5 = 31,
	X_D3DRS_PSALPHAOUTPUTS6 = 32,
	X_D3DRS_PSALPHAOUTPUTS7 = 33,
	X_D3DRS_PSRGBINPUTS0 = 34,
	X_D3DRS_PSRGBINPUTS1 = 35,
	X_D3DRS_PSRGBINPUTS2 = 36,
	X_D3DRS_PSRGBINPUTS3 = 37,
	X_D3DRS_PSRGBINPUTS4 = 38,
	X_D3DRS_PSRGBINPUTS5 = 39,
	X_D3DRS_PSRGBINPUTS6 = 40,
	X_D3DRS_PSRGBINPUTS7 = 41,
	X_D3DRS_PSCOMPAREMODE = 42,
	X_D3DRS_PSFINALCOMBINERCONSTANT0 = 43,
	X_D3DRS_PSFINALCOMBINERCONSTANT1 = 44,
	X_D3DRS_PSRGBOUTPUTS0 = 45,
	X_D3DRS_PSRGBOUTPUTS1 = 46,
	X_D3DRS_PSRGBOUTPUTS2 = 47,
	X_D3DRS_PSRGBOUTPUTS3 = 48,
	X_D3DRS_PSRGBOUTPUTS4 = 49,
	X_D3DRS_PSRGBOUTPUTS5 = 50,
	X_D3DRS_PSRGBOUTPUTS6 = 51,
	X_D3DRS_PSRGBOUTPUTS7 = 52,
	X_D3DRS_PSCOMBINERCOUNT = 53,
	X_D3DRS_PS_RESERVED = 54, // Dxbx note : This takes the slot of X_D3DPIXELSHADERDEF.PSTextureModes, set by D3DDevice_SetRenderState_LogicOp?
	X_D3DRS_PSDOTMAPPING = 55,
	X_D3DRS_PSINPUTTEXTURE = 56,
	// End of "pixel-shader" render states, continuing with "simple" render states :
	X_D3DRS_ZFUNC = 57, // D3DCMPFUNC
	X_D3DRS_ALPHAFUNC = 58, // D3DCMPFUNC
	X_D3DRS_ALPHABLENDENABLE = 59, // TRUE to enable alpha blending
	X_D3DRS_ALPHATESTENABLE = 60, // TRUE to enable alpha tests
	X_D3DRS_ALPHAREF = 61, // BYTE
	X_D3DRS_SRCBLEND = 62, // D3DBLEND
	X_D3DRS_DESTBLEND = 63, // D3DBLEND
	X_D3DRS_ZWRITEENABLE = 64, // TRUE to enable Z writes
	X_D3DRS_DITHERENABLE = 65, // TRUE to enable dithering
	X_D3DRS_SHADEMODE = 66, // D3DSHADEMODE
	X_D3DRS_COLORWRITEENABLE = 67, // D3DCOLORWRITEENABLE_ALPHA, etc. per-channel write enable
	X_D3DRS_STENCILZFAIL = 68, // D3DSTENCILOP to do if stencil test passes and Z test fails
	X_D3DRS_STENCILPASS = 69, // D3DSTENCILOP to do if both stencil and Z tests pass
	X_D3DRS_STENCILFUNC = 70, // D3DCMPFUNC
	X_D3DRS_STENCILREF = 71, // BYTE reference value used in stencil test
	X_D3DRS_STENCILMASK = 72, // BYTE mask value used in stencil test
	X_D3DRS_STENCILWRITEMASK = 73, // BYTE write mask applied to values written to stencil buffer
	X_D3DRS_BLENDOP = 74, // D3DBLENDOP setting
	X_D3DRS_BLENDCOLOR = 75, // D3DCOLOR for D3DBLEND_CONSTANTCOLOR
	X_D3DRS_SWATHWIDTH = 76, // D3DSWATHWIDTH (Xbox ext.)
	X_D3DRS_POLYGONOFFSETZSLOPESCALE = 77, // float Z factor for shadow maps (Xbox ext.)
	X_D3DRS_POLYGONOFFSETZOFFSET = 78, // Xbox ext.
	X_D3DRS_POINTOFFSETENABLE = 79, // Xbox ext.
	X_D3DRS_WIREFRAMEOFFSETENABLE = 80, // Xbox ext.
	X_D3DRS_SOLIDOFFSETENABLE = 81, // Xbox ext.
	X_D3DRS_DEPTHCLIPCONTROL = 82, // [4432+] Xbox ext.
	X_D3DRS_STIPPLEENABLE = 83, // [4627+] Xbox ext.
	X_D3DRS_SIMPLE_UNUSED8 = 84, // [4627+]
	X_D3DRS_SIMPLE_UNUSED7 = 85, // [4627+]
	X_D3DRS_SIMPLE_UNUSED6 = 86, // [4627+]
	X_D3DRS_SIMPLE_UNUSED5 = 87, // [4627+]
	X_D3DRS_SIMPLE_UNUSED4 = 88, // [4627+]
	X_D3DRS_SIMPLE_UNUSED3 = 89, // [4627+]
	X_D3DRS_SIMPLE_UNUSED2 = 90, // [4627+]
	X_D3DRS_SIMPLE_UNUSED1 = 91, // [4627+]
	// End of "simple" render states, continuing with "deferred" render states :
	X_D3DRS_FOGENABLE = 92,
	X_D3DRS_FOGTABLEMODE = 93,
	X_D3DRS_FOGSTART = 94,
	X_D3DRS_FOGEND = 95,
	X_D3DRS_FOGDENSITY = 96,
	X_D3DRS_RANGEFOGENABLE = 97,
	X_D3DRS_WRAP0 = 98,
	X_D3DRS_WRAP1 = 99,
	X_D3DRS_WRAP2 = 100, // Dxbx addition
	X_D3DRS_WRAP3 = 101, // Dxbx addition
	X_D3DRS_LIGHTING = 102,
	X_D3DRS_SPECULARENABLE = 103,
	X_D3DRS_LOCALVIEWER = 104, // Dxbx addition
	X_D3DRS_COLORVERTEX = 105,
	X_D3DRS_BACKSPECULARMATERIALSOURCE = 106, // Xbox ext. nsp.
	X_D3DRS_BACKDIFFUSEMATERIALSOURCE = 107, // Xbox ext. nsp.
	X_D3DRS_BACKAMBIENTMATERIALSOURCE = 108, // Xbox ext. nsp.
	X_D3DRS_BACKEMISSIVEMATERIALSOURCE = 109, // Xbox ext. nsp.
	X_D3DRS_SPECULARMATERIALSOURCE = 110,
	X_D3DRS_DIFFUSEMATERIALSOURCE = 111,
	X_D3DRS_AMBIENTMATERIALSOURCE = 112,
	X_D3DRS_EMISSIVEMATERIALSOURCE = 113,
	X_D3DRS_BACKAMBIENT = 114, // Xbox ext. nsp.
	X_D3DRS_AMBIENT = 115,
	X_D3DRS_POINTSIZE = 116,
	X_D3DRS_POINTSIZE_MIN = 117,
	X_D3DRS_POINTSPRITEENABLE = 118,
	X_D3DRS_POINTSCALEENABLE = 119,
	X_D3DRS_POINTSCALE_A = 120,
	X_D3DRS_POINTSCALE_B = 121,
	X_D3DRS_POINTSCALE_C = 122,
	X_D3DRS_POINTSIZE_MAX = 123,
	X_D3DRS_PATCHEDGESTYLE = 124, // Dxbx addition
	X_D3DRS_PATCHSEGMENTS = 125,
	X_D3DRS_SWAPFILTER = 126, // [4039+] Xbox ext. nsp. D3DTEXF_LINEAR etc. filter to use for Swap
	X_D3DRS_PRESENTATIONINTERVAL = 127, // [4627+] Xbox ext. nsp. TODO : Use 4361?
	X_D3DRS_DEFERRED_UNUSED8 = 128, // [4627+]
	X_D3DRS_DEFERRED_UNUSED7 = 129, // [4627+]
	X_D3DRS_DEFERRED_UNUSED6 = 130, // [4627+]
	X_D3DRS_DEFERRED_UNUSED5 = 131, // [4627+]
	X_D3DRS_DEFERRED_UNUSED4 = 132, // [4627+]
	X_D3DRS_DEFERRED_UNUSED3 = 133, // [4627+]
	X_D3DRS_DEFERRED_UNUSED2 = 134, // [4627+]
	X_D3DRS_DEFERRED_UNUSED1 = 135, // [4627+]
	// End of "deferred" render states, continuing with "complex" render states :
	X_D3DRS_PSTEXTUREMODES = 136, // Xbox ext.
	X_D3DRS_VERTEXBLEND = 137,
	X_D3DRS_FOGCOLOR = 138,
	X_D3DRS_FILLMODE = 139,
	X_D3DRS_BACKFILLMODE = 140, // Dxbx addition : Xbox ext. nsp.
	X_D3DRS_TWOSIDEDLIGHTING = 141, // Dxbx addition : Xbox ext. nsp.
	X_D3DRS_NORMALIZENORMALS = 142,
	X_D3DRS_ZENABLE = 143,
	X_D3DRS_STENCILENABLE = 144,
	X_D3DRS_STENCILFAIL = 145,
	X_D3DRS_FRONTFACE = 146, // Dxbx addition : Xbox ext. nsp.
	X_D3DRS_CULLMODE = 147,
	X_D3DRS_TEXTUREFACTOR = 148,
	X_D3DRS_ZBIAS = 149,
	X_D3DRS_LOGICOP = 150, // Xbox ext. nsp.
	X_D3DRS_EDGEANTIALIAS = 151, // Dxbx note : No Xbox ext. (according to Direct3D8) !
	X_D3DRS_MULTISAMPLEANTIALIAS = 152,
	X_D3DRS_MULTISAMPLEMASK = 153,
	X_D3DRS_MULTISAMPLETYPE = 154, // [-4039] Xbox ext.
	// Note : X_D3DRS_MULTISAMPLETYPE seems the only one that got removed, but it does need a slot, so the rest is increased by 1 compared to 5933.
	X_D3DRS_MULTISAMPLEMODE = 155, // [4361+] Xbox ext. // D3DMULTISAMPLEMODE for the backbuffer
	X_D3DRS_MULTISAMPLERENDERTARGETMODE = 156, // [4039+] Xbox ext.
	X_D3DRS_SHADOWFUNC = 157, // D3DCMPFUNC (Xbox extension)
	X_D3DRS_LINEWIDTH = 158, // Xbox ext.
	X_D3DRS_SAMPLEALPHA = 159, // Xbox ext.
	X_D3DRS_DXT1NOISEENABLE = 160, // Xbox ext.
	X_D3DRS_YUVENABLE = 161, // [3911+] Xbox ext.
	X_D3DRS_OCCLUSIONCULLENABLE = 162, // [3911+] Xbox ext.
	X_D3DRS_STENCILCULLENABLE = 163, // [3911+] Xbox ext.
	X_D3DRS_ROPZCMPALWAYSREAD = 164, // [3911+] Xbox ext.
	X_D3DRS_ROPZREAD = 165, // [3911+] Xbox ext.
	X_D3DRS_DONOTCULLUNCOMPRESSED = 166, // [3911+] Xbox ext.
	// End of "complex" render states.
	X_D3DRS_UNK = 0x7fffffff // deferred render state "unknown" flag
} X_D3DRENDERSTATETYPE;

// Render state boundaries :

#define X_D3DRS_PS_FIRST X_D3DRS_PSALPHAINPUTS0
#define X_D3DRS_PS_LAST X_D3DRS_PSINPUTTEXTURE

#define X_D3DRS_SIMPLE_FIRST X_D3DRS_ZFUNC
#define X_D3DRS_SIMPLE_LAST X_D3DRS_SIMPLE_UNUSED1

#define X_D3DRS_DEFERRED_FIRST X_D3DRS_FOGENABLE
#define X_D3DRS_DEFERRED_LAST X_D3DRS_DEFERRED_UNUSED1

#define X_D3DRS_COMPLEX_FIRST X_D3DRS_PSTEXTUREMODES
#define X_D3DRS_COMPLEX_LAST X_D3DRS_DONOTCULLUNCOMPRESSED

#define X_D3DRS_FIRST X_D3DRS_PS_FIRST
#define X_D3DRS_LAST X_D3DRS_COMPLEX_LAST

// X_D3DWRAP values :
constexpr DWORD X_D3DWRAP_U = 0x00000010;
constexpr DWORD X_D3DWRAP_V = 0x00001000;
constexpr DWORD X_D3DWRAP_W = 0x00100000;

// X_D3DTEXTURESTAGESTATETYPE values :
// Dxbx note : See DxbxFromOldVersion_D3DTSS(), as these might need correction for older SDK versions!
// The set starts out with "deferred" texture states :
constexpr DWORD X_D3DTSS_ADDRESSU = 0;
constexpr DWORD X_D3DTSS_ADDRESSV = 1;
constexpr DWORD X_D3DTSS_ADDRESSW = 2;
constexpr DWORD X_D3DTSS_MAGFILTER = 3;
constexpr DWORD X_D3DTSS_MINFILTER = 4;
constexpr DWORD X_D3DTSS_MIPFILTER = 5;
constexpr DWORD X_D3DTSS_MIPMAPLODBIAS = 6;
constexpr DWORD X_D3DTSS_MAXMIPLEVEL = 7;
constexpr DWORD X_D3DTSS_MAXANISOTROPY = 8;
constexpr DWORD X_D3DTSS_COLORKEYOP = 9; // Xbox ext.
constexpr DWORD X_D3DTSS_COLORSIGN = 10; // Xbox ext.
constexpr DWORD X_D3DTSS_ALPHAKILL = 11; // Xbox ext.
constexpr DWORD X_D3DTSS_COLOROP = 12;
constexpr DWORD X_D3DTSS_COLORARG0 = 13;
constexpr DWORD X_D3DTSS_COLORARG1 = 14;
constexpr DWORD X_D3DTSS_COLORARG2 = 15;
constexpr DWORD X_D3DTSS_ALPHAOP = 16;
constexpr DWORD X_D3DTSS_ALPHAARG0 = 17;
constexpr DWORD X_D3DTSS_ALPHAARG1 = 18;
constexpr DWORD X_D3DTSS_ALPHAARG2 = 19;
constexpr DWORD X_D3DTSS_RESULTARG = 20;
constexpr DWORD X_D3DTSS_TEXTURETRANSFORMFLAGS = 21;
// End of "deferred" texture states, continuing with the rest :
constexpr DWORD X_D3DTSS_BUMPENVMAT00 = 22;
constexpr DWORD X_D3DTSS_BUMPENVMAT01 = 23;
constexpr DWORD X_D3DTSS_BUMPENVMAT11 = 24;
constexpr DWORD X_D3DTSS_BUMPENVMAT10 = 25;
constexpr DWORD X_D3DTSS_BUMPENVLSCALE = 26;
constexpr DWORD X_D3DTSS_BUMPENVLOFFSET = 27;
constexpr DWORD X_D3DTSS_TEXCOORDINDEX = 28;
constexpr DWORD X_D3DTSS_BORDERCOLOR = 29;
constexpr DWORD X_D3DTSS_COLORKEYCOLOR = 30; // Xbox ext.
constexpr DWORD X_D3DTSS_UNSUPPORTED = 31; // Note : Somehow, this one comes through D3DDevice_SetTextureStageStateNotInline sometimes
// End of texture states.

// Texture state boundaries :

constexpr DWORD X_D3DTSS_DEFERRED_FIRST = X_D3DTSS_ADDRESSU;
constexpr DWORD X_D3DTSS_DEFERRED_LAST = X_D3DTSS_TEXTURETRANSFORMFLAGS;

constexpr DWORD X_D3DTSS_FIRST = X_D3DTSS_ADDRESSU;
constexpr DWORD X_D3DTSS_LAST = X_D3DTSS_COLORKEYCOLOR;

constexpr DWORD X_D3DTS_STAGECOUNT = 4; // Dxbx addition
constexpr DWORD X_D3DTS_STAGESIZE = 32; // Dxbx addition

constexpr DWORD X_PSH_COMBINECOUNT = 8; // Dxbx addition
constexpr DWORD X_PSH_CONSTANTCOUNT = 8; // Dxbx addition

// X_D3DTEXTUREOP values :
constexpr DWORD X_D3DTOP_DISABLE = 1;
constexpr DWORD X_D3DTOP_SELECTARG1 = 2;
constexpr DWORD X_D3DTOP_SELECTARG2 = 3;
constexpr DWORD X_D3DTOP_MODULATE = 4;
constexpr DWORD X_D3DTOP_MODULATE2X = 5;
constexpr DWORD X_D3DTOP_MODULATE4X = 6;
constexpr DWORD X_D3DTOP_ADD = 7;
constexpr DWORD X_D3DTOP_ADDSIGNED = 8;
constexpr DWORD X_D3DTOP_ADDSIGNED2X = 9;
constexpr DWORD X_D3DTOP_SUBTRACT = 10;
constexpr DWORD X_D3DTOP_ADDSMOOTH = 11;
constexpr DWORD X_D3DTOP_BLENDDIFFUSEALPHA = 12;
constexpr DWORD X_D3DTOP_BLENDCURRENTALPHA = 13;
constexpr DWORD X_D3DTOP_BLENDTEXTUREALPHA = 14;
constexpr DWORD X_D3DTOP_BLENDFACTORALPHA = 15;
constexpr DWORD X_D3DTOP_BLENDTEXTUREALPHAPM = 16;
constexpr DWORD X_D3DTOP_PREMODULATE = 17;
constexpr DWORD X_D3DTOP_MODULATEALPHA_ADDCOLOR = 18;
constexpr DWORD X_D3DTOP_MODULATECOLOR_ADDALPHA = 19;
constexpr DWORD X_D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20;
constexpr DWORD X_D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21;
constexpr DWORD X_D3DTOP_DOTPRODUCT3 = 22;
constexpr DWORD X_D3DTOP_MULTIPLYADD = 23;
constexpr DWORD X_D3DTOP_LERP = 24;
constexpr DWORD X_D3DTOP_BUMPENVMAP = 25;
constexpr DWORD X_D3DTOP_BUMPENVMAPLUMINANCE = 26;

constexpr DWORD X_D3DTOP_FIRST = X_D3DTOP_DISABLE;
constexpr DWORD X_D3DTOP_LAST = X_D3DTOP_BUMPENVMAPLUMINANCE;

// X_D3DTEXTUREADDRESS values :
constexpr DWORD X_D3DTADDRESS_WRAP = 1;
constexpr DWORD X_D3DTADDRESS_MIRROR = 2;
constexpr DWORD X_D3DTADDRESS_CLAMP = 3;
constexpr DWORD X_D3DTADDRESS_BORDER = 4;
constexpr DWORD X_D3DTADDRESS_CLAMPTOEDGE = 5;

// X_D3DTEXTUREFILTERTYPE Values
constexpr DWORD X_D3DTEXF_NONE = 0;
constexpr DWORD X_D3DTEXF_POINT = 1;
constexpr DWORD X_D3DTEXF_LINEAR = 2;
constexpr DWORD X_D3DTEXF_ANISOTROPIC = 3;
constexpr DWORD X_D3DTEXF_QUINCUNX = 4; // Xbox extension
constexpr DWORD X_D3DTEXF_GAUSSIANCUBIC = 5;

// X_D3DCLEAR values :
constexpr DWORD X_D3DCLEAR_ZBUFFER = 0x00000001;
constexpr DWORD X_D3DCLEAR_STENCIL = 0x00000002;
constexpr DWORD X_D3DCLEAR_TARGET_R = 0x00000010;  // Clear target surface R component (Xbox ext.)
constexpr DWORD X_D3DCLEAR_TARGET_G = 0x00000020;  // Clear target surface G component (Xbox ext.)
constexpr DWORD X_D3DCLEAR_TARGET_B = 0x00000040;  // Clear target surface B component (Xbox ext.)
constexpr DWORD X_D3DCLEAR_TARGET_A = 0x00000080;  // Clear target surface A component (Xbox ext.)
constexpr DWORD X_D3DCLEAR_TARGET = X_D3DCLEAR_TARGET_R | X_D3DCLEAR_TARGET_G | X_D3DCLEAR_TARGET_B | X_D3DCLEAR_TARGET_A;

// X_D3DCOLORWRITEENABLE values :
constexpr DWORD X_D3DCOLORWRITEENABLE_RED = (1 << 16);
constexpr DWORD X_D3DCOLORWRITEENABLE_GREEN = (1 << 8);
constexpr DWORD X_D3DCOLORWRITEENABLE_BLUE = (1 << 0);
constexpr DWORD X_D3DCOLORWRITEENABLE_ALPHA = (1 << 24);
constexpr DWORD X_D3DCOLORWRITEENABLE_ALL = 0x01010101; // Xbox ext.

// deferred texture stage state "unknown" flag
#define X_D3DTSS_UNK 0x7fffffff

typedef enum _D3DVERTEXBLENDFLAGS
{
	X_D3DVBF_DISABLE = 0,              // 1 matrix,   0 weights => final weight effectively 1 (Disable vertex blending)
	X_D3DVBF_1WEIGHTS = 1,             // 2 matrices, 1 weights => final weight calculated
	X_D3DVBF_2WEIGHTS = 3,             // 3 matrices, 2 weights => final weight calculated
	X_D3DVBF_3WEIGHTS = 5,             // 4 matrices, 3 weights => final weight calculated
	X_D3DVBF_2WEIGHTS2MATRICES = 2,    // 2 matrices, 2 weights (Xbox ext.)
	X_D3DVBF_3WEIGHTS3MATRICES = 4,    // 3 matrices, 3 weights (Xbox ext.)
	X_D3DVBF_4WEIGHTS4MATRICES = 6,    // 4 matrices, 4 weights (Xbox ext.)
	X_D3DVBF_FORCE_DWORD = 0x7fffffff
} X_D3DVERTEXBLENDFLAGS;

typedef DWORD X_VERTEXSHADERCONSTANTMODE;

// Xbox vertex shader constant modes
#define X_D3DSCM_96CONSTANTS                  0x00 // Enables constants 0..95
#define X_D3DSCM_192CONSTANTS                 0x01 // Enables constants -96..-1 on top of 0..95
#define X_D3DSCM_192CONSTANTSANDFIXEDPIPELINE 0x02 // Unsupported?
#define X_D3DSCM_NORESERVEDCONSTANTS          0x10  // Do not reserve constant -38 and -37

#define X_D3DSCM_RESERVED_CONSTANT_SCALE -38 // Becomes 58 after correction, contains Scale v
#define X_D3DSCM_RESERVED_CONSTANT_OFFSET -37 // Becomes 59 after correction, contains Offset

#define X_D3DSCM_CORRECTION                 96 // Add 96 to arrive at the range 0..191 (instead of -96..95)
#define X_D3DVS_CONSTREG_COUNT              192

// Special Registers, used to pass additional information to the shaders
// TODO co-locate shader workaround constants with shader code
#define CXBX_D3DVS_CONSTREG_VREGDEFAULTS_BASE      (X_D3DVS_CONSTREG_COUNT)
#define CXBX_D3DVS_CONSTREG_VREGDEFAULTS_SIZE      16
#define CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE (CXBX_D3DVS_CONSTREG_VREGDEFAULTS_BASE + CXBX_D3DVS_CONSTREG_VREGDEFAULTS_SIZE)
#define CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_SIZE 4
#define CXBX_D3DVS_SCREENSPACE_SCALE_BASE     (CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_BASE + CXBX_D3DVS_CONSTREG_VREGDEFAULTS_FLAG_SIZE)
#define CXBX_D3DVS_NORMALIZE_SCALE_SIZE     1

#define CXBX_D3DVS_SCREENSPACE_OFFSET_BASE     (CXBX_D3DVS_SCREENSPACE_SCALE_BASE + CXBX_D3DVS_NORMALIZE_SCALE_SIZE)
#define CXBX_D3DVS_NORMALIZE_OFFSET_SIZE     1

#define CXBX_D3DVS_TEXTURES_SCALE_BASE             (CXBX_D3DVS_SCREENSPACE_OFFSET_BASE + CXBX_D3DVS_NORMALIZE_OFFSET_SIZE)
#define CXBX_D3DVS_TEXTURES_SCALE_SIZE             4

#define CXBX_D3DVS_CONSTREG_FOGINFO                (CXBX_D3DVS_TEXTURES_SCALE_BASE + CXBX_D3DVS_TEXTURES_SCALE_SIZE)

#define X_D3DSCM_RESERVED_CONSTANT_SCALE_CORRECTED (X_D3DSCM_RESERVED_CONSTANT_SCALE + X_D3DSCM_CORRECTION)
#define X_D3DSCM_RESERVED_CONSTANT_OFFSET_CORRECTED (X_D3DSCM_RESERVED_CONSTANT_OFFSET + X_D3DSCM_CORRECTION)

// Xbox vertex declaration token bit masks
#define X_D3DVSD_MASK_TESSUV 0x10000000
#define X_D3DVSD_MASK_SKIP 0x10000000 // Skips (normally) dwords
#define X_D3DVSD_MASK_SKIPBYTES 0x08000000 // Skips bytes (no, really?!)
#define X_D3DVSD_STREAMTESSMASK (1 << 28)

// Xbox vertex shader types
#define X_VST_NORMAL                  1
#define X_VST_READWRITE               2
#define X_VST_STATE                   3

// Xbox vertex shader counts
#define X_VSH_MAX_ATTRIBUTES          16
#define X_VSH_MAX_STREAMS             16
#define X_VSH_MAX_INSTRUCTION_COUNT   136  // The maximum Xbox shader instruction count
#define X_VSH_INSTRUCTION_SIZE        4
#define X_VSH_INSTRUCTION_SIZE_BYTES  (X_VSH_INSTRUCTION_SIZE * sizeof(DWORD))

// Xbox Vertex Shader versions
#define VERSION_XVS                    0x2078 // 'x ' Xbox vertex shader. Corresponds to X_VST_NORMAL
#define VERSION_XVSS                   0x7378 // 'xs' Xbox vertex state shader. Corresponds to X_VST_STATE
#define VERSION_XVSW                   0x7778 // 'xw' Xbox vertex read/write shader. Corresponds to X_VST_READWRITE

/// nv2a microcode header
typedef struct
{
	uint16_t Version; // See VERSION_XVS*
	uint16_t NumInst;
}
X_VSH_SHADER_HEADER;

// ******************************************************************
// * X_VERTEXSHADERINPUT
// ******************************************************************
typedef struct _X_VERTEXSHADERINPUT
{
    DWORD StreamIndex;
    DWORD Offset;
    DWORD Format;
    BYTE TessellationType;
    BYTE TessellationSource;
	BYTE Padding0;
	BYTE Padding1;
}
X_VERTEXSHADERINPUT;

typedef struct {
	DWORD StreamIndex;
	DWORD Offset;
	DWORD SizeAndType;
	BYTE Flags;
	BYTE Source;
}
X_VertexShaderSlot;

// ******************************************************************
// * X_VERTEXATTRIBUTEFORMAT
// ******************************************************************
typedef struct _X_VERTEXATTRIBUTEFORMAT
{
	// Note : Alignment looks okay even without #pragma pack(1) / #include "AlignPrefix1.h" (and it's closure)
    X_VERTEXSHADERINPUT Slots[X_VSH_MAX_ATTRIBUTES];
}
X_VERTEXATTRIBUTEFORMAT;

// ******************************************************************
// * X_STREAMINPUT
// ******************************************************************
typedef struct _X_STREAMINPUT
{
    X_D3DVertexBuffer  *VertexBuffer;
    UINT                Stride;
    UINT                Offset;
} X_STREAMINPUT;

struct X_D3DVertexShader3948
{
#if 0 
	DWORD Signature; // Note : Debug XBE's have a 'Vshd' DWORD signature prefix
#endif
	DWORD RefCount; // Based on the observation this member is set to 1 in D3DDevice_CreateVertexShader and decreased in D3DDevice_DeleteVertexShader
	DWORD Flags; // Seems to contain at solely the four X_D3DUSAGE_PERSISTENT* flags
	DWORD MaxSlot;
	DWORD TextureCount;
	DWORD ProgramSize;
	DWORD ProgramAndConstantsDwords; // Sum of ProgramSize + constant count, expressed in instruction slots, taking 4 DWORD's per slot (see X_VSH_INSTRUCTION_SIZE)
	DWORD Dimensionality[4] ; // Guesswork, since all 4 bytes (for all 4 textures) are most often set to 0 (or 2 when a texture isn't used) and 1, 3 and 4 also occur (and nothing else)
	X_VERTEXATTRIBUTEFORMAT VertexAttribute;
	X_VertexShaderSlot Slot[4]; // Four more (for a total of 20)
	DWORD ProgramAndConstants[1 /*declare more for debugging purposes */+ X_VSH_MAX_INSTRUCTION_COUNT]; // The binary function data and constants (contents continues futher outside this struct, up to ProgramAndConstantsDwords * 4 (=X_VSH_INSTRUCTION_SIZE) DWORD's)
};

struct X_D3DVertexShader
{
#if 0 
	DWORD Signature; // Note : Debug XBE's have a 'Vshd' DWORD signature prefix
#endif
	DWORD RefCount; // Based on the observation this member is set to 1 in D3DDevice_CreateVertexShader and decreased in D3DDevice_DeleteVertexShader
	DWORD Flags; // Contains X_VERTEXSHADER_FLAG_* bits
	DWORD ProgramSize;
	DWORD ProgramAndConstantsDwords; // Sum of ProgramSize + constant count, expressed in instruction slots, taking 4 DWORD's per slot (see X_VSH_INSTRUCTION_SIZE)
	BYTE Dimensionality[4] ; // Guesswork, since all 4 bytes (for all 4 textures) are most often set to 0 (or 2 when a texture isn't used) and 1, 3 and 4 also occur (and nothing else)
	X_VERTEXATTRIBUTEFORMAT VertexAttribute;
	DWORD ProgramAndConstants[X_VSH_MAX_INSTRUCTION_COUNT]; // The binary function data and constants (contents continues futher outside this struct, up to ProgramAndConstantsDwords * 4 (=X_VSH_INSTRUCTION_SIZE) DWORD's)
};

// X_D3DVertexShader.Flags values :
#define X_VERTEXSHADER_FLAG_WRITE           (1 <<  0) // = 0x0001 // Set for Xbox ShaderType != X_VST_NORMAL 
#define X_VERTEXSHADER_FLAG_PASSTHROUGH     (1 <<  1) // = 0x0002
#define X_VERTEXSHADER_FLAG_STATE           (1 <<  3) // = 0x0008 // Set for Xbox ShaderType == X_VST_STATE
#define X_VERTEXSHADER_FLAG_PROGRAM         (1 <<  4) // = 0x0010 // Set when X_D3DVertexShader was created with assigned function data; introduced after XDK 3948; don't use directly, use g_X_VERTEXSHADER_FLAG_PROGRAM instead
#define X_VERTEXSHADER_FLAG_HASDIFFUSE      (1 << 10) // = 0x0400 Corresponds to X_D3DUSAGE_PERSISTENTDIFFUSE
#define X_VERTEXSHADER_FLAG_HASSPECULAR     (1 << 11) // = 0x0800 Corresponds to X_D3DUSAGE_PERSISTENTSPECULAR
#define X_VERTEXSHADER_FLAG_HASBACKDIFFUSE  (1 << 12) // = 0x1000 Corresponds to X_D3DUSAGE_PERSISTENTBACKDIFFUSE
#define X_VERTEXSHADER_FLAG_HASBACKSPECULAR (1 << 13) // = 0x2000 Corresponds to X_D3DUSAGE_PERSISTENTBACKSPECULAR

// X_D3DVertexShader3948.Flags values, only those which differ from the above :
#define X_VERTEXSHADER3948_FLAG_PROGRAM     (1 <<  2) // = 0x0004 // Test case: Amped, NASCAR Heat 2002; don't use directly, use g_X_VERTEXSHADER_FLAG_PROGRAM instead

// vertex shader input registers for fixed function vertex shader

//          Name                   Register number      D3DFVF
const int X_D3DVSDE_VERTEX       = -1; // Xbox extension for Begin/End drawing - equivalent to POSITION
const int X_D3DVSDE_POSITION     = 0; // Corresponds to X_D3DFVF_XYZ
const int X_D3DVSDE_BLENDWEIGHT  = 1; // Corresponds to X_D3DFVF_XYZB1? (was X_D3DFVF_XYZRHW?)
const int X_D3DVSDE_NORMAL       = 2; // Corresponds to X_D3DFVF_NORMAL
const int X_D3DVSDE_DIFFUSE      = 3; // Corresponds to X_D3DFVF_DIFFUSE
const int X_D3DVSDE_SPECULAR     = 4; // Corresponds to X_D3DFVF_SPECULAR
const int X_D3DVSDE_FOG          = 5; // Xbox extension
const int X_D3DVSDE_POINTSIZE    = 6; // Dxbx addition
const int X_D3DVSDE_BACKDIFFUSE  = 7; // Xbox extension
const int X_D3DVSDE_BACKSPECULAR = 8; // Xbox extension
const int X_D3DVSDE_TEXCOORD0    = 9; // Corresponds to X_D3DFVF_TEX1 (not X_D3DFVF_TEX0, which means no textures are present)
const int X_D3DVSDE_TEXCOORD1    = 10; // Corresponds to X_D3DFVF_TEX2
const int X_D3DVSDE_TEXCOORD2    = 11; // Corresponds to X_D3DFVF_TEX3
const int X_D3DVSDE_TEXCOORD3    = 12; // Corresponds to X_D3DFVF_TEX4

//typedef X_D3DVSDE = X_D3DVSDE_POSITION..High(DWORD)-2; // Unique declaration to make overloads possible;

// bit declarations for _Type fields
const int X_D3DVSDT_FLOAT1      = 0x12; // 1D float expanded to (value, 0.0, 0.0, 1.0)
const int X_D3DVSDT_FLOAT2      = 0x22; // 2D float expanded to (value, value, 0.0, 1.0)
const int X_D3DVSDT_FLOAT3      = 0x32; // 3D float expanded to (value, value, value, 1.0) In double word format this is ARGB, or in byte ordering it would be B, G, R, A.
const int X_D3DVSDT_FLOAT4      = 0x42; // 4D float
const int X_D3DVSDT_D3DCOLOR    = 0x40; // 4D packed unsigned bytes mapped to 0.0 to 1.0 range
//const int X_D3DVSDT_UBYTE4      = 0x05; // 4D unsigned byte   Dxbx note : Not supported on Xbox ?
const int X_D3DVSDT_SHORT2      = 0x25; // 2D signed short expanded to (value, value, 0.0, 1.0)
const int X_D3DVSDT_SHORT4      = 0x45; // 4D signed short

//  Xbox only declarations :
const int X_D3DVSDT_NORMSHORT1  = 0x11; // xbox ext. 1D signed, normalized short expanded to (value, 0.0, 0.0, 1.0). Signed, normalized shorts map from -1.0 to 1.0.
const int X_D3DVSDT_NORMSHORT2  = 0x21; // xbox ext. 2D signed, normalized short expanded to (value, value, 0.0, 1.0). Signed, normalized shorts map from -1.0 to 1.0.
const int X_D3DVSDT_NORMSHORT3  = 0x31; // xbox ext. 3D signed, normalized short expanded to (value, value, value, 1.0). Signed, normalized shorts map from -1.0 to 1.0.
const int X_D3DVSDT_NORMSHORT4  = 0x41; // xbox ext. 4D signed, normalized short expanded to (value, value, value, value). Signed, normalized shorts map from -1.0 to 1.0.
const int X_D3DVSDT_NORMPACKED3 = 0x16; // xbox ext. Three signed, normalized components packed in 32-bits. (11,11,10). Each component ranges from -1.0 to 1.0. Expanded to (value, value, value, 1.0).
const int X_D3DVSDT_SHORT1      = 0x15; // xbox ext. 1D signed short expanded to (value, 0., 0., 1). Signed shorts map to the range [-32768, 32767].
const int X_D3DVSDT_SHORT3      = 0x35; // xbox ext. 3D signed short expanded to (value, value, value, 1). Signed shorts map to the range [-32768, 32767].
const int X_D3DVSDT_PBYTE1      = 0x14; // xbox ext. 1D packed byte expanded to (value, 0., 0., 1). Packed bytes map to the range [0, 1].
const int X_D3DVSDT_PBYTE2      = 0x24; // xbox ext. 2D packed byte expanded to (value, value, 0., 1). Packed bytes map to the range [0, 1].
const int X_D3DVSDT_PBYTE3      = 0x34; // xbox ext. 3D packed byte expanded to (value, value, value, 1). Packed bytes map to the range [0, 1].
const int X_D3DVSDT_PBYTE4      = 0x44; // xbox ext. 4D packed byte expanded to (value, value, value, value). Packed bytes map to the range [0, 1].
const int X_D3DVSDT_FLOAT2H     = 0x72; // xbox ext. 3D float that expands to (value, value, 0.0, value). Useful for projective texture coordinates.
const int X_D3DVSDT_NONE        = 0x02; // xbox ext. nsp

typedef enum _X_D3DVSD_TOKENTYPE
{
	X_D3DVSD_TOKEN_NOP = 0,           // NOP or extension
	X_D3DVSD_TOKEN_STREAM,            // stream selector
	X_D3DVSD_TOKEN_STREAMDATA,        // stream data definition (map to vertex input memory)
	X_D3DVSD_TOKEN_TESSELLATOR,       // vertex input memory from tessellator
	X_D3DVSD_TOKEN_CONSTMEM,          // constant memory from shader
	X_D3DVSD_TOKEN_EXT,               // extension
	X_D3DVSD_TOKEN_END = 7,           // end-of-array (requires all DWORD bits to be 1)
	X_D3DVSD_FORCE_DWORD = 0x7fffffff,// force 32-bit size enum
} X_D3DVSD_TOKENTYPE;

#define X_D3DVSD_TOKENTYPESHIFT   29
#define X_D3DVSD_TOKENTYPEMASK    (7 << X_D3DVSD_TOKENTYPESHIFT)

#define X_D3DVSD_STREAMNUMBERSHIFT 0
#define X_D3DVSD_STREAMNUMBERMASK (0xF << X_D3DVSD_STREAMNUMBERSHIFT)

#define X_D3DVSD_DATALOADTYPESHIFT 28
#define X_D3DVSD_DATALOADTYPEMASK (0x1 << X_D3DVSD_DATALOADTYPESHIFT)

#define X_D3DVSD_DATATYPESHIFT 16
#define X_D3DVSD_DATATYPEMASK (0xFF << X_D3DVSD_DATATYPESHIFT)

#define X_D3DVSD_SKIPCOUNTSHIFT 16
#define X_D3DVSD_SKIPCOUNTMASK (0xF << X_D3DVSD_SKIPCOUNTSHIFT)

#define X_D3DVSD_VERTEXREGSHIFT 0
#define X_D3DVSD_VERTEXREGMASK (0x1F << X_D3DVSD_VERTEXREGSHIFT)

#define X_D3DVSD_VERTEXREGINSHIFT 20
#define X_D3DVSD_VERTEXREGINMASK (0xF << X_D3DVSD_VERTEXREGINSHIFT)

#define X_D3DVSD_CONSTCOUNTSHIFT 25
#define X_D3DVSD_CONSTCOUNTMASK (0xF << X_D3DVSD_CONSTCOUNTSHIFT)

#define X_D3DVSD_CONSTADDRESSSHIFT 0
#define X_D3DVSD_CONSTADDRESSMASK (0xFF << X_D3DVSD_CONSTADDRESSSHIFT)

#define X_D3DVSD_CONSTRSSHIFT 16
#define X_D3DVSD_CONSTRSMASK (0x1FFF << X_D3DVSD_CONSTRSSHIFT)

#define X_D3DVSD_EXTCOUNTSHIFT 24
#define X_D3DVSD_EXTCOUNTMASK (0x1F << X_D3DVSD_EXTCOUNTSHIFT)

#define X_D3DVSD_EXTINFOSHIFT 0
#define X_D3DVSD_EXTINFOMASK (0xFFFFFF << X_D3DVSD_EXTINFOSHIFT)

#define X_D3DVSD_MAKETOKENTYPE(Type) ((Type << X_D3DVSD_TOKENTYPESHIFT) & X_D3DVSD_TOKENTYPEMASK)

#define X_D3DVSD_STREAM(Stream) (X_D3DVSD_MAKETOKENTYPE(X_D3DVSD_TOKEN_STREAM) | (Stream))
#define X_D3DVSD_REG(Reg, Type) (X_D3DVSD_MAKETOKENTYPE(X_D3DVSD_TOKEN_STREAMDATA) | ((Type) << X_D3DVSD_DATATYPESHIFT) | (Reg))

#define X_D3DVSD_NOP() 0x00000000
#define X_D3DVSD_END() 0xFFFFFFFF

// FVF Definitions
#define X_D3DFVF_RESERVED0        0x001
#define X_D3DFVF_XYZ              0x002
#define X_D3DFVF_XYZRHW           0x004
#define X_D3DFVF_XYZB1            0x006
#define X_D3DFVF_XYZB2            0x008
#define X_D3DFVF_XYZB3            0x00a
#define X_D3DFVF_XYZB4            0x00c
#define X_D3DFVF_POSITION_MASK    0x00E
#define X_D3DFVF_NORMAL           0x010
#define X_D3DFVF_RESERVED1        0x020
#define X_D3DFVF_DIFFUSE          0x040
#define X_D3DFVF_SPECULAR         0x080
#define X_D3DFVF_TEXCOUNT_MASK    0xf00
#define X_D3DFVF_TEXCOUNT_SHIFT   8
#define X_D3DFVF_TEX0             0x000
#define X_D3DFVF_TEX1             0x100
#define X_D3DFVF_TEX2             0x200
#define X_D3DFVF_TEX3             0x300
#define X_D3DFVF_TEX4             0x400
#define X_D3DFVF_TEX5             0x500
#define X_D3DFVF_TEX6             0x600
#define X_D3DFVF_TEX7             0x700
#define X_D3DFVF_TEX8             0x800
#define X_D3DFVF_TEXTUREFORMAT1   0x003
#define X_D3DFVF_TEXTUREFORMAT2   0x000
#define X_D3DFVF_TEXTUREFORMAT3   0x001
#define X_D3DFVF_TEXTUREFORMAT4   0x002
#define X_D3DFVF_TEXCOORDSIZE_SHIFT(Index) ((Index) * 2 + 16)
#define X_D3DFVF_TEXCOORDSIZE1(Index) (X_D3DFVF_TEXTUREFORMAT1 << (Index * 2 + 16))
#define X_D3DFVF_TEXCOORDSIZE2(Index) (X_D3DFVF_TEXTUREFORMAT2)
#define X_D3DFVF_TEXCOORDSIZE3(Index) (X_D3DFVF_TEXTUREFORMAT3 << (Index * 2 + 16))
#define X_D3DFVF_TEXCOORDSIZE4(Index) (X_D3DFVF_TEXTUREFORMAT4 << (Index * 2 + 16))

// Values, used with D3DTSS_TEXCOORDINDEX, to specify that the vertex data (position
// and normal in the camera space) should be taken as texture coordinates.
// Low 16 bits are used to specify texture coordinate index, to take the WRAP mode from.
#define X_D3DTSS_TCI_PASSTHRU                             0x00000000
#define X_D3DTSS_TCI_CAMERASPACENORMAL                    0x00010000
#define X_D3DTSS_TCI_CAMERASPACEPOSITION                  0x00020000
#define X_D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR          0x00030000
#define X_D3DTSS_TCI_OBJECT                               0x00040000 // Warning! Collides with host Direct3D 9 D3DTSS_TCI_SPHEREMAP
#define X_D3DTSS_TCI_SPHEREMAP                            0x00050000

enum X_D3DTRANSFORMSTATETYPE {
	X_D3DTS_VIEW = 0,
	X_D3DTS_PROJECTION = 1,
	X_D3DTS_TEXTURE0 = 2,
	X_D3DTS_TEXTURE1 = 3,
	X_D3DTS_TEXTURE2 = 4,
	X_D3DTS_TEXTURE3 = 5,
	X_D3DTS_WORLD = 6,
	X_D3DTS_WORLD1 = 7,
	X_D3DTS_WORLD2 = 8,
	X_D3DTS_WORLD3 = 9,

	X_D3DTS_MAX = 10,
	X_D3DTS_FORCE_DWORD = 0x7FFFFFFF,
};

typedef DWORD NV2AMETHOD;

//
// Below declarations are used by Cxbx, not by the Xbox!!!
//

} // end of namespace xbox

#endif
