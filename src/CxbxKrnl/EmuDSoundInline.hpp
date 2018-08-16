// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDSoundInline.hpp
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
// *  (c) 2017 blueshogun96
// *  (c) 2017-2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUDSOUNDINLINE_H
#define EMUDSOUNDINLINE_H

#include "XADPCM.h"

CRITICAL_SECTION                    g_DSoundCriticalSection;
#define enterCriticalSection        EnterCriticalSection(&g_DSoundCriticalSection)
#define leaveCriticalSection        LeaveCriticalSection(&g_DSoundCriticalSection)

#define DSoundBufferGetPCMBufferSize(EmuFlags, size) (EmuFlags & DSE_FLAG_XADPCM) > 0 ? DWORD((size / float(XBOX_ADPCM_SRCSIZE)) * XBOX_ADPCM_DSTSIZE) : size
#define DSoundBufferGetXboxBufferSize(EmuFlags, size) (EmuFlags & DSE_FLAG_XADPCM) > 0 ? DWORD((size / float(XBOX_ADPCM_DSTSIZE)) * XBOX_ADPCM_SRCSIZE) : size

// Memory managed xbox audio function handler
inline DWORD DSoundSGEFreeBuffer() {
    int count =  (X_DS_SGE_SIZE_MAX - g_dwXbMemAllocated);
    count =  count / X_DS_SGE_PAGE_MAX;
    // Check for negative value, we don't need warning as we are allowing overflow one time only.
    if (count < 0) {
        return 0;
    }
    return (DWORD)count;
}
inline void DSoundSGEMemAlloc(DWORD size) {
    g_dwXbMemAllocated += size;
}
inline void DSoundSGEMemDealloc(DWORD size) {
    g_dwXbMemAllocated -= size;
}
inline bool DSoundSGEMenAllocCheck(DWORD sizeRequest) {
    int leftOverSize = X_DS_SGE_SIZE_MAX - (g_dwXbMemAllocated + sizeRequest);
    // Don't let xbox title to alloc any more.
    if (leftOverSize < 0) {
        return false;
    }
    return true;
}

#if 0
// Debugging controlled audio section for audible only
inline void DSoundDebugMuteFlag(DWORD Xb_bufferBytes, DWORD &EmuFlags) {
    if (Xb_bufferBytes != 0xEA0) {
        EmuFlags |= DSE_FLAG_DEBUG_MUTE;
    } else {
        EmuFlags ^= DSE_FLAG_DEBUG_MUTE;
    }
}
#else
#define DSoundDebugMuteFlag
#endif

void DSoundBufferOutputXBtoHost(DWORD emuFlags, DSBUFFERDESC &DSBufferDesc, LPVOID pXBaudioPtr, DWORD dwXBAudioBytes, LPVOID pPCaudioPtr, DWORD dwPCMAudioBytes) {
    if ((emuFlags & DSE_FLAG_XADPCM) > 0) {

        TXboxAdpcmDecoder_Decode_Memory((uint8_t*)pXBaudioPtr, dwXBAudioBytes, (uint8_t*)pPCaudioPtr, DSBufferDesc.lpwfxFormat->nChannels);

    // PCM format, no changes requirement.
    } else {
        memcpy_s(pPCaudioPtr, dwPCMAudioBytes, pXBaudioPtr, dwPCMAudioBytes);
    }
}

// Convert XADPCM to PCM format helper function
inline void XADPCM2PCMFormat(LPWAVEFORMATEX lpwfxFormat)
{

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    DbgPrintf(LOG_PREFIX, "EmuDSound: XADPCM WAVEFORMATEX\n"
              "{\n"
              "   wFormatTag              : 0x%.04hX\n"
              "   nChannels               : 0x%.02hd\n"
              "   nSamplesPerSec          : 0x%.08X\n"
              "   nAvgBytesPerSec         : 0x%.08X\n"
              "   nBlockAlign             : 0x%.02hd\n"
              "   wBitsPerSample          : 0x%.04hX\n"
              "   cbSize                  : 0x%.04hX\n"
              "}\n",
              lpwfxFormat->wFormatTag,
              lpwfxFormat->nChannels,
              lpwfxFormat->nSamplesPerSec,
              lpwfxFormat->nAvgBytesPerSec,
              lpwfxFormat->nBlockAlign,
              lpwfxFormat->wBitsPerSample,
              lpwfxFormat->cbSize);
#endif

    lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;

    //lpwfxFormat.wFormatTag;         /* format type */
    //lpwfxFormat.nChannels;          /* number of channels (i.e. mono, stereo...) */ NO CHANGE
    //lpwfxFormat.nSamplesPerSec;     /* sample rate */ NO CHANGE
    //lpwfxFormat.nAvgBytesPerSec;    /* for buffer estimation */
    //lpwfxFormat.nBlockAlign;        /* block size of data */
    //lpwfxFormat.wBitsPerSample;     /* number of bits per sample of mono data */
    //lpwfxFormat.cbSize;             /* the count in bytes of the size of extra information (after cbSize) */

    lpwfxFormat->wBitsPerSample = 16;
    lpwfxFormat->nBlockAlign = 2 * lpwfxFormat->nChannels;
    lpwfxFormat->nAvgBytesPerSec = lpwfxFormat->nSamplesPerSec * lpwfxFormat->nBlockAlign;
    lpwfxFormat->cbSize = 0;
    //Enable this only if you have Xbox ADPCM Codec installed on your PC, or else it will fail every time.
    //This is just to verify format conversion is correct or not.
#if 0
    if (waveOutOpen(NULL, WAVE_MAPPER, lpwfxFormat, NULL, NULL, WAVE_FORMAT_QUERY) != MMSYSERR_NOERROR) {
        return DSERR_BADFORMAT;
    }
#endif

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    DbgPrintf(LOG_PREFIX, "EmuDSound: Converted to PCM WAVEFORMATEX\n"
              "{\n"
              "   wFormatTag              : 0x%.04hX\n"
              "   nChannels               : 0x%.02hd\n"
              "   nSamplesPerSec          : 0x%.08X\n"
              "   nAvgBytesPerSec         : 0x%.08X\n"
              "   nBlockAlign             : 0x%.02hd\n"
              "   wBitsPerSample          : 0x%.04hX\n"
              "   cbSize                  : 0x%.04hX\n"
              "}\n",
              lpwfxFormat->wFormatTag,
              lpwfxFormat->nChannels,
              lpwfxFormat->nSamplesPerSec,
              lpwfxFormat->nAvgBytesPerSec,
              lpwfxFormat->nBlockAlign,
              lpwfxFormat->wBitsPerSample,
              lpwfxFormat->cbSize);
#endif
}

inline void GenerateXboxBufferCache(
    DSBUFFERDESC   &DSBufferDesc,
    DWORD          &dwEmuFlags,
    DWORD           X_BufferSizeRequest,
    LPVOID*         X_BufferCache,
    DWORD          &X_BufferCacheSize) {

    // Generate xbox buffer cache size
    // If the size is the same, don't realloc
    if (X_BufferCacheSize != X_BufferSizeRequest) {
        // Check if buffer cache exist, then copy over old ones.
        if (*X_BufferCache != xbnullptr && (dwEmuFlags & DSE_FLAG_BUFFER_EXTERNAL) == 0) {
            LPVOID tempBuffer = *X_BufferCache;
            *X_BufferCache = malloc(X_BufferSizeRequest);

            // This will perform partial alloc/dealloc instead of call twice for alloc and dealloc functions.
            DSoundSGEMemAlloc(X_BufferSizeRequest - X_BufferCacheSize);

            // Don't copy over the limit.
            if (X_BufferCacheSize > X_BufferSizeRequest) {
                X_BufferCacheSize = X_BufferSizeRequest;
            }
            memcpy_s(*X_BufferCache, X_BufferSizeRequest, tempBuffer, X_BufferCacheSize);
            free(tempBuffer);
        } else {
            *X_BufferCache = calloc(1, X_BufferSizeRequest);
            DSoundSGEMemAlloc(X_BufferSizeRequest);
        }
        X_BufferCacheSize = X_BufferSizeRequest;
    }
}

inline void GeneratePCMFormat(
    DSBUFFERDESC   &DSBufferDesc,
    LPCWAVEFORMATEX lpwfxFormat,
    DWORD          &dwEmuFlags,
    DWORD           X_BufferSizeRequest,
    LPVOID*         X_BufferCache,
    DWORD          &X_BufferCacheSize)
{
    bool bIsSpecial = false;
    DWORD checkAvgBps;

    // convert from Xbox to PC DSound
    {
        DSBufferDesc.dwReserved = 0;

        if (lpwfxFormat != xbnullptr) {

            //TODO: RadWolfie - Need implement support for WAVEFORMATEXTENSIBLE as stated in CDirectSoundStream_SetFormat function note below
            // Do we need to convert it? Or just only do the WAVEFORMATEX only?

            // NOTE: pwfxFormat is not always a WAVEFORMATEX structure, it can
            // be WAVEFORMATEXTENSIBLE if that's what the programmer(s) wanted
            // in the first place, FYI.

            // Allocate only once, does not need to re-allocate.
            if (DSBufferDesc.lpwfxFormat == nullptr) {
                // Only allocate extra value for setting extra values later on. WAVEFORMATEXTENSIBLE is the highest size I had seen.
                DSBufferDesc.lpwfxFormat = (WAVEFORMATEX*)calloc(1, sizeof(WAVEFORMATEXTENSIBLE));
            }
            if (lpwfxFormat->wFormatTag == WAVE_FORMAT_PCM) {
                // Test case: Hulk crash due to cbSize is not a valid size.
                memcpy(DSBufferDesc.lpwfxFormat, lpwfxFormat, sizeof(WAVEFORMATEX));
                DSBufferDesc.lpwfxFormat->cbSize = 0; // Let's enforce this value to prevent any other exception later on.
            } else {
                memcpy(DSBufferDesc.lpwfxFormat, lpwfxFormat, sizeof(WAVEFORMATEX) + lpwfxFormat->cbSize);
            }

            dwEmuFlags = dwEmuFlags & ~DSE_FLAG_AUDIO_CODECS;

            switch (DSBufferDesc.lpwfxFormat->wFormatTag) {
                case WAVE_FORMAT_PCM:
                    dwEmuFlags |= DSE_FLAG_PCM;

                    //TODO: Phantasy Star Online Episode I & II made an attempt to use avg byte/second below sample/second requirement.
                    //In other word, this is a workaround to fix title mistake...
                    checkAvgBps = lpwfxFormat->nSamplesPerSec * lpwfxFormat->nBlockAlign;
                    if (lpwfxFormat->nAvgBytesPerSec < checkAvgBps) {
                        DSBufferDesc.lpwfxFormat->nAvgBytesPerSec = checkAvgBps;
                    }
                    break;
                case WAVE_FORMAT_XBOX_ADPCM:
                    dwEmuFlags |= DSE_FLAG_XADPCM;
                    XADPCM2PCMFormat(DSBufferDesc.lpwfxFormat);
                    break;
                default:
                    dwEmuFlags |= DSE_FLAG_PCM_UNKNOWN;
                    break;
            }
        } else {
            bIsSpecial = true;
            dwEmuFlags |= DSE_FLAG_RECIEVEDATA;

            EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Creating dummy WAVEFORMATEX (pdsbd->lpwfxFormat = xbnullptr)...");

            // HACK: This is a special sound buffer, create dummy WAVEFORMATEX data.
            // It's supposed to recieve data rather than generate it.  Buffers created
            // with flags DSBCAPS_MIXIN, DSBCAPS_FXIN, and DSBCAPS_FXIN2 will have no
            // WAVEFORMATEX structure by default.

            // TODO: A better response to this scenario if possible.

            DSBufferDesc.lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEXTENSIBLE));

            //memset(pDSBufferDescSpecial->lpwfxFormat, 0, sizeof(WAVEFORMATEX)); 
            //memset(pDSBufferDescSpecial, 0, sizeof(DSBUFFERDESC)); 

            DSBufferDesc.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
            DSBufferDesc.lpwfxFormat->nChannels = 2;
            DSBufferDesc.lpwfxFormat->nSamplesPerSec = 22050;
            DSBufferDesc.lpwfxFormat->nBlockAlign = 4;
            DSBufferDesc.lpwfxFormat->nAvgBytesPerSec = DSBufferDesc.lpwfxFormat->nSamplesPerSec * DSBufferDesc.lpwfxFormat->nBlockAlign;
            DSBufferDesc.lpwfxFormat->wBitsPerSample = 16;
            DSBufferDesc.lpwfxFormat->cbSize = 0;

            DSBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
            DSBufferDesc.dwBufferBytes = 3 * DSBufferDesc.lpwfxFormat->nAvgBytesPerSec;

            //        DSBufferDesc.lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX)/*+pdsbd->lpwfxFormat->cbSize*/);

            ////    DSBufferDesc.lpwfxFormat->cbSize = sizeof( WAVEFORMATEX );
            //        DSBufferDesc.lpwfxFormat->nChannels = 1;
            //        DSBufferDesc.lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
            //        DSBufferDesc.lpwfxFormat->nSamplesPerSec = 22050;
            //        DSBufferDesc.lpwfxFormat->nBlockAlign = 4;
            //        DSBufferDesc.lpwfxFormat->nAvgBytesPerSec = 4 * 22050;
            //        DSBufferDesc.lpwfxFormat->wBitsPerSample = 16;

            // Give this buffer 3 seconds of data if needed
            /*if(pdsbd->dwBufferBytes == 0) {
                DSBufferDesc.dwBufferBytes = 3 * DSBufferDesc.lpwfxFormat->nAvgBytesPerSec;
            }*/
        }

        DSBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    }

    // TODO: Still a requirement? Need to retest it again. Can't remember which title cause problem or had been resolved.
    // sanity check
    if (!bIsSpecial) {
        if (DSBufferDesc.lpwfxFormat->nBlockAlign != (DSBufferDesc.lpwfxFormat->nChannels*DSBufferDesc.lpwfxFormat->wBitsPerSample) / 8) {
            DSBufferDesc.lpwfxFormat->nBlockAlign = (2 * DSBufferDesc.lpwfxFormat->wBitsPerSample) / 8;
            DSBufferDesc.lpwfxFormat->nAvgBytesPerSec = DSBufferDesc.lpwfxFormat->nSamplesPerSec * DSBufferDesc.lpwfxFormat->nBlockAlign;
        }
    }

    if (DSBufferDesc.lpwfxFormat != nullptr) {

        // NOTE: This process check for 2+ channels whenever XADPCM or PCM format from xbox titles input in nChannels.
        // Since the host do not support 2+ channels on PCM only format. We must convert/update allocated wfxFormat to EXTENSIBLE
        // format which had been allocated enough to update.
        if (DSBufferDesc.lpwfxFormat->nChannels > 2 && DSBufferDesc.lpwfxFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
            PWAVEFORMATEXTENSIBLE lpwfxFormatExtensible = (PWAVEFORMATEXTENSIBLE)DSBufferDesc.lpwfxFormat;
            lpwfxFormatExtensible->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            lpwfxFormatExtensible->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
            lpwfxFormatExtensible->Samples.wValidBitsPerSample = lpwfxFormatExtensible->Format.wBitsPerSample;
            lpwfxFormatExtensible->Samples.wSamplesPerBlock = 0;
            lpwfxFormatExtensible->Samples.wReserved = 0;
            lpwfxFormatExtensible->dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;

            // Add rear speakers
            if (lpwfxFormat->nChannels >= 4) {
                lpwfxFormatExtensible->dwChannelMask |= SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
            }
            // Add center speaker
            if (lpwfxFormat->nChannels >= 5) {
                lpwfxFormatExtensible->dwChannelMask |= SPEAKER_FRONT_CENTER;
            }
            // Add subwoofer mask (pretty sure 3 channels is 2.1, not 3.0 for xbox purpose)
            if (lpwfxFormat->nChannels == 6 || lpwfxFormat->nChannels == 3) {
                lpwfxFormatExtensible->dwChannelMask |= SPEAKER_LOW_FREQUENCY;
            }
            lpwfxFormatExtensible->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
        }
    }

    if (X_BufferSizeRequest < DSBSIZE_MIN) {
        X_BufferSizeRequest = DSBSIZE_MIN;
    } else if (X_BufferSizeRequest > DSBSIZE_MAX) {
        X_BufferSizeRequest = DSBSIZE_MAX;
    }
    if (X_BufferCache != nullptr) {
        GenerateXboxBufferCache(DSBufferDesc, dwEmuFlags, X_BufferSizeRequest, X_BufferCache, X_BufferCacheSize);
    }

    // Handle DSound Buffer only
    if (X_BufferCacheSize > 0) {
        DSBufferDesc.dwBufferBytes = DSoundBufferGetPCMBufferSize(dwEmuFlags, X_BufferCacheSize);
    }
}

inline void DSoundGenericUnlock(
    DWORD                   dwEmuFlags,
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DSBUFFERDESC           &DSBufferDesc,
    XTL::DSoundBuffer_Lock &Host_lock,
    LPVOID                  X_BufferCache,
    DWORD                   X_Offset,
    DWORD                   X_dwLockBytes1,
    DWORD                   X_dwLockBytes2)
{
    // close any existing locks
    if (Host_lock.pLockPtr1 != nullptr) {


        if (X_BufferCache != xbnullptr) {
            DSoundBufferOutputXBtoHost(dwEmuFlags, DSBufferDesc, ((PBYTE)X_BufferCache + X_Offset), X_dwLockBytes1, Host_lock.pLockPtr1, Host_lock.dwLockBytes1);

            if (Host_lock.pLockPtr2 != nullptr) {

                DSoundBufferOutputXBtoHost(dwEmuFlags, DSBufferDesc, X_BufferCache, X_dwLockBytes2, Host_lock.pLockPtr2, Host_lock.dwLockBytes2);
            }
        }

        HRESULT hRet = pDSBuffer->Unlock(Host_lock.pLockPtr1, Host_lock.dwLockBytes1, Host_lock.pLockPtr2, Host_lock.dwLockBytes2);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "DirectSoundBuffer Unlock Failed!");
        }

        Host_lock.pLockPtr1 = nullptr;
        Host_lock.pLockPtr2 = nullptr;
    }

}

// Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
inline void DSoundBufferCreate(LPDSBUFFERDESC pDSBufferDesc, LPDIRECTSOUNDBUFFER8 &pDSBuffer)
{
    LPDIRECTSOUNDBUFFER pTempBuffer;
    HRESULT hRetDS = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, NULL);

    if (hRetDS != DS_OK) {
        CxbxKrnlCleanup(LOG_PREFIX, "CreateSoundBuffer error: 0x%08X", hRetDS);
    } else {
        hRetDS = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&(pDSBuffer));
        pTempBuffer->Release();
        if (hRetDS != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "Create IDirectSoundBuffer8 error: 0x%08X", hRetDS);
        }
    }
}
inline void DSound3DBufferCreate(LPDIRECTSOUNDBUFFER8 pDSBuffer, LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer) {
    HRESULT hRetDS3D = pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&(pDS3DBuffer));
    if (hRetDS3D != DS_OK) {
        EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "CreateSound3DBuffer Failed!");
        pDS3DBuffer = nullptr;
    }
}

#define DSoundBufferSetDefault(pThis, dwEmuPlayFlags) \
    pThis->EmuDirectSoundBuffer8 = nullptr; \
    pThis->EmuDirectSound3DBuffer8 = nullptr; \
    pThis->X_BufferCache = xbnullptr; \
    pThis->EmuFlags = 0; \
    pThis->EmuPlayFlags = dwEmuPlayFlags; \
    pThis->X_BufferCacheSize = 0; \
    pThis->Xb_rtPauseEx = 0LL; \
    pThis->Xb_Volume = 0L; \
    pThis->Xb_VolumeMixbin = 0L; \
    pThis->Xb_dwHeadroom = 600; // default for 2D voice
    //pThis->EmuBufferDesc = { 0 }; // Enable this when become necessary.
    /*
    pThis->EmuLockPtr1 = xbnullptr; \
    pThis->EmuLockBytes1 = 0; \
    pThis->EmuLockPtr2 = xbnullptr; \
    pThis->EmuLockBytes2 = 0; \ */

inline void DSoundBufferRegionSetDefault(XTL::X_CDirectSoundBuffer *pThis) {
    pThis->EmuBufferToggle = XTL::X_DSB_TOGGLE_DEFAULT;
    pThis->EmuRegionLoopStartOffset = 0;
    pThis->EmuRegionLoopLength = 0;
    pThis->EmuRegionPlayStartOffset = 0;
    pThis->EmuRegionPlayLength = 0;

}

inline void DSoundBufferTransferSettings(
    LPDIRECTSOUNDBUFFER8       &pDSBufferOld,
    LPDIRECTSOUNDBUFFER8       &pDSBufferNew,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferOld,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferNew)
{
    DWORD dwFrequency;
    LONG lVolume, lPan;
    DS3DBUFFER ds3dBuffer;

	if (pDSBufferOld == nullptr) {
		return;
	}

    pDSBufferOld->GetVolume(&lVolume);
    pDSBufferOld->GetFrequency(&dwFrequency);

    pDSBufferNew->SetFrequency(dwFrequency);
    pDSBufferNew->SetVolume(lVolume);

    if (pDS3DBufferOld != nullptr && pDS3DBufferNew != nullptr) {
        pDS3DBufferOld->GetAllParameters(&ds3dBuffer);

        pDS3DBufferNew->SetAllParameters(&ds3dBuffer, DS3D_IMMEDIATE);
    } else {
        pDSBufferOld->GetPan(&lPan);
        pDSBufferNew->SetPan(lPan);
    }
}

inline void DSoundBufferReCreate(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DSBUFFERDESC               &DSBufferDesc,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer,
    LPDIRECTSOUNDBUFFER8       &pDSBufferNew,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferNew) {


    DSoundBufferCreate(&DSBufferDesc, pDSBufferNew);

    if (pDS3DBuffer != nullptr) {
        DSound3DBufferCreate(pDSBufferNew, pDS3DBufferNew);
    }

    DSoundBufferTransferSettings(pDSBuffer, pDSBufferNew, pDS3DBuffer, pDS3DBufferNew);
}

inline void DSoundBufferRelease(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer,
    DWORD                      &refCount) {

    // Release 3D buffer first
    if (pDS3DBuffer != nullptr) {
        refCount = pDS3DBuffer->Release();
        if (refCount > 0) {
            CxbxKrnlCleanup(LOG_PREFIX, "Nope, wasn't fully cleaned up.");
        }
    }

    refCount = pDSBuffer->Release();
    if (refCount) {
        // NOTE: This is base on AddRef call, so this is a requirement.
        // The reason is to have ability "transfer" ref count is due to some titles doesn't care about ref count.
        // If AddRef was called, then it will "internally" increment the ref count, not the return value check.
        while (pDSBuffer->Release() > 0) {}
    }
}

inline void DSoundBufferResizeSetSize(
    XTL::X_CDirectSoundBuffer*  pThis,
    HRESULT                    &hRet,
    DWORD                       Xb_dwByteLength) {

    // General return OK, nothing needs to set as invalid for now.
    hRet = DS_OK;

    DWORD Host_dwByteLength = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, Xb_dwByteLength);

    // Don't re-create buffer if size is the same.
    if (pThis->EmuBufferDesc.dwBufferBytes == Host_dwByteLength) {
        return;
    }

    pThis->EmuBufferDesc.dwBufferBytes = Host_dwByteLength;

    // NOTE: Test case JSRF, if we allow to re-alloc without checking allocated buffer size.
    // Then it is somehow binded to IDirectSound_SetPosition control for any allocated audio afterward.

    DWORD refCount;
    LPDIRECTSOUNDBUFFER8       pDSBufferNew = nullptr;
    LPDIRECTSOUND3DBUFFER8       pDS3DBufferNew = nullptr;

    DSoundBufferReCreate(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuDirectSound3DBuffer8,
                         pDSBufferNew, pDS3DBufferNew);

    // release old buffer
    DSoundBufferRelease(pThis->EmuDirectSoundBuffer8, pThis->EmuDirectSound3DBuffer8, refCount);

    pThis->EmuDirectSoundBuffer8 = pDSBufferNew;
    pThis->EmuDirectSound3DBuffer8 = pDS3DBufferNew;

    if (refCount) {
        while (pThis->EmuDirectSoundBuffer8->AddRef() < refCount);
    }
}

inline void DSoundBufferResizeUpdate(
    XTL::X_CDirectSoundBuffer*  pThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet,
    DWORD                       Xb_dwStartOffset,
    DWORD                       Xb_dwByteLength) {

    DSoundBufferResizeSetSize(pThis, hRet, Xb_dwByteLength);

    hRet = pThis->EmuDirectSoundBuffer8->Lock(0, 0, &pThis->Host_lock.pLockPtr1, &pThis->Host_lock.dwLockBytes1,
                                              nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    if (hRet != DS_OK) {
        CxbxKrnlCleanup(LOG_PREFIX, "Unable to lock region buffer!");
    }
    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        Xb_dwStartOffset,
                        Xb_dwByteLength,
                        0);
}

inline void DSoundBufferRegionCurrentLocation(
    XTL::X_CDirectSoundBuffer*  pThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet,
    DWORD                      &Xb_dwStartOffset,
    DWORD                      &Xb_dwByteLength) {
    if ((dwPlayFlags & X_DSBPLAY_LOOPING) > 0) {
        Xb_dwStartOffset = pThis->EmuRegionPlayStartOffset + pThis->EmuRegionLoopStartOffset;

        // Must check for zero length, then apply true length.
        if (pThis->EmuRegionLoopLength == 0) {
            if (pThis->EmuRegionPlayLength != 0) {
                Xb_dwByteLength = pThis->EmuRegionPlayLength;
            } else {
                Xb_dwByteLength = pThis->X_BufferCacheSize - Xb_dwStartOffset;
            }
        } else {
            Xb_dwByteLength = pThis->EmuRegionLoopLength;
        }
    } else {
        Xb_dwStartOffset = pThis->EmuRegionPlayStartOffset;

        // Must check for zero length, then apply true length.
        if (pThis->EmuRegionPlayLength != 0) {
            Xb_dwByteLength = pThis->EmuRegionPlayLength;
        } else {
            Xb_dwByteLength = pThis->X_BufferCacheSize - Xb_dwStartOffset;
        }
    }
}

inline void DSoundBufferUpdate(
    XTL::X_CDirectSoundBuffer*  pThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet) {

    // Process Play/Loop Region buffer
    DWORD Xb_dwByteLength;
    DWORD Xb_dwStartOffset;

    DSoundBufferRegionCurrentLocation(pThis, dwPlayFlags, hRet, Xb_dwStartOffset, Xb_dwByteLength);

    DSoundBufferResizeUpdate(pThis, dwPlayFlags, hRet, Xb_dwStartOffset, Xb_dwByteLength);
}

inline void DSoundBufferReplace(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DSBUFFERDESC               &DSBufferDesc,
    DWORD                       PlayFlags,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer)
{
    DWORD refCount, dwPlayCursor, dwStatus;
    LPDIRECTSOUNDBUFFER8       pDSBufferNew = nullptr;
    LPDIRECTSOUND3DBUFFER8       pDS3DBufferNew = nullptr;

    DSoundBufferReCreate(pDSBuffer, DSBufferDesc, pDS3DBuffer,
                         pDSBufferNew, pDS3DBufferNew);

    HRESULT hRet = pDSBuffer->GetStatus(&dwStatus);

    if (hRet != DS_OK) {
        CxbxKrnlCleanup(LOG_PREFIX, "Unable to retrieve current status for replace DS buffer!");
    }

    pDSBuffer->Stop();

    hRet = pDSBuffer->GetCurrentPosition(&dwPlayCursor, nullptr);

    if (hRet != DS_OK) {
        CxbxKrnlCleanup(LOG_PREFIX, "Unable to retrieve current position for replace DS buffer!");
    }

    // TODO: Untested if transfer buffer to new audio buffer is necessary.
    PVOID pLockPtr1Old, pLockPtr1New;
    DWORD dwLockBytes1Old, dwLockBytes1New;
    pDSBuffer->Lock(0, 0, &pLockPtr1Old, &dwLockBytes1Old, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    pDSBufferNew->Lock(0, 0, &pLockPtr1New, &dwLockBytes1New, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    if (dwLockBytes1Old > dwLockBytes1New) {
        memcpy_s(pLockPtr1New, dwLockBytes1New, pLockPtr1Old, dwLockBytes1New);
    } else {
        memcpy_s(pLockPtr1New, dwLockBytes1Old, pLockPtr1Old, dwLockBytes1Old);
    }
    pDSBuffer->Unlock(pLockPtr1Old, dwLockBytes1Old, nullptr, 0);
    pDSBufferNew->Unlock(pLockPtr1New, dwLockBytes1New, nullptr, 0);

    pDSBufferNew->SetCurrentPosition(dwPlayCursor);

    if (dwStatus & DSBSTATUS_PLAYING) {
        pDSBufferNew->Play(0, 0, PlayFlags);
    }

    // release old buffer
    DSoundBufferRelease(pDSBuffer, pDS3DBuffer, refCount);

    pDSBuffer = pDSBufferNew;
    pDS3DBuffer = pDS3DBufferNew;

    if (refCount) {
        while (pDSBuffer->AddRef() < refCount);
    }
}

inline void DSoundStreamWriteToBuffer(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DWORD                       dwOffset,
    PVOID                       pBufferData,
    DWORD                       dwBufferSize) {

    LPVOID pAudioPtr, pAudioPtr2;
    DWORD dwAudioBytes, dwAudioBytes2;

    HRESULT hRet = pDSBuffer->Lock(dwOffset, dwBufferSize, &pAudioPtr, &dwAudioBytes,
                                   &pAudioPtr2, &dwAudioBytes2, 0);

    if (hRet == DS_OK) {

        if (pAudioPtr != nullptr) {
            memcpy_s(pAudioPtr, dwAudioBytes, pBufferData, dwAudioBytes);
            if (pAudioPtr2 != nullptr) {
                memcpy_s(pAudioPtr2, dwAudioBytes2, (PBYTE)pBufferData + dwAudioBytes, dwAudioBytes2);
            }
            pDSBuffer->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
        }
    }
}

#define vector_hvp_iterator std::vector<XTL::host_voice_packet>::iterator
inline void DSoundStreamClearPacket(
    vector_hvp_iterator        &buffer,
    DWORD                       status,
    XTL::LPFNXMOCALLBACK        Xb_lpfnCallback,
    LPVOID                      Xb_lpvContext,
    XTL::X_CDirectSoundStream*  pThis) {

    free(buffer->pBuffer_data);

    // Peform release only, don't trigger any events below.
    if (status == XMP_STATUS_RELEASE_CXBXR) {
        DSoundSGEMemDealloc(buffer->xmp_data.dwMaxSize);
        buffer = pThis->Host_BufferPacketArray.erase(buffer);
        return;
    }

    if (buffer->xmp_data.pdwStatus != xbnullptr) {
        (*buffer->xmp_data.pdwStatus) = status;
    }
    if (buffer->xmp_data.pdwCompletedSize != xbnullptr) {
        (*buffer->xmp_data.pdwCompletedSize) = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, buffer->xmp_data.dwMaxSize);
    }
    DSoundSGEMemDealloc(buffer->xmp_data.dwMaxSize);

    auto unionEventContext = buffer->xmp_data.hCompletionEvent;
    buffer = pThis->Host_BufferPacketArray.erase(buffer);
    // NOTE: Packet must be erase before call the callback function below. See test case below.
    // Test case: Pocketbike Racer
    //  * (cause audio skipping due to callback function called process then another called to process outside callback (x2)
    //    It only need to call process once.

    // If a callback is set, only do the callback instead of event handle.
    if (Xb_lpfnCallback != xbnullptr) {
        Xb_lpfnCallback(Xb_lpvContext, unionEventContext, status);
    } else if (unionEventContext != 0) {
        BOOL checkHandle = SetEvent(unionEventContext);
        if (checkHandle == 0) {
            DWORD error = GetLastError();
            EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Unable to set event on packet's hCompletionEvent. %8X | error = %8X", unionEventContext, error);
        }
    }
}

// Generic force remove synch playback control flag.
inline void DSoundBufferSynchPlaybackFlagRemove(
    DWORD                  &dwEmuFlags

    )
{
    if (g_iDSoundSynchPlaybackCounter == 0) {
        return;
    }

    if ((dwEmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {
        g_iDSoundSynchPlaybackCounter--;
        dwEmuFlags ^= DSE_FLAG_SYNCHPLAYBACK_CONTROL;
    }
}
inline HRESULT DSoundBufferSynchPlaybackFlagAdd(
    DWORD                  &dwEmuFlags

    )
{
    if (g_iDSoundSynchPlaybackCounter >= DSOUND_MAX_SYNCHPLAYBACK_AUDIO) {
        return DSERR_GENERIC;
    }

    if ((dwEmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) == 0) {
        g_iDSoundSynchPlaybackCounter++;
        dwEmuFlags |= DSE_FLAG_SYNCHPLAYBACK_CONTROL;
    }
    return DS_OK;
}

inline bool DSoundStreamProcess(XTL::X_CDirectSoundStream* pThis) {

    // If title want to pause, then don't process the packets.
    if ((pThis->EmuFlags & DSE_FLAG_PAUSE) > 0) {
        return 0;
    }

    // If media object is being used as playback synch, then don't process the packets.
    if ((pThis->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {
        return 0;
    }

    // Do not allow to process if there is no packets.
    if (pThis->Host_BufferPacketArray.size() == 0) {
        return 0;
    }

    DWORD dwAudioBytes;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwAudioBytes);
    if (hRet == DS_OK) {
        std::vector<XTL::host_voice_packet>::iterator buffer = pThis->Host_BufferPacketArray.begin();
        if (buffer->isWritten == false) {

        prepareNextBufferPacket:
            DSoundStreamWriteToBuffer(pThis->EmuDirectSoundBuffer8, buffer->rangeStart, buffer->pBuffer_data, buffer->xmp_data.dwMaxSize);

            // Debug area begin
            //printf("DEBUG: next packet process | pThis = %08X | rangeStart = %08d | bufferSize - %08d | dwBufferBytes = %08d | dwWriteOffsetNext = %08d\n", pThis, buffer->rangeStart, buffer->xmp_data.dwMaxSize, pThis->EmuBufferDesc->dwBufferBytes, pThis->Host_dwWriteOffsetNext);
            // Debug area end

            if (pThis->Host_isProcessing == false) {
                pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
                pThis->Host_isProcessing = true;
            }

            buffer->isWritten = true;

        } else {
            // NOTE: p1. Do not use play cursor, use write cursor to check ahead since by the time it gets there. The buffer is already played.
            // p2. Plus play cursor is not reliable to check, write cursor is reliable as it is update more often.
            // Test case proof: Gauntlet Dark Legacy give 256 bytes of data to a per packet during intro FMV.
            DWORD writePos = 0;
            hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(nullptr, &writePos);
            if (hRet == DS_OK) {

                int bufPlayed = writePos - buffer->rangeStart;

                // Correct it if buffer was playing and is at beginning.
                if (bufPlayed < 0 && (buffer->isPlayed || (writePos + pThis->EmuBufferDesc.dwBufferBytes - buffer->rangeStart) < buffer->xmp_data.dwMaxSize)) {
                    bufPlayed = pThis->EmuBufferDesc.dwBufferBytes - (bufPlayed * -1);
                }

                if (bufPlayed >= 0) {
                    if (buffer->isPlayed == false) {
                        buffer->isPlayed = true;
                    }
                    if (bufPlayed >= (int)buffer->xmp_data.dwMaxSize) {

                        DSoundStreamClearPacket(buffer, XMP_STATUS_SUCCESS, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);

                        if (pThis->Host_BufferPacketArray.size() == 0) {
                            goto endOfPacket;
                        }
                        if (buffer->isWritten == false) {
                            goto prepareNextBufferPacket;
                        }
                    }
                    if (buffer->xmp_data.pdwCompletedSize != xbnullptr) {
                        (*buffer->xmp_data.pdwCompletedSize) = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, bufPlayed);
                    }
                    if (pThis->Host_BufferPacketArray.size() > 1) {
                        if ((buffer + 1)->isWritten == false) {
                            buffer++;
                            goto prepareNextBufferPacket;
                        }
                    }
                }
            }
            // Out of packets, let's stop it.
            if (pThis->Host_BufferPacketArray.size() == 0) {

            endOfPacket:
                pThis->Host_isProcessing = false;
                pThis->EmuDirectSoundBuffer8->Stop();
                return 0;
            }
        }
    }
    return 1;
}

//TODO: RadWolfie - Need to implement DirectSoundBuffer create support. Or not able to do so due to all three classes function differently.
//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
/*inline HRESULT HybridDirectSoundBuffer_Create(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
};*/

inline ULONG HybridDirectSoundBuffer_AddRef(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{

    enterCriticalSection;

    ULONG uRet = pDSBuffer->AddRef();

    leaveCriticalSection;

    return uRet;
}

inline ULONG HybridDirectSoundBuffer_Release(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{

    enterCriticalSection;

    ULONG uRet = pDSBuffer->Release();

    leaveCriticalSection;

    return uRet;
}

/*
//IDirectSoundStream
inline HRESULT HybridDirectSoundBuffer_Discontinuity(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
inline HRESULT HybridDirectSoundBuffer_Flush(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
inline HRESULT HybridDirectSoundBuffer_FlushEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
inline HRESULT HybridDirectSoundBuffer_GetInfo(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}*/

// Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_GetCurrentPosition(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor,
    DWORD                   EmuFlags)
{

    enterCriticalSection;

    DWORD dwCurrentPlayCursor, dwCurrentWriteCursor;
    HRESULT hRet = pDSBuffer->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor);

    if (hRet != DS_OK) {
        EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "GetCurrentPosition Failed!");
    }

    if (pdwCurrentPlayCursor != xbnullptr) {
        *pdwCurrentPlayCursor = DSoundBufferGetXboxBufferSize(EmuFlags, dwCurrentPlayCursor);
    }
    if (pdwCurrentWriteCursor != xbnullptr) {
        *pdwCurrentWriteCursor = DSoundBufferGetXboxBufferSize(EmuFlags, dwCurrentWriteCursor);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

/*
// NOTE: Flags are 100% not compatible, not use Hybrid version
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_GetStatus(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    PDWORD                  pdwStatus)
{

    enterCriticalSection;

    HRESULT hRet = pDSBuffer->GetStatus(pdwStatus);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}*/

/*
inline HRESULT HybridDirectSoundBuffer_GetVoiceProperties(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{

    return DS_OK;
}

//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Lock(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Pause(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DWORD                   dwPause,
    DWORD                  &dwEmuFlags,
    DWORD                   dwEmuPlayFlags,
    bool                    triggerPlayPermission,
    REFERENCE_TIME          rtTimeStamp,
    REFERENCE_TIME         &Xb_rtTimeStamp)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    switch (dwPause) {
        case X_DSSPAUSE_RESUME:
            if (triggerPlayPermission) {
                pDSBuffer->Play(0, 0, dwEmuPlayFlags);
            }
            DSoundBufferSynchPlaybackFlagRemove(dwEmuFlags);
            dwEmuFlags &= ~DSE_FLAG_PAUSE;
            Xb_rtTimeStamp = 0;
            break;
        case X_DSSPAUSE_PAUSE:
            pDSBuffer->Stop();
            DSoundBufferSynchPlaybackFlagRemove(dwEmuFlags);
            dwEmuFlags |= DSE_FLAG_PAUSE;
            Xb_rtTimeStamp = rtTimeStamp;
            break;
        case X_DSSPAUSE_SYNCHPLAYBACK:
            //TODO: Test case Rayman 3 - Hoodlum Havoc, Battlestar Galactica, Miami Vice, Star Wars: KotOR, and... ?

            //SynchPlayback flag append should only occur in HybridDirectSoundBuffer_Pause function, nothing else is able to do this.
            hRet = DSoundBufferSynchPlaybackFlagAdd(dwEmuFlags);
            if (hRet == DS_OK) {
                pDSBuffer->Stop();
            }
            break;
        // TODO: NOTE: If stream is playing, it perform same behavior as pause flag. If it is not played, it act as a queue until trigger to play it.
        case X_DSSPAUSE_PAUSENOACTIVATE:
            break;
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
inline HRESULT HybridDirectSoundBuffer_PauseEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}

//Not really require it.
//IDirectSoundStream
inline HRESULT HybridDirectSoundBuffer_Process(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/
/* PlayEx is now calling to Play function instead of this hybrid function.
//Both Play(Ex) only has one function, aka this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Play(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DWORD                  &dwFlags,
    DWORD                  &dwEmuFlags)
{

    if (dwFlags & ~(X_DSBPLAY_LOOPING | X_DSBPLAY_FROMSTART | X_DSBPLAY_SYNCHPLAYBACK)) {
        CxbxKrnlCleanup("Unsupported Playing Flags");
    }
    // rewind buffer
    if ((dwFlags & X_DSBPLAY_FROMSTART)) {
        if (pDSBuffer->SetCurrentPosition(0) != DS_OK) {
            EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Rewinding buffer failed!");
        }

        dwFlags &= ~X_DSBPLAY_FROMSTART;
    }

    if (dwFlags & X_DSBPLAY_SYNCHPLAYBACK) {
        dwFlags ^= X_DSBPLAY_SYNCHPLAYBACK;
        dwEmuFlags |= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
    }

    HRESULT hRet = DS_OK;

    if ((dwEmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) == 0) {
        hRet = pDSBuffer->Play(0, 0, dwFlags);
    }

    RETURN_RESULT_CHECK(hRet);
}
*/
/*
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_PlayEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    REFERENCE_TIME      rtTimeStamp,
    DWORD               dwEmuFlags)
{

    return DS_OK;
}*/

/*
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Restore(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetAllParameters(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    XTL::X_DS3DBUFFER*      pDS3DBufferParams,
    DWORD                   dwApply)
{
    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {

        DS3DBUFFER pDS3DBufferParamsTemp;
        pDS3DBufferParamsTemp.dwSize = sizeof(DS3DBUFFER);
        pDS3DBufferParamsTemp.vPosition = pDS3DBufferParams->vPosition;
        pDS3DBufferParamsTemp.vVelocity = pDS3DBufferParams->vVelocity;
        pDS3DBufferParamsTemp.dwInsideConeAngle = pDS3DBufferParams->dwInsideConeAngle;
        pDS3DBufferParamsTemp.dwOutsideConeAngle = pDS3DBufferParams->dwOutsideConeAngle;
        pDS3DBufferParamsTemp.vConeOrientation = pDS3DBufferParams->vConeOrientation;
        pDS3DBufferParamsTemp.lConeOutsideVolume = pDS3DBufferParams->lConeOutsideVolume;
        pDS3DBufferParamsTemp.flMinDistance = pDS3DBufferParams->flMinDistance;
        pDS3DBufferParamsTemp.flMaxDistance = pDS3DBufferParams->flMaxDistance;
        pDS3DBufferParamsTemp.dwMode = pDS3DBufferParams->dwMode;

        hRet = pDS3DBuffer->SetAllParameters(&pDS3DBufferParamsTemp, dwApply);
        if (hRet != DS_OK) {
            RETURN_RESULT_CHECK(hRet);
        }

        hRet = g_pDSoundPrimary3DListener8->SetDistanceFactor(pDS3DBufferParams->flDistanceFactor, dwApply);
        if (hRet != DS_OK) {
            RETURN_RESULT_CHECK(hRet);
        }

        hRet = g_pDSoundPrimary3DListener8->SetRolloffFactor(pDS3DBufferParams->flRolloffFactor, dwApply);
        if (hRet != DS_OK) {
            RETURN_RESULT_CHECK(hRet);
        }

        hRet = g_pDSoundPrimary3DListener8->SetDopplerFactor(pDS3DBufferParams->flDopplerFactor, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetBufferData(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetConeAngles(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetConeOrientation(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        // TODO: (DSound) Should we do restrictive or passive to return actual result back to titles?
        // Test case: Turok Evolution, Jet Set Radio Future, ?
        if (x == 0.0f && y == 0.0f && z == 0.0f) {
            printf("WARNING: SetConeOrientation was called with x = 0, y = 0, and z = 0. Current action is ignore call to PC.\n");
        } else {
            hRet = pDS3DBuffer->SetConeOrientation(x, y, z, dwApply);
        }
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetConeOutsideVolume(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetConeOutsideVolume(lConeOutsideVolume, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetCurrentPosition(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwNewPosition)
{

    return DS_OK;
}*/

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DListener_SetDistanceFactor(
    LPDIRECTSOUND3DLISTENER8    pDS3DListener,
    FLOAT                       flDistanceFactor,
    DWORD                       dwApply)
{

    enterCriticalSection;

    HRESULT hRet = pDS3DListener->SetDistanceFactor(flDistanceFactor, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DListener_SetDopplerFactor(
    LPDIRECTSOUND3DLISTENER8    pDS3DListener,
    FLOAT                       flDopplerFactor,
    DWORD                       dwApply)
{

    enterCriticalSection;

    HRESULT hRet = pDS3DListener->SetDopplerFactor(flDopplerFactor, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//TODO: PC DirectSound does not have SetHeadroom method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetEG(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LPVOID              pEnvelopeDesc)
{

    return DS_OK;
}

//TODO: PC DirectSound does not have SetHeadroom method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetFilter(
    LPDIRECTSOUNDBUFFER8     pDSBuffer,
    XTL::X_DSFILTERDESC*    pFilterDesc)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetFormat(
    LPDIRECTSOUNDBUFFER8   &pDSBuffer,
    LPCWAVEFORMATEX         pwfxFormat,
    DSBUFFERDESC           &BufferDesc,
    DWORD                  &dwEmuFlags,
    DWORD                  &dwPlayFlags,
    LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer,
    bool                    X_BufferAllocate,
    LPVOID                 &X_BufferCache,
    DWORD                  &X_BufferCacheSize)
{

    enterCriticalSection;

    pDSBuffer->Stop();

    if (X_BufferAllocate) {
        GeneratePCMFormat(BufferDesc, pwfxFormat, dwEmuFlags, X_BufferCacheSize, xbnullptr, X_BufferCacheSize);
    // Don't allocate for DS Stream class, it is using straight from the source.
    } else {
        GeneratePCMFormat(BufferDesc, pwfxFormat, dwEmuFlags, 0, xbnullptr, X_BufferCacheSize);
    }
    HRESULT hRet = DS_OK;
    if (g_pDSoundPrimaryBuffer == pDSBuffer) {
        hRet = pDSBuffer->SetFormat(BufferDesc.lpwfxFormat);
    } else {
        // DSound Stream only
        if (X_BufferCacheSize == 0) {
            // Allocate at least 5 second worth of bytes in PCM format.
            BufferDesc.dwBufferBytes = BufferDesc.lpwfxFormat->nAvgBytesPerSec * 5;
        }
        DSoundBufferReplace(pDSBuffer, BufferDesc, dwPlayFlags, pDS3DBuffer);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetFrequency(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwFrequency)
{

    enterCriticalSection;

    HRESULT hRet = S_OK;

    hRet = pDSBuffer->SetFrequency(dwFrequency);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

HRESULT HybridDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER8, LONG, DWORD, LPLONG, LONG, DWORD);

//IDirectSoundStream
//IDirectSoundBuffer
// NOTE: 0 to 10,000; For initialize value would be 3D default to 0 and 2D default to 600.
inline HRESULT HybridDirectSoundBuffer_SetHeadroom(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwHeadroom,
    DWORD              &Xb_dwHeadroom,
    LONG                Xb_volume,
    LONG                Xb_volumeMixbin,
    DWORD               dwEmuFlags)
{

    enterCriticalSection;

    HRESULT hRet;
    if (dwHeadroom > 10000) {
        hRet = DSERR_INVALIDPARAM;
    } else {
        Xb_dwHeadroom = dwHeadroom;
        hRet = DS_OK;
        HybridDirectSoundBuffer_SetVolume(pDSBuffer, Xb_volume, dwEmuFlags, xbnullptr, Xb_volumeMixbin, dwHeadroom);
    }

    leaveCriticalSection;

    return DS_OK;
}

/*
//TODO: PC DirectSound does not have SetI3DL2Source method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetI3DL2Source(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    PVOID               pds3db,
    DWORD               dwApply)
{

    return DS_OK;
}

//TODO: PC DirectSound does not have SetLFO method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetLFO(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LPCDSLFODESC        pLFODesc)
{

    return DS_OK;
}*/

/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetLoopRegion(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwLoopStart,
    DWORD               dwLoopLength)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetMaxDistance(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                flMaxDistance,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMaxDistance(flMaxDistance, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetMinDistance(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                flMinDistance,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMinDistance(flMinDistance, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//TODO: PC DirectSound does not have SetMixBins method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetMixBins(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    PVOID               pMixBins)
{

    return DS_OK;
}*/

//IDirectSoundStream x2
//IDirectSoundBuffer x2
inline HRESULT HybridDirectSoundBuffer_SetMixBinVolumes_8(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    XTL::X_LPDSMIXBINS   pMixBins,
    DWORD                EmuFlags,
    LONG                 Xb_volume,
    LONG                &Xb_volumeMixBin,
    DWORD                Xb_dwHeadroom)
{
    enterCriticalSection;

    HRESULT hRet = DSERR_INVALIDPARAM;

    if (pMixBins != xbnullptr) {
        DWORD counter = pMixBins->dwCount, count = pMixBins->dwCount;
        LONG volume = 0;
        if (pMixBins->lpMixBinVolumePairs != xbnullptr) {
            // Let's normalize audio level except for low frequency (subwoofer)
            for (DWORD i = 0; i < count; i++) {
#if 0 // This code isn't ideal for DirectSound, since it's not possible to set volume for each speakers.
                if (pMixBins->lpMixBinVolumePairs[i].dwMixBin != XDSMIXBIN_LOW_FREQUENCY
                    // We only want to focus on speaker volumes, nothing else.
                    && pMixBins->lpMixBinVolumePairs[i].dwMixBin < XDSMIXBIN_SPEAKERS_MAX) {
#endif
                if (pMixBins->lpMixBinVolumePairs[i].dwMixBin == XDSMIXBIN_FRONT_LEFT
                    // We only want to focus on front speaker volumes, nothing else.
                    || pMixBins->lpMixBinVolumePairs[i].dwMixBin == XDSMIXBIN_FRONT_RIGHT) {
                    volume += pMixBins->lpMixBinVolumePairs[i].lVolume;
                } else {
                    counter--;
                }
            }
            if (counter > 0) {
                Xb_volumeMixBin = volume / (LONG)counter;
                hRet = HybridDirectSoundBuffer_SetVolume(pDSBuffer, Xb_volume, EmuFlags, nullptr,
                                                         Xb_volumeMixBin, Xb_dwHeadroom);
            } else {
                hRet = DS_OK;
            }
        }
    }

    leaveCriticalSection;

    return hRet;
}

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetMode(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    DWORD                   dwMode,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMode(dwMode, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetNotificationPositions(
    LPDIRECTSOUNDBUFFER8     pDSBuffer,
    DWORD                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies)
{

    return DS_OK;
}*/
/*
//TODO: PC DirectSound does not have SetOutputBuffer method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetOutputBuffer(
    LPDIRECTSOUNDBUFFER8     pDSBuffer,
    X_CDirectSoundBuffer*   pOutputBuffer)
{

    return DS_OK;
}
*/
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetPitch(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LONG                lPitch)
{

    // Convert pitch back to frequency
    if (lPitch == 0) {
        lPitch = 48000; // NOTE: pitch = 0 is equal to 48 KHz.
    } else {
        lPitch = static_cast<LONG>(exp((lPitch / 4096.0f) * log(2)) * 48000.0f);
    }

    /* For research purpose of how to convert frequency to pitch and back to frequency.
    // Edit hertz variable to see the result.
    float hertz = 12000.0f;

    float hertzRatio = 48000.0f;
    float pitchRatio = 4096.0f;

    // Convert hertz to pitch
    float pitch = log2(hertz / hertzRatio) * pitchRatio;

    // Convert pitch to hertz
    hertz = exp((pitch / pitchRatio) * log(2)) * hertzRatio;*/

    RETURN_RESULT_CHECK(pDSBuffer->SetFrequency(lPitch));
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetPlayRegion(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwPlayStart,
    DWORD               dwPlayLength)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetPosition(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetPosition(x, y, z, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*
//TODO: PC DirectSound does not have SetRolloffCurve method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetRolloffCurve(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    const FLOAT        *pflPoints,
    DWORD               dwPointCount,
    DWORD               dwApply)
{

    return DS_OK;
}*/

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DListener_SetRolloffFactor(
    LPDIRECTSOUND3DLISTENER8    pDSBuffer,
    FLOAT                       fRolloffFactor,
    DWORD                       dwApply)
{

    enterCriticalSection;

    HRESULT hRet = pDSBuffer->SetRolloffFactor(fRolloffFactor, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSound3DBuffer_SetVelocity(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{

    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetVelocity(x, y, z, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream x2
//IDirectSoundBuffer
// NOTE: real volume = mixbins volume + 3D volume + volume - headroom
// 100 millibels (mB) = 1 dB
inline HRESULT HybridDirectSoundBuffer_SetVolume(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    LONG                    lVolume,
    DWORD                   dwEmuFlags,
    LPLONG                  Xb_lpVolume,
    LONG                    Xb_volumeMixbin,
    DWORD                   Xb_dwHeadroom)
{

    enterCriticalSection;

    // Preserve original volume
    if (Xb_lpVolume != xbnullptr) {
        *Xb_lpVolume = lVolume;
    }

#if 0 // TODO: Restore it once DSound work update comes up
    // For time being, this log is kept in case of something changed somewhere making a wrong input into the API.
    printf("DEBUG: SetVolume | lVolume = %ld | volumeMixbin = %ld | dwHeadroom = %8u\n", lVolume, Xb_volumeMixbin, Xb_dwHeadroom);
#endif

    lVolume += Xb_volumeMixbin - Xb_dwHeadroom;

    if ((dwEmuFlags & DSE_FLAG_PCM) > 0) {
        if (!g_XBAudio.codec_pcm) {
            lVolume = DSBVOLUME_MIN;
        }
    } else if ((dwEmuFlags & DSE_FLAG_XADPCM) > 0) {
        if (!g_XBAudio.codec_xadpcm) {
            lVolume = DSBVOLUME_MIN;
        }
    } else if ((dwEmuFlags & DSE_FLAG_PCM_UNKNOWN) > 0) {
        if (!g_XBAudio.codec_unknown) {
            lVolume = DSBVOLUME_MIN;
        }
    }
    if (lVolume <= -6400 && lVolume != DSBVOLUME_MIN) {
        lVolume = DSBVOLUME_MIN;
    } else if (lVolume > 0) {
        EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "HybridDirectSoundBuffer_SetVolume has received greater than 0: %ld", lVolume);
        lVolume = 0;
    }
    if ((dwEmuFlags & DSE_FLAG_DEBUG_MUTE) > 0) {
        lVolume = DSBVOLUME_MIN;
    }

    HRESULT hRet = pDSBuffer->SetVolume(lVolume);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}
/*/
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Stop(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}

//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_StopEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    REFERENCE_TIME      rtTimeStamp,
    DWORD               dwEmuFlags)
{

    return DS_OK;
}

//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Unlock(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}//*/

#endif
