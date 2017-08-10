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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUDSOUNDINLINE_H
#define EMUDSOUNDINLINE_H

#include "XADPCM.h"

// LOG_UNIMPLEMENTED indicates that Cxbx is missing an implementation from DSound api
#define LOG_UNIMPLEMENTED_DSOUND() \
    static bool b_echoOnce = true; \
    if (g_bPrintfOn && b_echoOnce) { \
            std::cout << __func__ << " unimplemented!\n"; \
            b_echoOnce = false; \
    }

CRITICAL_SECTION                    g_DSoundCriticalSection;
#define enterCriticalSection        EnterCriticalSection(&g_DSoundCriticalSection)
#define leaveCriticalSection        LeaveCriticalSection(&g_DSoundCriticalSection)

//Xbox Audio Decoder by blueshogun96 and revised by RadWolfie
void DSoundBufferXboxAdpcmDecoder(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DSBUFFERDESC*           pDSBufferDesc,
    DWORD                   dwOffset,
    LPVOID                  pAudioPtr,
    DWORD                   dwAudioBytes,
    LPVOID                  pAudioPtr2,
    DWORD                   dwAudioBytes2,
    bool                    isLock)
{

    if (!pDSBuffer) {
        return;
    }

    if (!pDSBufferDesc) {
        return;
    }
    if (!pAudioPtr) {
        return;
    }

    // Predict the size of the converted buffers we're going to need
    DWORD dwDecodedAudioBytes = TXboxAdpcmDecoder_guess_output_size(dwAudioBytes) * pDSBufferDesc->lpwfxFormat->nChannels;
    DWORD dwDecodedAudioBytes2 = 0;
    if (dwAudioBytes2 != 0) {
        dwDecodedAudioBytes2 = TXboxAdpcmDecoder_guess_output_size(dwAudioBytes2) * pDSBufferDesc->lpwfxFormat->nChannels;
    }

    // Allocate some temp buffers
    uint8_t* buffer1 = (uint8_t*)malloc(dwDecodedAudioBytes);
    uint8_t* buffer2 = NULL;

    if (dwAudioBytes2 != 0) {
        buffer2 = (uint8_t*)malloc(dwDecodedAudioBytes2);
    }
    // Attempt to decode Xbox ADPCM data to PCM
    //EmuWarning( "Guessing output size to be 0x%X bytes as opposed to 0x%X bytes.", TXboxAdpcmDecoder_guess_output_size(dwAudioBytes), dwAudioBytes );
    TXboxAdpcmDecoder_Decode_Memory((uint8_t*)pAudioPtr, dwAudioBytes, &buffer1[0], pDSBufferDesc->lpwfxFormat->nChannels);
    if (dwAudioBytes2 != 0) {
        TXboxAdpcmDecoder_Decode_Memory((uint8_t*)pAudioPtr2, dwAudioBytes2, &buffer2[0], pDSBufferDesc->lpwfxFormat->nChannels);
    }
    // Lock this Xbox ADPCM buffer
    void* pPtrX = NULL, *pPtrX2 = NULL;
    DWORD dwBytesX = 0, dwBytesX2 = 0;

    HRESULT hr = DS_OK;
    if (isLock == false) {
        hr = pDSBuffer->Lock(dwOffset, pDSBufferDesc->dwBufferBytes, &pPtrX, &dwBytesX, &pPtrX2, &dwBytesX2, 0);
    }
    if (SUCCEEDED(hr)) {
        // Write the converted PCM buffer bytes

        if (isLock == false) {

            if (dwDecodedAudioBytes > dwBytesX) dwDecodedAudioBytes = dwBytesX;
            memcpy(pPtrX, buffer1, dwDecodedAudioBytes);

            if (pPtrX2 != NULL) {
                if (dwDecodedAudioBytes2 > dwBytesX2) dwDecodedAudioBytes2 = dwBytesX2;
                memcpy(pPtrX2, buffer2, dwDecodedAudioBytes2);
            }

            hr = pDSBuffer->Unlock(pPtrX, dwBytesX, pPtrX2, dwBytesX2);

        } else {

            if (dwDecodedAudioBytes > dwAudioBytes) dwDecodedAudioBytes = dwAudioBytes;
            memcpy(pAudioPtr, buffer1, dwDecodedAudioBytes);

            if (pAudioPtr2 != NULL) {
                if (dwDecodedAudioBytes2 > dwAudioBytes2) dwDecodedAudioBytes2 = dwAudioBytes2;
                memcpy(pAudioPtr2, buffer2, dwDecodedAudioBytes2);
            }
            hr = pDSBuffer->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
        }
    }

    // Clean up our mess
    if (buffer1) free(buffer1);
    if (buffer2) free(buffer2);
}

// Convert XADPCM to PCM format helper function
inline void XADPCM2PCMFormat(LPWAVEFORMATEX lpwfxFormat)
{

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    DbgPrintf("EmuDSound: XADPCM WAVEFORMATEX\n"
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
    //lpwfxFormat.cbSize;             /* the count in bytes of the size of */

    lpwfxFormat->wBitsPerSample = 16;
    lpwfxFormat->nAvgBytesPerSec = lpwfxFormat->nChannels * lpwfxFormat->nSamplesPerSec * (lpwfxFormat->wBitsPerSample / 8);
    lpwfxFormat->nBlockAlign = 2 * lpwfxFormat->nChannels;
    lpwfxFormat->cbSize = 0;
    //Enable this only if you have Xbox ADPCM Codec installed on your PC, or else it will fail every time.
    //This is just to verify format conversion is correct or not.
#if 0
    if (waveOutOpen(NULL, WAVE_MAPPER, lpwfxFormat, NULL, NULL, WAVE_FORMAT_QUERY) != MMSYSERR_NOERROR) {
        return DSERR_BADFORMAT;
    }
#endif

#if 0 //For testing purpose if XADPCM to PCM is not accurate.
    DbgPrintf("EmuDSound: Converted to PCM WAVEFORMATEX\n"
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

inline void GeneratePCMFormat(
    DSBUFFERDESC*   pDSBufferDesc,
    LPCWAVEFORMATEX lpwfxFormat,
    DWORD          &dwEmuFlags)
{
    bool bIsSpecial = false;
    DWORD checkAvgBps;

    // convert from Xbox to PC DSound
    {

        if (pDSBufferDesc->dwBufferBytes < DSBSIZE_MIN) {
            pDSBufferDesc->dwBufferBytes = DSBSIZE_MIN;
        } else if (pDSBufferDesc->dwBufferBytes > DSBSIZE_MAX) {
            pDSBufferDesc->dwBufferBytes = DSBSIZE_MAX;
        }
        pDSBufferDesc->dwReserved = 0;

        if (lpwfxFormat != NULL) {

            //TODO: RadWolfie - Need implement support for WAVEFORMATEXTENSIBLE as stated in CDirectSoundStream_SetFormat function note below
            // Do we need to convert it? Or just only do the WAVEFORMATEX only?

            // NOTE: pwfxFormat is not always a WAVEFORMATEX structure, it can
            // be WAVEFORMATEXTENSIBLE if that's what the programmer(s) wanted
            // in the first place, FYI.

            if (lpwfxFormat->cbSize > 22) {
                EmuWarning("WAVEFORMATEXTENSIBLE is detected!");
            }

            pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)g_MemoryManager.Allocate(sizeof(WAVEFORMATEX) + lpwfxFormat->cbSize);
            memcpy(pDSBufferDesc->lpwfxFormat, lpwfxFormat, sizeof(WAVEFORMATEX));

            dwEmuFlags = dwEmuFlags & ~DSB_FLAG_AUDIO_CODECS;

            switch (pDSBufferDesc->lpwfxFormat->wFormatTag) {
                case WAVE_FORMAT_PCM:
                    dwEmuFlags |= DSB_FLAG_PCM;

                    //TODO: Phantasy Star Online Episode I & II made an attempt to use avg byte/second below sample/second requirement.
                    //In other word, this is a workaround to fix title mistake...
                    checkAvgBps = lpwfxFormat->nSamplesPerSec * lpwfxFormat->nBlockAlign;
                    if (lpwfxFormat->nAvgBytesPerSec < checkAvgBps) {
                        pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = checkAvgBps;
                    }
                    break;
                case WAVE_FORMAT_XBOX_ADPCM:
                    dwEmuFlags |= DSB_FLAG_XADPCM;
                    XADPCM2PCMFormat(pDSBufferDesc->lpwfxFormat);
                    break;
                default:
                    dwEmuFlags |= DSB_FLAG_PCM_UNKNOWN;
                    break;
            }
        } else {
            bIsSpecial = true;
            dwEmuFlags |= DSB_FLAG_RECIEVEDATA;

            EmuWarning("Creating dummy WAVEFORMATEX (pdsbd->lpwfxFormat = NULL)...");

            // HACK: This is a special sound buffer, create dummy WAVEFORMATEX data.
            // It's supposed to recieve data rather than generate it.  Buffers created
            // with flags DSBCAPS_MIXIN, DSBCAPS_FXIN, and DSBCAPS_FXIN2 will have no
            // WAVEFORMATEX structure by default.

            // TODO: A better response to this scenario if possible.

            pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)g_MemoryManager.Allocate(sizeof(WAVEFORMATEX));

            //memset(pDSBufferDescSpecial->lpwfxFormat, 0, sizeof(WAVEFORMATEX)); 
            //memset(pDSBufferDescSpecial, 0, sizeof(DSBUFFERDESC)); 

            pDSBufferDesc->lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
            pDSBufferDesc->lpwfxFormat->nChannels = 2;
            pDSBufferDesc->lpwfxFormat->nSamplesPerSec = 22050;
            pDSBufferDesc->lpwfxFormat->nBlockAlign = 4;
            pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec * pDSBufferDesc->lpwfxFormat->nBlockAlign;
            pDSBufferDesc->lpwfxFormat->wBitsPerSample = 16;

            pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
            pDSBufferDesc->dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
            pDSBufferDesc->dwBufferBytes = 3 * pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec;

            //        pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)g_MemoryManager.Allocate(sizeof(WAVEFORMATEX)/*+pdsbd->lpwfxFormat->cbSize*/);

            ////    pDSBufferDesc->lpwfxFormat->cbSize = sizeof( WAVEFORMATEX );
            //        pDSBufferDesc->lpwfxFormat->nChannels = 1;
            //        pDSBufferDesc->lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
            //        pDSBufferDesc->lpwfxFormat->nSamplesPerSec = 22050;
            //        pDSBufferDesc->lpwfxFormat->nBlockAlign = 4;
            //        pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = 4 * 22050;
            //        pDSBufferDesc->lpwfxFormat->wBitsPerSample = 16;

            // Give this buffer 3 seconds of data if needed
            /*if(pdsbd->dwBufferBytes == 0) {
                pDSBufferDesc->dwBufferBytes = 3 * pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec;
            }*/
        }

        pDSBufferDesc->guid3DAlgorithm = DS3DALG_DEFAULT;
    }

    // sanity check
    if (!bIsSpecial) {
        if (pDSBufferDesc->lpwfxFormat->nBlockAlign != (pDSBufferDesc->lpwfxFormat->nChannels*pDSBufferDesc->lpwfxFormat->wBitsPerSample) / 8) {
            pDSBufferDesc->lpwfxFormat->nBlockAlign = (2 * pDSBufferDesc->lpwfxFormat->wBitsPerSample) / 8;
            pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec * pDSBufferDesc->lpwfxFormat->nBlockAlign;
        }
    }

    //TODO: is this needed?
    if (pDSBufferDesc->lpwfxFormat != NULL) {
        // we only support 2 channels right now
        if (pDSBufferDesc->lpwfxFormat->nChannels > 2) {
            pDSBufferDesc->lpwfxFormat->nChannels = 2;
            pDSBufferDesc->lpwfxFormat->nBlockAlign = (2 * pDSBufferDesc->lpwfxFormat->wBitsPerSample) / 8;
            pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec * pDSBufferDesc->lpwfxFormat->nBlockAlign;
        }
    }
}

inline void DSoundGenericUnlock(
    DWORD                   dwEmuFlags,
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    LPDSBUFFERDESC          pDSBufferDesc,
    DWORD                   dwOffset,
    LPVOID                 &pLockPtr1,
    DWORD                   dwLockBytes1,
    LPVOID                  pLockPtr2,
    DWORD                   dwLockBytes2,
    DWORD                   dwLockFlags)
{
    // close any existing locks
    if (pLockPtr1 != nullptr) {
        if (dwEmuFlags & DSB_FLAG_XADPCM) {

            //Since it is already locked, don't even need this.
            /*if (dwLockFlags & DSBLOCK_FROMWRITECURSOR) {

            }*/
            if (dwLockFlags & DSBLOCK_ENTIREBUFFER) {
                dwLockBytes1 = pDSBufferDesc->dwBufferBytes;
            }


            DSoundBufferXboxAdpcmDecoder(pDSBuffer,
                                         pDSBufferDesc,
                                         dwOffset,
                                         pLockPtr1,
                                         dwLockBytes1,
                                         pLockPtr2,
                                         dwLockBytes2,
                                         true);
        } else {
            pDSBuffer->Unlock(pLockPtr1, dwLockBytes1, pLockPtr2, dwLockBytes2);
        }

        pLockPtr1 = 0;
    }

}

// resize an emulated directsound buffer, if necessary
inline void ResizeIDirectSoundBuffer(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    LPDSBUFFERDESC              pDSBufferDesc,
    DWORD                       PlayFlags,
    DWORD                       dwBytes,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer)
{

    if (dwBytes == pDSBufferDesc->dwBufferBytes || dwBytes == 0) {
        return;
    }
    DbgPrintf("EmuResizeIDirectSoundBuffer8 : Resizing! (0x%.08X->0x%.08X)\n", pDSBufferDesc->dwBufferBytes, dwBytes);

    DWORD dwPlayCursor, dwWriteCursor, dwStatus, refCount;
    LONG lVolume;

    pDSBuffer->GetVolume(&lVolume);

    HRESULT hRet = pDSBuffer->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("Unable to retrieve current position for resize reallocation!");
    }
    hRet = pDSBuffer->GetStatus(&dwStatus);

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("Unable to retrieve current status for resize reallocation!");
    }

    if (pDS3DBuffer != NULL) {
        pDS3DBuffer->Release();
    }
    // release old buffer
    refCount = pDSBuffer->Release();
    if (refCount) {
        while (pDSBuffer->Release() > 0) {}
    }
    pDSBufferDesc->dwBufferBytes = dwBytes;

    //Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
    LPDIRECTSOUNDBUFFER pTempBuffer;
    hRet = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, NULL);

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("CreateSoundBuffer Failed!");
        pDSBufferDesc = NULL;
    } else {
        hRet = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&(pDSBuffer));
        pTempBuffer->Release();
    }

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("IDirectSoundBuffer8 resize Failed!");
    }
    if (refCount) {
        while (pDSBuffer->AddRef() < refCount);
    }
    if (pDSBufferDesc->dwFlags & DSBCAPS_CTRL3D) {
        HRESULT hRet3D = pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&(pDS3DBuffer));
        if (FAILED(hRet3D)) {
            EmuWarning("CreateSound3DBuffer Failed!");
            pDS3DBuffer = NULL;
        }
    }

    pDSBuffer->SetVolume(lVolume);

    pDSBuffer->SetCurrentPosition(dwPlayCursor);

    if (dwStatus & DSBSTATUS_PLAYING) {
        pDSBuffer->Play(0, 0, PlayFlags);
    }
}

inline void DSoundBufferUpdate(
    LPDIRECTSOUNDBUFFER8    pThis,
    LPDSBUFFERDESC          pDSBufferDesc,
    LPVOID                  pBuffer,
    DWORD                   dwEmuFlags,
    DWORD                   dwOffset,
    LPVOID                 &pLockPtr1,
    DWORD                   dwLockBytes1,
    LPVOID                  pLockPtr2,
    DWORD                   dwLockBytes2,
    DWORD                   dwLockFlags)
{

    PVOID pAudioPtr, pAudioPtr2;
    DWORD dwAudioBytes, dwAudioBytes2, dwStatus;

    HRESULT hRet = pThis->GetStatus(&dwStatus);
    if (hRet == DS_OK) {
        if ((dwStatus & DSBSTATUS_PLAYING)) {

            DSoundGenericUnlock(dwEmuFlags,
                                pThis,
                                pDSBufferDesc,
                                dwOffset,
                                pLockPtr1,
                                dwLockBytes1,
                                pLockPtr2,
                                dwLockBytes2,
                                dwLockFlags);

            if (dwEmuFlags & DSB_FLAG_XADPCM) {
                DSoundBufferXboxAdpcmDecoder(pThis, pDSBufferDesc, dwOffset, pBuffer, pDSBufferDesc->dwBufferBytes, 0, 0, false);
            } else {
                HRESULT hRet = pThis->Lock(dwOffset, pDSBufferDesc->dwBufferBytes, &pAudioPtr, &dwAudioBytes, &pAudioPtr2, &dwAudioBytes2, 0);

                if (SUCCEEDED(hRet)) {
                    if (pAudioPtr != nullptr) {
                        memcpy(pAudioPtr, pBuffer, dwAudioBytes);
                    }
                    if (pAudioPtr2 != nullptr) {
                        memcpy(pAudioPtr2, (PVOID)((DWORD)pBuffer + dwAudioBytes), dwAudioBytes2);
                    }
                    pThis->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
                }
            }
        }
    }
}

// Generic force remove synch playback control flag.
inline void DSoundBufferRemoveSynchPlaybackFlag(
    DWORD                  &dwEmuFlags

    )
{
    if (g_iDSoundSynchPlaybackCounter == 0) {
        return;
    }

    if (dwEmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) {
        g_iDSoundSynchPlaybackCounter--;
        dwEmuFlags ^= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
    }
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

//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_GetCurrentPosition(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor)
{

    enterCriticalSection;

    HRESULT hRet = pDSBuffer->GetCurrentPosition(pdwCurrentPlayCursor, pdwCurrentWriteCursor);

    if (FAILED(hRet)) {
        EmuWarning("GetCurrentPosition Failed!");
    }
    if (pdwCurrentPlayCursor != 0 && pdwCurrentWriteCursor != 0) {
        DbgPrintf("*pdwCurrentPlayCursor := %d, *pdwCurrentWriteCursor := %d\n", *pdwCurrentPlayCursor, *pdwCurrentWriteCursor);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

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
}

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
    DWORD                  &dwEmuFlags)
{

    enterCriticalSection;

    DWORD dwStatus;
    HRESULT hRet = DS_OK, hStatus;
    switch (dwPause) {
        case X_DSSPAUSE_RESUME:
            pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
            DSoundBufferRemoveSynchPlaybackFlag(dwEmuFlags);
            break;
        case X_DSSPAUSE_PAUSE:
            hStatus = pDSBuffer->GetStatus(&dwStatus);
            if (hStatus == DS_OK && dwStatus & DSBSTATUS_PLAYING) {
                pDSBuffer->Stop();
            }
            DSoundBufferRemoveSynchPlaybackFlag(dwEmuFlags);
            break;
        case X_DSSPAUSE_SYNCHPLAYBACK:
            //TODO: Test case Rayman 3 - Hoodlum Havoc, Battlestar Galactica, Miami Vice, and... ?

            //SynchPlayback flag append should only occur in HybridDirectSoundBuffer_Pause function, nothing else is able to do this.
            if (g_iDSoundSynchPlaybackCounter >= DSOUND_MAX_SYNCHPLAYBACK_AUDIO) {
                hRet = DSERR_GENERIC;
            } else {

                g_iDSoundSynchPlaybackCounter++;
                dwEmuFlags |= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
                hRet = pDSBuffer->GetStatus(&dwStatus);
                if (hRet == DS_OK && dwStatus & DSBSTATUS_PLAYING) {
                    pDSBuffer->Stop();
                    pDSBuffer->SetCurrentPosition(0);
                }
            }
            break;
        case X_DSSPAUSE_PAUSENOACTIVATE:
            EmuWarning("X_DSSPAUSE_PAUSENOACTIVATE is unsupported!");
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
        if (FAILED(pDSBuffer->SetCurrentPosition(0))) {
            EmuWarning("Rewinding buffer failed!");
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
    LPCDS3DBUFFER           pDS3DBufferParams,
    DWORD                   dwApply)
{
    enterCriticalSection;

    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetAllParameters(pDS3DBufferParams, dwApply);
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

    HRESULT hRet = pDS3DBuffer->SetConeOrientation(x, y, z, dwApply);

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
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LPCWAVEFORMATEX     pwfxFormat,
    LPDSBUFFERDESC      pBufferDesc,
    DWORD              &dwEmuFlags)
{

    enterCriticalSection;

    GeneratePCMFormat(pBufferDesc, pwfxFormat, dwEmuFlags);

    HRESULT hRet = DS_OK;
    if (g_pDSoundPrimaryBuffer == pDSBuffer) {
        hRet = pDSBuffer->SetFormat(pBufferDesc->lpwfxFormat);
    } else {
        //TODO: RadWolfie - Need to create a new buffer in order for this to work base on MSDN document.
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

/*
//TODO: PC DirectSound does not have SetHeadroom method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetHeadroom(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwHeadroom)
{

    return DS_OK;
}

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
}

//TODO: PC DirectSound does not have SetMixBinVolumes method function.
//IDirectSoundStream x2
//IDirectSoundBuffer x2
inline HRESULT HybridDirectSoundBuffer_SetMixBinVolumes(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

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

//TODO: PC DirectSound does not have SetPitch method function.
//IDirectSoundStream
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_SetPitch(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LONG                lPitch)
{

    // TODO: Translate params, then make the PC DirectSound call

    return DS_OK;
}

//TODO: PC DirectSound does not have SetPitch method function.
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
inline HRESULT HybridDirectSoundBuffer_SetVolume(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LONG                lVolume,
    DWORD               dwEmuFlags)
{

    enterCriticalSection;

    if (dwEmuFlags & DSB_FLAG_PCM) {
        if (!g_XBAudio.GetPCM()) {
            lVolume = -10000;
        }
    } else if (dwEmuFlags & DSB_FLAG_XADPCM) {
        if (!g_XBAudio.GetXADPCM()) {
            lVolume = -10000;
        }
    } else if (dwEmuFlags & DSB_FLAG_PCM_UNKNOWN) {
        if (!g_XBAudio.GetUnknownCodec()) {
            lVolume = -10000;
        }
    }
    if (lVolume > -10000 && lVolume <= -6400) {
        lVolume = -10000;
    } else if (lVolume > 0) {
        EmuWarning("HybridDirectSoundBuffer_SetVolume has received greater than 0: %d", lVolume);
        lVolume = 0;
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

//TODO: RadWolfie - This is where I left off
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_StopEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    REFERENCE_TIME      rtTimeStamp,
    DWORD               dwEmuFlags)
{

    return DS_OK;
}

// Xbox DSound does not seem to have Unlock function, has been confirmed with Cxbx developers.
//IDirectSoundBuffer
inline HRESULT HybridDirectSoundBuffer_Unlock(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}//*/

#endif
