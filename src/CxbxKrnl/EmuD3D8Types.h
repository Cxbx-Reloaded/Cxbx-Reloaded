// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8Types.h
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
#ifndef EMUD3D8TYPES_H
#define EMUD3D8TYPES_H

// include direct3d 8x headers
#define DIRECT3D_VERSION 0x0800
#include <d3d8.h>
#include <d3dx8tex.h>
#include <d3d8types.h>

// TODO: fill out these enumeration tables for convienance
typedef DWORD X_D3DFORMAT;
typedef DWORD X_D3DBLENDOP;
typedef DWORD X_D3DBLEND;
typedef DWORD X_D3DCMPFUNC;
typedef DWORD X_D3DFILLMODE;
typedef DWORD X_D3DSHADEMODE;
typedef DWORD X_D3DSTENCILOP;
typedef DWORD X_D3DTEXTURESTAGESTATETYPE;
typedef PVOID X_D3DCALLBACK;

const int X_D3DFMT_L8 = 0x00;
const int X_D3DFMT_AL8 = 0x01;
const int X_D3DFMT_A1R5G5B5 = 0x02;
const int X_D3DFMT_X1R5G5B5 = 0x03;
const int X_D3DFMT_A4R4G4B4 = 0x04;
const int X_D3DFMT_R5G6B5 = 0x05;
const int X_D3DFMT_A8R8G8B8 = 0x06;
const int X_D3DFMT_X8R8G8B8 = 0x07;
const int X_D3DFMT_X8L8V8U8 = 0x07; // Alias

const int X_D3DFMT_P8 = 0x0b; // 8-bit Palletized

const int X_D3DFMT_A8 = 0x19;
const int X_D3DFMT_A8L8 = 0x1a;
const int X_D3DFMT_R6G5B5 = 0x27;
const int X_D3DFMT_L6V5U5 = 0x27; // Alias

const int X_D3DFMT_G8B8 = 0x28;
const int X_D3DFMT_V8U8 = 0x28; // Alias

const int X_D3DFMT_R8B8 = 0x29;
const int X_D3DFMT_D24S8 = 0x2a;
const int X_D3DFMT_F24S8 = 0x2b;
const int X_D3DFMT_D16 = 0x2c;
const int X_D3DFMT_D16_LOCKABLE = 0x2c; // Alias

const int X_D3DFMT_F16 = 0x2d;
const int X_D3DFMT_L16 = 0x32;
const int X_D3DFMT_V16U16 = 0x33;
const int X_D3DFMT_R5G5B5A1 = 0x38;
const int X_D3DFMT_R4G4B4A4 = 0x39;
const int X_D3DFMT_A8B8G8R8 = 0x3A;
const int X_D3DFMT_Q8W8V8U8 = 0x3A; // Alias

const int X_D3DFMT_B8G8R8A8 = 0x3B;
const int X_D3DFMT_R8G8B8A8 = 0x3C;

// YUV Formats

const int X_D3DFMT_YUY2 = 0x24;
const int X_D3DFMT_UYVY = 0x25;

// Compressed Formats

const int X_D3DFMT_DXT1 = 0x0C; // opaque/one-bit alpha

const int X_D3DFMT_DXT2 = 0x0E;
const int X_D3DFMT_DXT3 = 0x0E; // linear alpha

const int X_D3DFMT_DXT4 = 0x0F;
const int X_D3DFMT_DXT5 = 0x0F; // interpolated alpha

								// Linear Formats

const int X_D3DFMT_LIN_A1R5G5B5 = 0x10;
const int X_D3DFMT_LIN_R5G6B5 = 0x11;
const int X_D3DFMT_LIN_A8R8G8B8 = 0x12;
const int X_D3DFMT_LIN_L8 = 0x13;
const int X_D3DFMT_LIN_R8B8 = 0x16;
const int X_D3DFMT_LIN_G8B8 = 0x17;
const int X_D3DFMT_LIN_V8U8 = 0x17; // Alias

const int X_D3DFMT_LIN_AL8 = 0x1b;
const int X_D3DFMT_LIN_X1R5G5B5 = 0x1c;
const int X_D3DFMT_LIN_A4R4G4B4 = 0x1d;
const int X_D3DFMT_LIN_X8R8G8B8 = 0x1e;
const int X_D3DFMT_LIN_X8L8V8U8 = 0x1e; // Alias

const int X_D3DFMT_LIN_A8 = 0x1f;
const int X_D3DFMT_LIN_A8L8 = 0x20;
const int X_D3DFMT_LIN_D24S8 = 0x2E;
const int X_D3DFMT_LIN_F24S8 = 0x2f;
const int X_D3DFMT_LIN_D16 = 0x30;
const int X_D3DFMT_LIN_F16 = 0x31;
const int X_D3DFMT_LIN_L16 = 0x35;
const int X_D3DFMT_LIN_V16U16 = 0x36;
const int X_D3DFMT_LIN_R6G5B5 = 0x37;
const int X_D3DFMT_LIN_L6V5U5 = 0x37; // Alias

const int X_D3DFMT_LIN_R5G5B5A1 = 0x3D;
const int X_D3DFMT_LIN_R4G4B4A4 = 0x3e;
const int X_D3DFMT_LIN_A8B8G8R8 = 0x3f;
const int X_D3DFMT_LIN_B8G8R8A8 = 0x40;
const int X_D3DFMT_LIN_R8G8B8A8 = 0x41;
const int X_D3DFMT_VERTEXDATA = 0x64;

// Primitives supported by draw-primitive API
typedef enum _X_D3DPRIMITIVETYPE
{
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
    X_D3DRTYPE_SURFACE            =  1,
    X_D3DRTYPE_VOLUME             =  2,
    X_D3DRTYPE_TEXTURE            =  3,
    X_D3DRTYPE_VOLUMETEXTURE      =  4,
    X_D3DRTYPE_CUBETEXTURE        =  5,
    X_D3DRTYPE_VERTEXBUFFER       =  6,
    X_D3DRTYPE_INDEXBUFFER        =  7,
    X_D3DRTYPE_PUSHBUFFER         =  8,
    X_D3DRTYPE_PALETTE            =  9,
    X_D3DRTYPE_FIXUP              =  10,

    X_D3DRTYPE_FORCE_DWORD        = 0x7fffffff
}
X_D3DRESOURCETYPE;

typedef enum _X_D3DSET_DEPTH_CLIP_PLANES_FLAGS
{
    X_D3DSDCP_SET_VERTEXPROGRAM_PLANES         = 1,
    X_D3DSDCP_SET_FIXEDFUNCTION_PLANES         = 2,
    X_D3DSDCP_USE_DEFAULT_VERTEXPROGRAM_PLANES = 3,
    X_D3DSDCP_USE_DEFAULT_FIXEDFUNCTION_PLANES = 4,
} 
X_D3DSET_DEPTH_CLIP_PLANES_FLAGS;

typedef struct _X_D3DDISPLAYMODE
{
    UINT        Width;
    UINT        Height;
    UINT        RefreshRate;
    DWORD       Flags;
    X_D3DFORMAT Format;
}
X_D3DDISPLAYMODE;

typedef struct _X_D3DSURFACE_DESC
{
    X_D3DFORMAT         Format;
    X_D3DRESOURCETYPE   Type;
    DWORD               Usage;
    UINT                Size;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    UINT                Width;
    UINT                Height;
}
X_D3DSURFACE_DESC;

typedef struct _X_D3DPRESENT_PARAMETERS
{
    UINT                BackBufferWidth;
    UINT                BackBufferHeight;
    X_D3DFORMAT         BackBufferFormat;
    UINT                BackBufferCount;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    D3DSWAPEFFECT       SwapEffect;
    HWND                hDeviceWindow;
    BOOL                Windowed;
    BOOL                EnableAutoDepthStencil;
    X_D3DFORMAT         AutoDepthStencilFormat;
    DWORD               Flags;
    UINT                FullScreen_RefreshRateInHz;
    UINT                FullScreen_PresentationInterval;
    IDirect3DSurface8  *BufferSurfaces[3];
    IDirect3DSurface8  *DepthStencilSurface;
}
X_D3DPRESENT_PARAMETERS;

typedef struct _X_D3DGAMMARAMP
{
    BYTE    red[256];
    BYTE    green[256];
    BYTE    blue[256];
}
X_D3DGAMMARAMP;

#define X_PIXELSHADER_FAKE_HANDLE 0xDEADBEEF

struct X_D3DVertexShader
{
    union
    {
        DWORD   UnknownA;
        DWORD   Handle;
    };

    DWORD UnknownB;
    DWORD Flags;
    DWORD UnknownC[0x59];
};

typedef struct _X_D3DPIXELSHADERDEF	// <- blueshogun 10/1/07
{
   DWORD    PSAlphaInputs[8];          // Alpha inputs for each stage
   DWORD    PSFinalCombinerInputsABCD; // Final combiner inputs
   DWORD    PSFinalCombinerInputsEFG;  // Final combiner inputs (continued)
   DWORD    PSConstant0[8];            // C0 for each stage
   DWORD    PSConstant1[8];            // C1 for each stage
   DWORD    PSAlphaOutputs[8];         // Alpha output for each stage
   DWORD    PSRGBInputs[8];            // RGB inputs for each stage
   DWORD    PSCompareMode;             // Compare modes for clipplane texture mode
   DWORD    PSFinalCombinerConstant0;  // C0 in final combiner
   DWORD    PSFinalCombinerConstant1;  // C1 in final combiner
   DWORD    PSRGBOutputs[8];           // Stage 0 RGB outputs
   DWORD    PSCombinerCount;           // Active combiner count (Stages 0-7)
   DWORD    PSTextureModes;            // Texture addressing modes
   DWORD    PSDotMapping;              // Input mapping for dot product modes
   DWORD    PSInputTexture;            // Texture source for some texture modes
   DWORD    PSC0Mapping;               // Mapping of c0 regs to D3D constants
   DWORD    PSC1Mapping;               // Mapping of c1 regs to D3D constants
   DWORD    PSFinalCombinerConstants;  // Final combiner constant mapping
}X_D3DPIXELSHADERDEF;

// These structures are used by Cxbx, not by the Xbox!!!
typedef struct _PixelShader_ 
{
	//IDirect3DPixelShader9* pShader;
	//ID3DXConstantTable *pConstantTable;
	DWORD Handle;

	BOOL bBumpEnvMap;

	// constants
	DWORD PSRealC0[8];
	DWORD PSRealC1[8];
	DWORD PSRealFC0;
	DWORD PSRealFC1;

	BOOL bConstantsInitialized;
	BOOL bConstantsChanged;

	DWORD dwStatus;
	X_D3DPIXELSHADERDEF	PSDef;

	DWORD dwStageMap[4];

} PIXEL_SHADER;

typedef struct _STREAM_DYNAMIC_PATCH_
{
    BOOL  NeedPatch;       // This is to know whether is data which must be patched
    DWORD ConvertedStride;
    DWORD NbrTypes;        // Number of the stream data types
    UINT  *pTypes;         // The stream data types (xbox)
} STREAM_DYNAMIC_PATCH;

typedef struct _VERTEX_DYNAMIC_PATCH_
{
    UINT                         NbrStreams; // The number of streams the vertex shader uses
    STREAM_DYNAMIC_PATCH        *pStreamPatches;
} VERTEX_DYNAMIC_PATCH;

typedef struct _VERTEX_SHADER
{
    DWORD Handle;

    // These are the parameters given by the XBE,
    // we save them to be be able to return them when necassary.
    UINT                  Size;
    DWORD                *pDeclaration;
    DWORD                 DeclarationSize;
    DWORD                *pFunction;
    DWORD                 FunctionSize;
    DWORD                 Type;
    DWORD                 Status;

    // Needed for dynamic stream patching
    VERTEX_DYNAMIC_PATCH  VertexDynamicPatch;
} VERTEX_SHADER;

struct X_D3DResource
{
    DWORD Common;
    DWORD Data;

    union
    {
        DWORD                    Lock;
        IDirect3DResource8      *EmuResource8;
        IDirect3DBaseTexture8   *EmuBaseTexture8;
        IDirect3DTexture8       *EmuTexture8;
        IDirect3DVolumeTexture8 *EmuVolumeTexture8;
        IDirect3DCubeTexture8   *EmuCubeTexture8;
        IDirect3DSurface8       *EmuSurface8;
        IDirect3DVertexBuffer8  *EmuVertexBuffer8;
        IDirect3DIndexBuffer8   *EmuIndexBuffer8;
    };
};

// d3d resource "common" masks
#define X_D3DCOMMON_REFCOUNT_MASK      0x0000FFFF
#define X_D3DCOMMON_TYPE_MASK          0x00070000
#define X_D3DCOMMON_TYPE_SHIFT         16
#define X_D3DCOMMON_TYPE_VERTEXBUFFER  0x00000000
#define X_D3DCOMMON_TYPE_INDEXBUFFER   0x00010000
#define X_D3DCOMMON_TYPE_PUSHBUFFER    0x00020000
#define X_D3DCOMMON_TYPE_PALETTE       0x00030000
#define X_D3DCOMMON_TYPE_TEXTURE       0x00040000
#define X_D3DCOMMON_TYPE_SURFACE       0x00050000
#define X_D3DCOMMON_TYPE_FIXUP         0x00060000
#define X_D3DCOMMON_INTREFCOUNT_MASK   0x00780000
#define X_D3DCOMMON_INTREFCOUNT_SHIFT  19
#define X_D3DCOMMON_D3DCREATED         0x01000000
#define X_D3DCOMMON_ISLOCKED           0x02000010 // Surface is currently locked (potential unswizzle candidate)
#define X_D3DCOMMON_UNUSED_MASK        0xFE000000
#define X_D3DCOMMON_UNUSED_SHIFT       25

// special resource data flags (must set _SPECIAL *AND* specific flag(s))
#define X_D3DRESOURCE_DATA_FLAG_SPECIAL 0xFFFF0000
#define X_D3DRESOURCE_DATA_FLAG_SURFACE 0x00000001 // Backbuffer surface, etc
#define X_D3DRESOURCE_DATA_FLAG_YUVSURF 0x00000002 // YUV memory surface
#define X_D3DRESOURCE_DATA_FLAG_D3DREND 0x00000004 // D3D Render Target
#define X_D3DRESOURCE_DATA_FLAG_D3DSTEN 0x00000008 // D3D Stencil Surface
#define X_D3DRESOURCE_DATA_FLAG_TEXCLON 0x00000010 // HACK: Cloned resource
#define X_D3DRESOURCE_DATA_FLAG_8BITTEX 0x00000020 // Palettized texture (D3DFMT_P8)

#define IsSpecialResource(x) ( ((DWORD)(x) & X_D3DRESOURCE_DATA_FLAG_SPECIAL) == X_D3DRESOURCE_DATA_FLAG_SPECIAL)

/*
#define X_D3DRESOURCE_DATA_FLAG_SURFACE 0xEFFFFFFF
#define X_D3DRESOURCE_DATA_FLAG_YUVSURF 0xDFFFFFFF
#define X_D3DRESOURCE_DATA_FLAG_D3DREND 0xCFFFFFFF
#define X_D3DRESOURCE_DATA_FLAG_D3DSTEN 0xBFFFFFFF
*/
// special resource lock flags
#define X_D3DRESOURCE_LOCK_FLAG_NOSIZE  0xEFFFFFFF

// Lock flags
#define X_D3DLOCK_NOFLUSH               0x00000010 // Xbox extension
#define X_D3DLOCK_NOOVERWRITE           0x00000020
#define X_D3DLOCK_TILED                 0x00000040 // Xbox extension
#define X_D3DLOCK_READONLY              0x00000080

const int X_D3DMULTISAMPLE_NONE = 0x0011;
const int X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR = 0x1021;
const int X_D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX = 0x1121;
const int X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR = 0x2021;
const int X_D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR = 0x2012;
const int X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR = 0x1022;
const int X_D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN = 0x1222;
const int X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR = 0x2022;
const int X_D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN = 0x2222;
const int X_D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN = 0x1233;
const int X_D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN = 0x2233;

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
    X_D3DFORMAT Format;
    DWORD       Size;
};

// pixel container "format" masks
#define X_D3DFORMAT_RESERVED1_MASK        0x00000003      // Must be zero
#define X_D3DFORMAT_DMACHANNEL_MASK       0x00000003
#define X_D3DFORMAT_DMACHANNEL_A          0x00000001      // DMA channel A - the default for all system memory
#define X_D3DFORMAT_DMACHANNEL_B          0x00000002      // DMA channel B - unused
#define X_D3DFORMAT_CUBEMAP               0x00000004      // Set if the texture if a cube map
#define X_D3DFORMAT_BORDERSOURCE_COLOR    0x00000008
#define X_D3DFORMAT_DIMENSION_MASK        0x000000F0      // # of dimensions
#define X_D3DFORMAT_DIMENSION_SHIFT       4
#define X_D3DFORMAT_FORMAT_MASK           0x0000FF00
#define X_D3DFORMAT_FORMAT_SHIFT          8
#define X_D3DFORMAT_MIPMAP_MASK           0x000F0000
#define X_D3DFORMAT_MIPMAP_SHIFT          16
#define X_D3DFORMAT_USIZE_MASK            0x00F00000      // Log 2 of the U size of the base texture
#define X_D3DFORMAT_USIZE_SHIFT           20
#define X_D3DFORMAT_VSIZE_MASK            0x0F000000      // Log 2 of the V size of the base texture
#define X_D3DFORMAT_VSIZE_SHIFT           24
#define X_D3DFORMAT_PSIZE_MASK            0xF0000000      // Log 2 of the P size of the base texture
#define X_D3DFORMAT_PSIZE_SHIFT           28

// pixel container "size" masks
#define X_D3DSIZE_WIDTH_MASK              0x00000FFF   // Width  (Texels - 1)
#define X_D3DSIZE_HEIGHT_MASK             0x00FFF000   // Height (Texels - 1)
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

struct X_D3DSurface : public X_D3DPixelContainer
{

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
}X_D3DCALLBACKTYPE;

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
D3DVBLANKDATA;

typedef struct _D3DSWAPDATA 
{
    DWORD           Swap;
    DWORD           SwapVBlank;
    DWORD           MissedVBlanks;
    DWORD           TimeUntilSwapVBlank;
    DWORD           TimeBetweenSwapVBlanks;
} 
D3DSWAPDATA;

// D3DVBLANKCALLBACK
typedef void (__cdecl * D3DVBLANKCALLBACK)(D3DVBLANKDATA *pData);

// D3DSWAPCALLBACK
typedef void (__cdecl * D3DSWAPCALLBACK)(D3DSWAPDATA *pData);

// D3DCALLBACK
typedef void (__cdecl * D3DCALLBACK)(DWORD Context);

// X_D3DTEXTUREOP values :
#define X_D3DTOP_DISABLE 1
#define X_D3DTOP_SELECTARG1 2
#define X_D3DTOP_SELECTARG2 3
#define X_D3DTOP_MODULATE  4
#define X_D3DTOP_MODULATE2X  5
#define X_D3DTOP_MODULATE4X  6
#define X_D3DTOP_ADD  7
#define X_D3DTOP_ADDSIGNED  8
#define X_D3DTOP_ADDSIGNED2X  9
#define X_D3DTOP_SUBTRACT  10
#define X_D3DTOP_ADDSMOOTH  11
#define X_D3DTOP_BLENDDIFFUSEALPHA  12
#define X_D3DTOP_BLENDCURRENTALPHA  13
#define X_D3DTOP_BLENDTEXTUREALPHA  14
#define X_D3DTOP_BLENDFACTORALPHA  15
#define X_D3DTOP_BLENDTEXTUREALPHAPM  16
#define X_D3DTOP_PREMODULATE  17
#define X_D3DTOP_MODULATEALPHA_ADDCOLOR  18
#define X_D3DTOP_MODULATECOLOR_ADDALPHA  19
#define X_D3DTOP_MODULATEINVALPHA_ADDCOLOR  20
#define X_D3DTOP_MODULATEINVCOLOR_ADDALPHA  21
#define X_D3DTOP_DOTPRODUCT3  22
#define X_D3DTOP_MULTIPLYADD  23
#define X_D3DTOP_LERP  24
#define X_D3DTOP_BUMPENVMAP  25
#define X_D3DTOP_BUMPENVMAPLUMINANCE  26

// deferred render state "unknown" flag
#define X_D3DRS_UNK  0x7fffffff

// deferred texture stage state "unknown" flag
#define X_D3DTSS_UNK 0x7fffffff

typedef DWORD X_VERTEXSHADERCONSTANTMODE;

#define X_VSCM_96                     0
#define X_VSCM_192                    1
#define X_VSCM_192FIXEDPIPELINE       2
#define X_VSCM_NONERESERVED          16

// Vertex shader types
#define X_VST_NORMAL                  1
#define X_VST_READWRITE               2
#define X_VST_STATE                   3

// ******************************************************************
// * X_VERTEXSHADERINPUT
// ******************************************************************
typedef struct _X_VERTEXSHADERINPUT
{
    DWORD IndexOfStream;
    DWORD Offset;
    DWORD Format;
    BYTE  TesselationType;
    BYTE  TesselationSource;
}
X_VERTEXSHADERINPUT;

// ******************************************************************
// * X_VERTEXATTRIBUTEFORMAT
// ******************************************************************
typedef struct _X_VERTEXATTRIBUTEFORMAT
{
    X_VERTEXSHADERINPUT pVertexShaderInput[16];
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

#endif
