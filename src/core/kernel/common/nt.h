// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : nt.h
// *
// * note : XBox Kernel *NT* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_NT_H
#define XBOXKRNL_NT_H

#include "types.h"

namespace xbox
{

#define NtCurrentThread() ((HANDLE)-2)

#define X_MAXIMUM_WAIT_OBJECTS 64

// ******************************************************************
// * NtAllocateVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(184) ntstatus_xt NTAPI NtAllocateVirtualMemory
(
    IN OUT PVOID    *BaseAddress,
    IN ulong_xt         ZeroBits,
    IN OUT PULONG    AllocationSize,
    IN dword_xt         AllocationType,
    IN dword_xt         Protect
);

// ******************************************************************
// * 0x00B9 - NtCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(185) ntstatus_xt NTAPI NtCancelTimer
(
	IN HANDLE TimerHandle,
	OUT PBOOLEAN CurrentState OPTIONAL
);

// ******************************************************************
// * NtClearEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(186) ntstatus_xt NTAPI NtClearEvent
(
    IN HANDLE EventHandle
);

// ******************************************************************
// * NtClose
// ******************************************************************
// *
// * Closes an object handle
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(187) ntstatus_xt NTAPI NtClose
(
    IN HANDLE Handle
);

XBSYSAPI EXPORTNUM(188) ntstatus_xt NTAPI NtCreateDirectoryObject
(
	OUT PHANDLE             DirectoryHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

// ******************************************************************
// * NtCreateEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(189) ntstatus_xt NTAPI NtCreateEvent
(
    OUT PHANDLE             EventHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  boolean_xt             InitialState
);



// ******************************************************************
// * NtCreateFile
// ******************************************************************
// *
// * Causes a new file or directory to be created, or opens and
// * existing file, device, directory, or volume, giving the caller
// * a handle for the file object.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(190) ntstatus_xt NTAPI NtCreateFile
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
);

XBSYSAPI EXPORTNUM(191) ntstatus_xt NTAPI NtCreateIoCompletion
(
	OUT PHANDLE IoCompletionHandle,
	IN access_mask_xt DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN ulong_xt Count
);

// ******************************************************************
// * NtCreateMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(192) ntstatus_xt NTAPI NtCreateMutant
(
    OUT PHANDLE             MutantHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  boolean_xt             InitialOwner
);

// ******************************************************************
// * NtCreateSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(193) ntstatus_xt NTAPI NtCreateSemaphore
(
    OUT PHANDLE             SemaphoreHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  ulong_xt               InitialCount,
    IN  ulong_xt               MaximumCount
);

// ******************************************************************
// * 0x00C2 - NtCreateTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(194) ntstatus_xt NTAPI NtCreateTimer
(
	OUT PHANDLE TimerHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN TIMER_TYPE TimerType
);

// ******************************************************************
// * 0x00C3 - NtDeleteFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(195) ntstatus_xt NTAPI NtDeleteFile
(
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

// ******************************************************************
// * 0x00C4 - NtDeviceIoControlFile
// ******************************************************************
XBSYSAPI EXPORTNUM(196) ntstatus_xt NTAPI NtDeviceIoControlFile
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
);

// ******************************************************************
// * NtDuplicateObject
// ******************************************************************
XBSYSAPI EXPORTNUM(197) ntstatus_xt NTAPI NtDuplicateObject
(
    PVOID   SourceHandle,
    PVOID  *TargetHandle,
    dword_xt   Options
);

// ******************************************************************
// * NtFlushBuffersFile
// ******************************************************************
XBSYSAPI EXPORTNUM(198) ntstatus_xt NTAPI NtFlushBuffersFile
(
    PVOID                FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
);

// ******************************************************************
// * NtFreeVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(199) ntstatus_xt NTAPI NtFreeVirtualMemory
(
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG FreeSize,
    IN ulong_xt      FreeType
);

// ******************************************************************
// * 0x00C8 - NtFsControlFile
// ******************************************************************
XBSYSAPI EXPORTNUM(200) ntstatus_xt NTAPI NtFsControlFile
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
);

XBSYSAPI EXPORTNUM(201) ntstatus_xt NTAPI NtOpenDirectoryObject
(
	OUT PHANDLE DirectoryHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

// ******************************************************************
// * NtOpenFile
// ******************************************************************
XBSYSAPI EXPORTNUM(202) ntstatus_xt NTAPI NtOpenFile
(
    OUT PHANDLE             FileHandle,
    IN  access_mask_xt         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ulong_xt               ShareAccess,
    IN  ulong_xt               OpenOptions
);

XBSYSAPI EXPORTNUM(203) ntstatus_xt NTAPI NtOpenSymbolicLinkObject(
	OUT PHANDLE LinkHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

XBSYSAPI EXPORTNUM(205) ntstatus_xt NTAPI NtProtectVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ulong_xt NewProtect,
    OUT PULONG OldProtect
);


// ******************************************************************
// * NtPulseEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(205) ntstatus_xt NTAPI NtPulseEvent
(
    IN HANDLE                       EventHandle,
    OUT PLONG                       PreviousState OPTIONAL
);

// ******************************************************************
// * NtQueueApcThread
// ******************************************************************
XBSYSAPI EXPORTNUM(206) ntstatus_xt NTAPI NtQueueApcThread
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN PVOID                ApcReserved OPTIONAL
);

// ******************************************************************
// * NtQueryDirectoryFile
// ******************************************************************
XBSYSAPI EXPORTNUM(207) ntstatus_xt NTAPI NtQueryDirectoryFile
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
);

// ******************************************************************
// * 0x00D0 - NtQueryDirectoryObject
// ******************************************************************
XBSYSAPI EXPORTNUM(208) ntstatus_xt NTAPI NtQueryDirectoryObject(
	IN HANDLE DirectoryHandle,
	OUT PVOID Buffer,
	IN ulong_xt Length,
	IN boolean_xt RestartScan,
	IN OUT PULONG Context,
	OUT PULONG ReturnedLength OPTIONAL
);

// ******************************************************************
// * 0x00D1  - NtQueryEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(209) ntstatus_xt NTAPI NtQueryEvent
(
	IN HANDLE EventHandle,
	OUT PEVENT_BASIC_INFORMATION EventInformation
);

// ******************************************************************
// * NtQueryFullAttributesFile
// ******************************************************************
XBSYSAPI EXPORTNUM(210) ntstatus_xt NTAPI NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT PFILE_NETWORK_OPEN_INFORMATION  FileInformation
);

// ******************************************************************
// * NtQueryInformationFile
// ******************************************************************
// *
// * Return various kinds of information about a given file object.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(211) ntstatus_xt NTAPI NtQueryInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PVOID                       FileInformation,
    IN  ulong_xt                       Length,
    IN  FILE_INFORMATION_CLASS      FileInfo
);

// ******************************************************************
// * 0x00D4 - NtQueryIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(212) ntstatus_xt NTAPI NtQueryIoCompletion
(
	IN HANDLE IoCompletionHandle,
	OUT PIO_COMPLETION_BASIC_INFORMATION IoCompletionInformation
);

// ******************************************************************
// * 0x00D5 - NtQueryMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(213) ntstatus_xt NTAPI NtQueryMutant
(
	IN HANDLE MutantHandle,
	OUT PMUTANT_BASIC_INFORMATION MutantInformation
);

// ******************************************************************
// * 0x00D6 - NtQuerySemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(214) ntstatus_xt NTAPI NtQuerySemaphore
(
	IN HANDLE SemaphoreHandle,
	OUT PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
);

// ******************************************************************
// * 0x00D7 - NtQuerySymbolicLinkObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(215) ntstatus_xt NTAPI NtQuerySymbolicLinkObject
( 
    HANDLE LinkHandle, 
    OUT PSTRING LinkTarget, 
    OUT PULONG ReturnedLength OPTIONAL
);

// ******************************************************************
// * 0x00D8 - NtQueryTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(216) ntstatus_xt NTAPI NtQueryTimer
(
	IN HANDLE TimerHandle,
	OUT PTIMER_BASIC_INFORMATION TimerInformation
);

// ******************************************************************
// * NtQueryVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(217) ntstatus_xt NTAPI NtQueryVirtualMemory
(
    IN  PVOID                       BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION   Buffer
);

// ******************************************************************
// * NtQueryVolumeInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(218) ntstatus_xt NTAPI NtQueryVolumeInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
    IN  ulong_xt                       Length,
    IN  FS_INFORMATION_CLASS        FileInformationClass
);

// ******************************************************************
// * NtReadFile
// ******************************************************************
// *
// * Read data from an opened file.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(219) ntstatus_xt NTAPI NtReadFile
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  HANDLE          Event OPTIONAL,
    IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN  PVOID           ApcContext,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID           Buffer,
    IN  ulong_xt           Length,
    IN  PLARGE_INTEGER  ByteOffset OPTIONAL
);

// ******************************************************************
// * 0x00DC - NtReadFileScatter
// ******************************************************************
XBSYSAPI EXPORTNUM(220) ntstatus_xt NTAPI NtReadFileScatter
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ulong_xt Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
);

// ******************************************************************
// * NtReleaseMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(221) ntstatus_xt NTAPI NtReleaseMutant
(
    IN  HANDLE  MutantHandle,
    OUT PLONG   PreviousCount
);

// ******************************************************************
// * NtReleaseSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(222) ntstatus_xt NTAPI NtReleaseSemaphore
(
    IN  HANDLE  SemaphoreHandle,
    IN  ulong_xt   ReleaseCount,
    OUT PULONG  PreviousCount OPTIONAL
);

// ******************************************************************
// * 0x00DF - NtRemoveIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(223) ntstatus_xt NTAPI NtRemoveIoCompletion
(
	IN HANDLE IoCompletionHandle,
	OUT PVOID *KeyContext,
	OUT PVOID *ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER Timeout OPTIONAL
);

// ******************************************************************
// * NtResumeThread
// ******************************************************************
XBSYSAPI EXPORTNUM(224) ntstatus_xt NTAPI NtResumeThread
(
    IN  HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount
);

// ******************************************************************
// * NtSetEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(225) ntstatus_xt NTAPI NtSetEvent
(
    IN  HANDLE  EventHandle,
    OUT PLONG   PreviousState
);

// ******************************************************************
// * NtSetInformationFile
// ******************************************************************
// *
// * Changes various kinds of information about a given file object.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(226) ntstatus_xt NTAPI NtSetInformationFile
(
    IN  HANDLE  FileHandle,            // TODO: correct paramters
    OUT PIO_STATUS_BLOCK   IoStatusBlock,
    IN  PVOID   FileInformation,
    IN  ulong_xt   Length,
    IN  FILE_INFORMATION_CLASS   FileInformationClass
);

// ******************************************************************
// * 0x00E3 - NtSetIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(227) ntstatus_xt NTAPI NtSetIoCompletion
(
	IN HANDLE IoCompletionHandle,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN ntstatus_xt IoStatus,
	IN ulong_ptr_xt IoStatusInformation
);

// ******************************************************************
// * NtSetSystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(228) ntstatus_xt NTAPI NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL 
);

// ******************************************************************
// * 0x00E5 - NtSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(229) ntstatus_xt NTAPI NtSetTimerEx
(
	IN HANDLE TimerHandle,
	IN PLARGE_INTEGER DueTime,
	IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode,
	IN PVOID TimerContext OPTIONAL,
	IN boolean_xt WakeTimer,
	IN long_xt Period OPTIONAL,
	OUT PBOOLEAN PreviousState OPTIONAL
);

// ******************************************************************
// * 0x00E6 - NtSignalAndWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(230) ntstatus_xt NTAPI NtSignalAndWaitForSingleObjectEx
(
	IN HANDLE SignalHandle,
	IN HANDLE WaitHandle,
	IN KPROCESSOR_MODE WaitMode,
	IN boolean_xt Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL
);

// ******************************************************************
// * NtSuspendThread
// ******************************************************************
XBSYSAPI EXPORTNUM(231) ntstatus_xt NTAPI NtSuspendThread
(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  PreviousSuspendCount OPTIONAL
);

// ******************************************************************
// * NtUserIoApcDispatcher
// ******************************************************************
XBSYSAPI EXPORTNUM(232) void_xt NTAPI NtUserIoApcDispatcher
(
    PVOID            ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ulong_xt            Reserved
);

// ******************************************************************
// * NtWaitForSingleObject
// ******************************************************************
// *
// * Put the current thread into a wait state until the given
// * dispatcher object is set to a signaled state or (optionally)
// * until the wait times out.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(233) ntstatus_xt NTAPI NtWaitForSingleObject
(
    IN  HANDLE  Handle,
    IN  boolean_xt Alertable,
	IN	PLARGE_INTEGER Timeout
);

// ******************************************************************
// * NtWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(234) ntstatus_xt NTAPI NtWaitForSingleObjectEx
(
    IN  HANDLE          Handle,
    IN  char_xt          WaitMode,
    IN  boolean_xt         Alertable,
    IN  PLARGE_INTEGER  Timeout
);

// ******************************************************************
// * NtWaitForMultipleObjectsEx
// ******************************************************************
XBSYSAPI EXPORTNUM(235) ntstatus_xt NTAPI NtWaitForMultipleObjectsEx
(
    IN  ulong_xt           Count,
    IN  HANDLE         *Handles,
    IN  WAIT_TYPE       WaitType,
    IN  char_xt          WaitMode,
    IN  boolean_xt         Alertable,
    IN  PLARGE_INTEGER  Timeout
);

// ******************************************************************
// * NtWriteFile
// ******************************************************************
// *
// * Write data to an opened file.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(236) ntstatus_xt NTAPI NtWriteFile
(
	IN  HANDLE          FileHandle,
	IN  HANDLE          Event,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN  PVOID           Buffer,
	IN  ulong_xt           Length,
	IN  PLARGE_INTEGER  ByteOffset OPTIONAL
);

// ******************************************************************
// * 0x00ED - NtWriteFileGather
// ******************************************************************
XBSYSAPI EXPORTNUM(237) ntstatus_xt NTAPI NtWriteFileGather
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ulong_xt Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
);

// ******************************************************************
// * NtYieldExecution
// ******************************************************************
XBSYSAPI EXPORTNUM(238) void_xt NTAPI NtYieldExecution(void);

}

#endif


