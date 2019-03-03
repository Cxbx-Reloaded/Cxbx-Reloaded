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
#ifndef EMU_H
#define EMU_H

#include "common\xbe\Xbe.h"
#include "Logging.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <multimon.h>

// print out a log message to the kernel debug log file if level is high enough
void NTAPI EmuLogEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, const char *szWarningMessage, ...);

#define EmuLog(level, fmt, ...) EmuLogEx(LOG_PREFIX, level, fmt, ##__VA_ARGS__)

std::string FormatTitleId(uint32_t title_id);

// exception handler
extern LONG NTAPI lleException(EXCEPTION_POINTERS *e);
int EmuException(EXCEPTION_POINTERS *e);

// print call stack trace
#ifdef _DEBUG
void EmuPrintStackTrace(PCONTEXT ContextRecord);
#endif

// global flags specifying current emulation state
extern volatile thread_local bool g_bEmuException;
extern volatile bool g_bEmuSuspended;

// global exception patching address
extern void * funcExclude[2048];

// partition emulation directory handles
extern HANDLE g_hCurDir;
extern CHAR  *g_strCurDrive;
extern HWND   g_hEmuWindow;

#define GET_FRONT_WINDOW_HANDLE ((CxbxKrnl_hEmuParent != nullptr) ? CxbxKrnl_hEmuParent : g_hEmuWindow)

// thread notification routine
extern PVOID g_pfnThreadNotification[16];
extern int g_iThreadNotificationCount;

extern DWORD_PTR g_CPUXbox;
extern DWORD_PTR g_CPUOthers;

extern HANDLE g_CurrentProcessHandle; // Set in CxbxKrnlMain

// Delta added to host SystemTime, used in KiClockIsr and KeSetSystemTime
extern std::atomic_int64_t HostSystemTimeDelta;

#if HLE_XINPUT
// NOTE: this is an arbitrary latency
#define XINPUT_SETSTATE_LATENCY 4
#define XINPUT_SETSTATE_SLOTS 16

// XInputSetState status waiters
extern struct XInputSetStateStatus
{
    HANDLE  hDevice;
    DWORD   dwLatency;
    PVOID   pFeedback;
}
g_pXInputSetStateStatus[XINPUT_SETSTATE_SLOTS];

// 4 controllers
#define XINPUT_HANDLE_SLOTS 4

extern HANDLE g_hInputHandle[XINPUT_HANDLE_SLOTS];
#endif

typedef struct DUMMY_KERNEL
{
	IMAGE_DOS_HEADER DosHeader;
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_SECTION_HEADER SectionHeader;
} *PDUMMY_KERNEL;


extern bool g_DisablePixelShaders;
extern bool g_UncapFramerate;
extern bool g_UseAllCores;
extern bool g_SkipRdtscPatching;
extern bool g_ScaleViewport;
extern bool g_DirectHostBackBufferAccess;
#endif
