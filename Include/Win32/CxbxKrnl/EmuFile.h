// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFile.h
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
#ifndef EMUFILE_H
#define EMUFILE_H

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include "Emu.h"

// ******************************************************************
// * emulated "special" file handle
// ******************************************************************
struct EmuHandle
{
    enum _EmuHandleType m_Type;
};

// ******************************************************************
// * Various "special" handle types
// ******************************************************************
typedef enum _EmuHandleType
{
    EMUHANDLE_TYPE_PARTITION1 = 0,
    EMUHANDLE_TYPE_TDATA
}
EmuHandleType;

// ******************************************************************
// * is hFile a 'special' emulated handle?
// ******************************************************************
static inline bool IsEmuHandle(xboxkrnl::HANDLE hFile)
{
    bool ret = ((uint32)hFile) > 0x80000000;

    return ret;
}

// ******************************************************************
// * convert from 'special' emulated handle to a pointer
// ******************************************************************
static inline EmuHandle *EmuHandleToPtr(xboxkrnl::HANDLE hFile)
{
    return (EmuHandle*)((uint32)hFile - 0x80000000);
}

// ******************************************************************
// * convert from 'special' emulated handle to a pointer
// ******************************************************************
static inline HANDLE PtrToEmuHandle(EmuHandle *pEmuHandle)
{
    return (HANDLE)((uint32)pEmuHandle + 0x80000000);
}

#endif
