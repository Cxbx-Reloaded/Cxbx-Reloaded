// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Emu.h
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
#ifndef EMU_H
#define EMU_H

#include "Xbe.h"

/*
// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
    #include "Emuxapi.h"
};

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace win32
{
    #undef FIELD_OFFSET     // prevent macro redefinition warnings
    #include <windows.h>
    #include <stdio.h>

    #define DIRECT3D_VERSION 0x0800
    #include <d3d8.h>

    #define DIRECTINPUT_VERSION 0x0800
    #include <dinput.h>
};

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include "EmuD3D.h"
    #include "EmuDInput.h"
};

// ******************************************************************
// * globals
// ******************************************************************
extern win32::HWND              g_EmuWindow;       // Rendering Window
extern xboxkrnl::XINPUT_STATE   g_EmuController1;

#include "OOVPA.h"
#include "HLEDataBase.h"
#include "EmuKrnl.h"
#include "EmuLDT.h"
#include "EmuFS.h"
*/

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * func: EmuNoFunc
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuNoFunc();

// ******************************************************************
// * func: EmuInit
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuInit(Xbe::LibraryVersion *LibraryVersion, DebugMode DebugConsole, char *DebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

// ******************************************************************
// * func: EmuPanic
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuPanic();

// ******************************************************************
// * data: KernelThunkTable
// ******************************************************************
extern "C" CXBXKRNL_API uint32 KernelThunkTable[367];

#endif
