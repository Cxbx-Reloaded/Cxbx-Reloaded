// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuNtDll.cpp
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
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include <windows.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

// ******************************************************************
// * Loaded at run-time to avoid linker conflicts
// ******************************************************************
static HMODULE hNtDll = GetModuleHandle("ntdll");

#define IMPORT(API) NtDll::FPTR_##API NtDll::API = (NtDll::FPTR_##API)GetProcAddress(hNtDll, #API)

// Note : Keep IMPORT's sorted, to ease future sync's (and compares with EXTERN's):
/*
IMPORT(InterlockedCompareExchange);
IMPORT(InterlockedDecrement);
IMPORT(InterlockedExchange);
IMPORT(InterlockedExchangeAdd);
IMPORT(InterlockedFlushSList);
IMPORT(InterlockedIncrement);
IMPORT(InterlockedPopEntrySList);
IMPORT(InterlockedPushEntrySList);
*/
IMPORT(NtAllocateVirtualMemory);
IMPORT(NtCancelTimer);
IMPORT(NtClearEvent);
IMPORT(NtClose);
IMPORT(NtCreateDirectoryObject);
IMPORT(NtCreateEvent);
IMPORT(NtCreateFile);
IMPORT(NtCreateMutant);
IMPORT(NtCreateSemaphore);
IMPORT(NtCreateTimer);
IMPORT(NtDelayExecution);
IMPORT(NtDeviceIoControlFile);
IMPORT(NtDuplicateObject);
IMPORT(NtFlushBuffersFile);
IMPORT(NtFreeVirtualMemory);
IMPORT(NtFsControlFile);
IMPORT(NtOpenSymbolicLinkObject);
IMPORT(NtPulseEvent);
IMPORT(NtQueryDirectoryFile);
IMPORT(NtQueryFullAttributesFile);
IMPORT(NtQueryInformationFile);
IMPORT(NtQueryTimer);
IMPORT(NtQueryVirtualMemory);
IMPORT(NtQueryVolumeInformationFile);
IMPORT(NtQueueApcThread);
IMPORT(NtReadFile);
IMPORT(NtReleaseMutant);
IMPORT(NtReleaseSemaphore);
IMPORT(NtResumeThread);
IMPORT(NtSetEvent);
IMPORT(NtSetInformationFile);
IMPORT(NtSetLdtEntries);
IMPORT(NtSetTimer);
IMPORT(NtSuspendThread);
IMPORT(NtWaitForMultipleObjects);
IMPORT(NtWaitForSingleObject);
IMPORT(NtWriteFile);
IMPORT(NtYieldExecution);
IMPORT(RtlAllocateHeap);
IMPORT(RtlAnsiStringToUnicodeString);
IMPORT(RtlAppendStringToString);
IMPORT(RtlAppendUnicodeStringToString);
IMPORT(RtlAppendUnicodeToString);
IMPORT(RtlCharToInteger);
IMPORT(RtlCreateHeap);
IMPORT(RtlDestroyHeap);
IMPORT(RtlEnterCriticalSection);
IMPORT(RtlEqualString);
IMPORT(RtlFreeAnsiString);
IMPORT(RtlFreeHeap);
IMPORT(RtlInitAnsiString);
IMPORT(RtlInitializeCriticalSection);
IMPORT(RtlInitUnicodeString);
IMPORT(RtlLeaveCriticalSection);
IMPORT(RtlNtStatusToDosError);
IMPORT(RtlReAllocateHeap);
IMPORT(RtlSizeHeap);
IMPORT(RtlTimeFieldsToTime);
IMPORT(RtlTimeToTimeFields);
IMPORT(RtlTryEnterCriticalSection);
IMPORT(RtlUnicodeStringToAnsiString);
