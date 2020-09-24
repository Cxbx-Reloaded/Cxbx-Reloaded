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

#ifndef POOL_MANAGER_H
#define POOL_MANAGER_H


#include "core\kernel\memory-manager\VMManager.h"

#define POOL_BLOCK_SHIFT 5
#define POOL_LIST_HEADS (PAGE_SIZE / (1 << POOL_BLOCK_SHIFT)) // 0x80
#define POOL_SMALL_LISTS 8
#define POOL_TYPE_MASK 3


typedef struct _POOL_DESCRIPTOR {
	ULONG RunningAllocs;
	ULONG RunningDeAllocs;
	ULONG TotalPages;
	ULONG TotalBigPages;
	xbox::LIST_ENTRY ListHeads[POOL_LIST_HEADS];
} POOL_DESCRIPTOR, *PPOOL_DESCRIPTOR;


typedef struct _POOL_LOOKASIDE_LIST {
	xbox::SLIST_HEADER ListHead;
	USHORT Depth;
	USHORT Padding;
	ULONG TotalAllocates;
	ULONG AllocateHits;
} POOL_LOOKASIDE_LIST, *PPOOL_LOOKASIDE_LIST;


typedef struct _POOL_HEADER {
	union {
		struct {
			xbox::uchar_t PreviousSize;
			xbox::uchar_t PoolIndex;
			xbox::uchar_t PoolType;
			xbox::uchar_t BlockSize;
		};
		ULONG Ulong1;
	};
	ULONG PoolTag;
} POOL_HEADER, *PPOOL_HEADER;


typedef struct _POOL_BLOCK {
	xbox::uchar_t Fill[1 << POOL_BLOCK_SHIFT];
} POOL_BLOCK, *PPOOL_BLOCK;


#define POOL_OVERHEAD (static_cast<LONG>(sizeof(POOL_HEADER)))
#define POOL_SMALLEST_BLOCK (sizeof(POOL_BLOCK))
#define POOL_BUDDY_MAX (PAGE_SIZE - (POOL_OVERHEAD + POOL_SMALLEST_BLOCK ))
#define MARK_POOL_HEADER_ALLOCATED(POOLHEADER)      {(POOLHEADER)->PoolIndex = 0x80;}
#define IS_POOL_HEADER_MARKED_ALLOCATED(POOLHEADER) ((POOLHEADER)->PoolIndex == 0x80)
#define MARK_POOL_HEADER_FREED(POOLHEADER)          {(POOLHEADER)->PoolIndex = 0;}


/* PoolManager class */
class PoolManager
{
	public:
		// constructor
		PoolManager() {};
		// destructor
		~PoolManager() { DeleteCriticalSection(&m_CriticalSection); }
		// initializes the pool manager to the default configuration
		void InitializePool();
		// allocates pool memory
		VAddr AllocatePool(size_t Size, uint32_t Tag);
		// deallocates pool memory
		void DeallocatePool(VAddr addr);
		// queries the pool block size
		size_t QueryPoolSize(VAddr addr);


	private:
		// main (and only) pool type available on the Xbox
		POOL_DESCRIPTOR m_NonPagedPoolDescriptor;
		// lookaside lists
		POOL_LOOKASIDE_LIST m_ExpSmallNPagedPoolLookasideLists[POOL_SMALL_LISTS];
		// critical section lock to synchronize accesses
		CRITICAL_SECTION m_CriticalSection;
	
	
		// acquires the critical section
		void Lock();
		// releases the critical section
		void Unlock();
};


extern PoolManager g_PoolManager;

#endif
