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
// *  (c) 2017-2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#define LOG_PREFIX CXBXR_MODULE::DSSTREAM

// prevent name collisions
namespace xboxkrnl {
    #include <xboxkrnl/xboxkrnl.h>
};

#include <dsound.h>
#include "DirectSoundGlobal.hpp"

#include "Logging.h"
#include "DirectSoundLogging.hpp"
#include "..\XbDSoundLogging.hpp"

#include "DirectSoundInline.hpp"
#include "DSStream_PacketManager.hpp"

static inline void DSStream_Packet_UpdateHostBuffer(
    LPDIRECTSOUNDBUFFER8       &pDSBuffer,
    DWORD                       dwOffset,
    PVOID                       pBufferData,
    DWORD                       dwBufferSize
    )
{

    LPVOID pAudioPtr, pAudioPtr2;
    DWORD dwAudioBytes, dwAudioBytes2;

    HRESULT hRet = pDSBuffer->Lock(dwOffset, dwBufferSize, &pAudioPtr, &dwAudioBytes,
                                   &pAudioPtr2, &dwAudioBytes2, 0);

    if (hRet == DS_OK) {

        if (pAudioPtr != nullptr) {
            memcpy_s(pAudioPtr, dwAudioBytes, pBufferData, dwAudioBytes);
            if (pAudioPtr2 != nullptr) {
                memcpy_s(pAudioPtr2, dwAudioBytes2, (PBYTE)pBufferData + dwAudioBytes, dwAudioBytes2);
            }
            pDSBuffer->Unlock(pAudioPtr, dwAudioBytes, pAudioPtr2, dwAudioBytes2);
        }
    }
}

void DSStream_Packet_Clear(
    vector_hvp_iterator        &buffer,
    DWORD                       status,
    XTL::LPFNXMOCALLBACK        Xb_lpfnCallback,
    LPVOID                      Xb_lpvContext,
    XTL::X_CDirectSoundStream*  pThis
    )
{

    free(buffer->pBuffer_data);

    // Peform release only, don't trigger any events below.
    if (status == XMP_STATUS_RELEASE_CXBXR) {
        DSoundSGEMemDealloc(buffer->xmp_data.dwMaxSize);
        buffer = pThis->Host_BufferPacketArray.erase(buffer);
        return;
    }

    if (buffer->xmp_data.pdwStatus != xbnullptr) {
        (*buffer->xmp_data.pdwStatus) = status;
    }
    if (buffer->xmp_data.pdwCompletedSize != xbnullptr) {
        (*buffer->xmp_data.pdwCompletedSize) = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, buffer->xmp_data.dwMaxSize);
    }
    DSoundSGEMemDealloc(buffer->xmp_data.dwMaxSize);

    auto unionEventContext = buffer->xmp_data.hCompletionEvent;
    buffer = pThis->Host_BufferPacketArray.erase(buffer);
    // NOTE: Packet must be erase before call the callback function below. See test case below.
    // Test case: Pocketbike Racer
    //  * (cause audio skipping due to callback function called process then another called to process outside callback (x2)
    //    It only need to call process once.

    // If a callback is set, only do the callback instead of event handle.
    if (Xb_lpfnCallback != xbnullptr) {
        Xb_lpfnCallback(Xb_lpvContext, unionEventContext, status);
    } else if (unionEventContext != 0) {
        BOOL checkHandle = SetEvent(unionEventContext);
        if (checkHandle == 0) {
            DWORD error = GetLastError();
            EmuLog(LOG_LEVEL::WARNING, "Unable to set event on packet's hCompletionEvent. %8X | error = %8X", unionEventContext, error);
        }
    }
}

static inline void DSStream_Packet_UploadPartial(
    XTL::X_CDirectSoundStream* pThis,
    vector_hvp_iterator &bufferCurrent
    )
{
    // Don't write beyond given buffer data, force do nothing.
    if (bufferCurrent->bufPlayed >= bufferCurrent->xmp_data.dwMaxSize) {
        EmuLog(LOG_LEVEL::DEBUG, "Attempted packet buffer overflow | pThis = %08X | bufferCurrent = %08X | bufferSize - %08X | dwBufferBytes = %08X",
        pThis, bufferCurrent, bufferCurrent->xmp_data.dwMaxSize, pThis->EmuBufferDesc.dwBufferBytes);
        return;
    }

    uint32_t avgBytesPerSec = bufferCurrent->avgBytesPerSec;
    uint32_t bufferOffset = avgBytesPerSec * bufferCurrent->bufWrittenIndex;

    if ((bufferOffset + avgBytesPerSec) > bufferCurrent->xmp_data.dwMaxSize) {
        avgBytesPerSec = bufferCurrent->xmp_data.dwMaxSize - bufferOffset;
    }

    DSStream_Packet_UpdateHostBuffer(pThis->EmuDirectSoundBuffer8, bufferCurrent->nextWriteOffset, (uint8_t*)bufferCurrent->pBuffer_data + bufferOffset, avgBytesPerSec);
    bufferCurrent->nextWriteOffset += avgBytesPerSec;
    if (pThis->EmuBufferDesc.dwBufferBytes < bufferCurrent->nextWriteOffset) {
        bufferCurrent->nextWriteOffset -= pThis->EmuBufferDesc.dwBufferBytes;
    }

#if 0
    // Debug area begin
    EmuLog(LOG_LEVEL::DEBUG, "upload packet buffer process | pThis = %08X | bufferCurrent = %08X",
        pThis, bufferCurrent._Ptr);
    EmuLog(LOG_LEVEL::DEBUG, "nextWriteOffset = %08X | bufPlayed = %08X | bufWrittenIndex = %08X",
        bufferCurrent->nextWriteOffset, bufferCurrent->bufPlayed, bufferCurrent->bufWrittenIndex);
    EmuLog(LOG_LEVEL::DEBUG, "bufferSize - %08X | dwBufferBytes = %08X | dwLastWritePos = %08X | dwWriteOffsetNext = %08X\n",
        bufferCurrent->xmp_data.dwMaxSize, pThis->EmuBufferDesc.dwBufferBytes, pThis->Host_dwLastWritePos, pThis->Host_dwWriteOffsetNext);
    // Debug area end
#endif

    if (pThis->Host_isProcessing == false) {
        pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        pThis->Host_isProcessing = true;
    }
    bufferCurrent->bufWrittenIndex++;
}

static inline void DSStream_Packet_Starved(
    XTL::X_CDirectSoundStream* pThis
    )
{
    pThis->Host_isProcessing = false;
    pThis->EmuDirectSoundBuffer8->Stop();
}

bool DSStream_Packet_Process(
    XTL::X_CDirectSoundStream* pThis
    )
{

    // If title want to pause, then don't process the packets.
    if ((pThis->EmuFlags & DSE_FLAG_PAUSE) > 0) {
        return 0;
    }

    // If media object is being used as playback synch, then don't process the packets.
    if ((pThis->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {
        return 0;
    }

    // Do not allow to process if there is no packets.
    if (pThis->Host_BufferPacketArray.size() == 0) {
        return 0;
    }

    DWORD dwAudioBytes;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwAudioBytes);
    if (hRet == DS_OK) {
        vector_hvp_iterator bufferCurrent = pThis->Host_BufferPacketArray.begin();
        vector_hvp_iterator bufferPrev = bufferCurrent;
        if (bufferCurrent->bufWrittenIndex == 0) {

            DSStream_Packet_UploadPartial(pThis, bufferCurrent);
        } else {
            // NOTE: p1. Do not use play cursor, use write cursor to check ahead since by the time it gets there. The buffer is already played.
            // p2. Plus play cursor is not reliable to check, write cursor is reliable as it is update more often.
            // Test case proof: Gauntlet Dark Legacy give 256 bytes of data to a per packet during intro FMV.
            DWORD writePos = 0;
            hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(nullptr, &writePos);
            if (hRet == DS_OK) {

                int bufPlayed = writePos - bufferCurrent->lastWritePos;

                // Correct it if buffer was playing and is at beginning.
                if (writePos < bufferCurrent->lastWritePos) {
                    bufPlayed = writePos + (pThis->EmuBufferDesc.dwBufferBytes - bufferCurrent->lastWritePos);
                }
                bufferCurrent->lastWritePos = writePos;
                bufferCurrent->bufPlayed += bufPlayed;

                if (bufferCurrent->isPlayed == false) {
                    bufferCurrent->isPlayed = true;
                }
#if 0           // Extend debug verification
                if (pThis->Host_BufferPacketArray.size() == 1) {
                    EmuLog(LOG_LEVEL::DEBUG, "pThis: %08X; bufPlayed: %08X; bufdesc-bufferBytes: %08X; xmp-maxSize: %08X",
                    pThis,
                    bufPlayed,
                    pThis->EmuBufferDesc.dwBufferBytes,
                    bufferCurrent->xmp_data.dwMaxSize
                    );
                }
#endif
                if (bufferCurrent->bufPlayed >= bufferCurrent->xmp_data.dwMaxSize) {
                    bufPlayed = bufferCurrent->bufPlayed - bufferCurrent->xmp_data.dwMaxSize;
                    bufferCurrent->bufPlayed = bufferCurrent->xmp_data.dwMaxSize;
                }
                if (bufferCurrent->xmp_data.pdwCompletedSize != xbnullptr) {
                    (*bufferCurrent->xmp_data.pdwCompletedSize) = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, bufferCurrent->bufPlayed);
                }
                if (bufferCurrent->bufPlayed == bufferCurrent->xmp_data.dwMaxSize) {

                    DSStream_Packet_Clear(bufferCurrent, XMP_STATUS_SUCCESS, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);

                    if (pThis->Host_BufferPacketArray.size() == 0) {
                        DSStream_Packet_Starved(pThis);
                        return 0;
                    }
#if 0               // Extend debug verification
                    EmuLog(LOG_LEVEL::DEBUG, "nextBuffer: %08X; bufferCurrent->bufPlayed: %08X; bufPlayed: %08X;\n",
                            bufferCurrent._Ptr,
                            bufferCurrent->bufPlayed,
                            bufPlayed
                            );
#endif
#if 0               //TODO: How to send extra play process to next packet?
                    // Save what had been played in next packet.
                    //bufferCurrent->bufPlayed += bufPlayed;
#endif
                }
                if (bufferCurrent == bufferPrev) {
                    if ((bufferCurrent->bufWrittenIndex * bufferCurrent->avgBytesPerSec) <= bufferCurrent->bufPlayed) {
                        DSStream_Packet_UploadPartial(pThis, bufferCurrent);
                    }
                }
                if (pThis->Host_BufferPacketArray.size() > 1) {
                    if ((bufferCurrent->xmp_data.dwMaxSize - bufferCurrent->bufPlayed) <= pThis->EmuBufferDesc.lpwfxFormat->nAvgBytesPerSec
                        && (bufferCurrent + 1)->bufWrittenIndex == 0) {
                        bufferCurrent++;
                        DSStream_Packet_UploadPartial(pThis, bufferCurrent);
                    }
                }
            }
            // Out of packets, let's stop it.
            if (pThis->Host_BufferPacketArray.size() == 0) {
                DSStream_Packet_Starved(pThis);
                return 0;
            }
        }
    }
    return 1;
}
