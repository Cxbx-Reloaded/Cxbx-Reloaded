// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->CxbxKrnl.cpp
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
#include "Cxbx.h"

#define CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_
#include "CxbxKrnl.h"

#include "LDT.h"

#include <stdio.h>

// ******************************************************************
// * ntdll wrapped in namespace to avoid collisions
// ******************************************************************
namespace xntdll
{
    #include "xntdll.h"
};

// ******************************************************************
// * win32 wrapped in namespace to avoid collisions
// ******************************************************************
namespace win32
{
    #undef FIELD_OFFSET     // prevent macro redefinition warnings
    #include <windows.h>
};

using namespace win32;

// ******************************************************************
// * func: EmuXSwapFS
// ******************************************************************
// *
// * This function is used to swap between the native Win2k/XP FS:
// * structure, and the EmuX FS: structure. Before running Windows
// * code, you *must* swap over to Win2k/XP FS. Similarly, before
// * running Xbox code, you *must* swap back over to EmuX FS.
// *
// ******************************************************************
inline void EmuXSwapFS()
{
    __asm
    {
        mov ax, fs:[0x14]   // FS.ArbitraryUserPointer
        mov fs, ax
    }
}

// ******************************************************************
// * func: EmuXGenerateFS
// ******************************************************************
void EmuXGenerateFS()
{
    NT_TIB         *OrgNtTib;
    xboxkrnl::KPCR *NewPcr;

    uint16 NewFS=0;
    uint16 OrgFS=0;

    uint32 dwSize = sizeof(xboxkrnl::KPCR);

    NewPcr = (xboxkrnl::KPCR*)new char[dwSize];

    NewFS = LDTAllocate((uint32)NewPcr, (uint32)NewPcr + dwSize);

    // ******************************************************************
    // * Obtain "OrgFS"
    // ******************************************************************
    __asm
    {
        // Obtain "OrgFS"
        mov ax, fs
        mov OrgFS, ax

        // Obtain "OrgNtTib"
        mov eax, fs:[0x18]
        mov OrgNtTib, eax

        // Save "NewFS" inside OrgFS.ArbitraryUserPointer
        mov ax, NewFS
        mov fs:[0x14], ax
    }

    // ******************************************************************
    // * Generate TIB
    // ******************************************************************
    {
        void *TLSPtr = 0;

        xboxkrnl::KTHREAD *KThread = new xboxkrnl::KTHREAD();

        memcpy(&NewPcr->NtTib, OrgNtTib, sizeof(NT_TIB));

        NewPcr->NtTib.Self = &NewPcr->NtTib;
        NewPcr->PrcbData.CurrentThread = KThread;

        // Retrieve Win2k/XP TEB.ThreadLocalStoragePointer
        __asm
        {
            mov eax, fs:[0x2C]
            mov TLSPtr, eax
        }

        KThread->TlsData = (void*)TLSPtr;
    }

    // ******************************************************************
    // * Swap into the "NewFS"
    // ******************************************************************
    EmuXSwapFS();

    // ******************************************************************
    // * Save "OrgFS" inside NewFS.ArbitraryUserPointer
    // ******************************************************************
    __asm
    {
        mov ax, OrgFS
        mov fs:[0x14], ax   // NewFS.ArbitraryUserPointer
    }

    // ******************************************************************
    // * Swap back into the "OrgFS"
    // ******************************************************************
    EmuXSwapFS();
}

// ******************************************************************
// * func: EmuXInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXInit(DebugMode DebugConsole, char *DebugFilename, uint08 *XBEHeader, uint32 XBEHeaderSize, void (*Entry)())
{
    // ******************************************************************
    // * debug console allocation (if configured)
    // ******************************************************************
    if(DebugConsole == DM_CONSOLE)
    {
        if(AllocConsole())
        {
            freopen("CONOUT$", "wt", stdout);

            printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
        }
    }
    else if(DebugConsole == DM_FILE)
    {
        FreeConsole();

        freopen(DebugFilename, "wt", stdout);

        printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("CxbxKrnl [0x%.08X]: EmuXInit\n"
               "          (\n"
               "             DebugConsole        : 0x%.08X\n"
               "             DebugFilename       : \"%s\"\n"
               "             XBEHeader           : 0x%.08X\n"
               "             XBEHeaderSize       : 0x%.08X\n"
               "             Entry               : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), DebugConsole, DebugFilename, XBEHeader, XBEHeaderSize, Entry);
    }

    // ******************************************************************
    // * Load the necessary pieces of XBEHeader
    // ******************************************************************
    {
        uint32 old_protection = 0;

        VirtualProtect((void*)0x00010000, 0x1000, PAGE_READWRITE, &old_protection);

        uint32 dwSizeofHeaders   = *(uint32*)&XBEHeader[0x0108];
        uint32 dwCertificateAddr = *(uint32*)&XBEHeader[0x0118];

        *(uint32 *)0x00010108 = dwSizeofHeaders;
        *(uint32 *)0x00010118 = dwCertificateAddr;

        memcpy((void*)dwCertificateAddr, &XBEHeader[dwCertificateAddr - 0x00010000], sizeof(Xbe::Certificate));
    }

    // ******************************************************************
    // * Initialize LDT system
    // ******************************************************************
    {
        LDTSystemInit();
    }

    // ******************************************************************
    // * Initialize FS:* structure
    // ******************************************************************
    {
        EmuXGenerateFS();
    }

    printf("CxbxKrnl [0x%.08X]: Initial thread starting.\n", GetCurrentThreadId());

    EmuXSwapFS();   // XBox FS

    // This must be enabled or the debugger may crash (sigh)
    _asm _emit 0xF1

    Entry();

    EmuXSwapFS();   // Win2k/XP FS

    printf("CxbxKrnl [0x%.08X]: Initial thread ended.\n", GetCurrentThreadId());

    fflush(stdout);

    return;
}

// ******************************************************************
// * func: EmuXDummy
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXDummy()
{
    EmuXSwapFS();   // Win2k/XP FS

    MessageBox(NULL, "EmuXDummy()", "CxbxKrnl", MB_OK);

    EmuXSwapFS();   // XBox FS
}

// ******************************************************************
// * func: EmuXPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXPanic()
{
    EmuXSwapFS();   // Win2k/XP FS

    printf("CxbxKrnl [0x%.08X]: EmuXPanic()\n", GetCurrentThreadId());

    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);

    EmuXSwapFS();   // XBox FS
}

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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: PsCreateSystemThreadExProxy\n"
               "          (\n"
               "             StartContext1       : 0x%.08X\n"
               "             StartContext2       : 0x%.08X\n"
               "             StartRoutine        : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), StartContext1, StartContext2, StartRoutine);
    }
    #endif

    EmuXSwapFS();   // XBox FS

    __asm
    {
        mov         esi, StartRoutine
        push        StartContext1
        push        StartContext2
        lea         ebp, [esp-4]
        jmp near    esi
    }

    return 0;
}
#pragma warning(pop)

using namespace xboxkrnl;

// ******************************************************************
// * 0x0018 ExQueryNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(24) NTSTATUS NTAPI xboxkrnl::ExQueryNonVolatileSetting
(
	IN  NVRAM_SETTING_CLASS ValueIndex,
	OUT PNVRAM_TYPE_CLASS   Type,
	OUT PUCHAR              Value,
	IN  SIZE_T              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
)
{
    EmuXSwapFS();   // Win2k/XP FS

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: ExQueryNonVolatileSetting\n"
               "          (\n"
               "             ValueIndex          : 0x%.08X\n"
               "             Type                : 0x%.08X\n"
               "             Value               : 0x%.08X\n"
               "             ValueLength         : 0x%.08X\n"
               "             ResultLength        : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), ValueIndex, Type, Value, ValueLength, ResultLength);
    }
    #endif

    EmuXSwapFS();   // XBox FS

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

    MessageBox(NULL, "HalReturnToFirmware()", "CxbxKrnl", MB_OK);
    /*
    ReturnFirmwareHalt          = 0x0,
    ReturnFirmwareReboot        = 0x1,
    ReturnFirmwareQuickReboot   = 0x2,
    ReturnFirmwareHard          = 0x3,
    ReturnFirmwareFatal         = 0x4,
    ReturnFirmwareAll           = 0x5
    */

    EmuXSwapFS();   // XBox FS

    exit(1);
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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: KeInitializeDpc\n"
               "          (\n"
               "             Dpc                 : 0x%.08X\n"
               "             DeferredRoutine     : 0x%.08X\n"
               "             DeferredContext     : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), Dpc, DeferredRoutine, DeferredContext);
    }
    #endif

    EmuXSwapFS();   // XBox FS

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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: KeInitializeTimerEx\n"
               "          (\n"
               "             Timer               : 0x%.08X\n"
               "             Type                : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), Timer, Type);
    }
    #endif

    EmuXSwapFS();   // XBox FS

    return;
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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: NtClose\n"
               "          (\n"
               "             Handle              : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), Handle);
    }
    #endif

    if(CloseHandle(Handle) != TRUE)
        return STATUS_UNSUCCESSFUL;

    EmuXSwapFS();   // XBox FS

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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: PsCreateSystemThreadEx\n"
               "          (\n"
               "             ThreadHandle        : 0x%.08X\n"
               "             ThreadExtraSize     : 0x%.08X\n"
               "             KernelStackSize     : 0x%.08X\n"
               "             TlsDataSize         : 0x%.08X\n"
               "             ThreadId            : 0x%.08X\n"
               "             StartContext1       : 0x%.08X\n"
               "             StartContext2       : 0x%.08X\n"
               "             CreateSuspended     : 0x%.08X\n"
               "             DebugStack          : 0x%.08X\n"
               "             StartRoutine        : 0x%.08X\n"
               "          );\n",
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

    EmuXSwapFS();   // XBox FS

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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: RtlEnterCriticalSection\n"
               "          (\n"
               "             CriticalSection     : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    // We have to initialize this because the xbox software doesn't seem
    // to always do it. Redundant initializations seem to be ok :/
    InitializeCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EnterCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuXSwapFS();   // XBox FS
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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: RtlLeaveCriticalSection\n"
               "          (\n"
               "             CriticalSection     : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), CriticalSection);
    }
    #endif

    LeaveCriticalSection((win32::PRTL_CRITICAL_SECTION)CriticalSection);

    EmuXSwapFS();   // XBox FS
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
    #ifdef _DEBUG
    {
        printf("CxbxKrnl [0x%.08X]: RtlNtStatusToDosError\n"
               "          (\n"
               "             Status              : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), Status);
    }
    #endif

    EmuXSwapFS();   // XBox FS

    return 0;
}