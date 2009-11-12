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
typedef DWORD X_D3DTEXTURESTAGESTATETYPE;
typedef PVOID X_D3DCALLBACK;

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
	X_D3DCALLBACK_READ		= 1,
	X_D3DCALLBACK_WRITE		= 2
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

// D3DVBLANKCALLBACK
typedef void (__cdecl * D3DVBLANKCALLBACK)(D3DVBLANKDATA *pData);

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
