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

#define NtCurrentThread() ((HANDLE)-2)

// ******************************************************************
// * NtAllocateVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(184) NTSTATUS NTAPI NtAllocateVirtualMemory
(
    IN OUT PVOID    *BaseAddress,
    IN ULONG         ZeroBits,
    IN OUT PULONG    AllocationSize,
    IN DWORD         AllocationType,
    IN DWORD         Protect
);

// ******************************************************************
// * 0x00B9 - NtCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(185) NTSTATUS NTAPI NtCancelTimer
(
	IN HANDLE TimerHandle,
	OUT PBOOLEAN CurrentState OPTIONAL
);

// ******************************************************************
// * NtClearEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(186) NTSTATUS NTAPI NtClearEvent
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
XBSYSAPI EXPORTNUM(187) NTSTATUS NTAPI NtClose
(
    IN HANDLE Handle
);

XBSYSAPI EXPORTNUM(188) NTSTATUS NTAPI NtCreateDirectoryObject
(
	OUT PHANDLE             DirectoryHandle,
	IN  POBJECT_ATTRIBUTES  ObjectAttributes
);

// ******************************************************************
// * NtCreateEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(189) NTSTATUS NTAPI NtCreateEvent
(
    OUT PHANDLE             EventHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  BOOLEAN             InitialState
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
XBSYSAPI EXPORTNUM(190) NTSTATUS NTAPI NtCreateFile
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
);

XBSYSAPI EXPORTNUM(191) NTSTATUS NTAPI NtCreateIoCompletion
(
	OUT PHANDLE IoCompletionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN ULONG Count
);

// ******************************************************************
// * NtCreateMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(192) NTSTATUS NTAPI NtCreateMutant
(
    OUT PHANDLE             MutantHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  BOOLEAN             InitialOwner
);

// ******************************************************************
// * NtCreateSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(193) NTSTATUS NTAPI NtCreateSemaphore
(
    OUT PHANDLE             SemaphoreHandle,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  ULONG               InitialCount,
    IN  ULONG               MaximumCount
);

// ******************************************************************
// * 0x00C2 - NtCreateTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(194) NTSTATUS NTAPI NtCreateTimer
(
	OUT PHANDLE TimerHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN TIMER_TYPE TimerType
);

// ******************************************************************
// * 0x00C3 - NtDeleteFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(195) NTSTATUS NTAPI NtDeleteFile
(
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

// ******************************************************************
// * 0x00C4 - NtDeviceIoControlFile
// ******************************************************************
XBSYSAPI EXPORTNUM(196) NTSTATUS NTAPI NtDeviceIoControlFile
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
);

// ******************************************************************
// * NtDuplicateObject
// ******************************************************************
XBSYSAPI EXPORTNUM(197) NTSTATUS NTAPI NtDuplicateObject
(
    PVOID   SourceHandle,
    PVOID  *TargetHandle,
    DWORD   Options
);

// ******************************************************************
// * NtFlushBuffersFile
// ******************************************************************
XBSYSAPI EXPORTNUM(198) NTSTATUS NTAPI NtFlushBuffersFile
(
    PVOID                FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock
);

// ******************************************************************
// * NtFreeVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(199) NTSTATUS NTAPI NtFreeVirtualMemory
(
    IN OUT PVOID *BaseAddress,
    IN OUT PULONG FreeSize,
    IN ULONG      FreeType
);

// ******************************************************************
// * 0x00C8 - NtFsControlFile
// ******************************************************************
XBSYSAPI EXPORTNUM(200) NTSTATUS NTAPI NtFsControlFile
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
);

XBSYSAPI EXPORTNUM(201) NTSTATUS NTAPI NtOpenDirectoryObject
(
	OUT PHANDLE DirectoryHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

// ******************************************************************
// * NtOpenFile
// ******************************************************************
XBSYSAPI EXPORTNUM(202) NTSTATUS NTAPI NtOpenFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               OpenOptions
);

XBSYSAPI EXPORTNUM(203) NTSTATUS NTAPI NtOpenSymbolicLinkObject(
	OUT PHANDLE LinkHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

XBSYSAPI EXPORTNUM(205) NTSTATUS NTAPI NtProtectVirtualMemory(
    IN OUT PVOID *BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
);


// ******************************************************************
// * NtPulseEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(205) NTSTATUS NTAPI NtPulseEvent
(
    IN HANDLE                       EventHandle,
    OUT PLONG                       PreviousState OPTIONAL
);

// ******************************************************************
// * NtQueueApcThread
// ******************************************************************
XBSYSAPI EXPORTNUM(206) NTSTATUS NTAPI NtQueueApcThread
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN ULONG                ApcReserved OPTIONAL 
);

// ******************************************************************
// * NtQueryDirectoryFile
// ******************************************************************
XBSYSAPI EXPORTNUM(207) NTSTATUS NTAPI NtQueryDirectoryFile
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
);

// ******************************************************************
// * 0x00D0 - NtQueryDirectoryObject
// ******************************************************************
XBSYSAPI EXPORTNUM(208) NTSTATUS NTAPI NtQueryDirectoryObject(
	IN HANDLE DirectoryHandle,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN BOOLEAN RestartScan,
	IN OUT PULONG Context,
	OUT PULONG ReturnedLength OPTIONAL
);

// ******************************************************************
// * 0x00D1  - NtQueryEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(209) NTSTATUS NTAPI NtQueryEvent
(
	IN HANDLE EventHandle,
	OUT PEVENT_BASIC_INFORMATION EventInformation
);

// ******************************************************************
// * NtQueryFullAttributesFile
// ******************************************************************
XBSYSAPI EXPORTNUM(210) NTSTATUS NTAPI NtQueryFullAttributesFile
(
	IN  POBJECT_ATTRIBUTES          ObjectAttributes,
	OUT PFILE_NETWORK_OPEN_INFORMATION  Attributes
);

// ******************************************************************
// * NtQueryInformationFile
// ******************************************************************
// *
// * Return various kinds of information about a given file object.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(211) NTSTATUS NTAPI NtQueryInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PVOID                       FileInformation,
    IN  ULONG                       Length,
    IN  FILE_INFORMATION_CLASS      FileInfo
);

// ******************************************************************
// * 0x00D4 - NtQueryIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(212) NTSTATUS NTAPI NtQueryIoCompletion
(
	IN HANDLE IoCompletionHandle,
	OUT PIO_COMPLETION_BASIC_INFORMATION IoCompletionInformation
);

// ******************************************************************
// * 0x00D5 - NtQueryMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(213) NTSTATUS NTAPI NtQueryMutant
(
	IN HANDLE MutantHandle,
	OUT PMUTANT_BASIC_INFORMATION MutantInformation
);

// ******************************************************************
// * 0x00D6 - NtQuerySemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(214) NTSTATUS NTAPI NtQuerySemaphore
(
	IN HANDLE SemaphoreHandle,
	OUT PSEMAPHORE_BASIC_INFORMATION SemaphoreInformation
);

// ******************************************************************
// * 0x00D7 - NtQuerySymbolicLinkObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(215) NTSTATUS NTAPI NtQuerySymbolicLinkObject
( 
    HANDLE LinkHandle, 
    OUT PSTRING LinkTarget, 
    OUT PULONG ReturnedLength OPTIONAL
);

// ******************************************************************
// * 0x00D8 - NtQueryTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(216) NTSTATUS NTAPI NtQueryTimer
(
	IN HANDLE TimerHandle,
	OUT PTIMER_BASIC_INFORMATION TimerInformation
);

// ******************************************************************
// * NtQueryVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(217) NTSTATUS NTAPI NtQueryVirtualMemory
(
    IN  PVOID                       BaseAddress,
    OUT PMEMORY_BASIC_INFORMATION   Buffer
);

// ******************************************************************
// * NtQueryVolumeInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(218) NTSTATUS NTAPI NtQueryVolumeInformationFile
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
    IN  ULONG                       Length,
    IN  FS_INFORMATION_CLASS        FileInformationClass
);

// ******************************************************************
// * NtReadFile
// ******************************************************************
// *
// * Read data from an opened file.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(219) NTSTATUS NTAPI NtReadFile
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  HANDLE          Event OPTIONAL,
    IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN  PVOID           ApcContext,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID           Buffer,
    IN  ULONG           Length,
    IN  PLARGE_INTEGER  ByteOffset OPTIONAL
);

// ******************************************************************
// * 0x00DC - NtReadFileScatter
// ******************************************************************
XBSYSAPI EXPORTNUM(220) NTSTATUS NTAPI NtReadFileScatter
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
);

// ******************************************************************
// * NtReleaseMutant
// ******************************************************************
XBSYSAPI EXPORTNUM(221) NTSTATUS NTAPI NtReleaseMutant
(
    IN  HANDLE  MutantHandle,
    OUT PLONG   PreviousCount
);

// ******************************************************************
// * NtReleaseSemaphore
// ******************************************************************
XBSYSAPI EXPORTNUM(222) NTSTATUS NTAPI NtReleaseSemaphore
(
    IN  HANDLE  SemaphoreHandle,
    IN  ULONG   ReleaseCount,
    OUT PULONG  PreviousCount OPTIONAL
);

// ******************************************************************
// * 0x00DF - NtRemoveIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(223) NTSTATUS NTAPI NtRemoveIoCompletion
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
XBSYSAPI EXPORTNUM(224) NTSTATUS NTAPI NtResumeThread
(
    IN  HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount
);

// ******************************************************************
// * NtSetEvent
// ******************************************************************
XBSYSAPI EXPORTNUM(225) NTSTATUS NTAPI NtSetEvent
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
XBSYSAPI EXPORTNUM(226) NTSTATUS NTAPI NtSetInformationFile
(
    IN  HANDLE  FileHandle,            // TODO: correct paramters
    OUT PIO_STATUS_BLOCK   IoStatusBlock,
    IN  PVOID   FileInformation,
    IN  ULONG   Length,
    IN  FILE_INFORMATION_CLASS   FileInformationClass
);

// ******************************************************************
// * 0x00E3 - NtSetIoCompletion
// ******************************************************************
XBSYSAPI EXPORTNUM(227) NTSTATUS NTAPI NtSetIoCompletion
(
	IN HANDLE IoCompletionHandle,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN NTSTATUS IoStatus,
	IN ULONG_PTR IoStatusInformation
);

// ******************************************************************
// * NtSetSystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(228) NTSTATUS NTAPI NtSetSystemTime
(
	IN  PLARGE_INTEGER			SystemTime,
	OUT PLARGE_INTEGER			PreviousTime OPTIONAL 
);

// ******************************************************************
// * 0x00E5 - NtSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(229) NTSTATUS NTAPI NtSetTimerEx
(
	IN HANDLE TimerHandle,
	IN PLARGE_INTEGER DueTime,
	IN PTIMER_APC_ROUTINE TimerApcRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode,
	IN PVOID TimerContext OPTIONAL,
	IN BOOLEAN WakeTimer,
	IN LONG Period OPTIONAL,
	OUT PBOOLEAN PreviousState OPTIONAL
);

// ******************************************************************
// * 0x00E6 - NtSignalAndWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(230) NTSTATUS NTAPI NtSignalAndWaitForSingleObjectEx
(
	IN HANDLE SignalHandle,
	IN HANDLE WaitHandle,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL
);

// ******************************************************************
// * NtSuspendThread
// ******************************************************************
XBSYSAPI EXPORTNUM(231) NTSTATUS NTAPI NtSuspendThread
(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  PreviousSuspendCount OPTIONAL
);

// ******************************************************************
// * NtUserIoApcDispatcher
// ******************************************************************
XBSYSAPI EXPORTNUM(232) void_t NTAPI NtUserIoApcDispatcher
(
    PVOID            ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG            Reserved
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
XBSYSAPI EXPORTNUM(233) NTSTATUS NTAPI NtWaitForSingleObject
(
    IN  HANDLE  Handle,
    IN  BOOLEAN Alertable,
	IN	PLARGE_INTEGER Timeout
);

// ******************************************************************
// * NtWaitForSingleObjectEx
// ******************************************************************
XBSYSAPI EXPORTNUM(234) NTSTATUS NTAPI NtWaitForSingleObjectEx
(
    IN  HANDLE          Handle,
    IN  char_t          WaitMode,
    IN  BOOLEAN         Alertable,
    IN  PLARGE_INTEGER  Timeout
);

// ******************************************************************
// * NtWaitForMultipleObjectsEx
// ******************************************************************
XBSYSAPI EXPORTNUM(235) NTSTATUS NTAPI NtWaitForMultipleObjectsEx
(
    IN  ULONG           Count,
    IN  HANDLE         *Handles,
    IN  WAIT_TYPE       WaitType,
    IN  char_t          WaitMode,
    IN  BOOLEAN         Alertable,
    IN  PLARGE_INTEGER  Timeout
);

// ******************************************************************
// * NtWriteFile
// ******************************************************************
// *
// * Write data to an opened file.
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(236) NTSTATUS NTAPI NtWriteFile
(
	IN  HANDLE          FileHandle,
	IN  HANDLE          Event,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID           ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN  PVOID           Buffer,
	IN  ULONG           Length,
	IN  PLARGE_INTEGER  ByteOffset OPTIONAL
);

// ******************************************************************
// * 0x00ED - NtWriteFileGather
// ******************************************************************
XBSYSAPI EXPORTNUM(237) NTSTATUS NTAPI NtWriteFileGather
(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PFILE_SEGMENT_ELEMENT SegmentArray,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL
);

// ******************************************************************
// * NtYieldExecution
// ******************************************************************
XBSYSAPI EXPORTNUM(238) void_t NTAPI NtYieldExecution(void);

#endif


