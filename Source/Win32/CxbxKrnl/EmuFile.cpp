// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFile.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_LOCAL_

#include "EmuFile.h"

// Array of EmuHandles in the system
EmuHandle EmuHandle::Handles[EMU_MAX_HANDLES];

// Pointer to first free handle in array, or NULL if none
volatile EmuHandle *EmuHandle::FirstFree;

// Pointer to last free handle in array, or NULL if none
volatile EmuHandle *EmuHandle::LastFree;

// Lock on the handle system
CRITICAL_SECTION EmuHandle::HandleLock;

// ******************************************************************
// * Initialize the handle database
// ******************************************************************
bool EmuHandle::Initialize()
{
	size_t x;

	// Initialize the critical section
	InitializeCriticalSection(&HandleLock);

	// Mark all handles as free.  We also set up the linked list of
	// free handles here.
	for (x = 0; x < EMU_MAX_HANDLES; x++)
	{
		Handles[x].m_Type = EMUHANDLE_TYPE_EMPTY;
		Handles[x].m_NextFree = &Handles[x + 1];
	}

	// The last entry should have a NULL next entry
	Handles[EMU_MAX_HANDLES - 1].m_NextFree = NULL;

	// Set up the head and tail pointers
	FirstFree = &Handles[0];
	LastFree = &Handles[EMU_MAX_HANDLES];

    return true;
}

// ******************************************************************
// * func: EmuHandle::Lock
// *    Locks the handle database
// ******************************************************************
inline void EmuHandle::Lock(void)
{
	EnterCriticalSection(&HandleLock);
}

// ******************************************************************
// * func: EmuHandle::Unlock
// *    Unlocks the handle database
// ******************************************************************
inline void EmuHandle::Unlock(void)
{
	LeaveCriticalSection(&HandleLock);
}

// ******************************************************************
// * func: EmuHandle::Allocate
// *    Allocates a new handle
// ******************************************************************
EmuHandle volatile *EmuHandle::Allocate(void)
{
	volatile EmuHandle *Handle;

	// Lock the database
	Lock();

	// Get the first free entry
	Handle = FirstFree;

	// Remove it from the list
	FirstFree = Handle->m_NextFree;

	// If it was the last handle, clear LastFree
	if (!Handle->m_NextFree)
		LastFree = NULL;

	// Initialize the handle's fields
	Handle->m_Type = EMUHANDLE_TYPE_ALLOCATED;
	Handle->m_Object = NULL;

	// Unlock the database
	Unlock();

	return Handle;
}
