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
#include "common\xbe\Xbe.h"
#include "Logging.h"

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
#define ONE_MB (1024 * ONE_KB)
#define X64KB (64 * ONE_KB)

#define XBADDR_BITS 32
#define XBADDR_MAX UINT32_MAX

// Base addresses of various components
#define KSEG0_BASE                          0x80000000

#define PHYSICAL_MAP_BASE                   0x80000000
#define PHYSICAL_MAP_SIZE                   (256 * ONE_MB) // = 0x10000000
#define PHYSICAL_MAP_END                    (PHYSICAL_MAP_BASE + PHYSICAL_MAP_SIZE - 1) // 0x8FFFFFFF

#define CONTIGUOUS_MEMORY_BASE              KSEG0_BASE // = 0x80000000
#define XBOX_CONTIGUOUS_MEMORY_SIZE         (64 * ONE_MB)
#define CHIHIRO_CONTIGUOUS_MEMORY_SIZE      (128 * ONE_MB)

#define DEVKIT_MEMORY_BASE                  0xB0000000
#define DEVKIT_MEMORY_SIZE                  (256 * ONE_MB) // = 0x10000000
#define DEVKIT_MEMORY_END                   (DEVKIT_MEMORY_BASE + DEVKIT_MEMORY_SIZE - 1) // 0xBFFFFFFF

#define PAGE_TABLES_BASE                    0xC0000000
#define PAGE_TABLES_SIZE                    (4 * ONE_MB) // = 0x00400000
#define PAGE_TABLES_END                     (PAGE_TABLES_BASE + PAGE_TABLES_SIZE - 1)

#define SYSTEM_MEMORY_BASE                  0xD0000000
#define SYSTEM_MEMORY_SIZE                  (512 * ONE_MB) // = 0x20000000
#define SYSTEM_MEMORY_END                   (SYSTEM_MEMORY_BASE + SYSTEM_MEMORY_SIZE - 1) // 0xEFFFFFFF

#define XBOX_WRITE_COMBINED_BASE            0xF0000000 // WC (The WC memory is another name of the tiled memory)
#define XBOX_WRITE_COMBINED_SIZE            (128 * ONE_MB) // = 0x08000000
#define XBOX_WRITE_COMBINED_END             (XBOX_WRITE_COMBINED_BASE + XBOX_WRITE_COMBINED_SIZE - 1) // - 0xF7FFFFFF

#define XBOX_UNCACHED_BASE                  0xF8000000 // UC
#define XBOX_UNCACHED_SIZE                  ((128 - 4) * ONE_MB) // = 0x07C00000
#define XBOX_UNCACHED_END                   (XBOX_UNCACHED_BASE + XBOX_UNCACHED_SIZE - 1) // - 0xFFBFFFFF

#define XBOX_FLASH_ROM_BASE                 0xFFF00000
#define XBOX_FLASH_ROM_SIZE                 (1 * ONE_MB) // = 0x00100000
#define XBOX_FLASH_ROM_END                  (XBOX_FLASH_ROM_BASE + XBOX_FLASH_ROM_SIZE - 1) // - 0xFFFFFFF

// Miscellaneous base addresses
#define XBE_IMAGE_BASE                      0x00010000
#define PAGE_DIRECTORY_BASE                 0xC0300000
#define NV2A_INIT_VECTOR                    0xFF000008

// Define virtual base and alternate virtual base of kernel
#define XBOX_KERNEL_BASE                    (PHYSICAL_MAP_BASE + XBE_IMAGE_BASE)
#define KERNEL_PHYSICAL_ADDRESS             XBE_IMAGE_BASE // = 0x10000
#define KERNEL_SIZE                         sizeof(DUMMY_KERNEL)
#define KERNEL_STACK_SIZE                   12288 // 0x03000, needed by PsCreateSystemThreadEx, however the current implementation doesn't use it

// Miscellaneous memory variables
// Xbox pages are (1 << 12) = 0x00001000 = 4096 bytes in size. Large pages are 4 MiB instead
// NOTE: PAGE_SIZE is also defined in xfile.h (oxdk) and linux_wrapper.h (oxdk)
#define PAGE_SHIFT                          12 // 2^12 = 4 KiB
#define PAGE_SIZE                           (1 << PAGE_SHIFT)
#define PAGE_MASK                           (PAGE_SIZE - 1)

#define LARGE_PAGE_SHIFT                    22 // 2^22 = 4 MiB
#define LARGE_PAGE_SIZE                     (1 << LARGE_PAGE_SHIFT) // = 0x00400000 = 4 MiB
#define LARGE_PAGE_MASK                     (LARGE_PAGE_SIZE - 1)

#define BYTES_IN_PHYSICAL_MAP               (256 * ONE_MB) // this refers to the system RAM physical window 0x80000000 - 0x8FFFFFFF
#define MAXIMUM_ZERO_BITS                   21 // for XbAllocateVirtualMemory
#define MAX_VIRTUAL_ADDRESS                 0xFFFFFFFF

#define LOWEST_USER_ADDRESS                 XBE_IMAGE_BASE // = 0x00010000
#define HIGHEST_USER_ADDRESS                0x7FFEFFFF
#define HIGHEST_VMA_ADDRESS                 (HIGHEST_USER_ADDRESS - X64KB) // for NtAllocateVirtualMemory
#define USER_MEMORY_SIZE                    (HIGHEST_USER_ADDRESS - LOWEST_USER_ADDRESS + 1) // 0x7FFE0000 = 2 GiB - 128 KiB

// Memory size per system
#define XBOX_MEMORY_SIZE                    (64 * ONE_MB)
#define CHIHIRO_MEMORY_SIZE                 (128 * ONE_MB)

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
#define XBE_MAX_VA  (64 * ONE_MB)

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

typedef enum _CxbxMsgDlgIcon {
    CxbxMsgDlgIcon_Info=0,
    CxbxMsgDlgIcon_Warn,
    CxbxMsgDlgIcon_Error,
    CxbxMsgDlgIcon_Unknown
} CxbxMsgDlgIcon;

void CxbxPopupMessageEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, CxbxMsgDlgIcon icon, const char *message, ...);

#define CxbxPopupMessage(level, icon, fmt, ...) CxbxPopupMessageEx(LOG_PREFIX, level, icon, fmt, ##__VA_ARGS__)

#define LOG_TEST_CASE(message) do { static bool bTestCaseLogged = false; \
    if (!bTestCaseLogged) { bTestCaseLogged = true; \
	LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
		CxbxPopupMessage(LOG_LEVEL::INFO, CxbxMsgDlgIcon_Info, "Please report that %s shows the following message:\nLOG_TEST_CASE: %s\nIn %s (%s line %d)", \
		CxbxKrnl_Xbe->m_szAsciiTitle, message, __func__, __FILE__, __LINE__); } } } while (0)
// was g_pCertificate->wszTitleName

extern Xbe::Certificate *g_pCertificate;

/*! validate version string match */
bool CxbxKrnlVerifyVersion(const char *szVersion);

extern bool g_bIsDebugKernel;

bool CreateSettings();

bool HandleFirstLaunch();

/*! Cxbx Kernel Entry Point */
void CxbxKrnlMain(int argc, char* argv[]);

/*! initialize emulation */
__declspec(noreturn) void CxbxKrnlInit(void *pTLSData, Xbe::TLS *pTLS, Xbe::LibraryVersion *LibraryVersion, DebugMode DbgMode, const char *szDebugFilename, Xbe::Header *XbeHeader, uint32 XbeHeaderSize, void (*Entry)(), int BootFlags);

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
void CxbxKrnlShutDown();

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

/*! Generate a standard arg format string */
void CxbxConvertArgToString(std::string &dest, const char* krnlExe, const char* xbeFile, HWND hwndParent, DebugMode krnlDebug, const char* krnlDebugFile);

bool CxbxExec(std::string &execCommand, HANDLE* hProcess, bool requestHandleProcess);

bool CxbxIsElevated();

/*! kernel thunk table */
extern uint32 CxbxKrnl_KernelThunkTable[379];

extern bool g_bIsWine;

extern bool g_CxbxPrintUEM;
extern ULONG g_CxbxFatalErrorCode;

extern size_t g_SystemMaxMemory;

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
extern char szFilePath_Xbe[MAX_PATH*2];

#ifdef __cplusplus
}
#endif

// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
extern std::string CxbxGetLastErrorString(char * lpszFunction);

#endif
