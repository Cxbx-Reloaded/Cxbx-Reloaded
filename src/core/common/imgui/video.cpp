// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

#define LOG_PREFIX CXBXR_MODULE::GUI

#include <thread>

#include "video.hpp"
#include "ui.hpp"

#include "EmuShared.h"

#include "core/kernel/init/CxbxKrnl.h"

bool ImGuiVideo::Initialize()
{
	g_EmuShared->GetImGuiVideoWindows(&m_windows);
	return true;
}

void ImGuiVideo::Shutdown()
{
	g_EmuShared->SetImGuiVideoWindows(&m_windows);
}

void ImGuiVideo::DrawMenu()
{
	if (ImGui::BeginMenu("Video")) {
		ImGui::MenuItem("Show Vertex Stats", NULL, &m_windows.cache_stats_vertex);
		ImGui::EndMenu();
	}
}

void ImGuiVideo::DrawWidgets(bool is_focus, ImGuiWindowFlags input_handler)
{
	//TODO: move into plugin class usage.
	extern void CxbxImGui_Video_DrawWidgets(bool, ImGuiWindowFlags, bool);
	CxbxImGui_Video_DrawWidgets(is_focus, input_handler, m_windows.cache_stats_vertex);
}
