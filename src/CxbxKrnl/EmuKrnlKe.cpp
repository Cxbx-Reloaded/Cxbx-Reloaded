// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlKe.cpp
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
#include <xboxkrnl/xboxkrnl.h> // For KeBugCheck, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuFile.h" // For IsEmuHandle(), NtStatusToString()

#include <chrono>
#include <thread>

// ******************************************************************
// * 0x005C - KeAlertResumeThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(92) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeAlertResumeThread
(
	IN HANDLE ThreadHandle,
	IN OUT PULONG PreviousSuspendCount
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ThreadHandle)
		LOG_FUNC_ARG_OUT(PreviousSuspendCount)
		LOG_FUNC_END;

	// TODO : Result = NtDll::NtAlertResumeThread(ThreadHandle, PreviousSuspendCount);
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005D - KeAlertThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(93) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeAlertThread
(
	IN HANDLE ThreadHandle
)
{
	LOG_FUNC_ONE_ARG(ThreadHandle);

// TODO : Result = NtDll::NtAlertThread(ThreadHandle);
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005E - KeBoostPriorityThread()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(94) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeBoostPriorityThread
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x005F - KeBugCheck()
// ******************************************************************
XBSYSAPI EXPORTNUM(95) xboxkrnl::VOID NTAPI xboxkrnl::KeBugCheck
(
	IN ULONG BugCheckMode
)
{
	LOG_FORWARD("KeBugCheckEx");

	KeBugCheckEx(BugCheckMode, 0, 0, 0, 0);
}

// ******************************************************************
// * 0x0060 - KeBugCheckEx()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(96) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeBugCheckEx
(
	IN DWORD BugCheckCode,
	IN PVOID BugCheckParameter1,
	IN PVOID BugCheckParameter2,
	IN PVOID BugCheckParameter3,
	IN PVOID BugCheckParameter4
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BugCheckCode)
		LOG_FUNC_ARG(BugCheckParameter1)
		LOG_FUNC_ARG(BugCheckParameter2)
		LOG_FUNC_ARG(BugCheckParameter3)
		LOG_FUNC_ARG(BugCheckParameter4)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x0061 - KeCancelTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(96) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeCancelTimer
(
	IN PKTIMER Timer
)
{
	LOG_FUNC_ONE_ARG(Timer);

	LOG_UNIMPLEMENTED();

	RETURN(TRUE);
}

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) xboxkrnl::LONG NTAPI xboxkrnl::KeConnectInterrupt
(
	IN PKINTERRUPT  InterruptObject
)
{
	LOG_FUNC_ONE_ARG(InterruptObject);

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(99) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeDelayExecutionThread
(
	IN KPROCESSOR_MODE  WaitMode,
	IN BOOLEAN          Alertable,
	IN PLARGE_INTEGER   Interval
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Interval)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtDelayExecution(Alertable, (NtDll::LARGE_INTEGER*)Interval);

	RETURN(ret);
}

// ******************************************************************
// * 0x0064 - KeDisconnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(100) xboxkrnl::VOID NTAPI xboxkrnl::KeDisconnectInterrupt
(
	IN PKINTERRUPT  InterruptObject
) 
{
	LOG_FUNC_ONE_ARG(InterruptObject);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0068 - KeGetCurrentThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(104) xboxkrnl::PKTHREAD NTAPI xboxkrnl::KeGetCurrentThread(void)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();

	RETURN(NULL);
}

// ******************************************************************
// * 0x006B - KeInitializeDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(107) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeDpc
(
	KDPC                *Dpc,
	PKDEFERRED_ROUTINE   DeferredRoutine,
	PVOID                DeferredContext
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_ARG(DeferredRoutine)
		LOG_FUNC_ARG(DeferredContext)
		LOG_FUNC_END;

	// inialize Dpc field values
	Dpc->DeferredRoutine = DeferredRoutine;
	Dpc->Type = DpcObject;
	Dpc->DeferredContext = DeferredContext;
	Dpc->Inserted = FALSE;
}

// ******************************************************************
// * 0x006D - KeInitializeInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(109) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeInterrupt
(
	OUT PKINTERRUPT Interrupt,
	IN PKSERVICE_ROUTINE ServiceRoutine,
	IN PVOID ServiceContext,
	IN ULONG Vector,
	IN KIRQL Irql,
	IN KINTERRUPT_MODE InterruptMode,
	IN BOOLEAN ShareVector
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Interrupt)
		LOG_FUNC_ARG(ServiceRoutine)
		LOG_FUNC_ARG(ServiceContext)
		LOG_FUNC_ARG(Vector)
		LOG_FUNC_ARG(Irql)
		LOG_FUNC_ARG(InterruptMode)
		LOG_FUNC_ARG(ShareVector)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0071 - KeInitializeTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(113) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeTimerEx
(
	IN PKTIMER      Timer,
	IN TIMER_TYPE   Type
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Timer)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_END;

	Timer->Header.Type = Type + 8;  // 8 = TimerNotificationObject 
	Timer->Header.Inserted = 0;
	Timer->Header.Size = sizeof(KTIMER) / sizeof(ULONG);
	Timer->Header.SignalState = 0;

	Timer->TimerListEntry.Blink = NULL;
	Timer->TimerListEntry.Flink = NULL;
	Timer->Header.WaitListHead.Flink = &Timer->Header.WaitListHead;
	Timer->Header.WaitListHead.Blink = &Timer->Header.WaitListHead;

	Timer->DueTime.QuadPart = 0;
	Timer->Period = 0;
}

// ******************************************************************
// * 0x0077 - KeInsertQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(119) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeInsertQueueDpc
(
	IN PKDPC        Dpc,
	IN PVOID        SystemArgument1,
	IN PVOID        SystemArgument2
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_ARG(SystemArgument1)
		LOG_FUNC_ARG(SystemArgument2)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(TRUE);
}

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
// XBSYSAPI EXPORTNUM(120) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeInterruptTime; // Used for KernelThunk[120]

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryInterruptTime(void)
{
	// TODO : Some software might call this often and fill the log quickly,
	// in which case we should not LOG_FUNC nor RETURN (use normal return instead).
	LOG_FUNC();

	// Don't use NtDll::QueryInterruptTime, it's too new (Windows 10)
	
	// Instead, read KeInterruptTime from our kernel thunk table,
	// which we coupled to the host InterruptTime in ConnectWindowsTimersToThunkTable:
	ULONGLONG InterruptTime = *((PULONGLONG)CxbxKrnl_KernelThunkTable[120]);
	
	// TODO : Read InterruptTime atomically (or with a spinloop) to avoid errors
	// when High1Time and High2Time differ (during unprocessed overflow in LowPart).
	
	// TODO : Should we apply HostSystemTimeDelta to InterruptTime too?

	RETURN(InterruptTime);
}

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter(void)
{
	LOG_FUNC();

	::LARGE_INTEGER Counter;

	QueryPerformanceCounter(&Counter);

	RETURN(Counter.QuadPart);
}

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency(void)
{
	LOG_FUNC();

	// Xbox Performance Counter Frequency := 337F98h
	::LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);

	RETURN(Frequency.QuadPart);
}

// ******************************************************************
// * 0x0080 - KeQuerySystemTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(128) xboxkrnl::VOID NTAPI xboxkrnl::KeQuerySystemTime
(
	PLARGE_INTEGER CurrentTime
)
{
	LOG_FUNC_ONE_ARG(CurrentTime);

	if (CurrentTime != NULL)
	{
		LARGE_INTEGER HostSystemTime;
		GetSystemTimeAsFileTime((LPFILETIME)&HostSystemTime); // Available since Windows 2000 (NOT on XP!)

		// Apply the delta set in xboxkrnl::NtSetSystemTime to get the Xbox system time :
		CurrentTime->QuadPart = HostSystemTime.QuadPart + HostSystemTimeDelta.QuadPart;
	}
}

// ******************************************************************
// * 0x0081 - KeRaiseIrqlToDpcLevel()
// ******************************************************************
XBSYSAPI EXPORTNUM(129) xboxkrnl::UCHAR NTAPI xboxkrnl::KeRaiseIrqlToDpcLevel()
{
	LOG_FUNC();

	// I really tried to avoid adding this...
	//	__asm int 3;
	//	CxbxKrnlCleanup("KeRaiseIrqlToDpcLevel not implemented! (Tell blueshogun -_-)");
	LOG_UNIMPLEMENTED();

	RETURN(0);
}

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) xboxkrnl::LONG NTAPI xboxkrnl::KeSetBasePriorityThread
(
	IN PKTHREAD  Thread,
	IN PVOID  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Thread)
		LOG_FUNC_ARG_OUT(Priority)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(1);
}

// ******************************************************************
// * 0x0094 - KeSetPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(148) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN PVOID  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Thread)
		LOG_FUNC_ARG_OUT(Priority)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(1);
}

// ******************************************************************
// * 0x0095 - KeSetTimer()
// ******************************************************************
XBSYSAPI EXPORTNUM(149) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimer
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC          Dpc OPTIONAL
)
{
	LOG_FORWARD("KeSetTimerEx");

	// Call KeSetTimerEx with a period of zero
	return KeSetTimerEx(Timer, DueTime, 0, Dpc);
}

// ******************************************************************
// * 0x0096 - KeSetTimerEx()
// ******************************************************************
XBSYSAPI EXPORTNUM(150) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimerEx
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN LONG           Period OPTIONAL,
	IN PKDPC          Dpc OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Timer)
		LOG_FUNC_ARG(DueTime)
		LOG_FUNC_ARG(Period)
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_END;

	BOOLEAN Inserted;
	LARGE_INTEGER Interval;
	LARGE_INTEGER SystemTime;

#define RemoveEntryList(e) do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)

	if (Timer->Header.Type != 8 && Timer->Header.Type != 9) {
		CxbxKrnlCleanup("Assertion: '(Timer)->Header.Type == TimerNotificationObject) || ((Timer)->Header.Type == TimerSynchronizationObject)' in KeSetTimerEx()");
	}

	Inserted = Timer->Header.Inserted;
	if (Inserted != FALSE) {
		// Do some unlinking if already inserted in the linked list
		Timer->Header.Inserted = FALSE;
		RemoveEntryList(&Timer->TimerListEntry);
	}

	Timer->Header.SignalState = FALSE;
	Timer->Dpc = Dpc;
	Timer->Period = Period;

	if (/*!KiInsertTreeTimer(Timer,DueTime)*/ TRUE) {
		if (Timer->Header.WaitListHead.Flink != &Timer->Header.WaitListHead) {
			// KiWaitTest(Timer, 0);
		}

		if (Dpc != NULL) {
			// Call the Dpc routine if one is specified
			KeQuerySystemTime(&SystemTime);
			// Need to implement KeInsertQueueDpc xboxkrnl.exe export (ordinal 119)
			// KeInsertQueueDpc(Timer->Dpc, SystemTime.LowPart, SystemTime.HighPart);
		}

		if (Period != 0) {
			// Prepare the repetition if Timer is periodic
			Interval.QuadPart = (-10 * 1000) * Timer->Period;
			while (/*!KiInsertTreeTimer(Timer,Interval)*/TRUE) {
				;
			}
		}
	}

	RETURN(Inserted);
}

// ******************************************************************
// * 0x0097 - KeStallExecutionProcessor()
// ******************************************************************
XBSYSAPI EXPORTNUM(151) xboxkrnl::VOID NTAPI xboxkrnl::KeStallExecutionProcessor
(
	IN ULONG MicroSeconds
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(MicroSeconds)
		LOG_FUNC_END;

	// WinAPI Sleep usually sleeps for a minimum of 15ms, we want us. 
	// Thanks to C++11, we can do this in a nice way without resorting to
	// QueryPerformanceCounter
	std::this_thread::sleep_for(std::chrono::microseconds(MicroSeconds));
}

// ******************************************************************
// * 0x009A - KeSystemTime
// ******************************************************************
// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
// XBSYSAPI EXPORTNUM(154) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeSystemTime; // Used for KernelThunk[154]

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) xboxkrnl::DWORD VOLATILE xboxkrnl::KeTickCount = 0;

const xboxkrnl::ULONG CLOCK_TIME_INCREMENT = 0x2710;

// ******************************************************************
// * 0x009D - KeTimeIncrement
// ******************************************************************
XBSYSAPI EXPORTNUM(157) xboxkrnl::ULONG xboxkrnl::KeTimeIncrement = CLOCK_TIME_INCREMENT;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects()
// ******************************************************************
XBSYSAPI EXPORTNUM(158) xboxkrnl::NTSTATUS xboxkrnl::KeWaitForMultipleObjects
(
	IN ULONG Count,
	IN PVOID Object[],
	IN WAIT_TYPE WaitType,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL,
	IN VOID* WaitBlockArray
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Count)
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(WaitType)
		LOG_FUNC_ARG(WaitReason)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_ARG(WaitBlockArray)
		LOG_FUNC_END;

	// Unused arguments : WaitReason, WaitMode, WaitBlockArray

	NTSTATUS ret = STATUS_SUCCESS;

	for (uint i = 0; i < Count; i++)
		if (IsEmuHandle(Object[i]))
		{
			ret = WAIT_FAILED;
			EmuWarning("WaitFor EmuHandle not supported!");
			break;
		}

	if (ret == STATUS_SUCCESS)
	{
		// TODO : What should we do with the (currently ignored) WaitMode?

		ret = NtDll::NtWaitForMultipleObjects(
			Count,
			Object,
			(NtDll::OBJECT_WAIT_TYPE)WaitType,
			Alertable,
			(NtDll::PLARGE_INTEGER)Timeout);

		if (Count == 1)
			DbgPrintf("Finished waiting for 0x%.08X\n", Object[0]);

		if (ret == WAIT_FAILED)
			EmuWarning("KeWaitForMultipleObjects failed! (%s)", NtStatusToString(ret));
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x009F - KeWaitForSingleObject()
// ******************************************************************
XBSYSAPI EXPORTNUM(159) xboxkrnl::NTSTATUS xboxkrnl::KeWaitForSingleObject
(
	IN PVOID Object,
	IN KWAIT_REASON WaitReason,
	IN KPROCESSOR_MODE WaitMode,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Object)
		LOG_FUNC_ARG(WaitReason)
		LOG_FUNC_ARG(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG(Timeout)
		LOG_FUNC_END;

	EmuWarning("EmuKrnl: Redirecting KeWaitForSingleObject to NtWaitForSingleObjectEx");

	NTSTATUS ret = NtWaitForSingleObjectEx(Object, WaitMode, Alertable, Timeout);

	RETURN(ret);
}
