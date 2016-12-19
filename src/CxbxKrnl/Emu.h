// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Emu.h
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

#include "Common/Xbe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <multimon.h>

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...);
#else
inline void NTAPI EmuWarning(const char *szWarningMessage, ...) { }
#endif

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e);

// check the allocation size of a given virtual address
extern int EmuCheckAllocationSize(LPVOID pBase, bool largeBound);

// print call stack trace
#ifdef _DEBUG
void EmuPrintStackTrace(PCONTEXT ContextRecord);
#endif

// global flags specifying current emulation state
extern volatile bool g_bEmuException;
extern volatile bool g_bEmuSuspended;

// global exception patching address
extern uint32 funcExclude[2048];

// partition emulation directory handles
extern HANDLE g_hCurDir;
extern CHAR  *g_strCurDrive;
extern HANDLE g_hTDrive;
extern CHAR  *g_strTDrive;
extern HANDLE g_hUDrive;
extern CHAR  *g_strUDrive;
extern HANDLE g_hZDrive;
extern CHAR  *g_strZDrive;
extern HWND   g_hEmuWindow;

// thread notification routine
extern PVOID g_pfnThreadNotification[16];
extern int g_iThreadNotificationCount;

extern DWORD_PTR g_CPUXbox;
extern DWORD_PTR g_CPUOthers;

// Delta added to host SystemTime, used in xboxkrnl::KeQuerySystemTime and xboxkrnl::NtSetSystemTime
extern LARGE_INTEGER HostSystemTimeDelta;

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

extern void InitializeSectionStructures(void);

#endif
