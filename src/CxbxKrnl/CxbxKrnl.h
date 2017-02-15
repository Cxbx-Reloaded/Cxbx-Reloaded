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

// Opcodes
#define OPCODE_NOP_90 0x90
#define OPCODE_INT3_CC 0xCC
#define OPCODE_CALL_E8 0xE8
#define OPCODE_JMP_E9 0xE9

// Thread Information Block offsets - see https://www.microsoft.com/msj/archive/S2CE.aspx
#define TIB_ArbitraryDataSlot 0x14
#define TIB_LinearSelfAddress 0x18

/*! xbaddr is the type of a physical address */
typedef uint32 xbaddr;

#define XBADDR_BITS 32
#define XBADDR_MAX UINT32_MAX

/*! memory size per system */
#define XBOX_MEMORY_SIZE 64 * 1024 * 1024
#define CHIHIRO_MEMORY_SIZE 128 * 1024 * 1024

/*! maximum memory size our emulator must support */
#define EMU_MAX_MEMORY_SIZE CHIHIRO_MEMORY_SIZE

/*! base addresses of various components */
#define XBOX_BASE_ADDR 0x00010000
#define XBOX_KERNEL_BASE 0x80010000
#define XBOX_NV2A_INIT_VECTOR 0xFF000008

/*! base address of Cxbx host executable, see Cxbx project options, Linker, Advanced, Base Address */
#define CXBX_BASE_ADDR XBOX_BASE_ADDR

#define MAX_BUS_INTERRUPT_LEVEL 27
#define MAX_NUM_INTERRUPTS 256

/*! validate version string match */
CXBXKRNL_API bool CxbxKrnlVerifyVersion(const char *szVersion);

/*! Cxbx Kernel Entry Point */
CXBXKRNL_API void CxbxKrnlMain(int argc, char* argv[]);

/*! initialize emulation */
CXBXKRNL_API void CxbxKrnlInit(HWND hwndParent, void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, const char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

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

CXBXKRNL_API void CxbxInitPerformanceCounters(); // Implemented in EmuKrnlKe.cpp

/*! kernel thunk table */
extern CXBXKRNL_API uint32 CxbxKrnl_KernelThunkTable[379];

/*! thread local storage structure */
extern CXBXKRNL_API Xbe::TLS *CxbxKrnl_TLS;

/*! thread local storage data */
extern CXBXKRNL_API void *CxbxKrnl_TLSData;

/*! xbe header structure */
extern CXBXKRNL_API Xbe::Header *CxbxKrnl_XbeHeader;

extern Xbe *CxbxKrnl_Xbe;

/*! parent window handle */
extern CXBXKRNL_API HWND CxbxKrnl_hEmuParent;
extern CXBXKRNL_API DebugMode CxbxKrnl_DebugMode;
extern CXBXKRNL_API char* CxbxKrnl_DebugFileName;

extern void ConnectWindowsTimersToThunkTable();

#ifdef __cplusplus
}
#endif

#endif
