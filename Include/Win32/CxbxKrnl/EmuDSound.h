// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuDSound.h
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
#ifndef EMUDSOUND_H
#define EMUDSOUND_H

#include "Xbe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

#include <dsound.h>

// ******************************************************************
// * X_CDirectSound
// ******************************************************************
struct X_CDirectSound
{
    // TODO: Fill this in?
};

// ******************************************************************
// * X_CDirectSoundBuffer
// ******************************************************************
struct X_CDirectSoundBuffer
{
    BYTE    UnknownA[0x20];     // Offset: 0x00

    union                       // Offset: 0x20
    {
        PVOID                pMpcxBuffer;
        IDirectSoundBuffer  *EmuDirectSoundBuffer8;
    };

    BYTE    UnknownB[0x0C];     // Offset: 0x24
};

// ******************************************************************
// * X_CDirectSoundStream
// ******************************************************************
class X_CDirectSoundStream
{
    public:
        // ******************************************************************
        // * Construct VTable (or grab ptr to existing)
        // ******************************************************************
        X_CDirectSoundStream() : pVtbl(&vtbl) {};

    private:
        // ******************************************************************
        // * VTable (cached by each instance, via constructor)
        // ******************************************************************
        struct _vtbl
        {
            ULONG (WINAPI *AddRef)(X_CDirectSoundStream *pThis);            // 0x00
            ULONG (WINAPI *Release)(X_CDirectSoundStream *pThis);           // 0x04
            DWORD Unknown[0x02];                                            // 0x08
            HRESULT (WINAPI *Process)                                       // 0x10
            (
                X_CDirectSoundStream   *pThis,
                PVOID                   pInputBuffer,                       // TODO: Fillout params
                PVOID                   pOutputBuffer                       // TODO: Fillout params
            );
            HRESULT (WINAPI *Discontinuity)(X_CDirectSoundStream *pThis);   // 0x14
        }
        *pVtbl;

        // ******************************************************************
        // * Global Vtbl for this class
        // ******************************************************************
        static _vtbl vtbl;

        // ******************************************************************
        // * Debug Mode guard for detecting naughty data accesses
        // ******************************************************************
        #ifdef _DEBUG
        DWORD DebugGuard[256];
        #endif
};

// ******************************************************************
// * X_DSBUFFERDESC
// ******************************************************************
struct X_DSBUFFERDESC
{
    DWORD           dwSize; 
    DWORD           dwFlags; 
    DWORD           dwBufferBytes; 
    LPWAVEFORMATEX  lpwfxFormat; 
    LPVOID          lpMixBins;      // TODO: Implement
    DWORD           dwInputMixBin;
};

// ******************************************************************
// * X_DSSTREAMDESC
// ******************************************************************
struct X_DSSTREAMDESC
{
    DWORD                       dwFlags;
    DWORD                       dwMaxAttachedPackets;
    LPWAVEFORMATEX              lpwfxFormat;
    PVOID                       lpfnCallback;   // TODO: Correct Parameter
    LPVOID                      lpvContext;
    PVOID                       lpMixBins;      // TODO: Correct Parameter
}; 

// ******************************************************************
// * func: EmuDirectSoundCreate
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreate
(
    LPVOID          pguidDeviceId,
    LPDIRECTSOUND8 *ppDirectSound,
    LPUNKNOWN       pUnknown
);

// ******************************************************************
// * func: EmuDirectSoundCreateBuffer
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreateBuffer
(
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer
);

// ******************************************************************
// * func: EmuDirectSoundCreateStream
// ******************************************************************
HRESULT WINAPI EmuDirectSoundCreateStream
(
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateStream
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateStream
(
    LPDIRECTSOUND8          pThis,
    X_DSSTREAMDESC         *pdssd,
    X_CDirectSoundStream  **ppStream,
    PVOID                   pUnknown
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdssd,
    X_CDirectSoundBuffer  **ppBuffer,
    PVOID                   pUnknown
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVolume
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_SetVolume(X_CDirectSoundStream *pThis, LONG lVolume);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetRolloffFactor
(
    X_CDirectSoundStream *pThis,
    FLOAT                 fRolloffFactor,
    DWORD                 dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_AddRef
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_AddRef(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSoundStream_Release
// ******************************************************************
ULONG WINAPI EmuCDirectSoundStream_Release(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSoundStream_Process
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Process
(
    X_CDirectSoundStream   *pThis,
    PVOID                   pInputBuffer,   // TODO: Fillout params
    PVOID                   pOutputBuffer   // TODO: Fillout params
);

// ******************************************************************
// * func: EmuCDirectSoundStream_Discontinuity
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Discontinuity(X_CDirectSoundStream *pThis);

// ******************************************************************
// * func: EmuCDirectSoundStream_Pause
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_Pause
(
    PVOID   pStream,
    DWORD   dwPause
);

// ******************************************************************
// * func: EmuIDirectSound8_AddRef
// ******************************************************************
ULONG WINAPI EmuIDirectSound8_AddRef
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * func: EmuIDirectSound8_Release
// ******************************************************************
ULONG WINAPI EmuIDirectSound8_Release
(
    LPDIRECTSOUND8          pThis
);

// ******************************************************************
// * func: EmuIDirectSound8_DownloadEffectsImage
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_DownloadEffectsImage
(
    LPDIRECTSOUND8          pThis,
    LPCVOID                 pvImageBuffer,
    DWORD                   dwImageSize,
    PVOID                   pImageLoc,      // TODO: Use this param
    PVOID                  *ppImageDesc     // TODO: Use this param
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetHeadroom
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetHeadroom
(
    PVOID   pThis,
    DWORD   dwHeadroom
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetAllParameters
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetAllParameters
(
    PVOID   pThis,
    PVOID   pUnknown,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeAngles
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeAngles
(
    PVOID   pThis,
    DWORD   dwInsideConeAngle,
    DWORD   dwOutsideConeAngle,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeOutsideVolume
(
    PVOID   pThis,
    LONG    lConeOutsideVolume,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMaxDistance
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMaxDistance
(
    PVOID    pThis,
    D3DVALUE fMaxDistance,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetMinDistance
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetMinDistance
(
    PVOID    pThis,
    D3DVALUE fMinDistance,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetVelocity
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetVelocity
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetConeOrientation
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetConeOrientation
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetPosition
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetPosition
(
    PVOID    pThis,
    D3DVALUE x,
    D3DVALUE y,
    D3DVALUE z,
    DWORD    dwApply
);

// ******************************************************************
// * func: EmuCDirectSoundStream_SetFrequency
// ******************************************************************
HRESULT WINAPI EmuCDirectSoundStream_SetFrequency
(
    PVOID   pThis,
    DWORD   dwFrequency
);

// ******************************************************************
// * func: EmuIDirectSoundStream_SetI3DL2Source
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundStream_SetI3DL2Source
(
    PVOID   pThis,
    PVOID   pds3db,
    DWORD   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetOrientation
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetOrientation
(
    LPDIRECTSOUND8  pThis,
    FLOAT           xFront,
    FLOAT           yFront,
    FLOAT           zFront,
    FLOAT           xTop,
    FLOAT           yTop,
    FLOAT           zTop,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetDistanceFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetDistanceFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDistanceFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetRolloffFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetRolloffFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fRolloffFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetDopplerFactor
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetDopplerFactor
(
    LPDIRECTSOUND8  pThis,
    FLOAT           fDopplerFactor,
    DWORD           dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetI3DL2Listener
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetI3DL2Listener
(
    LPDIRECTSOUND8          pThis,
    PVOID                   pDummy, // TODO: fill this out
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetMixBinHeadroom
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetMixBinHeadroom
(
    LPDIRECTSOUND8          pThis,
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
);

// ******************************************************************
// * func: EmuIDirectSound8_SetPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetPosition
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetVelocity
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetVelocity
(
    LPDIRECTSOUND8          pThis,
    FLOAT                   x,
    FLOAT                   y,
    FLOAT                   z,
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_SetAllParameters
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_SetAllParameters
(
    LPDIRECTSOUND8          pThis,
    LPVOID                  pTodo,  // TODO: LPCDS3DLISTENER
    DWORD                   dwApply
);

// ******************************************************************
// * func: EmuIDirectSound8_CreateSoundBuffer
// ******************************************************************
HRESULT WINAPI EmuIDirectSound8_CreateSoundBuffer
(
    LPDIRECTSOUND8          pThis,
    X_DSBUFFERDESC         *pdsbd,
    X_CDirectSoundBuffer  **ppBuffer,
    LPUNKNOWN               pUnkOuter
);


// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetBufferData
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetBufferData
(
    X_CDirectSoundBuffer   *pThis,
    LPVOID                  pvBufferData,
    DWORD                   dwBufferBytes
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetPlayRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetLoopRegion
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetVolume
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetVolume
(
    X_CDirectSoundBuffer   *pThis,
    LONG                    lVolume
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_SetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwNewPosition
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_GetCurrentPosition
(
    X_CDirectSoundBuffer   *pThis,
    PDWORD                  pdwCurrentPlayCursor,
    PDWORD                  pdwCurrentWriteCursor
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Stop
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Stop
(
    X_CDirectSoundBuffer   *pThis
);

// ******************************************************************
// * func: EmuIDirectSoundBuffer8_Play
// ******************************************************************
HRESULT WINAPI EmuIDirectSoundBuffer8_Play
(
    X_CDirectSoundBuffer   *pThis,
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
);

// ******************************************************************
// * func: EmuCDirectSound_CommitDeferredSettings
// ******************************************************************
HRESULT WINAPI EmuCDirectSound_CommitDeferredSettings
(
    X_CDirectSound         *pThis
);

#endif
