// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
volatile thread_local  bool    g_bEmuException = false;
volatile bool    g_bEmuSuspended = false;
volatile bool    g_bPrintfOn = true;
bool g_XInputEnabled = false;

// Delta added to host SystemTime, used in xboxkrnl::KeQuerySystemTime and xboxkrnl::NtSetSystemTime
LARGE_INTEGER	HostSystemTimeDelta = {};

// Static Function(s)
static int ExitException(LPEXCEPTION_POINTERS e);

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
void NTAPI EmuWarning(const char *szWarningMessage, ...)
{
    if(szWarningMessage == NULL)
        return;

    char szBuffer1[1024];
    char szBuffer2[1024];

    va_list argp;

    sprintf(szBuffer1, "[0x%X] EmuWarn: ", GetCurrentThreadId());

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

std::string EIPToString(xbaddr EIP)
{
	int symbolOffset = 0;
	std::string symbolName = GetDetectedSymbolName(EIP, &symbolOffset);

	char buffer[256];
	sprintf(buffer, "0x%.08X(=%s+0x%x)", EIP, symbolName.c_str(), symbolOffset);

	std::string result = buffer;
	return result;
}

void EmuExceptionPrintDebugInformation(LPEXCEPTION_POINTERS e, bool IsBreakpointException)
{
	// print debug information
	{
		if (IsBreakpointException)
			printf("[0x%X] EmuMain: Received Breakpoint Exception (int 3)\n", GetCurrentThreadId());
		else
			printf("[0x%X] EmuMain: Received Exception (Code := 0x%.08X)\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode);

		printf("\n"
			" EIP := %s\n"
			" EFL := 0x%.08X\n"
			" EAX := 0x%.08X EBX := 0x%.08X ECX := 0x%.08X EDX := 0x%.08X\n"
			" ESI := 0x%.08X EDI := 0x%.08X ESP := 0x%.08X EBP := 0x%.08X\n"
			" CR2 := 0x%.08X\n"
			"\n",
			EIPToString(e->ContextRecord->Eip).c_str(),
			e->ContextRecord->EFlags,
			e->ContextRecord->Eax, e->ContextRecord->Ebx, e->ContextRecord->Ecx, e->ContextRecord->Edx,
			e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Esp, e->ContextRecord->Ebp,
			e->ContextRecord->Dr2);

#ifdef _DEBUG
		CONTEXT Context = *(e->ContextRecord);
		EmuPrintStackTrace(&Context);
#endif
	}

	fflush(stdout);
}

void EmuExceptionExitProcess()
{
	printf("[0x%X] EmuMain: Aborting Emulation\n", GetCurrentThreadId());
	fflush(stdout);

	if (CxbxKrnl_hEmuParent != NULL)
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

	EmuShared::Cleanup();
	ExitProcess(1);
}

bool EmuExceptionBreakpointAsk(LPEXCEPTION_POINTERS e)
{
	EmuExceptionPrintDebugInformation(e, /*IsBreakpointException=*/true);

	char buffer[256];
	sprintf(buffer,
		"Received Breakpoint Exception (int 3) @ EIP := %s\n"
		"\n"
		"  Press Abort to terminate emulation.\n"
		"  Press Retry to debug.\n"
		"  Press Ignore to continue emulation.",
		EIPToString(e->ContextRecord->Eip).c_str());

	int ret = MessageBox(g_hEmuWindow, buffer, "Cxbx-Reloaded", MB_ICONSTOP | MB_ABORTRETRYIGNORE);
	if (ret == IDABORT)
	{
		EmuExceptionExitProcess();
	}
	else if (ret == IDIGNORE)
	{
		printf("[0x%X] EmuMain: Ignored Breakpoint Exception\n", GetCurrentThreadId());
		fflush(stdout);

		e->ContextRecord->Eip += EmuX86_OpcodeSize((uint8_t*)e->ContextRecord->Eip); // Skip instruction size bytes

		return true;
	}

	return false;
}

void EmuExceptionNonBreakpointUnhandledShow(LPEXCEPTION_POINTERS e)
{
	EmuExceptionPrintDebugInformation(e, /*IsBreakpointException=*/false);

	char buffer[256];
	sprintf(buffer,
		"Received Exception Code 0x%.08X @ EIP := %s\n"
		"\n"
		"  Press \"OK\" to terminate emulation.\n"
		"  Press \"Cancel\" to debug.",
		e->ExceptionRecord->ExceptionCode, EIPToString(e->ContextRecord->Eip).c_str());

	if (MessageBox(g_hEmuWindow, buffer, "Cxbx-Reloaded", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
	{
		EmuExceptionExitProcess();
	}
}

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e)
{
    g_bEmuException = true;
	if (e->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT)
	{
		// notify user
		if (EmuExceptionBreakpointAsk(e))
		{
			// We're allowed to continue :
			g_bEmuException = false;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	else
	{
		// Pass the exception to our X86 implementation, to try and execute the failing instruction
		if (EmuX86_DecodeException(e))
		{
			// We're allowed to continue :
			g_bEmuException = false;
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		// notify user
		EmuExceptionNonBreakpointUnhandledShow(e);
	}

	// Unhandled exception :
	g_bEmuException = false;
    return EXCEPTION_CONTINUE_SEARCH;
}

// exception handle for that tough final exit :)
int ExitException(LPEXCEPTION_POINTERS e)
{
    static int count = 0;

	// debug information
    printf("[0x%X] EmuMain: * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
    printf("[0x%X] EmuMain: Received Exception [0x%.08X]@%s\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, EIPToString(e->ContextRecord->Eip).c_str());
    printf("[0x%X] EmuMain: * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());

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

	// TODO: Figure out why this causes a loop of Exceptions until the process dies
    //EnterCriticalSection(&dbgCritical);

    IMAGEHLP_MODULE64 module = { sizeof(IMAGEHLP_MODULE) };

    BOOL fSymInitialized = SymInitialize(g_CurrentProcessHandle, NULL, TRUE);

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
			g_CurrentProcessHandle,
            GetCurrentThread(),
            &frame,
            ContextRecord,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL))
            break;

        SymGetModuleInfo64(g_CurrentProcessHandle, frame.AddrPC.Offset, &module);
        if(module.ModuleName)
            printf(" %2d: %-8s 0x%.08X", i, module.ModuleName, frame.AddrPC.Offset);
        else
            printf(" %2d: %8c 0x%.08X", i, ' ', frame.AddrPC.Offset);

		BYTE symbol[sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN] = { 0 };
		std::string symbolName = "";
        DWORD64 dwDisplacement = 0;

        if(fSymInitialized)
        {
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)&symbol;
            pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN - 1;
            pSymbol->MaxNameLen = SYMBOL_MAXLEN;
			if (SymFromAddr(g_CurrentProcessHandle, frame.AddrPC.Offset, &dwDisplacement, pSymbol))
				symbolName = pSymbol->Name;
			else
			{
				// Try getting a symbol name from the HLE cache :
				int symbolOffset = 0;

				symbolName = GetDetectedSymbolName((xbaddr)frame.AddrPC.Offset, &symbolOffset);

				if (symbolOffset < 1000)
					dwDisplacement = (DWORD64)symbolOffset;
				else
					symbolName = "";
			}
        }

        if(symbolName.length() > 0)
            printf(" %s+0x%.04X\n", symbolName.c_str(), dwDisplacement);
        else
            printf("\n");
    }

    printf("\n");

    if(fSymInitialized)
        SymCleanup(g_CurrentProcessHandle);

    // LeaveCriticalSection(&dbgCritical);
}
#endif
