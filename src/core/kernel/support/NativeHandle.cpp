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
#include "EmuFile.h"

// prevent name collisions
namespace NtDll
{
	#include "core\kernel\support\EmuNtDll.h"
};

std::shared_mutex g_MapMtx;

std::unordered_map<xbox::PVOID, HANDLE> g_RegisteredObjects;
std::unordered_map<xbox::PVOID, HANDLE> g_RegisteredPartitions;

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

	// Get object from xHandle bind, then return host's handle.
	xbox::PVOID Object;
	const auto& result = xbox::ObReferenceObjectByHandle(xhandle, xbox::zeroptr, &Object);
	std::optional<HANDLE> hHandle;

	if (X_NT_SUCCESS(result)) {
		hHandle = GetObjectNativeHandle(Object);
		xbox::ObfDereferenceObject(Object);
	}
	return hHandle;
}
template std::optional<HANDLE> GetNativeHandle<true>(xbox::HANDLE xhandle);
template std::optional<HANDLE> GetNativeHandle<false>(xbox::HANDLE xhandle);

std::wstring CxbxGetFinalPathNameByHandle(HANDLE hFile)
{
	constexpr size_t INITIAL_BUF_SIZE = MAX_PATH;
	std::wstring path(INITIAL_BUF_SIZE, '\0');

	DWORD size = GetFinalPathNameByHandleW(hFile, path.data(), INITIAL_BUF_SIZE, VOLUME_NAME_DOS);
	if (size == 0) {
		return L"";
	}

	// If the function fails because lpszFilePath is too small to hold the string plus the terminating null character,
	// the return value is the required buffer size, in TCHARs. This value includes the size of the terminating null character.
	if (size >= INITIAL_BUF_SIZE) {
		path.resize(size);
		size = GetFinalPathNameByHandleW(hFile, path.data(), size, VOLUME_NAME_DOS);
	}
	path.resize(size);

	return path;
}

template<bool PartitionConversion>
void RegisterXboxObject(xbox::PVOID xobject, HANDLE nhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::unordered_map<xbox::PVOID, HANDLE>* nHandleList;
	// Check if type is device and does not have any remaining name length
	if (PartitionConversion && ObjectType->Type == xbox::IO_TYPE_DEVICE) {
		nHandleList = &g_RegisteredPartitions;
	}
	// Otherwise, perform normal routine
	else {
		nHandleList = &g_RegisteredObjects;
	}

	auto ret = nHandleList->try_emplace(xobject, nhandle);
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
			ret = nHandleList->try_emplace(xobject, nhandle);
			if (ret.second) {
				return;
			}
			now += std::chrono::milliseconds(5);
		}
		assert(ret.second == true);
		// If we reach here, it means that we could not insert the handle after more than two seconds of trying. This probably means
		// that we have forgotten to call RemoveXboxHandle on the old handle, or the other thread is waiting/deadlocked, so this is a bug
		CxbxrAbortEx(CXBXR_MODULE::CXBXR, "Failed to register new xbox handle after more than two seconds!");
	}
}
template void RegisterXboxObject<true>(xbox::PVOID xobject, HANDLE nhandle);
template void RegisterXboxObject<false>(xbox::PVOID xobject, HANDLE nhandle);

void RemoveXboxObject(xbox::PVOID xobject)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	const auto& it1 = g_RegisteredObjects.find(xobject);
	if (it1 != g_RegisteredObjects.end()) {
		NtDll::NtClose(it1->second);
		[[maybe_unused]] auto ret = g_RegisteredObjects.erase(xobject);
		assert(ret == 1);
	}
	const auto& it2 = g_RegisteredPartitions.find(xobject);
	if (it2 != g_RegisteredPartitions.end()) {
		NtDll::NtClose(it2->second);
		[[maybe_unused]] auto ret = g_RegisteredPartitions.erase(xobject);
		assert(ret == 1);
	}
}

template<bool PartitionConversion>
std::optional<HANDLE> GetObjectNativeHandle(xbox::PVOID xobject)
{
	std::shared_lock<std::shared_mutex> lck(g_MapMtx);
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::unordered_map<xbox::PVOID, HANDLE>* nHandleList;
	// Check if type is device and does not have any remaining name length
	if (PartitionConversion && ObjectType->Type == xbox::IO_TYPE_DEVICE) {
		nHandleList = &g_RegisteredPartitions;
	}
	// Otherwise, perform normal routine
	else {
		nHandleList = &g_RegisteredObjects;
	}
	const auto& it = nHandleList->find(xobject);
	if (it == nHandleList->end()) {
		return std::nullopt;
	}
	else {
		return it->second;
	}
}
template std::optional<HANDLE> GetObjectNativeHandle<true>(xbox::PVOID xobject);
template std::optional<HANDLE> GetObjectNativeHandle<false>(xbox::PVOID xobject);
