// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#define LOG_PREFIX CXBXR_MODULE::DSBUFFER

// prevent name collisions
namespace xboxkrnl {
    #include <xboxkrnl/xboxkrnl.h>
};

#include <dsound.h>
#include "DirectSoundGlobal.hpp" // Global variables

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include "..\XbDSoundLogging.hpp"



// TODO: Tasks need to do for DirectSound HLE
// * Missing IDirectSoundBuffer patch
//   * IDirectSoundBuffer_Set3DVoiceData (new, undocument)
//   * IDirectSoundBuffer_Use3DVoiceData (new, undocument)
//   * IDirectSoundBuffer_QueryInterface (not require)
//   * IDirectSoundBuffer_QueryInterfaceC (not require)

/* NOTE: SUCCEEDED define is only checking for is equal or greater than zero value.
    And FAILED check for less than zero value. Since DS_OK is only 0 base on DirectSound documentation,
    there is chance of failure which contain value greater than 0.
 */

#include "DirectSoundInline.hpp"

// ******************************************************************
// * patch: DirectSoundDoWork (buffer)
// ******************************************************************
void DirectSoundDoWork_Buffer(xboxkrnl::LARGE_INTEGER &time)
{

    vector_ds_buffer::iterator ppDSBuffer = g_pDSoundBufferCache.begin();
    for (; ppDSBuffer != g_pDSoundBufferCache.end(); ppDSBuffer++) {
        XTL::EmuDirectSoundBuffer* pThis = ((*ppDSBuffer)->emuDSBuffer);
        if (pThis->Host_lock.pLockPtr1 == nullptr || pThis->EmuBufferToggle != XTL::X_DSB_TOGGLE_DEFAULT) {
            continue;
        }
        // However there's a chance of locked buffers has been set which needs to be unlock.
        DSoundGenericUnlock(pThis->EmuFlags,
                            pThis->EmuDirectSoundBuffer8,
                            pThis->EmuBufferDesc,
                            pThis->Host_lock,
                            pThis->X_BufferCache,
                            pThis->X_lock.dwLockOffset,
                            pThis->X_lock.dwLockBytes1,
                            pThis->X_lock.dwLockBytes2);

        // TODO: Do we need this in async thread loop?
        if (pThis->Xb_rtPauseEx != 0LL && pThis->Xb_rtPauseEx <= time.QuadPart) {
            pThis->Xb_rtPauseEx = 0LL;
            pThis->EmuFlags &= ~DSE_FLAG_PAUSE;
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }

        if (pThis->Xb_rtStopEx != 0LL && pThis->Xb_rtStopEx <= time.QuadPart) {
            pThis->Xb_rtStopEx = 0LL;
            pThis->EmuDirectSoundBuffer8->Stop();
        }
    }
}

// ******************************************************************
// * patch: IDirectSoundBuffer_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer_AddRef)
(
    XbHybridDSBuffer*       pHybridThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pHybridThis);
    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;

    ULONG uRet = HybridDirectSoundBuffer_AddRef(pThis->EmuDirectSoundBuffer8);

    return uRet;
}

// ******************************************************************
// * EmuDirectSoundBuffer destructor handler
// ******************************************************************
XTL::EmuDirectSoundBuffer::~EmuDirectSoundBuffer()
{
    if (this->EmuDirectSound3DBuffer8 != nullptr) {
        this->EmuDirectSound3DBuffer8->Release();
    }

    // remove cache entry
    vector_ds_buffer::iterator ppDSBuffer = std::find(g_pDSoundBufferCache.begin(), g_pDSoundBufferCache.end(), this->pHybridThis);
    if (ppDSBuffer != g_pDSoundBufferCache.end()) {
        g_pDSoundBufferCache.erase(ppDSBuffer);
    }

    if (this->EmuBufferDesc.lpwfxFormat != nullptr) {
        free(this->EmuBufferDesc.lpwfxFormat);
    }
    if (this->X_BufferCache != xbnullptr && (this->EmuFlags & DSE_FLAG_BUFFER_EXTERNAL) == 0) {
        free(this->X_BufferCache);
        DSoundSGEMemDealloc(this->X_BufferCacheSize);
    }
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Release)
(
    XbHybridDSBuffer*       pHybridThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pHybridThis);

    ULONG uRet = 0;
    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;

    //if (!(pThis->EmuFlags & DSE_FLAG_RECIEVEDATA)) {
        uRet = pThis->EmuDirectSoundBuffer8->Release();

        if (uRet == 0) {
            size_t size = sizeof(SharedDSBuffer) - sizeof(XbHybridDSBuffer);
            SharedDSBuffer* pSharedThis = reinterpret_cast<SharedDSBuffer*>(reinterpret_cast<uint8_t*>(pHybridThis) - size);
            delete pSharedThis;
        }
    //}

    RETURN(uRet);
}

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*         pdsbd,
    OUT XbHybridDSBuffer**  ppBuffer)
{
    DSoundMutexGuardLock;

    // Research reveal DirectSound creation check is part of the requirement.
    if (!g_pDSound8 && !g_bDSoundCreateCalled) {
        HRESULT hRet;

        hRet = XTL::EMUPATCH(DirectSoundCreate)(nullptr, &g_pDSound8, nullptr);
        if (hRet != DS_OK) {
            CxbxKrnlCleanup("Unable to initialize DirectSound!");
        }
    }

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdsbd)
		LOG_FUNC_ARG_OUT(ppBuffer)
		LOG_FUNC_END;

    HRESULT hRet = DS_OK;

    //If out of space, return out of memory.
    if (X_DIRECTSOUND_CACHE_COUNT == X_DIRECTSOUND_CACHE_MAX || !DSoundSGEMenAllocCheck(pdsbd->dwBufferBytes)) {

        hRet = DSERR_OUTOFMEMORY;
        *ppBuffer = xbnullptr;
    } else {

        DSBUFFERDESC DSBufferDesc = { 0 };

        //TODO: Find out the cause for DSBCAPS_MUTE3DATMAXDISTANCE to have invalid arg.
        DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00020000 | 0x00040000 /*| 0x00080000*/;

        if (pdsbd->dwFlags & (~dwAcceptableMask)) {
            EmuLog(LOG_LEVEL::WARNING, "Use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)", pdsbd->dwFlags & (~dwAcceptableMask));
        }

        DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        DSBufferDesc.dwFlags = (pdsbd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY |
            (g_XBAudio.mute_on_unfocus ? 0 : DSBCAPS_GLOBALFOCUS);

        // HACK: Hot fix for titles not giving CTRL3D flag. Xbox might accept it, however the host does not.
        if ((pdsbd->dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE) > 0 && (pdsbd->dwFlags & DSBCAPS_CTRL3D) == 0) {
            DSBufferDesc.dwFlags &= ~DSBCAPS_MUTE3DATMAXDISTANCE;
        }

        // TODO: Garbage Collection
        SharedDSBuffer* pBuffer = new SharedDSBuffer((DSBufferDesc.dwFlags & DSBCAPS_CTRL3D) != 0);
        XbHybridDSBuffer* pHybridBuffer = reinterpret_cast<XbHybridDSBuffer*>(&pBuffer->dsb_i);
        *ppBuffer = pHybridBuffer;
        EmuDirectSoundBuffer* pEmuBuffer = pBuffer->emuDSBuffer;
        pEmuBuffer->pHybridThis = pHybridBuffer;

        DSoundBufferSetDefault(pEmuBuffer, 0, pdsbd->dwFlags);
        pEmuBuffer->Host_lock = { 0 };
        pEmuBuffer->Xb_rtStopEx = 0LL;

        DSoundBufferRegionSetDefault(pEmuBuffer);

        // We have to set DSBufferDesc last due to EmuFlags must be either 0 or previously written value to preserve other flags.
        GeneratePCMFormat(DSBufferDesc, pdsbd->lpwfxFormat, pdsbd->dwFlags, pEmuBuffer->EmuFlags, pdsbd->dwBufferBytes,
                          &pEmuBuffer->X_BufferCache, pEmuBuffer->X_BufferCacheSize, pEmuBuffer->Xb_VoiceProperties, pdsbd->lpMixBinsOutput,
                          pHybridBuffer->p_CDSVoice);
        pEmuBuffer->EmuBufferDesc = DSBufferDesc;

        EmuLog(LOG_LEVEL::DEBUG, "DirectSoundCreateBuffer, *ppBuffer := 0x%08X, bytes := 0x%08X", *ppBuffer, pEmuBuffer->EmuBufferDesc.dwBufferBytes);

        hRet = DSoundBufferCreate(&DSBufferDesc, pEmuBuffer->EmuDirectSoundBuffer8);
        if (FAILED(hRet)) {
            std::stringstream output;
            output << "Xbox:\n" << pdsbd;
            output << "\nHost converison:\n" << DSBufferDesc;
            EmuLog(LOG_LEVEL::WARNING, output.str().c_str());
            output.str("");
            output << static_cast<DS_RESULT>(hRet);
            CxbxKrnlCleanup("DSB: DSoundBufferCreate error: %s", output.str().c_str());
        }
        else {
            if (pdsbd->dwFlags & DSBCAPS_CTRL3D) {
                DSound3DBufferCreate(pEmuBuffer->EmuDirectSoundBuffer8, pEmuBuffer->EmuDirectSound3DBuffer8);
            }

            DSoundDebugMuteFlag(pEmuBuffer->X_BufferCacheSize, pEmuBuffer->EmuFlags);

            // Pre-set volume to enforce silence if one of audio codec is disabled.
            HybridDirectSoundBuffer_SetVolume(pEmuBuffer->EmuDirectSoundBuffer8, 0L, pEmuBuffer->EmuFlags,
                pEmuBuffer->Xb_VolumeMixbin, pHybridBuffer->p_CDSVoice);

            g_pDSoundBufferCache.push_back(pHybridBuffer);
        }
    }

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC*         pdsbd,
    OUT XbHybridDSBuffer**  ppBuffer,
    LPUNKNOWN               pUnkOuter)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("DirectSoundCreateBuffer");

    HRESULT hRet = EMUPATCH(DirectSoundCreateBuffer)(pdsbd, ppBuffer);

    return hRet;
}

/* ------------- Sorted relative functions begin ------------------*/

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    XbHybridDSBuffer*           pHybridThis,
    OUT PDWORD                  pdwCurrentPlayCursor,
    OUT PDWORD                  pdwCurrentWriteCursor)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG_OUT(pdwCurrentPlayCursor)
		LOG_FUNC_ARG_OUT(pdwCurrentWriteCursor)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_GetCurrentPosition(pThis->EmuDirectSoundBuffer8, pdwCurrentPlayCursor, pdwCurrentWriteCursor, pThis->EmuFlags);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetCurrentPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwNewPosition)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwNewPosition)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    dwNewPosition = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, dwNewPosition);

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if (hRet != DS_OK) {
        EmuLog(LOG_LEVEL::WARNING, "SetCurrentPosition Failed!");
    }

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    XbHybridDSBuffer*       pHybridThis,
    OUT LPDWORD             pdwStatus)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    DWORD dwStatusXbox = 0, dwStatusHost;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatusHost);

    // Conversion is a requirement to xbox.
    if (hRet == DS_OK) {
        if ((pThis->EmuFlags & DSE_FLAG_PAUSE) > 0) {
            dwStatusXbox = X_DSBSTATUS_PAUSED;
        } else if ((dwStatusHost & DSBSTATUS_PLAYING) > 0) {
            dwStatusXbox = X_DSBSTATUS_PLAYING;
        }
        if ((dwStatusHost & DSBSTATUS_LOOPING) > 0) {
            dwStatusXbox |= X_DSBSTATUS_LOOPING;
        }
    }

    if (pdwStatus != xbnullptr) {
        *pdwStatus = dwStatusXbox;
    } else {
        hRet = DSERR_INVALIDPARAM;
    }

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetVoiceProperties)
(
    XbHybridDSBuffer*       pHybridThis,
    OUT X_DSVOICEPROPS*     pVoiceProps)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG_OUT(pVoiceProps)
        LOG_FUNC_END;

    if (pVoiceProps == xbnullptr) {
        LOG_TEST_CASE("pVoiceProps == xbnullptr");
        RETURN(DS_OK);
    }

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_GetVoiceProperties(pThis->Xb_VoiceProperties, pVoiceProps);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Lock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Lock)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwOffset,
    DWORD                   dwBytes,
    LPVOID*                 ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID*                 ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    DWORD                   dwFlags)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwOffset)
		LOG_FUNC_ARG(dwBytes)
		LOG_FUNC_ARG(ppvAudioPtr1)
		LOG_FUNC_ARG(pdwAudioBytes1)
		LOG_FUNC_ARG(ppvAudioPtr2)
		LOG_FUNC_ARG(pdwAudioBytes2)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = D3D_OK;
    DWORD pcmSize = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, dwBytes);
    DWORD pcmOffset = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, dwOffset);

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    if (ppvAudioPtr2 == xbnullptr) {
        hRet = pThis->EmuDirectSoundBuffer8->Lock(pcmOffset, pcmSize, &pThis->Host_lock.pLockPtr1, &pThis->Host_lock.dwLockBytes1,
                                                  nullptr, 0, dwFlags);
        pThis->Host_lock.pLockPtr2 = nullptr;
    } else {
        hRet = pThis->EmuDirectSoundBuffer8->Lock(pcmOffset, pcmSize, &pThis->Host_lock.pLockPtr1, &pThis->Host_lock.dwLockBytes1,
                                                  &pThis->Host_lock.pLockPtr2, &pThis->Host_lock.dwLockBytes2, dwFlags);
    }

    if (hRet != DS_OK) {
        CxbxKrnlCleanup("DirectSoundBuffer Lock Failed!");
    }

    // Host lock position
    pThis->Host_lock.dwLockOffset = pcmOffset;
    pThis->Host_lock.dwLockFlags = dwFlags;
    pThis->X_lock.dwLockFlags = dwFlags;

    // Emulate to xbox's lock position
    pThis->X_lock.dwLockOffset = dwOffset;
    *ppvAudioPtr1 = pThis->X_lock.pLockPtr1 = ((LPBYTE)pThis->X_BufferCache + dwOffset);
    *pdwAudioBytes1 = pThis->X_lock.dwLockBytes1 = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, pThis->Host_lock.dwLockBytes1);
    if (pThis->Host_lock.pLockPtr2 != nullptr) {
        *ppvAudioPtr2 = pThis->X_lock.pLockPtr2 = pThis->X_BufferCache;
        *pdwAudioBytes2 = pThis->X_lock.dwLockBytes2 = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, pThis->Host_lock.dwLockBytes2);
    } else {
        // If secondary pointers are not used, then set them as zero.
        // There are applications bug didn't check for audio pointer that is null pointer which should not use invalid audio bytes.
        // Since internal functions do set them zero. We'll set them here as well.
        if (ppvAudioPtr2 != xbnullptr) {
            *ppvAudioPtr2 = xbnullptr;
        }
        if (pdwAudioBytes2 != xbnullptr) {
            *pdwAudioBytes2 = 0;
        }
    }

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Unlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Unlock)
(
    XbHybridDSBuffer*       pHybridThis,
    LPVOID                  ppvAudioPtr1,
    DWORD                   pdwAudioBytes1,
    LPVOID                  ppvAudioPtr2,
    DWORD                   pdwAudioBytes2
    )
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG(ppvAudioPtr1)
        LOG_FUNC_ARG(pdwAudioBytes1)
        LOG_FUNC_ARG(ppvAudioPtr2)
        LOG_FUNC_ARG(pdwAudioBytes2)
        LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    // TODO: Find out why pThis->EmuLockPtr1 is nullptr... (workaround atm is to check if it is not a nullptr.)
    if (pThis->X_BufferCache != xbnullptr && pThis->Host_lock.pLockPtr1 != nullptr) {

        memcpy_s((PBYTE)pThis->X_BufferCache + pThis->X_lock.dwLockOffset,
                 pThis->X_BufferCacheSize - pThis->X_lock.dwLockOffset,
                 pThis->X_lock.pLockPtr1,
                 pThis->X_lock.dwLockBytes1);

        if (pThis->Host_lock.pLockPtr2 != nullptr) {
            memcpy_s(pThis->X_BufferCache, pThis->X_BufferCacheSize, pThis->X_lock.pLockPtr2, pThis->X_lock.dwLockBytes2);
        }
    }

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Pause
// ******************************************************************
// Introduced in XDK 4721 revision.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Pause)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwPause)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwPause)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    HRESULT hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
                                                 1, 0LL, pThis->Xb_rtPauseEx);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
// Introduced in XDK 4721 revision.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_PauseEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(rtTimestamp)
		LOG_FUNC_ARG(dwPause)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
                                                 1, rtTimestamp, pThis->Xb_rtPauseEx);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Play
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Play)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwReserved1)
		LOG_FUNC_ARG(dwReserved2)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    if (dwFlags & ~(X_DSBPLAY_LOOPING | X_DSBPLAY_FROMSTART | X_DSBPLAY_SYNCHPLAYBACK)) {
        CxbxKrnlCleanup("Unsupported Playing Flags");
    }
    pThis->EmuPlayFlags = dwFlags;

    // rewind buffer
    if ((dwFlags & X_DSBPLAY_FROMSTART)) {

        pThis->EmuPlayFlags &= ~X_DSBPLAY_FROMSTART;
    }

    HRESULT hRet = DS_OK;

    if ((dwFlags & X_DSBPLAY_SYNCHPLAYBACK) > 0) {
        //SynchPlayback flag append should only occur in HybridDirectSoundBuffer_Pause function, nothing else is able to do this.
        hRet = DSoundBufferSynchPlaybackFlagAdd(pThis->EmuFlags);
        pThis->EmuPlayFlags ^= X_DSBPLAY_SYNCHPLAYBACK;
    }

    DSoundBufferUpdate(pHybridThis, dwFlags, hRet);

    if (hRet == DS_OK) {
        if (dwFlags & X_DSBPLAY_FROMSTART) {
            if (pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0) != DS_OK) {
                EmuLog(LOG_LEVEL::WARNING, "Rewinding buffer failed!");
            }
        }
        if ((pThis->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) == 0) {
            hRet = pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    XbHybridDSBuffer*   pThis,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(rtTimeStamp)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    //TODO: Need implement support for rtTimeStamp.
    if (rtTimeStamp != 0) {
        EmuLog(LOG_LEVEL::WARNING, "Not implemented for rtTimeStamp greater than 0 of %08d", rtTimeStamp);
    }

    HRESULT hRet = XTL::EMUPATCH(IDirectSoundBuffer_Play)(pThis, xbnull, xbnull, dwFlags);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DS3DBUFFER*            pc3DBufferParameters,
    DWORD                    dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pc3DBufferParameters)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetAllParameters(pThis->EmuDirectSound3DBuffer8, pc3DBufferParameters, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetBufferData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetBufferData)
(
    XbHybridDSBuffer*       pHybridThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG(pvBufferData)
        LOG_FUNC_ARG(dwBufferBytes)
        LOG_FUNC_END;


    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    // Release old buffer if exists, this is needed in order to set lock pointer buffer to nullptr.
    if (pThis->Host_lock.pLockPtr1 != nullptr) {

        DSoundGenericUnlock(pThis->EmuFlags,
                            pThis->EmuDirectSoundBuffer8,
                            pThis->EmuBufferDesc,
                            pThis->Host_lock,
                            xbnullptr,
                            pThis->X_lock.dwLockOffset,
                            pThis->X_lock.dwLockBytes1,
                            pThis->X_lock.dwLockBytes2);
    }

    //TODO: Current workaround method since dwBufferBytes do set to zero. Otherwise it will produce lock error message.
    if (dwBufferBytes == 0) {

        return DS_OK;
    }
    HRESULT hRet = DSERR_OUTOFMEMORY;
    DWORD dwStatus;

    // force wait until buffer is stop playing.
    pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    while ((dwStatus & DSBSTATUS_PLAYING) > 0) {
        SwitchToThread();
        pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    }

    // Allocate memory whenever made request internally
    if (pvBufferData == xbnullptr && DSoundSGEMenAllocCheck(dwBufferBytes)) {

        // Confirmed it perform a reset to default.
        DSoundBufferRegionSetDefault(pThis);

        GenerateXboxBufferCache(pThis->EmuBufferDesc, pThis->EmuFlags, dwBufferBytes, &pThis->X_BufferCache, pThis->X_BufferCacheSize);

        // Copy if given valid pointer.
        memcpy_s(pThis->X_BufferCache, pThis->X_BufferCacheSize, pvBufferData, dwBufferBytes);

        pThis->EmuFlags  ^= DSE_FLAG_BUFFER_EXTERNAL;

        DSoundDebugMuteFlag(pThis->X_BufferCacheSize, pThis->EmuFlags);

        // Only perform a resize, for lock emulation purpose.
        DSoundBufferResizeSetSize(pHybridThis, hRet, dwBufferBytes);

    } else if (pvBufferData != xbnullptr) {
        // Free internal buffer cache if exist
        if ((pThis->EmuFlags & DSE_FLAG_BUFFER_EXTERNAL) == 0) {
            free(pThis->X_BufferCache);
            DSoundSGEMemDealloc(pThis->X_BufferCacheSize);
        }
        pThis->X_BufferCache = pvBufferData;
        pThis->X_BufferCacheSize = dwBufferBytes;
        pThis->EmuFlags |= DSE_FLAG_BUFFER_EXTERNAL;

        DSoundDebugMuteFlag(pThis->X_BufferCacheSize, pThis->EmuFlags);

        // Only perform a resize, for lock emulation purpose.
        DSoundBufferResizeSetSize(pHybridThis, hRet, dwBufferBytes);

    }

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeAngles)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwInsideConeAngle)
		LOG_FUNC_ARG(dwOutsideConeAngle)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetConeAngles(pThis->EmuDirectSound3DBuffer8, dwInsideConeAngle, dwOutsideConeAngle, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeOrientation)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetConeOrientation(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(lConeOutsideVolume)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetConeOutsideVolume(pThis->EmuDirectSound3DBuffer8, lConeOutsideVolume, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetDistanceFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(flDistanceFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DListener_SetDistanceFactor(g_pDSoundPrimary3DListener8, flDistanceFactor, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetDopplerFactor)
(
    XbHybridDSBuffer*       pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flDopplerFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, flDopplerFactor, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetEG)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pEnvelopeDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    pThis->Xb_EnvolopeDesc = *pEnvelopeDesc;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFilter)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSFILTERDESC*         pFilterDesc)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pFilterDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// +s
// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFormat)
(
    XbHybridDSBuffer*       pHybridThis,
    LPCWAVEFORMATEX         pwfxFormat)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pwfxFormat)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->Xb_Flags,
                                                     pThis->EmuBufferDesc, pThis->EmuFlags,
                                                     pThis->EmuPlayFlags, pThis->EmuDirectSound3DBuffer8,
                                                     0, pThis->X_BufferCache, pThis->X_BufferCacheSize,
                                                     pThis->Xb_VoiceProperties, xbnullptr, pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFrequency)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwFrequency)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwFrequency)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency,
                                                        pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetHeadroom)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwHeadroom)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetHeadroom(pThis->EmuDirectSoundBuffer8, dwHeadroom,
                                                       pThis->Xb_VolumeMixbin, pThis->EmuFlags,
                                                       pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    XbHybridDSBuffer*       pHybridThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pds3db)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetI3DL2Source is using DSFXI3DL2Reverb structure, aka different interface.

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLFO
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetLFO) //Low Frequency Oscillators
(
    XbHybridDSBuffer*       pHybridThis,
    LPCDSLFODESC            pLFODesc)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pLFODesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetLoopRegion)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwLoopStart)
		LOG_FUNC_ARG(dwLoopLength)
		LOG_FUNC_END;

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    pThis->EmuRegionLoopStartOffset = dwLoopStart;
    pThis->EmuRegionLoopLength = dwLoopLength;
    pThis->EmuBufferToggle = X_DSB_TOGGLE_LOOP;

    DWORD dwStatus;
    pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    HRESULT hRet;

    if ((dwStatus & DSBSTATUS_PLAYING) > 0) {
        if ((dwStatus & DSBSTATUS_LOOPING) > 0) {
            pThis->EmuDirectSoundBuffer8->Stop();
            DSoundBufferUpdate(pHybridThis, pThis->EmuPlayFlags, hRet);
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    return DS_OK;
}

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMaxDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(flMaxDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetMaxDistance(pThis->EmuDirectSound3DBuffer8, flMaxDistance, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMinDistance)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flMinDistance,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(flMinDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetMinDistance(pThis->EmuDirectSound3DBuffer8, flMinDistance, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBins)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwMixBinMask)
{
    DSoundMutexGuardLock;
    HRESULT hRet = DS_OK;
    X_LPDSMIXBINS pMixBins = reinterpret_cast<X_LPDSMIXBINS>(dwMixBinMask);

    if (g_LibVersion_DSOUND < 4039) {
        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pHybridThis)
            LOG_FUNC_ARG(dwMixBinMask)
            LOG_FUNC_END;

        LOG_UNIMPLEMENTED();
    }
    else {
        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pHybridThis)
            LOG_FUNC_ARG(pMixBins)
            LOG_FUNC_END;

        EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
        hRet = HybridDirectSoundBuffer_SetMixBins(pThis->Xb_VoiceProperties, pMixBins, pThis->EmuBufferDesc.lpwfxFormat, pThis->EmuBufferDesc);
    }

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG(dwMixBinMask)
        LOG_FUNC_ARG(alVolumes)
        LOG_FUNC_END;

    // NOTE: Use this function for XDK 3911 only because the implementation was changed
    // somewhere around the December 2001 (4134) update (or earlier, maybe).

    LOG_UNIMPLEMENTED();

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_8
// ******************************************************************
// This revision is only used in XDK 4039 and higher.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
(
    XbHybridDSBuffer*       pHybridThis,
    X_LPDSMIXBINS           pMixBins)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetMixBinVolumes_8(pThis->EmuDirectSoundBuffer8, pMixBins, pThis->Xb_VoiceProperties,
                                                              pThis->EmuFlags, pThis->Xb_VolumeMixbin, pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMode)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwMode,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwMode)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetMode(pThis->EmuDirectSound3DBuffer8, dwMode, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetNotificationPositions)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwNotifyCount)
		LOG_FUNC_ARG(paNotifies)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = DSERR_INVALIDPARAM;

    // If we have a valid buffer, query a PC IDirectSoundNotify pointer and
    // use the paramaters as is since they are directly compatible, then release
    // the pointer. Any buffer that uses this *MUST* be created with the
    // DSBCAPS_CTRLPOSITIONNOTIFY flag!

    IDirectSoundNotify* pNotify = nullptr;

    if (pThis) {
        if (pThis->EmuDirectSoundBuffer8) {
            hRet = pThis->EmuDirectSoundBuffer8->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&pNotify);
            if (hRet == DS_OK) {
                hRet = pNotify->SetNotificationPositions(dwNotifyCount, paNotifies);
                if (hRet != DS_OK) {
                    EmuLog(LOG_LEVEL::WARNING, "Could not set notification position(s)!");
                }

                pNotify->Release();
            } else {
                EmuLog(LOG_LEVEL::WARNING, "Could not create notification interface!");
            }
        }
    }

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetOutputBuffer)
(
    XbHybridDSBuffer*   pHybridThis,
    XbHybridDSBuffer*   pOutputBuffer)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // NOTE: SetOutputBuffer is not possible in PC's DirectSound due to 3D controller requirement on ouput buffer to work simultaneously.
    // Test case: MultiPass sample
    // Best to emulate this LLE instead of HLE.

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPitch)
(
    XbHybridDSBuffer*       pHybridThis,
    LONG                    lPitch)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(lPitch)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetPitch(pThis->EmuDirectSoundBuffer8, lPitch,
                                                    pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPlayRegion)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(dwPlayStart)
		LOG_FUNC_ARG(dwPlayLength)
		LOG_FUNC_END;

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    pThis->EmuBufferToggle = X_DSB_TOGGLE_PLAY;
    pThis->EmuRegionPlayStartOffset = dwPlayStart;
    pThis->EmuRegionPlayLength = dwPlayLength;

    // Confirmed play region do overwrite loop region value.
    pThis->EmuRegionLoopStartOffset = 0;
    pThis->EmuRegionLoopLength = 0;

    DWORD dwStatus;
    pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    HRESULT hRet;

    if ((dwStatus & DSBSTATUS_PLAYING) > 0) {
        if ((dwStatus & DSBSTATUS_LOOPING) == 0) {
            pThis->EmuDirectSoundBuffer8->Stop();
            DSoundBufferUpdate(pHybridThis, pThis->EmuPlayFlags, hRet);
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPosition)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetPosition(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffCurve)
(
    XbHybridDSBuffer*       pHybridThis,
    const FLOAT*            pflPoints,
    DWORD                   dwPointCount,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pflPoints)
		LOG_FUNC_ARG(dwPointCount)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: Individual 3D buffer function.

    LOG_NOT_SUPPORTED();

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffFactor)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   flRolloffFactor,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(flRolloffFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetRolloffFactor is only supported for host primary buffer's 3D Listener.

    LOG_UNIMPLEMENTED();

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetVelocity)
(
    XbHybridDSBuffer*       pHybridThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSound3DBuffer_SetVelocity(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    XbHybridDSBuffer*       pHybridThis,
    LONG                    lVolume)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags,
                                                     pThis->Xb_VolumeMixbin, pHybridThis->p_CDSVoice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Stop
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Stop)
(
    XbHybridDSBuffer*       pHybridThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pHybridThis);

    HRESULT hRet = D3D_OK;

    if (pHybridThis != nullptr) {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
        hRet = pThis->EmuDirectSoundBuffer8->Stop();
        pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
    }

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_StopEx)
(
    XbHybridDSBuffer*       pHybridThis,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG(rtTimeStamp)
        LOG_FUNC_ARG(dwFlags)
        LOG_FUNC_END;

    EmuDirectSoundBuffer* pThis = pHybridThis->emuDSBuffer;
    HRESULT hRet = DS_OK;

    //TODO: RadWolfie - Rayman 3 crash at end of first intro for this issue...
    // if only return DS_OK, then it works fine until end of 2nd intro it crashed.

    // Do not allow to process - Xbox reject it.
    if (dwFlags > X_DSBSTOPEX_ALL) {
        hRet = DSERR_INVALIDPARAM;

    // Only flags can be process here.
    } else {
        if (dwFlags == X_DSBSTOPEX_IMMEDIATE) {
            hRet = pThis->EmuDirectSoundBuffer8->Stop();
            pThis->Xb_rtStopEx = 0LL;
        }
        else {
            bool isLooping;
            if ((pThis->EmuPlayFlags & X_DSBPLAY_LOOPING) > 0) {
                isLooping = true;
            }
            else {
                isLooping = false;
            }

            if ((dwFlags & X_DSBSTOPEX_ENVELOPE) > 0) {
                if (rtTimeStamp == 0LL) {
                    xboxkrnl::LARGE_INTEGER getTime;
                    xboxkrnl::KeQuerySystemTime(&getTime);
                    pThis->Xb_rtStopEx = getTime.QuadPart;
                }
                else {
                    pThis->Xb_rtStopEx = rtTimeStamp;
                }
                pThis->Xb_rtStopEx += (pThis->Xb_EnvolopeDesc.dwRelease * 512) / 48000;
            }
            else {
                pThis->Xb_rtStopEx = rtTimeStamp;
            }

            if ((dwFlags & X_DSBSTOPEX_RELEASEWAVEFORM) > 0) {
                // Release from loop region.
                pThis->EmuPlayFlags &= ~X_DSBPLAY_LOOPING;
            }

            DWORD dwValue, dwStatus;
            pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);

            if (pThis->EmuBufferToggle != X_DSB_TOGGLE_DEFAULT) {

                pThis->EmuDirectSoundBuffer8->GetCurrentPosition(nullptr, &dwValue);
                hRet = pThis->EmuDirectSoundBuffer8->Stop();

                DSoundBufferResizeUpdate(pHybridThis, pThis->EmuPlayFlags, hRet, 0, pThis->X_BufferCacheSize);

                dwValue += pThis->EmuRegionPlayStartOffset;
                if (isLooping) {
                    dwValue += pThis->EmuRegionLoopStartOffset;
                }

                pThis->EmuBufferToggle = X_DSB_TOGGLE_DEFAULT;
                pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwValue);
            }

            if (dwStatus & DSBSTATUS_PLAYING && rtTimeStamp != 0LL) {
                pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
            }
            else {
                pThis->EmuDirectSoundBuffer8->Stop();
                pThis->Xb_rtStopEx = 0LL;
            }
        }
    }

    return hRet;
}

// ******************************************************************
// * patch:  IDirectSoundBuffer_Set3DVoiceData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Set3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    DWORD a2
)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pHybridThis)
        LOG_FUNC_ARG(a2)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Use3DVoiceData)
(
    XbHybridDSBuffer*       pHybridThis,
    LPUNKNOWN               pUnknown)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pHybridThis)
		LOG_FUNC_ARG(pUnknown)
		LOG_FUNC_END;

    LOG_NOT_SUPPORTED();

    return DS_OK;
}
