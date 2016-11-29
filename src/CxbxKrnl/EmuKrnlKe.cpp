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

// prevent name collisions
namespace NtDll
{
	#include "EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()

using namespace xboxkrnl;

// TODO : Move operator<< to a central place
std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value)
{
	return os << value.QuadPart;
}

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
// * KeBugCheck
// ******************************************************************
XBSYSAPI EXPORTNUM(95) VOID NTAPI xboxkrnl::KeBugCheck
(
	IN ULONG BugCheckMode
)
{
	LOG_FUNC_ONE_ARG(BugCheckMode);

	KeBugCheckEx(BugCheckMode, 0, 0, 0, 0);
}

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
// * KeConnectInterrupt
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
// * 0x0063 - KeDelayExecutionThread
// ******************************************************************
XBSYSAPI EXPORTNUM(99) xboxkrnl::NTSTATUS NTAPI xboxkrnl::KeDelayExecutionThread
(
	IN KPROCESSOR_MODE  WaitMode,
	IN BOOLEAN          Alertable,
	IN PLARGE_INTEGER   Interval
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(WaitMode)
		LOG_FUNC_ARG(Alertable)
		LOG_FUNC_ARG_OUT(Interval)
		LOG_FUNC_END;

	NTSTATUS ret = NtDll::NtDelayExecution(Alertable, (NtDll::LARGE_INTEGER*)Interval);

	RETURN(ret);
}

// ******************************************************************
// * 0x006B - KeInitializeDpc
// ******************************************************************
XBSYSAPI EXPORTNUM(107) VOID NTAPI xboxkrnl::KeInitializeDpc
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
// * 0x006D - KeInitializeInterrupt
// ******************************************************************
XBSYSAPI EXPORTNUM(109) VOID NTAPI xboxkrnl::KeInitializeInterrupt
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
// * 0x0071 - KeInitializeTimerEx
// ******************************************************************
XBSYSAPI EXPORTNUM(113) VOID NTAPI xboxkrnl::KeInitializeTimerEx
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

// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
// XBSYSAPI EXPORTNUM(120) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeInterruptTime; // Used for KernelThunk[120]

// ******************************************************************
// * KeQueryPerformanceCounter
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter()
{
	LOG_FUNC();

	::LARGE_INTEGER Counter;

	QueryPerformanceCounter(&Counter);

	RETURN(Counter.QuadPart);
}

// ******************************************************************
// * KeQueryPerformanceFrequency
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency()
{
	LOG_FUNC();

	// Xbox Performance Counter Frequency := 337F98h
	::LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);

	RETURN(Frequency.QuadPart);
}

// ******************************************************************
// * 0x0080 - KeQuerySystemTime
// ******************************************************************
XBSYSAPI EXPORTNUM(128) VOID NTAPI xboxkrnl::KeQuerySystemTime
(
	PLARGE_INTEGER CurrentTime
)
{
	LOG_FUNC_ONE_ARG(CurrentTime);

	// TODO: optimize for WinXP if speed ever becomes important here

	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);

	SystemTimeToFileTime(&SystemTime, (LPFILETIME)CurrentTime);
}

// ******************************************************************
// * KeRaiseIrqlToDpcLevel
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
// * 0x008F - KeSetBasePriorityThread
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
// * 0x0096 - KeSetPriorityThread
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
// * 0x0095 - KeSetTimer
// ******************************************************************
XBSYSAPI EXPORTNUM(149) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetTimer
(
	IN PKTIMER        Timer,
	IN LARGE_INTEGER  DueTime,
	IN PKDPC          Dpc OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Timer)
		LOG_FUNC_ARG(DueTime)
		LOG_FUNC_ARG(Dpc)
		LOG_FUNC_END;

	// Call KeSetTimerEx with a period of zero
	BOOLEAN bRet = KeSetTimerEx(Timer, DueTime, 0, Dpc);

	RETURN(bRet);
}

// ******************************************************************
// * 0x0096 - KeSetTimerEx
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

// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
// XBSYSAPI EXPORTNUM(154) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeSystemTime; // Used for KernelThunk[154]

// ******************************************************************
// * 0x009C - KeTickCount
// ******************************************************************
XBSYSAPI EXPORTNUM(156) volatile xboxkrnl::DWORD xboxkrnl::KeTickCount = 0;

const xboxkrnl::ULONG CLOCK_TIME_INCREMENT = 0x2710;
XBSYSAPI EXPORTNUM(157) xboxkrnl::ULONG xboxkrnl::KeTimeIncrement = CLOCK_TIME_INCREMENT;

// ******************************************************************
// * 0x009E - KeWaitForMultipleObjects
// ******************************************************************
XBSYSAPI EXPORTNUM(158) xboxkrnl::NTSTATUS xboxkrnl::KeWaitForMultipleObjects
(
	IN ULONG Count,
	IN PVOID Object[],
	IN WAIT_TYPE WaitType,
	IN int WaitReason,
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

	EmuWarning("EmuKrnl: Redirecting KeWaitForMultipleObjects to NtWaitForMultipleObjectsEx");

	NTSTATUS ret = NtWaitForMultipleObjectsEx(Count, Object, WaitType, WaitMode, Alertable, Timeout);

	RETURN(ret);
}

// ******************************************************************
// * 0x009F - KeWaitForSingleObject
// ******************************************************************
XBSYSAPI EXPORTNUM(159) xboxkrnl::NTSTATUS xboxkrnl::KeWaitForSingleObject
(
	IN PVOID Object,
	IN int WaitReason,
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


