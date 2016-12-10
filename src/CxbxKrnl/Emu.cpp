// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Emu.cpp
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

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "CxbxKrnl.h"
#define COMPILE_MULTIMON_STUBS
#include "Emu.h"
#include "EmuX86.h"
#include "EmuFS.h"
#include "EmuAlloc.h"

// prevent name collisions
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"
#include "EmuShared.h"
#include "HLEDataBase.h"
#include "HLEIntercept.h"

#ifdef _DEBUG
#include <Dbghelp.h>
CRITICAL_SECTION dbgCritical;
#endif

// Global Variable(s)
HANDLE           g_hCurDir    = NULL;
CHAR            *g_strCurDrive= NULL;
HANDLE           g_hTDrive    = NULL;
CHAR            *g_strTDrive  = NULL;
HANDLE           g_hUDrive    = NULL;
CHAR            *g_strUDrive  = NULL;
HANDLE           g_hZDrive    = NULL;
CHAR            *g_strZDrive  = NULL;
volatile bool    g_bEmuException = false;
volatile bool    g_bEmuSuspended = false;
volatile bool    g_bPrintfOn = true;

// Delta added to host SystemTime, used in xboxkrnl::KeQuerySystemTime and xboxkrnl::NtSetSystemTime
LARGE_INTEGER	HostSystemTimeDelta = {};

// Static Function(s)
static int ExitException(LPEXCEPTION_POINTERS e);

// Dll entry point, exit point, ...
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    static HINSTANCE hInitInstance = NULL;

    if(fdwReason == DLL_PROCESS_ATTACH)
    {
#ifdef _DEBUG
        InitializeCriticalSection(&dbgCritical);
#endif

        EmuShared::Init();
        hInitInstance = hinstDLL;
    }

    if(fdwReason == DLL_PROCESS_DETACH)
    {
        if(hInitInstance == hinstDLL)
            EmuShared::Cleanup();

#ifdef _DEBUG
        DeleteCriticalSection(&dbgCritical);
#endif
    }

    return TRUE;
}

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...)
{
    if(szWarningMessage == NULL)
        return;

    char szBuffer1[1024];
    char szBuffer2[1024];

    va_list argp;

    sprintf(szBuffer1, "EmuWarn (0x%X): ", GetCurrentThreadId());

    va_start(argp, szWarningMessage);

    vsprintf(szBuffer2, szWarningMessage, argp);

    va_end(argp);

    strcat(szBuffer1, szBuffer2);

    if(g_bPrintfOn)
    {
        printf("%s\n", szBuffer1);
    }

    fflush(stdout);

    return;
}
#endif

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e)
{
    g_bEmuException = true;

    // notify user
    {
        char buffer[256];

        if(e->ExceptionRecord->ExceptionCode == 0x80000003)
        {
			// print debug information
			{
				printf("EmuMain (0x%X): Recieved Breakpoint Exception (int 3)\n", GetCurrentThreadId());

				printf("\n"
					" EIP := 0x%.08X EFL := 0x%.08X\n"
					" EAX := 0x%.08X EBX := 0x%.08X ECX := 0x%.08X EDX := 0x%.08X\n"
					" ESI := 0x%.08X EDI := 0x%.08X ESP := 0x%.08X EBP := 0x%.08X\n"
					" CR2 := 0x%.08X\n"
					"\n",
					e->ContextRecord->Eip, e->ContextRecord->EFlags,
					e->ContextRecord->Eax, e->ContextRecord->Ebx, e->ContextRecord->Ecx, e->ContextRecord->Edx,
					e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Esp, e->ContextRecord->Ebp,
					e->ContextRecord->Dr2);

#ifdef _DEBUG
				CONTEXT Context = *(e->ContextRecord);
				EmuPrintStackTrace(&Context);
#endif
			}

			fflush(stdout);

            sprintf(buffer,
                "Recieved Breakpoint Exception (int 3) @ EIP := 0x%.08X\n"
                "\n"
                "  Press Abort to terminate emulation.\n"
                "  Press Retry to debug.\n"
                "  Press Ignore to continue emulation.",
                e->ContextRecord->Eip);

            e->ContextRecord->Eip += 1;

            int ret = MessageBox(g_hEmuWindow, buffer, "Cxbx-Reloaded", MB_ICONSTOP | MB_ABORTRETRYIGNORE);

            if(ret == IDABORT)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(CxbxKrnl_hEmuParent != NULL)
                    SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
            else if(ret == IDIGNORE)
            {
                printf("EmuMain (0x%X): Ignored Breakpoint Exception\n", GetCurrentThreadId());

                g_bEmuException = false;

                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        else
        {
			// Pass the exception to our X86 implementation, to try and execute the failing instruction
			if (EmuX86_DecodeException(e))
			{
				g_bEmuException = false;
				return EXCEPTION_CONTINUE_EXECUTION;
			}


			// print debug information
			{
				printf("EmuMain (0x%X): Recieved Exception (Code := 0x%.08X)\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode);

				printf("\n"
					" EIP := 0x%.08X EFL := 0x%.08X\n"
					" EAX := 0x%.08X EBX := 0x%.08X ECX := 0x%.08X EDX := 0x%.08X\n"
					" ESI := 0x%.08X EDI := 0x%.08X ESP := 0x%.08X EBP := 0x%.08X\n"
					" CR2 := 0x%.08X\n"
					"\n",
					e->ContextRecord->Eip, e->ContextRecord->EFlags,
					e->ContextRecord->Eax, e->ContextRecord->Ebx, e->ContextRecord->Ecx, e->ContextRecord->Edx,
					e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Esp, e->ContextRecord->Ebp,
					e->ContextRecord->Dr2);

#ifdef _DEBUG
				CONTEXT Context = *(e->ContextRecord);
				EmuPrintStackTrace(&Context);
#endif
			}

			fflush(stdout);

            sprintf(buffer,
                "Recieved Exception Code 0x%.08X @ EIP := 0x%.08X\n"
                "\n"
                "  Press \"OK\" to terminate emulation.\n"
                "  Press \"Cancel\" to debug.",
                e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);

            if(MessageBox(g_hEmuWindow, buffer, "Cxbx-Reloaded", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
            {
                printf("EmuMain (0x%X): Aborting Emulation\n", GetCurrentThreadId());
                fflush(stdout);

                if(CxbxKrnl_hEmuParent != NULL)
                    SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

                ExitProcess(1);
            }
        }
    }

    g_bEmuException = false;

    return EXCEPTION_CONTINUE_SEARCH;
}

// check how many bytes were allocated for a structure
extern int EmuCheckAllocationSize(LPVOID pBase, bool largeBound)
{
    MEMORY_BASIC_INFORMATION MemoryBasicInfo;

    DWORD dwRet;
#ifdef _DEBUG_ALLOC
    dwRet = CxbxVirtualQueryDebug(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));
    if (dwRet == -1)
#endif
    dwRet = VirtualQuery(pBase, &MemoryBasicInfo, sizeof(MemoryBasicInfo));

    if(dwRet == 0)
        return 0;

    if(MemoryBasicInfo.State != MEM_COMMIT)
        return 0;

    // this is a hack in order to determine when pointers come from a large write-combined database
    if(largeBound && MemoryBasicInfo.RegionSize > 5*1024*1024)
        return -1;

    return MemoryBasicInfo.RegionSize - ((DWORD)pBase - (DWORD)MemoryBasicInfo.BaseAddress);
}

// exception handle for that tough final exit :)
int ExitException(LPEXCEPTION_POINTERS e)
{
    static int count = 0;

    // debug information
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
    printf("EmuMain (0x%X): Recieved Exception [0x%.08X]@0x%.08X\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip);
    printf("EmuMain (0x%X): * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());

    fflush(stdout);

    MessageBox(g_hEmuWindow, "Warning: Could not safely terminate process!", "Cxbx-Reloaded", MB_OK);

    count++;

    if(count > 1)
    {
        MessageBox(g_hEmuWindow, "Warning: Multiple Problems!", "Cxbx-Reloaded", MB_OK);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if(CxbxKrnl_hEmuParent != NULL)
        SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

    ExitProcess(1);

    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef _DEBUG
// print call stack trace
void EmuPrintStackTrace(PCONTEXT ContextRecord)
{
    static int const STACK_MAX     = 16;
    static int const SYMBOL_MAXLEN = 64;

    EnterCriticalSection(&dbgCritical);

    IMAGEHLP_MODULE64 module = { sizeof(IMAGEHLP_MODULE) };

    BOOL fSymInitialized;
    fSymInitialized = SymInitialize(GetCurrentProcess(), NULL, TRUE);

    STACKFRAME64 frame = { sizeof(STACKFRAME64) };
    frame.AddrPC.Offset    = ContextRecord->Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = ContextRecord->Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = ContextRecord->Esp;
    frame.AddrStack.Mode   = AddrModeFlat;

    for(int i = 0; i < STACK_MAX; i++)
    {
        if(!StackWalk64(
            IMAGE_FILE_MACHINE_I386,
            GetCurrentProcess(),
            GetCurrentThread(),
            &frame,
            ContextRecord,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
            break;

        DWORD64 dwDisplacement = 0;
        PSYMBOL_INFO pSymbol = 0;
        BYTE symbol[sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN];

        SymGetModuleInfo64(GetCurrentProcess(), frame.AddrPC.Offset, &module);

        if(fSymInitialized)
        {
            pSymbol = (PSYMBOL_INFO)symbol;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN - 1;
            pSymbol->MaxNameLen = SYMBOL_MAXLEN;

            if(!SymFromAddr(GetCurrentProcess(), frame.AddrPC.Offset, &dwDisplacement, pSymbol))
                pSymbol = 0;
        }

        if(module.ModuleName)
            printf(" %2d: %-8s 0x%.08X", i, module.ModuleName, frame.AddrPC.Offset);
        else
            printf(" %2d: %8c 0x%.08X", i, ' ', frame.AddrPC.Offset);

        if(pSymbol)
        {
            printf(" %s+0x%.04X\n", pSymbol->Name, dwDisplacement);
        }
        else
            printf("\n");
    }
    printf("\n");

    if(fSymInitialized)
        SymCleanup(GetCurrentProcess());

    LeaveCriticalSection(&dbgCritical);
}
#endif
