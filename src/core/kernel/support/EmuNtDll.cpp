// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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
IMPORT(NtDeleteFile);
IMPORT(NtDeviceIoControlFile);
IMPORT(NtDuplicateObject);
IMPORT(NtFlushBuffersFile);
IMPORT(NtFreeVirtualMemory);
IMPORT(NtFsControlFile);
IMPORT(NtOpenSymbolicLinkObject);
IMPORT(NtPulseEvent);
IMPORT(NtQueryDirectoryFile);
IMPORT(NtQueryEvent);
IMPORT(NtQueryFullAttributesFile);
IMPORT(NtQueryInformationFile);
IMPORT(NtQueryMutant);
IMPORT(NtQuerySemaphore);
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
IMPORT(RtlCompareMemory);
IMPORT(RtlCompareMemoryUlong);
IMPORT(RtlCompareString);
IMPORT(RtlCompareUnicodeString);
IMPORT(RtlCopyString);
IMPORT(RtlCopyUnicodeString);
IMPORT(RtlCreateHeap);
IMPORT(RtlCreateUnicodeString);
IMPORT(RtlDestroyHeap);
IMPORT(RtlDowncaseUnicodeChar);
IMPORT(RtlDowncaseUnicodeString);
IMPORT(RtlEnterCriticalSection);
IMPORT(RtlEqualString);
IMPORT(RtlEqualUnicodeString);
IMPORT(RtlExtendedIntegerMultiply);
IMPORT(RtlExtendedLargeIntegerDivide);
IMPORT(RtlExtendedMagicDivide);
IMPORT(RtlFillMemory);
IMPORT(RtlFillMemoryUlong);
IMPORT(RtlFreeAnsiString);
IMPORT(RtlFreeHeap);
IMPORT(RtlFreeUnicodeString);
IMPORT(RtlIntegerToChar);
IMPORT(RtlIntegerToUnicodeString);
IMPORT(RtlInitAnsiString);
IMPORT(RtlInitializeCriticalSection);
IMPORT(RtlInitUnicodeString);
IMPORT(RtlLeaveCriticalSection);
IMPORT(RtlMapGenericMask);
IMPORT(RtlMultiByteToUnicodeN);
IMPORT(RtlMultiByteToUnicodeSize);
IMPORT(RtlNtStatusToDosError);
IMPORT(RtlReAllocateHeap);
IMPORT(RtlSizeHeap);
IMPORT(RtlTimeFieldsToTime);
IMPORT(RtlTimeToTimeFields);
IMPORT(RtlTryEnterCriticalSection);
IMPORT(RtlUlongByteSwap);
IMPORT(RtlUnicodeStringToAnsiString);
IMPORT(RtlUnicodeStringToInteger); 
IMPORT(RtlUnicodeToMultiByteN);
IMPORT(RtlUnicodeToMultiByteSize);
IMPORT(RtlUpcaseUnicodeChar);
IMPORT(RtlUpcaseUnicodeString);
IMPORT(RtlUpcaseUnicodeToMultiByteN);
IMPORT(RtlUpperString);
IMPORT(RtlUshortByteSwap);
