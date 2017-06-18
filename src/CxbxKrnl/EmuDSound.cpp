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


/* TODO Task list:
 * * Need implement support for DirectSound3DListener in primary buffer
 * ** Need proper DirectSoundBuffer created after DirectSoundCreate function
 * ** etc...
 * * Need to extract whole DSound lib section out of xbe and start matching all functions including one missings.
 * ** Do this with 3+ xbe files with same versioning on DSound only
 */



// size of sound buffer cache (used for periodic sound buffer updates)
#define SOUNDBUFFER_CACHE_SIZE 0x200

// size of sound stream cache (used for periodic sound stream updates)
#define SOUNDSTREAM_CACHE_SIZE 0x200

// Xbox to PC volume ratio format (-10,000 / -6,400 )
#define XBOX_TO_PC_VOLUME_RATIO 1.5625

// Static Variable(s)
extern LPDIRECTSOUND8               g_pDSound8 = NULL; //This is necessary in order to allow share with EmuDSoundInline.hpp
static LPDIRECTSOUNDBUFFER          g_pDSoundPrimaryBuffer = NULL;
//TODO: RadWolfie - How to implement support if primary does not permit it for DSP usage?
static LPDIRECTSOUNDBUFFER8         g_pDSoundPrimaryBuffer8 = NULL;
static LPDIRECTSOUND3DLISTENER8     g_pDSoundPrimary3DListener8 = NULL;
static XTL::X_CDirectSoundBuffer*   g_pDSoundBufferCache[SOUNDBUFFER_CACHE_SIZE] = { 0 }; //Default initialize to all zero'd
static XTL::X_CDirectSoundStream*   g_pDSoundStreamCache[SOUNDSTREAM_CACHE_SIZE] = { 0 }; //Default initialize to all zero'd
static int                          g_bDSoundCreateCalled = FALSE;

#include "EmuDSoundInline.hpp"

// ******************************************************************
// * patch: DirectSoundCreate
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8* ppDirectSound,
    LPUNKNOWN       pUnknown)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: DirectSoundCreate\n"
              "(\n"
              "   pguidDeviceId             : 0x%.08X\n"
              "   ppDirectSound             : 0x%.08X\n"
              "   pUnknown                  : 0x%.08X\n"
              ");\n",
              pguidDeviceId, ppDirectSound, pUnknown);

    static bool initialized = false;

    HRESULT hRet = DS_OK;

    if (!initialized) {
        InitializeCriticalSection(&g_DSoundCriticalSection);
    }

    enterCriticalSection;

    // Set this flag when this function is called
    g_bDSoundCreateCalled = TRUE;

    if (!initialized || !g_pDSound8) {
        hRet = DirectSoundCreate8(NULL, ppDirectSound, NULL);

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("DirectSoundCreate8 Failed!");
        }

        g_pDSound8 = *ppDirectSound;

        hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

        if (FAILED(hRet)) {
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

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("Creating primary buffer for DirectSound Failed!");
        }

        /* Quote from MDSN "For the primary buffer, you must use the
         * IDirectSoundBuffer interface; IDirectSoundBuffer8 is not available."
         */
        // Return E_NOINTERFACE from QueryInterface method, make sense for g_pDSoundPrimaryBuffer
        // But how to set DSBCAPS_CTRLFX on primary buffer or should it be set for all current and future cache buffers?
        // We need LPDIRECTSOUNDFXI3DL2REVERB8 / IID_IDirectSoundFXI3DL2Reverb8 or use LPDIRECTSOUNDBUFFER8 / IID_IDirectSoundBuffer8

        hRet = g_pDSoundPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&g_pDSoundPrimary3DListener8);

        if (FAILED(hRet)) {
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

    return hRet;
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

    DbgPrintf("EmuDSound: IDirectSound_AddRef\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

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

    DbgPrintf("EmuDSound: IDirectSound_Release\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);


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
    PDWORD                  pdwSpeakerConfig)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSound_GetSpeakerConfig\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdwSpeakerConfig          : 0x%.08X\n"
              ");\n",
              pThis, pdwSpeakerConfig);

    *pdwSpeakerConfig = 0; // STEREO

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_EnableHeadphones)
(
    LPDIRECTSOUND8          pThis,
    BOOL                    fEnabled)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound8_EnableHeadphones\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fEnabled                  : 0x%.08X\n"
              ");\n",
              pThis, fEnabled);

    EmuWarning("EmuIDirectSound8_EnableHeadphones ignored");

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

    DbgPrintf("EmuDSound: IDirectSound_DownloadEffectsImage\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pvImageBuffer             : 0x%.08X\n"
              "   dwImageSize               : 0x%.08X\n"
              "   pImageLoc                 : 0x%.08X\n"
              "   ppImageDesc               : 0x%.08X\n"
              ");\n",
              pThis, pvImageBuffer, dwImageSize, pImageLoc, ppImageDesc);

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

    DbgPrintf("EmuDSound: DirectSoundDoWork();\n");

    //TODO: This need a lock in each frame. I think it does not wait for each frame.

    XTL::X_CDirectSoundBuffer* *pDSBuffer = g_pDSoundBufferCache;
    for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++, pDSBuffer++) {
        if ((*pDSBuffer) == nullptr || (*pDSBuffer)->EmuBuffer == nullptr) {
            continue;
        }
        DSoundBufferUpdate((*pDSBuffer)->EmuDirectSoundBuffer8,
                            (*pDSBuffer)->EmuBufferDesc,
                            (*pDSBuffer)->EmuBuffer,
                            (*pDSBuffer)->EmuFlags,
                            (*pDSBuffer)->EmuLockOffset,
                            (*pDSBuffer)->EmuLockPtr1,
                            (*pDSBuffer)->EmuLockBytes1,
                            (*pDSBuffer)->EmuLockPtr2,
                            (*pDSBuffer)->EmuLockBytes2);
    }

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
                            (*pDSStream)->EmuLockBytes2);
    }

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

    DbgPrintf("EmuDSound: IDirectSound_SetOrientation\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   xFront                    : %f\n"
              "   yFront                    : %f\n"
              "   zFront                    : %f\n"
              "   xTop                      : %f\n"
              "   yTop                      : %f\n"
              "   zTop                      : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, xFront, yFront, zFront, xTop, yTop, zTop, dwApply);

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, dwApply);

    leaveCriticalSection;

    return hRet;
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

    DbgPrintf("EmuDSound: IDirectSound_SetDistanceFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fDistanceFactor           : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, fDistanceFactor, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSound_SetRolloffFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fRolloffFactor            : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, fRolloffFactor, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSound_SetDopplerFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fDopplerFactor            : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, fDopplerFactor, dwApply);

    return HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, fDopplerFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSound_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound_SetI3DL2Listener\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pDummy                    : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pDummy, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSound_SetMixBinHeadroom\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwMixBinMask              : 0x%.08X\n"
              "   dwHeadroom                : 0x%.08X\n"
              ");\n",
              pThis, dwMixBinMask, dwHeadroom);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetMixBins\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pMixBins                  : 0x%.08X\n"
              ");\n",
              pThis, pMixBins);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes)
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
// * patch: IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetMixBinVolumes2)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetMixBinVolumes\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pMixBins                  : 0x%.08X\n"
              ");\n",
              pThis, pMixBins);

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

    DbgPrintf("EmuDSound: IDirectSound_SetPosition\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetPosition(x, y, z, dwApply);

    leaveCriticalSection;

    return hRet;
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

    DbgPrintf("EmuDSound: IDirectSound_SetVelocity\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetVelocity(x, y, z, dwApply);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPCDS3DLISTENER         pDS3DListenerParameters,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound_SetAllParameters\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pDS3DListenerParameters   : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pDS3DListenerParameters, dwApply);

    HRESULT hRet = g_pDSoundPrimary3DListener8->SetAllParameters(pDS3DListenerParameters, dwApply);

    leaveCriticalSection;

    return hRet;
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

    DbgPrintf("EmuDSound: CDirectSound_CommitDeferredSettings\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    HRESULT hRet = g_pDSoundPrimary3DListener8->CommitDeferredSettings();

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC*         pdsbd,
    X_CDirectSoundBuffer**  ppBuffer)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: DirectSoundCreateBuffer\n"
              "(\n"
              "   pdsbd                     : 0x%.08X\n"
              "   ppBuffer                  : 0x%.08X\n"
              ");\n",
              pdsbd, ppBuffer);

    DWORD dwEmuFlags = 0;

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)g_MemoryManager.Allocate(sizeof(DSBUFFERDESC));

    //DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00002000 | 0x00040000 | 0x00080000;
    DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00020000 | 0x00040000 /*| 0x00080000*/;

    if (pdsbd->dwFlags & (~dwAcceptableMask)) {
        EmuWarning("Use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)", pdsbd->dwFlags & (~dwAcceptableMask));
    }

    pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
    pDSBufferDesc->dwFlags = (pdsbd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    pDSBufferDesc->dwBufferBytes = pdsbd->dwBufferBytes;

    GeneratePCMFormat(pDSBufferDesc, pdsbd->lpwfxFormat, dwEmuFlags);

    // TODO: Garbage Collection
    *ppBuffer = new X_CDirectSoundBuffer();

    (*ppBuffer)->EmuDirectSoundBuffer8 = 0;
    (*ppBuffer)->EmuDirectSound3DBuffer8 = 0;
    (*ppBuffer)->EmuBuffer = 0;
    (*ppBuffer)->EmuBufferDesc = pDSBufferDesc;
    (*ppBuffer)->EmuLockOffset = 0;
    (*ppBuffer)->EmuLockPtr1 = 0;
    (*ppBuffer)->EmuLockBytes1 = 0;
    (*ppBuffer)->EmuLockPtr2 = 0;
    (*ppBuffer)->EmuLockBytes2 = 0;
    (*ppBuffer)->EmuFlags = dwEmuFlags;

    DbgPrintf("EmuDSound: DirectSoundCreateBuffer, *ppBuffer := 0x%.08X, bytes := 0x%.08X\n", *ppBuffer, pDSBufferDesc->dwBufferBytes);

    //Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
    LPDIRECTSOUNDBUFFER pTempBuffer;
    HRESULT hRet = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, NULL);

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("CreateSoundBuffer Failed!");
        (*ppBuffer)->EmuDirectSoundBuffer8 = NULL;
    } else {
        hRet = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&((*ppBuffer)->EmuDirectSoundBuffer8));
        pTempBuffer->Release();

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("CreateSoundBuffer8 Failed!");
        }
        if (pdsbd->dwFlags & DSBCAPS_CTRL3D) {

            HRESULT hRet3D = (*ppBuffer)->EmuDirectSoundBuffer8->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&((*ppBuffer)->EmuDirectSound3DBuffer8));
            if (FAILED(hRet3D)) {
                EmuWarning("CreateSound3DBuffer Failed!");
                (*ppBuffer)->EmuDirectSound3DBuffer8 = NULL;
            }

        }
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
// * patch: IDirectSound_CreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CreateBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC*         pdssd,
    X_CDirectSoundBuffer**  ppBuffer,
    PVOID                   pUnknown)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSound_CreateBuffer\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdssd                     : 0x%.08X\n"
              "   ppBuffer                  : 0x%.08X\n"
              "   pUnknown                  : 0x%.08X\n"
              ");\n",
              pThis, pdssd, ppBuffer, pUnknown);

    EMUPATCH(DirectSoundCreateBuffer)(pdssd, ppBuffer);

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_CreateSoundBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC*         pdsbd,
    X_CDirectSoundBuffer**  ppBuffer,
    LPUNKNOWN               pUnkOuter)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSound_CreateSoundBuffer\n"
              "(\n"
              "   pdsbd                     : 0x%.08X\n"
              "   ppBuffer                  : 0x%.08X\n"
              "   pUnkOuter                 : 0x%.08X\n"
              ");\n",
              pdsbd, ppBuffer, pUnkOuter);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetBufferData\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pvBufferData              : 0x%.08X\n"
              "   dwBufferBytes             : 0x%.08X\n"
              ");\n",
              pThis, pvBufferData, dwBufferBytes);

    // update buffer data cache
    pThis->EmuBuffer = pvBufferData;

    pThis->EmuLockOffset = 0;

    ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuPlayFlags, dwBufferBytes, pThis->EmuDirectSound3DBuffer8);

    if (pThis->EmuFlags & DSB_FLAG_ADPCM) {
        DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, 0, pvBufferData, dwBufferBytes, NULL, 0, false);
    }

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetPlayRegion\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwPlayStart               : 0x%.08X\n"
              "   dwPlayLength              : 0x%.08X\n"
              ");\n",
              pThis, dwPlayStart, dwPlayLength);

    // TODO: Translate params, then make the PC DirectSound call

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    // pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwPlayStart);

    LOG_UNIMPLEMENTED_DSOUND();

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Lock\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwOffset                  : 0x%.08X\n"
              "   dwBytes                   : 0x%.08X\n"
              "   ppvAudioPtr1              : 0x%.08X\n"
              "   pdwAudioBytes1            : 0x%.08X\n"
              "   ppvAudioPtr2              : 0x%.08X\n"
              "   pdwAudioBytes2            : 0x%.08X\n"
              "   dwFlags                   : 0x%.08X\n"
              ");\n",
              pThis, dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1,
              ppvAudioPtr2, pdwAudioBytes2, dwFlags);

    HRESULT hRet = D3D_OK;

    if (pThis->EmuBuffer != 0) {
        *ppvAudioPtr1 = pThis->EmuBuffer;
        *pdwAudioBytes1 = dwBytes;
    } else {
        if (dwBytes > pThis->EmuBufferDesc->dwBufferBytes) {
            ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuPlayFlags, dwBytes, pThis->EmuDirectSound3DBuffer8);
        }

        if (pThis->EmuLockPtr1 != nullptr) {
            if (pThis->EmuFlags & DSB_FLAG_ADPCM) {
                DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                            pThis->EmuBufferDesc,
                                            dwOffset,
                                            pThis->EmuLockPtr1,
                                            pThis->EmuLockBytes1,
                                            pThis->EmuLockPtr2,
                                            pThis->EmuLockBytes2,
                                            true);
            } else {
                pThis->EmuDirectSoundBuffer8->Unlock(pThis->EmuLockPtr1, pThis->EmuLockBytes1, pThis->EmuLockPtr2, pThis->EmuLockBytes2);
            }
        }

        // TODO: Verify dwFlags is the same as windows
        hRet = pThis->EmuDirectSoundBuffer8->Lock(dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("DirectSoundBuffer Lock Failed!");
        }
        pThis->EmuLockOffset = dwOffset;
        pThis->EmuLockPtr1 = *ppvAudioPtr1;
        pThis->EmuLockBytes1 = *pdwAudioBytes1;
        pThis->EmuLockPtr2 = (ppvAudioPtr2 != NULL) ? *ppvAudioPtr2 : NULL;
        pThis->EmuLockBytes2 = (pdwAudioBytes2 != NULL) ? *pdwAudioBytes2 : NULL;
    }

    leaveCriticalSection;

    return hRet;
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetHeadroom\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwHeadroom                : 0x%.08X\n"
              ");\n",
              pThis, dwHeadroom);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetLoopRegion\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwLoopStart               : 0x%.08X\n"
              "   dwLoopLength              : 0x%.08X\n"
              ");\n",
              pThis, dwLoopStart, dwLoopLength);

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    //ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuPlayFlags, dwLoopLength);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Release\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    ULONG uRet = 0;

    //TODO: RadWolfie - Need to move them into inline release function.
    if (pThis != 0) {
        if (!(pThis->EmuFlags & DSB_FLAG_RECIEVEDATA)) {
            uRet = pThis->EmuDirectSoundBuffer8->Release();

            if (uRet == 0) {
                if (pThis->EmuDirectSound3DBuffer8 != NULL) {
                    pThis->EmuDirectSound3DBuffer8->Release();
                }
                // remove cache entry
                for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++) {
                    if (g_pDSoundBufferCache[v] == pThis) {
                        g_pDSoundBufferCache[v] = 0;
                    }
                }

                if (pThis->EmuBufferDesc->lpwfxFormat != NULL) {
                    g_MemoryManager.Free(pThis->EmuBufferDesc->lpwfxFormat);
                }

                g_MemoryManager.Free(pThis->EmuBufferDesc);

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

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetPitch\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lPitch                    : 0x%.08X\n"
              ");\n",
              pThis, lPitch);

    // TODO: Translate params, then make the PC DirectSound call

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetStatus)
(
    X_CDirectSoundBuffer*   pThis,
    LPDWORD                 pdwStatus)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_GetStatus\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdwStatus                 : 0x%.08X\n"
              ");\n",
              pThis, pdwStatus);

    return HybridDirectSoundBuffer_GetStatus(pThis->EmuDirectSoundBuffer8, pdwStatus);
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetCurrentPosition\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwNewPosition             : 0x%.08X\n"
              ");\n",
              pThis, dwNewPosition);

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if (FAILED(hRet)) {
        EmuWarning("SetCurrentPosition Failed!");
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_GetCurrentPosition)
(
    X_CDirectSoundBuffer*   pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_GetCurrentPosition\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdwCurrentPlayCursor      : 0x%.08X\n"
              "   pdwCurrentWriteCursor     : 0x%.08X\n"
              ");\n",
              pThis, pdwCurrentPlayCursor, pdwCurrentWriteCursor);

    return HybridDirectSoundBuffer_GetCurrentPosition(pThis->EmuDirectSoundBuffer8, pdwCurrentPlayCursor, pdwCurrentWriteCursor);
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Play\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwReserved1               : 0x%.08X\n"
              "   dwReserved2               : 0x%.08X\n"
              "   dwFlags                   : 0x%.08X\n"
              ");\n",
              pThis, dwReserved1, dwReserved2, dwFlags);

    // close any existing locks
    if (pThis->EmuLockPtr1 != nullptr) {
        if (pThis->EmuFlags & DSB_FLAG_ADPCM) {
            DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuBufferDesc,
                                         pThis->EmuLockOffset,
                                         pThis->EmuLockPtr1,
                                         pThis->EmuLockBytes1,
                                         pThis->EmuLockPtr2,
                                         pThis->EmuLockBytes2,
                                         true);
        } else {
            pThis->EmuDirectSoundBuffer8->Unlock(pThis->EmuLockPtr1,
                                                 pThis->EmuLockBytes1,
                                                 pThis->EmuLockPtr2,
                                                 pThis->EmuLockBytes2
            );
        }

        pThis->EmuLockPtr1 = 0;
    }

    pThis->EmuPlayFlags = dwFlags;

    HRESULT hRet = HybridDirectSoundBuffer_Play(pThis->EmuDirectSoundBuffer8, pThis->EmuPlayFlags, pThis->EmuFlags);

    leaveCriticalSection;

    return hRet;
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Stop\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    HRESULT hRet = D3D_OK;

    if (pThis != nullptr && pThis->EmuDirectSoundBuffer8 != nullptr) {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        hRet = pThis->EmuDirectSoundBuffer8->Stop();
        pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer_StopEx
// ******************************************************************
extern "C" HRESULT __stdcall XTL::EMUPATCH(IDirectSoundBuffer_StopEx)
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

    if (pThis != nullptr && pThis->EmuDirectSoundBuffer8 != nullptr) {
        // TODO : Test Stop (emulated via Stop + SetCurrentPosition(0)) :
        hRet = pThis->EmuDirectSoundBuffer8->Stop();
        pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetVolume\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lVolume                   : 0x%.08X\n"
              ");\n",
              pThis, lVolume);

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume);
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetFrequency\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwFrequency               : 0x%.08X\n"
              ");\n",
              pThis, dwFrequency);

    return HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency);
}

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC*         pdssd,
    X_CDirectSoundStream**  ppStream)
{
    FUNC_EXPORTS;

    if (!g_pDSound8 && !g_bDSoundCreateCalled) {
        HRESULT hRet;

        EmuWarning("Initializing DirectSound pointer since it DirectSoundCreate was not called!");

        hRet = XTL::EMUPATCH(DirectSoundCreate)(NULL, &g_pDSound8, NULL);
        if (FAILED(hRet)) {
            CxbxKrnlCleanup("Unable to initialize DirectSound!");
        }
    }

    enterCriticalSection;

    DbgPrintf("EmuDSound: DirectSoundCreateStream\n"
              "(\n"
              "   pdssd                     : 0x%.08X (pdssd->dwFlags : 0x%.08X)\n"
              "   ppStream                  : 0x%.08X\n"
              ");\n",
              pdssd, pdssd->dwFlags, ppStream);

    DWORD dwEmuFlags = 0;
    // TODO: Garbage Collection
    *ppStream = new X_CDirectSoundStream();

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)g_MemoryManager.Allocate(sizeof(DSBUFFERDESC));


    DWORD dwAcceptableMask = 0x00000010; // TODO: Note 0x00040000 is being ignored (DSSTREAMCAPS_LOCDEFER)

    if (pdssd->dwFlags & (~dwAcceptableMask)) {
        EmuWarning("Use of unsupported pdssd->dwFlags mask(s) (0x%.08X)", pdssd->dwFlags & (~dwAcceptableMask));
    }
    pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
    //pDSBufferDesc->dwFlags = (pdssd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
    pDSBufferDesc->dwFlags = DSBCAPS_CTRLVOLUME;
    pDSBufferDesc->dwBufferBytes = DSBSIZE_MIN;


    GeneratePCMFormat(pDSBufferDesc, pdssd->lpwfxFormat, dwEmuFlags);

    (*ppStream)->EmuDirectSoundBuffer8 = 0;
    (*ppStream)->EmuDirectSound3DBuffer8 = 0;
    (*ppStream)->EmuBuffer = 0;
    (*ppStream)->EmuBufferDesc = pDSBufferDesc;
    (*ppStream)->EmuLockPtr1 = 0;
    (*ppStream)->EmuLockBytes1 = 0;
    (*ppStream)->EmuLockPtr2 = 0;
    (*ppStream)->EmuLockBytes2 = 0;
    (*ppStream)->EmuFlags = dwEmuFlags;
    (*ppStream)->EmuPlayFlags = DSBPLAY_LOOPING;

    DbgPrintf("EmuDSound: DirectSoundCreateStream, *ppStream := 0x%.08X\n", *ppStream);

    //Temporary creation since we need IDIRECTSOUNDBUFFER8, not IDIRECTSOUNDBUFFER class.
    LPDIRECTSOUNDBUFFER pTempBuffer;
    HRESULT hRet = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &pTempBuffer, NULL);

    if (FAILED(hRet)) {
        CxbxKrnlCleanup("CreateSoundBuffer Failed!");
        (*ppStream)->EmuDirectSoundBuffer8 = NULL;
    } else {
        hRet = pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&((*ppStream)->EmuDirectSoundBuffer8));
        pTempBuffer->Release();

        if (FAILED(hRet)) {
            CxbxKrnlCleanup("CreateSoundBuffer8 Failed!");
        }

        (*ppStream)->EmuDirectSoundBuffer8->SetCurrentPosition(0);
        (*ppStream)->EmuDirectSoundBuffer8->Play(0, 0, DSBPLAY_LOOPING); //Apparently DirectSoundStream do not wait, let's go ahead start play "nothing".

        if (pDSBufferDesc->dwFlags & DSBCAPS_CTRL3D) {

            HRESULT hRet3D = (*ppStream)->EmuDirectSoundBuffer8->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&((*ppStream)->EmuDirectSound3DBuffer8));
            if (FAILED(hRet3D)) {
                EmuWarning("CreateSound3DBuffer Failed!");
                (*ppStream)->EmuDirectSound3DBuffer8 = NULL;
            }
        }
    }

    //    __asm int 3

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
    X_CDirectSoundStream**  ppStream,
    PVOID                   pUnknown)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSound_CreateSoundStream\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdssd                     : 0x%.08X\n"
              "   ppStream                  : 0x%.08X\n"
              "   pUnknown                  : 0x%.08X\n"
              ");\n",
              pThis, pdssd, ppStream, pUnknown);

    EMUPATCH(DirectSoundCreateStream)(pdssd, ppStream);

    return DS_OK;
}

// ******************************************************************
// * patch: CMcpxStream_Dummy_0x10
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(CMcpxStream_Dummy_0x10)(DWORD dwDummy1, DWORD dwDummy2)
{
    FUNC_EXPORTS;

    enterCriticalSection;
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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetVolume\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lVolume                   : %d\n"
              ");\n",
              pThis, lVolume);

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume);
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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetRolloffFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fRolloffFactor            : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, fRolloffFactor, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_AddRef\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_Release\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    ULONG uRet = 0;
    //TODO: RadWolfie - Need to move them into inline release function.
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
                g_MemoryManager.Free(pThis->EmuBufferDesc->lpwfxFormat);
            }

            g_MemoryManager.Free(pThis->EmuBufferDesc);

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
    LPXMEDIAINFO            pInfo)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSoundStream_GetInfo\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pInfo                     : 0x%.08X\n"
              ");\n",
              pThis, pInfo);

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
    DWORD*                  pdwStatus)
{
    FUNC_EXPORTS;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(pdwStatus)
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

    DbgPrintf("EmuDSound: CDirectSoundStream_Process\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pInputBuffer              : 0x%.08X\n"
              "   pOutputBuffer             : 0x%.08X\n"
              ");\n",
              pThis, pInputBuffer, pOutputBuffer);

    if (pThis->EmuDirectSoundBuffer8 != NULL) {
        // update buffer data cache
        pThis->EmuBuffer = pInputBuffer->pvBuffer;

        ResizeIDirectSoundBuffer(pThis->EmuDirectSoundBuffer8, pThis->EmuBufferDesc, pThis->EmuPlayFlags, pInputBuffer->dwMaxSize, pThis->EmuDirectSound3DBuffer8);

        if (pInputBuffer->pdwStatus != 0) {
            *pInputBuffer->pdwStatus = S_OK;
        }

        PVOID pAudioPtr, pAudioPtr2;
        DWORD dwAudioBytes, dwAudioBytes2;
        HRESULT hRet;


        //NOTE : XADPCM audio has occurred in Rayman Arena first intro video, all other title's intro videos are PCM so far.
        if (pThis->EmuFlags & DSB_FLAG_ADPCM) {

            DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuBufferDesc,
                                         0,
                                         pThis->EmuBuffer,
                                         pInputBuffer->dwMaxSize,
                                         0,
                                         0,
                                         false);

        } else {
            hRet = pThis->EmuDirectSoundBuffer8->Lock(0, pThis->EmuBufferDesc->dwBufferBytes, &pAudioPtr, &dwAudioBytes, &pAudioPtr2, &dwAudioBytes2, 0);

            if (SUCCEEDED(hRet)) {

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

    DbgPrintf("EmuDSound: CDirectSoundStream_Discontinuity\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_Flush\n"
              "(\n"
              "   pThis           : 0x%.08X\n"
              ");\n",
              pThis);

    LOG_UNIMPLEMENTED_DSOUND();

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

    DbgPrintf("EmuDSound: CDirectSound_SynchPlayback\n"
              "(\n"
              "   pThis              : 0x%.08X\n"
              ");\n",
              pThis);

    //TODO: Test case Rayman 3 - Hoodlum Havoc, Battlestar Galactica, Miami Vice, and ...?

    XTL::X_CDirectSoundBuffer* *pDSBuffer = g_pDSoundBufferCache;
    for (int v = 0; v < SOUNDBUFFER_CACHE_SIZE; v++, pDSBuffer++) {
        if ((*pDSBuffer) == nullptr || (*pDSBuffer)->EmuBuffer == nullptr) {
            continue;
        }

        if ((*pDSBuffer)->EmuFlags & DSB_FLAG_SYNCHPLAYBACK_CONTROL) {
            (*pDSBuffer)->EmuDirectSoundBuffer8->SetCurrentPosition(0);
            (*pDSBuffer)->EmuDirectSoundBuffer8->Play(0, 0, (*pDSBuffer)->EmuPlayFlags);
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

    DbgPrintf("EmuDSound: CDirectSoundStream_Pause\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwPause                   : 0x%.08X\n"
              ");\n",
              pThis, dwPause);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetHeadroom\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwHeadroom                : 0x%.08X\n"
              ");\n",
              pThis, dwHeadroom);

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


    DbgPrintf("EmuDSound: CDirectSoundStream_SetConeAngles\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwInsideConeAngle         : 0x%.08X\n"
              "   dwOutsideConeAngle        : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, dwInsideConeAngle, dwOutsideConeAngle, dwApply);

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

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSoundStream_SetConeOutsideVolume\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lConeOutsideVolume        : %d\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, lConeOutsideVolume, dwApply);

    HRESULT hRet = DS_OK;
    if (pThis != NULL && pThis->EmuDirectSound3DBuffer8 != NULL) {
        pThis->EmuDirectSound3DBuffer8->SetConeOutsideVolume(lConeOutsideVolume, dwApply);
    }

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    X_CDirectSoundStream*   pThis,
    LPCDS3DBUFFER           pc3DBufferParameters,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: CDirectSoundStream_SetAllParameters\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pc3DBufferParameters      : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pc3DBufferParameters, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMaxDistance\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flMaxDistance              : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flMaxDistance, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMinDistance\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   fMinDistance              : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, fMinDistance, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetVelocity\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetConeOrientation\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetPosition\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetFrequency\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwFrequency               : %d\n"
              ");\n",
              pThis, dwFrequency);

    return HybridDirectSoundBuffer_SetFrequency(pThis->EmuDirectSoundBuffer8, dwFrequency);
}

// ******************************************************************
// * patch: IDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetI3DL2Source)
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundStream_SetI3DL2Source\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pds3db                    : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pds3db, dwApply);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMixBins\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pMixBins                  : 0x%.08X\n"
              ");\n",
              pThis, pMixBins);

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

    DbgPrintf("EmuDSound: IDirectSoundStream_Unknown1\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwUnknown1                : 0x%.08X\n"
              ");\n",
              pThis, dwUnknown1);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetMaxDistance\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flMaxDistance             : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flMaxDistance, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetMinDistance\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flMinDistance             : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flMinDistance, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetRolloffFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flRolloffFactor           : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flRolloffFactor, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetDistanceFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flDistanceFactor          : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flDistanceFactor, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetConeAngles\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwInsideConeAngle         : 0x%.08X\n"
              "   dwOutsideConeAngle        : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, dwInsideConeAngle,
              dwOutsideConeAngle, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetConeOrientation\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetConeOutsideVolume\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lConeOutsideVolume        : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, lConeOutsideVolume, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetPosition\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetVelocity\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   x                         : %f\n"
              "   y                         : %f\n"
              "   z                         : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, x, y, z, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetDopplerFactor\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   flDopplerFactor           : %f\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, flDopplerFactor, dwApply);

    return HybridDirectSound3DListener_SetDopplerFactor(g_pDSoundPrimary3DListener8, flDopplerFactor, dwApply);
}

// ******************************************************************
// * patch: IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer_SetI3DL2Source)
(
    X_CDirectSoundBuffer*   pThis,
    LPCDSI3DL2BUFFER        pds3db,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetI3DL2Source\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pds3db                    : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pds3db, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetMode\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   dwMode              : 0x%.08X\n"
              "   dwApply             : 0x%.08X\n"
              ");\n",
              pThis, dwMode, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetFormat\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pwfxFormat                : 0x%.08X\n"
              ");\n",
              pThis, pwfxFormat);

    return HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->EmuBufferDesc, pThis->EmuFlags);
}

// ******************************************************************
// * patch: DirectSoundUseFullHRTF
// ******************************************************************
STDAPI_(void) EMUPATCH(DirectSoundUseFullHRTF)
(
    void)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: DirectSoundUseFullHRTF()\n");

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetLFO\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pLFODesc                  : 0x%.08X\n"
              ");\n",
              pThis, pLFODesc);

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

    DbgPrintf("EmuDSound: XAudioCreateAdpcmFormat\n"
              "(\n"
              "   nChannels                 : 0x%.04X\n"
              "   nSamplesPerSec            : 0x%.08X\n"
              "   pwfx                      : 0x%.08X\n"
              ");\n",
              nChannels, nSamplesPerSec, pwfx);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetRolloffCurve\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pflPoints                 : 0x%.08X\n"
              "   dwPointCount              : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pflPoints, dwPointCount, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundStream_SetVolume\n"
              "(\n"
              "   pThis                   : 0x%.08X\n"
              "   lVolume                   : 0x%.08X\n"
              ");\n",
              pThis, lVolume);

    return HybridDirectSoundBuffer_SetVolume(pThis->EmuDirectSoundBuffer8, lVolume);
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

    DbgPrintf("EmuDSound: IDirectSound_EnableHeadphones\n"
              "(\n"
              "    pThis                    : 0x%.08X\n"
              "   fEnabled                : 0x%.08X\n"
              ");\n",
              pThis, fEnabled);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_AddRef\n"
              "(\n"
              "   pThis                   : 0x%.08X\n"
              ");\n",
              pThis);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Pause\n"
              "(\n"
              "    pThis                    : 0x%.08X\n"
              "   dwPause                 : 0x%.08X\n"
              ");\n",
              pThis, dwPause);

    // This function wasn't part of the XDK until 4721.

    // close any existing locks
    if (pThis->EmuLockPtr1 != nullptr) {
        if (pThis->EmuFlags & DSB_FLAG_ADPCM) {
            DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuBufferDesc,
                                         pThis->EmuLockOffset,
                                         pThis->EmuLockPtr1,
                                         pThis->EmuLockBytes1,
                                         pThis->EmuLockPtr2,
                                         pThis->EmuLockBytes2,
                                         true);
        } else {
            pThis->EmuDirectSoundBuffer8->Unlock(pThis->EmuLockPtr1,
                                                 pThis->EmuLockBytes1,
                                                 pThis->EmuLockPtr2,
                                                 pThis->EmuLockBytes2
            );
        }

        pThis->EmuLockPtr1 = 0;
    }

    return HybridDirectSoundBuffer_Pause(pThis->EmuDirectSoundBuffer8, dwPause, pThis->EmuFlags);
}

//// ******************************************************************
//// * patch: IDirectSoundBuffer_Pause
//// ******************************************************************
//extern "C" HRESULT __stdcall XTL::EmuIDirectSoundBuffer_PauseEx
//(
//    X_CDirectSoundBuffer   *pThis,
//    REFERENCE_TIME            rtTimestamp,
//    DWORD                    dwPause
//)
//{
//        
//
//    DbgPrintf("EmuDSound: IDirectSoundBuffer_PauseEx\n"
//            "(\n"
//            "    pThis                    : 0x%.08X\n"
//            "   rtTimestamp             : 0x%.08X\n"
//            "   dwPause                 : 0x%.08X\n"
//            ");\n",
//            pThis, rtTimestamp, dwPause);
//    
//    // This function wasn't part of the XDK until 4721.
//    // TODO: Implement time stamp feature (a thread maybe?)
//    LOG_UNIMPLEMENTED_DSOUND();    
//
//    HRESULT ret;
//
//    if(pThis != NULL) {
//        if(pThis->EmuDirectSoundBuffer8) {
//            if(dwPause == X_DSBPAUSE_PAUSE)
//                ret = pThis->EmuDirectSoundBuffer8->Stop();
//            if(dwPause == X_DSBPAUSE_RESUME) {
//                DWORD dwFlags = (pThis->EmuPlayFlags & X_DSBPLAY_LOOPING) ? DSBPLAY_LOOPING : 0;
//                ret = pThis->EmuDirectSoundBuffer8->Play(0, 0, dwFlags);
//            }
//            if(dwPause == X_DSBPAUSE_SYNCHPLAYBACK)
//                EmuWarning("DSBPAUSE_SYNCHPLAYBACK is not yet supported!");
//        }
//    }
//
//        
//
//    return ret;
//}

// ******************************************************************
// * patch: IDirectSound_GetOutputLevels
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetOutputLevels)
(
    LPDIRECTSOUND8*         pThis,
    X_DSOUTPUTLEVELS*       pOutputLevels,
    BOOL                    bResetPeakValues)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound_GetOutputLevels\n"
              "(\n"
              "    pThis                    : 0x%.08X\n"
              "   pOutputLevels           : 0x%.08X\n"
              "   bResetPeakValues        : 0x%.08X\n"
              ");\n",
              pThis, pOutputLevels, bResetPeakValues);

    // TODO: Anything?  Either way, I've never seen a game to date use this...

    LOG_UNIMPLEMENTED_DSOUND();

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetEG\n"
              "(\n"
              "    pThis                    : 0x%.08X\n"
              "   pEnvelopeDesc           : 0x%.08X\n"
              ");\n",
              pThis, pEnvelopeDesc);

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

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundStream_Flush()\n");

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_FlushEx
// ******************************************************************
extern "C" HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_FlushEx)
(
    X_CDirectSoundStream*   pThis,
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundStream_FlushEx\n"
              "(\n"
              "    pThis                    : 0x%.08X\n"
              "   rtTimeStamp             : 0x%.08X\n"
              "   dwFlags                 : 0x%.08X\n"
              ");\n",
              pThis, rtTimeStamp, dwFlags);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMode\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   dwMode              : 0x%.08X\n"
              "   dwApply             : 0x%.08X\n"
              ");\n",
              pThis, dwMode, dwApply);

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

    DbgPrintf("EmuDSound: XAudioDownloadEffectsImage\n"
              "(\n"
              "   pszImageName        : 0x%.08X\n"
              "   pImageLoc           : 0x%.08X\n"
              "   dwFlags             : 0x%.08X\n"
              "   ppImageDesc         : 0x%.08X\n"
              ");\n",
              pszImageName, pImageLoc, dwFlags, ppImageDesc);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetFilter\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   pFilterDesc         : 0x%.08X\n"
              ");\n",
              pThis, pFilterDesc);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetFilter\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   pFilterDesc         : 0x%.08X\n"
              ");\n",
              pThis, pFilterDesc);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}


// ******************************************************************
// * patch: IDirectSoundBuffer_PlayEx
// ******************************************************************
extern "C" HRESULT __stdcall XTL::EMUPATCH(IDirectSoundBuffer_PlayEx)
(
    X_CDirectSoundBuffer* pThis,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_PlayEx\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   rtTimeStamp               : 0x%.08X\n"
              "   dwFlags                   : 0x%.08X\n"
              ");\n",
              pThis, rtTimeStamp, dwFlags);

    // close any existing locks
    if (pThis->EmuLockPtr1 != nullptr) {
        if (pThis->EmuFlags & DSB_FLAG_ADPCM) {
            DSoundBufferXboxAdpcmDecoder(pThis->EmuDirectSoundBuffer8,
                                         pThis->EmuBufferDesc,
                                         pThis->EmuLockOffset,
                                         pThis->EmuLockPtr1,
                                         pThis->EmuLockBytes1,
                                         pThis->EmuLockPtr2,
                                         pThis->EmuLockBytes2,
                                         true);
        } else {
            pThis->EmuDirectSoundBuffer8->Unlock(pThis->EmuLockPtr1,
                                                 pThis->EmuLockBytes1,
                                                 pThis->EmuLockPtr2,
                                                 pThis->EmuLockBytes2
            );
        }

        pThis->EmuLockPtr1 = 0;
    }

    pThis->EmuPlayFlags = dwFlags;

    //TODO: Need implement support for rtTimeStamp.
    if (rtTimeStamp != 0) {
        EmuWarning("Not implemented for rtTimeStamp greater than 0 of %08d", rtTimeStamp);
    }

    HRESULT hRet = HybridDirectSoundBuffer_Play(pThis->EmuDirectSoundBuffer8, pThis->EmuPlayFlags, pThis->EmuFlags);

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound_GetCaps
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_GetCaps)
(
    X_CDirectSound*     pThis,
    X_DSCAPS*           pDSCaps)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound_GetCaps\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   pDSCaps             : 0x%.08X\n"
              ");\n",
              pThis, pDSCaps);

    // Get PC's DirectSound capabilities
    DSCAPS DSCapsPC;
    ZeroMemory(&DSCapsPC, sizeof(DSCAPS));

    HRESULT hRet = g_pDSound8->GetCaps(&DSCapsPC);
    if (FAILED(hRet)) {
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

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pThis)
        LOG_FUNC_ARG(lPitch)
        LOG_FUNC_END;

    HRESULT hRet = S_OK;

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return hRet;
}

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(DirectSoundGetSampleTime)()
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: DirectSoundGetSampleTime();\n");

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
// * patch: CDirectSoundStream_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes)
(
    X_CDirectSoundStream*   pThis,
    DWORD                   dwMixBinMask,
    const LONG*             alVolumes)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMixBinVolumes\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   dwMixBinMask        : 0x%.08X\n"
              "   alVolumes           : 0x%.08X\n"
              ");\n",
              pThis, dwMixBinMask, alVolumes);

    // NOTE: Use this function for XDK 3911 only because the implementation was changed
    // somewhere around the March 2002 (4361) update (or earlier, maybe).

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes2
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes2)
(
    X_CDirectSoundStream*   pThis,
    LPVOID                  pMixBins)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSoundStream_SetMixBinVolumes\n"
              "(\n"
              "   pThis               : 0x%.08X\n"
              "   pMixBins            : 0x%.08X\n"
              ");\n",
              pThis, pMixBins);

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
    PVOID                   pds3db,
    DWORD                   dwApply)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: CDirectSoundStream_SetI3DL2Source\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pds3db                    : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pds3db, dwApply);

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
    LPCDS3DBUFFER            pc3DBufferParameters,
    DWORD                    dwApply)
{
    FUNC_EXPORTS;

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetAllParameters\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pc3DBufferParameters      : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pc3DBufferParameters, dwApply);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetFormat\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pwfxFormat                : 0x%.08X\n"
              ");\n",
              pThis, pwfxFormat);

    return HybridDirectSoundBuffer_SetFormat(pThis->EmuDirectSoundBuffer8, pwfxFormat, pThis->EmuBufferDesc, pThis->EmuFlags);
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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetOutputBuffer\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pOutputBuffer             : 0x%.08X\n"
              ");\n",
              pThis, pOutputBuffer);

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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetOutputBuffer\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pOutputBuffer             : 0x%.08X\n"
              ");\n",
              pThis, pOutputBuffer);

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
    void**      ppMediaObject)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileCreateMediaObjectEx\n"
              "(\n"
              "   hFile                     : 0x%.08X\n"
              "   ppMediaObject             : 0x%.08X\n"
              ");\n",
              hFile, ppMediaObject);

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
    void**              ppMediaObject)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XWaveFileCreateMediaObject\n"
              "(\n"
              "   pszFileName               : (%s)\n"
              "   ppwfxFormat               : 0x%.08X\n"
              "   ppMediaObject             : 0x%.08X\n"
              ");\n",
              pszFileName, ppwfxFormat, ppMediaObject);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetEG\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pEnvelopeDesc             : 0x%.08X\n"
              ");\n",
              pThis, pEnvelopeDesc);

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
    LPVOID          pvData,
    DWORD           dwDataSize)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: IDirectSound_GetEffectData\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwEffectIndex             : 0x%.08X\n"
              "   dwOffset                  : 0x%.08X\n"
              "   pvData                    : 0x%.08X\n"
              "   dwDataSize                : 0x%.08X\n"
              ");\n",
              pThis, dwEffectIndex, dwOffset, pvData, dwDataSize);

    LOG_UNIMPLEMENTED_DSOUND();
    if (!pvData) {
        pvData = g_MemoryManager.Allocate(dwDataSize);
    }

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_SetNotificationPositions\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwNotifyCount             : 0x%.08X\n"
              "   paNotifies                : 0x%.08X\n"
              ");\n",
              pThis, dwNotifyCount, paNotifies);

    HRESULT hr = DSERR_INVALIDPARAM;

    // If we have a valid buffer, query a PC IDirectSoundNotify pointer and
    // use the paramaters as is since they are directly compatible, then release
    // the pointer. Any buffer that uses this *MUST* be created with the
    // DSBCAPS_CTRLPOSITIONNOTIFY flag!

    IDirectSoundNotify* pNotify = nullptr;

    if (pThis) {
        if (pThis->EmuDirectSoundBuffer8) {
            hr = pThis->EmuDirectSoundBuffer8->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&pNotify);
            if (SUCCEEDED(hr) && pNotify != nullptr) {
                hr = pNotify->SetNotificationPositions(dwNotifyCount, paNotifies);
                if (FAILED(hr)) {
                    EmuWarning("Could not set notification position(s)!");
                }

                pNotify->Release();
            } else {
                EmuWarning("Could not create notification interface!");
            }
        }
    }

    leaveCriticalSection;

    return hr;
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

    DbgPrintf("EmuDSound: CDirectSoundStream_SetRolloffCurve\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pflPoints                 : 0x%.08X\n"
              "   dwPointCount              : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, pflPoints, dwPointCount, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSound_SetEffectData\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   dwEfectIndex              : 0x%.08X\n"
              "   dwOffset                  : 0x%.08X\n"
              "   pvData                    : 0x%.08X\n"
              "   dwDataSize                : 0x%.08X\n"
              "   dwApply                   : 0x%.08X\n"
              ");\n",
              pThis, dwEffectIndex, dwOffset, pvData, dwDataSize, dwApply);

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

    DbgPrintf("EmuDSound: IDirectSoundBuffer_Use3DVoiceData\n"
              "(\n"
              "   pThis                  : 0x%.08X\n"
              "   pUnknown               : 0x%.08X\n"
              ");\n",
              pThis, pUnknown);

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
    void**      ppMediaObject)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG(dwMaxPackets)
        LOG_FUNC_ARG(ppMediaObject)
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

    DbgPrintf("EmuDSound: XFileMediaObject_Seek\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   lOffset                   : 0x%.08X\n"
              "   dwOrigin                  : 0x%.08X\n"
              "   pdwAbsolute               : 0x%.08X\n"
              ");\n",
              pThis, lOffset, dwOrigin, pdwAbsolute);

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

    DbgPrintf("EmuDSound: XFileMediaObject_DoWork\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;
}

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetStatus)
(
    X_XFileMediaObject* pThis,
    LPDWORD             pdwStatus)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileMediaObject_GetStatus\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pdwStatus                 : 0x%.08X\n"
              ");\n",
              pThis, pdwStatus);

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
    XMEDIAINFO*         pInfo)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileMediaObject_GetStatus\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pInfo                     : 0x%.08X\n"
              ");\n",
              pThis, pInfo);

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

    DbgPrintf("EmuDSound: XFileMediaObject_Process\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              "   pInputBuffer              : 0x%.08X\n"
              "   pOutputBuffer             : 0x%.08X\n"
              ");\n",
              pThis, pInputBuffer, pOutputBuffer);

    LOG_UNIMPLEMENTED_DSOUND();

    leaveCriticalSection;

    return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_AddRef)(X_XFileMediaObject* pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileMediaObject_AddRef\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    ULONG Ret = 0;

    if (pThis) {
        pThis->EmuRefCount++;
        Ret = pThis->EmuRefCount;
    }

    leaveCriticalSection;

    return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_Release)(X_XFileMediaObject* pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileMediaObject_Release\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    ULONG Ret = 0;

    if (pThis) {
        pThis->EmuRefCount--;

        if (pThis->EmuRefCount < 1) {
            delete pThis;
        }

        Ret = pThis->EmuRefCount;
    }

    leaveCriticalSection;

    return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Discontinuity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Discontinuity)(X_XFileMediaObject *pThis)
{
    FUNC_EXPORTS;

    enterCriticalSection;

    DbgPrintf("EmuDSound: XFileMediaObject_Discontinuity\n"
              "(\n"
              "   pThis                     : 0x%.08X\n"
              ");\n",
              pThis);

    leaveCriticalSection;

    return DS_OK;
}
