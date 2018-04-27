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

void PoolManager::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void PoolManager::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}
