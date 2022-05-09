// ******************************************************************
// *
// *  This file is part of the Cxbx-Reloaded project.
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
// *  (c) 2021 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

// Intended to store as permanent settings

typedef struct {
	bool build_hash;
	bool fps;
	bool hle_lle_stats;
	bool title_name;
	bool file_name;
} overlay_settings;

// Intended for EmuShared only below

const int IMGUI_INI_SIZE_MAX = 1024;

typedef struct {
	bool is_focus;
	bool Reserved[3];
	int  ini_size; // Cannot be touch anywhere except EmuShared's constructor.
	char ini_settings[IMGUI_INI_SIZE_MAX];
} imgui_general;

typedef struct {
	bool cache_stats_general;
	bool cache_visualization;
	bool Reserved[3];
} imgui_audio_windows;

typedef struct {
	bool cache_stats_vertex;
	bool Reserved[3];
} imgui_video_windows;
