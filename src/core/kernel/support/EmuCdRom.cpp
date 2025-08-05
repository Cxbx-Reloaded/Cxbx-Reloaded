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

#include <core\kernel\exports\xboxkrnl.h>
#include "EmuCdRom.hpp"
#include "EmuShared.h"
#include "EmuFile.h"
#include "NativeHandle.h"
#include "Logging.h"

#include <array>
#include <filesystem>

namespace xbox {

	static void NTAPI CdRomStartIo(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
	}

	static ntstatus_xt NTAPI CdRomReadOnly(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI CdRomDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static ntstatus_xt NTAPI DriverIrpReturn(PDEVICE_OBJECT DeviceObject, PIRP Irp)
	{
		LOG_UNIMPLEMENTED();
		return X_STATUS_SUCCESS;
	}

	static DRIVER_OBJECT CdRomDriverObject = {
		.DriverStartIo = CdRomStartIo,
		.DriverDeleteDevice = nullptr,
		.DriverUnknown2 = nullptr,
		.MajorFunction = {
			DriverIrpReturn,
			DriverIrpReturn,
			CdRomReadOnly,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			CdRomDeviceControl,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
			IoInvalidDeviceRequest,
		}
	};
}

std::string g_TitleMountPath;

static xbox::STRING xDeviceCdRom0;
static xbox::STRING xDriveCdRom0;
static xbox::STRING xDriveD;
static xbox::STRING xDriveMbfs;

static void EmuBugCheckInline(const xbox::ntstatus_xt result)
{
	if (!X_NT_SUCCESS(result)) {
		xbox::KeBugCheckEx(BCC_CXBXR_CDROM0, reinterpret_cast<xbox::PVOID>(result), nullptr, nullptr, nullptr);
	}
}

void EmuCdRomSetup(std::filesystem::path CdRomPath, int BootFlags)
{
	xbox::RtlInitAnsiString(&xDeviceCdRom0, DeviceCdrom0.c_str());
	xbox::RtlInitAnsiString(&xDriveCdRom0, DriveCdRom0.c_str());
	xbox::RtlInitAnsiString(&xDriveD, DriveD.c_str());
	xbox::RtlInitAnsiString(&xDriveMbfs, DriveMbfs.c_str()); // Not part of CdRom0 device.

	xbox::PDEVICE_OBJECT CdRomDeviceObject;
	xbox::ntstatus_xt result = xbox::IoCreateDevice(&xbox::CdRomDriverObject, 0, &xDeviceCdRom0, xbox::FILE_DEVICE_CD_ROM2, FALSE, &CdRomDeviceObject);
	EmuBugCheckInline(result);


	NtDll::HANDLE HostRootHandle = CreateFileW(CdRomPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	RegisterXboxObject(CdRomDeviceObject, HostRootHandle);
	RegisterXboxObject<true>(CdRomDeviceObject, HostRootHandle);

	result = xbox::IoCreateSymbolicLink(&xDriveCdRom0, &xDeviceCdRom0);
	EmuBugCheckInline(result);

	// NOTE: below are incomplete reverse engineered, more research is needed to understand the initialization process.
	CdRomDeviceObject->Flags |= 0x44; // TODO: What flags are these? Doesn't seem to match with ReactOS's info: https://github.com/reactos/reactos/blob/999345a4feaae1e74533c96fa2cdfa1bce50ec5f/sdk/include/xdk/iotypes.h

	CdRomDeviceObject->AlignmentRequirement = 1;

	CdRomDeviceObject->SectorSize = 0x800;

	CdRomDeviceObject->Flags &= ~X_DO_DEVICE_INITIALIZING;


	// Check if title mounth path is already set. This may occur from early boot of Chihiro title.
	char title_mount_path[sizeof(szFilePath_Xbe)];
	g_EmuShared->GetTitleMountPath(title_mount_path);
	std::filesystem::path default_mount_path = title_mount_path;
	bool isEmuDisk = CxbxrIsPathInsideEmuDisk(CdRomPath);

	if (default_mount_path.native()[0] == 0 && BootFlags == BOOT_NONE) {
		// Remember our first initialize mount path for CdRom0 and Mbfs.
		if (!isEmuDisk) {
			g_EmuShared->SetTitleMountPath(CdRomPath.string().c_str());
			default_mount_path = CdRomPath.c_str();
		}
	}

	if (default_mount_path.native()[0] != 0) {
		// Since Chihiro also map Mbfs to the same path as Cdrom0, we'll map it the same way.
		if (g_bIsChihiro) {
			// TODO: Need to map Mbfs device from somewhere.
			result = xbox::IoCreateSymbolicLink(&xDriveMbfs, &xDeviceCdRom0);
			EmuBugCheckInline(result);
		}
		g_TitleMountPath = default_mount_path.string();
	}

	if (isEmuDisk) {
		result = xbox::IoCreateSymbolicLink(&xDriveD, &xDeviceCdRom0);
		EmuBugCheckInline(result);
	}
}
