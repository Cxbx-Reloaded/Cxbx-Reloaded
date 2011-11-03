// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDSound.h
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
#ifndef EMUDSOUND_H
#define EMUDSOUND_H

#include "Common/Xbe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include <dsound.h>

// EmuIDirectSoundBuffer8_Play flags
#define X_DSBPLAY_LOOPING       0x00000001
#define X_DSBPLAY_FROMSTART     0x00000002

// EmuIDirectSoundBuffer8_Pause flags
#define X_DSBPAUSE_RESUME             0x00000000
#define X_DSBPAUSE_PAUSE              0x00000001
#define X_DSBPAUSE_SYNCHPLAYBACK      0x00000002


// ******************************************************************
// * X_DSBUFFERDESC
// ******************************************************************
struct X_DSBUFFERDESC
{
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwBufferBytes;
    LPWAVEFORMATEX  lpwfxFormat;
    LPVOID          lpMixBins;      // TODO: Implement
    DWORD           dwInputMixBin;
};

// ******************************************************************
// * X_DSSTREAMDESC
// ******************************************************************
struct X_DSSTREAMDESC
{
    DWORD                       dwFlags;
    DWORD                       dwMaxAttachedPackets;
    LPWAVEFORMATEX              lpwfxFormat;
    PVOID                       lpfnCallback;   // TODO: Correct Parameter
    LPVOID                      lpvContext;
    PVOID                       lpMixBins;      // TODO: Correct Parameter
};

// ******************************************************************
// * REFERENCE_TIME
// ******************************************************************
typedef LONGLONG REFERENCE_TIME, *PREFERENCE_TIME, *LPREFERENCE_TIME;

// ******************************************************************
// * XMEDIAPACKET
// ******************************************************************
typedef struct _XMEDIAPACKET
{
    LPVOID pvBuffer;
    DWORD dwMaxSize;
    PDWORD pdwCompletedSize;
    PDWORD pdwStatus;
    union {
        HANDLE  hCompletionEvent;
        PVOID  pContext;
    };
    PREFERENCE_TIME prtTimestamp;
}
XMEDIAPACKET, *PXMEDIAPACKET, *LPXMEDIAPACKET;

// ******************************************************************
// * XMEDIAINFO
// ******************************************************************
typedef struct _XMEDIAINFO
{
    DWORD  dwFlags;
    DWORD  dwInputSize;
    DWORD  dwOutputSize;
    DWORD  dwMaxLookahead;
} 
XMEDIAINFO, *PXEIDIAINFO, *LPXMEDIAINFO;

// XMEDIAINFO Flags
#define XMO_STREAMF_FIXED_SAMPLE_SIZE           0x00000001      // The object supports only a fixed sample size
#define XMO_STREAMF_FIXED_PACKET_ALIGNMENT      0x00000002      // The object supports only a fixed packet alignment
#define XMO_STREAMF_INPUT_ASYNC                 0x00000004      // The object supports receiving input data asynchronously
#define XMO_STREAMF_OUTPUT_ASYNC                0x00000008      // The object supports providing output data asynchronously
#define XMO_STREAMF_IN_PLACE                    0x00000010      // The object supports in-place modification of data
#define XMO_STREAMF_MASK                        0x0000001F

// ******************************************************************
// * X_DSFILTERDESC
// ******************************************************************
struct X_DSFILTERDESC
{
    DWORD dwMode;
    DWORD dwQCoefficient;
    DWORD adwCoefficients[4];
};

// X_DSFILTERDESC modes
#define DSFILTER_MODE_BYPASS        0x00000000      // The filter is bypassed
#define DSFILTER_MODE_DLS2          0x00000001      // DLS2 mode
#define DSFILTER_MODE_PARAMEQ       0x00000002      // Parametric equalizer mode
#define DSFILTER_MODE_MULTI         0x00000003      // Multifunction mode

// ******************************************************************
// * DSLFODESC
// ******************************************************************
typedef struct _DSLFODESC
{
    DWORD dwLFO;
    DWORD dwDelay;
    DWORD dwDelta;
    LONG lPitchModulation;
    LONG lFilterCutOffRange;
    LONG lAmplitudeModulation;
}
DSLFODESC, *LPCDSLFODESC;

// ******************************************************************
// * XBOXADPCMWAVEFORMAT
// ******************************************************************
typedef struct xbox_adpcmwaveformat_tag
{
    WAVEFORMATEX    wfx;                    // WAVEFORMATEX data
    WORD            wSamplesPerBlock;       // Number of samples per encoded block.  It must be 64.
}
XBOXADPCMWAVEFORMAT, *PXBOXADPCMWAVEFORMAT, *LPXBOXADPCMWAVEFORMAT;

typedef const XBOXADPCMWAVEFORMAT *LPCXBOXADPCMWAVEFORMAT;

// ******************************************************************
// * X_DSOUTPUTLEVELS
// ******************************************************************
struct X_DSOUTPUTLEVELS
{
    DWORD dwAnalogLeftTotalPeak;	// analog peak
    DWORD dwAnalogRightTotalPeak;
    DWORD dwAnalogLeftTotalRMS;		// analog RMS
    DWORD dwAnalogRightTotalRMS;
    DWORD dwDigitalFrontLeftPeak;	// digital peak levels
    DWORD dwDigitalFrontCenterPeak;
    DWORD dwDigitalFrontRightPeak;
    DWORD dwDigitalBackLeftPeak;
    DWORD dwDigitalBackRightPeak;
    DWORD dwDigitalLowFrequencyPeak;
    DWORD dwDigitalFrontLeftRMS;	// digital RMS levels
    DWORD dwDigitalFrontCenterRMS;
    DWORD dwDigitalFrontRightRMS;
    DWORD dwDigitalBackLeftRMS;
    DWORD dwDigitalBackRightRMS;
    DWORD dwDigitalLowFrequencyRMS;
};

// ******************************************************************
// * X_DSCAPS
// ******************************************************************
struct X_DSCAPS
{                                                       
    DWORD dwFree2DBuffers;
    DWORD dwFree3DBuffers;
    DWORD dwFreeBufferSGEs;
    DWORD dwMemoryAllocated;
};



typedef struct IDirectSoundStream IDirectSoundStream;
typedef IDirectSoundStream *LPDIRECTSOUNDSTREAM;

// ******************************************************************
// * X_CDirectSound
// ******************************************************************
struct X_CDirectSound
{
    // TODO: Fill this in?
};

// ******************************************************************
// * X_CDirectSoundBuffer
// ******************************************************************
struct X_CDirectSoundBuffer
{
    BYTE    UnknownA[0x20];     // Offset: 0x00

    union                       // Offset: 0x20
    {
        PVOID                pMpcxBuffer;
        IDirectSoundBuffer  *EmuDirectSoundBuffer8;
    };

    BYTE            UnknownB[0x0C];     // Offset: 0x24
    PVOID           EmuBuffer;          // Offset: 0x28
    DSBUFFERDESC   *EmuBufferDesc;      // Offset: 0x2C
    PVOID           EmuLockPtr1;        // Offset: 0x30
    DWORD           EmuLockBytes1;      // Offset: 0x34
    PVOID           EmuLockPtr2;        // Offset: 0x38
    DWORD           EmuLockBytes2;      // Offset: 0x3C
    DWORD           EmuPlayFlags;       // Offset: 0x40
    DWORD           EmuFlags;           // Offset: 0x44
};

#define DSB_FLAG_ADPCM 0x00000001
#define WAVE_FORMAT_XBOX_ADPCM 0x0069
#define DSB_FLAG_RECIEVEDATA 0x00001000

// ******************************************************************
// * X_CMcpxStream
// ******************************************************************
class X_CMcpxStream
{
    public:
        // construct vtable (or grab ptr to existing)
        X_CMcpxStream(class X_CDirectSoundStream *pParentStream) : pVtbl(&vtbl), pParentStream(pParentStream) {};

    private:
        // vtable (cached by each instance, via constructor)
        struct _vtbl
        {
            DWORD Unknown1;                                                 // 0x00 - ???
            DWORD Unknown2;                                                 // 0x04 - ???
            DWORD Unknown3;                                                 // 0x08 - ???
            DWORD Unknown4;                                                 // 0x0C - ???

            //
            // TODO: Function needs X_CMcpxStream "this" pointer (ecx!)
            //

            VOID (WINAPI *Dummy_0x10)(DWORD dwDummy1, DWORD dwDummy2);   // 0x10
        }
        *pVtbl;

        // global vtbl for this class
        static _vtbl vtbl;

        // debug mode guard for detecting naughty data accesses
        #ifdef _DEBUG
        DWORD DebugGuard[256];
        #endif

    public:

        class X_CDirectSoundStream *pParentStream;
};

// ******************************************************************
// * X_CDirectSoundStream
// ******************************************************************
class X_CDirectSoundStream
{
    public:
        // construct vtable (or grab ptr to existing)
        X_CDirectSoundStream() : pVtbl(&vtbl) { pMcpxStream = new X_CMcpxStream(this); };

    private:
        // vtable (cached by each instance, via constructor)
        struct _vtbl
        {
            ULONG (WINAPI *AddRef)(X_CDirectSoundStream *pThis);            // 0x00
            ULONG (WINAPI *Release)(X_CDirectSoundStream *pThis);           // 0x04
            
			HRESULT (WINAPI *GetInfo)										// 0x08
			(
				X_CDirectSoundStream   *pThis,
				XMEDIAINFO			   *pInfo
			);

            HRESULT (WINAPI *GetStatus)                                     // 0x0C
            (
                X_CDirectSoundStream   *pThis,
                DWORD                  *pdwStatus
            );

            HRESULT (WINAPI *Process)                                       // 0x10
            (
                X_CDirectSoundStream   *pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );

            HRESULT (WINAPI *Discontinuity)(X_CDirectSoundStream *pThis);   // 0x14

            HRESULT (WINAPI *Flush)(X_CDirectSoundStream *pThis);           // 0x18

            DWORD Unknown2;                                                 // 0x1C - ???
            DWORD Unknown3;                                                 // 0x20 - ???
            DWORD Unknown4;                                                 // 0x24 - ???
            DWORD Unknown5;                                                 // 0x28 - ???
            DWORD Unknown6;                                                 // 0x2C - ???
            DWORD Unknown7;                                                 // 0x30 - ???
            DWORD Unknown8;                                                 // 0x34 - ???
            DWORD Unknown9;                                                 // 0x38 - ???
        }
        *pVtbl;

        // global vtbl for this class
        static _vtbl vtbl;

        DWORD Spacer[8];
        PVOID pMcpxStream;

        // debug mode guard for detecting naughty data accesses
        #ifdef _DEBUG
        DWORD DebugGuard[256];
        #endif

    public:
        // cached data
        XTL::IDirectSoundBuffer *EmuDirectSoundBuffer8;
        PVOID                    EmuBuffer;
        DSBUFFERDESC            *EmuBufferDesc;
        PVOID                    EmuLockPtr1;
        DWORD                    EmuLockBytes1;
        PVOID                    EmuLockPtr2;
        DWORD                    EmuLockBytes2;
        DWORD                    EmuPlayFlags;
};

// ******************************************************************
// * func: EmuDirectSoundCreate
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreate
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * func: EmuDirectSoundDoWork
// ******************************************************************
VOID WINAPI EmuDirectSoundDoWork();

// ******************************************************************
// * func: EmuIDirectSound8_AddRef
// ******************************************************************
ULONG WINAPI EmuIDirectSound8_AddRef
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * func: EmuIDirectSound8_Release
// ******************************************************************
ULONG WINAPI EmuIDirectSound8_Release
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * func: EmuCDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI EmuCDirectSound_GetSpeakerConfig
(
    X_CDirectSound         *pThis,
    PDWORD                  pdwSpeakerConfig
);

// ******************************************************************
// * func: EmuIDirectSound8_EnableHeadphones
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_EnableHeadphones
(
    LPDIRECTSOUND8          pThis,
    BOOL                    fEnabled
);

// ******************************************************************
// * func: EmuIDirectSound8_SynchPlayback
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SynchPlayback
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * func: EmuIDirectSound8_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_DownloadEffectsImage
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    DWORD                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
);

// ******************************************************************
// * func: EmuIDirectSound8_SetOrientation
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetOrientation
(
    LPDIRECTSOUND8  pThis,
    FLOAT           xFront,
    FLOAT           yFront,
    FLOAT           zFront,
    FLOAT           xTop,
    FLOAT           yTop,
    FLOAT           zTop,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetDistanceFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetRolloffFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetDopplerFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetI3DL2Listener
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetMixBinHeadroom
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetMixBins
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetMixBins
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins    // TODO: fill this out
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetMixBinVolumes
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins    // TODO: fill this out
);

// ******************************************************************
// * func: EmuIDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetPosition
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetVelocity
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetVelocity
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetAllParameters
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetAllParameters
(
    LPDIRECTSOUND8          pThis,
    LPVOID                  pTodo,  // TODO: LPCDS3DLISTENER
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuCDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI EmuCDirectSound_CommitDeferredSettings
(
    X_CDirectSound         *pThis
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateSoundBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer,
    LPUNKNOWN               pUnkOuter
);

// ******************************************************************
// * func: EmuDirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreateBuffer
(
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdssd,
    X_CDirectSoundBuffer  **ppBuffer,
    PVOID                   pUnknown
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetBufferData
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetBufferData
(
    X_CDirectSoundBuffer   *pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetPlayRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Lock
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Lock
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwOffset,
    DWORD                   dwBytes,
    LPVOID                 *ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID                 *ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    DWORD                   dwFlags
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetHeadroom
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetHeadroom
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetLoopRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Release
// ******************************************************************
ULONG WINAPI EmuIDirectSoundBuffer8_Release
(
    X_CDirectSoundBuffer   *pThis
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetPitch
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetPitch
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lPitch
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_GetStatus
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_GetStatus
(
    X_CDirectSoundBuffer   *pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetVolume
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetVolume
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lVolume
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwNewPosition
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_GetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Stop
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Stop
(
    X_CDirectSoundBuffer   *pThis
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_StopEx
// ******************************************************************
extern "C" HRESULT __stdcall EmuIDirectSoundBuffer8_StopEx
(
    X_CDirectSoundBuffer *pBuffer,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Play
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Play
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_PlayEx
// ******************************************************************
extern "C" HRESULT __stdcall EmuIDirectSoundBuffer8_PlayEx
(
    X_CDirectSoundBuffer *pBuffer,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetVolume
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetVolume
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lVolume
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetFrequency
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetFrequency
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwFrequency
);

// ******************************************************************
// * func: EmuDirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreateStream
(
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateStream
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateStream
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream,
    PVOID                   pUnknown
);

// ******************************************************************
// * func: EmuCMcpxStream_Dummy_0x10
// ******************************************************************
VOID WINAPI EmuCMcpxStream_Dummy_0x10(DWORD dwDummy1, DWORD dwDummy2);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_SetVolume(X_CDirectSoundStream *pThis, LONG lVolume);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetRolloffFactor
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    DWORD                 dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_AddRef(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_Release(X_CDirectSoundStream *pThis);

// ******************************************************************
// * EmuCDirectSoundStream_GetInfo
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_GetInfo
(
	X_CDirectSoundStream*	pThis, 
	LPXMEDIAINFO			pInfo
);

// ******************************************************************
// * func: EmuCDirectSoundStream_GetStatus
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_GetStatus
(
    X_CDirectSoundStream   *pThis,
    DWORD                  *pdwStatus
);

// ******************************************************************
// * func: EmuCDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Process
(
    X_CDirectSoundStream   *pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer
);

// ******************************************************************
// * func: EmuCDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Discontinuity(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Flush(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI EmuCDirectSound_SynchPlayback(PVOID pUnknown);

// ******************************************************************
// * func: EmuCDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Pause
(
    PVOID   pStream,
    DWORD   dwPause
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetHeadroom
(
    PVOID   pThis,
    DWORD   dwHeadroom
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetAllParameters
(
    PVOID   pThis,
    PVOID   pUnknown,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeAngles
(
    PVOID   pThis,
    DWORD   dwInsideConeAngle,
    DWORD   dwOutsideConeAngle,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeOutsideVolume
(
    PVOID   pThis,
    LONG    lConeOutsideVolume,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMaxDistance
(
    PVOID    pThis,
    D3DVALUE fMaxDistance,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMinDistance
(
    PVOID    pThis,
    D3DVALUE fMinDistance,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetVelocity
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeOrientation
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetPosition
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetFrequency
(
    PVOID   pThis,
    DWORD   dwFrequency
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetI3DL2Source
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMixBins
(
    PVOID   pThis,
    PVOID   pMixBins
);

// ******************************************************************
// * func: EmuIDirectSoundStream_Unknown1
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_Unknown1
(
    PVOID   pThis,
    DWORD   dwUnknown1
);

// s+
// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetMaxDistance
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetMaxDistance
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetMinDistance
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetMinDistance
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetRolloffFactor
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flRolloffFactor,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetDistanceFactor
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetConeAngles
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetConeAngles
(
    LPDIRECTSOUNDBUFFER8    pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetConeOrientation
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetConeOrientation
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetConeOutsideVolume
(
    LPDIRECTSOUNDBUFFER8    pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetPosition
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetVelocity
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetVelocity
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetDopplerFactor
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply
);

typedef void* LPCDSI3DL2BUFFER;

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetI3DL2Source
(
    LPDIRECTSOUNDBUFFER8    pThis,
    LPCDSI3DL2BUFFER        pds3db,
    DWORD                   dwApply
);
// +s

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetMode
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetMode
(
    X_CDirectSoundBuffer   *pBuffer,
    DWORD                   dwMode,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetFormat
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetFormat
(
    X_CDirectSoundBuffer *pBuffer,
    LPCWAVEFORMATEX pwfxFormat
);

// ******************************************************************
// * func: EmuDirectSoundUseFullHRTF
// ******************************************************************
STDAPI_(void) EmuDirectSoundUseFullHRTF
(
    void
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetLFO
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetLFO
(
    LPDIRECTSOUNDBUFFER  pThis,
    LPCDSLFODESC         pLFODesc
);

// ******************************************************************
// * func: EmuXAudioCreateAdpcmFormat
// ******************************************************************
VOID WINAPI EmuXAudioCreateAdpcmFormat
(
    WORD                   nChannels,
    DWORD                  nSamplesPerSec,
    LPXBOXADPCMWAVEFORMAT  pwfx
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetRolloffCurve
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetRolloffCurve
(
    LPDIRECTSOUNDBUFFER  pThis,
    const FLOAT         *pflPoints,
    DWORD                dwPointCount,
    DWORD                dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetVolume
(
    LPDIRECTSOUNDSTREAM pStream,
    LONG                lVolume
);

// ******************************************************************
// * func: EmuIDirectSound_EnableHeadphones
// ******************************************************************
HRESULT WINAPI EmuIDirectSound_EnableHeadphones
(
	LPDIRECTSOUND		pThis,
	BOOL				fEnabled
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_AddRef
// ******************************************************************
ULONG WINAPI EmuIDirectSoundBuffer8_AddRef
(
    X_CDirectSoundBuffer   *pThis
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Pause
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Pause
(
    X_CDirectSoundBuffer   *pThis,
	DWORD					dwPause
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_PauseEx
// ******************************************************************
extern "C" HRESULT __stdcall EmuIDirectSoundBuffer_PauseEx
(
    X_CDirectSoundBuffer   *pThis,
	REFERENCE_TIME			rtTimestamp,
	DWORD					dwPause
);

// ******************************************************************
// * func: EmuIDirectSound8_GetOutputLevels
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_GetOutputLevels
(
	LPDIRECTSOUND8		   *pThis,
	X_DSOUTPUTLEVELS	   *pOutputLevels,
	BOOL					bResetPeakValues
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetEG
(
	LPVOID		pThis,
	LPVOID		pEnvelopeDesc
);

// ******************************************************************
// * func: EmuIDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_Flush();

// ******************************************************************
// * func: EmuIDirectSoundStream_FlushEx
// ******************************************************************
extern "C" HRESULT WINAPI EmuIDirectSoundStream_FlushEx
(
	X_CDirectSoundStream*	pThis,
	REFERENCE_TIME			rtTimeStamp,
	DWORD					dwFlags
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMode
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMode
(
    X_CDirectSoundStream   *pStream,
    DWORD                   dwMode,
    DWORD                   dwApply
);
	
// ******************************************************************
// * func: EmuXAudioDownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EmuXAudioDownloadEffectsImage
(
    LPCSTR		pszImageName,
    LPVOID		pImageLoc,
    DWORD		dwFlags,
    LPVOID	   *ppImageDesc
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetFilter
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetFilter
(
	LPVOID			pThis,
	X_DSFILTERDESC* pFilterDesc
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetFilter
(
	X_CDirectSoundStream*	pThis,
	X_DSFILTERDESC*			pFilterDesc
);

// ******************************************************************
// * func: EmuIDirectSound8_GetCaps
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_GetCaps
(
	X_CDirectSound*	pThis,
    X_DSCAPS*		pDSCaps
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetPitch
(	
	X_CDirectSoundStream*	pThis,
    LONG					lPitch
);

// ******************************************************************
// * func: EmuDirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI EmuDirectSoundGetSampleTime();

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMixBinVolumes
(
	X_CDirectSoundStream*	pThis,
    DWORD					dwMixBinMask,
    const LONG*				alVolumes
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMixBinVolumes2
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMixBinVolumes2
(
    X_CDirectSoundStream*	pThis,
    LPVOID					pMixBins
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetI3DL2Source
(
    X_CDirectSoundStream*   pThis,
    PVOID   pds3db,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetAllParameters
(
	X_CDirectSoundBuffer*	pThis,
    VOID*					pcDs3dBuffer,
    DWORD					dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream::SetFormat
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetFormat
(
	X_CDirectSoundStream*	pThis,
    LPCWAVEFORMATEX			pwfxFormat
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetOutputBuffer
(
	X_CDirectSoundBuffer*	pThis,
    X_CDirectSoundBuffer*	pOutputBuffer
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetOutputBuffer
(
	X_CDirectSoundStream*	pThis,
	X_CDirectSoundBuffer*	pOutputBuffer
);

// ******************************************************************
// * func: EmuXFileCreaeMediaObjectEx
// ******************************************************************
HRESULT WINAPI EmuXFileCreateMediaObjectEx
(
    HANDLE	hFile,
    void**	ppMediaObject
);

// ******************************************************************
// * func: EmuXWaveFileCreateMediaObject
// ******************************************************************
HRESULT WINAPI EmuXWaveFileCreateMediaObject
(
    LPCSTR			pszFileName,
    LPCWAVEFORMATEX *ppwfxFormat,
    void			**ppMediaObject
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetEG
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetEG
(
	X_CDirectSoundBuffer*	pThis,
    LPVOID					pEnvelopeDesc
);

// ******************************************************************
// * func: EmuIDirectSound8_GetEffectData
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_GetEffectData
(
	X_CDirectSound*	pThis,
    DWORD			dwEffectIndex,
    DWORD			dwOffset,
    LPVOID			pvData,
    DWORD			dwDataSize
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetNotificationPositions
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetNotificationPositions
(
	X_CDirectSoundBuffer*	pThis,
    DWORD					dwNotifyCount,
    LPCDSBPOSITIONNOTIFY	paNotifies
);


#endif
