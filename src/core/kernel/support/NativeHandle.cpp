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
#include <chrono>
#include <unordered_map>
#include <shared_mutex>
#include "Windows.h"
#include "assert.h"
#include "NativeHandle.h"
#include "core\kernel\init\CxbxKrnl.h"
#include "core/kernel/support/EmuFS.h"


std::unordered_map<xbox::HANDLE, HANDLE> g_RegisteredHandles;
std::shared_mutex g_MapMtx;

void RegisterXboxHandle(xbox::HANDLE xhandle, HANDLE nhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	auto ret = g_RegisteredHandles.try_emplace(xhandle, nhandle);
	if (ret.second == false) {
		// This can happen when an ob handle has been destroyed, but then a thread switch happens before the first thread
		// got a chance to remove the old handle from g_RegisteredHandles with RemoveXboxHandle
		// Test case: dashboard
		auto now = std::chrono::system_clock::now();
		auto timeout = now + std::chrono::milliseconds(2000);
		while (now <= timeout) {
			lck.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			lck.lock();
			ret = g_RegisteredHandles.try_emplace(xhandle, nhandle);
			if (ret.second) {
				return;
			}
			now += std::chrono::milliseconds(5);
		}

		// If we reach here, it means that we could not insert the handle after more than two seconds of trying. This probably means
		// that we have forgotten to call RemoveXboxHandle on the old handle, or the other thread is waiting/deadlocked, so this is a bug
		CxbxrAbortEx(CXBXR_MODULE::CXBXR, "Failed to register new xbox handle after more than two seconds!");
	}
}

void RemoveXboxHandle(xbox::HANDLE xhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	[[maybe_unused]] auto ret = g_RegisteredHandles.erase(xhandle);
	assert(ret == 1);
}

template<bool NoConversion>
std::optional<HANDLE> GetNativeHandle(xbox::HANDLE xhandle)
{
	// If SourceHandle is -2 = NtCurrentThread, then we are searching the handle of this thread
	// Test case: Metal Slug 3
	if (xhandle == NtCurrentThread()) {
		// Only used for threads as Windows doesn't allow non-special handle for same thread.
		if (NoConversion) {
			return xhandle;
		}
		else {
			xhandle = xbox::PspGetCurrentThread()->UniqueThread;
		}
	}
	// If xhandle is not special handle, check if it's the same as current thread.
	// Only used for threads as Windows doesn't allow non-special handle for same thread.
	// This will only triggered within kernel functions i.e. KeSetDisableBoostThread and KeSetBasePriorityThread.
	else if (NoConversion) {
		if (xhandle == xbox::PspGetCurrentThread()->UniqueThread) {
			return NtCurrentThread();
		}
	}

	std::shared_lock<std::shared_mutex> lck(g_MapMtx);
	const auto &it = g_RegisteredHandles.find(xhandle);
	if (it == g_RegisteredHandles.end()) {
		return std::nullopt;
	}
	else {
		return it->second;
	}
}

template std::optional<HANDLE> GetNativeHandle<true>(xbox::HANDLE xhandle);
template std::optional<HANDLE> GetNativeHandle<false>(xbox::HANDLE xhandle);
