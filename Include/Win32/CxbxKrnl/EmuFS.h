// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFS.h
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
#ifndef EMUFS_H
#define EMUFS_H

#include "Xbe.h"

// word @ FS:[0x14] := wSwapFS
// byte @ FS:[0x16] := bIsXboxFS

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * func: EmuGenerateFS
// ******************************************************************
extern void EmuGenerateFS(Xbe::TLS *pTLS, void *pTLSData);

// ******************************************************************
// * func: EmuCleanupFS
// ******************************************************************
extern void EmuCleanupFS();

// ******************************************************************
// * func: EmuInitFS
// ******************************************************************
extern void EmuInitFS();

// ******************************************************************
// * func: EmuIsXboxFS
// ******************************************************************
// *
// * This function will return true if the current FS register is
// * the Xbox emulation variety. Alternatively, false means the
// * Win2k/XP FS register is currently loaded.
// *
// ******************************************************************
static inline bool EmuIsXboxFS()
{
    unsigned char chk;

    __asm
    {
        mov ah, fs:[0x16]
        mov chk, ah
    }

    return (chk == 1);
}

// ******************************************************************
// * data: EmuAutoSleepRate
// ******************************************************************
// *
// * Xbox is a single process system, and because of this fact, demos
// * and games are likely to suffer from Xbox-Never-Sleeps syndrome.
// *
// * Basically, there are situations where the Xbe will have no
// * reason to bother yielding to other threads. One solution to this
// * problem is to keep track of the number of function intercepts,
// * and every so often, force a sleep. This is the rate at which
// * those forced sleeps occur.
// *
// ******************************************************************
extern uint32 EmuAutoSleepRate;

// ******************************************************************
// * func: EmuSwapFS
// ******************************************************************
// *
// * This function is used to swap between the native Win2k/XP FS:
// * structure, and the Emu FS: structure. Before running Windows
// * code, you *must* swap over to Win2k/XP FS. Similarly, before
// * running Xbox code, you *must* swap back over to Emu FS.
// *
// ******************************************************************
static inline void EmuSwapFS()
{
    // Note that this is only the *approximate* interception count,
    // because not all interceptions swap the FS register, and some
    // non-interception code uses it
    static uint32 dwInterceptionCount = 0;

    __asm
    {
        mov ax, fs:[0x14]
        mov fs, ax
    }

    // ******************************************************************
    // * Every "N" interceptions, perform various periodic services
    // ******************************************************************
    if(dwInterceptionCount++ >= EmuAutoSleepRate)
    {
        // If we're in the Xbox FS, wait until the next swap
        if(EmuIsXboxFS())
        {
            dwInterceptionCount--;
            return;
        }

        // Yield!
        Sleep(1);

        // Back to Zero!
        dwInterceptionCount = 0;
    }
}

#endif
