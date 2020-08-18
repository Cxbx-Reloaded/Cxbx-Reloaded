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


#include <xboxkrnl/xboxkrnl.h>
#include <dsound.h>
#include "DirectSoundGlobal.hpp"

Settings::s_audio            g_XBAudio = { 0 };
std::recursive_mutex         g_DSoundMutex;

//Currently disabled since below may not be needed since under -6,400 is just silence yet accepting up to -10,000
// Xbox to PC volume ratio format (-10,000 / -6,400 )
//#define XBOX_TO_PC_VOLUME_RATIO 1.5625

vector_ds_buffer                    g_pDSoundBufferCache;
vector_ds_stream                    g_pDSoundStreamCache;
LPDIRECTSOUND8               g_pDSound8 = nullptr; //This is necessary in order to allow share with EmuDSoundInline.hpp
LPDIRECTSOUNDBUFFER          g_pDSoundPrimaryBuffer = nullptr;
//TODO: RadWolfie - How to implement support if primary does not permit it for DSP usage?
LPDIRECTSOUNDBUFFER8         g_pDSoundPrimaryBuffer8 = nullptr;
LPDIRECTSOUND3DLISTENER8     g_pDSoundPrimary3DListener8 = nullptr;
int                          g_bDSoundCreateCalled = FALSE;
unsigned int                        g_iDSoundSynchPlaybackCounter = 0;
// Managed memory xbox audio variables
DWORD                               g_dwXbMemAllocated = 0;
DWORD                               g_dwFree2DBuffers = 0;
DWORD                               g_dwFree3DBuffers = 0;
