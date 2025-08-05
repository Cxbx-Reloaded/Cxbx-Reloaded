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

#define LOG_PREFIX CXBXR_MODULE::NT


#include <core\kernel\exports\xboxkrnl.h> // For NtAllocateVirtualMemory, etc.
#include "EmuKrnlNt.hpp"
#include "EmuKrnl.h"
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "core\kernel\support\EmuNtDll.h"
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxrAbort
#include "core\kernel\exports\EmuKrnlKe.h"
#include "EmuKrnlKi.h"
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\kernel\support\EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.
#include "core/kernel/support/NativeHandle.h" // For Xbox objects to native handle and back
#include "core\kernel\memory-manager\VMManager.h" // For g_VMManager
#include "core\kernel\support\NativeHandle.h"
#include "devices\Xbox.h"
#include "CxbxDebugger.h"

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include <assert.h>

#include <unordered_map>
#include <mutex>

// Prevent setting the system time from multiple threads at the same time
xbox::RTL_CRITICAL_SECTION xbox::NtSystemTimeCritSec;

// Source: ReactOS, modified for xbox compatibility layer
xbox::ntstatus_xt xbox::NtMakeTemporaryObject(
	IN HANDLE Handle
)
{
	/* Reference the object */
	PVOID Object;
	ntstatus_xt result = ObReferenceObjectByHandle(Handle, nullptr, &Object);
	if (!X_NT_SUCCESS(result)) {
		return result;
	}

	/* Set it as temporary and dereference it */
	ObMakeTemporaryObject(Object);
	ObfDereferenceObject(Object);

	return X_STATUS_SUCCESS;
}

xbox::ntstatus_xt xbox::NtCreateSymbolicLinkObject(
	OUT PHANDLE LinkHandle,
	IN OUT POBJECT_ATTRIBUTES ObjectAttributes,
	IN PSTRING LinkTarget)
{
	/* Xbox kernel does a bit different method to find LinkTarget */
	PVOID LinkTargetObject;
	ntstatus_xt result = ObpResolveLinkTarget(LinkTarget, &LinkTargetObject);
	if (X_NT_SUCCESS(result)) {

		/* Create the object */
		POBJECT_SYMBOLIC_LINK SymLinkObject;
		result = ObCreateObject(&ObSymbolicLinkObjectType, ObjectAttributes, sizeof(OBJECT_SYMBOLIC_LINK) + LinkTarget->Length + 1, reinterpret_cast<PVOID*>(&SymLinkObject));
		if (!X_NT_SUCCESS(result)) {
			ObfDereferenceObject(LinkTargetObject);
		}
		else {
			/* Initialize the remaining name, dos drive index and target object */
			PCHAR LinkTargetBuffer = reinterpret_cast<PCHAR>(SymLinkObject + 1);
			std::memcpy(LinkTargetBuffer, LinkTarget->Buffer, LinkTarget->Length);
			LinkTargetBuffer[LinkTarget->Length] = NULL;
			SymLinkObject->LinkTargetObject = LinkTargetObject;
			SymLinkObject->LinkTarget.Buffer = LinkTargetBuffer;
			SymLinkObject->LinkTarget.Length = LinkTarget->Length;
			SymLinkObject->LinkTarget.MaximumLength = LinkTarget->Length + 1;

			/* Insert it into the object tree */
			result = ObInsertObject(SymLinkObject, ObjectAttributes, 0, LinkHandle);
		}
	}

	return result;
}

// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(184) xbox::ntstatus_xt NTAPI xbox::NtAllocateVirtualMemory
(
	IN OUT PVOID    *BaseAddress,
	IN ulong_xt         ZeroBits,
	IN OUT PULONG    AllocationSize,
	IN dword_xt         AllocationType,
	IN dword_xt         Protect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(PULONG, BaseAddress)
		LOG_FUNC_ARG(ZeroBits)
		LOG_FUNC_ARG(AllocationSize)
		LOG_FUNC_ARG_TYPE(ALLOCATION_TYPE, AllocationType)
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, Protect)
	LOG_FUNC_END;

	NTSTATUS ret = g_VMManager.XbAllocateVirtualMemory((VAddr*)BaseAddress, ZeroBits, (size_t*)AllocationSize,
		AllocationType, Protect);

	RETURN(ret);
}

// ******************************************************************
// * 0x00B9 - NtCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(185) xbox::ntstatus_xt NTAPI xbox::NtCancelTimer
(
	IN HANDLE TimerHandle,
	OUT PBOOLEAN CurrentState OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TimerHandle)
		LOG_FUNC_ARG(CurrentState)
		LOG_FUNC_END;

	// redirect to Windows NT
	// TODO : Untested
	NTSTATUS ret = NtDll::NtCancelTimer(
		TimerHandle,
		/*OUT*/CurrentState);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtCancelTimer failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00BA - NtClearEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(186) xbox::ntstatus_xt NTAPI xbox::NtClearEvent
(
	IN HANDLE EventHandle
)
{
	LOG_FUNC_ONE_ARG(EventHandle);

	NTSTATUS ret = NtDll::NtClearEvent(EventHandle);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtClearEvent Failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00BB - NtClose()
// ******************************************************************
XBSYSAPI EXPORTNUM(187) xbox::ntstatus_xt NTAPI xbox::NtClose
(
	IN HANDLE Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	ntstatus_xt result = X_STATUS_SUCCESS;

	if (CxbxDebugger::CanReport())
	{
		CxbxDebugger::ReportFileClosed(Handle);
	}

	PVOID Object;
	ntstatus_xt hresult = ObReferenceObjectByHandle(Handle, zeroptr, &Object);
	if (X_NT_SUCCESS(hresult)) {
		// This was a handle created by Ob
		ObfDereferenceObject(Object);
		result = ObpClose(Handle);
	}
	// Otherwise, it could be native handle
	// What has not been managed by Ob:
	// * Mutant
	// * Semaphore
	// * Fiber(?)
	// * Event
	// * Timer
	// * What else?
	// TODO: Remove "else if" statement once all items from list above is done.
	else if (DWORD flags = 0; GetHandleInformation(Handle, &flags)) {
		result = NtDll::NtClose(Handle);
	}

	RETURN(result);
}

// ******************************************************************
// * 0x00BC - NtCreateDirectoryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(188) xbox::ntstatus_xt NTAPI xbox::NtCreateDirectoryObject
(
	OUT PHANDLE             DirectoryHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DirectoryHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_END;

	POBJECT_DIRECTORY directoryObject;
	ntstatus_xt status = ObCreateObject(&ObDirectoryObjectType, ObjectAttributes, sizeof(OBJECT_DIRECTORY), reinterpret_cast<PVOID *>(&directoryObject));

	if (X_NT_SUCCESS(status)) {
		std::memset(directoryObject, 0, sizeof(OBJECT_DIRECTORY));
		status = ObInsertObject(directoryObject, ObjectAttributes, 0, DirectoryHandle);
	}

	RETURN(status);
}

// ******************************************************************
// * 0x00BD - NtCreateEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(189) xbox::ntstatus_xt NTAPI xbox::NtCreateEvent
(
	OUT PHANDLE             EventHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
	IN  EVENT_TYPE          EventType,
	IN  boolean_xt             InitialState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(EventHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(EventType)
		LOG_FUNC_ARG(InitialState)
		LOG_FUNC_END;

	ntstatus_xt result;
#ifdef CXBX_KERNEL_REWORK_ENABLED
	if ((EventType != NotificationEvent) && (EventType != SynchronizationEvent)) {
		result = STATUS_INVALID_PARAMETER;
	}
	else {
		PKEVENT Event;

		result = ObCreateObject(&ExEventObjectType, ObjectAttributes, sizeof(KEVENT), (PVOID *)&Event);
		if (X_NT_SUCCESS(result)) {
			KeInitializeEvent(Event, EventType, InitialState);
			result = ObInsertObject(Event, ObjectAttributes, 0, EventHandle);
		}
	}
#else

	LOG_INCOMPLETE(); // TODO : Verify arguments, use ObCreateObject, KeInitializeEvent and ObInsertObject instead of this:

	// initialize object attributes
	NativeObjectAttributes nativeObjectAttributes;
	CxbxObjectAttributesToNT(ObjectAttributes, /*var*/nativeObjectAttributes);

	// TODO : Is this the correct ACCESS_MASK? :
	const ACCESS_MASK DesiredAccess = EVENT_ALL_ACCESS;

	// redirect to Win2k/XP
	result = NtDll::NtCreateEvent(
		/*OUT*/EventHandle,
		DesiredAccess,
		nativeObjectAttributes.NtObjAttrPtr,
		(NtDll::EVENT_TYPE)EventType,
		InitialState);

	// TODO : Instead of the above, we should consider using the Ke*Event APIs, but
	// that would require us to create the event's kernel object with the Ob* api's too!

	if (FAILED(result))
	{
		EmuLog(LOG_LEVEL::WARNING, "Trying fallback (without object attributes)...\nError code 0x%X", result);

		// If it fails, try again but without the object attributes stucture
		// This fixes Panzer Dragoon games on non-Vista OSes.
		result = NtDll::NtCreateEvent(
			/*OUT*/EventHandle,
			DesiredAccess,
			/*nativeObjectAttributes.NtObjAttrPtr*/ NULL,
			(NtDll::EVENT_TYPE)EventType,
			InitialState);

		if(FAILED(result))
			EmuLog(LOG_LEVEL::WARNING, "NtCreateEvent Failed!");
		else
			EmuLog(LOG_LEVEL::DEBUG, "NtCreateEvent EventHandle = 0x%.8X", *EventHandle);
	}
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateEvent EventHandle = 0x%.8X", *EventHandle);
#endif

	RETURN(result);
}

// ******************************************************************
// * 0x00BE - NtCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(190) xbox::ntstatus_xt NTAPI xbox::NtCreateFile
(
	OUT PHANDLE             FileHandle,
	IN  access_mask_xt         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
	IN  ulong_xt               FileAttributes,
	IN  ulong_xt               ShareAccess,
	IN  ulong_xt               CreateDisposition,
	IN  ulong_xt               CreateOptions
)
{
	LOG_FORWARD("IoCreateFile");
	/* Call the I/O Function */
	return xbox::IoCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		0);
}

// ******************************************************************
// * 0x00BF - NtCreateIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(191) xbox::ntstatus_xt NTAPI xbox::NtCreateIoCompletion
(
	OUT PHANDLE IoCompletionHandle,
	IN access_mask_xt DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN ulong_xt Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(IoCompletionHandle)
		LOG_FUNC_ARG(DesiredAccess)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(Count)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(STATUS_NOT_IMPLEMENTED);
}

// ******************************************************************
// * 0x00C0 - NtCreateMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(192) xbox::ntstatus_xt NTAPI xbox::NtCreateMutant
(
	OUT PHANDLE             MutantHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	IN  boolean_xt             InitialOwner
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(MutantHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(InitialOwner)
		LOG_FUNC_END;

/*
	NTSTATUS Status;

	if (!verify arguments) {
		Status = STATUS_INVALID_PARAMETER;
	}
	else {
		PKMUTANT Mutant;

		Status = ObCreateObject(&ExMutantObjectType, ObjectAttributes, sizeof(KMUTANT), (PVOID *)&Mutant);
		if (X_NT_SUCCESS(Status)) {
			KeInitializeMutant(Mutant, InitialOwner);
			Status = ObInsertObject(Mutant, ObjectAttributes, 0, /*OUT* /MutantHandle);
		}
	}

	RETURN(Status);
*/
	LOG_INCOMPLETE(); // TODO : Verify arguments, use ObCreateObject, KeInitializeMutant and ObInsertObject instead of this:

	// initialize object attributes
	NativeObjectAttributes nativeObjectAttributes;
	CxbxObjectAttributesToNT(ObjectAttributes, /*var*/nativeObjectAttributes);

	// TODO : Is this the correct ACCESS_MASK? :
	const ACCESS_MASK DesiredAccess = MUTANT_ALL_ACCESS;

	// redirect to Windows Nt
	NTSTATUS ret = NtDll::NtCreateMutant(
		/*OUT*/MutantHandle, 
		DesiredAccess,
		nativeObjectAttributes.NtObjAttrPtr,
		InitialOwner);

	if (FAILED(ret))
	{
		EmuLog(LOG_LEVEL::WARNING, "Trying fallback (without object attributes)...\nError code 0x%X", ret);

		// If it fails, try again but without the object attributes stucture
		ret = NtDll::NtCreateMutant(
			/*OUT*/MutantHandle, 
			DesiredAccess,
			/*nativeObjectAttributes.NtObjAttrPtr*/ NULL,
			InitialOwner);

		if(FAILED(ret))
			EmuLog(LOG_LEVEL::WARNING, "NtCreateMutant Failed!");
		else
			EmuLog(LOG_LEVEL::DEBUG, "NtCreateMutant MutantHandle = 0x%.8X", *MutantHandle);
	}
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateMutant MutantHandle = 0x%.8X", *MutantHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C1 - NtCreateSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(193) xbox::ntstatus_xt NTAPI xbox::NtCreateSemaphore
(
	OUT PHANDLE             SemaphoreHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
	IN  ulong_xt               InitialCount,
	IN  ulong_xt               MaximumCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(SemaphoreHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(InitialCount)
		LOG_FUNC_ARG(MaximumCount)
		LOG_FUNC_END;

/*
	NTSTATUS Status;

	if (!verify arguments) {
		Status = STATUS_INVALID_PARAMETER;
	}
	else {
		PKSEMAPHORE Semaphore;

		Status = ObCreateObject(&ExSemaphoreObjectType, ObjectAttributes, sizeof(KSEMAPHORE), (PVOID *)&Semaphore);
		if (X_NT_SUCCESS(Status)) {
			KeInitializeSemaphore(Semaphore, InitialCount, /*Limit=* /MaximumCount);
			Status = ObInsertObject(Semaphore, ObjectAttributes, 0, /*OUT* /SemaphoreHandle);
		}
	}

	RETURN(Status);
*/
	LOG_INCOMPLETE(); // TODO : Verify arguments, use ObCreateObject, KeInitializeSemaphore and ObInsertObject instead of this:

	// TODO : Is this the correct ACCESS_MASK? :
	const ACCESS_MASK DesiredAccess = SEMAPHORE_ALL_ACCESS;

	NativeObjectAttributes nativeObjectAttributes;
	CxbxObjectAttributesToNT(ObjectAttributes, nativeObjectAttributes);

	// redirect to Win2k/XP
	NTSTATUS ret = NtDll::NtCreateSemaphore(
		/*OUT*/SemaphoreHandle,
		DesiredAccess,
		(NtDll::POBJECT_ATTRIBUTES)nativeObjectAttributes.NtObjAttrPtr,
		InitialCount,
		MaximumCount);

	if (FAILED(ret))
	{
		EmuLog(LOG_LEVEL::WARNING, "Trying fallback (without object attributes)...\nError code 0x%X", ret);

		// If it fails, try again but without the object attributes stucture
		ret = NtDll::NtCreateSemaphore(
			/*OUT*/SemaphoreHandle,
			DesiredAccess,
			/*(NtDll::POBJECT_ATTRIBUTES)nativeObjectAttributes.NtObjAttrPtr*/ NULL,
			InitialCount,
			MaximumCount);

		if(FAILED(ret))
			EmuLog(LOG_LEVEL::WARNING, "NtCreateSemaphore failed!");
		else
			EmuLog(LOG_LEVEL::DEBUG, "NtCreateSemaphore SemaphoreHandle = 0x%.8X", *SemaphoreHandle);
	}
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateSemaphore SemaphoreHandle = 0x%.8X", *SemaphoreHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C2 - NtCreateTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(194) xbox::ntstatus_xt NTAPI xbox::NtCreateTimer
(
	OUT PHANDLE TimerHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN TIMER_TYPE TimerType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(TimerHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(TimerType)
		LOG_FUNC_END;

/*
	NTSTATUS Status;

	if (!verify arguments) {
		Status = STATUS_INVALID_PARAMETER;
	}
	else {
		PKTIMER Timer;

		Status = ObCreateObject(&ExTimerType, ObjectAttributes, sizeof(KTIMER), (PVOID *)&Timer);
		if (X_NT_SUCCESS(Status)) {
			KeInitializeTimerEx(Timer, TimerType);
			Status = ObInsertObject(Timer, ObjectAttributes, 0, /*OUT* /TimerHandle);
		}
	}

	RETURN(Status);
*/
	LOG_INCOMPLETE(); // TODO : Verify arguments, use ObCreateObject, KeInitializeTimerEx and ObInsertObject instead of this:

	// TODO : Is this the correct ACCESS_MASK? :
	const ACCESS_MASK DesiredAccess = TIMER_ALL_ACCESS;

	NativeObjectAttributes nativeObjectAttributes;
	CxbxObjectAttributesToNT(ObjectAttributes, nativeObjectAttributes);

	// redirect to Windows NT
	// TODO : Untested
	NTSTATUS ret = NtDll::NtCreateTimer
	(
		/*OUT*/TimerHandle,
		DesiredAccess,
		(NtDll::POBJECT_ATTRIBUTES)nativeObjectAttributes.NtObjAttrPtr,
		(NtDll::TIMER_TYPE)TimerType
	);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtCreateTimer failed!");
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateTimer TimerHandle = 0x%.8X", *TimerHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C3 - NtDeleteFile()
// ******************************************************************
// Source: ReactOS, modified to support xbox compatibility layer
XBSYSAPI EXPORTNUM(195) xbox::ntstatus_xt NTAPI xbox::NtDeleteFile
(
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	LOG_FUNC_ONE_ARG(ObjectAttributes);

	DUMMY_FILE_OBJECT LocalFileObject;
	OPEN_PACKET OpenPacket;
	/* Setup the Open Packet */
	std::memset(&OpenPacket, 0, sizeof(OPEN_PACKET));
	OpenPacket.Type = IO_TYPE_OPEN_PACKET;
	OpenPacket.Size = sizeof(OPEN_PACKET);
	OpenPacket.CreateOptions = FILE_DELETE_ON_CLOSE;
	OpenPacket.ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	OpenPacket.Disposition = FILE_OPEN;
	OpenPacket.DeleteOnly = TRUE;
	OpenPacket.LocalFileObject = &LocalFileObject;
	OpenPacket.DesiredAccess = DELETE;

	/*
	 * Attempt opening the file. This will call the I/O Parse Routine for
	 * the File Object (IopParseDevice) which will use the dummy file object
	 * send the IRP to its device object. Note that we have two statuses
	 * to worry about: the Object Manager's status (in Status) and the I/O
	 * status, which is in the Open Packet's Final Status, and determined
	 * by the Parse Check member.
	 */
	HANDLE Handle;
	ntstatus_xt result = ObOpenObjectByName(ObjectAttributes, &IoFileObjectType, &OpenPacket, &Handle);
	if (OpenPacket.ParseCheck == FALSE) return result;

	/* Retrn the Io status */
	return OpenPacket.FinalStatus;
}

namespace xbox {

	static ntstatus_xt IopDeviceFsIoControl
	(
		IN HANDLE FileHandle,
		IN HANDLE Event OPTIONAL,
		IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
		IN PVOID ApcContext OPTIONAL,
		OUT PIO_STATUS_BLOCK IoStatusBlock,
		IN ulong_xt IoControlCode,
		IN PVOID InputBuffer OPTIONAL,
		IN ulong_xt InputBufferLength,
		OUT PVOID OutputBuffer OPTIONAL,
		IN ulong_xt OutputBufferLength,
		IN boolean_xt isDeviceIoControlFile)
	{
		PFILE_OBJECT FileObject;
		ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<xbox::PVOID*>(&FileObject));
		if (!X_NT_SUCCESS(result)) {
			return result;
		}

		/* Can't use an I/O completion port and an APC at the same time */
		if (FileObject->CompletionContext && ApcRoutine) {
			/* Fail */
			ObfDereferenceObject(FileObject);
			return X_STATUS_INVALID_PARAMETER;
		}

		/* Check for an event */
		if (Event) {
#if ENABLE_OB_EVENT // TODO: Enable this block once event handle is handled by xbox's end.
			/* Reference it */
			PKEVENT EventObject;
			result = ObReferenceObjectByHandle(Event, &ExEventObjectType, reinterpret_cast<PVOID*>(&EventObject));
			if (!X_NT_SUCCESS(result)) {
				/* Dereference the file object and fail */
				ObfDereferenceObject(FileObject);
				return result;
			}

			/* Clear it */
			NtClearEvent(EventObject);
#else // Forward native handle
			NtClearEvent(Event);
#endif
		}

		// ...

		PDEVICE_OBJECT DeviceObject = FileObject->DeviceObject;

		if (isDeviceIoControlFile) {
			switch (IoControlCode)
			{
			case 0x4D014: { // IOCTL_SCSI_PASS_THROUGH_DIRECT
				PSCSI_PASS_THROUGH_DIRECT PassThrough = (PSCSI_PASS_THROUGH_DIRECT)InputBuffer;
				PDVDX2_AUTHENTICATION Authentication = (PDVDX2_AUTHENTICATION)PassThrough->DataBuffer;

				// Should be just enough info to pass XapiVerifyMediaInDrive
				Authentication->AuthenticationPage.CDFValid = 1;
				Authentication->AuthenticationPage.PartitionArea = 1;
				Authentication->AuthenticationPage.Authentication = 1;
			}
			break;

			case 0x70000: { // IOCTL_DISK_GET_DRIVE_GEOMETRY
				PDISK_GEOMETRY DiskGeometry = (PDISK_GEOMETRY)OutputBuffer;

				if (DeviceObject->DeviceType == FILE_DEVICE_DISK2) {
					DiskGeometry->MediaType = FixedMedia;
					DiskGeometry->TracksPerCylinder = 1;
					DiskGeometry->SectorsPerTrack = 1;
					DiskGeometry->BytesPerSector = 512;
					DiskGeometry->Cylinders.QuadPart = 0x1400000;	// 10GB, size of stock xbox HDD
				}
				else if (DeviceObject->DeviceType == FILE_DEVICE_MEMORY_UNIT) {
					DiskGeometry->MediaType = FixedMedia;
					DiskGeometry->TracksPerCylinder = 1;
					DiskGeometry->SectorsPerTrack = 1;
					DiskGeometry->BytesPerSector = 512;
					DiskGeometry->Cylinders.QuadPart = 0x4000;	// 8MB, Microsoft original MUs
				}
				else {
					EmuLog(LOG_LEVEL::WARNING, "%s: Unrecongnized handle 0x%X with IoControlCode IOCTL_DISK_GET_DRIVE_GEOMETRY.", __func__, FileHandle);
					result = X_STATUS_INVALID_HANDLE;
				}
			}
			break;

			case 0x74004: { // IOCTL_DISK_GET_PARTITION_INFO
				PPARTITION_INFORMATION partitioninfo = (PPARTITION_INFORMATION)OutputBuffer;

				switch (DeviceObject->DeviceType) {
				case FILE_DEVICE_DISK2: {
					XboxPartitionTable partitionTable = CxbxGetPartitionTable();
					xbox::PIDE_DISK_EXTENSION DeviceExtension = reinterpret_cast<xbox::PIDE_DISK_EXTENSION>(DeviceObject->DeviceExtension);
					dword_xt PartitionNumber = DeviceExtension->PartitionInformation.PartitionNumber;

					// Now we read from the partition table, to fill in the partitionInfo struct
					partitioninfo->PartitionNumber = PartitionNumber;
					partitioninfo->StartingOffset.QuadPart = partitionTable.TableEntries[PartitionNumber - 1].LBAStart * 512;
					partitioninfo->PartitionLength.QuadPart = partitionTable.TableEntries[PartitionNumber - 1].LBASize * 512;
					partitioninfo->HiddenSectors = partitionTable.TableEntries[PartitionNumber - 1].Reserved;
					partitioninfo->RecognizedPartition = true;
				}
				break;
				case FILE_DEVICE_MEMORY_UNIT: {
					partitioninfo->PartitionNumber = 0;
					partitioninfo->StartingOffset.QuadPart = 0; // FIXME: where does the MU partition start?
					partitioninfo->PartitionLength.QuadPart = 16384; // 8MB
					partitioninfo->HiddenSectors = 0;
					partitioninfo->RecognizedPartition = true;
				}
				break;
				default:
					EmuLog(LOG_LEVEL::WARNING, "%s: Unrecongnized handle 0x%X with IoControlCode IOCTL_DISK_GET_PARTITION_INFO.", __func__, FileHandle);
					result = X_STATUS_INVALID_HANDLE;
					break;
				}
			}
			break;

			default:
				LOG_UNIMPLEMENTED();
			}
		}
		else {
			switch (IoControlCode) {
			case fsctl_dismount_volume: {

				if (DeviceObject->DeviceType == FILE_DEVICE_DISK2) {
					// HACK: this should just free the resources assocoated with the volume, it should not reformat it
					xbox::PIDE_DISK_EXTENSION DeviceExtension = reinterpret_cast<xbox::PIDE_DISK_EXTENSION>(DeviceObject->DeviceExtension);
					dword_xt PartitionNumber = DeviceExtension->PartitionInformation.PartitionNumber;
					if (EmuDiskFormatPartition(PartitionNumber)) {
						result = X_STATUS_SUCCESS;
					}
					else {
						// TODO: Is it correct status?
						result = X_STATUS_INVALID_DEVICE_REQUEST;
					}
				}
				else {
					LOG_UNIMPLEMENTED();
				}
			}
			break;

			case fsctl_read_fatx_metadata: {
#if 0 // TODO: Enable this block and port memory unit support base on EmuDisk.
				const std::wstring path = CxbxGetFinalPathNameByHandle(FileHandle);
				size_t pos = path.rfind(L"\\EmuMu");
				if (pos != std::string::npos && path[pos + 6] == '\\') {
					// Ensure that InputBuffer is indeed what we think it is
					pfatx_volume_metadata volume = static_cast<pfatx_volume_metadata>(InputBuffer);
					assert(InputBufferLength == sizeof(fatx_volume_metadata));
					g_io_mu_metadata->read(path[pos + 7], volume->offset, static_cast<char*>(volume->buffer), volume->length);
					result = X_STATUS_SUCCESS;
				}
				else {
#endif
					result = X_STATUS_INVALID_HANDLE;
				//}
			}
			break;

			case fsctl_write_fatx_metadata: {
#if 0 // TODO: Enable this block and port memory unit support base on EmuDisk.
				const std::wstring path = CxbxGetFinalPathNameByHandle(FileHandle);
				size_t pos = path.rfind(L"\\EmuMu");
				if (pos != std::string::npos && path[pos + 6] == '\\') {
					// Ensure that InputBuffer is indeed what we think it is
					pfatx_volume_metadata volume = static_cast<pfatx_volume_metadata>(InputBuffer);
					assert(InputBufferLength == sizeof(fatx_volume_metadata));
					g_io_mu_metadata->write(path[pos + 7], volume->offset, static_cast<const char*>(volume->buffer), volume->length);
					result = X_STATUS_SUCCESS;
				}
				else {
#endif
					result = X_STATUS_INVALID_HANDLE;
				//}
			}
			break;

			}

			LOG_INCOMPLETE();
		}
		//FileObject->Event.Header.SignalState = 0;

		// ...

		// irp code goes here

		//int MajorFunction = isDeviceIoControlFile ? IRP_MJ_DEVICE_CONTROL : IRP_MJ_FILE_SYSTEM_CONTROL;

		/* Perform the call */
		//return IopPerformSynchronousRequest(...);

		// TODO: Remove ObfDereferenceObject as it may already had been done elsewhere.
		ObfDereferenceObject(FileObject);

		return result;
	}
}

// ******************************************************************
// * 0x00C4 - NtDeviceIoControlFile()
// ******************************************************************
// Source: ReactOS
XBSYSAPI EXPORTNUM(196) xbox::ntstatus_xt NTAPI xbox::NtDeviceIoControlFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ulong_xt IoControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ulong_xt InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ulong_xt OutputBufferLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(IoControlCode)
		LOG_FUNC_ARG(InputBuffer)
		LOG_FUNC_ARG(InputBufferLength)
		LOG_FUNC_ARG_OUT(OutputBuffer)
		LOG_FUNC_ARG(OutputBufferLength)
		LOG_FUNC_END;

	/* Call the Generic Function */
	ntstatus_xt result = IopDeviceFsIoControl(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, true);

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(IoStatusBlock)
	LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
}

// ******************************************************************
// * 0x00C5 - NtDuplicateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(197) xbox::ntstatus_xt NTAPI xbox::NtDuplicateObject
(
	HANDLE   SourceHandle,
	PHANDLE  TargetHandle,
	dword_xt Options
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SourceHandle)
		LOG_FUNC_ARG(TargetHandle)
		LOG_FUNC_ARG(Options)
		LOG_FUNC_END;

	ntstatus_xt result;

	PVOID Object;
	result = ObReferenceObjectByHandle(SourceHandle, zeroptr, &Object);
	if (X_NT_SUCCESS(result)) {
		if (ObpIsFlagSet(Options, DUPLICATE_CLOSE_SOURCE)) {
			NtClose(SourceHandle);
		}

		result = ObOpenObjectByPointer(Object, OBJECT_TO_OBJECT_HEADER(Object)->Type, TargetHandle);
		if (!X_NT_SUCCESS(result)) {
			*TargetHandle = NULL;
			RETURN(result);
		}

		ObfDereferenceObject(Object);
	}
	// TODO: Remove "else if" statement once all items from list from NtClose is done.
	// Check if Handle is from Host's end.
	else if (DWORD flags = 0; GetHandleInformation(SourceHandle, &flags)) {
		// On the xbox, the duplicated handle always has the same access rights of the source handle
		const ACCESS_MASK DesiredAccess = 0;
		const ULONG Attributes = 0;
		const ULONG nativeOptions = (Options | DUPLICATE_SAME_ATTRIBUTES | DUPLICATE_SAME_ACCESS);

		::HANDLE dupHandle;
		result = NtDll::NtDuplicateObject(
			/*SourceProcessHandle=*/g_CurrentProcessHandle,
			SourceHandle,
			/*TargetProcessHandle=*/g_CurrentProcessHandle,
			&dupHandle,
			DesiredAccess,
			Attributes,
			nativeOptions);

		if (!X_NT_SUCCESS(result)) {
			CxbxrAbort("NtDll::NtDuplicateObject failed to duplicate the handle 0x%.8X!", SourceHandle);
		}
	}

	RETURN(result);
}

// ******************************************************************
// * 0x00C6 - NtFlushBuffersFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(198) xbox::ntstatus_xt NTAPI xbox::NtFlushBuffersFile
(
	PVOID                FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;

	/* Get the File Object */
	PVOID Object;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, &Object);
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	// TODO: Implement irp work here.

	if (const auto& nhandle = GetObjectNativeHandle(Object)) {
		result = NtDll::NtFlushBuffersFile(*nhandle, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock);
	}
	else {
		EmuLog(LOG_LEVEL::ERROR2, "%s: Could not get native handle from xobject: %08X", __func__, Object);
		result = X_STATUS_INVALID_HANDLE;
	}

	ObfDereferenceObject(Object);

	RETURN(result);
}

// ******************************************************************
// * 0x00C7 - NtFreeVirtualMemory()
// ******************************************************************
// Frees virtual memory.
//
// Differences from NT: There is no ProcessHandle parameter.
XBSYSAPI EXPORTNUM(199) xbox::ntstatus_xt NTAPI xbox::NtFreeVirtualMemory
(
	IN OUT PVOID *BaseAddress,
	IN OUT PULONG FreeSize,
	IN ulong_xt      FreeType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(FreeSize)
		LOG_FUNC_ARG_TYPE(ALLOCATION_TYPE, FreeType)
	LOG_FUNC_END;

	NTSTATUS ret = g_VMManager.XbFreeVirtualMemory((VAddr*)BaseAddress, (size_t*)FreeSize, FreeType);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C8 - NtFsControlFile
// ******************************************************************
// Source: ReactOS
XBSYSAPI EXPORTNUM(200) xbox::ntstatus_xt NTAPI xbox::NtFsControlFile
(
	IN HANDLE               FileHandle,
	IN HANDLE               Event OPTIONAL,
	IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
	IN PVOID                ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN ulong_xt                FsControlCode,
	IN PVOID                InputBuffer OPTIONAL,
	IN ulong_xt                InputBufferLength,
	OUT PVOID               OutputBuffer OPTIONAL,
	IN ulong_xt                OutputBufferLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(FsControlCode)
		LOG_FUNC_ARG(InputBuffer)
		LOG_FUNC_ARG(InputBufferLength)
		LOG_FUNC_ARG(OutputBuffer)
		LOG_FUNC_ARG(OutputBufferLength)
		LOG_FUNC_END;

	/* Call the Generic Function */
	ntstatus_xt result = IopDeviceFsIoControl(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FsControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, false);

	LOG_FUNC_BEGIN_ARG_RESULT
		LOG_FUNC_ARG_RESULT(IoStatusBlock)
		LOG_FUNC_END_ARG_RESULT;

	RETURN(result);
}

// ******************************************************************
// * 0x00C9 - NtOpenDirectoryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(201) xbox::ntstatus_xt NTAPI xbox::NtOpenDirectoryObject
(
	OUT PHANDLE DirectoryHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	LOG_FORWARD("ObOpenObjectByName");

	return ObOpenObjectByName(ObjectAttributes, &ObDirectoryObjectType, NULL, DirectoryHandle);
}

// ******************************************************************
// * 0x00CA - NtOpenFile()
// ******************************************************************
// Opens a file or device object.  Same as calling:
//   NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes,
//     IoStatusBlock, NULL, 0, ShareAccess, OPEN_EXISTING, OpenOptions);
//
// Differences from NT: See NtCreateFile.
XBSYSAPI EXPORTNUM(202) xbox::ntstatus_xt NTAPI xbox::NtOpenFile
(
	OUT PHANDLE             FileHandle,
	IN  access_mask_xt         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  ulong_xt               ShareAccess,
	IN  ulong_xt               OpenOptions
)
{
	LOG_FORWARD("IoCreateFile");

	return xbox::IoCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		/*AllocationSize=*/NULL,
		/*FileAttributes=*/0,
		ShareAccess,
		/*Disposition=*/FILE_OPEN,
		/*CreateOptions=*/OpenOptions,
		/*Options=*/0);
}

// ******************************************************************
// * 0x00CB - NtOpenSymbolicLinkObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(203) xbox::ntstatus_xt NTAPI xbox::NtOpenSymbolicLinkObject
(
	OUT PHANDLE LinkHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	LOG_FORWARD("ObOpenObjectByName");

	return ObOpenObjectByName(ObjectAttributes, &ObSymbolicLinkObjectType, NULL, LinkHandle);
}

// ******************************************************************
// * 0x00CC - NtProtectVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(204) xbox::ntstatus_xt NTAPI xbox::NtProtectVirtualMemory
(
	IN OUT PVOID *BaseAddress,
	IN OUT PSIZE_T RegionSize,
	IN ulong_xt NewProtect,
	OUT PULONG OldProtect
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(RegionSize)
		LOG_FUNC_ARG_TYPE(PROTECTION_TYPE, NewProtect)
		LOG_FUNC_ARG_OUT(OldProtect)
	LOG_FUNC_END;


	DWORD Perms = NewProtect;
	NTSTATUS ret = g_VMManager.XbVirtualProtect((VAddr*)BaseAddress, (size_t*)RegionSize, &Perms);
	*OldProtect = Perms;

	RETURN(ret);
}

// ******************************************************************
// * 0x00CD - NtPulseEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(205) xbox::ntstatus_xt NTAPI xbox::NtPulseEvent
(
	IN HANDLE	EventHandle,
	OUT PLONG	PreviousState OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	// redirect to Windows NT
	// TODO : Untested
	NTSTATUS ret = NtDll::NtPulseEvent(
		EventHandle, 
		/*OUT*/(::PLONG)(PreviousState));

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtPulseEvent failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00CE - NtQueueApcThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(206) xbox::ntstatus_xt NTAPI xbox::NtQueueApcThread
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN PVOID                ApcReserved OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcRoutineContext)
		LOG_FUNC_ARG(ApcStatusBlock)
		LOG_FUNC_ARG(ApcReserved)
		LOG_FUNC_END;

	// Test case: Metal Slug 3, possibly other SNK games too

	PETHREAD Thread;
	ntstatus_xt result = ObReferenceObjectByHandle(ThreadHandle, &PsThreadObjectType, reinterpret_cast<PVOID *>(&Thread));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	PKAPC Apc = static_cast<PKAPC>(ExAllocatePoolWithTag(sizeof(KAPC), 'pasP'));
	if (Apc != zeroptr) {
		KeInitializeApc(Apc, &Thread->Tcb, KiFreeUserApc, zeroptr, reinterpret_cast<PKNORMAL_ROUTINE>(ApcRoutine), UserMode, ApcRoutineContext);
		if (!KeInsertQueueApc(Apc, ApcStatusBlock, ApcReserved, 0)) {
			ExFreePool(Apc);
			result = X_STATUS_UNSUCCESSFUL;
		}
	}
	else {
		result = X_STATUS_NO_MEMORY;
	}

	ObfDereferenceObject(Thread);

	RETURN(result);
}

// ******************************************************************
// * 0x00CF - NtQueryDirectoryFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(207) xbox::ntstatus_xt NTAPI xbox::NtQueryDirectoryFile
(
	IN  HANDLE                      FileHandle,
	IN  HANDLE                      Event OPTIONAL,
	IN  PVOID                       ApcRoutine, // Todo: define this routine's prototype
	IN  PVOID                       ApcContext,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT FILE_DIRECTORY_INFORMATION *FileInformation,
	IN  ulong_xt                       Length,
	IN  FILE_INFORMATION_CLASS      FileInformationClass,
	IN  PSTRING                     FileMask,
	IN  boolean_xt                     RestartScan
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_ARG(FileMask)
		LOG_FUNC_ARG(RestartScan)
		LOG_FUNC_END;

	NTSTATUS ret;

	if (FileInformationClass != FileDirectoryInformation)   // Due to unicode->string conversion
		CxbxrAbort("Unsupported FileInformationClass");

	/* Get File Object */
	PFILE_OBJECT FileObject;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObject));
	if (!X_NT_SUCCESS(result)) {
		/* Fail */
		RETURN(result);
	}

	if (FileObject->CompletionContext && ApcRoutine) {
		ObfDereferenceObject(FileObject);
		return X_STATUS_INVALID_PARAMETER;
	}

	const auto& nFileHandle = GetObjectNativeHandle(FileObject);

	NtDll::UNICODE_STRING NtFileMask;

	wchar_t wszObjectName[MAX_PATH];

	// initialize FileMask
	{
		if (FileMask != 0) {
			// Xbox expects directories to be listed when *.* is passed
			if (strncmp(FileMask->Buffer, "*.*", FileMask->Length) == 0) {
				FileMask->Length = 1;
				std::strcpy(FileMask->Buffer, "*");
			}

			mbstowcs(/*Dest=*/wszObjectName, /*Source=*/FileMask->Buffer, /*MaxCount=*/MAX_PATH);
		} else
			mbstowcs(/*Dest=*/wszObjectName, /*Source=*/"", /*MaxCount=*/MAX_PATH);

		NtDll::RtlInitUnicodeString(&NtFileMask, wszObjectName);
	}

	NtDll::FILE_DIRECTORY_INFORMATION *NtFileDirInfo = 
		(NtDll::FILE_DIRECTORY_INFORMATION *) malloc(NtFileDirectoryInformationSize + NtPathBufferSize);

	// Short-hand pointer to Nt filename :
	wchar_t *wcstr = NtFileDirInfo->FileName;
	char    *mbstr = FileInformation->FileName;

	// Go, query that directory :
	do
	{
		ZeroMemory(wcstr, MAX_PATH * sizeof(wchar_t));

		ret = NtDll::NtQueryDirectoryFile(
			*nFileHandle,
			Event, 
			(NtDll::PIO_APC_ROUTINE)ApcRoutine,
			ApcContext,
			(NtDll::IO_STATUS_BLOCK*)IoStatusBlock, 
			/*FileInformation=*/NtFileDirInfo,
			NtFileDirectoryInformationSize + NtPathBufferSize,
			(NtDll::FILE_INFORMATION_CLASS)FileInformationClass, 
			/*ReturnSingleEntry=*/TRUE,
			&NtFileMask,
			RestartScan
		);

		RestartScan = FALSE;
	}
	// Xbox does not return . and ..
	while (wcscmp(wcstr, L".") == 0 || wcscmp(wcstr, L"..") == 0);

	// convert from PC to Xbox
	{
		// TODO : assert that NtDll::FILE_DIRECTORY_INFORMATION has same members and size as xbox::FILE_DIRECTORY_INFORMATION
		memcpy(/*Dst=*/FileInformation, /*Src=*/NtFileDirInfo, /*Size=*/NtFileDirectoryInformationSize);
		wcstombs(/*Dest=*/mbstr, /*Source=*/wcstr, MAX_PATH);
		FileInformation->FileNameLength /= sizeof(wchar_t);
	}

	// TODO: Cache the last search result for quicker access with CreateFile (xbox does this internally!)
	free(NtFileDirInfo);

	ObfDereferenceObject(FileObject);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D0 - NtQueryDirectoryObject
// ******************************************************************
XBSYSAPI EXPORTNUM(208) xbox::ntstatus_xt NTAPI xbox::NtQueryDirectoryObject
(
	IN HANDLE DirectoryHandle,
	OUT PVOID Buffer,
	IN ulong_xt Length,
	IN boolean_xt RestartScan,
	IN OUT PULONG Context,
	OUT PULONG ReturnedLength OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(DirectoryHandle)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(RestartScan)
		LOG_FUNC_ARG(Context)
		LOG_FUNC_ARG_OUT(ReturnedLength)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00D1  - NtQueryEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(209) xbox::ntstatus_xt NTAPI xbox::NtQueryEvent
(
	IN HANDLE EventHandle,
	OUT PEVENT_BASIC_INFORMATION EventInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(EventInformation)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtQueryEvent(
		(NtDll::HANDLE)EventHandle,
		/*EventInformationClass*/NtDll::EVENT_INFORMATION_CLASS::EventBasicInformation,
		EventInformation,
		sizeof(EVENT_BASIC_INFORMATION),
		/*ReturnLength=*/nullptr);

	if (ret != X_STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQueryEvent failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D2 - NtQueryFullAttributesFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(210) xbox::ntstatus_xt NTAPI xbox::NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT xbox::PFILE_NETWORK_OPEN_INFORMATION   FileInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_END;

	OPEN_PACKET OpenPacket{};
	[[maybe_unused]] DUMMY_FILE_OBJECT LocalFileObject;

	OpenPacket.Type = IO_TYPE_OPEN_PACKET;
	OpenPacket.Size = sizeof(OPEN_PACKET);
	OpenPacket.ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	OpenPacket.Disposition = FILE_OPEN;
	OpenPacket.DesiredAccess = FILE_READ_ATTRIBUTES;
	OpenPacket.NetworkInformation = FileInformation;
	OpenPacket.LocalFileObject = &LocalFileObject;
	OpenPacket.QueryOnly = true;


	// Since we perform query information, we can pass it on to ObOpenObjectByName function to do the work for us.
	HANDLE handle;
	ntstatus_xt result = ObOpenObjectByName(ObjectAttributes, &IoFileObjectType, &OpenPacket, &handle);

	if (OpenPacket.ParseCheck == false) {
		/* Parse failed */
		//DPRINT("IopQueryAttributesFile failed for '%wZ' with 0x%lx\n", ObjectAttributes->ObjectName, result);
		RETURN(result);
	}
	else {
		result = OpenPacket.FinalStatus;
	}

	RETURN(result);
}

// ******************************************************************
// * 0x00D3 - NtQueryInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(211) xbox::ntstatus_xt NTAPI xbox::NtQueryInformationFile
(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PVOID                       FileInformation,
	IN  ulong_xt                       Length,
	IN  FILE_INFORMATION_CLASS      FileInformationClass
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_END;

	/* Validate the information class */
	if ((FileInformationClass < 0) ||
		(FileInformationClass >= FileMaximumInformation) ||
		!(IopQueryOperationLength[FileInformationClass]))
	{
		/* Invalid class */
		return X_STATUS_INVALID_INFO_CLASS;
	}

	/* Validate the length */
	if (Length < IopQueryOperationLength[FileInformationClass])
	{
		/* Invalid length */
		return X_STATUS_INFO_LENGTH_MISMATCH;
	}

	PFILE_OBJECT FileObject;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObject));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	// TODO: Need to implement IRP here for query callback.

	// Start with sizeof(corresponding struct)
	size_t bufferSize = IopQueryOperationLength[FileInformationClass];

	const auto& nHandle = GetObjectNativeHandle(FileObject);
	if (!nHandle) {
		RETURN(X_STATUS_INVALID_PARAMETER);
	}

	ObfDereferenceObject(FileObject);

	// We need to retry the operation in case the buffer is too small to fit the data
	PVOID ntFileInfo;
	do
	{
		ntFileInfo = malloc(bufferSize);

		result = NtDll::NtQueryInformationFile(
			*nHandle,
			(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
			ntFileInfo,
			bufferSize,
			(NtDll::FILE_INFORMATION_CLASS)FileInformationClass);

		// Buffer is too small; make a larger one
		if (result == X_STATUS_BUFFER_OVERFLOW)
		{
			free(ntFileInfo);

			bufferSize *= 2;
			// Bail out if the buffer gets too big
			if (bufferSize > 65536)
				return STATUS_INVALID_PARAMETER;   // TODO: what's the appropriate error code to return here?
		}
	} while (result == X_STATUS_BUFFER_OVERFLOW);
	
	// Convert and copy NT data to the given Xbox struct
	result = NTToXboxFileInformation(ntFileInfo, FileInformation, FileInformationClass, Length);

	// Make sure to free the memory first
	free(ntFileInfo);

	if (FAILED(result))
		EmuLog(LOG_LEVEL::WARNING, "NtQueryInformationFile failed! (0x%.08X)", result);

	// Prioritize the buffer overflow over real return code,
	// in case the Xbox program decides to follow the same procedure above
	if (result == X_STATUS_BUFFER_OVERFLOW)
		RETURN(result);

	RETURN(result);
}

// ******************************************************************
// * 0x00D4 - NtQueryIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(212) xbox::ntstatus_xt NTAPI xbox::NtQueryIoCompletion
(
	IN HANDLE IoCompletionHandle,
	OUT PIO_COMPLETION_BASIC_INFORMATION IoCompletionInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoCompletionHandle)
		LOG_FUNC_ARG_OUT(IoCompletionInformation)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00D5 - NtQueryMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(213) xbox::ntstatus_xt NTAPI xbox::NtQueryMutant
(
	IN HANDLE MutantHandle,
	OUT PMUTANT_BASIC_INFORMATION MutantInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MutantHandle)
		LOG_FUNC_ARG_OUT(MutantInformation)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtQueryMutant(
		(NtDll::HANDLE)MutantHandle,
		/*MutantInformationClass*/NtDll::MUTANT_INFORMATION_CLASS::MutantBasicInformation,
		MutantInformation,
		sizeof(MUTANT_BASIC_INFORMATION),
		/*ReturnLength=*/nullptr);

	if (ret != X_STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQueryMutant failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D6 - NtQuerySemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(214) xbox::ntstatus_xt NTAPI xbox::NtQuerySemaphore
(
	IN HANDLE SemaphoreHandle,
	OUT PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SemaphoreHandle)
		LOG_FUNC_ARG_OUT(SemaphoreInformation)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtQuerySemaphore(
		(NtDll::HANDLE)SemaphoreHandle,
		/*SemaphoreInformationClass*/NtDll::SEMAPHORE_INFORMATION_CLASS::SemaphoreBasicInformation,
		SemaphoreInformation,
		sizeof(SEMAPHORE_BASIC_INFORMATION),
		/*ReturnLength=*/nullptr);

	if (ret != X_STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQuerySemaphore failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D7 - NtQuerySymbolicLinkObject()
// ******************************************************************
// Source: ReactOS, modified for xbox compatibility layer
XBSYSAPI EXPORTNUM(215) xbox::ntstatus_xt NTAPI xbox::NtQuerySymbolicLinkObject
(
	HANDLE LinkHandle,
	OUT PSTRING LinkTarget,
	OUT PULONG ReturnedLength OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(LinkHandle)
		LOG_FUNC_ARG_OUT(LinkTarget)
		LOG_FUNC_ARG_OUT(ReturnedLength)
		LOG_FUNC_END;

	POBJECT_SYMBOLIC_LINK SymlinkObject;
	POBJECT_HEADER ObjectHeader;

	/* Reference the object */
	ntstatus_xt result = ObReferenceObjectByHandle(LinkHandle, &ObSymbolicLinkObjectType,reinterpret_cast<PVOID*>(&SymlinkObject));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	/* Get the object header */
	ObjectHeader = OBJECT_TO_OBJECT_HEADER(SymlinkObject);

	/*
	 * So here's the thing: If you specify a return length, then the
	 * implementation will use the maximum length. If you don't, then
	 * it will use the length.
	 */
	ULONG LengthUsed = SymlinkObject->LinkTarget.Length;

	/* Make sure our buffer will fit */
	if (LengthUsed <= LinkTarget->MaximumLength) {
		/* Copy the buffer */
		std::memcpy(LinkTarget->Buffer, SymlinkObject->LinkTarget.Buffer, LengthUsed);

		/* Copy the new length */
		LinkTarget->Length = SymlinkObject->LinkTarget.Length;
	}
	else {
		/* Otherwise set the failure status */
		result = X_STATUS_BUFFER_TOO_SMALL;
	}

	/* In both cases, check if the required length was requested */
	if (ReturnedLength) {
		/* Then return it */
		*ReturnedLength = SymlinkObject->LinkTarget.MaximumLength;
	}

	/* Dereference the object */
	ObfDereferenceObject(SymlinkObject);

	/* Return query status */
	return result;
}

// ******************************************************************
// * 0x00D8 - NtQueryTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(216) xbox::ntstatus_xt NTAPI xbox::NtQueryTimer
(
	IN HANDLE TimerHandle,
	OUT PTIMER_BASIC_INFORMATION TimerInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TimerHandle)
		LOG_FUNC_ARG_OUT(TimerInformation)
		LOG_FUNC_END;

	// redirect to Windows NT
	// TODO : Untested
	NTSTATUS ret = NtDll::NtQueryTimer(
		TimerHandle,
		/*TIMER_INFORMATION_CLASS*/NtDll::TimerBasicInformation,
		/*OUT*/TimerInformation,
		/*TimerInformationLength=*/sizeof(TIMER_BASIC_INFORMATION),
		/*OUT ReturnLength*/nullptr
	);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D9 - NtQueryVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(217) xbox::ntstatus_xt NTAPI xbox::NtQueryVirtualMemory
(
	IN  PVOID                       BaseAddress,
	OUT PMEMORY_BASIC_INFORMATION   Buffer
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG_OUT(Buffer)
	LOG_FUNC_END;

	if (!Buffer)
	{
		EmuLog(LOG_LEVEL::WARNING, "NtQueryVirtualMemory : PMEMORY_BASIC_INFORMATION Buffer is nullptr!\n");
		LOG_IGNORED();
		RETURN(STATUS_INVALID_PARAMETER);
	}

	NTSTATUS ret = g_VMManager.XbVirtualMemoryStatistics((VAddr)BaseAddress, Buffer);

	if (ret == X_STATUS_SUCCESS)
	{
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->AllocationBase    = 0x%.08X", Buffer->AllocationBase);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->AllocationProtect = 0x%.08X", Buffer->AllocationProtect);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->BaseAddress       = 0x%.08X", Buffer->BaseAddress);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->RegionSize        = 0x%.08X", Buffer->RegionSize);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->State             = 0x%.08X", Buffer->State);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->Protect           = 0x%.08X", Buffer->Protect);
		EmuLog(LOG_LEVEL::DEBUG, "   Buffer->Type              = 0x%.08X", Buffer->Type);
	}

	#if 0
	if (FAILED(result)) {
		EmuLog(LOG_LEVEL::WARNING, "NtQueryVirtualMemory failed (%s)!", NtStatusToString(result));

		// Bugfix for "Forza Motorsport", which iterates over 2 Gb of memory in 64kb chunks,
		// but fails on this last query. It's not done though, as after this Forza tries to
		// NtAllocateVirtualMemory at address 0x00000000 (3 times, actually) which fails too...
		//
		// Ported back from dxbx, translator PatrickvL

		if (BaseAddress == (PVOID)0x7FFF0000) {
			Buffer->BaseAddress = BaseAddress;
			Buffer->AllocationBase = BaseAddress;
			Buffer->AllocationProtect = PAGE_READONLY;
			Buffer->RegionSize = 64 * 1024;             // size, in bytes, of the region beginning at the base address in which all pages have identical attributes
			Buffer->State = 4096;                       // MEM_DECOMMIT | PAGE_EXECUTE_WRITECOPY etc
			Buffer->Protect = PAGE_READONLY;            // One of the flags listed for the AllocationProtect member is specified
			Buffer->Type = 262144;                      // Specifies the type of pages in the region. (MEM_IMAGE, MEM_MAPPED or MEM_PRIVATE)

			result = X_STATUS_SUCCESS;

			EmuLog(LOG_LEVEL::DEBUG, "NtQueryVirtualMemory: Applied fix for Forza Motorsport!");
		}
	}
	#endif

	RETURN(ret);
}

xbox::uchar_xt IopQueryFsOperationLength[] = {
	0,
	sizeof(NtDll::FILE_FS_VOLUME_INFORMATION),
	0,
	sizeof(NtDll::FILE_FS_SIZE_INFORMATION),
	sizeof(NtDll::FILE_FS_DEVICE_INFORMATION),
	sizeof(NtDll::FILE_FS_ATTRIBUTE_INFORMATION),
	0,//sizeof(NtDll::FILE_FS_CONTROL_INFORMATION),
	sizeof(NtDll::FILE_FS_FULL_SIZE_INFORMATION),
	sizeof(NtDll::FILE_FS_OBJECTID_INFORMATION),
	0,//sizeof(NtDll::FILE_FS_DRIVER_PATH_INFORMATION),
	0xFF
};

// ******************************************************************
// * 0x00DA - NtQueryVolumeInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(218) xbox::ntstatus_xt NTAPI xbox::NtQueryVolumeInformationFile
(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
	IN  ulong_xt                       Length,
	IN  FS_INFORMATION_CLASS        FileInformationClass
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_END;

	/* Validate the information class */
	if ((FileInformationClass < 0) ||
		(FileInformationClass >= FileFsMaximumInformation) ||
		!(IopQueryFsOperationLength[FileInformationClass])) {
		/* Invalid class */
		return X_STATUS_INVALID_INFO_CLASS;
	}

	/* Validate the length */
	if (Length < IopQueryFsOperationLength[FileInformationClass]) {
		/* Invalid length */
		return X_STATUS_INFO_LENGTH_MISMATCH;
	}

	/* Get File Object */
	PFILE_OBJECT FileObject;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObject));
	if (!X_NT_SUCCESS(result)) {
		return result;
	}

	// FileFsSizeInformation is a special case that should read from our emulated partition table
	if ((DWORD)FileInformationClass == FileFsSizeInformation) {
		PFILE_FS_SIZE_INFORMATION XboxSizeInfo = (PFILE_FS_SIZE_INFORMATION)FileInformation;

		switch (FileObject->DeviceObject->DeviceType) {
		case FILE_DEVICE_DISK2: {

			XboxPartitionTable partitionTable = CxbxGetPartitionTable();
			PIDE_DISK_EXTENSION DeviceExtension = reinterpret_cast<PIDE_DISK_EXTENSION>(FileObject->DeviceObject->DeviceExtension);
			int partitionNumber = DeviceExtension->PartitionInformation.PartitionNumber;
			FATX_SUPERBLOCK superBlock = CxbxGetFatXSuperBlock(partitionNumber);

			XboxSizeInfo->BytesPerSector = 512;

			// In some cases, the emulated partition hasn't been formatted yet, as these are forwarded to a real folder, this doesn't actually matter.
			// We just pretend they are valid by defaulting the SectorsPerAllocationUnit value to the most common for system partitions
			XboxSizeInfo->SectorsPerAllocationUnit = 32;

			// If there is a valid cluster size, we calculate SectorsPerAllocationUnit from that instead
			if (superBlock.ClusterSize > 0) {
				XboxSizeInfo->SectorsPerAllocationUnit = superBlock.ClusterSize;
			}

			XboxSizeInfo->TotalAllocationUnits.QuadPart = partitionTable.TableEntries[partitionNumber - 1].LBASize / XboxSizeInfo->SectorsPerAllocationUnit;
			XboxSizeInfo->AvailableAllocationUnits.QuadPart = partitionTable.TableEntries[partitionNumber - 1].LBASize / XboxSizeInfo->SectorsPerAllocationUnit;

			result = X_STATUS_SUCCESS;
		}
		break;

		case FILE_DEVICE_MEMORY_UNIT:

			XboxSizeInfo->BytesPerSector = 512;
			XboxSizeInfo->SectorsPerAllocationUnit = 32;
			XboxSizeInfo->TotalAllocationUnits.QuadPart = 512; // 8MB -> ((1024)^2 * 8) / (BytesPerSector * SectorsPerAllocationUnit)
			XboxSizeInfo->AvailableAllocationUnits.QuadPart = 512; // constant, so there's always free space available to write stuff

			result = X_STATUS_SUCCESS;
			break;

		case FILE_DEVICE_CD_ROM2:
			XboxSizeInfo->BytesPerSector = 2048;
			XboxSizeInfo->SectorsPerAllocationUnit = 1;
			XboxSizeInfo->TotalAllocationUnits.QuadPart = 3820880; // assuming DVD-9 (dual layer), redump reports a total size in bytes of 7825162240

			result = X_STATUS_SUCCESS;
			break;

		default:
			EmuLog(LOG_LEVEL::WARNING, "%s: Unrecongnized handle 0x%X with class FileFsSizeInformation.", __func__, FileHandle);
			result = X_STATUS_INVALID_HANDLE;
			break;
		}
		ObfDereferenceObject(FileObject);

		if (X_NT_SUCCESS(result)) {
			LOG_FUNC_BEGIN_ARG_RESULT
				LOG_FUNC_ARG_RESULT(FileInformation)
			LOG_FUNC_END_ARG_RESULT;
		}

		RETURN(result);
	}

	// Get the required size for the host buffer
	// This may differ than the xbox buffer size so we also need to handle conversions!
	ULONG HostBufferSize = 0;
	switch ((DWORD)FileInformationClass) {
		case FileFsVolumeInformation:
			// Reserve a large enough buffer for the file information 
			// including the variable length path field
			HostBufferSize = sizeof(NtDll::FILE_FS_VOLUME_INFORMATION) + MAX_PATH;
			break;
		case FileFsLabelInformation:
			HostBufferSize = sizeof(NtDll::FILE_FS_LABEL_INFORMATION);
			break;
		case FileFsDeviceInformation:
			HostBufferSize = sizeof(NtDll::FILE_FS_DEVICE_INFORMATION);
			break;
		case FileFsAttributeInformation:
			HostBufferSize = sizeof(NtDll::FILE_FS_ATTRIBUTE_INFORMATION);
			break;
		case FileFsFullSizeInformation:
			HostBufferSize = sizeof(NtDll::FILE_FS_FULL_SIZE_INFORMATION);
			break;
		case FileFsObjectIdInformation:
			HostBufferSize = sizeof(NtDll::FILE_FS_OBJECTID_INFORMATION);
			break;
	}

	PVOID NativeFileInformation = _aligned_malloc(HostBufferSize, 8);

	const auto& nFileHandle = GetObjectNativeHandle(FileObject);

	NTSTATUS ret = NtDll::NtQueryVolumeInformationFile(
		*nFileHandle,
		(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
		(NtDll::PFILE_FS_SIZE_INFORMATION)NativeFileInformation, HostBufferSize,
		(NtDll::FS_INFORMATION_CLASS)FileInformationClass);

	// Convert Xbox NativeFileInformation to FileInformation
	if (ret == X_STATUS_SUCCESS) {
		switch ((DWORD)FileInformationClass) {
				case FileFsVolumeInformation: {
					PFILE_FS_VOLUME_INFORMATION XboxVolumeInfo = (PFILE_FS_VOLUME_INFORMATION)FileInformation;
					NtDll::PFILE_FS_VOLUME_INFORMATION HostVolumeInfo = (NtDll::PFILE_FS_VOLUME_INFORMATION)NativeFileInformation;

					// Most options can just be directly copied to the Xbox version, only the strings differ
					XboxVolumeInfo->VolumeCreationTime.QuadPart = HostVolumeInfo->VolumeCreationTime.QuadPart;
					XboxVolumeInfo->VolumeSerialNumber = HostVolumeInfo->VolumeSerialNumber;
					XboxVolumeInfo->VolumeLabelLength = HostVolumeInfo->VolumeLabelLength;
					XboxVolumeInfo->SupportsObjects = HostVolumeInfo->SupportsObjects;

					// Convert strings to the Xbox format 
					wcstombs(XboxVolumeInfo->VolumeLabel, HostVolumeInfo->VolumeLabel, HostVolumeInfo->VolumeLabelLength);
				}
				break;
			default:
				// For all other types, just do a memcpy and hope for the best!
				EmuLog(LOG_LEVEL::WARNING, "NtQueryVolumeInformationFile: Unknown FileInformationClass");
				memcpy_s(FileInformation, Length, NativeFileInformation, HostBufferSize);
				break;
		}
	}

	_aligned_free(NativeFileInformation);

	if (FAILED(ret)) {
		EmuLog(LOG_LEVEL::WARNING, "NtQueryVolumeInformationFile failed! (%s)\n", NtStatusToString(ret));
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00DB - NtReadFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(219) xbox::ntstatus_xt NTAPI xbox::NtReadFile
(
	IN  HANDLE          FileHandle,            // TODO: correct paramters
	IN  HANDLE          Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID           Buffer,
	IN  ulong_xt           Length,
	IN  PLARGE_INTEGER  ByteOffset OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(ByteOffset)
		LOG_FUNC_END;

	// Halo...
	//    if(ByteOffset != 0 && ByteOffset->QuadPart == 0x00120800)
	//        _asm int 3

	if (CxbxDebugger::CanReport())
	{
		uint64_t Offset = ~0;
		if (ByteOffset)
			Offset = ByteOffset->QuadPart;

		CxbxDebugger::ReportFileRead(FileHandle, Length, Offset);
	}

	// TODO: This may need removal and use actual driver for Chihiro.
	//       Check if there's another branch has chihiro support somewhere? I think nope.
	// If we are emulating the Chihiro, we need to hook mbcom
	if (g_bIsChihiro && FileHandle == CHIHIRO_MBCOM_HANDLE) {
		g_MediaBoard->ComRead(ByteOffset->QuadPart, Buffer, Length);

		// Update the Status Block
		IoStatusBlock->Status = STATUS_SUCCESS;
		IoStatusBlock->Information = Length;
		return STATUS_SUCCESS;
	}

	PFILE_OBJECT FileObject;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObject));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

#ifdef ENABLE_DRIVER_FILESYSTEM // TODO: Implement file system's driver support then enable read/write access check.
	if (!FileObject->ReadAccess) {
		ObfDereferenceObject(FileObject);
		RETURN(X_STATUS_ACCESS_DENIED);
	}
#endif

	if (ApcRoutine != nullptr) {
		// Pack the original parameters to a wrapped context for a custom APC routine
		CxbxIoDispatcherContext* cxbxContext = new CxbxIoDispatcherContext(IoStatusBlock, ApcRoutine, ApcContext);
		ApcRoutine = CxbxIoApcDispatcher;
		ApcContext = cxbxContext;
	}

	// TODO: Start irp work here...

	if (const auto& nFileHandle = GetObjectNativeHandle(FileObject)) {
		result = NtDll::NtReadFile(
			*nFileHandle,
			Event,
			ApcRoutine,
			ApcContext,
			IoStatusBlock,
			Buffer,
			Length,
			(NtDll::LARGE_INTEGER*)ByteOffset,
			/*Key=*/nullptr);

		if (FAILED(result)) {
			EmuLog(LOG_LEVEL::WARNING, "NtReadFile Failed! (0x%.08X)", result);
		}
	}
	else {
		result = X_STATUS_INVALID_PARAMETER;
	}

	ObfDereferenceObject(FileObject);
	RETURN(result);
}

// ******************************************************************
// * 0x00DC - NtReadFileScatter
// ******************************************************************
XBSYSAPI EXPORTNUM(220) xbox::ntstatus_xt NTAPI xbox::NtReadFileScatter
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ulong_xt Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(SegmentArray)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(ByteOffset)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00DD - NtReleaseMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(221) xbox::ntstatus_xt NTAPI xbox::NtReleaseMutant
(
	IN  HANDLE  MutantHandle,
	OUT PLONG   PreviousCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MutantHandle)
		LOG_FUNC_ARG_OUT(PreviousCount)
		LOG_FUNC_END;

	// redirect to NtCreateMutant
	NTSTATUS ret = NtDll::NtReleaseMutant(MutantHandle, (::PLONG)(PreviousCount));

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtReleaseMutant Failed!");

	RETURN(X_STATUS_SUCCESS); // TODO : RETURN(result);
}

// ******************************************************************
// * 0x00DE - NtReleaseSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(222) xbox::ntstatus_xt NTAPI xbox::NtReleaseSemaphore
(
	IN  HANDLE              SemaphoreHandle,
	IN  ulong_xt               ReleaseCount,
	OUT PULONG              PreviousCount OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SemaphoreHandle)
		LOG_FUNC_ARG(ReleaseCount)
		LOG_FUNC_ARG_OUT(PreviousCount)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtReleaseSemaphore(
		SemaphoreHandle, 
		ReleaseCount, 
		(::PULONG)PreviousCount);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtReleaseSemaphore failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00DF - NtRemoveIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(223) xbox::ntstatus_xt NTAPI xbox::NtRemoveIoCompletion
(
	IN HANDLE IoCompletionHandle,
	OUT PVOID *KeyContext,
	OUT PVOID *ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER Timeout OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoCompletionHandle)
		LOG_FUNC_ARG_OUT(KeyContext)
		LOG_FUNC_ARG_OUT(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(Timeout)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E0 - NtResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(224) xbox::ntstatus_xt NTAPI xbox::NtResumeThread
(
	IN  HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	PETHREAD Thread;
	ntstatus_xt result = ObReferenceObjectByHandle(ThreadHandle, &PsThreadObjectType, reinterpret_cast<PVOID *>(&Thread));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	ulong_xt PrevSuspendCount = KeResumeThread(&Thread->Tcb);
	ObfDereferenceObject(Thread);

	if (PreviousSuspendCount) {
		*PreviousSuspendCount = PrevSuspendCount;
	}

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E1 - NtSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(225) xbox::ntstatus_xt NTAPI xbox::NtSetEvent
(
	IN  HANDLE EventHandle,
	OUT PLONG  PreviousState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtSetEvent(
		EventHandle, 
		(::PLONG)(PreviousState));

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtSetEvent Failed!");

	RETURN(ret);
}

xbox::ntstatus_xt IopOpenLinkOrRenameTarget(
	OUT xbox::PHANDLE Handle,
	IN xbox::PIRP Irp,
	IN xbox::FILE_RENAME_INFORMATION* RenameInfo,
	IN xbox::PFILE_OBJECT FileObject,
	OUT xbox::PFILE_OBJECT* FileObjectTarget
)
{
	using namespace xbox;
	ntstatus_xt Status;
	HANDLE TargetHandle;
	OBJECT_STRING FileName;
	PFILE_OBJECT TargetFileObject;
	IO_STATUS_BLOCK IoStatusBlock;
	OBJECT_ATTRIBUTES ObjectAttributes;
	ACCESS_MASK DesiredAccess = FILE_WRITE_DATA;

	//PAGED_CODE();

#if 0
	/* First, establish whether our target is a directory */
	if (!(FileObject->Flags & FO_DIRECT_DEVICE_OPEN)) {
		FILE_BASIC_INFORMATION BasicInfo;
		Status = IopGetBasicInformationFile(FileObject, &BasicInfo);
		if (!X_NT_SUCCESS(Status)) {
			return Status;
		}

		if (BasicInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			DesiredAccess = FILE_ADD_SUBDIRECTORY;
		}
	}
#endif

	/* Setup the string to the target */
	FileName = RenameInfo->FileName;

	X_InitializeObjectAttributes(&ObjectAttributes,&FileName,
		/*(FileObject->Flags & FO_OPENED_CASE_SENSITIVE ? 0 : */ OBJ_CASE_INSENSITIVE /*)*/,
		RenameInfo->RootDirectory);

	/* And open its parent directory */
	Status = IoCreateFile(&TargetHandle,
		DesiredAccess | SYNCHRONIZE,
		&ObjectAttributes,
		&IoStatusBlock,
		NULL,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_OPEN_FOR_BACKUP_INTENT,
		IO_FORCE_ACCESS_CHECK | IO_OPEN_TARGET_DIRECTORY | IO_NO_PARAMETER_CHECKING);

	if (!X_NT_SUCCESS(Status)) {
		return Status;
	}

	/* Once open, continue only if:
	 * Target exists and we're allowed to overwrite it
	 */
#if 0 // TODO: add IRP function then enable below for any remaining fix up.
	PIO_STACK_LOCATION Stack;
	Stack = IoGetNextIrpStackLocation(Irp);
	if (Stack->Parameters.SetFile.FileInformationClass == FileLinkInformation &&
		!RenameInfo->ReplaceIfExists &&
		IoStatusBlock.Information == FILE_EXISTS) {
		ObCloseHandle(TargetHandle);
		return X_STATUS_OBJECT_NAME_COLLISION;
	}
#endif

	/* Now, we'll get the associated device of the target, to check for same device location
	 * So, get the FO first
	 */
	Status = ObReferenceObjectByHandle(TargetHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&TargetFileObject));
	if (!X_NT_SUCCESS(Status)) {
		ObpClose(TargetHandle);
		return Status;
	}

	/* We can dereference, we have the handle */
	ObfDereferenceObject(TargetFileObject);
	/* If we're not on the same device, error out **/
	if (TargetFileObject->DeviceObject != FileObject->DeviceObject)
	{
		ObpClose(TargetHandle);
		return X_STATUS_NOT_SAME_DEVICE;
	}

	/* Return parent directory file object and handle */
#if 0 // TODO: use stack and remove else block.
	Stack->Parameters.SetFile.FileObject = TargetFileObject;
#else
	*FileObjectTarget = TargetFileObject;
#endif
	*Handle = TargetHandle;

	return X_STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00E2 - NtSetInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(226) xbox::ntstatus_xt NTAPI xbox::NtSetInformationFile
(
	IN  HANDLE                   FileHandle,
	OUT PIO_STATUS_BLOCK         IoStatusBlock,
	IN  PVOID                    FileInformation,
	IN  ulong_xt                    Length,
	IN  FILE_INFORMATION_CLASS   FileInformationClass
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_END;

	PFILE_OBJECT FileObjectSource;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObjectSource));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	// TODO: Do irp work here.

	[[maybe_unused]] OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE FileHandleTarget = zeroptr;
	NtDll::PVOID ntFileInfo = nullptr;
	bool isXbox2Nt = false;
	switch (FileInformationClass) {
		case FileRenameInformation: {
			isXbox2Nt = true;
			FILE_RENAME_INFORMATION* xboxRenameInfo = reinterpret_cast<FILE_RENAME_INFORMATION*>(FileInformation);

			PFILE_OBJECT FileObjectTarget;
			result = IopOpenLinkOrRenameTarget(&FileHandleTarget, zeroptr, xboxRenameInfo, FileObjectSource, &FileObjectTarget);

			if (X_NT_SUCCESS(result)) {

				const auto& ParentDirHandle = GetObjectNativeHandle<false>(FileObjectTarget);

				// Get file name only since FileObjectTarget will return parent directory handle.
				auto FileName = PSTRING_to_string(&xboxRenameInfo->FileName);
				if (std::size_t n; (n = FileName.find_last_of("\\")) != std::string::npos) {
					FileName = FileName.substr(n + 1);
				}

				// Build the native FILE_RENAME_INFORMATION struct
				std::wstring convertedFileName = string_to_wstring(FileName);
				Length = sizeof(NtDll::FILE_RENAME_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
				NtDll::FILE_RENAME_INFORMATION* ntRenameInfo = reinterpret_cast<NtDll::FILE_RENAME_INFORMATION*>(ExAllocatePool(Length));
				ntRenameInfo->ReplaceIfExists = xboxRenameInfo->ReplaceIfExists;
				ntRenameInfo->RootDirectory = *ParentDirHandle;
				ntRenameInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
				wmemcpy_s(ntRenameInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());
				ntFileInfo = ntRenameInfo;
			}
			break;
		}
		case FileLinkInformation: {
			// TODO: Is FileLinkInformation used?
			// NOTE: This is untested code as there appear to be no FileLinkInformation usage in dumped kernel.
			LOG_TEST_CASE("FileInformationClass == FileLinkInformation");
			isXbox2Nt = true;
			FILE_LINK_INFORMATION* xboxLinkInfo = reinterpret_cast<FILE_LINK_INFORMATION*>(FileInformation);
			OBJECT_STRING ObjectStringTarget;
			RtlInitAnsiString(&ObjectStringTarget, xboxLinkInfo->FileName);

			// Forward details to rename function looks the same for FileLinkInformation.
			FILE_RENAME_INFORMATION xboxRenameInfo{
			    .ReplaceIfExists = xboxLinkInfo->ReplaceIfExists,
			    .RootDirectory = xboxLinkInfo->RootDirectory,
			    .FileName = ObjectStringTarget
			};

			PFILE_OBJECT FileObjectTarget;
			result = IopOpenLinkOrRenameTarget(&FileHandleTarget, zeroptr, &xboxRenameInfo, FileObjectSource, &FileObjectTarget);

			if (X_NT_SUCCESS(result)) {

				const auto& ParentDirHandle = GetObjectNativeHandle<false>(FileObjectTarget);

				// Get file name only since FileObjectTarget will return parent directory handle.
				auto FileName = PSTRING_to_string(&ObjectStringTarget);
				if (std::size_t n = FileName.find_last_of("\\") != std::string::npos) {
					FileName = FileName.substr(n + 1);
				}

				// Build the native FILE_RENAME_INFORMATION struct
				std::wstring convertedFileName = string_to_wstring(FileName);
				Length = sizeof(NtDll::FILE_RENAME_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
				NtDll::FILE_RENAME_INFORMATION* ntRenameInfo = reinterpret_cast<NtDll::FILE_RENAME_INFORMATION*>(ExAllocatePool(Length));
				ntRenameInfo->ReplaceIfExists = xboxLinkInfo->ReplaceIfExists;
				ntRenameInfo->RootDirectory = *ParentDirHandle;
				ntRenameInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
				wmemcpy_s(ntRenameInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());
				ntFileInfo = ntRenameInfo;
			}
			break;
		}
		default: {
			// The following classes of file information structs are identical between platforms:
			//   FileBasicInformation
			//   FileDispositionInformation
			//   FileEndOfFileInformation
			//   FileLinkInformation
			//   FilePositionInformation
			ntFileInfo = FileInformation;
			break;
		}
	}

	if (X_NT_SUCCESS(result)) {
		const auto& ntHandleSource = GetObjectNativeHandle<false>(FileObjectSource);
		result = NtDll::NtSetInformationFile(
			*ntHandleSource,
			IoStatusBlock,
			ntFileInfo,
			Length,
			FileInformationClass);

		if (FileHandleTarget) {
			NtClose(FileHandleTarget);
		}
	}

	if (isXbox2Nt && ntFileInfo) {
		ExFreePool(ntFileInfo);
	}

	ObfDereferenceObject(FileObjectSource);

	RETURN(result);
}

// ******************************************************************
// * 0x00E3 - NtSetIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(227) xbox::ntstatus_xt NTAPI xbox::NtSetIoCompletion
(
	IN HANDLE IoCompletionHandle,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN ntstatus_xt IoStatus,
	IN ulong_ptr_xt IoStatusInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(IoCompletionHandle)
		LOG_FUNC_ARG(KeyContext)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG(IoStatus)
		LOG_FUNC_ARG(IoStatusInformation)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E4 - NtSetSystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(228) xbox::ntstatus_xt NTAPI xbox::NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SystemTime)
		LOG_FUNC_ARG_OUT(PreviousTime)
	LOG_FUNC_END;

	NTSTATUS ret = X_STATUS_SUCCESS;
	LARGE_INTEGER NewSystemTime, OldSystemTime;
	// LARGE_INTEGER LocalTime;
	// TIME_FIELDS TimeFields;

	if (SystemTime == nullptr) {
		ret = STATUS_ACCESS_VIOLATION;
	}
	else {
		RtlEnterCriticalSectionAndRegion(&NtSystemTimeCritSec);
		NewSystemTime = *SystemTime;
		if (NewSystemTime.u.HighPart > 0 && NewSystemTime.u.HighPart <= 0x20000000) {
			/* Convert the time and set it in HAL */
			// NOTE: disabled, as this requires emulating the RTC, which we don't yet
			// ExSystemTimeToLocalTime(&NewSystemTime, &LocalTime);
			// RtlTimeToTimeFields(&LocalTime, &TimeFields);
			// HalSetRealTimeClock(&TimeFields);

			/* Now set system time */
			KeSetSystemTime(&NewSystemTime, &OldSystemTime);

			// Is the previous time requested?
			if (PreviousTime != nullptr) {
				PreviousTime->QuadPart = OldSystemTime.QuadPart;
			}
		}
		else {
			ret = STATUS_INVALID_PARAMETER;
		}
		RtlLeaveCriticalSectionAndRegion(&NtSystemTimeCritSec);
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00E5 - NtSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(229) xbox::ntstatus_xt NTAPI xbox::NtSetTimerEx
(
	IN HANDLE TimerHandle,
	IN PLARGE_INTEGER DueTime,
	IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode,
	IN PVOID TimerContext OPTIONAL,
	IN boolean_xt WakeTimer,
	IN long_xt Period OPTIONAL,
	OUT PBOOLEAN PreviousState OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(TimerHandle)
		LOG_FUNC_ARG(DueTime)
		LOG_FUNC_ARG(TimerApcRoutine)
		LOG_FUNC_ARG(ApcMode)
		LOG_FUNC_ARG(TimerContext)
		LOG_FUNC_ARG(WakeTimer)
		LOG_FUNC_ARG(Period)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	// redirect to Windows NT
	// TODO : Untested
	NTSTATUS ret = NtDll::NtSetTimer(
		TimerHandle,
		(NtDll::PLARGE_INTEGER)DueTime,
		(NtDll::PTIMER_APC_ROUTINE)TimerApcRoutine,
		(NtDll::PVOID)TimerContext,
		WakeTimer,
		Period,
		/*OUT*/PreviousState);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtSetTimerEx failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00E6 - NtSignalAndWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(230) xbox::ntstatus_xt NTAPI xbox::NtSignalAndWaitForSingleObjectEx
(
	IN HANDLE SignalHandle,
	IN HANDLE WaitHandle,
	IN KPROCESSOR_MODE WaitMode,
	IN boolean_xt Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SignalHandle)
		LOG_FUNC_ARG(WaitHandle)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E7 - NtSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(231) xbox::ntstatus_xt NTAPI xbox::NtSuspendThread
(
	IN  HANDLE  ThreadHandle,
	OUT PULONG  PreviousSuspendCount OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	PETHREAD Thread;
	ntstatus_xt result = ObReferenceObjectByHandle(ThreadHandle, &PsThreadObjectType, reinterpret_cast<PVOID *>(&Thread));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

	if (Thread != PspGetCurrentThread()) {
		if (Thread->Tcb.HasTerminated) {
			ObfDereferenceObject(Thread);
			RETURN(X_STATUS_THREAD_IS_TERMINATING);
		}
	}

	ulong_xt PrevSuspendCount = KeSuspendThread(&Thread->Tcb);
	ObfDereferenceObject(Thread);
	if (PrevSuspendCount == X_STATUS_SUSPEND_COUNT_EXCEEDED) {
		RETURN(X_STATUS_SUSPEND_COUNT_EXCEEDED);
	}

	if (PreviousSuspendCount) {
		*PreviousSuspendCount = PrevSuspendCount;
	}

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E8 - NtUserIoApcDispatcher()
// ******************************************************************
XBSYSAPI EXPORTNUM(232) xbox::void_xt NTAPI xbox::NtUserIoApcDispatcher
(
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	ulong_xt            Reserved
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG(IoStatusBlock)
		LOG_FUNC_ARG(Reserved)
		LOG_FUNC_END;

	ULONG dwErrorCode = 0;
	ULONG dwTransferred = 0;

	if (X_NT_SUCCESS(IoStatusBlock->Status)) {
		dwTransferred = (ULONG)IoStatusBlock->Information;
	} else {
		dwErrorCode = RtlNtStatusToDosError(IoStatusBlock->Status);
	}

	LPOVERLAPPED_COMPLETION_ROUTINE CompletionRoutine = reinterpret_cast<LPOVERLAPPED_COMPLETION_ROUTINE>(ApcContext);
	LPOVERLAPPED lpOverlapped = reinterpret_cast<LPOVERLAPPED>(IoStatusBlock);

	(CompletionRoutine)(dwErrorCode, dwTransferred, lpOverlapped);

	EmuLog(LOG_LEVEL::DEBUG, "NtUserIoApcDispatcher Completed");
}

// ******************************************************************
// * 0x00E9 - NtWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(233) xbox::ntstatus_xt NTAPI xbox::NtWaitForSingleObject
(
    IN  HANDLE  Handle,
    IN  boolean_xt Alertable,
    IN  PLARGE_INTEGER   Timeout
)
{
	LOG_FORWARD("NtWaitForMultipleObjectsEx");

	return xbox::NtWaitForMultipleObjectsEx(
		/*Count=*/1,
		&Handle,
		/*WaitType=*/WaitAll,
		/*WaitMode=*/KernelMode,
		Alertable,
		Timeout
	);
}

// ******************************************************************
// * 0x00EA - NtWaitForSingleObjectEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(234) xbox::ntstatus_xt NTAPI xbox::NtWaitForSingleObjectEx
(
	IN  HANDLE          Handle,
	IN  KPROCESSOR_MODE WaitMode,
	IN  boolean_xt         Alertable,
	IN  PLARGE_INTEGER  Timeout
)
{
	LOG_FORWARD("NtWaitForMultipleObjectsEx");

	return xbox::NtWaitForMultipleObjectsEx(
		/*Count=*/1,
		&Handle,
		/*WaitType=*/WaitAll,
		WaitMode,
		Alertable,
		Timeout
	);
}

// ******************************************************************
// * 0x00EB - NtWaitForMultipleObjectsEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(235) xbox::ntstatus_xt NTAPI xbox::NtWaitForMultipleObjectsEx
(
	IN  ulong_xt           Count,
	IN  HANDLE         *Handles,
	IN  WAIT_TYPE       WaitType,
	IN  char_xt            WaitMode,
	IN  boolean_xt         Alertable,
	IN  PLARGE_INTEGER  Timeout
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Handles)
		LOG_FUNC_ARG(WaitType)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_END;

	if (!Count || (Count > X_MAXIMUM_WAIT_OBJECTS)) {
		RETURN(X_STATUS_INVALID_PARAMETER);
	}

	// This function can wait on thread handles, which are currently created by ob,
	// so we need to check their presence in the handle array
	::HANDLE nativeHandles[X_MAXIMUM_WAIT_OBJECTS];
	for (ulong_xt i = 0; i < Count; ++i) {
		if (const auto &nativeHandle = GetNativeHandle(Handles[i])) {
			// This is a ob handle, so replace it with its native counterpart
			nativeHandles[i] = *nativeHandle;
			EmuLog(LOG_LEVEL::DEBUG, "xbox handle: %p", nativeHandles[i]);
		}
		else {
			nativeHandles[i] = Handles[i];
			EmuLog(LOG_LEVEL::DEBUG, "native handle: %p", nativeHandles[i]);
		}
	}

	// Because user APCs from NtQueueApcThread are now handled by the kernel, we need to wait for them ourselves

	PKTHREAD kThread = KeGetCurrentThread();
	kThread->WaitStatus = X_STATUS_SUCCESS;
	if (!AddWaitObject(kThread, Timeout)) {
		RETURN(X_STATUS_TIMEOUT);
	}

	xbox::ntstatus_xt ret = WaitApc<true>([Count, &nativeHandles, WaitType, Alertable](xbox::PKTHREAD kThread) -> std::optional<ntstatus_xt> {
		NtDll::LARGE_INTEGER ExpireTime;
		ExpireTime.QuadPart = 0;
		NTSTATUS Status = NtDll::NtWaitForMultipleObjects(
			Count,
			nativeHandles,
			(NtDll::OBJECT_WAIT_TYPE)WaitType,
			Alertable,
			&ExpireTime);
		if (Status == STATUS_TIMEOUT) {
			return std::nullopt;
		}
		// If the wait was satisfied with the host, then also unwait the thread on the guest side, to be sure to remove WaitBlocks that might have been added
		// to the thread. Test case: Steel Battalion
		xbox::KiUnwaitThreadAndLock(kThread, Status, 0);
		return std::make_optional<ntstatus_xt>(kThread->WaitStatus);
		}, Timeout, Alertable, WaitMode, kThread);

	RETURN(ret);
}

// ******************************************************************
// * 0x00EC - NtWriteFile()
// ******************************************************************
// Writes a file.
//
// Differences from NT: There is no Key parameter.
XBSYSAPI EXPORTNUM(236) xbox::ntstatus_xt NTAPI xbox::NtWriteFile
(
	IN  HANDLE          FileHandle,
	IN  HANDLE          Event,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN  PVOID           Buffer,
	IN  ulong_xt           Length,
	IN  PLARGE_INTEGER  ByteOffset OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(ByteOffset)
		LOG_FUNC_END;

	// Halo..
	//    if(ByteOffset != 0 && ByteOffset->QuadPart == 0x01C00800)
	//        _asm int 3

	if (CxbxDebugger::CanReport())
	{
		uint64_t Offset = ~0;
		if (ByteOffset)
			Offset = ByteOffset->QuadPart;
		
		CxbxDebugger::ReportFileWrite(FileHandle, Length, Offset);
	}

	// TODO: This may need removal and use actual driver for Chihiro.
	//       Check if there's another branch has chihiro support somewhere? I think nope.
	// If we are emulating the Chihiro, we need to hook mbcom
	if (g_bIsChihiro && FileHandle == CHIHIRO_MBCOM_HANDLE) {
		g_MediaBoard->ComWrite(ByteOffset->QuadPart, Buffer, Length);

		// Update the Status Block
		IoStatusBlock->Status = STATUS_SUCCESS;
		IoStatusBlock->Information = Length;
		return STATUS_SUCCESS;
	}

	PFILE_OBJECT FileObject;
	ntstatus_xt result = ObReferenceObjectByHandle(FileHandle, &IoFileObjectType, reinterpret_cast<PVOID*>(&FileObject));
	if (!X_NT_SUCCESS(result)) {
		RETURN(result);
	}

#ifdef ENABLE_DRIVER_FILESYSTEM // TODO: Implement file system's driver support then enable read/write access check.
	if (!FileObject->WriteAccess) {
		ObfDereferenceObject(FileObject);
		RETURN(X_STATUS_ACCESS_DENIED);
	}
#endif

	if (ApcRoutine != nullptr) {
		// Pack the original parameters to a wrapped context for a custom APC routine
		CxbxIoDispatcherContext* cxbxContext = new CxbxIoDispatcherContext(IoStatusBlock, ApcRoutine, ApcContext);
		ApcRoutine = CxbxIoApcDispatcher;
		ApcContext = cxbxContext;
	}

	// TODO: Do irp work here...

	if (const auto& nFileHandle = GetObjectNativeHandle(FileObject)) {
		result = NtDll::NtWriteFile(
			*nFileHandle,
			Event,
			ApcRoutine,
			ApcContext,
			IoStatusBlock,
			Buffer,
			Length,
			(NtDll::LARGE_INTEGER*)ByteOffset,
			/*Key=*/nullptr);

		if (FAILED(result))
			EmuLog(LOG_LEVEL::WARNING, "NtWriteFile Failed! (0x%.08X)", result);
	}
	else {
		result = X_STATUS_INVALID_PARAMETER;
	}

	ObfDereferenceObject(FileObject);
	RETURN(result);
}

// ******************************************************************
// * 0x00ED - NtWriteFileGather
// ******************************************************************
XBSYSAPI EXPORTNUM(237) xbox::ntstatus_xt NTAPI xbox::NtWriteFileGather
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ulong_xt Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(SegmentArray)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(ByteOffset)
	LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(X_STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00EE - NtYieldExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(238) xbox::void_xt NTAPI xbox::NtYieldExecution()
{
	// NOTE: Logging this fills the debug log far too quickly, so don't.
	// LOG_FUNC();

	NtDll::NtYieldExecution();
}

