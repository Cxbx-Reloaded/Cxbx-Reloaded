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
// * func: EmuXGenerateFS
// ******************************************************************
void EmuXGenerateFS()
{
    uint16 OrgFS = 0;

    uint32 dwSize = sizeof(EMUX_KPCR);

    uint32 pNewFS = (uint32)new char[dwSize];

    uint16 NewFS = LDTAllocate(pNewFS, pNewFS + dwSize);

    // ******************************************************************
    // * Save the "old" FS  : [OrgFS = FS]
    // ******************************************************************
    __asm
    {
        mov ax, fs
        mov OrgFS, ax
    }

    // ******************************************************************
    // * Update "new" FS    : [FS = NewFS, FS:[0x025C] = OrgFS]
    // ******************************************************************
    __asm
    {
        mov ax, NewFS
        mov fs, ax

        mov ax, OrgFS
        mov fs:[0x025C], ax
    }
}

// ******************************************************************
// * func: EmuXInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXInit(uint32 DebugConsole, uint08 *XBEHeader, uint32 XBEHeaderSize, void (*Entry)())
{
    // ******************************************************************
    // * debug console allocation (if configured)
    // ******************************************************************
    if(DebugConsole)
    {
        AllocConsole();

        freopen("CONOUT$", "wt", stdout);

        printf("CxbxKrnl [0x%.08X]: Debug console allocated.\n", GetCurrentThreadId());
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("CxbxKrnl [0x%.08X]: EmuXInit\n"
               "          (\n"
               "             DebugConsole        : 0x%.08X\n"
               "             XBEHeader           : 0x%.08X\n"
               "             XBEHeaderSize       : 0x%.08X\n"
               "             Entry               : 0x%.08X\n"
               "          );\n",
               GetCurrentThreadId(), DebugConsole, XBEHeader, XBEHeaderSize, Entry);
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

        // ******************************************************************
        // * Restore "old" FS   : [FS = FS:[0x025C]]
        // ******************************************************************
        __asm
        {
            mov     ax, fs:[0x025C]
            mov     fs, ax
        }

        NT_TIB *dbgTIB = 0;

        __asm
        {
            mov esi, fs:[18h]
            mov dbgTIB, esi
        }

        // TODO: Allocate new FS: within LDT, copy OriginalTIB to KPCR.NtTib, load new FS
        printf("CxbxKrnl [0x%.08X]: NT_TIB.Self=0x%.08X\n", GetCurrentThreadId(), dbgTIB->Self);
    }

    printf("CxbxKrnl [0x%.08X]: Initial thread starting.\n", GetCurrentThreadId());

    Entry();

    printf("CxbxKrnl [0x%.08X]: Initial thread ended.\n", GetCurrentThreadId());

    // just spin forever (for now...)
    while(true)
        Sleep(1000);

    return;
}

// ******************************************************************
// * func: EmuXDummy
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXDummy()
{
    MessageBox(NULL, "EmuXDummy()", "CxbxKrnl", MB_OK);
}

// ******************************************************************
// * func: EmuXPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXPanic()
{
    printf("CxbxKrnl [0x%.08X]: EmuXPanic()\n", GetCurrentThreadId());

    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);

    exit(1);
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

    __asm
    {
        mov         esi, StartRoutine
        push        StartContext2
        push        StartContext1
        lea         ebp, [esp-4]
        int 3
        jmp near    esi
    }

    return 0;
}
#pragma warning(pop)

using namespace xboxkrnl;

// ******************************************************************
// * 0x00BB - NtClose
// ******************************************************************
XBSYSAPI EXPORTNUM(187) NTSTATUS NTAPI xboxkrnl::NtClose
(
	IN HANDLE Handle
)
{
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
    MessageBox(NULL, "HalReturnToFirmware()", "CxbxKrnl", MB_OK);
    /*
    ReturnFirmwareHalt          = 0x0,
    ReturnFirmwareReboot        = 0x1,
    ReturnFirmwareQuickReboot   = 0x2,
    ReturnFirmwareHard          = 0x3,
    ReturnFirmwareFatal         = 0x4,
    ReturnFirmwareAll           = 0x5
    */

    exit(1);
}