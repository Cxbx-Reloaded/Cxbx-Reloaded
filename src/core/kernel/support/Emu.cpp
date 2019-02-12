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
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
    #include <xboxkrnl\xboxkrnl.h>
};

#include "core\kernel\init\CxbxKrnl.h"
#include "Emu.h"
#include "devices\x86\EmuX86.h"
#include "EmuFS.h"
#include "EmuXTL.h"
#include "EmuShared.h"
#include "core\hle\Intercept.hpp"
#include "CxbxDebugger.h"

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
bool g_DisablePixelShaders = false;
bool g_UncapFramerate = false;
bool g_UseAllCores = false;
bool g_SkipRdtscPatching = false;
bool g_ScaleViewport = false;
bool g_DirectHostBackBufferAccess = false;

const char log_debug[] = "DEBUG: ";
const char log_info[]  = "INFO : ";
const char log_warn[]  = "WARN : ";
const char log_fatal[] = "FATAL: ";
const char log_unkwn[] = "???? : ";

// Delta added to host SystemTime, used in KiClockIsr and KeSetSystemTime
// This shouldn't need to be atomic, but because raising the IRQL to high lv in KeSetSystemTime doesn't really stop KiClockIsr from running,
// we need it for now to prevent reading a corrupted value while KeSetSystemTime is in the middle of updating it
std::atomic_int64_t HostSystemTimeDelta(0);

// Static Function(s)
static int ExitException(LPEXCEPTION_POINTERS e);

std::string FormatTitleId(uint32_t title_id)
{
	std::stringstream ss;
	
	// If the Title ID prefix is a printable character, parse it
	// This shows the correct game serial number for retail titles!
	// EG: MS-001 for 1st tile published by MS, EA-002 for 2nd title by EA, etc
	// Some special Xbes (Dashboard, XDK Samples) use non-alphanumeric serials
	// We fall back to Hex for those
	// ergo720: we cannot use isalnum() here because it will treat chars in the range -1 - 255 as valid ascii chars which can
	// lead to unicode characters being printed in the title (e.g.: dashboard uses 0xFE and 0xFF)
	uint8_t pTitleId1 = (title_id >> 24) & 0xFF;
	uint8_t pTitleId2 = (title_id >> 16) & 0xFF;

	if ((pTitleId1 < 65 || pTitleId1 > 90) || (pTitleId2 < 65 || pTitleId2 > 90)) {
		// Prefix was non-printable, so we need to print a hex reprentation of the entire title_id
		ss << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << title_id;
		return ss.str();
	}	

	ss << pTitleId1 << pTitleId2;
	ss << "-";
	ss << std::setfill('0') << std::setw(3) << std::dec << (title_id & 0x0000FFFF);

	return ss.str();
}

// print out a warning message to the kernel debug log file
void NTAPI EmuLogEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, const char *szWarningMessage, ...)
{
	if (szWarningMessage == NULL) {
		return;
	}

	LOG_CHECK_ENABLED_EX(cxbxr_module, level) {
		if (g_bPrintfOn) {

			va_list argp;
			const char* level_str;

			LOG_THREAD_INIT;

			switch (level) {
				default:
					level_str = log_unkwn;
					break;
				case LOG_LEVEL::DEBUG:
					level_str = log_debug;
					break;
				case LOG_LEVEL::INFO:
					level_str = log_info;
					break;
				case LOG_LEVEL::WARNING:
					level_str = log_warn;
					break;
				case LOG_LEVEL::FATAL:
					level_str = log_fatal;
					break;
			}

			std::cout << _logThreadPrefix << level_str
				<< g_EnumModules2String[to_underlying(cxbxr_module)];

			va_start(argp, szWarningMessage);

			vfprintf(stdout, szWarningMessage, argp);

			va_end(argp);

			fprintf(stdout, "\n");

			fflush(stdout);

		}
	}
}

std::string EIPToString(xbaddr EIP)
{
	char buffer[256];
	
	if (EIP < g_SystemMaxMemory) {
		int symbolOffset = 0;
		std::string symbolName = GetDetectedSymbolName(EIP, &symbolOffset);
		sprintf(buffer, "0x%.08X(=%s+0x%x)", EIP, symbolName.c_str(), symbolOffset);
	} else {
		sprintf(buffer, "0x%.08X", EIP);
	}
	
	std::string result = buffer;

	return result;
}

void EmuExceptionPrintDebugInformation(LPEXCEPTION_POINTERS e, bool IsBreakpointException)
{
	// print debug information
	{
		if (IsBreakpointException)
			printf("[0x%.4X] MAIN: Received Breakpoint Exception (int 3)\n", GetCurrentThreadId());
		else
			printf("[0x%.4X] MAIN: Received Exception (Code := 0x%.8X)\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode);

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
	printf("[0x%.4X] MAIN: Aborting Emulation\n", GetCurrentThreadId());
	fflush(stdout);

	if (CxbxKrnl_hEmuParent != NULL)
		SendMessage(CxbxKrnl_hEmuParent, WM_PARENTNOTIFY, WM_DESTROY, 0);

	EmuShared::Cleanup();
	ExitProcess(1);
}

bool EmuExceptionBreakpointAsk(LPEXCEPTION_POINTERS e)
{
	EmuExceptionPrintDebugInformation(e, /*IsBreakpointException=*/true);

	// We can skip Xbox as long as they are logged so we know about them
	// There's no need to prevent emulation, we can just pretend we have a debugger attached and continue
	// This is because some games (such as Crash Bandicoot) spam exceptions;
	e->ContextRecord->Eip += EmuX86_OpcodeSize((uint8_t*)e->ContextRecord->Eip); // Skip 1 size bytes
	return true;

#if 1
#else
	char buffer[256];
	sprintf(buffer,
		"Received Breakpoint Exception (int 3) @ EIP := %s\n"
		"\n"
		"  Press Abort to terminate emulation.\n"
		"  Press Retry to debug.\n"
		"  Press Ignore to continue emulation.",
		EIPToString(e->ContextRecord->Eip).c_str());

	int ret = CxbxMessageBox(buffer, MB_ICONSTOP | MB_ABORTRETRYIGNORE, g_hEmuWindow);
	if (ret == IDABORT)
	{
		EmuExceptionExitProcess();
	}
	else if (ret == IDIGNORE)
	{
		printf("[0x%.4X] MAIN: Ignored Breakpoint Exception\n", GetCurrentThreadId());
		fflush(stdout);

		e->ContextRecord->Eip += EmuX86_OpcodeSize((uint8_t*)e->ContextRecord->Eip); // Skip instruction size bytes

		return true;
	}

	return false;
#endif
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

	if (CxbxMessageBox(buffer, MB_ICONSTOP | MB_OKCANCEL, g_hEmuWindow) == IDOK)
	{
		EmuExceptionExitProcess();
	}
}

// Returns weither the given address is part of an Xbox managed memory region
bool IsXboxCodeAddress(xbaddr addr)
{
	// TODO : Replace the following with a (fast) check weither
	// the given address lies in xbox allocated virtual memory,
	// for example by g_VMManager.CheckConflictingVMA(addr, 0).
	return (addr >= XBE_IMAGE_BASE) && (addr <= XBE_MAX_VA);
	// Note : Not IS_USER_ADDRESS(), that would include host DLL code
}

void genericException(EXCEPTION_POINTERS *e) {
	// Try to report this exception to the debugger, which may allow handling of this exception
	if (CxbxDebugger::CanReport()) {
		bool DebuggerHandled = false;
		CxbxDebugger::ReportAndHandleException(e->ExceptionRecord, DebuggerHandled);
		if (!DebuggerHandled) {
			// Kill the process immediately without the Cxbx notifier
			EmuExceptionExitProcess();
		}

		// Bypass exception
	}
	else {
		// notify user
		EmuExceptionNonBreakpointUnhandledShow(e);
	}
}

static thread_local bool bOverrideException;

bool IsRdtscInstruction(xbaddr addr); // Implemented in CxbxKrnl.cpp
void EmuX86_Opcode_RDTSC(EXCEPTION_POINTERS *e); // Implemented in EmuX86.cpp
bool lleTryHandleException(EXCEPTION_POINTERS *e)
{
	// Initalize local thread variable
	bOverrideException = false;

	// Only handle exceptions which originate from Xbox code
	if (!IsXboxCodeAddress(e->ContextRecord->Eip)) {
		return false;
	}

	// Make sure access-violations reach EmuX86_DecodeException() as soon as possible
	if (e->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
		switch (e->ExceptionRecord->ExceptionCode) {
		case STATUS_PRIVILEGED_INSTRUCTION:
			// Check if this exception came from rdtsc 
			if (IsRdtscInstruction(e->ContextRecord->Eip)) {
				// If so, use a return value that updates with Xbox frequency;
				EmuX86_Opcode_RDTSC(e);
				e->ContextRecord->Eip += 2; // Skip OPCODE_PATCH_RDTSC 0xEF 0x90 (OUT DX, EAX; NOP)
				return true;
			}
			break;
		case STATUS_BREAKPOINT:
			// Pass breakpoint down to EmuException since VEH doesn't have call stack viewable.
			return false;
		default:
			// Skip past CxbxDebugger-specific exceptions thrown when an unsupported was attached (ie Visual Studio)
			if (CxbxDebugger::IsDebuggerException(e->ExceptionRecord->ExceptionCode)) {
				return true;
			}
		}
	}

	// Pass the exception to our X86 implementation, to try and execute the failing instruction
	if (EmuX86_DecodeException(e)) {
		// We're allowed to continue :
		return true;
	}

	genericException(e);

	// We do not need EmuException to handle it again.
	bOverrideException = true;

	// Unhandled exception :
	return false;
}

// Only for LLE emulation coding (to help performance a little bit better)
LONG NTAPI lleException(EXCEPTION_POINTERS *e)
{
	g_bEmuException = true;
	LONG result = lleTryHandleException(e) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
	g_bEmuException = false;
	return result;
}

// Only for Cxbx emulation coding (to catch all of last resort exception may occur.)
bool EmuTryHandleException(EXCEPTION_POINTERS *e)
{

	// Check if lle exception is already called first before emu exception.
	if (bOverrideException) {
		return false;
	}

	if (e->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
		bool isInt2Dh = *(uint16_t*)(e->ContextRecord->Eip - 2) == 0x2DCD;

		switch (e->ExceptionRecord->ExceptionCode) {
		case STATUS_BREAKPOINT:
			// First, check if the breakpoint was prefixed with int 0x2dh, if so, it's NOT a breakpoint, but a Debugger Command!
			// Note: Because of a Windows quirk, this does NOT work when a debugger is attached, we'll get an UNCATCHABLE breakpoint instead
			// But at least this gives us a working implementaton of Xbox DbgPrint when we don't attach a debugger
			if (isInt2Dh ){
				e->ContextRecord->Eip += 1;

				// Now perform the command (stored in EAX)
				switch (e->ContextRecord->Eax) {
					case 1: // DEBUG_PRINT
						// In this case, ECX should point to an ANSI String
						printf("DEBUG_PRINT: %s\n", ((xboxkrnl::PANSI_STRING)e->ContextRecord->Ecx)->Buffer);
						break;
					default:
						printf("Unhandled Debug Command: int 2Dh, EAX = %d", e->ContextRecord->Eip);
				}

				return true;
			}

			// Otherwise, let the user choose between continue or break
			return EmuExceptionBreakpointAsk(e);
		default:
			printf("Unhandled Debug Command: int 2Dh, EAX = %d", e->ContextRecord->Eip);
			// Skip past CxbxDebugger-specific exceptions thrown when an unsupported was attached (ie Visual Studio)
			if (CxbxDebugger::IsDebuggerException(e->ExceptionRecord->ExceptionCode)) {
				return true;
			}
		}
	}

	genericException(e);

	// Unhandled exception :
	return false;
}

int EmuException(EXCEPTION_POINTERS *e)
{
	g_bEmuException = true;
	LONG result = EmuTryHandleException(e) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
	g_bEmuException = false;
	return result;
}

// exception handle for that tough final exit :)
int ExitException(LPEXCEPTION_POINTERS e)
{
    static int count = 0;

	// debug information
    printf("[0x%.4X] MAIN: * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());
    printf("[0x%.4X] MAIN: Received Exception [0x%.8X]@%s\n", GetCurrentThreadId(), e->ExceptionRecord->ExceptionCode, EIPToString(e->ContextRecord->Eip).c_str());
    printf("[0x%.4X] MAIN: * * * * * EXCEPTION * * * * *\n", GetCurrentThreadId());

    fflush(stdout);

    (void)CxbxMessageBox("Warning: Could not safely terminate process!", MB_OK, g_hEmuWindow);

    count++;

    if(count > 1)
    {
        (void)CxbxMessageBox("Warning: Multiple Problems!", MB_OK, g_hEmuWindow);
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

		if (fSymInitialized)
		{
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)&symbol;
			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO) + SYMBOL_MAXLEN - 1;
			pSymbol->MaxNameLen = SYMBOL_MAXLEN;
			if (SymFromAddr(g_CurrentProcessHandle, frame.AddrPC.Offset, &dwDisplacement, pSymbol))
				symbolName = pSymbol->Name;
		}

		if (symbolName.empty()) {
			// Try getting a symbol name from the HLE cache :
			int symbolOffset = 0;

			symbolName = GetDetectedSymbolName((xbaddr)frame.AddrPC.Offset, &symbolOffset);
			if (symbolOffset < 1000)
				dwDisplacement = (DWORD64)symbolOffset;
			else
				symbolName = "";
        }

		if (!symbolName.empty())
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
