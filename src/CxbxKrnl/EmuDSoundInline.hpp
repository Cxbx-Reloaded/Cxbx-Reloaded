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
    uint8_t* buffer2 = nullptr;

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
    void* pPtrX = xbnullptr, *pPtrX2 = xbnullptr;
    DWORD dwBytesX = 0, dwBytesX2 = 0;

    HRESULT hr = DS_OK;
    if (isLock == false) {
        hr = pDSBuffer->Lock(dwOffset, pDSBufferDesc->dwBufferBytes, &pPtrX, &dwBytesX, &pPtrX2, &dwBytesX2, 0);
    }
    if (hr == DS_OK) {
        // Write the converted PCM buffer bytes

        if (isLock == false) {

            if (dwDecodedAudioBytes > dwBytesX) dwDecodedAudioBytes = dwBytesX;
            memcpy(pPtrX, buffer1, dwDecodedAudioBytes);

            if (pPtrX2 != xbnullptr) {
                if (dwDecodedAudioBytes2 > dwBytesX2) dwDecodedAudioBytes2 = dwBytesX2;
                memcpy(pPtrX2, buffer2, dwDecodedAudioBytes2);
            }

            hr = pDSBuffer->Unlock(pPtrX, dwBytesX, pPtrX2, dwBytesX2);

        } else {

            if (dwDecodedAudioBytes > dwAudioBytes) dwDecodedAudioBytes = dwAudioBytes;
            memcpy(pAudioPtr, buffer1, dwDecodedAudioBytes);

            if (pAudioPtr2 != xbnullptr) {
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

        if (lpwfxFormat != xbnullptr) {

            //TODO: RadWolfie - Need implement support for WAVEFORMATEXTENSIBLE as stated in CDirectSoundStream_SetFormat function note below
            // Do we need to convert it? Or just only do the WAVEFORMATEX only?

            // NOTE: pwfxFormat is not always a WAVEFORMATEX structure, it can
            // be WAVEFORMATEXTENSIBLE if that's what the programmer(s) wanted
            // in the first place, FYI.

            if (lpwfxFormat->cbSize > 22) {
                EmuWarning("WAVEFORMATEXTENSIBLE is detected!");
            }
            if (pDSBufferDesc->lpwfxFormat == nullptr) {
                pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX) + lpwfxFormat->cbSize);
            }
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

            EmuWarning("Creating dummy WAVEFORMATEX (pdsbd->lpwfxFormat = xbnullptr)...");

            // HACK: This is a special sound buffer, create dummy WAVEFORMATEX data.
            // It's supposed to recieve data rather than generate it.  Buffers created
            // with flags DSBCAPS_MIXIN, DSBCAPS_FXIN, and DSBCAPS_FXIN2 will have no
            // WAVEFORMATEX structure by default.

            // TODO: A better response to this scenario if possible.

            pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX));

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

            //        pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)malloc(sizeof(WAVEFORMATEX)/*+pdsbd->lpwfxFormat->cbSize*/);

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
    if (pDSBufferDesc->lpwfxFormat != nullptr) {
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
    LPVOID                 &pLockPtr2,
    DWORD                   dwLockBytes2,
    DWORD                   dwLockFlags)
{
    // close any existing locks
    if (pLockPtr1 != xbnullptr) {
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

        pLockPtr1 = xbnullptr;
        pLockPtr2 = xbnullptr;
    }

}

#define DSoundBufferSelectionT(pThis) \
    ((pThis->EmuDirectSoundBuffer8Region != nullptr) ? pThis->EmuDirectSoundBuffer8Region : pThis->EmuDirectSoundBuffer8)

#define DSound3DBufferSelectionT(pThis) \
    ((pThis->EmuDirectSoundBuffer8Region != nullptr) ? pThis->EmuDirectSound3DBuffer8Region : pThis->EmuDirectSound3DBuffer8)

//Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
inline void DSoundBufferCreate(LPDSBUFFERDESC &pDSBufferDesc, LPDIRECTSOUNDBUFFER8 &pDSBuffer)
{
    LPDIRECTSOUNDBUFFER pTempBuffer;
    HRESULT hRetDS = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, NULL);
    if (hRetDS != DS_OK) {
        CxbxKrnlCleanup("CreateSoundBuffer Failed!");
        pDSBufferDesc = xbnullptr;
    } else {
        hRetDS = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&(pDSBuffer));
        pTempBuffer->Release();
        if (hRetDS != DS_OK) {
            CxbxKrnlCleanup("Create IDirectSoundBuffer8 Failed!");
        }
    }
}
inline void DSound3DBufferCreate(LPDIRECTSOUNDBUFFER8 pDSBuffer, LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer) {
    HRESULT hRetDS3D = pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&(pDS3DBuffer));
    if (hRetDS3D != DS_OK) {
        EmuWarning("CreateSound3DBuffer Failed!");
        pDS3DBuffer = nullptr;
    }
}

#define DSoundBufferSetDefault(pThis, pdsd, dwEmuFlags, dwEmuPlayFlags) \
    pThis->EmuDirectSoundBuffer8 = nullptr; \
    pThis->EmuDirectSound3DBuffer8 = nullptr; \
    pThis->EmuBuffer = xbnullptr; \
    pThis->EmuBufferDesc = pdsd; \
    pThis->EmuLockPtr1 = xbnullptr; \
    pThis->EmuLockBytes1 = 0; \
    pThis->EmuLockPtr2 = xbnullptr; \
    pThis->EmuLockBytes2 = 0; \
    pThis->EmuFlags = dwEmuFlags; \
    pThis->EmuPlayFlags = dwEmuPlayFlags;

inline void DSoundBufferRegionSetDefault(XTL::X_CDirectSoundBuffer *pThis) {
    pThis->EmuBufferToggle = XTL::X_DSB_TOGGLE_DEFAULT;
    pThis->EmuRegionLoopStartOffset = 0;
    pThis->EmuRegionLoopLength = 0;
    pThis->EmuRegionPlayStartOffset = 0;
    pThis->EmuRegionPlayLength = 0;
    pThis->EmuDirectSoundBuffer8Region = nullptr;
    pThis->EmuDirectSound3DBuffer8Region = nullptr;

}

inline void DSoundBufferRegionRelease(XTL::X_CDirectSoundBuffer *pThis)
{
    // NOTE: DSB Buffer8Region and 3DBuffer8Region are set
    // to nullptr inside DSoundBufferRegionSetDefault function.
    if (pThis->EmuDirectSoundBuffer8Region != nullptr) {
        pThis->EmuDirectSoundBuffer8Region->Release();

        if (pThis->EmuDirectSound3DBuffer8Region != nullptr) {
            pThis->EmuDirectSound3DBuffer8Region->Release();
        }
    }

    DSoundBufferRegionSetDefault(pThis);
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

    pDSBufferOld->GetVolume(&lVolume);
    pDSBufferOld->GetFrequency(&dwFrequency);
    pDSBufferOld->GetPan(&lPan);

    pDSBufferNew->SetPan(lPan);
    pDSBufferNew->SetFrequency(dwFrequency);
    pDSBufferNew->SetVolume(lVolume);

    if (pDS3DBufferOld != nullptr && pDS3DBufferNew != nullptr) {
        pDS3DBufferOld->GetAllParameters(&ds3dBuffer);

        pDS3DBufferNew->SetAllParameters(&ds3dBuffer, DS3D_IMMEDIATE);
    }
}

inline void DSoundBufferReplace(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    LPDSBUFFERDESC              pDSBufferDesc,
    DWORD                       PlayFlags,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer)
{
    DWORD refCount, dwPlayCursor, dwStatus;
    LPDIRECTSOUNDBUFFER8       pDSBufferNew = nullptr;
    LPDIRECTSOUND3DBUFFER8       pDS3DBufferNew = nullptr;

    DSoundBufferCreate(pDSBufferDesc, pDSBufferNew);

    if (pDS3DBuffer != nullptr) {
        DSound3DBufferCreate(pDSBufferNew, pDS3DBufferNew);
    }

    DSoundBufferTransferSettings(pDSBuffer, pDSBufferNew, pDS3DBuffer, pDS3DBufferNew);

    // NOTE: pDS3DBuffer will be set from almost the end of the function with pDS3DBufferNew.
    if (pDS3DBuffer != nullptr) {
        pDS3DBuffer->Release();
    }
    HRESULT hRet = pDSBuffer->GetStatus(&dwStatus);

    if (hRet != DS_OK) {
        CxbxKrnlCleanup("Unable to retrieve current status for replace DS buffer!");
    }

    pDSBuffer->Stop();

    hRet = pDSBuffer->GetCurrentPosition(&dwPlayCursor, nullptr);

    if (hRet != DS_OK) {
        CxbxKrnlCleanup("Unable to retrieve current position for replace DS buffer!");
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
    refCount = pDSBuffer->Release();
    if (refCount) {
        // NOTE: This is base on AddRef call, so this is a requirement.
        // The reason is to have ability "transfer" ref count is due to some titles doesn't care about ref count.
        // If AddRef was called, then it will "internally" increment the ref count, not the return value check.
        while (pDSBuffer->Release() > 0) {}
    }

    pDSBuffer = pDSBufferNew;
    pDS3DBuffer = pDS3DBufferNew;

    if (refCount) {
        while (pDSBuffer->AddRef() < refCount);
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

    pDSBufferDesc->dwBufferBytes = dwBytes;

    DSoundBufferReplace(pDSBuffer, pDSBufferDesc, PlayFlags, pDS3DBuffer);
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

                if (hRet == DS_OK) {
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

    if (hRet != DS_OK) {
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
    LPDSBUFFERDESC          pBufferDesc,
    DWORD                  &dwEmuFlags,
    DWORD                  &dwPlayFlags,
    LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer)
{

    enterCriticalSection;

    GeneratePCMFormat(pBufferDesc, pwfxFormat, dwEmuFlags);

    HRESULT hRet = DS_OK;
    if (g_pDSoundPrimaryBuffer == pDSBuffer) {
        hRet = pDSBuffer->SetFormat(pBufferDesc->lpwfxFormat);
    } else {
        DSoundBufferReplace(pDSBuffer, pBufferDesc, dwPlayFlags, pDS3DBuffer);
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
    if (lVolume <= -6400) {
        lVolume = DSBVOLUME_MIN;
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
