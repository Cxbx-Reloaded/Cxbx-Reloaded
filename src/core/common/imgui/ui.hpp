// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.
//
// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

#pragma once

#include <imgui.h>

#include <mutex>
#include <functional>

#include "settings.h"
#include "audio.hpp"
#include "video.hpp"

constexpr float IMGUI_MIN_DIST_TOP = 20.0f;
constexpr float IMGUI_MIN_DIST_SIDE = 1.0f;

class ImGuiUI
{
public:
	ImGuiUI() = default;
	virtual ~ImGuiUI() = default;

	void ToggleImGui();
	bool IsImGuiFocus();
	void UpdateFPSCounter();

	void DrawMenu();
	void DrawWidgets();
	void DrawLightgunLaser(int port);

protected:

	bool Initialize();
	void Shutdown();

	template<class C, class T>
	void Render(C callback, T arg)
	{
		// Some games seem to call Swap concurrently, so we need to ensure only one thread
		// at a time can render ImGui
		std::unique_lock lock(m_imgui_mutex, std::try_to_lock);
		if (!lock) return;

		callback(this, arg);
	}

	void UpdateCurrentMSpFAndFPS();

	std::mutex m_imgui_mutex;
	ImGuiContext* m_imgui_context;
	char m_file_path[FILENAME_MAX+1];
	bool m_is_focus;
	// Using as their own member than integrate may be helpful to bind different plugin at latter change?
	ImGuiAudio m_audio;
	ImGuiVideo m_video;
	overlay_settings m_settings;
	unsigned int m_lle_flags;
	float fps_counter;
	static const ImColor m_laser_col[4];
	// Make them as settings storage.
	/*bool m_show_fps;
	bool m_show_LLE_stats;
	// Make them as memory storage.
	bool m_show_vertex_stats;
	bool m_show_audio_stats;
	*/
};
