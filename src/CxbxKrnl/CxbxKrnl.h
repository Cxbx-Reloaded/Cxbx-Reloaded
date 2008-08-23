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

#include "Cxbx.h"

#include "Common/Xbe.h"

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>
#include <multimon.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! validate version string match */
CXBXKRNL_API bool CxbxKrnlVerifyVersion(const char *szVersion);

/*! initialize emulation */
CXBXKRNL_API void CxbxKrnlInit(HWND hwndParent, void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

/*! cleanup emulation */
CXBXKRNL_API void CxbxKrnlCleanup(const char *szErrorMessage, ...);

/*! register a thread handle */
CXBXKRNL_API void CxbxKrnlRegisterThread(HANDLE hThread);

/*! suspend emulation */
CXBXKRNL_API void CxbxKrnlSuspend();

/*! resume emulation */
CXBXKRNL_API void CxbxKrnlResume();

/*! terminate the calling thread */
CXBXKRNL_API void CxbxKrnlTerminateThread();

/*! kernel panic (trap for unimplemented kernel functions) */
CXBXKRNL_API void CxbxKrnlPanic();

/*! empty function */
CXBXKRNL_API void CxbxKrnlNoFunc();

/*! kernel thunk table */
extern CXBXKRNL_API uint32 CxbxKrnl_KernelThunkTable[367];

/*! thread local storage structure */
extern CXBXKRNL_API Xbe::TLS *CxbxKrnl_TLS;

/*! thread local storage data */
extern CXBXKRNL_API void *CxbxKrnl_TLSData;

/*! xbe header structure */
extern CXBXKRNL_API Xbe::Header *CxbxKrnl_XbeHeader;

/*! parent window handle */
extern CXBXKRNL_API HWND CxbxKrnl_hEmuParent;

#ifdef __cplusplus
}
#endif

#endif
