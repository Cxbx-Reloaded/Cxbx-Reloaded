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

// Sizes
#define ONE_KB 1024
#define ONE_MB (1024 * 1024)
#define X64KB 64 * ONE_KB

// Thread Information Block offsets - see https://www.microsoft.com/msj/archive/S2CE.aspx
#define TIB_ArbitraryDataSlot 0x14
#define TIB_LinearSelfAddress 0x18

#define XBADDR_BITS 32
#define XBADDR_MAX UINT32_MAX

// Define virtual base and alternate virtual base of kernel.
#define KSEG0_BASE                  0x80000000

// Define virtual base addresses for physical memory windows.
#define MM_SYSTEM_PHYSICAL_MAP      KSEG0_BASE // = 0x80000000
#define KERNEL_SIZE sizeof(DUMMY_KERNEL)
#define MM_XBOX_HIGHEST_PHYSICAL_PAGE	0x03FFF
#define MM_CHIHIRO_HIGHEST_PHYSICAL_PAGE    0x07FFF
#define MM_64M_PHYSICAL_PAGE        0x04000
#define MM_XBOX_INSTANCE_PHYSICAL_PAGE   0x03FE0
#define MM_CHIHIRO_INSTANCE_PHYSICAL_PAGE   0x07FF0
#define MM_INSTANCE_PAGE_COUNT      16
#define CONTIGUOUS_MEMORY_BASE MM_SYSTEM_PHYSICAL_MAP // = 0x80000000
#define CONTIGUOUS_MEMORY_XBOX_SIZE (64 * ONE_MB)
#define CONTIGUOUS_MEMORY_CHIHIRO_SIZE (128 * ONE_MB)
#define TILED_MEMORY_BASE 0xF0000000 // Tiled memory is a mirror of contiguous memory, residing at 0xF0000000
#define TILED_MEMORY_XBOX_SIZE CONTIGUOUS_MEMORY_XBOX_SIZE
#define TILED_MEMORY_CHIHIRO_SIZE CONTIGUOUS_MEMORY_CHIHIRO_SIZE
#define NV2A_MEMORY_BASE 0xFD000000 // See NV2A_ADDR
#define NV2A_MEMORY_SIZE 0x01000000 // See NV2A_SIZE
#define NV2A_PRAMIN_ADDR 0xFD700000
#define NV2A_PRAMIN_SIZE 0x100000
#define NV2A_USER_ADDR 0xFD800000
#define NV2A_USER_SIZE 0x800000
#define APU_BASE 0xFE800000
#define APU_SIZE 0x80000
#define AC97_BASE 0xFEC00000
#define AC97_SIZE 0x1000
#define USB0_BASE 0xFED00000
#define USB0_SIZE 0x1000
#define USB1_BASE 0xFED08000
#define USB1_SIZE 0x1000
#define NVNet_BASE 0xFEF00000
#define NVNet_SIZE 0x400
#define BIOS_BASE 0xFF000000
#define BIOS_XBOX_SIZE 0xFFFE00
#define BIOS_CHIHIRO_SIZE 0x1000000
#define MCPX_BASE 0xFFFFFE00
#define MCPX_SIZE 0x200
#define MAX_VIRTUAL_ADDRESS 0xFFFFFFFF

/*! memory size per system */
#define XBOX_MEMORY_SIZE (64 * ONE_MB)
#define CHIHIRO_MEMORY_SIZE (128 * ONE_MB)
#define XBE_IMAGE_BASE 0x00010000

/*! base addresses of various components */
#define XBOX_KERNEL_BASE (MM_SYSTEM_PHYSICAL_MAP + XBE_IMAGE_BASE)

#define XBOX_WRITE_COMBINED_BASE 0xF0000000
#define XBOX_WRITE_COMBINED_SIZE 0x08000000 // - 0xF7FFFFF

#define XBOX_UNCACHED_BASE       0xF8000000
#define XBOX_UNCACHED_SIZE       0x07F00000 // - 0xFFEFFFFF

#define XBOX_NV2A_INIT_VECTOR    0xFF000008

#define XBOX_FLASH_ROM_BASE      0xFFF00000
#define XBOX_FLASH_ROM_SIZE      0x00100000 // - 0xFFFFFFF

#define HIGHEST_USER_ADDRESS     0x7FFEFFFF
#define HIGHEST_VAD_ADDRESS      HIGHEST_USER_ADDRESS - X64KB // for NtAllocateVirtualMemory


// For now, virtual addresses are somewhat limited, as we use
// these soley for loading XBE sections. The largest that we
// know of, is "BLiNX: the time sweeper", which has a section
// (called "$$XTIMAG") at 0x031C5260+0x00002800, which would
// fit in 51 MB. If we ever encounter an even larger XBE, this
// value will have to be increased likewise (maybe up to 64 MB
// for XBOX_MEMORY_SIZE or even 128 MB for CHIHIRO_MEMORY_SIZE).
#define XBE_MAX_VA	(64 * ONE_MB)

/*! base address of Cxbx host executable, see Cxbx project options, Linker, Advanced, Base Address */
#define CXBX_BASE_ADDR XBE_IMAGE_BASE
#define CXBX_BASE_OF_CODE 0x00001000

#define MAX_BUS_INTERRUPT_LEVEL 27
// MAX_BUS_INTERRUPT_LEVEL = PROFILE_LEVEL = 27
#define MAX_NUM_INTERRUPTS 256
#define IRQ_BASE 0x30 

#define PRIMARY_VECTOR_BASE IRQ_BASE

// Source : ReactOS halirq.h : https://doxygen.reactos.org/d1/da9/halppc_2include_2halirq_8h_source.html
#define IRQ2VECTOR(irq)     ((irq)+IRQ_BASE)
#define VECTOR2IRQ(vector)  ((vector)-IRQ_BASE)
#define VECTOR2IRQL(vector) (PROFILE_LEVEL - VECTOR2IRQ(vector))

void CxbxPopupMessage(const char *message, ...);

#define LOG_TEST_CASE(message) do { static bool bPopupShown = false; \
    if (!bPopupShown) { bPopupShown = true; \
    CxbxPopupMessage("Please report that %s shows this test-case: %s\nIn %s (%s line %d)", \
    CxbxKrnl_Xbe->m_szAsciiTitle, message, __func__, __FILE__, __LINE__); } } while(0)
// was g_pCertificate->wszTitleName

extern Xbe::Certificate *g_pCertificate;

/*! validate version string match */
bool CxbxKrnlVerifyVersion(const char *szVersion);

/*! Cxbx Kernel Entry Point */
void CxbxKrnlMain(int argc, char* argv[]);

/*! initialize emulation */
__declspec(noreturn) void CxbxKrnlInit(HWND hwndParent, void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, const char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)());

/*! cleanup emulation */
__declspec(noreturn) void CxbxKrnlCleanup(const char *szErrorMessage, ...);

/*! register a thread handle */
void CxbxKrnlRegisterThread(HANDLE hThread);

/*! suspend emulation */
void CxbxKrnlSuspend();

/*! resume emulation */
void CxbxKrnlResume();

/*! terminate the calling thread */
__declspec(noreturn) void CxbxKrnlTerminateThread();

/*! kernel panic (trap for unimplemented kernel functions) */
void CxbxKrnlPanic();

/*! empty function */
void CxbxKrnlNoFunc();

void CxbxInitPerformanceCounters(); // Implemented in EmuKrnlKe.cpp

void CxbxInitFilePaths();

void CxbxRestorePersistentMemoryRegions();

void ConnectWindowsTimersToThunkTable();

/*! kernel thunk table */
extern uint32 CxbxKrnl_KernelThunkTable[379];

extern bool g_IsWine;

void InitXboxThread(DWORD_PTR cores);

/*! thread local storage structure */
extern Xbe::TLS *CxbxKrnl_TLS;

/*! thread local storage data */
extern void *CxbxKrnl_TLSData;

/*! xbe header structure */
extern Xbe::Header *CxbxKrnl_XbeHeader;

extern Xbe *CxbxKrnl_Xbe;

/*! parent window handle */
extern HWND CxbxKrnl_hEmuParent;
extern DebugMode CxbxKrnl_DebugMode;
extern std::string CxbxKrnl_DebugFileName;

/*! file paths */
extern char szFilePath_CxbxReloaded_Exe[MAX_PATH];
extern char szFolder_CxbxReloadedData[MAX_PATH];
extern char szFilePath_EEPROM_bin[MAX_PATH];

#ifdef __cplusplus
}
#endif

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
extern std::string CxbxGetLastErrorString(char * lpszFunction);

#endif
