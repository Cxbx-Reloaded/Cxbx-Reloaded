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
// *   src->core->HLE->XACTENG->XactEng.cpp
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

#define LOG_PREFIX CXBXR_MODULE::XACT

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl/CxbxKrnl.h"
#include "Common/Logging.h"
#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuFS.h"
#include "CxbxKrnl/EmuShared.h"
#include "CxbxKrnl/EmuXTL.h"
#include "CxbxKrnl/VMManager.h"

#include <mmreg.h>
#include <msacm.h>
#include <process.h>
#include <clocale>


// NOTES: For Xbox, the XACT library is a wrapper for DirectSound.
//		  It is not be necesarry to emulate the entire API because of 
//		  this, but atm only the functions containing low level code are being
//		  emulated.
//		  Only timing require a patch which is done in LLE.


// ******************************************************************
// * patch: XACTEngineCreate
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XACTEngineCreate)
(
	X_XACT_RUNTIME_PARAMETERS* pParams, 
	X_XACTEngine** ppEngine
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pParams)
		LOG_FUNC_ARG(ppEngine)
		LOG_FUNC_END;

	// TODO: Any other form of initialization?

	*ppEngine = (X_XACTEngine*)g_VMManager.AllocateZeroed(sizeof( X_XACTEngine ));

		
	
	RETURN(S_OK);
}

// ******************************************************************
// * EmuXACTEngineDoWork
// ******************************************************************
void WINAPI XTL::EMUPATCH(XACTEngineDoWork)()
{
	LOG_FUNC();

	// TODO: Anything else required here?
	// AFAIK, this function just calls DirectSoundDoWork()

	//
	//EmuDirectSoundDoWork();
	//

		
}

// ******************************************************************
// * EmuIXACTEngine_RegisterWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_RegisterWaveBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTWaveBank**	ppWaveBank
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pvData)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(ppWaveBank)
		LOG_FUNC_END;

	// TODO: Implement

	*ppWaveBank = (X_XACTWaveBank*)g_VMManager.AllocateZeroed(sizeof( X_XACTWaveBank ));

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_RegisterStreamedWaveBank)
(
	X_XACTEngine*							pThis,
	X_XACT_WAVEBANK_STREAMING_PARAMETERS*	pParams,
	X_XACTWaveBank**						ppWaveBank
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pParams)
		LOG_FUNC_ARG(ppWaveBank)
		LOG_FUNC_END;

	// TODO: Implement

	*ppWaveBank = (X_XACTWaveBank*)g_VMManager.AllocateZeroed(sizeof( X_XACTWaveBank ));

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_CreateSoundBank
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_CreateSoundBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTSoundBank**	ppSoundBank
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pvData)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(ppSoundBank)
		LOG_FUNC_END;

	// TODO: Implement

	*ppSoundBank = (X_XACTSoundBank*)g_VMManager.AllocateZeroed(sizeof( X_XACTSoundBank ));

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_DownloadEffectsImage)
(
	X_XACTEngine*		pThis,
	PVOID				pvData,
	DWORD				dwSize,
	LPVOID				pEffectLoc,
	LPVOID*				ppImageDesc
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pvData)
		LOG_FUNC_ARG(dwSize)
		LOG_FUNC_ARG(pEffectLoc)
		LOG_FUNC_ARG(ppImageDesc)
		LOG_FUNC_END;

	// TODO: Implement

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_CreateSoundSource
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_CreateSoundSource)
(
	X_XACTEngine*		pThis,
	DWORD				dwFlags,
	X_XACTSoundSource** ppSoundSource
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(ppSoundSource)
		LOG_FUNC_END;

	*ppSoundSource = (X_XACTSoundSource*)g_VMManager.AllocateZeroed(sizeof( X_XACTSoundSource ));

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_EnableHeadphones)
(
	X_XACTEngine*		pThis,
	BOOL				fEnabled
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fEnabled)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_SetListenerOrientation)
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
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(xFront)
		LOG_FUNC_ARG(yFront)
		LOG_FUNC_ARG(zFront)
		LOG_FUNC_ARG(xTop)
		LOG_FUNC_ARG(yTop)
		LOG_FUNC_ARG(zTop)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_SetListenerPosition)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_SetListenerVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_SetListenerVelocity)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_SetMasterVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_SetMasterVolume)
(
	X_XACTEngine*	pThis,
    WORD			wCategory,
    LONG			lVolume
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(wCategory)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_CommitDeferredSettings)
(
	X_XACTEngine* pThis
)
{
	LOG_FUNC_ONE_ARG(pThis);

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTSoundBank_GetSoundCueIndexFromFriendlyName)
(
	X_XACTEngine*		pThis,
	PCSTR				pFriendlyName,
	DWORD*				pdwSoundCueIndex
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pFriendlyName)
		LOG_FUNC_ARG(pdwSoundCueIndex)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTSoundBank_Play
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTSoundBank_Play)
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	X_XACTSoundSource*	pSoundSource,
	DWORD				dwFlags,
	X_XACTSoundCue**	ppSoundCue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwSoundCueIndex)
		LOG_FUNC_ARG(pSoundSource)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(ppSoundCue)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTSoundBank_Stop
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTSoundBank_Stop)
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	DWORD				dwFlags,
	X_XACTSoundCue*		pSoundCue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwSoundCueIndex)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(pSoundCue)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTSoundSource_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTSoundSource_SetPosition)
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTSoundSource_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTSoundSource_SetVelocity)
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_RegisterNotification
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_RegisterNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pNotificationDesc)
		LOG_FUNC_END;

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_GetNotification
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_GetNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc,
    LPVOID						    pNotification
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pNotificationDesc)
		LOG_FUNC_ARG(pNotification)
		LOG_FUNC_END;

	// TODO: The contents of XACT_NOTIFICATION can vary from one XDK to the next.
	// The definition for 4627 is different than 5558.

	RETURN(S_OK);
}

// ******************************************************************
// * EmuIXACTEngine_UnRegisterWaveBank
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IXACTEngine_UnRegisterWaveBank)
(
	X_XACTEngine*	pThis,
    X_XACTWaveBank*	pWaveBank
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pWaveBank)
		LOG_FUNC_END;


	// Even though the documentation doesn't tell us much, I'm
	// assuming that after this function is called, the pointer
	// to IXACTWaveBank is released.

//	if(pWaveBank)
//		g_VMManager.Deallocate((VAddr)pWaveBank);

	RETURN(S_OK);
}
