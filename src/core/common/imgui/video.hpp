// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.
#pragma once

#include <imgui.h>
#include "settings.h"

class ImGuiVideo
{
public:
	ImGuiVideo() = default;
	virtual ~ImGuiVideo() = default;

	bool Initialize();
	void Shutdown();

	void DrawMenu();
	void DrawWidgets(bool is_focus, ImGuiWindowFlags input_handler);

protected:
	imgui_video_windows m_windows;
};
