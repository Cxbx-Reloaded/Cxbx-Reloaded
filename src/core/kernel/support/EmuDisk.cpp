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
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::FILE

#include "core/kernel/exports/EmuKrnlIo.hpp"
#include "EmuDisk.hpp"
#include "EmuFile.h"
#include "NativeHandle.h"
#include "Logging.h"
#include "common/FilePaths.hpp"

#include <array>
#include <filesystem>

namespace xbox {

	static void NTAPI DiskStartIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
	}

	static ntstatus_xt NTAPI DiskReadWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI DiskFlushBuffers(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;

	}

	static ntstatus_xt NTAPI DiskDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI DriverIrpReturn(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static DRIVER_OBJECT DiskDriverObject = {
		.DriverStartIo = DiskStartIo,
		.DriverDeleteDevice = nullptr,
		.DriverUnknown2 = nullptr,
		.MajorFunction = {
			DriverIrpReturn,
			DriverIrpReturn,
			DiskReadWrite,
			DiskReadWrite,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			DiskFlushBuffers,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			DiskDeviceControl,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
		}
	};
}

static xbox::STRING xDeviceHarddisk0, xDeviceHarddisk0PartitionX, xPartitionPrefix;

struct PartitionDevice {
	xbox::PDEVICE_OBJECT DeviceObject;
	std::string HostDevicePath;
	HANDLE HostBinHandle;
	HANDLE HostRootHandle;
};

static std::array<PartitionDevice, 20> DiskPartitionArray;

static_assert(DiskPartitionArray.size() == 20, "Require partitions to be limited to 20");

static int XboxGetPartitionNumber(xbox::OBJECT_STRING& ElementName) {
	// Source has xbox::RtlPrefixString function which seems useful. Need to figure out if can do better or not.
	// convert path name to lowercase by requirement.
	std::string path_name(ElementName.Buffer, ElementName.Length);
	std::transform(path_name.begin(), path_name.end(), path_name.begin(), xbox::RtlLowerChar);
	std::string_view partitionString = xPartitionPrefix.Buffer;
	const size_t pos = path_name.rfind(partitionString);
	if (pos == std::string::npos) {
		return 0;
	}
	const std::string partitionNumberString = path_name.substr(pos + partitionString.length(), path_name.length() - partitionString.length());

	// wcstol returns 0 on non-numeric characters, so we don't need to error check here
	return strtol(partitionNumberString.data(), nullptr, 0);
}

std::string CxbxrDiskDeviceByHostPath(const std::filesystem::path& HostDiskDevicePath)
{
	for (const auto& Partition : DiskPartitionArray) {
		if (!Partition.HostDevicePath.empty()) {
			if (_strnicmp(HostDiskDevicePath.string().c_str(), Partition.HostDevicePath.c_str(), Partition.HostDevicePath.length()) == 0)
				return Partition.HostDevicePath;
		}
	}
	return "";
}

namespace xbox {

	// TODO: Need to port EmuDiskPartitionSetup into this function once IRP is implemented.
	static ntstatus_xt DiskPartitionCreate(int PartitionNumber)
	{
		// Not implemented and we don't have full support yet.
		// For now, we simply register xbox HardDisk0's partitions from EmuDiskPartitionSetup function.
		LOG_UNIMPLEMENTED();
		return X_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	static ntstatus_xt NTAPI DiskParseDirectory(
		IN PVOID ParseObject,
		IN POBJECT_TYPE ObjectType,
		IN ulong_xt Attributes,
		IN OUT POBJECT_STRING CompleteName,
		IN OUT POBJECT_STRING RemainingName,
		IN OUT PVOID ParseContext OPTIONAL,
		OUT PVOID* Object
	)
	{
		*Object = nullptr;

		if (!RemainingName->Length) {
			return X_STATUS_ACCESS_DENIED;
		}

		bool hasTrailingBackslash = false;
		if (RemainingName->Buffer[RemainingName->Length - 1] == OBJ_NAME_PATH_SEPARATOR) {
			hasTrailingBackslash = true;
		}

		OBJECT_STRING ElementName;
		ObDissectName(*RemainingName, &ElementName, RemainingName);

		if (RemainingName->Length && RemainingName->Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
			return X_STATUS_OBJECT_NAME_INVALID;
		}

		if (RemainingName->Length || hasTrailingBackslash) {
			RemainingName->Buffer--;
			RemainingName->Length++;
			RemainingName->MaximumLength = RemainingName->Length;
		}

		// Check for partition prefix
		if (_strnicmp(ElementName.Buffer, PartitionPrefix.c_str(), PartitionPrefix.length()) == 0) {
			// Find number after partition prefix from char to integer
			//
			size_t PartitionNumber = XboxGetPartitionNumber(ElementName);

			// Verify partition index is within range
			if (PartitionNumber < DiskPartitionArray.size()) {

				// Then check if partition array's device object exist
				// If not, then create one. If not success, then return status.
				// Check if partition exist
				if (DiskPartitionArray[PartitionNumber].HostDevicePath.length() == 0) {

					// If not exist, then we try create partition object
					ntstatus_xt result = DiskPartitionCreate(PartitionNumber);

					if (!X_NT_SUCCESS(result)) {
						if (result == X_STATUS_OBJECT_NAME_NOT_FOUND && RemainingName->Length != 0) {
							result = X_STATUS_OBJECT_PATH_NOT_FOUND;
						}
						return result;
					}
				}

				PartitionDevice& partition = DiskPartitionArray[PartitionNumber];

				// Pass to generic function to handle the work. Used for HardDisk & CdRom, not sure what else use it.
				return IopParseDevice(partition.DeviceObject, ObjectType, Attributes, CompleteName, RemainingName, ParseContext, Object);
			}
		}

		if (RemainingName->Length) {
			return X_STATUS_OBJECT_PATH_NOT_FOUND;
		}

		return X_STATUS_OBJECT_NAME_NOT_FOUND;
	}
}

static xbox::OBJECT_TYPE DiskDirectoryObjectType = {
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	nullptr,
	nullptr,
	&xbox::DiskParseDirectory,
	&xbox::ObpDefaultObject,
	'kisD'
};

// TODO: Get DiskDriverObject supported from export kernel functions. (require irp work)
bool EmuDiskFormatPartition(xbox::dword_xt PartitionNumber)
{
	const std::filesystem::path partitionPath = DiskPartitionArray[PartitionNumber].HostDevicePath;

	// Sanity check, make sure we are actually deleting something within the Cxbx-Reloaded folder
	if (!CxbxrIsPathInsideEmuDisk(partitionPath)) {
		EmuLog(LOG_LEVEL::WARNING, "Attempting to format a path that is not within a Cxbx-Reloaded data folder... Ignoring!\n");
		return false;
	}

	// Format the disk's partition, by iterating through the contents and removing all files/folders within
	// Previously, we deleted and re-created the folder, but that caused permission issues for some users
	std::error_code er;
	for (const auto& directoryEntry : std::filesystem::directory_iterator(partitionPath, er)) {
		if (er) {
			EmuLog(LOG_LEVEL::ERROR2, "%s", er.message().c_str());
		}
		else {
			std::filesystem::remove_all(directoryEntry, er);
		}
	}

	printf("Formatted EmuDisk Partition%d\n", PartitionNumber);
	return true;
}

static void EmuBugCheckInline(const xbox::ntstatus_xt result)
{
	if (!X_NT_SUCCESS(result)) {
		xbox::KeBugCheckEx(BCC_CXBXR_HARDDISK0, reinterpret_cast<xbox::PVOID>(result), nullptr, nullptr, nullptr);
	}
}

static void EmuDiskPartitionSetup(size_t partitionIndex, bool IsFile=false)
{
	// Make sure we do not extend over # of partitions.
	if (partitionIndex >= DiskPartitionArray.size()) {
		return;
	}

	char partitionX = '0' + static_cast<char>(partitionIndex);
	PartitionDevice& partitionPath = DiskPartitionArray[partitionIndex];
	partitionPath.HostDevicePath = g_DiskBasePath + PartitionPrefix + partitionX;

	bool succeeded{ true };
	std::filesystem::path partitionHeaderPath = partitionPath.HostDevicePath + ".bin";
	if (!std::filesystem::exists(partitionHeaderPath)) {
		CxbxCreatePartitionHeaderFile(partitionHeaderPath, partitionIndex == 0);
	}
	partitionPath.HostBinHandle = CreateFileW(partitionHeaderPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	// If this path is not a raw file partition, create the directory for it
	if (!IsFile) {
		std::error_code error; // We do not want filesystem to throw an exception on directory creation. Instead, listen for return value to fail.
		succeeded = std::filesystem::exists(partitionPath.HostDevicePath) || std::filesystem::create_directory(partitionPath.HostDevicePath, error);
	}
	if (!succeeded) {
		return;
	}

	xbox::PDEVICE_OBJECT DiskDeviceObject;
	xbox::ntstatus_xt result = xbox::IoCreateDevice(&xbox::DiskDriverObject, sizeof(xbox::IDE_DISK_EXTENSION), nullptr, xbox::FILE_DEVICE_DISK2, FALSE, &DiskDeviceObject);
	EmuBugCheckInline(result);

	// NOTE: below are incomplete reverse engineered, more research is needed to understand the initialization process.
	DiskDeviceObject->Flags |= 0x45; // TODO: What flags are these? Doesn't seem to match with ReactOS's info: https://github.com/reactos/reactos/blob/999345a4feaae1e74533c96fa2cdfa1bce50ec5f/sdk/include/xdk/iotypes.h

	DiskDeviceObject->AlignmentRequirement = 1;

	DiskDeviceObject->SectorSize = 0x200;

	xbox::PIDE_DISK_EXTENSION DiskExtension = static_cast<xbox::PIDE_DISK_EXTENSION>(DiskDeviceObject->DeviceExtension);

	DiskExtension->DeviceObject = DiskDeviceObject;
	DiskExtension->PartitionInformation.PartitionLength.QuadPart = 0; // TODO: what size?
	DiskExtension->PartitionInformation.PartitionNumber = partitionIndex;

	DiskDeviceObject->Flags &= ~X_DO_DEVICE_INITIALIZING;

	partitionPath.DeviceObject = DiskDeviceObject;

	if (succeeded) {
		partitionPath.HostRootHandle = CreateFileA(partitionPath.HostDevicePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		// Force pretend has directory instead of binary partition.
		RegisterXboxObject(DiskDeviceObject, partitionPath.HostRootHandle);
		RegisterXboxObject<true>(DiskDeviceObject, partitionPath.HostBinHandle);
		return;
	}

	EmuLog(LOG_LEVEL::FATAL, "Failed to register host device (%s)\n", partitionPath.HostDevicePath.c_str());

	partitionPath.HostDevicePath = "";
	partitionPath.HostRootHandle = NULL;
}

static xbox::ntstatus_xt EmuBindDeviceNameToObjectType(xbox::STRING& xTargetName, xbox::OBJECT_TYPE& ObjectType)
{
	xbox::OBJECT_ATTRIBUTES objAttrs;
	X_InitializeObjectAttributes(&objAttrs, &xTargetName, OBJ_PERMANENT | OBJ_CASE_INSENSITIVE, xbox::zeroptr);

	PVOID TargetDirectoryObject;
	xbox::ntstatus_xt result = xbox::ObCreateObject(&ObjectType, &objAttrs, 0, &TargetDirectoryObject);
	EmuBugCheckInline(result);

	xbox::HANDLE xHandle;
	result = xbox::ObInsertObject(TargetDirectoryObject, &objAttrs, 0, &xHandle);
	EmuBugCheckInline(result);
	xbox::NtClose(xHandle);

	return result;
}

void EmuDiskSetup()
{
	xbox::RtlInitAnsiString(&xDeviceHarddisk0, DeviceHarddisk0.c_str());
	xbox::RtlInitAnsiString(&xPartitionPrefix, PartitionPrefix.c_str());
	std::transform(xPartitionPrefix.Buffer, xPartitionPrefix.Buffer + xPartitionPrefix.Length, xPartitionPrefix.Buffer, xbox::RtlLowerChar);

	xbox::ntstatus_xt result = EmuBindDeviceNameToObjectType(xDeviceHarddisk0, DiskDirectoryObjectType);
	EmuBugCheckInline(result);

	xbox::PDEVICE_OBJECT DiskDeviceObject;
	result = xbox::IoCreateDevice(&xbox::DiskDriverObject, sizeof(xbox::IDE_DISK_EXTENSION), nullptr, xbox::FILE_DEVICE_DISK2, FALSE, &DiskDeviceObject);
	EmuBugCheckInline(result);

	// NOTE: below are incomplete reverse engineered, more research is needed to understand the initialization process.
	DiskDeviceObject->Flags |= 0x45; // TODO: What flags are these? Doesn't seem to match with ReactOS's info: https://github.com/reactos/reactos/blob/999345a4feaae1e74533c96fa2cdfa1bce50ec5f/sdk/include/xdk/iotypes.h

	DiskDeviceObject->AlignmentRequirement = 1;

	DiskDeviceObject->SectorSize = 0x200;

	xbox::PIDE_DISK_EXTENSION DiskExtension = static_cast<xbox::PIDE_DISK_EXTENSION>(DiskDeviceObject->DeviceExtension);

	DiskExtension->DeviceObject = DiskDeviceObject;
	DiskExtension->PartitionInformation.PartitionLength.QuadPart = 0; // TODO: what size?
	DiskExtension->PartitionInformation.PartitionNumber = 0;

	DiskDeviceObject->Flags &= ~X_DO_DEVICE_INITIALIZING;

	NtDll::HANDLE HostRootHandle = CreateFileA(g_DiskBasePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	RegisterXboxObject(DiskDeviceObject, HostRootHandle);

	// Setup our partitions and create them if they haven't been done.
	// Partition 0 contains configuration data, and is accessed as a native file, instead as a folder.
	EmuDiskPartitionSetup(0, true);
	// Contain title's saved data and its patch/updated content.
	EmuDiskPartitionSetup(1);
	// Contain dashboard content.
	EmuDiskPartitionSetup(2);
	// The following partitions are for caching purposes - for now we allocate up to 7 (as xbmp needs that many)
	EmuDiskPartitionSetup(3);
	EmuDiskPartitionSetup(4);
	EmuDiskPartitionSetup(5);
	EmuDiskPartitionSetup(6);
	EmuDiskPartitionSetup(7);
}
