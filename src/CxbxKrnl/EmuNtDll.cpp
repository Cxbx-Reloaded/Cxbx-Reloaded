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

#define IMPORT(API) \
NtDll::FPTR_##API                          NtDll::API                          = (NtDll::FPTR_##API)GetProcAddress(hNtDll, #API)

IMPORT(RtlInitAnsiString);
IMPORT(RtlInitUnicodeString);
IMPORT(RtlAnsiStringToUnicodeString);
IMPORT(RtlAppendStringToString);
IMPORT(RtlAppendUnicodeStringToString);
IMPORT(RtlAppendUnicodeToString);
IMPORT(RtlUnicodeStringToAnsiString);
IMPORT(RtlFreeAnsiString);
IMPORT(RtlNtStatusToDosError);
IMPORT(RtlTimeFieldsToTime);
IMPORT(RtlTimeToTimeFields);
IMPORT(RtlTryEnterCriticalSection);
IMPORT(RtlInitializeCriticalSection);
IMPORT(RtlEnterCriticalSection);
IMPORT(RtlLeaveCriticalSection);
IMPORT(RtlCreateHeap);
IMPORT(RtlAllocateHeap);
IMPORT(RtlFreeHeap);
IMPORT(RtlReAllocateHeap);
IMPORT(RtlSizeHeap);
IMPORT(RtlDestroyHeap);
IMPORT(RtlEqualString);
IMPORT(NtWaitForSingleObject);
IMPORT(NtWaitForMultipleObjects);
IMPORT(NtAllocateVirtualMemory);
IMPORT(NtFreeVirtualMemory);
IMPORT(NtQueryVirtualMemory);
IMPORT(NtClearEvent);
IMPORT(NtClose);
IMPORT(NtDelayExecution);
IMPORT(NtDuplicateObject);
IMPORT(NtFlushBuffersFile);
IMPORT(NtQueryInformationFile);
IMPORT(NtQueryDirectoryFile);
IMPORT(NtQueryFullAttributesFile);
IMPORT(NtQueryVolumeInformationFile);
IMPORT(NtCreateEvent);
IMPORT(NtCreateMutant);
IMPORT(NtReleaseMutant);
IMPORT(NtCreateSemaphore);
IMPORT(NtReleaseSemaphore);
IMPORT(NtCreateDirectoryObject);
IMPORT(NtCreateFile);
IMPORT(NtReadFile);
IMPORT(NtWriteFile);
IMPORT(NtYieldExecution);
IMPORT(NtSetInformationFile);
IMPORT(NtSetEvent);
IMPORT(NtSuspendThread);
IMPORT(NtResumeThread);
IMPORT(NtSetLdtEntries);
IMPORT(NtQueueApcThread);
