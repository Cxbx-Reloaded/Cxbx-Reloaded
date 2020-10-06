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
#ifndef EMUXACTENG_H
#define EMUXACTENG_H

#include "common\xbe\Xbe.h"
#include "core\hle\XAPI\Xapi.h" // For EMUPATCH

#include <windows.h>

#include <dsound.h>

namespace xbox {

// ******************************************************************
// * XACT Interfaces
// ******************************************************************
struct X_XACTEngine;
struct X_XACTWaveBank;
struct X_XACTSoundBank;
struct X_XACTSoundSource;
struct X_XACTSoundCue;

// ******************************************************************
// * X_XACT_RUNTIME_PARAMETERS
// ******************************************************************
struct X_XACT_RUNTIME_PARAMETERS 
{
    DWORD	dwMax2DHwVoices;
    DWORD   dwMax3DHwVoices;
    DWORD   dwMaxConcurrentStreams;
    DWORD   dwMaxNotifications;
    DWORD   dwInteractiveAudioLookaheadTime; // Time in ms
};

// ******************************************************************
// * X_XACT_WAVEBANK_STREAMING_PARAMETERS
// ******************************************************************
struct X_XACT_WAVEBANK_STREAMING_PARAMETERS 
{
    HANDLE                      hFile;                          // file handle associated with wavebank data
    DWORD                       dwOffset;                       // offset within file of wavebank header
    DWORD                       dwPacketSizeInMilliSecs;        // stream packet size to use for each stream (in ms)
};

// ******************************************************************
// * X_XACT_NOTIFICATION_DESCRIPTION
// ******************************************************************
struct X_XACT_NOTIFICATION_DESCRIPTION
{
    WORD				wType;
    WORD				wFlags;
    X_XACTSoundBank*	pSoundBank;
    X_XACTWaveBank*		pWaveBank;
    DWORD				dwSoundCueIndex;
    X_XACTSoundCue*		pSoundCue;
    PVOID				pvContext;
    HANDLE				hEvent;
};

typedef const X_XACT_NOTIFICATION_DESCRIPTION *PCXACT_NOTIFICATION_DESCRIPTION;

//struct X_XACT_NOTIFICATION
//{
//    X_XACT_NOTIFICATION_DESCRIPTION Header;
//    X_XACT_NOTIFICATION_UNION Data;
//    REFERENCE_TIME rtTimeStamp;
//};
//
//typedef const X_XACT_NOTIFICATION *PXACT_NOTIFICATION;

// ******************************************************************
// * X_XACTEngine
// ******************************************************************
struct X_XACTEngine
{
	// TODO: Fill this in?
};

// ******************************************************************
// * X_XACTWaveBank
// ******************************************************************
struct X_XACTWaveBank
{
	// TODO: Fill this in?
};

// ******************************************************************
// * X_XACTSoundBank
// ******************************************************************
struct X_XACTSoundBank
{
	// TODO: Fill this in?
};

// ******************************************************************
// * X_XACTSoundSource
// ******************************************************************
struct X_XACTSoundSource
{
	// TODO: Fill this in?
};

// ******************************************************************
// * X_XACTSoundCue
// ******************************************************************
struct X_XACTSoundCue
{
	// TODO: Fill this in?
};


// ******************************************************************
// * EmuXACTEngineCreate
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XACTEngineCreate)( X_XACT_RUNTIME_PARAMETERS* pParams, X_XACTEngine** ppEngine );

// ******************************************************************
// * EmuXACTEngineDoWork
// ******************************************************************
void WINAPI EMUPATCH(XACTEngineDoWork)();

// ******************************************************************
// * EmuIXACTEngine_RegisterWaveBank
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_RegisterWaveBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	dword_xt				dwSize,
	X_XACTWaveBank**	ppWaveBank
);

// ******************************************************************
// * EmuIXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_RegisterStreamedWaveBank)
(
	X_XACTEngine*							pThis,
	X_XACT_WAVEBANK_STREAMING_PARAMETERS*	pParams,
	X_XACTWaveBank**						ppWaveBank
);

// ******************************************************************
// * EmuIXACTEngine_CreateSoundBank
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_CreateSoundBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	dword_xt				dwSize,
	X_XACTSoundBank**	ppSoundBank
);

// ******************************************************************
// * EmuIXACTEngine_DownloadEffectsImage
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_DownloadEffectsImage)
(
	X_XACTEngine*		pThis,
	PVOID				pvData,
	dword_xt				dwSize,
	LPVOID				pEffectLoc,
	LPVOID*				ppImageDesc
);

// ******************************************************************
// * EmuIXACTEngine_CreateSoundSource
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_CreateSoundSource)
(
	X_XACTEngine*		pThis,
	dword_xt				dwFlags,
	X_XACTSoundSource** ppSoundSource
);

// ******************************************************************
// * EmuIXACTEngine_EnableHeadphones
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_EnableHeadphones)
(
	X_XACTEngine*		pThis,
	bool_xt				fEnabled
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerOrientation
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_SetListenerOrientation)
(
	X_XACTEngine*	pThis,
	float			xFront,
	float			yFront,
	float			zFront,
	float			xTop,
	float			yTop,
	float			zTop,
	dword_xt			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_SetListenerPosition)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	dword_xt			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerVelocity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_SetListenerVelocity)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	dword_xt			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetMasterVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_SetMasterVolume)
(
	X_XACTEngine*	pThis,
    word_xt			wCategory,
    long_xt			lVolume
);

// ******************************************************************
// * EmuIXACTEngine_CommitDeferredSettings
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_CommitDeferredSettings)(X_XACTEngine* pThis);

// ******************************************************************
// * EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTSoundBank_GetSoundCueIndexFromFriendlyName)
(
	X_XACTEngine*		pThis,
	PCSTR				pFriendlyName,
	dword_xt*				pdwSoundCueIndex
);

// ******************************************************************
// * EmuIXACTSoundBank_Play
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTSoundBank_Play)
(
	X_XACTSoundBank*	pThis,
	dword_xt				dwSoundCueIndex,
	X_XACTSoundSource*	pSoundSource,
	dword_xt				dwFlags,
	X_XACTSoundCue**	ppSoundCue
);

// ******************************************************************
// * EmuIXACTSoundBank_Stop
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTSoundBank_Stop)
(
	X_XACTSoundBank*	pThis,
	dword_xt				dwSoundCueIndex,
	dword_xt				dwFlags,
	X_XACTSoundCue*		pSoundCue
);

// ******************************************************************
// * EmuIXACTSoundSource_SetPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTSoundSource_SetPosition)
(
	X_XACTSoundSource*	pThis,
    float_xt				x,
    float_xt				y,
    float_xt				z,
    dword_xt				dwApply
);

// ******************************************************************
// * EmuIXACTSoundSource_SetVelocity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTSoundSource_SetVelocity)
(
	X_XACTSoundSource*	pThis,
    float_xt				x,
    float_xt				y,
    float_xt				z,
    dword_xt				dwApply
);

// ******************************************************************
// * EmuIXACTEngine_RegisterNotification
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_RegisterNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc
);

// ******************************************************************
// * EmuIXACTEngine_GetNotification
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_GetNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc,
    LPVOID						    pNotification
);

// ******************************************************************
// * EmuIXACTEngine_UnRegisterWaveBank
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IXACTEngine_UnRegisterWaveBank)
(
	X_XACTEngine*	pThis,
    X_XACTWaveBank*	pWaveBank
);

} // end of namespace xbox

#endif
