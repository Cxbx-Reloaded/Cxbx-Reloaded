// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.
//
// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

#pragma once

#include "../imgui/ui.hpp"

class RenderBase : public ImGuiUI
{
public:
	RenderBase() = default;
	virtual ~RenderBase() = default;

	virtual bool Initialize();
	virtual void Shutdown();

	template<class C, class T>
	void Render(std::function<void(C, T)> callback, T arg)
	{
		ImGuiUI::Render(callback, arg);
	}
};

extern std::unique_ptr<RenderBase> g_renderbase;
