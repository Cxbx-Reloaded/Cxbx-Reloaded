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
    __asm
    {
        mov ax, fs:[0x14]
        mov fs, ax
    }
}

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
        mov ah, fs:[0x14]
        mov chk, ah
    }

    return (chk == 1);
}

// ******************************************************************
// * func: EmuGenerateFS
// ******************************************************************
void EmuGenerateFS(Xbe::TLS *pTLS);

// ******************************************************************
// * func: EmuCleanupFS
// ******************************************************************
void EmuCleanupFS();

// ******************************************************************
// * func: EmuInitFS
// ******************************************************************
void EmuInitFS();

#endif
