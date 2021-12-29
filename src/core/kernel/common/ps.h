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

#include "types.h"

namespace xbox
{

#define PsGetCurrentThread() (CONTAINING_RECORD((KeGetCurrentThread()),ETHREAD,Tcb))

// ******************************************************************
// * PsCreateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(254) ntstatus_xt NTAPI PsCreateSystemThread
(
	OUT PHANDLE         ThreadHandle,
	OUT PHANDLE          ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_xt         DebuggerThread
);

// ******************************************************************
// * PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) ntstatus_xt NTAPI PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ulong_xt           ThreadExtensionSize,
	IN  ulong_xt           KernelStackSize,
	IN  ulong_xt           TlsDataSize,
	OUT PHANDLE          ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_xt         CreateSuspended,
	IN  boolean_xt         DebuggerThread,
	IN  PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
);

// ******************************************************************
// * 0x0100 - PsQueryStatistics()
// ******************************************************************
XBSYSAPI EXPORTNUM(256) ntstatus_xt NTAPI PsQueryStatistics
(
	IN OUT PPS_STATISTICS ProcessStatistics
);

// ******************************************************************
// * PsSetCreateThreadNotifyRoutine
// ******************************************************************
XBSYSAPI EXPORTNUM(257) ntstatus_xt NTAPI PsSetCreateThreadNotifyRoutine
(
	IN PCREATE_THREAD_NOTIFY_ROUTINE NotifyRoutine
);

// ******************************************************************
// * PsTerminateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(258) void_xt NTAPI PsTerminateSystemThread(IN ntstatus_xt ExitStatus);

XBSYSAPI EXPORTNUM(259) OBJECT_TYPE PsThreadObjectType;

PETHREAD PspGetCurrentThread();

}

#endif


