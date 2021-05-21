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


#include <core\kernel\exports\xboxkrnl.h>
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

xbox::X_XFileMediaObject::_vtbl xbox::X_XFileMediaObject::vtbl =
{
    &xbox::EMUPATCH(XFileMediaObject_AddRef),            // 0x00
    &xbox::EMUPATCH(XFileMediaObject_Release),           // 0x04
    &xbox::EMUPATCH(XFileMediaObject_GetInfo),           // 0x08
    &xbox::EMUPATCH(XFileMediaObject_GetStatus),         // 0x0C
    &xbox::EMUPATCH(XFileMediaObject_Process),           // 0x10
    &xbox::EMUPATCH(XFileMediaObject_Discontinuity),     // 0x14
    0xBEEFD007,                                         // 0x18
    &xbox::EMUPATCH(XFileMediaObject_Seek),              // 0x1C
    0xBEEFD009,                                         // 0x20
    &xbox::EMUPATCH(XFileMediaObject_DoWork),            // 0x24
};

/* NOTE: SUCCEEDED define is only checking for is equal or greater than zero value.
    And FAILED check for less than zero value. Since DS_OK is only 0 base on DirectSound documentation,
    there is chance of failure which contain value greater than 0.
 */

#if 0 // NOTE: Not require to patch
// ******************************************************************
// * patch: XAudioCreateAdpcmFormat
// ******************************************************************
xbox::void_xt WINAPI xbox::EMUPATCH(XAudioCreateAdpcmFormat)
(
    word_xt                        nChannels,
    dword_xt                       nSamplesPerSec,
    OUT LPXBOXADPCMWAVEFORMAT   pwfx)
{
    DSoundMutexGuardLock;

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(nChannels)
		LOG_FUNC_ARG(nSamplesPerSec)
		LOG_FUNC_ARG_OUT(pwfx)
		LOG_FUNC_END;

    // Fill out the pwfx structure with the appropriate data
    pwfx->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
    pwfx->wfx.nChannels = nChannels;
    pwfx->wfx.nSamplesPerSec = nSamplesPerSec;
    pwfx->wfx.nBlockAlign = nChannels * 36;
    pwfx->wfx.nAvgBytesPerSec = (nSamplesPerSec / 64/*Always 64 samples per block*/) * pwfx->wfx.nBlockAlign;
    pwfx->wfx.wBitsPerSample = 4;
    pwfx->wfx.cbSize = 2;
    pwfx->wSamplesPerBlock = 64;
}
#endif

// ******************************************************************
// * patch: XAudioDownloadEffectsImage
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(XAudioDownloadEffectsImage)
(
    LPCSTR      pszImageName,
    LPVOID      pImageLoc,
    dword_xt       dwFlags,
    LPVOID*     ppImageDesc)
{
    DSoundMutexGuardLock;

    LOG_FUNC_BEGIN
        LOG_FUNC_ARG(pszImageName)
        LOG_FUNC_ARG(pImageLoc)
        LOG_FUNC_ARG(dwFlags)
        LOG_FUNC_ARG(ppImageDesc)
        LOG_FUNC_END;

    LOG_INCOMPLETE();

    xbox::hresult_xt result = S_OK;
    if (ppImageDesc) { //only process image section/file which the guest code asks for ImageDesc.

        PBYTE           pvImageBuffer;
        dword_xt        dwImageSize;

        if (dwFlags & 1) { // dwFlags == XAUDIO_DOWNLOADFX_XBESECTION, The DSP effects image is located in a section of the XBE.
            /*
            //future code for loading imgae from XBE section. these codes are reversd from PGR2.

            PXBEIMAGE_SECTION pImageSectionHandle=XGetSectionHandle(pszImageName);    //get section handle by section name, not implemented yet.
            // perhaps use pImageSectionHandle = CxbxKrnl_Xbe->FindSection(pszImageName); will be easier.

            if (XeLoadSection(pImageSectionHandle) > 0) {  //load section handle and get the loaded address.
            //note this sction must be freed after the internal image bacup and ImageDesc was created.
            //EmuKnrlXe.cpp implements XeLoadSection(). could reference that code.
                pvImageBuffer = pImageSectionHandle->VirtualAddress;
            }

            dwImageSize=pImageSectionHandle->VirtualSize;  //get section size by section handle.

            result = xbox::EMUPATCH(CDirectSound_DownloadEffectsImage)(pThis_tmp, pvImageBuffer,dwImageSize,pImageLoc,ppImageDesc);

            if(pImageSectionHandle<>0 && pImageSectionHandle!=-1)
                XeUnloadSection(pImageSectionHandle);

            */

            LOG_TEST_CASE("Loading dsp images from xbe sections is currently not yet supported");

            result = S_OK;//this line should be removed once the section loading code was implemented.
        }
        else { // load from file
            LPDIRECTSOUND8  pThis_tmp = zeroptr;
            HANDLE hFile;
            DWORD dwBytesRead;

            // using xbox::NtCreateFile() directly instead of Host CreateFile();
            OBJECT_ATTRIBUTES obj;
            ANSI_STRING file_name;
            IO_STATUS_BLOCK io_status_block;
            RtlInitAnsiString(&file_name, pszImageName);
            XB_InitializeObjectAttributes(&obj, &file_name, obj_case_insensitive, ObDosDevicesDirectory());
            ntstatus_xt NtStatusCreateFile;
            //LARGE_INTEGER tmp_LargeInt;
            //tmp_LargeInt.QuadPart= dwImageSize;
            NtStatusCreateFile = NtCreateFile(
                &hFile,
                FILE_GENERIC_READ, // FILE_READ_DATA, GENERIC_READ, DesiredAccess,
                &obj,
                &io_status_block,
                zeroptr, // AllocationSize OPTIONAL, must be none zero, no effect for read acceess.
                FILE_ATTRIBUTE_NORMAL, // FileAttributes,
                FILE_SHARE_READ, // ShareAccess,
                FILE_OPEN, // CreateDisposition,
                FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE); // CreateOptions; CreateFileA Convert dwCreationDisposition== 3 OPEN_EXISTING  to CreateOptions = 1 FILE_DIRECTORY_FILE!!?? but with 1, this will fail.

            //process possible error with NtCreateFile()
            if (NtStatusCreateFile < 0) {
                //ULONG DOSERRORNtCreateFile=RtlNtStatusToDosError(NtStatusCreateFile);
                EmuLog(LOG_LEVEL::WARNING, "%s: Image file NtCreateFile() error", __func__);
                if (NtStatusCreateFile == status_object_name_collision) {
                    EmuLog(LOG_LEVEL::WARNING, "%s: Image file name collision", __func__);
                }
                else if (NtStatusCreateFile == status_file_is_a_directory) {
                    EmuLog(LOG_LEVEL::WARNING, "%s: Image file name is a directory or invalid", __func__);
                }
                hFile= INVALID_HANDLE_VALUE;
            }

            if (hFile != INVALID_HANDLE_VALUE) {
                FILE_STANDARD_INFORMATION FileStdInfo;
                NTSTATUS NtStatusQueryInfoFile = NtQueryInformationFile(
                    hFile,
                    &io_status_block,
                    &FileStdInfo, // FileInformation
                    sizeof(FILE_STANDARD_INFORMATION),
                    FileStandardInformation); // FileInformationClass; Enumation of the file information class.
                if (NtStatusQueryInfoFile >= 0) {
                    dwImageSize = FileStdInfo.EndOfFile.u.LowPart;
                }
                else {
                    EmuLog(LOG_LEVEL::WARNING, "%s: Image file NtQueryInformationFile() error.", __func__);
                    dwImageSize = 0;
                }

                if (dwImageSize > 0) { //proceed the process only if the file size > 0
                    pvImageBuffer = new BYTE[dwImageSize]; //allocate buffer to read in to image file.

                    //use NtReadFile() to replace host CreatFile();
                    ntstatus_xt NtStatusReadFile = NtReadFile(
                        hFile,
                        0, // Event OPTIONAL
                        0, // ApcRoutine OPTIONAL
                        0, // ApcContext
                        &io_status_block,
                        pvImageBuffer,
                        dwImageSize,
                        zeroptr); // ByteOffset OPTIONAL

                    DWORD dwBytesRead = 0;
                    if (NtStatusReadFile == status_pending) {
                        NtStatusReadFile = NtWaitForSingleObject(hFile, 0, 0);
                        if (NtStatusReadFile < 0){ //something wrong
                            EmuLog(LOG_LEVEL::WARNING, "%s: Image file NtReadFile error", __func__);
                            if (NtStatusReadFile != status_end_of_file) {
                                if ((NtStatusReadFile & 0xC0000000) == 0x80000000) { //Error happened during file reading
                                    dwBytesRead = io_status_block.Information;
                                    EmuLog(LOG_LEVEL::WARNING, "%s: NtReadFile read file end", __func__);
                                    // ULONG DOSErrorNtReadFile = RtlNtStatusToDosError(NtStatusReadFile); this is supposed to be the error code of xbox::CreateFile()
                                }
                            } else {
                                dwBytesRead = 0;
                            }
                        }
                        NtStatusReadFile = io_status_block.Status;
                    }
                    if (NtStatusReadFile >= 0) {
                        dwBytesRead = io_status_block.Information;
                    }

                    if (dwBytesRead == dwImageSize) { // only process the image if the whole image was read successfully.
                        result = xbox::EMUPATCH(CDirectSound_DownloadEffectsImage)(pThis_tmp, pvImageBuffer,dwImageSize,pImageLoc,ppImageDesc);
                    }
                    else {
                        EmuLog(LOG_LEVEL::WARNING, "%s: Image file NtReadFile read in lenth not enough", __func__);
                    }

                    if (pvImageBuffer) {
                        delete[] pvImageBuffer;
                    }

                    if (hFile != INVALID_HANDLE_VALUE) {
                        NtClose(hFile);
                    }
                }
            }
        }
    }

    return result;
}

// ******************************************************************
// * patch: XAudioSetEffectData
// ******************************************************************
xbox::hresult_xt WINAPI xbox::EMUPATCH(XAudioSetEffectData)
(
    dword_xt               dwEffectIndex,
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileCreateMediaObject)
(
    LPCSTR          pstrFileName,
    dword_xt           dwDesiredAccess,
    dword_xt           dwShareMode,
    dword_xt           dwCreationDisposition,
    dword_xt           dwFlagsAndAttributes,
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileCreateMediaObjectAsync)
(
    HANDLE      hFile,
    dword_xt       dwMaxPackets,
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileCreateMediaObjectEx)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XWaveFileCreateMediaObject)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XWaveFileCreateMediaObjectEx)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileMediaObject_Seek)
(
    X_XFileMediaObject* pThis,
    long_xt                lOffset,
    dword_xt               dwOrigin,
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
xbox::void_xt WINAPI xbox::EMUPATCH(XFileMediaObject_DoWork)(X_XFileMediaObject* pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

    LOG_UNIMPLEMENTED();

}

// ******************************************************************
// * patch: XFileMediaObject_GetStatus
// ******************************************************************
// NOTE: Does not require a patch.
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileMediaObject_GetStatus)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileMediaObject_GetInfo)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileMediaObject_Process)
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
xbox::ulong_xt WINAPI xbox::EMUPATCH(XFileMediaObject_AddRef)
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
xbox::ulong_xt WINAPI xbox::EMUPATCH(XFileMediaObject_Release)
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
xbox::hresult_xt WINAPI xbox::EMUPATCH(XFileMediaObject_Discontinuity)
(
	X_XFileMediaObject *pThis)
{
    DSoundMutexGuardLock;

	LOG_FUNC_ONE_ARG(pThis);

	LOG_UNIMPLEMENTED();

    return DS_OK;
}

