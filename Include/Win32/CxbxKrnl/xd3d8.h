// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->xd3d8.h
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
#ifndef XD3D8_H
#define XD3D8_H

#include <d3d8.h>
#include <d3dx8tex.h>

#include "Emu.h"

// ******************************************************************
// * X_D3DFORMAT
// ******************************************************************
// NOTE: HACK: These enumerations are not equivalent!
typedef D3DFORMAT X_D3DFORMAT;

// ******************************************************************
// * func: EmuXB2PC_D3DFormat
// ******************************************************************
inline D3DFORMAT EmuXB2PC_D3DFormat(X_D3DFORMAT Format)
{
    if(Format == 0x07)
        return D3DFMT_X8R8G8B8;
    else if(Format == 0x06)
        return D3DFMT_A8R8G8B8;
    else if(Format == 0x2A)
        return D3DFMT_D24S8;
    else if(Format == 0x2C)
        return D3DFMT_D16;

    EmuCleanup("EmuXB2PC_D3DFormat: Unknown Format");

    return Format;
}

// ******************************************************************
// * func: EmuPC2XB_D3DFormat
// ******************************************************************
inline X_D3DFORMAT EmuPC2XB_D3DFormat(D3DFORMAT Format)
{
    if(Format == D3DFMT_X8R8G8B8)
        return (D3DFORMAT)0x07;
    else if(Format == D3DFMT_A8R8G8B8)
        return (D3DFORMAT)0x06;

    EmuCleanup("EmuPC2XB_D3DFormat: Unknown Format");

    return Format;
}

// ******************************************************************
// * X_D3DRESOURCETYPE
// ******************************************************************
// NOTE: HACK: These enumerations are not equivalent when > 7!
typedef D3DRESOURCETYPE X_D3DRESOURCETYPE;

// ******************************************************************
// * X_D3DDISPLAYMODE
// ******************************************************************
typedef struct _X_D3DDISPLAYMODE
{
    UINT        Width;
    UINT        Height;
    UINT        RefreshRate;
    DWORD       Flags;
    X_D3DFORMAT Format;
}
X_D3DDISPLAYMODE;

// ******************************************************************
// * X_D3DSURFACEDESC
// ******************************************************************
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

// ******************************************************************
// * D3DResource
// ******************************************************************
struct D3DResource
{
    DWORD Common;
    DWORD Data;
    DWORD Lock;
};

// ******************************************************************
// * D3DVertexBuffer
// ******************************************************************
struct D3DVertexBuffer : public D3DResource
{

};

// ******************************************************************
// * D3DVertexToPrimitive
// ******************************************************************
extern UINT D3DVertexToPrimitive[11][2];

// ******************************************************************
// * D3DVertexToPrimitiveCount
// ******************************************************************
#define D3DVertex2PrimitiveCount(PrimitiveType, VertexCount)    \
	(((VertexCount)-D3DVertexToPrimitive[PrimitiveType][1])/D3DVertexToPrimitive[PrimitiveType][0])

// ******************************************************************
// * EmuD3DDefferedRenderState
// ******************************************************************
extern DWORD *EmuD3DDefferedRenderState;

// ******************************************************************
// * EmuPrimitiveType
// ******************************************************************
extern D3DPRIMITIVETYPE EmuPrimitiveTypeLookup[];

// ******************************************************************
// * EmuPrimitiveType
// ******************************************************************
static inline D3DPRIMITIVETYPE EmuPrimitiveType(int PrimitiveType)
{
    if(PrimitiveType == 0x7FFFFFFF)
        return D3DPT_FORCE_DWORD;

    return EmuPrimitiveTypeLookup[PrimitiveType];
}

// ******************************************************************
// * func: EmuIDirect3D8_CreateDevice
// ******************************************************************
HRESULT WINAPI EmuIDirect3D8_CreateDevice
(
    UINT                    Adapter,
    D3DDEVTYPE              DeviceType,
    HWND                    hFocusWindow,
    DWORD                   BehaviorFlags,
    D3DPRESENT_PARAMETERS  *pPresentationParameters,
    IDirect3DDevice8      **ppReturnedDeviceInterface
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateTexture
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateTexture
(
    UINT                Width,
    UINT                Height,
    UINT                Levels,
    DWORD               Usage,
    D3DFORMAT           Format,
    D3DPOOL             Pool,
    IDirect3DTexture8 **ppTexture
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_GetDisplayMode
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_GetDisplayMode
(
    X_D3DDISPLAYMODE         *pMode
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Clear
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Clear
(
    DWORD                  Count,
    CONST D3DRECT         *pRects,
    DWORD                  Flags,
    D3DCOLOR               Color,
    float                  Z,
    DWORD                  Stencil
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Present
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Present
(
    CONST RECT* pSourceRect,
    CONST RECT* pDestRect,
    PVOID       pDummy1,
    PVOID       pDummy2
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_Swap
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Swap
(
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_GetDesc
// ******************************************************************
HRESULT WINAPI EmuIDirect3DSurface8_GetDesc
(
    PVOID               pThis,
    X_D3DSURFACE_DESC  *pDesc
);

// ******************************************************************
// * func: EmuIDirect3DSurface8_LockRect
// ******************************************************************
HRESULT WINAPI EmuIDirect3DSurface8_LockRect
(
    PVOID               pThis,
    D3DLOCKED_RECT     *pLockedRect,
    CONST RECT         *pRect,
    DWORD               Flags
);

// ******************************************************************
// * func: EmuIDirect3DTexture8_GetSurfaceLevel
// ******************************************************************
HRESULT WINAPI EmuIDirect3DTexture8_GetSurfaceLevel
(
    PVOID               pThis,
    UINT                Level,
    IDirect3DSurface8 **ppSurfaceLevel
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_CreateVertexBuffer
(
    UINT              Length,
    DWORD             Usage,
    DWORD             FVF,
    D3DPOOL           Pool,
    D3DVertexBuffer **ppVertexBuffer
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
D3DVertexBuffer* WINAPI EmuIDirect3DDevice8_CreateVertexBuffer2
(
    UINT Length
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_CullMode
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_CullMode
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetRenderState_ZEnable
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetRenderState_ZEnable
(
    DWORD Value
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetTransform
// ******************************************************************
VOID WINAPI EmuIDirect3DDevice8_SetTransform
(
    D3DTRANSFORMSTATETYPE State,
    CONST D3DMATRIX      *pMatrix
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
VOID WINAPI EmuIDirect3DVertexBuffer8_Lock
(
    D3DVertexBuffer *ppVertexBuffer,
    UINT             OffsetToLock,
    UINT             SizeToLock,
    BYTE           **ppbData,
    DWORD            Flags
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock2
// ******************************************************************
BYTE* WINAPI EmuIDirect3DVertexBuffer8_Lock2
(
    D3DVertexBuffer *ppVertexBuffer,
    DWORD            Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetStreamSource
(
    UINT             StreamNumber,
    D3DVertexBuffer *pStreamData,
    UINT             Stride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetVertexShader
(
    DWORD            Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_DrawVertices
(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT             StartVertex,
    UINT             VertexCount
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetLight
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetLight
(
    DWORD            Index,
    CONST D3DLIGHT8 *pLight
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetMaterial
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_SetMaterial
(
    CONST D3DMATERIAL8 *pMaterial
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_LightEnable
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_LightEnable
(
    DWORD            Index,
    BOOL             bEnable
);

#endif
