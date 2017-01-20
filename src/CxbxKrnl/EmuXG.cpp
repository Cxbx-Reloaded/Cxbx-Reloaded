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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

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

/* Leave unpatched
// ******************************************************************
// * func: EmuXGIsSwizzledFormat
// ******************************************************************
PVOID WINAPI XTL::EMUPATCH(EmuXGIsSwizzledFormat)
(
    X_D3DFORMAT Format
)
{
	LOG_FUNC_ONE_ARG(Format);

	RETURN(FALSE);
}
*/

/* Leave unpatched
// ******************************************************************
// * func: EmuXGSwizzleRect
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(EmuXGSwizzleRect)
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
*/

// ******************************************************************
// * func: EmuXGSwizzleBox
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(EmuXGSwizzleBox)
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
// * func: EmuXGWriteSurfaceOrTextureToXPR
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(EmuXGWriteSurfaceOrTextureToXPR)
( 
	LPVOID			pResource,
	const char*		cPath,
	BOOL			bWriteSurfaceAsTexture
)
{
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
// * func: EmuXGSetTextureHeader
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(EmuXGSetTextureHeader)
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
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Width)
		LOG_FUNC_ARG(Height)
		LOG_FUNC_ARG(Levels)
		LOG_FUNC_ARG(Usage)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG(Pool)
		LOG_FUNC_ARG(pTexture)
		LOG_FUNC_ARG(Data)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_END;

	// NOTES: This function simply creates a texture that needs to be registered
	// via D3DDevice_Register afterwards.  So, do I just create the texture via
	// EmuIDirect3DDevice8_CreateTexture, or just fill in the interface and let
	// EmuIDirect3DDevice8_Register do the rest?  Trial and error.

	X_D3DTexture* pTempTexture = NULL;
	DWORD l2w = (DWORD) log( (float)Width ) / log(2.0f);
	DWORD l2h = (DWORD) log( (float)Height ) / log(2.0f);

	/*if( Data != 0 )
		CxbxKrnlCleanup( "Data != 0 (XGSetTextureHeader)" );

	if( Pitch != 0 )
		CxbxKrnlCleanup( "Pitch != 0 (XGSetTextureHeader)" );*/

	// Generate a temporary texture and fill in the necessary fields within
	// the X_D3DTexture interface (lazy, I know).
	
	pTempTexture = (X_D3DTexture*) XTL::EMUPATCH(EmuIDirect3DDevice8_CreateTexture2)(Width, Height, 0, Levels, Usage, Format,
		XTL::D3DRTYPE_TEXTURE);
	

	pTexture->Data		= pTempTexture->Data;
	pTexture->Common	= X_D3DCOMMON_TYPE_TEXTURE; //pTempTexture->Common;
//	pTexture->Format	= pTempTexture->Format;
	pTexture->Lock		= pTempTexture->Lock; // 0;
	pTexture->Size		= pTempTexture->Size;

	
	XTL::EMUPATCH(EmuIDirect3DResource8_Release)(pTempTexture);
	

	// Manually fill in Format parameters
	/*pTexture->Format |= ( ( ( Width >> 1 ) & 0xF ) << X_D3DFORMAT_USIZE_SHIFT ) |	// Width
					   ( ( ( Height>> 1 ) & 0xF ) << X_D3DFORMAT_VSIZE_SHIFT ) |	// Height
					   ( ( ( Levels     ) & 0xF ) << X_D3DFORMAT_MIPMAP_SHIFT ) |	// Mip Levels
				//	   ( ( ( ((DWORD)Format)) & 0xFF ) << X_D3DFORMAT_FORMAT_SHIFT ) |	// Format (Already set)
					   ( ( ( 2			) & 0xF ) << X_D3DFORMAT_DIMENSION_SHIFT );	// Dimensions
*/
	pTexture->Format |= ( ( l2w & 0xF ) << X_D3DFORMAT_USIZE_SHIFT );
	pTexture->Format |= ( ( l2h & 0xF ) << X_D3DFORMAT_VSIZE_SHIFT );
	pTexture->Format |= ( ( Levels & 0xF ) << X_D3DFORMAT_MIPMAP_SHIFT );
	pTexture->Format |= ( ( ( ((DWORD)Format)) & 0xFF ) << X_D3DFORMAT_FORMAT_SHIFT );
	pTexture->Format |= ( ( 2 & 0xF ) << X_D3DFORMAT_DIMENSION_SHIFT );

//	D3DCOLOR_XRGB(
	DbgPrintf( "pTexture->Format:= 0x%.08X\n", pTexture->Format );

}

// ******************************************************************
// * func: EmuXFONT_OpenBitmapFontFromMemory 
// ******************************************************************
//HRESULT WINAPI XTL::EMUPATCH(EmuXFONT_OpenBitmapFontFromMemory) 
//(
//	CONST void		*pFontData,
//	unsigned		uFontDataSize,
//	void			**ppFont
//)
//{
//		
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
