// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Kernel.cpp
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
#include "Kernel.h"

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

        printf("%s", "CxbxKrnl: Debug console allocated.\n");
    }

    // ******************************************************************
    // * debug trace
    // ******************************************************************
    {
        printf("CxbxKrnl: EmuXInit\n"
               "          (\n"
               "             DebugConsole        : 0x%.08X\n"
               "             XBEHeader           : 0x%.08X\n"
               "             XBEHeaderSize       : 0x%.08X\n"
               "             Entry               : 0x%.08X\n"
               "          );\n",
               DebugConsole, XBEHeader, XBEHeaderSize, Entry);
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

    printf("CxbxKrnl: Execution starting.\n");

    Entry();

    printf("CxbxKrnl: Execution ended.\n");

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
    printf("CxbxKrnl: EmuXPanic()\n");

    MessageBox(NULL, "Kernel Panic! Process will now terminate.", "CxbxKrnl", MB_OK | MB_ICONEXCLAMATION);

    exit(1);
}

using namespace xboxkrnl;

// ******************************************************************
// * 0x00BB - NtClose
// ******************************************************************
XBSYSAPI EXPORTNUM(187) NTSTATUS NTAPI xboxkrnl::NtClose
(
	IN HANDLE Handle
)
{
    MessageBox(NULL, "NtClose()", "CxbxKrnl", MB_OK);
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
        printf("CxbxKrnl: PsCreateSystemThreadEx\n"
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
               ThreadHandle, ThreadExtraSize, KernelStackSize, TlsDataSize, ThreadId,
               StartContext1, StartContext2, CreateSuspended, DebugStack, StartRoutine);
    }
    #endif

    EmuXPanic();

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