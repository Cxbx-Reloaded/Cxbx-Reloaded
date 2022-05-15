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


#include <core\kernel\exports\xboxkrnl.h> // For IoCompletionObjectType, etc.
#include "EmuKrnlIo.hpp"
#include "EmuKrnlNt.hpp"
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\kernel\support\EmuFile.h" // For CxbxCreateSymbolicLink(), etc.
#include "core/kernel/support/NativeHandle.h" // For Xbox objects to native handle and back
#include "CxbxDebugger.h"

static void IopClearStackLocation(IN xbox::PIO_STACK_LOCATION IoStackLocation)
{
	IoStackLocation->MinorFunction = 0;
	IoStackLocation->Flags = 0;
	IoStackLocation->Control &= SL_ERROR_RETURNED;
	IoStackLocation->Parameters.Others.Argument1 = 0;
	IoStackLocation->Parameters.Others.Argument2 = 0;
	IoStackLocation->Parameters.Others.Argument3 = 0;
	IoStackLocation->Parameters.SetFile.FileObject = NULL;
}

static xbox::void_xt IopDeleteDevice
(
	IN xbox::PDEVICE_OBJECT DeviceObject,
	IN xbox::KIRQL OldIrql
)
{
	using namespace xbox;
	if (DeviceObject->DeletePending) {
		KfLowerIrql(OldIrql);
		PDRIVER_DELETEDEVICE DriverDeleteDevice = DeviceObject->DriverObject->DriverDeleteDevice;
		if (DriverDeleteDevice) {
			DriverDeleteDevice(DeviceObject);
		}
		else {
			ObfDereferenceObject(DeviceObject);
		}
	}
}

// ******************************************************************
// * 0x003B - IoAllocateIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(59) xbox::PVOID NTAPI xbox::IoAllocateIrp
(
	IN cchar_xt StackSize
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
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
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
	IN ulong_xt IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ulong_xt InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ulong_xt OutputBufferLength OPTIONAL,
	IN boolean_xt InternalDeviceIoControl,
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
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
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
// Source: ReactOS, excluded file object extension check.
XBSYSAPI EXPORTNUM(63) xbox::ntstatus_xt NTAPI xbox::IoCheckShareAccess
(
	IN access_mask_xt DesiredAccess,
	IN ulong_xt DesiredShareAccess,
	OUT PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess,
	IN boolean_xt Update
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(DesiredShareAccess)
		LOG_FUNC_ARG_OUT(FileObject)
		LOG_FUNC_ARG_OUT(ShareAccess)
		LOG_FUNC_ARG(Update)
		LOG_FUNC_END;

	//PAGED_CODE();

	/* Get access masks */
	boolean_xt ReadAccess = (DesiredAccess & (FILE_READ_DATA | FILE_EXECUTE)) != 0;
	boolean_xt WriteAccess = (DesiredAccess & (FILE_WRITE_DATA | FILE_APPEND_DATA)) != 0;
	boolean_xt DeleteAccess = (DesiredAccess & DELETE) != 0;

	/* Set them in the file object */
	FileObject->ReadAccess = ReadAccess;
	FileObject->WriteAccess = WriteAccess;
	FileObject->DeleteAccess = DeleteAccess;

	/* Check if we have any access */
	if ((ReadAccess) || (WriteAccess) || (DeleteAccess)) {
		/* Get shared access masks */
		boolean_xt SharedRead = (DesiredShareAccess & FILE_SHARE_READ) != 0;
		boolean_xt SharedWrite = (DesiredShareAccess & FILE_SHARE_WRITE) != 0;
		boolean_xt SharedDelete = (DesiredShareAccess & FILE_SHARE_DELETE) != 0;

		/* Check if the shared access is violated */
		if ((ReadAccess && (ShareAccess->SharedRead < ShareAccess->OpenCount)) ||
			(WriteAccess && (ShareAccess->SharedWrite < ShareAccess->OpenCount)) ||
			(DeleteAccess && (ShareAccess->SharedDelete < ShareAccess->OpenCount)) ||
			((ShareAccess->Readers != 0) && !SharedRead) ||
			((ShareAccess->Writers != 0) && !SharedWrite) ||
			((ShareAccess->Deleters != 0) && !SharedDelete)) {

			/* Sharing violation, fail */
			RETURN(X_STATUS_SHARING_VIOLATION);
		}

		/* Set them */
		FileObject->SharedRead = SharedRead;
		FileObject->SharedWrite = SharedWrite;
		FileObject->SharedDelete = SharedDelete;

		/* It's not, check if caller wants us to update it */
		if (Update) {
			/* Increase open count */
			ShareAccess->OpenCount++;

			/* Update shared access */
			ShareAccess->Readers += ReadAccess;
			ShareAccess->Writers += WriteAccess;
			ShareAccess->Deleters += DeleteAccess;
			ShareAccess->SharedRead += SharedRead;
			ShareAccess->SharedWrite += SharedWrite;
			ShareAccess->SharedDelete += SharedDelete;
		}
	}

	/* Validation successful */
	RETURN(X_STATUS_SUCCESS);
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
	&xbox::ObpDefaultObject,
	'pmoC' // = first four characters of "Completion" in reverse
};

// ******************************************************************
// * 0x0041 - IoCreateDevice()
// ******************************************************************
// Source: ReactOS, modified to fit in xbox compatibility layer
XBSYSAPI EXPORTNUM(65) xbox::ntstatus_xt NTAPI xbox::IoCreateDevice
(
	IN  PDRIVER_OBJECT      DriverObject,
	IN  ulong_xt            DeviceExtensionSize,
	IN  PSTRING             DeviceName OPTIONAL,
	IN  ulong_xt            DeviceType,
	IN  boolean_xt          Exclusive,
	OUT PDEVICE_OBJECT*     DeviceObject
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

	xbox::OBJECT_ATTRIBUTES ObjectAttributes;

	/* Initialize the Object Attributes */
	X_InitializeObjectAttributes(&ObjectAttributes, DeviceName, 0, zeroptr);

	/* Honor exclusive flag */
	if (Exclusive) {
		ObjectAttributes.Attributes |= OBJ_EXCLUSIVE;
	}

	/* Create a permanent object for named devices */
	bool DeviceHasName = DeviceName && DeviceName->Length;
	if (DeviceHasName) {
		ObjectAttributes.Attributes |= OBJ_PERMANENT;
	}

	/* Align the Extension Size to 8-bytes plus total size */
	ulong_xt TotalSize = RoundUp(sizeof(DEVICE_OBJECT) + DeviceExtensionSize, sizeof(longlong_xt));

	/* Create the Device Object */
	PDEVICE_OBJECT CreatedDeviceObject;
	*DeviceObject = nullptr;
	ntstatus_xt result = ObCreateObject(&IoDeviceObjectType, &ObjectAttributes, TotalSize, (PVOID*)&CreatedDeviceObject);

	if (!X_NT_SUCCESS(result)) {
		CreatedDeviceObject = nullptr;
	}
	else {
		/* Clear the whole Object and extension so we don't null stuff manually */
		std::memset(CreatedDeviceObject, 0, TotalSize);

		/* Setup the Type and Size. */
		CreatedDeviceObject->Type = IO_TYPE_DEVICE;
		CreatedDeviceObject->Size = sizeof(DEVICE_OBJECT) + DeviceExtensionSize;

		/* Set Device Object Data */
		CreatedDeviceObject->DeviceType = static_cast<uchar_xt>(DeviceType);
		CreatedDeviceObject->DeviceExtension = DeviceExtensionSize ? CreatedDeviceObject + 1 : nullptr;
		CreatedDeviceObject->StackSize = 1;
		CreatedDeviceObject->AlignmentRequirement = 0;

		/* Link the Extension to Object */
		if (CreatedDeviceObject->DeviceExtension) {
			PIDE_DISK_EXTENSION DeviceObjectExtension = reinterpret_cast<PIDE_DISK_EXTENSION>(CreatedDeviceObject->DeviceExtension);
			DeviceObjectExtension->DeviceObject = CreatedDeviceObject;
		}

		/* Set the Flags */
		CreatedDeviceObject->Flags = X_DO_DEVICE_INITIALIZING;
		if (Exclusive) {
			CreatedDeviceObject->Flags |= X_DO_EXCLUSIVE;
		}
		if (DeviceHasName) {
			CreatedDeviceObject->Flags |= X_DO_DEVICE_HAS_NAME;
		}

		/* Create the Device Lock */
		if (DeviceType == xbox::FILE_DEVICE_CD_ROM2 ||
			DeviceType == xbox::FILE_DEVICE_DISK2 ||
			DeviceType == xbox::FILE_DEVICE_MEDIA_BOARD ||
			DeviceType == xbox::FILE_DEVICE_MEMORY_UNIT) {
			/* Initialize Lock Event */
			KeInitializeEvent(&CreatedDeviceObject->DeviceLock, SynchronizationEvent, TRUE);

			CreatedDeviceObject->MountedOrSelfDevice = nullptr;
		}
		else {
			CreatedDeviceObject->MountedOrSelfDevice = CreatedDeviceObject;
		}

		KeInitializeDeviceQueue(&CreatedDeviceObject->DeviceQueue);

		/* Insert the Object */
		xbox::HANDLE handle;
		result = ObInsertObject(CreatedDeviceObject, &ObjectAttributes, 1, &handle);

		if (X_NT_SUCCESS(result)) {
			/* Close the temporary handle and return to caller */
			NtClose(handle);
			CreatedDeviceObject->DriverObject = DriverObject;
		}
		else {
			CreatedDeviceObject = nullptr;
		}
	}

	*DeviceObject = CreatedDeviceObject;
	return result;
}

// ******************************************************************
// * 0x0042 - IoCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(66) xbox::ntstatus_xt NTAPI xbox::IoCreateFile
(
	OUT PHANDLE             FileHandle,
	IN  access_mask_xt         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
	IN  ulong_xt               FileAttributes,
	IN  ulong_xt               ShareAccess,
	IN  ulong_xt               Disposition,
	IN  ulong_xt               CreateOptions,
	IN  ulong_xt               Options
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

	// TODO: This may need removal and use actual driver for Chihiro.
	//       Check if there's another branch has chihiro support somewhere? I think nope.
	// If we are emulating the Chihiro, we need to hook mbcom, so return an easily identifiable handle
	if (g_bIsChihiro) {
		if (strncmp(ObjectAttributes->ObjectName->Buffer, DriveMbcom.c_str(), DriveMbcom.length()) == 0) {
			*FileHandle = CHIHIRO_MBCOM_HANDLE;
			return X_STATUS_SUCCESS;
		}
	}

	if (Options & IO_CHECK_CREATE_PARAMETERS) {
		/* Validate parameters */
		if (FileAttributes & ~FILE_ATTRIBUTE_VALID_FLAGS)
		{
			//DPRINT1("File Create 'FileAttributes' Parameter contains invalid flags!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if (ShareAccess & ~FILE_SHARE_VALID_FLAGS)
		{
			//DPRINT1("File Create 'ShareAccess' Parameter contains invalid flags!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if (Disposition > FILE_MAXIMUM_DISPOSITION)
		{
			//DPRINT1("File Create 'Disposition' Parameter is out of range!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if (CreateOptions & ~FILE_VALID_OPTION_FLAGS)
		{
			//DPRINT1("File Create 'CreateOptions' parameter contains invalid flags!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & (FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT)) &&
			(!(DesiredAccess & SYNCHRONIZE)))
		{
			//DPRINT1("File Create 'CreateOptions' parameter FILE_SYNCHRONOUS_IO_* requested, but 'DesiredAccess' does not have SYNCHRONIZE!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & FILE_DELETE_ON_CLOSE) && (!(DesiredAccess & DELETE)))
		{
			//DPRINT1("File Create 'CreateOptions' parameter FILE_DELETE_ON_CLOSE requested, but 'DesiredAccess' does not have DELETE!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & (FILE_SYNCHRONOUS_IO_NONALERT | FILE_SYNCHRONOUS_IO_ALERT)) ==
			(FILE_SYNCHRONOUS_IO_NONALERT | FILE_SYNCHRONOUS_IO_ALERT))
		{
			//DPRINT1("File Create 'FileAttributes' parameter both FILE_SYNCHRONOUS_IO_NONALERT and FILE_SYNCHRONOUS_IO_ALERT specified!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & FILE_DIRECTORY_FILE) && !(CreateOptions & FILE_NON_DIRECTORY_FILE) &&
			(CreateOptions & ~(FILE_DIRECTORY_FILE |
				FILE_SYNCHRONOUS_IO_ALERT |
				FILE_SYNCHRONOUS_IO_NONALERT |
				FILE_WRITE_THROUGH |
				FILE_COMPLETE_IF_OPLOCKED |
				FILE_OPEN_FOR_BACKUP_INTENT |
				FILE_DELETE_ON_CLOSE |
				FILE_OPEN_FOR_FREE_SPACE_QUERY |
				FILE_OPEN_BY_FILE_ID |
				FILE_OPEN_REPARSE_POINT)))
		{
			//DPRINT1("File Create 'CreateOptions' Parameter has flags incompatible with FILE_DIRECTORY_FILE!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & FILE_COMPLETE_IF_OPLOCKED) && (CreateOptions & FILE_RESERVE_OPFILTER))
		{
			//DPRINT1("File Create 'CreateOptions' Parameter both FILE_COMPLETE_IF_OPLOCKED and FILE_RESERVE_OPFILTER specified!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}

		if ((CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING) && (DesiredAccess & FILE_APPEND_DATA))
		{
			//DPRINT1("File Create 'CreateOptions' parameter FILE_NO_INTERMEDIATE_BUFFERING requested, but 'DesiredAccess' FILE_APPEND_DATA requires it!\n");
			RETURN(X_STATUS_INVALID_PARAMETER);
		}
	}

	/* Check if we have allocation size */
	LARGE_INTEGER SafeAllocationSize;
	if (AllocationSize)
	{
		/* Capture it */
		SafeAllocationSize = *AllocationSize;
	}
	else
	{
		/* Otherwise, no size */
		SafeAllocationSize.QuadPart = 0;
	}

	/* Setup the Open Packet */
	xbox::OPEN_PACKET OpenPacket;
	std::memset(&OpenPacket, 0, sizeof(OPEN_PACKET));
	OpenPacket.Type = IO_TYPE_OPEN_PACKET;
	OpenPacket.Size = sizeof(xbox::OPEN_PACKET);
	//OpenPacket.FileObject = nullptr;
	//OpenPacket.FinalStatus = X_STATUS_SUCCESS;
	//OpenPacket.Information = ; // not set
	//OpenPacket.ParseCheck = ; // not set
	OpenPacket.AllocationSize = SafeAllocationSize;
	OpenPacket.CreateOptions = CreateOptions;
	OpenPacket.FileAttributes = static_cast<ushort_xt>(FileAttributes);
	OpenPacket.ShareAccess = static_cast<ushort_xt>(ShareAccess);
	OpenPacket.Options = Options;
	OpenPacket.Disposition = Disposition;
	// Following fields are not included from ReactOS
	OpenPacket.DesiredAccess = DesiredAccess;
	//OpenPacket.NetworkInformation; // not set
	//OpenPacket.LocalFileObject; // not set, internal use only
	//OpenPacket.QueryOnly = false;
	//OpenPacket.DeleteOnly = false;

	HANDLE LocalHandle;
	ntstatus_xt result = xbox::ObOpenObjectByName(ObjectAttributes, nullptr, &OpenPacket, &LocalHandle);

	/* Now check for Ob or Io failure */
	if (!X_NT_SUCCESS(result) || OpenPacket.ParseCheck == false) {
		/* Check if Ob thinks well went well */
		if (X_NT_SUCCESS(result)) {
			/*
			 * Tell it otherwise. Because we didn't use an ObjectType,
			 * it incorrectly returned us a handle to God knows what.
			 */
			xbox::NtClose(LocalHandle);
			RETURN(X_STATUS_OBJECT_TYPE_MISMATCH);
		}

		/* Now check the Io status */
		if (!X_NT_SUCCESS(OpenPacket.FinalStatus)) {

			/* Use this status instead of Ob's */
			result = OpenPacket.FinalStatus;

			if (X_NT_WARNING(result)) {
				/* In this case, we copy the I/O Status back */
				IoStatusBlock->Status = OpenPacket.FinalStatus;
				IoStatusBlock->Information = OpenPacket.Information;
			}
		}
		else if (OpenPacket.FileObject && OpenPacket.ParseCheck == false) {
			/* Clear the device object to invalidate the FO, and dereference */
			OpenPacket.FileObject->DeviceObject = nullptr;
			ObfDereferenceObject(OpenPacket.FileObject);
		}
	}
	else {
		/* We reached success and have a valid file handle */
		OpenPacket.FileObject->Flags |= FO_HANDLE_CREATED;
		assert(OpenPacket.FileObject->Type == IO_TYPE_FILE);

		/* Write back the handle and I/O Status */
		*FileHandle = LocalHandle;
		IoStatusBlock->Information = OpenPacket.Information;
		IoStatusBlock->Status = OpenPacket.FinalStatus;

		/* Get the Io status */
		result = OpenPacket.FinalStatus;
	}

	if (!OpenPacket.ParseCheck && OpenPacket.FileObject) {
		ObfDereferenceObject(OpenPacket.FileObject);
	}

	if (X_NT_SUCCESS(result)) {
		LOG_FUNC_BEGIN_ARG_RESULT
			LOG_FUNC_ARG_RESULT(FileHandle)
		LOG_FUNC_END_ARG_RESULT;
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0043 - IoCreateSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(67) xbox::ntstatus_xt NTAPI xbox::IoCreateSymbolicLink
(
	IN PSTRING SymbolicLinkName,
	IN PSTRING DeviceName
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SymbolicLinkName)
		LOG_FUNC_ARG(DeviceName)
		LOG_FUNC_END;

	/* Initialize the object attributes and create the link */
	HANDLE LinkHandle;
	OBJECT_ATTRIBUTES objectAttr;
	X_InitializeObjectAttributes(&objectAttr, SymbolicLinkName, OBJ_PERMANENT | OBJ_CASE_INSENSITIVE, zeroptr);
	ntstatus_xt result = NtCreateSymbolicLinkObject(&LinkHandle, &objectAttr, DeviceName);

	if (X_NT_SUCCESS(result)) {
		NtClose(LinkHandle);
	}

	RETURN(result);
}

// ******************************************************************
// * 0x0044 - IoDeleteDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(68) xbox::void_xt NTAPI xbox::IoDeleteDevice
(
	IN PDEVICE_OBJECT DeviceObject
)
{
	LOG_FUNC_ONE_ARG(DeviceObject);

	/* Check if the device has a name */
	if (DeviceObject->Flags & X_DO_DEVICE_HAS_NAME) {
		/* It does, make it temporary so we can remove it */
		ObMakeTemporaryObject(DeviceObject);
	}

	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

	/* Set the pending delete flag */
	DeviceObject->DeletePending = true;

	if (DeviceObject->ReferenceCount) {
		KfLowerIrql(OldIrql);
	}
	else {
		IopDeleteDevice(DeviceObject, OldIrql);
	}
}

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(69) xbox::ntstatus_xt NTAPI xbox::IoDeleteSymbolicLink
(
	IN PSTRING SymbolicLinkName
)
{
	LOG_FUNC_ONE_ARG(SymbolicLinkName);

	/* Initialize the object attributes and open the link */
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE Handle;
	X_InitializeObjectAttributes(&ObjectAttributes, SymbolicLinkName, OBJ_CASE_INSENSITIVE, zeroptr);
	ntstatus_xt result = NtOpenSymbolicLinkObject(&Handle, &ObjectAttributes);

	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	/* Make the link temporary and close its handle */
	result = NtMakeTemporaryObject(Handle);
	if (X_NT_SUCCESS(result)) {
		NtClose(Handle);
	}

	RETURN(result);
}

namespace xbox {

	// Do NOT use "FILE_ALL_ACCESS" mask for Windows' end, it doesn't serve any purpose for what we're doing.
	// And it cause return of "ACCESS DENIED" for attempt to open file, it may work fine for create file
	// but not open the file right after file creation.
	// Test case:
	//  - Metal Arms: Glitch in the System (on profile creation)
	GENERIC_MAPPING IopFileMapping = {
		.GenericRead = FILE_GENERIC_READ,
		.GenericWrite = FILE_GENERIC_WRITE,
		.GenericExecute = FILE_GENERIC_EXECUTE,
		// TODO: Restore back to "FILE_ALL_ACCESS" for GenericAll member variable after the kernel's driver is
		//       fully implemented.
		// NOTE: Using "FILE_ALL_ACCESS" mask on a host's filesystem for a user does not have full access rights
		//       cause this problem. If using AppData option is generally not a problem. Within executable
		//       directory may will be the problem, depending on host's file permission rights.
		// Testcase:
		// * Metal Arms: Glitch in the System
		.GenericAll = FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE
	};

	static ntstatus_xt NTAPI IopParseFile(
		IN PVOID ParseObject,
		IN struct _OBJECT_TYPE* ObjectType,
		IN ulong_xt Attributes,
		IN OUT POBJECT_STRING CompleteName,
		IN OUT POBJECT_STRING RemainingName,
		IN OUT PVOID Context OPTIONAL,
		OUT PVOID* Object
	)
	{
		xbox::POPEN_PACKET OpenPacket = reinterpret_cast<xbox::POPEN_PACKET>(Context);

		if (OpenPacket == nullptr || OpenPacket->Type != xbox::IO_TYPE_OPEN_PACKET || OpenPacket->Size != sizeof(xbox::OPEN_PACKET)) {
			return X_STATUS_OBJECT_TYPE_MISMATCH;
		}

		xbox::PFILE_OBJECT FileObject = reinterpret_cast<xbox::PFILE_OBJECT>(ParseObject);

		OpenPacket->RelatedFileObject = FileObject;

		return IopParseDevice(FileObject->DeviceObject, ObjectType, Attributes, CompleteName, RemainingName, Context, Object);
	}
}

// Source: ReactOS
bool IopValidateOpenPacket(const xbox::POPEN_PACKET OpenPacket) {
	/* Validate the packet */
	if (!(OpenPacket) ||
		(OpenPacket->Type != xbox::IO_TYPE_OPEN_PACKET) ||
		(OpenPacket->Size != sizeof(xbox::OPEN_PACKET)))
	{
		/* Fail */
		return false;
	}

	/* Good packet */
	return true;
}

// Source: ReactOS, modified to support xbox compatibility layer
xbox::ntstatus_xt IopCheckDeviceAndDriver(
	IN xbox::POPEN_PACKET OpenPacket,
	IN xbox::PDEVICE_OBJECT DeviceObject
)
{
	/* Make sure the object is valid */
	if (DeviceObject->DeletePending ||
		DeviceObject->Flags & X_DO_DEVICE_INITIALIZING) {
		/* It's unloading or initializing, so fail */
		/*DPRINT1("You are seeing this because the following ROS driver: %wZ\n"
			" sucks. Please fix it's AddDevice Routine\n",
			&DeviceObject->DriverObject->DriverName);*/
		return X_STATUS_NO_SUCH_DEVICE;
	}
	else if (DeviceObject->Flags & X_DO_EXCLUSIVE &&
		DeviceObject->ReferenceCount &&
		!OpenPacket->RelatedFileObject) {
		return X_STATUS_ACCESS_DENIED;
	}

	DeviceObject->ReferenceCount++;
	return X_STATUS_SUCCESS;
}

// Generic function to handle the workload. Used for HardDisk, CdRom, and MediaBoard, not sure what else use it.
// Source: ReactOS, modified to support xbox compatibility layer
xbox::ntstatus_xt NTAPI xbox::IopParseDevice(
	IN PVOID ParseObject,
	IN struct _OBJECT_TYPE* ObjectType,
	IN ulong_xt Attributes,
	IN OUT POBJECT_STRING CompleteName,
	IN OUT POBJECT_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	OUT PVOID* Object
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ParseObject)
		LOG_FUNC_ARG(ObjectType)
		LOG_FUNC_ARG(Attributes)
		LOG_FUNC_ARG(CompleteName)
		LOG_FUNC_ARG(RemainingName)
		LOG_FUNC_ARG(Context)
		LOG_FUNC_ARG_OUT(Object)
	LOG_FUNC_END;

	// It appear to be generic function with device interface go with it.
	// So for now it is acting like IopParseDevice simulated function.
	// May need to have its own function in order for other interface able use same tasks.

	/* Assume failure */
	*Object = nullptr;

	POPEN_PACKET OpenPacket = reinterpret_cast<POPEN_PACKET>(Context);

	/* If packet is used and doesn't have any remaining length, we return an existing parsed object */
	if (!OpenPacket && !RemainingName->Length) {
		xbox::ObfReferenceObject(ParseObject);
		*Object = ParseObject;
		return X_STATUS_SUCCESS;
	}

	/* Validate the open packet */
	if (!IopValidateOpenPacket(OpenPacket)) {
		return X_STATUS_OBJECT_TYPE_MISMATCH;
	}

	PDEVICE_OBJECT ParseDeviceObject = reinterpret_cast<PDEVICE_OBJECT>(ParseObject);

	if (OpenPacket->RelatedFileObject) {
		ParseDeviceObject = OpenPacket->RelatedFileObject->DeviceObject;
	}

	KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

	/* Validate device status */
	ntstatus_xt result = IopCheckDeviceAndDriver(OpenPacket, ParseDeviceObject);
	KfLowerIrql(OldIrql);
	if (!X_NT_SUCCESS(result)) {
		/* We failed, return status */
		OpenPacket->FinalStatus = result;
		RETURN(result);
	}

	// Perform mount volume, except in this case, we don't need to yet.

	ParseDeviceObject = nullptr;
	/* Map the generic mask */
	RtlMapGenericMask(&OpenPacket->DesiredAccess, &IopFileMapping);

	// relative irp stuff start here.

	PFILE_OBJECT FileObject;

	/* Check if we can simply use a dummy file */
	bool UseDummyFile = OpenPacket->QueryOnly || OpenPacket->DeleteOnly;

	bool DirectOpen = false;

	/* Check if this is a direct open */
	if (!(RemainingName->Length) &&
		!(OpenPacket->RelatedFileObject) &&
		((OpenPacket->DesiredAccess & ~(SYNCHRONIZE |
			FILE_READ_ATTRIBUTES |
			READ_CONTROL)) == 0) &&
		!(UseDummyFile))
	{
		/* Remember this for later */
		DirectOpen = true;
	}

	/* Check if we don't need to create an object */
	if (UseDummyFile) {
		PDUMMY_FILE_OBJECT LocalFileObject = OpenPacket->LocalFileObject;

		FileObject = reinterpret_cast<PFILE_OBJECT>(&LocalFileObject->ObjectHeader.Body);
		std::memset(FileObject, 0, sizeof(FILE_OBJECT));
		LocalFileObject->ObjectHeader.Type = &IoFileObjectType;
		LocalFileObject->ObjectHeader.PointerCount = 1;
	}
	else {
		/* Create the actual file object */
		OBJECT_ATTRIBUTES ObjAttrs;
		X_InitializeObjectAttributes(&ObjAttrs, zeroptr, Attributes, zeroptr);
		result = ObCreateObject(&IoFileObjectType, &ObjAttrs, sizeof(FILE_OBJECT), reinterpret_cast<PVOID*>(&FileObject));
		if (!X_NT_SUCCESS(result)) {
			OpenPacket->FinalStatus = result;
			RETURN(result);
		}

		/* Clear the file object */
		std::memset(FileObject, 0, sizeof(FILE_OBJECT));

		/* Check if this is Synch I/O */
		if (OpenPacket->CreateOptions &
			(FILE_SYNCHRONOUS_IO_ALERT | FILE_SYNCHRONOUS_IO_NONALERT)) {
			/* Set the synch flag */
			FileObject->Flags |= FO_SYNCHRONOUS_IO;

			/* Check if it's also alertable */
			if (OpenPacket->CreateOptions & FILE_SYNCHRONOUS_IO_ALERT) {
				/* It is, set the alertable flag */
				FileObject->Flags |= FO_ALERTABLE_IO;
			}
		}

		/* Check if this is synch I/O */
		if (FileObject->Flags & FO_SYNCHRONOUS_IO)
		{
			/* Initialize the event */
			KeInitializeEvent(&FileObject->Lock, SynchronizationEvent, FALSE);
		}

		/* Check if the caller requested no intermediate buffering */
		if (OpenPacket->CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING)
		{
			/* Set the correct flag for the FSD to read */
			FileObject->Flags |= FO_NO_INTERMEDIATE_BUFFERING;
		}

		/* Check if the caller says the file will be only read sequentially */
		if (OpenPacket->CreateOptions & FILE_SEQUENTIAL_ONLY)
		{
			/* Set the correct flag for the FSD to read */
			FileObject->Flags |= FO_SEQUENTIAL_ONLY;
		}

		/* Check if the caller believes the file will be only read randomly */
		if (OpenPacket->CreateOptions & FILE_RANDOM_ACCESS)
		{
			/* Set the correct flag for the FSD to read */
			FileObject->Flags |= FO_RANDOM_ACCESS;
		}

	}

	/* Setup the file header */
	FileObject->Type = IO_TYPE_FILE;
	// FILE_OBJECT does not have size field
	FileObject->RelatedFileObject = OpenPacket->RelatedFileObject;
	FileObject->DeviceObject = reinterpret_cast<PDEVICE_OBJECT>(ParseObject);

	/* Initialize the File Object event and set the FO */
	KeInitializeEvent(&FileObject->Event, NotificationEvent, false);
	OpenPacket->FileObject = FileObject;

	// Here we queue the IRP and call the driver, except we don't have IRP and driver functions implement
	// For temporary time being, below is emulation block at higher level emulation.
#if 1 // Cxbx-Reloaded emulation portion.

	PVOID RelativeParseObject;
	if (FileObject->RelatedFileObject) {
		RelativeParseObject = FileObject->RelatedFileObject;
	}
	else {
		RelativeParseObject = ParseObject;
	}
	NtDll::HANDLE RootDirectory;

	// Get parent directory handle instead of child handle.
	if (OpenPacket->Options & IO_OPEN_TARGET_DIRECTORY) {
		OpenPacket->CreateOptions |= FILE_DIRECTORY_FILE;
		auto ObjectName = PSTRING_to_string(RemainingName);
		std::size_t n = ObjectName.find_last_of("\\");
		if (n != std::string::npos) {
			ObjectName = ObjectName.substr(0, n);
			RemainingName->Length = static_cast<ushort_xt>(n);
		}
		else {
			RemainingName->Length = 0;
		}
	}
	// TODO: May need some accuracy correction but right now dashboard and titles are currently happy.
	// If there is remaining name left, we want to get non-partition handle.
	if (RemainingName->Length || OpenPacket->CreateOptions & FILE_DIRECTORY_FILE) {
		const auto& nHandle = GetObjectNativeHandle<false>(RelativeParseObject);
		RootDirectory = *nHandle;
	}
	// Otherwise, get partition handle or the latter.
	else {
		const auto& nHandle = GetObjectNativeHandle(RelativeParseObject);
		RootDirectory = *nHandle;
	}
	if (!RootDirectory) {
		// Then it must be from xbox's end which we don't have any support.
		// TODO: How to free object resource?
		EmuLog(LOG_LEVEL::ERROR2, "IopParseDevice attempt call GetObjectNativeHandle could not find any.");
		return X_STATUS_OBJECT_NAME_NOT_FOUND;
	}

	// We don't need slash, so let's go ahead remove it as Windows doesn't support it.
	if (RemainingName->Length && RemainingName->Buffer[0] == OBJ_NAME_PATH_SEPARATOR) {
		RemainingName->Buffer++;
		RemainingName->Length--;
		RemainingName->MaximumLength = RemainingName->Length;
	}

	// When a Synchronous CreateOption is specified, DesiredAccess must have SYNCHRONIZE set
	if ((OpenPacket->CreateOptions & FILE_SYNCHRONOUS_IO_NONALERT) != 0 ||
		(OpenPacket->CreateOptions & FILE_SYNCHRONOUS_IO_ALERT) != 0) {
		OpenPacket->DesiredAccess |= SYNCHRONIZE;
	}

	// TODO: Verify if this doesn't cause regression, plus move this into driver's functionality.
	// Titles can make assumptions about where files are located, but cxbxr can't for end users
	// Prevent issues with files stored on Windows Storage Spaces (and possibly other "exotic" places)
	// Test case: JSRF
	OpenPacket->CreateOptions &= (~FILE_NO_INTERMEDIATE_BUFFERING);

	// TODO: After move support down to irp driver / file system. Verify if we still need to force override.
	// Testcase:
	//  * Grand Theft Auto (series) - Always reformat partition, without this will not be able to reformat it.
	//  * Panzer Dragoon Orta - Copy files and seems like also rename files as well cause this problem.
	// Force ShareAccess to all
	OpenPacket->ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

	// Force sanction before call to NtDll::NtCreateFile
	// Testcase:
	//  * Exhibition Demo Disc - Attempt to create music folder fail internally which then will show unable to copy soundtrack dialog.
	OpenPacket->FileAttributes &= FILE_ATTRIBUTE_VALID_FLAGS;

	xbox::OBJECT_ATTRIBUTES ObjectAttributes = {
		.RootDirectory = RootDirectory,
		.ObjectName = RemainingName,
		.Attributes = OBJ_CASE_INSENSITIVE // ???
	};

	NativeObjectAttributes nativeObjectAttributes = {
		.NtObjAttrPtr = nullptr
	};

	std::wstring RelativeHostPath;
	if (RemainingName->Length) {
		auto ObjectName = PSTRING_to_string(RemainingName);
		RelativeHostPath = string_to_wstring(ObjectName);
		// sanitize remaining path name in order to use NtDll::NtCreateFile.
		// Testcase:
		// * Call of Duty 3
		std::wstring doubleslash_search = L"\\\\";
		std::wstring singleslash_str = L"\\";
		std::size_t n = 0;
		while ((n = RelativeHostPath.find(doubleslash_search, n)) != std::string::npos) {
			RelativeHostPath.replace(n, doubleslash_search.size(), singleslash_str);
			n += singleslash_str.size();
		}
	}

	NtDll::HANDLE FileHandle;
	// Copy the wide string to the unicode string
	wcscpy_s(nativeObjectAttributes.wszObjectName, RelativeHostPath.c_str());
	NtDll::RtlInitUnicodeString(&nativeObjectAttributes.NtUnicodeString, nativeObjectAttributes.wszObjectName);
	// And initialize the NT ObjectAttributes with that :
	InitializeObjectAttributes(&nativeObjectAttributes.NtObjAttr, &nativeObjectAttributes.NtUnicodeString, ObjectAttributes.Attributes, RootDirectory, NULL);
	// ObjectAttributes are given, so make sure the pointer we're going to pass to Windows is assigned :
	nativeObjectAttributes.NtObjAttrPtr = &nativeObjectAttributes.NtObjAttr;

	NtDll::IO_STATUS_BLOCK HostIoStatusBlock;
	// redirect to NtCreateFile
	result = NtDll::NtCreateFile(
		&FileHandle,
		OpenPacket->DesiredAccess | GENERIC_READ,
		nativeObjectAttributes.NtObjAttrPtr,
		&HostIoStatusBlock,
		NtDll::PLARGE_INTEGER(&OpenPacket->AllocationSize),
		OpenPacket->FileAttributes,
		OpenPacket->ShareAccess,
		OpenPacket->Disposition,
		OpenPacket->CreateOptions,
		NULL,
		0);
	OpenPacket->Information = reinterpret_cast<ulong_ptr_xt>(HostIoStatusBlock.Information);

	if (CxbxDebugger::CanReport()) {
		CxbxDebugger::ReportFileOpened(FileHandle, nativeObjectAttributes.NtUnicodeString.Buffer, SUCCEEDED(result));
	}

	if (X_NT_SUCCESS(result)) {

		// TODO: FileObject fields needs implement in driver's filesystem section. For now, ReadAccess/WriteAccess are not used.
		RegisterXboxObject(FileObject, FileHandle);
		if (RemainingName->Length) {
			AttachStringToXboxObject(FileObject, std::string(RemainingName->Buffer, RemainingName->Length));
		}
	}
	else {
		std::string xPathName(CompleteName->Buffer, CompleteName->Length);
		EmuLog(LOG_LEVEL::ERROR2, "Unable to access directory or file: %s", xPathName.c_str());
	}
#endif

	/* Make sure we are using a dummy */
	if (UseDummyFile) {
		/* Check if this was a query */
		if (OpenPacket->QueryOnly) {
			ulong_xt getLength;
			result = IoQueryFileInformation(FileObject,
			    FileNetworkOpenInformation,
			    sizeof(FILE_NETWORK_OPEN_INFORMATION),
			    OpenPacket->NetworkInformation,
			    &getLength);
		}

		// TODO: This call is not correct, should be IopDeleteFile
		RemoveXboxObject(FileObject);

		OpenPacket->ParseCheck = true;
		OpenPacket->FileObject = nullptr;
		OpenPacket->FinalStatus = result;

		RETURN(X_STATUS_OBJECT_NAME_NOT_FOUND);
	}

	//ObfReferenceObject(FileObject); // TODO: Find out what may be the cause for this reference not able to "delete" file and/or keep holding on.
	*Object = FileObject;
	OpenPacket->ParseCheck = true;
	OpenPacket->FinalStatus = result;

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT_TYPE(PFILE_OBJECT, Object)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
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
	xbox::IopParseDevice,
	&xbox::ObpDefaultObject,
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
	xbox::IopParseFile,
	(PVOID)offsetof(xbox::FILE_OBJECT, Event.Header),
	'eliF' // = "File" in reverse
};

// ******************************************************************
// * 0x0048 - IoFreeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(72) xbox::void_xt NTAPI xbox::IoFreeIrp
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
	IN ushort_xt PacketSize,
	IN cchar_xt StackSize
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
XBSYSAPI EXPORTNUM(74) xbox::ntstatus_xt NTAPI xbox::IoInvalidDeviceRequest
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DeviceObject)
		LOG_FUNC_ARG(Irp)
		LOG_FUNC_END;

	Irp->IoStatus.Status = X_STATUS_INVALID_DEVICE_REQUEST;
	IofCompleteRequest(Irp, 0/*=IO_NO_INCREMENT*/); // See ReactOS's value list at https://github.com/reactos/reactos/blob/master/sdk/include/xdk/iotypes.h

	RETURN(X_STATUS_INVALID_DEVICE_REQUEST);
}

// Source: ReactOS, modified to work in cxbxr environment for now.
xbox::ntstatus_xt IopQueryDeviceInformation
(
	IN xbox::PFILE_OBJECT FileObject,
	IN xbox::FILE_INFORMATION_CLASS FileInformationClass,
	IN xbox::ulong_xt Length,
	OUT xbox::PVOID FileInformation,
	OUT xbox::PULONG ReturnedLength,
	IN xbox::boolean_xt File
)
{
	using namespace xbox;

	/* Reference the object */
	ObfReferenceObject(FileObject);

	KEVENT Event;
	bool LocalEvent = false;

	/* Check if this is a file that was opened for Synch I/O */
	if (FileObject->Flags & FO_SYNCHRONOUS_IO) {
		/* Lock it */
		//(void)IopLockFileObject(FileObject); // Not implemented.

		/* Use File Object event */
		NtClearEvent(&FileObject->Event);
	}
	else {
		/* Use local event */
		KeInitializeEvent(&Event, SynchronizationEvent, false);
		LocalEvent = true;
	}

	// Which then lead to bunch of irp work. For now we just map to Windows' file system call.
	LOG_INCOMPLETE();
	ntstatus_xt result;
	if (const auto& nhandle = GetObjectNativeHandle(FileObject)) {

		NativeObjectAttributes nativeObjectAttributes;
		NtDll::FILE_NETWORK_OPEN_INFORMATION nativeNetOpenInfo;
		nativeObjectAttributes.wszObjectName[0] = NULL;
		NtDll::RtlInitUnicodeString(&nativeObjectAttributes.NtUnicodeString, nativeObjectAttributes.wszObjectName);
		// And initialize the NT ObjectAttributes with that :
		InitializeObjectAttributes(&nativeObjectAttributes.NtObjAttr, &nativeObjectAttributes.NtUnicodeString, 0x40/*Not tested and may not be accurate as we don't have Attributes passed down*/, *nhandle, NULL);
		nativeObjectAttributes.NtObjAttrPtr = &nativeObjectAttributes.NtObjAttr;

		result = NtDll::NtQueryFullAttributesFile(
			nativeObjectAttributes.NtObjAttrPtr,
			&nativeNetOpenInfo);

		// Convert Attributes to Xbox
		NTToXboxFileInformation(&nativeNetOpenInfo, FileInformation, FileNetworkOpenInformation, sizeof(xbox::FILE_NETWORK_OPEN_INFORMATION));

		if (FAILED(result)) {
			EmuLog(LOG_LEVEL::WARNING, "NtQueryFullAttributesFile failed! (0x%.08X)", result);
		}
	}
	else {
		result = X_STATUS_INVALID_PARAMETER;
	}

	return result;

}

// ******************************************************************
// * 0x004B - IoQueryFileInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(75) xbox::ntstatus_xt NTAPI xbox::IoQueryFileInformation
(
	IN PFILE_OBJECT FileObject,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN ulong_xt Length,
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

	/* Call the shared routine */
	RETURN(IopQueryDeviceInformation(FileObject, FileInformationClass, Length, FileInformation, ReturnedLength, true));
}

// ******************************************************************
// * 0x004C - IoQueryVolumeInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(76) xbox::ntstatus_xt NTAPI xbox::IoQueryVolumeInformation
(
	IN PFILE_OBJECT FileObject,
	IN FS_INFORMATION_CLASS FsInformationClass,
	IN ulong_xt Length,
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
XBSYSAPI EXPORTNUM(77) xbox::void_xt NTAPI xbox::IoQueueThreadIrp
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
// Source: ReactOS, excluded file object extension check.
XBSYSAPI EXPORTNUM(78) xbox::void_xt NTAPI xbox::IoRemoveShareAccess
(
	IN PFILE_OBJECT FileObject,
	IN PSHARE_ACCESS ShareAccess
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG(ShareAccess)
		LOG_FUNC_END;

	//PAGED_CODE();

	/* Otherwise, check if there's any access present */
	if ((FileObject->ReadAccess) ||
		(FileObject->WriteAccess) ||
		(FileObject->DeleteAccess))
	{
		/* Decrement the open count */
		ShareAccess->OpenCount--;

		/* Remove share access */
		ShareAccess->Readers -= FileObject->ReadAccess;
		ShareAccess->Writers -= FileObject->WriteAccess;
		ShareAccess->Deleters -= FileObject->DeleteAccess;
		ShareAccess->SharedRead -= FileObject->SharedRead;
		ShareAccess->SharedWrite -= FileObject->SharedWrite;
		ShareAccess->SharedDelete -= FileObject->SharedDelete;
	}
}

// ******************************************************************
// * 0x004F - IoSetIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(79) xbox::ntstatus_xt NTAPI xbox::IoSetIoCompletion
(
	IN PKQUEUE IoCompletion,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN ntstatus_xt IoStatus,
	IN ulong_xt IoStatusInformation
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
// Source: ReactOS, excluded file object extension check.
XBSYSAPI EXPORTNUM(80) xbox::void_xt NTAPI xbox::IoSetShareAccess
(
	IN access_mask_xt DesiredAccess,
	IN ulong_xt       DesiredShareAccess,
	IN PFILE_OBJECT   FileObject,
	OUT PSHARE_ACCESS ShareAccess
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(DesiredShareAccess)
		LOG_FUNC_ARG(FileObject)
		LOG_FUNC_ARG_OUT(ShareAccess)
		LOG_FUNC_END;

	boolean_xt ReadAccess;
	boolean_xt WriteAccess;
	boolean_xt DeleteAccess;
	boolean_xt SharedRead;
	boolean_xt SharedWrite;
	boolean_xt SharedDelete;
	//PAGED_CODE();

	ReadAccess = (DesiredAccess & (FILE_READ_DATA | FILE_EXECUTE)) != 0;
	WriteAccess = (DesiredAccess & (FILE_WRITE_DATA | FILE_APPEND_DATA)) != 0;
	DeleteAccess = (DesiredAccess & DELETE) != 0;

	/* Update basic access */
	FileObject->ReadAccess = ReadAccess;
	FileObject->WriteAccess = WriteAccess;
	FileObject->DeleteAccess = DeleteAccess;

	/* Check if we have no access as all */
	if (!(ReadAccess) && !(WriteAccess) && !(DeleteAccess)) {
		/* Otherwise, clear data */
		ShareAccess->OpenCount = 0;
		ShareAccess->Readers = 0;
		ShareAccess->Writers = 0;
		ShareAccess->Deleters = 0;
		ShareAccess->SharedRead = 0;
		ShareAccess->SharedWrite = 0;
		ShareAccess->SharedDelete = 0;
	}
	else {
		/* Calculate shared access */
		SharedRead = (DesiredShareAccess & FILE_SHARE_READ) != 0;
		SharedWrite = (DesiredShareAccess & FILE_SHARE_WRITE) != 0;
		SharedDelete = (DesiredShareAccess & FILE_SHARE_DELETE) != 0;

		/* Set it in the FO */
		FileObject->SharedRead = SharedRead;
		FileObject->SharedWrite = SharedWrite;
		FileObject->SharedDelete = SharedDelete;

		/* Otherwise, set data */
		ShareAccess->OpenCount = 1;
		ShareAccess->Readers = ReadAccess;
		ShareAccess->Writers = WriteAccess;
		ShareAccess->Deleters = DeleteAccess;
		ShareAccess->SharedRead = SharedRead;
		ShareAccess->SharedWrite = SharedWrite;
		ShareAccess->SharedDelete = SharedDelete;
	}
}

// ******************************************************************
// * 0x0051 - IoStartNextPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(81) xbox::void_xt NTAPI xbox::IoStartNextPacket
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
XBSYSAPI EXPORTNUM(82) xbox::void_xt NTAPI xbox::IoStartNextPacketByKey
(
	IN PDEVICE_OBJECT DeviceObject,
	IN ulong_xt Key
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
XBSYSAPI EXPORTNUM(83) xbox::void_xt NTAPI xbox::IoStartPacket
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
XBSYSAPI EXPORTNUM(84) xbox::ntstatus_xt NTAPI xbox::IoSynchronousDeviceIoControlRequest
(
	IN ulong_xt IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ulong_xt InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ulong_xt OutputBufferLength,
	OUT PULONG ReturnedOutputBufferLength OPTIONAL,
	IN boolean_xt InternalDeviceIoControl
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
XBSYSAPI EXPORTNUM(85) xbox::ntstatus_xt NTAPI xbox::IoSynchronousFsdRequest
(
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
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
XBSYSAPI EXPORTNUM(86) xbox::ntstatus_xt FASTCALL xbox::IofCallDriver
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
XBSYSAPI EXPORTNUM(87) xbox::void_xt FASTCALL xbox::IofCompleteRequest
(
	IN PIRP Irp,
	IN cchar_xt PriorityBoost
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
XBSYSAPI EXPORTNUM(90) xbox::ntstatus_xt NTAPI xbox::IoDismountVolume
(
	IN PDEVICE_OBJECT DeviceObject
)
{
	LOG_FUNC_ONE_ARG(DeviceObject);

	NTSTATUS ret = X_STATUS_SUCCESS;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x005B - IoDismountVolumeByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(91) xbox::ntstatus_xt NTAPI xbox::IoDismountVolumeByName
(
	IN PSTRING VolumeName
)
{
	LOG_FUNC_ONE_ARG(VolumeName);

	NTSTATUS ret = X_STATUS_SUCCESS;

	// TODO: Anything?
	LOG_UNIMPLEMENTED();

	RETURN(ret);
}

// ******************************************************************
// * 0x0167 - IoMarkIrpMustComplete()
// ******************************************************************
XBSYSAPI EXPORTNUM(359) xbox::cchar_xt NTAPI xbox::IoMarkIrpMustComplete
(
	IN PIRP Irp
)
{
	LOG_FUNC_ONE_ARG(Irp);

	xbox::cchar_xt ret = 0; // ShareAccess->OpenCount;

	LOG_UNIMPLEMENTED();

	RETURN(ret);
}
