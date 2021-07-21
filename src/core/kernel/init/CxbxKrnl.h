// ******************************************************************
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
#include "common\AddressRanges.h"
#include "common/ReserveAddressRanges.h"
#include "common\xbe\Xbe.h"
#include "Logging.h"
#include <optional>

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
#define ONE_MB (1024 * ONE_KB)
#define X64KB (64 * ONE_KB)

#define XBADDR_BITS 32
#define XBADDR_MAX UINT32_MAX

// Define virtual base addresses used by the system.
#define XBOX_CONTIGUOUS_MEMORY_LIMIT        0x03FDF
#define XBOX_INSTANCE_PHYSICAL_PAGE         0x03FE0
#define XBOX_PFN_DATABASE_PHYSICAL_PAGE     0x03FF0
#define XBOX_HIGHEST_PHYSICAL_PAGE          0x03FFF

#define X64M_PHYSICAL_PAGE                  0x04000

#define CHIHIRO_CONTIGUOUS_MEMORY_LIMIT     0x07FCF
#define CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE  0x07FD0
#define CHIHIRO_INSTANCE_PHYSICAL_PAGE      0x07FF0
#define CHIHIRO_HIGHEST_PHYSICAL_PAGE       0x07FFF

#define PAGE_DIRECTORY_PHYSICAL_ADDRESS     0x0F000
#define D3D_PHYSICAL_PAGE                   0x00000
#define DEBUGKIT_FIRST_UPPER_HALF_PAGE      X64M_PHYSICAL_PAGE // = 0x4000

#define XBOX_PFN_ADDRESS                    ((XBOX_PFN_DATABASE_PHYSICAL_PAGE << PAGE_SHIFT) + (PCHAR)KSEG0_BASE)
#define CHIHIRO_PFN_ADDRESS                 ((CHIHIRO_PFN_DATABASE_PHYSICAL_PAGE << PAGE_SHIFT) + (PCHAR)KSEG0_BASE)

#define NV2A_INSTANCE_PAGE_COUNT            16

// For now, virtual addresses are somewhat limited, as we use
// these soley for loading XBE sections. The largest that we
// know of, is "BLiNX: the time sweeper", which has a section
// (called "$$XTIMAG") at 0x031C5260+0x00002800, which would
// fit in 51 MB. If we ever encounter an even larger XBE, this
// value will have to be increased likewise (maybe up to 64 MB
// for XBOX_MEMORY_SIZE or even 128 MB for CHIHIRO_MEMORY_SIZE).
#ifdef CXBXR_EMU
#define XBE_MAX_VA  (128 * ONE_MB)
#else
#define XBE_MAX_VA  (64 * ONE_MB)
#endif

/*! base address of Cxbx host executable, see Cxbx project options, Linker, Advanced, Base Address */
#define CXBX_BASE_ADDR     XBE_IMAGE_BASE
#define CXBX_BASE_OF_CODE  0x00001000

// NT_TIB (Thread Information Block) offsets - see https://www.microsoft.com/msj/archive/S2CE.aspx
#define TIB_ArbitraryDataSlot  0x14 // = 20 = offsetof(NT_TIB, ArbitraryUserPointer)
#define TIB_LinearSelfAddress  0x18 // = 24 = offsetof(NT_TIB, Self)

#define MAX_BUS_INTERRUPT_LEVEL 27
#define MAX_NUM_INTERRUPTS 256
#define IRQ_BASE 0x30 

#define PRIMARY_VECTOR_BASE IRQ_BASE

// Source : ReactOS halirq.h : https://doxygen.reactos.org/d1/da9/halppc_2include_2halirq_8h_source.html
#define IRQ2VECTOR(irq)     ((irq)+IRQ_BASE)
#define VECTOR2IRQ(vector)  ((vector)-IRQ_BASE)
#define VECTOR2IRQL(vector) (MAX_BUS_INTERRUPT_LEVEL - VECTOR2IRQ(vector))

/* Xbox PAGE Masks */
#define XBOX_PAGE_NOACCESS          0x01
#define XBOX_PAGE_READONLY          0x02
#define XBOX_PAGE_READWRITE         0x04
#define XBOX_PAGE_WRITECOPY         0x08 // ?
#define XBOX_PAGE_EXECUTE           0x10
#define XBOX_PAGE_EXECUTE_READ      0x20
#define XBOX_PAGE_EXECUTE_READWRITE 0x40
#define XBOX_PAGE_EXECUTE_WRITECOPY 0x80 // ?
#define XBOX_PAGE_GUARD             0x100
#define XBOX_PAGE_NOCACHE           0x200
#define XBOX_PAGE_WRITECOMBINE      0x400

/* Xbox MEM Masks */
#define XBOX_MEM_COMMIT             0x1000
#define XBOX_MEM_RESERVE            0x2000
#define XBOX_MEM_DECOMMIT           0x4000
#define XBOX_MEM_RELEASE            0x8000
#define XBOX_MEM_FREE               0x10000
#define XBOX_MEM_PRIVATE            0x20000
#define XBOX_MEM_MAPPED             0x40000 // ?
#define XBOX_MEM_RESET              0x80000
#define XBOX_MEM_TOP_DOWN           0x100000
#define XBOX_MEM_WRITE_WATCH        0x200000 // ?
#define XBOX_MEM_PHYSICAL           0x400000 // ?
#define XBOX_MEM_NOZERO             0x800000 // Replaces MEM_ROTATE on WinXP+
#define XBOX_MEM_IMAGE              0x1000000 // ?

extern Xbe::Certificate *g_pCertificate;

extern bool g_bIsDebugKernel;

bool CreateSettings();

bool HandleFirstLaunch();

/*! Cxbx Kernel Entry Point */
void CxbxKrnlEmulate(unsigned int system, blocks_reserved_t blocks_reserved);

/*! initialize emulation */
__declspec(noreturn) void CxbxKrnlInit(void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, const char *szDebugFilename, Xbe::Header *XbeHeader, uint32_t XbeHeaderSize, void (*Entry)(), int BootFlags);

/*! cleanup emulation */
__declspec(noreturn) void CxbxKrnlCleanupEx(CXBXR_MODULE cxbxr_module, const char *szErrorMessage, ...);

#define CxbxKrnlCleanup(fmt, ...) CxbxKrnlCleanupEx(LOG_PREFIX, fmt, ##__VA_ARGS__)

/*! register a thread handle */
void CxbxKrnlRegisterThread(HANDLE hThread);

/*! suspend emulation */
void CxbxKrnlSuspend();

/*! resume emulation */
void CxbxKrnlResume();

/*! terminate gracefully the emulation */
void CxbxKrnlShutDown(bool is_reboot = false);

/*! display the fatal error message*/
void CxbxKrnlPrintUEM(ULONG ErrorCode);

/*! display the cause of the fatal error message*/
void CxbxPrintUEMInfo(ULONG ErrorCode);

/*! terminate the calling thread */
__declspec(noreturn) void CxbxKrnlTerminateThread();

/*! kernel panic (trap for unimplemented kernel functions) */
void CxbxKrnlPanic();

/*! empty function */
void CxbxKrnlNoFunc();

void CxbxInitPerformanceCounters(); // Implemented in EmuKrnlKe.cpp

void CxbxInitFilePaths();

// For emulation usage only
bool CxbxLockFilePath();
void CxbxUnlockFilePath();

bool CxbxIsElevated();

/*! kernel thunk table */
extern uint32_t CxbxKrnl_KernelThunkTable[379];

extern bool g_bClipCursor;
extern bool g_CxbxPrintUEM;
extern ULONG g_CxbxFatalErrorCode;

extern size_t g_SystemMaxMemory;

void InitXboxThread();

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
extern char szFilePath_Xbe[xbox::max_path*2];

#ifdef __cplusplus
}
#endif

std::optional<std::string> CxbxExec(bool useDebugger, HANDLE *hProcess, bool requestHandleProcess);

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
extern std::string CxbxGetLastErrorString(char * lpszFunction);

// The reason of having EmuLogOutputEx in LOG_TEST_CASE is to allow dump to log directly for any test cases triggered.
// Which will make developers easier to note which applications has triggered quicker, easier, and doesn't require any individual log enabled to capture them.
// NOTE: This #define is here rather than Logging.h, because it has a dependency on CxbxKrnl_Xbe
#define LOG_TEST_CASE(message) do { \
	static bool bTestCaseLogged = false; \
	if (bTestCaseLogged) break; \
	bTestCaseLogged = true; \
	if (g_CurrentLogPopupTestCase) { \
		LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
			PopupInfo(nullptr, "Please report that %s shows the following message:\nLOG_TEST_CASE: %s\nIn %s (%s line %d)", \
			CxbxKrnl_Xbe->m_szAsciiTitle, message, __func__, __FILE__, __LINE__); \
			continue; \
		} \
	} \
	EmuLogOutputEx(LOG_PREFIX, LOG_LEVEL::INFO, "Please report that %s shows the following message:\nLOG_TEST_CASE: %s\nIn %s (%s line %d)", \
	CxbxKrnl_Xbe->m_szAsciiTitle, message, __func__, __FILE__, __LINE__); \
} while (0)
// was g_pCertificate->wszTitleName

#endif
