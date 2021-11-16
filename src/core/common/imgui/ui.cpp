// Copyright 2021 Cxbx-Reloaded Project
// Licensed under GPLv2+
// Refer to the COPYING file included.
//
// Copyright 2010 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the COPYING file included.

#define LOG_PREFIX CXBXR_MODULE::GUI

#include <thread>

#include "ui.hpp"
#include "EmuShared.h"

#include "core/kernel/init/CxbxKrnl.h"

const ImColor ImGuiUI::m_laser_col[4] = {
		ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), // ply1: red
		ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), // ply2: green
		ImColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f)), // ply3: blue
		ImColor(ImVec4(1.0f, 1.0f, 0.0f, 1.0f))  // ply4: yellow
};

bool ImGuiUI::Initialize()
{
	IMGUI_CHECKVERSION();
	m_imgui_context = ImGui::CreateContext();
	if (!m_imgui_context) {
		CxbxrKrnlAbort("Unable to create ImGui context!");
		return false;
	}

	ImGuiIO& io = ImGui::GetIO();
#if 0 // TODO: Currently most voted for memory, so this block of code is disabled. And may will add an option between file vs memory.
	// May be best ideal to do manual update call than ImGui's internal auto update.
	g_EmuShared->GetDataLocation(m_file_path);
	if (m_file_path[0] == '\0') {
		return false;
	}
	strcat_s(m_file_path, "/imgui.ini");
	io.IniFilename = m_file_path;
#else
	io.IniFilename = nullptr;
	char temp_ini_settings[IMGUI_INI_SIZE_MAX];
	g_EmuShared->GetImGuiIniSettings(temp_ini_settings);
	ImGui::LoadIniSettingsFromMemory(temp_ini_settings, IMGUI_INI_SIZE_MAX);
#endif

	ImGui::StyleColorsDark();
	g_EmuShared->GetImGuiFocusFlag(&m_is_focus);

	g_EmuShared->GetOverlaySettings(&m_settings);
	g_EmuShared->GetFlagsLLE(&m_lle_flags);

	// Internal initialize (when necessary, move into its own function.)
	fps_counter = 30.0f;

	// Miscs
	m_audio.Initialize();
	m_video.Initialize();

	return true;
}

void ImGuiUI::Shutdown()
{
	size_t ini_size = IMGUI_INI_SIZE_MAX;
	const char* temp_ini_settings = ImGui::SaveIniSettingsToMemory(&ini_size);
	if (ini_size > IMGUI_INI_SIZE_MAX) {
		CxbxrKrnlAbort("ImGui ini settings is too large: %d > %d (IMGUI_INI_SIZE_MAX)", ini_size, IMGUI_INI_SIZE_MAX);
	}
	g_EmuShared->SetImGuiIniSettings(temp_ini_settings);
	g_EmuShared->SetOverlaySettings(&m_settings);
	m_audio.Shutdown();
	m_video.Shutdown();
	ImGui::DestroyContext(m_imgui_context);
}

bool ImGuiUI::IsImGuiFocus()
{
	return m_is_focus;
}

void ImGuiUI::ToggleImGui()
{
	m_is_focus = !m_is_focus;
	g_EmuShared->SetImGuiFocusFlag(m_is_focus);
}

static clock_t      g_DeltaTime = 0; // Used for benchmarking/fps count
static unsigned int g_Frames = 0;

// ******************************************************************
// * update the current milliseconds per frame
// ******************************************************************
void ImGuiUI::UpdateCurrentMSpFAndFPS() {
	if (g_EmuShared) {

		fps_counter = (float)(g_Frames * 0.5 + fps_counter * 0.5);
		g_EmuShared->SetCurrentFPS(&fps_counter);
	}
}

void ImGuiUI::UpdateFPSCounter()
{
	static clock_t lastDrawFunctionCallTime = 0;
	clock_t currentDrawFunctionCallTime = clock();

	g_DeltaTime += currentDrawFunctionCallTime - lastDrawFunctionCallTime;
	lastDrawFunctionCallTime = currentDrawFunctionCallTime;
	g_Frames++;

	if (g_DeltaTime >= CLOCKS_PER_SEC) {
		UpdateCurrentMSpFAndFPS();
		g_Frames = 0;
		g_DeltaTime -= CLOCKS_PER_SEC;
	}
}

void ImGuiUI::DrawMenu()
{
	if (!m_is_focus) {
		return;
	}
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Settings")) {
			if (ImGui::BeginMenu("Overlay")) {
				bool bChanged = false;
				bChanged |= ImGui::MenuItem("Show FPS", NULL, &m_settings.fps);
				bChanged |= ImGui::MenuItem("Show HLE/LLE Stats", NULL, &m_settings.hle_lle_stats);
				if (bChanged) {
					g_EmuShared->SetOverlaySettings(&m_settings);
					ipc_send_gui_update(IPC_UPDATE_GUI::OVERLAY, 1);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		m_video.DrawMenu();
		m_audio.DrawMenu();
		ImGui::EndMainMenuBar();
	}
}

void ImGuiUI::DrawWidgets()
{
	if (m_settings.fps || m_settings.hle_lle_stats) {

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - (IMGUI_MIN_DIST_SIDE/* * m_backbuffer_scale*/),
			IMGUI_MIN_DIST_TOP/* * m_backbuffer_scale*/), ImGuiCond_Always, ImVec2(1.0f, 0.0f));

		ImGui::SetNextWindowSize(ImVec2(200.0f/* * m_backbuffer_scale*/, 0.0f));
		ImGui::SetNextWindowBgAlpha(0.5f);
		if (ImGui::Begin("overlay_stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing)) {

			if (m_settings.fps) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS: %.2f  MS / F : %.2f", fps_counter, (float)(1000.0 / fps_counter));
			}

			if (m_settings.hle_lle_stats) {
				std::string flagString = "LLE-";

				// TODO: Need improvement in upstream for all of bLLE_xxx globals
				// Set LLE flags string based on selected LLE flags
				if (m_lle_flags & LLE_APU) {
					flagString.append("A");
				}
				if (m_lle_flags & LLE_GPU) {
					flagString.append("G");
				}
				if (m_lle_flags & LLE_USB) {
					flagString.append("U");
				}
				if (m_lle_flags & LLE_JIT) {
					flagString.append("J");
				}
				if (m_lle_flags == 0) {
					flagString = "HLE";
				}

				// Align text to the right
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() - ImGui::CalcTextSize(flagString.c_str()).x
					- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), flagString.c_str());
			}
			ImGui::End();
		}
	}

	ImGuiWindowFlags input_handler = m_is_focus ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoInputs;

	m_video.DrawWidgets(m_is_focus, input_handler);

	m_audio.DrawWidgets(m_is_focus, input_handler);
}

void ImGuiUI::DrawLightgunLaser(int port)
{
	ImGui::Begin("Laser", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration);

	ImGui::GetForegroundDrawList()->AddCircleFilled(g_InputDeviceManager.CalcLaserPos(port), 5, m_laser_col[port], 0);

	ImGui::End();
}
