// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXKrnl.cpp
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

#include "Cxbx.h"
#include "EmuX.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

using namespace win32;

// ******************************************************************
// * Loaded at run-time to avoid linker conflicts
// ******************************************************************
xntdll::FPTR_RtlInitAnsiString NT_RtlInitAnsiString = (xntdll::FPTR_RtlInitAnsiString)GetProcAddress(GetModuleHandle("ntdll"), "RtlInitAnsiString");

// ******************************************************************
// * (HELPER) PsCreateSystemThreadExProxyParam
// ******************************************************************
typedef struct _PsCreateSystemThreadExProxyParam
{
    IN PVOID StartContext1;
    IN PVOID StartContext2;
    IN PVOID StartRoutine;
}
PsCreateSystemThreadExProxyParam;

// ******************************************************************
// * PsCreateSystemThreadExProxy
// ******************************************************************
#pragma warning(push)
#pragma warning(disable: 4731)  // disable ebp modification warning
DWORD WINAPI PsCreateSystemThreadExProxy
(
    IN PVOID Parameter
)
{
    PsCreateSystemThreadExProxyParam *iPsCreateSystemThreadExProxyParam = (PsCreateSystemThreadExProxyParam*)Parameter;

    uint32 StartContext1 = (uint32)iPsCreateSystemThreadExProxyParam->StartContext1;
    uint32 StartContext2 = (uint32)iPsCreateSystemThreadExProxyParam->StartContext2;
    uint32 StartRoutine  = (uint32)iPsCreateSystemThreadExProxyParam->StartRoutine;

    delete iPsCreateSystemThreadExProxyParam;

    EmuXGenerateFS();

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): PsCreateSystemThreadExProxy\n"
               "(\n"
               "   StartContext1       : 0x%.08X\n"
               "   StartContext2       : 0x%.08X\n"
               "   StartRoutine        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), StartContext1, StartContext2, StartRoutine);
    }
    #endif

    EmuXSwapFS();   // Xbox FS

    __asm
    {
        mov         esi, StartRoutine
        push        StartContext2
        push        StartContext1
        push        offset callComplete
        lea         ebp, [esp-4]
        jmp near    esi

callComplete:

        nop
    }

    // EmuXSwapFS();

    return 0;
}
#pragma warning(pop)

using namespace xboxkrnl;

// ******************************************************************
// * 0x0018 ExQueryNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(24) NTSTATUS NTAPI xboxkrnl::ExQueryNonVolatileSetting
(
	IN  DWORD               ValueIndex,
	OUT DWORD              *Type,
	OUT PUCHAR              Value,
	IN  SIZE_T              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): ExQueryNonVolatileSetting\n"
               "(\n"
               "   ValueIndex          : 0x%.08X\n"
               "   Type                : 0x%.08X\n"
               "   Value               : 0x%.08X\n"
               "   ValueLength         : 0x%.08X\n"
               "   ResultLength        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ValueIndex, Type, Value, ValueLength, ResultLength);
    }
    #endif

    // ******************************************************************
    // * handle eeprom read
    // ******************************************************************
    switch(ValueIndex)
    {
        case EEPROM_MISC:
        {
            if(Type != 0)
                *Type  = 0x04;

            if(Value != 0)
                *Value = 0;

            if(ResultLength != 0)
                *ResultLength = 0x04;
        }
        break;

        default:
        {
            printf("EmuXKrnl (0x%.08X): ExQueryNonVolatileSetting unknown ValueIndex : %.08X\n", ValueIndex);
        }
        break;
    }

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0031 - HalReturnToFirmware
// ******************************************************************
XBSYSAPI EXPORTNUM(49) VOID DECLSPEC_NORETURN xboxkrnl::HalReturnToFirmware
(
	RETURN_FIRMWARE Routine
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): HalReturnToFirmware\n"
               "(\n"
               "   Routine             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Routine);
    }
    #endif

    MessageBox(NULL, "HalReturnToFirmware()", "EmuXKrnl", MB_OK);

    ExitProcess(0);

    /*
    ReturnFirmwareHalt          = 0x0,
    ReturnFirmwareReboot        = 0x1,
    ReturnFirmwareQuickReboot   = 0x2,
    ReturnFirmwareHard          = 0x3,
    ReturnFirmwareFatal         = 0x4,
    ReturnFirmwareAll           = 0x5
    */

    EmuXSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x0063 - KeDelayExecutionThread
// ******************************************************************
XBSYSAPI EXPORTNUM(99) NTSTATUS NTAPI xboxkrnl::KeDelayExecutionThread
(
    IN KPROCESSOR_MODE  WaitMode,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): KeDelayExecutionThread\n"
               "(\n"
               "   WaitMode            : 0x%.08X\n"
               "   Alertable           : 0x%.08X\n"
               "   Interval            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), WaitMode, Alertable, Interval);
    }
    #endif

    // TODO: Worry about Interval.LargePart if necessary
    if((sint32)Interval->LowPart < 0)
        Sleep(-(sint32)Interval->LowPart/10000);
    else
        EmuXPanic();

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
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
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): KeInitializeDpc\n"
               "(\n"
               "   Dpc                 : 0x%.08X\n"
               "   DeferredRoutine     : 0x%.08X\n"
               "   DeferredContext     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Dpc, DeferredRoutine, DeferredContext);
    }
    #endif

    Dpc->Number = 0;
	Dpc->DeferredRoutine = DeferredRoutine;
    Dpc->Type = DpcObject;
	Dpc->DeferredContext = DeferredContext;

    EmuXSwapFS();   // Xbox FS

    return;
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
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): KeInitializeTimerEx\n"
               "(\n"
               "   Timer               : 0x%.08X\n"
               "   Type                : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Timer, Type);
    }
    #endif
    
    Timer->Header.Type               = Type + 8;
    Timer->Header.Inserted           = 0;
    Timer->Header.Size               = sizeof(*Timer) / sizeof(ULONG);
    Timer->Header.SignalState        = 0;
    Timer->TimerListEntry.Blink      = NULL;
    Timer->TimerListEntry.Flink      = NULL;
    Timer->Header.WaitListHead.Flink = &Timer->Header.WaitListHead;
    Timer->Header.WaitListHead.Blink = &Timer->Header.WaitListHead;
    Timer->DueTime.QuadPart          = 0;
    Timer->Period                    = 0;

    EmuXSwapFS();   // Xbox FS

    return;
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
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): KeSetTimer\n"
               "(\n"
               "   Timer               : 0x%.08X\n"
               "   DueTime             : 0x%I64X\n"
               "   Dpc                 : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Timer, DueTime, Dpc);
    }
    #endif

    EmuXSwapFS();   // Xbox FS

    return FALSE;
}

// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(184) NTSTATUS xboxkrnl::NtAllocateVirtualMemory
(
    IN OUT PVOID    *BaseAddress,
    IN ULONG         ZeroBits,
    IN OUT SIZE_T    AllocationSize,
    IN DWORD         AllocationType,
    IN DWORD         Protect
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): NtAllocateVirtualMemory\n"
               "(\n"
               "   BaseAddress         : 0x%.08X\n"
               "   ZeroBits            : 0x%.08X\n"
               "   AllocationSize      : 0x%.08X\n"
               "   AllocationType      : 0x%.08X\n"
               "   Protect             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect);
    }
    #endif

    *BaseAddress = VirtualAlloc(*BaseAddress, AllocationSize, AllocationType, Protect);

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00BB - NtClose
// ******************************************************************
XBSYSAPI EXPORTNUM(187) NTSTATUS NTAPI xboxkrnl::NtClose
(
	IN HANDLE Handle
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): NtClose\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

    if(CloseHandle(Handle) != TRUE)
        return STATUS_UNSUCCESSFUL;

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00CA - NtOpenFile
// ******************************************************************
XBSYSAPI EXPORTNUM(202) NTSTATUS xboxkrnl::NtOpenFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               OpenOptions
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): NtOpenFile\n"
               "(\n"
               "   FileHandle          : 0x%.08X\n"
               "   DesiredAccess       : 0x%.08X\n"
               "   ObjectAttributes    : 0x%.08X\n"
               "   IoStatusBlock       : 0x%.08X\n"
               "   ShareAccess         : 0x%.08X\n"
               "   OpenOptions         : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), FileHandle, DesiredAccess, ObjectAttributes, 
               IoStatusBlock, ShareAccess, OpenOptions);
    }
    #endif

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00DA - NtQueryVolumeInformationFile
// ******************************************************************
XBSYSAPI EXPORTNUM(218) NTSTATUS NTAPI xboxkrnl::NtQueryVolumeInformationFile
(
    IN  HANDLE                  FileHandle,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FS_INFORMATION_CLASS    FileInformationClass
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): NtQueryVolumeInformationFile\n"
               "(\n"
               "   FileHandle          : 0x%.08X\n"
               "   IoStatusBlock       : 0x%.08X\n"
               "   FileInformation     : 0x%.08X\n"
               "   Length              : 0x%.08X\n"
               "   FileInformationClass: 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), FileHandle, IoStatusBlock, FileInformation, 
               Length, FileInformationClass);
    }
    #endif

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x00FF - PsCreateSystemThreadEx
// ******************************************************************
XBSYSAPI EXPORTNUM(255) NTSTATUS NTAPI xboxkrnl::PsCreateSystemThreadEx
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
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): PsCreateSystemThreadEx\n"
               "(\n"
               "   ThreadHandle        : 0x%.08X\n"
               "   ThreadExtraSize     : 0x%.08X\n"
               "   KernelStackSize     : 0x%.08X\n"
               "   TlsDataSize         : 0x%.08X\n"
               "   ThreadId            : 0x%.08X\n"
               "   StartContext1       : 0x%.08X\n"
               "   StartContext2       : 0x%.08X\n"
               "   CreateSuspended     : 0x%.08X\n"
               "   DebugStack          : 0x%.08X\n"
               "   StartRoutine        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), ThreadHandle, ThreadExtraSize, KernelStackSize, TlsDataSize, ThreadId,
               StartContext1, StartContext2, CreateSuspended, DebugStack, StartRoutine);
    }
    #endif

    DWORD dwThreadId = NULL;

    // PsCreateSystemThreadExProxy is responsible for cleaning up this pointer
    ::PsCreateSystemThreadExProxyParam *iPsCreateSystemThreadProxyParam = new ::PsCreateSystemThreadExProxyParam();

    iPsCreateSystemThreadProxyParam->StartContext1 = StartContext1;
    iPsCreateSystemThreadProxyParam->StartContext2 = StartContext2;
    iPsCreateSystemThreadProxyParam->StartRoutine  = StartRoutine;

    *ThreadHandle = CreateThread(NULL, NULL, &PsCreateSystemThreadExProxy, iPsCreateSystemThreadProxyParam, NULL, &dwThreadId);

    if(ThreadId != NULL)
        *ThreadId = dwThreadId;

    EmuXSwapFS();   // Xbox FS

    return STATUS_SUCCESS;
}

// ******************************************************************
// * 0x0115 RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(277) VOID NTAPI xboxkrnl::RtlEnterCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): RtlEnterCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    // We have to initialize this because the Xbox software doesn't seem
    // to always do it. Redundant initializations seem to be ok :/
    InitializeCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EnterCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuXSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x0121 - RtlInitAnsiString
// ******************************************************************
XBSYSAPI EXPORTNUM(289) VOID NTAPI xboxkrnl::RtlInitAnsiString 
(
  IN OUT PANSI_STRING DestinationString,
  IN     PCSZ         SourceString
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): RtlInitAnsiString\n"
               "(\n"
               "   DestinationString   : 0x%.08X\n"
               "   SourceString        : 0x%.08X (\"%s\")\n"
               ");\n",
               GetCurrentThreadId(), DestinationString, SourceString, SourceString);
    }
    #endif

    NT_RtlInitAnsiString((xntdll::PANSI_STRING)DestinationString, (xntdll::PCSZ)SourceString);

    EmuXSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0123 - RtlInitializeCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(291) VOID NTAPI xboxkrnl::RtlInitializeCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): RtlInitializeCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    InitializeCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuXSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x0126 RtlEnterCriticalSection
// ******************************************************************
XBSYSAPI EXPORTNUM(294) VOID NTAPI xboxkrnl::RtlLeaveCriticalSection
(
  IN PRTL_CRITICAL_SECTION CriticalSection
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): RtlLeaveCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    LeaveCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuXSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x012D - RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) xboxkrnl::ULONG NTAPI xboxkrnl::RtlNtStatusToDosError
(
	IN NTSTATUS	Status
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuXKrnl (0x%.08X): RtlNtStatusToDosError\n"
               "(\n"
               "   Status              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Status);
    }
    #endif

    EmuXSwapFS();   // Xbox FS

    return 0;
}