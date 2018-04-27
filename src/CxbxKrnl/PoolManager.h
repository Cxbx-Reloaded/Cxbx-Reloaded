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
// *   Cxbx->Win32->CxbxKrnl->PoolManager.h
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

#include <windows.h>
#include "VMManager.h"

#define POOL_BLOCK_SHIFT 5
#define POOL_LIST_HEADS (PAGE_SIZE / (1 << POOL_BLOCK_SHIFT)) // 0x80
#define POOL_SMALL_LISTS 8


typedef struct _POOL_DESCRIPTOR {
	ULONG RunningAllocs;
	ULONG RunningDeAllocs;
	ULONG TotalPages;
	ULONG TotalBigPages;
	xboxkrnl::LIST_ENTRY ListHeads[POOL_LIST_HEADS];
} POOL_DESCRIPTOR, *PPOOL_DESCRIPTOR;

typedef struct _POOL_LOOKASIDE_LIST {
	xboxkrnl::SLIST_HEADER ListHead;
	USHORT Depth;
	USHORT Padding;
	ULONG TotalAllocates;
	ULONG AllocateHits;
} POOL_LOOKASIDE_LIST, *PPOOL_LOOKASIDE_LIST;


class PoolManager
{
	public:
		// constructor
		PoolManager() {};
		// destructor
		~PoolManager() { DeleteCriticalSection(&m_CriticalSection); }
		// initializes the pool manager to the default configuration
		void InitializePool();

	private:
		// main (and only) pool type available on the Xbox
		POOL_DESCRIPTOR m_NonPagedPoolDescriptor;
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
