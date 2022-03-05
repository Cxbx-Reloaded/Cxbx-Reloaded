// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

#define LOG_PREFIX CXBXR_MODULE::GUI

#include <thread>

#include "audio.hpp"

#include "EmuShared.h"

bool ImGuiAudio::Initialize()
{
	g_EmuShared->GetImGuiAudioWindows(&m_windows);
	return true;
}

void ImGuiAudio::Shutdown()
{
	g_EmuShared->SetImGuiAudioWindows(&m_windows);
}

void ImGuiAudio::DrawMenu()
{
	if (ImGui::BeginMenu("Audio")) {
		ImGui::MenuItem("Debug General Cache Stats", NULL, &m_windows.cache_stats_general);
		ImGui::EndMenu();
	}
}

void ImGuiAudio::DrawWidgets(bool is_focus, ImGuiWindowFlags input_handler)
{
	//TODO: In need of make interface class to return generic info in some way.
	extern void DSound_PrintStats(bool, ImGuiWindowFlags, bool m_show_audio_stats);
	DSound_PrintStats(is_focus, input_handler, m_windows.cache_stats_general);
}
