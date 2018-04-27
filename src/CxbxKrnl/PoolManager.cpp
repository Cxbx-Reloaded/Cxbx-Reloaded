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
// *   Cxbx->Win32->CxbxKrnl->PoolManager.cpp
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

#include "PoolManager.h"
#include <assert.h>

#define LOG_PREFIX "PMEM"


#define MARK_POOL_HEADER_ALLOCATED(POOLHEADER)      {(POOLHEADER)->PoolIndex = 0x80;}
#define MARK_POOL_HEADER_FREED(POOLHEADER)          {(POOLHEADER)->PoolIndex = 0;}
#define IS_POOL_HEADER_MARKED_ALLOCATED(POOLHEADER) ((POOLHEADER)->PoolIndex == 0x80)


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
		LIST_ENTRY_INITIALIZE_HEAD(m_NonPagedPoolDescriptor.ListHeads[Index]);
	}

	for (Index = 0; Index < POOL_SMALL_LISTS; Index++) {
		Lookaside = &m_ExpSmallNPagedPoolLookasideLists[Index];
		Lookaside->ListHead.Alignment = 0;
		Lookaside->Depth = 2;
		Lookaside->TotalAllocates = 0;
		Lookaside->AllocateHits = 0;
	}

	printf(LOG_PREFIX " Pool manager initialized!\n");
}

void* PoolManager::AllocatePool(size_t Size, uint32_t Tag)
{
	PVOID Block;
	PPOOL_HEADER Entry;
	PPOOL_LOOKASIDE_LIST LookasideList;
	PPOOL_HEADER NextEntry;
	PPOOL_HEADER SplitEntry;
	PPOOL_DESCRIPTOR PoolDesc = &m_NonPagedPoolDescriptor;
	ULONG Index;
	ULONG ListNumber;
	ULONG NeededSize;
	xboxkrnl::PLIST_ENTRY ListHead;
	ULONG NumberOfPages;

	assert(Size);


	if (Size > POOL_BUDDY_MAX) {
		Lock();

		PoolDesc->RunningAllocs += 1;
		Entry = reinterpret_cast<PPOOL_HEADER>(g_VMManager.AllocateSystemMemory(PoolType, XBOX_PAGE_READWRITE, Size, false));

		if (Entry != nullptr) {
			NumberOfPages = ROUND_UP_4K(Size) >> PAGE_SHIFT;
			PoolDesc->TotalBigPages += NumberOfPages;
			Unlock();
		}
		else {
			EmuWarning("AllocatePool returns nullptr");
			Unlock();
		}

		return Entry;
	}

	ListNumber = ((Size + POOL_OVERHEAD + (POOL_SMALLEST_BLOCK - 1)) >> POOL_BLOCK_SHIFT);
	NeededSize = ListNumber;

	if (NeededSize <= POOL_SMALL_LISTS) {
		LookasideList = &m_ExpSmallNPagedPoolLookasideLists[NeededSize - 1];
		LookasideList->TotalAllocates += 1;

		Entry = reinterpret_cast<PPOOL_HEADER>(xboxkrnl::KRNL(InterlockedPopEntrySList(&LookasideList->ListHead)));

		if (Entry != nullptr) {
			Entry -= 1;
			LookasideList->AllocateHits += 1;

			Entry->PoolType = static_cast<UCHAR>(1);
			MARK_POOL_HEADER_ALLOCATED(Entry);

			Entry->PoolTag = Tag;
			(reinterpret_cast<PULONG>((reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)))[0] = 0;

			return reinterpret_cast<PUCHAR>(Entry) + POOL_OVERHEAD;
		}
	}

	Lock();

	PoolDesc->RunningAllocs += 1;
	ListHead = &PoolDesc->ListHeads[ListNumber];

	do {
		do {
			if (IS_LIST_EMPTY(ListHead) == false) {
				Block = REMOVE_HEAD_LIST(ListHead);
				Entry = reinterpret_cast<PPOOL_HEADER>((static_cast<PCHAR>(Block) - POOL_OVERHEAD));

				assert(Entry->BlockSize >= NeededSize);
				assert(Entry->PoolType == 0);

				if (Entry->BlockSize != NeededSize) {
					if (Entry->PreviousSize == 0) {
						SplitEntry = reinterpret_cast<PPOOL_HEADER>((reinterpret_cast<PPOOL_BLOCK>(Entry) + NeededSize));
						SplitEntry->BlockSize = Entry->BlockSize - static_cast<UCHAR>(NeededSize);
						SplitEntry->PreviousSize = static_cast<UCHAR>(NeededSize);

						NextEntry = reinterpret_cast<PPOOL_HEADER>((reinterpret_cast<PPOOL_BLOCK>(SplitEntry) + SplitEntry->BlockSize));
						if (PAGE_END(NextEntry) == false) {
							NextEntry->PreviousSize = SplitEntry->BlockSize;
						}
					}
					else {
						SplitEntry = Entry;
						Entry->BlockSize -= static_cast<UCHAR>(NeededSize);
						Entry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + Entry->BlockSize);
						Entry->PreviousSize = SplitEntry->BlockSize;

						NextEntry = reinterpret_cast<PPOOL_HEADER>(reinterpret_cast<PPOOL_BLOCK>(Entry) + NeededSize);
						if (PAGE_END(NextEntry) == false) {
							NextEntry->PreviousSize = static_cast<UCHAR>(NeededSize);
						}
					}
					Entry->BlockSize = static_cast<UCHAR>(NeededSize);
					SplitEntry->PoolType = 0;
					Index = SplitEntry->BlockSize;

					LIST_ENTRY_INSERT_TAIL(&PoolDesc->ListHeads[Index - 1], (reinterpret_cast<xboxkrnl::PLIST_ENTRY>((reinterpret_cast<PCHAR>(SplitEntry)
						+ POOL_OVERHEAD))));
				}

				Entry->PoolType = static_cast<UCHAR>(1);

				MARK_POOL_HEADER_ALLOCATED(Entry);

				Unlock();

				Entry->PoolTag = Tag;
				(reinterpret_cast<PULONGLONG>((reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD)))[0] = 0;

				return reinterpret_cast<PCHAR>(Entry) + POOL_OVERHEAD;
			}
			ListHead += 1;

		} while (ListHead != &PoolDesc->ListHeads[POOL_LIST_HEADS]);

		Entry = reinterpret_cast<PPOOL_HEADER>(g_VMManager.AllocateSystemMemory(PoolType, XBOX_PAGE_READWRITE, PAGE_SIZE, false));

		if (Entry == nullptr) {
			EmuWarning("AllocatePool returns nullptr");
			Unlock();

			return Entry;
		}
		PoolDesc->TotalPages += 1;
		Entry->PoolType = 0;

		if ((PAGE_SIZE / POOL_SMALLEST_BLOCK) > 255) {
			Entry->BlockSize = 255;

		}
		else {
			Entry->BlockSize = static_cast<UCHAR>((PAGE_SIZE / POOL_SMALLEST_BLOCK));
		}

		Entry->PreviousSize = 0;
		ListHead = &PoolDesc->ListHeads[POOL_LIST_HEADS - 1];

		LIST_ENTRY_INSERT_HEAD(ListHead, (reinterpret_cast<xboxkrnl::PLIST_ENTRY>((reinterpret_cast<UCHAR>(Entry) + POOL_OVERHEAD))));

	} while (true);
}

void PoolManager::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void PoolManager::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}
