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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl {
    #include <xboxkrnl/xboxkrnl.h>
};

#include <dsound.h>
#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"
#include "MemoryManager.h"
#include "Logging.h"

#include <mmreg.h>
#include <msacm.h>
#include <process.h>
#include <clocale>

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



// size of sound buffer cache (used for periodic sound buffer updates)
#define SOUNDBUFFER_CACHE_SIZE 0x200

// size of sound stream cache (used for periodic sound stream updates)
#define SOUNDSTREAM_CACHE_SIZE 0x200

//Currently disabled since below may not be needed since under -6,400 is just silence yet accepting up to -10,000
// Xbox to PC volume ratio format (-10,000 / -6,400 )
//#define XBOX_TO_PC_VOLUME_RATIO 1.5625

// Xbox maximum synch playback audio
#define DSOUND_MAX_SYNCHPLAYBACK_AUDIO 29

// Static Variable(s)
XBAudio                             g_XBAudio = XBAudio();
extern LPDIRECTSOUND8               g_pDSound8 = NULL; //This is necessary in order to allow share with EmuDSoundInline.hpp
static LPDIRECTSOUNDBUFFER          g_pDSoundPrimaryBuffer = NULL;
//TODO: RadWolfie - How to implement support if primary does not permit it for DSP usage?
static LPDIRECTSOUNDBUFFER8         g_pDSoundPrimaryBuffer8 = NULL;
static LPDIRECTSOUND3DLISTENER8     g_pDSoundPrimary3DListener8 = NULL;
static XTL::X_CDirectSoundBuffer*   g_pDSoundBufferCache[SOUNDBUFFER_CACHE_SIZE] = { 0 }; //Default initialize to all zero'd
static XTL::X_CDirectSoundStream*   g_pDSoundStreamCache[SOUNDSTREAM_CACHE_SIZE] = { 0 }; //Default initialize to all zero'd
static int                          g_bDSoundCreateCalled = FALSE;
unsigned int                        g_iDSoundSynchPlaybackCounter = 0;

#define RETURN_RESULT_CHECK(hRet) { \
    static bool bPopupShown = false; if (!bPopupShown && hRet) { bPopupShown = true; \
    printf("Return result report: 0x%08X\nIn %s (%s)\n", hRet, __func__, __FILE__); \
    MessageBoxA(NULL, "An issue has been found. Please report game title and console's output of return result," \
    " function, and file name to https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/485", \
    "WARNING", MB_OK | MB_ICONWARNING); } return hRet; }

#include "EmuDSoundInline.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void CxbxInitAudio()
{
    g_EmuShared->GetXBAudio(&g_XBAudio);
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
    }

    enterCriticalSection;

    // Set this flag when this function is called
    g_bDSoundCreateCalled = TRUE;

    if (!initialized || !g_pDSound8) {
        hRet = DirectSoundCreate8(&g_XBAudio.GetAudioAdapter(), ppDirectSound, NULL);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup("DirectSoundCreate8 Failed!");
        }

        g_pDSound8 = *ppDirectSound;

        hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup("g_pDSound8->SetCooperativeLevel Failed!");
        }

        int v = 0;
        // clear sound buffer cache
        XTL::X_CDirectSoundBuffer* *pDSBuffer = g_pDSoundBufferCache;
        for (v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++) {
            if ((*pDSBuffer) == nullptr)
                continue;
            while (XTL::EMUPATCH(IDirectSoundBuffer_Release)((*pDSBuffer))) {};
            g_pDSoundBufferCache[v] = 0;
        }

        // clear sound stream cache
        XTL::X_CDirectSoundStream* *pDSStream = g_pDSoundStreamCache;
        for (v = 0; v < SOUNDSTREAM_CACHE_SIZE; v++) {
            if ((*pDSStream) == nullptr)
                continue;
            while (XTL::EMUPATCH(CDirectSoundStream_Release)((*pDSStream))) {};
            g_pDSoundStreamCache[v] = 0;
        }

        //Create Primary Buffer in order for Xbox's DirectSound to manage complete control of it.
        DSBUFFERDESC bufferDesc;
        bufferDesc.dwSize = sizeof(DSBUFFERDESC);
        bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D; //DSBCAPS_CTRLFX is not supported on primary buffer.
        bufferDesc.dwBufferBytes = 0;
        bufferDesc.dwReserved = 0;
        bufferDesc.lpwfxFormat = NULL;
        bufferDesc.guid3DAlgorithm = GUID_NULL;

        hRet = g_pDSound8->CreateSoundBuffer(&bufferDesc, &g_pDSoundPrimaryBuffer, NULL);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup("Creating primary buffer for DirectSound Failed!");
        }

        /* Quote from MDSN "For the primary buffer, you must use the
         * IDirectSoundBuffer interface; IDirectSoundBuffer8 is not available."
         */
        // Return E_NOINTERFACE from QueryInterface method, make sense for g_pDSoundPrimaryBuffer
        // But how to set DSBCAPS_CTRLFX on primary buffer or should it be set for all current and future cache buffers?
        // We need LPDIRECTSOUNDFXI3DL2REVERB8 / IID_IDirectSoundFXI3DL2Reverb8 or use LPDIRECTSOUNDBUFFER8 / IID_IDirectSoundBuffer8

        hRet = g_pDSoundPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&g_pDSoundPrimary3DListener8);

        if (hRet != DS_OK) {
            CxbxKrnlCleanup("Creating primary 3D Listener for DirectSound Failed!");
        }

        initialized = true;
    }

    // This way we can be sure that this function returns a valid
    // DirectSound8 pointer even if we initialized it elsewhere!
    if (!(*ppDirectSound) && g_pDSound8) {
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

    LOG_UNIMPLEMENTED_DSOUND();

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

    //TODO: This need a lock in each frame. I think it does not wait for each frame.
    XTL::X_CDirectSoundBuffer* *pDSBuffer = g_pDSoundBufferCache;
    for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++, pDSBuffer++) {
        if ((*pDSBuffer) == nullptr || (*pDSBuffer)->EmuBuffer == nullptr || (*pDSBuffer)->EmuBufferToggle != X_DSB_TOGGLE_DEFAULT) {
            continue;
        }
        /* Bypass Update buffer, audio appear to be working just fine without need to update audio.
        DSoundBufferUpdate((*pDSBuffer)->EmuDirectSoundBuffer8,
                            (*pDSBuffer)->EmuBufferDesc,
                            (*pDSBuffer)->EmuBuffer,
                            (*pDSBuffer)->EmuFlags,
                            (*pDSBuffer)->EmuLockOffset,
                            (*pDSBuffer)->EmuLockPtr1,
                            (*pDSBuffer)->EmuLockBytes1,
                            (*pDSBuffer)->EmuLockPtr2,
                            (*pDSBuffer)->EmuLockBytes2,
                            (*pDSBuffer)->EmuLockFlags);
        */
        // However there's a chance of locked buffers has been set which needs to be unlock.
        DSoundGenericUnlock((*pDSBuffer)->EmuFlags,
                            (*pDSBuffer)->EmuDirectSoundBuffer8,
                            (*pDSBuffer)->EmuBufferDesc,
                            (*pDSBuffer)->EmuLockOffset,
                            (*pDSBuffer)->EmuLockPtr1,
                            (*pDSBuffer)->EmuLockBytes1,
                            (*pDSBuffer)->EmuLockPtr2,
                            (*pDSBuffer)->EmuLockBytes2,
                            (*pDSBuffer)->EmuLockFlags);
    }

    // Bypass Update buffer, audio appear to be working just fine without need to update audio.
    /*
    XTL::X_CDirectSoundStream* *pDSStream = g_pDSoundStreamCache;
    for (int v = 0; v < SOUNDSTREAM_CACHE_SIZE; v++, pDSStream++) {
        if ((*pDSStream) == nullptr || (*pDSStream)->EmuBuffer == nullptr) {
            continue;
        }
        DSoundBufferUpdate((*pDSStream)->EmuDirectSoundBuffer8,
                            (*pDSStream)->EmuBufferDesc,
                            (*pDSStream)->EmuBuffer,
                            (*pDSStream)->EmuFlags,
                            0,
                            (*pDSStream)->EmuLockPtr1,
                            (*pDSStream)->EmuLockBytes1,
                            (*pDSStream)->EmuLockPtr2,
                            (*pDSStream)->EmuLockBytes2,
                            0);
    }
    */
    leaveCriticalSection;

    return;
}

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

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, dwApply);

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_12)
(
    LPDIRECTSOUND8          pThis,
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

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes_8
// ******************************************************************
// This revision is only used in XDK 4039 and higher.
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes_8)
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

    // NOTE: Read the above notes, and the rest is self explanitory...

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
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
// * patch: IDirectSound_SetPosition
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
            CxbxKrnlCleanup("Unable to initialize DirectSound!");
        }
    }

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdsbd)
		LOG_FUNC_ARG_OUT(ppBuffer)
		LOG_FUNC_END;

    DWORD dwEmuFlags = 0;

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)malloc(sizeof(DSBUFFERDESC));

    //DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00002000 | 0x00040000 | 0x00080000;
    DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00020000 | 0x00040000 /*| 0x00080000*/;

    if (pdsbd->dwFlags & (~dwAcceptableMask)) {
        EmuWarning("Use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)", pdsbd->dwFlags & (~dwAcceptableMask));
    }

    pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
    pDSBufferDesc->dwFlags = (pdsbd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLFREQUENCY;
    pDSBufferDesc->dwBufferBytes = pdsbd->dwBufferBytes;
    pDSBufferDesc->lpwfxFormat = nullptr;

    GeneratePCMFormat(pDSBufferDesc, pdsbd->lpwfxFormat, dwEmuFlags);

    // TODO: Garbage Collection
    *ppBuffer = new X_CDirectSoundBuffer();

    DSoundBufferSetDefault((*ppBuffer), pDSBufferDesc, dwEmuFlags, 0);
    (*ppBuffer)->EmuLockOffset = 0;
    DSoundBufferRegionSetDefault(*ppBuffer);

    DbgPrintf("EmuDSound: DirectSoundCreateBuffer, *ppBuffer := 0x%.08X, bytes := 0x%.08X\n", *ppBuffer, pDSBufferDesc->dwBufferBytes);

    DSoundBufferCreate(pDSBufferDesc, (*ppBuffer)->EmuDirectSoundBuffer8);
    if (pdsbd->dwFlags & DSBCAPS_CTRL3D) {
        DSound3DBufferCreate((*ppBuffer)->EmuDirectSoundBuffer8, (*ppBuffer)->EmuDirectSound3DBuffer8);
    }

    // cache this sound buffer
    {
        int v = 0;
        for (v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++) {
            if (g_pDSoundBufferCache[v] == 0) {
                g_pDSoundBufferCache[v] = *ppBuffer;
                break;
            }
        }

        if (v == SOUNDBUFFER_CACHE_SIZE) {
            CxbxKrnlCleanup("SoundBuffer cache out of slots!");
        }
    }

    leaveCriticalSection;

    return S_OK;
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

    //TODO: Current workaround method since dwBufferBytes do set to zero. Otherwise it will produce lock error message.
    if (dwBufferBytes == 0) {
        leaveCriticalSection;
        return DS_OK;
    }
    ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc,
                             pThis->EmuPlayFlags, dwBufferBytes, pThis->EmuDirectSound3DBuffer8);
    DSoundBufferRegionRelease(pThis);

    XTL::EMUPATCH(IDirectSoundBuffer_Lock)(pThis, 0, dwBufferBytes, &pThis->EmuLockPtr1,
                                           &pThis->EmuLockBytes1, nullptr, NULL, pThis->EmuLockFlags);

    if (pThis->EmuLockPtr1 != xbnullptr) {
        memcpy(pThis->EmuLockPtr1, pvBufferData, pThis->EmuLockBytes1);

        // TODO: We could reduce allocate and free buffer if the size is the same.
        if (pThis->EmuBuffer != xbnullptr) {
            free(pThis->EmuBuffer);
        }
        pThis->EmuBuffer = malloc(dwBufferBytes);
        memcpy(pThis->EmuBuffer, pvBufferData, dwBufferBytes);
    }

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->EmuLockOffset,
                        pThis->EmuLockPtr1,
                        pThis->EmuLockBytes1,
                        pThis->EmuLockPtr2,
                        pThis->EmuLockBytes2,
                        pThis->EmuLockFlags);

    leaveCriticalSection;

    return S_OK;
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

    if (dwBytes > pThis->EmuBufferDesc->dwBufferBytes) {
        if (pThis->EmuBuffer != xbnullptr) {
            free(pThis->EmuBuffer);
            pThis->EmuBuffer = xbnullptr;
        }
        ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc,
                                 pThis->EmuPlayFlags, dwBytes, pThis->EmuDirectSound3DBuffer8);
        DSoundBufferRegionRelease(pThis);
    }

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->EmuLockOffset,
                        pThis->EmuLockPtr1,
                        pThis->EmuLockBytes1,
                        pThis->EmuLockPtr2,
                        pThis->EmuLockBytes2,
                        pThis->EmuLockFlags);

    hRet = pThis->EmuDirectSoundBuffer8->Lock(dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1,
                                              ppvAudioPtr2, pdwAudioBytes2, dwFlags);

    if (hRet != DS_OK) {
        CxbxKrnlCleanup("DirectSoundBuffer Lock Failed!");
    }
    pThis->EmuLockOffset = dwOffset;
    pThis->EmuLockPtr1 = *ppvAudioPtr1;
    pThis->EmuLockBytes1 = *pdwAudioBytes1;
    pThis->EmuLockPtr2 = (ppvAudioPtr2 != xbnullptr) ? *ppvAudioPtr2 : xbnullptr;
    pThis->EmuLockBytes2 = (pdwAudioBytes2 != xbnullptr) ? *pdwAudioBytes2 : 0;
    pThis->EmuLockFlags = dwFlags;

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

    if (pThis->EmuBuffer != xbnullptr) {
        memcpy_s((PBYTE)pThis->EmuBuffer + pThis->EmuLockOffset,
                 pThis->EmuBufferDesc->dwBufferBytes - pThis->EmuLockOffset,
                 pThis->EmuLockPtr1,
                 pThis->EmuLockBytes1);
    }

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->EmuLockOffset,
                        pThis->EmuLockPtr1,
                        pThis->EmuLockBytes1,
                        pThis->EmuLockPtr2,
                        pThis->EmuLockBytes2,
                        pThis->EmuLockFlags);

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

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    // DirectSound does not provide SetHeadroom method.
    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
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

    pThis->EmuRegionLoopStartOffset = pThis->EmuRegionPlayStartOffset + dwLoopStart;
    pThis->EmuRegionLoopStartOffset = dwLoopStart;
    pThis->EmuRegionLoopLength = dwLoopLength;
    pThis->EmuBufferToggle = X_DSB_TOGGLE_LOOP;

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

    if (pThis != 0) {
        if (!(pThis->EmuFlags & DSB_FLAG_RECIEVEDATA)) {
            uRet = pThis->EmuDirectSoundBuffer8->Release();

            if (uRet == 0) {
                if (pThis->EmuDirectSound3DBuffer8 != nullptr) {
                    pThis->EmuDirectSound3DBuffer8->Release();
                }
                // remove cache entry
                for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++) {
                    if (g_pDSoundBufferCache[v] == pThis) {
                        g_pDSoundBufferCache[v] = 0;
                    }
                }

                if (pThis->EmuBufferDesc->lpwfxFormat != nullptr) {
                    free(pThis->EmuBufferDesc->lpwfxFormat);
                }
                if (pThis->EmuBuffer != xbnullptr) {
                    free(pThis->EmuBuffer);
                }
                if (pThis->EmuDirectSoundBuffer8Region != nullptr) {
                    pThis->EmuDirectSoundBuffer8Region->Release();
                }
                if (pThis->EmuDirectSound3DBuffer8Region != nullptr) {
                    pThis->EmuDirectSound3DBuffer8Region->Release();
                }
                free(pThis->EmuBufferDesc);

                delete pThis;
            }
        }
    }

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

    return HybridDirectSoundBuffer_SetPitch(DSoundBufferSelectionT(pThis), lPitch);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    X_CDirectSoundBuffer*   pThis,
    OUT LPDWORD                 pdwStatus)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_GetStatus(DSoundBufferSelectionT(pThis), pdwStatus);
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

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if (hRet != DS_OK) {
        EmuWarning("SetCurrentPosition Failed!");
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

    return HybridDirectSoundBuffer_GetCurrentPosition(DSoundBufferSelectionT(pThis), pdwCurrentPlayCursor, pdwCurrentWriteCursor);
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
                        pThis->EmuLockOffset,
                        pThis->EmuLockPtr1,
                        pThis->EmuLockBytes1,
                        pThis->EmuLockPtr2,
                        pThis->EmuLockBytes2,
                        pThis->EmuLockFlags);

    if (dwFlags & ~(X_DSBPLAY_LOOPING | X_DSBPLAY_FROMSTART | X_DSBPLAY_SYNCHPLAYBACK)) {
        CxbxKrnlCleanup("Unsupported Playing Flags");
    }
    pThis->EmuPlayFlags = dwFlags;

    // rewind buffer
    if ((dwFlags & X_DSBPLAY_FROMSTART)) {

        pThis->EmuPlayFlags &= ~X_DSBPLAY_FROMSTART;
    }

    if (dwFlags & X_DSBPLAY_SYNCHPLAYBACK) {
        pThis->EmuPlayFlags ^= X_DSBPLAY_SYNCHPLAYBACK;
        pThis->EmuFlags |= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
    }

    HRESULT hRet = DS_OK;

    // Process Play/Loop Region buffer (Region Buffer creation can be only place inside Play function
    if (pThis->EmuDirectSoundBuffer8Region == nullptr && pThis->EmuBufferToggle != X_DSB_TOGGLE_DEFAULT) {
        DWORD byteLength;
        DWORD startOffset;
        LPDSBUFFERDESC emuBufferDescRegion = (LPDSBUFFERDESC)malloc(sizeof(DSBUFFERDESC));
        memcpy_s(emuBufferDescRegion, sizeof(DSBUFFERDESC), pThis->EmuBufferDesc, sizeof(DSBUFFERDESC));

        switch (pThis->EmuBufferToggle) {
            case X_DSB_TOGGLE_LOOP:
                startOffset = pThis->EmuRegionPlayStartOffset + pThis->EmuRegionLoopStartOffset;

                // Must check for zero length, then apply true length.
                if (pThis->EmuRegionLoopLength == 0) {
                    if (pThis->EmuRegionPlayLength != 0) {
                        byteLength = pThis->EmuRegionPlayLength;
                    } else {
                        byteLength = pThis->EmuBufferDesc->dwBufferBytes - startOffset;
                    }
                } else {
                    byteLength = pThis->EmuRegionLoopLength;
                }
                break;
            case X_DSB_TOGGLE_PLAY:
                startOffset = pThis->EmuRegionPlayStartOffset;

                // Must check for zero length, then apply true length.
                if (pThis->EmuRegionPlayLength != 0) {
                    byteLength = pThis->EmuRegionPlayLength;
                } else {
                    byteLength = pThis->EmuBufferDesc->dwBufferBytes - startOffset;
                }
                break;
            default:
                free(emuBufferDescRegion);
                CxbxKrnlCleanup("Unknown TOGGLE region for DirectSoundBuffer class usage.");
        }
        emuBufferDescRegion->dwBufferBytes = byteLength;

        DSoundBufferCreate(emuBufferDescRegion, pThis->EmuDirectSoundBuffer8Region);
        if (pThis->EmuDirectSound3DBuffer8 != nullptr) {
            DSound3DBufferCreate(pThis->EmuDirectSoundBuffer8Region, pThis->EmuDirectSound3DBuffer8Region);
        }
        DSoundBufferTransferSettings(pThis->EmuDirectSoundBuffer8, pThis->EmuDirectSoundBuffer8Region,
                             pThis->EmuDirectSound3DBuffer8, pThis->EmuDirectSound3DBuffer8Region);

        hRet = pThis->EmuDirectSoundBuffer8Region->Lock(0, 0, &pThis->EmuLockPtr1, &pThis->EmuLockBytes1,
                                                        nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
        if (hRet != DS_OK) {
            CxbxKrnlCleanup("Unable to lock region buffer!");
        }
        if (pThis->EmuLockPtr1 != xbnullptr) {
            memcpy_s(pThis->EmuLockPtr1, pThis->EmuLockBytes1, (PVOID)((PBYTE)pThis->EmuBuffer + startOffset), byteLength);
            if (pThis->EmuFlags & DSB_FLAG_XADPCM) {
                DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8Region,
                                             emuBufferDescRegion, 0, pThis->EmuLockPtr1,
                                             pThis->EmuLockBytes1, nullptr, NULL, true);
            }
            pThis->EmuDirectSoundBuffer8Region->Unlock(pThis->EmuLockPtr1, pThis->EmuLockBytes1, nullptr, 0);
        }
        free(emuBufferDescRegion);
    }

    if (dwFlags & X_DSBPLAY_FROMSTART) {
        if (DSoundBufferSelectionT(pThis)->SetCurrentPosition(0) != DS_OK) {
            EmuWarning("Rewinding buffer failed!");
        }
    }
    if ((pThis->EmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) == 0) {
        hRet = DSoundBufferSelectionT(pThis)->Play(0, 0, pThis->EmuPlayFlags);
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
        if (pThis->EmuDirectSoundBuffer8Region != nullptr) {
            // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
            hRet = pThis->EmuDirectSoundBuffer8Region->Stop();
            pThis->EmuDirectSoundBuffer8Region->SetCurrentPosition(0);
        } else if (pThis->EmuDirectSoundBuffer8 != nullptr) {
            // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
            hRet = pThis->EmuDirectSoundBuffer8->Stop();
            pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
        }
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

    HRESULT hRet = D3D_OK;

    //TODO: RadWolfie - Rayman 3 crash at end of first intro for this issue...
    // if only return DS_OK, then it works fine until end of 2nd intro it crashed.
    if (pThis != nullptr) {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        switch (dwFlags) {
            case X_DSBSTOPEX_IMMEDIATE:
                hRet = DSoundBufferSelectionT(pThis)->Stop();
                DSoundBufferSelectionT(pThis)->SetCurrentPosition(0);
                break;
            case X_DSBSTOPEX_ENVELOPE:
                // TODO: How to mock up "release phase"?
                break;
            case X_DSBSTOPEX_RELEASEWAVEFORM:

                // Release from loop region.
                DWORD dwValue, dwStatus;
                if (pThis->EmuDirectSoundBuffer8Region != nullptr) {
                    pThis->EmuDirectSoundBuffer8Region->GetStatus(&dwStatus);

                    DSoundBufferTransferSettings(pThis->EmuDirectSoundBuffer8Region, pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuDirectSound3DBuffer8Region, pThis->EmuDirectSound3DBuffer8);

                    hRet = pThis->EmuDirectSoundBuffer8Region->Stop();
                    pThis->EmuDirectSoundBuffer8Region->GetCurrentPosition(&dwValue, nullptr);
                    dwValue += pThis->EmuRegionLoopStartOffset + pThis->EmuRegionPlayStartOffset;
                    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwValue);

                    if (dwStatus & DSBSTATUS_PLAYING) {
                        pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
                    }

                    // Finally, release region buffer.
                    DSoundBufferRegionRelease(pThis);
                }
                // TODO: How to mock up "release phase"?
                break;
            default:
                CxbxKrnlCleanup("Unknown dwFlags from IDirectSoundBuffer_StopEx: %8X", dwFlags);
        }
    }

    leaveCriticalSection;

    RETURN_RESULT_CHECK(hRet);
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

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    return HybridDirectSoundBuffer_SetVolume(DSoundBufferSelectionT(pThis), lVolume, pThis->EmuFlags);
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

    return HybridDirectSoundBuffer_SetFrequency(DSoundBufferSelectionT(pThis), dwFrequency);
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
            CxbxKrnlCleanup("Unable to initialize DirectSound!");
        }
    }

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pdssd)
		LOG_FUNC_ARG_OUT(ppStream)
		LOG_FUNC_END;

    DWORD dwEmuFlags = 0;
    // TODO: Garbage Collection
    *ppStream = new X_CDirectSoundStream();

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)malloc(sizeof(DSBUFFERDESC));


    DWORD dwAcceptableMask = 0x00000010; // TODO: Note 0x00040000 is being ignored (DSSTREAMCAPS_LOCDEFER)

    if (pdssd->dwFlags & (~dwAcceptableMask)) {
        EmuWarning("Use of unsupported pdssd->dwFlags mask(s) (0x%.08X)", pdssd->dwFlags & (~dwAcceptableMask));
    }
    pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
    //pDSBufferDesc->dwFlags = (pdssd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    pDSBufferDesc->dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; //aka DSBCAPS_DEFAULT
    pDSBufferDesc->dwBufferBytes = DSBSIZE_MIN;
    pDSBufferDesc->lpwfxFormat = nullptr;

    GeneratePCMFormat(pDSBufferDesc, pdssd->lpwfxFormat, dwEmuFlags);

    DSoundBufferSetDefault((*ppStream), pDSBufferDesc, dwEmuFlags, DSBPLAY_LOOPING);

    DbgPrintf("EmuDSound: DirectSoundCreateStream, *ppStream := 0x%.08X\n", *ppStream);

    DSoundBufferCreate(pDSBufferDesc, (*ppStream)->EmuDirectSoundBuffer8);
    if (pDSBufferDesc->dwFlags & DSBCAPS_CTRL3D) {
        DSound3DBufferCreate((*ppStream)->EmuDirectSoundBuffer8, (*ppStream)->EmuDirectSound3DBuffer8);
    }

    (*ppStream)->EmuDirectSoundBuffer8->SetCurrentPosition(0);
    //Apparently DirectSoundStream do not wait, let's go ahead start play "nothing".
    (*ppStream)->EmuDirectSoundBuffer8->Play(0, 0, DSBPLAY_LOOPING); 

        // cache this sound stream
    {
        int v = 0;
        for (v = 0; v < SOUNDSTREAM_CACHE_SIZE; v++) {
            if (g_pDSoundStreamCache[v] == 0) {
                g_pDSoundStreamCache[v] = *ppStream;
                break;
            }
        }

        if (v == SOUNDSTREAM_CACHE_SIZE) {
            CxbxKrnlCleanup("SoundStream cache out of slots!");
        }
    }

    leaveCriticalSection;

    return DS_OK;
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
//    EmuWarning("EmuCMcpxStream_Dummy_0x10 is ignored!");

    leaveCriticalSection;

    return;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_SetVolume)
(
    X_CDirectSoundStream*   pThis,
    LONG                    lVolume)
{
    FUNC_EXPORTS;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lVolume)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags);
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

    LOG_UNIMPLEMENTED_DSOUND();

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
            for (int v = 0; v < SOUNDSTREAM_CACHE_SIZE; v++) {
                if (g_pDSoundStreamCache[v] == pThis) {
                    g_pDSoundStreamCache[v] = 0;
                }
            }

            if (pThis->EmuBufferDesc->lpwfxFormat != NULL) {
                free(pThis->EmuBufferDesc->lpwfxFormat);
            }
            // NOTE: Do not release EmuBuffer! EmuBuffer is using xbox buffer.
            free(pThis->EmuBufferDesc);

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
    EmuWarning("EmuDirectSound_CDirectSoundStream_GetInfo is not yet supported!");

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
    OUT DWORD*                  pdwStatus)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG_OUT(pdwStatus)
        LOG_FUNC_END;

    return HybridDirectSoundBuffer_GetStatus(pThis->EmuDirectSoundBuffer8, pdwStatus);
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

    if (pThis->EmuDirectSoundBuffer8 != nullptr) {
        // update buffer data cache
        pThis->EmuBuffer = pInputBuffer->pvBuffer;

        ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc,
                                 pThis->EmuPlayFlags, pInputBuffer->dwMaxSize, pThis->EmuDirectSound3DBuffer8);

        if (pInputBuffer->pdwStatus != 0) {
            *pInputBuffer->pdwStatus = S_OK;
        }

        PVOID pAudioPtr, pAudioPtr2;
        DWORD dwAudioBytes, dwAudioBytes2;
        HRESULT hRet;


        //NOTE : XADPCM audio has occurred in Rayman Arena first intro video, all other title's intro videos are PCM so far.
        if (pThis->EmuFlags & DSB_FLAG_XADPCM) {

            DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuBufferDesc,
                                         0,
                                         pThis->EmuBuffer,
                                         pInputBuffer->dwMaxSize,
                                         0,
                                         0,
                                         false);

        } else {
            hRet = pThis->EmuDirectSoundBuffer8->Lock(0, pThis->EmuBufferDesc->dwBufferBytes, &pAudioPtr, &dwAudioBytes,
                                                      &pAudioPtr2, &dwAudioBytes2, 0);

            if (hRet == DS_OK) {

                if (pAudioPtr != 0) {
                    memcpy(pAudioPtr, pThis->EmuBuffer, dwAudioBytes);
                }
                if (pAudioPtr2 != 0) {
                    memcpy(pAudioPtr2, (PVOID)((DWORD)pThis->EmuBuffer + dwAudioBytes), dwAudioBytes2);
                }
                pThis->EmuDirectSoundBuffer8->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
            }
        }
        //TODO: RadWolfie - If remove this part, XADPCM audio will stay running, Rayman Arena, except...
        //...PCM audio does not for rest of titles. However it should not be here, so this is currently a workaround fix for now.
        hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwAudioBytes);
        if (hRet == DS_OK) {
            if ((dwAudioBytes & DSBSTATUS_PLAYING)) {
                pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
                pThis->EmuDirectSoundBuffer8->Play(0, 0, DSBPLAY_LOOPING);
            }
        }
    } else {
        if (pInputBuffer->pdwStatus != 0) {
            *pInputBuffer->pdwStatus = S_OK;
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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

    if (pThis != NULL) {
        DSoundBufferRemoveSynchPlaybackFlag(pThis->EmuFlags);
    }

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

    XTL::X_CDirectSoundBuffer* *pDSBuffer = g_pDSoundBufferCache;
    for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++, pDSBuffer++) {
        if ((*pDSBuffer) == nullptr || (*pDSBuffer)->EmuBuffer == nullptr) {
            continue;
        }

        if ((*pDSBuffer)->EmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) {
            // RadWolfie: I don't think SetCurrentPosition is necessary?
            //DSoundBufferSelectionT((*pDSBuffer))->SetCurrentPosition(0);
            DSoundBufferSelectionT((*pDSBuffer))->Play(0, 0, (*pDSBuffer)->EmuPlayFlags);
            (*pDSBuffer)->EmuFlags ^= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
        }
    }

    XTL::X_CDirectSoundStream* *pDSStream = g_pDSoundStreamCache;
    for (int v = 0; v < SOUNDSTREAM_CACHE_SIZE; v++, pDSStream++) {
        if ((*pDSStream) == nullptr || (*pDSStream)->EmuBuffer == nullptr) {
            continue;
        }
        if ((*pDSStream)->EmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) {
            (*pDSStream)->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            (*pDSStream)->EmuDirectSoundBuffer8->Play(0, 0, DSBPLAY_LOOPING);
            (*pDSStream)->EmuFlags ^= DSB_FLAG_SYNCHPLAYBACK_CONTROL;
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

    return HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags);
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

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwHeadroom)
		LOG_FUNC_END;

    // DirectSound does not provide SetHeadroom method.
    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
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
// * patch: IDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    X_DSI3DL2BUFFER*        pds3db,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    return XTL::EMUPATCH(CDirectSoundStream_SetI3DL2Source)(pThis, pds3db, dwApply);
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBins)
(
    PVOID   pThis, // X_CDirectSoundStream *pThis
    PVOID   pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_Unknown1
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_Unknown1)
(
    PVOID   pThis,
    DWORD   dwUnknown1)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(dwUnknown1)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

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

    return HybridDirectSound3DBuffer_SetMaxDistance(DSound3DBufferSelectionT(pThis), flMaxDistance, dwApply);
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

    return HybridDirectSound3DBuffer_SetMinDistance(DSound3DBufferSelectionT(pThis), flMinDistance, dwApply);
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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
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

    return HybridDirectSound3DBuffer_SetConeAngles(DSound3DBufferSelectionT(pThis), dwInsideConeAngle, dwOutsideConeAngle, dwApply);
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

    return HybridDirectSound3DBuffer_SetConeOrientation(DSound3DBufferSelectionT(pThis), x, y, z, dwApply);
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

    return HybridDirectSound3DBuffer_SetConeOutsideVolume(DSound3DBufferSelectionT(pThis), lConeOutsideVolume, dwApply);
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

    return HybridDirectSound3DBuffer_SetPosition(DSound3DBufferSelectionT(pThis), x, y, z, dwApply);
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

    return HybridDirectSound3DBuffer_SetVelocity(DSound3DBufferSelectionT(pThis), x, y, z, dwApply);
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
    LOG_UNIMPLEMENTED_DSOUND();

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

    return HybridDirectSound3DBuffer_SetMode(DSound3DBufferSelectionT(pThis), dwMode, dwApply);
}

// +s
// ******************************************************************
// * patch: IDirectSoundBuffer_SetFormat
// ******************************************************************
// TODO: When SetFormat is called, does Loop and Play region reset, including region buffer?
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
                                                     pThis->EmuPlayFlags, pThis->EmuDirectSound3DBuffer8);
    
    DSoundBufferRegionRelease(pThis);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: DirectSoundUseFullHRTF
// ******************************************************************
STDAPI_(void) XTL::EMUPATCH(DirectSoundUseFullHRTF)
(
    void)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC();

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;
}

// ******************************************************************
// * patch: DirectSoundUseLightHRTF
// ******************************************************************
STDAPI_(void) XTL::EMUPATCH(DirectSoundUseLightHRTF)
(
    void)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC();

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;
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

    //DSP relative function
    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume, pThis->EmuFlags);
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
    EmuWarning("EmuIDirectSound_EnableHeadphones ignored");

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

    // This function wasn't part of the XDK until 4721.

    DSoundGenericUnlock(pThis->EmuFlags,
                        pThis->EmuDirectSoundBuffer8,
                        pThis->EmuBufferDesc,
                        pThis->EmuLockOffset,
                        pThis->EmuLockPtr1,
                        pThis->EmuLockBytes1,
                        pThis->EmuLockPtr2,
                        pThis->EmuLockBytes2,
                        pThis->EmuLockFlags);

    return HybridDirectSoundBuffer_Pause(DSoundBufferSelectionT(pThis), dwPause, pThis->EmuFlags);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_PauseEx
// ******************************************************************
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

    // This function wasn't part of the XDK until 4721.
    // TODO: Implement time stamp feature (a thread maybe?)

    HRESULT hRet = HybridDirectSoundBuffer_Pause(DSoundBufferSelectionT(pThis), dwPause, pThis->EmuFlags);

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
    static bool bShowOnce = true;
    if (bShowOnce) {
        bShowOnce = false;
        LOG_UNIMPLEMENTED_DSOUND();
    }

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetEG)
(
    LPVOID        pThis,
    LPVOID        pEnvelopeDesc)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pEnvelopeDesc)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_Flush)()
{
    FUNC_EXPORTS;

	LOG_FUNC();

    LOG_UNIMPLEMENTED_DSOUND();

    return XTL::EMUPATCH(CDirectSoundStream_Flush)(NULL);
}

// ******************************************************************
// * patch: IDirectSoundStream_FlushEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_FlushEx)
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

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return XTL::EMUPATCH(CDirectSoundStream_Flush)(NULL);
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

	LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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
        EmuWarning("Not implemented for rtTimeStamp greater than 0 of %08d", rtTimeStamp);
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

    // Get PC's DirectSound capabilities
    DSCAPS DSCapsPC;
    ZeroMemory(&DSCapsPC, sizeof(DSCAPS));

    HRESULT hRet = g_pDSound8->GetCaps(&DSCapsPC);
    if (hRet != DS_OK) {
        EmuWarning("Failed to get PC DirectSound caps!");
    }

    // Convert PC -> Xbox
    if (pDSCaps) {
        // WARNING: This may not be accurate under Windows Vista...
        pDSCaps->dwFree2DBuffers = DSCapsPC.dwFreeHwMixingAllBuffers;
        pDSCaps->dwFree3DBuffers = DSCapsPC.dwFreeHw3DAllBuffers;
        pDSCaps->dwFreeBufferSGEs = 256;                            // TODO: Verify max on a real Xbox
        pDSCaps->dwMemoryAllocated = DSCapsPC.dwFreeHwMemBytes;    // TODO: Bytes or MegaBytes?
    }

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
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

    // FIXME: This is the best I could think of for now.
    // Check the XDK documentation for the description of what this function 
    // can actually do.  BTW, this function accesses the NVIDIA SoundStorm APU
    // register directly (0xFE80200C).

    // TODO: Handle reset at certain event?
    // TODO: Wait until a DirectSoundBuffer/Stream is being played?
    static DWORD dwStart = GetTickCount();
    DWORD dwRet = GetTickCount() - dwStart;

    leaveCriticalSection;

    return 0;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_12
// ******************************************************************
// This revision API is only used in XDK 4039 and higher.
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

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes_8
// ******************************************************************
// This revision API was used in XDK 3911 until API had changed in XDK 4039.
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes_8)
(
    X_CDirectSoundStream*   pThis,
    LPVOID                  pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pMixBins)
		LOG_FUNC_END;

    // NOTE: Read the above notes, and the rest is self explanitory...

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
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
    LOG_UNIMPLEMENTED_DSOUND();

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

    return HybridDirectSound3DBuffer_SetAllParameters(DSound3DBufferSelectionT(pThis), pc3DBufferParameters, dwApply);
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

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pwfxFormat)
		LOG_FUNC_END;

    return HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->EmuBufferDesc,
                                             pThis->EmuFlags, pThis->EmuPlayFlags, pThis->EmuDirectSound3DBuffer8);
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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hFile)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return E_FAIL;
}

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR              pszFileName,
    LPCWAVEFORMATEX*    ppwfxFormat,
    OUT void**              ppMediaObject)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pszFileName)
		LOG_FUNC_ARG(ppwfxFormat)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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
                    EmuWarning("Could not set notification position(s)!");
                }

                pNotify->Release();
            } else {
                EmuWarning("Could not create notification interface!");
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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

    EmuWarning("IDirectSoundBuffer_Use3DVoiceData not yet supported!");

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE      hFile,
    DWORD       dwMaxPackets,
    OUT void**      ppMediaObject)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG(dwMaxPackets)
        LOG_FUNC_ARG_OUT(ppMediaObject)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    *ppMediaObject = new X_XFileMediaObject();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    LONG                lOffset,
    DWORD               dwOrigin,
    LPDWORD             pdwAbsolute)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lOffset)
		LOG_FUNC_ARG(dwOrigin)
		LOG_FUNC_ARG(pdwAbsolute)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;
}

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject* pThis,
    OUT LPDWORD             pdwStatus)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetInfo)
(
    X_XFileMediaObject* pThis,
    OUT XMEDIAINFO*         pInfo)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pInfo)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Process)
(
    X_XFileMediaObject* pThis,
    LPXMEDIAPACKET      pInputBuffer,
    LPXMEDIAPACKET      pOutputBuffer)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pInputBuffer)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_AddRef)
(
	X_XFileMediaObject* pThis)
{
    FUNC_EXPORTS;

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
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_Release)
(
	X_XFileMediaObject* pThis)
{
    FUNC_EXPORTS;

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
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Discontinuity)
(
	X_XFileMediaObject *pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

	LOG_FUNC_ONE_ARG(pThis);

	LOG_UNIMPLEMENTED_DSOUND();

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

    LOG_UNIMPLEMENTED_DSOUND();

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

    HRESULT hRet = HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundStream_PauseEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_PauseEx)
(
    X_CDirectSoundStream   *pThis,
    REFERENCE_TIME          rtTimestamp,
    DWORD                   dwPause)
{
      FUNC_EXPORTS;

        LOG_FUNC_BEGIN
            LOG_FUNC_ARG(pThis)
            LOG_FUNC_ARG(rtTimestamp)
            LOG_FUNC_ARG(dwPause)
            LOG_FUNC_END;

    return XTL::EMUPATCH(CDirectSoundStream_PauseEx)(pThis, rtTimestamp, dwPause);
}
