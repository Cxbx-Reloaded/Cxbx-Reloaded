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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::FSC

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For FscGetCacheSize, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"

// prevent name collisions
namespace NtDll
{
#include "core\kernel\support\EmuNtDll.h"
};

#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )

#define FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES 2048

// global variables
xboxkrnl::LONG g_FscNumberOfCachePages = 16; // 16 = default number of file system cache pages

// ******************************************************************
// * 0x0023 - FscGetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(35) xboxkrnl::ULONG NTAPI xboxkrnl::FscGetCacheSize()
{
	LOG_FUNC();

	RETURN(g_FscNumberOfCachePages);
}

// ******************************************************************
// * 0x0024 - FscInvalidateIdleBlocks()
// ******************************************************************
XBSYSAPI EXPORTNUM(36) xboxkrnl::VOID NTAPI xboxkrnl::FscInvalidateIdleBlocks()
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

static xboxkrnl::KEVENT g_FscCacheEvent;

xboxkrnl::VOID xboxkrnl::InitializeFscCacheEvent()
{
    KeInitializeEvent(&g_FscCacheEvent, SynchronizationEvent, TRUE);
}

// ******************************************************************
// * 0x0025 - FscSetCacheSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(37) xboxkrnl::NTSTATUS NTAPI xboxkrnl::FscSetCacheSize
(
	ULONG NumberOfCachePages
)
{
	LOG_FUNC_ONE_ARG(NumberOfCachePages);

	NTSTATUS ret = STATUS_SUCCESS;
	KeWaitForSingleObject(&g_FscCacheEvent, Executive, 0, 0, 0);
	UCHAR orig_irql = KeRaiseIrqlToDpcLevel();

	if (NumberOfCachePages > FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES) {
		ret = STATUS_INVALID_PARAMETER;
	}
	else {
		// TODO : Actually allocate file system cache pages.
#if 0
		if (NumberOfCachePages > g_FscNumberOfCachePages) {
			ret = FscGrowCacheSize(NumberOfCachePages);
		}
		else if (NumberOfCachePages < g_FscNumberOfCachePages) {
			ret = FscShrinkCacheSize(NumberOfCachePages);
		}
#endif
		g_FscNumberOfCachePages = NumberOfCachePages;
	}

	KfLowerIrql(orig_irql);
	KeSetEvent(&g_FscCacheEvent, 0, 0);
	RETURN(ret);
}

