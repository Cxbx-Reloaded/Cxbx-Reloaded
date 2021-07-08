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
#define LOG_PREFIX CXBXR_MODULE::DSSTREAM


#include <core\kernel\exports\xboxkrnl.h>
#include <dsound.h>
#include "DirectSoundGlobal.hpp" // Global variables

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include "..\XbDSoundLogging.hpp"

#include "DSStream_PacketManager.hpp"

// TODO: Tasks need to do for DirectSound HLE
// * Missing CDirectSoundStream patch
//   * CDirectSoundStream_Set3DVoiceData (new, undocument)
//   * CDirectSoundStream_Use3DVoiceData (new, undocument)
//   * IDirectSoundStream_QueryInterface (not require)
//   * IDirectSoundStream_QueryInterfaceC (not require)

xbox::X_CMcpxStream::_vtbl xbox::X_CMcpxStream::vtbl =
{
    0xBEEFC001,                     // 0x00
    0xBEEFC002,                     // 0x04
    0xBEEFC003,                     // 0x08
    0xBEEFC004,                     // 0x0C
    &xbox::EMUPATCH(CMcpxStream_Dummy_0x10),// 0x10
};

xbox::X_CDirectSoundStream::_vtbl xbox::X_CDirectSoundStream::vtbl_r1 =
{
    &xbox::EMUPATCH(CDirectSoundStream_AddRef),          // 0x00
    &xbox::EMUPATCH(CDirectSoundStream_Release),         // 0x04
/*
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo) PURE;
*/
    &xbox::EMUPATCH(CDirectSoundStream_GetInfo),        // 0x08
    &xbox::EMUPATCH(CDirectSoundStream_GetStatus__r1),  // 0x0C
    &xbox::EMUPATCH(CDirectSoundStream_Process),        // 0x10
    &xbox::EMUPATCH(CDirectSoundStream_Discontinuity),  // 0x14
    &xbox::EMUPATCH(CDirectSoundStream_Flush),          // 0x18
    0xBEEFB003,                                         // 0x1C // unknown function
    0xBEEFB004,                                         // 0x20 // DS_CRefCount_AddRef
    0xBEEFB005,                                         // 0x24 // DS_CRefCount_Release
    0xBEEFB006,                                         // 0x28
    0xBEEFB007,                                         // 0x2C
    0xBEEFB008,                                         // 0x30
    0xBEEFB009,                                         // 0x34
    0xBEEFB00A,                                         // 0x38
};

xbox::X_CDirectSoundStream::_vtbl xbox::X_CDirectSoundStream::vtbl_r2 =
{
    &xbox::EMUPATCH(CDirectSoundStream_AddRef),          // 0x00
    &xbox::EMUPATCH(CDirectSoundStream_Release),         // 0x04
/*
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo) PURE;
*/
    &xbox::EMUPATCH(CDirectSoundStream_GetInfo),        // 0x08
    &xbox::EMUPATCH(CDirectSoundStream_GetStatus__r2),  // 0x0C
    &xbox::EMUPATCH(CDirectSoundStream_Process),        // 0x10
    &xbox::EMUPATCH(CDirectSoundStream_Discontinuity),  // 0x14
    &xbox::EMUPATCH(CDirectSoundStream_Flush),          // 0x18
    0xBEEFB003,                                         // 0x1C // unknown function
    0xBEEFB004,                                         // 0x20 // DS_CRefCount_AddRef
    0xBEEFB005,                                         // 0x24 // DS_CRefCount_Release
    0xBEEFB006,                                         // 0x28
    0xBEEFB007,                                         // 0x2C
    0xBEEFB008,                                         // 0x30
    0xBEEFB009,                                         // 0x34
    0xBEEFB00A,                                         // 0x38
};

// construct vtable (or grab ptr to existing)
xbox::X_CDirectSoundStream::X_CDirectSoundStream(bool is3D) : Xb_Voice(is3D)
{
    pMcpxStream = new xbox::X_CMcpxStream(this);
    if (g_LibVersion_DSOUND < 4134) {
        pVtbl = &vtbl_r1;
    }
    else {
        pVtbl = &vtbl_r2;
    }
}

/* NOTE: SUCCEEDED define is only checking for is equal or greater than zero value.
    And FAILED check for less than zero value. Since DS_OK is only 0 base on DirectSound documentation,
    there is chance of failure which contain value greater than 0.
 */

#include "DirectSoundInline.hpp"

// ******************************************************************
// * patch: DirectSoundDoWork (stream)
// ******************************************************************
void DirectSoundDoWork_Stream(xbox::LARGE_INTEGER& time)
{
    // Actually, DirectSoundStream need to process buffer packets here.
    vector_ds_stream::iterator ppDSStream = g_pDSoundStreamCache.begin();
    for (; ppDSStream != g_pDSoundStreamCache.end(); ppDSStream++) {
        if ((*ppDSStream)->Host_BufferPacketArray.empty()) {
            continue;
        }
        xbox::X_CDirectSoundStream* pThis = (*ppDSStream);
        // TODO: Do we need this in async thread loop?
        if (pThis->Xb_rtPauseEx != 0LL && pThis->Xb_rtPauseEx <= time.QuadPart) {
            pThis->Xb_rtPauseEx = 0LL;
            pThis->EmuFlags &= ~(DSE_FLAG_PAUSE | DSE_FLAG_PAUSENOACTIVATE);
            // Don't call play here, let DSStream_Packet_Process deal with it.
        }
        // If has flush async requested then verify time has expired to perform flush process.
        if ((pThis->EmuFlags & DSE_FLAG_FLUSH_ASYNC) > 0 && pThis->Xb_rtFlushEx <= time.QuadPart) {
            if (pThis->Xb_rtFlushEx == 0LL) {
                EmuLog(LOG_LEVEL::WARNING, "Attempted to flush without Xb_rtFlushEx set to non-zero");
            }
            DSStream_Packet_Flush(pThis);
        } else {
            DSStream_Packet_Process(pThis);
        }
    }
}

// ******************************************************************
// * patch: CDirectSoundStream_AddRef
// ******************************************************************
xbox::ulong_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_AddRef)
(
    X_CDirectSoundStream*   pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = HybridDirectSoundBuffer_AddRef(pThis->EmuDirectSoundBuffer8);

    return uRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_Release
// ******************************************************************
xbox::ulong_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_Release)
(
    X_CDirectSoundStream*   pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = 0;
    if (pThis != 0 && (pThis->EmuDirectSoundBuffer8 != 0)) {
        uRet = pThis->EmuDirectSoundBuffer8->Release();

        if (uRet == 0) {
            if (pThis->EmuDirectSound3DBuffer8 != nullptr) {
                pThis->EmuDirectSound3DBuffer8->Release();
            }

            // remove cache entry
            vector_ds_stream::iterator ppDSStream = std::find(g_pDSoundStreamCache.begin(), g_pDSoundStreamCache.end(), pThis);
            if (ppDSStream != g_pDSoundStreamCache.end()) {
                g_pDSoundStreamCache.erase(ppDSStream);
            }

            for (auto buffer = pThis->Host_BufferPacketArray.begin(); buffer != pThis->Host_BufferPacketArray.end();) {
                DSStream_Packet_Clear(buffer, XMP_STATUS_FLUSHED, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);
            }

            if (pThis->EmuBufferDesc.lpwfxFormat != nullptr) {
                free(pThis->EmuBufferDesc.lpwfxFormat);
            }
            // NOTE: Do not release X_BufferCache! X_BufferCache is using xbox buffer.

            delete pThis;
        }
    }

    RETURN(uRet);
}

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    OUT X_CDirectSoundStream**  ppStream)
{
    DSoundMutexGuardLock;

    // Research reveal DirectSound creation check is part of the requirement.
    if (!g_pDSound8 && !g_bDSoundCreateCalled) {
        HRESULT hRet;

        hRet = xbox::EMUPATCH(DirectSoundCreate)(nullptr, &g_pDSound8, nullptr);
        if (hRet != DS_OK) {
            CxbxKrnlCleanup("Unable to initialize DirectSound!");
        }
    }

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdssd)
		LOG_FUNC_ARG_OUT(ppStream)
		LOG_FUNC_END;

    HRESULT hRet = DS_OK;

    //If out of space, return out of memory.
    if (X_DIRECTSOUND_CACHE_COUNT == X_DIRECTSOUND_CACHE_MAX) {

        hRet = DSERR_OUTOFMEMORY;
        *ppStream = xbox::zeroptr;
    } else {

        DSBUFFERDESC DSBufferDesc = { 0 };


        DWORD dwAcceptableMask = 0x00000010; // TODO: Note 0x00040000 is being ignored (DSSTREAMCAPS_LOCDEFER)

        if (pdssd->dwFlags & (~dwAcceptableMask)) {
            EmuLog(LOG_LEVEL::WARNING, "Use of unsupported pdssd->dwFlags mask(s) (0x%.08X)", pdssd->dwFlags & (~dwAcceptableMask));
        }
        DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        //DSBufferDesc->dwFlags = (pdssd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
        DSBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2 | //aka DSBCAPS_DEFAULT + control position
            (g_XBAudio.mute_on_unfocus ? 0 : DSBCAPS_GLOBALFOCUS);

        if ((pdssd->dwFlags & DSBCAPS_CTRL3D) > 0) {
            DSBufferDesc.dwFlags |= DSBCAPS_CTRL3D;
        } else {
            DSBufferDesc.dwFlags |= DSBCAPS_CTRLPAN;
        }

        // TODO: Garbage Collection
        *ppStream = new X_CDirectSoundStream((DSBufferDesc.dwFlags & DSBCAPS_CTRL3D) != 0);

        DSoundBufferSetDefault((*ppStream), DSBPLAY_LOOPING, pdssd->dwFlags);
        (*ppStream)->Xb_rtFlushEx = 0LL;

        // We have to set DSBufferDesc last due to EmuFlags must be either 0 or previously written value to preserve other flags.
        GeneratePCMFormat(DSBufferDesc, pdssd->lpwfxFormat, (DWORD &)pdssd->dwFlags, (*ppStream)->EmuFlags, 0,
                          xbox::zeroptr, (*ppStream)->X_BufferCacheSize, (*ppStream)->Xb_VoiceProperties, pdssd->mixBinsOutput,
                          &(*ppStream)->Xb_Voice);

        // Test case: Star Wars: KotOR has one packet greater than 5 seconds worth. Increasing to 10 seconds allow stream to work until
        // another test case below proven host's buffer size does not matter since packet's size can be greater than host's buffer size.
        // Test case: GTA 3 / Vice City, and some other titles has packet's buffer size are bigger than 10 seconds worth of buffer size.
        // Allocate at least 5 second worth of bytes in PCM format to allow partial upload packet's buffer.
        DSBufferDesc.dwBufferBytes = DSBufferDesc.lpwfxFormat->nAvgBytesPerSec * 5;
        (*ppStream)->EmuBufferDesc = DSBufferDesc;

        (*ppStream)->X_MaxAttachedPackets = pdssd->dwMaxAttachedPackets;
        (*ppStream)->Host_BufferPacketArray.reserve(pdssd->dwMaxAttachedPackets);
        (*ppStream)->Host_dwWriteOffsetNext = 0;
        (*ppStream)->Host_dwLastWritePos = 0;
        (*ppStream)->Host_isProcessing = false;
        (*ppStream)->Xb_lpfnCallback = pdssd->lpfnCallback;
        (*ppStream)->Xb_lpvContext = pdssd->lpvContext;
        (*ppStream)->Xb_Status = 0;
        //TODO: Implement mixbin variable support. Or just merge pdssd struct into DS Stream class.

        LOG_FUNC_BEGIN_ARG_RESULT
            LOG_FUNC_ARG_RESULT(ppStream)
        LOG_FUNC_END_ARG_RESULT;

        hRet = DSoundBufferCreate(&DSBufferDesc, (*ppStream)->EmuDirectSoundBuffer8);
        if (FAILED(hRet)) {
            std::stringstream output;
            output << "Xbox:\n" << pdssd;
            output << "\nHost converison:\n" << DSBufferDesc;
            EmuLog(LOG_LEVEL::WARNING, output.str().c_str());
            output.str("");
            output << static_cast<DS_RESULT>(hRet);
            CxbxKrnlCleanup("DSS: DSoundBufferCreate error: %s", output.str().c_str());
        }
        else {
            if (DSBufferDesc.dwFlags & DSBCAPS_CTRL3D) {
                DSound3DBufferCreate((*ppStream)->EmuDirectSoundBuffer8, (*ppStream)->EmuDirectSound3DBuffer8);
            }

            DSoundDebugMuteFlag((*ppStream)->EmuBufferDesc.dwBufferBytes, (*ppStream)->EmuFlags);

            // Pre-set volume to enforce silence if one of audio codec is disabled.
            HybridDirectSoundBuffer_SetVolume((*ppStream)->EmuDirectSoundBuffer8, 0L, (*ppStream)->EmuFlags,
                (*ppStream)->Xb_VolumeMixbin, &(*ppStream)->Xb_Voice);

            g_pDSoundStreamCache.push_back(*ppStream);
        }
    }

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_CreateSoundStream
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSound_CreateSoundStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC*         pdssd,
    OUT X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("DirectSoundCreateStream");

    HRESULT hRet = EMUPATCH(DirectSoundCreateStream)(pdssd, ppStream);

    return hRet;
}

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(CMcpxStream_Dummy_0x10)(dword_xt dwDummy1, dword_xt dwDummy2)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwDummy1)
		LOG_FUNC_ARG(dwDummy2)
		LOG_FUNC_END;

    // Causes deadlock in Halo...
    // TODO: Verify that this is a Vista related problem (I HATE Vista!)
//    EmuLog(LOG_LEVEL::WARNING, "EmuCMcpxStream_Dummy_0x10 is ignored!");

    return;
}

/* ------------- Sorted relative functions begin ------------------*/

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_Discontinuity)
(
    X_CDirectSoundStream*   pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    // default ret = DSERR_GENERIC

    // Perform check if packets exist, then mark the last submited packet as end of stream.
    if (!pThis->Host_BufferPacketArray.empty()) {
        pThis->Host_BufferPacketArray.back().isStreamEnd = true;
    }

    return DS_OK;
}

xbox::hresult_xt CxbxrImpl_CDirectSoundStream_Flush
(
    xbox::X_CDirectSoundStream* pThis)
{

    DSoundBufferSynchPlaybackFlagRemove(pThis->EmuFlags);

    while (DSStream_Packet_Flush(pThis));

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_Flush)
(
    X_CDirectSoundStream*   pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    return CxbxrImpl_CDirectSoundStream_Flush(pThis);
}

// ******************************************************************
// * patch: CDirectSoundStream_FlushEx
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    dword_xt                   dwFlags)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(rtTimeStamp)
		LOG_FUNC_ARG_TYPE(DSSFLUSHEX_FLAG, dwFlags)
		LOG_FUNC_END;

    HRESULT hRet = DSERR_INVALIDPARAM;
    // Reset flags here to reprocess dwFlags request.
    DSStream_Packet_FlushEx_Reset(pThis);

    // Cannot use rtTimeStamp here, it must be flush.
    if (dwFlags == X_DSSFLUSHEX_IMMEDIATE) {

        hRet = CxbxrImpl_CDirectSoundStream_Flush(pThis);

    }
    // Remaining flags require X_DSSFLUSHEX_ASYNC to be include.
    else if ((dwFlags & X_DSSFLUSHEX_ASYNC) > 0 && !pThis->Host_BufferPacketArray.empty()) {
        // If rtTimeStamp is zero'd, then call flush once and allow process flush in worker thread.
        if (rtTimeStamp == 0LL) {
            xbox::LARGE_INTEGER getTime;
            xbox::KeQuerySystemTime(&getTime);
            pThis->Xb_rtFlushEx = getTime.QuadPart;
            pThis->EmuFlags |= DSE_FLAG_IS_FLUSHING;
        }
        else {
            pThis->Xb_rtFlushEx = rtTimeStamp;
        }

        pThis->EmuFlags |= DSE_FLAG_FLUSH_ASYNC;

        // Set or remove flags (This is the only place it will set/remove other than flush perform remove the flags.)
        if ((dwFlags & X_DSSFLUSHEX_ENVELOPE) > 0) {
            pThis->Xb_rtFlushEx += (pThis->Xb_EnvolopeDesc.dwRelease * 512) / 48000;
        }

        if ((dwFlags & X_DSSFLUSHEX_ENVELOPE2) > 0) {
            pThis->EmuFlags |= DSE_FLAG_ENVELOPE2;
        }
        else {
            pThis->EmuFlags ^= DSE_FLAG_ENVELOPE2;
        }
    }

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetInfo
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_GetInfo)
(
    X_CDirectSoundStream*   pThis,
    OUT LPXMEDIAINFO            pInfo)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pInfo)
		LOG_FUNC_END;

    if (pInfo) {
        pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC;
        pInfo->dwInputSize = pThis->EmuBufferDesc.lpwfxFormat->nBlockAlign;
        pInfo->dwOutputSize = 0;
        pInfo->dwMaxLookahead = std::max(static_cast<uint32_t>(pThis->EmuBufferDesc.lpwfxFormat->nChannels * static_cast<uint32_t>(pThis->EmuBufferDesc.lpwfxFormat->wBitsPerSample) / 8) * 32, static_cast<uint32_t>(pThis->EmuBufferDesc.lpwfxFormat->nBlockAlign) * 2);
    }

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus (3911+)
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_GetStatus__r1)
(
    X_CDirectSoundStream*   pThis,
    OUT dword_xt*           pdwStatus)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pdwStatus)
        LOG_FUNC_END;

    DWORD dwStatusXbox = 0, dwStatusHost;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatusHost);

    // Convert host to xbox status flag.
    if (hRet == DS_OK) {
        if (pThis->Host_BufferPacketArray.size() != pThis->X_MaxAttachedPackets) {
            dwStatusXbox = X_DSSSTATUS_READY;
        }

    }
    else {
        hRet = DSERR_GENERIC;
    }

    if (pdwStatus != xbox::zeroptr) {
        *pdwStatus = dwStatusXbox;
    }

    // Only used for debug any future issues with custom stream's packet management
    EmuLog(LOG_LEVEL::DEBUG, "packet array size: %d", pThis->Host_BufferPacketArray.size());

    LOG_FUNC_BEGIN_ARG_RESULT
        LOG_FUNC_ARG_RESULT_TYPE(DSSSTATUS_FLAG, pdwStatus)
    LOG_FUNC_END_ARG_RESULT;

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus (4134+)
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_GetStatus__r2)
(
    X_CDirectSoundStream*   pThis,
    OUT dword_xt*           pdwStatus)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pdwStatus)
        LOG_FUNC_END;

    DWORD dwStatusXbox = pThis->Xb_Status, dwStatusHost;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatusHost);

    // Convert host to xbox status flag.
    if (hRet == DS_OK) {
        if (pThis->Host_BufferPacketArray.size() != pThis->X_MaxAttachedPackets) {
            dwStatusXbox |= X_DSSSTATUS_READY;
        }
        // HACK: Likely a hack but force mimic stream's status is playing while in background is ongoing in flush process.
        // Testcase: Obscure; Crash Twinsanity
        if ((pThis->EmuFlags & DSE_FLAG_IS_FLUSHING) != 0) {
            // TODO: Find a way to implement deterred commands system if possible.
            // Then this hack may could be remove or replace to determine internal status base on deterred commands?
            // NOTE: It may not be likely behave like on hardware in paused/stopped state. See todo note above.
            LOG_TEST_CASE("Internal stream is currently flushing, enforcing status to playing state");
            dwStatusXbox |= X_DSSSTATUS_PLAYING;
        }
        else if (!pThis->Host_BufferPacketArray.empty()) {
            if ((pThis->EmuFlags & DSE_FLAG_PAUSE) != 0) {
                dwStatusXbox |= X_DSSSTATUS_PAUSED;
            }
            else if ((pThis->EmuFlags & (DSE_FLAG_PAUSE | DSE_FLAG_PAUSENOACTIVATE | DSE_FLAG_IS_FLUSHING)) == 0) {
                dwStatusXbox |= X_DSSSTATUS_PLAYING;
            }
        }
    } else {
        dwStatusXbox = 0;
        hRet = DSERR_GENERIC;
    }

    if (pdwStatus != xbox::zeroptr) {
        *pdwStatus = dwStatusXbox;
    }

    // Only used for debug any future issues with custom stream's packet management
    EmuLog(LOG_LEVEL::DEBUG, "packet array size: %d", pThis->Host_BufferPacketArray.size());

    LOG_FUNC_BEGIN_ARG_RESULT
        LOG_FUNC_ARG_RESULT_TYPE(DSSSTATUS_FLAG, pdwStatus)
    LOG_FUNC_END_ARG_RESULT;

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetVoiceProperties
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_GetVoiceProperties)
(
    X_CDirectSoundStream*   pThis,
    OUT X_DSVOICEPROPS*     pVoiceProps
)
{
    DSoundMutexGuardLock;
 LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pVoiceProps)
        LOG_FUNC_END;

    if (pVoiceProps == xbox::zeroptr) {
        LOG_TEST_CASE("pVoiceProps == xbox::zeroptr");
        RETURN(DS_OK);
    }

    HRESULT hRet = HybridDirectSoundBuffer_GetVoiceProperties(pThis->Xb_VoiceProperties, pVoiceProps);

    return hRet;
}


xbox::hresult_xt CxbxrImpl_CDirectSoundStream_PauseEx(
    xbox::X_CDirectSoundStream* pThis,
    xbox::REFERENCE_TIME        rtTimestamp,
    xbox::dword_xt              dwPause)
{
    xbox::hresult_xt hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
        pThis->Host_isProcessing, rtTimestamp, pThis->Xb_rtPauseEx);

    if ((pThis->EmuFlags & DSE_FLAG_PAUSE) != 0) {
        pThis->Host_isProcessing = false;
    }
    else if (!pThis->Host_isProcessing) {
        DSStream_Packet_Process(pThis);
    }

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_Pause)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                dwPause)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_TYPE(DSSPAUSE_FLAG, dwPause)
		LOG_FUNC_END;

	if (!pThis) {
		LOG_TEST_CASE("CDirectSoundStream_Pause called with pThis = nullptr");
		return xbox::status_success;
	}

    HRESULT hRet = CxbxrImpl_CDirectSoundStream_PauseEx(pThis, 0LL, dwPause);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_PauseEx
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    dword_xt                dwPause)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(rtTimestamp)
        LOG_FUNC_ARG_TYPE(DSSPAUSE_FLAG, dwPause)
        LOG_FUNC_END;

    // This function wasn't part of the XDK until 4721. (Same as IDirectSoundBuffer_PauseEx?)

    HRESULT hRet = CxbxrImpl_CDirectSoundStream_PauseEx(pThis, rtTimestamp, dwPause);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream   *pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pInputBuffer)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // Research data:
    // * Max packet size permitted is 0x2000 (or 8,192 decimal) of buffer.
    //   * Somehow other titles are using more than 0x2000 for max size. Am using a hacky host method for now (see pBuffer_data).

    if (pThis->EmuDirectSoundBuffer8 != nullptr) {

        if (pInputBuffer != xbox::zeroptr) {

            // Add packets from title until it gets full.
            if (pThis->Host_BufferPacketArray.size() != pThis->X_MaxAttachedPackets) {
                host_voice_packet packet_input;
                packet_input.pBuffer_data = nullptr;
                packet_input.xmp_data = *pInputBuffer;
                packet_input.xmp_data.dwMaxSize = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, pInputBuffer->dwMaxSize);
                if (packet_input.xmp_data.dwMaxSize != 0) {
                    packet_input.pBuffer_data = malloc(packet_input.xmp_data.dwMaxSize);
                    DSoundSGEMemAlloc(packet_input.xmp_data.dwMaxSize);
                }
                packet_input.nextWriteOffset = pThis->Host_dwWriteOffsetNext;
                packet_input.lastWritePos = packet_input.nextWriteOffset;
                pThis->Host_dwWriteOffsetNext += packet_input.xmp_data.dwMaxSize;
                // Packet size may be larger than host's pre-allocated buffer size, loop is a requirement until within range is known.
                while (pThis->EmuBufferDesc.dwBufferBytes <= pThis->Host_dwWriteOffsetNext) {
                    pThis->Host_dwWriteOffsetNext -= pThis->EmuBufferDesc.dwBufferBytes;
                }
                packet_input.bufWrittenBytes = 0;
                packet_input.bufPlayed = 0;
                packet_input.isPlayed = false;
                packet_input.isStreamEnd = false;

                DSoundBufferOutputXBtoHost(pThis->EmuFlags, pThis->EmuBufferDesc, pInputBuffer->pvBuffer, pInputBuffer->dwMaxSize, packet_input.pBuffer_data, packet_input.xmp_data.dwMaxSize);

                pThis->Host_BufferPacketArray.push_back(packet_input);

                if (pInputBuffer->pdwStatus != xbox::zeroptr) {
                    (*pInputBuffer->pdwStatus) = XMP_STATUS_PENDING;
                }
                if (pInputBuffer->pdwCompletedSize != xbox::zeroptr) {
                    (*pInputBuffer->pdwCompletedSize) = 0;
                }
                if (pThis->Host_isProcessing == false && pThis->Host_BufferPacketArray.size() == 1) {
                    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(packet_input.nextWriteOffset);
                }

                if ((pThis->Xb_Status & X_DSSSTATUS_STARVED) > 0) {
                    pThis->Xb_Status &= ~X_DSSSTATUS_STARVED;
                }
                pThis->EmuFlags &= ~DSE_FLAG_IS_FLUSHING;
                DSStream_Packet_Process(pThis);
            // Once full it needs to change status to flushed when cannot hold any more packets.
            } else {
                if (pInputBuffer->pdwStatus != xbox::zeroptr) {
                    (*pInputBuffer->pdwStatus) = XMP_STATUS_FAILURE;
                }
            }
        }

        //TODO: What to do with output buffer audio variable? Need test case or functional source code.
        // NOTE: pOutputBuffer is reserved, must be set to NULL from titles.
        if (pOutputBuffer != xbox::zeroptr) {
            LOG_TEST_CASE("pOutputBuffer is not nullptr, please report title test case to issue tracker. Thanks!");
        }

    } else {
        if (pInputBuffer != xbox::zeroptr && pInputBuffer->pdwStatus != xbox::zeroptr) {
            (*pInputBuffer->pdwStatus) = XMP_STATUS_SUCCESS;
        }
    }

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    X_DS3DBUFFER*           pc3DBufferParameters,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pc3DBufferParameters)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetAllParameters(pThis->EmuDirectSound3DBuffer8, pc3DBufferParameters, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwInsideConeAngle,
    dword_xt                   dwOutsideConeAngle,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwInsideConeAngle)
		LOG_FUNC_ARG(dwOutsideConeAngle)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetConeAngles(pThis->EmuDirectSound3DBuffer8, dwInsideConeAngle, dwOutsideConeAngle, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetConeOrientation(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lConeOutsideVolume,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lConeOutsideVolume)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetConeOutsideVolume(pThis->EmuDirectSound3DBuffer8, lConeOutsideVolume, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetDistanceFactor
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetDistanceFactor)
(
    X_CDirectSoundStream*   pThis,
    float_xt                   flDistanceFactor,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flDistanceFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DListener_SetDistanceFactor(g_pDSoundPrimary3DListener8, flDistanceFactor, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetDopplerFactor
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetDopplerFactor)
(
    X_CDirectSoundStream*   pThis,
    float_xt                   flDopplerFactor,
    dword_xt                   dwApply)
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
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pEnvelopeDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    pThis->Xb_EnvolopeDesc = *pEnvelopeDesc;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetEG
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    X_DSENVOLOPEDESC*       pEnvelopeDesc)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetEG");

    return xbox::EMUPATCH(CDirectSoundStream_SetEG)(pThis, pEnvelopeDesc);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pFilterDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetFilter
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetFilter");

    return xbox::EMUPATCH(CDirectSoundStream_SetFilter)(pThis, pFilterDesc);
}

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*   pThis,
    LPCWAVEFORMATEX         pwfxFormat)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pwfxFormat)
		LOG_FUNC_END;

    while (DSStream_Packet_Flush(pThis));

    HRESULT hRet = HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->Xb_Flags,
                                                     pThis->EmuBufferDesc, pThis->EmuFlags, pThis->EmuPlayFlags,
                                                     pThis->EmuDirectSound3DBuffer8, 0, pThis->X_BufferCache,
                                                     pThis->X_BufferCacheSize, pThis->Xb_VoiceProperties,
                                                     xbox::zeroptr, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwFrequency)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwFrequency)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetFrequency
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwFrequency)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetFrequency");

    return xbox::EMUPATCH(CDirectSoundStream_SetFrequency)(pThis, dwFrequency);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwHeadroom)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetHeadroom(pThis->EmuDirectSoundBuffer8, dwHeadroom,
                                                       pThis->Xb_VolumeMixbin, pThis->EmuFlags, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwHeadroom)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetHeadroom");

    return xbox::EMUPATCH(CDirectSoundStream_SetHeadroom)(pThis, dwHeadroom);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pds3db)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetI3DL2Source is using DSFXI3DL2Reverb structure, aka different interface.

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pLFODesc)
        LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetLFO
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetLFO");

    return xbox::EMUPATCH(CDirectSoundStream_SetLFO)(pThis, pLFODesc);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                flMaxDistance,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flMaxDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetMaxDistance(pThis->EmuDirectSound3DBuffer8, flMaxDistance, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMinDistance,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fMinDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetMinDistance(pThis->EmuDirectSound3DBuffer8, fMinDistance, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    X_DSMIXBINBUNION  mixBins) // Can be dword_xt (up to 4039) or X_LPDSMIXBINS (4039+)
{
    DSoundMutexGuardLock;

    if (g_LibVersion_DSOUND < 4039) {
        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pThis)
            LOG_FUNC_ARG(mixBins.dwMixBinMask)
            LOG_FUNC_END;
    }
    else {
        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pThis)
            LOG_FUNC_ARG(mixBins.pMixBins)
            LOG_FUNC_END;
    }

    return HybridDirectSoundBuffer_SetMixBins(pThis->Xb_VoiceProperties, mixBins, pThis->EmuBufferDesc);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetMixBins
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    X_DSMIXBINBUNION  mixBins) // Can be dword_xt (up to 4039) or X_LPDSMIXBINS (4039+)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetMixBins");

    return xbox::EMUPATCH(CDirectSoundStream_SetMixBins)(pThis, mixBins);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwMode,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMode)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetMode(pThis->EmuDirectSound3DBuffer8, dwMode, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12)
(
    X_CDirectSoundStream*   pThis,
    dword_xt                   dwMixBinMask,
    const long_xt*             alVolumes)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMixBinMask)
		LOG_FUNC_ARG(alVolumes)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetMixBinVolumes_12(pThis->EmuDirectSoundBuffer8, dwMixBinMask, alVolumes, pThis->Xb_VoiceProperties,
                                                              pThis->EmuFlags, pThis->Xb_VolumeMixbin, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
// This revision API is only used in XDK 4039 and higher.
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*   pThis,
    X_LPDSMIXBINS           pMixBins)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetMixBinVolumes_8(pThis->EmuDirectSoundBuffer8, pMixBins, pThis->Xb_VoiceProperties,
                                                              pThis->EmuFlags, pThis->Xb_VolumeMixbin, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
    X_CDirectSoundStream*   pThis,
    XbHybridDSBuffer*       pOutputBuffer)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // NOTE: SetOutputBuffer is not possible in PC's DirectSound due to 3D controller requirement on ouput buffer to work simultaneously.
    // Test case: Red Faction 2
    // Best to emulate this LLE instead of HLE.

    LOG_NOT_SUPPORTED();

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lPitch)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(lPitch)
        LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetPitch(pThis->EmuDirectSoundBuffer8, lPitch, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lPitch)
{
    DSoundMutexGuardLock;

	LOG_FORWARD("CDirectSoundStream_SetPitch");

    return xbox::EMUPATCH(CDirectSoundStream_SetPitch)(pThis, lPitch);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetPosition)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetPosition(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
    X_CDirectSoundStream*   pThis,
    const float_xt*            pflPoints,
    dword_xt                   dwPointCount,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pflPoints)
		LOG_FUNC_ARG(dwPointCount)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream*   pThis,
    float_xt                   fRolloffFactor,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fRolloffFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetRolloffFactor is only supported for host primary buffer's 3D Listener.

    LOG_UNIMPLEMENTED();

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetVelocity)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    dword_xt                   dwApply)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSound3DBuffer_SetVelocity(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(CDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lVolume)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags,
                                                     pThis->Xb_VolumeMixbin, &pThis->Xb_Voice);

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    long_xt                    lVolume
)
{
    DSoundMutexGuardLock;

    LOG_FORWARD("CDirectSoundStream_SetVolume");

    return xbox::EMUPATCH(CDirectSoundStream_SetVolume)(pThis, lVolume);
}

// ******************************************************************
// * patch:  IDirectSoundStream_Set3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_Set3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_xt a2
)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(a2)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    RETURN(xbox::status_success);
}

// ******************************************************************
// * patch:  IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(IDirectSoundStream_Use3DVoiceData)
(
    X_CDirectSoundStream*   pThis,
    dword_xt a2
)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(a2)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    RETURN(xbox::status_success);
}
