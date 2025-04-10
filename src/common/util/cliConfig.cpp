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

#include <chrono>
#include <unordered_map>

#include "cliConverter.hpp"
#include "cliConfig.hpp"

std::unordered_map<std::string, std::string> g_cli_configs;

namespace cli_config {

bool GenConfig(char** argv, int argc)
{
    g_cli_configs = cliToMapPairs(argv, argc);
    return (g_cli_configs.size() != 0);
}

size_t ConfigSize()
{
    return g_cli_configs.size();
}

bool GenCMD(std::string& cmd_line_out)
{
    cmd_line_out = cliMapPairsToString(g_cli_configs);
    return (cmd_line_out.length() != 0);
}

// Generic check if key exist
bool hasKey(std::string key)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end()) {
        return true;
    }
    return false;
}

// Delete the key if it exist
void DeleteKey(std::string key)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end()) {
        g_cli_configs.erase(found);
    }
}

// Generic getter
bool GetValue(const std::string key, std::string* value)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end() && found->second.length() != 0) {
        if (value != nullptr) {
            *value = found->second;
        }
        return true;
    }

    if (value != nullptr) {
        *value = "";
    }
    return false;
}
bool GetValue(const std::string key, int* value)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end() && found->second.length() != 0) {
        if (value != nullptr) {
            *value = std::stoi(found->second, nullptr, 10);
        }
        return true;
    }

    if (value != nullptr) {
        *value = 0;
    }
    return false;
}
bool GetValue(const std::string key, long long* value)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end() && found->second.length() != 0) {
        if (value != nullptr) {
            *value = std::stoll(found->second, nullptr, 10);
        }
        return true;
    }

    if (value != nullptr) {
        *value = 0;
    }
    return false;
}

// Generic setter (allow modification from gui/cli, emulation does not need it.)
void SetValue(const std::string key, const std::string value)
{
    auto found = g_cli_configs.find(key);
    if (found != g_cli_configs.end()) {
        found->second = value;
    }
    else {
        g_cli_configs[key] = value;
    }
}
void SetValue(const std::string key, const char* value)
{
    SetValue(key, std::string(value));
}
void SetValue(const std::string key, const void* value)
{
    SetValue(key, std::to_string((size_t)value));
}
void SetValue(const std::string key, int value)
{
    SetValue(key, std::to_string(value));
}
void SetValue(const std::string key, long long value)
{
    SetValue(key, std::to_string(value));
}

// Custom setter for emulation accessible.
void SetLoad(const std::string value)
{
    SetValue(cli_config::load, value);
}

void SetSID(long long value)
{
    // If sid key exist, then do not replace old or new one.
    if (!hasKey(cli_config::sid)) {
        SetValue(cli_config::sid, value);
    }
}

long long GetSessionID()
{
    long long sessionID = 0;

    // Check if previous session ID had been set then use it.
    if (!GetValue(cli_config::sid, &sessionID)) {
        sessionID = std::chrono::steady_clock::now().time_since_epoch().count();
        // From now and the future will continue to use the same sessionID until all processes end.
        SetSID(sessionID);
    }

    return sessionID;
}

void SetSystemType(const std::string value)
{
    // If system types key exist, then do not replace old one.
    if (hasKey(cli_config::system_retail)
        || hasKey(cli_config::system_devkit)
        || hasKey(cli_config::system_chihiro)) {
        return;
    }
    // If one of system types match, then set it.
    if (value.compare(cli_config::system_retail) == 0
        || value.compare(cli_config::system_devkit) == 0
        || value.compare(cli_config::system_chihiro) == 0) {
        SetValue(value, "");
    }
}

void ClearSystemType()
{
    // Clear any system types key existence.
    DeleteKey(cli_config::system_retail);
    DeleteKey(cli_config::system_devkit);
    DeleteKey(cli_config::system_chihiro);
}

}
