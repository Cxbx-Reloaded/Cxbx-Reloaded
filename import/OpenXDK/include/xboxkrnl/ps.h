// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : ps.h
// *
// * note : XBox Kernel *Process Structure* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_PS_H
#define XBOXKRNL_PS_H

// ******************************************************************
// * PsCreateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(254) NTSTATUS NTAPI PsCreateSystemThread
(
	OUT PHANDLE         ThreadHandle,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  BOOLEAN         DebuggerThread
);

// ******************************************************************
// * PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) NTSTATUS NTAPI PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ULONG           ThreadExtensionSize,
	IN  ULONG           KernelStackSize,
	IN  ULONG           TlsDataSize,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  BOOLEAN         CreateSuspended,
	IN  BOOLEAN         DebuggerThread,
	IN  PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
);

XBSYSAPI VOID *PsQueryStatistics;

// ******************************************************************
// * PsSetCreateThreadNotifyRoutine
// ******************************************************************
XBSYSAPI EXPORTNUM(257) NTSTATUS NTAPI PsSetCreateThreadNotifyRoutine
(
	IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
);

// ******************************************************************
// * PsTerminateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(258) VOID NTAPI PsTerminateSystemThread(IN NTSTATUS ExitStatus);

XBSYSAPI EXPORTNUM(259) volatile OBJECT_TYPE PsThreadObjectType;

#endif


