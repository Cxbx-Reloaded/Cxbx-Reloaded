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

#ifndef XBD3D8TYPES_ENUMS_H
#define XBD3D8TYPES_ENUMS_H

#include "XbD3D8Types_D3D9Compat.h"

namespace xbox {

// Xbox D3D types, probably just copies of the Windows Direct3D 9 counterparts :

typedef ::HWND X_HWND;
typedef ::RECT X_RECT;
typedef ::POINT X_POINT;
typedef ::DWORD X_D3DPOOL; // Note : Seems to be ignored in all XDK API's

/* Back buffer types */
typedef enum _X_D3DBACKBUFFER_TYPE
{
	X_D3DBACKBUFFER_TYPE_MONO = 0,
	X_D3DBACKBUFFER_TYPE_LEFT = 1, // TODO : Remove, likely not present on Xbox
	X_D3DBACKBUFFER_TYPE_RIGHT = 2, // TODO : Remove, likely not present on Xbox

	X_D3DBACKBUFFER_TYPE_FORCE_DWORD = 0x7fffffff
} X_D3DBACKBUFFER_TYPE;

typedef enum _X_D3DBASISTYPE { // Based on Direct3D 9 TODO : verify on Xbox
	D3DBASIS_BEZIER = 0,
	D3DBASIS_BSPLINE = 1,
	D3DBASIS_CATMULL_ROM = 2,
	D3DBASIS_FORCE_DWORD = 0x7fffffff
} X_D3DBASISTYPE, *LPX_D3DBASISTYPE;

#ifdef CXBX_USE_D3D11
typedef DWORD X_D3DCOLOR;
#else
typedef D3DCOLOR X_D3DCOLOR;
#endif

/* CubeMap Face identifiers */
typedef enum _X_D3DCUBEMAP_FACES
{
	X_D3DCUBEMAP_FACE_POSITIVE_X = 0,
	X_D3DCUBEMAP_FACE_NEGATIVE_X = 1,
	X_D3DCUBEMAP_FACE_POSITIVE_Y = 2,
	X_D3DCUBEMAP_FACE_NEGATIVE_Y = 3,
	X_D3DCUBEMAP_FACE_POSITIVE_Z = 4,
	X_D3DCUBEMAP_FACE_NEGATIVE_Z = 5,

	X_D3DCUBEMAP_FACE_FORCE_DWORD = 0x7fffffff
} X_D3DCUBEMAP_FACES;

typedef enum _X_D3DDEGREETYPE { // Based on Direct3D 9 TODO : verify on Xbox
	D3DDEGREE_LINEAR = 1,
	D3DDEGREE_QUADRATIC = 2,
	D3DDEGREE_CUBIC = 3,
	D3DDEGREE_QUINTIC = 5,
	D3DCULL_FORCE_DWORD = 0x7fffffff
} X_D3DDEGREETYPE, *LPX_D3DDEGREETYPE;

typedef enum _X_D3DDEVTYPE
{
	X_D3DDEVTYPE_HAL = 1,
	X_D3DDEVTYPE_REF = 2,
	X_D3DDEVTYPE_SW = 3,

	X_D3DDEVTYPE_NULLREF = 4, // TODO : Remove, likely not present on Xbox

	X_D3DDEVTYPE_FORCE_DWORD = 0x7fffffff
} X_D3DDEVTYPE;

/* Creation Parameters */
typedef struct _X_D3DDEVICE_CREATION_PARAMETERS
{
	UINT            AdapterOrdinal;
	X_D3DDEVTYPE    DeviceType;
	X_HWND          hFocusWindow;
	DWORD           BehaviorFlags;
} X_D3DDEVICE_CREATION_PARAMETERS;

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DVECTOR {
	float x;
	float y;
	float z;
	_X_D3DVECTOR& operator=(const ::D3DXVECTOR3& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; }
} X_D3DVECTOR;
#else
typedef D3DVECTOR X_D3DVECTOR;
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DBOX {
	UINT Left;
	UINT Top;
	UINT Right;
	UINT Bottom;
	UINT Front;
	UINT Back;
} X_D3DBOX;
#else
typedef D3DBOX X_D3DBOX;
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DLOCKED_BOX {
	int  RowPitch;
	int  SlicePitch;
	void* pBits;
} X_D3DLOCKED_BOX;
#else
typedef D3DLOCKED_BOX X_D3DLOCKED_BOX;
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DLOCKED_RECT {
	INT   Pitch;
	void* pBits;
} X_D3DLOCKED_RECT;
#else
typedef D3DLOCKED_RECT X_D3DLOCKED_RECT;
#endif

// Host-type aliases for D3D11 mode (used in trampoline function signatures)
#ifdef CXBX_USE_D3D11
typedef X_D3DLOCKED_RECT D3DLOCKED_RECT;
typedef X_D3DLOCKED_BOX D3DLOCKED_BOX;
typedef X_D3DBOX D3DBOX;
#endif

#ifdef CXBX_USE_D3D11
typedef XMMATRIX X_D3DMATRIX; // XMMATRIX is the storage type; CXMMATRIX is only for parameter passing
#else
typedef D3DXMATRIX X_D3DMATRIX; // TODO : Or D3DMATRIX?
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DRECT {
	LONG x1;
	LONG y1;
	LONG x2;
	LONG y2;
} X_D3DRECT;
#else
typedef D3DRECT X_D3DRECT;
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DRECTPATCH_INFO { // Based on Direct3D 9 TODO : verify on Xbox
	UINT          StartVertexOffsetWidth;
	UINT          StartVertexOffsetHeight;
	UINT          Width;
	UINT          Height;
	UINT          Stride;
	X_D3DBASISTYPE  Basis;
	X_D3DDEGREETYPE Degree;
} X_D3DRECTPATCH_INFO, *LPX_D3DRECTPATCH_INFO;
#else
typedef D3DRECTPATCH_INFO X_D3DRECTPATCH_INFO;
#endif

#ifdef CXBX_USE_D3D11
typedef struct _X_D3DTRIPATCH_INFO { // Based on Direct3D 9 TODO : verify on Xbox
	UINT          StartVertexOffset;
	UINT          NumVertices;
	X_D3DBASISTYPE  Basis;
	X_D3DDEGREETYPE Degree;
} X_D3DTRIPATCH_INFO, *LPX_D3DTRIPATCH_INFO;
#else
typedef D3DTRIPATCH_INFO X_D3DTRIPATCH_INFO;
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

typedef enum _X_D3DSTATEBLOCKTYPE { // Based on Direct3D 9 TODO : verify on Xbox
	D3DSBT_ALL = 1,
	D3DSBT_PIXELSTATE = 2,
	D3DSBT_VERTEXSTATE = 3,
	D3DSBT_FORCE_DWORD = 0x7fffffff
} X_D3DSTATEBLOCKTYPE;

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

	X_D3DFMT_FIRST = X_D3DFMT_L8, // Alias
	X_D3DFMT_LAST = X_D3DFMT_LIN_R8G8B8A8, // Alias

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
	X_HWND              hDeviceWindow;
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

} // end of namespace xbox

#endif // XBD3D8TYPES_ENUMS_H
