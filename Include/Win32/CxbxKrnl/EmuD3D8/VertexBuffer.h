// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexBuffer.h
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
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Cxbx.h"

// fixup xbox extensions to be compatible with PC direct3d
extern UINT EmuFixupVerticesA
(
    DWORD                           PrimitiveType,
    UINT                           &PrimitiveCount,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8,
    UINT                            dwOffset,
    PVOID                           pVertexStreamZeroData,
    UINT                            uiVertexStreamZeroStride, 
    PVOID                          *ppNewVertexStreamZeroData
);

// fixup xbox extensions to be compatible with PC direct3d
extern VOID EmuFixupVerticesB
(
    UINT                            nStride,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8
);

// inline vertex buffer emulation
extern DWORD                  *g_pIVBVertexBuffer;
extern X_D3DPRIMITIVETYPE      g_IVBPrimitiveType;

extern struct _D3DIVB
{
    XTL::D3DXVECTOR3 Position;   // Position
    XTL::DWORD       dwSpecular; // Specular
    XTL::DWORD       dwDiffuse;  // Diffuse
    XTL::D3DXVECTOR3 Normal;     // Normal
    XTL::D3DXVECTOR2 TexCoord1;  // TexCoord1
    XTL::D3DXVECTOR2 TexCoord2;  // TexCoord2
    XTL::D3DXVECTOR2 TexCoord3;  // TexCoord3
    XTL::D3DXVECTOR2 TexCoord4;  // TexCoord4
}
*g_IVBTable;

extern UINT g_IVBTblOffs;

extern VOID EmuFlushIVB();

#endif