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
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"
#include "EmuAlloc.h"
#include "EmuXTL.h"

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
    &XTL::EMUPATCH(CDirectSoundStream_AddRef),         // 0x00
    &XTL::EMUPATCH(CDirectSoundStream_Release),        // 0x04
/*
    STDMETHOD(GetInfo)(THIS_ LPXMEDIAINFO pInfo) PURE;
*/
	&XTL::EMUPATCH(CDirectSoundStream_GetInfo),        // 0x08
    &XTL::EMUPATCH(CDirectSoundStream_GetStatus),      // 0x0C
    &XTL::EMUPATCH(CDirectSoundStream_Process),        // 0x10
    &XTL::EMUPATCH(CDirectSoundStream_Discontinuity),  // 0x14
    &XTL::EMUPATCH(CDirectSoundStream_Flush),          // 0x18
    0xBEEFB003,                                 // 0x1C
    0xBEEFB004,                                 // 0x20
    0xBEEFB005,                                 // 0x24
    0xBEEFB006,                                 // 0x28
    0xBEEFB007,                                 // 0x2C
    0xBEEFB008,                                 // 0x30
    0xBEEFB009,                                 // 0x34
    0xBEEFB00A,                                 // 0x38
};

XTL::X_XFileMediaObject::_vtbl XTL::X_XFileMediaObject::vtbl = 
{
	&XTL::EMUPATCH(XFileMediaObject_AddRef),		// 0x00
	&XTL::EMUPATCH(XFileMediaObject_Release),		// 0x04
	&XTL::EMUPATCH(XFileMediaObject_GetInfo),		// 0x08
	&XTL::EMUPATCH(XFileMediaObject_GetStatus),	// 0x0C
	&XTL::EMUPATCH(XFileMediaObject_Process),		// 0x10
	&XTL::EMUPATCH(XFileMediaObject_Discontinuity),// 0x14
	0xBEEFD007,								// 0x18
	&XTL::EMUPATCH(XFileMediaObject_Seek),			// 0x1C
	0xBEEFD009,								// 0x20
	&XTL::EMUPATCH(XFileMediaObject_DoWork),		// 0x24
};


// size of sound buffer cache (used for periodic sound buffer updates)
#define SOUNDBUFFER_CACHE_SIZE 0x100

// size of sound stream cache (used for periodic sound stream updates)
#define SOUNDSTREAM_CACHE_SIZE 0x100

// Static Variable(s)
static XTL::LPDIRECTSOUND8          g_pDSound8 = NULL;
static int                          g_pDSound8RefCount = 0;
static XTL::X_CDirectSoundBuffer   *g_pDSoundBufferCache[SOUNDBUFFER_CACHE_SIZE];
static XTL::X_CDirectSoundStream   *g_pDSoundStreamCache[SOUNDSTREAM_CACHE_SIZE];
static int							g_bDSoundCreateCalled = FALSE;

// periodically update sound buffers
static void HackUpdateSoundBuffers()
{
    for(int v=0;v<SOUNDBUFFER_CACHE_SIZE;v++)
    {
        if(g_pDSoundBufferCache[v] == 0 || g_pDSoundBufferCache[v]->EmuBuffer == 0)
            continue;

        PVOID pAudioPtr, pAudioPtr2;
        DWORD dwAudioBytes, dwAudioBytes2;

        // unlock existing lock
        if(g_pDSoundBufferCache[v]->EmuLockPtr1 != 0)
            g_pDSoundBufferCache[v]->EmuDirectSoundBuffer8->Unlock(g_pDSoundBufferCache[v]->EmuLockPtr1, g_pDSoundBufferCache[v]->EmuLockBytes1, g_pDSoundBufferCache[v]->EmuLockPtr2, g_pDSoundBufferCache[v]->EmuLockBytes2);

        HRESULT hRet = g_pDSoundBufferCache[v]->EmuDirectSoundBuffer8->Lock(0, g_pDSoundBufferCache[v]->EmuBufferDesc->dwBufferBytes, &pAudioPtr, &dwAudioBytes, &pAudioPtr2, &dwAudioBytes2, 0);

        if(SUCCEEDED(hRet))
        {
            if(pAudioPtr != 0)
                memcpy(pAudioPtr,  g_pDSoundBufferCache[v]->EmuBuffer, dwAudioBytes);

            if(pAudioPtr2 != 0)
                memcpy(pAudioPtr2, (PVOID)((DWORD)g_pDSoundBufferCache[v]->EmuBuffer+dwAudioBytes), dwAudioBytes2);

            g_pDSoundBufferCache[v]->EmuDirectSoundBuffer8->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
        }

        // TODO: relock old lock ??
    }
}

// periodically update sound streams
static void HackUpdateSoundStreams()
{
    for(int v=0;v<SOUNDSTREAM_CACHE_SIZE;v++)
    {
        if(g_pDSoundStreamCache[v] == 0 || g_pDSoundStreamCache[v]->EmuBuffer == NULL || g_pDSoundStreamCache[v]->EmuDirectSoundBuffer8 == NULL)
            continue;

        PVOID pAudioPtr, pAudioPtr2;
        DWORD dwAudioBytes, dwAudioBytes2;

        HRESULT hRet = g_pDSoundStreamCache[v]->EmuDirectSoundBuffer8->Lock(0, g_pDSoundStreamCache[v]->EmuBufferDesc->dwBufferBytes, &pAudioPtr, &dwAudioBytes, &pAudioPtr2, &dwAudioBytes2, 0);

        if(SUCCEEDED(hRet))
        {
            if(pAudioPtr != 0)
                memcpy(pAudioPtr,  g_pDSoundStreamCache[v]->EmuBuffer, dwAudioBytes);

            if(pAudioPtr2 != 0)
                memcpy(pAudioPtr2, (PVOID)((DWORD)g_pDSoundStreamCache[v]->EmuBuffer+dwAudioBytes), dwAudioBytes2);

            g_pDSoundStreamCache[v]->EmuDirectSoundBuffer8->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
        }

        g_pDSoundStreamCache[v]->EmuDirectSoundBuffer8->SetCurrentPosition(0);
        g_pDSoundStreamCache[v]->EmuDirectSoundBuffer8->Play(0, 0, 0);
    }

    return;
}

// resize an emulated directsound buffer, if necessary
static void EmuResizeIDirectSoundBuffer8(XTL::X_CDirectSoundBuffer *pThis, DWORD dwBytes)
{
    if(dwBytes == pThis->EmuBufferDesc->dwBufferBytes || dwBytes == 0)
        return;

    DbgPrintf("EmuResizeIDirectSoundBuffer8 : Resizing! (0x%.08X->0x%.08X)\n", pThis->EmuBufferDesc->dwBufferBytes, dwBytes);

    DWORD dwPlayCursor, dwWriteCursor, dwStatus;

    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve current position for resize reallocation!");

    hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve current status for resize reallocation!");

    // release old buffer
    while(pThis->EmuDirectSoundBuffer8->Release() > 0) { }

    pThis->EmuBufferDesc->dwBufferBytes = dwBytes;

    hRet = g_pDSound8->CreateSoundBuffer(pThis->EmuBufferDesc, &pThis->EmuDirectSoundBuffer8, NULL);

    if(FAILED(hRet))
        CxbxKrnlCleanup("IDirectSoundBuffer8 resize Failed!");

    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwPlayCursor);

    if(dwStatus & DSBSTATUS_PLAYING)
        pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
}

// resize an emulated directsound stream, if necessary
static void EmuResizeIDirectSoundStream8(XTL::X_CDirectSoundStream *pThis, DWORD dwBytes)
{
    if(dwBytes == pThis->EmuBufferDesc->dwBufferBytes)
        return;

    DWORD dwPlayCursor, dwWriteCursor, dwStatus;

    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve current position for resize reallocation!");

    hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);

    if(FAILED(hRet))
        CxbxKrnlCleanup("Unable to retrieve current status for resize reallocation!");

    // release old buffer
    while(pThis->EmuDirectSoundBuffer8->Release() > 0) { }

    pThis->EmuBufferDesc->dwBufferBytes = dwBytes;

    hRet = g_pDSound8->CreateSoundBuffer(pThis->EmuBufferDesc, &pThis->EmuDirectSoundBuffer8, NULL);

    if(FAILED(hRet))
        CxbxKrnlCleanup("IDirectSoundBuffer8 resize Failed!");

    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwPlayCursor);

    if(dwStatus & DSBSTATUS_PLAYING)
        pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
}

// ******************************************************************
// * patch: DirectSoundCreate
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreate)
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
)
{
    

    DbgPrintf("EmuDSound: EmuDirectSoundCreate\n"
           "(\n"
           "   pguidDeviceId             : 0x%.08X\n"
           "   ppDirectSound             : 0x%.08X\n"
           "   pUnknown                  : 0x%.08X\n"
           ");\n",
           pguidDeviceId, ppDirectSound, pUnknown);

    static bool initialized = false;

    HRESULT hRet = DS_OK;

	// Set this flag when this function is called
	g_bDSoundCreateCalled = TRUE;

    if(!initialized || !g_pDSound8)
    {
        hRet = DirectSoundCreate8(NULL, ppDirectSound, NULL);

        if(FAILED(hRet))
            CxbxKrnlCleanup("DirectSoundCreate8 Failed!");

        g_pDSound8 = *ppDirectSound;

        hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

        if(FAILED(hRet))
            CxbxKrnlCleanup("g_pDSound8->SetCooperativeLevel Failed!");

        int v=0;
        // clear sound buffer cache
        for(v=0;v<SOUNDBUFFER_CACHE_SIZE;v++)
            g_pDSoundBufferCache[v] = 0;

        // clear sound stream cache
        for(v=0;v<SOUNDSTREAM_CACHE_SIZE;v++)
            g_pDSoundStreamCache[v] = 0;

        initialized = true;
    }

	// This way we can be sure that this function returns a valid
	// DirectSound8 pointer even if we initialized it elsewhere!
	if(!(*ppDirectSound) && g_pDSound8)
		*ppDirectSound = g_pDSound8;

    g_pDSound8RefCount = 1;

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSound8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSound8_AddRef)
(
    LPDIRECTSOUND8          pThis
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_AddRef\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = g_pDSound8RefCount++;

    

    return uRet;
}

// ******************************************************************
// * patch: IDirectSound8_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSound8_Release)
(
    LPDIRECTSOUND8          pThis
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_Release\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = g_pDSound8RefCount--;

    /* temporarily (?) disabled
    if(uRet == 1)
        pThis->Release();
    //*/

    

    return uRet;
}

// ******************************************************************
// * patch: CDirectSound_GetSpeakerConfig
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_GetSpeakerConfig)
(
    X_CDirectSound         *pThis,
    PDWORD                  pdwSpeakerConfig
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSound_GetSpeakerConfig\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pdwSpeakerConfig          : 0x%.08X\n"
           ");\n",
           pThis, pdwSpeakerConfig);

    *pdwSpeakerConfig = 0; // STEREO

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_EnableHeadphones)
(
    LPDIRECTSOUND8          pThis,
    BOOL                    fEnabled
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_EnableHeadphones\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fEnabled                  : 0x%.08X\n"
           ");\n",
           pThis, fEnabled);

    EmuWarning("EmuIDirectSound8_EnableHeadphones ignored");

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SynchPlayback
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SynchPlayback)
(
    LPDIRECTSOUND8          pThis
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_SynchPlayback\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    EmuWarning("EmuIDirectSound8_SynchPlayback ignored");

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_DownloadEffectsImage)
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    DWORD                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_DownloadEffectsImage\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pvImageBuffer             : 0x%.08X\n"
           "   dwImageSize               : 0x%.08X\n"
           "   pImageLoc                 : 0x%.08X\n"
           "   ppImageDesc               : 0x%.08X\n"
           ");\n",
           pThis, pvImageBuffer, dwImageSize, pImageLoc, ppImageDesc);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: DirectSoundDoWork
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(DirectSoundDoWork)()
{
    

    DbgPrintf("EmuDSound: EmuDirectSoundDoWork();\n");

    HackUpdateSoundBuffers();
    HackUpdateSoundStreams();

    

    return;
}

// ******************************************************************
// * patch: IDirectSound8_SetOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetOrientation)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           xFront,
    FLOAT           yFront,
    FLOAT           zFront,
    FLOAT           xTop,
    FLOAT           yTop,
    FLOAT           zTop,
    DWORD           dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_SetOrientation\n"
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

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetDistanceFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_SetDistanceFactor\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fDistanceFactor           : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fDistanceFactor, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetRolloffFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_SetRolloffFactor\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fRolloffFactor            : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fRolloffFactor, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetDopplerFactor)
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_SetDopplerFactor\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fDopplerFactor            : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fDopplerFactor, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetI3DL2Listener)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_SetI3DL2Listener\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pDummy                    : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, pDummy, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetMixBinHeadroom)
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_SetMixBinHeadroom\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwMixBinMask              : 0x%.08X\n"
               "   dwHeadroom                : 0x%.08X\n"
               ");\n",
               pThis, dwMixBinMask, dwHeadroom);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetMixBins)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetMixBins\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pMixBins                  : 0x%.08X\n"
               ");\n",
               pThis, pMixBins);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetMixBinVolumes)
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pMixBins
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetMixBinVolumes\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pMixBins                  : 0x%.08X\n"
               ");\n",
               pThis, pMixBins);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetPosition)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_SetPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, x, y, z, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetVelocity)
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_SetVelocity\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, x, y, z, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetAllParameters)
(
    LPDIRECTSOUND8          pThis,
    LPVOID                  pTodo,  // TODO: LPCDS3DLISTENER
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_SetAllParameters\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pTodo                     : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, pTodo, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_CommitDeferredSettings)
(
    X_CDirectSound         *pThis
)
{
    DbgPrintf("EmuDSound: EmuCDirectSound_CommitDeferredSettings\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    // TODO: Translate params, then make the PC DirectSound call

    return DS_OK;
}

// ******************************************************************
// * patch: DirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateBuffer)
(
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer
)
{
    DbgPrintf("EmuDSound: EmuDirectSoundCreateBuffer\n"
           "(\n"
           "   pdsbd                     : 0x%.08X\n"
           "   ppBuffer                  : 0x%.08X\n"
           ");\n",
           pdsbd, ppBuffer);

    DWORD dwEmuFlags = 0;

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)CxbxMalloc(sizeof(DSBUFFERDESC));
	DSBUFFERDESC *pDSBufferDescSpecial = NULL;
	bool bIsSpecial = false;

    // convert from Xbox to PC DSound
    {
        //DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00002000 | 0x00040000 | 0x00080000;
		DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00020000 | 0x00040000 /*| 0x00080000*/;

        if(pdsbd->dwFlags & (~dwAcceptableMask))
            EmuWarning("Use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)", pdsbd->dwFlags & (~dwAcceptableMask));

        pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
        pDSBufferDesc->dwFlags = (pdsbd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
        pDSBufferDesc->dwBufferBytes = pdsbd->dwBufferBytes;

        if(pDSBufferDesc->dwBufferBytes < DSBSIZE_MIN)
            pDSBufferDesc->dwBufferBytes = DSBSIZE_MIN;
        else if(pDSBufferDesc->dwBufferBytes > DSBSIZE_MAX)
            pDSBufferDesc->dwBufferBytes = DSBSIZE_MAX;

        pDSBufferDesc->dwReserved = 0;

        if(pdsbd->lpwfxFormat != NULL)
        {
            pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)CxbxMalloc(sizeof(WAVEFORMATEX)+pdsbd->lpwfxFormat->cbSize);
            memcpy(pDSBufferDesc->lpwfxFormat, pdsbd->lpwfxFormat, sizeof(WAVEFORMATEX));

            if(pDSBufferDesc->lpwfxFormat->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
            {
                dwEmuFlags |= DSB_FLAG_ADPCM;

                EmuWarning("WAVE_FORMAT_XBOX_ADPCM Unsupported!");

                pDSBufferDesc->lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
                pDSBufferDesc->lpwfxFormat->nBlockAlign = (pDSBufferDesc->lpwfxFormat->nChannels*pDSBufferDesc->lpwfxFormat->wBitsPerSample)/8;

                // the above calculation can yield zero for wBitsPerSample < 8, so we'll bound it to 1 byte minimum
                if(pDSBufferDesc->lpwfxFormat->nBlockAlign == 0)
                    pDSBufferDesc->lpwfxFormat->nBlockAlign = 1;

                pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec*pDSBufferDesc->lpwfxFormat->nBlockAlign;
                pDSBufferDesc->lpwfxFormat->wBitsPerSample = 8;

                /* TODO: Get ADPCM working!
                pDSBufferDesc->lpwfxFormat->cbSize = 32;
                #define WAVE_FORMAT_ADPCM 2
                pDSBufferDesc->lpwfxFormat->wFormatTag = WAVE_FORMAT_ADPCM;
                */
            }
        }
		else
		{
			bIsSpecial = true;
			dwEmuFlags |= DSB_FLAG_RECIEVEDATA;

			EmuWarning("Creating dummy WAVEFORMATEX (pdsbd->lpwfxFormat = NULL)...");
		
			// HACK: This is a special sound buffer, create dummy WAVEFORMATEX data.
			// It's supposed to recieve data rather than generate it.  Buffers created
			// with flags DSBCAPS_MIXIN, DSBCAPS_FXIN, and DSBCAPS_FXIN2 will have no
			// WAVEFORMATEX structure by default.

			// TODO: A better response to this scenario if possible.

			pDSBufferDescSpecial = (DSBUFFERDESC*)CxbxMalloc(sizeof(DSBUFFERDESC));
			pDSBufferDescSpecial->lpwfxFormat =  (WAVEFORMATEX*)CxbxMalloc(sizeof(WAVEFORMATEX));

			//memset(pDSBufferDescSpecial->lpwfxFormat, 0, sizeof(WAVEFORMATEX)); 
		    //memset(pDSBufferDescSpecial, 0, sizeof(DSBUFFERDESC)); 

		    pDSBufferDescSpecial->lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM; 
		    pDSBufferDescSpecial->lpwfxFormat->nChannels = 2; 
		    pDSBufferDescSpecial->lpwfxFormat->nSamplesPerSec = 22050; 
		    pDSBufferDescSpecial->lpwfxFormat->nBlockAlign = 4; 
		    pDSBufferDescSpecial->lpwfxFormat->nAvgBytesPerSec = pDSBufferDescSpecial->lpwfxFormat->nSamplesPerSec * 
																 pDSBufferDescSpecial->lpwfxFormat->nBlockAlign; 
		    pDSBufferDescSpecial->lpwfxFormat->wBitsPerSample = 16; 
		 
		    pDSBufferDescSpecial->dwSize = sizeof(DSBUFFERDESC); 
		    pDSBufferDescSpecial->dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; 
		    pDSBufferDescSpecial->dwBufferBytes = 3 * pDSBufferDescSpecial->lpwfxFormat->nAvgBytesPerSec;

	//		pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)CxbxMalloc(sizeof(WAVEFORMATEX)/*+pdsbd->lpwfxFormat->cbSize*/);

	////	pDSBufferDesc->lpwfxFormat->cbSize = sizeof( WAVEFORMATEX );
	//		pDSBufferDesc->lpwfxFormat->nChannels = 1;
	//		pDSBufferDesc->lpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;
	//		pDSBufferDesc->lpwfxFormat->nSamplesPerSec = 22050;
	//		pDSBufferDesc->lpwfxFormat->nBlockAlign = 4;
	//		pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = 4 * 22050;
	//		pDSBufferDesc->lpwfxFormat->wBitsPerSample = 16;

			// Give this buffer 3 seconds of data if needed
			/*if(pdsbd->dwBufferBytes == 0)
				pDSBufferDesc->dwBufferBytes = 3 * pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec;*/
		}

        pDSBufferDesc->guid3DAlgorithm = DS3DALG_DEFAULT;
    }

    // sanity check
	if(!bIsSpecial)
	{
		if(pDSBufferDesc->lpwfxFormat->nBlockAlign != (pDSBufferDesc->lpwfxFormat->nChannels*pDSBufferDesc->lpwfxFormat->wBitsPerSample)/8)
		{
			pDSBufferDesc->lpwfxFormat->nBlockAlign = (2*pDSBufferDesc->lpwfxFormat->wBitsPerSample)/8;
			pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec * pDSBufferDesc->lpwfxFormat->nBlockAlign;
		}
	}

    // TODO: Garbage Collection
    *ppBuffer = new X_CDirectSoundBuffer();

    (*ppBuffer)->EmuDirectSoundBuffer8 = 0;
    (*ppBuffer)->EmuBuffer = 0;
	(*ppBuffer)->EmuBufferDesc = bIsSpecial ? pDSBufferDescSpecial : pDSBufferDesc;
    (*ppBuffer)->EmuLockPtr1 = 0;
    (*ppBuffer)->EmuLockBytes1 = 0;
    (*ppBuffer)->EmuLockPtr2 = 0;
    (*ppBuffer)->EmuLockBytes2 = 0;
    (*ppBuffer)->EmuFlags = dwEmuFlags;

    DbgPrintf("EmuDSound: EmuDirectSoundCreateBuffer, *ppBuffer := 0x%.08X, bytes := 0x%.08X\n", *ppBuffer, pDSBufferDesc->dwBufferBytes);

    HRESULT hRet = g_pDSound8->CreateSoundBuffer(bIsSpecial ? pDSBufferDescSpecial : pDSBufferDesc, &((*ppBuffer)->EmuDirectSoundBuffer8), NULL);

    if(FAILED(hRet))
	{
        EmuWarning("CreateSoundBuffer Failed!");
		(*ppBuffer)->EmuDirectSoundBuffer8 = NULL;
	}

    // cache this sound buffer
    {
        int v=0;
        for(v=0;v<SOUNDBUFFER_CACHE_SIZE;v++)
        {
            if(g_pDSoundBufferCache[v] == 0)
            {
                g_pDSoundBufferCache[v] = *ppBuffer;
                break;
            }
        }

        if(v == SOUNDBUFFER_CACHE_SIZE)
            CxbxKrnlCleanup("SoundBuffer cache out of slots!");
    }

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_CreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_CreateBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdssd,
    X_CDirectSoundBuffer  **ppBuffer,
    PVOID                   pUnknown
)
{
       DbgPrintf("EmuDSound: EmuIDirectSound8_CreateBuffer\n"
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
// * patch: IDirectSound8_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_CreateSoundBuffer)
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer,
    LPUNKNOWN               pUnkOuter
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_CreateSoundBuffer\n"
               "(\n"
               "   pdsbd                     : 0x%.08X\n"
               "   ppBuffer                  : 0x%.08X\n"
               "   pUnkOuter                 : 0x%.08X\n"
               ");\n",
               pdsbd, ppBuffer, pUnkOuter);

    return EMUPATCH(DirectSoundCreateBuffer)(pdsbd, ppBuffer);
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetBufferData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetBufferData)
(
    X_CDirectSoundBuffer   *pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetBufferData\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pvBufferData              : 0x%.08X\n"
           "   dwBufferBytes             : 0x%.08X\n"
           ");\n",
           pThis, pvBufferData, dwBufferBytes);

    // update buffer data cache
    pThis->EmuBuffer = pvBufferData;

	EmuResizeIDirectSoundBuffer8(pThis, dwBufferBytes);

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetPlayRegion)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetPlayRegion\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwPlayStart               : 0x%.08X\n"
           "   dwPlayLength              : 0x%.08X\n"
           ");\n",
           pThis, dwPlayStart, dwPlayLength);

    // TODO: Translate params, then make the PC DirectSound call

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_Lock
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_Lock)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwOffset,
    DWORD                   dwBytes,
    LPVOID                 *ppvAudioPtr1,
    LPDWORD                 pdwAudioBytes1,
    LPVOID                 *ppvAudioPtr2,
    LPDWORD                 pdwAudioBytes2,
    DWORD                   dwFlags
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_Lock\n"
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

    if(pThis->EmuBuffer != 0)
    {
        *ppvAudioPtr1 = pThis->EmuBuffer;
        *pdwAudioBytes1 = dwBytes;
    }
    else
    {
        if(dwBytes > pThis->EmuBufferDesc->dwBufferBytes)
			EmuResizeIDirectSoundBuffer8(pThis, dwBytes);

        if(pThis->EmuLockPtr1 != 0)
            pThis->EmuDirectSoundBuffer8->Unlock(pThis->EmuLockPtr1, pThis->EmuLockBytes1, pThis->EmuLockPtr2, pThis->EmuLockBytes2);

        // TODO: Verify dwFlags is the same as windows
        hRet = pThis->EmuDirectSoundBuffer8->Lock(dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);

        if(FAILED(hRet))
            CxbxKrnlCleanup("DirectSoundBuffer Lock Failed!");

        pThis->EmuLockPtr1 = *ppvAudioPtr1;
        pThis->EmuLockBytes1 = *pdwAudioBytes1;
        pThis->EmuLockPtr2 = (ppvAudioPtr2 != NULL) ? *ppvAudioPtr2 : NULL;
        pThis->EmuLockBytes2 = (pdwAudioBytes2 != NULL) ? *pdwAudioBytes2 : NULL;
    }

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetHeadroom)
(
    X_CDirectSoundBuffer  *pThis,
    DWORD                  dwHeadroom
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetHeadroom\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwHeadroom                : 0x%.08X\n"
           ");\n",
           pThis, dwHeadroom);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetLoopRegion)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetLoopRegion\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwLoopStart               : 0x%.08X\n"
           "   dwLoopLength              : 0x%.08X\n"
           ");\n",
           pThis, dwLoopStart, dwLoopLength);

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

    //EmuResizeIDirectSoundBuffer8(pThis, dwLoopLength);

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_Release)
(
    X_CDirectSoundBuffer   *pThis
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_Release\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = 0;

    if(pThis != 0)
    {
		if(!(pThis->EmuFlags & DSB_FLAG_RECIEVEDATA))
		{
			uRet = pThis->EmuDirectSoundBuffer8->Release();

			if(uRet == 0)
			{
				// remove cache entry
				for(int v=0;v<SOUNDBUFFER_CACHE_SIZE;v++)
				{
					if(g_pDSoundBufferCache[v] == pThis)
						g_pDSoundBufferCache[v] = 0;
				}

				if(pThis->EmuBufferDesc->lpwfxFormat != NULL)
					CxbxFree(pThis->EmuBufferDesc->lpwfxFormat);

				CxbxFree(pThis->EmuBufferDesc);

				delete pThis;
			}
		}
    }

    

    return uRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetPitch)
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lPitch
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetPitch\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   lPitch                    : 0x%.08X\n"
           ");\n",
           pThis, lPitch);

    // TODO: Translate params, then make the PC DirectSound call

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_GetStatus)
(
    X_CDirectSoundBuffer   *pThis,
    LPDWORD                 pdwStatus
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_GetStatus\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pdwStatus                 : 0x%.08X\n"
           ");\n",
           pThis, pdwStatus);

    HRESULT hRet = DS_OK;

    if(pThis != 0 && pThis->EmuBuffer == 0)
    {
        hRet = pThis->EmuDirectSoundBuffer8->GetStatus(pdwStatus);
    }
    else
    {
        *pdwStatus = 0;
    }

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetCurrentPosition)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwNewPosition
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetCurrentPosition\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwNewPosition             : 0x%.08X\n"
           ");\n",
           pThis, dwNewPosition);

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if(FAILED(hRet))
        EmuWarning("SetCurrentPosition Failed!");

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_GetCurrentPosition)
(
    X_CDirectSoundBuffer   *pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_GetCurrentPosition\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pdwCurrentPlayCursor      : 0x%.08X\n"
           "   pdwCurrentWriteCursor     : 0x%.08X\n"
           ");\n",
           pThis, pdwCurrentPlayCursor, pdwCurrentWriteCursor);

	HRESULT hRet = E_FAIL;

    HackUpdateSoundBuffers();
    HackUpdateSoundStreams();

    // NOTE: TODO: This call always seems to fail on primary buffers!
	if( pThis && pThis->EmuDirectSoundBuffer8 )
	{
		hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(pdwCurrentPlayCursor, pdwCurrentWriteCursor);

		if(FAILED(hRet))
			EmuWarning("GetCurrentPosition Failed!");

		if(pdwCurrentPlayCursor != 0 && pdwCurrentWriteCursor != 0)
		{
			DbgPrintf("*pdwCurrentPlayCursor := %d, *pdwCurrentWriteCursor := %d\n", *pdwCurrentPlayCursor, *pdwCurrentWriteCursor);
		}
	}

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_Play
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_Play)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_Play\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwReserved1               : 0x%.08X\n"
           "   dwReserved2               : 0x%.08X\n"
           "   dwFlags                   : 0x%.08X\n"
           ");\n",
           pThis, dwReserved1, dwReserved2, dwFlags);

    if(dwFlags & ~(X_DSBPLAY_LOOPING | X_DSBPLAY_FROMSTART | X_DSBPLAY_SYNCHPLAYBACK))
        CxbxKrnlCleanup("Unsupported Playing Flags");

    // rewind buffer
    if((dwFlags & X_DSBPLAY_FROMSTART) != X_DSBPLAY_FROMSTART)
    {
        if(FAILED(pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0)))
            EmuWarning("Rewinding buffer failed!");

        dwFlags &= ~X_DSBPLAY_FROMSTART;
    }

    HackUpdateSoundBuffers();

    // close any existing locks
    if(pThis->EmuLockPtr1 != 0)
    {
        pThis->EmuDirectSoundBuffer8->Unlock
        (
            pThis->EmuLockPtr1,
            pThis->EmuLockBytes1,
            pThis->EmuLockPtr2,
            pThis->EmuLockBytes2
        );

        pThis->EmuLockPtr1 = 0;
    }

    HRESULT hRet;

    if(pThis->EmuFlags & DSB_FLAG_ADPCM)
    {
        hRet = D3D_OK;
    }
    else
    {
        hRet = pThis->EmuDirectSoundBuffer8->Play(0, 0, dwFlags);
    }

    pThis->EmuPlayFlags = dwFlags;

    

    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_Stop
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_Stop)
(
    X_CDirectSoundBuffer   *pThis
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_Stop\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

	HRESULT hRet = D3D_OK;
    
	if (pThis != nullptr) {
		hRet = pThis->EmuDirectSoundBuffer8->Stop();
	}
   
    return hRet;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_StopEx
// ******************************************************************
extern "C" HRESULT __stdcall XTL::EMUPATCH(IDirectSoundBuffer8_StopEx)
(
    X_CDirectSoundBuffer *pBuffer,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_StopEx\n"
           "(\n"
           "   pBuffer                   : 0x%.08X\n"
           "   rtTimeStamp               : 0x%.08X\n"
           "   dwFlags                   : 0x%.08X\n"
           ");\n",
           pBuffer, rtTimeStamp, dwFlags);

    if(pBuffer->EmuDirectSoundBuffer8 == 0)
        EmuWarning("pBuffer->EmuDirectSoundBuffer8 == 0");

    EmuWarning("StopEx not yet implemented!");

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetVolume)
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lVolume
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetVolume\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   lVolume                   : 0x%.08X\n"
           ");\n",
           pThis, lVolume);

    // TODO: Ensure that 4627 & 4361 are intercepting far enough back
    // (otherwise pThis is manipulated!)

//    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetVolume(lVolume);

    

//    return hRet;
    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetFrequency)
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwFrequency
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetFrequency\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwFrequency               : 0x%.08X\n"
           ");\n",
           pThis, dwFrequency);

//    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetFrequency(dwFrequency);

    

//    return hRet;
    return S_OK;
}

// ******************************************************************
// * patch: DirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(DirectSoundCreateStream)
(
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream
)
{
    

    DbgPrintf("EmuDSound: EmuDirectSoundCreateStream\n"
           "(\n"
           "   pdssd                     : 0x%.08X (pdssd->dwFlags : 0x%.08X)\n"
           "   ppStream                  : 0x%.08X\n"
           ");\n",
           pdssd, pdssd->dwFlags, ppStream);

    // TODO: Garbage Collection
    *ppStream = new X_CDirectSoundStream();

    DSBUFFERDESC *pDSBufferDesc = (DSBUFFERDESC*)CxbxMalloc(sizeof(DSBUFFERDESC));

    // convert from Xbox to PC DSound
    {
        DWORD dwAcceptableMask = 0x00000010; // TODO: Note 0x00040000 is being ignored (DSSTREAMCAPS_LOCDEFER)

        if(pdssd->dwFlags & (~dwAcceptableMask))
            EmuWarning("Use of unsupported pdssd->dwFlags mask(s) (0x%.08X)", pdssd->dwFlags & (~dwAcceptableMask));

        pDSBufferDesc->dwSize = sizeof(DSBUFFERDESC);
//        pDSBufferDesc->dwFlags = (pdssd->dwFlags & dwAcceptableMask) | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
        pDSBufferDesc->dwFlags = DSBCAPS_CTRLVOLUME;
        pDSBufferDesc->dwBufferBytes = DSBSIZE_MIN;

        pDSBufferDesc->dwReserved = 0;

        if(pdssd->lpwfxFormat != NULL)
        {
            pDSBufferDesc->lpwfxFormat = (WAVEFORMATEX*)CxbxMalloc(sizeof(WAVEFORMATEX));
            memcpy(pDSBufferDesc->lpwfxFormat, pdssd->lpwfxFormat, sizeof(WAVEFORMATEX));
        }

        pDSBufferDesc->guid3DAlgorithm = DS3DALG_DEFAULT;

        if(pDSBufferDesc->lpwfxFormat != NULL && pDSBufferDesc->lpwfxFormat->wFormatTag != WAVE_FORMAT_PCM)
        {
            EmuWarning("Invalid WAVE_FORMAT!");
            if(pDSBufferDesc->lpwfxFormat->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
                EmuWarning("WAVE_FORMAT_XBOX_ADPCM Unsupported!");

            (*ppStream)->EmuDirectSoundBuffer8 = 0;

            

            return DS_OK;
        }

		if(pDSBufferDesc->lpwfxFormat != NULL)
		{
			// we only support 2 channels right now
			if(pDSBufferDesc->lpwfxFormat->nChannels > 2)
			{
				pDSBufferDesc->lpwfxFormat->nChannels = 2;
				pDSBufferDesc->lpwfxFormat->nBlockAlign = (2*pDSBufferDesc->lpwfxFormat->wBitsPerSample)/8;
				pDSBufferDesc->lpwfxFormat->nAvgBytesPerSec = pDSBufferDesc->lpwfxFormat->nSamplesPerSec * pDSBufferDesc->lpwfxFormat->nBlockAlign;
			}
		}
    }

    (*ppStream)->EmuBuffer = 0;
    (*ppStream)->EmuBufferDesc = pDSBufferDesc;
    (*ppStream)->EmuLockPtr1 = 0;
    (*ppStream)->EmuLockBytes1 = 0;
    (*ppStream)->EmuLockPtr2 = 0;
    (*ppStream)->EmuLockBytes2 = 0;

    DbgPrintf("EmuDSound: EmuDirectSoundCreateStream, *ppStream := 0x%.08X\n", *ppStream);

	if(!g_pDSound8)
	{
		if( !g_bDSoundCreateCalled )
		{
			HRESULT hRet;

			EmuWarning("Initializing DirectSound pointer since it DirectSoundCreate was not called!");

			// Create the DirectSound buffer before continuing...
			if(FAILED(DirectSoundCreate8( NULL, &g_pDSound8, NULL )))
				CxbxKrnlCleanup("Unable to initialize DirectSound!");

			hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

			if(FAILED(hRet))
				CxbxKrnlCleanup("g_pDSound8->SetCooperativeLevel Failed!");

			int v=0;
			// clear sound buffer cache
			for(v=0;v<SOUNDBUFFER_CACHE_SIZE;v++)
				g_pDSoundBufferCache[v] = 0;

			// clear sound stream cache
			for(v=0;v<SOUNDSTREAM_CACHE_SIZE;v++)
				g_pDSoundStreamCache[v] = 0;

			// Let's count DirectSound as being initialized now
			g_bDSoundCreateCalled = TRUE;
		}
		else
			EmuWarning("DirectSound not initialized!");
	}

    HRESULT hRet = g_pDSound8->CreateSoundBuffer(pDSBufferDesc, &(*ppStream)->EmuDirectSoundBuffer8, NULL);

    if(FAILED(hRet))
        EmuWarning("CreateSoundBuffer Failed!");

//	__asm int 3

    // cache this sound stream
    {
        int v=0;
        for(v=0;v<SOUNDSTREAM_CACHE_SIZE;v++)
        {
            if(g_pDSoundStreamCache[v] == 0)
            {
                g_pDSoundStreamCache[v] = *ppStream;
                break;
            }
        }

        if(v == SOUNDSTREAM_CACHE_SIZE)
            CxbxKrnlCleanup("SoundStream cache out of slots!");
    }

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_CreateStream
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_CreateStream)
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream,
    PVOID                   pUnknown
)
{
        DbgPrintf("EmuDSound: EmuIDirectSound8_CreateStream\n"
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
	// Causes deadlock in Halo...
	// TODO: Verify that this is a Vista related problem (I HATE Vista!)
//    EmuWarning("EmuCMcpxStream_Dummy_0x10 is ignored!");
    return;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_SetVolume)(X_CDirectSoundStream *pThis, LONG lVolume)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetVolume\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   lVolume                   : %d\n"
           ");\n",
           pThis, lVolume);

    // TODO: Actually SetVolume

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetRolloffFactor)
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    DWORD                 dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetRolloffFactor\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fRolloffFactor            : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fRolloffFactor, dwApply);

    // TODO: Actually SetRolloffFactor

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_AddRef)(X_CDirectSoundStream *pThis)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_AddRef\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    if(pThis != 0)
        if(pThis->EmuDirectSoundBuffer8 != 0) // HACK: Ignore unsupported codecs.
            pThis->EmuDirectSoundBuffer8->AddRef();

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(CDirectSoundStream_Release)(X_CDirectSoundStream *pThis)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_Release\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    ULONG uRet = 0;

    if(pThis != 0 && (pThis->EmuDirectSoundBuffer8 != 0))
    {
        uRet = pThis->EmuDirectSoundBuffer8->Release();

        if(uRet == 0)
        {
            // remove cache entry
            for(int v=0;v<SOUNDSTREAM_CACHE_SIZE;v++)
            {
                if(g_pDSoundStreamCache[v] == pThis)
                    g_pDSoundStreamCache[v] = 0;
            }

            if(pThis->EmuBufferDesc->lpwfxFormat != NULL)
                CxbxFree(pThis->EmuBufferDesc->lpwfxFormat);

            CxbxFree(pThis->EmuBufferDesc);

            delete pThis;
        }
    }

    

    return uRet;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetInfo
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_GetInfo)
(
	X_CDirectSoundStream*	pThis, 
	LPXMEDIAINFO			pInfo
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_GetInfo\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pInfo                     : 0x%.08X\n"
           ");\n",
           pThis, pInfo);

	// TODO: A (real) implementation?
	EmuWarning("EmuCDirectSoundStream_GetInfo is not yet supported!");

	if( pInfo )
	{
		pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
		pInfo->dwInputSize = 0x40000;
		pInfo->dwOutputSize = 0x40000;
		pInfo->dwMaxLookahead = 0x4000;
	}

		

	return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_GetStatus)
(
    X_CDirectSoundStream   *pThis,
    DWORD                  *pdwStatus
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_GetStatus\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pdwStatus                 : 0x%.08X\n"
           ");\n",
           pThis, pdwStatus);

    EmuWarning("EmuCDirectSoundStream_GetStatus is not yet implemented");

    *pdwStatus = DSBSTATUS_PLAYING;

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Process)
(
    X_CDirectSoundStream   *pThis,
    PXMEDIAPACKET           pInputBuffer,
    PXMEDIAPACKET           pOutputBuffer
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_Process\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pInputBuffer              : 0x%.08X\n"
           "   pOutputBuffer             : 0x%.08X\n"
           ");\n",
           pThis, pInputBuffer, pOutputBuffer);

    if(pThis->EmuDirectSoundBuffer8 != NULL)
    {
        // update buffer data cache
        pThis->EmuBuffer = pInputBuffer->pvBuffer;

        EmuResizeIDirectSoundStream8(pThis, pInputBuffer->dwMaxSize);

        if(pInputBuffer->pdwStatus != 0)
            *pInputBuffer->pdwStatus = S_OK;

        HackUpdateSoundStreams();
    }
    else
    {
        if(pInputBuffer->pdwStatus != 0)
            *pInputBuffer->pdwStatus = S_OK;
    }

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Discontinuity)(X_CDirectSoundStream *pThis)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_Discontinuity\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

    // TODO: Actually Process

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Flush)(X_CDirectSoundStream *pThis)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_Flush\n"
		   "(\n"
		   "   pThis           : 0x%.08X\n"
		   ");\n",
           pThis);

    // TODO: Actually Flush

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSound_SynchPlayback
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSound_SynchPlayback)(PVOID pUnknown)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSound_SynchPlayback\n"
		   "(\n"
		   "   pUnknown           : 0x%.08X\n"
		   ");\n",
		   pUnknown);

    

    return DS_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_Pause)
(
    PVOID   pStream,
    DWORD   dwPause
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_Pause\n"
           "(\n"
           "   pStream                   : 0x%.08X\n"
           "   dwPause                   : 0x%.08X\n"
           ");\n",
           pStream, dwPause);

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetHeadroom)
(
    PVOID   pThis,
    DWORD   dwHeadroom
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundStream_SetHeadroom\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwHeadroom                : 0x%.08X\n"
           ");\n",
           pThis, dwHeadroom);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeAngles)
(
    PVOID   pThis,
    DWORD   dwInsideConeAngle,
    DWORD   dwOutsideConeAngle,
    DWORD   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetConeAngles\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwInsideConeAngle         : 0x%.08X\n"
           "   dwOutsideConeAngle        : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, dwInsideConeAngle, dwOutsideConeAngle, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeOutsideVolume)
(
    PVOID   pThis,
    LONG    lConeOutsideVolume,
    DWORD   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetConeOutsideVolume\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   lConeOutsideVolume        : %d\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, lConeOutsideVolume, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetAllParameters)
(
    PVOID    pThis,
    PVOID    pUnknown,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetAllParameters\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pUnknown                  : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pUnknown, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMaxDistance)
(
    PVOID    pThis,
    D3DVALUE fMaxDistance,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetMaxDistance\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fMaxDistance              : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fMaxDistance, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMinDistance)
(
    PVOID    pThis,
    D3DVALUE fMinDistance,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetMinDistance\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   fMinDistance              : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, fMinDistance, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetVelocity)
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetVelocity\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   x                         : %f\n"
           "   y                         : %f\n"
           "   z                         : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, x, y, z, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetConeOrientation)
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetConeOrientation\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   x                         : %f\n"
           "   y                         : %f\n"
           "   z                         : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, x, y, z, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetPosition)
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetPosition\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   x                         : %f\n"
           "   y                         : %f\n"
           "   z                         : %f\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, x, y, z, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFrequency)
(
    PVOID   pThis,
    DWORD   dwFrequency
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetFrequency\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwFrequency               : %d\n"
           ");\n",
           pThis, dwFrequency);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetI3DL2Source)
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundStream_SetI3DL2Source\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pds3db                    : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pds3db, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBins
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBins)
(
    PVOID   pThis,
    PVOID   pMixBins
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetMixBins\n"
            "(\n"
            "   pThis                     : 0x%.08X\n"
            "   pMixBins                  : 0x%.08X\n"
            ");\n",
            pThis, pMixBins);

    // TODO: Actually implement this.

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_Unknown1
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_Unknown1)
(
    PVOID   pThis,
    DWORD   dwUnknown1
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundStream_Unknown1\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwUnknown1                : 0x%.08X\n"
           ");\n",
           pThis, dwUnknown1);

    // TODO: Actually implement this

    

    return S_OK;
}

// s+
// ******************************************************************
// * patch: IDirectSoundBuffer8_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetMaxDistance)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flMaxDistance,
    DWORD                   dwApply
)
{
	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetMaxDistance\n"
		"(\n"
		"   pThis                     : 0x%.08X\n"
		"   flMaxDistance             : %f\n"
		"   dwApply                   : 0x%.08X\n"
		");\n",
		pThis, flMaxDistance, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetMinDistance)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flMinDistance,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetMinDistance\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   flMinDistance             : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, flMinDistance, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetRolloffFactor)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flRolloffFactor,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetRolloffFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   flRolloffFactor           : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, flRolloffFactor, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetDistanceFactor)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flDistanceFactor,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetDistanceFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   flDistanceFactor          : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, flDistanceFactor, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetConeAngles)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    DWORD                   dwInsideConeAngle,
    DWORD                   dwOutsideConeAngle,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetConeAngles\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwInsideConeAngle         : 0x%.08X\n"
               "   dwOutsideConeAngle        : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, dwInsideConeAngle,
               dwOutsideConeAngle, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetConeOrientation)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetConeOrientation\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, x, y, z, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetConeOutsideVolume)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    LONG                    lConeOutsideVolume,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetConeOutsideVolume\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   lConeOutsideVolume        : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, lConeOutsideVolume, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetPosition)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, x, y, z, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetVelocity)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetVelocity\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, x, y, z, dwApply);

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetDopplerFactor)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    FLOAT                   flDopplerFactor,
    DWORD                   dwApply
)
{
        
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetConeOutsideVolume\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   flDopplerFactor           : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, flDopplerFactor, dwApply);
        

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetI3DL2Source)
(
    LPDIRECTSOUNDBUFFER8    pThis,
    LPCDSI3DL2BUFFER        pds3db,
    DWORD                   dwApply
)
{
        
        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetI3DL2Source\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pds3db                    : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               pThis, pds3db, dwApply);
        

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetMode)
(
    X_CDirectSoundBuffer   *pBuffer,
    DWORD                   dwMode,
    DWORD                   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetFormat\n"
           "(\n"
           "   pBuffer             : 0x%.08X\n"
           "   dwMode              : 0x%.08X\n"
           "   dwApply             : 0x%.08X\n"
           ");\n",
           pBuffer, dwMode, dwApply);

    HRESULT hRet = DS_OK;

    EmuWarning("EmuIDirectSoundBuffer8_SetMode ignored");

    

    return hRet;
}

// +s
// ******************************************************************
// * patch: IDirectSoundBuffer8_SetFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetFormat)
(
    X_CDirectSoundBuffer *pBuffer,
    LPCWAVEFORMATEX pwfxFormat
)
{
    

        DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetFormat\n"
               "(\n"
               "   pBuffer                   : 0x%.08X\n"
               "   pwfxFormat                : 0x%.08X\n"
               ");\n",
               pBuffer,pwfxFormat);

    HRESULT hRet = DS_OK;

    

    return hRet;
}

// ******************************************************************
// * patch: DirectSoundUseFullHRTF
// ******************************************************************
STDAPI_(void) EMUPATCH(DirectSoundUseFullHRTF)
(
    void
)
{
    

    DbgPrintf("EmuDSound: EmuDirectSoundUseFullHRTF()\n");

    // TODO: Actually implement this

    
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetLFO
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetLFO)
(
    LPDIRECTSOUNDBUFFER  pThis,
    LPCDSLFODESC         pLFODesc
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetLFO\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pLFODesc                  : 0x%.08X\n"
           ");\n",
           pThis, pLFODesc);

    // TODO: Implement

    

    return S_OK;
}

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XAudioCreateAdpcmFormat)
(
    WORD                   nChannels,
    DWORD                  nSamplesPerSec,
    LPXBOXADPCMWAVEFORMAT  pwfx
)
{
    

    DbgPrintf("EmuDSound: EmuXAudioCreateAdpcmFormat\n"
           "(\n"
           "   nChannels                 : 0x%.04X\n"
           "   nSamplesPerSec            : 0x%.08X\n"
           "   pwfx                      : 0x%.08X\n"
           ");\n",
           nChannels, nSamplesPerSec, pwfx);

    // Fill out the pwfx structure with the appropriate data
    pwfx->wfx.wFormatTag        = WAVE_FORMAT_XBOX_ADPCM;
    pwfx->wfx.nChannels         = nChannels;
    pwfx->wfx.nSamplesPerSec    = nSamplesPerSec;
    pwfx->wfx.nAvgBytesPerSec   = (nSamplesPerSec*nChannels * 36)/64;
    pwfx->wfx.nBlockAlign       = nChannels * 36;
    pwfx->wfx.wBitsPerSample    = 4;
    pwfx->wfx.cbSize            = 2;
    pwfx->wSamplesPerBlock      = 64;

    
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetRolloffCurve
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetRolloffCurve)
(
    LPDIRECTSOUNDBUFFER  pThis,
    const FLOAT         *pflPoints,
    DWORD                dwPointCount,
    DWORD                dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetRolloffCurve\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pflPoints                 : 0x%.08X\n"
           "   dwPointCount              : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pflPoints, dwPointCount, dwApply);

    // TODO: Implement

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_SetVolume)
(
    LPDIRECTSOUNDSTREAM pStream,
    LONG                lVolume
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundStream_SetVolume\n"
           "(\n"
           "   pStream                   : 0x%.08X\n"
           "   lVolume                   : 0x%.08X\n"
           ");\n",
           pStream, lVolume);

    // TODO: Implement

    

    return DS_OK;
}


// ******************************************************************
// * patch: IDirectSound_EnableHeadphones
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound_EnableHeadphones)
(
	LPDIRECTSOUND		pThis,
	BOOL				fEnabled
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSound_EnableHeadphones\n"
			"(\n"
			"	pThis					: 0x%.08X\n"
			"   fEnabled				: 0x%.08X\n"
			");\n",
			pThis, fEnabled);

		

	return DS_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_AddRef)
(
    X_CDirectSoundBuffer   *pThis
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer_AddRef\n"
			"(\n"
			"   pThis                   : 0x%.08X\n"
			");\n",
			pThis);
	
	ULONG ret = 0;

	if(pThis != 0)
	{
		// HACK: Skip this on unsupported flags
		if(pThis->EmuFlags & DSB_FLAG_RECIEVEDATA)
		{
			EmuWarning("Not adding reference to a potentially pad pointer!");
		}
		else
		{
			if(pThis->EmuDirectSoundBuffer8 != 0) // HACK: Ignore unsupported codecs.
				ret = pThis->EmuDirectSoundBuffer8->AddRef();
		}
	}

		

	return ret;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_Pause
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_Pause)
(
    X_CDirectSoundBuffer   *pThis,
	DWORD					dwPause
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer_Pause\n"
			"(\n"
			"	pThis					: 0x%.08X\n"
			"   dwPause                 : 0x%.08X\n"
			");\n",
			pThis, dwPause);

	// This function wasn't part of the XDK until 4721.
	HRESULT ret = S_OK;

	// Unstable!
	/*if(pThis != NULL)
	{
		if(pThis->EmuDirectSoundBuffer8)
		{
			if(dwPause == X_DSBPAUSE_PAUSE)
				ret = pThis->EmuDirectSoundBuffer8->Stop();
			if(dwPause == X_DSBPAUSE_RESUME)
			{
				DWORD dwFlags = (pThis->EmuPlayFlags & X_DSBPLAY_LOOPING) ? DSBPLAY_LOOPING : 0;
				ret = pThis->EmuDirectSoundBuffer8->Play(0, 0, dwFlags);
			}
			if(dwPause == X_DSBPAUSE_SYNCHPLAYBACK)
				EmuWarning("DSBPAUSE_SYNCHPLAYBACK is not yet supported!");
		}
	}*/

		

	return ret;
}

//// ******************************************************************
//// * patch: IDirectSoundBuffer_Pause
//// ******************************************************************
//extern "C" HRESULT __stdcall XTL::EmuIDirectSoundBuffer_PauseEx
//(
//    X_CDirectSoundBuffer   *pThis,
//	REFERENCE_TIME			rtTimestamp,
//	DWORD					dwPause
//)
//{
//		
//
//	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer_PauseEx\n"
//			"(\n"
//			"	pThis					: 0x%.08X\n"
//			"   rtTimestamp             : 0x%.08X\n"
//			"   dwPause                 : 0x%.08X\n"
//			");\n",
//			pThis, rtTimestamp, dwPause);
//	
//	// This function wasn't part of the XDK until 4721.
//	// TODO: Implement time stamp feature (a thread maybe?)
//	EmuWarning("IDirectSoundBuffer_PauseEx not fully implemented!");
//
//	HRESULT ret;
//
//	if(pThis != NULL)
//	{
//		if(pThis->EmuDirectSoundBuffer8)
//		{
//			if(dwPause == X_DSBPAUSE_PAUSE)
//				ret = pThis->EmuDirectSoundBuffer8->Stop();
//			if(dwPause == X_DSBPAUSE_RESUME)
//			{
//				DWORD dwFlags = (pThis->EmuPlayFlags & X_DSBPLAY_LOOPING) ? DSBPLAY_LOOPING : 0;
//				ret = pThis->EmuDirectSoundBuffer8->Play(0, 0, dwFlags);
//			}
//			if(dwPause == X_DSBPAUSE_SYNCHPLAYBACK)
//				EmuWarning("DSBPAUSE_SYNCHPLAYBACK is not yet supported!");
//		}
//	}
//
//		
//
//	return ret;
//}

// ******************************************************************
// * patch: IDirectSound8_GetOutputLevels
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_GetOutputLevels)
(
	LPDIRECTSOUND8		   *pThis,
	X_DSOUTPUTLEVELS	   *pOutputLevels,
	BOOL					bResetPeakValues
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSound8_GetOutputLevels\n"
			"(\n"
			"	pThis					: 0x%.08X\n"
			"   pOutputLevels           : 0x%.08X\n"
			"   bResetPeakValues        : 0x%.08X\n"
			");\n",
			pThis, pOutputLevels, bResetPeakValues);

	// TODO: Anything?  Either way, I've never seen a game to date use this...

	

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetEG)
(
	LPVOID		pThis,
	LPVOID		pEnvelopeDesc
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetEG\n"
			"(\n"
			"	pThis					: 0x%.08X\n"
			"   pEnvelopeDesc           : 0x%.08X\n"
			");\n",
			pThis, pEnvelopeDesc);

	// TODO: Implement this...

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_Flush
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_Flush)()
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundStream_Flush()\n");

	// TODO: Actually implement

	

	return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_FlushEx
// ******************************************************************
extern "C" HRESULT WINAPI XTL::EMUPATCH(IDirectSoundStream_FlushEx)
(
	X_CDirectSoundStream*	pThis,
	REFERENCE_TIME			rtTimeStamp,
	DWORD					dwFlags
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundStream_FlushEx\n"
			"(\n"
			"	pThis					: 0x%.08X\n"
			"   rtTimeStamp             : 0x%.08X\n"
			"   dwFlags                 : 0x%.08X\n"
			");\n",
			pThis, rtTimeStamp, dwFlags);

	// TODO: Actually implement

		

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMode
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMode)
(
    X_CDirectSoundStream   *pStream,
    DWORD                   dwMode,
    DWORD                   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetFormat\n"
           "(\n"
           "   pStream             : 0x%.08X\n"
           "   dwMode              : 0x%.08X\n"
           "   dwApply             : 0x%.08X\n"
           ");\n",
           pStream, dwMode, dwApply);

    HRESULT hRet = DS_OK;

    EmuWarning("EmuCDirectSoundStream_SetMode ignored");

    

    return hRet;
}

// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR		pszImageName,
    LPVOID		pImageLoc,
    DWORD		dwFlags,
    LPVOID	   *ppImageDesc
)
{
		

	DbgPrintf("EmuDSound: EmuXAudioDownloadEffectsImage\n"
           "(\n"
           "   pszImageName        : 0x%.08X\n"
		   "   pImageLoc           : 0x%.08X\n"
		   "   dwFlags             : 0x%.08X\n"
		   "   ppImageDesc         : 0x%.08X\n"
		   ");\n",
		   pszImageName, pImageLoc, dwFlags, ppImageDesc );

	 	

	 return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetFilter)
(
	LPVOID			pThis,
	X_DSFILTERDESC* pFilterDesc
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetFilter\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   pFilterDesc         : 0x%.08X\n"
		   ");\n",
		   pThis, pFilterDesc);

	// TODO: Implement

	EmuWarning("IDirectSoundBuffer8_SetFilter not yet supported!");

		

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetFilter
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFilter)
(
	X_CDirectSoundStream*	pThis,
	X_DSFILTERDESC*			pFilterDesc
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetFilter\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   pFilterDesc         : 0x%.08X\n"
		   ");\n",
		   pThis, pFilterDesc);

	// TODO: Implement

	EmuWarning("CDirectSoundStream_SetFilter not yet supported!");

		

	return S_OK;
}


// ******************************************************************
// * patch: IDirectSoundBuffer8_PlayEx
// ******************************************************************
extern "C" HRESULT __stdcall XTL::EMUPATCH(IDirectSoundBuffer8_PlayEx)
(
    X_CDirectSoundBuffer *pBuffer,
    REFERENCE_TIME        rtTimeStamp,
    DWORD                 dwFlags
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_PlayEx\n"
           "(\n"
           "   pBuffer                   : 0x%.08X\n"
           "   rtTimeStamp               : 0x%.08X\n"
           "   dwFlags                   : 0x%.08X\n"
           ");\n",
           pBuffer, rtTimeStamp, dwFlags);

    if(pBuffer->EmuDirectSoundBuffer8 == 0)
        EmuWarning("pBuffer->EmuDirectSoundBuffer8 == 0");

//    EmuWarning("PlayEx not yet implemented!");

	// TODO: Handle other non-PC standard flags
	DWORD dwPCFlags = ( dwFlags & DSBPLAY_LOOPING ) ? DSBPLAY_LOOPING : 0;
	HRESULT hr = pBuffer->EmuDirectSoundBuffer8->Play( 0, 0, dwPCFlags );

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_GetCaps
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_GetCaps)
(
	X_CDirectSound*	pThis,
    X_DSCAPS*		pDSCaps
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSound8_GetCaps\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   pDSCaps             : 0x%.08X\n"
		   ");\n",
		   pThis, pDSCaps);

	// Get PC's DirectSound capabilities
	DSCAPS DSCapsPC;
	ZeroMemory( &DSCapsPC, sizeof( DSCAPS ) );

	HRESULT hRet = g_pDSound8->GetCaps( &DSCapsPC );
	if(FAILED(hRet))
		EmuWarning("Failed to get PC DirectSound caps!");

	// Convert PC -> Xbox
	if(pDSCaps)
	{
		// WARNING: This may not be accurate under Windows Vista...
		pDSCaps->dwFree2DBuffers	= DSCapsPC.dwFreeHwMixingAllBuffers;
		pDSCaps->dwFree3DBuffers	= DSCapsPC.dwFreeHw3DAllBuffers;
		pDSCaps->dwFreeBufferSGEs	= 256;							// TODO: Verify max on a real Xbox
		pDSCaps->dwMemoryAllocated	= DSCapsPC.dwFreeHwMemBytes;	// TODO: Bytes or MegaBytes?
	}	

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundStream_SetPitch
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetPitch)
(	
	X_CDirectSoundStream*	pThis,
    LONG					lPitch
)
{
    

    DbgPrintf("EmuDSound: EmuIDirectSoundStream_SetPitch\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   lPitch              : 0x%.08X\n"
		   ");\n",
		   pThis, lPitch);

	HRESULT hRet = S_OK;

	EmuWarning("IDirectSoundStream_SetPitch not yet implemented!");

		

	return hRet;
}

// ******************************************************************
// * patch: DirectSoundGetSampleTime
// ******************************************************************
DWORD WINAPI XTL::EMUPATCH(DirectSoundGetSampleTime)()
{
		
	
	DbgPrintf("EmuDSound: EmuDirectSoundGetSampleTime();\n");

	// FIXME: This is the best I could think of for now.
	// Check the XDK documentation for the description of what this function 
	// can actually do.  BTW, this function accesses the NVIDIA SoundStorm APU
	// register directly (0xFE80200C).

	// TODO: Handle reset at certain event?
	// TODO: Wait until a DirectSoundBuffer/Stream is being played?
	static DWORD dwStart = GetTickCount();
	DWORD dwRet = GetTickCount() - dwStart;

	

	return 0;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes)
(
	X_CDirectSoundStream*	pThis,
    DWORD					dwMixBinMask,
    const LONG*				alVolumes
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetMixBinVolumes\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   dwMixBinMask        : 0x%.08X\n"
		   "   alVolumes           : 0x%.08X\n"
		   ");\n",
		   pThis, dwMixBinMask, alVolumes);

	// NOTE: Use this function for XDK 3911 only because the implementation was changed
	// somewhere around the March 2002 (4361) update (or earlier, maybe).

		

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetMixBinVolumes2
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetMixBinVolumes2)
(
	X_CDirectSoundStream*	pThis,
    LPVOID					pMixBins
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetMixBinVolumes\n"
           "(\n"
           "   pThis               : 0x%.08X\n"
		   "   pMixBins            : 0x%.08X\n"
		   ");\n",
		   pThis, pMixBins);

	// NOTE: Read the above notes, and the rest is self explanitory...

		

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetI3DL2Source)
(
    X_CDirectSoundStream*   pThis,
    PVOID   pds3db,
    DWORD   dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetI3DL2Source\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pds3db                    : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pds3db, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetAllParameters)
(
	X_CDirectSoundBuffer*	pThis,
    VOID*					pcDs3dBuffer,
    DWORD					dwApply
)
{
	

    DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetAllParameters\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pds3db                    : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pcDs3dBuffer, dwApply);

    // TODO: Actually implement this

    

    return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream::SetFormat
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetFormat)
(
	X_CDirectSoundStream*	pThis,
    LPCWAVEFORMATEX			pwfxFormat
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetFormat\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pwfxFormat                : 0x%.08X\n"
           ");\n",
           pThis, pwfxFormat);

    // TODO: Actually implement this

	// NOTE: pwfxFormat is not always a WAVEFORMATEX structure, it can
	// be WAVEFORMATEXTENSIBLE if that's what the programmer(s) wanted
	// in the first place, FYI.

//	if(pThis)
//		pThis->EmuDirectSoundBuffer8->SetFormat(pwfxFormat);

    

    return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetOutputBuffer)
(
	X_CDirectSoundBuffer*	pThis,
    X_CDirectSoundBuffer*	pOutputBuffer
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetOutputBuffer\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pOutputBuffer             : 0x%.08X\n"
           ");\n",
           pThis, pOutputBuffer);

	// TODO: Implement

		

	return S_OK;
}

// ******************************************************************
// * patch: CDirectSoundStream_SetOutputBuffer
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetOutputBuffer)
(
	X_CDirectSoundStream*	pThis,
	X_CDirectSoundBuffer*	pOutputBuffer
)
{
		

	DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetOutputBuffer\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pOutputBuffer             : 0x%.08X\n"
           ");\n",
           pThis, pOutputBuffer);

	// TODO: Implement

		

	return S_OK;
}

// ******************************************************************
// * patch: XFileCreaeMediaObjectEx
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObjectEx)
(
    HANDLE	hFile,
    void**	ppMediaObject
)
{
		

	DbgPrintf("EmuDSound: EmuXFileCreateMediaObjectEx\n"
           "(\n"
           "   hFile                     : 0x%.08X\n"
           "   ppMediaObject             : 0x%.08X\n"
           ");\n",
           hFile, ppMediaObject);

	// TODO: Implement

		

	return E_FAIL;
}

// ******************************************************************
// * patch: XWaveFileCreateMediaObject
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XWaveFileCreateMediaObject)
(
    LPCSTR			pszFileName,
    LPCWAVEFORMATEX *ppwfxFormat,
    void			**ppMediaObject
)
{
		

	DbgPrintf("EmuDSound: EmuXWaveFileCreateMediaObject\n"
           "(\n"
		   "   pszFileName               : (%s)\n"
		   "   ppwfxFormat               : 0x%.08X\n"
           "   ppMediaObject             : 0x%.08X\n"
           ");\n",
           pszFileName, ppwfxFormat, ppMediaObject);

	// TODO: Implement

		

	return E_FAIL;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetEG
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetEG)
(
	X_CDirectSoundBuffer*	pThis,
    LPVOID					pEnvelopeDesc
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetEG\n"
           "(\n"
		   "   pThis                     : 0x%.08X\n"
           "   pEnvelopeDesc             : 0x%.08X\n"
           ");\n",
           pThis, pEnvelopeDesc);

	// TODO: Implement

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirectSound8_GetEffectData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_GetEffectData)
(
	X_CDirectSound*	pThis,
    DWORD			dwEffectIndex,
    DWORD			dwOffset,
    LPVOID			pvData,
    DWORD			dwDataSize
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSound8_GetEffectData\n"
           "(\n"
		   "   pThis                     : 0x%.08X\n"
           "   dwEffectIndex             : 0x%.08X\n"
		   "   dwOffset                  : 0x%.08X\n"
		   "   pvData                    : 0x%.08X\n"
		   "   dwDataSize                : 0x%.08X\n"
           ");\n",
           pThis, dwEffectIndex, dwOffset, pvData, dwDataSize);

	// TODO: Implement
	if( !pvData )
		pvData = CxbxMalloc( dwDataSize );

		

	return S_OK;
}

// ******************************************************************
// * patch: IDirectSoundBuffer8_SetNotificationPositions
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSoundBuffer8_SetNotificationPositions)
(
	X_CDirectSoundBuffer*	pThis,
    DWORD					dwNotifyCount,
    LPCDSBPOSITIONNOTIFY	paNotifies
)
{
		

	DbgPrintf("EmuDSound: EmuIDirectSoundBuffer8_SetNotificationPositions\n"
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

	if( pThis )
	{
		if( pThis->EmuDirectSoundBuffer8 )
		{
			hr = pThis->EmuDirectSoundBuffer8->QueryInterface( IID_IDirectSoundNotify, (LPVOID*) pNotify );
			if( SUCCEEDED( hr ) && pNotify != nullptr )
			{
				hr = pNotify->SetNotificationPositions( dwNotifyCount, paNotifies );
				if( FAILED( hr ) )
					EmuWarning( "Could not set notification position(s)!" );

				pNotify->Release();
			}
			else
				EmuWarning( "Could not create notification interface!" );
		}
	}

		

	return hr;
}

// ******************************************************************
// * patch: CDirectSoundStream::SetRolloffCurve
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(CDirectSoundStream_SetRolloffCurve)
(
	X_CDirectSoundBuffer	*pThis,
    const FLOAT				*pflPoints,
    DWORD					dwPointCount,
    DWORD					dwApply
)
{
    

    DbgPrintf("EmuDSound: EmuCDirectSoundStream_SetRolloffCurve\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   pflPoints                 : 0x%.08X\n"
           "   dwPointCount              : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, pflPoints, dwPointCount, dwApply);

    // TODO: Implement

    

    return DS_OK;
}

// ******************************************************************
// * patch: IDirectSound8_SetEffectData
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(IDirectSound8_SetEffectData)
(
	LPVOID pThis,
    DWORD dwEffectIndex,
    DWORD dwOffset,
    LPCVOID pvData,
    DWORD dwDataSize,
    DWORD dwApply
)
{
	

    DbgPrintf("EmuDSound: EmuIDirectSound8_SetEffectData\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           "   dwEfectIndex              : 0x%.08X\n"
           "   dwOffset                  : 0x%.08X\n"
		   "   pvData                    : 0x%.08X\n"
		   "   dwDataSize                : 0x%.08X\n"
           "   dwApply                   : 0x%.08X\n"
           ");\n",
           pThis, dwEffectIndex, dwOffset, pvData, dwDataSize, dwApply);

    // TODO: Implement

    

    return DS_OK;
}

// ******************************************************************
// * patch: XFileCreateMediaObjectAsync
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE	hFile,
    DWORD	dwMaxPackets,
    void	**ppMediaObject
)
{
		

	DbgPrintf("EmuDSound: EmuXFileCreateMediaObjectAsync\n"
           "(\n"
           "   hFile                     : 0x%.08X\n"
		   "   dwMaxPackets              : 0x%.08X\n"
           "   ppMediaObject             : 0x%.08X\n"
           ");\n",
           hFile, dwMaxPackets, ppMediaObject);

	// TODO: Implement

	EmuWarning( "XFileCreateMediaObjectAsync not yet (properly) implemented!!!" );

	*ppMediaObject = new X_XFileMediaObject();

		

	return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Seek
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Seek)
(
	X_XFileMediaObject* pThis,
    LONG				lOffset,
    DWORD				dwOrigin,
    LPDWORD				pdwAbsolute
)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_Seek\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   lOffset                   : 0x%.08X\n"
           "   dwOrigin                  : 0x%.08X\n"
		   "   pdwAbsolute               : 0x%.08X\n"
           ");\n",
           pThis, lOffset, dwOrigin, pdwAbsolute);

	// TODO: Implement

		

	return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_DoWork\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

	// TODO: Implement

		
}

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetStatus)
(
	X_XFileMediaObject* pThis,
    LPDWORD				pdwStatus
)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_GetStatus\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pdwStatus                 : 0x%.08X\n"
           ");\n",
           pThis, pdwStatus);

	// TODO: Implement

		

	return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_GetInfo
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_GetInfo)
(
	X_XFileMediaObject     *pThis,
	XMEDIAINFO			   *pInfo
)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_GetStatus\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pInfo                     : 0x%.08X\n"
           ");\n",
           pThis, pInfo);

	// TODO: Implement

		

	return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_Process
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Process)
(
	X_XFileMediaObject	   *pThis,
    LPXMEDIAPACKET			pInputBuffer, 
    LPXMEDIAPACKET			pOutputBuffer
)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_Process\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
		   "   pInputBuffer              : 0x%.08X\n"
		   "   pOutputBuffer             : 0x%.08X\n"
           ");\n",
           pThis, pInputBuffer, pOutputBuffer);

	// TODO: Implement

		

	return DS_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_AddRef
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_AddRef)(X_XFileMediaObject *pThis)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_AddRef\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

	ULONG Ret = 0;

	if( pThis )
	{
		pThis->EmuRefCount++;
		Ret = pThis->EmuRefCount;
	}

		

	return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Release
// ******************************************************************
ULONG WINAPI XTL::EMUPATCH(XFileMediaObject_Release)(X_XFileMediaObject *pThis)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_Release\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

	ULONG Ret = 0;

	if( pThis )
	{
		pThis->EmuRefCount--;

		if( pThis->EmuRefCount < 1 )
			delete pThis;

		Ret = pThis->EmuRefCount;
	}

		

	return Ret;
}

// ******************************************************************
// * patch: XFileMediaObject_Discontinuity
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XFileMediaObject_Discontinuity)(X_XFileMediaObject *pThis)
{
		

	DbgPrintf("EmuDSound: EmuXFileMediaObject_Discontinuity\n"
           "(\n"
           "   pThis                     : 0x%.08X\n"
           ");\n",
           pThis);

		

	return DS_OK;
}