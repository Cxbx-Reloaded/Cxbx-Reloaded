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
// *  (c) 2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::XMO

// prevent name collisions
namespace xboxkrnl {
    #include <xboxkrnl/xboxkrnl.h>
};

#include <dsound.h>
#include "DirectSoundGlobal.hpp" // Global variables

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include "..\XbDSoundLogging.hpp"

#include "DirectSoundInline.hpp"

// TODO: Tasks need to do for DirectSound HLE
// * Need create patches
//   * Ac97CreateMediaObject (Need OOVPA)
//   - WmaCreateDecoder (Need OOVPA, not require) Test case: WMAStream sample
//   - WmaCreateDecoderEx (Is just a forward to WmaCreateDecoder, nothing else)
//   - WmaCreateInMemoryDecoder (Need OOVPA, not require) Test case: WMAInMemory sample
//   - WmaCreateInMemoryDecoderEx (Is just a forward to WmaCreateInMemoryDecoder, nothing else)
//   * XWmaDecoderCreateMediaObject (Need OOVPA)

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

// ******************************************************************
// * patch: XAudioCreateAdpcmFormat
// ******************************************************************
VOID WINAPI XTL::EMUPATCH(XAudioCreateAdpcmFormat)
(
    WORD                   nChannels,
    DWORD                  nSamplesPerSec,
    LPXBOXADPCMWAVEFORMAT  pwfx)
{
    DSoundMutexGuardLock;

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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pszImageName)
		LOG_FUNC_ARG(pImageLoc)
		LOG_FUNC_ARG(dwFlags)
		LOG_FUNC_ARG(ppImageDesc)
		LOG_FUNC_END;

	LOG_NOT_SUPPORTED();

    return S_OK;
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
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(dwEffectIndex)
        LOG_FUNC_ARG(pDesc)
        LOG_FUNC_ARG(pRawDesc)
        LOG_FUNC_END;

    LOG_NOT_SUPPORTED();

    return E_FAIL;
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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pstrFileName)
		LOG_FUNC_ARG(dwDesiredAccess)
		LOG_FUNC_ARG(dwShareMode)
		LOG_FUNC_ARG(dwCreationDisposition)
		LOG_FUNC_ARG(dwFlagsAndAttributes)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    return E_FAIL;
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
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG(dwMaxPackets)
        LOG_FUNC_ARG_OUT(ppMediaObject)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    *ppMediaObject = new X_XFileMediaObject();

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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(hFile)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

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
    //

    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pszFileName)
		LOG_FUNC_ARG(ppwfxFormat)
		LOG_FUNC_ARG_OUT(ppMediaObject)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

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
    //

    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pszFileName)
        LOG_FUNC_ARG(hFile)
        LOG_FUNC_ARG_OUT(ppMediaObject)
        LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    return E_FAIL;
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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(lOffset)
		LOG_FUNC_ARG(dwOrigin)
		LOG_FUNC_ARG(pdwAbsolute)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

    return S_OK;
}

// ******************************************************************
// * patch: XFileMediaObject_DoWork
// ******************************************************************
// NOTE: Does not require a patch.
VOID WINAPI XTL::EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    LOG_UNIMPLEMENTED();

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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pdwStatus)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG_OUT(pInfo)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

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
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pThis)
		LOG_FUNC_ARG(pInputBuffer)
		LOG_FUNC_ARG(pOutputBuffer)
		LOG_FUNC_END;

    LOG_UNIMPLEMENTED();

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
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG Ret = 0;

    if (pThis) { // TODO : this should be an assert()
        Ret = ++(pThis->EmuRefCount);
    }

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
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    ULONG Ret = 0;

    if (pThis) { // TODO : this should be an assert()
        Ret = --(pThis->EmuRefCount);
        if (Ret == 0) {
            delete pThis;
        }
    }

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
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

	LOG_UNIMPLEMENTED();

    return DS_OK;
}

