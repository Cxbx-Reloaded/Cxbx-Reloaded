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

typedef struct _DSEFFECTMAP {
	LPVOID lpvCodeSegment;
	DWORD dwCodeSize;
	LPVOID lpvStateSegment;
	DWORD dwStateSize;
	LPVOID lpvYMemorySegment;
	DWORD dwYMemorySize;
	LPVOID lpvScratchSegment;
	DWORD dwScratchSize;
} DSEFFECTMAP, *LPDSEFFECTMAP;
/*Members
lpvCodeSegment
Starting address of the DSP code segment for this effect.
dwCodeSize
Value that contains the code segment size, in DWORDs.
lpvStateSegment
Starting address of the effect state segment.
dwStateSize
Value that contains the size of the effect state segment, in DWORDs.
lpvYMemorySegment
Starting address of the DSP Y-memory segment.
dwYMemorySize
Value that contains the Y-memory segment size, in DWORDs.
lpvScratchSegment
Starting address of the scratch memory segment.
dwScratchSize
Value that contains the size of the scratch segment, in DWORDs.
*/

typedef struct _DSEFFECTIMAGEDESC {
	DWORD dwEffectCount;
	DWORD dwTotalScratchSize;
	DSEFFECTMAP aEffectMaps[1];
} DSEFFECTIMAGEDESC, *LPDSEFFECTIMAGEDESC;

/*Members
dwEffectCount 
Value that contains the number of effects in the image. 
dwTotalScratchSize 
Value that contains the total amount of space required by effects that use scratch space for delay lines. 
aEffectMaps 
Variable-length array that contains the effect descriptions. 
*/

//Custom flags (4 bytes support up to 31 shifts,starting from 0)
#define DSE_FLAG_PCM                    (1 << 0)
#define DSE_FLAG_XADPCM                 (1 << 1)
#define DSE_FLAG_PCM_UNKNOWN            (1 << 2)
#define DSE_FLAG_SYNCHPLAYBACK_CONTROL  (1 << 10)
#define DSE_FLAG_PAUSE                  (1 << 11)
#define DSE_FLAG_PAUSENOACTIVATE        (1 << 12)
#define DSE_FLAG_FLUSH_ASYNC            (1 << 13)
#define DSE_FLAG_ENVELOPE               (1 << 14)
#define DSE_FLAG_ENVELOPE2              (1 << 15) // NOTE: This flag is a requirement for GetStatus to return X_DSSSTATUS_ENVELOPECOMPLETE value.
#define DSE_FLAG_RECIEVEDATA            (1 << 20)
#define DSE_FLAG_IS_ACTIVATED           (1 << 21) // Only used for DirectSoundStream class, to acknowledge pause's no activate flag.
#define DSE_FLAG_IS_FLUSHING            (1 << 22) // Only used for DirectSoundStream class, to acknowledge pause's no activate flag.
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

            xbox::void_xt (WINAPI *Dummy_0x10)(xbox::dword_xt dwDummy1, xbox::dword_xt dwDummy2);   // 0x10
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
        X_CDirectSoundStream(bool is3D);

    private:
        // vtable (cached by each instance, via constructor)
        struct _vtbl
        {
            ulong_xt (WINAPI *AddRef)(X_CDirectSoundStream *pThis);            // 0x00
            ulong_xt (WINAPI *Release)(X_CDirectSoundStream *pThis);           // 0x04
            
            hresult_xt (WINAPI *GetInfo)                                       // 0x08
            (
                X_CDirectSoundStream*   pThis,
                XMEDIAINFO*             pInfo
            );

            hresult_xt (WINAPI *GetStatus)                                     // 0x0C
            (
                X_CDirectSoundStream*   pThis,
                LPDWORD                 pdwStatus
            );

            hresult_xt (WINAPI *Process)                                       // 0x10
            (
                X_CDirectSoundStream*   pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );

            hresult_xt (WINAPI *Discontinuity)(X_CDirectSoundStream *pThis);   // 0x14

            hresult_xt (WINAPI *Flush)(X_CDirectSoundStream *pThis);           // 0x18

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
        static _vtbl vtbl_r1;
        static _vtbl vtbl_r2;

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
            ulong_xt (WINAPI *AddRef)(X_XFileMediaObject *pThis);            // 0x00
            ulong_xt (WINAPI *Release)(X_XFileMediaObject *pThis);           // 0x04

            hresult_xt (WINAPI *GetInfo)                                        // 0x08
            (
                X_XFileMediaObject*     pThis,
                XMEDIAINFO*             pInfo
            );
                                                                                                              
            hresult_xt (WINAPI *GetStatus)                                     // 0x0C
            (
                X_XFileMediaObject*     pThis,
                LPDWORD                 pdwStatus
            );
 
            hresult_xt (WINAPI *Process)                                       // 0x10
            (
                X_XFileMediaObject*     pThis,
                PXMEDIAPACKET           pInputBuffer,
                PXMEDIAPACKET           pOutputBuffer
            );
           
            hresult_xt (WINAPI *Discontinuity)(X_XFileMediaObject *pThis);        // 0x14
                                                                           
            DWORD Unknown7;
/*
            HRESULT (WINAPI *Flush)(X_XFileMediaObject *pThis);                // 0x18
*/
            hresult_xt (WINAPI *Seek)                                            // 0x1C
            (
                X_XFileMediaObject*     pThis,
                long_xt                    lOffset, 
                dword_xt                   dwOrigin, 
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
xbox::hresult_xt WINAPI EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * patch: DirectSoundDoWork
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(DirectSoundDoWork)();

// ******************************************************************
// * patch: IDirectSound_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(IDirectSound_AddRef)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: IDirectSound_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(IDirectSound_Release)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSound_GetSpeakerConfig
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSound_GetSpeakerConfig)
(
    X_CDirectSound*         pThis,
    PDWORD                  pdwSpeakerConfig
);

// ******************************************************************
// * patch: IDirectSound_SynchPlayback
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSound_DownloadEffectsImage
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSound_DownloadEffectsImage)
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    dword_xt                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
);

// ******************************************************************
// * patch: IDirectSound_SetOrientation
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetOrientation)
(
    LPDIRECTSOUND8  pThis,
    float_xt           xFront,
    float_xt           yFront,
    float_xt           zFront,
    float_xt           xTop,
    float_xt           yTop,
    float_xt           zTop,
    dword_xt           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetDistanceFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetDistanceFactor)
(
    LPDIRECTSOUND8  pThis,
    float_xt           fDistanceFactor,
    dword_xt           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetRolloffFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetRolloffFactor)
(
    LPDIRECTSOUND8  pThis,
    float_xt           fRolloffFactor,
    dword_xt           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetDopplerFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetDopplerFactor)
(
    LPDIRECTSOUND8  pThis,
    float_xt           fDopplerFactor,
    dword_xt           dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetI3DL2Listener
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    X_DSI3DL2LISTENER      *pds3dl,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetMixBinHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetMixBinHeadroom)
(
    LPDIRECTSOUND8          pThis,
    dword_xt                   dwMixBinMask,
    dword_xt                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBins
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBins)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSMIXBINBUNION  mixBins
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwMixBinMask,
    const long_xt*             alVolumes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
(
    XbHybridDSBuffer*       pHybridThis,
    X_LPDSMIXBINS           pMixBins
);

// ******************************************************************
// * patch: IDirectSound_SetPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetPosition)
(
    LPDIRECTSOUND8          pThis,
    float_xt                   x,
    float_xt                   y,
    float_xt                   z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetVelocity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetVelocity)
(
    LPDIRECTSOUND8          pThis,
    float_xt                   x,
    float_xt                   y,
    float_xt                   z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetAllParameters
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPCDS3DLISTENER         pDS3DListenerParameters,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSound_CommitDeferredSettings
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSound_CommitDeferredSettings)
(
    X_CDirectSound*         pThis
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8              pThis,
    X_DSBUFFERDESC*             pdsbd,
    XbHybridDSBuffer**          ppBuffer,
    LPUNKNOWN                   pUnkOuter
);

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*             pdsbd,
    XbHybridDSBuffer**          ppBuffer
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetBufferData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetBufferData)
(
    XbHybridDSBuffer*           pHybridThis,
    LPVOID                      pvBufferData,
    dword_xt                       dwBufferBytes
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetPlayRegion)
(
    XbHybridDSBuffer*           pHybridThis,
    dword_xt                       dwPlayStart,
    dword_xt                       dwPlayLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Lock
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Lock)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwOffset,
    dword_xt                   dwBytes,
    LPVOID*                 ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID*                 ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    dword_xt                   dwFlags
);
// ******************************************************************
// * patch: IDirectSoundBuffer_Unlock
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Unlock)
(
    XbHybridDSBuffer*       pHybridThis,
    LPVOID                  ppvAudioPtr1,
    dword_xt                   pdwAudioBytes1,
    LPVOID                  ppvAudioPtr2,
    dword_xt                   pdwAudioBytes2
    );

// ******************************************************************
// * patch: IDirectSoundBuffer_SetHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetHeadroom)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwHeadroom
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetLoopRegion)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwLoopStart,
    dword_xt                   dwLoopLength
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(IDirectSoundBuffer_Release)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPitch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetPitch)
(
    XbHybridDSBuffer*       pHybridThis,
    long_xt                    lPitch
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    XbHybridDSBuffer*       pHybridThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_xt                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetCurrentPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwNewPosition
);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Stop
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Stop)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_StopEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_xt                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Play
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Play)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwReserved1,
    dword_xt                   dwReserved2,
    dword_xt                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_xt                   dwFlags
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_xt                    lVolume
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFrequency
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetFrequency)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwFrequency
);

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream
);

// ******************************************************************
// * patch: IDirectSound_CreateSoundStream
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_CreateSoundStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown
);

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CMcpxStream_Dummy_0x10)(dword_xt dwDummy1, dword_xt dwDummy2);

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream *pThis,
    float_xt                 fRolloffFactor,
    dword_xt                 dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(CDirectSoundStream_AddRef)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSoundStream_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(CDirectSoundStream_Release)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * CDirectSoundStream_GetInfo
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_GetInfo)
(
    X_CDirectSoundStream*    pThis, 
    LPXMEDIAINFO            pInfo
);

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus (3911+)
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_GetStatus__r1)
(
    X_CDirectSoundStream*   pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus (4134+)
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_GetStatus__r2)
(
    X_CDirectSoundStream*   pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream*   pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_Discontinuity)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_Flush)(X_CDirectSoundStream *pThis);

// ******************************************************************
// * patch: CDirectSound_SynchPlayback
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_Pause)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwPause
);

// ******************************************************************
// * patch: CDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwHeadroom
);

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    X_DS3DBUFFER*           pc3DBufferParameters,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwInsideConeAngle,
    dword_xt                   dwOutsideConeAngle,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lConeOutsideVolume,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMaxDistance,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMinDistance,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetVelocity)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetPosition)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwFrequency
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    X_DSMIXBINBUNION  mixBins
);

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMaxDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   flMaxDistance,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMinDistance
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMinDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   flMinDistance,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   flRolloffFactor,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetDistanceFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   flDistanceFactor,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeAngles
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetConeAngles)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwInsideConeAngle,
    dword_xt                   dwOutsideConeAngle,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOrientation)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   x,
    float_xt                   y,
    float_xt                   z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    long_xt                    lConeOutsideVolume,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPosition
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   x,
    float_xt                   y,
    float_xt                   z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVelocity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetVelocity)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   x,
    float_xt                   y,
    float_xt                   z,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetDopplerFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    float_xt                   flDopplerFactor,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_xt                   dwApply
);
// +s

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMode
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetMode)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwMode,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetFormat)
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
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetLFO)
(
    XbHybridDSBuffer*       pHybridThis,
    LPCDSLFODESC            pLFODesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream *pThis,
    LPCDSLFODESC         pLFODesc
);

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat // NOTE: Not require to patch
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XAudioCreateAdpcmFormat)
(
    word_xt                        nChannels,
    dword_xt                       nSamplesPerSec,
    OUT LPXBOXADPCMWAVEFORMAT   pwfx
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetRolloffCurve)
(
    XbHybridDSBuffer*       pHybridThis,
    const float_xt*            pflPoints,
    dword_xt                   dwPointCount,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pStream,
    long_xt                    lVolume
);

// ******************************************************************
// * patch: IDirectSound_EnableHeadphones
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_EnableHeadphones)
(
    LPDIRECTSOUND8      pThis,
    bool_xt                fEnabled
);

// ******************************************************************
// * patch: IDirectSoundBuffer_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(IDirectSoundBuffer_AddRef)
(
    XbHybridDSBuffer*       pHybridThis
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Pause
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Pause)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwPause
);

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_PauseEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimestamp,
    dword_xt                   dwPause
);

// ******************************************************************
// * patch: IDirectSound_GetOutputLevels
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_GetOutputLevels)
(
    LPDIRECTSOUND8*         pThis,
    X_DSOUTPUTLEVELS*       pOutputLevels,
    bool_xt                    bResetPeakValues
);

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_FlushEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_xt                   dwFlags
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream*   pStream,
    dword_xt                   dwMode,
    dword_xt                   dwApply
);
    
// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR          pszImageName,
    LPVOID          pImageLoc,
    dword_xt           dwFlags,
    LPVOID*         ppImageDesc
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFilter
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetFilter)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSFILTERDESC*         pFilterDesc
);

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc
);

// ******************************************************************
// * patch: IDirectSound_GetCaps
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_GetCaps)
(
    X_CDirectSound*    pThis,
    X_DSCAPS*        pDSCaps
);

// ******************************************************************
// * patch: CDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetPitch)
(    
    X_CDirectSoundStream*   pThis,
    long_xt                    lPitch
);

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(DirectSoundGetSampleTime)();

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12)
(
    X_CDirectSoundStream*    pThis,
    dword_xt                    dwMixBinMask,
    const long_xt*              alVolumes
);

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*    pThis,
    X_LPDSMIXBINS            pMixBins
);

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DS3DBUFFER*            pc3DBufferParameters,
    dword_xt                    dwApply
);

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*   pThis,
    LPCWAVEFORMATEX         pwfxFormat
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetOutputBuffer)
(
    XbHybridDSBuffer*       pHybridThis,
    XbHybridDSBuffer*       pOutputBuffer
);

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
    X_CDirectSoundStream*   pThis,
    XbHybridDSBuffer*       pOutputBuffer
);

// ******************************************************************
// * patch: XFileCreaeMediaObjectEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileCreateMediaObjectEx)
(
    HANDLE    hFile,
    void**    ppMediaObject
);

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR              pszFileName,
    LPCWAVEFORMATEX    *ppwfxFormat,
    void              **ppMediaObject // XFileMediaObject
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetEG)
(
    XbHybridDSBuffer*   pHybridThis,
    X_DSENVOLOPEDESC*   pEnvelopeDesc
);

// ******************************************************************
// * patch: IDirectSound_GetEffectData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_GetEffectData)
(
    X_CDirectSound*     pThis,
    dword_xt               dwEffectIndex,
    dword_xt               dwOffset,
    LPVOID              pvData,
    dword_xt               dwDataSize
);

// ******************************************************************
// * patch: IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_SetNotificationPositions)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies
);

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
    X_CDirectSoundStream*   pThis,
    const float_xt*            pflPoints,
    dword_xt                   dwPointCount,
    dword_xt                   dwApply
);

// ******************************************************************
// * patch: IDirectSound_SetEffectData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_SetEffectData)
(
    LPVOID pThis,
    dword_xt dwEffectIndex,
    dword_xt dwOffset,
    LPCVOID pvData,
    dword_xt dwDataSize,
    dword_xt dwApply
);

// ******************************************************************
// * patch: IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Use3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    LPUNKNOWN               pUnknown
);

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE    hFile,
    dword_xt    dwMaxPackets,
    void    **ppMediaObject
);

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    long_xt                lOffset,
    dword_xt               dwOrigin,
    LPDWORD             pdwAbsolute
);

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis);

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject*     pThis,
    LPDWORD                 pdwStatus
);

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileMediaObject_GetInfo)
(
    X_XFileMediaObject       *pThis,
    XMEDIAINFO               *pInfo
);

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileMediaObject_Process)
(
    X_XFileMediaObject       *pThis,
    LPXMEDIAPACKET            pInputBuffer, 
    LPXMEDIAPACKET            pOutputBuffer
);

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(XFileMediaObject_AddRef)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: XFileMediaObject_Release
// ******************************************************************
xbox::ulong_xt WINAPI EMUPATCH(XFileMediaObject_Release)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: XFileMediaObject_Discontinuity
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileMediaObject_Discontinuity)(X_XFileMediaObject *pThis);

// ******************************************************************
// * patch: IDirectSound_GetSpeakerConfig
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_GetSpeakerConfig)
    (X_CDirectSound* pThis, OUT LPDWORD* pdwSpeakerConfig);

// ******************************************************************
// * patch: IDirectSound_CommitDeferredSettings
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_CommitDeferredSettings)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: IDirectSound_CommitEffectData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSound_CommitEffectData)
    (X_CDirectSound* pThis);

// ******************************************************************
// * patch: CDirectSoundStream_PauseEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    dword_xt                   dwPause);

// ******************************************************************
// * patch: XFileCreaeMediaObject
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XFileCreateMediaObject)
(
    LPCSTR              pstrFileName,
    dword_xt               dwDesiredAccess,
    dword_xt               dwShareMode,
    dword_xt               dwCreationDisposition,
    dword_xt               dwFlagsAndAttributes,
    OUT void**          ppMediaObject);

// ******************************************************************
// * patch: XWaveFileCreateMediaObjectEx
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XWaveFileCreateMediaObjectEx)
(
    LPCSTR              pszFileName,
    HANDLE              hFile,
    OUT void**          ppMediaObject); //XWaveFileMediaObject

// ******************************************************************
// * patch: XAudioSetEffectData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XAudioSetEffectData)
(
    dword_xt               dwEffectIndex,
    void*               pDesc,
    void*               pRawDesc);

// ******************************************************************
// * patch: CDirectSoundStream_SetDistanceFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetDistanceFactor)
(
    X_CDirectSoundStream*   pThis,
    float_xt                   flDistanceFactor,
    dword_xt                   dwApply);

// ******************************************************************
// * patch: CDirectSoundStream_SetDopplerFactor
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_SetDopplerFactor)
(
    X_CDirectSoundStream*   pThis,
    float_xt                   flDopplerFactor,
    dword_xt                   dwApply);

// ******************************************************************
// * patch: IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_GetVoiceProperties)
(
    XbHybridDSBuffer*       pHybridThis,
    OUT X_DSVOICEPROPS*     pVoiceProps);

// ******************************************************************
// * patch: CDirectSoundStream_GetVoiceProperties
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(CDirectSoundStream_GetVoiceProperties)
(
    X_CDirectSoundStream*   pThis,
    OUT X_DSVOICEPROPS*     pVoiceProps);

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lVolume);

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lPitch);

// ******************************************************************
// * patch: IDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc);

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwHeadroom);

// ******************************************************************
// * patch: IDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwFrequency);

// ******************************************************************
// * patch: IDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    X_DSMIXBINBUNION  mixBins);

// ******************************************************************
// * patch:  CDirectSound3DCalculator_Calculate3D
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDirectSound3DCalculator_Calculate3D)
(
    dword_xt a1,
    dword_xt a2);

// ******************************************************************
// * patch:  CDirectSound3DCalculator_GetVoiceData
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(CDirectSound3DCalculator_GetVoiceData)
(
    dword_xt a1,
    dword_xt a2,
    dword_xt a3,
    dword_xt a4,
    dword_xt a5);

// ******************************************************************
// * patch:  IDirectSoundBuffer_Set3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundBuffer_Set3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    dword_xt a2);

// ******************************************************************
// * patch:  IDirectSoundStream_Set3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_Set3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_xt a2
);

// ******************************************************************
// * patch:  IDirectSoundStrea,_Use3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(IDirectSoundStream_Use3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_xt a2
);

} // end of extern "C"

} // end of namespace xbox

#endif
