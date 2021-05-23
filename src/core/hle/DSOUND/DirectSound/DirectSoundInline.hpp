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
// *  (c) 2017 blueshogun96
// *  (c) 2017-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <mutex>
#include <optional>

#include "common/audio/XADPCM.h"
#include "core/hle/DSOUND/XbDSoundTypes.h"
#include "core/hle/DSOUND/common/windows/WFXformat.hpp"

#include <mmreg.h>

#define DSoundBufferGetPCMBufferSize(EmuFlags, size) (EmuFlags & DSE_FLAG_XADPCM) > 0 ? DWORD((size / float(XBOX_ADPCM_SRCSIZE)) * XBOX_ADPCM_DSTSIZE) : size
#define DSoundBufferGetXboxBufferSize(EmuFlags, size) (EmuFlags & DSE_FLAG_XADPCM) > 0 ? DWORD((size / float(XBOX_ADPCM_DSTSIZE)) * XBOX_ADPCM_SRCSIZE) : size

typedef struct IDirectSound3DBuffer8* LPDIRECTSOUND3DBUFFER8;
typedef LONGLONG REFERENCE_TIME;

// Memory managed xbox audio function handler
static inline DWORD DSoundSGEFreeBuffer() {
    int count =  (XTL_DS_SGE_SIZE_MAX - g_dwXbMemAllocated);
    count =  count / XTL_DS_SGE_PAGE_MAX;
    // Check for negative value, we don't need warning as we are allowing overflow one time only.
    if (count < 0) {
        return 0;
    }
    return (DWORD)count;
}
static inline void DSoundSGEMemAlloc(DWORD size) {
    g_dwXbMemAllocated += size;
}
static inline void DSoundSGEMemDealloc(DWORD size) {
    g_dwXbMemAllocated -= size;
}
static inline bool DSoundSGEMenAllocCheck(DWORD sizeRequest) {
    int leftOverSize = XTL_DS_SGE_SIZE_MAX - (g_dwXbMemAllocated + sizeRequest);
    // Don't let xbox title to alloc any more.
    if (leftOverSize < 0) {
        return false;
    }
    return true;
}

#if 0
// Debugging controlled audio section for audible only
static inline void DSoundDebugMuteFlag(DWORD Xb_bufferBytes, DWORD &EmuFlags) {
    if (Xb_bufferBytes != 0xEA0) {
        EmuFlags |= DSE_FLAG_DEBUG_MUTE;
    } else {
        EmuFlags ^= DSE_FLAG_DEBUG_MUTE;
    }
}
#else
#define DSoundDebugMuteFlag
#endif

static void DSoundBufferOutputXBtoHost(DWORD emuFlags, DSBUFFERDESC &DSBufferDesc, LPVOID pXBaudioPtr, DWORD dwXBAudioBytes, LPVOID pPCaudioPtr, DWORD dwPCMAudioBytes) {
    if ((emuFlags & DSE_FLAG_XADPCM) > 0) {

        TXboxAdpcmDecoder_Decode_Memory((uint8_t*)pXBaudioPtr, dwXBAudioBytes, (uint8_t*)pPCaudioPtr, DSBufferDesc.lpwfxFormat->nChannels);

    // PCM format, no changes requirement.
    } else {
        memcpy_s(pPCaudioPtr, dwPCMAudioBytes, pXBaudioPtr, dwPCMAudioBytes);
    }
}

static inline void GenerateXboxBufferCache(
    DSBUFFERDESC   &DSBufferDesc,
    DWORD          &dwEmuFlags,
    DWORD           X_BufferSizeRequest,
    LPVOID*         X_BufferCache,
    DWORD          &X_BufferCacheSize) {

    // Generate xbox buffer cache size
    // If the size is the same, don't realloc
    if (X_BufferCacheSize != X_BufferSizeRequest) {
        // Check if buffer cache exist, then copy over old ones.
        if (*X_BufferCache != xbox::zeroptr && (dwEmuFlags & DSE_FLAG_BUFFER_EXTERNAL) == 0) {
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

static inline void InitVoiceProperties(xbox::X_DSVOICEPROPS& Xb_VoiceProperties)
{
    Xb_VoiceProperties.l3DConeVolume = 0;
    Xb_VoiceProperties.l3DDistanceVolume = 0;
    Xb_VoiceProperties.l3DDopplerPitch = 0;
    Xb_VoiceProperties.lI3DL2DirectVolume = 0;
    Xb_VoiceProperties.lI3DL2RoomVolume = 0;
    Xb_VoiceProperties.lPitch = -4597;
}

static inline void GenerateMixBinDefault(
    xbox::X_DSVOICEPROPS    &Xb_VoiceProperties,
    ::LPCWAVEFORMATEX        lpwfxFormat,
    xbox::X_DSMIXBINBUNION   mixbins_output,
    bool                    is3D)
{
    std::unique_ptr<xbox::DSOUND::CMixBinConverter> convertedMixBins;

    if (g_LibVersion_DSOUND < 4039) {
        // Convert old style mixbin to a new style mixbin
        convertedMixBins = std::make_unique<xbox::DSOUND::CMixBinConverter>(mixbins_output.dwMixBinMask, nullptr);
        mixbins_output.pMixBins = convertedMixBins->GetMixBins();
    }

    auto& xb_mixbinArray = Xb_VoiceProperties.MixBinVolumePairs;
    unsigned int i;

    // Use custom mixbin if provided.
    if (mixbins_output.pMixBins != xbox::zeroptr) {

        Xb_VoiceProperties.dwMixBinCount = mixbins_output.pMixBins->dwCount;
        auto& mixbinArray_output = mixbins_output.pMixBins->lpMixBinVolumePairs;

        for (i = 0; i < Xb_VoiceProperties.dwMixBinCount; i++) {
            xb_mixbinArray[i].dwMixBin = mixbinArray_output[i].dwMixBin;
            xb_mixbinArray[i].lVolume = 0;
        }
    }
    // Otherwise use default setup as instructed by internally.
    else {
        unsigned int counter = 0;


        if (is3D) {

            xb_mixbinArray[counter].dwMixBin = 6;
            xb_mixbinArray[counter].lVolume = 0;
            counter++;

            xb_mixbinArray[counter].dwMixBin = 8;
            xb_mixbinArray[counter].lVolume = 0;
            counter++;

            xb_mixbinArray[counter].dwMixBin = 7;
            xb_mixbinArray[counter].lVolume = 0;
            counter++;

            xb_mixbinArray[counter].dwMixBin = 9;
            xb_mixbinArray[counter].lVolume = 0;
            counter++;

            xb_mixbinArray[counter].dwMixBin = 10;
            xb_mixbinArray[counter].lVolume = 0;
            counter++;
        }
        else {
            // If format is PCM/XADPCM, then use stereo mixbin as default.
            if (lpwfxFormat == xbox::zeroptr || lpwfxFormat->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
                counter = 2;
                for (i = 0; i < counter; i++) {
                    xb_mixbinArray[i].dwMixBin = i;
                    xb_mixbinArray[i].lVolume = 0;
                }
            }
            // Otherwise, the format is extensible and using channel mask as default.
            else {
                const WAVEFORMATEXTENSIBLE* wfxFormatExtensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(lpwfxFormat);
                auto& channelMask = wfxFormatExtensible->dwChannelMask;

                if ((channelMask & SPEAKER_FRONT_LEFT) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 0;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }

                if ((channelMask & SPEAKER_FRONT_RIGHT) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 1;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }

                if ((channelMask & SPEAKER_FRONT_CENTER) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 2;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }

                if ((channelMask & SPEAKER_LOW_FREQUENCY) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 3;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }

                if ((channelMask & SPEAKER_BACK_LEFT) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 4;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }

                if ((channelMask & SPEAKER_BACK_RIGHT) > 0) {
                    xb_mixbinArray[counter].dwMixBin = 5;
                    xb_mixbinArray[counter].lVolume = 0;
                    counter++;
                }
            }
        }

        Xb_VoiceProperties.dwMixBinCount = counter;
    }

    // Finally, set the rest as invalid/silent.
    for (int i = Xb_VoiceProperties.dwMixBinCount; i < 8; i++) {
        xb_mixbinArray[i].dwMixBin = 0xFFFFFFFF;
        xb_mixbinArray[i].lVolume = -10000;
    }
}

static inline void GeneratePCMFormat(
    DSBUFFERDESC   &DSBufferDesc,
    xbox::LPCWAVEFORMATEX Xb_lpwfxFormat,
    DWORD          &Xb_flags,
    DWORD          &dwEmuFlags,
    DWORD           X_BufferSizeRequest,
    LPVOID*         X_BufferCache,
    DWORD          &X_BufferCacheSize,
    xbox::X_DSVOICEPROPS& Xb_VoiceProperties,
    xbox::X_DSMIXBINBUNION mixbins_output,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    bool bIsSpecial = false;

    // convert from Xbox to PC DSound
    {
        DSBufferDesc.dwReserved = 0;

        // Allocate only once, does not need to re-allocate.
        if (DSBufferDesc.lpwfxFormat == nullptr) {
            // Only allocate extra value for setting extra values later on. WAVEFORMATEXTENSIBLE is the highest size I had seen.
            DSBufferDesc.lpwfxFormat = (WAVEFORMATEX*)calloc(1, sizeof(WAVEFORMATEXTENSIBLE));
        }

        if (DSBufferDesc.lpwfxFormat == nullptr) {
            CxbxKrnlCleanup("Unable to allocate DSBufferDesc.Xb_lpwfxFormat");
        }

        if (Xb_lpwfxFormat != xbox::zeroptr) {

            PWAVEFORMATEXTENSIBLE lpwfxFormatHost = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(DSBufferDesc.lpwfxFormat);

            if (Xb_lpwfxFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
                memcpy(lpwfxFormatHost, Xb_lpwfxFormat, sizeof(WAVEFORMATEX) + Xb_lpwfxFormat->cbSize);
            }
            else {
                // Other formats will enforce use WAVEFORMATEX size, we cannot allow corruption sneak into WAVEFORMATEXTENSIBLE's structure.
                memcpy(lpwfxFormatHost, Xb_lpwfxFormat, sizeof(WAVEFORMATEX));
                lpwfxFormatHost->Format.cbSize = 0;
            }

            dwEmuFlags = dwEmuFlags & ~DSE_FLAG_AUDIO_CODECS;

            CODEC_FORMAT cf_audio = WFXformat_SyncHostFormat(DSBufferDesc.lpwfxFormat, Xb_lpwfxFormat, X_BufferSizeRequest, Xb_flags, Xb_Voice);
            if (cf_audio == CF_PCM) {
                    dwEmuFlags |= DSE_FLAG_PCM;
            }
            else if (cf_audio == CF_XADPCM) {
                    dwEmuFlags |= DSE_FLAG_XADPCM;
            }
            else {
                    dwEmuFlags |= DSE_FLAG_PCM_UNKNOWN;
            }

        } else {
            dwEmuFlags |= DSE_FLAG_RECIEVEDATA;

            (void)WFXformat_SyncHostFormat(DSBufferDesc.lpwfxFormat, Xb_lpwfxFormat, X_BufferSizeRequest, Xb_flags, Xb_Voice);

            DSBufferDesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
            DSBufferDesc.dwBufferBytes = 5 * DSBufferDesc.lpwfxFormat->nAvgBytesPerSec;
        }

        DSBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
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

    GenerateMixBinDefault(Xb_VoiceProperties, DSBufferDesc.lpwfxFormat, mixbins_output, ((DSBufferDesc.dwFlags& DSBCAPS_CTRL3D) > 0));
}

static inline void DSoundGenericUnlock(
    DWORD                   dwEmuFlags,
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DSBUFFERDESC           &DSBufferDesc,
    xbox::DSoundBuffer_Lock &Host_lock,
    LPVOID                  X_BufferCache,
    DWORD                   X_Offset,
    DWORD                   X_dwLockBytes1,
    DWORD                   X_dwLockBytes2)
{
    // close any existing locks
    if (Host_lock.pLockPtr1 != nullptr) {


        if (X_BufferCache != xbox::zeroptr) {
            DSoundBufferOutputXBtoHost(dwEmuFlags, DSBufferDesc, ((PBYTE)X_BufferCache + X_Offset), X_dwLockBytes1, Host_lock.pLockPtr1, Host_lock.dwLockBytes1);

            if (Host_lock.pLockPtr2 != nullptr) {

                DSoundBufferOutputXBtoHost(dwEmuFlags, DSBufferDesc, X_BufferCache, X_dwLockBytes2, Host_lock.pLockPtr2, Host_lock.dwLockBytes2);
            }
        }

        HRESULT hRet = pDSBuffer->Unlock(Host_lock.pLockPtr1, Host_lock.dwLockBytes1, Host_lock.pLockPtr2, Host_lock.dwLockBytes2);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup("DirectSoundBuffer Unlock Failed!");
        }

        Host_lock.pLockPtr1 = nullptr;
        Host_lock.pLockPtr2 = nullptr;
    }

}

// Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
static inline HRESULT DSoundBufferCreate(LPDSBUFFERDESC pDSBufferDesc, LPDIRECTSOUNDBUFFER8 &pDSBuffer)
{
    LPDIRECTSOUNDBUFFER pTempBuffer;
	//Todo:shall we check the pDSBufferDesc->dwBufferBytes with legal ranges between DSBSIZE_MIN and DSBSIZE_MAX again?
    HRESULT hRetDS = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, nullptr);

	if (hRetDS == DS_OK) {
		hRetDS = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&(pDSBuffer));
		pTempBuffer->Release();
		if (pDSBuffer == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "CreateSoundBuffer:QueryInterface Failed!");
		}
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "CreateSoundBuffer Failed!");
	}
    return hRetDS;
}

static inline void DSound3DBufferCreate(LPDIRECTSOUNDBUFFER8 pDSBuffer, LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer) {
    HRESULT hRetDS3D = pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&(pDS3DBuffer));
    if (hRetDS3D != DS_OK) {
        EmuLog(LOG_LEVEL::WARNING, "CreateSound3DBuffer Failed!");
        pDS3DBuffer = nullptr;
    }
}

#define DSoundBufferSetDefault(pThis, dwEmuPlayFlags, Xb_dwFlags) \
    pThis->EmuDirectSoundBuffer8 = nullptr; \
    pThis->EmuDirectSound3DBuffer8 = nullptr; \
    pThis->X_BufferCache = xbox::zeroptr; \
    pThis->EmuFlags = 0; \
    pThis->EmuPlayFlags = dwEmuPlayFlags; \
    pThis->X_BufferCacheSize = 0; \
    pThis->Xb_rtPauseEx = 0LL; \
    pThis->Xb_VolumeMixbin = 0L; \
    pThis->Xb_EnvolopeDesc = { 0 }; \
    InitVoiceProperties(pThis->Xb_VoiceProperties); /* The rest will initialize in GeneratePCMFormat to GenerateMixBinDefault. */ \
    pThis->Xb_Flags = Xb_dwFlags;
    //pThis->EmuBufferDesc = { 0 }; // Enable this when become necessary.
    /*
    pThis->EmuLockPtr1 = xbox::zeroptr; \
    pThis->EmuLockBytes1 = 0; \
    pThis->EmuLockPtr2 = xbox::zeroptr; \
    pThis->EmuLockBytes2 = 0; \ */

static inline void DSoundBufferRegionSetDefault(xbox::EmuDirectSoundBuffer *pThis) {
    pThis->EmuBufferToggle = xbox::X_DSB_TOGGLE_DEFAULT;
    pThis->EmuRegionLoopStartOffset = 0;
    pThis->EmuRegionLoopLength = 0;
    pThis->EmuRegionPlayStartOffset = 0;
    pThis->EmuRegionPlayLength = 0;

}

static inline void DSoundBufferTransferSettings(
    LPDIRECTSOUNDBUFFER8       &pDSBufferOld,
    LPDIRECTSOUNDBUFFER8       &pDSBufferNew,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferOld,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferNew,
    xbox::CDirectSoundVoice*     Xb_Voice)
{
    LONG lVolume, lPan;
    DS3DBUFFER ds3dBuffer;

	if (pDSBufferOld == nullptr) {
		return;
	}

    // if sync current frequency used (then use pitch only).
    uint32_t freq = converter_pitch2freq(Xb_Voice->GetPitch());
    pDSBufferNew->SetFrequency(freq);

    pDSBufferOld->GetVolume(&lVolume);
    pDSBufferNew->SetVolume(lVolume);

    if (pDS3DBufferOld != nullptr && pDS3DBufferNew != nullptr) {
        pDS3DBufferOld->GetAllParameters(&ds3dBuffer);

        pDS3DBufferNew->SetAllParameters(&ds3dBuffer, DS3D_IMMEDIATE);
    } else {
        pDSBufferOld->GetPan(&lPan);
        pDSBufferNew->SetPan(lPan);
    }
}

static inline void DSoundBufferReCreate(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DSBUFFERDESC               &DSBufferDesc,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer,
    LPDIRECTSOUNDBUFFER8       &pDSBufferNew,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBufferNew,
    xbox::CDirectSoundVoice*     Xb_Voice) {


    HRESULT hretDS = DSoundBufferCreate(&DSBufferDesc, pDSBufferNew);
	//create new DS3DBuffer from the new DSBuffer if the new DSBuffer is created successfully.
    if (pDSBufferNew != nullptr) {
        DSound3DBufferCreate(pDSBufferNew, pDS3DBufferNew);
		DSoundBufferTransferSettings(pDSBuffer, pDSBufferNew, pDS3DBuffer, pDS3DBufferNew, Xb_Voice);//Sanity checks inside.
	}
	else {
		EmuLog(LOG_LEVEL::WARNING, "DSoundBufferReCreate Failed!");
	}

    
}

static inline void DSoundBufferRelease(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer,
    DWORD                      &refCount) {

    // Release 3D buffer first
    if (pDS3DBuffer != nullptr) {
        refCount = pDS3DBuffer->Release();
        if (refCount > 0) {
            CxbxKrnlCleanup("Nope, wasn't fully cleaned up.");
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

static inline void DSoundBufferResizeSetSize(
    xbox::XbHybridDSBuffer*      pHybridThis,
    HRESULT                    &hRet,
    DWORD                       Xb_dwByteLength) {

    xbox::EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    // General return OK, nothing needs to set as invalid for now.
    hRet = DS_OK;

    DWORD Host_dwByteLength = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, Xb_dwByteLength);

    // Don't re-create buffer if size is the same.
    if (pThis->EmuBufferDesc.dwBufferBytes == Host_dwByteLength) {
        return;
    }

	if (Host_dwByteLength< DSBSIZE_MIN) { //the min. buffer bytes must be equal or greater then DSBSIZE_MIN
		pThis->EmuBufferDesc.dwBufferBytes = DSBSIZE_MIN;
	}
	else {
		pThis->EmuBufferDesc.dwBufferBytes = Host_dwByteLength;
	}
    // NOTE: Test case JSRF, if we allow to re-alloc without checking allocated buffer size.
    // Then it is somehow binded to IDirectSound_SetPosition control for any allocated audio afterward.

    DWORD refCount;
    LPDIRECTSOUNDBUFFER8       pDSBufferNew = nullptr;
    LPDIRECTSOUND3DBUFFER8       pDS3DBufferNew = nullptr;

    DSoundBufferReCreate(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuDirectSound3DBuffer8,
                         pDSBufferNew, pDS3DBufferNew, pHybridThis->p_CDSVoice);

    // release old buffer
    DSoundBufferRelease(pThis->EmuDirectSoundBuffer8, pThis->EmuDirectSound3DBuffer8, refCount);

    pThis->EmuDirectSoundBuffer8 = pDSBufferNew;
    pThis->EmuDirectSound3DBuffer8 = pDS3DBufferNew;

    if (refCount) {
        while (pThis->EmuDirectSoundBuffer8->AddRef() < refCount);
    }
}

static inline void DSoundBufferResizeUpdate(
    xbox::XbHybridDSBuffer*      pHybridThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet,
    DWORD                       Xb_dwStartOffset,
    DWORD                       Xb_dwByteLength) {

    xbox::EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    DSoundBufferResizeSetSize(pHybridThis, hRet, Xb_dwByteLength); 

    hRet = pThis->EmuDirectSoundBuffer8->Lock(0, 0, &pThis->Host_lock.pLockPtr1, &pThis->Host_lock.dwLockBytes1,
                                              nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    if (hRet != DS_OK) {
        CxbxKrnlCleanup("Unable to lock region buffer!");
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

static inline void DSoundBufferRegionCurrentLocation(
    xbox::XbHybridDSBuffer*      pHybridThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet,
    DWORD                      &Xb_dwStartOffset,
    DWORD                      &Xb_dwByteLength)
{
    xbox::EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
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

static inline void DSoundBufferUpdate(
    xbox::XbHybridDSBuffer*      pHybridThis,
    DWORD                       dwPlayFlags,
    HRESULT                    &hRet) {

    // Process Play/Loop Region buffer
    DWORD Xb_dwByteLength;
    DWORD Xb_dwStartOffset;

    DSoundBufferRegionCurrentLocation(pHybridThis, dwPlayFlags, hRet, Xb_dwStartOffset, Xb_dwByteLength);

    DSoundBufferResizeUpdate(pHybridThis, dwPlayFlags, hRet, Xb_dwStartOffset, Xb_dwByteLength);
}

static inline void DSoundBufferRegenWithNewFormat(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DSBUFFERDESC               &DSBufferDesc,
    DWORD                       PlayFlags,
    LPDIRECTSOUND3DBUFFER8     &pDS3DBuffer,
    xbox::CDirectSoundVoice*   Xb_Voice)
{
    DWORD refCount, dwPlayCursor, dwStatus;
    LPDIRECTSOUNDBUFFER8       pDSBufferNew = nullptr;
    LPDIRECTSOUND3DBUFFER8       pDS3DBufferNew = nullptr;

    DSoundBufferReCreate(pDSBuffer, DSBufferDesc, pDS3DBuffer,
                         pDSBufferNew, pDS3DBufferNew, Xb_Voice);

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
    DSoundBufferRelease(pDSBuffer, pDS3DBuffer, refCount);

    pDSBuffer = pDSBufferNew;
    pDS3DBuffer = pDS3DBufferNew;

    if (refCount) {
        while (pDSBuffer->AddRef() < refCount);
    }
}

// Generic force remove synch playback control flag.
static inline void DSoundBufferSynchPlaybackFlagRemove(
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

static inline HRESULT DSoundBufferSynchPlaybackFlagAdd(
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

static inline HRESULT DSoundBufferUpdateHostVolume(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    uint32_t                dwEmuFlags,
    int32_t                 volume

    )
{

    if ((dwEmuFlags & DSE_FLAG_PCM) > 0) {
        if (!g_XBAudio.codec_pcm) {
            volume = DSBVOLUME_MIN;
        }
    } else if ((dwEmuFlags & DSE_FLAG_XADPCM) > 0) {
        if (!g_XBAudio.codec_xadpcm) {
            volume = DSBVOLUME_MIN;
        }
    } else if ((dwEmuFlags & DSE_FLAG_PCM_UNKNOWN) > 0) {
        if (!g_XBAudio.codec_unknown) {
            volume = DSBVOLUME_MIN;
        }
    }
    if (volume <= -6400 && volume != DSBVOLUME_MIN) {
        volume = DSBVOLUME_MIN;
    } else if (volume > 0) {
        EmuLog(LOG_LEVEL::WARNING, "volume has received greater than 0: %ld", volume);
        volume = 0;
    }
    if ((dwEmuFlags & DSE_FLAG_DEBUG_MUTE) > 0) {
        volume = DSBVOLUME_MIN;
    }

    return pDSBuffer->SetVolume(volume);
}

//TODO: RadWolfie - Need to implement DirectSoundBuffer create support. Or not able to do so due to all three classes function differently.
//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
/*static inline HRESULT HybridDirectSoundBuffer_Create(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
};*/

static inline ULONG HybridDirectSoundBuffer_AddRef(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{
    ULONG uRet = pDSBuffer->AddRef();

    return uRet;
}

static inline ULONG HybridDirectSoundBuffer_Release(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{
    ULONG uRet = pDSBuffer->Release();

    return uRet;
}

/*
//IDirectSoundStream
static inline HRESULT HybridDirectSoundBuffer_Discontinuity(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
static inline HRESULT HybridDirectSoundBuffer_Flush(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
static inline HRESULT HybridDirectSoundBuffer_FlushEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}

//IDirectSoundStream
static inline HRESULT HybridDirectSoundBuffer_GetInfo(
    LPDIRECTSOUNDBUFFER8 pDSBuffer) {

    return DS_OK;
}*/

// Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_GetCurrentPosition(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor,
    DWORD                   EmuFlags)
{
    DWORD dwCurrentPlayCursor, dwCurrentWriteCursor;
    HRESULT hRet = pDSBuffer->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor);

    if (hRet != DS_OK) {
        EmuLog(LOG_LEVEL::WARNING, "GetCurrentPosition Failed!");
    }

    if (pdwCurrentPlayCursor != xbox::zeroptr) {
        *pdwCurrentPlayCursor = DSoundBufferGetXboxBufferSize(EmuFlags, dwCurrentPlayCursor);
    }
    if (pdwCurrentWriteCursor != xbox::zeroptr) {
        *pdwCurrentWriteCursor = DSoundBufferGetXboxBufferSize(EmuFlags, dwCurrentWriteCursor);
    }

    RETURN_RESULT_CHECK(hRet);
}

/*
// NOTE: Flags are 100% not compatible, not use Hybrid version
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_GetStatus(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    PDWORD                  pdwStatus)
{
    HRESULT hRet = pDSBuffer->GetStatus(pdwStatus);

    RETURN_RESULT_CHECK(hRet);
}*/

/*
static inline HRESULT HybridDirectSoundBuffer_GetVoiceProperties(
    LPDIRECTSOUNDBUFFER8    pDSBuffer)
{

    return DS_OK;
}

//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Lock(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Pause(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    DWORD                   dwPause,
    DWORD                  &dwEmuFlags,
    DWORD                   dwEmuPlayFlags,
    bool                    triggerPlayPermission,
    REFERENCE_TIME          rtTimeStamp,
    REFERENCE_TIME         &Xb_rtTimeStamp)
{
    HRESULT hRet = DS_OK;
    switch (dwPause) {
        case X_DSSPAUSE_RESUME:
            if (triggerPlayPermission) {
                pDSBuffer->Play(0, 0, dwEmuPlayFlags);
            }
            DSoundBufferSynchPlaybackFlagRemove(dwEmuFlags);
            dwEmuFlags &= ~(DSE_FLAG_PAUSE | DSE_FLAG_PAUSENOACTIVATE);
            Xb_rtTimeStamp = 0;
            break;
        case X_DSSPAUSE_PAUSE:
            pDSBuffer->Stop();
            DSoundBufferSynchPlaybackFlagRemove(dwEmuFlags);
            if ((dwEmuFlags & DSE_FLAG_PAUSENOACTIVATE) == 0) {
                dwEmuFlags |= DSE_FLAG_PAUSE;
                Xb_rtTimeStamp = rtTimeStamp;
            }
            break;
        case X_DSSPAUSE_SYNCHPLAYBACK:
            //TODO: Test case Rayman 3 - Hoodlum Havoc, Battlestar Galactica, Miami Vice, Star Wars: KotOR, and... ?

            //SynchPlayback flag append should only occur in HybridDirectSoundBuffer_Pause function, nothing else is able to do this.
            hRet = DSoundBufferSynchPlaybackFlagAdd(dwEmuFlags);
            if (hRet == DS_OK) {
                pDSBuffer->Stop();
            }
            break;
        // NOTE: If stream is paused with packets, it will trigger to play. If it is not played, it act as a queue until trigger to play it.
        case X_DSSPAUSE_PAUSENOACTIVATE:
            dwEmuFlags &= ~DSE_FLAG_PAUSE;
            if ((dwEmuFlags & DSE_FLAG_IS_ACTIVATED) == 0) {
                dwEmuFlags |= DSE_FLAG_PAUSENOACTIVATE;
                Xb_rtTimeStamp = rtTimeStamp;
            }
            break;
    }

    RETURN_RESULT_CHECK(hRet);
}
/*
static inline HRESULT HybridDirectSoundBuffer_PauseEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}

//Not really require it.
//IDirectSoundStream
static inline HRESULT HybridDirectSoundBuffer_Process(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/
/* PlayEx is now calling to Play function instead of this hybrid function.
//Both Play(Ex) only has one function, aka this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Play(
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
            EmuLog(LOG_LEVEL::WARNING, "Rewinding buffer failed!");
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
static inline HRESULT HybridDirectSoundBuffer_PlayEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    REFERENCE_TIME      rtTimeStamp,
    DWORD               dwEmuFlags)
{

    return DS_OK;
}*/

/*
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Restore(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetAllParameters(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    xbox::X_DS3DBUFFER*      pDS3DBufferParams,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {

        ::DS3DBUFFER pDS3DBufferParamsTemp;
        pDS3DBufferParamsTemp.dwSize = sizeof(DS3DBUFFER);
        CopyD3DVector(pDS3DBufferParamsTemp.vPosition, pDS3DBufferParams->vPosition);
        CopyD3DVector(pDS3DBufferParamsTemp.vVelocity, pDS3DBufferParams->vVelocity);
        pDS3DBufferParamsTemp.dwInsideConeAngle = pDS3DBufferParams->dwInsideConeAngle;
        pDS3DBufferParamsTemp.dwOutsideConeAngle = pDS3DBufferParams->dwOutsideConeAngle;
        CopyD3DVector(pDS3DBufferParamsTemp.vConeOrientation, pDS3DBufferParams->vConeOrientation);
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

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetBufferData(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetConeAngles(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetConeOrientation(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{
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

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetConeOutsideVolume(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetConeOutsideVolume(lConeOutsideVolume, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetCurrentPosition(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwNewPosition)
{

    return DS_OK;
}*/

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DListener_SetDistanceFactor(
    LPDIRECTSOUND3DLISTENER8    pDS3DListener,
    FLOAT                       flDistanceFactor,
    DWORD                       dwApply)
{
    HRESULT hRet = pDS3DListener->SetDistanceFactor(flDistanceFactor, dwApply);

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DListener_SetDopplerFactor(
    LPDIRECTSOUND3DLISTENER8    pDS3DListener,
    FLOAT                       flDopplerFactor,
    DWORD                       dwApply)
{
    HRESULT hRet = pDS3DListener->SetDopplerFactor(flDopplerFactor, dwApply);

    RETURN_RESULT_CHECK(hRet);
}
/*
//TODO: PC DirectSound does not have SetEG method function.
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetEG(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LPVOID              pEnvelopeDesc)
{

    return DS_OK;
}

//TODO: PC DirectSound does not have SetFilter method function.
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetFilter(
    LPDIRECTSOUNDBUFFER8     pDSBuffer,
    xbox::X_DSFILTERDESC*    pFilterDesc)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetFormat(
    LPDIRECTSOUNDBUFFER8   &pDSBuffer,
    xbox::LPCWAVEFORMATEX   Xb_pwfxFormat,
    DWORD                   Xb_flags,
    DSBUFFERDESC           &BufferDesc,
    DWORD                  &dwEmuFlags,
    DWORD                  &dwPlayFlags,
    LPDIRECTSOUND3DBUFFER8 &pDS3DBuffer,
    bool                    X_BufferAllocate,
    LPVOID                 &X_BufferCache,
    DWORD                  &X_BufferCacheSize,
    xbox::X_DSVOICEPROPS    &Xb_VoiceProperties,
    xbox::X_LPDSMIXBINS      mixbins_output,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    pDSBuffer->Stop();

    xbox::X_DSMIXBINBUNION mixBins;
    mixBins.pMixBins = mixbins_output;

    if (X_BufferAllocate) {
        GeneratePCMFormat(BufferDesc, Xb_pwfxFormat, Xb_flags, dwEmuFlags, X_BufferCacheSize,
                           xbox::zeroptr, X_BufferCacheSize, Xb_VoiceProperties, mixBins, Xb_Voice);
    // Don't allocate for DS Stream class, it is using straight from the source.
    } else {
        GeneratePCMFormat(BufferDesc, Xb_pwfxFormat, Xb_flags, dwEmuFlags, 0,
                           xbox::zeroptr, X_BufferCacheSize, Xb_VoiceProperties, mixBins, Xb_Voice);
    }
    HRESULT hRet = DS_OK;
    if ((void*)g_pDSoundPrimaryBuffer == (void*)pDSBuffer) {
        hRet = pDSBuffer->SetFormat(BufferDesc.lpwfxFormat);
    } else {
        // DSound Stream only
        if (X_BufferCacheSize == 0) {
            // Allocate at least 5 second worth of bytes in PCM format.
            BufferDesc.dwBufferBytes = BufferDesc.lpwfxFormat->nAvgBytesPerSec * 5;
        }
        DSoundBufferRegenWithNewFormat(pDSBuffer, BufferDesc, dwPlayFlags, pDS3DBuffer, Xb_Voice);
    }

    RETURN_RESULT_CHECK(hRet);
}

static HRESULT HybridDirectSoundBuffer_SetPitch(LPDIRECTSOUNDBUFFER8, LONG, xbox::CDirectSoundVoice*);

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetFrequency(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwFrequency,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    HRESULT hRet = S_OK;

    int32_t pitch = converter_freq2pitch((dwFrequency!=0 ? dwFrequency : Xb_Voice->GetFrequencyDefault()));

    hRet = HybridDirectSoundBuffer_SetPitch(pDSBuffer, pitch, Xb_Voice);

    RETURN_RESULT_CHECK(hRet);
}

static HRESULT HybridDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER8, LONG, DWORD, LONG, xbox::CDirectSoundVoice*);

//IDirectSoundStream
//IDirectSoundBuffer
// NOTE: 0 to 10,000; For initialize value would be 3D default to 0 and 2D default to 600.
static inline HRESULT HybridDirectSoundBuffer_SetHeadroom(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwHeadroom,
    LONG                Xb_volumeMixbin,
    DWORD               dwEmuFlags,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    HRESULT hRet;
    if (dwHeadroom > 10000) {
        hRet = DSERR_INVALIDPARAM;
    } else {
        hRet = DS_OK;
        Xb_Voice->SetHeadroom(dwHeadroom);
        int32_t volume = Xb_Voice->GetVolume();
        hRet = DSoundBufferUpdateHostVolume(pDSBuffer, dwEmuFlags, volume);
    }

    return DS_OK;
}

/*
//TODO: PC DirectSound does not have SetI3DL2Source method function.
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetI3DL2Source(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    PVOID               pds3db,
    DWORD               dwApply)
{

    return DS_OK;
}

//TODO: PC DirectSound does not have SetLFO method function.
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetLFO(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LPCDSLFODESC        pLFODesc)
{

    return DS_OK;
}*/

/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetLoopRegion(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwLoopStart,
    DWORD               dwLoopLength)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetMaxDistance(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                flMaxDistance,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMaxDistance(flMaxDistance, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetMinDistance(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                flMinDistance,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMinDistance(flMinDistance, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetMixBins(
    xbox::X_DSVOICEPROPS&   Xb_VoiceProperties,
    xbox::X_DSMIXBINBUNION  mixBins,
    DSBUFFERDESC&           BufferDesc
)
{
    HRESULT ret = DS_OK;

    GenerateMixBinDefault(Xb_VoiceProperties, BufferDesc.lpwfxFormat, mixBins, ((BufferDesc.dwFlags & DSBCAPS_CTRL3D) > 0));

    return ret;
}

//IDirectSoundStream x2
//IDirectSoundBuffer x2
static inline HRESULT HybridDirectSoundBuffer_SetMixBinVolumes_8(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    xbox::X_LPDSMIXBINS pMixBins,
    xbox::X_DSVOICEPROPS& Xb_VoiceProperties,
    DWORD                EmuFlags,
    LONG                &Xb_volumeMixBin,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    HRESULT hRet = DSERR_INVALIDPARAM;

    if (pMixBins != xbox::zeroptr && pMixBins->lpMixBinVolumePairs != xbox::zeroptr) {
        // Update the mixbin volume only, do not reassign volume pair array.
        for (DWORD i = 0; i < pMixBins->dwCount; i++) {
            const auto& it_in = pMixBins->lpMixBinVolumePairs[i];
            auto it_out = std::find_if(Xb_VoiceProperties.MixBinVolumePairs, Xb_VoiceProperties.MixBinVolumePairs + Xb_VoiceProperties.dwMixBinCount,
                [&it_in](const auto& e) {
                    return e.dwMixBin == it_in.dwMixBin;
                });
            // Once found a match, set the volume.
            // NOTE If titles input duplicate with different volume,
            //      it will override previous value.
            if (it_out != Xb_VoiceProperties.MixBinVolumePairs + Xb_VoiceProperties.dwMixBinCount) {
                it_out->lVolume = it_in.lVolume;
            }
        }

        // Since we cannot set per-channel volumes, we want to pick "dominant" volume
        LONG maxVolume = DSBVOLUME_MIN;
        for(unsigned i = 0; i < Xb_VoiceProperties.dwMixBinCount; i++) {
            const auto& it_out = Xb_VoiceProperties.MixBinVolumePairs[i];
            if (it_out.dwMixBin != XDSMIXBIN_LOW_FREQUENCY && it_out.dwMixBin < XDSMIXBIN_SPEAKERS_MAX) {
                if (maxVolume < it_out.lVolume) {
                    maxVolume = it_out.lVolume;
                }
            }
        }

        Xb_volumeMixBin = maxVolume;
        int32_t Xb_volume = Xb_Voice->GetVolume() + Xb_Voice->GetHeadroom();
        hRet = HybridDirectSoundBuffer_SetVolume(pDSBuffer, Xb_volume, EmuFlags,
                                                    Xb_volumeMixBin, Xb_Voice);
    }

    return hRet;
}


//IDirectSoundStream x2
//IDirectSoundBuffer x2
static inline HRESULT HybridDirectSoundBuffer_SetMixBinVolumes_12(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    xbox::dword_xt       dwMixBinMask,
    const xbox::long_xt* alVolumes,
    xbox::X_DSVOICEPROPS& Xb_VoiceProperties,
    DWORD                EmuFlags,
    LONG                &Xb_volumeMixBin,
    xbox::CDirectSoundVoice* Xb_Voice)
{
    // Convert volumes/mask to xbox::X_DSMIXBINS and call the newer version of the function to keep implementations consistent
    xbox::DSOUND::CMixBinConverter convertedMixBins(dwMixBinMask, alVolumes);
    return HybridDirectSoundBuffer_SetMixBinVolumes_8(pDSBuffer, convertedMixBins.GetMixBins(), Xb_VoiceProperties, EmuFlags, Xb_volumeMixBin, Xb_Voice);
}

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetMode(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    DWORD                   dwMode,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetMode(dwMode, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetNotificationPositions(
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
static inline HRESULT HybridDirectSoundBuffer_SetOutputBuffer(
    LPDIRECTSOUNDBUFFER8     pDSBuffer,
    X_CDirectSoundBuffer*   pOutputBuffer)
{

    return DS_OK;
}
*/
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetPitch(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    LONG                lPitch,
    xbox::CDirectSoundVoice* Xb_Voice)
{

    Xb_Voice->SetPitch(lPitch);
    // Convert pitch back to frequency
    uint32_t setFrequency = converter_pitch2freq(lPitch);

    RETURN_RESULT_CHECK(pDSBuffer->SetFrequency(setFrequency));
}
/*
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetPlayRegion(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    DWORD               dwPlayStart,
    DWORD               dwPlayLength)
{

    return DS_OK;
}*/

//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetPosition(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetPosition(x, y, z, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}
/*
//TODO: PC DirectSound does not have SetRolloffCurve method function.
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_SetRolloffCurve(
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
static inline HRESULT HybridDirectSound3DListener_SetRolloffFactor(
    LPDIRECTSOUND3DLISTENER8    pDSBuffer,
    FLOAT                       fRolloffFactor,
    DWORD                       dwApply)
{
    HRESULT hRet = pDSBuffer->SetRolloffFactor(fRolloffFactor, dwApply);

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSound
//IDirectSoundStream
//IDirectSoundBuffer
static inline HRESULT HybridDirectSound3DBuffer_SetVelocity(
    LPDIRECTSOUND3DBUFFER8  pDS3DBuffer,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    HRESULT hRet = DS_OK;
    if (pDS3DBuffer != nullptr) {
        hRet = pDS3DBuffer->SetVelocity(x, y, z, dwApply);
    }

    RETURN_RESULT_CHECK(hRet);
}

//IDirectSoundStream x2
//IDirectSoundBuffer
// NOTE: real volume = mixbins volume + 3D volume + volume - headroom
// 100 millibels (mB) = 1 dB
static inline HRESULT HybridDirectSoundBuffer_SetVolume(
    LPDIRECTSOUNDBUFFER8    pDSBuffer,
    LONG                    lVolume,
    DWORD                   dwEmuFlags,
    LONG                    Xb_volumeMixbin,
    xbox::CDirectSoundVoice* Xb_Voice)
{

#if 0 // TODO: Restore it once DSound work update comes up
    // For time being, this log is kept in case of something changed somewhere making a wrong input into the API.
    printf("DEBUG: SetVolume | lVolume = %ld | volumeMixbin = %ld | dwHeadroom = %8u\n", lVolume, Xb_volumeMixbin, Xb_dwHeadroom);
#endif

    Xb_Voice->SetVolume(lVolume);
    lVolume = Xb_Voice->GetVolume();
    lVolume += Xb_volumeMixbin;

    HRESULT hRet = DSoundBufferUpdateHostVolume(pDSBuffer, dwEmuFlags, lVolume);

    RETURN_RESULT_CHECK(hRet);
}
/*/
//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Stop(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}

//Only has one function, this is not a requirement.
//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_StopEx(
    LPDIRECTSOUNDBUFFER8 pDSBuffer,
    REFERENCE_TIME      rtTimeStamp,
    DWORD               dwEmuFlags)
{

    return DS_OK;
}

//IDirectSoundBuffer
static inline HRESULT HybridDirectSoundBuffer_Unlock(
    LPDIRECTSOUNDBUFFER8 pDSBuffer)
{

    return DS_OK;
}//*/

static inline HRESULT HybridDirectSoundBuffer_GetVoiceProperties(
    xbox::X_DSVOICEPROPS& Xb_VoiceProperties,
    xbox::X_DSVOICEPROPS* out_VoiceProperties
)
{
    HRESULT ret = DS_OK;

    if (out_VoiceProperties != xbox::zeroptr) {
        // Simply copy the data from Xb_VoiceProperties.
        *out_VoiceProperties = Xb_VoiceProperties;
    }
    else {
        ret = DSERR_INVALIDPARAM;
    }

    return ret;
}
