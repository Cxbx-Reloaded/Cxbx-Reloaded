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
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2019      RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <string>

// Command Line Interface functions
// NOTE - Reason only provide functions is to prevent misuse "exec"

namespace cli_config {

static constexpr char exec[] = "exec";
static constexpr char arg1[] = "arg1";
static constexpr char load[] = "load";
static constexpr char hwnd[] = "hwnd";
static constexpr char debug_mode[] = "dm";
static constexpr char debug_file[] = "df";
static constexpr char sid[] = "sid";
static constexpr char system_retail[] = "retail";
static constexpr char system_devkit[] = "devkit";
static constexpr char system_chihiro[] = "chihiro";

bool GenConfig(char** argv, int argc);
size_t ConfigSize();
bool GenCMD(std::string& cmd_line_out);

// Generic check if key exist
bool hasKey(const std::string key);
// Generic getter
bool GetValue(const std::string key, std::string* value);

long long GetSessionID();

// Change xbe path to launch.
void SetLoad(const std::string value);

void SetSystemType(const std::string value);
void ClearSystemType();

}
