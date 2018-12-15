// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->core->HLE->XACTENG->XactEng.h
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

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include <dsound.h>


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
HRESULT WINAPI EMUPATCH(XACTEngineCreate)( X_XACT_RUNTIME_PARAMETERS* pParams, X_XACTEngine** ppEngine );

// ******************************************************************
// * EmuXACTEngineDoWork
// ******************************************************************
void WINAPI EMUPATCH(XACTEngineDoWork)();

// ******************************************************************
// * EmuIXACTEngine_RegisterWaveBank
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_RegisterWaveBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTWaveBank**	ppWaveBank
);

// ******************************************************************
// * EmuIXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_RegisterStreamedWaveBank)
(
	X_XACTEngine*							pThis,
	X_XACT_WAVEBANK_STREAMING_PARAMETERS*	pParams,
	X_XACTWaveBank**						ppWaveBank
);

// ******************************************************************
// * EmuIXACTEngine_CreateSoundBank
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_CreateSoundBank)
(
	X_XACTEngine*		pThis,
	LPVOID				pvData,
	DWORD				dwSize,
	X_XACTSoundBank**	ppSoundBank
);

// ******************************************************************
// * EmuIXACTEngine_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_DownloadEffectsImage)
(
	X_XACTEngine*		pThis,
	PVOID				pvData,
	DWORD				dwSize,
	LPVOID				pEffectLoc,
	LPVOID*				ppImageDesc
);

// ******************************************************************
// * EmuIXACTEngine_CreateSoundSource
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_CreateSoundSource)
(
	X_XACTEngine*		pThis,
	DWORD				dwFlags,
	X_XACTSoundSource** ppSoundSource
);

// ******************************************************************
// * EmuIXACTEngine_EnableHeadphones
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_EnableHeadphones)
(
	X_XACTEngine*		pThis,
	BOOL				fEnabled
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerOrientation
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_SetListenerOrientation)
(
	X_XACTEngine*	pThis,
	float			xFront,
	float			yFront,
	float			zFront,
	float			xTop,
	float			yTop,
	float			zTop,
	DWORD			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_SetListenerPosition)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetListenerVelocity
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_SetListenerVelocity)
(
	X_XACTEngine*	pThis,
	float			x,
	float			y, 
	float			z,
	DWORD			dwApply
);

// ******************************************************************
// * EmuIXACTEngine_SetMasterVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_SetMasterVolume)
(
	X_XACTEngine*	pThis,
    WORD			wCategory,
    LONG			lVolume
);

// ******************************************************************
// * EmuIXACTEngine_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_CommitDeferredSettings)(X_XACTEngine* pThis);

// ******************************************************************
// * EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTSoundBank_GetSoundCueIndexFromFriendlyName)
(
	X_XACTEngine*		pThis,
	PCSTR				pFriendlyName,
	DWORD*				pdwSoundCueIndex
);

// ******************************************************************
// * EmuIXACTSoundBank_Play
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTSoundBank_Play)
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	X_XACTSoundSource*	pSoundSource,
	DWORD				dwFlags,
	X_XACTSoundCue**	ppSoundCue
);

// ******************************************************************
// * EmuIXACTSoundBank_Stop
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTSoundBank_Stop)
(
	X_XACTSoundBank*	pThis,
	DWORD				dwSoundCueIndex,
	DWORD				dwFlags,
	X_XACTSoundCue*		pSoundCue
);

// ******************************************************************
// * EmuIXACTSoundSource_SetPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTSoundSource_SetPosition)
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
);

// ******************************************************************
// * EmuIXACTSoundSource_SetVelocity
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTSoundSource_SetVelocity)
(
	X_XACTSoundSource*	pThis,
    FLOAT				x,
    FLOAT				y,
    FLOAT				z,
    DWORD				dwApply
);

// ******************************************************************
// * EmuIXACTEngine_RegisterNotification
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_RegisterNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc
);

// ******************************************************************
// * EmuIXACTEngine_GetNotification
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_GetNotification)
(
	X_XACTEngine*					pThis,
    PCXACT_NOTIFICATION_DESCRIPTION pNotificationDesc,
    LPVOID						    pNotification
);

// ******************************************************************
// * EmuIXACTEngine_UnRegisterWaveBank
// ******************************************************************
HRESULT WINAPI EMUPATCH(IXACTEngine_UnRegisterWaveBank)
(
	X_XACTEngine*	pThis,
    X_XACTWaveBank*	pWaveBank
);

#endif
