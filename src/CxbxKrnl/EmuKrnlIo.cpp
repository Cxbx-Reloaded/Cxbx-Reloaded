// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlIo.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For IoCompletionObjectType, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuFile.h" // For CxbxCreateSymbolicLink(), etc.

// ******************************************************************
// * 0x003B - IoAllocateIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(59) xboxkrnl::PVOID NTAPI xboxkrnl::IoAllocateIrp
(
	IN CHAR StackSize
)
{
	LOG_FUNC_ONE_ARG(StackSize);

	LOG_UNIMPLEMENTED();

	xboxkrnl::PVOID ret = nullptr;

	RETURN(ret);
}

// ******************************************************************
// * 0x003C - IoBuildAsynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(60) xboxkrnl::PVOID NTAPI xboxkrnl::IoBuildAsynchronousFsdRequest
(
	IN ULONG MajorFunction,
	PDEVICE_OBJECT	DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	ULONG Length OPTIONAL,
	PLARGE_INTEGER StartingOffset OPTIONAL,
	PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MajorFunction)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG_OUT(Length)
		LOG_FUNC_ARG(StartingOffset)
		LOG_FUNC_ARG(IoStatusBlock)
		LOG_FUNC_END;

	xboxkrnl::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003D - IoBuildDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(61) xboxkrnl::PVOID NTAPI xboxkrnl::IoBuildDeviceIoControlRequest
(
	IN ULONG IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength OPTIONAL,
	BOOLEAN InternalDeviceIoControl,
	PKEVENT Event,
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
		LOG_FUNC_ARG(IoStatusBlock)
		LOG_FUNC_END;

	xboxkrnl::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003E - IoBuildSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(62) xboxkrnl::PVOID NTAPI xboxkrnl::IoBuildSynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length OPTIONAL,
	PLARGE_INTEGER StartingOffset OPTIONAL,
	PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MajorFunction)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(StartingOffset)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;

	xboxkrnl::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x003F - IoCheckShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(63) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCheckShareAccess
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
XBSYSAPI EXPORTNUM(64) xboxkrnl::OBJECT_TYPE xboxkrnl::IoCompletionObjectType =
{
	/*
	ExAllocatePoolWithTag,
	ExFreePool,
	NULL,
	IopDeleteIoCompletion,
	NULL,
	*/
	NULL, // &ObpDefaultObject,
	'pmoC' // = first four characters of "Completion" in reverse
};

// ******************************************************************
// * 0x0041 - IoCreateDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(65) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCreateDevice
(
	IN  PDRIVER_OBJECT		DriverObject,
	IN  ULONG				DeviceExtensionSize,
	IN  PSTRING				DeviceName  OPTIONAL,
	IN  ULONG				DeviceType,
	// TODO? : IN ULONG		DeviceCharacteristics,
	IN  BOOLEAN				Exclusive,
	OUT PDEVICE_OBJECT*		DeviceObject
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DriverObject)
		LOG_FUNC_ARG(DeviceExtensionSize)
		LOG_FUNC_ARG(DeviceName)
		LOG_FUNC_ARG_OUT(DeviceType)
		LOG_FUNC_ARG(Exclusive)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0042 - IoCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(66) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCreateFile
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

	NTSTATUS ret = CxbxObjectAttributesToNT(ObjectAttributes, /*OUT*/nativeObjectAttributes, "IoCreateFile");

	if (!FAILED(ret))
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

	if (FAILED(ret))
	{
		DbgPrintf("EmuKrnl (0x%X): IoCreateFile Failed! (0x%.08X)\n", GetCurrentThreadId(), ret);
	}
	else
	{
		DbgPrintf("EmuKrnl (0x%X): IoCreateFile = 0x%.08X\n", GetCurrentThreadId(), *FileHandle);
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0043 - IoCreateSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(67) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoCreateSymbolicLink
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
XBSYSAPI EXPORTNUM(68) xboxkrnl::VOID NTAPI xboxkrnl::IoDeleteDevice
(
	IN PDEVICE_OBJECT irql
)
{
	LOG_FUNC_ONE_ARG(irql);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(69) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoDeleteSymbolicLink
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
XBSYSAPI EXPORTNUM(70) xboxkrnl::OBJECT_TYPE xboxkrnl::IoDeviceObjectType =
{
	/*
	ExAllocatePoolWithTag,
	ExFreePool,
	NULL,
	NULL,
	IoParseDevice,
	*/
	NULL, // &ObpDefaultObject,
	'iveD' // = first four characters of "Device" in reverse
};

// ******************************************************************
// * 0x0047 - IoFileObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(71) xboxkrnl::OBJECT_TYPE xboxkrnl::IoFileObjectType =
{
	/*
	ExAllocatePoolWithTag,
	ExFreePool,
	IopCloseFile,
	IopDeleteFile,
	IopParseFile,
	*/
	NULL, // (PVOID)FIELD_OFFSET(FILE_OBJECT, Event.Header),
	'eliF' // = "File" in reverse
};

// ******************************************************************
// * 0x0048 - IoFreeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(72) void NTAPI xboxkrnl::IoFreeIrp
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
XBSYSAPI EXPORTNUM(73) xboxkrnl::PVOID NTAPI xboxkrnl::IoInitializeIrp
(
	IN PIRP Irp,
	IN USHORT PacketSize,
	IN CHAR StackSize
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_ARG(PacketSize)
		LOG_FUNC_ARG(StackSize)
		LOG_FUNC_END;

	xboxkrnl::PVOID ret = nullptr;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x004A - IoInvalidDeviceRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(74) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoInvalidDeviceRequest
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
XBSYSAPI EXPORTNUM(75) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoQueryFileInformation
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
XBSYSAPI EXPORTNUM(76) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoQueryVolumeInformation
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
XBSYSAPI EXPORTNUM(77) xboxkrnl::VOID NTAPI xboxkrnl::IoQueueThreadIrp
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
XBSYSAPI EXPORTNUM(78) xboxkrnl::VOID NTAPI xboxkrnl::IoRemoveShareAccess
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

/* TODO : Finish of these Dxbx ported signatures :
// ******************************************************************
// * 0x004F - IoSetIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(79) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoSetIoCompletion
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0050 - IoSetShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(80) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoSetShareAccess
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0051 - IoStartNextPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(81) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoStartNextPacket
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0052 - IoStartNextPacketByKey()
// ******************************************************************
XBSYSAPI EXPORTNUM(82) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoStartNextPacketByKey
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0053 - IoStartPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(83) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoStartPacket
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0054 - IoSynchronousDeviceIoControlRequest()
// ******************************************************************
// IoSynchronousDeviceIoControlRequest:
// NICE.  Makes kernel driver stuff sooooo much easier.  This does a
// blocking IOCTL on the specified device.
//
// New to the XBOX.
XBSYSAPI EXPORTNUM(84) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoSynchronousDeviceIoControlRequest
(
	ULONG	IoControlCode,
	PDEVICE_OBJECT	DeviceObject,
	PVOID	InputBuffer	OPTIONAL,
	ULONG	InputBufferLength,
	PVOID	OutputBuffer	OPTIONAL,
	ULONG	OutputBufferLength,
	PDWORD	unknown_use_zero	OPTIONAL,
	BOOLEAN	InternalDeviceIoControl
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoControlCode)
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(InputBuffer)
		LOG_FUNC_ARG(InputBufferLength)
		LOG_FUNC_ARG(OutputBuffer)
		LOG_FUNC_ARG(OutputBufferLength)
		LOG_FUNC_ARG(unknown_use_zero)
		LOG_FUNC_ARG(InternalDeviceIoControl)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0055 - IoSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(85) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoSynchronousFsdRequest
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0056 - IofCallDriver()
// ******************************************************************
XBSYSAPI EXPORTNUM(86) xboxkrnl::NTSTATUS FASTCALL xboxkrnl::IofCallDriver
(
	PDEVICE_OBJECT	DeviceObject,
	PIRP	Irp
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
XBSYSAPI EXPORTNUM(87) xboxkrnl::NTSTATUS FASTCALL xboxkrnl::IofCompleteRequest
(
	PIRP	Irp
	CCHAR	PriorityBoost
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_ARG(PriorityBoost)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}
*/

// ******************************************************************
// * 0x005A - IoDismountVolume()
// ******************************************************************
XBSYSAPI EXPORTNUM(90) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoDismountVolume
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
XBSYSAPI EXPORTNUM(91) xboxkrnl::NTSTATUS NTAPI xboxkrnl::IoDismountVolumeByName
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

