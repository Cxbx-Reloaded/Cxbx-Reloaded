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

	// When video backends has its own class, make DeviceRelease call as virtual requirement for parent class usage.
	void SetDeviceRelease(const std::function<void()>& func_register) {
		m_device_release = func_register;
	}

	void DeviceRelease() {
		if (m_device_release) {
			m_device_release();
		}
	}

	void SetWindowRelease(const std::function<void()>& func_register) {
		m_window_release = func_register;
	}

	void WindowRelease() {
		if (m_window_release) {
			m_window_release();
		}
	}

protected:

	std::function<void()> m_device_release;
	std::function<void()> m_window_release;
};

extern std::unique_ptr<RenderBase> g_renderbase;
