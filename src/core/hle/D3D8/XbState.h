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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef XBSTATE_H
#define XBSTATE_H

#include "core\hle\D3D8\XbD3D8Types.h"

#define CXBX_D3DRS_UNSUPPORTED (XTL::X_D3DRS_LAST + 1)

// XDK version independent renderstate table, containing pointers to the original locations.
extern DWORD *EmuMappedD3DRenderState[CXBX_D3DRS_UNSUPPORTED]; // 1 extra for the unsupported value

struct X_Stream {
    DWORD Stride;
    DWORD Offset;
    XTL::X_D3DVertexBuffer *pVertexBuffer;
};

// EmuD3DDeferredRenderState
extern DWORD *EmuD3DDeferredRenderState;

// EmuD3DDeferredTextureState
extern DWORD *EmuD3DDeferredTextureState;

extern void EmuUpdateDeferredStates();

extern void CxbxUpdateNativeD3DResources();

#endif
