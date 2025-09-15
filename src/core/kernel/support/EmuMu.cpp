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

#include "EmuMu.hpp"
#include "EmuFile.h"
#include "NativeHandle.h"
#include "Logging.h"
#include "common/FilePaths.hpp"

#include <array>
#include <filesystem>

namespace xbox {

	static void NTAPI MuStartIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
	}

	static ntstatus_xt NTAPI MuReadWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI MuDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static DRIVER_OBJECT MuDriverObject = {
		.DriverStartIo = MuStartIo,
		.DriverDeleteDevice = nullptr,
		.DriverUnknown2 = nullptr,
		.MajorFunction = {
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			MuReadWrite,
			MuReadWrite,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			MuDeviceControl,
			MuDeviceControl,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
		}
	};

}

static xbox::STRING xDeviceMU, xDeviceMuX, xDeviceMUPrefix;

struct PartitionDevice {
	xbox::PDEVICE_OBJECT DeviceObject;
	std::string HostDevicePath;
	HANDLE HostBinHandle;
	HANDLE HostRootHandle;
};

static std::array<PartitionDevice, 8> MuPartitionArray;

static_assert(MuPartitionArray.size() == 8, "Require partitions to be limited to 8");

static int XboxGetPartitionNumber(xbox::OBJECT_STRING& ElementName) {
	// Source has xbox::RtlPrefixString function which seems useful. Need to figure out if can do better or not.
	// convert path name to lowercase by requirement.
	std::string path_name(ElementName.Buffer, ElementName.Length);
	std::transform(path_name.begin(), path_name.end(), path_name.begin(), xbox::RtlLowerChar);
	std::string_view PartitionString = DeviceMUPrefix;
	const size_t pos = path_name.rfind(PartitionString);
	if (pos == std::string::npos) {
		return 0;
	}
	const std::string PartitionNumberString = path_name.substr(pos + PartitionString.length(), path_name.length() - PartitionString.length());

	// wcstol returns 0 on non-numeric characters, so we don't need to error check here
	return strtol(PartitionNumberString.data(), nullptr, 0);
}

// TODO: Get MuDriverObject supported from export kernel functions. (require irp work)
bool EmuMuFormatPartition(xbox::dword_xt PartitionNumber)
{
	const std::filesystem::path PartitionPath = MuPartitionArray[PartitionNumber].HostDevicePath;

	// Sanity check, make sure we are actually deleting something within the Cxbx-Reloaded folder
	if (!CxbxrIsPathInsideEmuMu(PartitionPath)) {
		EmuLog(LOG_LEVEL::WARNING, "Attempting to format a path that is not within a Cxbx-Reloaded data folder... Ignoring!\n");
		return false;
	}

	// Format the MU's partition, by iterating through the contents and removing all files/folders within
	// Previously, we deleted and re-created the folder, but that caused permission issues for some users
	std::error_code er;
	for (const auto& directoryEntry : std::filesystem::directory_iterator(PartitionPath, er)) {
		if (er) {
			EmuLog(LOG_LEVEL::ERROR2, "%s", er.message().c_str());
		}
		else {
			std::filesystem::remove_all(directoryEntry, er);
		}
	}

	printf("Formatted EmuMu Partition%d\n", PartitionNumber);
	return true;
}

static void EmuBugCheckInline(const xbox::ntstatus_xt result)
{
	if (!X_NT_SUCCESS(result)) {
		xbox::KeBugCheckEx(BCC_CXBXR_MU, reinterpret_cast<xbox::PVOID>(result), nullptr, nullptr, nullptr);
	}
}

static void EmuMuPartitionSetup(xbox::dword_xt MuIndex)
{
	// Make sure we do not extend over # of MUs.
	if (MuIndex >= MuPartitionArray.size()) {
		return;
	}

	char MuLetter = 'F' + static_cast<char>(MuIndex);
	PartitionDevice& MuPath = MuPartitionArray[MuIndex];
	MuPath.HostDevicePath = g_MuBasePath + MuLetter;

	bool succeeded{ true };
	std::filesystem::path partitionHeaderPath = MuPath.HostDevicePath + ".bin";
	if (!std::filesystem::exists(partitionHeaderPath)) {
		CxbxCreatePartitionHeaderFile(partitionHeaderPath, MuIndex == 0);
	}
	MuPath.HostBinHandle = CreateFileW(partitionHeaderPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	// If this path is not a raw file partition, create the directory for it
	std::error_code error; // We do not want filesystem to throw an exception on directory creation. Instead, listen for return value to fail.
	succeeded = std::filesystem::exists(MuPath.HostDevicePath) || std::filesystem::create_directory(MuPath.HostDevicePath, error);
	if (!succeeded) {
		return;
	}


	char MuX = '0' + static_cast<char>(MuIndex);
	std::string DeviceName = DeviceMUPrefix + MuX;
	xbox::STRING xDeviceName;
	xbox::RtlInitAnsiString(&xDeviceName, DeviceName.c_str());
	std::transform(xDeviceName.Buffer, xDeviceName.Buffer + xDeviceName.Length, xDeviceName.Buffer, xbox::RtlLowerChar);
	xbox::PDEVICE_OBJECT MuDeviceObject;
	xbox::ntstatus_xt result = xbox::IoCreateDevice(&xbox::MuDriverObject, sizeof(xbox::MU_EXTENSION), &xDeviceName, xbox::FILE_DEVICE_MEMORY_UNIT, FALSE, &MuDeviceObject);
	EmuBugCheckInline(result);

	// NOTE: below are incomplete reverse engineered, more research is needed to understand the initialization process.
	MuDeviceObject->Flags |= 0x45; // TODO: What flags are these? Doesn't seem to match with ReactOS's info: https://github.com/reactos/reactos/blob/999345a4feaae1e74533c96fa2cdfa1bce50ec5f/sdk/include/xdk/iotypes.h

	MuDeviceObject->AlignmentRequirement = 1;

	MuDeviceObject->SectorSize = 0x200;

	xbox::PMU_EXTENSION MuExtension = static_cast<xbox::PMU_EXTENSION>(MuDeviceObject->DeviceExtension);

	MuExtension->DeviceObject = MuDeviceObject;
	MuExtension->PartitionNumber = MuIndex;

	MuDeviceObject->Flags |= X_DO_DIRECT_IO;
	MuDeviceObject->Flags &= ~X_DO_DEVICE_INITIALIZING;

	MuPath.DeviceObject = MuDeviceObject;

	if (succeeded) {
		MuPath.HostRootHandle = CreateFileA(MuPath.HostDevicePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		// Force pretend has directory instead of binary partition.
		RegisterXboxObject(MuDeviceObject, MuPath.HostRootHandle);
		RegisterXboxObject<true>(MuDeviceObject, MuPath.HostBinHandle);
		return;
	}

	EmuLog(LOG_LEVEL::FATAL, "Failed to register host device (%s)\n", MuPath.HostDevicePath.c_str());

	MuPath.HostDevicePath = "";
	MuPath.HostRootHandle = NULL;
}

void EmuMuSetup()
{
	xbox::RtlInitAnsiString(&xDeviceMUPrefix, DeviceMUPrefix.c_str());
	std::transform(xDeviceMUPrefix.Buffer, xDeviceMUPrefix.Buffer + xDeviceMUPrefix.Length, xDeviceMUPrefix.Buffer, xbox::RtlLowerChar);

	// Create the MU directories and the bin files
	EmuMuPartitionSetup(0); // (F) Port #1 - Top Slot
	EmuMuPartitionSetup(1); // (G) Port #1 - Bottom Slot
	EmuMuPartitionSetup(2); // (H) Port #2 - Top Slot
	EmuMuPartitionSetup(3); // (I) Port #2 - Bottom Slot
	EmuMuPartitionSetup(4); // (J) Port #3 - Top Slot
	EmuMuPartitionSetup(5); // (K) Port #3 - Bottom Slot
	EmuMuPartitionSetup(6); // (L) Port #4 - Top Slot
	EmuMuPartitionSetup(7); // (M) Port #4 - Bottom Slot
}
