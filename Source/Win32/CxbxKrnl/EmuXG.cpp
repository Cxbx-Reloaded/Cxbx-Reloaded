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
#define _XBOXKRNL_LOCAL_

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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace XTL
{
    #include "EmuXTL.h"
};

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
        printf("EmuXapi (0x%X): EmuXGIsSwizzledFormat\n"
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

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXapi (0x%X): EmuXGSwizzleRect\n"
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
    }
    #endif

    if(pRect == NULL && pPoint == NULL && Pitch == 0)
        memcpy(pDest, pSource, Width*Height*BytesPerPixel);
    else
        EmuCleanup("Unhandled swizzle (temporarily)");

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

	while( (i >= dwWidth) || (i >= dwHeight) || (i >= dwDepth) )
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
