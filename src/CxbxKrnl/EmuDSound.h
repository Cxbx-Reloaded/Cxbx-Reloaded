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

#undef FIELD_OFFSET     // prevent macro redefinition warnings

#include <dsound.h>
#include "CxbxKrnl.h"
#include "Emu.h"

#ifdef __cplusplus
extern "C" {
#endif

void CxbxInitAudio();

#ifdef __cplusplus
}
#endif

// EmuIDirectSoundBuffer_Play flags
#define X_DSBPLAY_LOOPING       0x00000001
#define X_DSBPLAY_FROMSTART     0x00000002
#define X_DSBPLAY_SYNCHPLAYBACK 0x00000004

// EmuIDirectSoundBuffer_Pause flags
#define X_DSBPAUSE_RESUME             0x00000000
#define X_DSBPAUSE_PAUSE              0x00000001
#define X_DSBPAUSE_SYNCHPLAYBACK      0x00000002

// EmuIDirectSoundStream_Pause flags
#define X_DSSPAUSE_RESUME             0x00000000
#define X_DSSPAUSE_PAUSE              0x00000001
#define X_DSSPAUSE_SYNCHPLAYBACK      0x00000002
#define X_DSSPAUSE_PAUSENOACTIVATE    0x00000003


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
    DWORD dwAnalogLeftTotalPeak;    // analog peak
    DWORD dwAnalogRightTotalPeak;
    DWORD dwAnalogLeftTotalRMS;        // analog RMS
    DWORD dwAnalogRightTotalRMS;
    DWORD dwDigitalFrontLeftPeak;    // digital peak levels
    DWORD dwDigitalFrontCenterPeak;
    DWORD dwDigitalFrontRightPeak;
    DWORD dwDigitalBackLeftPeak;
    DWORD dwDigitalBackRightPeak;
    DWORD dwDigitalLowFrequencyPeak;
    DWORD dwDigitalFrontLeftRMS;    // digital RMS levels
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
        PVOID                   pMpcxBuffer;
        LPDIRECTSOUNDBUFFER8    EmuDirectSoundBuffer8;
    };

    BYTE                    UnknownB[0x0C];     // Offset: 0x24
    PVOID                   EmuBuffer;          // Offset: 0x28
    DSBUFFERDESC*           EmuBufferDesc;      // Offset: 0x2C
    PVOID                   EmuLockPtr1;        // Offset: 0x30
    DWORD                   EmuLockBytes1;      // Offset: 0x34
    PVOID                   EmuLockPtr2;        // Offset: 0x38
    DWORD                   EmuLockBytes2;      // Offset: 0x3C
    DWORD                   EmuPlayFlags;       // Offset: 0x40
    DWORD                   EmuFlags;           // Offset: 0x44
    LPDIRECTSOUND3DBUFFER8  EmuDirectSound3DBuffer8;
    DWORD                   EmuLockOffset;
    DWORD                   EmuLockFlags;
};

#define WAVE_FORMAT_XBOX_ADPCM 0x0069
//Custom flags?
#define DSB_FLAG_PCM                    0x00000001
#define DSB_FLAG_XADPCM                 0x00000002
#define DSB_FLAG_PCM_UNKNOWN            0x00000010
#define DSB_FLAG_SYNCHPLAYBACK_CONTROL  0x00000100
#define DSB_FLAG_RECIEVEDATA            0x00001000
#define DSB_FLAG_AUDIO_CODECS           (DSB_FLAG_PCM | DSB_FLAG_XADPCM | DSB_FLAG_PCM_UNKNOWN)

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
            
            HRESULT (WINAPI *GetInfo)                                       // 0x08
            (
                X_CDirectSoundStream*   pThis,
                XMEDIAINFO*             pInfo
            );

            HRESULT (WINAPI *GetStatus)                                     // 0x0C
            (
                X_CDirectSoundStream*   pThis,
                LPDWORD                 pdwStatus
            );

            HRESULT (WINAPI *Process)                                       // 0x10
            (
                X_CDirectSoundStream*   pThis,
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
        LPDIRECTSOUNDBUFFER8        EmuDirectSoundBuffer8;
        LPDIRECTSOUND3DBUFFER8      EmuDirectSound3DBuffer8;
        PVOID                       EmuBuffer;
        LPDSBUFFERDESC              EmuBufferDesc;
        PVOID                       EmuLockPtr1;
        DWORD                       EmuLockBytes1;
        PVOID                       EmuLockPtr2;
        DWORD                       EmuLockBytes2;
        DWORD                       EmuPlayFlags;
        DWORD                       EmuFlags;
};

// ******************************************************************
// * X_XFileMediaObject
// ******************************************************************
class X_XFileMediaObject
{
    public:
    X_XFileMediaObject() : pVtbl(&vtbl) { EmuRefCount = 1; }

    private:
        // vtable (cached by each instance, via constructor)
        struct _vtbl
        {
            ULONG (WINAPI *AddRef)(X_XFileMediaObject *pThis);            // 0x00
            ULONG (WINAPI *Release)(X_XFileMediaObject *pThis);           // 0x04

            HRESULT (WINAPI *GetInfo)                                        // 0x08
            (
                X_XFileMediaObject*     pThis,
                XMEDIAINFO*             pInfo
            );
                                                                                                              
            HRESULT (WINAPI *GetStatus)                                     // 0x0C
            (
                X_XFileMediaObject*     pThis,
                LPDWORD                 pdwStatus
            );
 
            HRESULT (WINAPI *Process)                                       // 0x10
            (
                X_XFileMediaObject*     pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );
           
            HRESULT (WINAPI *Discontinuity)(X_XFileMediaObject *pThis);        // 0x14
                                                                           
            DWORD Unknown7;
/*
            HRESULT (WINAPI *Flush)(X_XFileMediaObject *pThis);                // 0x18
*/
            HRESULT (WINAPI *Seek)                                            // 0x1C
            (
                X_XFileMediaObject*     pThis,
                LONG                    lOffset, 
                DWORD                   dwOrigin, 
                LPDWORD                 pdwAbsolute
            );
/*
            HRESULT (WINAPI *GetLength)                                        // 0x20
            (
                X_XFileMediaObject       *pThis,
                LPDWORD                    pdwLength
            );*/

            DWORD Unknown9;

            void (WINAPI *DoWork)(X_XFileMediaObject *pThis);                // 0x24
        }
        *pVtbl;

        // global vtbl for this class
        static _vtbl vtbl;

        // debug mode guard for detecting naughty data accesses
        #ifdef _DEBUG
        DWORD DebugGuard[256];
        #endif

    public:
        // TODO: More?
        LPVOID                EmuBuffer;
        HANDLE                EmuHandle;
        UINT                EmuRefCount;
};

// ******************************************************************
// * patch: DirectSoundCreate
// ******************************************************************
HRESULT WINAPI EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * patch: DirectSoundDoWork
// ******************************************************************
VOID WINAPI EMUPATCH(DirectSoundDoWork)();

// ******************************************************************
// * patch: IDirectSound_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSound_AddRef)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: IDirectSound_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSound_Release)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSound_GetSpeakerConfig)
(
    X_CDirectSound*         pThis,
    PDWORD                  pdwSpeakerConfig
);

// ******************************************************************
// * patch: IDirectSound8_EnableHeadphones
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound8_EnableHeadphones)
(
    LPDIRECTSOUND8          pThis,
    BOOL                    fEnabled
);

// ******************************************************************
// * patch: IDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: IDirectSound_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_DownloadEffectsImage)
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    DWORD                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
);

// ******************************************************************
// * patch: IDirectSound_SetOrientation
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetOrientation)
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
// * patch: IDirectSound_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetDistanceFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetRolloffFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetDopplerFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetMixBinHeadroom)
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBins
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBins)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins    // TODO: fill this out
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes)
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes2)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins    // TODO: fill this out
);

// ******************************************************************
// * patch: IDirectSound_SetPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetPosition)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetVelocity
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetVelocity)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetAllParameters
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPCDS3DLISTENER         pDS3DListenerParameters,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSound_CommitDeferredSettings)
(
    X_CDirectSound*         pThis
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC*         pdsbd,
    X_CDirectSoundBuffer**  ppBuffer,
    LPUNKNOWN               pUnkOuter
);

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*         pdsbd,
    X_CDirectSoundBuffer**  ppBuffer
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetBufferData
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetBufferData)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetPlayRegion)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Lock
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Lock)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwOffset,
    DWORD                   dwBytes,
    LPVOID*                 ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID*                 ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    DWORD                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetHeadroom
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetHeadroom)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetLoopRegion)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSoundBuffer_Release)
(
    X_CDirectSoundBuffer*   pThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPitch
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetPitch)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lPitch
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    X_CDirectSoundBuffer*   pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetCurrentPosition)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwNewPosition
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    X_CDirectSoundBuffer*   pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Stop
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Stop)
(
    X_CDirectSoundBuffer*   pThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
extern "C" HRESULT __stdcall EMUPATCH(IDirectSoundBuffer_StopEx)
(
    X_CDirectSoundBuffer *pBuffer,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Play
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Play)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
extern "C" HRESULT __stdcall EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    X_CDirectSoundBuffer*   pBuffer,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFrequency
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetFrequency)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwFrequency
);

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundStream
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_CreateSoundStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown
);

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
VOID WINAPI EMUPATCH(CMcpxStream_Dummy_0x10)(DWORD dwDummy1, DWORD dwDummy2);

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI EMUPATCH(CDirectSoundStream_SetVolume)(X_CDirectSoundStream *pThis, LONG lVolume);

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    DWORD                 dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(CDirectSoundStream_AddRef)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(CDirectSoundStream_Release)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * CDirectSoundStream_GetInfo
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_GetInfo)
(
    X_CDirectSoundStream*    pThis, 
    LPXMEDIAINFO            pInfo
);

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_GetStatus)
(
    X_CDirectSoundStream*   pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream*   pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_Discontinuity)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_Flush)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_Pause)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwPause
);

// ******************************************************************
// * patch: CDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    LPCDS3DBUFFER           pc3DBufferParameters,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMaxDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMinDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetVelocity)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetPosition)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwFrequency
);

// ******************************************************************
// * patch: IDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetI3DL2Source)
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBins)
(
    PVOID   pThis,
    PVOID   pMixBins
);

// ******************************************************************
// * patch: IDirectSoundStream_Unknown1
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_Unknown1)
(
    PVOID   pThis,
    DWORD   dwUnknown1
);

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMaxDistance)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMinDistance
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMinDistance)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flRolloffFactor,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetDistanceFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeAngles
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetConeAngles)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOrientation)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPosition
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetPosition)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVelocity
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetVelocity)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetDopplerFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply
);

typedef void* LPCDSI3DL2BUFFER;

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    X_CDirectSoundBuffer*   pThis,
    LPCDSI3DL2BUFFER        pds3db,
    DWORD                   dwApply
);
// +s

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMode
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMode)
(
    X_CDirectSoundBuffer*   pBuffer,
    DWORD                   dwMode,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetFormat)
(
    X_CDirectSoundBuffer *pBuffer,
    LPCWAVEFORMATEX pwfxFormat
);

// ******************************************************************
// * patch: DirectSoundUseFullHRTF
// ******************************************************************
STDAPI_(void) EMUPATCH(DirectSoundUseFullHRTF)
(
    void
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLFO
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetLFO)
(
    LPDIRECTSOUNDBUFFER8 pThis,
    LPCDSLFODESC         pLFODesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetLFO
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream *pThis,
    LPCDSLFODESC         pLFODesc
);

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat
// ******************************************************************
VOID WINAPI EMUPATCH(XAudioCreateAdpcmFormat)
(
    WORD                   nChannels,
    DWORD                  nSamplesPerSec,
    LPXBOXADPCMWAVEFORMAT  pwfx
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffCurve)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    const FLOAT*            pflPoints,
    DWORD                   dwPointCount,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pStream,
    LONG                    lVolume
);

// ******************************************************************
// * patch: IDirectSound_EnableHeadphones
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_EnableHeadphones)
(
    LPDIRECTSOUND8      pThis,
    BOOL                fEnabled
);

// ******************************************************************
// * patch: IDirectSoundBuffer_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSoundBuffer_AddRef)
(
    X_CDirectSoundBuffer*   pThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Pause
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Pause)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwPause
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
extern "C" HRESULT __stdcall EmuIDirectSoundBuffer_PauseEx
(
    X_CDirectSoundBuffer*   pThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause
);

// ******************************************************************
// * patch: IDirectSound_GetOutputLevels
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_GetOutputLevels)
(
    LPDIRECTSOUND8*         pThis,
    X_DSOUTPUTLEVELS*       pOutputLevels,
    BOOL                    bResetPeakValues
);

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetEG)
(
    LPVOID        pThis,
    LPVOID        pEnvelopeDesc
);

// ******************************************************************
// * patch: IDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_Flush)();

// ******************************************************************
// * patch: IDirectSoundStream_FlushEx
// ******************************************************************
extern "C" HRESULT WINAPI EMUPATCH(IDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream*   pStream,
    DWORD                   dwMode,
    DWORD                   dwApply
);
    
// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR          pszImageName,
    LPVOID          pImageLoc,
    DWORD           dwFlags,
    LPVOID*         ppImageDesc
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFilter
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetFilter)
(
    LPVOID            pThis,
    X_DSFILTERDESC* pFilterDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*    pThis,
    X_DSFILTERDESC*            pFilterDesc
);

// ******************************************************************
// * patch: IDirectSound_GetCaps
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_GetCaps)
(
    X_CDirectSound*    pThis,
    X_DSCAPS*        pDSCaps
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetPitch)
(    
    X_CDirectSoundStream*    pThis,
    LONG                    lPitch
);

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI EMUPATCH(DirectSoundGetSampleTime)();

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes)
(
    X_CDirectSoundStream*    pThis,
    DWORD                    dwMixBinMask,
    const LONG*                alVolumes
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes2
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes2)
(
    X_CDirectSoundStream*    pThis,
    LPVOID                    pMixBins
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    PVOID   pds3db,
    DWORD   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    X_CDirectSoundBuffer*    pThis,
    LPCDS3DBUFFER            pc3DBufferParameters,
    DWORD                    dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*    pThis,
    LPCWAVEFORMATEX            pwfxFormat
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetOutputBuffer)
(
    X_CDirectSoundBuffer*    pThis,
    X_CDirectSoundBuffer*    pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
    X_CDirectSoundStream*    pThis,
    X_CDirectSoundBuffer*    pOutputBuffer
);

// ******************************************************************
// * patch: XFileCreaeMediaObjectEx
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileCreateMediaObjectEx)
(
    HANDLE    hFile,
    void**    ppMediaObject
);

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
HRESULT WINAPI EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR            pszFileName,
    LPCWAVEFORMATEX *ppwfxFormat,
    void            **ppMediaObject
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetEG)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  pEnvelopeDesc
);

// ******************************************************************
// * patch: IDirectSound_GetEffectData
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_GetEffectData)
(
    X_CDirectSound*     pThis,
    DWORD               dwEffectIndex,
    DWORD               dwOffset,
    LPVOID              pvData,
    DWORD               dwDataSize
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetNotificationPositions)
(
    X_CDirectSoundBuffer*    pThis,
    DWORD                    dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies
);

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
    X_CDirectSoundBuffer*   pThis,
    const FLOAT*            pflPoints,
    DWORD                   dwPointCount,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetEffectData
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_SetEffectData)
(
    LPVOID pThis,
    DWORD dwEffectIndex,
    DWORD dwOffset,
    LPCVOID pvData,
    DWORD dwDataSize,
    DWORD dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Use3DVoiceData)
(
    LPVOID pThis,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE    hFile,
    DWORD    dwMaxPackets,
    void    **ppMediaObject
);

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    LONG                lOffset,
    DWORD               dwOrigin,
    LPDWORD             pdwAbsolute
);

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
VOID WINAPI EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis);

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject*     pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileMediaObject_GetInfo)
(
    X_XFileMediaObject       *pThis,
    XMEDIAINFO               *pInfo
);

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileMediaObject_Process)
(
    X_XFileMediaObject       *pThis,
    LPXMEDIAPACKET            pInputBuffer, 
    LPXMEDIAPACKET            pOutputBuffer
);

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(XFileMediaObject_AddRef)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: XFileMediaObject_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(XFileMediaObject_Release)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: XFileMediaObject_Discontinuity
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileMediaObject_Discontinuity)(X_XFileMediaObject *pThis);

#endif
