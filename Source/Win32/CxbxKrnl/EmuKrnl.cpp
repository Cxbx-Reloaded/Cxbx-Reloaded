// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnl.cpp
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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <stdio.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

#include "Emu.h"
#include "EmuFS.h"
#include "EmuFile.h"

// ******************************************************************
// * Loaded at run-time to avoid linker conflicts
// ******************************************************************
HMODULE hNtDll = GetModuleHandle("ntdll");

xntdll::FPTR_RtlInitAnsiString            NT_RtlInitAnsiString            = (xntdll::FPTR_RtlInitAnsiString)GetProcAddress(hNtDll, "RtlInitAnsiString");
xntdll::FPTR_RtlNtStatusToDosError        NT_RtlNtStatusToDosError        = (xntdll::FPTR_RtlNtStatusToDosError)GetProcAddress(hNtDll, "RtlNtStatusToDosError");
xntdll::FPTR_NtAllocateVirtualMemory      NT_NtAllocateVirtualMemory      = (xntdll::FPTR_NtAllocateVirtualMemory)GetProcAddress(hNtDll, "NtAllocateVirtualMemory");
xntdll::FPTR_NtClose                      NT_NtClose                      = (xntdll::FPTR_NtClose)GetProcAddress(hNtDll, "NtClose");
xntdll::FPTR_RtlInitializeCriticalSection NT_RtlInitializeCriticalSection = (xntdll::FPTR_RtlInitializeCriticalSection)GetProcAddress(hNtDll, "RtlInitializeCriticalSection");
xntdll::FPTR_RtlEnterCriticalSection      NT_RtlEnterCriticalSection      = (xntdll::FPTR_RtlEnterCriticalSection)GetProcAddress(hNtDll, "RtlEnterCriticalSection");
xntdll::FPTR_RtlLeaveCriticalSection      NT_RtlLeaveCriticalSection      = (xntdll::FPTR_RtlLeaveCriticalSection)GetProcAddress(hNtDll, "RtlLeaveCriticalSection");

// ******************************************************************
// * (Helper) PCSTProxyParam
// ******************************************************************
typedef struct _PCSTProxyParam
{
    IN PVOID StartContext1;
    IN PVOID StartContext2;
    IN PVOID StartRoutine;
}
PCSTProxyParam;

// ******************************************************************
// * (Helper) PCSTProxy
// ******************************************************************
#pragma warning(push)
#pragma warning(disable: 4731)  // disable ebp modification warning
DWORD WINAPI PCSTProxy
(
    IN PVOID Parameter
)
{
    PCSTProxyParam *iPCSTProxyParam = (PCSTProxyParam*)Parameter;

    uint32 StartContext1 = (uint32)iPCSTProxyParam->StartContext1;
    uint32 StartContext2 = (uint32)iPCSTProxyParam->StartContext2;
    uint32 StartRoutine  = (uint32)iPCSTProxyParam->StartRoutine;

    delete iPCSTProxyParam;

    EmuGenerateFS(g_TlsAdjust);

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): PCSTProxy\n"
               "(\n"
               "   StartContext1       : 0x%.08X\n"
               "   StartContext2       : 0x%.08X\n"
               "   StartRoutine        : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), StartContext1, StartContext2, StartRoutine);
    }
    #endif

    EmuSwapFS();   // Xbox FS

    // ******************************************************************
    // * use the special calling convention
    // ******************************************************************
    __try
    {
        __asm
        {
            mov         esi, StartRoutine
            push        StartContext2
            push        StartContext1
            push        offset callComplete
            lea         ebp, [esp-4]
            jmp near    esi
        }
    }
    __except(EmuException(GetExceptionInformation()))
    {
        printf("Emu: WARNING!! Problem with ExceptionFilter\n");
    }

callComplete:

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): ExQueryNonVolatileSetting\n"
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
            printf("EmuKrnl (0x%.08X): ExQueryNonVolatileSetting unknown ValueIndex : %.08X\n", ValueIndex);
            break;
    }

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): HalReturnToFirmware\n"
               "(\n"
               "   Routine             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Routine);
    }
    #endif

    MessageBox(NULL, "Warning: XBE is quitting (HalReturnToFirmware).\n\nThis process may not terminate elegantly.", "Cxbx", MB_OK);

    ExitProcess(0);

    EmuSwapFS();   // Xbox FS
}

// ******************************************************************
// * 0x0042 - IoCreateFile
// ******************************************************************
XBSYSAPI EXPORTNUM(66) NTSTATUS NTAPI xboxkrnl::IoCreateFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               Disposition,
    IN  ULONG               CreateOptions,
    IN  ULONG               Options
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): IoCreateFile\n"
               "(\n"
               "   FileHandle          : 0x%.08X\n"
               "   DesiredAccess       : 0x%.08X\n"
               "   ObjectAttributes    : 0x%.08X (%s)\n"
               "   IoStatusBlock       : 0x%.08X\n"
               "   AllocationSize      : 0x%.08X\n"
               "   FileAttributes      : 0x%.08X\n"
               "   ShareAccess         : 0x%.08X\n"
               "   Disposition         : 0x%.08X\n"
               "   CreateOptions       : 0x%.08X\n"
               "   Options             : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), FileHandle, DesiredAccess, ObjectAttributes, ObjectAttributes->ObjectName->Buffer,
               IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, Disposition, CreateOptions, Options);
    }
    #endif

    NTSTATUS ret = STATUS_SUCCESS;

    // ******************************************************************
    // * For now, just check for 'special' cases
    // ******************************************************************
    if(strcmp(ObjectAttributes->ObjectName->Buffer, "\\Device\\Harddisk0\\partition1\\") == 0)
    {
        EmuHandle *iEmuHandle = new EmuHandle;

        iEmuHandle->m_Type = EMUHANDLE_TYPE_PARTITION1;

        *FileHandle = PtrToEmuHandle(iEmuHandle);

        // TODO: Update IoStatusBlock if necessary
    }
    else if(strcmp(ObjectAttributes->ObjectName->Buffer, "\\Device\\Harddisk0\\partition1\\TDATA") == 0)
    {
        EmuHandle *iEmuHandle = new EmuHandle;

        iEmuHandle->m_Type = EMUHANDLE_TYPE_TDATA;

        *FileHandle = PtrToEmuHandle(iEmuHandle);

        // TODO: Update IoStatusBlock if necessary
    }
    else
    {
        EmuPanic();

        /* TODO: In the future, this will look something like...
        ret = xntdll::ZwCreateFile
        (
            FileHandle, DesiredAccess, (xntdll::_OBJECT_ATTRIBUTES*)ObjectAttributes, (xntdll::_IO_STATUS_BLOCK *)IoStatusBlock,
            (xntdll::_LARGE_INTEGER *)AllocationSize, FileAttributes, ShareAccess, Disposition, 
            CreateOptions, 0, 0
        );
        */
    }

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x0043 IoCreateSymbolicLink
// ******************************************************************
XBSYSAPI EXPORTNUM(67) NTSTATUS xboxkrnl::IoCreateSymbolicLink
(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): IoCreateSymbolicLink\n"
               "(\n"
               "   SymbolicLinkName    : 0x%.08X (%s)\n"
               "   DeviceName          : 0x%.08X (%s)\n"
               ");\n",
               GetCurrentThreadId(), SymbolicLinkName, SymbolicLinkName->Buffer,
               DeviceName, DeviceName->Buffer);
    }
    #endif

    // TODO: Actually um...implement this function
    NTSTATUS ret = STATUS_OBJECT_NAME_COLLISION;

    EmuSwapFS();   // Xbox FS

    return ret;
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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): KeDelayExecutionThread\n"
               "(\n"
               "   WaitMode            : 0x%.08X\n"
               "   Alertable           : 0x%.08X\n"
               "   Interval            : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), WaitMode, Alertable, Interval);
    }
    #endif

    // TODO: Worry about Interval.LargePart if necessary
    if((sint32)Interval->u.LowPart < 0)
        Sleep(-(sint32)Interval->u.LowPart/10000);
    else
        EmuPanic();

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): KeInitializeDpc\n"
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

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): KeInitializeTimerEx\n"
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

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): KeSetTimer\n"
               "(\n"
               "   Timer               : 0x%.08X\n"
               "   DueTime             : 0x%I64X\n"
               "   Dpc                 : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Timer, DueTime, Dpc);
    }
    #endif

    EmuSwapFS();   // Xbox FS

    return TRUE;
}

// ******************************************************************
// * 0x00A4 - LaunchDataPage (actually a pointer)
// ******************************************************************
XBSYSAPI EXPORTNUM(164) xboxkrnl::DWORD xboxkrnl::LaunchDataPage = 0;

// ******************************************************************
// * 0x00B8 - NtAllocateVirtualMemory
// ******************************************************************
XBSYSAPI EXPORTNUM(184) NTSTATUS xboxkrnl::NtAllocateVirtualMemory
(
    IN OUT PVOID    *BaseAddress,
    IN ULONG         ZeroBits,
    IN OUT PULONG    AllocationSize,
    IN DWORD         AllocationType,
    IN DWORD         Protect
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): NtAllocateVirtualMemory\n"
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

    NTSTATUS ret = NT_NtAllocateVirtualMemory(GetCurrentProcess(), BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BB - NtClose
// ******************************************************************
XBSYSAPI EXPORTNUM(187) NTSTATUS NTAPI xboxkrnl::NtClose
(
	IN HANDLE Handle
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): NtClose\n"
               "(\n"
               "   Handle              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

    NTSTATUS ret = STATUS_SUCCESS;
    
    // ******************************************************************
    // * delete 'special' handles
    // ******************************************************************
    if(IsEmuHandle(Handle))
    {
        EmuHandle *iEmuHandle = EmuHandleToPtr(Handle);

        delete iEmuHandle;

        ret = STATUS_SUCCESS;
    }
    // ******************************************************************
    // * close normal handles
    // ******************************************************************
    else
    {
        ret = NT_NtClose(Handle);
    }

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x00BE - NtCreateFile
// ******************************************************************
XBSYSAPI EXPORTNUM(190) NTSTATUS NTAPI xboxkrnl::NtCreateFile
(
	OUT PHANDLE             FileHandle, 
	IN  ACCESS_MASK         DesiredAccess,
	IN  POBJECT_ATTRIBUTES	ObjectAttributes,
	OUT PIO_STATUS_BLOCK	IoStatusBlock,
	IN  PLARGE_INTEGER	    AllocationSize OPTIONAL, 
	IN  ULONG	            FileAttributes, 
	IN  ULONG	            ShareAccess, 
	IN  ULONG	            CreateDisposition, 
	IN  ULONG	            CreateOptions 
)
{
    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS

        printf("EmuKrnl (0x%.08X): NtCreateFile [See IoCreateFile Params...]\n", GetCurrentThreadId());

        EmuSwapFS();   // Xbox FS
    }
    #endif

    xboxkrnl::IoCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, 0);

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
    // Note: Since we are simply redirecting to IoCreateFile, we do not
    //       want to swap FS registers like normal (except for when
    //       _DEBUG_TRACE is enabled)

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        EmuSwapFS();   // Win2k/XP FS

        printf("EmuKrnl (0x%.08X): NtOpenFile [See IoCreateFile Params...]\n", GetCurrentThreadId());

        EmuSwapFS();   // Xbox FS
    }
    #endif

    xboxkrnl::IoCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, 0, 0, ShareAccess, 1, OpenOptions, 0);

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): NtQueryVolumeInformationFile\n"
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

    // ******************************************************************
    // * For now, handle 'special' case
    // ******************************************************************
    if(IsEmuHandle(FileHandle))
    {
        EmuHandle *iEmuHandle = EmuHandleToPtr(FileHandle);

        // ******************************************************************
        // * For now, only handle partition 1 'special' case
        // ******************************************************************
        if(iEmuHandle->m_Type == EMUHANDLE_TYPE_PARTITION1 && FileInformationClass == FileFsSizeInformation)
        {
            FILE_FS_SIZE_INFORMATION *SizeInfo = (FILE_FS_SIZE_INFORMATION*)FileInformation;

            SizeInfo->TotalAllocationUnits.QuadPart     = 0x4C468;
            SizeInfo->AvailableAllocationUnits.QuadPart = 0x2F125;
            SizeInfo->SectorsPerAllocationUnit          = 32;
            SizeInfo->BytesPerSector                    = 512;
        }
        else
        {
            EmuPanic();
        }
    }
    else
    {
        EmuPanic();
    }

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): PsCreateSystemThreadEx\n"
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

    // ******************************************************************
    // * create thread, using our special proxy technique
    // ******************************************************************
    {
        DWORD dwThreadId;

        // PCSTProxy is responsible for cleaning up this pointer
        ::PCSTProxyParam *iPCSTProxyParam = new ::PCSTProxyParam();

        iPCSTProxyParam->StartContext1 = StartContext1;
        iPCSTProxyParam->StartContext2 = StartContext2;
        iPCSTProxyParam->StartRoutine  = StartRoutine;

        *ThreadHandle = CreateThread(NULL, NULL, &PCSTProxy, iPCSTProxyParam, NULL, &dwThreadId);

        if(ThreadId != NULL)
            *ThreadId = dwThreadId;
    }

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): RtlEnterCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    // This seems redundant, but xbox software doesn't always do it
    NT_RtlInitializeCriticalSection((xntdll::_RTL_CRITICAL_SECTION*)CriticalSection);

    NT_RtlEnterCriticalSection((xntdll::_RTL_CRITICAL_SECTION*)CriticalSection);

    EmuSwapFS();   // Xbox FS

    return;
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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): RtlInitAnsiString\n"
               "(\n"
               "   DestinationString   : 0x%.08X\n"
               "   SourceString        : 0x%.08X (\"%s\")\n"
               ");\n",
               GetCurrentThreadId(), DestinationString, SourceString, SourceString);
    }
    #endif

    NT_RtlInitAnsiString((xntdll::PANSI_STRING)DestinationString, (xntdll::PCSZ)SourceString);

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): RtlInitializeCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    NT_RtlInitializeCriticalSection((xntdll::_RTL_CRITICAL_SECTION*)CriticalSection);

    EmuSwapFS();   // Xbox FS

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
    EmuSwapFS();   // Win2k/XP FS

    // Note: We need to execute this before debug output to avoid trouble
    NT_RtlLeaveCriticalSection((xntdll::_RTL_CRITICAL_SECTION*)CriticalSection);

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): RtlLeaveCriticalSection\n"
               "(\n"
               "   CriticalSection     : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    EmuSwapFS();   // Xbox FS

    return;
}

// ******************************************************************
// * 0x012D - RtlNtStatusToDosError
// ******************************************************************
XBSYSAPI EXPORTNUM(301) xboxkrnl::ULONG NTAPI xboxkrnl::RtlNtStatusToDosError
(
	IN NTSTATUS	Status
)
{
    EmuSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG_TRACE
    {
        printf("EmuKrnl (0x%.08X): RtlNtStatusToDosError\n"
               "(\n"
               "   Status              : 0x%.08X\n"
               ");\n",
               GetCurrentThreadId(), Status);
    }
    #endif

    ULONG ret = NT_RtlNtStatusToDosError(Status);

    EmuSwapFS();   // Xbox FS

    return ret;
}

// ******************************************************************
// * 0x0141 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) XBOX_HARDWARE_INFO xboxkrnl::XboxHardwareInfo = 
{
    0xC0000035,
    0,0,0,0
};
