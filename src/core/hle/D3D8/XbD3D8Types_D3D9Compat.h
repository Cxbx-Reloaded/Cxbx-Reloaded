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

#ifndef XBD3D8TYPES_D3D9COMPAT_H
#define XBD3D8TYPES_D3D9COMPAT_H

#undef UNICODE // make sure dxerr.h DXGetErrorString is aliassed to *A, not *W

#include "xbox_types.h"

#ifdef CXBX_USE_D3D11
// include direct3d 11 headers
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h> // XMVECTORF32
using namespace DirectX;
typedef DWORD D3DCOLOR;
typedef D3D11_RECT D3DRECT;

// Pre-define D3DVECTOR so dsound.h (which is included inside namespace xbox
// in DirectSound.hpp) doesn't redefine it and cause type conflicts.
#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
	float x;
	float y;
	float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

// D3D9 compatibility types and constants
// These reproduce D3D9 enum values so code that converts Xbox values
// to host D3D9 intermediate values can compile unchanged. In D3D11
// mode, these intermediate values get re-mapped to D3D11 state objects
// by RenderStates.cpp and TextureStates.cpp.

typedef struct _D3DMATRIX {
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
} D3DMATRIX;

typedef DWORD D3DBACKBUFFER_TYPE;
#define D3DBACKBUFFER_TYPE_MONO 0

typedef DWORD D3DDEVTYPE;
typedef DWORD D3DFORMAT; // Unused in D3D11 mode (EMUFORMAT maps to DXGI_FORMAT)
typedef DWORD D3DMULTISAMPLE_TYPE;
typedef DWORD D3DSWAPEFFECT;

typedef enum _D3DCUBEMAP_FACES {
	D3DCUBEMAP_FACE_POSITIVE_X = 0,
	D3DCUBEMAP_FACE_NEGATIVE_X = 1,
	D3DCUBEMAP_FACE_POSITIVE_Y = 2,
	D3DCUBEMAP_FACE_NEGATIVE_Y = 3,
	D3DCUBEMAP_FACE_POSITIVE_Z = 4,
	D3DCUBEMAP_FACE_NEGATIVE_Z = 5,
} D3DCUBEMAP_FACES;

// Texture transform flags
#define D3DTTFF_PROJECTED 256

// Blend operations
typedef enum _D3DBLENDOP {
	D3DBLENDOP_ADD         = 1,
	D3DBLENDOP_SUBTRACT    = 2,
	D3DBLENDOP_REVSUBTRACT = 3,
	D3DBLENDOP_MIN         = 4,
	D3DBLENDOP_MAX         = 5,
} D3DBLENDOP;

// Blend factors
typedef enum _D3DBLEND {
	D3DBLEND_ZERO            = 1,
	D3DBLEND_ONE             = 2,
	D3DBLEND_SRCCOLOR        = 3,
	D3DBLEND_INVSRCCOLOR     = 4,
	D3DBLEND_SRCALPHA        = 5,
	D3DBLEND_INVSRCALPHA     = 6,
	D3DBLEND_DESTALPHA       = 7,
	D3DBLEND_INVDESTALPHA    = 8,
	D3DBLEND_DESTCOLOR       = 9,
	D3DBLEND_INVDESTCOLOR    = 10,
	D3DBLEND_SRCALPHASAT     = 11,
	D3DBLEND_BLENDFACTOR     = 14,
	D3DBLEND_INVBLENDFACTOR  = 15,
} D3DBLEND;

// Comparison functions
typedef enum _D3DCMPFUNC {
	D3DCMP_NEVER        = 1,
	D3DCMP_LESS         = 2,
	D3DCMP_EQUAL        = 3,
	D3DCMP_LESSEQUAL    = 4,
	D3DCMP_GREATER      = 5,
	D3DCMP_NOTEQUAL     = 6,
	D3DCMP_GREATEREQUAL = 7,
	D3DCMP_ALWAYS       = 8,
} D3DCMPFUNC;

// Fill modes
typedef enum _D3DFILLMODE {
	D3DFILL_POINT     = 1,
	D3DFILL_WIREFRAME = 2,
	D3DFILL_SOLID     = 3,
} D3DFILLMODE;

// Shade modes
typedef enum _D3DSHADEMODE {
	D3DSHADE_FLAT    = 1,
	D3DSHADE_GOURAUD = 2,
} D3DSHADEMODE;

// Stencil operations
typedef enum _D3DSTENCILOP {
	D3DSTENCILOP_KEEP    = 1,
	D3DSTENCILOP_ZERO    = 2,
	D3DSTENCILOP_REPLACE = 3,
	D3DSTENCILOP_INCRSAT = 4,
	D3DSTENCILOP_DECRSAT = 5,
	D3DSTENCILOP_INVERT  = 6,
	D3DSTENCILOP_INCR    = 7,
	D3DSTENCILOP_DECR    = 8,
} D3DSTENCILOP;

// Primitive types
typedef enum _D3DPRIMITIVETYPE {
	D3DPT_POINTLIST     = 1,
	D3DPT_LINELIST      = 2,
	D3DPT_LINESTRIP     = 3,
	D3DPT_TRIANGLELIST  = 4,
	D3DPT_TRIANGLESTRIP = 5,
	D3DPT_TRIANGLEFAN   = 6,
	D3DPT_FORCE_DWORD   = 0x7fffffff,
} D3DPRIMITIVETYPE;

// Transform state types
typedef enum _D3DTRANSFORMSTATETYPE {
	D3DTS_VIEW         = 2,
	D3DTS_PROJECTION   = 3,
	D3DTS_TEXTURE0     = 16,
	D3DTS_TEXTURE1     = 17,
	D3DTS_TEXTURE2     = 18,
	D3DTS_TEXTURE3     = 19,
	D3DTS_TEXTURE4     = 20,
	D3DTS_TEXTURE5     = 21,
	D3DTS_TEXTURE6     = 22,
	D3DTS_TEXTURE7     = 23,
	D3DTS_FORCE_DWORD  = 0x7fffffff,
} D3DTRANSFORMSTATETYPE;

// Texture stage state types
typedef enum _D3DTEXTURESTAGESTATETYPE {
	D3DTSS_COLOROP                 = 1,
	D3DTSS_COLORARG1               = 2,
	D3DTSS_COLORARG2               = 3,
	D3DTSS_ALPHAOP                 = 4,
	D3DTSS_ALPHAARG1               = 5,
	D3DTSS_ALPHAARG2               = 6,
	D3DTSS_BUMPENVMAT00            = 7,
	D3DTSS_BUMPENVMAT01            = 8,
	D3DTSS_BUMPENVMAT10            = 9,
	D3DTSS_BUMPENVMAT11            = 10,
	D3DTSS_TEXCOORDINDEX           = 11,
	D3DTSS_BUMPENVLSCALE           = 22,
	D3DTSS_BUMPENVLOFFSET          = 23,
	D3DTSS_TEXTURETRANSFORMFLAGS   = 24,
	D3DTSS_COLORARG0               = 26,
	D3DTSS_ALPHAARG0               = 27,
	D3DTSS_RESULTARG               = 28,
} D3DTEXTURESTAGESTATETYPE;

// Sampler state types
typedef enum _D3DSAMPLERSTATETYPE {
	D3DSAMP_ADDRESSU      = 1,
	D3DSAMP_ADDRESSV      = 2,
	D3DSAMP_ADDRESSW      = 3,
	D3DSAMP_BORDERCOLOR   = 4,
	D3DSAMP_MAGFILTER     = 5,
	D3DSAMP_MINFILTER     = 6,
	D3DSAMP_MIPFILTER     = 7,
	D3DSAMP_MIPMAPLODBIAS = 8,
	D3DSAMP_MAXMIPLEVEL   = 9,
	D3DSAMP_MAXANISOTROPY = 10,
} D3DSAMPLERSTATETYPE;

// Texture operations (fixed-function texture combine stages)
typedef enum _D3DTEXTUREOP {
	D3DTOP_DISABLE                   = 1,
	D3DTOP_SELECTARG1                = 2,
	D3DTOP_SELECTARG2                = 3,
	D3DTOP_MODULATE                  = 4,
	D3DTOP_MODULATE2X                = 5,
	D3DTOP_MODULATE4X                = 6,
	D3DTOP_ADD                       = 7,
	D3DTOP_ADDSIGNED                 = 8,
	D3DTOP_ADDSIGNED2X               = 9,
	D3DTOP_SUBTRACT                  = 10,
	D3DTOP_ADDSMOOTH                 = 11,
	D3DTOP_BLENDDIFFUSEALPHA         = 12,
	D3DTOP_BLENDTEXTUREALPHA         = 13,
	D3DTOP_BLENDFACTORALPHA          = 14,
	D3DTOP_BLENDTEXTUREALPHAPM       = 15,
	D3DTOP_BLENDCURRENTALPHA         = 16,
	D3DTOP_PREMODULATE               = 17,
	D3DTOP_MODULATEALPHA_ADDCOLOR    = 18,
	D3DTOP_MODULATECOLOR_ADDALPHA    = 19,
	D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20,
	D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
	D3DTOP_BUMPENVMAP                = 22,
	D3DTOP_BUMPENVMAPLUMINANCE       = 23,
	D3DTOP_DOTPRODUCT3               = 24,
	D3DTOP_MULTIPLYADD               = 25,
	D3DTOP_LERP                      = 26,
} D3DTEXTUREOP;

// Texture argument constants
#define D3DTA_SELECTMASK   0x0000000f
#define D3DTA_DIFFUSE      0x00000000
#define D3DTA_CURRENT      0x00000001
#define D3DTA_TEXTURE      0x00000002
#define D3DTA_TFACTOR      0x00000003
#define D3DTA_SPECULAR     0x00000004
#define D3DTA_TEMP         0x00000005
#define D3DTA_COMPLEMENT   0x00000010
#define D3DTA_ALPHAREPLICATE 0x00000020

// Render state type
typedef DWORD D3DRENDERSTATETYPE;

// Render state values (D3D9 enum constants, used as intermediate host mappings)
#define D3DRS_ZENABLE                   7
#define D3DRS_FILLMODE                  8
#define D3DRS_SHADEMODE                 9
#define D3DRS_ZWRITEENABLE              14
#define D3DRS_ALPHATESTENABLE           15
#define D3DRS_LASTPIXEL                 16
#define D3DRS_SRCBLEND                  19
#define D3DRS_DESTBLEND                 20
#define D3DRS_CULLMODE                  22
#define D3DRS_ZFUNC                     23
#define D3DRS_ALPHAREF                  24
#define D3DRS_ALPHAFUNC                 25
#define D3DRS_DITHERENABLE              26
#define D3DRS_ALPHABLENDENABLE          27
#define D3DRS_FOGENABLE                 28
#define D3DRS_SPECULARENABLE            29
#define D3DRS_FOGCOLOR                  34
#define D3DRS_FOGTABLEMODE              35
#define D3DRS_FOGSTART                  36
#define D3DRS_FOGEND                    37
#define D3DRS_FOGDENSITY                38
#define D3DRS_RANGEFOGENABLE            48
#define D3DRS_STENCILENABLE             52
#define D3DRS_STENCILFAIL               53
#define D3DRS_STENCILZFAIL              54
#define D3DRS_STENCILPASS               55
#define D3DRS_STENCILFUNC               56
#define D3DRS_STENCILREF                57
#define D3DRS_STENCILMASK               58
#define D3DRS_STENCILWRITEMASK          59
#define D3DRS_TEXTUREFACTOR             60
#define D3DRS_WRAP0                     128
#define D3DRS_WRAP1                     129
#define D3DRS_WRAP2                     130
#define D3DRS_WRAP3                     131
#define D3DRS_WRAP4                     132
#define D3DRS_WRAP5                     133
#define D3DRS_WRAP6                     134
#define D3DRS_WRAP7                     135
#define D3DRS_CLIPPING                  136
#define D3DRS_LIGHTING                  137
#define D3DRS_AMBIENT                   139
#define D3DRS_FOGVERTEXMODE             140
#define D3DRS_COLORVERTEX               141
#define D3DRS_LOCALVIEWER               142
#define D3DRS_NORMALIZENORMALS          143
#define D3DRS_DIFFUSEMATERIALSOURCE     145
#define D3DRS_SPECULARMATERIALSOURCE    146
#define D3DRS_AMBIENTMATERIALSOURCE     147
#define D3DRS_EMISSIVEMATERIALSOURCE    148
#define D3DRS_VERTEXBLEND               151
#define D3DRS_CLIPPLANEENABLE           152
#define D3DRS_POINTSIZE                 154
#define D3DRS_POINTSIZE_MIN             155
#define D3DRS_POINTSPRITEENABLE         156
#define D3DRS_POINTSCALEENABLE          157
#define D3DRS_POINTSCALE_A              158
#define D3DRS_POINTSCALE_B              159
#define D3DRS_POINTSCALE_C              160
#define D3DRS_MULTISAMPLEANTIALIAS      161
#define D3DRS_MULTISAMPLEMASK           162
#define D3DRS_PATCHEDGESTYLE            163
#define D3DRS_DEBUGMONITORTOKEN         165
#define D3DRS_POINTSIZE_MAX             166
#define D3DRS_INDEXEDVERTEXBLENDENABLE  167
#define D3DRS_COLORWRITEENABLE          168
#define D3DRS_TWEENFACTOR               170
#define D3DRS_BLENDOP                   171
#define D3DRS_POSITIONORDER             172 // D3DORDERTYPE for N-patches
#define D3DRS_NORMALORDER               173
#define D3DRS_DEPTHBIAS                 195
#define D3DRS_WRAP8                     198
#define D3DRS_WRAP9                     199
#define D3DRS_WRAP10                    200
#define D3DRS_WRAP11                    201
#define D3DRS_WRAP12                    202
#define D3DRS_WRAP13                    203
#define D3DRS_WRAP14                    204
#define D3DRS_WRAP15                    205
#define D3DRS_SEPARATEALPHABLENDENABLE  206
#define D3DRS_SRCBLENDALPHA             207
#define D3DRS_DESTBLENDALPHA            208
#define D3DRS_BLENDOPALPHA              209
#define D3DRS_LINEPATTERN               10 // removed in D3D9
#define D3DRS_ZVISIBLE                  30 // removed in D3D9
#define D3DRS_ANTIALIASEDLINEENABLE     176
#define D3DRS_MINTESSELLATIONLEVEL      178
#define D3DRS_MAXTESSELLATIONLEVEL      179
#define D3DRS_ADAPTIVETESS_X            184
#define D3DRS_ADAPTIVETESS_Y            185
#define D3DRS_ADAPTIVETESS_Z            186
#define D3DRS_ADAPTIVETESS_W            187
#define D3DRS_ENABLEADAPTIVETESSELLATION 188
#define D3DRS_TWOSIDEDSTENCILMODE       185
#define D3DRS_CCW_STENCILFAIL           186
#define D3DRS_CCW_STENCILZFAIL          187
#define D3DRS_CCW_STENCILPASS           188
#define D3DRS_CCW_STENCILFUNC           189
#define D3DRS_COLORWRITEENABLE1         190
#define D3DRS_COLORWRITEENABLE2         191
#define D3DRS_COLORWRITEENABLE3         192
#define D3DRS_BLENDFACTOR               193
#define D3DRS_SRGBWRITEENABLE           194
#define D3DRS_SLOPESCALEDEPTHBIAS       175
#define D3DRS_SCISSORTESTENABLE         174
#define D3DRS_SOFTWAREVERTEXPROCESSING  153
#define D3DRS_SEPARATEDESTALPHAENABLE   210 // Non-standard, used by Cxbx

// Resource types
typedef enum _D3DRESOURCETYPE {
	D3DRTYPE_SURFACE       = 1,
	D3DRTYPE_VOLUME        = 2,
	D3DRTYPE_TEXTURE       = 3,
	D3DRTYPE_VOLUMETEXTURE = 4,
	D3DRTYPE_CUBETEXTURE   = 5,
	D3DRTYPE_VERTEXBUFFER  = 6,
	D3DRTYPE_INDEXBUFFER   = 7,
} D3DRESOURCETYPE;

// Resource pool
typedef enum _D3DPOOL {
	D3DPOOL_DEFAULT   = 0,
	D3DPOOL_MANAGED   = 1,
	D3DPOOL_SYSTEMMEM = 2,
} D3DPOOL;

// Resource usage flags
#define D3DUSAGE_RENDERTARGET 0x00000001
#define D3DUSAGE_DEPTHSTENCIL 0x00000002
#define D3DUSAGE_DYNAMIC      0x00000200

// Lock flags
#define D3DLOCK_READONLY  0x00000010
#define D3DLOCK_DISCARD   0x00002000
#define D3DLOCK_NOOVERWRITE 0x00001000
#define D3DLOCK_NOSYSLOCK 0x00000800

// Display mode (for DlgVideoConfig)
typedef struct _D3DDISPLAYMODE {
	UINT Width;
	UINT Height;
	UINT RefreshRate;
	DXGI_FORMAT Format;
} D3DDISPLAYMODE;

// Adapter identifier (stub for DlgVideoConfig)
typedef struct _D3DADAPTER_IDENTIFIER9 {
	char Driver[512];
	char Description[512];
	char DeviceName[32];
	LARGE_INTEGER DriverVersion;
	DWORD VendorId;
	DWORD DeviceId;
	DWORD SubSysId;
	DWORD Revision;
	GUID DeviceIdentifier;
	DWORD WHQLLevel;
} D3DADAPTER_IDENTIFIER;

// Device caps (stub for DlgVideoConfig)
typedef struct _D3DCAPS9 {
	DWORD Caps;
	// Only the fields needed - add more as required
} D3DCAPS;

// Texture filter types
#define D3DTEXF_NONE           0
#define D3DTEXF_POINT          1
#define D3DTEXF_LINEAR         2
#define D3DTEXF_ANISOTROPIC    3
#define D3DTEXF_PYRAMIDALQUAD  6
#define D3DTEXF_GAUSSIANQUAD   7

// Texture address modes
#define D3DTADDRESS_WRAP       1
#define D3DTADDRESS_MIRROR     2
#define D3DTADDRESS_CLAMP      3
#define D3DTADDRESS_BORDER     4
#define D3DTADDRESS_MIRRORONCE 5

// Texture coordinate index flags
#define D3DTSS_TCI_SPHEREMAP   0x00040000

// Device type constants (for DlgVideoConfig)
#define D3DDEVTYPE_HAL  1
#define D3DDEVTYPE_REF  2

// D3D_OK and D3D error codes
#define D3D_OK S_OK
#define _FACD3D 0x876
#define MAKE_D3DHRESULT(code) MAKE_HRESULT(1, _FACD3D, code)
#define D3DERR_INVALIDCALL              MAKE_D3DHRESULT(2156)
#define D3DERR_NOTAVAILABLE             MAKE_D3DHRESULT(2154)
#define D3DERR_WRONGTEXTUREFORMAT       MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION MAKE_D3DHRESULT(2073)
#define D3DERR_UNSUPPORTEDCOLORARG      MAKE_D3DHRESULT(2074)
#define D3DERR_UNSUPPORTEDALPHAOPERATION MAKE_D3DHRESULT(2075)
#define D3DERR_UNSUPPORTEDALPHAARG      MAKE_D3DHRESULT(2076)
#define D3DERR_TOOMANYOPERATIONS        MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGTEXTUREFILTER MAKE_D3DHRESULT(2078)
#define D3DERR_UNSUPPORTEDFACTORVALUE   MAKE_D3DHRESULT(2079)
#define D3DERR_CONFLICTINGRENDERSTATE   MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER MAKE_D3DHRESULT(2082)
#define D3DERR_CONFLICTINGTEXTUREPALETTE MAKE_D3DHRESULT(2086)

// Clear flags
#define D3DCLEAR_TARGET  0x00000001
#define D3DCLEAR_ZBUFFER 0x00000002
#define D3DCLEAR_STENCIL 0x00000004

// Cull modes (host D3D9 intermediate values)
typedef enum _D3DCULL {
	D3DCULL_NONE = 1,
	D3DCULL_CW   = 2,
	D3DCULL_CCW  = 3,
} D3DCULL;

// Color write enable flags (host D3D9 intermediate values)
#define D3DCOLORWRITEENABLE_RED   (1 << 0)
#define D3DCOLORWRITEENABLE_GREEN (1 << 1)
#define D3DCOLORWRITEENABLE_BLUE  (1 << 2)
#define D3DCOLORWRITEENABLE_ALPHA (1 << 3)

// Wrap coordinate flags (host D3D9 intermediate values)
#define D3DWRAPCOORD_0 (1 << 0)
#define D3DWRAPCOORD_1 (1 << 1)
#define D3DWRAPCOORD_2 (1 << 2)
#define D3DWRAPCOORD_3 (1 << 3)

// Vertex blend flags (host D3D9 intermediate values)
typedef enum _D3DVERTEXBLENDFLAGS {
	D3DVBF_DISABLE  = 0,
	D3DVBF_1WEIGHTS = 1,
	D3DVBF_2WEIGHTS = 2,
	D3DVBF_3WEIGHTS = 3,
} D3DVERTEXBLENDFLAGS;

// Material color source (host D3D9 intermediate values)
typedef enum _D3DMATERIALCOLORSOURCE {
	D3DMCS_MATERIAL = 0,
	D3DMCS_COLOR1   = 1,
	D3DMCS_COLOR2   = 2,
} D3DMATERIALCOLORSOURCE;

// Fog modes (host D3D9 intermediate values)
typedef enum _D3DFOGMODE {
	D3DFOG_NONE   = 0,
	D3DFOG_EXP    = 1,
	D3DFOG_EXP2   = 2,
	D3DFOG_LINEAR = 3,
} D3DFOGMODE;

// Present interval flags
#define D3DPRESENT_INTERVAL_DEFAULT   0x00000000
#define D3DPRESENT_INTERVAL_ONE       0x00000001
#define D3DPRESENT_INTERVAL_TWO       0x00000002
#define D3DPRESENT_INTERVAL_THREE     0x00000004
#define D3DPRESENT_INTERVAL_FOUR      0x00000008
#define D3DPRESENT_INTERVAL_IMMEDIATE 0x80000000

// Vertex declaration usage types
typedef enum _D3DDECLUSAGE {
	D3DDECLUSAGE_POSITION     = 0,
	D3DDECLUSAGE_BLENDWEIGHT  = 1,
	D3DDECLUSAGE_BLENDINDICES = 2,
	D3DDECLUSAGE_NORMAL       = 3,
	D3DDECLUSAGE_PSIZE        = 4,
	D3DDECLUSAGE_TEXCOORD     = 5,
	D3DDECLUSAGE_TANGENT      = 6,
	D3DDECLUSAGE_BINORMAL     = 7,
	D3DDECLUSAGE_TESSFACTOR   = 8,
	D3DDECLUSAGE_POSITIONT    = 9,
	D3DDECLUSAGE_COLOR        = 10,
	D3DDECLUSAGE_FOG          = 11,
	D3DDECLUSAGE_DEPTH        = 12,
	D3DDECLUSAGE_SAMPLE       = 13,
} D3DDECLUSAGE;

// Texture filter type
typedef DWORD D3DTEXTUREFILTERTYPE;

// D3D locked rect/box (D3D9 types used by Lock2DSurface/Lock3DSurface)
typedef struct _D3DLOCKED_RECT {
	INT Pitch;
	void *pBits;
} D3DLOCKED_RECT;

typedef struct _D3DLOCKED_BOX {
	INT RowPitch;
	INT SlicePitch;
	void *pBits;
} D3DLOCKED_BOX;

typedef struct _D3DBOX {
	UINT Left;
	UINT Top;
	UINT Right;
	UINT Bottom;
	UINT Front;
	UINT Back;
} D3DBOX;

// Behavior flags for CreateDevice (D3D9 values, used by Xbox title calls)
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING  0x00000020
#define D3DCREATE_HARDWARE_VERTEXPROCESSING  0x00000040

// D3DX constants
#define D3DX_DEFAULT ((UINT)-1)

// D3DX math compatibility types and functions
// These reproduce enough of the <d3dx9math.h> API to compile existing code.
// Implemented using DirectXMath internally.

struct D3DXVECTOR3 {
	float x, y, z;
	D3DXVECTOR3() : x(0), y(0), z(0) {}
	D3DXVECTOR3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	operator float*() { return &x; }
	operator const float*() const { return &x; }
	operator XMFLOAT3() const { return XMFLOAT3(x, y, z); }
	D3DXVECTOR3 operator+(const D3DXVECTOR3& rhs) const { return D3DXVECTOR3(x + rhs.x, y + rhs.y, z + rhs.z); }
};

struct D3DXVECTOR4 {
	float x, y, z, w;
	D3DXVECTOR4() : x(0), y(0), z(0), w(0) {}
	D3DXVECTOR4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	explicit D3DXVECTOR4(const float* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}
	operator float*() { return &x; }
	operator const float*() const { return &x; }
	operator D3DXVECTOR3() const { return D3DXVECTOR3(x, y, z); }
	operator XMFLOAT4() const { return XMFLOAT4(x, y, z, w); }
	D3DXVECTOR4 operator+(const D3DXVECTOR4& rhs) const { return D3DXVECTOR4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
};

struct D3DXCOLOR {
	float r, g, b, a;
	D3DXCOLOR() : r(0), g(0), b(0), a(0) {}
	D3DXCOLOR(DWORD argb) {
		a = ((argb >> 24) & 0xFF) / 255.0f;
		r = ((argb >> 16) & 0xFF) / 255.0f;
		g = ((argb >> 8) & 0xFF) / 255.0f;
		b = (argb & 0xFF) / 255.0f;
	}
	D3DXCOLOR(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	operator DWORD() const {
		return (DWORD(a * 255.0f + 0.5f) << 24) | (DWORD(r * 255.0f + 0.5f) << 16) |
		   	   (DWORD(g * 255.0f + 0.5f) << 8) | DWORD(b * 255.0f + 0.5f);
	}
	operator float*() { return &r; }
	operator const float*() const { return &r; }
	operator D3DXVECTOR3() const { return D3DXVECTOR3(r, g, b); }
	operator D3DXVECTOR4() const { return D3DXVECTOR4(r, g, b, a); }
};

struct D3DXMATRIX : public D3DMATRIX {
	D3DXMATRIX() { memset(this, 0, sizeof(D3DXMATRIX)); }
	D3DXMATRIX(const D3DMATRIX& m) { memcpy(this, &m, sizeof(D3DMATRIX)); }
	D3DXMATRIX(const XMMATRIX& m) { XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), m); }
	operator float*() { return &_11; }
	operator const float*() const { return &_11; }
	operator XMMATRIX() const { return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this)); }
};

inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut) {
	memset(pOut, 0, sizeof(D3DXMATRIX));
	pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f;
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2) {
	XMMATRIX m1 = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pM1));
	XMMATRIX m2 = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pM2));
	XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(pOut), XMMatrixMultiply(m1, m2));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* pOut, float* pDeterminant, const D3DXMATRIX* pM) {
	XMMATRIX m = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pM));
	XMVECTOR det;
	XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(pOut), XMMatrixInverse(&det, m));
	if (pDeterminant) *pDeterminant = XMVectorGetX(det);
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, const D3DXMATRIX* pM) {
	XMMATRIX m = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pM));
	XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(pOut), XMMatrixTranspose(m));
	return pOut;
}

inline D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM) {
	XMVECTOR v = XMVectorSet(pV->x, pV->y, pV->z, 1.0f);
	XMMATRIX m = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(pM));
	XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(pOut), XMVector3Transform(v, m));
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV) {
	XMVECTOR v = XMVectorSet(pV->x, pV->y, pV->z, 0.0f);
	XMVECTOR n = XMVector3Normalize(v);
	pOut->x = XMVectorGetX(n);
	pOut->y = XMVectorGetY(n);
	pOut->z = XMVectorGetZ(n);
	return pOut;
}

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

// Alias all host Direct3D symbols to generic symbols
#ifndef CXBX_USE_D3D11
// D3D9-only aliases for error reporting and capability types
#define DXGetErrorString                DXGetErrorString9A
#define DXGetErrorDescription           DXGetErrorDescription9A
#define D3DLockData                     void
#define D3DADAPTER_IDENTIFIER           D3DADAPTER_IDENTIFIER9
#define D3DCAPS                         D3DCAPS9
#else
// D3D11 replacements for error reporting (dxerr9 is not available)
inline const char* DXGetErrorString(HRESULT hr) { (void)hr; return "D3D11_ERROR"; }
inline const char* DXGetErrorDescription(HRESULT hr) { (void)hr; return ""; }
#define D3DLockData                     D3D11_MAPPED_SUBRESOURCE
#endif
#define EMUFORMAT                      _9_11(D3DFORMAT,                   DXGI_FORMAT)
#define D3DVERTEXELEMENT                _9_11(D3DVERTEXELEMENT9,           D3D11_INPUT_ELEMENT_DESC)
#define D3DVIEWPORT                     _9_11(D3DVIEWPORT9,                D3D11_VIEWPORT)
#define D3DSurfaceDesc                  _9_11(D3DSURFACE_DESC,             D3D11_TEXTURE2D_DESC)

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
#define IDirect3DSurface                _9_11(IDirect3DSurface9,           ID3D11Texture2D)
#define IDirect3DVolume                 _9_11(IDirect3DVolume9,            ID3D11Texture3D)
#define IDirect3DQuery                  _9_11(IDirect3DQuery9,             ID3D11Query)


// D3D9 D3FFORMAT to D3D11 DXGI_FORMAT mapping : https://docs.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-legacy-formats
#define EMUFMT_A1R5G5B5                _9_11(D3DFMT_A1R5G5B5,             DXGI_FORMAT_B5G5R5A1_UNORM) // Note : EMUFMT_X1R5G5B5 maps to same DXGI_FORMAT_B5G5R5A1_UNORM
#define EMUFMT_A4R4G4B4                _9_11(D3DFMT_A4R4G4B4,             DXGI_FORMAT_B4G4R4A4_UNORM)
#define EMUFMT_A8                      _9_11(D3DFMT_A8,                   DXGI_FORMAT_A8_UNORM)
#define EMUFMT_A8B8G8R8                _9_11(D3DFMT_A8B8G8R8,             DXGI_FORMAT_R8G8B8A8_UNORM)
#define EMUFMT_A8L8                    _9_11(D3DFMT_A8L8,                 DXGI_FORMAT_R8G8_UNORM)
#define EMUFMT_A8R8G8B8                _9_11(D3DFMT_A8R8G8B8,             DXGI_FORMAT_B8G8R8A8_UNORM)
#define EMUFMT_D16                     _9_11(D3DFMT_D16,                  DXGI_FORMAT_D16_UNORM)
#define EMUFMT_D24FS8                  _9_11(D3DFMT_D24FS8,               DXGI_FORMAT_R24G8_TYPELESS)
#define EMUFMT_D24S8                   _9_11(D3DFMT_D24S8,                DXGI_FORMAT_D24_UNORM_S8_UINT)
#define EMUFMT_DXT1                    _9_11(D3DFMT_DXT1,                 DXGI_FORMAT_BC1_UNORM)
#define EMUFMT_DXT3                    _9_11(D3DFMT_DXT3,                 DXGI_FORMAT_BC2_UNORM)
#define EMUFMT_DXT5                    _9_11(D3DFMT_DXT5,                 DXGI_FORMAT_BC3_UNORM)
#define EMUFMT_L16                     _9_11(D3DFMT_L16,                  DXGI_FORMAT_R16_UNORM)
#define EMUFMT_L8                      _9_11(D3DFMT_L8,                   DXGI_FORMAT_R8_UNORM)
#define EMUFMT_P8                      _9_11(D3DFMT_P8,                   DXGI_FORMAT_R8_UINT)
#define EMUFMT_Q8W8V8U8                _9_11(D3DFMT_Q8W8V8U8,             DXGI_FORMAT_R8G8B8A8_SNORM)
#define EMUFMT_R16F                    _9_11(D3DFMT_R16F,                 DXGI_FORMAT_R16_FLOAT)
#define EMUFMT_R5G6B5                  _9_11(D3DFMT_R5G6B5,               DXGI_FORMAT_B5G6R5_UNORM)
#define EMUFMT_UNKNOWN                 _9_11(D3DFMT_UNKNOWN,              DXGI_FORMAT_UNKNOWN) // Note : Used when format is unknown (duh)
#define EMUFMT_V16U16                  _9_11(D3DFMT_V16U16,               DXGI_FORMAT_R16G16_SNORM)
#define EMUFMT_V8U8                    _9_11(D3DFMT_V8U8,                 DXGI_FORMAT_R8G8_SNORM)
#define EMUFMT_X1R5G5B5                _9_11(D3DFMT_X1R5G5B5,             DXGI_FORMAT_B5G5R5A1_UNORM) // Note : EMUFMT_A1R5G5B5 maps to same DXGI_FORMAT_B5G5R5A1_UNORM
#define EMUFMT_X8R8G8B8                _9_11(D3DFMT_X8R8G8B8,             DXGI_FORMAT_B8G8R8A8_UNORM) // Note : B8G8R8X8_UNORM has optional SRV support; A8 variant is universally supported
#define EMUFMT_YUY2                    _9_11(D3DFMT_YUY2,                 DXGI_FORMAT_YUY2)

#define	DXGI_FORMAT_NOT_AVAILABLE       DXGI_FORMAT_UNKNOWN // Used for formats with no suitable DXGI equivalent
// Direct3D 9 format to Direct3D 11 best-effort mappings :
#define EMUFMT_D15S1                   _9_11(D3DFMT_D15S1,                DXGI_FORMAT_D16_UNORM) // Closest; stencil bit is lost
#define EMUFMT_D16_LOCKABLE            _9_11(D3DFMT_D16_LOCKABLE,         DXGI_FORMAT_D16_UNORM) // D3D11 uses staging textures for readback
#define EMUFMT_D24X4S4                 _9_11(D3DFMT_D24X4S4,              DXGI_FORMAT_D24_UNORM_S8_UINT) // Superset; 8-bit stencil instead of 4
#define EMUFMT_D24X8                   _9_11(D3DFMT_D24X8,                DXGI_FORMAT_D24_UNORM_S8_UINT) // Superset; stencil channel unused
#define EMUFMT_D32                     _9_11(D3DFMT_D32,                  DXGI_FORMAT_D32_FLOAT) // Integer-to-float precision change
#define EMUFMT_DXT2                    _9_11(D3DFMT_DXT2,                 DXGI_FORMAT_BC2_UNORM) // Same block compression as DXT3; premultiplied alpha is a convention
#define EMUFMT_DXT4                    _9_11(D3DFMT_DXT4,                 DXGI_FORMAT_BC3_UNORM) // Same block compression as DXT5; premultiplied alpha is a convention
#define EMUFMT_INDEX16                 _9_11(D3DFMT_INDEX16,              DXGI_FORMAT_R16_UINT) // Index buffer format
#define EMUFMT_L6V5U5                  _9_11(D3DFMT_L6V5U5,               DXGI_FORMAT_NOT_AVAILABLE) // No DXGI equivalent; requires software conversion to ARGB
#define EMUFMT_UYVY                    _9_11(D3DFMT_UYVY,                 DXGI_FORMAT_NOT_AVAILABLE) // No DXGI equivalent; requires software conversion
#define EMUFMT_X8L8V8U8                _9_11(D3DFMT_X8L8V8U8,             DXGI_FORMAT_R8G8B8A8_SNORM) // Approximate; L channel is unsigned in D3D9 but signed in DXGI
#define EMUFMT_VERTEXDATA              _9_11(D3DFMT_VERTEXDATA,           DXGI_FORMAT_NOT_AVAILABLE) // Not a texture format; used in vertex declarations only

typedef xbox::word_xt INDEX16; // TODO: Move INDEX16 into xbox namespace

#endif // XBD3D8TYPES_D3D9COMPAT_H
