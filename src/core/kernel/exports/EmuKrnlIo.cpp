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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::IO


#include <xboxkrnl/xboxkrnl.h> // For IoCompletionObjectType, etc.
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\kernel\support\EmuFile.h" // For CxbxCreateSymbolicLink(), etc.
#include "CxbxDebugger.h"

// ******************************************************************
// * 0x003B - IoAllocateIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(59) xbox::PVOID NTAPI xbox::IoAllocateIrp
(
	IN CCHAR StackSize
)
{
	LOG_FUNC_ONE_ARG(StackSize);

	LOG_UNIMPLEMENTED();

	xbox::PVOID ret = nullptr;

	RETURN(ret);
}

// ******************************************************************
// * 0x003C - IoBuildAsynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(60) xbox::PVOID NTAPI xbox::IoBuildAsynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MajorFunction)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(StartingOffset)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;

	xbox::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003D - IoBuildDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(61) xbox::PVOID NTAPI xbox::IoBuildDeviceIoControlRequest
(
	IN ULONG IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength OPTIONAL,
	IN BOOLEAN InternalDeviceIoControl,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoControlCode)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(InputBuffer)
		LOG_FUNC_ARG(InputBufferLength)
		LOG_FUNC_ARG_OUT(OutputBuffer)
		LOG_FUNC_ARG(OutputBufferLength)
		LOG_FUNC_ARG(InternalDeviceIoControl)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;

	xbox::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003E - IoBuildSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(62) xbox::PVOID NTAPI xbox::IoBuildSynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MajorFunction)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(StartingOffset)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;

	xbox::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003F - IoCheckShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(63) xbox::NTSTATUS NTAPI xbox::IoCheckShareAccess
(
	IN ACCESS_MASK DesiredAccess,
	IN ULONG DesiredShareAccess,
	OUT PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess,
	IN BOOLEAN Update
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(DesiredShareAccess)
		LOG_FUNC_ARG_OUT(FileObject)
		LOG_FUNC_ARG_OUT(ShareAccess)
		LOG_FUNC_ARG(Update)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0040 - IoCompletionObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(64) xbox::OBJECT_TYPE xbox::IoCompletionObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL,
	NULL, // TODO : xbox::IopDeleteIoCompletion,
	NULL,
	NULL, // &xbox::ObpDefaultObject,
	'pmoC' // = first four characters of "Completion" in reverse
};

// ******************************************************************
// * 0x0041 - IoCreateDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(65) xbox::NTSTATUS NTAPI xbox::IoCreateDevice
(
	IN  PDRIVER_OBJECT		DriverObject,
	IN  ULONG				DeviceExtensionSize,
	IN  PSTRING				DeviceName OPTIONAL,
	IN  ULONG				DeviceType,
	IN  BOOLEAN				Exclusive,
	OUT PDEVICE_OBJECT*		DeviceObject
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DriverObject)
		LOG_FUNC_ARG(DeviceExtensionSize)
		LOG_FUNC_ARG(DeviceName)
		LOG_FUNC_ARG(DeviceType)
		LOG_FUNC_ARG(Exclusive)
		LOG_FUNC_ARG_OUT(DeviceObject)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0042 - IoCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(66) xbox::NTSTATUS NTAPI xbox::IoCreateFile
(
	OUT PHANDLE             FileHandle,
	IN  ACCESS_MASK         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
	IN  ULONG               FileAttributes,
	IN  ULONG               ShareAccess,
	IN  ULONG               Disposition,
	IN  ULONG               CreateOptions,
	IN  ULONG               Options
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(FileHandle)
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(AllocationSize)
		LOG_FUNC_ARG(FileAttributes)
		LOG_FUNC_ARG(ShareAccess)
		LOG_FUNC_ARG_TYPE(CREATE_DISPOSITION, Disposition)
		LOG_FUNC_ARG_TYPE(CREATE_OPTION, CreateOptions)
		LOG_FUNC_ARG(Options)
		LOG_FUNC_END;

	NativeObjectAttributes nativeObjectAttributes;

	// If we are NOT accessing a directory, and we match a partition path, we need to redirect to a partition.bin file
	bool isDirectPartitionAccess = false;
	std::string objectName = std::string(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
	if ((CreateOptions & FILE_DIRECTORY_FILE) == 0 && _strnicmp(objectName.c_str(), DeviceHarddisk0PartitionPrefix.c_str(), DeviceHarddisk0PartitionPrefix.length()) == 0 && objectName.length() <= DeviceHarddisk0PartitionPrefix.length() + 2) {
		isDirectPartitionAccess = true;
	}

	NTSTATUS ret = CxbxObjectAttributesToNT(ObjectAttributes, /*OUT*/nativeObjectAttributes, "IoCreateFile", isDirectPartitionAccess);

	// When a Synchronous CreateOption is specified, DesiredAccess must have SYNCHRONIZE set
	if ((CreateOptions & FILE_SYNCHRONOUS_IO_NONALERT) != 0 ||
		(CreateOptions & FILE_SYNCHRONOUS_IO_ALERT) != 0) {
		DesiredAccess |= SYNCHRONIZE;
	}

	// Force ShareAccess to all 
	ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    if (SUCCEEDED(ret))
    {
        // redirect to NtCreateFile
        ret = NtDll::NtCreateFile(
            FileHandle,
            DesiredAccess | GENERIC_READ,
            nativeObjectAttributes.NtObjAttrPtr,
            NtDll::PIO_STATUS_BLOCK(IoStatusBlock),
            NtDll::PLARGE_INTEGER(AllocationSize),
            FileAttributes,
            ShareAccess,
            Disposition,
            CreateOptions,
            NULL,
            0);
        
        if (CxbxDebugger::CanReport())
        {
            CxbxDebugger::ReportFileOpened(*FileHandle, nativeObjectAttributes.NtUnicodeString.Buffer, SUCCEEDED(ret));
        }
    }

	if (FAILED(ret))
	{
		EmuLog(LOG_LEVEL::WARNING, "IoCreateFile Failed! (%s)\n", NtStatusToString(ret));
		*FileHandle = INVALID_HANDLE_VALUE;
	}
	else
	{
		EmuLog(LOG_LEVEL::DEBUG, "IoCreateFile = 0x%.8X", *FileHandle);
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0043 - IoCreateSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(67) xbox::NTSTATUS NTAPI xbox::IoCreateSymbolicLink
(
	IN PSTRING SymbolicLinkName,
	IN PSTRING DeviceName
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SymbolicLinkName)
		LOG_FUNC_ARG(DeviceName)
		LOG_FUNC_END;

	NTSTATUS ret = CxbxCreateSymbolicLink(PSTRING_to_string(SymbolicLinkName), PSTRING_to_string(DeviceName));

	RETURN(ret);
}

// ******************************************************************
// * 0x0044 - IoDeleteDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(68) xbox::VOID NTAPI xbox::IoDeleteDevice
(
	IN PDEVICE_OBJECT Irql
)
{
	LOG_FUNC_ONE_ARG(Irql);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(69) xbox::NTSTATUS NTAPI xbox::IoDeleteSymbolicLink
(
	IN PSTRING SymbolicLinkName
)
{
	LOG_FUNC_ONE_ARG(SymbolicLinkName);

	EmuNtSymbolicLinkObject* symbolicLink = FindNtSymbolicLinkObjectByName(PSTRING_to_string(SymbolicLinkName));

	NTSTATUS ret = STATUS_OBJECT_NAME_NOT_FOUND;

	if (symbolicLink != NULL)
		// Destroy the object once all handles to it are closed too :
		ret = symbolicLink->NtClose();

	RETURN(ret);
}

// ******************************************************************
// * 0x0046 - IoDeviceObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(70) xbox::OBJECT_TYPE xbox::IoDeviceObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL,
	NULL,
	NULL, // TODO : xbox::IoParseDevice,
	NULL, // TODO : &xbox::ObpDefaultObject,
	'iveD' // = first four characters of "Device" in reverse
};

// ******************************************************************
// * 0x0047 - IoFileObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(71) xbox::OBJECT_TYPE xbox::IoFileObjectType =
{
	xbox::ExAllocatePoolWithTag,
	xbox::ExFreePool,
	NULL, // TODO : xbox::IopCloseFile,
	NULL, // TODO : xbox::IopDeleteFile,
	NULL, // TODO : xbox::IopParseFile,
	(PVOID)offsetof(xbox::FILE_OBJECT, Event.Header),
	'eliF' // = "File" in reverse
};

// ******************************************************************
// * 0x0048 - IoFreeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(72) xbox::VOID NTAPI xbox::IoFreeIrp
(
	IN PIRP Irp
)
{
	LOG_FUNC_ONE_ARG(Irp);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0049 - IoInitializeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(73) xbox::PVOID NTAPI xbox::IoInitializeIrp
(
	IN PIRP Irp,
	IN USHORT PacketSize,
	IN CCHAR StackSize
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_ARG(PacketSize)
		LOG_FUNC_ARG(StackSize)
		LOG_FUNC_END;

	xbox::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x004A - IoInvalidDeviceRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(74) xbox::NTSTATUS NTAPI xbox::IoInvalidDeviceRequest
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x004B - IoQueryFileInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(75) xbox::NTSTATUS NTAPI xbox::IoQueryFileInformation
(
	IN PFILE_OBJECT FileObject,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN ULONG Length,
	OUT PVOID FileInformation,
	OUT PULONG ReturnedLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_ARG_OUT(ReturnedLength)
		LOG_FUNC_END;

	// Dxbx note : This is almost identical to NtQueryInformationFile
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x004C - IoQueryVolumeInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(76) xbox::NTSTATUS NTAPI xbox::IoQueryVolumeInformation
(
	IN PFILE_OBJECT FileObject,
	IN FS_INFORMATION_CLASS FsInformationClass,
	IN ULONG Length,
	OUT PVOID FsInformation,
	OUT PULONG ReturnedLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG(FsInformationClass)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG_OUT(FsInformation)
		LOG_FUNC_ARG_OUT(ReturnedLength)
		LOG_FUNC_END;

	// Dxbx note : This is almost identical to NtQueryVolumeInformationFile
	// DxbxPC2XB_FS_INFORMATION
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x004D - IoQueueThreadIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(77) xbox::VOID NTAPI xbox::IoQueueThreadIrp
(
	IN PIRP Irp
)
{
	LOG_FUNC_ONE_ARG(Irp);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x004E - IoRemoveShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(78) xbox::VOID NTAPI xbox::IoRemoveShareAccess
(
	IN PFILE_OBJECT FileObject,
	IN PSHARE_ACCESS ShareAccess
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG(ShareAccess)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x004F - IoSetIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(79) xbox::NTSTATUS NTAPI xbox::IoSetIoCompletion
(
	IN PKQUEUE IoCompletion,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN NTSTATUS IoStatus,
	IN ULONG IoStatusInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoCompletion)
		LOG_FUNC_ARG(KeyContext)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG(IoStatus)
		LOG_FUNC_ARG(IoStatusInformation)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0050 - IoSetShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(80) xbox::CCHAR NTAPI xbox::IoSetShareAccess
(
	IN ULONG DesiredAccess,
	IN ULONG DesiredShareAccess,
	IN PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(DesiredShareAccess)
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG_OUT(ShareAccess)
		LOG_FUNC_END;

	xbox::CCHAR ret = 0; // ShareAccess->OpenCount;

	LOG_UNIMPLEMENTED();
	
	RETURN(ret);
}

// ******************************************************************
// * 0x0051 - IoStartNextPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(81) xbox::VOID NTAPI xbox::IoStartNextPacket
(
	IN PDEVICE_OBJECT DeviceObject
)
{
	LOG_FUNC_ONE_ARG(DeviceObject);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0052 - IoStartNextPacketByKey()
// ******************************************************************
XBSYSAPI EXPORTNUM(82) xbox::VOID NTAPI xbox::IoStartNextPacketByKey
(
	IN PDEVICE_OBJECT DeviceObject,
	IN ULONG Key
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(Key)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0053 - IoStartPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(83) xbox::VOID NTAPI xbox::IoStartPacket
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	OUT PULONG Key OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_ARG_OUT(Key)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0054 - IoSynchronousDeviceIoControlRequest()
// ******************************************************************
// IoSynchronousDeviceIoControlRequest:
// NICE.  Makes kernel driver stuff sooooo much easier.  This does a
// blocking IOCTL on the specified device.
//
// New to the XBOX.
XBSYSAPI EXPORTNUM(84) xbox::NTSTATUS NTAPI xbox::IoSynchronousDeviceIoControlRequest
(
	IN ULONG IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnedOutputBufferLength OPTIONAL,
	IN BOOLEAN InternalDeviceIoControl
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoControlCode)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(InputBuffer)
		LOG_FUNC_ARG(InputBufferLength)
		LOG_FUNC_ARG_OUT(OutputBuffer)
		LOG_FUNC_ARG(OutputBufferLength)
		LOG_FUNC_ARG_OUT(ReturnedOutputBufferLength)
		LOG_FUNC_ARG(InternalDeviceIoControl)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0055 - IoSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(85) xbox::NTSTATUS NTAPI xbox::IoSynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	IN PLARGE_INTEGER StartingOffset OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MajorFunction)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(StartingOffset)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0056 - IofCallDriver()
// ******************************************************************
XBSYSAPI EXPORTNUM(86) xbox::NTSTATUS FASTCALL xbox::IofCallDriver
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0057 - IofCompleteRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(87) xbox::VOID FASTCALL xbox::IofCompleteRequest
(
	IN PIRP Irp,
	IN CCHAR PriorityBoost
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_ARG(PriorityBoost)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x005A - IoDismountVolume()
// ******************************************************************
XBSYSAPI EXPORTNUM(90) xbox::NTSTATUS NTAPI xbox::IoDismountVolume
(
	IN PDEVICE_OBJECT DeviceObject
)
{
	LOG_FUNC_ONE_ARG(DeviceObject);

	NTSTATUS ret = STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x005B - IoDismountVolumeByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(91) xbox::NTSTATUS NTAPI xbox::IoDismountVolumeByName
(
	IN PSTRING VolumeName
)
{
	LOG_FUNC_ONE_ARG(VolumeName);

	NTSTATUS ret = STATUS_SUCCESS;

	// TODO: Anything?
	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0167 - IoMarkIrpMustComplete()
// ******************************************************************
XBSYSAPI EXPORTNUM(359) xbox::CCHAR NTAPI xbox::IoMarkIrpMustComplete
(
	IN PIRP Irp
)
{
	LOG_FUNC_ONE_ARG(Irp);

	xbox::CCHAR ret = 0; // ShareAccess->OpenCount;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}
