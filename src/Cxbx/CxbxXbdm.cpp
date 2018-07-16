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
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "XBDM"

#include <cstdio>
#include <cctype>
#include <clocale>

#include "CxbxCommon.h"
#include "Logging.h"

namespace xbdm {
	// 0x0001 (1)
	void DmAllocatePool() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0002 (2)
	void DmAllocatePoolWithTag() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0003 (3)
	void DmCloseCounters() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0004 (4)
	void DmCloseLoadedModules() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0005 (5)
	void DmCloseModuleSections() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0006 (6)
	void DmCloseNotificationSession() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0007 (7)
	void DmClosePerformanceCounter() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0008 (8)
	void DmContinueThread() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0009 (9)
	void DmFreePool() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000A (10)
	void DmGetMemory() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000B (11)
	void DmGetModuleLongName() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000C (12)
	void DmGetProcAddress() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000D (13)
	void DmGetThreadContext() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000E (14)
	void DmGetThreadInfo() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x000F (15)
	void DmGetThreadList() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0010 (16)
	void DmGetXbeInfo() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0011 (17)
	void DmGetXboxName() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0012 (18)
	void DmGetXtlData() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0013 (19)
	void DmGo() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0014 (20)
	void DmHaltThread() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0015 (21)
	void DmIsDebuggerPresent() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0016 (22)
	void DmIsThreadStopped() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0017 (23)
	void DmLoadExtension() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0018 (24)
	void DmNotify() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0019 (25)
	void DmOpenNotificationSession() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x001A (26)
	void DmOpenPerformanceCounter() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x001B (27)
	void DmQueryPerformanceCounterHandle() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x001C (28)
	void DmReboot() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x001D (29) Unassigned

	// 0x001E (30)
	HRESULT DmRegisterCommandProcessor(LPCSTR sz, void *pfn) // PDM_CMDPROC  pfn
	{ 
		LOG_FUNC_BEGIN
			LOG_FUNC_ARG(sz)
			LOG_FUNC_ARG(pfn)
			LOG_FUNC_END; 

		LOG_UNIMPLEMENTED(); // TODO : Connect the command processor callback to Cxbx'x debug console

		return S_OK;
	}

	// 0x001F (31) Unassigned
	// 0x0020 (32)
	void DmRegisterNotificationProcessor() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0021 (33)
	void DmRegisterPerformanceCounter() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0022 (34)
	void DmRemoveBreakpoint() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0023 (35)
	void DmResumeThread() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }

	// 0x0024 (36)
	void DmSendNotificationString(LPCSTR sz)
	{ 
		// Just send this string to Cxbx's debug output :
		DbgPrintf("%s\n", sz);
	}

	// 0x0025 (37)
	void DmSetBreakpoint() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0026 (38)
	void DmSetDataBreakpoint() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0027 (39)
	void DmSetInitialBreakpoint() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0028 (40)
	void DmSetMemory() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0029 (41)
	void DmSetThreadContext() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002A (42)
	void DmSetTitle() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002B (43)
	void DmSetXboxName() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002C (44)
	void DmStartProfile() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002D (45)
	void DmStop() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002E (46)
	void DmStopOn() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x002F (47)
	void DmStopProfile() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0030 (48)
	void DmSuspendThread() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0031 (49)
	void DmThreadUserData() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0032 (50)
	void DmUnloadExtension() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0033 (51)
	void DmWalkLoadedModules() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0034 (52)
	void DmWalkModuleSections() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0035 (53)
	void DmWalkPerformanceCounters() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0036 (54)
	void PsGetCurrentThreadId() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0037 (55)
	void DmEnableGPUCounter() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0038 (56)
	void DmIsBreakpoint() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0039 (57)
	void DmCloseUserList() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003A (58)
	void DmSetUserAccess() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003B (59)
	void DmGetUserAccess() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003C (60)
	void DmWalkUserList() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003D (61)
	void DmAddUser() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003E (62)
	void DmEnableSecurity() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x003F (63)
	void DmIsSecurityEnabled() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0040 (64)
	void DmRemoveUser() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0041 (65)
	void _CAP_Start_Profiling() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0042 (66)
	void _CAP_End_Profiling() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0043 (67)
	void DmProfileControl() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0044 (68)
	void _penter() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0045 (69)
	void _pexit() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0046 (70)
	void _CAP_Enter_Function() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0047 (71)
	void _CAP_Exit_Function() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
	// 0x0048 (72)
	void DmRegisterCommandProcessorEx() { LOG_FUNC(); LOG_UNIMPLEMENTED(); }
}

uint32 Cxbx_LibXbdmThunkTable[1 + 72] =
{
#define PANIC(numb) numb
#define FUNC(f) f
#define VARIABLE(v) v

	(uint32)PANIC(0x0000),                                         // 0x0000 (0)   NULL
	(uint32)FUNC(&xbdm::DmAllocatePool),                           // 0x0001 (1)
	(uint32)FUNC(&xbdm::DmAllocatePoolWithTag),                    // 0x0002 (2)
	(uint32)FUNC(&xbdm::DmCloseCounters),                          // 0x0003 (3)
	(uint32)FUNC(&xbdm::DmCloseLoadedModules),                     // 0x0004 (4)
	(uint32)FUNC(&xbdm::DmCloseModuleSections),                    // 0x0005 (5)
	(uint32)FUNC(&xbdm::DmCloseNotificationSession),               // 0x0006 (6)
	(uint32)FUNC(&xbdm::DmClosePerformanceCounter),                // 0x0007 (7)
	(uint32)FUNC(&xbdm::DmContinueThread),                         // 0x0008 (8)
	(uint32)FUNC(&xbdm::DmFreePool),                               // 0x0009 (9)
	(uint32)FUNC(&xbdm::DmGetMemory),                              // 0x000A (10)
	(uint32)FUNC(&xbdm::DmGetModuleLongName),                      // 0x000B (11)
	(uint32)FUNC(&xbdm::DmGetProcAddress),                         // 0x000C (12)
	(uint32)FUNC(&xbdm::DmGetThreadContext),                       // 0x000D (13)
	(uint32)FUNC(&xbdm::DmGetThreadInfo),                          // 0x000E (14)
	(uint32)FUNC(&xbdm::DmGetThreadList),                          // 0x000F (15)
	(uint32)FUNC(&xbdm::DmGetXbeInfo),                             // 0x0010 (16)
	(uint32)FUNC(&xbdm::DmGetXboxName),                            // 0x0011 (17)
	(uint32)FUNC(&xbdm::DmGetXtlData),                             // 0x0012 (18)
	(uint32)FUNC(&xbdm::DmGo),                                     // 0x0013 (19)
	(uint32)FUNC(&xbdm::DmHaltThread),                             // 0x0014 (20)
	(uint32)FUNC(&xbdm::DmIsDebuggerPresent),                      // 0x0015 (21)
	(uint32)FUNC(&xbdm::DmIsThreadStopped),                        // 0x0016 (22)
	(uint32)FUNC(&xbdm::DmLoadExtension),                          // 0x0017 (23)
	(uint32)FUNC(&xbdm::DmNotify),                                 // 0x0018 (24)
	(uint32)FUNC(&xbdm::DmOpenNotificationSession),                // 0x0019 (25)
	(uint32)FUNC(&xbdm::DmOpenPerformanceCounter),                 // 0x001A (26)
	(uint32)FUNC(&xbdm::DmQueryPerformanceCounterHandle),          // 0x001B (27)
	(uint32)FUNC(&xbdm::DmReboot),                                 // 0x001C (28)
	(uint32)PANIC(0x001D),                                         // 0x001D (29)
	(uint32)FUNC(&xbdm::DmRegisterCommandProcessor),               // 0x001E (30)
	(uint32)PANIC(0x001F),                                         // 0x001F (31)
	(uint32)FUNC(&xbdm::DmRegisterNotificationProcessor),          // 0x0020 (32)
	(uint32)FUNC(&xbdm::DmRegisterPerformanceCounter),             // 0x0021 (33)
	(uint32)FUNC(&xbdm::DmRemoveBreakpoint),                       // 0x0022 (34)
	(uint32)FUNC(&xbdm::DmResumeThread),                           // 0x0023 (35)
	(uint32)FUNC(&xbdm::DmSendNotificationString),                 // 0x0024 (36)
	(uint32)FUNC(&xbdm::DmSetBreakpoint),                          // 0x0025 (37)
	(uint32)FUNC(&xbdm::DmSetDataBreakpoint),                      // 0x0026 (38)
	(uint32)FUNC(&xbdm::DmSetInitialBreakpoint),                   // 0x0027 (39)
	(uint32)FUNC(&xbdm::DmSetMemory),                              // 0x0028 (40)
	(uint32)FUNC(&xbdm::DmSetThreadContext),                       // 0x0029 (41)
	(uint32)FUNC(&xbdm::DmSetTitle),                               // 0x002A (42)
	(uint32)FUNC(&xbdm::DmSetXboxName),                            // 0x002B (43)
	(uint32)FUNC(&xbdm::DmStartProfile),                           // 0x002C (44)
	(uint32)FUNC(&xbdm::DmStop),                                   // 0x002D (45)
	(uint32)FUNC(&xbdm::DmStopOn),                                 // 0x002E (46)
	(uint32)FUNC(&xbdm::DmStopProfile),                            // 0x002F (47)
	(uint32)FUNC(&xbdm::DmSuspendThread),                          // 0x0030 (48)
	(uint32)FUNC(&xbdm::DmThreadUserData),                         // 0x0031 (49)
	(uint32)FUNC(&xbdm::DmUnloadExtension),                        // 0x0032 (50)
	(uint32)FUNC(&xbdm::DmWalkLoadedModules),                      // 0x0033 (51)
	(uint32)FUNC(&xbdm::DmWalkModuleSections),                     // 0x0034 (52)
	(uint32)FUNC(&xbdm::DmWalkPerformanceCounters),                // 0x0035 (53)
	(uint32)FUNC(&xbdm::PsGetCurrentThreadId),                     // 0x0036 (54)
	(uint32)FUNC(&xbdm::DmEnableGPUCounter),                       // 0x0037 (55)
	(uint32)FUNC(&xbdm::DmIsBreakpoint),                           // 0x0038 (56)
	(uint32)FUNC(&xbdm::DmCloseUserList),                          // 0x0039 (57)
	(uint32)FUNC(&xbdm::DmSetUserAccess),                          // 0x003A (58)
	(uint32)FUNC(&xbdm::DmGetUserAccess),                          // 0x003B (59)
	(uint32)FUNC(&xbdm::DmWalkUserList),                           // 0x003C (60)
	(uint32)FUNC(&xbdm::DmAddUser),                                // 0x003D (61)
	(uint32)FUNC(&xbdm::DmEnableSecurity),                         // 0x003E (62)
	(uint32)FUNC(&xbdm::DmIsSecurityEnabled),                      // 0x003F (63)
	(uint32)FUNC(&xbdm::DmRemoveUser),                             // 0x0040 (64)
	(uint32)FUNC(&xbdm::_CAP_Start_Profiling),                     // 0x0041 (65)
	(uint32)FUNC(&xbdm::_CAP_End_Profiling),                       // 0x0042 (66)
	(uint32)FUNC(&xbdm::DmProfileControl),                         // 0x0043 (67)
	(uint32)FUNC(&xbdm::_penter),                                  // 0x0044 (68)
	(uint32)FUNC(&xbdm::_pexit),                                   // 0x0045 (69)
	(uint32)FUNC(&xbdm::_CAP_Enter_Function),                      // 0x0046 (70)
	(uint32)FUNC(&xbdm::_CAP_Exit_Function),                       // 0x0047 (71)
	(uint32)FUNC(&xbdm::DmRegisterCommandProcessorEx),             // 0x0048 (72)
};
