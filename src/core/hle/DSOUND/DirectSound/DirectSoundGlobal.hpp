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
#pragma once

#include <dsound.h>

#include <thread>
#include <mutex>
#include "core/kernel/support/Emu.h"
#include "common/win32/EmuShared.h"
#include "core/hle/D3D8/Direct3D9/Direct3D9.h" // For D3D_OK
#include "core/hle/DSOUND/DirectSound/DirectSound.hpp"
#include "common/Settings.hpp"

extern Settings::s_audio            g_XBAudio;
extern std::recursive_mutex         g_DSoundMutex;
#define DSoundMutexGuardLock        std::lock_guard<std::recursive_mutex> guard(g_DSoundMutex)

//Currently disabled since below may not be needed since under -6,400 is just silence yet accepting up to -10,000
// Xbox to PC volume ratio format (-10,000 / -6,400 )
//#define XBOX_TO_PC_VOLUME_RATIO 1.5625

// Xbox maximum synch playback audio
#define DSOUND_MAX_SYNCHPLAYBACK_AUDIO 29

#define vector_ds_buffer std::vector<xbox::XbHybridDSBuffer*>
#define vector_ds_stream std::vector<xbox::X_CDirectSoundStream*>
extern vector_ds_buffer                    g_pDSoundBufferCache;
extern vector_ds_stream                    g_pDSoundStreamCache;
extern LPDIRECTSOUND8               g_pDSound8; //This is necessary in order to allow share with EmuDSoundInline.hpp
extern LPDIRECTSOUNDBUFFER          g_pDSoundPrimaryBuffer;
//TODO: RadWolfie - How to implement support if primary does not permit it for DSP usage?
extern LPDIRECTSOUNDBUFFER8         g_pDSoundPrimaryBuffer8;
extern LPDIRECTSOUND3DLISTENER8     g_pDSoundPrimary3DListener8;
extern int                          g_bDSoundCreateCalled;
extern unsigned int                        g_iDSoundSynchPlaybackCounter;
// Managed memory xbox audio variables
extern DWORD                               g_dwXbMemAllocated;
extern DWORD                               g_dwFree2DBuffers;
extern DWORD                               g_dwFree3DBuffers;

struct DsBufferStreaming {
	DWORD streamInterval = 1;
	DWORD streamAhead = 50;
	float tweakCopyOffset = 0;
};
extern DsBufferStreaming g_dsBufferStreaming;

// size of DirectSound cache max size
#define X_DIRECTSOUND_CACHE_MAX 0x800

#define X_DIRECTSOUND_CACHE_COUNT (g_pDSoundBufferCache.size() + g_pDSoundStreamCache.size())

#define RETURN_RESULT_CHECK(hRet) { \
    static bool bPopupShown = false; if (!bPopupShown && hRet) { bPopupShown = true; \
    printf("Return result report: 0x%08X\nIn %s (%s)\n", hRet, __func__, __FILE__); \
    EmuLog(LOG_LEVEL::WARNING, "An issue has been found. Please report game title and console's output of return result," \
    " function, and file name to https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/485"); } return hRet; }

extern void DirectSoundDoWork_Buffer(xbox::LARGE_INTEGER& time);
extern void DirectSoundDoWork_Stream(xbox::LARGE_INTEGER& time);
extern void dsound_async_worker();
extern void dsound_worker();
extern uint64_t dsound_next(uint64_t now);
