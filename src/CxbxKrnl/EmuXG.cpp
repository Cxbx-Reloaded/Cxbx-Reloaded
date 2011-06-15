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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"

// ******************************************************************
// * func: EmuXGIsSwizzledFormat
// ******************************************************************
PVOID WINAPI XTL::EmuXGIsSwizzledFormat
(
    XTL::D3DFORMAT Format
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        DbgPrintf("EmuXapi (0x%X): EmuXGIsSwizzledFormat\n"
               "(\n"
               "   Format              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Format);
        EmuSwapFS();   // Xbox FS
    }
    #endif

    return FALSE;
}

// ******************************************************************
// * func: EmuXGSwizzleRect
// ******************************************************************
VOID WINAPI XTL::EmuXGSwizzleRect
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
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXGSwizzleRect\n"
           "(\n"
           "   pSource             : 0x%.08X\n"
           "   Pitch               : 0x%.08X\n"
           "   pRect               : 0x%.08X\n"
           "   pDest               : 0x%.08X\n"
           "   Width               : 0x%.08X\n"
           "   Height              : 0x%.08X\n"
           "   pPoint              : 0x%.08X\n"
           "   BytesPerPixel       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pSource, Pitch, pRect, pDest, Width, Height,
           pPoint, BytesPerPixel);

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

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * func: EmuXGSwizzleBox
// ******************************************************************
VOID WINAPI XTL::EmuXGSwizzleBox
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
    EmuSwapFS();   // Win2k/XP FS

    DbgPrintf("EmuXapi (0x%X): EmuXGSwizzleBox\n"
           "(\n"
           "   pSource             : 0x%.08X\n"
           "   RowPitch            : 0x%.08X\n"
           "   SlicePitch          : 0x%.08X\n"
           "   pBox                : 0x%.08X\n"
           "   pDest               : 0x%.08X\n"
           "   Width               : 0x%.08X\n"
           "   Height              : 0x%.08X\n"
           "   Depth               : 0x%.08X\n"
           "   pPoint              : 0x%.08X\n"
           "   BytesPerPixel       : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pSource, RowPitch, SlicePitch, pBox, pDest, Width, Height,
           Depth, pPoint, BytesPerPixel);

	if(pDest != (LPVOID) 0x80000000)
	{
		if(pBox == NULL && pPoint == NULL && RowPitch == 0 && SlicePitch == 0)
		{
			memcpy(pDest, pSource, Width*Height*Depth*BytesPerPixel);
		}
		else
		{
			if(pPoint != NULL && (pPoint->u != 0 || pPoint->u != 0 || pPoint->w != 0))
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

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * func: EmuXGUnswizzleRect
// ******************************************************************
VOID WINAPI XTL::EmuXGUnswizzleRect
(
    PVOID           pSrcBuff,
    DWORD           dwWidth,
    DWORD           dwHeight,
    DWORD           dwDepth,
    PVOID           pDstBuff,
    DWORD           dwPitch,
    RECT            rSrc,
    POINT           poDst,
    DWORD           dwBPP
)
{
    DWORD dwOffsetU = 0, dwMaskU = 0;
    DWORD dwOffsetV = 0, dwMaskV = 0;
    DWORD dwOffsetW = 0, dwMaskW = 0;

    DWORD i = 1;
    DWORD j = 1;

//  while( (i >= dwWidth) || (i >= dwHeight) || (i >= dwDepth) )
    while( (i <= dwWidth) || (i <= dwHeight) || (i <= dwDepth) )
    {
        if(i < dwWidth)
        {
            dwMaskU |= j;
            j<<=1;
        }

        if(i < dwHeight)
        {
            dwMaskV |= j;
            j<<=1;
        }

        if(i < dwDepth)
        {
            dwMaskW |= j;
            j<<=1;
        }

        i<<=1;
    }

    DWORD dwSU = 0;
    DWORD dwSV = 0;
    DWORD dwSW = 0;
    DWORD dwMaskMax=0;

    // get the biggest mask
    if(dwMaskU > dwMaskV)
        dwMaskMax=dwMaskU;
    else
        dwMaskMax=dwMaskV;
    if(dwMaskW > dwMaskMax)
        dwMaskMax=dwMaskW;

    for(i = 1; i <= dwMaskMax; i<<=1)
    {
        if(i<=dwMaskU)
        {
            if(dwMaskU & i) dwSU |= (dwOffsetU & i);
            else            dwOffsetU<<=1;
        }

        if(i<=dwMaskV)
        {
            if(dwMaskV & i) dwSV |= (dwOffsetV & i);
            else            dwOffsetV<<=1;
        }

        if(i<=dwMaskW)
        {
            if(dwMaskW & i) dwSW |= (dwOffsetW & i);
            else            dwOffsetW<<=1;
        }
    }

    DWORD dwW = dwSW;
    DWORD dwV = dwSV;
    DWORD dwU = dwSU;

    for(DWORD z=0; z<dwDepth; z++)
    {
        dwV = dwSV;

        for(DWORD y=0; y<dwHeight; y++)
        {
            dwU = dwSU;

            for (DWORD x=0; x<dwWidth; x++)
            {
                memcpy(pDstBuff, &((BYTE*)pSrcBuff)[(dwU|dwV|dwW)*dwBPP], dwBPP);
                pDstBuff=(PVOID)(((DWORD)pDstBuff)+dwBPP);

                dwU = (dwU - dwMaskU) & dwMaskU;
            }
            pDstBuff=(PVOID)(((DWORD)pDstBuff)+(dwPitch-dwWidth*dwBPP));
            dwV = (dwV - dwMaskV) & dwMaskV;
        }
        dwW = (dwW - dwMaskW) & dwMaskW;
    }
}

// ******************************************************************
// * func: EmuXGWriteSurfaceOrTextureToXPR
// ******************************************************************
HRESULT WINAPI XTL::EmuXGWriteSurfaceOrTextureToXPR
( 
	LPVOID			pResource,
	const char*		cPath,
	BOOL			bWriteSurfaceAsTexture
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGWriteSurfaceOrTextureToXPR\n"
           "(\n"
           "   pResource              : 0x%.08X\n"
		   "   cPath                  : 0x%.08X\n"
		   "   bWriteSurfaceAsTexture : 0x%.08X\n"
		   ");\n",
		   GetCurrentThreadId(), pResource, cPath, bWriteSurfaceAsTexture);

	// TODO: If necessary, either reverse the .xbx and .xpr file formats
	// and write the surface/texture to a file, or output a generic .xbx
	// file and be done with it.

	EmuWarning("(Temporarily) ignoring EmuXGWriteSurfaceOrTextureToXPR. Need file specs.");

	EmuSwapFS();	// Xbox FS

	return S_OK;
}

// ******************************************************************
// * func: EmuXGSetTextureHeader
// ******************************************************************
VOID WINAPI XTL::EmuXGSetTextureHeader
(
	UINT			Width,
	UINT			Height,
	UINT			Levels,
	DWORD			Usage,
	D3DFORMAT		Format,
	D3DPOOL			Pool,
	X_D3DTexture*	pTexture,
	UINT			Data,
	UINT			Pitch
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXapi (0x%X): EmuXGSetTextureHeader\n"
           "(\n"
           "   Width                  : 0x%.08X\n"
		   "   Height                 : 0x%.08X\n"
		   "   Levels                 : 0x%.08X\n"
		   "   Usage                  : 0x%.08X\n"
		   "   Format                 : 0x%.08X\n"
		   "   Pool                   : 0x%.08X\n"
		   "   pTexture               : 0x%.08X\n"
		   "   Data                   : 0x%.08X\n"
		   "   Pitch                  : 0x%.08X\n"
		   ");\n",
		   GetCurrentThreadId(), Width, Height, Levels, Usage, 
				Format, Pool, pTexture, Data, Pitch);

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
	EmuSwapFS();
	pTempTexture = (X_D3DTexture*) XTL::EmuIDirect3DDevice8_CreateTexture2(Width, Height, 0, Levels, Usage, Format, 
		XTL::D3DRTYPE_TEXTURE);
	EmuSwapFS();

	pTexture->Data		= pTempTexture->Data;
	pTexture->Common	= X_D3DCOMMON_TYPE_TEXTURE; //pTempTexture->Common;
//	pTexture->Format	= pTempTexture->Format;
	pTexture->Lock		= pTempTexture->Lock; // 0;
	pTexture->Size		= pTempTexture->Size;

	EmuSwapFS();
	XTL::EmuIDirect3DResource8_Release(pTempTexture);
	EmuSwapFS();

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

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * func: EmuXFONT_OpenBitmapFontFromMemory 
// ******************************************************************
//HRESULT WINAPI XTL::EmuXFONT_OpenBitmapFontFromMemory 
//(
//	CONST void		*pFontData,
//	unsigned		uFontDataSize,
//	void			**ppFont
//)
//{
//	EmuSwapFS();	// Win2k/XP FS
//
//	DbgPrintf("EmuXapi (0x%X): EmuXFONT_OpenBitmapFontFromMemory\n"
//           "(\n"
//		   "   pFontData              : 0x%.08X\n"
//		   "   uFontDataSize          : 0x%.08X\n"
//		   "   ppFont                 : 0x%.08X\n"
//		   ");\n",
//		   GetCurrentThreadId(), pFontData, uFontDataSize, ppFont);
//
//	__asm int 3;
//
//	EmuSwapFS(); // Xbox FS
//
//	return E_FAIL;
//}
