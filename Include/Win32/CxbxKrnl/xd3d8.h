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
// * EmuPrimitiveType
// ******************************************************************
static D3DPRIMITIVETYPE EmuPrimitiveTypeLookup[] = 
{
    /* NULL                 = 0         */ (D3DPRIMITIVETYPE)0,
    /* D3DPT_POINTLIST      = 1,        */ D3DPT_POINTLIST,
    /* D3DPT_LINELIST       = 2,        */ D3DPT_LINELIST,
    /* D3DPT_LINELOOP       = 3,  Xbox  */ D3DPT_LINELIST,
    /* D3DPT_LINESTRIP      = 4,        */ D3DPT_LINESTRIP,
    /* D3DPT_TRIANGLELIST   = 5,        */ D3DPT_TRIANGLELIST,
    /* D3DPT_TRIANGLESTRIP  = 6,        */ D3DPT_TRIANGLESTRIP,
    /* D3DPT_TRIANGLEFAN    = 7,        */ D3DPT_TRIANGLEFAN,
    /* D3DPT_QUADLIST       = 8,  Xbox  */ D3DPT_TRIANGLEFAN,
    /* D3DPT_QUADSTRIP      = 9,  Xbox  */ D3DPT_TRIANGLEFAN,
    /* D3DPT_POLYGON        = 10, Xbox  */ D3DPT_TRIANGLEFAN,
    /* D3DPT_MAX            = 11,       */ (D3DPRIMITIVETYPE)11
};

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
// * func: EmuIDirect3DDevice8_Swap
// ******************************************************************
HRESULT WINAPI EmuIDirect3DDevice8_Swap
(
    DWORD Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_CreateVertexBuffer
// ******************************************************************
D3DVertexBuffer* WINAPI EmuIDirect3DDevice8_CreateVertexBuffer2
(
    UINT Length
);

// ******************************************************************
// * func: EmuIDirect3DVertexBuffer8_Lock
// ******************************************************************
BYTE* WINAPI EmuIDirect3DVertexBuffer8_Lock
(
    D3DVertexBuffer *ppVertexBuffer,
    DWORD            Flags
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetStreamSource
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetStreamSource
(
    UINT             StreamNumber,
    D3DVertexBuffer *pStreamData,
    UINT             Stride
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_SetVertexShader
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_SetVertexShader
(
    DWORD            Handle
);

// ******************************************************************
// * func: EmuIDirect3DDevice8_DrawVertices
// ******************************************************************
void WINAPI EmuIDirect3DDevice8_DrawVertices
(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT             StartVertex,
    UINT             VertexCount
);

#endif
