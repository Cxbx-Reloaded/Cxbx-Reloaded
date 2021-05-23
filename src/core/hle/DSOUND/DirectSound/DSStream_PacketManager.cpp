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


#include <core\kernel\exports\xboxkrnl.h>
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
    xbox::LPFNXMOCALLBACK        Xb_lpfnCallback,
    LPVOID                      Xb_lpvContext,
    xbox::X_CDirectSoundStream*  pThis
    )
{

    free(buffer->pBuffer_data);

    if (buffer->xmp_data.pdwStatus != xbox::zeroptr) {
        (*buffer->xmp_data.pdwStatus) = status;
    }
    if (buffer->xmp_data.pdwCompletedSize != xbox::zeroptr) {
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
    if (Xb_lpfnCallback != xbox::zeroptr) {
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
    xbox::X_CDirectSoundStream* pThis,
    vector_hvp_iterator &packetCurrent
    )
{
    // Don't write beyond given buffer data, force do nothing.
    if (packetCurrent->bufWrittenBytes == packetCurrent->xmp_data.dwMaxSize) {
        return;
    }
    LPWAVEFORMATEX pwfxFormat = pThis->EmuBufferDesc.lpwfxFormat;

    // Write one avg second worth of buffer ahead into host's audio buffer.
    if (packetCurrent->bufWrittenBytes <= (packetCurrent->bufPlayed + pwfxFormat->nAvgBytesPerSec)) {
        uint32_t bufWriteLength = pwfxFormat->nAvgBytesPerSec;
        uint32_t bufferOffset = packetCurrent->bufWrittenBytes;

        if ((bufferOffset + bufWriteLength) > packetCurrent->xmp_data.dwMaxSize) {
            bufWriteLength = packetCurrent->xmp_data.dwMaxSize - bufferOffset;
        }
        packetCurrent->bufWrittenBytes += bufWriteLength;

        DSStream_Packet_UpdateHostBuffer(pThis->EmuDirectSoundBuffer8, packetCurrent->nextWriteOffset, (uint8_t*)packetCurrent->pBuffer_data + bufferOffset, bufWriteLength);
        packetCurrent->nextWriteOffset += bufWriteLength;
        if (pThis->EmuBufferDesc.dwBufferBytes < packetCurrent->nextWriteOffset) {
            packetCurrent->nextWriteOffset -= pThis->EmuBufferDesc.dwBufferBytes;
        }

#if 0
        // Debug area begin
        EmuLog(LOG_LEVEL::DEBUG, "upload packet buffer process | pThis = %08X | packetCurrent = %08X",
            pThis, packetCurrent._Ptr);
        EmuLog(LOG_LEVEL::DEBUG, "nextWriteOffset = %08X | bufPlayed = %08X | bufWrittenIndex = %08X",
            packetCurrent->nextWriteOffset, packetCurrent->bufPlayed, packetCurrent->bufWrittenIndex);
        EmuLog(LOG_LEVEL::DEBUG, "bufferSize - %08X | dwBufferBytes = %08X | dwLastWritePos = %08X | dwWriteOffsetNext = %08X\n",
            packetCurrent->xmp_data.dwMaxSize, pThis->EmuBufferDesc.dwBufferBytes, pThis->Host_dwLastWritePos, pThis->Host_dwWriteOffsetNext);
        // Debug area end
#endif
    }
}

static inline void DSStream_Packet_Stop_Internal(
    xbox::X_CDirectSoundStream* pThis
    )
{
    pThis->Host_isProcessing = false;
    pThis->EmuDirectSoundBuffer8->Stop();
}

static inline void DSStream_Packet_Stop(
    xbox::X_CDirectSoundStream* pThis
    )
{
    DSStream_Packet_Stop_Internal(pThis);

    if (pThis->Host_BufferPacketArray.empty()) {
        if ((pThis->EmuFlags & DSE_FLAG_ENVELOPE2) > 0) {
            pThis->Xb_Status |= X_DSSSTATUS_ENVELOPECOMPLETE;
        }
    }
    // We need to log this info since some titles may have stop/played audio often in middle of processing (relative to videos).
    EmuLog(LOG_LEVEL::INFO, "Stopped: pThis = %08X; Remaining packet(s): %d",
        pThis, pThis->Host_BufferPacketArray.size()
    );
}

static inline void DSStream_Packet_Starved(
    xbox::X_CDirectSoundStream* pThis
    )
{
    DSStream_Packet_Stop_Internal(pThis);
    pThis->Xb_Status |= X_DSSSTATUS_STARVED | X_DSSSTATUS_PAUSED;
    // We need to log this info since some titles may have stop/played audio often in middle of processing (relative to videos).
    EmuLog(LOG_LEVEL::INFO, "Starved: pThis = %08X;",
        pThis
    );
}

static inline void DSStream_Packet_Complete(
    xbox::X_CDirectSoundStream* pThis
    )
{
    if ((pThis->EmuFlags & DSE_FLAG_ENVELOPE2) != 0) {
        pThis->Xb_Status = X_DSSSTATUS_ENVELOPECOMPLETE;
    }
    else {
        pThis->Xb_Status = 0;
    }
}

// Prefill buffer with at least 1 second worth of buffer. See "nAvgBytesPerSec" below for inspection.
static void DSStream_Packet_Prefill(
    xbox::X_CDirectSoundStream* pThis,
    vector_hvp_iterator packetCurrent
    )
{
    unsigned int streamBufferSize = 0;

    while (packetCurrent != pThis->Host_BufferPacketArray.end()) {
        // If buffer had been written, then ignore it.
        if (packetCurrent->bufWrittenBytes == 0) {
            DSStream_Packet_UploadPartial(pThis, packetCurrent);
        }
        // Get remaining size left in current packet.
        streamBufferSize += (packetCurrent->xmp_data.dwMaxSize - packetCurrent->bufPlayed);
        if (streamBufferSize > pThis->EmuBufferDesc.lpwfxFormat->nAvgBytesPerSec) {
            break;
        }
        packetCurrent++;
    }
}

bool DSStream_Packet_Process(
    xbox::X_CDirectSoundStream* pThis
    )
{

    // Do not allow to process if there is no packets.
    if (pThis->Host_BufferPacketArray.empty()) {
        return 0;
    }

    // Do not allow to process when the voice is not activated.
    if ((pThis->EmuFlags & DSE_FLAG_PAUSENOACTIVATE) != 0 &&
        (pThis->EmuFlags & DSE_FLAG_IS_ACTIVATED) == 0) {
        return 0;
    }

    if (pThis->EmuFlags & DSE_FLAG_IS_FLUSHING) {
        return 0;
    }

    if (!(pThis->EmuFlags & DSE_FLAG_IS_ACTIVATED)) {
        pThis->EmuFlags |= DSE_FLAG_IS_ACTIVATED;
    }

    // If title want to pause, then don't process the packets.
    // If media object is being used as playback synch, then don't process the packets.
    if ((pThis->EmuFlags & DSE_FLAG_PAUSE) > 0 ||
        (pThis->EmuFlags & DSE_FLAG_SYNCHPLAYBACK_CONTROL) > 0) {

        vector_hvp_iterator packetCurrent = pThis->Host_BufferPacketArray.begin();
        DSStream_Packet_Prefill(pThis, packetCurrent);
        return 0;
    }

    DWORD dwAudioBytes;
    HRESULT hRet = pThis->EmuDirectSoundBuffer8->GetStatus(&dwAudioBytes);
    if (hRet == DS_OK) {
        vector_hvp_iterator packetCurrent = pThis->Host_BufferPacketArray.begin();

        // NOTE: p1. Do not use play cursor, use write cursor to check ahead since by the time it gets there. The buffer is already played.
        // p2. Plus play cursor is not reliable to check, write cursor is reliable as it is update more often.
        // Test case proof: Gauntlet Dark Legacy give 256 bytes of data to a per packet during intro FMV.
        // TODO: Need a better solution to give actual playtime since beginning of audio do run ahead of time.
        DWORD writePos = 0;
        hRet = pThis->EmuDirectSoundBuffer8->GetCurrentPosition(nullptr, &writePos);
        if (hRet == DS_OK) {
            // Disabled do/while loop since xbox thread will be processing as well.
            //do {
                int bufPlayed = writePos - packetCurrent->lastWritePos;

                // Correct it if buffer was playing and is at beginning.
                if (writePos < packetCurrent->lastWritePos) {
                    bufPlayed = writePos + (pThis->EmuBufferDesc.dwBufferBytes - packetCurrent->lastWritePos);
                }
                packetCurrent->lastWritePos = writePos;
                packetCurrent->bufPlayed += bufPlayed;

                if (packetCurrent->isPlayed == false) {
                    packetCurrent->isPlayed = true;
                }
#if 0           // Extend debug verification
                if (pThis->Host_BufferPacketArray.size() == 1) {
                    EmuLog(LOG_LEVEL::DEBUG, "pThis: %08X; bufPlayed: %08X; bufdesc-bufferBytes: %08X; xmp-maxSize: %08X",
                        pThis,
                        bufPlayed,
                        pThis->EmuBufferDesc.dwBufferBytes,
                        packetCurrent->xmp_data.dwMaxSize
                    );
                }
#endif
                // if bufPlayed is over dwMaxSize, then re-correct it to return dwMaxSize only.
                if (packetCurrent->bufPlayed >= packetCurrent->xmp_data.dwMaxSize) {
                    bufPlayed = packetCurrent->bufPlayed - packetCurrent->xmp_data.dwMaxSize;
                    packetCurrent->bufPlayed = packetCurrent->xmp_data.dwMaxSize;
                }
                // Return current completed size to Xbox's application.
                if (packetCurrent->xmp_data.pdwCompletedSize != xbox::zeroptr) {
                    (*packetCurrent->xmp_data.pdwCompletedSize) = DSoundBufferGetXboxBufferSize(pThis->EmuFlags, packetCurrent->bufPlayed);
                }
                // Once bufPlayed is equal to dwMaxSize, we know the packet is completed.
                if (packetCurrent->bufPlayed == packetCurrent->xmp_data.dwMaxSize) {

                    bool isStreamEnd = packetCurrent->isStreamEnd;
                    DSStream_Packet_Clear(packetCurrent, XMP_STATUS_SUCCESS, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);

                    if (pThis->Host_BufferPacketArray.empty()) {
                        if (isStreamEnd) {
                            DSStream_Packet_Stop(pThis);
                            DSStream_Packet_Complete(pThis);
                        }
                        else {
                            DSStream_Packet_Starved(pThis);
                        }
                        return 0;
                    }
                    //the iterator might be invalidated, so we update it again.
                    packetCurrent = pThis->Host_BufferPacketArray.begin();
#if 0               // Extend debug verification
                    EmuLog(LOG_LEVEL::DEBUG, "nextBuffer: %08X; packetCurrent->bufPlayed: %08X; bufPlayed: %08X;\n",
                        packetCurrent._Ptr,
                        packetCurrent->bufPlayed,
                        bufPlayed
                    );
#endif
                    // Is this important or already have it handled by using while statement?
                    if (packetCurrent->bufWrittenBytes == 0) {
                        EmuLog(LOG_LEVEL::INFO, "audio gap detected: %08X; packetCurrent->bufPlayed: %08X; bufPlayed: %08X;\n",
                            &*packetCurrent,
                            packetCurrent->bufPlayed,
                            bufPlayed
                        );
                    }
                    // Since we have later packet ready to be process, let's process it in the loop.
                }
                // Otherwise, continue upload partial of the packet's data to host if there are any left.
                else {
                    DSStream_Packet_UploadPartial(pThis, packetCurrent);
                    //break; // Leave loop since there is more buffer haven't been process in the current packet.
                }
            //} while (true);

            DSStream_Packet_Prefill(pThis, packetCurrent);
        }
        // Out of packets, let's stop stream's buffer.
        if (pThis->Host_BufferPacketArray.empty()) {
            DSStream_Packet_Starved(pThis);
            return 0;
        }
    }

    // The only place when Host_isProcessing is set to true to start play Host's audio buffer.
    if (pThis->Host_isProcessing == false) {
        pThis->EmuDirectSoundBuffer8->Play(0, 0, pThis->EmuPlayFlags);
        pThis->Host_isProcessing = true;
        // Add debug log for verify if Host_isProcessing has set to true.
        EmuLog(LOG_LEVEL::DEBUG, "pStream = %08X:Host_isProcessing is set to true.", pThis);
    }
    return 1;
}

void DSStream_Packet_FlushEx_Reset(
    xbox::X_CDirectSoundStream* pThis
    )
{
    // Remove flags only (This is the only place it will remove beside FlushEx perform re-set the flags.)
    pThis->EmuFlags &= ~(DSE_FLAG_FLUSH_ASYNC | DSE_FLAG_ENVELOPE | DSE_FLAG_ENVELOPE2);
    pThis->Xb_rtFlushEx = 0LL;
}

bool DSStream_Packet_Flush(
    xbox::X_CDirectSoundStream* pThis
    )
{
    if ((pThis->EmuFlags & DSE_FLAG_IS_FLUSHING) == 0) {
        pThis->EmuFlags |= DSE_FLAG_IS_FLUSHING;
    }
    // If host's audio is still playing then return busy-state until buffer has stop playing.
    DWORD dwStatus;
    pThis->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
    if ((dwStatus & DSBSTATUS_PLAYING) > 0) {
        // We need to stop the play state
        pThis->EmuDirectSoundBuffer8->Stop();
        DSStream_Packet_Process(pThis);
        return true;
    }
    // Once stopped, we can safely flush the packets.
    pThis->EmuDirectSoundBuffer8->SetCurrentPosition(0);
    pThis->Host_dwLastWritePos = 0;
    pThis->Host_isProcessing = false;
    for (auto buffer = pThis->Host_BufferPacketArray.begin(); buffer != pThis->Host_BufferPacketArray.end();) {
        DSStream_Packet_Clear(buffer, XMP_STATUS_FLUSHED, pThis->Xb_lpfnCallback, pThis->Xb_lpvContext, pThis);
    }
    // Clear flags and set status to zero.
    DSStream_Packet_Complete(pThis);
    DSStream_Packet_FlushEx_Reset(pThis);

    // TESTCASE: Burnout 3 sets stream to pause state then calling SetFormat without processing any packets.
    // Which then doesn't need to clear pause flag.
    if ((pThis->EmuFlags & DSE_FLAG_IS_ACTIVATED) != 0) {
        pThis->EmuFlags &= ~(DSE_FLAG_PAUSE | DSE_FLAG_IS_ACTIVATED);
    }
    pThis->EmuFlags &= ~DSE_FLAG_IS_FLUSHING;
    return false;
}
