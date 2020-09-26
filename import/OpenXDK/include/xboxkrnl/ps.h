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

#include "xboxkrnl/xboxkrnl_types.h"

namespace xbox
{

#define PsGetCurrentThread() (CONTAINING_RECORD((KeGetCurrentThread()),ETHREAD,Tcb))

// ******************************************************************
// * PsCreateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(254) NTSTATUS NTAPI PsCreateSystemThread
(
	OUT PHANDLE         ThreadHandle,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_t         DebuggerThread
);

// ******************************************************************
// * PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) NTSTATUS NTAPI PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ulong_t           ThreadExtensionSize,
	IN  ulong_t           KernelStackSize,
	IN  ulong_t           TlsDataSize,
	OUT PHANDLE         ThreadId OPTIONAL,
	IN  PKSTART_ROUTINE StartRoutine,
	IN  PVOID           StartContext,
	IN  boolean_t         CreateSuspended,
	IN  boolean_t         DebuggerThread,
	IN  PKSYSTEM_ROUTINE SystemRoutine OPTIONAL
);

// ******************************************************************
// * 0x0100 - PsQueryStatistics()
// ******************************************************************
XBSYSAPI EXPORTNUM(256) NTSTATUS NTAPI PsQueryStatistics
(
	IN OUT PPS_STATISTICS ProcessStatistics
);

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
XBSYSAPI EXPORTNUM(258) void_t NTAPI PsTerminateSystemThread(IN NTSTATUS ExitStatus);

XBSYSAPI EXPORTNUM(259) volatile OBJECT_TYPE PsThreadObjectType;

}

#endif


