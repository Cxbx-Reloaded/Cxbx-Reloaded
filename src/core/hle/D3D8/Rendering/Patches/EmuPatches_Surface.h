// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUPATCHES_SURFACE_H
#define EMUPATCHES_SURFACE_H

#include "core\hle\XAPI\Xapi.h" // For EMUPATCH
#include "core\hle\D3D8\XbD3D8Types.h"

namespace xbox {

// ******************************************************************
// * patch: D3DDevice_CopyRects
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_CopyRects)
(
   	X_D3DSurface       *pSourceSurface,
   	CONST X_RECT       *pSourceRectsArray,
   	uint_xt             cRects,
   	X_D3DSurface       *pDestinationSurface,
   	CONST X_POINT      *pDestPointsArray
);

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer2
// ******************************************************************
X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetBackBuffer2)
(
   	int_xt                 BackBuffer
);

X_D3DSurface* WINAPI EMUPATCH(D3DDevice_GetBackBuffer2_0__LTCG_eax1)();

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetBackBuffer)
(
   	int_xt             BackBuffer,
   	D3DBACKBUFFER_TYPE Type,
   	X_D3DSurface     **ppBackBuffer
);

// ******************************************************************
// * patch: D3DDevice_GetBackBuffer_8__LTCG_eax1
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_GetBackBuffer_8__LTCG_eax1)
(
   	D3DBACKBUFFER_TYPE Type,
   	X_D3DSurface     **ppBackBuffer
);

// ******************************************************************
// * patch: D3DDevice_Present
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Present)
(
   	CONST X_RECT* pSourceRect,
   	CONST X_RECT* pDestRect,
   	PVOID         pDummy1,
   	PVOID         pDummy2
);

dword_xt WINAPI EMUPATCH(D3DDevice_Swap_0__LTCG_eax1)();

// ******************************************************************
// * patch: Lock2DSurface
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock2DSurface)
(
   	X_D3DPixelContainer *pPixelContainer,
   	X_D3DCUBEMAP_FACES   FaceType,
   	uint_xt              Level,
   	X_D3DLOCKED_RECT    *pLockedRect,
	X_RECT              *pRect,
   	dword_xt             Flags
);

// ******************************************************************
// * patch: Lock2DSurface_16__LTCG_esi4_eax5
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock2DSurface_16__LTCG_esi4_eax5)
(
   	X_D3DPixelContainer *pPixelContainer,
   	X_D3DCUBEMAP_FACES   FaceType,
   	uint_xt              Level,
   	dword_xt             Flags
);

// ******************************************************************
// * patch: Lock3DSurface
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock3DSurface)
(
   	X_D3DPixelContainer *pPixelContainer,
   	uint_xt              Level,
   	D3DLOCKED_BOX       *pLockedVolume,
   	D3DBOX              *pBox,
   	dword_xt             Flags
);

// ******************************************************************
// * patch: Lock3DSurface_16__LTCG_eax4
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(Lock3DSurface_16__LTCG_eax4)
(
   	X_D3DPixelContainer *pPixelContainer,
   	uint_xt              Level,
   	D3DLOCKED_BOX       *pLockedVolume,
   	dword_xt             Flags
);

// ******************************************************************
// * patch: D3DDevice_PersistDisplay
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(D3DDevice_Clear)
(
   	dword_xt           Count,
   	CONST X_D3DRECT   *pRects,
   	dword_xt           Flags,
   	X_D3DCOLOR         Color,
   	float              Z,
   	dword_xt           Stencil
);

xbox::hresult_xt WINAPI EMUPATCH(D3DDevice_PersistDisplay)();

} // namespace xbox

#endif // EMUPATCHES_SURFACE_H
