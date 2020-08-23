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


#include <xboxkrnl/xboxkrnl.h> // For NtAllocateVirtualMemory, etc.
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "core\kernel\support\EmuNtDll.h"
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup
#include "core\kernel\exports\EmuKrnlKe.h"
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "core\kernel\support\EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.
#include "core\kernel\memory-manager\VMManager.h" // For g_VMManager
#include "CxbxDebugger.h"

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include <assert.h>

#include <unordered_map>
#include <mutex>

// Used to keep track of duplicate handles created by NtQueueApcThread()
std::unordered_map<HANDLE, HANDLE>	g_DuplicateHandles;
// Prevent setting the system time from multiple threads at the same time
std::mutex NtSystemTimeMtx;


// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(184) xbox::NTSTATUS NTAPI xbox::NtAllocateVirtualMemory
(
	IN OUT PVOID    *BaseAddress,
	IN ULONG         ZeroBits,
	IN OUT PULONG    AllocationSize,
	IN DWORD         AllocationType,
	IN DWORD         Protect
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
XBSYSAPI EXPORTNUM(185) xbox::NTSTATUS NTAPI xbox::NtCancelTimer
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
XBSYSAPI EXPORTNUM(186) xbox::NTSTATUS NTAPI xbox::NtClearEvent
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
XBSYSAPI EXPORTNUM(187) xbox::NTSTATUS NTAPI xbox::NtClose
(
	IN HANDLE Handle
)
{
	LOG_FUNC_ONE_ARG(Handle);

	NTSTATUS ret = STATUS_SUCCESS;

	if (IsEmuHandle(Handle))
	{
		// delete 'special' handles
		EmuHandle *iEmuHandle = HandleToEmuHandle(Handle);
		ret = iEmuHandle->NtClose();

		LOG_UNIMPLEMENTED(); // TODO : Base this on the Ob* functions
	}
    else
    {
        if (CxbxDebugger::CanReport())
        {
            CxbxDebugger::ReportFileClosed(Handle);
        }

		// Prevent exceptions when using invalid NTHandle
		DWORD flags = 0;
		if (GetHandleInformation(Handle, &flags) != 0) {
			ret = NtDll::NtClose(Handle);

			// Delete duplicate threads created by our implementation of NtQueueApcThread()
			if( GetHandleInformation( g_DuplicateHandles[Handle], &flags ) != 0 )
			{
				EmuLog(LOG_LEVEL::DEBUG, "Closing duplicate handle..." );

				CloseHandle( g_DuplicateHandles[Handle] );
				g_DuplicateHandles.erase(Handle);
			}
		}
    }

	RETURN(ret);
}

// ******************************************************************
// * 0x00BC - NtCreateDirectoryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(188) xbox::NTSTATUS NTAPI xbox::NtCreateDirectoryObject
(
	OUT PHANDLE             DirectoryHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(DirectoryHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_END;

	NativeObjectAttributes nativeObjectAttributes;
	NTSTATUS ret = CxbxObjectAttributesToNT(
		ObjectAttributes, 
		nativeObjectAttributes,
		"NtCreateDirectoryObject");

	if (ret == STATUS_SUCCESS)
	{
		// TODO : Is this the correct ACCESS_MASK? :
		const ACCESS_MASK DesiredAccess = DIRECTORY_CREATE_OBJECT;

		ret = NtDll::NtCreateDirectoryObject(
			/*OUT*/DirectoryHandle,
			DesiredAccess,
			nativeObjectAttributes.NtObjAttrPtr);
	}

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtCreateDirectoryObject Failed!");
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateDirectoryObject DirectoryHandle = 0x%.8X", *DirectoryHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00BD - NtCreateEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(189) xbox::NTSTATUS NTAPI xbox::NtCreateEvent
(
	OUT PHANDLE             EventHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
	IN  EVENT_TYPE          EventType,
	IN  BOOLEAN             InitialState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(EventHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG(EventType)
		LOG_FUNC_ARG(InitialState)
		LOG_FUNC_END;

/*
	NTSTATUS Status;

	if ((EventType != NotificationEvent) && (EventType != SynchronizationEvent)) {
		Status = STATUS_INVALID_PARAMETER;
	}
	else {
		PKEVENT Event;

		Status = ObCreateObject(&ExEventObjectType, ObjectAttributes, sizeof(KEVENT), (PVOID *)&Event);
		if (NT_SUCCESS(Status)) {
			KeInitializeEvent(Event, EventType, InitialState);
			Status = ObInsertObject(Event, ObjectAttributes, 0, EventHandle);
		}
	}

	RETURN(Status);
*/
	LOG_INCOMPLETE(); // TODO : Verify arguments, use ObCreateObject, KeInitializeEvent and ObInsertObject instead of this:

	// initialize object attributes
	NativeObjectAttributes nativeObjectAttributes;
	CxbxObjectAttributesToNT(ObjectAttributes, /*var*/nativeObjectAttributes);

	// TODO : Is this the correct ACCESS_MASK? :
	const ACCESS_MASK DesiredAccess = EVENT_ALL_ACCESS;

	// redirect to Win2k/XP
	NTSTATUS ret = NtDll::NtCreateEvent(
		/*OUT*/EventHandle,
		DesiredAccess,
		nativeObjectAttributes.NtObjAttrPtr,
		(NtDll::EVENT_TYPE)EventType,
		InitialState);

	// TODO : Instead of the above, we should consider using the Ke*Event APIs, but
	// that would require us to create the event's kernel object with the Ob* api's too!

	if (FAILED(ret))
	{
		EmuLog(LOG_LEVEL::WARNING, "Trying fallback (without object attributes)...\nError code 0x%X", ret);

		// If it fails, try again but without the object attributes stucture
		// This fixes Panzer Dragoon games on non-Vista OSes.
		ret = NtDll::NtCreateEvent(
			/*OUT*/EventHandle,
			DesiredAccess,
			/*nativeObjectAttributes.NtObjAttrPtr*/ NULL,
			(NtDll::EVENT_TYPE)EventType,
			InitialState);

		if(FAILED(ret))
			EmuLog(LOG_LEVEL::WARNING, "NtCreateEvent Failed!");
		else
			EmuLog(LOG_LEVEL::DEBUG, "NtCreateEvent EventHandle = 0x%.8X", *EventHandle);
	}
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtCreateEvent EventHandle = 0x%.8X", *EventHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00BE - NtCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(190) xbox::NTSTATUS NTAPI xbox::NtCreateFile
(
	OUT PHANDLE             FileHandle,
	IN  ACCESS_MASK         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
	IN  ULONG               FileAttributes,
	IN  ULONG               ShareAccess,
	IN  ULONG               CreateDisposition,
	IN  ULONG               CreateOptions
)
{
	LOG_FORWARD("IoCreateFile");

	// TODO : How to base IoCreateFile on ObCreateObject, KeInitialize and ObInsertObject ?

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
XBSYSAPI EXPORTNUM(191) xbox::NTSTATUS NTAPI xbox::NtCreateIoCompletion
(
	OUT PHANDLE IoCompletionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN ULONG Count
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
XBSYSAPI EXPORTNUM(192) xbox::NTSTATUS NTAPI xbox::NtCreateMutant
(
	OUT PHANDLE             MutantHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	IN  BOOLEAN             InitialOwner
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
		if (NT_SUCCESS(Status)) {
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
XBSYSAPI EXPORTNUM(193) xbox::NTSTATUS NTAPI xbox::NtCreateSemaphore
(
	OUT PHANDLE             SemaphoreHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
	IN  ULONG               InitialCount,
	IN  ULONG               MaximumCount
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
		if (NT_SUCCESS(Status)) {
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
XBSYSAPI EXPORTNUM(194) xbox::NTSTATUS NTAPI xbox::NtCreateTimer
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
		if (NT_SUCCESS(Status)) {
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
XBSYSAPI EXPORTNUM(195) xbox::NTSTATUS NTAPI xbox::NtDeleteFile
(
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	LOG_FUNC_ONE_ARG(ObjectAttributes);

	NativeObjectAttributes nativeObjectAttributes;
	NTSTATUS ret = CxbxObjectAttributesToNT(
		ObjectAttributes,
		nativeObjectAttributes,
		"NtDeleteFile");

	if (ret == STATUS_SUCCESS)
	{
		ret = NtDll::NtDeleteFile(
			nativeObjectAttributes.NtObjAttrPtr);
	}

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtDeleteFile Failed!");
	
	RETURN(ret);
}

// ******************************************************************
// * 0x00C4 - NtDeviceIoControlFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(196) xbox::NTSTATUS NTAPI xbox::NtDeviceIoControlFile
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength
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

	NTSTATUS ret = STATUS_SUCCESS;

	switch (IoControlCode)
	{
	case 0x4D014: // IOCTL_SCSI_PASS_THROUGH_DIRECT
	{
		PSCSI_PASS_THROUGH_DIRECT PassThrough = (PSCSI_PASS_THROUGH_DIRECT)InputBuffer;
		PDVDX2_AUTHENTICATION Authentication = (PDVDX2_AUTHENTICATION)PassThrough->DataBuffer;

		// Should be just enough info to pass XapiVerifyMediaInDrive
		Authentication->AuthenticationPage.CDFValid = 1;
		Authentication->AuthenticationPage.PartitionArea = 1;
		Authentication->AuthenticationPage.Authentication = 1;
		break;
	}
	case 0x70000: // IOCTL_DISK_GET_DRIVE_GEOMETRY
	{
		PDISK_GEOMETRY DiskGeometry = (PDISK_GEOMETRY)OutputBuffer;

		DiskGeometry->MediaType = FixedMedia;
		DiskGeometry->TracksPerCylinder = 1;
		DiskGeometry->SectorsPerTrack = 1;
		DiskGeometry->BytesPerSector = 512;
		DiskGeometry->Cylinders.QuadPart = 0x1400000;	// Around 10GB, size of stock xbox HDD
		break;
	}
	case 0x74004: // IOCTL_DISK_GET_PARTITION_INFO 
	{
		PPARTITION_INFORMATION partitioninfo = (PPARTITION_INFORMATION)OutputBuffer;

		XboxPartitionTable partitionTable = CxbxGetPartitionTable();
		int partitionNumber = CxbxGetPartitionNumberFromHandle(FileHandle);
		
		// Now we read from the partition table, to fill in the partitionInfo struct
		partitioninfo->PartitionNumber = partitionNumber;
		partitioninfo->StartingOffset.QuadPart = partitionTable.TableEntries[partitionNumber - 1].LBAStart * 512;
		partitioninfo->PartitionLength.QuadPart = partitionTable.TableEntries[partitionNumber - 1].LBASize * 512;
		partitioninfo->HiddenSectors = partitionTable.TableEntries[partitionNumber - 1].Reserved;
		partitioninfo->RecognizedPartition = true;
		break;
	}
	default:
		LOG_UNIMPLEMENTED();
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00C5 - NtDuplicateObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(197) xbox::NTSTATUS NTAPI xbox::NtDuplicateObject
(
	HANDLE  SourceHandle,
	HANDLE *TargetHandle,
	DWORD   Options
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SourceHandle)
		LOG_FUNC_ARG(TargetHandle)
		LOG_FUNC_ARG(Options)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_SUCCESS;

	if (IsEmuHandle(SourceHandle)) {
		EmuHandle* iEmuHandle = HandleToEmuHandle(SourceHandle);
		ret = iEmuHandle->NtDuplicateObject(TargetHandle, Options);
/*
		PVOID Object;

		ret = ObReferenceObjectByHandle(SourceHandle, /*ObjectType=* /NULL, &Object);
		if (NT_SUCCESS(ret)) {
			if (ObpIsFlagSet(Options, DUPLICATE_CLOSE_SOURCE))
				NtClose(SourceHandle);

			status = ObOpenObjectByPointer(Object, OBJECT_TO_OBJECT_HEADER(Object)->Type, /*OUT* /TargetHandle);
			ObDereferenceObject(Object);
		}
		else
			*TargetHandle = NULL;
*/
	}
	else
	{
		// TODO : What arguments should we use?
		const ACCESS_MASK DesiredAccess = 0;
		const ULONG Attributes = 0;

		// redirect to Win2k/XP
		ret = NtDll::NtDuplicateObject(
			/*SourceProcessHandle=*/g_CurrentProcessHandle,
			SourceHandle,
			/*TargetProcessHandle=*/g_CurrentProcessHandle,
			TargetHandle,
			DesiredAccess,
			Attributes,
			Options);
	}

	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "Object was not duplicated!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00C6 - NtFlushBuffersFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(198) xbox::NTSTATUS NTAPI xbox::NtFlushBuffersFile
(
	PVOID                FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_END;
	NTSTATUS ret = STATUS_SUCCESS;
	
	if (IsEmuHandle(FileHandle)) 
		LOG_UNIMPLEMENTED();
	else
		ret = NtDll::NtFlushBuffersFile(FileHandle, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C7 - NtFreeVirtualMemory()
// ******************************************************************
// Frees virtual memory.
//
// Differences from NT: There is no ProcessHandle parameter.
XBSYSAPI EXPORTNUM(199) xbox::NTSTATUS NTAPI xbox::NtFreeVirtualMemory
(
	IN OUT PVOID *BaseAddress,
	IN OUT PULONG FreeSize,
	IN ULONG      FreeType
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
XBSYSAPI EXPORTNUM(200) xbox::NTSTATUS NTAPI xbox::NtFsControlFile
(
	IN HANDLE               FileHandle,
	IN HANDLE               Event OPTIONAL,
	IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
	IN PVOID                ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN ULONG                FsControlCode,
	IN PVOID                InputBuffer OPTIONAL,
	IN ULONG                InputBufferLength,
	OUT PVOID               OutputBuffer OPTIONAL,
	IN ULONG                OutputBufferLength
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

	NTSTATUS ret = STATUS_INVALID_PARAMETER;

	switch (FsControlCode) {
		case 0x00090020: // FSCTL_DISMOUNT_VOLUME 
			int partitionNumber = CxbxGetPartitionNumberFromHandle(FileHandle);
			if (partitionNumber > 0) {
				CxbxFormatPartitionByHandle(FileHandle);
				ret = STATUS_SUCCESS;
			}
			break;
	}

	LOG_UNIMPLEMENTED();
	RETURN(ret);
}

// ******************************************************************
// * 0x00C9 - NtOpenDirectoryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(201) xbox::NTSTATUS NTAPI xbox::NtOpenDirectoryObject
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
XBSYSAPI EXPORTNUM(202) xbox::NTSTATUS NTAPI xbox::NtOpenFile
(
	OUT PHANDLE             FileHandle,
	IN  ACCESS_MASK         DesiredAccess,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN  ULONG               ShareAccess,
	IN  ULONG               OpenOptions
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
XBSYSAPI EXPORTNUM(203) xbox::NTSTATUS NTAPI xbox::NtOpenSymbolicLinkObject
(
	OUT PHANDLE LinkHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
)
{
	/* TODO :
	LOG_FORWARD("ObOpenObjectByName");

	return ObOpenObjectByName(ObjectAttributes, &ObSymbolicLinkObjectType, NULL, LinkHandle);
	*/
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(LinkHandle)
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_OBJECT_PATH_NOT_FOUND;
	EmuNtSymbolicLinkObject* symbolicLinkObject =
		FindNtSymbolicLinkObjectByName(PSTRING_to_string(ObjectAttributes->ObjectName));

	if (symbolicLinkObject != NULL)
	{
		// Return a new handle (which is an EmuHandle, actually) :
		*LinkHandle = symbolicLinkObject->NewHandle();
		ret = STATUS_SUCCESS;
	}

	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtOpenSymbolicLinkObject failed! (%s)", NtStatusToString(ret));
	else
		EmuLog(LOG_LEVEL::DEBUG, "NtOpenSymbolicLinkObject LinkHandle^ = 0x%.8X", *LinkHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00CC - NtProtectVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(204) xbox::NTSTATUS NTAPI xbox::NtProtectVirtualMemory
(
	IN OUT PVOID *BaseAddress,
	IN OUT PSIZE_T RegionSize,
	IN ULONG NewProtect,
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
XBSYSAPI EXPORTNUM(205) xbox::NTSTATUS NTAPI xbox::NtPulseEvent
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
		/*OUT*/PreviousState);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtPulseEvent failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00CE - NtQueueApcThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(206) xbox::NTSTATUS NTAPI xbox::NtQueueApcThread
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN ULONG                ApcReserved OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG(ApcRoutine)
		LOG_FUNC_ARG(ApcRoutineContext)
		LOG_FUNC_ARG(ApcStatusBlock)
		LOG_FUNC_ARG(ApcReserved)
		LOG_FUNC_END;

	// In order for NtQueueApcThread or QueueUserAPC to work, you must... I repeat...
	// YOU MUST duplicate the handle with the appropriate permissions first!  So far,
	// the only game that I know of using this is Metal Slug 3, and it won't launch
	// without it.  Other SNK games might use it also, beware.

	// TODO: Use our implementation of NtDuplicateObject instead? 

	HANDLE hApcThread = NULL;

	// Just to be safe, let's see if the appropriate permissions are even set for the
	// target thread first...

	NTSTATUS ret = NtDll::NtQueueApcThread(
		(NtDll::HANDLE)ThreadHandle,
		(NtDll::PIO_APC_ROUTINE)ApcRoutine,
		ApcRoutineContext,
		(NtDll::PIO_STATUS_BLOCK)ApcStatusBlock,
		ApcReserved);

	if( FAILED( ret ) )
	{
		EmuLog(LOG_LEVEL::WARNING,  "Duplicating handle with THREAD_SET_CONTEXT..." );

		// If we get here, then attempt to duplicate the thread.
		if(!DuplicateHandle(g_CurrentProcessHandle, ThreadHandle, g_CurrentProcessHandle, &hApcThread, THREAD_SET_CONTEXT,FALSE,0))
			EmuLog(LOG_LEVEL::WARNING, "DuplicateHandle failed!");
		else
		{
			g_DuplicateHandles[ThreadHandle] = hApcThread;	// Save this thread because we'll need to de-reference it later
			EmuLog(LOG_LEVEL::DEBUG, "DuplicateHandle returned 0x%X (ThreadId 0x%.4X)", hApcThread, GetThreadId( hApcThread ) );
		}


		ret = NtDll::NtQueueApcThread(
			(NtDll::HANDLE)hApcThread,
			(NtDll::PIO_APC_ROUTINE)ApcRoutine,
			ApcRoutineContext,
			(NtDll::PIO_STATUS_BLOCK)ApcStatusBlock,
			ApcReserved);
	}
	if (FAILED(ret))
	{
		EmuLog(LOG_LEVEL::WARNING, "NtQueueApcThread failed!");
		CloseHandle( g_DuplicateHandles[ThreadHandle] );
		g_DuplicateHandles.erase( ThreadHandle );
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00CF - NtQueryDirectoryFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(207) xbox::NTSTATUS NTAPI xbox::NtQueryDirectoryFile
(
	IN  HANDLE                      FileHandle,
	IN  HANDLE                      Event OPTIONAL,
	IN  PVOID                       ApcRoutine, // Todo: define this routine's prototype
	IN  PVOID                       ApcContext,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT FILE_DIRECTORY_INFORMATION *FileInformation,
	IN  ULONG                       Length,
	IN  FILE_INFORMATION_CLASS      FileInformationClass,
	IN  PSTRING                     FileMask,
	IN  BOOLEAN                     RestartScan
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
		CxbxKrnlCleanup("Unsupported FileInformationClass");

	NtDll::UNICODE_STRING NtFileMask;

	wchar_t wszObjectName[MAX_PATH];

	// initialize FileMask
	{
		if (FileMask != 0) {
			// Xbox expects directories to be listed when *.* is passed
			if (strncmp(FileMask->Buffer, "*.*", FileMask->Length) == 0) {
				FileMask->Length = 1;
				FileMask->Buffer = "*";
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
			FileHandle, 
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

	RETURN(ret);
}

// ******************************************************************
// * 0x00D0 - NtQueryDirectoryObject
// ******************************************************************
XBSYSAPI EXPORTNUM(208) xbox::NTSTATUS NTAPI xbox::NtQueryDirectoryObject
(
	IN HANDLE DirectoryHandle,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN BOOLEAN RestartScan,
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00D1  - NtQueryEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(209) xbox::NTSTATUS NTAPI xbox::NtQueryEvent
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

	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQueryEvent failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D2 - NtQueryFullAttributesFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(210) xbox::NTSTATUS NTAPI xbox::NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT xbox::PFILE_NETWORK_OPEN_INFORMATION   Attributes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG_OUT(Attributes)
		LOG_FUNC_END;

	//	__asm int 3;
	NativeObjectAttributes nativeObjectAttributes;
	NtDll::FILE_NETWORK_OPEN_INFORMATION nativeNetOpenInfo;

	NTSTATUS ret = CxbxObjectAttributesToNT(
		ObjectAttributes,
		/*var*/nativeObjectAttributes,
		"NtQueryFullAttributesFile");

	if (ret == STATUS_SUCCESS)
		ret = NtDll::NtQueryFullAttributesFile(
			nativeObjectAttributes.NtObjAttrPtr,
			&nativeNetOpenInfo);

	// Convert Attributes to Xbox
	NTToXboxFileInformation(&nativeNetOpenInfo, Attributes, FileNetworkOpenInformation, sizeof(xbox::FILE_NETWORK_OPEN_INFORMATION));

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtQueryFullAttributesFile failed! (0x%.08X)", ret);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D3 - NtQueryInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(211) xbox::NTSTATUS NTAPI xbox::NtQueryInformationFile
(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PVOID                       FileInformation,
	IN  ULONG                       Length,
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

	NTSTATUS ret;
	PVOID ntFileInfo;

	// Start with sizeof(corresponding struct)
	size_t bufferSize = XboxFileInfoStructSizes[FileInformationClass];

	// We need to retry the operation in case the buffer is too small to fit the data
	do
	{
		ntFileInfo = malloc(bufferSize);

		ret = NtDll::NtQueryInformationFile(
			FileHandle,
			(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
			ntFileInfo,
			bufferSize,
			(NtDll::FILE_INFORMATION_CLASS)FileInformationClass);

		// Buffer is too small; make a larger one
		if (ret == STATUS_BUFFER_OVERFLOW)
		{
			free(ntFileInfo);

			bufferSize *= 2;
			// Bail out if the buffer gets too big
			if (bufferSize > 65536)
				return STATUS_INVALID_PARAMETER;   // TODO: what's the appropriate error code to return here?
		}
	} while (ret == STATUS_BUFFER_OVERFLOW);
	
	// Convert and copy NT data to the given Xbox struct
	NTSTATUS convRet = NTToXboxFileInformation(ntFileInfo, FileInformation, FileInformationClass, Length);

	// Make sure to free the memory first
	free(ntFileInfo);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtQueryInformationFile failed! (0x%.08X)", ret);

	// Prioritize the buffer overflow over real return code,
	// in case the Xbox program decides to follow the same procedure above
	if (convRet == STATUS_BUFFER_OVERFLOW)
		return convRet;

	RETURN(ret);
}

// ******************************************************************
// * 0x00D4 - NtQueryIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(212) xbox::NTSTATUS NTAPI xbox::NtQueryIoCompletion
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00D5 - NtQueryMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(213) xbox::NTSTATUS NTAPI xbox::NtQueryMutant
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

	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQueryMutant failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D6 - NtQuerySemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(214) xbox::NTSTATUS NTAPI xbox::NtQuerySemaphore
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

	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQuerySemaphore failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D7 - NtQuerySymbolicLinkObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(215) xbox::NTSTATUS NTAPI xbox::NtQuerySymbolicLinkObject
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

	NTSTATUS ret = 0;
	EmuNtSymbolicLinkObject* symbolicLinkObject = NULL;

	// Check that we actually got an EmuHandle :
	ret = STATUS_INVALID_HANDLE;

	EmuHandle* iEmuHandle = HandleToEmuHandle(LinkHandle);
	// Retrieve the NtSymbolicLinkObject and populate the output arguments :
	ret = STATUS_SUCCESS;
	symbolicLinkObject = (EmuNtSymbolicLinkObject*)iEmuHandle->NtObject;

	if (symbolicLinkObject->IsHostBasedPath) {
		// TODO : What should we do with symbolic links 
		ret = STATUS_UNRECOGNIZED_VOLUME;
	} else {
		if (LinkTarget != NULL)
		{
			if (LinkTarget->MaximumLength >= symbolicLinkObject->XboxSymbolicLinkPath.length() + sizeof(char)) {
				copy_string_to_PSTRING_to(symbolicLinkObject->XboxSymbolicLinkPath, LinkTarget);
			}
			else {
				ret = STATUS_BUFFER_TOO_SMALL;
			}
		}

		if (ReturnedLength != NULL)
		{
			*ReturnedLength = symbolicLinkObject->XboxSymbolicLinkPath.length(); // Return full length (even if buffer was too small)
		}
	}
	if (ret != STATUS_SUCCESS)
		EmuLog(LOG_LEVEL::WARNING, "NtQuerySymbolicLinkObject failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D8 - NtQueryTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(216) xbox::NTSTATUS NTAPI xbox::NtQueryTimer
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
XBSYSAPI EXPORTNUM(217) xbox::NTSTATUS NTAPI xbox::NtQueryVirtualMemory
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

	if (ret == STATUS_SUCCESS)
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
	if (FAILED(ret)) {
		EmuLog(LOG_LEVEL::WARNING, "NtQueryVirtualMemory failed (%s)!", NtStatusToString(ret));

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

			ret = STATUS_SUCCESS;

			EmuLog(LOG_LEVEL::DEBUG, "NtQueryVirtualMemory: Applied fix for Forza Motorsport!");
		}
	}
	#endif

	RETURN(ret);
}

// ******************************************************************
// * 0x00DA - NtQueryVolumeInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(218) xbox::NTSTATUS NTAPI xbox::NtQueryVolumeInformationFile
(
	IN  HANDLE                      FileHandle,
	OUT PIO_STATUS_BLOCK            IoStatusBlock,
	OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
	IN  ULONG                       Length,
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

	// FileFsSizeInformation is a special case that should read from our emulated partition table
	if ((DWORD)FileInformationClass == FileFsSizeInformation) {
		PFILE_FS_SIZE_INFORMATION XboxSizeInfo = (PFILE_FS_SIZE_INFORMATION)FileInformation;

		XboxPartitionTable partitionTable = CxbxGetPartitionTable();
		int partitionNumber = CxbxGetPartitionNumberFromHandle(FileHandle);
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

		RETURN(STATUS_SUCCESS);
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

	NTSTATUS ret = NtDll::NtQueryVolumeInformationFile(
		FileHandle,
		(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
		(NtDll::PFILE_FS_SIZE_INFORMATION)NativeFileInformation, HostBufferSize,
		(NtDll::FS_INFORMATION_CLASS)FileInformationClass);

	// Convert Xbox NativeFileInformation to FileInformation
	if (ret == STATUS_SUCCESS) {
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
XBSYSAPI EXPORTNUM(219) xbox::NTSTATUS NTAPI xbox::NtReadFile
(
	IN  HANDLE          FileHandle,            // TODO: correct paramters
	IN  HANDLE          Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID           Buffer,
	IN  ULONG           Length,
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

	NTSTATUS ret = NtDll::NtReadFile(
		FileHandle,
		Event,
		(PVOID)ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		(NtDll::LARGE_INTEGER*)ByteOffset,
		/*Key=*/nullptr);

    if (FAILED(ret)) {
        EmuLog(LOG_LEVEL::WARNING, "NtReadFile Failed! (0x%.08X)", ret);
    }

	RETURN(ret);
}

// ******************************************************************
// * 0x00DC - NtReadFileScatter
// ******************************************************************
XBSYSAPI EXPORTNUM(220) xbox::NTSTATUS NTAPI xbox::NtReadFileScatter
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ULONG Length,
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00DD - NtReleaseMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(221) xbox::NTSTATUS NTAPI xbox::NtReleaseMutant
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
	NTSTATUS ret = NtDll::NtReleaseMutant(MutantHandle, PreviousCount);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtReleaseMutant Failed!");

	RETURN(STATUS_SUCCESS); // TODO : RETURN(ret);
}

// ******************************************************************
// * 0x00DE - NtReleaseSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(222) xbox::NTSTATUS NTAPI xbox::NtReleaseSemaphore
(
	IN  HANDLE              SemaphoreHandle,
	IN  ULONG               ReleaseCount,
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
		PreviousCount);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtReleaseSemaphore failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00DF - NtRemoveIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(223) xbox::NTSTATUS NTAPI xbox::NtRemoveIoCompletion
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E0 - NtResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(224) xbox::NTSTATUS NTAPI xbox::NtResumeThread
(
	IN  HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtResumeThread(
		ThreadHandle, 
		PreviousSuspendCount);

	// TODO : Once we do our own thread-switching, implement NtResumeThread using KetResumeThread

	//Sleep(10);

	RETURN(ret);
}

// ******************************************************************
// * 0x00E1 - NtSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(225) xbox::NTSTATUS NTAPI xbox::NtSetEvent
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
		PreviousState);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtSetEvent Failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00E2 - NtSetInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(226) xbox::NTSTATUS NTAPI xbox::NtSetInformationFile
(
	IN  HANDLE                   FileHandle,
	OUT PIO_STATUS_BLOCK         IoStatusBlock,
	IN  PVOID                    FileInformation,
	IN  ULONG                    Length,
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
	
	XboxToNTFileInformation(convertedFileInfo, FileInformation, FileInformationClass, &Length);

	NTSTATUS ret = NtDll::NtSetInformationFile(
		FileHandle,
		IoStatusBlock,
		convertedFileInfo,
		Length,
		FileInformationClass);

	RETURN(ret);
}

// ******************************************************************
// * 0x00E3 - NtSetIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(227) xbox::NTSTATUS NTAPI xbox::NtSetIoCompletion
(
	IN HANDLE IoCompletionHandle,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN NTSTATUS IoStatus,
	IN ULONG_PTR IoStatusInformation
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E4 - NtSetSystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(228) xbox::NTSTATUS NTAPI xbox::NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SystemTime)
		LOG_FUNC_ARG_OUT(PreviousTime)
	LOG_FUNC_END;

	NTSTATUS ret = STATUS_SUCCESS;
	LARGE_INTEGER NewSystemTime, OldSystemTime;
	// LARGE_INTEGER LocalTime;
	// TIME_FIELDS TimeFields;

	if (SystemTime == nullptr) {
		ret = STATUS_ACCESS_VIOLATION;
	}
	else {
		NtSystemTimeMtx.lock();
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
		NtSystemTimeMtx.unlock();
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00E5 - NtSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(229) xbox::NTSTATUS NTAPI xbox::NtSetTimerEx
(
	IN HANDLE TimerHandle,
	IN PLARGE_INTEGER DueTime,
	IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode,
	IN PVOID TimerContext OPTIONAL,
	IN BOOLEAN WakeTimer,
	IN LONG Period OPTIONAL,
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
XBSYSAPI EXPORTNUM(230) xbox::NTSTATUS NTAPI xbox::NtSignalAndWaitForSingleObjectEx
(
	IN HANDLE SignalHandle,
	IN HANDLE WaitHandle,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00E7 - NtSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(231) xbox::NTSTATUS NTAPI xbox::NtSuspendThread
(
	IN  HANDLE  ThreadHandle,
	OUT PULONG  PreviousSuspendCount OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtSuspendThread(
		ThreadHandle, 
		PreviousSuspendCount);

	// TODO : Once we do our own thread-switching, implement NtSuspendThread using KeSuspendThread

	RETURN(ret);
}

// ******************************************************************
// * 0x00E8 - NtUserIoApcDispatcher()
// ******************************************************************
XBSYSAPI EXPORTNUM(232) xbox::VOID NTAPI xbox::NtUserIoApcDispatcher
(
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG            Reserved
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ApcContext)
		LOG_FUNC_ARG(IoStatusBlock)
		LOG_FUNC_ARG(Reserved)
		LOG_FUNC_END;

	ULONG dwErrorCode = 0;
	ULONG dwTransferred = 0;

	if (NT_SUCCESS(IoStatusBlock->Status)) {
		dwTransferred = (ULONG)IoStatusBlock->Information;
	} else {
		dwErrorCode = RtlNtStatusToDosError(IoStatusBlock->Status);
	}

	LPOVERLAPPED_COMPLETION_ROUTINE CompletionRoutine = (LPOVERLAPPED_COMPLETION_ROUTINE)ApcContext;
	LPOVERLAPPED lpOverlapped = (LPOVERLAPPED)CONTAINING_RECORD(IoStatusBlock, OVERLAPPED, Internal);

	(CompletionRoutine)(dwErrorCode, dwTransferred, lpOverlapped);

	EmuLog(LOG_LEVEL::DEBUG, "NtUserIoApcDispatcher Completed");
}

// ******************************************************************
// * 0x00E9 - NtWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(233) xbox::NTSTATUS NTAPI xbox::NtWaitForSingleObject
(
    IN  HANDLE  Handle,
    IN  BOOLEAN Alertable,
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
XBSYSAPI EXPORTNUM(234) xbox::NTSTATUS NTAPI xbox::NtWaitForSingleObjectEx
(
	IN  HANDLE          Handle,
	IN  KPROCESSOR_MODE WaitMode,
	IN  BOOLEAN         Alertable,
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
XBSYSAPI EXPORTNUM(235) xbox::NTSTATUS NTAPI xbox::NtWaitForMultipleObjectsEx
(
	IN  ULONG           Count,
	IN  HANDLE         *Handles,
	IN  WAIT_TYPE       WaitType,
	IN  CHAR            WaitMode,
	IN  BOOLEAN         Alertable,
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

	return NtDll::NtWaitForMultipleObjects(
		Count,
		Handles,
		(NtDll::OBJECT_WAIT_TYPE)WaitType,
		Alertable,
		(NtDll::PLARGE_INTEGER)Timeout);
}

// ******************************************************************
// * 0x00EC - NtWriteFile()
// ******************************************************************
// Writes a file.
//
// Differences from NT: There is no Key parameter.
XBSYSAPI EXPORTNUM(236) xbox::NTSTATUS NTAPI xbox::NtWriteFile
(
	IN  HANDLE          FileHandle,
	IN  HANDLE          Event,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN  PVOID           Buffer,
	IN  ULONG           Length,
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

	NTSTATUS ret = NtDll::NtWriteFile(
		FileHandle,
		Event,
		(PVOID)ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		(NtDll::LARGE_INTEGER*)ByteOffset,
		/*Key=*/nullptr);

	if (FAILED(ret))
		EmuLog(LOG_LEVEL::WARNING, "NtWriteFile Failed! (0x%.08X)", ret);

	RETURN(ret);
}

// ******************************************************************
// * 0x00ED - NtWriteFileGather
// ******************************************************************
XBSYSAPI EXPORTNUM(237) xbox::NTSTATUS NTAPI xbox::NtWriteFileGather
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ULONG Length,
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

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00EE - NtYieldExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(238) xbox::VOID NTAPI xbox::NtYieldExecution()
{
	// NOTE: Logging this fills the debug log far too quickly, so don't.
	// LOG_FUNC();

	NtDll::NtYieldExecution();
}

