// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuLDT.cpp
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

#include "Cxbx.h"

#include "Emu.h"
#include "EmuFS.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

// ******************************************************************
// * Maximum number of threads this LDT system will handle
// ******************************************************************
#define MAXIMUM_XBOX_THREADS 256

// ******************************************************************
// * Table of free LDT entries
// ******************************************************************
static uint16 FreeLDTEntries[MAXIMUM_XBOX_THREADS];

// ******************************************************************
// * Critical section lock
// ******************************************************************
static CRITICAL_SECTION EmuLDTLock;

// ******************************************************************
// * func: EmuInitLDT
// ******************************************************************
void EmuInitLDT()
{
    InitializeCriticalSection(&EmuLDTLock);

    for(uint32 v=0;v<MAXIMUM_XBOX_THREADS;v++)
        FreeLDTEntries[v] = (uint16)((v*8) + 7 + 8);
}

// ******************************************************************
// * func: EmuAllocateLDT
// ******************************************************************
uint16 EmuAllocateLDT(uint32 dwBaseAddr, uint32 dwLimit)
{
    NtDll::LDT_ENTRY LDTEntry;

    int x=0;

    EnterCriticalSection(&EmuLDTLock);

    // ******************************************************************
    // * Locate a free LDT entry
    // ******************************************************************
    {
        for(x=0;x<MAXIMUM_XBOX_THREADS;x++)
            if(FreeLDTEntries[x])
                break;

        if(x == MAXIMUM_XBOX_THREADS)
        {
            LeaveCriticalSection(&EmuLDTLock);

			EmuCleanup("Could not locate free LDT entry (too many threads?)");

            return 0;
        }
    }

    // ******************************************************************
    // * Set up selector information
    // ******************************************************************
    {
        LDTEntry.BaseLow                    = (WORD)(dwBaseAddr & 0xFFFF);
        LDTEntry.HighWord.Bits.BaseMid      = (dwBaseAddr >> 16) & 0xFF;
        LDTEntry.HighWord.Bits.BaseHi       = (dwBaseAddr >> 24) & 0xFF;
	    LDTEntry.HighWord.Bits.Type         = 0x13; // RW data segment
	    LDTEntry.HighWord.Bits.Dpl          = 3;    // user segment
	    LDTEntry.HighWord.Bits.Pres         = 1;    // present
	    LDTEntry.HighWord.Bits.Sys          = 0;
	    LDTEntry.HighWord.Bits.Reserved_0   = 0;
	    LDTEntry.HighWord.Bits.Default_Big  = 1;    // 386 segment
	    LDTEntry.HighWord.Bits.Granularity  = (dwLimit >= 0x00100000) ? 1 : 0;

        if(LDTEntry.HighWord.Bits.Granularity)
            dwLimit >>= 12;

        LDTEntry.LimitLow                   = (WORD)(dwLimit & 0xFFFF);
	    LDTEntry.HighWord.Bits.LimitHi      = (dwLimit >> 16) & 0xF;
    }

    // ******************************************************************
    // * Allocate selector
    // ******************************************************************
    {
        using namespace NtDll;

        if(!NT_SUCCESS(NtDll::NtSetLdtEntries((x*8)+7+8, LDTEntry, 0, LDTEntry)))
        {
            LeaveCriticalSection(&EmuLDTLock);

			EmuCleanup("Could not set LDT entries");

            return 0;
        }
    }

    LeaveCriticalSection(&EmuLDTLock);

    FreeLDTEntries[x] = 0;

    return (x*8)+7+8;
}

// ******************************************************************
// * func: EmuDeallocateLDT
// ******************************************************************
void EmuDeallocateLDT(uint16 wSelector)
{
    NtDll::LDT_ENTRY LDTEntry;

    EnterCriticalSection(&EmuLDTLock);

    ZeroMemory(&LDTEntry, sizeof(LDTEntry));

    NtDll::NtSetLdtEntries(wSelector, LDTEntry, 0, LDTEntry);

    FreeLDTEntries[(wSelector >> 3)-1] = wSelector;

    LeaveCriticalSection(&EmuLDTLock);

    return;
}
