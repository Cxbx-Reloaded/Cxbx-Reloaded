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

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

// ******************************************************************
// * func: EmuNoFunc
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuNoFunc();

// ******************************************************************
// * func: EmuVerifyVersion
// ******************************************************************
extern "C" CXBXKRNL_API bool NTAPI EmuVerifyVersion(const char *szVersion);

// ******************************************************************
// * func: EmuInit
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuInit(void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

// ******************************************************************
// * func: EmuCleanThread
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuCleanThread();

// ******************************************************************
// * func: EmuWarning
// ******************************************************************
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...);
#else
inline void NTAPI EmuWarning(const char *szWarningMessage, ...) { }
#endif

// ******************************************************************
// * func: EmuCleanup
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuCleanup(const char *szErrorMessage, ...);

// ******************************************************************
// * func: EmuPanic
// ******************************************************************
extern "C" CXBXKRNL_API void NTAPI EmuPanic();

// ******************************************************************
// * func: EmuException
// ******************************************************************
extern int EmuException(LPEXCEPTION_POINTERS e);

// ******************************************************************
// * data: KernelThunkTable
// ******************************************************************
extern "C" CXBXKRNL_API uint32 KernelThunkTable[367];

// ******************************************************************
// * data: g_pTLS
// ******************************************************************
extern Xbe::TLS *g_pTLS;

// ******************************************************************
// * data: g_pTLSData
// ******************************************************************
extern void *g_pTLSData;

// ******************************************************************
// * data: g_pXbeHeader
// ******************************************************************
extern Xbe::Header *g_pXbeHeader;

// ******************************************************************
// * data: Hard Disk Directory Handles
// ******************************************************************
extern HANDLE g_hCurDir;
extern HANDLE g_hTDrive;
extern HANDLE g_hUDrive;
extern HANDLE g_hZDrive;

#endif
