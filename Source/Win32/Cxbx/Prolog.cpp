// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->Cxbx->Prolog.cpp
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
#include "Prolog.h"

// ******************************************************************
// * .cxbxplg
// ******************************************************************
#pragma code_seg(".cxbxplg")
#pragma comment(linker, "/SECTION:.cxbxplg,RW")

// ******************************************************************
// * Func : Prolog
// ******************************************************************
__declspec(allocate(".cxbxplg")) uint08 Prolog[] =
{
    // ******************************************************************
    // *
    // * WARNING:
    // *
    // * This function is hard-coded as an array to stress the point that
    // * it can not be modified (unless you really know what you're
    // * doing). EmuExe will modify the contents of this function
    // * directly, so this precise structure is necessary.
    // *
    // ******************************************************************
    0xBE, 0xC3, 0xC3, 0xC3, 0xC3,   // mov esi, 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0x68, 0xC3, 0xC3, 0xC3, 0xC3,   // push 0xC3C3C3C3
    0xFF, 0xD6,                     // call esi
    0xC3                            // ret
};

#pragma code_seg()
