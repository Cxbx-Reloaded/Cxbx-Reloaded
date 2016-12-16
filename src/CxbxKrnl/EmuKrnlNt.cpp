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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

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
#include "EmuAlloc.h" // For CxbxFree(), CxbxMalloc(), etc.

#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)

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
		LOG_FUNC_ARG(BaseAddress)
		LOG_FUNC_ARG(ZeroBits)
		LOG_FUNC_ARG(AllocationSize)
		LOG_FUNC_ARG(AllocationType)
		LOG_FUNC_ARG(Protect)
		LOG_FUNC_END;

	// TODO: The flag known as MEM_NOZERO (which appears to be exclusive to Xbox) has the exact
	// same value as MEM_ROTATE which causes problems for Windows XP, but not Vista.  Removing
	// this flag fixes Azurik for XP.
	DWORD MEM_NOZERO = 0x800000;

	if (AllocationType & MEM_NOZERO)
	{
		EmuWarning("MEM_NOZERO flag is not supported!");
		AllocationType &= ~MEM_NOZERO;
	}

	NTSTATUS ret = NtDll::NtAllocateVirtualMemory(GetCurrentProcess(), BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect);
	if (ret == 0xC00000F3)
		EmuWarning("Invalid Param!");

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

	NTSTATUS ret = NtDll::NtClearEvent(EventHandle);

	if (FAILED(ret))
		EmuWarning("NtClearEvent Failed!");

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

	// delete 'special' handles
	if (IsEmuHandle(Handle))
	{
		EmuHandle *iEmuHandle = HandleToEmuHandle(Handle);
		ret = iEmuHandle->NtClose();
	}
	// close normal handles
	else
	{
		ret = NtDll::NtClose(Handle);
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

	NTSTATUS ret = 0;

	NativeObjectAttributes nativeObjectAttributes;
	ACCESS_MASK DesiredAccess = 0;

	ret = CxbxObjectAttributesToNT(ObjectAttributes, nativeObjectAttributes, "NtCreateDirectoryObject");
	if (ret == STATUS_SUCCESS)
	{
		// TODO -oDxbx : Is this the correct ACCESS_MASK? :
		DesiredAccess = DIRECTORY_CREATE_OBJECT;

		ret = /*# JwaNative::*/ NtDll::NtCreateDirectoryObject(DirectoryHandle, DesiredAccess, nativeObjectAttributes.NtObjAttrPtr);
	}

	if (FAILED(ret))
		EmuWarning("NtCreateDirectoryObject Failed!");

	DbgPrintf("EmuKrnl (0x%X): NtCreateDirectoryObject DirectoryHandle = 0x%.08X\n", GetCurrentThreadId(), *DirectoryHandle);

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

	NativeObjectAttributes nativeObjectAttributes;
	ACCESS_MASK DesiredAccess = 0;

	// initialize object attributes
	NTSTATUS ret = CxbxObjectAttributesToNT(ObjectAttributes, nativeObjectAttributes); /*var*/
	if (ret == STATUS_SUCCESS)
	{
		// TODO -oDxbx : Is this the correct ACCESS_MASK? :
		DesiredAccess = EVENT_ALL_ACCESS;

		// redirect to Win2k/XP
		ret = NtDll::NtCreateEvent(EventHandle, DesiredAccess, nativeObjectAttributes.NtObjAttrPtr, (NtDll::EVENT_TYPE)EventType, InitialState);
		// TODO : Instead of the above, we should consider using the Ke*Event APIs, but
		// that would require us to create the event's kernel object with the Ob* api's too!
	}

	if (FAILED(ret))
		EmuWarning("NtCreateEvent Failed!");

	DbgPrintf("EmuKrnl (0x%X): NtCreateEvent EventHandle = 0x%.08X\n", GetCurrentThreadId(), *EventHandle);

	RETURN(ret);
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

	char *szBuffer = (ObjectAttributes != 0) ? ObjectAttributes->ObjectName->Buffer : 0;
	wchar_t wszObjectName[160];

	NtDll::UNICODE_STRING    NtUnicodeString;
	NtDll::OBJECT_ATTRIBUTES NtObjAttr;

	// initialize object attributes
	if (szBuffer != 0)
	{
		mbstowcs(wszObjectName, "\\??\\", 4);
		mbstowcs(wszObjectName + 4, szBuffer, 160);

		NtDll::RtlInitUnicodeString(&NtUnicodeString, wszObjectName);

		InitializeObjectAttributes(&NtObjAttr, &NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);
	}

	NtObjAttr.RootDirectory = 0;

	// redirect to NtCreateMutant
	NTSTATUS ret = NtDll::NtCreateMutant(MutantHandle, MUTANT_ALL_ACCESS, (szBuffer != 0) ? &NtObjAttr : 0, InitialOwner);

	if (FAILED(ret))
		EmuWarning("NtCreateMutant Failed!");

	DbgPrintf("EmuKrnl (0x%X): NtCreateMutant MutantHandle = 0x%.08X\n", GetCurrentThreadId(), *MutantHandle);

	RETURN(ret);
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

	// redirect to Win2k/XP
	NTSTATUS ret = NtDll::NtCreateSemaphore
	(
		SemaphoreHandle,
		SEMAPHORE_ALL_ACCESS,
		(NtDll::POBJECT_ATTRIBUTES)ObjectAttributes,
		InitialCount,
		MaximumCount
	);

	if (FAILED(ret))
		EmuWarning("NtCreateSemaphore failed!");

	DbgPrintf("EmuKrnl (0x%X): NtCreateSemaphore SemaphoreHandle = 0x%.08X\n", GetCurrentThreadId(), *SemaphoreHandle);

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

	switch (IoControlCode)
	{
		// IOCTL_SCSI_PASS_THROUGH_DIRECT
	case 0x4D014:
	{
		PSCSI_PASS_THROUGH_DIRECT PassThrough = (PSCSI_PASS_THROUGH_DIRECT)InputBuffer;
		PDVDX2_AUTHENTICATION Authentication = (PDVDX2_AUTHENTICATION)PassThrough->DataBuffer;

		// Should be just enough info to pass XapiVerifyMediaInDrive
		Authentication->AuthenticationPage.CDFValid = 1;
		Authentication->AuthenticationPage.PartitionArea = 1;
		Authentication->AuthenticationPage.Authentication = 1;
		break;
	}
	default:
		LOG_UNIMPLEMENTED();
	}

	RETURN(STATUS_SUCCESS);
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

	NTSTATUS ret;

	if (IsEmuHandle(SourceHandle)) {
		EmuHandle* iEmuHandle = HandleToEmuHandle(SourceHandle);
		ret = iEmuHandle->NtDuplicateObject(TargetHandle, Options);
	}
	else {
		// redirect to Win2k/XP
		ret = NtDll::NtDuplicateObject
		(
			GetCurrentProcess(),
			SourceHandle,
			GetCurrentProcess(),
			TargetHandle,
			0, 0, Options
		);
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

	NTSTATUS ret = NtDll::NtFlushBuffersFile(FileHandle, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock);

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

	NTSTATUS ret = NtDll::NtFreeVirtualMemory(GetCurrentProcess(), BaseAddress, FreeSize, FreeType);

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
		NULL, 
		0, 
		ShareAccess, 
		FILE_OPEN, 
		OpenOptions, 
		0);
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

	NTSTATUS ret = 0;
	EmuNtSymbolicLinkObject* symbolicLinkObject = NULL;

	symbolicLinkObject = FindNtSymbolicLinkObjectByName(std::string(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length));
	if (symbolicLinkObject != NULL)
	{
        // Return a new handle (which is an EmuHandle, actually) :
		*LinkHandle = symbolicLinkObject->NewHandle();
		ret = STATUS_SUCCESS;
	}
	else
		if (ret != STATUS_SUCCESS)
			EmuWarning("NtOpenSymbolicLinkObject failed! (%s)", NtStatusToString(ret));
		else
			DbgPrintf("EmuKrnl : NtOpenSymbolicLinkObject LinkHandle^ = 0x%.08X", *LinkHandle);

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
	IN HANDLE                       EventHandle,
	OUT PLONG                       PreviousState OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(EventHandle)
		LOG_FUNC_ARG_OUT(PreviousState)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
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

	NTSTATUS ret;

	// TODO: Not too sure how this one works.  If there's any special *magic* that needs to be
	//		 done, let me know!
	ret = NtDll::NtQueueApcThread((NtDll::HANDLE) ThreadHandle, (NtDll::PIO_APC_ROUTINE) ApcRoutine, ApcRoutineContext,
		(NtDll::PIO_STATUS_BLOCK) ApcStatusBlock, ApcReserved);

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

	if (FileInformationClass != 1)   // Due to unicode->string conversion
		CxbxKrnlCleanup("Unsupported FileInformationClass");

	NtDll::UNICODE_STRING NtFileMask;

	wchar_t wszObjectName[160];

	// initialize FileMask
	{
		if (FileMask != 0)
			mbstowcs(wszObjectName, FileMask->Buffer, 160);
		else
			mbstowcs(wszObjectName, "", 160);

		NtDll::RtlInitUnicodeString(&NtFileMask, wszObjectName);
	}

	NtDll::FILE_DIRECTORY_INFORMATION *FileDirInfo = (NtDll::FILE_DIRECTORY_INFORMATION*)CxbxMalloc(0x40 + 160 * 2);

	char    *mbstr = FileInformation->FileName;
	wchar_t *wcstr = FileDirInfo->FileName;

	do
	{
		ZeroMemory(wcstr, 160 * 2);

		ret = NtDll::NtQueryDirectoryFile
		(
			FileHandle, Event, (NtDll::PIO_APC_ROUTINE)ApcRoutine, ApcContext, (NtDll::IO_STATUS_BLOCK*)IoStatusBlock, FileDirInfo,
			0x40 + 160 * 2, (NtDll::FILE_INFORMATION_CLASS)FileInformationClass, TRUE, &NtFileMask, RestartScan
		);

		// convert from PC to Xbox
		{
			memcpy(FileInformation, FileDirInfo, 0x40);

			wcstombs(mbstr, wcstr, 160);

			FileInformation->FileNameLength /= 2;
		}

		RestartScan = FALSE;
	}
	// Xbox does not return . and ..
	while (strcmp(mbstr, ".") == 0 || strcmp(mbstr, "..") == 0);

	// TODO: Cache the last search result for quicker access with CreateFile (xbox does this internally!)
	CxbxFree(FileDirInfo);

	RETURN(ret);
}

// ******************************************************************
// * 0x00D2 - NtQueryFullAttributesFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(210) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT PVOID                       Attributes
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ObjectAttributes)
		LOG_FUNC_ARG_OUT(Attributes)
		LOG_FUNC_END;

	//	__asm int 3;
	NativeObjectAttributes nativeObjectAttributes;
	NTSTATUS ret = CxbxObjectAttributesToNT(ObjectAttributes, nativeObjectAttributes, "NtQueryFullAttributesFile"); /*var*/

	if (ret == STATUS_SUCCESS)
		ret = NtDll::NtQueryFullAttributesFile(nativeObjectAttributes.NtObjAttrPtr, Attributes);

	if (FAILED(ret))
		EmuWarning("NtQueryFullAttributesFile failed! (0x%.08X)\n", ret);

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
	IN  FILE_INFORMATION_CLASS      FileInfo
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG_OUT(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInfo)
		LOG_FUNC_END;

	// TODO: IIRC, this function is deprecated.  Maybe we should just use
	// ZwQueryInformationFile instead?

	//    if(FileInfo != FilePositionInformation && FileInfo != FileNetworkOpenInformation)
	//        CxbxKrnlCleanup("Unknown FILE_INFORMATION_CLASS 0x%.08X", FileInfo);

	NTSTATUS ret = NtDll::NtQueryInformationFile
	(
		FileHandle,
		(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
		(NtDll::PFILE_FS_SIZE_INFORMATION)FileInformation,
		Length,
		(NtDll::FILE_INFORMATION_CLASS)FileInfo
	);

	//
	// DEBUGGING!
	//
	{
		/*
		_asm int 3;
		NtDll::FILE_NETWORK_OPEN_INFORMATION *pInfo = (NtDll::FILE_NETWORK_OPEN_INFORMATION*)FileInformation;

		if(FileInfo == FileNetworkOpenInformation && (pInfo->AllocationSize.LowPart == 57344))
		{
		DbgPrintf("pInfo->AllocationSize : %d\n", pInfo->AllocationSize.LowPart);
		DbgPrintf("pInfo->EndOfFile      : %d\n", pInfo->EndOfFile.LowPart);

		pInfo->EndOfFile.LowPart = 0x1000;
		pInfo->AllocationSize.LowPart = 0x1000;

		fflush(stdout);
		}
		*/
	}

	if (FAILED(ret))
		EmuWarning("NtQueryInformationFile failed! (0x%.08X)", ret);

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

	NTSTATUS result = 0;
	EmuNtSymbolicLinkObject* symbolicLinkObject = NULL;

	// Check that we actually got an EmuHandle :
	result = STATUS_INVALID_HANDLE;

	EmuHandle* iEmuHandle = HandleToEmuHandle(LinkHandle);
	// Retrieve the NtSymbolicLinkObject and populate the output arguments :
	result = STATUS_SUCCESS;
	symbolicLinkObject = (EmuNtSymbolicLinkObject*)iEmuHandle->NtObject;
	if (LinkTarget != NULL)
	{
		if (LinkTarget->Length > LinkTarget->MaximumLength)
		{
			result = STATUS_BUFFER_TOO_SMALL;
			LinkTarget->Length = LinkTarget->MaximumLength;
		}

		memcpy(LinkTarget->Buffer, symbolicLinkObject->XboxFullPath.c_str(), LinkTarget->Length);
	}

	if (ReturnedLength != NULL)
	{
		*ReturnedLength = symbolicLinkObject->XboxFullPath.length(); // Return full length (even if buffer was too small)
	}

	if (result != STATUS_SUCCESS)
		EmuWarning("NtQuerySymbolicLinkObject failed! (%s)", NtStatusToString(result));

	RETURN(result);
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

	NTSTATUS ret = NtDll::NtQueryVirtualMemory
	(
		GetCurrentProcess(),
		BaseAddress,
		(NtDll::MEMORY_INFORMATION_CLASS)NtDll::MemoryBasicInformation,
		(NtDll::PMEMORY_BASIC_INFORMATION)Buffer,
		sizeof(MEMORY_BASIC_INFORMATION),
		0
	);

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

			DbgPrintf("EmuKrnl (0x%X): NtQueryVirtualMemory: Applied fix for Forza Motorsport!\n", GetCurrentThreadId());
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

	NTSTATUS ret = NtDll::NtQueryVolumeInformationFile
	(
		FileHandle,
		(NtDll::PIO_STATUS_BLOCK)IoStatusBlock,
		(NtDll::PFILE_FS_SIZE_INFORMATION)FileInformation, Length,
		(NtDll::FS_INFORMATION_CLASS)FileInformationClass
	);

	if (ret == STATUS_SUCCESS)
	{
		// NOTE: TODO: Dynamically fill in, or allow configuration?
		if (FileInformationClass == FileFsSizeInformation)
		{
			FILE_FS_SIZE_INFORMATION *SizeInfo = (FILE_FS_SIZE_INFORMATION*)FileInformation;

			SizeInfo->TotalAllocationUnits.QuadPart = 0x4C468;
			SizeInfo->AvailableAllocationUnits.QuadPart = 0x2F125;
			SizeInfo->SectorsPerAllocationUnit = 32;
			SizeInfo->BytesPerSector = 512;
		}
		else
			LOG_UNIMPLEMENTED();

	}
	else
	{
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
	IN  PVOID           ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext,
	OUT PVOID           IoStatusBlock,
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

	NTSTATUS ret = NtDll::NtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, (NtDll::LARGE_INTEGER*)ByteOffset, 0);

	if (FAILED(ret))
		EmuWarning("NtReadFile Failed! (0x%.08X)", ret);

	RETURN(ret);
}

// ******************************************************************
// * 0x00DD - NtReleaseMutant()
// ******************************************************************
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

	// redirect to NtCreateMutant
	NTSTATUS ret = NtDll::NtReleaseMutant(MutantHandle, PreviousCount);

	if (FAILED(ret))
		EmuWarning("NtReleaseMutant Failed!");

	RETURN(STATUS_SUCCESS); // TODO : RETURN(ret);
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

	NTSTATUS ret = NtDll::NtReleaseSemaphore(SemaphoreHandle, ReleaseCount, PreviousCount);

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

	NTSTATUS ret = NtDll::NtResumeThread(ThreadHandle, PreviousSuspendCount);

	Sleep(10);

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

	NTSTATUS ret = NtDll::NtSetEvent(EventHandle, PreviousState);

	if (FAILED(ret))
		EmuWarning("NtSetEvent Failed!");

	RETURN(ret);
}

// ******************************************************************
// * 0x00E2 - NtSetInformationFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(226) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtSetInformationFile
(
	IN  HANDLE  FileHandle,            // TODO: correct paramters
	OUT PVOID   IoStatusBlock,
	IN  PVOID   FileInformation,
	IN  ULONG   Length,
	IN  ULONG   FileInformationClass
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(FileHandle)
		LOG_FUNC_ARG_OUT(IoStatusBlock)
		LOG_FUNC_ARG(FileInformation)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(FileInformationClass)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtSetInformationFile(FileHandle, IoStatusBlock, FileInformation, Length, FileInformationClass);

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

	NTSTATUS ret = NtDll::NtSuspendThread(ThreadHandle, PreviousSuspendCount);

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

	uint32 dwEsi, dwEax, dwEcx;

	dwEsi = (uint32)IoStatusBlock;

	if ((IoStatusBlock->u1.Status & 0xC0000000) == 0xC0000000)
	{
		dwEcx = 0;
		dwEax = NtDll::RtlNtStatusToDosError(IoStatusBlock->u1.Status);
	}
	else
	{
		dwEcx = (DWORD)IoStatusBlock->Information;
		dwEax = 0;
	}

	/*
	// ~XDK 3911??
	if(true)
	{
	dwEsi = dw2;
	dwEcx = dw1;
	dwEax = dw3;

	}
	else
	{
	dwEsi = dw1;
	dwEcx = dw2;
	dwEax = dw3;
	}//*/

	__asm
	{
		pushad
		/*
		mov esi, IoStatusBlock
		mov ecx, dwEcx
		mov eax, dwEax
		*/
		// TODO: Figure out if/why this works!? Matches prototype, but not xboxkrnl disassembly
		// Seems to be XDK/version dependand??
		mov esi, dwEsi
		mov ecx, dwEcx
		mov eax, dwEax

		push esi
		push ecx
		push eax

		call ApcContext

		popad
	}

	DbgPrintf("EmuKrnl (0x%X): NtUserIoApcDispatcher Completed\n", GetCurrentThreadId());
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
	LOG_FORWARD("KeWaitForMultipleObjects");

	return xboxkrnl::KeWaitForMultipleObjects(
		/*Count=*/1,
		&Handle,
		/*WaitType=*/WaitAll,
		/*WaitReason=*/WrUserRequest,
		/*WaitMode=*/KernelMode,
		Alertable,
		Timeout,
		/*WaitBlockArray*/NULL
	);
}

// ******************************************************************
// * 0x00EA - NtWaitForSingleObjectEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(234) xboxkrnl::NTSTATUS NTAPI xboxkrnl::NtWaitForSingleObjectEx
(
	IN  HANDLE          Handle,
	IN  KPROCESSOR_MODE WaitMode,
	IN  BOOLEAN         Alertable,
	IN  PLARGE_INTEGER  Timeout
)
{
	LOG_FORWARD("KeWaitForMultipleObjects");

	return xboxkrnl::KeWaitForMultipleObjects(
		/*Count=*/1,
		&Handle,
		/*WaitType=*/WaitAll,
		/*WaitReason=*/WrUserRequest,
		WaitMode,
		Alertable,
		Timeout,
		/*WaitBlockArray*/NULL
	);
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

	return xboxkrnl::KeWaitForMultipleObjects(
		Count,
		Handles,
		WaitType,
		/*WaitReason=*/WrUserRequest,
		WaitMode,
		Alertable,
		Timeout,
		/*WaitBlockArray*/NULL
	);
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

	NTSTATUS ret = NtDll::NtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, (NtDll::LARGE_INTEGER*)ByteOffset, 0);

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

