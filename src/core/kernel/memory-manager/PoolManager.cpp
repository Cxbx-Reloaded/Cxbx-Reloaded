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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::POOLMEM

#include "PoolManager.h"
#include "Logging.h"
#include "core\kernel\exports\EmuKrnl.h" // For InitializeListHead(), etc.
#include <assert.h>


PoolManager g_PoolManager;


void PoolManager::InitializePool()
{
	ULONG Index;
	PPOOL_LOOKASIDE_LIST Lookaside;

	// Set up the critical section to synchronize accesses
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x400);

	// Set up the nonpaged pool descriptor and the lookaside lists
	m_NonPagedPoolDescriptor.RunningAllocs = 0;
	m_NonPagedPoolDescriptor.RunningDeAllocs = 0;
	m_NonPagedPoolDescriptor.TotalPages = 0;
	m_NonPagedPoolDescriptor.TotalBigPages = 0;

	for (Index = 0; Index < POOL_LIST_HEADS; Index++) {
		InitializeListHead(&m_NonPagedPoolDescriptor.ListHeads[Index]);
	}

	for (Index = 0; Index < POOL_SMALL_LISTS; Index++) {
		Lookaside = &m_ExpSmallNPagedPoolLookasideLists[Index];
		Lookaside->ListHead.Alignment = 0;
		Lookaside->Depth = 2;
		Lookaside->TotalAllocates = 0;
		Lookaside->AllocateHits = 0;
	}

	printf("Pool manager initialized!\n");
}

VAddr PoolManager::AllocatePool(size_t Size, uint32_t Tag)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Size)
		LOG_FUNC_ARG(Tag)
	LOG_FUNC_END;

	PVOID Block;
	PPOOL_HEADER Entry;
	PPOOL_LOOKASIDE_LIST LookasideList;
	PPOOL_HEADER NextEntry;
	PPOOL_HEADER SplitEntry;
	PPOOL_DESCRIPTOR PoolDesc = &m_NonPagedPoolDescriptor;
	ULONG Index;
	ULONG ListNumber;
	ULONG NeededSize;
	xbox::PLIST_ENTRY ListHead;
	ULONG NumberOfPages;

	assert(Size);


	if (Size > POOL_BUDDY_MAX) {

		// The size requested is larger than 4056 bytes (maximum size mantained by a pool descriptor), so ask the VMManager to allocate
		// the allocation and return it to the title

		Lock();

		PoolDesc->RunningAllocs += 1;
		Entry = reinterpret_cast<PPOOL_HEADER>(g_VMManager.AllocateSystemMemory(PoolType, XBOX_PAGE_READWRITE, Size, false));

		if (Entry != nullptr) {
			NumberOfPages = ROUND_UP_4K(Size) >> PAGE_SHIFT;
			PoolDesc->TotalBigPages += NumberOfPages;
			Unlock();
		}
		else {
			EmuLog(LOG_LEVEL::WARNING, "AllocatePool returns nullptr");
			Unlock();
		}

		RETURN(reinterpret_cast<VAddr>(Entry));
	}

	ListNumber = ((Size + POOL_OVERHEAD + (POOL_SMALLEST_BLOCK - 1)) >> POOL_BLOCK_SHIFT);
	NeededSize = ListNumber;

	if (NeededSize <= POOL_SMALL_LISTS) {

		// Small size requested, try to use a lookaside list to satisfy the allocation. ergo720: however, on Windows NT lookaside lists
		// are first created with ExXxxLookasideListEx and ExXxxLookasideList, which are not exported by the Xbox kernel and are likely
		// not present. If so, then lookaside lists are never created and the following is likely dead code...

		LookasideList = &m_ExpSmallNPagedPoolLookasideLists[NeededSize - 1];
		LookasideList->TotalAllocates += 1;

		Entry = reinterpret_cast<PPOOL_HEADER>(xbox::KRNL(InterlockedPopEntrySList(&LookasideList->ListHead)));

		if (Entry != nullptr) {
			Entry -= 1;
			LookasideList->AllocateHits += 1;

			Entry->PoolType = static_cast<xbox::uchar_xt>(1);
			MARK_POOL_HEADER_ALLOCATED(Entry);

			Entry->PoolTag = Tag;
			(reinterpret_cast<PULONG>((reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)))[0] = 0;

			RETURN(reinterpret_cast<VAddr>(Entry) + POOL_OVERHEAD);
		}
	}

	Lock();

	PoolDesc->RunningAllocs += 1;
	ListHead = &PoolDesc->ListHeads[ListNumber];

	// We failed to satisfy the request with a lookaside list, but the size is smaller then 4056 bytes. Try to use a pool descriptor for
	// the allocation

	do {
		do {
			if (IsListEmpty(ListHead) == false) {
				Block = RemoveHeadList(ListHead);
				Entry = reinterpret_cast<PPOOL_HEADER>((static_cast<PCHAR>(Block) - POOL_OVERHEAD));

				assert(Entry->BlockSize >= NeededSize);
				assert(Entry->PoolType == 0);

				if (Entry->BlockSize != NeededSize) {
					if (Entry->PreviousSize == 0) {
						SplitEntry = reinterpret_cast<PPOOL_HEADER>((reinterpret_cast<PPOOL_BLOCK>(Entry) + NeededSize));
						SplitEntry->BlockSize = Entry->BlockSize - static_cast<xbox::uchar_xt>(NeededSize);
						SplitEntry->PreviousSize = static_cast<xbox::uchar_xt>(NeededSize);

						NextEntry = reinterpret_cast<PPOOL_HEADER>((reinterpret_cast<PPOOL_BLOCK>(SplitEntry) + SplitEntry->BlockSize));
						if (PAGE_END(NextEntry) == false) {
							NextEntry->PreviousSize = SplitEntry->BlockSize;
						}
					}
					else {
						SplitEntry = Entry;
						Entry->BlockSize -= static_cast<xbox::uchar_xt>(NeededSize);
						Entry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + Entry->BlockSize);
						Entry->PreviousSize = SplitEntry->BlockSize;

						NextEntry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + NeededSize);
						if (PAGE_END(NextEntry) == false) {
							NextEntry->PreviousSize = static_cast<xbox::uchar_xt>(NeededSize);
						}
					}
					Entry->BlockSize = static_cast<xbox::uchar_xt>(NeededSize);
					SplitEntry->PoolType = 0;
					Index = SplitEntry->BlockSize;

					InsertTailList(&PoolDesc->ListHeads[Index - 1], (reinterpret_cast<xbox::PLIST_ENTRY>((reinterpret_cast<PCHAR>(SplitEntry)
						+ POOL_OVERHEAD))));
				}

				Entry->PoolType = static_cast<xbox::uchar_xt>(1);

				MARK_POOL_HEADER_ALLOCATED(Entry);

				Unlock();

				Entry->PoolTag = Tag;
				(reinterpret_cast<PULONGLONG>((reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)))[0] = 0;

				RETURN(reinterpret_cast<VAddr>(Entry) + POOL_OVERHEAD);
			}
			ListHead += 1;

		} while (ListHead != &PoolDesc->ListHeads[POOL_LIST_HEADS]);

		// The current pool descriptor is exhausted, so we ask the VMManager to allocate a page to create a new one

		Entry = reinterpret_cast<PPOOL_HEADER>(g_VMManager.AllocateSystemMemory(PoolType, XBOX_PAGE_READWRITE, PAGE_SIZE, false));

		if (Entry == nullptr) {
			EmuLog(LOG_LEVEL::WARNING, "AllocatePool returns nullptr");
			Unlock();

			RETURN(reinterpret_cast<VAddr>(Entry));
		}
		PoolDesc->TotalPages += 1;
		Entry->PoolType = 0;

		if ((PAGE_SIZE / POOL_SMALLEST_BLOCK) > 255) {
			Entry->BlockSize = 255;
		}
		else {
			Entry->BlockSize = static_cast<xbox::uchar_xt>((PAGE_SIZE / POOL_SMALLEST_BLOCK));
		}

		Entry->PreviousSize = 0;
		ListHead = &PoolDesc->ListHeads[POOL_LIST_HEADS - 1];

		InsertHeadList(ListHead, (reinterpret_cast<xbox::PLIST_ENTRY>((reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD))));

	} while (true);
}

void PoolManager::DeallocatePool(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PPOOL_HEADER Entry;
	ULONG Index;
	PPOOL_LOOKASIDE_LIST LookasideList;
	PPOOL_HEADER NextEntry;
	PPOOL_DESCRIPTOR PoolDesc = &m_NonPagedPoolDescriptor;
	bool Combined;
	ULONG BigPages;

	if (CHECK_ALIGNMENT(addr, PAGE_SIZE)) {
		Lock();

		PoolDesc->RunningDeAllocs += 1;
		
		BigPages = g_VMManager.DeallocateSystemMemory(PoolType, addr, 0);

		PoolDesc->TotalBigPages -= BigPages;

		Unlock();

		return;
	}

	Entry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PCHAR>(addr) - POOL_OVERHEAD);

	assert((Entry->PoolType & POOL_TYPE_MASK) != 0);

	if (!IS_POOL_HEADER_MARKED_ALLOCATED(Entry)) {
		CxbxKrnlCleanup("Pool at address 0x%X is already free!", addr);
	}

	MARK_POOL_HEADER_FREED(Entry);

	assert(Entry->PoolType);

	Index = Entry->BlockSize;

	if (Index <= POOL_SMALL_LISTS) {
		LookasideList = &m_ExpSmallNPagedPoolLookasideLists[Index - 1];

		if (QUERY_DEPTH_SLIST(&LookasideList->ListHead) < LookasideList->Depth) {
			Entry += 1;
			xbox::KRNL(InterlockedPushEntrySList)(&LookasideList->ListHead, reinterpret_cast<xbox::PSINGLE_LIST_ENTRY>(Entry));

			return;
		}
	}

	Lock();

	PoolDesc->RunningDeAllocs += 1;

	Combined = false;
	NextEntry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + Entry->BlockSize);
	if (PAGE_END(NextEntry) == false) {
		if (NextEntry->PoolType == 0) {
			Combined = true;
			RemoveEntryList((reinterpret_cast<xbox::PLIST_ENTRY>(reinterpret_cast<PCHAR>(NextEntry) + POOL_OVERHEAD)));
			Entry->BlockSize += NextEntry->BlockSize;
		}
	}

	if (Entry->PreviousSize != 0) {
		NextEntry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) - Entry->PreviousSize);
		if (NextEntry->PoolType == 0) {
			Combined = true;
			RemoveEntryList((reinterpret_cast<xbox::PLIST_ENTRY>(reinterpret_cast<PCHAR>(NextEntry) + POOL_OVERHEAD)));
			NextEntry->BlockSize += Entry->BlockSize;
			Entry = NextEntry;
		}
	}
	
	if (CHECK_ALIGNMENT(reinterpret_cast<VAddr>(Entry), PAGE_SIZE) &&
		(PAGE_END(reinterpret_cast<PPOOL_BLOCK>(Entry) + Entry->BlockSize) != false)) {

		g_VMManager.DeallocateSystemMemory(PoolType, reinterpret_cast<VAddr>(Entry), 0);

		PoolDesc->TotalPages -= 1;
	}
	else {
		Entry->PoolType = 0;
		Index = Entry->BlockSize;

		if (Combined != false) {
			NextEntry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + Entry->BlockSize);
			if (PAGE_END(NextEntry) == false) {
				NextEntry->PreviousSize = Entry->BlockSize;
			}
			InsertTailList(&PoolDesc->ListHeads[Index - 1], (reinterpret_cast<xbox::PLIST_ENTRY>(
				reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)));
		}
		else {
			InsertHeadList(&PoolDesc->ListHeads[Index - 1], (reinterpret_cast<xbox::PLIST_ENTRY>(
				reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)));
		}
	}

	Unlock();
}

size_t PoolManager::QueryPoolSize(VAddr addr)
{
	LOG_FUNC_ONE_ARG(addr);

	PPOOL_HEADER Entry;
	size_t size;

	if (CHECK_ALIGNMENT(addr, PAGE_SIZE)) {
		RETURN(g_VMManager.QuerySize(addr));
	}

	Entry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PCHAR>(addr) - POOL_OVERHEAD);
	size = static_cast<size_t>((Entry->BlockSize << POOL_BLOCK_SHIFT) - POOL_OVERHEAD);

	RETURN(size);
}

void PoolManager::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void PoolManager::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}
