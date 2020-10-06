// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : kernel.h
// *
// * note : XBox Kernel Declarations
// *
// ******************************************************************
#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

namespace xbox
{

// ******************************************************************
// * 0x005C - KeAlertResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(92) ntstatus_xt NTAPI KeAlertResumeThread
(
	IN HANDLE ThreadHandle,
	IN OUT PULONG PreviousSuspendCount
);

// ******************************************************************
// * 0x005D - KeAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(93) ntstatus_xt NTAPI KeAlertThread
(
	IN HANDLE ThreadHandle
);

// ******************************************************************
// * 0x005E - KeBoostPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(94) ntstatus_xt NTAPI KeBoostPriorityThread
(
	IN PKTHREAD Thread,
	IN KPRIORITY Increment
);

// ******************************************************************
// * 0x005F - KeBugCheck()
// ******************************************************************
XBSYSAPI EXPORTNUM(95) void_xt NTAPI KeBugCheck
(
	IN ulong_xt BugCheckMode
);

// ******************************************************************
// * 0x0060 - KeBugCheckEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(96) ntstatus_xt NTAPI KeBugCheckEx
(
	IN dword_xt BugCheckCode,
	IN PVOID BugCheckParameter1,
	IN PVOID BugCheckParameter2,
	IN PVOID BugCheckParameter3,
	IN PVOID BugCheckParameter4
);

// ******************************************************************
// * 0x0061 - KeCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(97) boolean_xt NTAPI KeCancelTimer
(
	IN PKTIMER
);

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) boolean_xt NTAPI KeConnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(99) ntstatus_xt NTAPI KeDelayExecutionThread
(
    IN KPROCESSOR_MODE  WaitMode,
    IN boolean_xt          Alertable,
    IN PLARGE_INTEGER   Interval
);

// ******************************************************************
// * 0x0064 - KeDisconnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(100) void_xt NTAPI KeDisconnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

// ******************************************************************
// * 0x0065 - KeEnterCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(101) void_xt NTAPI KeEnterCriticalRegion
(
	void_xt
);

// ******************************************************************
// * 0x0067 - KeGetCurrentIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(103) KIRQL NTAPI KeGetCurrentIrql(void);

// ******************************************************************
// * 0x0068 - KeGetCurrentThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(104) PKTHREAD NTAPI KeGetCurrentThread(void);

// ******************************************************************
// * 0x0069 - KeInitializeApc()
// ******************************************************************
XBSYSAPI EXPORTNUM(105) void_xt NTAPI KeInitializeApc
(
	IN PKAPC Apc,
	IN PKTHREAD Thread,
	IN PKKERNEL_ROUTINE KernelRoutine,
	IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
	IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
	IN KPROCESSOR_MODE ApcMode OPTIONAL,
	IN PVOID NormalContext OPTIONAL
);

// ******************************************************************
// * 0x006A - KeInitializeDeviceQueue()
// ******************************************************************
XBSYSAPI EXPORTNUM(106) void_xt NTAPI KeInitializeDeviceQueue
(
	OUT PKDEVICE_QUEUE DeviceQueue
);

// ******************************************************************
// * 0x006B - KeInitializeDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(107) void_xt NTAPI KeInitializeDpc
(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
);

// ******************************************************************
// * 0x006C - KeInitializeEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(108) void_xt NTAPI KeInitializeEvent
(
	IN PRKEVENT Event,
	IN EVENT_TYPE Type,
	IN boolean_xt SignalState
);

// ******************************************************************
// * 0x006D - KeInitializeInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(109) void_xt NTAPI KeInitializeInterrupt
(
    OUT PKINTERRUPT Interrupt,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN ulong_xt Vector,
    IN KIRQL Irql,
    IN KINTERRUPT_MODE InterruptMode,
    IN boolean_xt ShareVector
);

// ******************************************************************
// * 0x006E - KeInitializeMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(110) void_xt NTAPI KeInitializeMutant
(
	IN PRKMUTANT Mutant,
	IN boolean_xt InitialOwner
);

// ******************************************************************
// * 0x006F - KeInitializeQueue()
// ******************************************************************
XBSYSAPI EXPORTNUM(111) void_xt NTAPI KeInitializeQueue
(
	IN PKQUEUE Queue,
	IN ulong_xt Count OPTIONAL
);

// ******************************************************************
// * 0x0070 - KeInitializeSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(112) void_xt NTAPI KeInitializeSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN long_xt Count,
	IN long_xt Limit
);

// ******************************************************************
// * 0x0071 - KeInitializeTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(113) void_xt NTAPI KeInitializeTimerEx
(
    IN PKTIMER      Timer,
    IN TIMER_TYPE   Type
);

XBSYSAPI EXPORTNUM(114) boolean_xt NTAPI KeInsertByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
	IN ulong_xt SortKey
);

XBSYSAPI EXPORTNUM(115) boolean_xt NTAPI KeInsertDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
);

XBSYSAPI EXPORTNUM(116) long_xt NTAPI KeInsertHeadQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
);

XBSYSAPI EXPORTNUM(117) long_xt NTAPI KeInsertQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
);

XBSYSAPI EXPORTNUM(118) boolean_xt NTAPI KeInsertQueueApc
(
	IN PRKAPC Apc,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2,
	IN KPRIORITY Increment
);

// ******************************************************************
// * 0x0077 - KeInsertQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(119) boolean_xt NTAPI KeInsertQueueDpc
(
    IN PKDPC        Dpc,
    IN PVOID        SystemArgument1,
    IN PVOID        SystemArgument2
);

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
XBSYSAPI EXPORTNUM(120) KSYSTEM_TIME KeInterruptTime;

// ******************************************************************
// * 0x0079 - KeIsExecutingDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(121) boolean_xt NTAPI KeIsExecutingDpc();

// ******************************************************************
// * 0x007A - KeLeaveCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(122) void_xt NTAPI KeLeaveCriticalRegion
(
	void_xt
);

XBSYSAPI EXPORTNUM(123) long_xt NTAPI KePulseEvent
(
	IN PRKEVENT Event,
	IN KPRIORITY Increment,
	IN boolean_xt Wait
);

XBSYSAPI EXPORTNUM(124) long_xt NTAPI KeQueryBasePriorityThread
(
	IN PKTHREAD Thread
);

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) ulonglong_xt NTAPI KeQueryInterruptTime(void);

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
// ******************************************************************
XBSYSAPI EXPORTNUM(126) ulonglong_xt NTAPI KeQueryPerformanceCounter(void);

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) ulonglong_xt NTAPI KeQueryPerformanceFrequency(void);

// ******************************************************************
// * 0x0080 - KeQuerySystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(128) void_xt NTAPI KeQuerySystemTime
(
    PLARGE_INTEGER CurrentTime
);

// ******************************************************************
// * 0x0081 - KeRaiseIrqlToDpcLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(129) uchar_xt NTAPI KeRaiseIrqlToDpcLevel();

// ******************************************************************
// * 0x0082 - KeRaiseIrqlToSynchLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(130) uchar_xt NTAPI KeRaiseIrqlToSynchLevel();

XBSYSAPI EXPORTNUM(131) long_xt NTAPI KeReleaseMutant
(
	IN PRKMUTANT Mutant,
	IN KPRIORITY Increment,
	IN boolean_xt Abandoned,
	IN boolean_xt Wait
);

XBSYSAPI EXPORTNUM(132) long_xt NTAPI KeReleaseSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN KPRIORITY Increment,
	IN long_xt Adjustment,
	IN boolean_xt Wait
);

XBSYSAPI EXPORTNUM(133) PKDEVICE_QUEUE_ENTRY NTAPI KeRemoveByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN ulong_xt SortKey
);

XBSYSAPI EXPORTNUM(134) PKDEVICE_QUEUE_ENTRY NTAPI KeRemoveDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue
);

XBSYSAPI EXPORTNUM(135) boolean_xt NTAPI KeRemoveEntryDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
);

XBSYSAPI EXPORTNUM(136) PLIST_ENTRY NTAPI KeRemoveQueue
(
	IN PRKQUEUE Queue,
	IN KPROCESSOR_MODE WaitMode,
	IN PLARGE_INTEGER Timeout
);

// ******************************************************************
// * 0x0089 - KeRemoveQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(137) boolean_xt NTAPI KeRemoveQueueDpc
(
	IN PKDPC Dpc
);

// ******************************************************************
// * 0x008A - KeResetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(138) long_xt NTAPI KeResetEvent
(
	IN PRKEVENT Event
);

// ******************************************************************
// * 0x008B - KeRestoreFloatingPointState()
// ******************************************************************
XBSYSAPI EXPORTNUM(139) ntstatus_xt NTAPI KeRestoreFloatingPointState
(
	IN PKFLOATING_SAVE     PublicFloatSave
);

// ******************************************************************
// * 0x008C - KeResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(140) ulong_xt NTAPI KeResumeThread
(
	IN PKTHREAD Thread
);

XBSYSAPI EXPORTNUM(141) PLIST_ENTRY NTAPI KeRundownQueue
(
	IN PRKQUEUE Queue
);

// ******************************************************************
// * 0x008E - KeSaveFloatingPointState()
// ******************************************************************
XBSYSAPI EXPORTNUM(142) ntstatus_xt NTAPI KeSaveFloatingPointState
(
	OUT PKFLOATING_SAVE     PublicFloatSave
);

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) long_xt NTAPI KeSetBasePriorityThread
(
    IN PKTHREAD  Thread,
    IN long_xt	Priority
);

XBSYSAPI EXPORTNUM(144) ulong_xt NTAPI KeSetDisableBoostThread
(
	IN PKTHREAD Thread,
	IN ulong_xt Disable
);

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) long_xt NTAPI KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN boolean_xt		Wait
);

XBSYSAPI EXPORTNUM(146) void_xt NTAPI KeSetEventBoostPriority
(
	IN PRKEVENT Event,
	IN PRKTHREAD *Thread
);

XBSYSAPI EXPORTNUM(147) KPRIORITY NTAPI KeSetPriorityProcess
(
	IN PKPROCESS Process,
	IN KPRIORITY BasePriority
);

// ******************************************************************
// * 0x0094 - KeSetPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(148) boolean_xt NTAPI KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN long_xt  Priority
);

// ******************************************************************
// * 0x0095 - KeSetTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(149) boolean_xt NTAPI KeSetTimer
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * 0x0096 - KeSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(150) boolean_xt NTAPI KeSetTimerEx
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN long_xt           Period OPTIONAL,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * 0x0097 - KeStallExecutionProcessor()
// ******************************************************************
XBSYSAPI EXPORTNUM(151) void_xt NTAPI KeStallExecutionProcessor
(
	IN ulong_xt MicroSeconds
);

// ******************************************************************
// * 0x0098 - KeSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(152) ulong_xt NTAPI KeSuspendThread
(
	IN PKTHREAD Thread
);

// ******************************************************************
// * 0x0099 - KeSynchronizeExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(153) boolean_xt NTAPI KeSynchronizeExecution
(
	IN PKINTERRUPT Interrupt,
	IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
	IN PVOID SynchronizeContext
);

// ******************************************************************
// * 0x009A - KeSystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(154) KSYSTEM_TIME KeSystemTime;

// ******************************************************************
// * 0x009B - KeTestAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(155) boolean_xt NTAPI KeTestAlertThread
(
	IN KPROCESSOR_MODE AlertMode
);

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) volatile dword_xt KeTickCount;

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) ulong_xt KeTimeIncrement;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects()
// ******************************************************************
XBSYSAPI EXPORTNUM(158) ntstatus_xt NTAPI KeWaitForMultipleObjects
(
    IN ulong_xt Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN boolean_xt Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK	WaitBlockArray  OPTIONAL
);

// ******************************************************************
// * 0x009F - KeWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(159) ntstatus_xt NTAPI KeWaitForSingleObject
(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN boolean_xt Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

}

#endif


