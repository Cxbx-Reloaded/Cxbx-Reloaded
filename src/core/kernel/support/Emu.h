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

#include <windows.h>
#include <multimon.h>

std::string FormatTitleId(uint32_t title_id);

// exception handler
class ExceptionManager {
public:
	ExceptionManager();
	virtual ~ExceptionManager();
	// We want to make sure the core's exception is trigger front and last of exception management. For two reasons:
	// Performance wise for LLE emulation and general emulation, to remove false positive exception from drivers, before we show an actual fatal error message.
	void EmuX86_Init();
	// If any custom VEH should be add here than try on their own. See private AddVEH function's comment.
	bool AddVEH(unsigned long first, PVECTORED_EXCEPTION_HANDLER veh_handler);
private:
	std::vector<void*> veh_handles;
	bool accept_request;
	// Since Vectored Exception Handlers are global than per thread, we only need to register once.
	bool AddVEH(unsigned long first, PVECTORED_EXCEPTION_HANDLER veh_handler, bool override_request);
};
extern class ExceptionManager* g_ExceptionManager;

// print call stack trace
#ifdef _DEBUG
void EmuPrintStackTrace(PCONTEXT ContextRecord);
#endif

// global flags specifying current emulation state
extern volatile thread_local bool g_bEmuException;
extern volatile bool g_bEmuSuspended;

// global exception patching address
extern void * funcExclude[2048];

extern HWND   g_hEmuWindow;

#define GET_FRONT_WINDOW_HANDLE ((CxbxKrnl_hEmuParent != nullptr) ? CxbxKrnl_hEmuParent : g_hEmuWindow)

// thread notification routine
extern PVOID g_pfnThreadNotification[16];
extern int g_iThreadNotificationCount;

extern HANDLE g_CurrentProcessHandle; // Set in CxbxKrnlMain

// Delta added to host SystemTime, used in KiClockIsr and KeSetSystemTime
extern std::atomic_int64_t HostSystemTimeDelta;

typedef struct DUMMY_KERNEL
{
	IMAGE_DOS_HEADER DosHeader;
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_SECTION_HEADER SectionHeader;
} *PDUMMY_KERNEL;

extern bool g_DisablePixelShaders;
extern bool g_UseAllCores;
extern bool g_SkipRdtscPatching;
#endif
