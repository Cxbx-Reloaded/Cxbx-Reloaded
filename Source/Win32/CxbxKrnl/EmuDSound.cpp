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
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuShared.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace XTL
{
    #include "EmuXTL.h"
};

#include "ResCxbxDll.h"

#include <process.h>
#include <locale.h>

// ******************************************************************
// * Static Variable(s)
// ******************************************************************
static XTL::LPDIRECTSOUND8 g_pDSound8 = NULL;

// ******************************************************************
// * EmuStatic Variable(s)
// ******************************************************************
XTL::X_CDirectSoundStream::_vtbl XTL::X_CDirectSoundStream::vtbl = 
{
    &XTL::EmuCDirectSoundStream_AddRef,         // 0x00 - AddRef
    &XTL::EmuCDirectSoundStream_Release,        // 0x04
    {0xCDCDCDCD, 0xCDCDCDCD},                   // 0x08 - Unknown
    &XTL::EmuCDirectSoundStream_Process,        // 0x10 - Process
    &XTL::EmuCDirectSoundStream_Discontinuity   // 0x14 - Discontinuity
};

// ******************************************************************
// * func: EmuDirectSoundCreate
// ******************************************************************
HRESULT WINAPI XTL::EmuDirectSoundCreate
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuDirectSoundCreate\n"
               "(\n"
               "   pguidDeviceId             : 0x%.08X\n"
               "   ppDirectSound             : 0x%.08X\n"
               "   pUnknown                  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pguidDeviceId, ppDirectSound, pUnknown);
    }
    #endif

    HRESULT hRet = DirectSoundCreate8(NULL, ppDirectSound, NULL);

    g_pDSound8 = *ppDirectSound;

    hRet = g_pDSound8->SetCooperativeLevel(g_hEmuWindow, DSSCL_PRIORITY);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuDirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EmuDirectSoundCreateBuffer
(
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuDirectSoundCreateBuffer\n"
               "(\n"
               "   pdsbd                     : 0x%.08X\n"
               "   ppBuffer                  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pdsbd, ppBuffer);
    }
    #endif

    DSBUFFERDESC DSBufferDesc;

    // ******************************************************************
    // * Convert from Xbox to PC DSound
    // ******************************************************************
    {
        DWORD dwAcceptableMask = 0x00000010 | 0x00000020 | 0x00000080 | 0x00000100 | 0x00002000 | 0x00040000 | 0x00080000;

        if(pdsbd->dwFlags & (~dwAcceptableMask))
            printf("*Warning* use of unsupported pdsbd->dwFlags mask(s) (0x%.08X)\n", pdsbd->dwFlags & (~dwAcceptableMask));

        DSBufferDesc.dwSize = sizeof(DSBufferDesc);
        DSBufferDesc.dwFlags = pdsbd->dwFlags & dwAcceptableMask;

        if(pdsbd->dwBufferBytes < DSBSIZE_MIN)
            DSBufferDesc.dwBufferBytes = 16384; // NOTE: HACK: TEMPORARY FOR STELLA
        else
            DSBufferDesc.dwBufferBytes = pdsbd->dwBufferBytes;

        DSBufferDesc.dwReserved = 0;
        DSBufferDesc.lpwfxFormat = pdsbd->lpwfxFormat;              // TODO: Make sure this is the same as PC
        DSBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    }

    // Todo: Garbage Collection
    *ppBuffer = new X_CDirectSoundBuffer();

    HRESULT hRet = g_pDSound8->CreateSoundBuffer(&DSBufferDesc, &((*ppBuffer)->EmuDirectSoundBuffer8), NULL);

    if(FAILED(hRet))
        EmuWarning("CreateSoundBuffer FAILED");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuDirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI XTL::EmuDirectSoundCreateStream
(
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuDirectSoundCreateStream\n"
               "(\n"
               "   pdssd                     : 0x%.08X\n"
               "   ppStream                  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pdssd, ppStream);
    }
    #endif

    *ppStream = new X_CDirectSoundStream();

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_CreateStream
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_CreateStream
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream,
    PVOID                   pUnknown
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_CreateStream\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pdssd                     : 0x%.08X\n"
               "   ppStream                  : 0x%.08X\n"
               "   pUnknown                  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pdssd, ppStream, pUnknown);
    }
    #endif

    *ppStream = new X_CDirectSoundStream();

    EmuSwapFS();   // XBox FS

    return DS_OK;
}


// ******************************************************************
// * func: EmuIDirectSound8_CreateBuffer
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_CreateBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdssd,
    X_CDirectSoundBuffer  **ppBuffer,
    PVOID                   pUnknown
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
       EmuSwapFS();   // Win2k/XP FS
       printf("EmuDSound (0x%X): EmuIDirectSound8_CreateBuffer\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pdssd                     : 0x%.08X\n"
               "   ppBuffer                  : 0x%.08X\n"
               "   pUnknown                  : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pdssd, ppBuffer, pUnknown);
       EmuSwapFS();   // XBox FS
    }
    #endif

    EmuDirectSoundCreateBuffer(pdssd, ppBuffer);

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI XTL::EmuCDirectSoundStream_SetVolume(X_CDirectSoundStream *pThis, LONG lVolume)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetVolume\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   lVolume                   : %d\n"
               ");\n",
               GetCurrentThreadId(), pThis, lVolume);
    }
    #endif

    // TODO: Actually SetVolume

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetRolloffFactor
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    DWORD                 dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetRolloffFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fRolloffFactor            : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fRolloffFactor, dwApply);
    }
    #endif

    // TODO: Actually SetRolloffFactor

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI XTL::EmuCDirectSoundStream_AddRef(X_CDirectSoundStream *pThis)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_AddRef\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    // TODO: Actually AddRef

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI XTL::EmuCDirectSoundStream_Release(X_CDirectSoundStream *pThis)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_Release\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    // TODO: Actually Release

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_Process
(
    X_CDirectSoundStream   *pThis,
    PVOID                   pInputBuffer,   // TODO: Fillout params
    PVOID                   pOutputBuffer   // TODO: Fillout params
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_Process\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pInputBuffer              : 0x%.08X\n"
               "   pOutputBuffer             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pInputBuffer, pOutputBuffer);
    }
    #endif

    // TODO: Actually Process

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_Discontinuity(X_CDirectSoundStream *pThis)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_Discontinuity\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    // TODO: Actually Process

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_Pause
(
    PVOID   pStream,
    DWORD   dwPause
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_Pause\n"
               "(\n"
               "   pStream                   : 0x%.08X\n"
               "   dwPause                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pStream, dwPause);
    }
    #endif

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_AddRef
// ******************************************************************
ULONG WINAPI XTL::EmuIDirectSound8_AddRef
(
    LPDIRECTSOUND8          pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_AddRef\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    ULONG uRet = pThis->AddRef();

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirectSound8_Release
// ******************************************************************
ULONG WINAPI XTL::EmuIDirectSound8_Release
(
    LPDIRECTSOUND8          pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_Release\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    ULONG uRet = pThis->Release();

    EmuSwapFS();   // XBox FS

    return uRet;
}

// ******************************************************************
// * func: EmuIDirectSound8_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_DownloadEffectsImage
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    DWORD                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_DownloadEffectsImage\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pvImageBuffer             : 0x%.08X\n"
               "   dwImageSize               : 0x%.08X\n"
               "   pImageLoc                 : 0x%.08X\n"
               "   ppImageDesc               : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pvImageBuffer, dwImageSize, pImageLoc, ppImageDesc);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundStream_SetHeadroom
(
    PVOID   pThis,
    DWORD   dwHeadroom
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundStream_SetHeadroom\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwHeadroom                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwHeadroom);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetConeAngles
(
    PVOID   pThis,
    DWORD   dwInsideConeAngle,
    DWORD   dwOutsideConeAngle,
    DWORD   dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetConeAngles\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwInsideConeAngle         : 0x%.08X\n"
               "   dwOutsideConeAngle        : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwInsideConeAngle, dwOutsideConeAngle, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetConeOutsideVolume
(
    PVOID   pThis,
    LONG    lConeOutsideVolume,
    DWORD   dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetConeOutsideVolume\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   lConeOutsideVolume        : %d\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, lConeOutsideVolume, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetAllParameters
(
    PVOID    pThis,
    PVOID    pUnknown,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetAllParameters\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pUnknown                  : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pUnknown, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetMaxDistance
(
    PVOID    pThis,
    D3DVALUE fMaxDistance,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetMaxDistance\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fMaxDistance              : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fMaxDistance, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetMinDistance
(
    PVOID    pThis,
    D3DVALUE fMinDistance,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetMinDistance\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fMinDistance              : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fMinDistance, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetVelocity
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetVelocity\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, x, y, z, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetConeOrientation
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetConeOrientation\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, x, y, z, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetPosition
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, x, y, z, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuCDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSoundStream_SetFrequency
(
    PVOID   pThis,
    DWORD   dwFrequency
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSoundStream_SetFrequency\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwFrequency               : %d\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwFrequency);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundStream_SetI3DL2Source
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundStream_SetI3DL2Source\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pds3db                    : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pds3db, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetOrientation
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetOrientation
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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetOrientation\n"
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
               GetCurrentThreadId(), pThis, xFront, yFront, zFront, xTop, yTop, zTop, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetDistanceFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetDistanceFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fDistanceFactor           : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fDistanceFactor, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetRolloffFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetRolloffFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fRolloffFactor            : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fRolloffFactor, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetDopplerFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetDopplerFactor\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   fDopplerFactor            : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, fDopplerFactor, dwApply);
    }
    #endif

    // TODO: Actually implement this

    EmuSwapFS();   // XBox FS

    return S_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_CreateSoundBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer,
    LPUNKNOWN               pUnkOuter
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_CreateSoundBuffer\n"
               "(\n"
               "   pdsbd                     : 0x%.08X\n"
               "   ppBuffer                  : 0x%.08X\n"
               "   pUnkOuter                 : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pdsbd, ppBuffer, pUnkOuter);
        EmuSwapFS();   // XBox FS
    }
    #endif

    return EmuDirectSoundCreateBuffer(pdsbd, ppBuffer);
}

// ******************************************************************
// * func: EmuIDirectSound8_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetI3DL2Listener
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetI3DL2Listener\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pDummy                    : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pDummy, dwApply);
        EmuSwapFS();   // XBox FS
    }
    #endif

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetMixBinHeadroom
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetMixBinHeadroom\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwMixBinMask              : 0x%.08X\n"
               "   dwHeadroom                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwMixBinMask, dwHeadroom);
        EmuSwapFS();   // XBox FS
    }
    #endif

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetPosition
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, x, y, z, dwApply);
        EmuSwapFS();   // XBox FS
    }
    #endif

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetVelocity
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetVelocity\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   x                         : %f\n"
               "   y                         : %f\n"
               "   z                         : %f\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, x, y, z, dwApply);
        EmuSwapFS();   // XBox FS
    }
    #endif

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSound8_SetAllParameters
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSound8_SetAllParameters
(
    LPDIRECTSOUND8          pThis,
    LPVOID                  pTodo,  // TODO: LPCDS3DLISTENER
    DWORD                   dwApply
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS
        printf("EmuDSound (0x%X): EmuIDirectSound8_SetAllParameters\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pTodo                     : 0x%.08X\n"
               "   dwApply                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pTodo, dwApply);
        EmuSwapFS();   // XBox FS
    }
    #endif

    // TODO: Actually do something

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetBufferData
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_SetBufferData
(
    X_CDirectSoundBuffer   *pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_SetBufferData\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pvBufferData              : 0x%.08X\n"
               "   dwBufferBytes             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pvBufferData, dwBufferBytes);
    }
    #endif

    // TODO: HACK: This should dynamically recreate the buffer when necessary (size change)
    PVOID pAudioPtr, pAudioPtr2;
    DWORD dwAudioBytes, dwAudioBytes2;

    HRESULT hRet = pThis->EmuDirectSoundBuffer8->Lock(0, dwBufferBytes, &pAudioPtr, &dwAudioBytes, &pAudioPtr2, &dwAudioBytes2, 0);

    if(SUCCEEDED(hRet))
    {
//        memcpy(pAudioPtr,  pvBufferData, dwAudioBytes);
//        memcpy(pAudioPtr2, (PVOID)((DWORD)pvBufferData+dwAudioBytes), dwAudioBytes2);

        pThis->EmuDirectSoundBuffer8->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
    }

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_SetPlayRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_SetPlayRegion\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwPlayStart               : 0x%.08X\n"
               "   dwPlayLength              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwPlayStart, dwPlayLength);
    }
    #endif

    // Todo: Translate params, then make the PC DirectSound call

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_SetLoopRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_SetLoopRegion\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwLoopStart               : 0x%.08X\n"
               "   dwLoopLength              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwLoopStart, dwLoopLength);
    }
    #endif

    // Todo: Translate params, then make the PC DirectSound call

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetVolume
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_SetVolume
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lVolume
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_SetVolume\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   lVolume                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, lVolume);
    }
    #endif

    // Todo: Translate params, then make the PC DirectSound call

    EmuSwapFS();   // XBox FS

    return DS_OK;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_SetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwNewPosition
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_SetCurrentPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwNewPosition             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwNewPosition);
    }
    #endif

    // NOTE: TODO: This call *will* (by MSDN) fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->SetCurrentPosition(dwNewPosition);

    if(FAILED(hRet))
        EmuWarning("SetCurrentPosition FAILED");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_GetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_GetCurrentPosition\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   pdwCurrentPlayCursor      : 0x%.08X\n"
               "   pdwCurrentWriteCursor     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, pdwCurrentPlayCursor, pdwCurrentWriteCursor);
    }
    #endif

    // NOTE: TODO: This call always seems to fail on primary buffers!
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(pdwCurrentPlayCursor, pdwCurrentWriteCursor);

    if(FAILED(hRet))
        EmuWarning("GetCurrentPosition FAILED");

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Play
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_Play
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_Play\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               "   dwReserved1               : 0x%.08X\n"
               "   dwReserved2               : 0x%.08X\n"
               "   dwFlags                   : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis, dwReserved1, dwReserved2, dwFlags);
    }
    #endif

    if(dwFlags & (~DSBPLAY_LOOPING))
        EmuCleanup("Unsupported Playing Flags");

    HRESULT hRet = pThis->EmuDirectSoundBuffer8->Play(0, 0, dwFlags);

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Stop
// ******************************************************************
HRESULT WINAPI XTL::EmuIDirectSoundBuffer8_Stop
(
    X_CDirectSoundBuffer   *pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuIDirectSoundBuffer8_Stop\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    HRESULT hRet = pThis->EmuDirectSoundBuffer8->Stop();

    EmuSwapFS();   // XBox FS

    return hRet;
}

// ******************************************************************
// * func: EmuCDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI XTL::EmuCDirectSound_CommitDeferredSettings
(
    X_CDirectSound         *pThis
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuDSound (0x%X): EmuCDirectSound_CommitDeferredSettings\n"
               "(\n"
               "   pThis                     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), pThis);
    }
    #endif

    // Todo: Translate params, then make the PC DirectSound call

    EmuSwapFS();   // XBox FS

    return DS_OK;
}
