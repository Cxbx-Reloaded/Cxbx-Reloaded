// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDSound.cpp
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
#define _XBOXKRNL_DEFEXTRN_
#define LOG_PREFIX CXBXR_MODULE::DSOUND

// prevent name collisions
namespace xboxkrnl {
    #include <xboxkrnl/xboxkrnl.h>
};

#include <dsound.h>
#include <thread>
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuXTL.h"
#include "Common/Settings.hpp"


#ifndef _DEBUG_TRACE
//#define _DEBUG_TRACE
#include "Logging.h"
#undef _DEBUG_TRACE
#else
#include "Logging.h"
#endif

#include <mmreg.h>
#include <msacm.h>
#include <process.h>
#include <clocale>

// Temporary APU Timer Functions
// TODO: Move these to LLE APUDevice once we have one!

#define APU_TIMER_FREQUENCY	48000
LARGE_INTEGER APUInitialPerformanceCounter;
double NativeToXboxAPU_FactorForPerformanceFrequency = 0;

void ResetApuTimer()
{
	// Measure current host performance counter and frequency
	QueryPerformanceCounter(&APUInitialPerformanceCounter);
	NativeToXboxAPU_FactorForPerformanceFrequency = (double)APU_TIMER_FREQUENCY / APUInitialPerformanceCounter.QuadPart;
}

uint32_t GetAPUTime()
{
	::LARGE_INTEGER PerformanceCounter;
	QueryPerformanceCounter(&PerformanceCounter);

	// Re-Base on the time DirectSoundCreate was called
	PerformanceCounter.QuadPart -= APUInitialPerformanceCounter.QuadPart;
	// Apply a delta to make it appear to tick at 48khz
	PerformanceCounter.QuadPart = (ULONGLONG)(NativeToXboxAPU_FactorForPerformanceFrequency * PerformanceCounter.QuadPart);
	return (DWORD)PerformanceCounter.QuadPart;
}


// TODO: Tasks need to do for DirectSound HLE
// * Need create patches
//   * Ac97CreateMediaObject (Need OOVPA)
//   - WmaCreateDecoder (Need OOVPA, not require) Test case: WMAStream sample
//   - WmaCreateDecoderEx (Is just a forward to WmaCreateDecoder, nothing else)
//   - WmaCreateInMemoryDecoder (Need OOVPA, not require) Test case: WMAInMemory sample
//   - WmaCreateInMemoryDecoderEx (Is just a forward to WmaCreateInMemoryDecoder, nothing else)
//   * XWmaDecoderCreateMediaObject (Need OOVPA)
// * Missing CDirectSoundStream patch
//   * CDirectSoundStream_Set3DVoiceData (new, undocument)
//   * CDirectSoundStream_Use3DVoiceData (new, undocument)
//   * IDirectSoundStream_QueryInterface (not require)
//   * IDirectSoundStream_QueryInterfaceC (not require)
// * Missing IDirectSound patch
//   * IDirectSound_Compact (xbox doesn't use this method and cannot make oovpa for it.)
//   * IDirectSound_GetTime (not require)
//   * IDirectSound_MapBufferData (undocument)
//   * IDirectSound_SetCooperativeLevel (xbox doesn't use this method and cannot make oovpa for it.)
//   * IDirectSound_UnmapBufferData (undocument)
//   * IDirectSound_QueryInterfaceC (not require)
//   * IDirectSound_QueryInterface (not require)
// * Missing IDirectSoundBuffer patch
//   * IDirectSoundBuffer_Set3DVoiceData (new, undocument)
//   * IDirectSoundBuffer_Use3DVoiceData (new, undocument)
//   * IDirectSoundBuffer_QueryInterface (not require)
//   * IDirectSoundBuffer_QueryInterfaceC (not require)
// * Ixxxxx_ patch might not be a requirement.
//   * IDirectSound3DCalculator_Calculate3D
//   * IDirectSound3DCalculator_GetMixBinVolumes
//   * IDirectSound3DCalculator_GetPanData
//   * IDirectSound3DCalculator_GetVoiceData

// TODO: Tasks need to do for DirectSound LLE
// * Need to map range of 0x0fe800000 to 0xfe87ffff (?) and 0x0fec00000 to 0xfec00fff (?)

XTL::X_CMcpxStream::_vtbl XTL::X_CMcpxStream::vtbl =
{
    0xBEEFC001,                     // 0x00
    0xBEEFC002,                     // 0x04
    0xBEEFC003,                     // 0x08
    0xBEEFC004,                     // 0x0C
    &XTL::EMUPATCH(CMcpxStream_Dummy_0x10),// 0x10
};

XTL::X_CDirectSoundStream::_vtbl XTL::X_CDirectSoundStream::vtbl =
{
    &XTL::EMUPATCH(CDirectSoundStream_AddRef),          // 0x00
    &XTL::EMUPATCH(CDirectSoundStream_Release),         // 0x04
/*
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo) PURE;
*/
    &XTL::EMUPATCH(CDirectSoundStream_GetInfo),         // 0x08
    &XTL::EMUPATCH(CDirectSoundStream_GetStatus),       // 0x0C
    &XTL::EMUPATCH(CDirectSoundStream_Process),         // 0x10
    &XTL::EMUPATCH(CDirectSoundStream_Discontinuity),   // 0x14
    &XTL::EMUPATCH(CDirectSoundStream_Flush),           // 0x18
    0xBEEFB003,                                         // 0x1C
    0xBEEFB004,                                         // 0x20
    0xBEEFB005,                                         // 0x24
    0xBEEFB006,                                         // 0x28
    0xBEEFB007,                                         // 0x2C
    0xBEEFB008,                                         // 0x30
    0xBEEFB009,                                         // 0x34
    0xBEEFB00A,                                         // 0x38
};

XTL::X_XFileMediaObject::_vtbl XTL::X_XFileMediaObject::vtbl =
{
    &XTL::EMUPATCH(XFileMediaObject_AddRef),            // 0x00
    &XTL::EMUPATCH(XFileMediaObject_Release),           // 0x04
    &XTL::EMUPATCH(XFileMediaObject_GetInfo),           // 0x08
    &XTL::EMUPATCH(XFileMediaObject_GetStatus),         // 0x0C
    &XTL::EMUPATCH(XFileMediaObject_Process),           // 0x10
    &XTL::EMUPATCH(XFileMediaObject_Discontinuity),     // 0x14
    0xBEEFD007,                                         // 0x18
    &XTL::EMUPATCH(XFileMediaObject_Seek),              // 0x1C
    0xBEEFD009,                                         // 0x20
    &XTL::EMUPATCH(XFileMediaObject_DoWork),            // 0x24
};

/* NOTE: SUCCEEDED define is only checking for is equal or greater than zero value.
    And FAILED check for less than zero value. Since DS_OK is only 0 base on DirectSound documentation,
    there is chance of failure which contain value greater than 0.
 */

// size of DirectSound cache max size
#define X_DIRECTSOUND_CACHE_MAX 0x800

#define X_DIRECTSOUND_CACHE_COUNT (g_pDSoundBufferCache.size() + g_pDSoundStreamCache.size())

//Currently disabled since below may not be needed since under -6,400 is just silence yet accepting up to -10,000
// Xbox to PC volume ratio format (-10,000 / -6,400 )
//#define XBOX_TO_PC_VOLUME_RATIO 1.5625

// Xbox maximum synch playback audio
#define DSOUND_MAX_SYNCHPLAYBACK_AUDIO 29

#define vector_ds_buffer std::vector<XTL::X_CDirectSoundBuffer*>
#define vector_ds_stream std::vector<XTL::X_CDirectSoundStream*>

// Static Variable(s)
static Settings::s_audio            g_XBAudio = { 0 };
extern LPDIRECTSOUND8               g_pDSound8 = nullptr; //This is necessary in order to allow share with EmuDSoundInline.hpp
static LPDIRECTSOUNDBUFFER          g_pDSoundPrimaryBuffer = nullptr;
//TODO: RadWolfie - How to implement support if primary does not permit it for DSP usage?
static LPDIRECTSOUNDBUFFER8         g_pDSoundPrimaryBuffer8 = nullptr;
static LPDIRECTSOUND3DLISTENER8     g_pDSoundPrimary3DListener8 = nullptr;
vector_ds_buffer                    g_pDSoundBufferCache;
vector_ds_stream                    g_pDSoundStreamCache;
static int                          g_bDSoundCreateCalled = FALSE;
unsigned int                        g_iDSoundSynchPlaybackCounter = 0;
// Managed memory xbox audio variables
#define                             X_DS_SGE_COUNT_MAX 2047
#define                             X_DS_SGE_PAGE_MAX (4 * ONE_KB)
#define                             X_DS_SGE_SIZE_MAX (X_DS_SGE_COUNT_MAX * X_DS_SGE_PAGE_MAX)
DWORD                               g_dwXbMemAllocated = 0;
DWORD                               g_dwFree2DBuffers = 0;
DWORD                               g_dwFree3DBuffers = 0;
std::thread dsound_thread;
static void dsound_thread_worker(LPVOID);

#define RETURN_RESULT_CHECK(hRet) { \
    static bool bPopupShown = false; if (!bPopupShown && hRet) { bPopupShown = true; \
    printf("Return result report: 0x%08X\nIn %s (%s)\n", hRet, __func__, __FILE__); \
    EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "An issue has been found. Please report game title and console's output of return result," \
    " function, and file name to https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/485"); } return hRet; }

#include "EmuDSoundInline.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void CxbxInitAudio()
{
    g_EmuShared->GetAudioSettings(&g_XBAudio);
}

#ifdef __cplusplus
}
#endif

// ******************************************************************
// * patch: DirectSoundCreate
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    OUT LPDIRECTSOUND8* ppDirectSound,
    LPUNKNOWN       pUnknown)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pguidDeviceId)
		LOG_FUNC_ARG_OUT(ppDirectSound)
		LOG_FUNC_ARG(pUnknown)
		LOG_FUNC_END;

    static bool initialized = false;

    HRESULT hRet = DS_OK;

    if (!initialized) {
        InitializeCriticalSection(&g_DSoundCriticalSection);
        dsound_thread = std::thread(dsound_thread_worker, nullptr);
    }

    enterCriticalSection;

	ResetApuTimer();

    // Set this flag when this function is called
    g_bDSoundCreateCalled = TRUE;

    if (!initialized || g_pDSound8 == nullptr) {
        hRet = DirectSoundCreate8(&g_XBAudio.adapterGUID, &g_pDSound8, NULL);

        LPCSTR dsErrorMsg = nullptr;

        switch (hRet) {
            case DS_OK:
                // Is not a fatal error.
                break;
            case DSERR_ALLOCATED:
                dsErrorMsg = "Audio adapter is already allocated. Possible fault within Cxbx-Reloaded's emulator."
                            "\n\nPlease report to respective game compatibility issue.";
                break;
            case DSERR_INVALIDPARAM:
                dsErrorMsg = "DirectSoundCreate8 return invalid parameter."
                            "\n\nPlease report to respective game compatibility issue.";
                break;
            case DSERR_NOAGGREGATION:
                dsErrorMsg = "Audio adapter does not support aggregation."
                            "\n\nPlease use different audio adapter.";
                break;
            case DSERR_NODRIVER:
                dsErrorMsg = "Please select a valid audio adapter from Cxbx-Reloaded's config audio dialog."
                            "\n\nThen try again.";
                break;
            case DSERR_OUTOFMEMORY:
                dsErrorMsg = "Unable to allocate DirectSound subsystem class."
                            "\n\nPlease close any opened application(s) or restart computer before trying again.";
                break;
            default:
                dsErrorMsg = "DirectSoundCreate8 unknown failed: 0x%08X";
        }

        if (dsErrorMsg != nullptr) {
            CxbxKrnlCleanup(LOG_PREFIX, dsErrorMsg, hRet);
        }

        hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "g_pDSound8->SetCooperativeLevel Failed!");
        }

        // clear sound buffer cache
        vector_ds_buffer::iterator ppDSBuffer = g_pDSoundBufferCache.begin();
        for (; ppDSBuffer != g_pDSoundBufferCache.end();) {
            while (XTL::EMUPATCH(IDirectSoundBuffer_Release)((*ppDSBuffer)) != 0) {};
            ppDSBuffer = g_pDSoundBufferCache.begin();
        }
        g_pDSoundBufferCache.reserve(X_DIRECTSOUND_CACHE_MAX);

        // clear sound stream cache
        vector_ds_stream::iterator ppDSStream = g_pDSoundStreamCache.begin();
        for (; ppDSStream != g_pDSoundStreamCache.end();) {
            while (XTL::EMUPATCH(CDirectSoundStream_Release)((*ppDSStream)) != 0);
            ppDSStream = g_pDSoundStreamCache.begin();
        }
        g_pDSoundStreamCache.reserve(X_DIRECTSOUND_CACHE_MAX);

        //Create Primary Buffer in order for Xbox's DirectSound to manage complete control of it.
        DSBUFFERDESC bufferDesc = { 0 };
        bufferDesc.dwSize = sizeof(DSBUFFERDESC);
        bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D; //DSBCAPS_CTRLFX is not supported on primary buffer.
        bufferDesc.guid3DAlgorithm = GUID_NULL;

        hRet = g_pDSound8->CreateSoundBuffer(&bufferDesc, &g_pDSoundPrimaryBuffer, nullptr);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "Creating primary buffer for DirectSound Failed!");
        }

        /* Quote from MDSN "For the primary buffer, you must use the
         * IDirectSoundBuffer interface; IDirectSoundBuffer8 is not available."
         */
        // Return E_NOINTERFACE from QueryInterface method, make sense for g_pDSoundPrimaryBuffer
        // But how to set DSBCAPS_CTRLFX on primary buffer or should it be set for all current and future cache buffers?
        // We need LPDIRECTSOUNDFXI3DL2REVERB8 / IID_IDirectSoundFXI3DL2Reverb8 or use LPDIRECTSOUNDBUFFER8 / IID_IDirectSoundBuffer8

        hRet = g_pDSoundPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&g_pDSoundPrimary3DListener8);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "Creating primary 3D Listener for DirectSound Failed!");
        }

        initialized = true;
    }

    // This way we can be sure that this function returns a valid
    // DirectSound8 pointer even if we initialized it elsewhere!
    if (ppDirectSound != nullptr) {
        *ppDirectSound = g_pDSound8;
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSound_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSound_AddRef)
(
    LPDIRECTSOUND8          pThis)
{
    FUNC_EXPORTS;

    return 1;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = g_pDSound8->AddRef();

    leaveCriticalSection;

    return uRet;
}

// ******************************************************************
// * patch: IDirectSound_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSound_Release)
(
    LPDIRECTSOUND8          pThis)
{
    FUNC_EXPORTS;

    return 0;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = g_pDSound8->Release();
    if (uRet == 0) {
        g_bDSoundCreateCalled = false;
        g_pDSound8 = NULL;
    }

    leaveCriticalSection;

    return uRet;
}

// ******************************************************************
// * patch: CDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_GetSpeakerConfig)
(
    X_CDirectSound*         pThis,
    OUT PDWORD                  pdwSpeakerConfig)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwSpeakerConfig)
		LOG_FUNC_END;

    *pdwSpeakerConfig = 0; // STEREO

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8          pThis)
{
    FUNC_EXPORTS;

	LOG_FUNC_ONE_ARG(pThis);

    return XTL::EMUPATCH(CDirectSound_SynchPlayback)(pThis);
}

// ******************************************************************
// * patch: IDirectSound_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_DownloadEffectsImage)
(
    LPDIRECTSOUND8  pThis,
    LPCVOID         pvImageBuffer,
    DWORD           dwImageSize,
    PVOID           pImageLoc,      // TODO: Use this param
    PVOID*          ppImageDesc)    // TODO: Use this param
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pvImageBuffer)
		LOG_FUNC_ARG(dwImageSize)
		LOG_FUNC_ARG(pImageLoc)
		LOG_FUNC_ARG(ppImageDesc)
		LOG_FUNC_END;

    // This function is relative to DSP for Interactive 3-D Audio Level 2 (I3DL2)

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: DirectSoundDoWork
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(DirectSoundDoWork)()
{
    FUNC_EXPORTS;

    if (!g_bDSoundCreateCalled) {
        return;
    }

    enterCriticalSection;

	LOG_FUNC();

    xboxkrnl::LARGE_INTEGER getTime;
    xboxkrnl::KeQuerySystemTime(&getTime);

    vector_ds_buffer::iterator ppDSBuffer = g_pDSoundBufferCache.begin();
    for (; ppDSBuffer != g_pDSoundBufferCache.end(); ppDSBuffer++) {
        if ((*ppDSBuffer)->Host_lock.pLockPtr1 == nullptr || (*ppDSBuffer)->EmuBufferToggle != X_DSB_TOGGLE_DEFAULT) {
            continue;
        }
        XTL::X_CDirectSoundBuffer* pThis = *ppDSBuffer;
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
        if (pThis->Xb_rtPauseEx != 0 && pThis->Xb_rtPauseEx <= getTime.QuadPart) {
            pThis->Xb_rtPauseEx = 0LL;
            pThis->EmuFlags ^= DSE_FLAG_PAUSE;
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    // Actually, DirectSoundStream need to process buffer packets here.
    vector_ds_stream::iterator ppDSStream = g_pDSoundStreamCache.begin();
    for (; ppDSStream != g_pDSoundStreamCache.end(); ppDSStream++) {
        if ((*ppDSStream)->Host_BufferPacketArray.size() == 0) {
            continue;
        }
        X_CDirectSoundStream* pThis = (*ppDSStream);
        // TODO: Do we need this in async thread loop?
        if (pThis->Xb_rtPauseEx != 0 && pThis->Xb_rtPauseEx <= getTime.QuadPart) {
            pThis->Xb_rtPauseEx = 0LL;
            pThis->EmuFlags ^= DSE_FLAG_PAUSE;
            // Don't call play here, let DSoundStreamProcess deal with it.
        }
        if ((pThis->EmuFlags & DSE_FLAG_FLUSH_ASYNC) == 0) {
            DSoundStreamProcess(pThis);
        } else {
            // Confirmed flush packet must be done in DirectSoundDoWork only when title is ready.
            if (pThis->Xb_rtFlushEx != 0 && pThis->Xb_rtFlushEx <= getTime.QuadPart) {
                pThis->Xb_rtFlushEx = 0LL;
                DSoundStreamProcess(pThis);
            }
        }
    }

    leaveCriticalSection;

    return;
}
// For Async process purpose only
static void dsound_thread_worker(LPVOID nullPtr)
{
    while (true) {
        SwitchToThread();

        enterCriticalSection;

        vector_ds_stream::iterator ppDSStream = g_pDSoundStreamCache.begin();
        for (; ppDSStream != g_pDSoundStreamCache.end(); ppDSStream++) {
            if ((*ppDSStream)->Host_BufferPacketArray.size() == 0) {
                continue;
            }
            if (((*ppDSStream)->EmuFlags & DSE_FLAG_FLUSH_ASYNC) > 0 && (*ppDSStream)->Xb_rtFlushEx == 0) {
                DSoundStreamProcess((*ppDSStream));
            }
        }

        leaveCriticalSection;
    }
}

// Kismet given name for RadWolfie's experiment major issue in the mutt.
#define DirectSuicideWork XTL::EMUPATCH(DirectSoundDoWork)

// ******************************************************************
// * patch: IDirectSound_SetOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetOrientation)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           xFront,
    FLOAT           yFront,
    FLOAT           zFront,
    FLOAT           xTop,
    FLOAT           yTop,
    FLOAT           zTop,
    DWORD           dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(xFront)
		LOG_FUNC_ARG(yFront)
		LOG_FUNC_ARG(zFront)
		LOG_FUNC_ARG(xTop)
		LOG_FUNC_ARG(yTop)
		LOG_FUNC_ARG(zTop)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = DS_OK;
    // TODO: (DSound) Should we do restrictive or passive to return actual result back to titles?
    // Test case: Jet Set Radio Future, ?
    if (xFront == 0.0f && yFront == 0.0f && zFront == 0.0f) {
        printf("WARNING: SetOrientation was called with xFront = 0, yFront = 0, and zFront = 0. Current action is ignore call to PC.\n");
    }
    if (xTop == 0.0f && yTop == 0.0f && zTop == 0.0f) {
        printf("WARNING: SetOrientation was called with xTop = 0, yTop = 0, and zTop = 0. Current action is ignore call to PC.\n");
    } else {
        hRet = g_pDSoundPrimary3DListener8->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, dwApply);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSound_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetDistanceFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fDistanceFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DListener_SetDistanceFactor(g_pDSoundPrimary3DListener8, fDistanceFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSound_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetRolloffFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fRolloffFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DListener_SetRolloffFactor(g_pDSoundPrimary3DListener8, fRolloffFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSound_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetDopplerFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fDopplerFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, fDopplerFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSound_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    X_DSI3DL2LISTENER      *pds3dl,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pds3dl)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // This function is relative to DSP, although it needs SetFX from LPDIRECTSOUNDBUFFER8 or LPDIRECTSOUNDFXI3DL2REVERB8 class.

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetMixBinHeadroom)
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMixBinMask)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBins)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(dwMixBinMask)
        LOG_FUNC_ARG(alVolumes)
        LOG_FUNC_END;

    // NOTE: Use this function for XDK 3911 only because the implementation was changed
    // somewhere around the December 2001 (4134) update (or earlier, maybe).

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_8
// ******************************************************************
// This revision is only used in XDK 4039 and higher.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
(
    X_CDirectSoundBuffer*   pThis,
    X_LPDSMIXBINS           pMixBins)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetMixBinVolumes_8(pThis->EmuDirectSoundBuffer8, pMixBins, pThis->EmuFlags, pThis->Xb_Volume, pThis->Xb_VolumeMixbin, pThis->Xb_dwHeadroom);
}

// ******************************************************************
// * patch: IDirectSound_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetPosition)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetPosition(x, y, z, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSound_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetVelocity)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetVelocity(x, y, z, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSound_SetAllParameters
// ******************************************************************
// NOTE: No conversion requirement for XB to PC.
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPCDS3DLISTENER         pDS3DListenerParameters,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pDS3DListenerParameters)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetAllParameters(pDS3DListenerParameters, dwApply);

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: CDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_CommitDeferredSettings)
(
    X_CDirectSound*         pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    HRESULT hRet = g_pDSoundPrimary3DListener8->CommitDeferredSettings();

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*         pdsbd,
    OUT X_CDirectSoundBuffer**  ppBuffer)
{
    FUNC_EXPORTS;

    // Research reveal DirectSound creation check is part of the requirement.
    if (!g_pDSound8 && !g_bDSoundCreateCalled) {
        HRESULT hRet;

        hRet = XTL::EMUPATCH(DirectSoundCreate)(NULL, &g_pDSound8, NULL);
        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "Unable to initialize DirectSound!");
        }
    }

    enterCriticalSection;

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
            EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)", pdsbd->dwFlags & (~dwAcceptableMask));
        }

        // HACK: Hot fix for titles not giving CTRL3D flag. Xbox might accept it, however the host does not.
        if ((pdsbd->dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE) > 0 && (pdsbd->dwFlags & DSBCAPS_CTRL3D) == 0) {
            pdsbd->dwFlags &= ~DSBCAPS_MUTE3DATMAXDISTANCE;
        }

        DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        DSBufferDesc.dwFlags = (pdsbd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;

        // TODO: Garbage Collection
        *ppBuffer = new X_CDirectSoundBuffer();

        DSoundBufferSetDefault((*ppBuffer), 0);
        (*ppBuffer)->Host_lock = { 0 };

        DSoundBufferRegionSetDefault(*ppBuffer);

        // We have to set DSBufferDesc last due to EmuFlags must be either 0 or previously written value to preserve other flags.
        GeneratePCMFormat(DSBufferDesc, pdsbd->lpwfxFormat, (*ppBuffer)->EmuFlags, pdsbd->dwBufferBytes, &(*ppBuffer)->X_BufferCache, (*ppBuffer)->X_BufferCacheSize);
        (*ppBuffer)->EmuBufferDesc = DSBufferDesc;

        DbgPrintf(LOG_PREFIX, "DirectSoundCreateBuffer, *ppBuffer := 0x%08X, bytes := 0x%08X\n", *ppBuffer, (*ppBuffer)->EmuBufferDesc.dwBufferBytes);

        DSoundBufferCreate(&DSBufferDesc, (*ppBuffer)->EmuDirectSoundBuffer8);
        if (pdsbd->dwFlags & DSBCAPS_CTRL3D) {
            DSound3DBufferCreate((*ppBuffer)->EmuDirectSoundBuffer8, (*ppBuffer)->EmuDirectSound3DBuffer8);
            (*ppBuffer)->Xb_dwHeadroom = 0; // Default for 3D
        }

        DSoundDebugMuteFlag((*ppBuffer)->X_BufferCacheSize, (*ppBuffer)->EmuFlags);

        // Pre-set volume to enforce silence if one of audio codec is disabled.
        HybridDirectSoundBuffer_SetVolume((*ppBuffer)->EmuDirectSoundBuffer8, 0L, (*ppBuffer)->EmuFlags, nullptr, 
                                          (*ppBuffer)->Xb_VolumeMixbin, (*ppBuffer)->Xb_dwHeadroom);

        g_pDSoundBufferCache.push_back(*ppBuffer);
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC*         pdsbd,
    OUT X_CDirectSoundBuffer**  ppBuffer,
    LPUNKNOWN               pUnkOuter)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pdsbd)
		LOG_FUNC_ARG_OUT(ppBuffer)
		LOG_FUNC_ARG(pUnkOuter)
		LOG_FUNC_END;

    return EMUPATCH(DirectSoundCreateBuffer)(pdsbd, ppBuffer);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetBufferData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetBufferData)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pvBufferData)
        LOG_FUNC_ARG(dwBufferBytes)
        LOG_FUNC_END;


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

        leaveCriticalSection;
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
        DSoundBufferResizeSetSize(pThis, hRet, dwBufferBytes);

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
        DSoundBufferResizeSetSize(pThis, hRet, dwBufferBytes);

    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPlayRegion)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwPlayStart)
		LOG_FUNC_ARG(dwPlayLength)
		LOG_FUNC_END;

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

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
            DSoundBufferUpdate(pThis, pThis->EmuPlayFlags, hRet);
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Lock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Lock)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwOffset,
    DWORD                   dwBytes,
    LPVOID*                 ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID*                 ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    DWORD                   dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwOffset)
		LOG_FUNC_ARG(dwBytes)
		LOG_FUNC_ARG(ppvAudioPtr1)
		LOG_FUNC_ARG(pdwAudioBytes1)
		LOG_FUNC_ARG(ppvAudioPtr2)
		LOG_FUNC_ARG(pdwAudioBytes2)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

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
        CxbxKrnlCleanup(LOG_PREFIX, "DirectSoundBuffer Lock Failed!");
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
    } else if (ppvAudioPtr2 != xbnullptr) {
        *ppvAudioPtr2 = xbnullptr;
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Unlock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Unlock)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  ppvAudioPtr1,
    DWORD                   pdwAudioBytes1,
    LPVOID                  ppvAudioPtr2,
    DWORD                   pdwAudioBytes2
    )
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(ppvAudioPtr1)
        LOG_FUNC_ARG(pdwAudioBytes1)
        LOG_FUNC_ARG(ppvAudioPtr2)
        LOG_FUNC_ARG(pdwAudioBytes2)
        LOG_FUNC_END;

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

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetHeadroom)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwHeadroom)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetHeadroom(pThis->EmuDirectSoundBuffer8, dwHeadroom, pThis->Xb_dwHeadroom,
                                               pThis->Xb_Volume, pThis->Xb_VolumeMixbin, pThis->EmuFlags);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetLoopRegion)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwLoopStart)
		LOG_FUNC_ARG(dwLoopLength)
		LOG_FUNC_END;

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    pThis->EmuRegionLoopStartOffset = dwLoopStart;
    pThis->EmuRegionLoopLength = dwLoopLength;
    pThis->EmuBufferToggle = X_DSB_TOGGLE_LOOP;

    DWORD dwStatus;
    pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    HRESULT hRet;

    if ((dwStatus & DSBSTATUS_PLAYING) > 0) {
        if ((dwStatus & DSBSTATUS_LOOPING) > 0) {
            pThis->EmuDirectSoundBuffer8->Stop();
            DSoundBufferUpdate(pThis, pThis->EmuPlayFlags, hRet);
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Release)
(
    X_CDirectSoundBuffer*   pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = 0;

    //if (!(pThis->EmuFlags & DSE_FLAG_RECIEVEDATA)) {
        uRet = pThis->EmuDirectSoundBuffer8->Release();

        if (uRet == 0) {
            if (pThis->EmuDirectSound3DBuffer8 != nullptr) {
                pThis->EmuDirectSound3DBuffer8->Release();
            }

            // remove cache entry
            vector_ds_buffer::iterator ppDSBuffer = std::find(g_pDSoundBufferCache.begin(), g_pDSoundBufferCache.end(), pThis);
            if (ppDSBuffer != g_pDSoundBufferCache.end()) {
                g_pDSoundBufferCache.erase(ppDSBuffer);
            }

            if (pThis->EmuBufferDesc.lpwfxFormat != nullptr) {
                free(pThis->EmuBufferDesc.lpwfxFormat);
            }
            if (pThis->X_BufferCache != xbnullptr && (pThis->EmuFlags & DSE_FLAG_BUFFER_EXTERNAL) == 0) {
                free(pThis->X_BufferCache);
                DSoundSGEMemDealloc(pThis->X_BufferCacheSize);
            }

            delete pThis;
        }
    //}

    leaveCriticalSection;

    return uRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPitch)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lPitch)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lPitch)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetPitch(pThis->EmuDirectSoundBuffer8, lPitch);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    X_CDirectSoundBuffer*   pThis,
    OUT LPDWORD             pdwStatus)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

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

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetCurrentPosition)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwNewPosition)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwNewPosition)
		LOG_FUNC_END;

    dwNewPosition = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, dwNewPosition);

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if (hRet != DS_OK) {
        EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "SetCurrentPosition Failed!");
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    X_CDirectSoundBuffer*   pThis,
    OUT PDWORD                  pdwCurrentPlayCursor,
    OUT PDWORD                  pdwCurrentWriteCursor)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwCurrentPlayCursor)
		LOG_FUNC_ARG_OUT(pdwCurrentWriteCursor)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_GetCurrentPosition(pThis->EmuDirectSoundBuffer8, pdwCurrentPlayCursor, pdwCurrentWriteCursor, pThis->EmuFlags);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Play
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Play)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwReserved1)
		LOG_FUNC_ARG(dwReserved2)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    if (dwFlags & ~(X_DSBPLAY_LOOPING | X_DSBPLAY_FROMSTART | X_DSBPLAY_SYNCHPLAYBACK)) {
        CxbxKrnlCleanup(LOG_PREFIX, "Unsupported Playing Flags");
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

    DSoundBufferUpdate(pThis, dwFlags, hRet);

    if (hRet == DS_OK) {
        if (dwFlags & X_DSBPLAY_FROMSTART) {
            if (pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0) != DS_OK) {
                EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Rewinding buffer failed!");
            }
        }
        if ((pThis->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) == 0) {
            hRet = pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        }
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Stop
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Stop)
(
    X_CDirectSoundBuffer*   pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    HRESULT hRet = D3D_OK;

    if (pThis != nullptr) {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        hRet = pThis->EmuDirectSoundBuffer8->Stop();
        pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_StopEx)
(
    X_CDirectSoundBuffer*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(rtTimeStamp)
        LOG_FUNC_ARG(dwFlags)
        LOG_FUNC_END;

    HRESULT hRet = DS_OK;

    //TODO: RadWolfie - Rayman 3 crash at end of first intro for this issue...
    // if only return DS_OK, then it works fine until end of 2nd intro it crashed.

    // Do not allow to process - Xbox reject it.
    if (dwFlags > X_DSBSTOPEX_ALL) {
        hRet = DSERR_INVALIDPARAM;

    // Only flags can be process here.
    } else {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        if (dwFlags == X_DSBSTOPEX_IMMEDIATE) {
            hRet = pThis->EmuDirectSoundBuffer8->Stop();
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
        } else {
            bool isLooping;
            if ((pThis->EmuPlayFlags & X_DSBPLAY_LOOPING) > 0) {
                isLooping = true;
            } else {
                isLooping = false;
            }
            if ((dwFlags & X_DSBSTOPEX_ENVELOPE) > 0) {
                // TODO: How to mock up "release phase"?
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

                DSoundBufferResizeUpdate(pThis, pThis->EmuPlayFlags, hRet, 0, pThis->X_BufferCacheSize);

                dwValue += pThis->EmuRegionPlayStartOffset;
                if (isLooping) {
                    dwValue += pThis->EmuRegionLoopStartOffset;
                }

                pThis->EmuBufferToggle = X_DSB_TOGGLE_DEFAULT;
                pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwValue);
            }

            if (dwStatus & DSBSTATUS_PLAYING) {
                pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
            }
        }
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetVolume)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lVolume)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags, &pThis->Xb_Volume,
                                             pThis->Xb_VolumeMixbin, pThis->Xb_dwHeadroom);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFrequency)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwFrequency)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwFrequency)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency);
}

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    OUT X_CDirectSoundStream**  ppStream)
{
    FUNC_EXPORTS;

    // Research reveal DirectSound creation check is part of the requirement.
    if (!g_pDSound8 && !g_bDSoundCreateCalled) {
        HRESULT hRet;

        hRet = XTL::EMUPATCH(DirectSoundCreate)(NULL, &g_pDSound8, NULL);
        if (hRet != DS_OK) {
            CxbxKrnlCleanup(LOG_PREFIX, "Unable to initialize DirectSound!");
        }
    }

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdssd)
		LOG_FUNC_ARG_OUT(ppStream)
		LOG_FUNC_END;

    HRESULT hRet = DS_OK;

    //If out of space, return out of memory.
    if (X_DIRECTSOUND_CACHE_COUNT == X_DIRECTSOUND_CACHE_MAX) {

        hRet = DSERR_OUTOFMEMORY;
        *ppStream = xbnullptr;
    } else {
        // TODO: Garbage Collection
        *ppStream = new X_CDirectSoundStream();

        DSBUFFERDESC DSBufferDesc = { 0 };


        DWORD dwAcceptableMask = 0x00000010; // TODO: Note 0x00040000 is being ignored (DSSTREAMCAPS_LOCDEFER)

        if (pdssd->dwFlags & (~dwAcceptableMask)) {
            EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Use of unsupported pdssd->dwFlags mask(s) (0x%.08X)", pdssd->dwFlags & (~dwAcceptableMask));
        }
        DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);
        //DSBufferDesc->dwFlags = (pdssd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
        DSBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2; //aka DSBCAPS_DEFAULT + control position

        if ((pdssd->dwFlags & DSBCAPS_CTRL3D) > 0) {
            DSBufferDesc.dwFlags |= DSBCAPS_CTRL3D;
        } else {
            DSBufferDesc.dwFlags |= DSBCAPS_CTRLPAN;
        }

        DSoundBufferSetDefault((*ppStream), DSBPLAY_LOOPING);
        (*ppStream)->Xb_rtFlushEx = 0LL;

        // We have to set DSBufferDesc last due to EmuFlags must be either 0 or previously written value to preserve other flags.
        GeneratePCMFormat(DSBufferDesc, pdssd->lpwfxFormat, (*ppStream)->EmuFlags, 0, xbnullptr, (*ppStream)->X_BufferCacheSize);

        // Test case: Star Wars: KotOR has one packet greater than 5 seconds worth. Increasing to 10 seconds works out fine, can increase more if need to.
        // Allocate at least 10 second worth of bytes in PCM format.
        DSBufferDesc.dwBufferBytes = DSBufferDesc.lpwfxFormat->nAvgBytesPerSec * 10;
        (*ppStream)->EmuBufferDesc = DSBufferDesc;

        (*ppStream)->Host_dwTriggerRange = (DSBufferDesc.lpwfxFormat->nSamplesPerSec / DSBufferDesc.lpwfxFormat->wBitsPerSample);

        (*ppStream)->X_MaxAttachedPackets = pdssd->dwMaxAttachedPackets;
        (*ppStream)->Host_BufferPacketArray.reserve(pdssd->dwMaxAttachedPackets);
        (*ppStream)->Host_dwWriteOffsetNext = 0;
        (*ppStream)->Host_isProcessing = false;
        (*ppStream)->Xb_lpfnCallback = pdssd->lpfnCallback;
        (*ppStream)->Xb_lpvContext = pdssd->lpvContext;
        //TODO: Implement mixbin variable support. Or just merge pdssd struct into DS Stream class.

        DbgPrintf(LOG_PREFIX, "DirectSoundCreateStream, *ppStream := 0x%.08X\n", *ppStream);

        DSoundBufferCreate(&DSBufferDesc, (*ppStream)->EmuDirectSoundBuffer8);
        if (DSBufferDesc.dwFlags & DSBCAPS_CTRL3D) {
            DSound3DBufferCreate((*ppStream)->EmuDirectSoundBuffer8, (*ppStream)->EmuDirectSound3DBuffer8);
            (*ppStream)->Xb_dwHeadroom = 0; // Default for 3D
        }

        DSoundDebugMuteFlag((*ppStream)->EmuBufferDesc.dwBufferBytes, (*ppStream)->EmuFlags);

        // Pre-set volume to enforce silence if one of audio codec is disabled.
        HybridDirectSoundBuffer_SetVolume((*ppStream)->EmuDirectSoundBuffer8, 0L, (*ppStream)->EmuFlags, nullptr,
            (*ppStream)->Xb_VolumeMixbin, (*ppStream)->Xb_dwHeadroom);

        g_pDSoundStreamCache.push_back(*ppStream);
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_CreateSoundStream
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CreateSoundStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC*         pdssd,
    OUT X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pdssd)
		LOG_FUNC_ARG_OUT(ppStream)
		LOG_FUNC_ARG(pUnknown)
		LOG_FUNC_END;

    return EMUPATCH(DirectSoundCreateStream)(pdssd, ppStream);;
}

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(CMcpxStream_Dummy_0x10)(DWORD dwDummy1, DWORD dwDummy2)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(dwDummy1)
		LOG_FUNC_ARG(dwDummy2)
		LOG_FUNC_END;

    // Causes deadlock in Halo...
    // TODO: Verify that this is a Vista related problem (I HATE Vista!)
//    EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuCMcpxStream_Dummy_0x10 is ignored!");

    leaveCriticalSection;

    return;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lVolume)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags, &pThis->Xb_Volume,
                                             pThis->Xb_VolumeMixbin, pThis->Xb_dwHeadroom);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   fRolloffFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fRolloffFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetRolloffFactor is only supported for host primary buffer's 3D Listener.

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_AddRef)
(
    X_CDirectSoundStream*   pThis)
{
    FUNC_EXPORTS;

	LOG_FUNC_ONE_ARG(pThis);

    return HybridDirectSoundBuffer_AddRef(pThis->EmuDirectSoundBuffer8);
}

// ******************************************************************
// * patch: CDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_Release)
(
    X_CDirectSoundStream*   pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG uRet = 0;
    if (pThis != 0 && (pThis->EmuDirectSoundBuffer8 != 0)) {
        uRet = pThis->EmuDirectSoundBuffer8->Release();

        if (uRet == 0) {
            if (pThis->EmuDirectSound3DBuffer8 != NULL) {
                pThis->EmuDirectSound3DBuffer8->Release();
            }

            // remove cache entry
            vector_ds_stream::iterator ppDSStream = std::find(g_pDSoundStreamCache.begin(), g_pDSoundStreamCache.end(), pThis);
            if (ppDSStream != g_pDSoundStreamCache.end()) {
                g_pDSoundStreamCache.erase(ppDSStream);
            }

            for (auto buffer = pThis->Host_BufferPacketArray.begin(); buffer != pThis->Host_BufferPacketArray.end();) {
                DSoundStreamClearPacket(buffer, XMP_STATUS_RELEASE_CXBXR, nullptr, nullptr, pThis);
            }

            if (pThis->EmuBufferDesc.lpwfxFormat != nullptr) {
                free(pThis->EmuBufferDesc.lpwfxFormat);
            }
            // NOTE: Do not release X_BufferCache! X_BufferCache is using xbox buffer.

            delete pThis;
        }
    }

    leaveCriticalSection;

    return uRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetInfo
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_GetInfo)
(
    X_CDirectSoundStream*   pThis,
    OUT LPXMEDIAINFO            pInfo)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pInfo)
		LOG_FUNC_END;

    // TODO: A (real) implementation?
    EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuDirectSound_CDirectSoundStream_GetInfo is not yet supported!");

    if (pInfo) {
        pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
        pInfo->dwInputSize = 0x40000;
        pInfo->dwOutputSize = 0x40000;
        pInfo->dwMaxLookahead = 0x4000;
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_GetStatus)
(
    X_CDirectSoundStream*   pThis,
    OUT DWORD*              pdwStatus)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pdwStatus)
        LOG_FUNC_END;

    DWORD dwStatusXbox = 0, dwStatusHost;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatusHost);

    // Convert host to xbox status flag.
    if (hRet == DS_OK) {
        DWORD testSize = pThis->Host_BufferPacketArray.size();
        if ((dwStatusHost & DSBSTATUS_PLAYING) > 0) {
            dwStatusXbox |= X_DSSSTATUS_PLAYING;

        } else {

            if ((pThis->EmuFlags & DSE_FLAG_PAUSE) > 0) {
                dwStatusXbox |= X_DSSSTATUS_PAUSED;

            // Set to paused when has packet(s) queued and is not processing.
            } else if (pThis->Host_BufferPacketArray.size() != 0 && pThis->Host_isProcessing == false) {
                dwStatusXbox |= X_DSSSTATUS_PAUSED;
            }

            if (pThis->Host_BufferPacketArray.size() == 0) {
                dwStatusXbox |= X_DSSSTATUS_STARVED;

                if ((pThis->EmuFlags & DSE_FLAG_ENVELOPE2) > 0) {
                    dwStatusXbox |= X_DSSSTATUS_ENVELOPECOMPLETE;
                }
            }
        }
        if (pThis->Host_BufferPacketArray.size() != pThis->X_MaxAttachedPackets) {
            dwStatusXbox |= X_DSSSTATUS_READY;
        }
        *pdwStatus = dwStatusXbox;

    } else if (pdwStatus != xbnullptr) {
        *pdwStatus = 0;
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream   *pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pInputBuffer)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // Research data:
    // * Max packet size permitted is 0x2000 (or 8,192 decimal) of buffer.
    //   * Somehow other titles are using more than 0x2000 for max size. Am using a hacky host method for now (see pBuffer_data).

    if (pThis->EmuDirectSoundBuffer8 != nullptr) {

        if (pInputBuffer != xbnullptr) {

            // Add packets from title until it gets full.
            if (pThis->Host_BufferPacketArray.size() != pThis->X_MaxAttachedPackets) {
                host_voice_packet packet_input;
                packet_input.xmp_data = *pInputBuffer;
                packet_input.xmp_data.dwMaxSize = DSoundBufferGetPCMBufferSize(pThis->EmuFlags, pInputBuffer->dwMaxSize);
                if (packet_input.xmp_data.dwMaxSize == 0) {
                    packet_input.pBuffer_data = nullptr;
                } else {
                    packet_input.pBuffer_data = malloc(packet_input.xmp_data.dwMaxSize);
                    DSoundSGEMemAlloc(packet_input.xmp_data.dwMaxSize);
                }
                packet_input.rangeStart = pThis->Host_dwWriteOffsetNext;
                pThis->Host_dwWriteOffsetNext += packet_input.xmp_data.dwMaxSize;
                if (pThis->EmuBufferDesc.dwBufferBytes <= pThis->Host_dwWriteOffsetNext) {
                    pThis->Host_dwWriteOffsetNext -= pThis->EmuBufferDesc.dwBufferBytes;
                }
                packet_input.isWritten = false;
                packet_input.isPlayed = false;

                DSoundBufferOutputXBtoHost(pThis->EmuFlags, pThis->EmuBufferDesc, pInputBuffer->pvBuffer, pInputBuffer->dwMaxSize, packet_input.pBuffer_data, packet_input.xmp_data.dwMaxSize);
                pThis->Host_BufferPacketArray.push_back(packet_input);

                if (pInputBuffer->pdwStatus != xbnullptr) {
                    (*pInputBuffer->pdwStatus) = XMP_STATUS_PENDING;
                }
                if (pInputBuffer->pdwCompletedSize != xbnullptr) {
                    (*pInputBuffer->pdwCompletedSize) = 0;
                }
                if (pThis->Host_isProcessing == false && pThis->Host_BufferPacketArray.size() == 1) {
                    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(packet_input.rangeStart);
                }
            // Once full it needs to change status to flushed when cannot hold any more packets.
            } else {
                if (pInputBuffer->pdwStatus != xbnullptr) {
                    (*pInputBuffer->pdwStatus) = XMP_STATUS_FLUSHED;
                }
            }
        }

        //TODO: What to do with output buffer audio variable? Need test case or functional source code.
        // NOTE: pOutputBuffer is reserved, must be set to NULL from titles.
        if (pOutputBuffer != xbnullptr) {
            LOG_TEST_CASE("pOutputBuffer is not nullptr, please report title test case to issue tracker. Thanks!");
        }

    } else {
        if (pInputBuffer != xbnullptr && pInputBuffer->pdwStatus != xbnullptr) {
            (*pInputBuffer->pdwStatus) = XMP_STATUS_SUCCESS;
        }
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Discontinuity)
(
    X_CDirectSoundStream*   pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    // NOTE: Perform check if has pending data. if so, stop stream.
    // default ret = DSERR_GENERIC

    pThis->EmuDirectSoundBuffer8->Stop();
    pThis->Host_isProcessing = false;

    // NOTE: Must reset flags in discontinuity and rtTimeStamps.
    pThis->EmuFlags &= ~(DSE_FLAG_PAUSE | DSE_FLAG_FLUSH_ASYNC | DSE_FLAG_ENVELOPE | DSE_FLAG_ENVELOPE2);
    DSoundBufferSynchPlaybackFlagRemove(pThis->EmuFlags);
    pThis->Xb_rtFlushEx = 0LL;
    pThis->Xb_rtPauseEx = 0LL;

    for (auto buffer = pThis->Host_BufferPacketArray.begin(); buffer != pThis->Host_BufferPacketArray.end();) {
        DSoundStreamClearPacket(buffer, XMP_STATUS_FLUSHED, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Flush)
(
    X_CDirectSoundStream*   pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    DSoundBufferSynchPlaybackFlagRemove(pThis->EmuFlags);

    // Remove flags only (This is the only place it will remove other than FlushEx perform set/remove the flags.)
    pThis->EmuFlags &= ~(DSE_FLAG_FLUSH_ASYNC | DSE_FLAG_ENVELOPE | DSE_FLAG_ENVELOPE2);
    pThis->Xb_rtFlushEx = 0LL;

    while (DSoundStreamProcess(pThis));

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_SynchPlayback)
(
    LPDIRECTSOUND8 pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    //TODO: Test case Rayman 3 - Hoodlum Havoc, Battlestar Galactica, Miami Vice, and ...?

    vector_ds_buffer::iterator ppDSBuffer = g_pDSoundBufferCache.begin();
    for (; ppDSBuffer != g_pDSoundBufferCache.end(); ppDSBuffer++) {
        if ((*ppDSBuffer)->X_BufferCache == nullptr) {
            continue;
        }

        if (((*ppDSBuffer)->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {
            DSoundBufferSynchPlaybackFlagRemove((*ppDSBuffer)->EmuFlags);
            (*ppDSBuffer)->EmuDirectSoundBuffer8->Play(0, 0, (*ppDSBuffer)->EmuPlayFlags);
        }
    }

    vector_ds_stream::iterator ppDSStream = g_pDSoundStreamCache.begin();
    for (; ppDSStream != g_pDSoundStreamCache.end(); ppDSStream++) {
        if ((*ppDSStream)->Host_BufferPacketArray.size() == 0) {
            continue;
        }
        if (((*ppDSStream)->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {
            DSoundBufferSynchPlaybackFlagRemove((*ppDSStream)->EmuFlags);
            DSoundStreamProcess((*ppDSStream));
        }
    }

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Pause)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwPause)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwPause)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
                                        pThis->Host_isProcessing, 0LL, pThis->Xb_rtPauseEx);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwHeadroom)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetHeadroom(pThis->EmuDirectSoundBuffer8, dwHeadroom, pThis->Xb_dwHeadroom,
                                               pThis->Xb_Volume, pThis->Xb_VolumeMixbin, pThis->EmuFlags);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwInsideConeAngle)
		LOG_FUNC_ARG(dwOutsideConeAngle)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeAngles(pThis->EmuDirectSound3DBuffer8, dwInsideConeAngle, dwOutsideConeAngle, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lConeOutsideVolume)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeOutsideVolume(pThis->EmuDirectSound3DBuffer8, lConeOutsideVolume, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    X_DS3DBUFFER*           pc3DBufferParameters,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pc3DBufferParameters)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetAllParameters(pThis->EmuDirectSound3DBuffer8, pc3DBufferParameters, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                flMaxDistance,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flMaxDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMaxDistance(pThis->EmuDirectSound3DBuffer8, flMaxDistance, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                fMinDistance,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fMinDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMinDistance(pThis->EmuDirectSound3DBuffer8, fMinDistance, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetVelocity)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetVelocity(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeOrientation(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetPosition)
(
    X_CDirectSoundStream*   pThis,
    D3DVALUE                x,
    D3DVALUE                y,
    D3DVALUE                z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetPosition(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwFrequency)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwFrequency)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    PVOID                   pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return S_OK;
}

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMaxDistance)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flMaxDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMaxDistance(pThis->EmuDirectSound3DBuffer8, flMaxDistance, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMinDistance)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flMinDistance,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flMinDistance)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMinDistance(pThis->EmuDirectSound3DBuffer8, flMinDistance, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flRolloffFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flRolloffFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetRolloffFactor is only supported for host primary buffer's 3D Listener.

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetDistanceFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flDistanceFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    leaveCriticalSection;

    return HybridDirectSound3DListener_SetDistanceFactor(g_pDSoundPrimary3DListener8, flDistanceFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeAngles)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwInsideConeAngle)
		LOG_FUNC_ARG(dwOutsideConeAngle)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeAngles(pThis->EmuDirectSound3DBuffer8, dwInsideConeAngle, dwOutsideConeAngle, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeOrientation)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeOrientation(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetConeOutsideVolume)
(
    X_CDirectSoundBuffer*   pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lConeOutsideVolume)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetConeOutsideVolume(pThis->EmuDirectSound3DBuffer8, lConeOutsideVolume, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetPosition)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetPosition(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetVelocity)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(x)
		LOG_FUNC_ARG(y)
		LOG_FUNC_ARG(z)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetVelocity(pThis->EmuDirectSound3DBuffer8, x, y, z, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetDopplerFactor)
(
    X_CDirectSoundBuffer*   pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flDopplerFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, flDopplerFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    X_CDirectSoundBuffer*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pds3db)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetI3DL2Source is using DSFXI3DL2Reverb structure, aka different interface.

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMode)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwMode,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMode)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMode(pThis->EmuDirectSound3DBuffer8, dwMode, dwApply);
}

// +s
// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFormat)
(
    X_CDirectSoundBuffer*   pThis,
    LPCWAVEFORMATEX         pwfxFormat)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pwfxFormat)
		LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, 
                                                     pThis->EmuBufferDesc, pThis->EmuFlags, 
                                                     pThis->EmuPlayFlags, pThis->EmuDirectSound3DBuffer8,
                                                     0, pThis->X_BufferCache, pThis->X_BufferCacheSize);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: DirectSoundUseFullHRTF
// ******************************************************************
void WINAPI XTL::EMUPATCH(DirectSoundUseFullHRTF)
(
    void)
{
    FUNC_EXPORTS;

    //NOTE: enter/leave criticalsection is not required! Titles are calling it before DirectSoundCreate.

	LOG_FUNC();

    LOG_IGNORED();
}

// ******************************************************************
// * patch: DirectSoundUseLightHRTF
// ******************************************************************
void WINAPI XTL::EMUPATCH(DirectSoundUseLightHRTF)
(
    void)
{
    FUNC_EXPORTS;

    //NOTE: enter/leave criticalsection is not required! Titles are calling it before DirectSoundCreate.

	LOG_FUNC();

    LOG_IGNORED();
}

// ******************************************************************
// * patch: DirectSoundUseFullHRTF4Channel
// ******************************************************************
void WINAPI XTL::EMUPATCH(DirectSoundUseFullHRTF4Channel)
(
    void)
{
    FUNC_EXPORTS;

    //NOTE: enter/leave criticalsection is not required! Titles are calling it before DirectSoundCreate.

	LOG_FUNC();

    LOG_IGNORED();
}

// ******************************************************************
// * patch: DirectSoundUseLightHRTF4Channel
// ******************************************************************
void WINAPI XTL::EMUPATCH(DirectSoundUseLightHRTF4Channel)
(
    void)
{
    FUNC_EXPORTS;

    //NOTE: enter/leave criticalsection is not required! Titles are calling it before DirectSoundCreate.

	LOG_FUNC();

    LOG_IGNORED();
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetLFO
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetLFO) //Low Frequency Oscillators
(
    LPDIRECTSOUNDBUFFER8    pThis,
    LPCDSLFODESC            pLFODesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pLFODesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetLFO
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pLFODesc)
        LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XAudioCreateAdpcmFormat)
(
    WORD                   nChannels,
    DWORD                  nSamplesPerSec,
    LPXBOXADPCMWAVEFORMAT  pwfx)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(nChannels)
		LOG_FUNC_ARG(nSamplesPerSec)
		LOG_FUNC_ARG(pwfx)
		LOG_FUNC_END;

    // Fill out the pwfx structure with the appropriate data
    pwfx->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.nAvgBytesPerSec = (nSamplesPerSec*nChannels * 36) / 64;
    pwfx->wfx.nBlockAlign = nChannels * 36;
    pwfx->wfx.wBitsPerSample = 4;
    pwfx->wfx.cbSize = 2;
    pwfx->wSamplesPerBlock = 64;

    leaveCriticalSection;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetRolloffCurve
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetRolloffCurve)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    const FLOAT*            pflPoints,
    DWORD                   dwPointCount,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pflPoints)
		LOG_FUNC_ARG(dwPointCount)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: Individual 3D buffer function.

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}


// ******************************************************************
// * patch: IDirectSound_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_EnableHeadphones)
(
    LPDIRECTSOUND8      pThis,
    BOOL                fEnabled)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(fEnabled)
		LOG_FUNC_END;

    //Windows Vista and later does not set speaker configuration from SetSpeakerConfig function.
    EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuIDirectSound_EnableHeadphones ignored");

	leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer_AddRef)
(
    X_CDirectSoundBuffer*   pThis)
{
    FUNC_EXPORTS;

	LOG_FUNC_ONE_ARG(pThis);

    return HybridDirectSoundBuffer_AddRef(pThis->EmuDirectSoundBuffer8);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Pause
// ******************************************************************
// Introduced in XDK 4721 revision.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Pause)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwPause)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwPause)
		LOG_FUNC_END;

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->Host_lock,
                        pThis->X_BufferCache,
                        pThis->X_lock.dwLockOffset,
                        pThis->X_lock.dwLockBytes1,
                        pThis->X_lock.dwLockBytes2);

    return HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
                                         1, 0LL, pThis->Xb_rtPauseEx);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
// Introduced in XDK 4721 revision.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_PauseEx)
(
    X_CDirectSoundBuffer   *pThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause)
{
      FUNC_EXPORTS;

        enterCriticalSection;

        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pThis)
            LOG_FUNC_ARG(rtTimestamp)
            LOG_FUNC_ARG(dwPause)
            LOG_FUNC_END;

    HRESULT hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags,
                                                 1, rtTimestamp, pThis->Xb_rtPauseEx);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_GetOutputLevels
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetOutputLevels)
(
    LPDIRECTSOUND8*         pThis,
    OUT X_DSOUTPUTLEVELS*       pOutputLevels,
    BOOL                    bResetPeakValues)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pOutputLevels)
		LOG_FUNC_ARG(bResetPeakValues)
		LOG_FUNC_END;

    // TODO: Anything?  Either way, I've never seen a game to date use this...

    // NOTE: It ask for access to real time output (require capture device I believe).

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    LPVOID                  pEnvelopeDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pEnvelopeDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_FlushEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(rtTimeStamp)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    HRESULT hRet = DSERR_INVALIDPARAM;

    // Cannot use rtTimeStamp here, it must be flush.
    if (dwFlags == X_DSSFLUSHEX_IMMEDIATE) {

        hRet = XTL::EMUPATCH(CDirectSoundStream_Flush)(pThis);

    // Remaining flags require X_DSSFLUSHEX_ASYNC to be include.
    } else if ((dwFlags & X_DSSFLUSHEX_ASYNC) > 0) {

        pThis->EmuFlags |= DSE_FLAG_FLUSH_ASYNC;
        pThis->Xb_rtFlushEx = rtTimeStamp;

        // Set or remove flags (This is the only place it will set/remove other than Flush perform remove the flags.)
        if ((dwFlags & X_DSSFLUSHEX_ENVELOPE) > 0) {
            pThis->EmuFlags |= DSE_FLAG_ENVELOPE;
        } else {
            pThis->EmuFlags ^= DSE_FLAG_ENVELOPE;
        }
        if ((dwFlags & X_DSSFLUSHEX_ENVELOPE2) > 0) {
            pThis->EmuFlags |= DSE_FLAG_ENVELOPE2;
        } else {
            pThis->EmuFlags ^= DSE_FLAG_ENVELOPE2;
        }
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwMode,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMode)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetMode(pThis->EmuDirectSound3DBuffer8, dwMode, dwApply);
}

// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR      pszImageName,
    LPVOID      pImageLoc,
    DWORD       dwFlags,
    LPVOID*     ppImageDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pszImageName)
		LOG_FUNC_ARG(pImageLoc)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(ppImageDesc)
		LOG_FUNC_END;

	LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetFilter)
(
    LPVOID              pThis,
    X_DSFILTERDESC*     pFilterDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pFilterDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pFilterDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}


// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    X_CDirectSoundBuffer* pThis,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(rtTimeStamp)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_END;

    //TODO: Need implement support for rtTimeStamp.
    if (rtTimeStamp != 0) {
        EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Not implemented for rtTimeStamp greater than 0 of %08d", rtTimeStamp);
    }

    HRESULT hRet = XTL::EMUPATCH(IDirectSoundBuffer_Play)(pThis, NULL, NULL, dwFlags);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_GetCaps
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetCaps)
(
    X_CDirectSound*     pThis,
    OUT X_DSCAPS*           pDSCaps)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pDSCaps)
		LOG_FUNC_END;

    // Convert PC -> Xbox
    if (pDSCaps) {
        pDSCaps->dwFreeBufferSGEs = DSoundSGEFreeBuffer();
        // To prevent pass down overflow size.
        pDSCaps->dwMemoryAllocated = (X_DS_SGE_SIZE_MAX < g_dwXbMemAllocated ? X_DS_SGE_SIZE_MAX : g_dwXbMemAllocated);

        // TODO: What are the max values for 2D and 3D Buffers? Once discover, then perform real time update in global variable.
        pDSCaps->dwFree2DBuffers = (pDSCaps->dwFreeBufferSGEs == 0 ? 0 : 0x200 /* TODO: Replace me to g_dwFree2DBuffers*/ );
        pDSCaps->dwFree3DBuffers = (pDSCaps->dwFreeBufferSGEs == 0 ? 0 : 0x200 /* TODO: Replace me to g_dwFree3DBuffers*/ );

        DbgPrintf(LOG_PREFIX, "X_DSCAPS: dwFree2DBuffers = %8X | dwFree3DBuffers = %8X | dwFreeBufferSGEs = %08X | dwMemAlloc = %08X\n", pDSCaps->dwFree2DBuffers, pDSCaps->dwFree3DBuffers, pDSCaps->dwFreeBufferSGEs, pDSCaps->dwMemoryAllocated);
    }

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lPitch)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(lPitch)
        LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetPitch(pThis->EmuDirectSoundBuffer8, lPitch);
}

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(DirectSoundGetSampleTime)()
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC();

	DWORD dwRet = GetAPUTime();

    leaveCriticalSection;

    return dwRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_12)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwMixBinMask)
		LOG_FUNC_ARG(alVolumes)
		LOG_FUNC_END;

    // NOTE: Use this function for XDK 3911 only because the implementation was changed
    // somewhere around the March 2002 (4361) update (or earlier, maybe).

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
// This revision API is only used in XDK 4039 and higher.
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*   pThis,
    X_LPDSMIXBINS           pMixBins)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetMixBinVolumes_8(pThis->EmuDirectSoundBuffer8, pMixBins, pThis->EmuFlags, pThis->Xb_Volume, pThis->Xb_VolumeMixbin, pThis->Xb_dwHeadroom);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pds3db)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: SetI3DL2Source is using DSFXI3DL2Reverb structure, aka different interface.

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetAllParameters)
(
    X_CDirectSoundBuffer*    pThis,
    X_DS3DBUFFER*            pc3DBufferParameters,
    DWORD                    dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pc3DBufferParameters)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DBuffer_SetAllParameters(pThis->EmuDirectSound3DBuffer8, pc3DBufferParameters, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFormat)
(
    X_CDirectSoundStream*   pThis,
    LPCWAVEFORMATEX         pwfxFormat)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pwfxFormat)
		LOG_FUNC_END;

    pThis->Host_isProcessing = false;
    pThis->EmuDirectSoundBuffer8->Stop();

    for (auto buffer = pThis->Host_BufferPacketArray.begin(); buffer != pThis->Host_BufferPacketArray.end();) {
        // TODO: Also need to pass down callback and context as well?
        DSoundStreamClearPacket(buffer, XMP_STATUS_FLUSHED, nullptr, nullptr, pThis);
    }

    HRESULT hRet = HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->EmuBufferDesc,
                                             pThis->EmuFlags, pThis->EmuPlayFlags, pThis->EmuDirectSound3DBuffer8,
                                             0, pThis->X_BufferCache, pThis->X_BufferCacheSize);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetOutputBuffer)
(
    X_CDirectSoundBuffer*   pThis,
    X_CDirectSoundBuffer*   pOutputBuffer)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // NOTE: SetOutputBuffer is not possible in PC's DirectSound due to 3D controller requirement on ouput buffer to work simultaneously.
    // Test case: MultiPass sample
    // Best to emulate this LLE instead of HLE.

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
    X_CDirectSoundStream*   pThis,
    X_CDirectSoundBuffer*   pOutputBuffer)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    // NOTE: SetOutputBuffer is not possible in PC's DirectSound due to 3D controller requirement on ouput buffer to work simultaneously.
    // Test case: Red Faction 2
    // Best to emulate this LLE instead of HLE.

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XFileCreaeMediaObjectEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObjectEx)
(
    HANDLE      hFile,
    OUT void**      ppMediaObject)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hFile)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
// NOTE: Does not require any patch.
HRESULT WINAPI XTL::EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR                  pszFileName,
    LPCWAVEFORMATEX*        ppwfxFormat,
    OUT void**              ppMediaObject) //XFileMediaObject, include XMediaObject interface
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pszFileName)
		LOG_FUNC_ARG(ppwfxFormat)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetEG)
(
    X_CDirectSoundBuffer*   pThis,
    LPVOID                  pEnvelopeDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pEnvelopeDesc)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound_GetEffectData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetEffectData)
(
    X_CDirectSound* pThis,
    DWORD           dwEffectIndex,
    DWORD           dwOffset,
    OUT LPVOID          pvData,
    DWORD           dwDataSize)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwEffectIndex)
		LOG_FUNC_ARG(dwOffset)
		LOG_FUNC_ARG_OUT(pvData)
		LOG_FUNC_ARG(dwDataSize)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    /* RadWolfie: Should not allocate memory, xbox xbe is just asking for input data, not allocate then input data...
    if (!pvData) {
        pvData = malloc(dwDataSize); // TODO : FIXME : Shouldn't this be : *pvData = ...  ?!?
    }*/

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetNotificationPositions)
(
    X_CDirectSoundBuffer*   pThis,
    DWORD                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwNotifyCount)
		LOG_FUNC_ARG(paNotifies)
		LOG_FUNC_END;

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
                    EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Could not set notification position(s)!");
                }

                pNotify->Release();
            } else {
                EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Could not create notification interface!");
            }
        }
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
}

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
    X_CDirectSoundBuffer*   pThis,
    const FLOAT*            pflPoints,
    DWORD                   dwPointCount,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pflPoints)
		LOG_FUNC_ARG(dwPointCount)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound_SetEffectData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetEffectData)
(
    LPVOID  pThis,
    DWORD   dwEffectIndex,
    DWORD   dwOffset,
    LPCVOID pvData,
    DWORD   dwDataSize,
    DWORD   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwEffectIndex)
		LOG_FUNC_ARG(dwOffset)
		LOG_FUNC_ARG(pvData)
		LOG_FUNC_ARG(dwDataSize)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    // NOTE: DSP relative function

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_Use3DVoiceData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_Use3DVoiceData)
(
    LPVOID      pThis,
    LPUNKNOWN   pUnknown)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pUnknown)
		LOG_FUNC_END;

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE      hFile,
    DWORD       dwMaxPackets,
    OUT void**      ppMediaObject)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG(dwMaxPackets)
        LOG_FUNC_ARG_OUT(ppMediaObject)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    *ppMediaObject = new X_XFileMediaObject();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    LONG                lOffset,
    DWORD               dwOrigin,
    LPDWORD             pdwAbsolute)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lOffset)
		LOG_FUNC_ARG(dwOrigin)
		LOG_FUNC_ARG(pdwAbsolute)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
// NOTE: Does not require a patch.
VOID WINAPI XTL::EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;
}

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject* pThis,
    OUT LPDWORD             pdwStatus)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetInfo)
(
    X_XFileMediaObject* pThis,
    OUT XMEDIAINFO*         pInfo)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pInfo)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Process)
(
    X_XFileMediaObject* pThis,
    LPXMEDIAPACKET      pInputBuffer,
    LPXMEDIAPACKET      pOutputBuffer)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pInputBuffer)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
// NOTE: Does not require a patch.
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_AddRef)
(
	X_XFileMediaObject* pThis)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG Ret = 0;

    if (pThis) { // TODO : this should be an assert()
        Ret = ++(pThis->EmuRefCount);
    }

    leaveCriticalSection;

    return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Release
// ******************************************************************
// NOTE: Does not require a patch.
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_Release)
(
	X_XFileMediaObject* pThis)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG Ret = 0;

    if (pThis) { // TODO : this should be an assert()
        Ret = --(pThis->EmuRefCount);
        if (Ret == 0) {
            delete pThis;
        }
    }

    leaveCriticalSection;

    return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Discontinuity
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Discontinuity)
(
	X_XFileMediaObject *pThis)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

	LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetSpeakerConfig)
(
    X_CDirectSound*     pThis,
    OUT LPDWORD*        pdwSpeakerConfig)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pdwSpeakerConfig)
    LOG_FUNC_END;

    //For now, let's set it to stereo.
    *pdwSpeakerConfig = X_DSSPEAKER_STEREO;

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CommitDeferredSettings)
(
    X_CDirectSound*     pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_ONE_ARG(pThis);

    HRESULT hRet = DS_OK;
    if (g_pDSoundPrimary3DListener8 != nullptr) {
        hRet = g_pDSoundPrimary3DListener8->CommitDeferredSettings();
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_CommitEffectData
// ******************************************************************
// This API is used relative with DSP effect.
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CommitEffectData)
(
    X_CDirectSound*     pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_ONE_ARG(pThis);

    // NOTE: DSP relative function.

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_PauseEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause)
{
      FUNC_EXPORTS;

        enterCriticalSection;

        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pThis)
            LOG_FUNC_ARG(rtTimestamp)
            LOG_FUNC_ARG(dwPause)
            LOG_FUNC_END;

    // This function wasn't part of the XDK until 4721. (Same as IDirectSoundBuffer_PauseEx?)
    // TODO: Implement time stamp feature (a thread maybe?)

    HRESULT hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags, pThis->EmuPlayFlags, 
                                                pThis->Host_isProcessing, rtTimestamp, pThis->Xb_rtPauseEx);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: XFileCreaeMediaObject
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObject)
(
    LPCSTR          pstrFileName,
    DWORD           dwDesiredAccess,
    DWORD           dwShareMode,
    DWORD           dwCreationDisposition,
    DWORD           dwFlagsAndAttributes,
    OUT void**      ppMediaObject)
{
    //FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pstrFileName)
		LOG_FUNC_ARG(dwDesiredAccess)
		LOG_FUNC_ARG(dwShareMode)
		LOG_FUNC_ARG(dwCreationDisposition)
		LOG_FUNC_ARG(dwFlagsAndAttributes)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: XWaveFileCreateMediaObjectEx
// ******************************************************************
// NOTE: Does not require a patch.
HRESULT WINAPI XTL::EMUPATCH(XWaveFileCreateMediaObjectEx)
(
    LPCSTR              pszFileName,
    HANDLE              hFile,
    OUT void**          ppMediaObject) //XWaveFileMediaObject, include XFileMediaObject and XMediaObject interfaces
{
    //FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pszFileName)
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG_OUT(ppMediaObject)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: XAudioSetEffectData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XAudioSetEffectData)
(
    DWORD               dwEffectIndex,
    void*               pDesc,
    void*               pRawDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(dwEffectIndex)
        LOG_FUNC_ARG(pDesc)
        LOG_FUNC_ARG(pRawDesc)
        LOG_FUNC_END;

    LOG_NOT_SUPPORTED();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetDistanceFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(flDistanceFactor)
		LOG_FUNC_ARG(dwApply)
		LOG_FUNC_END;

    return HybridDirectSound3DListener_SetDistanceFactor(g_pDSoundPrimary3DListener8, flDistanceFactor, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetDopplerFactor)
(
    X_CDirectSoundStream*   pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(flDopplerFactor)
        LOG_FUNC_ARG(dwApply)
        LOG_FUNC_END;

    return HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, flDopplerFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetVoiceProperties)
(
    X_CDirectSoundBuffer*   pThis,
    OUT void*               pVoiceProps)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pVoiceProps)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetVoiceProperties
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_GetVoiceProperties)
(
    X_CDirectSoundStream*   pThis,
    OUT void*               pVoiceProps)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pVoiceProps)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lVolume)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(lVolume)
        LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags, &pThis->Xb_Volume,
                                             pThis->Xb_VolumeMixbin, pThis->Xb_dwHeadroom);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetPitch)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lPitch)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(lPitch)
        LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetPitch(pThis->EmuDirectSoundBuffer8, lPitch);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetLFO
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetLFO)
(
    X_CDirectSoundStream*   pThis,
    LPCDSLFODESC            pLFODesc) {
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pLFODesc)
        LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_SetLFO)(pThis, pLFODesc);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetEG)
(
    X_CDirectSoundStream*   pThis,
    LPVOID                  pEnvelopeDesc)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pEnvelopeDesc)
        LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_SetEG)(pThis, pEnvelopeDesc);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetFilter)
(
    X_CDirectSoundStream*   pThis,
    X_DSFILTERDESC*         pFilterDesc)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pFilterDesc)
        LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_SetFilter)(pThis, pFilterDesc);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwHeadroom)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(dwHeadroom)
        LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_SetHeadroom)(pThis, dwHeadroom);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetFrequency)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwFrequency)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(dwFrequency)
        LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetMixBins)
(
    X_CDirectSoundStream*   pThis,
    PVOID                   pMixBins)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pMixBins)
        LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_SetMixBins)(pThis, pMixBins);
}
