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
// *  (c) 2017-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUDSOUND_H
#define EMUDSOUND_H

#include "core\kernel\init\CxbxKrnl.h"
#include "core\hle\DSOUND\XbDSoundTypes.h"
#include "core\hle\DSOUND\common\XbInternalStruct.hpp"

typedef struct IDirectSound3DListener8* LPDIRECTSOUND3DLISTENER8;
typedef struct IDirectSound3DBuffer8* LPDIRECTSOUND3DBUFFER8;

#ifdef __cplusplus
extern "C" {
#endif

void CxbxInitAudio();

#ifdef __cplusplus
}
#endif

namespace xbox {

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <dsound.h> // TODO: FIXME after global namespace xbox issue is resolved.

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
struct XbHybridDSBuffer;
struct EmuDirectSoundBuffer
{
    virtual ~EmuDirectSoundBuffer();
    XbHybridDSBuffer*       pHybridThis;

    union
    {
        PVOID                   pMpcxBuffer;
        LPDIRECTSOUNDBUFFER8    EmuDirectSoundBuffer8;
    };
    LPVOID                  X_BufferCache;
    DSBUFFERDESC            EmuBufferDesc;
    /*LPVOID                  EmuLockPtr1;
    DWORD                   EmuLockBytes1;
    LPVOID                  EmuLockPtr2;
    DWORD                   EmuLockBytes2;*/
    DWORD                   EmuPlayFlags;
    DWORD                   EmuFlags;
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
    LONG                    Xb_VolumeMixbin;
    X_DSENVOLOPEDESC        Xb_EnvolopeDesc;
    X_DSVOICEPROPS          Xb_VoiceProperties;
    DWORD                   Xb_Flags;
};

struct XbHybridDSBuffer : DSBUFFER_S::DSBUFFER_I {
    EmuDirectSoundBuffer*   emuDSBuffer;
};

struct SharedDSBuffer : DSBUFFER_S {
    SharedDSBuffer(bool is3D) : DSBUFFER_S(is3D) {
        emuDSBuffer = new EmuDirectSoundBuffer();
    }
    EmuDirectSoundBuffer*   emuDSBuffer;

    virtual ~SharedDSBuffer() {
        delete emuDSBuffer;
    }
};

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
#define DSE_FLAG_IS_ACTIVATED           (1 << 21) // Only used for DirectSoundStream class, to acknowledge pause's no activate flag.
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

            xbox::void_t (WINAPI *Dummy_0x10)(xbox::dword_t dwDummy1, xbox::dword_t dwDummy2);   // 0x10
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
    xbox::XMEDIAPACKET xmp_data;
    PVOID   pBuffer_data;
    DWORD   nextWriteOffset;
    DWORD   lastWritePos;
    DWORD   bufPlayed;
    DWORD   bufWrittenBytes;
    bool    isPlayed;
    bool    isStreamEnd;
};

// ******************************************************************
// * X_CDirectSoundStream
// ******************************************************************
class X_CDirectSoundStream
{
    public:
        // construct vtable (or grab ptr to existing)
        X_CDirectSoundStream(bool is3D) : pVtbl(&vtbl), Xb_Voice(is3D) { pMcpxStream = new X_CMcpxStream(this); };

    private:
        // vtable (cached by each instance, via constructor)
        struct _vtbl
        {
            ULONG (WINAPI *AddRef)(X_CDirectSoundStream *pThis);            // 0x00
            ULONG (WINAPI *Release)(X_CDirectSoundStream *pThis);           // 0x04
            
            hresult_t (WINAPI *GetInfo)                                       // 0x08
            (
                X_CDirectSoundStream*   pThis,
                XMEDIAINFO*             pInfo
            );

            hresult_t (WINAPI *GetStatus)                                     // 0x0C
            (
                X_CDirectSoundStream*   pThis,
                LPDWORD                 pdwStatus
            );

            hresult_t (WINAPI *Process)                                       // 0x10
            (
                X_CDirectSoundStream*   pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );

            hresult_t (WINAPI *Discontinuity)(X_CDirectSoundStream *pThis);   // 0x14

            hresult_t (WINAPI *Flush)(X_CDirectSoundStream *pThis);           // 0x18

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
        // Placeholder until have positive offset
        CDirectSoundVoice                       Xb_Voice;
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
        bool                                    Host_isProcessing;
        LPFNXMOCALLBACK                         Xb_lpfnCallback;
        LPVOID                                  Xb_lpvContext;
        REFERENCE_TIME                          Xb_rtFlushEx;
        REFERENCE_TIME                          Xb_rtPauseEx;
        LONG                                    Xb_VolumeMixbin;
        X_DSENVOLOPEDESC                        Xb_EnvolopeDesc;
        X_DSVOICEPROPS                          Xb_VoiceProperties;
        DWORD                                   Host_dwLastWritePos;
        DWORD                                   Xb_Flags;
        DWORD                                   Xb_Status;
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

            hresult_t (WINAPI *GetInfo)                                        // 0x08
            (
                X_XFileMediaObject*     pThis,
                XMEDIAINFO*             pInfo
            );
                                                                                                              
            hresult_t (WINAPI *GetStatus)                                     // 0x0C
            (
                X_XFileMediaObject*     pThis,
                LPDWORD                 pdwStatus
            );
 
            hresult_t (WINAPI *Process)                                       // 0x10
            (
                X_XFileMediaObject*     pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );
           
            hresult_t (WINAPI *Discontinuity)(X_XFileMediaObject *pThis);        // 0x14
                                                                           
            DWORD Unknown7;
/*
            HRESULT (WINAPI *Flush)(X_XFileMediaObject *pThis);                // 0x18
*/
            hresult_t (WINAPI *Seek)                                            // 0x1C
            (
                X_XFileMediaObject*     pThis,
                long_t                    lOffset, 
                dword_t                   dwOrigin, 
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
xbox::hresult_t WINAPI EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * patch: DirectSoundDoWork
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(DirectSoundDoWork)();

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
xbox::hresult_t WINAPI EMUPATCH(CDirectSound_GetSpeakerConfig)
(
    X_CDirectSound*         pThis,
    PDWORD                  pdwSpeakerConfig
);

// ******************************************************************
// * patch: IDirectSound_SynchPlayback
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: IDirectSound_DownloadEffectsImage
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_DownloadEffectsImage)
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    dword_t                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
);

// ******************************************************************
// * patch: IDirectSound_SetOrientation
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetOrientation)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           xFront,
    FLOAT           yFront,
    FLOAT           zFront,
    FLOAT           xTop,
    FLOAT           yTop,
    FLOAT           zTop,
    dword_t           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetDistanceFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetDistanceFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    dword_t           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetRolloffFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetRolloffFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    dword_t           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetDopplerFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetDopplerFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    dword_t           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetI3DL2Listener
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    X_DSI3DL2LISTENER      *pds3dl,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetMixBinHeadroom
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetMixBinHeadroom)
(
    LPDIRECTSOUND8          pThis,
    dword_t                   dwMixBinMask,
    dword_t                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBins
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBins)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwMixBinMask
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwMixBinMask,
    const long_t*             alVolumes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
(
    XbHybridDSBuffer*       pHybridThis,
    X_LPDSMIXBINS           pMixBins
);

// ******************************************************************
// * patch: IDirectSound_SetPosition
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetPosition)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetVelocity
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetVelocity)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetAllParameters
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPCDS3DLISTENER         pDS3DListenerParameters,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSound_CommitDeferredSettings
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSound_CommitDeferredSettings)
(
    X_CDirectSound*         pThis
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8              pThis,
    X_DSBUFFERDESC*             pdsbd,
    XbHybridDSBuffer**          ppBuffer,
    LPUNKNOWN                   pUnkOuter
);

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*             pdsbd,
    XbHybridDSBuffer**          ppBuffer
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetBufferData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetBufferData)
(
    XbHybridDSBuffer*           pHybridThis,
    LPVOID                      pvBufferData,
    dword_t                       dwBufferBytes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetPlayRegion)
(
    XbHybridDSBuffer*           pHybridThis,
    dword_t                       dwPlayStart,
    dword_t                       dwPlayLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Lock
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Lock)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwOffset,
    dword_t                   dwBytes,
    LPVOID*                 ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID*                 ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    dword_t                   dwFlags
);
// ******************************************************************
// * patch: IDirectSoundBuffer_Unlock
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Unlock)
(
    XbHybridDSBuffer*       pHybridThis,
    LPVOID                  ppvAudioPtr1,
    dword_t                   pdwAudioBytes1,
    LPVOID                  ppvAudioPtr2,
    dword_t                   pdwAudioBytes2
    );

// ******************************************************************
// * patch: IDirectSoundBuffer_SetHeadroom
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetHeadroom)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetLoopRegion)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwLoopStart,
    dword_t                   dwLoopLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Release
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSoundBuffer_Release)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPitch
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetPitch)
(
    XbHybridDSBuffer*       pHybridThis,
    long_t                    lPitch
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    XbHybridDSBuffer*       pHybridThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_t                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetCurrentPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwNewPosition
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Stop
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Stop)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_StopEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_t                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Play
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Play)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwReserved1,
    dword_t                   dwReserved2,
    dword_t                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_t                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_t                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFrequency
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetFrequency)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwFrequency
);

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundStream
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_CreateSoundStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown
);

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(CMcpxStream_Dummy_0x10)(dword_t dwDummy1, dword_t dwDummy2);

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    dword_t                 dwApply
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
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_GetInfo)
(
    X_CDirectSoundStream*    pThis, 
    LPXMEDIAINFO            pInfo
);

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_GetStatus)
(
    X_CDirectSoundStream*   pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream*   pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_Discontinuity)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_Flush)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSound_SynchPlayback
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_Pause)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwPause
);

// ******************************************************************
// * patch: CDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwHeadroom
);

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    X_DS3DBUFFER*           pc3DBufferParameters,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwInsideConeAngle,
    dword_t                   dwOutsideConeAngle,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    X_CDirectSoundStream*   pThis,
    long_t                    lConeOutsideVolume,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMaxDistance,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMinDistance,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetVelocity)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetPosition)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwFrequency
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwMixBinMask
);

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMaxDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flMaxDistance,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMinDistance
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMinDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flMaxDistance,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flRolloffFactor,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetDistanceFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flDistanceFactor,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeAngles
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetConeAngles)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwInsideConeAngle,
    dword_t                   dwOutsideConeAngle,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOrientation)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_t                    lConeOutsideVolume,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPosition
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVelocity
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetVelocity)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetDopplerFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flDopplerFactor,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_t                   dwApply
);
// +s

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMode
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetMode)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwMode,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetFormat)
(
    XbHybridDSBuffer*       pHybridThis,
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
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetLFO)
(
    XbHybridDSBuffer*       pHybridThis,
    LPCDSLFODESC            pLFODesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream *pThis,
    LPCDSLFODESC         pLFODesc
);

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat // NOTE: Not require to patch
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(XAudioCreateAdpcmFormat)
(
    word_t                        nChannels,
    dword_t                       nSamplesPerSec,
    OUT LPXBOXADPCMWAVEFORMAT   pwfx
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffCurve)
(
    XbHybridDSBuffer*       pHybridThis,
    const FLOAT*            pflPoints,
    dword_t                   dwPointCount,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pStream,
    long_t                    lVolume
);

// ******************************************************************
// * patch: IDirectSound_EnableHeadphones
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_EnableHeadphones)
(
    LPDIRECTSOUND8      pThis,
    BOOL                fEnabled
);

// ******************************************************************
// * patch: IDirectSoundBuffer_AddRef
// ******************************************************************
ULONG WINAPI EMUPATCH(IDirectSoundBuffer_AddRef)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Pause
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Pause)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwPause
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_PauseEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimestamp,
    dword_t                   dwPause
);

// ******************************************************************
// * patch: IDirectSound_GetOutputLevels
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_GetOutputLevels)
(
    LPDIRECTSOUND8*         pThis,
    X_DSOUTPUTLEVELS*       pOutputLevels,
    BOOL                    bResetPeakValues
);

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_FlushEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_t                   dwFlags
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream*   pStream,
    dword_t                   dwMode,
    dword_t                   dwApply
);
    
// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR          pszImageName,
    LPVOID          pImageLoc,
    dword_t           dwFlags,
    LPVOID*         ppImageDesc
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFilter
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetFilter)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSFILTERDESC*         pFilterDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc
);

// ******************************************************************
// * patch: IDirectSound_GetCaps
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_GetCaps)
(
    X_CDirectSound*    pThis,
    X_DSCAPS*        pDSCaps
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetPitch)
(    
    X_CDirectSoundStream*   pThis,
    long_t                    lPitch
);

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
xbox::dword_t WINAPI EMUPATCH(DirectSoundGetSampleTime)();

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12)
(
    X_CDirectSoundStream*    pThis,
    dword_t                    dwMixBinMask,
    const long_t*              alVolumes
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*    pThis,
    X_LPDSMIXBINS            pMixBins
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DS3DBUFFER*            pc3DBufferParameters,
    dword_t                    dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*   pThis,
    LPCWAVEFORMATEX         pwfxFormat
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetOutputBuffer)
(
    XbHybridDSBuffer*       pHybridThis,
    XbHybridDSBuffer*       pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
    X_CDirectSoundStream*   pThis,
    XbHybridDSBuffer*       pOutputBuffer
);

// ******************************************************************
// * patch: XFileCreaeMediaObjectEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileCreateMediaObjectEx)
(
    HANDLE    hFile,
    void**    ppMediaObject
);

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR              pszFileName,
    LPCWAVEFORMATEX    *ppwfxFormat,
    void              **ppMediaObject // XFileMediaObject
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetEG)
(
    XbHybridDSBuffer*   pHybridThis,
    X_DSENVOLOPEDESC*   pEnvelopeDesc
);

// ******************************************************************
// * patch: IDirectSound_GetEffectData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_GetEffectData)
(
    X_CDirectSound*     pThis,
    dword_t               dwEffectIndex,
    dword_t               dwOffset,
    LPVOID              pvData,
    dword_t               dwDataSize
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_SetNotificationPositions)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies
);

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
    X_CDirectSoundStream*   pThis,
    const FLOAT*            pflPoints,
    dword_t                   dwPointCount,
    dword_t                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetEffectData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_SetEffectData)
(
    LPVOID pThis,
    dword_t dwEffectIndex,
    dword_t dwOffset,
    LPCVOID pvData,
    dword_t dwDataSize,
    dword_t dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Use3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    LPUNKNOWN               pUnknown
);

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE    hFile,
    dword_t    dwMaxPackets,
    void    **ppMediaObject
);

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    long_t                lOffset,
    dword_t               dwOrigin,
    LPDWORD             pdwAbsolute
);

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis);

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject*     pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileMediaObject_GetInfo)
(
    X_XFileMediaObject       *pThis,
    XMEDIAINFO               *pInfo
);

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileMediaObject_Process)
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
xbox::hresult_t WINAPI EMUPATCH(XFileMediaObject_Discontinuity)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: IDirectSound_GetSpeakerConfig
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_GetSpeakerConfig)
    (X_CDirectSound* pThis, OUT LPDWORD* pdwSpeakerConfig);

// ******************************************************************
// * patch: IDirectSound_CommitDeferredSettings
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_CommitDeferredSettings)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: IDirectSound_CommitEffectData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSound_CommitEffectData)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: CDirectSoundStream_PauseEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    dword_t                   dwPause);

// ******************************************************************
// * patch: XFileCreaeMediaObject
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XFileCreateMediaObject)
(
    LPCSTR              pstrFileName,
    dword_t               dwDesiredAccess,
    dword_t               dwShareMode,
    dword_t               dwCreationDisposition,
    dword_t               dwFlagsAndAttributes,
    OUT void**          ppMediaObject);

// ******************************************************************
// * patch: XWaveFileCreateMediaObjectEx
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XWaveFileCreateMediaObjectEx)
(
    LPCSTR              pszFileName,
    HANDLE              hFile,
    OUT void**          ppMediaObject); //XWaveFileMediaObject

// ******************************************************************
// * patch: XAudioSetEffectData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(XAudioSetEffectData)
(
    dword_t               dwEffectIndex,
    void*               pDesc,
    void*               pRawDesc);

// ******************************************************************
// * patch: CDirectSoundStream_SetDistanceFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetDistanceFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDistanceFactor,
    dword_t                   dwApply);

// ******************************************************************
// * patch: CDirectSoundStream_SetDopplerFactor
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_SetDopplerFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDopplerFactor,
    dword_t                   dwApply);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_GetVoiceProperties)
(
    XbHybridDSBuffer*       pHybridThis,
    OUT X_DSVOICEPROPS*     pVoiceProps);

// ******************************************************************
// * patch: CDirectSoundStream_GetVoiceProperties
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(CDirectSoundStream_GetVoiceProperties)
(
    X_CDirectSoundStream*   pThis,
    OUT X_DSVOICEPROPS*     pVoiceProps);

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    long_t                    lVolume);

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    long_t                    lPitch);

// ******************************************************************
// * patch: IDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwHeadroom);

// ******************************************************************
// * patch: IDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwFrequency);

// ******************************************************************
// * patch: IDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    dword_t                   dwMixBinMask);

// ******************************************************************
// * patch:  CDirectSound3DCalculator_Calculate3D
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(CDirectSound3DCalculator_Calculate3D)
(
    dword_t a1,
    dword_t a2);

// ******************************************************************
// * patch:  CDirectSound3DCalculator_GetVoiceData
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(CDirectSound3DCalculator_GetVoiceData)
(
    dword_t a1,
    dword_t a2,
    dword_t a3,
    dword_t a4,
    dword_t a5);

// ******************************************************************
// * patch:  IDirectSoundBuffer_Set3DVoiceData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundBuffer_Set3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_t a2);

// ******************************************************************
// * patch:  IDirectSoundStream_Set3DVoiceData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_Set3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_t a2
);

// ******************************************************************
// * patch:  IDirectSoundStrea,_Use3DVoiceData
// ******************************************************************
xbox::hresult_t WINAPI EMUPATCH(IDirectSoundStream_Use3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_t a2
);

} // end of extern "C"

} // end of namespace xbox

#endif
