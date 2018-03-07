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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlNt.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "KRNL"

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For NtAllocateVirtualMemory, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuFile.h" // For EmuNtSymbolicLinkObject, NtStatusToString(), etc.
#include "EmuKrnl.h" // For OBJECT_TO_OBJECT_HEADER()
#include "EmuKrnlKe.h" // For KeClearEvent(), KeQueryEvent(), GetMode, GetHostEvent(), KeQueryMutant()
#include "VMManager.h" // For g_VMManager
#include "CxbxDebugger.h"

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include <assert.h>

#define MEM_KNOWN_FLAGS (MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN | MEM_RESET | MEM_NOZERO)

#define MM_HIGHEST_USER_ADDRESS     0x7FFEFFFF
#define X64K                        ((ULONG)64*1024)
#define MM_HIGHEST_VAD_ADDRESS      (MM_HIGHEST_USER_ADDRESS - X64K)

// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(184) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtAllocateVirtualMemory
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
		LOG_FUNC_ARG(Protect)
		LOG_FUNC_END;

	// BaseAddress may be unassigned
	assert(AllocationSize != xbnullptr); // AllocationSize must be assigned

	NTSTATUS ret = 0;

	ULONG_PTR RequestedBaseAddress = (BaseAddress) ? (ULONG_PTR)*BaseAddress : 0;
	ULONG RequestedAllocationSize = *AllocationSize;

	DbgPrintf("KNRL: NtAllocateVirtualMemory requested range : 0x%.8X - 0x%.8X\n", RequestedBaseAddress, RequestedBaseAddress + RequestedAllocationSize);

	// Don't allow base address to exceed highest virtual address
	if (RequestedBaseAddress > MM_HIGHEST_VAD_ADDRESS)
		ret = STATUS_INVALID_PARAMETER;

	// Limit number of zero bits upto 20
	if (ZeroBits > 21)
		ret = STATUS_INVALID_PARAMETER;

	// No empty allocation allowed
	if (RequestedAllocationSize == 0)
		ret = STATUS_INVALID_PARAMETER;

	// Allocation should fit in remaining address range
	if (RequestedBaseAddress > 0)
		if (((ULONG_PTR)MM_HIGHEST_VAD_ADDRESS - RequestedBaseAddress + 1) < RequestedAllocationSize)
			ret = STATUS_INVALID_PARAMETER;

	// Only known flags are allowed
	if ((AllocationType & ~MEM_KNOWN_FLAGS) != 0)
		ret = STATUS_INVALID_PARAMETER;

	// No other flags allowed in combination with MEM_RESET
	if (AllocationType & MEM_RESET)
	{
		if (AllocationType != MEM_RESET)
			ret = STATUS_INVALID_PARAMETER;
	}
	else
		// At least MEM_RESET, MEM_COMMIT or MEM_RESERVE must be set
		if ((AllocationType & (MEM_COMMIT | MEM_RESERVE)) == 0)
			ret = STATUS_INVALID_PARAMETER;

	if (ret == 0)
	{
		ret = g_VMManager.XbAllocateVirtualMemory(
			(VAddr*)&RequestedBaseAddress,
			ZeroBits,
			(size_t*)&RequestedAllocationSize,
			AllocationType,
			Protect);

		if (NT_SUCCESS(ret))
		{
			ULONG_PTR ResultingBaseAddress = RequestedBaseAddress;
			ULONG ResultingAllocationSize = RequestedAllocationSize;

			DbgPrintf("KNRL: NtAllocateVirtualMemory resulting range : 0x%.8X - 0x%.8X\n", ResultingBaseAddress, ResultingBaseAddress + ResultingAllocationSize - 1);

			if (BaseAddress)
				*BaseAddress = (PVOID*)ResultingBaseAddress;

			*AllocationSize = RequestedAllocationSize;
		}
		else
			if (ret == STATUS_INVALID_PARAMETER_5) // = 0xC00000F3
				EmuWarning("Invalid Param!");
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00B9 - NtCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(185) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCancelTimer
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
		EmuWarning("NtCancelTimer failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00BA - NtClearEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(186) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtClearEvent
(
	IN HANDLE EventHandle
)
{
	LOG_FUNC_ONE_ARG(EventHandle);

	PVOID EventObj;

	NTSTATUS ret = ObReferenceObjectByHandle(EventHandle, &ExEventObjectType, &EventObj);
	if (SUCCEEDED(ret)) {
		PKEVENT KernelEvent = (PKEVENT)EventObj;

		KeClearEvent(KernelEvent);
		ObfDereferenceObject(EventObj);
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00BB - NtClose()
// ******************************************************************
XBSYSAPI EXPORTNUM(187) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtClose
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
	}
    else
    {
		KIRQL OldIrql = KeRaiseIrqlToDpcLevel();

		LOG_INCOMPLETE(); // TODO : Base this on the Ob* functions, 
		// Check if this is an event handle
		PVOID KernelObject; // TODO : Use ObpDestroyObjectHandle()
		if (SUCCEEDED(EmuObFindObjectByHandle(Handle, &KernelObject))) {			
			// For now, retrieve the host couterpart event and close that handle
			HANDLE hostEvent = GetHostEvent((PKEVENT)KernelObject, GetMode::Erase);
			if (hostEvent) {
				CloseHandle(hostEvent);
			}
		}
		else {
			if (CxbxDebugger::CanReport())
			{
				CxbxDebugger::ReportFileClosed(Handle);
			}

			// close normal handles
			ret = NtDll::NtClose(Handle);
		}

		KeLowerIrql(OldIrql);
    }

	RETURN(ret);
}

// ******************************************************************
// * 0x00BC - NtCreateDirectoryObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(188) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateDirectoryObject
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
		EmuWarning("NtCreateDirectoryObject Failed!");
	else
		DbgPrintf("KRNL: NtCreateDirectoryObject DirectoryHandle = 0x%.8X\n", *DirectoryHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00BD - NtCreateEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(189) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateEvent
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
}

// ******************************************************************
// * 0x00BE - NtCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(190) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateFile
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

	return xboxkrnl::IoCreateFile(
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

XBSYSAPI EXPORTNUM(191) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateIoCompletion
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
XBSYSAPI EXPORTNUM(192) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateMutant
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

	PVOID MutantObj;

	NTSTATUS Status = ObCreateObject(&ExMutantObjectType, ObjectAttributes, sizeof(KMUTANT), &MutantObj);
	if (NT_SUCCESS(Status)) {
		KeInitializeMutant((PKMUTANT)MutantObj, InitialOwner);
		Status = ObInsertObject(MutantObj, ObjectAttributes, 0, /*OUT*/MutantHandle);
	}

	RETURN(Status);
}

// ******************************************************************
// * 0x00C1 - NtCreateSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(193) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateSemaphore
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
		EmuWarning("Trying fallback (without object attributes)...\nError code 0x%X", ret);

		// If it fails, try again but without the object attributes stucture
		ret = NtDll::NtCreateSemaphore(
			/*OUT*/SemaphoreHandle,
			DesiredAccess,
			/*(NtDll::POBJECT_ATTRIBUTES)nativeObjectAttributes.NtObjAttrPtr*/ NULL,
			InitialCount,
			MaximumCount);

		if(FAILED(ret))
			EmuWarning("NtCreateSemaphore failed!");
		else
			DbgPrintf("KRNL: NtCreateSemaphore SemaphoreHandle = 0x%.8X\n", *SemaphoreHandle);
	}
	else
		DbgPrintf("KRNL: NtCreateSemaphore SemaphoreHandle = 0x%.8X\n", *SemaphoreHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C2 - NtCreateTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(194) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtCreateTimer
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
		EmuWarning("NtCreateTimer failed!");
	else
		DbgPrintf("KRNL: NtCreateTimer TimerHandle = 0x%.8X\n", *TimerHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00C3 - NtDeleteFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(195) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtDeleteFile
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
		EmuWarning("NtDeleteFile Failed!");
	
	RETURN(ret);
}

// ******************************************************************
// * 0x00C4 - NtDeviceIoControlFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(196) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtDeviceIoControlFile
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
XBSYSAPI EXPORTNUM(197) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtDuplicateObject
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
		EmuWarning("Object was not duplicated!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00C6 - NtFlushBuffersFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(198) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtFlushBuffersFile
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
XBSYSAPI EXPORTNUM(199) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtFreeVirtualMemory
(
	IN OUT PVOID *BaseAddress,
	IN OUT PULONG FreeSize,
	IN ULONG      FreeType
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(FreeSize)
		LOG_FUNC_ARG(FreeType)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_INVALID_PARAMETER;

	if (BaseAddress) { // Must be assigned
		// FreeSize may be unassigned
		ULONG RegionSize = (FreeSize) ? *FreeSize : 0;

		ret = g_VMManager.XbFreeVirtualMemory((VAddr*)BaseAddress, (size_t*)&RegionSize, FreeType);

		if (SUCCEEDED(ret)) {
			if (FreeSize != xbnullptr)
				*FreeSize = RegionSize;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00C8 - NtFsControlFile
// ******************************************************************
XBSYSAPI EXPORTNUM(200) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtFsControlFile
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
XBSYSAPI EXPORTNUM(201) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtOpenDirectoryObject
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
XBSYSAPI EXPORTNUM(202) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtOpenFile
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

	return xboxkrnl::IoCreateFile(
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
XBSYSAPI EXPORTNUM(203) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtOpenSymbolicLinkObject
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
		EmuWarning("NtOpenSymbolicLinkObject failed! (%s)", NtStatusToString(ret));
	else
		DbgPrintf("KRNL: NtOpenSymbolicLinkObject LinkHandle^ = 0x%.8X", *LinkHandle);

	RETURN(ret);
}

// ******************************************************************
// * 0x00CC - NtProtectVirtualMemory()
// ******************************************************************
XBSYSAPI EXPORTNUM(204) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtProtectVirtualMemory
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
		LOG_FUNC_ARG(NewProtect)
		LOG_FUNC_ARG_OUT(OldProtect)
		LOG_FUNC_END;

	LOG_IGNORED();

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x00CD - NtPulseEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(205) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtPulseEvent
(
	IN HANDLE	EventHandle,
	OUT PLONG	PreviousState OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	PVOID EventObj;

	NTSTATUS ret = ObReferenceObjectByHandle(EventHandle, &ExEventObjectType, &EventObj);
	if (SUCCEEDED(ret)) {
		PKEVENT KernelEvent = (PKEVENT)EventObj;

		ret = KePulseEvent(KernelEvent, (KPRIORITY)1, FALSE);
		ObfDereferenceObject(EventObj);
		if (PreviousState) {
			*PreviousState = ret;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00CE - NtQueueApcThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(206) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueueApcThread
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

	// TODO: Not too sure how this one works.  If there's any special *magic* that needs to be
	//		 done, let me know!
	NTSTATUS ret = NtDll::NtQueueApcThread(
		(NtDll::HANDLE)ThreadHandle,
		(NtDll::PIO_APC_ROUTINE)ApcRoutine,
		ApcRoutineContext,
		(NtDll::PIO_STATUS_BLOCK)ApcStatusBlock,
		ApcReserved);

	if (FAILED(ret))
		EmuWarning("NtQueueApcThread failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00CF - NtQueryDirectoryFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(207) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryDirectoryFile
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
		// TODO : assert that NtDll::FILE_DIRECTORY_INFORMATION has same members and size as xboxkrnl::FILE_DIRECTORY_INFORMATION
		memcpy(/*Dst=*/FileInformation, /*Src=*/NtFileDirInfo, /*Size=*/NtFileDirectoryInformationSize);
		wcstombs(/*Dest=*/mbstr, /*Source=*/wcstr, MAX_PATH);
		FileInformation->FileNameLength /= sizeof(wchar_t);
	}

	// TODO: Cache the last search result for quicker access with CreateFile (xbox does this internally!)
	free(NtFileDirInfo);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D2 - NtQueryFullAttributesFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(210) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT xboxkrnl::PFILE_NETWORK_OPEN_INFORMATION   Attributes
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
	NTToXboxFileInformation(&nativeNetOpenInfo, Attributes, FileNetworkOpenInformation, sizeof(xboxkrnl::FILE_NETWORK_OPEN_INFORMATION));

	if (FAILED(ret))
		EmuWarning("NtQueryFullAttributesFile failed! (0x%.08X)", ret);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D1  - NtQueryEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(209) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryEvent
(
	IN HANDLE EventHandle,
	OUT PEVENT_BASIC_INFORMATION EventInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(EventInformation)
		LOG_FUNC_END;

	PVOID EventObj;

	NTSTATUS ret = ObReferenceObjectByHandle(EventHandle, &ExEventObjectType, &EventObj);
	if (SUCCEEDED(ret)) {
		PKEVENT KernelEvent = (PKEVENT)EventObj;

		// Read event type and signal state before potential destruction
		EVENT_TYPE EventType = (EVENT_TYPE)(KernelEvent->Header.Type);
		ret = KeQueryEvent(KernelEvent); // Allow Ke-level intervention (for GetHostEvent)
		ObfDereferenceObject(EventObj);
		// Avoid exceptions when argument is absent
		if (EventInformation) {
			EventInformation->EventType = EventType;
			EventInformation->EventState = ret;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00D3 - NtQueryInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(211) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryInformationFile
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
		EmuWarning("NtQueryInformationFile failed! (0x%.08X)", ret);

	// Prioritize the buffer overflow over real return code,
	// in case the Xbox program decides to follow the same procedure above
	if (convRet == STATUS_BUFFER_OVERFLOW)
		return convRet;

	RETURN(ret);
}

// ******************************************************************
// * 0x00D5 - NtQueryMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(213) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryMutant
(
	IN HANDLE MutantHandle,
	OUT PMUTANT_BASIC_INFORMATION MutantInformation
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MutantHandle)
		LOG_FUNC_ARG_OUT(MutantInformation)
		LOG_FUNC_END;

	PVOID MutantObj;
	
	NTSTATUS Status = ObReferenceObjectByHandle(MutantHandle, &ExMutantObjectType, &MutantObj);

	if (NT_SUCCESS(Status)) {
		PKMUTANT KernelMutant = (PKMUTANT)MutantObj;

		KeQueryMutant(KernelMutant, MutantInformation);

		ObfDereferenceObject(MutantObj);
	}

	RETURN(Status);
}

// ******************************************************************
// * 0x00D6 - NtQuerySemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(214) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQuerySemaphore
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
		EmuWarning("NtQuerySemaphore failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D7 - NtQuerySymbolicLinkObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(215) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQuerySymbolicLinkObject
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
			if (LinkTarget->Length > LinkTarget->MaximumLength)
			{
				ret = STATUS_BUFFER_TOO_SMALL;
				LinkTarget->Length = LinkTarget->MaximumLength;
			}

			copy_string_to_PSTRING_to(symbolicLinkObject->XboxSymbolicLinkPath, LinkTarget);
		}

		if (ReturnedLength != NULL)
		{
			*ReturnedLength = symbolicLinkObject->XboxSymbolicLinkPath.length(); // Return full length (even if buffer was too small)
		}
	}
	if (ret != STATUS_SUCCESS)
		EmuWarning("NtQuerySymbolicLinkObject failed! (%s)", NtStatusToString(ret));

	RETURN(ret);
}

// ******************************************************************
// * 0x00D8 - NtQueryTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(216) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryTimer
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
XBSYSAPI EXPORTNUM(217) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryVirtualMemory
(
	IN  PVOID                       BaseAddress,
	OUT PMEMORY_BASIC_INFORMATION   Buffer
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG_OUT(Buffer)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtQueryVirtualMemory(
		/*ProcessHandle=*/g_CurrentProcessHandle,
		BaseAddress,
		(NtDll::MEMORY_INFORMATION_CLASS)NtDll::MemoryBasicInformation,
		(NtDll::PMEMORY_BASIC_INFORMATION)Buffer,
		/*Length=*/sizeof(MEMORY_BASIC_INFORMATION),
		/*ResultLength=*/nullptr);

	if (FAILED(ret)) {
		EmuWarning("NtQueryVirtualMemory failed (%s)!", NtStatusToString(ret));

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

			DbgPrintf("KRNL: NtQueryVirtualMemory: Applied fix for Forza Motorsport!\n");
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00DA - NtQueryVolumeInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(218) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryVolumeInformationFile
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
				EmuWarning("NtQueryVolumeInformationFile: Unknown FileInformationClass");
				memcpy_s(FileInformation, Length, NativeFileInformation, HostBufferSize);
				break;
		}
	}

	_aligned_free(NativeFileInformation);

	if (FAILED(ret)) {
		EmuWarning("NtQueryVolumeInformationFile failed! (%s)\n", NtStatusToString(ret));
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00DA - NtReadFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(219) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReadFile
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
		u64 Offset = ~0;
		if (ByteOffset)
			Offset = ByteOffset->QuadPart;

		CxbxDebugger::ReportFileRead(FileHandle, Length, Offset);
	}

	NTSTATUS ret = NtDll::NtReadFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		(NtDll::LARGE_INTEGER*)ByteOffset,
		/*Key=*/nullptr);

    if (FAILED(ret)) {
        EmuWarning("NtReadFile Failed! (0x%.08X)", ret);
    }

	RETURN(ret);
}

// ******************************************************************
// * 0x00DD - NtReleaseMutant()
// ******************************************************************
xboxkrnl::NTSTATUS NTAPI NtReleaseMutant_SEH
(
	IN  HANDLE  MutantHandle,
	OUT PLONG   PreviousCount
)
{
	using namespace xboxkrnl;

	PVOID MutantObj;
	NTSTATUS ret = ObReferenceObjectByHandle(MutantHandle, &ExMutantObjectType, &MutantObj);
	if (NT_SUCCESS(ret)) {
		__try {
			LONG Count = KeReleaseMutant((PKMUTANT)MutantObj, 1, FALSE, FALSE);
			ObfDereferenceObject(MutantObj);
			if (PreviousCount) {
				*PreviousCount = Count;
			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			ObfDereferenceObject(MutantObj);
			ret = GetExceptionCode();
		}
	}

	return ret;
}

XBSYSAPI EXPORTNUM(221) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReleaseMutant
(
	IN  HANDLE  MutantHandle,
	OUT PLONG   PreviousCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MutantHandle)
		LOG_FUNC_ARG_OUT(PreviousCount)
		LOG_FUNC_END;

	// Note : Forward to separate function so that can use Structured Exception Handling (SEH)
	// which isn't possible here because of the object unwinding that results from our logging.
	NTSTATUS ret = NtReleaseMutant_SEH(MutantHandle, PreviousCount);

	RETURN(ret);
}

// ******************************************************************
// * 0x00DE - NtReleaseSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(222) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtReleaseSemaphore
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
		EmuWarning("NtReleaseSemaphore failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00E0 - NtResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(224) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtResumeThread
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
XBSYSAPI EXPORTNUM(225) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetEvent
(
	IN  HANDLE EventHandle,
	OUT PLONG  PreviousState
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	PVOID EventObj;

	NTSTATUS ret = ObReferenceObjectByHandle(EventHandle, &ExEventObjectType, &EventObj);
	if (SUCCEEDED(ret)) {
		PKEVENT KernelEvent = (PKEVENT)EventObj;

		ret = KeSetEvent(KernelEvent, (KPRIORITY)1, FALSE);
		ObfDereferenceObject(EventObj);
		if (PreviousState) {
			*PreviousState = ret;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00E2 - NtSetInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(226) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetInformationFile
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
// * 0x00E4 - NtSetSystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(228) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(SystemTime)
		LOG_FUNC_ARG_OUT(PreviousTime)
		LOG_FUNC_END;

	NTSTATUS ret = STATUS_SUCCESS; // TODO : Does Xbox returns STATUS_PRIVILEGE_NOT_HELD (supports SeSystemtimePrivlege)?

	if (PreviousTime == NULL && SystemTime == NULL)
		ret = STATUS_ACCESS_VIOLATION;
	else
	{
		// Surely, we won't set the system time here, but we CAN remember a delta to the host system time;
		LARGE_INTEGER HostSystemTime;
		GetSystemTimeAsFileTime((LPFILETIME)&HostSystemTime); // Available since Windows 2000 (NOT on XP!)

		// Is the previous time requested?
		if (PreviousTime != NULL)
			// Apply current HostSystemTimeDelta, same as in xboxkrnl::KeQuerySystemTime :
			PreviousTime->QuadPart = HostSystemTime.QuadPart + HostSystemTimeDelta.QuadPart;

		// Is a new time given?
		if (SystemTime != NULL)
		{
			if (SystemTime->QuadPart > 0)
				// Calculate new HostSystemTimeDelta, to be used in xboxkrnl::KeQuerySystemTime :
				HostSystemTimeDelta.QuadPart = HostSystemTime.QuadPart - SystemTime->QuadPart;
			else
				ret = STATUS_INVALID_PARAMETER;
		}
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x00E5 - NtSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(229) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetTimerEx
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
		EmuWarning("NtSetTimerEx failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00E7 - NtSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(231) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSuspendThread
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
XBSYSAPI EXPORTNUM(232) xboxkrnl::VOID NTAPI xboxkrnl::NtUserIoApcDispatcher
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

	DbgPrintf("KRNL: NtUserIoApcDispatcher Completed\n");
}

// ******************************************************************
// * 0x00E9 - NtWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(233) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForSingleObject
(
    IN  HANDLE  Handle,
    IN  BOOLEAN Alertable,
    IN  PLARGE_INTEGER   Timeout
)
{
	// LOG_FORWARD("NtWaitForSingleObjectEx");

	return NtWaitForSingleObjectEx(
		Handle,
		/*WaitMode=*/KernelMode,
		Alertable,
		Timeout
	);
}

// ******************************************************************
// * 0x00EA - NtWaitForSingleObjectEx()
// ******************************************************************
xboxkrnl::NTSTATUS NTAPI NtWaitForSingleObjectEx_SEH
(
	IN  xboxkrnl::HANDLE          Handle,
	IN  xboxkrnl::KPROCESSOR_MODE WaitMode,
	IN  xboxkrnl::BOOLEAN         Alertable,
	IN  xboxkrnl::PLARGE_INTEGER  Timeout
)
{
	using namespace xboxkrnl;

	// Temporary work-around : detect Windows handle
	DWORD handleFlags = 0;
	if (GetHandleInformation(Handle, &handleFlags) != 0) {
		// This was a Windows handle, so we treat it like that
		return WaitForSingleObjectEx(Handle, Timeout ? Timeout->u.LowPart : 0, (BOOL)Alertable);
	}

	PVOID Object;
	NTSTATUS Status = ObReferenceObjectByHandle(Handle, NULL, &Object);

	if (NT_SUCCESS(Status)) {
		POBJECT_HEADER ObjectHeader = OBJECT_TO_OBJECT_HEADER(Object);
		PVOID WaitObject = ObjectHeader->Type->DefaultObject;

		// Note : The real kernel is loaded at 0x80000000, and as such,
		// can check for absolute addresses on DefaultObject, by checking
		// the sign-bit. Since our kernel doesn't reside there, absolute
		// addresses (such as &ObpDefaultObject) must be checked by a limit
		// on offsetof values (the other contents of OBJECT_TYPE.DefaultObject)
		if ((uintptr_t)WaitObject < PAGE_SIZE) {
			WaitObject = (PVOID)((PCHAR)Object + (uintptr_t)WaitObject);
		}

		__try {
			KWAIT_BLOCK WaitBlock;

			Status = KeWaitForMultipleObjects(
				/*Count=*/1,
				&WaitObject,
				/*WaitType=*/WaitAll,
				/*WaitReason=*/WrUserRequest,
				WaitMode,
				Alertable,
				Timeout,
				&WaitBlock
			);
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			Status = GetExceptionCode();
		}

		ObfDereferenceObject(Object);
	}

	return Status;
}

XBSYSAPI EXPORTNUM(234) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForSingleObjectEx
(
	IN  HANDLE          Handle,
	IN  KPROCESSOR_MODE WaitMode,
	IN  BOOLEAN         Alertable,
	IN  PLARGE_INTEGER  Timeout
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Handle)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_END;

	NTSTATUS ret = NtWaitForSingleObjectEx_SEH(Handle, WaitMode, Alertable, Timeout);

	RETURN(ret);
}

// ******************************************************************
// * 0x00EB - NtWaitForMultipleObjectsEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(235) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForMultipleObjectsEx
(
	IN  ULONG           Count,
	IN  HANDLE         *Handles,
	IN  WAIT_TYPE       WaitType,
	IN  CHAR            WaitMode,
	IN  BOOLEAN         Alertable,
	IN  PLARGE_INTEGER  Timeout
)
{
	LOG_FORWARD("KeWaitForMultipleObjects");

	KWAIT_BLOCK WaitBlockArray[9];

	return xboxkrnl::KeWaitForMultipleObjects(
		Count,
		Handles,
		WaitType,
		/*WaitReason=*/WrUserRequest,
		WaitMode,
		Alertable,
		Timeout,
		WaitBlockArray);
}

// ******************************************************************
// * 0x00EC - NtWriteFile()
// ******************************************************************
// Writes a file.
//
// Differences from NT: There is no Key parameter.
XBSYSAPI EXPORTNUM(236) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWriteFile
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
		u64 Offset = ~0;
		if (ByteOffset)
			Offset = ByteOffset->QuadPart;
		
		CxbxDebugger::ReportFileWrite(FileHandle, Length, Offset);
	}

	NTSTATUS ret = NtDll::NtWriteFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		(NtDll::LARGE_INTEGER*)ByteOffset,
		/*Key=*/nullptr);

	if (FAILED(ret))
		EmuWarning("NtWriteFile Failed! (0x%.08X)", ret);

	RETURN(ret);
}

// ******************************************************************
// * 0x00EE - NtYieldExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(238) xboxkrnl::VOID NTAPI xboxkrnl::NtYieldExecution()
{
	// NOTE: Logging this fills the debug log far too quickly, so don't.
	// LOG_FUNC();

	NtDll::NtYieldExecution();
}

