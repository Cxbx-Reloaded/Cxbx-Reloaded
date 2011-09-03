// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXactEng.cpp
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

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"

#include <mmreg.h>
#include <msacm.h>
#include <process.h>
#include <clocale>


// NOTES: For Xbox, the XACT library is almost like a wrapper for DirectSound.
//		  It may or may not be necesarry to emulate the entire API because of 
//		  this, but atm only the functions containing low level code are being
//		  emulated.


// ******************************************************************
// * func: EmuXACTEngineCreate
// ******************************************************************
HRESULT WINAPI XTL::EmuXACTEngineCreate
(
	X_XACT_RUNTIME_PARAMETERS* pParams, 
	X_XACTEngine** ppEngine
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuXACTEngineCreate\n"
           "(\n"
           "   pParams                   : 0x%.08X\n"
           "   ppEngine                  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pParams, ppEngine);

	// TODO: Any other form of initialization?

	*ppEngine = (X_XACTEngine*) CxbxMalloc( sizeof( X_XACTEngine ) );

	EmuSwapFS();	// Xbox FS
	
	return S_OK;
}

// ******************************************************************
// * EmuXACTEngineDoWork
// ******************************************************************
void WINAPI XTL::EmuXACTEngineDoWork()
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuXACTEngineCreate()\n", GetCurrentThreadId() );

	// TODO: Anything else required here?
	// AFAIK, this function just calls DirectSoundDoWork()

	//EmuSwapFS();
	//EmuDirectSoundDoWork();
	//EmuSwapFS();

	EmuSwapFS();	// Xbox FS
}

// ******************************************************************
// * EmuIXACTEngine_RegisterWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_RegisterWaveBank
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTWaveBank**	ppWaveBank
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_RegisterWaveBank\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pvData                    : 0x%.08X\n"
		   "   dwSize                    : 0x%.08X\n"
		   "   ppWaveBank                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pvData, dwSize, ppWaveBank);

	// TODO: Implement

	*ppWaveBank = (X_XACTWaveBank*) CxbxMalloc( sizeof( X_XACTWaveBank ) );

	EmuSwapFS();	// Xbox FS

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_RegisterStreamedWaveBank
(
	X_XACTEngine*							pThis,
	X_XACT_WAVEBANK_STREAMING_PARAMETERS*	pParams,
	X_XACTWaveBank**						ppWaveBank
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_RegisterWaveBank\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pParams                   : 0x%.08X\n"
		   "   ppWaveBank                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pParams, ppWaveBank);

	// TODO: Implement

	*ppWaveBank = (X_XACTWaveBank*) CxbxMalloc( sizeof( X_XACTWaveBank ) );

	EmuSwapFS();	// Xbox FS

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_CreateSoundBank
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_CreateSoundBank
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTSoundBank**	ppSoundBank
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_CreateSoundBank\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pvData                    : 0x%.08X\n"
		   "   dwSize                    : 0x%.08X\n"
		   "   ppSoundBank               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pvData, dwSize, ppSoundBank);

	// TODO: Implement

	*ppSoundBank = (X_XACTSoundBank*) CxbxMalloc( sizeof( X_XACTSoundBank ) );

	EmuSwapFS();	// Xbox FS

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_DownloadEffectsImage
(
	X_XACTEngine*		pThis,
	PVOID				pvData,
	DWORD				dwSize,
	LPVOID				pEffectLoc,
	LPVOID*				ppImageDesc
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_DownloadEffectsImage\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pvData                    : 0x%.08X\n"
		   "   dwSize                    : 0x%.08X\n"
		   "   pEffectLoc                : 0x%.08X\n"
		   "   ppImageDesc               : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, dwSize, pEffectLoc, ppImageDesc);

	// TODO: Implement

	EmuSwapFS();	// Xbox FS

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_CreateSoundSource
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_CreateSoundSource
(
	X_XACTEngine*		pThis,
	DWORD				dwFlags,
	X_XACTSoundSource** ppSoundSource
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_CreateSoundSource\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   dwFlags                   : 0x%.08X\n"
		   "   ppSoundSource             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, dwFlags, ppSoundSource);

	*ppSoundSource = (X_XACTSoundSource*) CxbxMalloc( sizeof( X_XACTSoundSource ) );

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_EnableHeadphones
(
	X_XACTEngine*		pThis,
	BOOL				fEnabled
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_EnableHeadphones\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   fEnabled                  : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, fEnabled);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerOrientation
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_SetListenerOrientation
(
	X_XACTEngine*	pThis,
	float			xFront,
	float			yFront,
	float			zFront,
	float			xTop,
	float			yTop,
	float			zTop,
	DWORD			dwApply
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_SetListenerOrientation\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   xFront                    : %f"
		   "   yFront                    : %f"
		   "   zFront                    : %f"
		   "   xTop                      : %f"
		   "   yTop                      : %f"
		   "   zTop                      : %f"
		   "   dwApply                   : 0x%.08X"
           ");\n",
           GetCurrentThreadId(), pThis, xFront, yFront, zFront, xTop, yTop, zTop, dwApply);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_SetListenerPosition
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_SetListenerPosition\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   x                         : %f"
		   "   y                         : %f"
		   "   z                         : %f"
		   "   dwApply                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, x, y, z, dwApply);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerVelocity
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_SetListenerVelocity
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_SetListenerVelocity\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   x                         : %f"
		   "   y                         : %f"
		   "   z                         : %f"
		   "   dwApply                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, x, y, z, dwApply);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_SetMasterVolume
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_SetMasterVolume
(
	X_XACTEngine*	pThis,
    WORD			wCategory,
    LONG			lVolume
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_SetMasterVolume\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   wCategory                 : 0x%.08X\n"
		   "   lVolume                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, wCategory, lVolume);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_CommitDeferredSettings(X_XACTEngine* pThis)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_CommitDeferredSettings\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName
(
	X_XACTEngine*		pThis,
	PCSTR				pFriendlyName,
	DWORD*				pdwSoundCueIndex
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pFriendlyName             : (%s)\n"
		   "   pdwSoundCueIndex          : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pFriendlyName, pdwSoundCueIndex);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTSoundBank_Play
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTSoundBank_Play
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	X_XACTSoundSource*	pSoundSource,
	DWORD				dwFlags,
	X_XACTSoundCue**	ppSoundCue
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTSoundBank_Play\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   dwSoundCueIndex           : 0x%.08X\n"
		   "   pSoundSource              : 0x%.08X\n"
		   "   dwFlags                   : 0x%.08X\n"
		   "   ppSoundCue                : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, dwSoundCueIndex, pSoundSource, dwFlags, ppSoundCue);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTSoundBank_Stop
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTSoundBank_Stop
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	DWORD				dwFlags,
	X_XACTSoundCue*		pSoundCue
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTSoundBank_Stop\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   dwSoundCueIndex           : 0x%.08X\n"
		   "   dwFlags                   : 0x%.08X\n"
		   "   pSoundCue                 : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, dwSoundCueIndex, dwFlags, pSoundCue);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTSoundSource_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTSoundSource_SetPosition
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTSoundSource_SetPosition\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   x                         : %f\n"
		   "   y                         : %f\n"
		   "   z                         : %f\n"
		   "   dwApply                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, x, y, z, dwApply);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTSoundSource_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTSoundSource_SetVelocity
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTSoundSource_SetVelocity\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   x                         : %f\n"
		   "   y                         : %f\n"
		   "   z                         : %f\n"
		   "   dwApply                   : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, x, y, z, dwApply);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_RegisterNotification
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_RegisterNotification
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_RegisterNotification\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pNotificationDesc         : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pNotificationDesc);

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_GetNotification
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_GetNotification
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc,
    LPVOID						    pNotification
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_GetNotification\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pNotificationDesc         : 0x%.08X\n"
		   "   pNotification             : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pNotificationDesc, pNotification);

	// TODO: The contents of XACT_NOTIFICATION can vary from one XDK to the next.
	// The definition for 4627 is different than 5558.

	EmuSwapFS();

	return S_OK;
}

// ******************************************************************
// * EmuIXACTEngine_UnRegisterWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EmuIXACTEngine_UnRegisterWaveBank
(
	X_XACTEngine*	pThis,
    X_XACTWaveBank*	pWaveBank
)
{
	EmuSwapFS();	// Win2k/XP FS

	DbgPrintf("EmuXactEng (0x%X): EmuIXACTEngine_UnRegisterWaveBank\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pWaveBank                 : 0x%.08X\n"
           ");\n",
           GetCurrentThreadId(), pThis, pWaveBank);

	// Even though the documentation doesn't tell us much, I'm
	// assuming that after this function is called, the pointer
	// to IXACTWaveBank is released.

//	if(pWaveBank)
//		free(pWaveBank);

	EmuSwapFS();

	return S_OK;
}