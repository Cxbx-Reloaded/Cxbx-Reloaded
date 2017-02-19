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

XBSYSAPI VOID *KeInitializeApc;
XBSYSAPI VOID *KeInitializeDeviceQueue;

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


XBSYSAPI VOID *KeInitializeMutant;

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

XBSYSAPI VOID *KeInsertByKeyDeviceQueue;
XBSYSAPI VOID *KeInsertDeviceQueue;
XBSYSAPI VOID *KeInsertHeadQueue;
XBSYSAPI VOID *KeInsertQueue;
XBSYSAPI VOID *KeInsertQueueApc;

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

XBSYSAPI VOID *KePulseEvent;
XBSYSAPI VOID *KeQueryBasePriorityThread;

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

XBSYSAPI VOID *KeReleaseMutant;
XBSYSAPI VOID *KeReleaseSemaphore;
XBSYSAPI VOID *KeRemoveByKeyDeviceQueue;
XBSYSAPI VOID *KeRemoveDeviceQueue;
XBSYSAPI VOID *KeRemoveEntryDeviceQueue;
XBSYSAPI VOID *KeRemoveQueue;

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

XBSYSAPI VOID *KeResumeThread;
XBSYSAPI VOID *KeRundownQueue;

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

XBSYSAPI VOID *KeSetDisableBoostThread;

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) LONG NTAPI KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN BOOLEAN		Wait
);

XBSYSAPI VOID *KeSetEventBoostPriority;
XBSYSAPI VOID *KeSetPriorityProcess;

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

XBSYSAPI VOID *KeSuspendThread;
XBSYSAPI VOID *KeSynchronizeExecution;
XBSYSAPI VOID *KeSystemTime;
XBSYSAPI VOID *KeTestAlertThread;

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
XBSYSAPI EXPORTNUM(158) NTSTATUS KeWaitForMultipleObjects
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
XBSYSAPI EXPORTNUM(159) NTSTATUS KeWaitForSingleObject
(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

#endif


