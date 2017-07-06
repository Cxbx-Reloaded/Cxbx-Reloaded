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

// ******************************************************************
// * 0x005C - KeAlertResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(92) NTSTATUS NTAPI KeAlertResumeThread
(
	IN HANDLE ThreadHandle,
	IN OUT PULONG PreviousSuspendCount
);

// ******************************************************************
// * 0x005D - KeAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(93) NTSTATUS NTAPI KeAlertThread
(
	IN HANDLE ThreadHandle
);

// ******************************************************************
// * 0x005E - KeBoostPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(94) NTSTATUS NTAPI KeBoostPriorityThread
(
	IN PKTHREAD Thread,
	IN KPRIORITY Increment
);

// ******************************************************************
// * 0x005F - KeBugCheck()
// ******************************************************************
XBSYSAPI EXPORTNUM(95) VOID NTAPI KeBugCheck
(
	IN ULONG BugCheckMode
);

// ******************************************************************
// * 0x0060 - KeBugCheckEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(96) NTSTATUS NTAPI KeBugCheckEx
(
	IN DWORD BugCheckCode,
	IN PVOID BugCheckParameter1,
	IN PVOID BugCheckParameter2,
	IN PVOID BugCheckParameter3,
	IN PVOID BugCheckParameter4
);

// ******************************************************************
// * 0x0061 - KeCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(97) BOOLEAN NTAPI KeCancelTimer
(
	IN PKTIMER
);

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) BOOLEAN NTAPI KeConnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(99) NTSTATUS NTAPI KeDelayExecutionThread
(
    IN KPROCESSOR_MODE  WaitMode,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
);

// ******************************************************************
// * 0x0064 - KeDisconnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(100) VOID NTAPI KeDisconnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

// ******************************************************************
// * 0x0065 - KeEnterCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(101) VOID NTAPI KeEnterCriticalRegion
(
	VOID
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
XBSYSAPI EXPORTNUM(105) VOID NTAPI KeInitializeApc
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
XBSYSAPI EXPORTNUM(106) VOID NTAPI KeInitializeDeviceQueue
(
	OUT PKDEVICE_QUEUE DeviceQueue
);

// ******************************************************************
// * 0x006B - KeInitializeDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(107) VOID NTAPI KeInitializeDpc
(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
);

// ******************************************************************
// * 0x006C - KeInitializeEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(108) VOID NTAPI KeInitializeEvent
(
	IN PRKEVENT Event,
	IN EVENT_TYPE Type,
	IN BOOLEAN SignalState
);

// ******************************************************************
// * 0x006D - KeInitializeInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(109) VOID NTAPI KeInitializeInterrupt
(
    OUT PKINTERRUPT Interrupt,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector
);

// ******************************************************************
// * 0x006E - KeInitializeMutant()
// ******************************************************************
XBSYSAPI EXPORTNUM(110) VOID NTAPI KeInitializeMutant
(
	IN PRKMUTANT Mutant,
	IN BOOLEAN InitialOwner
);

// ******************************************************************
// * 0x006F - KeInitializeQueue()
// ******************************************************************
XBSYSAPI EXPORTNUM(111) VOID NTAPI KeInitializeQueue
(
	IN PKQUEUE Queue,
	IN ULONG Count OPTIONAL
);

// ******************************************************************
// * 0x0070 - KeInitializeSemaphore()
// ******************************************************************
XBSYSAPI EXPORTNUM(112) VOID NTAPI KeInitializeSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN LONG Count,
	IN LONG Limit
);

// ******************************************************************
// * 0x0071 - KeInitializeTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(113) VOID NTAPI KeInitializeTimerEx
(
    IN PKTIMER      Timer,
    IN TIMER_TYPE   Type
);

XBSYSAPI EXPORTNUM(114) BOOLEAN NTAPI KeInsertByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
	IN ULONG SortKey
);

XBSYSAPI EXPORTNUM(115) BOOLEAN NTAPI KeInsertDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
);

XBSYSAPI EXPORTNUM(116) LONG NTAPI KeInsertHeadQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
);

XBSYSAPI EXPORTNUM(117) LONG NTAPI KeInsertQueue
(
	IN PRKQUEUE Queue,
	IN PLIST_ENTRY Entry
);

XBSYSAPI EXPORTNUM(118) BOOLEAN NTAPI KeInsertQueueApc
(
	IN PRKAPC Apc,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2,
	IN KPRIORITY Increment
);

// ******************************************************************
// * 0x0077 - KeInsertQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(119) BOOLEAN NTAPI KeInsertQueueDpc
(
    IN PKDPC        Dpc,
    IN PVOID        SystemArgument1,
    IN PVOID        SystemArgument2
);

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
XBSYSAPI EXPORTNUM(120) PKSYSTEM_TIME KeInterruptTime;

// ******************************************************************
// * 0x0079 - KeIsExecutingDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(121) BOOLEAN NTAPI KeIsExecutingDpc();

// ******************************************************************
// * 0x007A - KeLeaveCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(122) VOID NTAPI KeLeaveCriticalRegion
(
	VOID
);

XBSYSAPI EXPORTNUM(123) LONG NTAPI KePulseEvent
(
	IN PRKEVENT Event,
	IN KPRIORITY Increment,
	IN BOOLEAN Wait
);

XBSYSAPI EXPORTNUM(124) LONG NTAPI KeQueryBasePriorityThread
(
	IN PKTHREAD Thread
);

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) ULONGLONG NTAPI KeQueryInterruptTime(void);

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
// ******************************************************************
XBSYSAPI EXPORTNUM(126) ULONGLONG NTAPI KeQueryPerformanceCounter(void);

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) ULONGLONG NTAPI KeQueryPerformanceFrequency(void);

// ******************************************************************
// * 0x0080 - KeQuerySystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(128) VOID NTAPI KeQuerySystemTime
(
    PLARGE_INTEGER CurrentTime
);

// ******************************************************************
// * 0x0081 - KeRaiseIrqlToDpcLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(129) UCHAR NTAPI KeRaiseIrqlToDpcLevel();

// ******************************************************************
// * 0x0082 - KeRaiseIrqlToSynchLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(130) UCHAR NTAPI KeRaiseIrqlToSynchLevel();

XBSYSAPI EXPORTNUM(131) LONG NTAPI KeReleaseMutant
(
	IN PRKMUTANT Mutant,
	IN KPRIORITY Increment,
	IN BOOLEAN Abandoned,
	IN BOOLEAN Wait
);

XBSYSAPI EXPORTNUM(132) LONG NTAPI KeReleaseSemaphore
(
	IN PRKSEMAPHORE Semaphore,
	IN KPRIORITY Increment,
	IN BOOLEAN Adjustment,
	IN BOOLEAN Wait
);

XBSYSAPI EXPORTNUM(133) PKDEVICE_QUEUE_ENTRY NTAPI KeRemoveByKeyDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN ULONG SortKey
);

XBSYSAPI EXPORTNUM(134) BOOLEAN NTAPI KeRemoveDeviceQueue
(
	IN PKDEVICE_QUEUE DeviceQueue,
	IN ULONG SortKey
);

XBSYSAPI EXPORTNUM(135) BOOLEAN NTAPI KeRemoveEntryDeviceQueue
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
XBSYSAPI EXPORTNUM(137) BOOLEAN NTAPI KeRemoveQueueDpc
(
	IN PKDPC Dpc
);

// ******************************************************************
// * 0x008A - KeResetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(138) LONG NTAPI KeResetEvent
(
	IN PRKEVENT Event
);

// ******************************************************************
// * 0x008B - KeRestoreFloatingPointState()
// ******************************************************************
XBSYSAPI EXPORTNUM(139) NTSTATUS NTAPI KeRestoreFloatingPointState
(
	IN PKFLOATING_SAVE     PublicFloatSave
);

// ******************************************************************
// * 0x008C - KeResumeThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(140) ULONG NTAPI KeResumeThread
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
XBSYSAPI EXPORTNUM(142) NTSTATUS NTAPI KeSaveFloatingPointState
(
	OUT PKFLOATING_SAVE     PublicFloatSave
);

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) LONG NTAPI KeSetBasePriorityThread
(
    IN PKTHREAD  Thread,
    IN LONG	Priority
);

XBSYSAPI EXPORTNUM(144) ULONG NTAPI KeSetDisableBoostThread
(
	IN PKTHREAD Thread,
	IN ULONG Disable
);

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) LONG NTAPI KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN BOOLEAN		Wait
);

XBSYSAPI EXPORTNUM(146) VOID NTAPI KeSetEventBoostPriority
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
XBSYSAPI EXPORTNUM(148) BOOLEAN NTAPI KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN LONG  Priority
);

// ******************************************************************
// * 0x0095 - KeSetTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(149) BOOLEAN NTAPI KeSetTimer
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * 0x0096 - KeSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(150) BOOLEAN NTAPI KeSetTimerEx
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN LONG           Period OPTIONAL,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * 0x0097 - KeStallExecutionProcessor()
// ******************************************************************
XBSYSAPI EXPORTNUM(151) VOID NTAPI KeStallExecutionProcessor
(
	IN ULONG MicroSeconds
);

// ******************************************************************
// * 0x0098 - KeSuspendThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(152) ULONG NTAPI KeSuspendThread
(
	IN PKTHREAD Thread
);

// ******************************************************************
// * 0x0099 - KeSynchronizeExecution()
// ******************************************************************
XBSYSAPI EXPORTNUM(153) BOOLEAN NTAPI KeSynchronizeExecution
(
	IN PKINTERRUPT Interrupt,
	IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,
	IN PVOID SynchronizeContext
);

// ******************************************************************
// * 0x009A - KeSystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(154) PKSYSTEM_TIME KeSystemTime;

// ******************************************************************
// * 0x009B - KeTestAlertThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(155) BOOLEAN NTAPI KeTestAlertThread
(
	IN KPROCESSOR_MODE AlertMode
);

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) volatile DWORD KeTickCount;

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) ULONG KeTimeIncrement;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects()
// ******************************************************************
XBSYSAPI EXPORTNUM(158) NTSTATUS NTAPI KeWaitForMultipleObjects
(
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK	WaitBlockArray  OPTIONAL
);

// ******************************************************************
// * 0x009F - KeWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(159) NTSTATUS NTAPI KeWaitForSingleObject
(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

#endif


