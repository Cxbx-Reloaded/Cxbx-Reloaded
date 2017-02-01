// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

// Copied over from Dxbx. 
typedef struct _DpcData {
	CRITICAL_SECTION Lock;
	HANDLE DpcThread;
	HANDLE DpcEvent;
	xboxkrnl::LIST_ENTRY DpcQueue;
	xboxkrnl::LIST_ENTRY TimerQueue;
} DpcData;

DpcData g_DpcData = { 0 }; // TODO : InitializeCriticalSection(Lock)

// See also :
// https://github.com/reactos/reactos/blob/40a16a9cf1cdfca399e9154b42d32c30b63480f5/reactos/drivers/filesystems/udfs/Include/env_spec_w32.h
void InitializeListHead(xboxkrnl::PLIST_ENTRY pListHead)
{
	pListHead->Flink = pListHead->Blink = pListHead;
}

bool IsListEmpty(xboxkrnl::PLIST_ENTRY pListHead)
{
	return (pListHead->Flink == pListHead);
}

void InsertHeadList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_ListHead = pListHead;
	xboxkrnl::PLIST_ENTRY _EX_Flink = _EX_ListHead->Flink;

	pEntry->Flink = _EX_Flink;
	pEntry->Blink = _EX_ListHead;
	_EX_Flink->Blink = pEntry;
	_EX_ListHead->Flink = pEntry;
}

void InsertTailList(xboxkrnl::PLIST_ENTRY pListHead, xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_ListHead = pListHead;
	xboxkrnl::PLIST_ENTRY _EX_Blink = _EX_ListHead->Blink;
	
	pEntry->Flink = _EX_ListHead;
	pEntry->Blink = _EX_Blink;
	_EX_Blink->Flink = pEntry;
	_EX_ListHead->Blink = pEntry;
}

//#define RemoveEntryList(e) do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)

void RemoveEntryList(xboxkrnl::PLIST_ENTRY pEntry)
{
	xboxkrnl::PLIST_ENTRY _EX_Flink = pEntry->Flink;
	xboxkrnl::PLIST_ENTRY _EX_Blink = pEntry->Blink;

	_EX_Blink->Flink = _EX_Flink;
	_EX_Flink->Blink = _EX_Blink;
}

xboxkrnl::PLIST_ENTRY RemoveHeadList(xboxkrnl::PLIST_ENTRY pListHead)
{
	xboxkrnl::PLIST_ENTRY Result = pListHead->Flink;

	RemoveEntryList(pListHead->Flink);
	return Result;
}

xboxkrnl::PLIST_ENTRY RemoveTailList(xboxkrnl::PLIST_ENTRY pListHead)
{
	xboxkrnl::PLIST_ENTRY Result = pListHead->Blink;
	
	RemoveEntryList(pListHead->Blink);
	return Result;
}

// ******************************************************************
// * KeGetPcr()
// * NOTE: This is a macro on the Xbox, however we implement it 
// * as a function so it can suit our emulated KPCR structure
// ******************************************************************
xboxkrnl::KPCR* KeGetPcr()
{
	xboxkrnl::KPCR* Pcr = nullptr;

	__asm {
		push eax
		mov eax, fs:[0x14]
		mov Pcr, eax
		pop eax
	}

	return Pcr;
}

DWORD BootTickCount = 0;

// The Xbox GetTickCount is measured in milliseconds, just like the native GetTickCount.
// The only difference we'll take into account here, is that the Xbox will probably reboot
// much more often than Windows, so we correct this with a 'BootTickCount' value :
DWORD CxbxXboxGetTickCount()
{
	return GetTickCount() - BootTickCount;
}

// CONTAINING_RECORD macro
// Gets the value of structure member (field - num1),given the type(MYSTRUCT, in this code) and the List_Entry head(temp, in this code)
// See http://stackoverflow.com/questions/8240273/a-portable-way-to-calculate-pointer-to-the-whole-structure-using-pointer-to-a-fi
//#define CONTAINING_RECORD(ptr, type, field) \
//	(((type) *)((char *)(ptr) - offsetof((type), member)))

DWORD __stdcall EmuThreadDpcHandler(LPVOID lpVoid)
{
	xboxkrnl::PKDPC pkdpc;
	DWORD dwWait;
	DWORD dwNow;
	LONG lWait;
	xboxkrnl::PKTIMER pktimer;

	while (true)
	{
		EnterCriticalSection(&(g_DpcData.Lock));

//    if (g_DpcData._fShutdown)
//        break; // while 

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == 0);
//    g_DpcData._dwDpcThreadId = g_DpcData._dwThreadId;
//    Assert(g_DpcData._dwDpcThreadId != 0);

		while (!IsListEmpty(&(g_DpcData.DpcQueue)))
		{
			pkdpc = CONTAINING_RECORD(RemoveHeadList(&(g_DpcData.DpcQueue)), xboxkrnl::KDPC, DpcListEntry);
			pkdpc->DpcListEntry.Flink = nullptr;
			pkdpc->DeferredRoutine(
				pkdpc,
				pkdpc->DeferredContext,
				pkdpc->SystemArgument1,
				pkdpc->SystemArgument2);
		}

		dwWait = INFINITE;
		if (!IsListEmpty(&(g_DpcData.TimerQueue)))
		{
			while (true)
			{
				dwNow = CxbxXboxGetTickCount();
				dwWait = INFINITE;
				pktimer = (xboxkrnl::PKTIMER)g_DpcData.TimerQueue.Flink;
				pkdpc = nullptr;
				while (pktimer != (xboxkrnl::PKTIMER)&(g_DpcData.TimerQueue))
				{
					lWait = (LONG)pktimer->DueTime.u.LowPart - dwNow;
					if (lWait <= 0) 
					{
						pktimer->DueTime.u.LowPart = pktimer->Period + dwNow;
						pkdpc = pktimer->Dpc;
						break; // while
					}

					if (dwWait > (DWORD)lWait)
						dwWait = (DWORD)lWait;

					pktimer = (xboxkrnl::PKTIMER)pktimer->TimerListEntry.Flink;
				}

				if (pkdpc == nullptr)
					break; // while

				pkdpc->DeferredRoutine(pkdpc,
					pkdpc->DeferredContext,
					pkdpc->SystemArgument1,
					pkdpc->SystemArgument2);
			}
		}

//    Assert(g_DpcData._dwThreadId == GetCurrentThreadId());
//    Assert(g_DpcData._dwDpcThreadId == g_DpcData._dwThreadId);
//    g_DpcData._dwDpcThreadId = 0;
		LeaveCriticalSection(&(g_DpcData.Lock));

		// TODO : Wait for shutdown too here
		WaitForSingleObject(g_DpcData.DpcEvent, dwWait);
	} // while

	return S_OK;
}

void InitDpcAndTimerThread()
{
	DWORD dwThreadId;

	InitializeCriticalSection(&(g_DpcData.Lock));
	InitializeListHead(&(g_DpcData.DpcQueue));
	InitializeListHead(&(g_DpcData.TimerQueue));
	g_DpcData.DpcEvent = CreateEvent(/*lpEventAttributes=*/nullptr, /*bManualReset=*/FALSE, /*bInitialState=*/FALSE, /*lpName=*/nullptr);
	g_DpcData.DpcThread = CreateThread(/*lpThreadAttributes=*/nullptr, /*dwStackSize=*/0, (LPTHREAD_START_ROUTINE)&EmuThreadDpcHandler, /*lpParameter=*/nullptr, /*dwCreationFlags=*/0, &dwThreadId);
	SetThreadPriority(g_DpcData.DpcThread, THREAD_PRIORITY_HIGHEST);
}

// Xbox Performance Counter Frequency = 337F98 = ACPI timer frequency (3.375000 Mhz)
#define XBOX_PERFORMANCE_FREQUENCY 3375000 

LARGE_INTEGER NativePerformanceCounter = { 0 };
LARGE_INTEGER NativePerformanceFrequency = { 0 };
double NativeToXbox_FactorForPerformanceFrequency;

void ConnectKeInterruptTimeToThunkTable(); // forward

CXBXKRNL_API void CxbxInitPerformanceCounters()
{
	BootTickCount = GetTickCount();

	// Measure current host performance counter and frequency
	QueryPerformanceCounter(&NativePerformanceCounter);
	QueryPerformanceFrequency(&NativePerformanceFrequency);
	// TODO : If anything like speed-stepping influences this, prevent or fix it here

	// Calculate the host-to-xbox performance frequency factor,
	// used the return Xbox-like results in KeQueryPerformanceCounter:
	NativeToXbox_FactorForPerformanceFrequency = (double)XBOX_PERFORMANCE_FREQUENCY / NativePerformanceFrequency.QuadPart;

	ConnectKeInterruptTimeToThunkTable();

	// Let's initialize the Dpc and timer handling thread too,
	// here for now (should be called by our caller)
	InitDpcAndTimerThread();
}

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

xboxkrnl::PKINTERRUPT EmuInterruptList[MAX_BUS_INTERRUPT_LEVEL + 1][MAX_NUM_INTERRUPTS] = { 0 };
xboxkrnl::DWORD EmuFreeInterrupt[MAX_BUS_INTERRUPT_LEVEL + 1] = { 0 };

// ******************************************************************
// * 0x0062 - KeConnectInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(98) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeConnectInterrupt
(
	IN PKINTERRUPT  InterruptObject
)
{
	LOG_FUNC_ONE_ARG(InterruptObject);

	BOOLEAN ret = FALSE;

	// here we have to connect the interrupt object to the vector
	// more than 1 interrupt object can be connected!
	if (!InterruptObject->Connected)
	{
		if (EmuFreeInterrupt[InterruptObject->BusInterruptLevel] < MAX_NUM_INTERRUPTS)
		{
			InterruptObject->Connected = TRUE;
			EmuInterruptList[InterruptObject->BusInterruptLevel][EmuFreeInterrupt[InterruptObject->BusInterruptLevel]++] = InterruptObject;
			ret = TRUE;
		}
		else
			EmuWarning("Out of interrupt places!");
	}
	// else do nothing

	RETURN(ret);
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
// * 0x0065 - KeEnterCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(101) xboxkrnl::VOID NTAPI xboxkrnl::KeEnterCriticalRegion
(
	VOID
)
{
	LOG_FUNC();

	// TODO : Disable kernel APCs

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0067 - KeGetCurrentIrql()
// ******************************************************************
XBSYSAPI EXPORTNUM(103) xboxkrnl::KIRQL NTAPI xboxkrnl::KeGetCurrentIrql(void)
{
	LOG_FUNC();

	KIRQL Irql;

	Irql = KeGetPcr()->Irql;

	RETURN(Irql);
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
// * 0x006C - KeInitializeEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(108) xboxkrnl::VOID NTAPI xboxkrnl::KeInitializeEvent
(
	IN PRKEVENT Event,
	IN EVENT_TYPE Type,
	IN BOOLEAN State
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Type)
		LOG_FUNC_ARG(State)
		LOG_FUNC_END;

	Event->Header.Type = Type;
	Event->Header.Size = sizeof(KEVENT) / sizeof(LONG);
	Event->Header.SignalState = State;
	InitializeListHead(&(Event->Header.WaitListHead));
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

	// TODO : Untested :
	Interrupt->ServiceRoutine = (PVOID)ServiceRoutine;
	Interrupt->ServiceContext = ServiceContext;
	Interrupt->BusInterruptLevel = Vector - 0x30;
	Interrupt->Irql = Irql;
	Interrupt->Mode = InterruptMode;
	Interrupt->ShareVector = ShareVector;
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
	InitializeListHead(&(Timer->Header.WaitListHead));

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

	if (Dpc->DpcListEntry.Flink == nullptr)
	{
		EnterCriticalSection(&(g_DpcData.Lock));

		if (Dpc->DpcListEntry.Flink == nullptr)
		{
			Dpc->SystemArgument1 = SystemArgument1;
			Dpc->SystemArgument2 = SystemArgument2;
			InsertTailList(&(g_DpcData.DpcQueue), &(Dpc->DpcListEntry));
		}

		LeaveCriticalSection(&(g_DpcData.Lock));

		SetEvent(g_DpcData.DpcEvent);
	}

	RETURN(TRUE);
}

// ******************************************************************
// * 0x0078 - KeInterruptTime
// ******************************************************************
// Dxbx note : This was once a value, but instead we now point to
// the native Windows versions (see ConnectWindowsTimersToThunkTable) :
XBSYSAPI EXPORTNUM(120) xboxkrnl::PKSYSTEM_TIME xboxkrnl::KeInterruptTime = nullptr; // Set by ConnectKeInterruptTimeToThunkTable

void ConnectKeInterruptTimeToThunkTable()
{
	xboxkrnl::KeInterruptTime = (xboxkrnl::PKSYSTEM_TIME)CxbxKrnl_KernelThunkTable[120];
}

// ******************************************************************
// * 0x007A - KeLeaveCriticalRegion()
// ******************************************************************
XBSYSAPI EXPORTNUM(122) xboxkrnl::VOID NTAPI xboxkrnl::KeLeaveCriticalRegion
(
	VOID
)
{
	LOG_FUNC();

	// TODO : Enable kernel APCs

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x007D - KeQueryInterruptTime()
// ******************************************************************
XBSYSAPI EXPORTNUM(125) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryInterruptTime(void)
{
	// TODO : Some software might call this often and fill the log quickly,
	// in which case we should not LOG_FUNC nor RETURN (use normal return instead).
	LOG_FUNC();
	
	LARGE_INTEGER InterruptTime;

	// Don't use NtDll::QueryInterruptTime, it's too new (Windows 10)
	// Instead, read KeInterruptTime from our kernel thunk table,
	// which we coupled to the host InterruptTime in ConnectWindowsTimersToThunkTable:
//	NtDll::PKSYSTEM_TIME KeInterruptTime = (NtDll::PKSYSTEM_TIME)CxbxKrnl_KernelThunkTable[120];

	while (true)
	{
		InterruptTime.u.HighPart = KeInterruptTime->High1Time;
		InterruptTime.u.LowPart = KeInterruptTime->LowPart;
		// TODO : Should we apply HostSystemTimeDelta to InterruptTime too?

		// Read InterruptTime atomically with a spinloop to avoid errors
		// when High1Time and High2Time differ (during unprocessed overflow in LowPart).
		if (InterruptTime.u.HighPart == KeInterruptTime->High2Time)
			break;
	}

	// Weird construction because there doesn't seem to exist an implicit
	// conversion of LARGE_INTEGER to ULONGLONG :
	RETURN(*((PULONGLONG)&InterruptTime));
}

// ******************************************************************
// * 0x007E - KeQueryPerformanceCounter()
// ******************************************************************
XBSYSAPI EXPORTNUM(126) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceCounter(void)
{
	LOG_FUNC();

	::LARGE_INTEGER PerformanceCounter;

	// TODO : When Cxbx emulates the RDTSC opcode, use the same handling here.

	// Dxbx note : Xbox actually uses the RDTSC machine code instruction for this,
	// and we we're bound to a single core, so we could do that too, but on Windows
	// rdtsc is not a very stable counter, so instead, we'll use the native PeformanceCounter :
	QueryPerformanceCounter(&PerformanceCounter);

	// Re-base the performance counter to increase accuracy of the following conversion :
	PerformanceCounter.QuadPart -= NativePerformanceCounter.QuadPart;
	// We appy a conversion factor here, to fake Xbox1-like increment-speed behaviour :
	PerformanceCounter.QuadPart = (ULONGLONG)(NativeToXbox_FactorForPerformanceFrequency * PerformanceCounter.QuadPart);

	RETURN(PerformanceCounter.QuadPart);
}

// ******************************************************************
// * 0x007F - KeQueryPerformanceFrequency()
// ******************************************************************
XBSYSAPI EXPORTNUM(127) xboxkrnl::ULONGLONG NTAPI xboxkrnl::KeQueryPerformanceFrequency(void)
{
	LOG_FUNC();

	// Dxbx note : We return the real Xbox1 frequency here,
	// to make subsequent calculations behave the same as on the real Xbox1 :
	ULONGLONG ret = XBOX_PERFORMANCE_FREQUENCY;

	RETURN(ret);
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

	if(KeGetCurrentIrql() > DISPATCH_LEVEL)
		CxbxKrnlCleanup("Bugcheck: Caller of KeRaiseIrqlToDpcLevel is higher than DISPATCH_LEVEL!");

	KIRQL kRet = NULL;
	KeGetPcr()->Irql = DISPATCH_LEVEL;

#ifdef _DEBUG_TRACE
	DbgPrintf("Raised IRQL to DISPATCH_LEVEL (2).\n");
	DbgPrintf("Old IRQL is %d.\n", kRet);
#endif

	// We reached the DISPATCH_LEVEL, so the queue can be processed now :
	// TODO : ExecuteDpcQueue();
	
	RETURN(kRet);
}

// ******************************************************************
// * 0x0089 - KeRemoveQueueDpc()
// ******************************************************************
XBSYSAPI EXPORTNUM(137) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeRemoveQueueDpc
(
	IN PKDPC Dpc
)
{
	LOG_FUNC_ONE_ARG(Dpc);

	if (Dpc->DpcListEntry.Flink != NULL)
	{
		EnterCriticalSection(&(g_DpcData.Lock));
		if (Dpc->DpcListEntry.Flink != NULL)
		{
			RemoveEntryList(&(Dpc->DpcListEntry));
			Dpc->DpcListEntry.Flink = NULL;
		}

		LeaveCriticalSection(&(g_DpcData.Lock));
	}

	return TRUE;
}

// ******************************************************************
// * 0x008A - KeResetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(138) xboxkrnl::LONG NTAPI xboxkrnl::KeResetEvent
(
	IN PRKEVENT Event
)
{
	LOG_FUNC_ONE_ARG(Event);

	// TODO : Untested & incomplete
	LONG ret = Event->Header.SignalState;
	Event->Header.SignalState = 0;

	return ret;
}

// ******************************************************************
// * 0x008F - KeSetBasePriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(143) xboxkrnl::LONG NTAPI xboxkrnl::KeSetBasePriorityThread
(
	IN PKTHREAD  Thread,
	IN LONG  Priority
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Thread)
		LOG_FUNC_ARG_OUT(Priority)
		LOG_FUNC_END;

	LONG ret = GetThreadPriority((HANDLE)Thread);

	// This would work normally, but it will slow down the emulation, 
	// don't do that if the priority is higher then normal (so our own)!
	if((Priority <= THREAD_PRIORITY_NORMAL) && ((HANDLE)Thread != GetCurrentThread())) {
		SetThreadPriority((HANDLE)Thread, Priority);
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0091 - KeSetEvent()
// ******************************************************************
XBSYSAPI EXPORTNUM(145) xboxkrnl::LONG NTAPI xboxkrnl::KeSetEvent
(
	IN PRKEVENT		Event,
	IN KPRIORITY	Increment,
	IN BOOLEAN		Wait	
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Event)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Wait)
		LOG_FUNC_END;

	// TODO : Untested & incomplete
	LONG ret = Event->Header.SignalState;
	Event->Header.SignalState = TRUE;

	RETURN(ret);
}

// ******************************************************************
// * 0x0094 - KeSetPriorityThread()
// ******************************************************************
XBSYSAPI EXPORTNUM(148) xboxkrnl::BOOLEAN NTAPI xboxkrnl::KeSetPriorityThread
(
    IN PKTHREAD  Thread,
    IN LONG  Priority
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

#define KiRemoveTreeTimer(Timer)               \
    (Timer)->Header.Inserted = FALSE;          \
    RemoveEntryList(&(Timer)->TimerListEntry)

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

	if (Timer->Header.Type != 8 && Timer->Header.Type != 9) {
		CxbxKrnlCleanup("Assertion: '(Timer)->Header.Type == TimerNotificationObject) || ((Timer)->Header.Type == TimerSynchronizationObject)' in KeSetTimerEx()");
	}

	Inserted = Timer->Header.Inserted;
	if (Inserted != FALSE) {
		// Do some unlinking if already inserted in the linked list
		KiRemoveTreeTimer(Timer);
	}

	Timer->Header.SignalState = FALSE;
	Timer->Dpc = Dpc;
	Timer->Period = Period;

	if (/*!KiInsertTreeTimer(Timer, DueTime)*/ TRUE) {
		if (!IsListEmpty(&(Timer->Header.WaitListHead))) {
			// KiWaitTest(Timer, 0);
		}

		if (Dpc != NULL) {
			// Call the Dpc routine if one is specified
			KeQuerySystemTime(&SystemTime);
			KeInsertQueueDpc(Timer->Dpc, (PVOID)SystemTime.u.LowPart, (PVOID)SystemTime.u.HighPart);
		}

		if (Period != 0) {
			// Prepare the repetition if Timer is periodic
			Interval.QuadPart = (LONGLONG)(-10 * 1000) * Timer->Period;
			while (/*!KiInsertTreeTimer(Timer, Interval)*/FALSE) {
				;
			}
		}
	}
/* Dxbx has this :
	EnterCriticalSection(&(g_DpcData.Lock));
	if (Timer->TimerListEntry.Flink == nullptr) 
	{
		Timer->DueTime.QuadPart := (DueTime.QuadPart / -10000) + CxbxXboxGetTickCount();
		Timer->Period = Period;
		Timer->Dpc = Dpc;
		InsertTailList(&(g_DpcData.TimerQueue), &(Timer->TimerListEntry));
	}

	LeaveCriticalSection(&(g_DpcData.Lock));
	SetEvent(g_DpcData.DpcEvent);
*/

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
	LOG_FUNC_ONE_ARG(MicroSeconds);

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
	IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
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
		// TODO : What should we do with the (currently ignored)
		//        WaitReason, WaitMode, WaitBlockArray?

		if (Count == 1)
		{
			// Note : WaitType is irrelevant here
			ret = NtDll::NtWaitForSingleObject(
				Object[0],
				Alertable,
				(NtDll::PLARGE_INTEGER)Timeout);

			DbgPrintf("Finished waiting for 0x%.08X\n", Object[0]);
		}
		else
			// Unused arguments : WaitReason, WaitMode, WaitBlockArray
			ret = NtDll::NtWaitForMultipleObjects(
				Count,
				Object,
				(NtDll::OBJECT_WAIT_TYPE)WaitType,
				Alertable,
				(NtDll::PLARGE_INTEGER)Timeout);

		if (FAILED(ret))
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
	LOG_FORWARD("KeWaitForMultipleObjects");

	return xboxkrnl::KeWaitForMultipleObjects(
		/*Count=*/1,
		&Object,
		/*WaitType=*/WaitAll,
		WaitReason,
		WaitMode,
		Alertable,
		Timeout,
		/*WaitBlockArray*/NULL
	);
}
