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
#define _XBOXKRNL_LOCAL_

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

NtDll::FPTR_RtlInitAnsiString              NtDll::RtlInitAnsiString            = (NtDll::FPTR_RtlInitAnsiString)GetProcAddress(hNtDll, "RtlInitAnsiString");
NtDll::FPTR_RtlInitUnicodeString           NtDll::RtlInitUnicodeString         = (NtDll::FPTR_RtlInitUnicodeString)GetProcAddress(hNtDll, "RtlInitUnicodeString");
NtDll::FPTR_RtlAnsiStringToUnicodeString   NtDll::RtlAnsiStringToUnicodeString = (NtDll::FPTR_RtlAnsiStringToUnicodeString)GetProcAddress(hNtDll, "RtlAnsiStringToUnicodeString");
NtDll::FPTR_RtlUnicodeStringToAnsiString   NtDll::RtlUnicodeStringToAnsiString = (NtDll::FPTR_RtlUnicodeStringToAnsiString)GetProcAddress(hNtDll, "RtlUnicodeStringToAnsiString");
NtDll::FPTR_RtlNtStatusToDosError          NtDll::RtlNtStatusToDosError        = (NtDll::FPTR_RtlNtStatusToDosError)GetProcAddress(hNtDll, "RtlNtStatusToDosError");
NtDll::FPTR_RtlInitializeCriticalSection   NtDll::RtlInitializeCriticalSection = (NtDll::FPTR_RtlInitializeCriticalSection)GetProcAddress(hNtDll, "RtlInitializeCriticalSection");
NtDll::FPTR_RtlEnterCriticalSection        NtDll::RtlEnterCriticalSection      = (NtDll::FPTR_RtlEnterCriticalSection)GetProcAddress(hNtDll, "RtlEnterCriticalSection");
NtDll::FPTR_RtlLeaveCriticalSection        NtDll::RtlLeaveCriticalSection      = (NtDll::FPTR_RtlLeaveCriticalSection)GetProcAddress(hNtDll, "RtlLeaveCriticalSection");
NtDll::FPTR_RtlCreateHeap                  NtDll::RtlCreateHeap                = (NtDll::FPTR_RtlCreateHeap)GetProcAddress(hNtDll, "RtlCreateHeap");
NtDll::FPTR_RtlAllocateHeap                NtDll::RtlAllocateHeap              = (NtDll::FPTR_RtlAllocateHeap)GetProcAddress(hNtDll, "RtlAllocateHeap");
NtDll::FPTR_RtlFreeHeap                    NtDll::RtlFreeHeap                  = (NtDll::FPTR_RtlFreeHeap)GetProcAddress(hNtDll, "RtlFreeHeap");
NtDll::FPTR_RtlSizeHeap                    NtDll::RtlSizeHeap                  = (NtDll::FPTR_RtlSizeHeap)GetProcAddress(hNtDll, "RtlSizeHeap");
NtDll::FPTR_NtWaitForSingleObject          NtDll::NtWaitForSingleObject        = (NtDll::FPTR_NtWaitForSingleObject)GetProcAddress(hNtDll, "NtWaitForSingleObject");
NtDll::FPTR_NtAllocateVirtualMemory        NtDll::NtAllocateVirtualMemory      = (NtDll::FPTR_NtAllocateVirtualMemory)GetProcAddress(hNtDll, "NtAllocateVirtualMemory");
NtDll::FPTR_NtFreeVirtualMemory            NtDll::NtFreeVirtualMemory          = (NtDll::FPTR_NtFreeVirtualMemory)GetProcAddress(hNtDll, "NtFreeVirtualMemory");
NtDll::FPTR_NtClearEvent                   NtDll::NtClearEvent                 = (NtDll::FPTR_NtClearEvent)GetProcAddress(hNtDll, "NtClearEvent");
NtDll::FPTR_NtClose                        NtDll::NtClose                      = (NtDll::FPTR_NtClose)GetProcAddress(hNtDll, "NtClose");
NtDll::FPTR_NtDelayExecution               NtDll::NtDelayExecution             = (NtDll::FPTR_NtDelayExecution)GetProcAddress(hNtDll, "NtDelayExecution");
NtDll::FPTR_NtDuplicateObject              NtDll::NtDuplicateObject            = (NtDll::FPTR_NtDuplicateObject)GetProcAddress(hNtDll, "NtDuplicateObject");
NtDll::FPTR_NtQueryInformationFile         NtDll::NtQueryInformationFile       = (NtDll::FPTR_NtQueryInformationFile)GetProcAddress(hNtDll, "NtQueryInformationFile");
NtDll::FPTR_NtQueryDirectoryFile           NtDll::NtQueryDirectoryFile         = (NtDll::FPTR_NtQueryDirectoryFile)GetProcAddress(hNtDll, "NtQueryDirectoryFile");
NtDll::FPTR_NtQueryFullAttributesFile      NtDll::NtQueryFullAttributesFile    = (NtDll::FPTR_NtQueryFullAttributesFile)GetProcAddress(hNtDll, "NtQueryFullAttributesFile");
NtDll::FPTR_NtQueryVolumeInformationFile   NtDll::NtQueryVolumeInformationFile = (NtDll::FPTR_NtQueryVolumeInformationFile)GetProcAddress(hNtDll, "NtQueryVolumeInformationFile");
NtDll::FPTR_NtCreateEvent                  NtDll::NtCreateEvent                = (NtDll::FPTR_NtCreateEvent)GetProcAddress(hNtDll, "NtCreateEvent");
NtDll::FPTR_NtCreateMutant                 NtDll::NtCreateMutant               = (NtDll::FPTR_NtCreateMutant)GetProcAddress(hNtDll, "NtCreateMutant");
NtDll::FPTR_NtCreateFile                   NtDll::NtCreateFile                 = (NtDll::FPTR_NtCreateFile)GetProcAddress(hNtDll, "NtCreateFile");
NtDll::FPTR_NtReadFile                     NtDll::NtReadFile                   = (NtDll::FPTR_NtReadFile)GetProcAddress(hNtDll, "NtReadFile");
NtDll::FPTR_NtWriteFile                    NtDll::NtWriteFile                  = (NtDll::FPTR_NtWriteFile)GetProcAddress(hNtDll, "NtWriteFile");
NtDll::FPTR_NtYieldExecution               NtDll::NtYieldExecution             = (NtDll::FPTR_NtYieldExecution)GetProcAddress(hNtDll, "NtYieldExecution");
NtDll::FPTR_NtSetInformationFile           NtDll::NtSetInformationFile         = (NtDll::FPTR_NtSetInformationFile)GetProcAddress(hNtDll, "NtSetInformationFile");
NtDll::FPTR_NtSetEvent                     NtDll::NtSetEvent                   = (NtDll::FPTR_NtSetEvent)GetProcAddress(hNtDll, "NtSetEvent");
NtDll::FPTR_NtResumeThread                 NtDll::NtResumeThread               = (NtDll::FPTR_NtResumeThread)GetProcAddress(hNtDll, "NtResumeThread");
NtDll::FPTR_NtSetLdtEntries                NtDll::NtSetLdtEntries              = (NtDll::FPTR_NtSetLdtEntries)GetProcAddress(hNtDll, "NtSetLdtEntries");
