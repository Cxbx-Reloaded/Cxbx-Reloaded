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
#ifndef XGRAPHIC_H
#define XGRAPHIC_H

#include "core\hle\D3D8\XbD3D8Types.h"

namespace XTL {

typedef struct _XGPOINT3D
{
    DWORD u;
    DWORD v;
    DWORD w;
}
XGPOINT3D;

// ******************************************************************
// * patch: XGIsSwizzledFormat
// ******************************************************************
PVOID WINAPI EMUPATCH(XGIsSwizzledFormat)
(
    X_D3DFORMAT     Format
);

#if 0 // Leave unpatched
// ******************************************************************
// * patch: XGSwizzleRect
// ******************************************************************
VOID WINAPI EMUPATCH(XGSwizzleRect)
(
    LPCVOID       pSource,
    DWORD         Pitch,
    LPCRECT       pRect,
    LPVOID        pDest,
    DWORD         Width,
    DWORD         Height,
    CONST LPPOINT pPoint,
    DWORD         BytesPerPixel
);
#endif

// ******************************************************************
// * patch: XGSwizzleBox
// ******************************************************************
VOID WINAPI EMUPATCH(XGSwizzleBox)
(
    LPCVOID          pSource,
    DWORD            RowPitch,
    DWORD            SlicePitch,
    CONST D3DBOX    *pBox,
    LPVOID           pDest,
    DWORD            Width,
    DWORD            Height,
    DWORD            Depth,
    CONST XGPOINT3D *pPoint,
    DWORD            BytesPerPixel
);

// ******************************************************************
// * patch: XGWriteSurfaceOrTextureToXPR
// ******************************************************************
HRESULT WINAPI EMUPATCH(XGWriteSurfaceOrTextureToXPR)
( 
	LPVOID			pResource,
	const char*		cPath,
	BOOL			bWriteSurfaceAsTexture
);

// ******************************************************************
// * patch: XGSetTextureHeader
// ******************************************************************
VOID	WINAPI EMUPATCH(XGSetTextureHeader)
(
	UINT			Width,
	UINT			Height,
	UINT			Levels,
	DWORD			Usage,
	X_D3DFORMAT		Format,
	D3DPOOL			Pool,
	X_D3DTexture*	pTexture,
	UINT			Data,
	UINT			Pitch
);

// ******************************************************************
// * patch: XFONT_OpenBitmapFontFromMemory 
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFONT_OpenBitmapFontFromMemory) 
(
	CONST void		*pFontData,
	unsigned		uFontDataSize,
	void			**ppFont
);

} // end of namespace XTL

#endif
