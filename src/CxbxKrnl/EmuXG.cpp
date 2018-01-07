// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXG.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "XGRP"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include "Emu.h"
#include "EmuFS.h"
#include "Logging.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"
#include "EmuD3D8Logging.h" // for log rendering of X_D3DFORMAT, etc.

// ******************************************************************
// * patch: XGIsSwizzledFormat
// ******************************************************************
PVOID WINAPI XTL::EMUPATCH(XGIsSwizzledFormat)
(
    X_D3DFORMAT Format
)
{
	FUNC_EXPORTS
	LOG_FUNC_ONE_ARG(Format);

	RETURN(FALSE);
}

#if 0 // Leave unpatched
// ******************************************************************
// * patch: XGSwizzleRect
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XGSwizzleRect)
(
    LPCVOID       pSource,
    DWORD         Pitch,
    LPCRECT       pRect,
    LPVOID        pDest,
    DWORD         Width,
    DWORD         Height,
    CONST LPPOINT pPoint,
    DWORD         BytesPerPixel
)
{
	FUNC_EXPORTS
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSource)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_ARG(pRect)
		LOG_FUNC_ARG(pDest)
		LOG_FUNC_ARG(Width)
		LOG_FUNC_ARG(Height)
		LOG_FUNC_ARG(pPoint)
		LOG_FUNC_ARG(BytesPerPixel)
		LOG_FUNC_END;

    if(pRect == NULL && pPoint == NULL && Pitch == 0)
    {
        memcpy(pDest, pSource, Width*Height*BytesPerPixel);
    }
    else
    {
        if(pPoint != NULL && (pPoint->x != 0 || pPoint->y != 0))
            CxbxKrnlCleanup("Temporarily unsupported swizzle (very easy fix)");

        DWORD dwMaxY = Height;
        DWORD dwChunkSize = Width;

        uint08 *pSrc = (uint08*)pSource;
        uint08 *pDst = (uint08*)pDest;

        if(pRect != 0)
        {
            pSrc += pRect->top*Pitch;
            pSrc += pRect->left;

            dwMaxY = pRect->bottom - pRect->top;
            dwChunkSize = pRect->right - pRect->left;
        }

        for(DWORD y=0;y<dwMaxY;y++)
        {
            memcpy(pSrc, pDst, dwChunkSize);
            pSrc += Pitch;
            pDst += Pitch;
        }
    }
}
#endif

// ******************************************************************
// * patch: XGSwizzleBox
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XGSwizzleBox)
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
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pSource)
		LOG_FUNC_ARG(RowPitch)
		LOG_FUNC_ARG(SlicePitch)
		LOG_FUNC_ARG(pBox)
		LOG_FUNC_ARG(pDest)
		LOG_FUNC_ARG(Width)
		LOG_FUNC_ARG(Height)
		LOG_FUNC_ARG(Depth)
		LOG_FUNC_ARG(pPoint)
		LOG_FUNC_ARG(BytesPerPixel)
		LOG_FUNC_END;

	if(pDest != (LPVOID) 0x80000000)
	{
		if(pBox == NULL && pPoint == NULL && RowPitch == 0 && SlicePitch == 0)
		{
			memcpy(pDest, pSource, Width*Height*Depth*BytesPerPixel);
		}
		else
		{
			if(pPoint != NULL && (pPoint->u != 0 || pPoint->v != 0 || pPoint->w != 0))
				CxbxKrnlCleanup("Temporarily unsupported swizzle (very easy fix)");

			DWORD dwMaxY = Height;
			DWORD dwMaxZ = Depth;
			DWORD dwChunkSize = Width;

			uint08 *pSrc = (uint08*)pSource;
			uint08 *pDst = (uint08*)pDest;

			if(pBox != 0)
			{
				pSrc += pBox->Top*RowPitch;
				pSrc += pBox->Left;

				dwMaxY = pBox->Bottom - pBox->Top;
				dwChunkSize = pBox->Right - pBox->Left;
			}

			for(DWORD y=0;y<dwMaxY;y++)
			{
				memcpy(pSrc, pDst, dwChunkSize);
				pSrc += RowPitch;
				pDst += RowPitch;
			}
        }
	}
}

// ******************************************************************
// * patch: XGWriteSurfaceOrTextureToXPR
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XGWriteSurfaceOrTextureToXPR)
( 
	LPVOID			pResource,
	const char*		cPath,
	BOOL			bWriteSurfaceAsTexture
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pResource)
		LOG_FUNC_ARG(cPath)
		LOG_FUNC_ARG(bWriteSurfaceAsTexture)
		LOG_FUNC_END;

	// TODO: If necessary, either reverse the .xbx and .xpr file formats
	// and write the surface/texture to a file, or output a generic .xbx
	// file and be done with it.

	LOG_IGNORED(); // (Temporarily) ignoring EmuXGWriteSurfaceOrTextureToXPR. Need file specs.

	RETURN(S_OK);
}

// ******************************************************************
// * patch: XGSetTextureHeader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XGSetTextureHeader)
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
)
{
	//FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Width)
		LOG_FUNC_ARG(Height)
		LOG_FUNC_ARG(Levels)
		LOG_FUNC_ARG_TYPE(X_D3DUSAGE, Usage) // TODO : How to embed this?
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG(Pool) // TODO : How to embed this?
		LOG_FUNC_ARG(pTexture)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_END;

	/*if( Data != 0 )
		CxbxKrnlCleanup( "Data != 0 (XGSetTextureHeader)" );

	if( Pitch != 0 )
		CxbxKrnlCleanup( "Pitch != 0 (XGSetTextureHeader)" );*/

	pTexture->Common = X_D3DCOMMON_TYPE_TEXTURE + 1; // Set refcount to 1
	pTexture->Data = Data;
	// Note : Do NOT touch pTexture->Lock, as callers can have set it already !

	// Width or Height both a power of two?
	DWORD l2w; _BitScanReverse(&l2w, Width); // MSVC intrinsic; GCC has __builtin_clz
	DWORD l2h; _BitScanReverse(&l2h, Height);
	if (((1 << l2w) == Width) && ((1 << l2h) == Height)) {
		Width = Height = Pitch = 1; // When setting Format, clear Size field
	} else {
		l2w = l2h = 0; // When setting Size, clear D3DFORMAT_USIZE and VSIZE
	}

	// TODO : Must these be set using Usage / Pool / something else?
	const int Dimensions = 2;
	const int Depth = 1;

	// Manually fill in Format parameters
	pTexture->Format = 0
		| ((Dimensions << X_D3DFORMAT_DIMENSION_SHIFT) & X_D3DFORMAT_DIMENSION_MASK)
		| (((DWORD)Format << X_D3DFORMAT_FORMAT_SHIFT) & X_D3DFORMAT_FORMAT_MASK)
		| ((Levels << X_D3DFORMAT_MIPMAP_SHIFT) & X_D3DFORMAT_MIPMAP_MASK)
		| ((l2w << X_D3DFORMAT_USIZE_SHIFT) & X_D3DFORMAT_USIZE_MASK)
		| ((l2h << X_D3DFORMAT_VSIZE_SHIFT) & X_D3DFORMAT_VSIZE_MASK)
		| ((Depth << X_D3DFORMAT_PSIZE_SHIFT) & X_D3DFORMAT_PSIZE_MASK)
		;

	pTexture->Size = 0
		| (((Width - 1) /*X_D3DSIZE_WIDTH_SHIFT*/) & X_D3DSIZE_WIDTH_MASK)
		| (((Height - 1) << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK)
        | (((Pitch - 1) << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK);
}

// ******************************************************************
// * patch: XFONT_OpenBitmapFontFromMemory 
// ******************************************************************
//HRESULT WINAPI XTL::EMUPATCH(XFONT_OpenBitmapFontFromMemory) 
//(
//	CONST void		*pFontData,
//	unsigned		uFontDataSize,
//	void			**ppFont
//)
//{
//		FUNC_EXPORTS
//
//		LOG_FUNC_BEGIN
//			LOG_FUNC_ARG(pFontData)
//			LOG_FUNC_ARG(uFontDataSize)
//			LOG_FUNC_ARG(ppFont)
//			LOG_FUNC_END;
//
//	__asm int 3;
//
//	 
//
//	RETURN(E_FAIL);
//}
