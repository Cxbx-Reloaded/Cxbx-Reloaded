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

#include "EmuMediaBoard.hpp"
#include "EmuCdRom.hpp"
#include "EmuShared.h"
#include "EmuFile.h"
#include "FilePaths.hpp"
#include "NativeHandle.h"
#include "Logging.h"
#include "common/FilePaths.hpp"

#include <array>

namespace xbox {

	static void NTAPI MediaBoardStartIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
	}

	static ntstatus_xt NTAPI MediaBoardReadWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI MediaBoardDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI DriverIrpReturn(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static DRIVER_OBJECT MediaBoardDriverObject = {
		.DriverStartIo = MediaBoardStartIo,
		.DriverDeleteDevice = nullptr,
		.DriverUnknown2 = nullptr,
		.MajorFunction_IRP_MJ = {
			.CREATE = DriverIrpReturn,
			.CLOSE = DriverIrpReturn,
			.READ = MediaBoardReadWrite,
			.WRITE = MediaBoardReadWrite,
			.UNKNOWN4 = IoInvalidDeviceRequest,
			.UNKNOWN5 = IoInvalidDeviceRequest,
			.FLUSH_BUFFERS = IoInvalidDeviceRequest,
			.UNKNOWN7 = IoInvalidDeviceRequest,
			.UNKNOWN8 = IoInvalidDeviceRequest,
			.UNKNOWN9 = IoInvalidDeviceRequest,
			.DEVICE_CONTROL = MediaBoardDeviceControl,
			.INTERNAL_DEVICE_CONTROL = IoInvalidDeviceRequest,
			.UNKNOWN12 = IoInvalidDeviceRequest,
			.UNKNOWN13 = IoInvalidDeviceRequest,
		}
	};

}

static xbox::STRING xDeviceCdRom0;
static xbox::STRING xDeviceMediaBoard;
static xbox::STRING xDriveD;
static xbox::STRING xDriveMbfs;
static xbox::STRING xDriveMbcom;
static xbox::STRING xDriveMbrom0;
static xbox::STRING xDriveMbrom1;

struct PartitionDevice {
	xbox::PDEVICE_OBJECT DeviceObject;
	std::string HostDevicePath;
	HANDLE HostBinHandle;
	HANDLE HostRootHandle;
};

static std::array<PartitionDevice, 4> MediaBoardPartitionArray;

static_assert(MediaBoardPartitionArray.size() == 4, "Require partitions to be limited to 4");

static void EmuBugCheckInline(const xbox::ntstatus_xt result)
{
	if (!X_NT_SUCCESS(result)) {
		xbox::KeBugCheckEx(BCC_CXBXR_MEDIABOARD, reinterpret_cast<xbox::PVOID>(result), nullptr, nullptr, nullptr);
	}
}

static void EmuMediaBoardPartitionSetup(
	size_t partitionIndex,
	xbox::STRING& DosDeviceName,
	bool IsFile = false,
	std::filesystem::path* HostPath = nullptr
)
{
	// Make sure we do not extend over # of partitions.
	if (partitionIndex >= MediaBoardPartitionArray.size()) {
		EmuBugCheckInline(X_STATUS_INVALID_PARAMETER);
	}

	char partitionX = '0' + static_cast<char>(partitionIndex);
	PartitionDevice& partitionPath = MediaBoardPartitionArray[partitionIndex];
	if (HostPath) {
		partitionPath.HostDevicePath = HostPath->string();
	}
	else {
		partitionPath.HostDevicePath = g_MediaBoardBasePath + PartitionPrefix + partitionX;
	}
	std::string XboxDevicePartitionPath = DeviceMediaBoardPartitionPrefix + partitionX;
	xbox::STRING xXboxDevicePartitionPath;
	xbox::RtlInitAnsiString(&xXboxDevicePartitionPath, XboxDevicePartitionPath.c_str());

	bool succeeded{ true };
	std::string partitionHeaderPath = partitionPath.HostDevicePath + ".bin";
	if (!std::filesystem::exists(partitionHeaderPath)) {
		CxbxCreatePartitionHeaderFile(partitionHeaderPath, partitionIndex == 0);
	}
	partitionPath.HostBinHandle = CreateFileA(partitionHeaderPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	// If this path is not a raw file partition, create the directory for it
	if (!IsFile) {
		std::error_code error; // We do not want filesystem to throw an exception on directory creation. Instead, listen for return value to fail.
		succeeded = std::filesystem::exists(partitionPath.HostDevicePath) || std::filesystem::create_directory(partitionPath.HostDevicePath, error);
	}
	if (!succeeded) {
		return;
	}

	xbox::PDEVICE_OBJECT MediaBoardDeviceObject;
	xbox::ntstatus_xt result = xbox::IoCreateDevice(&xbox::MediaBoardDriverObject, sizeof(xbox::IDE_DISK_EXTENSION), &xXboxDevicePartitionPath, xbox::FILE_DEVICE_MEDIA_BOARD, FALSE, &MediaBoardDeviceObject);
	EmuBugCheckInline(result);

	result = xbox::IoCreateSymbolicLink(&DosDeviceName, &xXboxDevicePartitionPath);
	EmuBugCheckInline(result);

	if (partitionIndex == 0) {
		result = xbox::IoCreateSymbolicLink(&xDeviceCdRom0, &xXboxDevicePartitionPath);
		EmuBugCheckInline(result);
	}

	MediaBoardDeviceObject->Flags |= 0x45; // TODO: What flags are these? Doesn't seem to match with ReactOS's info: https://github.com/reactos/reactos/blob/999345a4feaae1e74533c96fa2cdfa1bce50ec5f/sdk/include/xdk/iotypes.h

	MediaBoardDeviceObject->AlignmentRequirement = 1;

	MediaBoardDeviceObject->SectorSize = 0x200;

	xbox::PIDE_DISK_EXTENSION DiskExtension = static_cast<xbox::PIDE_DISK_EXTENSION>(MediaBoardDeviceObject->DeviceExtension);

	DiskExtension->DeviceObject = MediaBoardDeviceObject;
	DiskExtension->PartitionInformation.PartitionLength.QuadPart = 0; // TODO: what size?
	DiskExtension->PartitionInformation.PartitionNumber = partitionIndex;

	MediaBoardDeviceObject->Flags &= ~X_DO_DEVICE_INITIALIZING;

	partitionPath.DeviceObject = MediaBoardDeviceObject;

	if (succeeded) {
		partitionPath.HostRootHandle = CreateFileA(partitionPath.HostDevicePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		// Force pretend has directory instead of binary partition.
		RegisterXboxObject(MediaBoardDeviceObject, partitionPath.HostRootHandle);
		RegisterXboxObject<true>(MediaBoardDeviceObject, partitionPath.HostBinHandle);
		return;
	}

	EmuLog(LOG_LEVEL::FATAL, "Failed to register host device (%s)\n", partitionPath.HostDevicePath.c_str());

	partitionPath.HostDevicePath = "";
	partitionPath.HostRootHandle = NULL;
}

void EmuMediaBoardSetup(std::filesystem::path CdRomPath, int BootFlags)
{
	xbox::RtlInitAnsiString(&xDeviceCdRom0, DeviceCdrom0.c_str());
	xbox::RtlInitAnsiString(&xDeviceMediaBoard, DeviceMediaBoard.c_str());
	xbox::RtlInitAnsiString(&xDriveMbfs, DriveMbfs.c_str());
	xbox::RtlInitAnsiString(&xDriveMbcom, DriveMbcom.c_str());
	xbox::RtlInitAnsiString(&xDriveMbrom0, DriveMbrom0.c_str());
	xbox::RtlInitAnsiString(&xDriveMbrom1, DriveMbrom1.c_str());
	xbox::RtlInitAnsiString(&xDriveD, DriveD.c_str());

	// Create directory object for MediaBoard device
	xbox::HANDLE xHandle;
	xbox::OBJECT_ATTRIBUTES objAttrs;
	X_InitializeObjectAttributes(&objAttrs, &xDeviceMediaBoard, OBJ_PERMANENT | OBJ_CASE_INSENSITIVE, xbox::zeroptr);
	xbox::ntstatus_xt result = xbox::NtCreateDirectoryObject(&xHandle, &objAttrs);
	EmuBugCheckInline(result);
	xbox::NtClose(xHandle);

	// Check if title mount path is already set. This may occur from early boot of Chihiro title.
	char title_mount_path[sizeof(szFilePath_Xbe)];
	g_EmuShared->GetTitleMountPath(title_mount_path);
	std::filesystem::path default_mount_path = title_mount_path;
	bool isEmuDisk = CxbxrIsPathInsideEmuDisk(CdRomPath);

	if (default_mount_path.native()[0] == 0 && BootFlags == BOOT_NONE) {
		// Remember our first initialize mount path for CdRom0
		if (!isEmuDisk) {
			g_EmuShared->SetTitleMountPath(CdRomPath.string().c_str());
			default_mount_path = CdRomPath.c_str();
		}
	}

	if (default_mount_path.native()[0] != 0) {
		g_TitleMountPath = default_mount_path.string();
	}

	EmuMediaBoardPartitionSetup(0, xDriveMbfs, false, &default_mount_path);
	EmuMediaBoardPartitionSetup(1, xDriveMbcom, true);
	EmuMediaBoardPartitionSetup(2, xDriveMbrom0, true);
	EmuMediaBoardPartitionSetup(3, xDriveMbrom1, true);

	// Since chihiro titles could not be in EmuMediaBoard folder, we need to always
	// mount D drive from the kernel's side.
	result = xbox::IoCreateSymbolicLink(&xDriveD, &xDeviceCdRom0);
	EmuBugCheckInline(result);
}
