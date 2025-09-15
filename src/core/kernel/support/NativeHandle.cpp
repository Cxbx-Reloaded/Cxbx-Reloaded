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

typedef struct {
	std::string HashStr;
	size_t Counter;
} RegisteredString;
std::unordered_map<std::size_t, RegisteredString> g_HashStrings;

typedef struct {
	HANDLE handle;
	std::size_t StrHash;
} RegisteredObject;
std::unordered_map<xbox::PVOID, RegisteredObject> g_RegisteredObjects;
std::unordered_map<xbox::PVOID, RegisteredObject> g_RegisteredPartitions;

void StrHashDereference(const std::size_t& StrHash)
{
	// If string hash size is zero, skip it.
	if (!StrHash) {
		return;
	}
	const auto& it = g_HashStrings.find(StrHash);
	if (it != g_HashStrings.end()) {
		it->second.Counter--;
		// If counter is zero, then erase it.
		if (!it->second.Counter) {
			g_HashStrings.erase(it);
		}
		return;
	}

	// Should not be reachable. If it does, then need to find where extra deference occur and correct it.
	assert(it != g_HashStrings.end());
}

void AttachStringToXboxObject(xbox::PVOID xobject, const std::string& string)
{
	if (string.empty()) {
		return;
	}

	using namespace xbox;
	POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(xobject);
	// Check if an object already has string.
	if (ObjectHeader->Flags & OB_FLAG_NAMED_OBJECT) {
		// Skip it.
		return;
	}

	std::unique_lock<std::shared_mutex> lck(g_MapMtx);

	// Find existing xobject and attach string to them.
	std::size_t StrHash = std::hash<std::string>{}(string);
	std::size_t counter{};
	const auto& it1 = g_RegisteredObjects.find(xobject);
	if (it1 != g_RegisteredObjects.end()) {
		// Dereference if previous string hash size is non-zero.
		StrHashDereference(it1->second.StrHash);
		// Bind string hash to xobject
		it1->second.StrHash = StrHash;
		counter++;
	}
	const auto& it2 = g_RegisteredPartitions.find(xobject);
	if (it2 != g_RegisteredPartitions.end()) {
		// Dereference if previous string hash size is non-zero.
		StrHashDereference(it2->second.StrHash);
		// Bind string hash to xobject
		it2->second.StrHash = StrHash;
		counter++;
	}

	// If we don't have any counter, then it is invalid parameter for xobject.
	if (!counter) {
		return;
	}
	// Add string to the list
	const auto& it3 = g_HashStrings.find(StrHash);
	if (it3 != g_HashStrings.end()) {
		// Reference existing string.
		it3->second.Counter += counter;
	}
	else {
		// Register new string.
		RegisteredString hash_str = {string, counter};
		g_HashStrings[StrHash] = hash_str;
	}
}

std::string GetObjectString(xbox::PVOID xobject)
{
	std::shared_lock<std::shared_mutex> lck(g_MapMtx);
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::unordered_map<xbox::PVOID, RegisteredObject>* nHandleList;
	// Check if type is device and does not have any remaining name length
	if (ObjectType->Type == xbox::IO_TYPE_DEVICE) {
		nHandleList = &g_RegisteredPartitions;
	}
	// Otherwise, perform normal routine
	else {
		nHandleList = &g_RegisteredObjects;
	}
	const auto& it = nHandleList->find(xobject);
	if (it == nHandleList->end()) {
		return "";
	}
	else {
		return g_HashStrings[it->second.StrHash].HashStr;
	}
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

template<bool PartitionConversion>
void RegisterXboxObject(xbox::PVOID xobject, HANDLE nhandle)
{
	std::unique_lock<std::shared_mutex> lck(g_MapMtx);
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::unordered_map<xbox::PVOID, RegisteredObject>* nHandleList;
	// Check if type is device and does not have any remaining name length
	if (PartitionConversion && ObjectType->Type == xbox::IO_TYPE_DEVICE) {
		nHandleList = &g_RegisteredPartitions;
	}
	// Otherwise, perform normal routine
	else {
		nHandleList = &g_RegisteredObjects;
	}

	auto ret = nHandleList->try_emplace(xobject, RegisteredObject{ .handle = nhandle, .StrHash = 0 });
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
			ret = nHandleList->try_emplace(xobject, RegisteredObject{ .handle = nhandle, .StrHash = 0 });
			if (ret.second) {
				return;
			}
			now += std::chrono::milliseconds(5);
		}
		assert(ret.second == true);
		// If we reach here, it means that we could not insert the handle after more than two seconds of trying. This probably means
		// that we have forgotten to call RemoveXboxObject on the old xobject, or the other thread is waiting/deadlocked, so this is a bug
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
		NtDll::NtClose(it1->second.handle);
		StrHashDereference(it1->second.StrHash);
		[[maybe_unused]] auto ret = g_RegisteredObjects.erase(xobject);
		assert(ret == 1);
	}
	const auto& it2 = g_RegisteredPartitions.find(xobject);
	if (it2 != g_RegisteredPartitions.end()) {
		NtDll::NtClose(it2->second.handle);
		StrHashDereference(it2->second.StrHash);
		[[maybe_unused]] auto ret = g_RegisteredPartitions.erase(xobject);
		assert(ret == 1);
	}
}

template<bool PartitionConversion>
std::optional<HANDLE> GetObjectNativeHandle(xbox::PVOID xobject)
{
	std::shared_lock<std::shared_mutex> lck(g_MapMtx);
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::unordered_map<xbox::PVOID, RegisteredObject>* nHandleList;
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
		return it->second.handle;
	}
}
template std::optional<HANDLE> GetObjectNativeHandle<true>(xbox::PVOID xobject);
template std::optional<HANDLE> GetObjectNativeHandle<false>(xbox::PVOID xobject);

// ******************************************************************
// Convert object to string
// - Mainly used for convert back to absolute file path
// ******************************************************************

static std::string DeviceObject2Str(xbox::PVOID xobject)
{
	xbox::PDEVICE_OBJECT DeviceObject = reinterpret_cast<xbox::PDEVICE_OBJECT>(xobject);
	std::string path;
	switch (DeviceObject->DeviceType) {
	case xbox::FILE_DEVICE_DISK2: {
		xbox::PIDE_DISK_EXTENSION DiskExtension = reinterpret_cast<xbox::PIDE_DISK_EXTENSION>(DeviceObject->DeviceExtension);
		path = DeviceHarddisk0PartitionPrefix + std::to_string(DiskExtension->PartitionInformation.PartitionNumber);
		break;
	}
	case xbox::FILE_DEVICE_CD_ROM2:
		path = DeviceCdrom0;
		break;
	case xbox::FILE_DEVICE_MEDIA_BOARD:
		path = DriveMbfs; // TODO: Verify if need new string for MediaBoard.
		break;
	case xbox::FILE_DEVICE_MEMORY_UNIT: {
		xbox::PMU_EXTENSION MuExtension = reinterpret_cast<xbox::PMU_EXTENSION>(DeviceObject->DeviceExtension);
		path = DeviceMUPrefix + std::to_string(MuExtension->PartitionNumber);
		break;
	}
	default:
		path = "$unknown";
		break;
	}

	return path;
}

static std::string NamedObject2Str(xbox::PVOID xobject)
{
	using namespace xbox;
	xbox::POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(xobject);
	if (ObjectHeader->Flags & OB_FLAG_NAMED_OBJECT) {
		xbox::POBJECT_HEADER_NAME_INFO ObjectNameInfo = OBJECT_TO_OBJECT_HEADER_NAME_INFO(xobject);
		return std::string(ObjectNameInfo->Name.Buffer, ObjectNameInfo->Name.Length);
	}

	return GetObjectString(xobject);
}

std::string Object2RemainingPath(xbox::PVOID xobject)
{
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::string RemainingName;
	switch (ObjectType->Type) {
	case xbox::IO_TYPE_FILE: {
		xbox::PFILE_OBJECT FileObject = reinterpret_cast<xbox::PFILE_OBJECT>(xobject);

		std::string ObjectName = NamedObject2Str(FileObject);

		if (FileObject->RelatedFileObject) {
			RemainingName = Object2RemainingPath(FileObject->RelatedFileObject);
			if (!ObjectName.empty()) {
				RemainingName += "\\" + NamedObject2Str(FileObject);
			}
		}
		else {
			RemainingName = ObjectName;
		}
		break;
	}
	default:
		RemainingName = "";
		break;
	}

	return RemainingName;
}

std::string FileObject2AbsolutePath(xbox::PVOID xobject)
{
	std::string path, RemainingName;
	xbox::PFILE_OBJECT FileObject = reinterpret_cast<xbox::PFILE_OBJECT>(xobject);

	RemainingName = Object2RemainingPath(xobject);

	if (RemainingName.empty()) {
		path = DeviceObject2Str(FileObject->DeviceObject);
	}
	else {
		path = DeviceObject2Str(FileObject->DeviceObject) + "\\" + RemainingName;
	}

	return path;
}

std::string Object2AbsolutePath(xbox::PVOID xobject)
{
	xbox::PTYPE_OBJECT ObjectType = reinterpret_cast<xbox::PTYPE_OBJECT>(xobject);
	std::string path;
	switch (ObjectType->Type) {
	case xbox::IO_TYPE_DEVICE:
		path = DeviceObject2Str(xobject);
		break;
	case xbox::IO_TYPE_FILE:
		path = FileObject2AbsolutePath(xobject);
		break;
	default:
		path = "$unknown";
		break;
	}

	return path;
}
