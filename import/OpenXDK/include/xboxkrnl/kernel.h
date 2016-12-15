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

XBSYSAPI EXPORTNUM(92) NTSTATUS NTAPI KeAlertResumeThread
(
	IN HANDLE ThreadHandle,
	IN OUT PULONG PreviousSuspendCount
);

XBSYSAPI EXPORTNUM(93) NTSTATUS NTAPI KeAlertThread
(
	IN HANDLE ThreadHandle
);

XBSYSAPI EXPORTNUM(94) NTSTATUS NTAPI KeBoostPriorityThread
(
);

// ******************************************************************
// * KeBugCheck
// ******************************************************************
XBSYSAPI EXPORTNUM(95) VOID NTAPI KeBugCheck
(
	IN ULONG BugCheckMode
);

XBSYSAPI EXPORTNUM(96) NTSTATUS NTAPI KeBugCheckEx
(
	IN DWORD BugCheckCode,
	IN PVOID BugCheckParameter1,
	IN PVOID BugCheckParameter2,
	IN PVOID BugCheckParameter3,
	IN PVOID BugCheckParameter4
);

XBSYSAPI EXPORTNUM(97) BOOLEAN NTAPI KeCancelTimer
(
	IN PKTIMER
);

// ******************************************************************
// * KeConnectInterrupt
// ******************************************************************
XBSYSAPI EXPORTNUM(98) LONG NTAPI KeConnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

// ******************************************************************
// * KeDelayExecutionThread
// ******************************************************************
XBSYSAPI EXPORTNUM(99) NTSTATUS NTAPI KeDelayExecutionThread
(
    IN KPROCESSOR_MODE  WaitMode,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
);

// ******************************************************************
// * KeDisconnectInterrupt
// ******************************************************************
XBSYSAPI EXPORTNUM(100) VOID NTAPI KeDisconnectInterrupt
(
    IN PKINTERRUPT  InterruptObject
);

XBSYSAPI VOID *KeEnterCriticalRegion;
XBSYSAPI VOID *KeGetCurrentIrql;

// ******************************************************************
// * KeGetCurrentThread
// ******************************************************************
XBSYSAPI EXPORTNUM(104) PKTHREAD NTAPI KeGetCurrentThread(void);

XBSYSAPI VOID *KeInitializeApc;
XBSYSAPI VOID *KeInitializeDeviceQueue;

// ******************************************************************
// * KeInitializeDpc
// ******************************************************************
XBSYSAPI EXPORTNUM(107) VOID NTAPI KeInitializeDpc
(
    KDPC                *Dpc,
    PKDEFERRED_ROUTINE   DeferredRoutine,
    PVOID                DeferredContext
);

XBSYSAPI VOID *KeInitializeEvent;

// ******************************************************************
// * KeInitializeInterrupt
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
XBSYSAPI VOID *KeInitializeQueue;
XBSYSAPI VOID *KeInitializeSemaphore;

// ******************************************************************
// * KeInitializeTimerEx
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
// * KeInsertQueueDpc
// ******************************************************************
XBSYSAPI EXPORTNUM(119) BOOLEAN NTAPI KeInsertQueueDpc
(
    IN PKDPC        Dpc,
    IN PVOID        SystemArgument1,
    IN PVOID        SystemArgument2
);

XBSYSAPI VOID *KeInterruptTime;
XBSYSAPI VOID *KeIsExecutingDpc;
XBSYSAPI VOID *KeLeaveCriticalRegion;
XBSYSAPI VOID *KePulseEvent;
XBSYSAPI VOID *KeQueryBasePriorityThread;

// ******************************************************************
// * KeQueryInterruptTime
// ******************************************************************
XBSYSAPI EXPORTNUM(125) ULONGLONG NTAPI KeQueryInterruptTime(void);

// ******************************************************************
// * KeQueryPerformanceCounter
// ******************************************************************
XBSYSAPI EXPORTNUM(126) ULONGLONG NTAPI KeQueryPerformanceCounter(void);

// ******************************************************************
// * KeQueryPerformanceFrequency
// ******************************************************************
XBSYSAPI EXPORTNUM(127) ULONGLONG NTAPI KeQueryPerformanceFrequency(void);

// ******************************************************************
// * KeQuerySystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(128) VOID NTAPI KeQuerySystemTime
(
    PLARGE_INTEGER CurrentTime
);

// ******************************************************************
// * KeRaiseIrqlToDpcLevel
// ******************************************************************
XBSYSAPI EXPORTNUM(129) UCHAR NTAPI KeRaiseIrqlToDpcLevel();

XBSYSAPI VOID *KeRaiseIrqlToSynchLevel;
XBSYSAPI VOID *KeReleaseMutant;
XBSYSAPI VOID *KeReleaseSemaphore;
XBSYSAPI VOID *KeRemoveByKeyDeviceQueue;
XBSYSAPI VOID *KeRemoveDeviceQueue;
XBSYSAPI VOID *KeRemoveEntryDeviceQueue;
XBSYSAPI VOID *KeRemoveQueue;
XBSYSAPI VOID *KeRemoveQueueDpc;
XBSYSAPI VOID *KeResetEvent;
XBSYSAPI VOID *KeRestoreFloatingPointState;
XBSYSAPI VOID *KeResumeThread;
XBSYSAPI VOID *KeRundownQueue;
XBSYSAPI VOID *KeSaveFloatingPointState;

//XBSYSAPI VOID *KeSetBasePriorityThread;
XBSYSAPI EXPORTNUM(143) LONG NTAPI KeSetBasePriorityThread
(
    IN PKTHREAD  Thread,
    IN PVOID  Priority
);



XBSYSAPI VOID *KeSetDisableBoostThread;
XBSYSAPI VOID *KeSetEvent;
XBSYSAPI VOID *KeSetEventBoostPriority;
XBSYSAPI VOID *KeSetPriorityProcess;

//XBSYSAPI VOID *KeSetPriorityThread;
XBSYSAPI EXPORTNUM(148) BOOLEAN NTAPI KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN PVOID  Priority
);

// ******************************************************************
// * KeSetTimer
// ******************************************************************
XBSYSAPI EXPORTNUM(149) BOOLEAN NTAPI KeSetTimer
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * KeSetTimerEx
// ******************************************************************
XBSYSAPI EXPORTNUM(150) BOOLEAN NTAPI KeSetTimerEx
(
    IN PKTIMER        Timer,
    IN LARGE_INTEGER  DueTime,
    IN LONG           Period OPTIONAL,
    IN PKDPC          Dpc OPTIONAL
);

// ******************************************************************
// * KeStallExecutionProcessor
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
// * KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) volatile DWORD KeTickCount;

XBSYSAPI EXPORTNUM(157) ULONG KeTimeIncrement;

XBSYSAPI EXPORTNUM(158) NTSTATUS KeWaitForMultipleObjects
(
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN VOID* WaitBlockArray
);

XBSYSAPI EXPORTNUM(159) NTSTATUS KeWaitForSingleObject
(
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
);

#endif


