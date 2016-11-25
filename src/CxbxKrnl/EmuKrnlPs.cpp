// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlPs.cpp
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
#include <xboxkrnl/xboxkrnl.h> // For PsCreateSystemThreadEx, etc.
};

#include <process.h> // For __beginthreadex(), etc.

#include "Logging.h" // For LOG_FUNC()

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

#include "CxbxKrnl.h" // For CxbxKrnl_TLS
#include "Emu.h" // For EmuWarning()
#include "EmuFS.h" // For EmuGenerateFS
#include "EmuXTL.h"

// PsCreateSystemThread proxy parameters
typedef struct _PCSTProxyParam
{
	IN PVOID  StartContext1;
	IN PVOID  StartContext2;
	IN PVOID  StartRoutine;
	IN BOOL   StartSuspended;
	IN HANDLE hStartedEvent;
}
PCSTProxyParam;

// Global Variable(s)
extern PVOID g_pfnThreadNotification[16] = { NULL };
extern int g_iThreadNotificationCount = 0;

// Separate function for logging, otherwise in PCSTProxy __try wont work (Compiler Error C2712)
void PCSTProxy_log
(
	uint32 StartContext1,
	uint32 StartContext2,
	uint32 StartRoutine,
	BOOL   StartSuspended,
	HANDLE hStartedEvent
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(StartContext1)
		LOG_FUNC_ARG(StartContext2)
		LOG_FUNC_ARG(StartRoutine)
		LOG_FUNC_END;
}

// PsCreateSystemThread proxy procedure
#pragma warning(push)
#pragma warning(disable: 4731)  // disable ebp modification warning
static unsigned int WINAPI PCSTProxy
(
	IN PVOID Parameter
)
{
	PCSTProxyParam *iPCSTProxyParam = (PCSTProxyParam*)Parameter;

	uint32 StartContext1 = (uint32)iPCSTProxyParam->StartContext1;
	uint32 StartContext2 = (uint32)iPCSTProxyParam->StartContext2;
	uint32 StartRoutine = (uint32)iPCSTProxyParam->StartRoutine;
	BOOL   StartSuspended = (BOOL)iPCSTProxyParam->StartSuspended;
	HANDLE hStartedEvent = (HANDLE)iPCSTProxyParam->hStartedEvent;

	// Once deleted, unable to directly access iPCSTProxyParam in remainder of function.
	delete iPCSTProxyParam;

	PCSTProxy_log(StartContext1, StartContext2, StartRoutine, StartSuspended, hStartedEvent);

	if (StartSuspended == TRUE)
		SuspendThread(GetCurrentThread());

	EmuGenerateFS(CxbxKrnl_TLS, CxbxKrnl_TLSData);

	// call thread notification routine(s)
	if (g_iThreadNotificationCount != 0)
	{
		for (int i = 0; i < 16; i++)
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if (pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmuKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", GetCurrentThreadId(), g_iThreadNotificationCount, pfnNotificationRoutine);



			pfnNotificationRoutine(TRUE);


		}
	}

	// use the special calling convention
	__try
	{
		SetEvent(hStartedEvent);

		__asm
		{
			mov         esi, StartRoutine
			push        StartContext2
			push        StartContext1
			push        offset callComplete
			lea         ebp, [esp - 4]
			jmp near    esi
		}
	}
	__except (EmuException(GetExceptionInformation()))
	{
		EmuWarning("Problem with ExceptionFilter!");
	}

callComplete:



	// call thread notification routine(s)
	if (g_iThreadNotificationCount != 0)
	{
		for (int i = 0; i < 16; i++)
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if (pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmuKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", GetCurrentThreadId(), g_iThreadNotificationCount, pfnNotificationRoutine);



			pfnNotificationRoutine(FALSE);


		}
	}

	CxbxKrnlTerminateThread();

	return 0;
}
#pragma warning(pop)

using namespace xboxkrnl;

// ******************************************************************
// * 0x00FF - PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) xboxkrnl::NTSTATUS NTAPI xboxkrnl::PsCreateSystemThreadEx
(
	OUT PHANDLE         ThreadHandle,
	IN  ULONG           ThreadExtraSize,
	IN  ULONG           KernelStackSize,
	IN  ULONG           TlsDataSize,
	OUT PULONG          ThreadId OPTIONAL,
	IN  PVOID           StartContext1,
	IN  PVOID           StartContext2,
	IN  BOOLEAN         CreateSuspended,
	IN  BOOLEAN         DebugStack,
	IN  PKSTART_ROUTINE StartRoutine
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(ThreadHandle)
		LOG_FUNC_ARG(ThreadExtraSize)
		LOG_FUNC_ARG(KernelStackSize)
		LOG_FUNC_ARG(TlsDataSize)
		LOG_FUNC_ARG_OUT(ThreadId)
		LOG_FUNC_ARG(StartContext1)
		LOG_FUNC_ARG(StartContext2)
		LOG_FUNC_ARG(CreateSuspended)
		LOG_FUNC_ARG(DebugStack)
		LOG_FUNC_ARG(StartRoutine)
		LOG_FUNC_END;

	static bool bFirstTime = false;

	// create thread, using our special proxy technique
	{
		DWORD dwThreadId;

		// PCSTProxy is responsible for cleaning up this pointer
		::PCSTProxyParam *iPCSTProxyParam = new ::PCSTProxyParam();

		iPCSTProxyParam->StartContext1 = StartContext1;
		iPCSTProxyParam->StartContext2 = StartContext2;
		iPCSTProxyParam->StartRoutine = StartRoutine;
		iPCSTProxyParam->StartSuspended = CreateSuspended;
		iPCSTProxyParam->hStartedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		*ThreadHandle = (HANDLE)_beginthreadex(NULL, NULL, PCSTProxy, iPCSTProxyParam, NULL, (uint*)&dwThreadId);

		// Make sure Xbox1 code runs on one core :
		SetThreadAffinityMask(ThreadHandle, g_CPUXbox);

		WaitForSingleObject(iPCSTProxyParam->hStartedEvent, 1000);

		//        *ThreadHandle = CreateThread(NULL, NULL, PCSTProxy, iPCSTProxyParam, NULL, &dwThreadId);

		DbgPrintf("EmuKrnl (0x%X): ThreadHandle : 0x%X, ThreadId : 0x%.08X\n", GetCurrentThreadId(), *ThreadHandle, dwThreadId);

		// we must duplicate this handle in order to retain Suspend/Resume thread rights from a remote thread
		{
			HANDLE hDupHandle = NULL;

			DuplicateHandle(GetCurrentProcess(), *ThreadHandle, GetCurrentProcess(), &hDupHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

			CxbxKrnlRegisterThread(hDupHandle);
		}

		if (ThreadId != NULL)
			*ThreadId = dwThreadId;
	}

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x0102 - PsTerminateSystemThread
// ******************************************************************
XBSYSAPI EXPORTNUM(258) VOID NTAPI xboxkrnl::PsTerminateSystemThread
(
	IN NTSTATUS ExitStatus
)
{
	LOG_FUNC_ONE_ARG(ExitStatus);

	// call thread notification routine(s)
	if (g_iThreadNotificationCount != 0)
	{
		for (int i = 0; i < 16; i++)
		{
			XTL::XTHREAD_NOTIFY_PROC pfnNotificationRoutine = (XTL::XTHREAD_NOTIFY_PROC)g_pfnThreadNotification[i];

			// If the routine doesn't exist, don't execute it!
			if (pfnNotificationRoutine == NULL)
				continue;

			DbgPrintf("EmuKrnl (0x%X): Calling pfnNotificationRoutine[%d] (0x%.08X)\n", GetCurrentThreadId(), g_iThreadNotificationCount, pfnNotificationRoutine);

			pfnNotificationRoutine(FALSE);
		}
	}

	//    CxbxKrnlTerminateThread();

	EmuCleanupFS();

	_endthreadex(ExitStatus);
	//ExitThread(ExitStatus);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(259) volatile xboxkrnl::POBJECT_TYPE xboxkrnl::PsThreadObjectType = NULL;
