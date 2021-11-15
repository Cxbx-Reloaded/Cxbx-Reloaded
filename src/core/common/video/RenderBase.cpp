// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.
//
// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

//#define LOG_PREFIX CXBXR_MODULE::GUI

#include <thread>

#include "RenderBase.hpp"

//#include "core/kernel/init/CxbxKrnl.h"

std::unique_ptr<RenderBase> g_renderbase;

bool RenderBase::Initialize(int backbuffer_scale)
{
	if (!ImGuiUI::Initialize(backbuffer_scale)) {
		return false;
	}

	return true;
}

void RenderBase::Shutdown()
{
	DeviceRelease();
	m_device_release = std::function<void()>{};
	WindowRelease();
	m_window_release = std::function<void()>{};
	ImGuiUI::Shutdown();
}
