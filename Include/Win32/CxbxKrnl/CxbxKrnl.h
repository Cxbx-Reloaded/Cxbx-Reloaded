// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->CxbxKrnl.h
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
#ifndef CXBXKRNL_H
#define CXBXKRNL_H

// ******************************************************************
// * namespace used to avoid collisions with ntdll and win32
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#if defined(__cplusplus)
extern "C"
{
#endif

// ******************************************************************
// * cxbxkrnl exports, others import
// ******************************************************************
#ifndef CXBXKRNL_INTERNAL
#define CXBXKRNL_API DECLSPEC_IMPORT
#else
#define CXBXKRNL_API DECLSPEC_EXPORT
#endif

// ******************************************************************
// * data: KernelThunkTable
// ******************************************************************
extern CXBXKRNL_API uint32 KernelThunkTable[367];

// ******************************************************************
// * func: EmuXInit
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXInit(uint32 DebugConsole, uint08 *XBEHeader, uint32 XBEHeaderSize, void (*Entry)());

// ******************************************************************
// * func: EmuXDummy
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXDummy();

// ******************************************************************
// * func: EmuXPanic
// ******************************************************************
CXBXKRNL_API void NTAPI EmuXPanic();

// ******************************************************************
// * _EMUX_KPCR
// ******************************************************************
// *
// * Emulated Xbox KPCR. Includes special field(s) for emulation
// * purposes.
// *
// ******************************************************************
typedef struct _EMUX_KPCR
{
    struct xboxkrnl::_KPCR  Pcr;
    uint16                  OriginalFS;     // 0x025C (our cached FS register from win2k/XP)
}
EMUX_KPCR, *PEMUX_KPCR;

#if defined(__cplusplus)
}
#endif

#endif
