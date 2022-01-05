// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2021 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#include <core\kernel\exports\xboxkrnl.h>
#include <unordered_map>
#include <shared_mutex>
#include "Windows.h"
#include "assert.h"
#include "NativeHandle.h"


std::unordered_map<xbox::HANDLE, HANDLE> g_RegisteredHandles;
std::shared_mutex g_MapMtx;

void RegisterXboxHandle(xbox::HANDLE xhandle, HANDLE nhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	[[maybe_unused]] const auto &ret = g_RegisteredHandles.emplace(xhandle, nhandle);
	// Even when duplicating xbox handles with NtDuplicateObject, the duplicate will still be different then the source handle
	assert(ret.second == true);
}

void RemoveXboxHandle(xbox::HANDLE xhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	[[maybe_unused]] auto ret = g_RegisteredHandles.erase(xhandle);
	assert(ret == 1);
}

std::optional<HANDLE> GetNativeHandle(xbox::HANDLE xhandle)
{
	// If SourceHandle is -2 = NtCurrentThread, then we are searching the handle of this thread
	// Test case: Metal Slug 3
	if (xhandle == NtCurrentThread()) {
		xhandle = xbox::PspGetCurrentThread()->UniqueThread;
	}

	std::shared_lock<std::shared_mutex> lck(g_MapMtx);
	auto &it = g_RegisteredHandles.find(xhandle);
	if (it == g_RegisteredHandles.end()) {
		return std::nullopt;
	}
	else {
		return it->second;
	}
}
