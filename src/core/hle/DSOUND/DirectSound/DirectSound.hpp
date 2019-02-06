// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   src->core->HLE->DSound->DirectSound->DirectSound.h
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
// *  (c) 2017-2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUDSOUND_H
#define EMUDSOUND_H

#undef FIELD_OFFSET     // prevent macro redefinition warnings

#include <dsound.h>
#include "core\kernel\init\CxbxKrnl.h"
#include "..\XbDSoundTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void CxbxInitAudio();

#ifdef __cplusplus
}
#endif

// ******************************************************************
// * X_CDirectSound
// ******************************************************************
struct X_CDirectSound
{
    // TODO: Fill this in?
};

enum X_DSB_TOGGLE {
    X_DSB_TOGGLE_DEFAULT = 0,
    X_DSB_TOGGLE_PLAY,
    X_DSB_TOGGLE_LOOP
};

typedef struct _DSoundBuffer_Lock {
    PVOID   pLockPtr1;
    DWORD   dwLockBytes1;
    PVOID   pLockPtr2;
    DWORD   dwLockBytes2;
    DWORD   dwLockOffset;
    DWORD   dwLockFlags;
} DSoundBuffer_Lock;

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
    LPVOID                  X_BufferCache;      // Offset: 0x28
    DSBUFFERDESC            EmuBufferDesc;      // Offset: 0x2C
    /*LPVOID                  EmuLockPtr1;        // Offset: 0x30
    DWORD                   EmuLockBytes1;      // Offset: 0x34
    LPVOID                  EmuLockPtr2;        // Offset: 0x38
    DWORD                   EmuLockBytes2;      // Offset: 0x3C*/
    DWORD                   EmuPlayFlags;       // Offset: 0x40
    DWORD                   EmuFlags;           // Offset: 0x44
    LPDIRECTSOUND3DBUFFER8  EmuDirectSound3DBuffer8;
    //DWORD                   EmuLockOffset;
    //DWORD                   EmuLockFlags;
    // Play/Loop Region Section
    X_DSB_TOGGLE            EmuBufferToggle;
    DWORD                   EmuRegionLoopStartOffset;
    DWORD                   EmuRegionLoopLength;
    DWORD                   EmuRegionPlayStartOffset;
    DWORD                   EmuRegionPlayLength;
    DWORD                   X_BufferCacheSize;
    DSoundBuffer_Lock       Host_lock;
    DSoundBuffer_Lock       X_lock;
    REFERENCE_TIME          Xb_rtPauseEx;
    REFERENCE_TIME          Xb_rtStopEx;
    LONG                    Xb_Volume;
    LONG                    Xb_VolumeMixbin;
    DWORD                   Xb_dwHeadroom;
    X_DSENVOLOPEDESC        Xb_EnvolopeDesc;
};

#define WAVE_FORMAT_XBOX_ADPCM 0x0069
//Custom flags (4 bytes support up to 31 shifts,starting from 0)
#define DSE_FLAG_PCM                    (1 << 0)
#define DSE_FLAG_XADPCM                 (1 << 1)
#define DSE_FLAG_PCM_UNKNOWN            (1 << 2)
#define DSE_FLAG_SYNCHPLAYBACK_CONTROL  (1 << 10)
#define DSE_FLAG_PAUSE                  (1 << 11)
#define DSE_FLAG_FLUSH_ASYNC            (1 << 12)
#define DSE_FLAG_ENVELOPE               (1 << 13)
#define DSE_FLAG_ENVELOPE2              (1 << 14) // NOTE: This flag is a requirement for GetStatus to return X_DSSSTATUS_ENVELOPECOMPLETE value.
#define DSE_FLAG_RECIEVEDATA            (1 << 20)
#define DSE_FLAG_DEBUG_MUTE             (1 << 30) // Cxbx-R debugging usage only
#define DSE_FLAG_BUFFER_EXTERNAL        (1 << 31)
#define DSE_FLAG_AUDIO_CODECS           (DSE_FLAG_PCM | DSE_FLAG_XADPCM | DSE_FLAG_PCM_UNKNOWN)

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

// host_voice_packet is needed for DirectSoundStream packet handling internally.
struct host_voice_packet {
    XTL::XMEDIAPACKET xmp_data;
    PVOID   pBuffer_data;
    DWORD   rangeStart;
    bool    isWritten;
    bool    isPlayed;
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
        LPDIRECTSOUNDBUFFER8                    EmuDirectSoundBuffer8;
        LPDIRECTSOUND3DBUFFER8                  EmuDirectSound3DBuffer8;
        PVOID                                   X_BufferCache; // Not really needed...
        DSBUFFERDESC                            EmuBufferDesc;
        PVOID                                   EmuLockPtr1;
        DWORD                                   EmuLockBytes1;
        PVOID                                   EmuLockPtr2;
        DWORD                                   EmuLockBytes2;
        DWORD                                   EmuPlayFlags;
        DWORD                                   EmuFlags;
        DWORD                                   X_BufferCacheSize; // Not really needed...
        DWORD                                   X_MaxAttachedPackets;
        std::vector<struct host_voice_packet>   Host_BufferPacketArray;
        DWORD                                   Host_dwWriteOffsetNext;
        DWORD                                   Host_dwTriggerRange;
        bool                                    Host_isProcessing;
        LPFNXMOCALLBACK                         Xb_lpfnCallback;
        LPVOID                                  Xb_lpvContext;
        REFERENCE_TIME                          Xb_rtFlushEx;
        REFERENCE_TIME                          Xb_rtPauseEx;
        LONG                                    Xb_Volume;
        LONG                                    Xb_VolumeMixbin;
        DWORD                                   Xb_dwHeadroom;
        X_DSENVOLOPEDESC                        Xb_EnvolopeDesc;
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

extern "C" {

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
    X_DSI3DL2LISTENER      *pds3dl,
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
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
(
    X_CDirectSoundBuffer*   pThis,
    X_LPDSMIXBINS           pMixBins
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
// * patch: IDirectSoundBuffer_Unlock
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_Unlock)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  ppvAudioPtr1,
    DWORD                   pdwAudioBytes1,
    LPVOID                  ppvAudioPtr2,
    DWORD                   pdwAudioBytes2
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
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_StopEx)
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
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_PlayEx)
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
    X_DS3DBUFFER*           pc3DBufferParameters,
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
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    PVOID   pMixBins
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

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    X_CDirectSoundBuffer*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
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
void WINAPI EMUPATCH(DirectSoundUseFullHRTF)(void);

// ******************************************************************
// * patch: DirectSoundUseLightHRTF
// ******************************************************************
void WINAPI EMUPATCH(DirectSoundUseLightHRTF)(void);

// ******************************************************************
// * patch: DirectSoundUseFullHRTF4Channel
// ******************************************************************
void WINAPI EMUPATCH(DirectSoundUseFullHRTF4Channel)(void);

// ******************************************************************
// * patch: DirectSoundUseLightHRTF4Channel
// ******************************************************************
void WINAPI EMUPATCH(DirectSoundUseLightHRTF4Channel)(void);

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
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetVolume)
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
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_PauseEx)
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
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_FlushEx
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_FlushEx)
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
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc
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
    X_CDirectSoundStream*   pThis,
    LONG                    lPitch
);

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI EMUPATCH(DirectSoundGetSampleTime)();

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12)
(
    X_CDirectSoundStream*    pThis,
    DWORD                    dwMixBinMask,
    const LONG*              alVolumes
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*    pThis,
    X_LPDSMIXBINS            pMixBins
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    X_CDirectSoundBuffer*    pThis,
    X_DS3DBUFFER*            pc3DBufferParameters,
    DWORD                    dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*   pThis,
    LPCWAVEFORMATEX         pwfxFormat
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
    LPCSTR              pszFileName,
    LPCWAVEFORMATEX    *ppwfxFormat,
    void              **ppMediaObject // XFileMediaObject
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_SetEG)
(
    X_CDirectSoundBuffer*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc
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

// ******************************************************************
// * patch: IDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_GetSpeakerConfig)
    (X_CDirectSound* pThis, OUT LPDWORD* pdwSpeakerConfig);

// ******************************************************************
// * patch: IDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_CommitDeferredSettings)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: IDirectSound_CommitEffectData
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSound_CommitEffectData)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: CDirectSoundStream_PauseEx
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause);

// ******************************************************************
// * patch: XFileCreaeMediaObject
// ******************************************************************
HRESULT WINAPI EMUPATCH(XFileCreateMediaObject)
(
    LPCSTR              pstrFileName,
    DWORD               dwDesiredAccess,
    DWORD               dwShareMode,
    DWORD               dwCreationDisposition,
    DWORD               dwFlagsAndAttributes,
    OUT void**          ppMediaObject);

// ******************************************************************
// * patch: XWaveFileCreateMediaObjectEx
// ******************************************************************
HRESULT WINAPI EMUPATCH(XWaveFileCreateMediaObjectEx)
(
    LPCSTR              pszFileName,
    HANDLE              hFile,
    OUT void**          ppMediaObject); //XWaveFileMediaObject

// ******************************************************************
// * patch: XAudioSetEffectData
// ******************************************************************
HRESULT WINAPI EMUPATCH(XAudioSetEffectData)
(
    DWORD               dwEffectIndex,
    void*               pDesc,
    void*               pRawDesc);

// ******************************************************************
// * patch: CDirectSoundStream_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetDistanceFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply);

// ******************************************************************
// * patch: CDirectSoundStream_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_SetDopplerFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundBuffer_GetVoiceProperties)
(
    X_CDirectSoundBuffer*   pThis,
    OUT void*               pVoiceProps);

// ******************************************************************
// * patch: CDirectSoundStream_GetVoiceProperties
// ******************************************************************
HRESULT WINAPI EMUPATCH(CDirectSoundStream_GetVoiceProperties)
(
    X_CDirectSoundStream*   pThis,
    OUT void*               pVoiceProps);

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lVolume);

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lPitch);

// ******************************************************************
// * patch: IDirectSoundStream_SetLFO
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwHeadroom);

// ******************************************************************
// * patch: IDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwFrequency);

// ******************************************************************
// * patch: IDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI EMUPATCH(IDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    PVOID                   pMixBins);
#endif
}
