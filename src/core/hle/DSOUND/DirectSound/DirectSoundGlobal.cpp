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
#define LOG_PREFIX CXBXR_MODULE::DSOUND

#include <imgui.h>
#include "core/common/imgui/ui.hpp"

#include <core\kernel\exports\xboxkrnl.h>
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

void DSound_PrintStats(bool is_focus, ImGuiWindowFlags input_handler, bool m_show_audio_stats)
{
    DSoundMutexGuardLock;

    if (m_show_audio_stats) {

        ImGui::SetNextWindowPos(ImVec2(IMGUI_MIN_DIST_SIDE, IMGUI_MIN_DIST_TOP), ImGuiCond_FirstUseEver, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(200, 275), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Debugging stats", nullptr, input_handler | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
            if (ImGui::CollapsingHeader("Audio Buffers", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::Text("DirectSound Cache:");
                ImGui::BulletText("Total DSBuffer cache = %u", g_pDSoundBufferCache.size());
                ImGui::BulletText("Total DSStream cache = %u", g_pDSoundStreamCache.size());
                ImGui::Separator();

                // Generate DSBuffer stats

                DWORD dwStatus;
                HRESULT hRet;
                unsigned index = 0, isActive = 0;

                if (ImGui::CollapsingHeader("Active DSBuffer cache")) {

                    for (const auto& i : g_pDSoundBufferCache) {
                        const auto& buffer = i->emuDSBuffer;
                        hRet = buffer->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
                        if (hRet == DS_OK && (dwStatus & DSBSTATUS_PLAYING) != 0) {
                            if (isActive) {
                                ImGui::Separator();
                            }
                            isActive++;

                            ImGui::BulletText("DSBufferCache[%u] = 0x%p", index, reinterpret_cast<void*>(i));
                            ImGui::Indent();
                            ImGui::BulletText("status = %s", ((dwStatus & DSBSTATUS_LOOPING) != 0) ? "looping" : "play once");

                            ImGui::BulletText("X_BufferCacheSize = 0x%u", buffer->X_BufferCacheSize);
                            ImGui::BulletText("EmuFlags = %X", buffer->EmuFlags);
                            ImGui::BulletText("EmuRegionToggle = 0x%X", buffer->EmuBufferToggle);
                            if (buffer->EmuBufferToggle == xbox::X_DSB_TOGGLE_PLAY) {
                                ImGui::Indent();
                                ImGui::BulletText("EmuRegionPlayStartOffset = 0x%X", buffer->EmuRegionPlayStartOffset);
                                ImGui::BulletText("EmuRegionPlayLength = 0x%X", buffer->EmuRegionPlayLength);
                                ImGui::Unindent();
                            }
                            else if (buffer->EmuBufferToggle == xbox::X_DSB_TOGGLE_LOOP) {
                                ImGui::Indent();
                                ImGui::BulletText("EmuRegionLoopStartOffset = 0x%X", buffer->EmuRegionLoopStartOffset);
                                ImGui::BulletText("EmuRegionLoopLength = 0x%X", buffer->EmuRegionLoopLength);
                                ImGui::Unindent();
                            }
                            ImGui::Unindent();
                        }
                        index++;
                    }

                    if (isActive == 0) {
                        ImGui::BulletText("(none)");
                    }

                    ImGui::Text("Total active DSBuffer = %u", isActive);
                }
                // Generate DSStream stats

                index = 0;
                isActive = 0;

                if (ImGui::CollapsingHeader("Active DSStream cache")) {
                    for (const auto& i : g_pDSoundStreamCache) {
                        hRet = i->EmuDirectSoundBuffer8->GetStatus(&dwStatus);
                        if (hRet == DS_OK && (dwStatus & DSBSTATUS_PLAYING) != 0) {
                            if (isActive) {
                                ImGui::Separator();
                            }
                            isActive++;
                            ImGui::BulletText("DSStreamCache[%u] = 0x%p", index, reinterpret_cast<void*>(i));
                            ImGui::Indent();
                            ImGui::BulletText("Max packets allow = %u", i->X_MaxAttachedPackets);
                            ImGui::BulletText("Total packets = %u", i->Host_BufferPacketArray.size());
                            ImGui::BulletText("is processing = %d", i->Host_isProcessing);
                            ImGui::BulletText("EmuFlags = %X", i->EmuFlags);
                            ImGui::BulletText("Xb_Status = %X", i->Xb_Status);
                            ImGui::Unindent();
                        }
                        index++;
                    }

                    if (isActive == 0) {
                        ImGui::BulletText("(none)");
                    }

                    ImGui::Text("Total active DSStream = %u", isActive);
                }
            }
            ImGui::End();
        }
    }
}
