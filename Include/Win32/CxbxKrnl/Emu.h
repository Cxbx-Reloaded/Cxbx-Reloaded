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

// pointless function
extern "C" CXBXKRNL_API void NTAPI EmuNoFunc();

// verify szVersion matches the current build version
extern "C" CXBXKRNL_API bool NTAPI EmuVerifyVersion(const char *szVersion);

// cleanup and terminate the current thread
extern "C" CXBXKRNL_API void NTAPI EmuCleanThread();

// initialize emulation
extern "C" CXBXKRNL_API void NTAPI EmuInit(void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

// print out a warning message to the kernel debug log file
#ifdef _DEBUG_WARNINGS
extern "C" CXBXKRNL_API void NTAPI EmuWarning(const char *szWarningMessage, ...);
#else
inline void NTAPI EmuWarning(const char *szWarningMessage, ...) { }
#endif

// cleanup emulation
extern "C" CXBXKRNL_API void NTAPI EmuCleanup(const char *szErrorMessage, ...);

// kernel function not implemented, panic
extern "C" CXBXKRNL_API void NTAPI EmuPanic();

// exception handler
extern int EmuException(LPEXCEPTION_POINTERS e);

// global exception patching address
extern uint32 g_HaloHack[4];

// global kernel thunk table
extern "C" CXBXKRNL_API uint32 KernelThunkTable[367];

// global thread local storage structure
extern Xbe::TLS *g_pTLS;

// global thread local storage data
extern void *g_pTLSData;

// global Xbe header cache
extern Xbe::Header *g_pXbeHeader;

// partition emulation directory handles
extern HANDLE g_hCurDir;
extern HANDLE g_hTDrive;
extern HANDLE g_hUDrive;
extern HANDLE g_hZDrive;

// thread notification routine
extern PVOID g_pfnThreadNotification;

// extra stuff for multimonitors (win95+)
typedef struct _MONITORINFO
{
  DWORD  cbSize; 
  RECT   rcMonitor; 
  RECT   rcWork; 
  DWORD  dwFlags; 
}
MONITORINFO, *LPMONITORINFO; 

#endif
