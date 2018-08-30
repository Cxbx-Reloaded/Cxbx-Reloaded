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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlFs.cpp
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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

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
#include "EmuNtDll.h"
};

#include "Emu.h" // For EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, )
#include "VMManager.h"
#include "EmuKrnlFs.h"
#include "EmuKrnl.h"

#define FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES 2048

// Synchronization event for FscSetCacheSize.
HANDLE g_FscSetCacheSizeEvent;
// Synchronization event for when the number of free cache elements is not zero.
HANDLE g_FscWaitingForElementEvent;

// default number of file system cache pages
xboxkrnl::ULONG g_FscNumberOfCachePages;
// array of the system cache elements
xboxkrnl::PFSCACHE_ELEMENT g_FscElementArray;
// doubly linked list of cache elements ordered with the policy LRU (least recently used)
xboxkrnl::LIST_ENTRY g_FscLruList;


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

	PLIST_ENTRY NextListEntry;
	PFSCACHE_ELEMENT Element;

	g_VMManager.Lock();

	NextListEntry = g_FscLruList.Blink;

	while (NextListEntry != &g_FscLruList) {
		Element = CONTAINING_RECORD(NextListEntry, FSCACHE_ELEMENT, ListEntry);
		NextListEntry = Element->ListEntry.Blink;

		if (Element->CacheExtension == nullptr) {
			break;
		}

		if (Element->UsageCount == 0) {
			assert(!Element->DeletePending);

			Element->CacheExtension = nullptr;

			RemoveEntryList(&Element->ListEntry);
			InsertHeadList(&g_FscLruList, &Element->ListEntry);

			//if (!IsListEmpty(&g_FscWaitingForElementEvent.Header.WaitListHead)) {
			//	KeSetEvent(&g_FscWaitingForElementEvent, 0, FALSE);
			//}
		}
	}

	g_VMManager.Unlock();
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

	xboxkrnl::NTSTATUS status;

	//KeWaitForSingleObject(&g_FscSetCacheSizeEvent, Executive, KernelMode, FALSE, NULL);
	WaitForSingleObject(g_FscSetCacheSizeEvent, INFINITE);

	if (NumberOfCachePages > FSCACHE_MAXIMUM_NUMBER_OF_CACHE_PAGES) {
		status = STATUS_INVALID_PARAMETER;
	}
	else if (NumberOfCachePages > g_FscNumberOfCachePages) {
		status = g_VMManager.AllocateFileCacheMemory(NumberOfCachePages);
	}
	else if (NumberOfCachePages < g_FscNumberOfCachePages) {
		status = g_VMManager.DeallocateFileCacheMemory(NumberOfCachePages);
	}
	else {
		status = STATUS_SUCCESS;
	}

	//KeSetEvent(&g_FscSetCacheSizeEvent, 0, FALSE);
	SetEvent(g_FscSetCacheSizeEvent);

	RETURN(status);
}

// ******************************************************************
// * The following are internal Fsc functions not exported by the kernel
// ******************************************************************

xboxkrnl::VOID xboxkrnl::FscBuildElementLruList()
{
	LOG_FUNC();

	PFSCACHE_ELEMENT ElementArray;
	ULONG NumberOfCachePages;
	ULONG Index;

	InitializeListHead(&g_FscLruList);

	ElementArray = g_FscElementArray;
	NumberOfCachePages = g_FscNumberOfCachePages;

	for (Index = 0; Index < NumberOfCachePages; Index++) {
		if (ElementArray[Index].CacheExtension == nullptr) {
			InsertHeadList(&g_FscLruList, &ElementArray[Index].ListEntry);
		}
		else {
			InsertTailList(&g_FscLruList, &ElementArray[Index].ListEntry);
		}
	}
}
