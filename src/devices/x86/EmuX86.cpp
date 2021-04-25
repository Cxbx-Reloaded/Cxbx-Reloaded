// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2016-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2016-2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *  All rights reserved 
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::X86

// Link the library into our project.
#pragma comment(lib, "distorm.lib")

// Cxbx uses dynamic linking of distorm, which by default chooses for 64 bits offsets :
#define SUPPORT_64BIT_OFFSET

#include "distorm.h"
#include "mnemonics.h"

#include "core\kernel\init\CxbxKrnl.h"
#include "core\kernel\support\Emu.h" // For EmuLog
#include "devices\x86\EmuX86.h"
#include "core\hle\Intercept.hpp" // for bLLE_GPU

#include <assert.h>
#include "devices\Xbox.h" // For g_PCIBus
#include <atomic>
#include <map>
#include "Logging.h"

extern uint32_t GetAPUTime();
extern std::atomic_bool g_bEnableAllInterrupts;

//
// Read & write handlers handlers for I/O
//

static int field_pin = 0;

static thread_local bool g_tls_isEmuX86Managed;

uint32_t EmuX86_IORead(xbox::addr_xt addr, int size)
{
	// If we are running a Chihiro game, emulate the Chihiro LPC device
	if (g_bIsChihiro) {
		if (addr >= 0x4000 && addr <= 0x40FF) {
			return g_MediaBoard->LpcRead(addr, size);
		}
	}

	switch (addr) {
	case 0x8008: { // TODO : Move 0x8008 TIMER to a device
		if (size == sizeof(uint32_t)) {
			// HACK: This is very wrong.
			// This timer should count at a specific frequency (3579.545 ticks per ms)
			// But this is enough to keep NXDK from hanging for now.
			LARGE_INTEGER performanceCount;
			QueryPerformanceCounter(&performanceCount);
			return static_cast<uint32_t>(performanceCount.QuadPart);
		}
		break;
	}
	case 0x80C0: { // TODO : Move 0x80C0 TV encoder to a device
		if (size == sizeof(uint8_t)) {
			// field pin from tv encoder?
			field_pin = (field_pin + 1) & 1;
			return field_pin << 5;
		}
		break;
	}
	}

	// Pass the IO Read to the PCI Bus, this will handle devices with BARs set to IO addresses
	uint32_t value = 0;
	if (g_PCIBus->IORead(addr, &value, size)) {
		return value;
	}

	EmuLog(LOG_LEVEL::WARNING, "EmuX86_IORead(0x%08X, %d) [Unhandled]", addr, size);
	return 0;
}

void EmuX86_IOWrite(xbox::addr_xt addr, uint32_t value, int size)
{
	// If we are running a Chihiro game, emulate the Chihiro LPC device
	if (g_bIsChihiro) {
		if (addr >= 0x4000 && addr <= 0x40FF) {
			g_MediaBoard->LpcWrite(addr, value, size);
			return;
		}
	}

	// Pass the IO Write to the PCI Bus, this will handle devices with BARs set to IO addresses
	if (g_PCIBus->IOWrite(addr, value, size)) {
		return;
	}

	EmuLog(LOG_LEVEL::WARNING, "EmuX86_IOWrite(0x%08X, 0x%04X, %d) [Unhandled]", addr, value, size);
}

//
// Read & write handlers for pass-through access to (host committed, virtual) xbox memory
//
// Only allowed to be called outside our EmuException exception handler,
// to prevent recursive exceptions when accessing unallocated memory.
//

uint32_t EmuX86_Mem_Read(xbox::addr_xt addr, int size)
{
	switch (size) {
	case sizeof(uint32_t) :
		return *(uint32_t*)addr;
	case sizeof(uint16_t) :
		return *(uint16_t*)addr;
	case sizeof(uint8_t) :
		return *(uint8_t*)addr;
	default:
		// UNREACHABLE(size);
		assert(false);
		return 0;
	}
}

void EmuX86_Mem_Write(xbox::addr_xt addr, uint32_t value, int size)
{
	switch (size) {
	case sizeof(uint32_t) :
		*(uint32_t*)addr = (uint32_t)value;
		break;
	case sizeof(uint16_t) :
		*(uint16_t*)addr = (uint16_t)value;
		break;
	case sizeof(uint8_t) :
		*(uint8_t*)addr = (uint8_t)value;
		break;
	default:
		// UNREACHABLE(size);
		assert(false);
		return;
	}
}

uint32_t EmuFlash_Read32(xbox::addr_xt addr) // TODO : Move to EmuFlash.cpp
{
	uint32_t r;

    // Some games attempt to read from the BIOS ROM directly, for purposes of hardware-version detection
    // And other (currently unknown) reasons. We can't include the entire bios, and we don't want to require it
    // So we specifiy the specific values that games rely on.

	switch (addr) {
    case 0x08: // ? Test Case - Shenmue II attempts to read this address but never uses the resulting value? 
        r = 0x2B16D065;
        break;
	case 0x78: // ROM_VERSION
		r = 0x90; // Luke's hardware revision 1.6 Xbox returns this (also since XboxKrnlVersion is set to 5838)
		break;
	default:
		EmuLog(LOG_LEVEL::WARNING, "Read32 FLASH_ROM (0x%.8X) [Unknown address]", addr);
		return -1;
	}

	EmuLog(LOG_LEVEL::DEBUG, "Read32 FLASH_ROM (0x%.8X) = 0x%.8X [HANDLED]", addr, r);
	return r;
}

//
// Read & write handlers for memory-mapped hardware devices
//

uint32_t EmuX86_Read(xbox::addr_xt addr, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuLog(LOG_LEVEL::WARNING, "EmuX86_Read(0x%08X, %d) [Unaligned unimplemented]", addr, size);
		// LOG_UNIMPLEMENTED();
		return 0;
	}

	uint32_t value;

	if (addr >= FLASH_DEVICE1_BASE) { // 0xFF000000 - 0xFFFFFFF
		return EmuFlash_Read32((addr - FLASH_DEVICE1_BASE) % KiB(256)); // NOTE: Bios is a 256kb rom, mirrored through the address space
	}

	// TODO: Remove this once we have an LLE APU Device
	if(addr == 0xFE80200C) {
		return GetAPUTime();
	}

	// Pass the Read to the PCI Bus, this will handle devices with BARs set to MMIO addresses
	if (g_PCIBus->MMIORead(addr, &value, size)) {
		return value;
	}

	// EmuX86 is not suppose to do direct read to host memory and should be handle from
	// redirect from above statements. If it doesn't meet any requirement, then should be
	// handle as possible fatal crash instead of return corrupt value.
	g_tls_isEmuX86Managed = false;

	return 0;
}

void EmuX86_Write(xbox::addr_xt addr, uint32_t value, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuLog(LOG_LEVEL::WARNING, "EmuX86_Write(0x%08X, 0x%08X, %d) [Unaligned unimplemented]", addr, value, size);
		// LOG_UNIMPLEMENTED();
		return;
	}

	if (addr >= FLASH_DEVICE1_BASE) { // 0xFF000000 - 0xFFFFFFF
		EmuLog(LOG_LEVEL::WARNING, "EmuX86_Write(0x%08X, 0x%08X) [FLASH_ROM]", addr, value);
		return;
	}

	// Pass the Write to the PCI Bus, this will handle devices with BARs set to MMIO addresses
	if (g_PCIBus->MMIOWrite(addr, value, size)) {
		return;
	}

	// EmuX86 is not suppose to do direct write to host memory and should be handle from
	// redirect from above statements. If it doesn't meet any requirement, then should be
	// handle as possible fatal crash instead of set corrupt value.
	g_tls_isEmuX86Managed = false;
}

int ContextRecordOffsetByRegisterType[/*_RegisterType*/R_DR7 + 1] = { 0 };

// Populate ContextRecordOffsetByRegisterType for each distorm::_RegisterType
// supported by the XBox1's Coppermine Pentium III.
// Based on https://maximumcrack.wordpress.com/2011/08/07/fpu-mmx-xmm-and-bbq/
void EmuX86_InitContextRecordOffsetByRegisterType()
{
	// Unsupported by XBox CPU : R_RAX, R_RCX, R_RDX, R_RBX, R_RSP, R_RBP, R_RSI, R_RDI, R_R8, R_R9, R_R10, R_R11, R_R12, R_R13, R_R14, R_R15,
	ContextRecordOffsetByRegisterType[R_EAX] = offsetof(CONTEXT, Eax);
	ContextRecordOffsetByRegisterType[R_ECX] = offsetof(CONTEXT, Ecx);
	ContextRecordOffsetByRegisterType[R_EDX] = offsetof(CONTEXT, Edx);
	ContextRecordOffsetByRegisterType[R_EBX] = offsetof(CONTEXT, Ebx);
	ContextRecordOffsetByRegisterType[R_ESP] = offsetof(CONTEXT, Esp);
	ContextRecordOffsetByRegisterType[R_EBP] = offsetof(CONTEXT, Ebp);
	ContextRecordOffsetByRegisterType[R_ESI] = offsetof(CONTEXT, Esi);
	ContextRecordOffsetByRegisterType[R_EDI] = offsetof(CONTEXT, Edi);
	// Unsupported by XBox CPU : R_R8D, R_R9D, R_R10D, R_R11D, R_R12D, R_R13D, R_R14D, R_R15D,
	ContextRecordOffsetByRegisterType[R_AX] = offsetof(CONTEXT, Eax);
	ContextRecordOffsetByRegisterType[R_CX] = offsetof(CONTEXT, Ecx);
	ContextRecordOffsetByRegisterType[R_DX] = offsetof(CONTEXT, Edx);
	ContextRecordOffsetByRegisterType[R_BX] = offsetof(CONTEXT, Ebx);
	ContextRecordOffsetByRegisterType[R_SP] = offsetof(CONTEXT, Esp); // ??
	ContextRecordOffsetByRegisterType[R_BP] = offsetof(CONTEXT, Ebp); // ??
	ContextRecordOffsetByRegisterType[R_SI] = offsetof(CONTEXT, Esi); // ??
	ContextRecordOffsetByRegisterType[R_DI] = offsetof(CONTEXT, Edi); // ??
	// Unsupported by XBox CPU : R_R8W, R_R9W, R_R10W, R_R11W, R_R12W, R_R13W, R_R14W, R_R15W,
	ContextRecordOffsetByRegisterType[R_AL] = offsetof(CONTEXT, Eax);
	ContextRecordOffsetByRegisterType[R_CL] = offsetof(CONTEXT, Ecx);
	ContextRecordOffsetByRegisterType[R_DL] = offsetof(CONTEXT, Edx);
	ContextRecordOffsetByRegisterType[R_BL] = offsetof(CONTEXT, Ebx);
	ContextRecordOffsetByRegisterType[R_AH] = offsetof(CONTEXT, Eax) + 1;
	ContextRecordOffsetByRegisterType[R_CH] = offsetof(CONTEXT, Ecx) + 1;
	ContextRecordOffsetByRegisterType[R_DH] = offsetof(CONTEXT, Edx) + 1;
	ContextRecordOffsetByRegisterType[R_BH] = offsetof(CONTEXT, Ebx) + 1;
	// Unsupported by XBox CPU : R_R8B, R_R9B, R_R10B, R_R11B, R_R12B, R_R13B, R_R14B, R_R15B,
	ContextRecordOffsetByRegisterType[R_SPL] = offsetof(CONTEXT, Esp); // ??
	ContextRecordOffsetByRegisterType[R_BPL] = offsetof(CONTEXT, Ebp); // ??
	ContextRecordOffsetByRegisterType[R_SIL] = offsetof(CONTEXT, Esi); // ??
	ContextRecordOffsetByRegisterType[R_DIL] = offsetof(CONTEXT, Edi); // ??
	ContextRecordOffsetByRegisterType[R_ES] = offsetof(CONTEXT, SegEs);
	ContextRecordOffsetByRegisterType[R_CS] = offsetof(CONTEXT, SegCs);
	ContextRecordOffsetByRegisterType[R_SS] = offsetof(CONTEXT, SegSs);
	ContextRecordOffsetByRegisterType[R_DS] = offsetof(CONTEXT, SegDs);
	ContextRecordOffsetByRegisterType[R_FS] = offsetof(CONTEXT, SegFs);
	ContextRecordOffsetByRegisterType[R_GS] = offsetof(CONTEXT, SegGs);
	// R_RIP, TODO : Does this also mean EIP and is that enum missing in distorm? https://github.com/gdabah/distorm/issues/110
	// Unsupported by XBox CPU : R_ST0, R_ST1, R_ST2, R_ST3, R_ST4, R_ST5, R_ST6, R_ST7,
	ContextRecordOffsetByRegisterType[R_MM0] = offsetof(CONTEXT, ExtendedRegisters[(10 + 0) * 16]);
	ContextRecordOffsetByRegisterType[R_MM1] = offsetof(CONTEXT, ExtendedRegisters[(10 + 1) * 16]);
	ContextRecordOffsetByRegisterType[R_MM2] = offsetof(CONTEXT, ExtendedRegisters[(10 + 2) * 16]);
	ContextRecordOffsetByRegisterType[R_MM3] = offsetof(CONTEXT, ExtendedRegisters[(10 + 3) * 16]);
	ContextRecordOffsetByRegisterType[R_MM4] = offsetof(CONTEXT, ExtendedRegisters[(10 + 4) * 16]);
	ContextRecordOffsetByRegisterType[R_MM5] = offsetof(CONTEXT, ExtendedRegisters[(10 + 5) * 16]);
	ContextRecordOffsetByRegisterType[R_MM6] = offsetof(CONTEXT, ExtendedRegisters[(10 + 6) * 16]);
	ContextRecordOffsetByRegisterType[R_MM7] = offsetof(CONTEXT, ExtendedRegisters[(10 + 7) * 16]);
	// Unsupported by XBox CPU : R_XMM0, R_XMM1, R_XMM2, R_XMM3, R_XMM4, R_XMM5, R_XMM6, R_XMM7, R_XMM8, R_XMM9, R_XMM10, R_XMM11, R_XMM12, R_XMM13, R_XMM14, R_XMM15,
	// Unsupported by XBox CPU : R_YMM0, R_YMM1, R_YMM2, R_YMM3, R_YMM4, R_YMM5, R_YMM6, R_YMM7, R_YMM8, R_YMM9, R_YMM10, R_YMM11, R_YMM12, R_YMM13, R_YMM14, R_YMM15,
	// Unsupported by XBox CPU : R_UNUSED0, R_CR0, R_CR2, R_CR3, R_CR4, R_UNUSED1, R_UNUSED2, R_UNUSED3, R_CR8,
	ContextRecordOffsetByRegisterType[R_DR0] = offsetof(CONTEXT, Dr0);
	ContextRecordOffsetByRegisterType[R_DR1] = offsetof(CONTEXT, Dr1);
	ContextRecordOffsetByRegisterType[R_DR2] = offsetof(CONTEXT, Dr2);
	ContextRecordOffsetByRegisterType[R_DR3] = offsetof(CONTEXT, Dr3);
	// Unsupported by XBox CPU : R_UNUSED4, R_UNUSED5,
	ContextRecordOffsetByRegisterType[R_DR6] = offsetof(CONTEXT, Dr6);
	ContextRecordOffsetByRegisterType[R_DR7] = offsetof(CONTEXT, Dr7);

	/* struct CONTEXT { // ! markers below, are used in the above offsetof calls
	DWORD ContextFlags;
    !DWORD   Dr0;
    !DWORD   Dr1;
    !DWORD   Dr2;
    !DWORD   Dr3;
    !DWORD   Dr6;
    !DWORD   Dr7;
	struct _FLOATING_SAVE_AREA {
		DWORD   ControlWord;
		DWORD   StatusWord;
		DWORD   TagWord;
		DWORD   ErrorOffset;
		DWORD   ErrorSelector;
		DWORD   DataOffset;
		DWORD   DataSelector;
		BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
		DWORD   Spare0;
		} FLOATING_SAVE_AREA FloatSave;
    !DWORD   SegGs;
    !DWORD   SegFs;
    !DWORD   SegEs;
    !DWORD   SegDs;
    !DWORD   Edi;
    !DWORD   Esi;
    !DWORD   Ebx;
    !DWORD   Edx;
    !DWORD   Ecx;
    !DWORD   Eax;
	!DWORD   Ebp;
    DWORD   Eip;
    !DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    !DWORD   Esp;
    !DWORD   SegSs;
	!BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];*/
}

std::map<uint8_t, uint32_t> g_MemoryBackedRegisters;

void EmuX86_InitMemoryBackedRegisters()
{
	g_MemoryBackedRegisters[R_CR0] = 0;
	g_MemoryBackedRegisters[R_CR2] = 0;
	g_MemoryBackedRegisters[R_CR3] = 0;
	g_MemoryBackedRegisters[R_CR4] = 0;
}

inline void * EmuX86_GetRegisterPointer(const LPEXCEPTION_POINTERS e, const uint8_t reg)
{
	int offset = ContextRecordOffsetByRegisterType[reg];
	if (offset > 0) {
		return (void*)((uintptr_t)(e->ContextRecord) + offset);
	}

	// Xbox titles use some registers thatare not accessible in user mode
	// They are also not available in the Context/Exception Record, so we can't map them
	// Instead, we store the values in a key->value map.
	auto it = g_MemoryBackedRegisters.find(reg);
	if (it != g_MemoryBackedRegisters.end()) {
		return &it->second;
	}

	assert(false); 
	return nullptr;
}

int EmuX86_DistormRegSize(const uint8_t reg)
{
	switch (reg) {
		case R_RAX: return 8; case R_RCX: return 8; case R_RDX: return 8; case R_RBX: return 8; case R_RSP: return 8; case R_RBP: return 8; case R_RSI: return 8; case R_RDI: return 8; case R_R8: return 8; case R_R9: return 8; case R_R10: return 8; case R_R11: return 8; case R_R12: return 8; case R_R13: return 8; case R_R14: return 8; case R_R15: return 8;
		case R_EAX: return 4; case R_ECX: return 4; case R_EDX: return 4; case R_EBX: return 4; case R_ESP: return 4; case R_EBP: return 4; case R_ESI: return 4; case R_EDI: return 4; case R_R8D: return 4; case R_R9D: return 4; case R_R10D: return 4; case R_R11D: return 4; case R_R12D: return 4; case R_R13D: return 4; case R_R14D: return 4; case R_R15D: return 4;
		case R_AX: return 2; case R_CX: return 2; case R_DX: return 2; case R_BX: return 2; case R_SP: return 2; case R_BP: return 2; case R_SI: return 2; case R_DI: return 2; case R_R8W: return 2; case R_R9W: return 2; case R_R10W: return 2; case R_R11W: return 2; case R_R12W: return 2; case R_R13W: return 2; case R_R14W: return 2; case R_R15W: return 2;
		case R_AL: return 1; case R_CL: return 1; case R_DL: return 1; case R_BL: return 1; case R_AH: return 1; case R_CH: return 1; case R_DH: return 1; case R_BH: return 1; case R_R8B: return 1; case R_R9B: return 1; case R_R10B: return 1; case R_R11B: return 1; case R_R12B: return 1; case R_R13B: return 1; case R_R14B: return 1; case R_R15B: return 1;
		case R_SPL: return 1; case R_BPL: return 1; case R_SIL: return 1; case R_DIL: return 1;
		case R_ES: return 2; case R_CS: return 2; case R_SS: return 2; case R_DS: return 2; case R_FS: return 2; case R_GS: return 2;
		case R_RIP: return 8;
		case R_ST0: return 10; case R_ST1: return 10; case R_ST2: return 10; case R_ST3: return 10; case R_ST4: return 10; case R_ST5: return 10; case R_ST6: return 10; case R_ST7: return 10;
		case R_MM0: return 10; case R_MM1: return 10; case R_MM2: return 10; case R_MM3: return 10; case R_MM4: return 10; case R_MM5: return 10; case R_MM6: return 10; case R_MM7: return 10;
		case R_XMM0: return 16; case R_XMM1: return 16; case R_XMM2: return 16; case R_XMM3: return 16; case R_XMM4: return 16; case R_XMM5: return 16; case R_XMM6: return 16; case R_XMM7: return 16; case R_XMM8: return 16; case R_XMM9: return 16; case R_XMM10: return 16; case R_XMM11: return 16; case R_XMM12: return 16; case R_XMM13: return 16; case R_XMM14: return 16; case R_XMM15: return 16;
		case R_YMM0: return 16; case R_YMM1: return 16; case R_YMM2: return 16; case R_YMM3: return 16; case R_YMM4: return 16; case R_YMM5: return 16; case R_YMM6: return 16; case R_YMM7: return 16; case R_YMM8: return 16; case R_YMM9: return 16; case R_YMM10: return 16; case R_YMM11: return 16; case R_YMM12: return 16; case R_YMM13: return 16; case R_YMM14: return 16; case R_YMM15: return 16;
		case R_CR0: return 4; case R_UNUSED0: return 0; case R_CR2: return 4; case R_CR3: return 4; case R_CR4: return 4; case R_UNUSED1: return 0; case R_UNUSED2: return 0; case R_UNUSED3: return 0; case R_CR8: return 4;
		case R_DR0: return 4; case R_DR1: return 4; case R_DR2: return 4; case R_DR3: return 4; case R_UNUSED4: return 0; case R_UNUSED5: return 0; case R_DR6: return 4; case R_DR7: return 4;
		default: assert(false); return 0;
	}
}

inline uint32_t EmuX86_GetRegisterValue32(const LPEXCEPTION_POINTERS e, const uint8_t reg)
{
	if (reg != R_NONE) {
		assert(EmuX86_DistormRegSize(reg) == sizeof(uint32_t));

		void* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr != nullptr)
			return *(uint32_t *)regptr;

		assert(false);
	}

	return 0;
}

uint32_t EmuX86_Distorm_read_disp(const _DInst& info)
{
#ifdef _DEBUG
	// Assert disp is set according to expectations :
	switch (info.dispSize) {
	case 0:
		assert(info.disp == 0); // Assume dispSize==0 implies disp==0
		break;
	case 8:
		if (info.disp & 0x80) {
			assert((info.disp & 0xFFFFFF80) == 0xFFFFFF80); // Assume 8 bit disp is sign-extended to 32 bit
		}
		else {
			assert(info.disp < 0x80); // Assume positive signed 8 bit never reaches 0x80
		}
		break;
	case 16:
		if (info.disp & 0x8000) {
			assert((info.disp & 0xFFFF8000) == 0xFFFF8000); // Assume 16 bit disp is sign-extended to 32 bit
		}
		else {
			assert(info.disp < 0x8000); // Assume positive signed 16 bit never reaches 0x8000
		}
		break;
	case 32:
		break; // No checks for 32 bit
	default:
		assert(false); // Assume dispSize is either 0, 8, 16 or 32
		break;
	}
#endif
	return (uint32_t)info.disp;
}

typedef struct _OperandAddress {
	xbox::addr_xt addr = 0;
	bool is_internal_addr = false; // If set, addr points to a CPU context (or Distorm immedate value) member (instead of Xbox memory)
	int size = 0; // Expressed in bytes, not bits!
} OperandAddress;

bool EmuX86_Operand_Addr_ForReadOnly(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand, OperandAddress &opAddr)
{
	opAddr.size = info.ops[operand].size / 8; // Convert size in bits into bytes
	switch (info.ops[operand].type) {
	case O_NONE: // operand is to be ignored.
	{
		assert(opAddr.size == 0);

		// Ignore O_NONE operand
		return false;
	}
	case O_REG: // index holds global register index.
	{
		assert(opAddr.size == EmuX86_DistormRegSize(info.ops[operand].index));

		opAddr.is_internal_addr = true;
		opAddr.addr = (xbox::addr_xt)EmuX86_GetRegisterPointer(e, info.ops[operand].index);
		return true;
	}
	case O_IMM: // instruction.imm.
	{
		assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t));

		opAddr.is_internal_addr = true;
		opAddr.addr = (xbox::addr_xt)(&info.imm);
		return true;
	}
	case O_IMM1: // instruction.imm.ex.i1.
	{
		assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t));

		opAddr.is_internal_addr = true;
		opAddr.addr = (xbox::addr_xt)(&info.imm.ex.i1);
		return true;
	}
	case O_IMM2: // instruction.imm.ex.i2.
	{
		assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t));

		opAddr.is_internal_addr = true;
		opAddr.addr = (xbox::addr_xt)(&info.imm.ex.i2);
		return true;
	}
	case O_DISP: // memory dereference with displacement only, instruction.disp.
	{
		// Disabled as software is expected to hit this situaton, eg:
		// TEST byte ptr DS:[FEF00098h],1
		//assert(opAddr.size == sizeof(uint32_t));

		opAddr.is_internal_addr = false;
		opAddr.addr = EmuX86_Distorm_read_disp(info);
		return true;
	}
	case O_SMEM: // simple memory dereference with optional displacement(a single register memory dereference).
	{
		assert(opAddr.size == 0 || opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t)); // TODO : How to handle size == 0 further on?

		opAddr.is_internal_addr = false;
		opAddr.addr = EmuX86_GetRegisterValue32(e, info.ops[operand].index)
					+ EmuX86_Distorm_read_disp(info);
		return true;
	}
	case O_MEM: // complex memory dereference(optional fields : s / i / b / disp).
	{
		assert(opAddr.size == 0 || opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t)); // TODO : How to handle size == 0 further on?
		assert(info.scale < 3 || info.scale == 4 || info.scale == 8); // Assume scale is either 0, 1, 2, 4 or 8

		uint32_t index = EmuX86_GetRegisterValue32(e, info.ops[operand].index);
		if (info.scale >= 2)
			index *= info.scale;

		opAddr.is_internal_addr = false;
		opAddr.addr = EmuX86_GetRegisterValue32(e, info.base) // Note : Returns 0 when base == R_NONE
					+ index
					+ EmuX86_Distorm_read_disp(info);

		return true;
	}
	case O_PC: // the relative address of a branch instruction(instruction.imm.addr).
	{
		assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint32_t));

		opAddr.is_internal_addr = false;
		opAddr.addr = (xbox::addr_xt)e->ContextRecord->Eip + (xbox::addr_xt)INSTRUCTION_GET_TARGET(&info);
		return true;
	}
	case O_PTR: // the absolute target address of a far branch instruction(instruction.imm.ptr.seg / off).
	{
		assert(opAddr.size == 0);

		opAddr.is_internal_addr = false;
		opAddr.addr = (xbox::addr_xt)info.imm.ptr.off; // TODO : Needs test-case. What about info.imm.ptr.seg ?
		return true;
	}
	default:
		// UNREACHABLE(info.ops[operand].type);
		assert(false);
		return false;
	}

	assert(false);
	return false;
}

bool EmuX86_Operand_Addr_ForReadWrite(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand, OperandAddress &opAddr)
{
	// Disallow write-access to immediate value adresses
	switch (info.ops[operand].type) {
	case O_IMM:
	case O_IMM1:
	case O_IMM2:
		EmuLog(LOG_LEVEL::WARNING, "Refused operand write-access to immedate value address!");
		assert(false);
		return false;
	}

	// Except for the above restriction, writes may access the same operand addresses as reads :
	return EmuX86_Operand_Addr_ForReadOnly(e, info, operand, OUT opAddr);
}

uint32_t EmuX86_Addr_Read(const OperandAddress &opAddr)
{
	assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t));

	if (opAddr.is_internal_addr) {
		return EmuX86_Mem_Read(opAddr.addr, opAddr.size);
	}
	else {
		return EmuX86_Read(opAddr.addr, opAddr.size);
	}
}

void EmuX86_Addr_Write(const OperandAddress &opAddr, const uint32_t value)
{
	assert(opAddr.size == sizeof(uint8_t) || opAddr.size == sizeof(uint16_t) || opAddr.size == sizeof(uint32_t));

	if (opAddr.is_internal_addr) {
		EmuX86_Mem_Write(opAddr.addr, value, opAddr.size);
	}
	else {
		EmuX86_Write(opAddr.addr, value, opAddr.size);
	}
}

bool EmuX86_Operand_Read(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand, OUT uint32_t *value)
{
	OperandAddress opAddr;
	if (EmuX86_Operand_Addr_ForReadOnly(e, info, operand, OUT opAddr)) {
		*value = EmuX86_Addr_Read(opAddr);
		return true;
	}

	return false;
}

bool EmuX86_Operand_Write(LPEXCEPTION_POINTERS e, _DInst& info, int operand, uint32_t value)
{
	OperandAddress opAddr;
	if (EmuX86_Operand_Addr_ForReadWrite(e, info, operand, OUT opAddr)) {
		EmuX86_Addr_Write(opAddr, value);
		return true;
	}

	return false;
}

inline void EmuX86_SetFlags(LPEXCEPTION_POINTERS e, DWORD mask, DWORD value)
{
	// By Ron Jeffery, see https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
	e->ContextRecord->EFlags ^= ((e->ContextRecord->EFlags ^ value) & mask);
}

#define BIT(flag, bit) ((static_cast<uint32_t>((bool)bit)) << (flag))
#define BITMASK(flag) BIT(flag, 1)

// TODO : Verify callers compile bool arguments into bit-operations;
// If not, change these arguments back to unsigned int's and use only
// the least significant bit.
inline void EmuX86_SetFlags_OSZAPC
(
	LPEXCEPTION_POINTERS e,
	const bool OF,
	const bool SF,
	const bool ZF,
	const bool AF,
	const bool PF,
	const bool CF
)
{
	EmuX86_SetFlags(e,
		BITMASK(EMUX86_EFLAG_OF) | BITMASK(EMUX86_EFLAG_SF) | BITMASK(EMUX86_EFLAG_ZF) | BITMASK(EMUX86_EFLAG_AF) | BITMASK(EMUX86_EFLAG_PF) | BITMASK(EMUX86_EFLAG_CF),
		BIT(EMUX86_EFLAG_OF, OF) | BIT(EMUX86_EFLAG_SF, SF) | BIT(EMUX86_EFLAG_ZF, ZF) | BIT(EMUX86_EFLAG_AF, AF) | BIT(EMUX86_EFLAG_PF, PF) | BIT(EMUX86_EFLAG_CF, CF)
	);
}

inline void EmuX86_SetFlags_OSZPC
(
	LPEXCEPTION_POINTERS e,
	const bool OF,
	const bool SF,
	const bool ZF,
	const bool PF,
	const bool CF
)
{
	EmuX86_SetFlags(e,
		BITMASK(EMUX86_EFLAG_OF) | BITMASK(EMUX86_EFLAG_SF) | BITMASK(EMUX86_EFLAG_ZF) | BITMASK(EMUX86_EFLAG_PF) | BITMASK(EMUX86_EFLAG_CF),
		BIT(EMUX86_EFLAG_OF, OF) | BIT(EMUX86_EFLAG_SF, SF) | BIT(EMUX86_EFLAG_ZF, ZF) | BIT(EMUX86_EFLAG_PF, PF) | BIT(EMUX86_EFLAG_CF, CF)
	);
}

inline void EmuX86_SetFlags_OSZAP
(
	LPEXCEPTION_POINTERS e,
	const bool OF,
	const bool SF,
	const bool ZF,
	const bool AF,
	const bool PF
)
{
	EmuX86_SetFlags(e,
		BITMASK(EMUX86_EFLAG_OF) | BITMASK(EMUX86_EFLAG_SF) | BITMASK(EMUX86_EFLAG_ZF) | BITMASK(EMUX86_EFLAG_AF) | BITMASK(EMUX86_EFLAG_PF),
		BIT(EMUX86_EFLAG_OF, OF) | BIT(EMUX86_EFLAG_SF, SF) | BIT(EMUX86_EFLAG_ZF, ZF) | BIT(EMUX86_EFLAG_AF, AF) | BIT(EMUX86_EFLAG_PF, PF)
	);
}

inline bool EmuX86_HasFlag(LPEXCEPTION_POINTERS e, DWORD flag)
{
	return (e->ContextRecord->EFlags & flag);
}

inline bool EmuX86_HasFlag_AF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_AF));
}

inline bool EmuX86_HasFlag_CF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_CF));
}

inline bool EmuX86_HasFlag_OF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_OF));
}

inline bool EmuX86_HasFlag_PF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_PF));
}

inline bool EmuX86_HasFlag_SF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_SF));
}

inline bool EmuX86_HasFlag_ZF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_ZF));
}

inline bool EmuX86_HasFlag_DF(LPEXCEPTION_POINTERS e)
{
	return EmuX86_HasFlag(e, BITMASK(EMUX86_EFLAG_DF));
}

// EFLAGS Cross-Reference : http://datasheets.chipdb.org/Intel/x86/Intel%20Architecture/EFLAGS.PDF

// TODO : Review these CPU flag calculations, maybe peek at how MAME or Bochs does this.
// see : https://github.com/mamedev/mame/blob/master/src/devices/cpu/i386/i386priv.h#L301

#define BitSize (info.ops[0].size) // Note : Uses 'info' argument of functions using this macro
// TODO : Use templates for these, so 8, 16 and 32 bit versions will compile into efficient bit manipulations
#define OF_Add(r,s,d) (((r ^ s) & (r ^ d)) >> (BitSize-1)) & 1 // Result, Src, Dest
#define OF_Sub(r,s,d) (((d ^ s) & (r ^ d)) >> (BitSize-1)) & 1 // Result, Src, Dest
#define SFCalc(result) (result >> (BitSize-1)) & 1
#define ZFCalc(result) (result == 0)
#define AFCalc(r,s,d) ((r ^ s ^ d) >> 3) & 1 // Result, Src, Dest
#define PFCalc(result) (0x6996 >> ((result ^ (result >> 4)) & 0xF)) & 1 // See https://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
#define CFCalc(result) (result >> BitSize) & 1 // TODO : Instead of looking at an actual overflow bit, use high bit of (result XOR dest XOR src)
// Flags calculated : Overflow (for addition or subtraction), Sign, Zero, Adjust, Parity and Carry

// See https://x86.renejeschke.de/ for affected CPU flags per instruction

void SignExtend32(uint32_t& dest, size_t from)
{
	int32_t signExtended = 0;
	switch (from) {
		case 8:
			signExtended = (int8_t)dest;
			break;
		case 16:
			signExtended = (int16_t)dest;
			break;
		default:
			assert(false);
			break;
	}

#if 0
	if ((uint32_t)signExtended != dest) {
		DebugBreak();
	}
#endif

	dest = (uint32_t)signExtended;
}

// Keep opcode emulations alphabetically ordered :

bool EmuX86_Opcode_ADD(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// ADD reads value from source :
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// ADD reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);
	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		add eax, src		// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	return true;
}

bool EmuX86_Opcode_AND(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// AND reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		and eax, src		// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	return true;
}

void EmuX86_Opcode_CDQ(LPEXCEPTION_POINTERS e, _DInst& info)
{
	e->ContextRecord->Edx = (e->ContextRecord->Eax & 0x80000000) ? 0xFFFFFFFF : 0;
}

void EmuX86_Opcode_CLI()
{
	g_bEnableAllInterrupts = false;
}

bool EmuX86_Opcode_CMP(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	uint32_t dest = 0;
	if (!EmuX86_Operand_Read(e, info, 0, &dest))
		return false;

	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		cmp eax, src		// perform the operation, this updates eflags for us!
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	return true;
}

bool EmuX86_Opcode_CMPXCHG(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	uint32_t dest = 0;
	if (!EmuX86_Operand_Read(e, info, 0, &dest))
		return false;

	uint32_t mask;
	switch (info.ops[0].size) {
	case 8: mask = 0xFF; break; // TODO : Needs test-case
	case 16: mask = 0xFFFF; break; // TODO : Needs test-case
	case 32: mask = 0xFFFFFFFF; break;
	default: assert(false); break;
	}

	uint32_t eaxVal = e->ContextRecord->Eax;
	if ((eaxVal & mask) == (dest & mask)) {
		// Write the source value to the destination operand
		if (!EmuX86_Operand_Write(e, info, 0, src)) {
			return false;
		}
	} else	{
		// Write the destination operand to eax
		e->ContextRecord->Eax = (e->ContextRecord->Eax & ~mask) | (dest & mask);
	}

	// Perform arithmatic operation for flag calculation
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, eaxVal
		cmp eax, dest		// perform the operation, this updates eflags for us!
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	return true;
}

void EmuX86_Opcode_CPUID(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// This CPUID emulation is based on :
	// https://github.com/docbrown/vxb/wiki/Xbox-CPUID-Information
	// https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information and
	// https://www.sandpile.org/x86/cpuid.htm
	switch (e->ContextRecord->Eax) { // simpler than EmuX86_GetRegisterValue32(e, R_EAX)
	case 0: { // CPUID Function 0, return the maximum supported standard level and vendor ID string
		// Maximum supported standard level
		e->ContextRecord->Eax = 2;
		// "GenuineIntel" Intel processor
		e->ContextRecord->Ebx = (ULONG)'uneG';
		e->ContextRecord->Edx = (ULONG)'Ieni';
		e->ContextRecord->Ecx = (ULONG)'letn';
		return;
	}
	case 1: { // CPUID Function 1, Return the processor type / family / model / stepping and feature flags
		// Family 6, Model 8, Stepping 10
		e->ContextRecord->Eax = 0x68a;
		e->ContextRecord->Ebx = 0;
		e->ContextRecord->Ecx = 0;
		// Feature Flags 
		e->ContextRecord->Edx = 0x383F9FF; // FPU, VME, DE, PSE, TSC, MSR, PAE, MCE, CX8, SEP, MTRR, PGE, MCA, CMOV, PAT, PSE36, MMX, FXSR, SSE
		return;
	}
	case 2: { // CPUID Function 2, Return the processor configuration descriptors
		// AL : 01 = number of times this level must be queried to obtain all configuration descriptors
		// EAX nibble 1 = 01h : code TLB, 4K pages, 4 ways, 32 entries
		// EAX nibble 2 = 02h : code TLB, 4M pages, fully, 2 entries
		// EAX nibble 3 = 03h : data TLB, 4K pages, 4 ways, 64 entries
		e->ContextRecord->Eax = 0x3020101;
		// EBX and ECX nibbles = 00h : null descriptor (=unused descriptor)
		e->ContextRecord->Ebx = 0;
		e->ContextRecord->Ecx = 0;
		// EDX nibble 0 = 41h : code and data L2 cache, 128 KB, 4 ways, 32 byte lines
		// EDX nibble 1 = 08h : code L1 cache, 16 KB, 4 ways, 32 byte lines
		// EDX nibble 2 = 04h : data TLB, 4M pages, 4 ways, 8 entries
		// EDX nibble 3 = 0Ch : data L1 cache, 16 KB, 4 ways, 32 byte lines
		e->ContextRecord->Edx = 0xC040841;
		return;
	}
	}

	// Note : CPUID instructions never update CPU flags
}

bool EmuX86_Opcode_DEC(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// DEC reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		dec eax				// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

bool EmuX86_Opcode_IN(LPEXCEPTION_POINTERS e, _DInst& info)
{
	uint32_t addr;
	if (!EmuX86_Operand_Read(e, info, 1, &addr))
		return false;

	// IN does an I/O read on the address, writing the value to the second operand :
	uint32_t value = EmuX86_IORead(addr, info.ops[0].size / 8); // Convert size in bits into bytes

	if (!EmuX86_Operand_Write(e, info, 0, value)) {
		return false;
	}

	// Note : IN instructions never update CPU flags

	return true;
}

bool EmuX86_Opcode_INC(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// INC reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		inc eax				// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));
	
	return true;
}

bool EmuX86_Opcode_JMP(LPEXCEPTION_POINTERS e, _DInst& info)
{
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadOnly(e, info, 0, OUT opAddr))
		assert(false);

	e->ContextRecord->Eip = opAddr.addr;
	return true;
}

// Jump if condition is met
// https://c9x.me/x86/html/file_module_x86_id_146.html
// Returns true if branch was taken
bool EmuX86_Opcode_Jcc(LPEXCEPTION_POINTERS e, _DInst& info, bool condition)
{
	if (condition)
		return EmuX86_Opcode_JMP(e, info);

	return false;
}

bool EmuX86_Opcode_LEA(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// LEA reads effective address from source operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadOnly(e, info, 1, OUT opAddr))
		return false;

	uint32_t value = opAddr.addr;

	// LEA writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : LEA instructions never update CPU flags

	return true;
}

bool EmuX86_Opcode_MOV(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// MOV reads value from source :
	uint32_t value = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// MOV writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : MOV instructions never update CPU flags

	return true;
}

bool EmuX86_Opcode_MOVSX(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// MOVSX reads value from source :
	uint32_t value = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// Do MOVSX sign-extension
	switch (info.ops[1].size) {
	case 8:
		if (value & 0x80)
			value |= 0xFFFFFF00;
		break;
	case 16:
		if (value & 0x8000)
			value |= 0xFFFF0000;
		break;
	}

	// MOVSX writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : MOVSX instructions never update CPU flags

	return true;
}

bool EmuX86_Opcode_MOVZX(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// MOVZX reads value from source :
	uint32_t value = 0;

	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// MOVZX zero-extension is implemented implicitly
	assert(value <= 0xFFFF); // Assume input never exceeds a 16 bit value

	// MOVZX writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : MOVZX instructions never update CPU flags

	return true;
}

bool EmuX86_Opcode_NEG(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// NEG reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);
	uint32_t src = dest;

	// NEG Destination
	uint32_t result = 0-dest;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	// The OF, SF, ZF, AF, CF, and PF flags are set according to the result.
	EmuX86_SetFlags_OSZAPC(e,
		/*EMUX86_EFLAG_OF*/OF_Sub(result, src, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, src, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/(dest != 0));

	return true;
}

bool EmuX86_Opcode_NOT(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// NOT reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;
	
	uint32_t dest = EmuX86_Addr_Read(opAddr);
	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		not eax				// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	// Flags are not effected

	return true;
}

bool EmuX86_Opcode_OR(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// OR reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);
	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		or eax, src			// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	return true;
}

bool EmuX86_Opcode_OUT(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// OUT will address the first operand :
	uint32_t addr;
	if (!EmuX86_Operand_Read(e, info, 0, &addr))
		return false;

	uint32_t value;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// OUT does an I/O write on the address, using the value from the second operand :
	EmuX86_IOWrite(addr, value, info.ops[1].size / 8); // Convert size in bits into bytes

	// Note : OUT instructions never update CPU flags

	return true;
}

// https://c9x.me/x86/html/file_module_x86_id_248.html
bool EmuX86_Opcode_POP(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Recognize POP ESP (which increments BEFORE reading from stack) :
	bool bAccessesESP = (info.ops[0].type == O_REG) && (info.ops[0].index == R_ESP);
	uint32_t value;

	if (bAccessesESP) {
		e->ContextRecord->Esp += sizeof(uint32_t);
		value = EmuX86_Mem_Read(e->ContextRecord->Esp, sizeof(uint32_t));
	}
	else {
		value = EmuX86_Mem_Read(e->ContextRecord->Esp, sizeof(uint32_t));
		e->ContextRecord->Esp += sizeof(uint32_t);
	}

	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	return true;
}

bool EmuX86_Opcode_PUSH(LPEXCEPTION_POINTERS e, _DInst& info)
{
	uint32_t value;
	if (!EmuX86_Operand_Read(e, info, 0, &value))
		return false;

	e->ContextRecord->Esp -= sizeof(uint32_t);
	EmuX86_Mem_Write(e->ContextRecord->Esp, value, sizeof(uint32_t));
	return true;
}

ULONGLONG CxbxGetPerformanceCounter(bool acpi); // implemented in EmuKrnlKe.cpp
void EmuX86_Opcode_RDTSC(LPEXCEPTION_POINTERS e)
{
	// We use CxbxGetPerformanceCounter. KeQueryPerformanceCounter is a differnet frequency and cannot be used!
	ULARGE_INTEGER PerformanceCount;
	PerformanceCount.QuadPart = CxbxGetPerformanceCounter(/*acpi*/false);
	e->ContextRecord->Eax = PerformanceCount.LowPart;
	e->ContextRecord->Edx = PerformanceCount.HighPart;
}

bool EmuX86_Opcode_SAR(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// SAR reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	uint8_t byteSrc = src;

	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		mov cl, byteSrc		
		sar eax, cl			// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

bool EmuX86_Opcode_SBB(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// SBB reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		sbb eax, src		// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

// Set Byte on Condition
// https://c9x.me/x86/html/file_module_x86_id_288.html
inline bool EmuX86_Opcode_SETcc(LPEXCEPTION_POINTERS e, _DInst& info, bool condition)
{
	uint8_t value = (condition) ? 1 : 0;

	return EmuX86_Operand_Write(e, info, 0, value);
}

bool EmuX86_Opcode_SHL(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// SHL reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	uint8_t byteSrc = src;

	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		mov cl, byteSrc
		shl eax, cl			// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}


	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

bool EmuX86_Opcode_SHR(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// SHR reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;


	uint32_t dest = EmuX86_Addr_Read(opAddr);
	
	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	uint8_t byteSrc = src;

	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		mov cl, byteSrc
		shr eax, cl			// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}


	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

void EmuX86_Opcode_STI()
{
	g_bEnableAllInterrupts = true;
}

bool EmuX86_Opcode_STOS(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read a value from src
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// Write the value to dest
	if (!EmuX86_Operand_Write(e, info, 0, src)) {
		return false;
	}

	// Determine increment/decrement size
	uint8_t size = info.ops[1].size / 8;

	// Increment/decrement ESI and EDI based on the current set flags
	if (EmuX86_HasFlag_DF(e)) {
		e->ContextRecord->Esi -= size;
		e->ContextRecord->Edi -= size;
	}
	else {
		e->ContextRecord->Esi += size;
		e->ContextRecord->Edi += size;
	}

	return true;
}

bool EmuX86_Opcode_SUB(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// SUB reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		sub eax, src		// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	return true;
}

bool EmuX86_Opcode_TEST(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TEST reads first value :
	uint32_t dest = 0;
	if (!EmuX86_Operand_Read(e, info, 0, &dest))
		return false;

	// TEST reads second value :
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		test eax, src		// perform the operation, this updates eflags for us!
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	return true;
}

bool EmuX86_Opcode_XOR(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// Read value from Source and Destination
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &src))
		return false;

	// XOR reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	uint32_t dest = EmuX86_Addr_Read(opAddr);

	if (info.ops[0].size > info.ops[1].size) {
		SignExtend32(src, info.ops[1].size);
	}

	uint32_t result = 0;
	uint32_t eflags = e->ContextRecord->EFlags;
	__asm {
		push eflags			// push context eflags on the stack
		popfd				// pop context eflags into host eflags
		mov eax, dest
		xor eax, src		// perform the operation, this updates eflags for us!
		mov result, eax
		pushfd				// push the updated host flags onto the stack
		pop eflags			// pop the updated host flags back into our eflags register
	}

	// Write back the flags
	e->ContextRecord->EFlags = eflags;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);


	return true;
}

bool EmuX86_DecodeOpcode(const uint8_t *Eip, _DInst &info)
{
	unsigned int decodedInstructionsCount = 0;

	_CodeInfo ci;
	ci.code = (uint8_t*)Eip;
	ci.codeLen = 20;
	ci.codeOffset = 0;
	ci.dt = (_DecodeType)Decode32Bits;
	ci.features = DF_NONE;

	// Checking for DECRES_SUCCESS won't work, since we're passing distorm_decompose
	// a codeLen big enough to decode any instruction-length, plus distorm doesn't
	// halt cleanly after reaching maxInstructions 1. So instead, just call distorm :
	distorm_decompose(&ci, &info, /*maxInstructions=*/1, &decodedInstructionsCount);
	// and check if it successfully decoded one instruction :
	return (decodedInstructionsCount == 1);
}

const char *Distorm_RegStrings[/*_RegisterType*/] = {
	"RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI", "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "R8D", "R9D", "R10D", "R11D", "R12D", "R13D", "R14D", "R15D",
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "R8W", "R9W", "R10W", "R11W", "R12W", "R13W", "R14W", "R15W",
	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH", "R8B", "R9B", "R10B", "R11B", "R12B", "R13B", "R14B", "R15B",
	"SPL", "BPL", "SIL", "DIL",
	"ES", "CS", "SS", "DS", "FS", "GS",
	"RIP",
	"ST0", "ST1", "ST2", "ST3", "ST4", "ST5", "ST6", "ST7",
	"MM0", "MM1", "MM2", "MM3", "MM4", "MM5", "MM6", "MM7",
	"XMM0", "XMM1", "XMM2", "XMM3", "XMM4", "XMM5", "XMM6", "XMM7", "XMM8", "XMM9", "XMM10", "XMM11", "XMM12", "XMM13", "XMM14", "XMM15",
	"YMM0", "YMM1", "YMM2", "YMM3", "YMM4", "YMM5", "YMM6", "YMM7", "YMM8", "YMM9", "YMM10", "YMM11", "YMM12", "YMM13", "YMM14", "YMM15",
	"CR0", "UNUSED0", "CR2", "CR3", "CR4", "UNUSED1", "UNUSED2", "UNUSED3", "CR8",
	"DR0", "DR1", "DR2", "DR3", "UNUSED4", "UNUSED5", "DR6", "DR7",
};

const char *Distorm_OpcodeString(const int opcode)
{
	// Note : Distorm's GET_MNEMONIC_NAME() doesn't link somehow...
	switch (opcode) {
	case I_AAA: return "AAA";
	case I_AAD: return "AAD";
	case I_AAM: return "AAM";
	case I_AAS: return "AAS";
	case I_ADC: return "ADC";
	case I_ADD: return "ADD";
	case I_ADDPD: return "ADDPD";
	case I_ADDPS: return "ADDPS";
	case I_ADDSD: return "ADDSD";
	case I_ADDSS: return "ADDSS";
	case I_ADDSUBPD: return "ADDSUBPD";
	case I_ADDSUBPS: return "ADDSUBPS";
	case I_AESDEC: return "AESDEC";
	case I_AESDECLAST: return "AESDECLAST";
	case I_AESENC: return "AESENC";
	case I_AESENCLAST: return "AESENCLAST";
	case I_AESIMC: return "AESIMC";
	case I_AESKEYGENASSIST: return "AESKEYGENASSIST";
	case I_AND: return "AND";
	case I_ANDNPD: return "ANDNPD";
	case I_ANDNPS: return "ANDNPS";
	case I_ANDPD: return "ANDPD";
	case I_ANDPS: return "ANDPS";
	case I_ARPL: return "ARPL";
	case I_BLENDPD: return "BLENDPD";
	case I_BLENDPS: return "BLENDPS";
	case I_BLENDVPD: return "BLENDVPD";
	case I_BLENDVPS: return "BLENDVPS";
	case I_BOUND: return "BOUND";
	case I_BSF: return "BSF";
	case I_BSR: return "BSR";
	case I_BSWAP: return "BSWAP";
	case I_BT: return "BT";
	case I_BTC: return "BTC";
	case I_BTR: return "BTR";
	case I_BTS: return "BTS";
	case I_CALL: return "CALL";
	case I_CALL_FAR: return "CALL_FAR";
	case I_CBW: return "CBW";
	case I_CDQ: return "CDQ";
	case I_CDQE: return "CDQE";
	case I_CLC: return "CLC";
	case I_CLD: return "CLD";
	case I_CLFLUSH: return "CLFLUSH";
	case I_CLGI: return "CLGI";
	case I_CLI: return "CLI";
	case I_CLTS: return "CLTS";
	case I_CMC: return "CMC";
	case I_CMOVA: return "CMOVA";
	case I_CMOVAE: return "CMOVAE";
	case I_CMOVB: return "CMOVB";
	case I_CMOVBE: return "CMOVBE";
	case I_CMOVG: return "CMOVG";
	case I_CMOVGE: return "CMOVGE";
	case I_CMOVL: return "CMOVL";
	case I_CMOVLE: return "CMOVLE";
	case I_CMOVNO: return "CMOVNO";
	case I_CMOVNP: return "CMOVNP";
	case I_CMOVNS: return "CMOVNS";
	case I_CMOVNZ: return "CMOVNZ";
	case I_CMOVO: return "CMOVO";
	case I_CMOVP: return "CMOVP";
	case I_CMOVS: return "CMOVS";
	case I_CMOVZ: return "CMOVZ";
	case I_CMP: return "CMP";
	case I_CMPEQPD: return "CMPEQPD";
	case I_CMPEQPS: return "CMPEQPS";
	case I_CMPEQSD: return "CMPEQSD";
	case I_CMPEQSS: return "CMPEQSS";
	case I_CMPLEPD: return "CMPLEPD";
	case I_CMPLEPS: return "CMPLEPS";
	case I_CMPLESD: return "CMPLESD";
	case I_CMPLESS: return "CMPLESS";
	case I_CMPLTPD: return "CMPLTPD";
	case I_CMPLTPS: return "CMPLTPS";
	case I_CMPLTSD: return "CMPLTSD";
	case I_CMPLTSS: return "CMPLTSS";
	case I_CMPNEQPD: return "CMPNEQPD";
	case I_CMPNEQPS: return "CMPNEQPS";
	case I_CMPNEQSD: return "CMPNEQSD";
	case I_CMPNEQSS: return "CMPNEQSS";
	case I_CMPNLEPD: return "CMPNLEPD";
	case I_CMPNLEPS: return "CMPNLEPS";
	case I_CMPNLESD: return "CMPNLESD";
	case I_CMPNLESS: return "CMPNLESS";
	case I_CMPNLTPD: return "CMPNLTPD";
	case I_CMPNLTPS: return "CMPNLTPS";
	case I_CMPNLTSD: return "CMPNLTSD";
	case I_CMPNLTSS: return "CMPNLTSS";
	case I_CMPORDPD: return "CMPORDPD";
	case I_CMPORDPS: return "CMPORDPS";
	case I_CMPORDSD: return "CMPORDSD";
	case I_CMPORDSS: return "CMPORDSS";
	case I_CMPS: return "CMPS";
	case I_CMPUNORDPD: return "CMPUNORDPD";
	case I_CMPUNORDPS: return "CMPUNORDPS";
	case I_CMPUNORDSD: return "CMPUNORDSD";
	case I_CMPUNORDSS: return "CMPUNORDSS";
	case I_CMPXCHG: return "CMPXCHG";
	case I_CMPXCHG16B: return "CMPXCHG16B";
	case I_CMPXCHG8B: return "CMPXCHG8B";
	case I_COMISD: return "COMISD";
	case I_COMISS: return "COMISS";
	case I_CPUID: return "CPUID";
	case I_CQO: return "CQO";
	case I_CRC32: return "CRC32";
	case I_CVTDQ2PD: return "CVTDQ2PD";
	case I_CVTDQ2PS: return "CVTDQ2PS";
	case I_CVTPD2DQ: return "CVTPD2DQ";
	case I_CVTPD2PI: return "CVTPD2PI";
	case I_CVTPD2PS: return "CVTPD2PS";
	case I_CVTPH2PS: return "CVTPH2PS";
	case I_CVTPI2PD: return "CVTPI2PD";
	case I_CVTPI2PS: return "CVTPI2PS";
	case I_CVTPS2DQ: return "CVTPS2DQ";
	case I_CVTPS2PD: return "CVTPS2PD";
	case I_CVTPS2PH: return "CVTPS2PH";
	case I_CVTPS2PI: return "CVTPS2PI";
	case I_CVTSD2SI: return "CVTSD2SI";
	case I_CVTSD2SS: return "CVTSD2SS";
	case I_CVTSI2SD: return "CVTSI2SD";
	case I_CVTSI2SS: return "CVTSI2SS";
	case I_CVTSS2SD: return "CVTSS2SD";
	case I_CVTSS2SI: return "CVTSS2SI";
	case I_CVTTPD2DQ: return "CVTTPD2DQ";
	case I_CVTTPD2PI: return "CVTTPD2PI";
	case I_CVTTPS2DQ: return "CVTTPS2DQ";
	case I_CVTTPS2PI: return "CVTTPS2PI";
	case I_CVTTSD2SI: return "CVTTSD2SI";
	case I_CVTTSS2SI: return "CVTTSS2SI";
	case I_CWD: return "CWD";
	case I_CWDE: return "CWDE";
	case I_DAA: return "DAA";
	case I_DAS: return "DAS";
	case I_DEC: return "DEC";
	case I_DIV: return "DIV";
	case I_DIVPD: return "DIVPD";
	case I_DIVPS: return "DIVPS";
	case I_DIVSD: return "DIVSD";
	case I_DIVSS: return "DIVSS";
	case I_DPPD: return "DPPD";
	case I_DPPS: return "DPPS";
	case I_EMMS: return "EMMS";
	case I_ENTER: return "ENTER";
	case I_EXTRACTPS: return "EXTRACTPS";
	case I_EXTRQ: return "EXTRQ";
	case I_F2XM1: return "F2XM1";
	case I_FABS: return "FABS";
	case I_FADD: return "FADD";
	case I_FADDP: return "FADDP";
	case I_FBLD: return "FBLD";
	case I_FBSTP: return "FBSTP";
	case I_FCHS: return "FCHS";
	case I_FCLEX: return "FCLEX";
	case I_FCMOVB: return "FCMOVB";
	case I_FCMOVBE: return "FCMOVBE";
	case I_FCMOVE: return "FCMOVE";
	case I_FCMOVNB: return "FCMOVNB";
	case I_FCMOVNBE: return "FCMOVNBE";
	case I_FCMOVNE: return "FCMOVNE";
	case I_FCMOVNU: return "FCMOVNU";
	case I_FCMOVU: return "FCMOVU";
	case I_FCOM: return "FCOM";
	case I_FCOMI: return "FCOMI";
	case I_FCOMIP: return "FCOMIP";
	case I_FCOMP: return "FCOMP";
	case I_FCOMPP: return "FCOMPP";
	case I_FCOS: return "FCOS";
	case I_FDECSTP: return "FDECSTP";
	case I_FDIV: return "FDIV";
	case I_FDIVP: return "FDIVP";
	case I_FDIVR: return "FDIVR";
	case I_FDIVRP: return "FDIVRP";
	case I_FEDISI: return "FEDISI";
	case I_FEMMS: return "FEMMS";
	case I_FENI: return "FENI";
	case I_FFREE: return "FFREE";
	case I_FIADD: return "FIADD";
	case I_FICOM: return "FICOM";
	case I_FICOMP: return "FICOMP";
	case I_FIDIV: return "FIDIV";
	case I_FIDIVR: return "FIDIVR";
	case I_FILD: return "FILD";
	case I_FIMUL: return "FIMUL";
	case I_FINCSTP: return "FINCSTP";
	case I_FINIT: return "FINIT";
	case I_FIST: return "FIST";
	case I_FISTP: return "FISTP";
	case I_FISTTP: return "FISTTP";
	case I_FISUB: return "FISUB";
	case I_FISUBR: return "FISUBR";
	case I_FLD: return "FLD";
	case I_FLD1: return "FLD1";
	case I_FLDCW: return "FLDCW";
	case I_FLDENV: return "FLDENV";
	case I_FLDL2E: return "FLDL2E";
	case I_FLDL2T: return "FLDL2T";
	case I_FLDLG2: return "FLDLG2";
	case I_FLDLN2: return "FLDLN2";
	case I_FLDPI: return "FLDPI";
	case I_FLDZ: return "FLDZ";
	case I_FMUL: return "FMUL";
	case I_FMULP: return "FMULP";
	case I_FNCLEX: return "FNCLEX";
	case I_FNINIT: return "FNINIT";
	case I_FNOP: return "FNOP";
	case I_FNSAVE: return "FNSAVE";
	case I_FNSTCW: return "FNSTCW";
	case I_FNSTENV: return "FNSTENV";
	case I_FNSTSW: return "FNSTSW";
	case I_FPATAN: return "FPATAN";
	case I_FPREM: return "FPREM";
	case I_FPREM1: return "FPREM1";
	case I_FPTAN: return "FPTAN";
	case I_FRNDINT: return "FRNDINT";
	case I_FRSTOR: return "FRSTOR";
	case I_FSAVE: return "FSAVE";
	case I_FSCALE: return "FSCALE";
	case I_FSETPM: return "FSETPM";
	case I_FSIN: return "FSIN";
	case I_FSINCOS: return "FSINCOS";
	case I_FSQRT: return "FSQRT";
	case I_FST: return "FST";
	case I_FSTCW: return "FSTCW";
	case I_FSTENV: return "FSTENV";
	case I_FSTP: return "FSTP";
	case I_FSTSW: return "FSTSW";
	case I_FSUB: return "FSUB";
	case I_FSUBP: return "FSUBP";
	case I_FSUBR: return "FSUBR";
	case I_FSUBRP: return "FSUBRP";
	case I_FTST: return "FTST";
	case I_FUCOM: return "FUCOM";
	case I_FUCOMI: return "FUCOMI";
	case I_FUCOMIP: return "FUCOMIP";
	case I_FUCOMP: return "FUCOMP";
	case I_FUCOMPP: return "FUCOMPP";
	case I_FXAM: return "FXAM";
	case I_FXCH: return "FXCH";
	case I_FXRSTOR: return "FXRSTOR";
	case I_FXRSTOR64: return "FXRSTOR64";
	case I_FXSAVE: return "FXSAVE";
	case I_FXSAVE64: return "FXSAVE64";
	case I_FXTRACT: return "FXTRACT";
	case I_FYL2X: return "FYL2X";
	case I_FYL2XP1: return "FYL2XP1";
	case I_GETSEC: return "GETSEC";
	case I_HADDPD: return "HADDPD";
	case I_HADDPS: return "HADDPS";
	case I_HLT: return "HLT";
	case I_HSUBPD: return "HSUBPD";
	case I_HSUBPS: return "HSUBPS";
	case I_IDIV: return "IDIV";
	case I_IMUL: return "IMUL";
	case I_IN: return "IN";
	case I_INC: return "INC";
	case I_INS: return "INS";
	case I_INSERTPS: return "INSERTPS";
	case I_INSERTQ: return "INSERTQ";
	case I_INT: return "INT";
	case I_INT1: return "INT1";
	case I_INTO: return "INTO";
	case I_INT_3: return "INT_3";
	case I_INVD: return "INVD";
	case I_INVEPT: return "INVEPT";
	case I_INVLPG: return "INVLPG";
	case I_INVLPGA: return "INVLPGA";
	case I_INVPCID: return "INVPCID";
	case I_INVVPID: return "INVVPID";
	case I_IRET: return "IRET";
	case I_JA: return "JA";
	case I_JAE: return "JAE";
	case I_JB: return "JB";
	case I_JBE: return "JBE";
	case I_JCXZ: return "JCXZ";
	case I_JECXZ: return "JECXZ";
	case I_JG: return "JG";
	case I_JGE: return "JGE";
	case I_JL: return "JL";
	case I_JLE: return "JLE";
	case I_JMP: return "JMP";
	case I_JMP_FAR: return "JMP_FAR";
	case I_JNO: return "JNO";
	case I_JNP: return "JNP";
	case I_JNS: return "JNS";
	case I_JNZ: return "JNZ";
	case I_JO: return "JO";
	case I_JP: return "JP";
	case I_JRCXZ: return "JRCXZ";
	case I_JS: return "JS";
	case I_JZ: return "JZ";
	case I_LAHF: return "LAHF";
	case I_LAR: return "LAR";
	case I_LDDQU: return "LDDQU";
	case I_LDMXCSR: return "LDMXCSR";
	case I_LDS: return "LDS";
	case I_LEA: return "LEA";
	case I_LEAVE: return "LEAVE";
	case I_LES: return "LES";
	case I_LFENCE: return "LFENCE";
	case I_LFS: return "LFS";
	case I_LGDT: return "LGDT";
	case I_LGS: return "LGS";
	case I_LIDT: return "LIDT";
	case I_LLDT: return "LLDT";
	case I_LMSW: return "LMSW";
	case I_LODS: return "LODS";
	case I_LOOP: return "LOOP";
	case I_LOOPNZ: return "LOOPNZ";
	case I_LOOPZ: return "LOOPZ";
	case I_LSL: return "LSL";
	case I_LSS: return "LSS";
	case I_LTR: return "LTR";
	case I_LZCNT: return "LZCNT";
	case I_MASKMOVDQU: return "MASKMOVDQU";
	case I_MASKMOVQ: return "MASKMOVQ";
	case I_MAXPD: return "MAXPD";
	case I_MAXPS: return "MAXPS";
	case I_MAXSD: return "MAXSD";
	case I_MAXSS: return "MAXSS";
	case I_MFENCE: return "MFENCE";
	case I_MINPD: return "MINPD";
	case I_MINPS: return "MINPS";
	case I_MINSD: return "MINSD";
	case I_MINSS: return "MINSS";
	case I_MONITOR: return "MONITOR";
	case I_MOV: return "MOV";
	case I_MOVAPD: return "MOVAPD";
	case I_MOVAPS: return "MOVAPS";
	case I_MOVBE: return "MOVBE";
	case I_MOVD: return "MOVD";
	case I_MOVDDUP: return "MOVDDUP";
	case I_MOVDQ2Q: return "MOVDQ2Q";
	case I_MOVDQA: return "MOVDQA";
	case I_MOVDQU: return "MOVDQU";
	case I_MOVHLPS: return "MOVHLPS";
	case I_MOVHPD: return "MOVHPD";
	case I_MOVHPS: return "MOVHPS";
	case I_MOVLHPS: return "MOVLHPS";
	case I_MOVLPD: return "MOVLPD";
	case I_MOVLPS: return "MOVLPS";
	case I_MOVMSKPD: return "MOVMSKPD";
	case I_MOVMSKPS: return "MOVMSKPS";
	case I_MOVNTDQ: return "MOVNTDQ";
	case I_MOVNTDQA: return "MOVNTDQA";
	case I_MOVNTI: return "MOVNTI";
	case I_MOVNTPD: return "MOVNTPD";
	case I_MOVNTPS: return "MOVNTPS";
	case I_MOVNTQ: return "MOVNTQ";
	case I_MOVNTSD: return "MOVNTSD";
	case I_MOVNTSS: return "MOVNTSS";
	case I_MOVQ: return "MOVQ";
	case I_MOVQ2DQ: return "MOVQ2DQ";
	case I_MOVS: return "MOVS";
	case I_MOVSD: return "MOVSD";
	case I_MOVSHDUP: return "MOVSHDUP";
	case I_MOVSLDUP: return "MOVSLDUP";
	case I_MOVSS: return "MOVSS";
	case I_MOVSX: return "MOVSX";
	case I_MOVSXD: return "MOVSXD";
	case I_MOVUPD: return "MOVUPD";
	case I_MOVUPS: return "MOVUPS";
	case I_MOVZX: return "MOVZX";
	case I_MPSADBW: return "MPSADBW";
	case I_MUL: return "MUL";
	case I_MULPD: return "MULPD";
	case I_MULPS: return "MULPS";
	case I_MULSD: return "MULSD";
	case I_MULSS: return "MULSS";
	case I_MWAIT: return "MWAIT";
	case I_NEG: return "NEG";
	case I_NOP: return "NOP";
	case I_NOT: return "NOT";
	case I_OR: return "OR";
	case I_ORPD: return "ORPD";
	case I_ORPS: return "ORPS";
	case I_OUT: return "OUT";
	case I_OUTS: return "OUTS";
	case I_PABSB: return "PABSB";
	case I_PABSD: return "PABSD";
	case I_PABSW: return "PABSW";
	case I_PACKSSDW: return "PACKSSDW";
	case I_PACKSSWB: return "PACKSSWB";
	case I_PACKUSDW: return "PACKUSDW";
	case I_PACKUSWB: return "PACKUSWB";
	case I_PADDB: return "PADDB";
	case I_PADDD: return "PADDD";
	case I_PADDQ: return "PADDQ";
	case I_PADDSB: return "PADDSB";
	case I_PADDSW: return "PADDSW";
	case I_PADDUSB: return "PADDUSB";
	case I_PADDUSW: return "PADDUSW";
	case I_PADDW: return "PADDW";
	case I_PALIGNR: return "PALIGNR";
	case I_PAND: return "PAND";
	case I_PANDN: return "PANDN";
	case I_PAUSE: return "PAUSE";
	case I_PAVGB: return "PAVGB";
	case I_PAVGUSB: return "PAVGUSB";
	case I_PAVGW: return "PAVGW";
	case I_PBLENDVB: return "PBLENDVB";
	case I_PBLENDW: return "PBLENDW";
	case I_PCLMULQDQ: return "PCLMULQDQ";
	case I_PCMPEQB: return "PCMPEQB";
	case I_PCMPEQD: return "PCMPEQD";
	case I_PCMPEQQ: return "PCMPEQQ";
	case I_PCMPEQW: return "PCMPEQW";
	case I_PCMPESTRI: return "PCMPESTRI";
	case I_PCMPESTRM: return "PCMPESTRM";
	case I_PCMPGTB: return "PCMPGTB";
	case I_PCMPGTD: return "PCMPGTD";
	case I_PCMPGTQ: return "PCMPGTQ";
	case I_PCMPGTW: return "PCMPGTW";
	case I_PCMPISTRI: return "PCMPISTRI";
	case I_PCMPISTRM: return "PCMPISTRM";
	case I_PEXTRB: return "PEXTRB";
	case I_PEXTRD: return "PEXTRD";
	case I_PEXTRQ: return "PEXTRQ";
	case I_PEXTRW: return "PEXTRW";
	case I_PF2ID: return "PF2ID";
	case I_PF2IW: return "PF2IW";
	case I_PFACC: return "PFACC";
	case I_PFADD: return "PFADD";
	case I_PFCMPEQ: return "PFCMPEQ";
	case I_PFCMPGE: return "PFCMPGE";
	case I_PFCMPGT: return "PFCMPGT";
	case I_PFMAX: return "PFMAX";
	case I_PFMIN: return "PFMIN";
	case I_PFMUL: return "PFMUL";
	case I_PFNACC: return "PFNACC";
	case I_PFPNACC: return "PFPNACC";
	case I_PFRCP: return "PFRCP";
	case I_PFRCPIT1: return "PFRCPIT1";
	case I_PFRCPIT2: return "PFRCPIT2";
	case I_PFRSQIT1: return "PFRSQIT1";
	case I_PFRSQRT: return "PFRSQRT";
	case I_PFSUB: return "PFSUB";
	case I_PFSUBR: return "PFSUBR";
	case I_PHADDD: return "PHADDD";
	case I_PHADDSW: return "PHADDSW";
	case I_PHADDW: return "PHADDW";
	case I_PHMINPOSUW: return "PHMINPOSUW";
	case I_PHSUBD: return "PHSUBD";
	case I_PHSUBSW: return "PHSUBSW";
	case I_PHSUBW: return "PHSUBW";
	case I_PI2FD: return "PI2FD";
	case I_PI2FW: return "PI2FW";
	case I_PINSRB: return "PINSRB";
	case I_PINSRD: return "PINSRD";
	case I_PINSRQ: return "PINSRQ";
	case I_PINSRW: return "PINSRW";
	case I_PMADDUBSW: return "PMADDUBSW";
	case I_PMADDWD: return "PMADDWD";
	case I_PMAXSB: return "PMAXSB";
	case I_PMAXSD: return "PMAXSD";
	case I_PMAXSW: return "PMAXSW";
	case I_PMAXUB: return "PMAXUB";
	case I_PMAXUD: return "PMAXUD";
	case I_PMAXUW: return "PMAXUW";
	case I_PMINSB: return "PMINSB";
	case I_PMINSD: return "PMINSD";
	case I_PMINSW: return "PMINSW";
	case I_PMINUB: return "PMINUB";
	case I_PMINUD: return "PMINUD";
	case I_PMINUW: return "PMINUW";
	case I_PMOVMSKB: return "PMOVMSKB";
	case I_PMOVSXBD: return "PMOVSXBD";
	case I_PMOVSXBQ: return "PMOVSXBQ";
	case I_PMOVSXBW: return "PMOVSXBW";
	case I_PMOVSXDQ: return "PMOVSXDQ";
	case I_PMOVSXWD: return "PMOVSXWD";
	case I_PMOVSXWQ: return "PMOVSXWQ";
	case I_PMOVZXBD: return "PMOVZXBD";
	case I_PMOVZXBQ: return "PMOVZXBQ";
	case I_PMOVZXBW: return "PMOVZXBW";
	case I_PMOVZXDQ: return "PMOVZXDQ";
	case I_PMOVZXWD: return "PMOVZXWD";
	case I_PMOVZXWQ: return "PMOVZXWQ";
	case I_PMULDQ: return "PMULDQ";
	case I_PMULHRSW: return "PMULHRSW";
	case I_PMULHRW: return "PMULHRW";
	case I_PMULHUW: return "PMULHUW";
	case I_PMULHW: return "PMULHW";
	case I_PMULLD: return "PMULLD";
	case I_PMULLW: return "PMULLW";
	case I_PMULUDQ: return "PMULUDQ";
	case I_POP: return "POP";
	case I_POPA: return "POPA";
	case I_POPCNT: return "POPCNT";
	case I_POPF: return "POPF";
	case I_POR: return "POR";
	case I_PREFETCH: return "PREFETCH";
	case I_PREFETCHNTA: return "PREFETCHNTA";
	case I_PREFETCHT0: return "PREFETCHT0";
	case I_PREFETCHT1: return "PREFETCHT1";
	case I_PREFETCHT2: return "PREFETCHT2";
	case I_PREFETCHW: return "PREFETCHW";
	case I_PSADBW: return "PSADBW";
	case I_PSHUFB: return "PSHUFB";
	case I_PSHUFD: return "PSHUFD";
	case I_PSHUFHW: return "PSHUFHW";
	case I_PSHUFLW: return "PSHUFLW";
	case I_PSHUFW: return "PSHUFW";
	case I_PSIGNB: return "PSIGNB";
	case I_PSIGND: return "PSIGND";
	case I_PSIGNW: return "PSIGNW";
	case I_PSLLD: return "PSLLD";
	case I_PSLLDQ: return "PSLLDQ";
	case I_PSLLQ: return "PSLLQ";
	case I_PSLLW: return "PSLLW";
	case I_PSRAD: return "PSRAD";
	case I_PSRAW: return "PSRAW";
	case I_PSRLD: return "PSRLD";
	case I_PSRLDQ: return "PSRLDQ";
	case I_PSRLQ: return "PSRLQ";
	case I_PSRLW: return "PSRLW";
	case I_PSUBB: return "PSUBB";
	case I_PSUBD: return "PSUBD";
	case I_PSUBQ: return "PSUBQ";
	case I_PSUBSB: return "PSUBSB";
	case I_PSUBSW: return "PSUBSW";
	case I_PSUBUSB: return "PSUBUSB";
	case I_PSUBUSW: return "PSUBUSW";
	case I_PSUBW: return "PSUBW";
	case I_PSWAPD: return "PSWAPD";
	case I_PTEST: return "PTEST";
	case I_PUNPCKHBW: return "PUNPCKHBW";
	case I_PUNPCKHDQ: return "PUNPCKHDQ";
	case I_PUNPCKHQDQ: return "PUNPCKHQDQ";
	case I_PUNPCKHWD: return "PUNPCKHWD";
	case I_PUNPCKLBW: return "PUNPCKLBW";
	case I_PUNPCKLDQ: return "PUNPCKLDQ";
	case I_PUNPCKLQDQ: return "PUNPCKLQDQ";
	case I_PUNPCKLWD: return "PUNPCKLWD";
	case I_PUSH: return "PUSH";
	case I_PUSHA: return "PUSHA";
	case I_PUSHF: return "PUSHF";
	case I_PXOR: return "PXOR";
	case I_RCL: return "RCL";
	case I_RCPPS: return "RCPPS";
	case I_RCPSS: return "RCPSS";
	case I_RCR: return "RCR";
	case I_RDFSBASE: return "RDFSBASE";
	case I_RDGSBASE: return "RDGSBASE";
	case I_RDMSR: return "RDMSR";
	case I_RDPMC: return "RDPMC";
	case I_RDRAND: return "RDRAND";
	case I_RDTSC: return "RDTSC";
	case I_RDTSCP: return "RDTSCP";
	case I_RET: return "RET";
	case I_RETF: return "RETF";
	case I_ROL: return "ROL";
	case I_ROR: return "ROR";
	case I_ROUNDPD: return "ROUNDPD";
	case I_ROUNDPS: return "ROUNDPS";
	case I_ROUNDSD: return "ROUNDSD";
	case I_ROUNDSS: return "ROUNDSS";
	case I_RSM: return "RSM";
	case I_RSQRTPS: return "RSQRTPS";
	case I_RSQRTSS: return "RSQRTSS";
	case I_SAHF: return "SAHF";
	case I_SAL: return "SAL";
	case I_SALC: return "SALC";
	case I_SAR: return "SAR";
	case I_SBB: return "SBB";
	case I_SCAS: return "SCAS";
	case I_SETA: return "SETA";
	case I_SETAE: return "SETAE";
	case I_SETB: return "SETB";
	case I_SETBE: return "SETBE";
	case I_SETG: return "SETG";
	case I_SETGE: return "SETGE";
	case I_SETL: return "SETL";
	case I_SETLE: return "SETLE";
	case I_SETNO: return "SETNO";
	case I_SETNP: return "SETNP";
	case I_SETNS: return "SETNS";
	case I_SETNZ: return "SETNZ";
	case I_SETO: return "SETO";
	case I_SETP: return "SETP";
	case I_SETS: return "SETS";
	case I_SETZ: return "SETZ";
	case I_SFENCE: return "SFENCE";
	case I_SGDT: return "SGDT";
	case I_SHL: return "SHL";
	case I_SHLD: return "SHLD";
	case I_SHR: return "SHR";
	case I_SHRD: return "SHRD";
	case I_SHUFPD: return "SHUFPD";
	case I_SHUFPS: return "SHUFPS";
	case I_SIDT: return "SIDT";
	case I_SKINIT: return "SKINIT";
	case I_SLDT: return "SLDT";
	case I_SMSW: return "SMSW";
	case I_SQRTPD: return "SQRTPD";
	case I_SQRTPS: return "SQRTPS";
	case I_SQRTSD: return "SQRTSD";
	case I_SQRTSS: return "SQRTSS";
	case I_STC: return "STC";
	case I_STD: return "STD";
	case I_STGI: return "STGI";
	case I_STI: return "STI";
	case I_STMXCSR: return "STMXCSR";
	case I_STOS: return "STOS";
	case I_STR: return "STR";
	case I_SUB: return "SUB";
	case I_SUBPD: return "SUBPD";
	case I_SUBPS: return "SUBPS";
	case I_SUBSD: return "SUBSD";
	case I_SUBSS: return "SUBSS";
	case I_SWAPGS: return "SWAPGS";
	case I_SYSCALL: return "SYSCALL";
	case I_SYSENTER: return "SYSENTER";
	case I_SYSEXIT: return "SYSEXIT";
	case I_SYSRET: return "SYSRET";
	case I_TEST: return "TEST";
	case I_TZCNT: return "TZCNT";
	case I_UCOMISD: return "UCOMISD";
	case I_UCOMISS: return "UCOMISS";
	case I_UD2: return "UD2";
	case I_UNDEFINED: return "UNDEFINED";
	case I_UNPCKHPD: return "UNPCKHPD";
	case I_UNPCKHPS: return "UNPCKHPS";
	case I_UNPCKLPD: return "UNPCKLPD";
	case I_UNPCKLPS: return "UNPCKLPS";
	case I_VADDPD: return "VADDPD";
	case I_VADDPS: return "VADDPS";
	case I_VADDSD: return "VADDSD";
	case I_VADDSS: return "VADDSS";
	case I_VADDSUBPD: return "VADDSUBPD";
	case I_VADDSUBPS: return "VADDSUBPS";
	case I_VAESDEC: return "VAESDEC";
	case I_VAESDECLAST: return "VAESDECLAST";
	case I_VAESENC: return "VAESENC";
	case I_VAESENCLAST: return "VAESENCLAST";
	case I_VAESIMC: return "VAESIMC";
	case I_VAESKEYGENASSIST: return "VAESKEYGENASSIST";
	case I_VANDNPD: return "VANDNPD";
	case I_VANDNPS: return "VANDNPS";
	case I_VANDPD: return "VANDPD";
	case I_VANDPS: return "VANDPS";
	case I_VBLENDPD: return "VBLENDPD";
	case I_VBLENDPS: return "VBLENDPS";
	case I_VBLENDVPD: return "VBLENDVPD";
	case I_VBLENDVPS: return "VBLENDVPS";
	case I_VBROADCASTF128: return "VBROADCASTF128";
	case I_VBROADCASTSD: return "VBROADCASTSD";
	case I_VBROADCASTSS: return "VBROADCASTSS";
	case I_VCMPEQPD: return "VCMPEQPD";
	case I_VCMPEQPS: return "VCMPEQPS";
	case I_VCMPEQSD: return "VCMPEQSD";
	case I_VCMPEQSS: return "VCMPEQSS";
	case I_VCMPEQ_OSPD: return "VCMPEQ_OSPD";
	case I_VCMPEQ_OSPS: return "VCMPEQ_OSPS";
	case I_VCMPEQ_OSSD: return "VCMPEQ_OSSD";
	case I_VCMPEQ_OSSS: return "VCMPEQ_OSSS";
	case I_VCMPEQ_UQPD: return "VCMPEQ_UQPD";
	case I_VCMPEQ_UQPS: return "VCMPEQ_UQPS";
	case I_VCMPEQ_UQSD: return "VCMPEQ_UQSD";
	case I_VCMPEQ_UQSS: return "VCMPEQ_UQSS";
	case I_VCMPEQ_USPD: return "VCMPEQ_USPD";
	case I_VCMPEQ_USPS: return "VCMPEQ_USPS";
	case I_VCMPEQ_USSD: return "VCMPEQ_USSD";
	case I_VCMPEQ_USSS: return "VCMPEQ_USSS";
	case I_VCMPFALSEPD: return "VCMPFALSEPD";
	case I_VCMPFALSEPS: return "VCMPFALSEPS";
	case I_VCMPFALSESD: return "VCMPFALSESD";
	case I_VCMPFALSESS: return "VCMPFALSESS";
	case I_VCMPFALSE_OSPD: return "VCMPFALSE_OSPD";
	case I_VCMPFALSE_OSPS: return "VCMPFALSE_OSPS";
	case I_VCMPFALSE_OSSD: return "VCMPFALSE_OSSD";
	case I_VCMPFALSE_OSSS: return "VCMPFALSE_OSSS";
	case I_VCMPGEPD: return "VCMPGEPD";
	case I_VCMPGEPS: return "VCMPGEPS";
	case I_VCMPGESD: return "VCMPGESD";
	case I_VCMPGESS: return "VCMPGESS";
	case I_VCMPGE_OQPD: return "VCMPGE_OQPD";
	case I_VCMPGE_OQPS: return "VCMPGE_OQPS";
	case I_VCMPGE_OQSD: return "VCMPGE_OQSD";
	case I_VCMPGE_OQSS: return "VCMPGE_OQSS";
	case I_VCMPGTPD: return "VCMPGTPD";
	case I_VCMPGTPS: return "VCMPGTPS";
	case I_VCMPGTSD: return "VCMPGTSD";
	case I_VCMPGTSS: return "VCMPGTSS";
	case I_VCMPGT_OQPD: return "VCMPGT_OQPD";
	case I_VCMPGT_OQPS: return "VCMPGT_OQPS";
	case I_VCMPGT_OQSD: return "VCMPGT_OQSD";
	case I_VCMPGT_OQSS: return "VCMPGT_OQSS";
	case I_VCMPLEPD: return "VCMPLEPD";
	case I_VCMPLEPS: return "VCMPLEPS";
	case I_VCMPLESD: return "VCMPLESD";
	case I_VCMPLESS: return "VCMPLESS";
	case I_VCMPLE_OQPD: return "VCMPLE_OQPD";
	case I_VCMPLE_OQPS: return "VCMPLE_OQPS";
	case I_VCMPLE_OQSD: return "VCMPLE_OQSD";
	case I_VCMPLE_OQSS: return "VCMPLE_OQSS";
	case I_VCMPLTPD: return "VCMPLTPD";
	case I_VCMPLTPS: return "VCMPLTPS";
	case I_VCMPLTSD: return "VCMPLTSD";
	case I_VCMPLTSS: return "VCMPLTSS";
	case I_VCMPLT_OQPD: return "VCMPLT_OQPD";
	case I_VCMPLT_OQPS: return "VCMPLT_OQPS";
	case I_VCMPLT_OQSD: return "VCMPLT_OQSD";
	case I_VCMPLT_OQSS: return "VCMPLT_OQSS";
	case I_VCMPNEQPD: return "VCMPNEQPD";
	case I_VCMPNEQPS: return "VCMPNEQPS";
	case I_VCMPNEQSD: return "VCMPNEQSD";
	case I_VCMPNEQSS: return "VCMPNEQSS";
	case I_VCMPNEQ_OQPD: return "VCMPNEQ_OQPD";
	case I_VCMPNEQ_OQPS: return "VCMPNEQ_OQPS";
	case I_VCMPNEQ_OQSD: return "VCMPNEQ_OQSD";
	case I_VCMPNEQ_OQSS: return "VCMPNEQ_OQSS";
	case I_VCMPNEQ_OSPD: return "VCMPNEQ_OSPD";
	case I_VCMPNEQ_OSPS: return "VCMPNEQ_OSPS";
	case I_VCMPNEQ_OSSD: return "VCMPNEQ_OSSD";
	case I_VCMPNEQ_OSSS: return "VCMPNEQ_OSSS";
	case I_VCMPNEQ_USPD: return "VCMPNEQ_USPD";
	case I_VCMPNEQ_USPS: return "VCMPNEQ_USPS";
	case I_VCMPNEQ_USSD: return "VCMPNEQ_USSD";
	case I_VCMPNEQ_USSS: return "VCMPNEQ_USSS";
	case I_VCMPNGEPD: return "VCMPNGEPD";
	case I_VCMPNGEPS: return "VCMPNGEPS";
	case I_VCMPNGESD: return "VCMPNGESD";
	case I_VCMPNGESS: return "VCMPNGESS";
	case I_VCMPNGE_UQPD: return "VCMPNGE_UQPD";
	case I_VCMPNGE_UQPS: return "VCMPNGE_UQPS";
	case I_VCMPNGE_UQSD: return "VCMPNGE_UQSD";
	case I_VCMPNGE_UQSS: return "VCMPNGE_UQSS";
	case I_VCMPNGTPD: return "VCMPNGTPD";
	case I_VCMPNGTPS: return "VCMPNGTPS";
	case I_VCMPNGTSD: return "VCMPNGTSD";
	case I_VCMPNGTSS: return "VCMPNGTSS";
	case I_VCMPNGT_UQPD: return "VCMPNGT_UQPD";
	case I_VCMPNGT_UQPS: return "VCMPNGT_UQPS";
	case I_VCMPNGT_UQSD: return "VCMPNGT_UQSD";
	case I_VCMPNGT_UQSS: return "VCMPNGT_UQSS";
	case I_VCMPNLEPD: return "VCMPNLEPD";
	case I_VCMPNLEPS: return "VCMPNLEPS";
	case I_VCMPNLESD: return "VCMPNLESD";
	case I_VCMPNLESS: return "VCMPNLESS";
	case I_VCMPNLE_UQPD: return "VCMPNLE_UQPD";
	case I_VCMPNLE_UQPS: return "VCMPNLE_UQPS";
	case I_VCMPNLE_UQSD: return "VCMPNLE_UQSD";
	case I_VCMPNLE_UQSS: return "VCMPNLE_UQSS";
	case I_VCMPNLTPD: return "VCMPNLTPD";
	case I_VCMPNLTPS: return "VCMPNLTPS";
	case I_VCMPNLTSD: return "VCMPNLTSD";
	case I_VCMPNLTSS: return "VCMPNLTSS";
	case I_VCMPNLT_UQPD: return "VCMPNLT_UQPD";
	case I_VCMPNLT_UQPS: return "VCMPNLT_UQPS";
	case I_VCMPNLT_UQSD: return "VCMPNLT_UQSD";
	case I_VCMPNLT_UQSS: return "VCMPNLT_UQSS";
	case I_VCMPORDPD: return "VCMPORDPD";
	case I_VCMPORDPS: return "VCMPORDPS";
	case I_VCMPORDSD: return "VCMPORDSD";
	case I_VCMPORDSS: return "VCMPORDSS";
	case I_VCMPORD_SPD: return "VCMPORD_SPD";
	case I_VCMPORD_SPS: return "VCMPORD_SPS";
	case I_VCMPORD_SSD: return "VCMPORD_SSD";
	case I_VCMPORD_SSS: return "VCMPORD_SSS";
	case I_VCMPTRUEPD: return "VCMPTRUEPD";
	case I_VCMPTRUEPS: return "VCMPTRUEPS";
	case I_VCMPTRUESD: return "VCMPTRUESD";
	case I_VCMPTRUESS: return "VCMPTRUESS";
	case I_VCMPTRUE_USPD: return "VCMPTRUE_USPD";
	case I_VCMPTRUE_USPS: return "VCMPTRUE_USPS";
	case I_VCMPTRUE_USSD: return "VCMPTRUE_USSD";
	case I_VCMPTRUE_USSS: return "VCMPTRUE_USSS";
	case I_VCMPUNORDPD: return "VCMPUNORDPD";
	case I_VCMPUNORDPS: return "VCMPUNORDPS";
	case I_VCMPUNORDSD: return "VCMPUNORDSD";
	case I_VCMPUNORDSS: return "VCMPUNORDSS";
	case I_VCMPUNORD_SPD: return "VCMPUNORD_SPD";
	case I_VCMPUNORD_SPS: return "VCMPUNORD_SPS";
	case I_VCMPUNORD_SSD: return "VCMPUNORD_SSD";
	case I_VCMPUNORD_SSS: return "VCMPUNORD_SSS";
	case I_VCOMISD: return "VCOMISD";
	case I_VCOMISS: return "VCOMISS";
	case I_VCVTDQ2PD: return "VCVTDQ2PD";
	case I_VCVTDQ2PS: return "VCVTDQ2PS";
	case I_VCVTPD2DQ: return "VCVTPD2DQ";
	case I_VCVTPD2PS: return "VCVTPD2PS";
	case I_VCVTPS2DQ: return "VCVTPS2DQ";
	case I_VCVTPS2PD: return "VCVTPS2PD";
	case I_VCVTSD2SI: return "VCVTSD2SI";
	case I_VCVTSD2SS: return "VCVTSD2SS";
	case I_VCVTSI2SD: return "VCVTSI2SD";
	case I_VCVTSI2SS: return "VCVTSI2SS";
	case I_VCVTSS2SD: return "VCVTSS2SD";
	case I_VCVTSS2SI: return "VCVTSS2SI";
	case I_VCVTTPD2DQ: return "VCVTTPD2DQ";
	case I_VCVTTPS2DQ: return "VCVTTPS2DQ";
	case I_VCVTTSD2SI: return "VCVTTSD2SI";
	case I_VCVTTSS2SI: return "VCVTTSS2SI";
	case I_VDIVPD: return "VDIVPD";
	case I_VDIVPS: return "VDIVPS";
	case I_VDIVSD: return "VDIVSD";
	case I_VDIVSS: return "VDIVSS";
	case I_VDPPD: return "VDPPD";
	case I_VDPPS: return "VDPPS";
	case I_VERR: return "VERR";
	case I_VERW: return "VERW";
	case I_VEXTRACTF128: return "VEXTRACTF128";
	case I_VEXTRACTPS: return "VEXTRACTPS";
	case I_VFMADD132PD: return "VFMADD132PD";
	case I_VFMADD132PS: return "VFMADD132PS";
	case I_VFMADD132SD: return "VFMADD132SD";
	case I_VFMADD132SS: return "VFMADD132SS";
	case I_VFMADD213PD: return "VFMADD213PD";
	case I_VFMADD213PS: return "VFMADD213PS";
	case I_VFMADD213SD: return "VFMADD213SD";
	case I_VFMADD213SS: return "VFMADD213SS";
	case I_VFMADD231PD: return "VFMADD231PD";
	case I_VFMADD231PS: return "VFMADD231PS";
	case I_VFMADD231SD: return "VFMADD231SD";
	case I_VFMADD231SS: return "VFMADD231SS";
	case I_VFMADDSUB132PD: return "VFMADDSUB132PD";
	case I_VFMADDSUB132PS: return "VFMADDSUB132PS";
	case I_VFMADDSUB213PD: return "VFMADDSUB213PD";
	case I_VFMADDSUB213PS: return "VFMADDSUB213PS";
	case I_VFMADDSUB231PD: return "VFMADDSUB231PD";
	case I_VFMADDSUB231PS: return "VFMADDSUB231PS";
	case I_VFMSUB132PD: return "VFMSUB132PD";
	case I_VFMSUB132PS: return "VFMSUB132PS";
	case I_VFMSUB132SD: return "VFMSUB132SD";
	case I_VFMSUB132SS: return "VFMSUB132SS";
	case I_VFMSUB213PD: return "VFMSUB213PD";
	case I_VFMSUB213PS: return "VFMSUB213PS";
	case I_VFMSUB213SD: return "VFMSUB213SD";
	case I_VFMSUB213SS: return "VFMSUB213SS";
	case I_VFMSUB231PD: return "VFMSUB231PD";
	case I_VFMSUB231PS: return "VFMSUB231PS";
	case I_VFMSUB231SD: return "VFMSUB231SD";
	case I_VFMSUB231SS: return "VFMSUB231SS";
	case I_VFMSUBADD132PD: return "VFMSUBADD132PD";
	case I_VFMSUBADD132PS: return "VFMSUBADD132PS";
	case I_VFMSUBADD213PD: return "VFMSUBADD213PD";
	case I_VFMSUBADD213PS: return "VFMSUBADD213PS";
	case I_VFMSUBADD231PD: return "VFMSUBADD231PD";
	case I_VFMSUBADD231PS: return "VFMSUBADD231PS";
	case I_VFNMADD132PD: return "VFNMADD132PD";
	case I_VFNMADD132PS: return "VFNMADD132PS";
	case I_VFNMADD132SD: return "VFNMADD132SD";
	case I_VFNMADD132SS: return "VFNMADD132SS";
	case I_VFNMADD213PD: return "VFNMADD213PD";
	case I_VFNMADD213PS: return "VFNMADD213PS";
	case I_VFNMADD213SD: return "VFNMADD213SD";
	case I_VFNMADD213SS: return "VFNMADD213SS";
	case I_VFNMADD231PD: return "VFNMADD231PD";
	case I_VFNMADD231PS: return "VFNMADD231PS";
	case I_VFNMADD231SD: return "VFNMADD231SD";
	case I_VFNMADD231SS: return "VFNMADD231SS";
	case I_VFNMSUB132PD: return "VFNMSUB132PD";
	case I_VFNMSUB132PS: return "VFNMSUB132PS";
	case I_VFNMSUB132SD: return "VFNMSUB132SD";
	case I_VFNMSUB132SS: return "VFNMSUB132SS";
	case I_VFNMSUB213PD: return "VFNMSUB213PD";
	case I_VFNMSUB213PS: return "VFNMSUB213PS";
	case I_VFNMSUB213SD: return "VFNMSUB213SD";
	case I_VFNMSUB213SS: return "VFNMSUB213SS";
	case I_VFNMSUB231PD: return "VFNMSUB231PD";
	case I_VFNMSUB231PS: return "VFNMSUB231PS";
	case I_VFNMSUB231SD: return "VFNMSUB231SD";
	case I_VFNMSUB231SS: return "VFNMSUB231SS";
	case I_VHADDPD: return "VHADDPD";
	case I_VHADDPS: return "VHADDPS";
	case I_VHSUBPD: return "VHSUBPD";
	case I_VHSUBPS: return "VHSUBPS";
	case I_VINSERTF128: return "VINSERTF128";
	case I_VINSERTPS: return "VINSERTPS";
	case I_VLDDQU: return "VLDDQU";
	case I_VLDMXCSR: return "VLDMXCSR";
	case I_VMASKMOVDQU: return "VMASKMOVDQU";
	case I_VMASKMOVPD: return "VMASKMOVPD";
	case I_VMASKMOVPS: return "VMASKMOVPS";
	case I_VMAXPD: return "VMAXPD";
	case I_VMAXPS: return "VMAXPS";
	case I_VMAXSD: return "VMAXSD";
	case I_VMAXSS: return "VMAXSS";
	case I_VMCALL: return "VMCALL";
	case I_VMCLEAR: return "VMCLEAR";
	case I_VMFUNC: return "VMFUNC";
	case I_VMINPD: return "VMINPD";
	case I_VMINPS: return "VMINPS";
	case I_VMINSD: return "VMINSD";
	case I_VMINSS: return "VMINSS";
	case I_VMLAUNCH: return "VMLAUNCH";
	case I_VMLOAD: return "VMLOAD";
	case I_VMMCALL: return "VMMCALL";
	case I_VMOVAPD: return "VMOVAPD";
	case I_VMOVAPS: return "VMOVAPS";
	case I_VMOVD: return "VMOVD";
	case I_VMOVDDUP: return "VMOVDDUP";
	case I_VMOVDQA: return "VMOVDQA";
	case I_VMOVDQU: return "VMOVDQU";
	case I_VMOVHLPS: return "VMOVHLPS";
	case I_VMOVHPD: return "VMOVHPD";
	case I_VMOVHPS: return "VMOVHPS";
	case I_VMOVLHPS: return "VMOVLHPS";
	case I_VMOVLPD: return "VMOVLPD";
	case I_VMOVLPS: return "VMOVLPS";
	case I_VMOVMSKPD: return "VMOVMSKPD";
	case I_VMOVMSKPS: return "VMOVMSKPS";
	case I_VMOVNTDQ: return "VMOVNTDQ";
	case I_VMOVNTDQA: return "VMOVNTDQA";
	case I_VMOVNTPD: return "VMOVNTPD";
	case I_VMOVNTPS: return "VMOVNTPS";
	case I_VMOVQ: return "VMOVQ";
	case I_VMOVSD: return "VMOVSD";
	case I_VMOVSHDUP: return "VMOVSHDUP";
	case I_VMOVSLDUP: return "VMOVSLDUP";
	case I_VMOVSS: return "VMOVSS";
	case I_VMOVUPD: return "VMOVUPD";
	case I_VMOVUPS: return "VMOVUPS";
	case I_VMPSADBW: return "VMPSADBW";
	case I_VMPTRLD: return "VMPTRLD";
	case I_VMPTRST: return "VMPTRST";
	case I_VMREAD: return "VMREAD";
	case I_VMRESUME: return "VMRESUME";
	case I_VMRUN: return "VMRUN";
	case I_VMSAVE: return "VMSAVE";
	case I_VMULPD: return "VMULPD";
	case I_VMULPS: return "VMULPS";
	case I_VMULSD: return "VMULSD";
	case I_VMULSS: return "VMULSS";
	case I_VMWRITE: return "VMWRITE";
	case I_VMXOFF: return "VMXOFF";
	case I_VMXON: return "VMXON";
	case I_VORPD: return "VORPD";
	case I_VORPS: return "VORPS";
	case I_VPABSB: return "VPABSB";
	case I_VPABSD: return "VPABSD";
	case I_VPABSW: return "VPABSW";
	case I_VPACKSSDW: return "VPACKSSDW";
	case I_VPACKSSWB: return "VPACKSSWB";
	case I_VPACKUSDW: return "VPACKUSDW";
	case I_VPACKUSWB: return "VPACKUSWB";
	case I_VPADDB: return "VPADDB";
	case I_VPADDD: return "VPADDD";
	case I_VPADDQ: return "VPADDQ";
	case I_VPADDSB: return "VPADDSB";
	case I_VPADDSW: return "VPADDSW";
	case I_VPADDUSW: return "VPADDUSW";
	case I_VPADDW: return "VPADDW";
	case I_VPALIGNR: return "VPALIGNR";
	case I_VPAND: return "VPAND";
	case I_VPANDN: return "VPANDN";
	case I_VPAVGB: return "VPAVGB";
	case I_VPAVGW: return "VPAVGW";
	case I_VPBLENDVB: return "VPBLENDVB";
	case I_VPBLENDW: return "VPBLENDW";
	case I_VPCLMULQDQ: return "VPCLMULQDQ";
	case I_VPCMPEQB: return "VPCMPEQB";
	case I_VPCMPEQD: return "VPCMPEQD";
	case I_VPCMPEQQ: return "VPCMPEQQ";
	case I_VPCMPEQW: return "VPCMPEQW";
	case I_VPCMPESTRI: return "VPCMPESTRI";
	case I_VPCMPESTRM: return "VPCMPESTRM";
	case I_VPCMPGTB: return "VPCMPGTB";
	case I_VPCMPGTD: return "VPCMPGTD";
	case I_VPCMPGTQ: return "VPCMPGTQ";
	case I_VPCMPGTW: return "VPCMPGTW";
	case I_VPCMPISTRI: return "VPCMPISTRI";
	case I_VPCMPISTRM: return "VPCMPISTRM";
	case I_VPERM2F128: return "VPERM2F128";
	case I_VPERMILPD: return "VPERMILPD";
	case I_VPERMILPS: return "VPERMILPS";
	case I_VPEXTRB: return "VPEXTRB";
	case I_VPEXTRD: return "VPEXTRD";
	case I_VPEXTRQ: return "VPEXTRQ";
	case I_VPEXTRW: return "VPEXTRW";
	case I_VPHADDD: return "VPHADDD";
	case I_VPHADDSW: return "VPHADDSW";
	case I_VPHADDW: return "VPHADDW";
	case I_VPHMINPOSUW: return "VPHMINPOSUW";
	case I_VPHSUBD: return "VPHSUBD";
	case I_VPHSUBSW: return "VPHSUBSW";
	case I_VPHSUBW: return "VPHSUBW";
	case I_VPINSRB: return "VPINSRB";
	case I_VPINSRD: return "VPINSRD";
	case I_VPINSRQ: return "VPINSRQ";
	case I_VPINSRW: return "VPINSRW";
	case I_VPMADDUBSW: return "VPMADDUBSW";
	case I_VPMADDWD: return "VPMADDWD";
	case I_VPMAXSB: return "VPMAXSB";
	case I_VPMAXSD: return "VPMAXSD";
	case I_VPMAXSW: return "VPMAXSW";
	case I_VPMAXUB: return "VPMAXUB";
	case I_VPMAXUD: return "VPMAXUD";
	case I_VPMAXUW: return "VPMAXUW";
	case I_VPMINSB: return "VPMINSB";
	case I_VPMINSD: return "VPMINSD";
	case I_VPMINSW: return "VPMINSW";
	case I_VPMINUB: return "VPMINUB";
	case I_VPMINUD: return "VPMINUD";
	case I_VPMINUW: return "VPMINUW";
	case I_VPMOVMSKB: return "VPMOVMSKB";
	case I_VPMOVSXBD: return "VPMOVSXBD";
	case I_VPMOVSXBQ: return "VPMOVSXBQ";
	case I_VPMOVSXBW: return "VPMOVSXBW";
	case I_VPMOVSXDQ: return "VPMOVSXDQ";
	case I_VPMOVSXWD: return "VPMOVSXWD";
	case I_VPMOVSXWQ: return "VPMOVSXWQ";
	case I_VPMOVZXBD: return "VPMOVZXBD";
	case I_VPMOVZXBQ: return "VPMOVZXBQ";
	case I_VPMOVZXBW: return "VPMOVZXBW";
	case I_VPMOVZXDQ: return "VPMOVZXDQ";
	case I_VPMOVZXWD: return "VPMOVZXWD";
	case I_VPMOVZXWQ: return "VPMOVZXWQ";
	case I_VPMULDQ: return "VPMULDQ";
	case I_VPMULHRSW: return "VPMULHRSW";
	case I_VPMULHUW: return "VPMULHUW";
	case I_VPMULHW: return "VPMULHW";
	case I_VPMULLD: return "VPMULLD";
	case I_VPMULLW: return "VPMULLW";
	case I_VPMULUDQ: return "VPMULUDQ";
	case I_VPOR: return "VPOR";
	case I_VPSADBW: return "VPSADBW";
	case I_VPSHUFB: return "VPSHUFB";
	case I_VPSHUFD: return "VPSHUFD";
	case I_VPSHUFHW: return "VPSHUFHW";
	case I_VPSHUFLW: return "VPSHUFLW";
	case I_VPSIGNB: return "VPSIGNB";
	case I_VPSIGND: return "VPSIGND";
	case I_VPSIGNW: return "VPSIGNW";
	case I_VPSLLD: return "VPSLLD";
	case I_VPSLLDQ: return "VPSLLDQ";
	case I_VPSLLQ: return "VPSLLQ";
	case I_VPSLLW: return "VPSLLW";
	case I_VPSRAD: return "VPSRAD";
	case I_VPSRAW: return "VPSRAW";
	case I_VPSRLD: return "VPSRLD";
	case I_VPSRLDQ: return "VPSRLDQ";
	case I_VPSRLQ: return "VPSRLQ";
	case I_VPSRLW: return "VPSRLW";
	case I_VPSUBB: return "VPSUBB";
	case I_VPSUBD: return "VPSUBD";
	case I_VPSUBQ: return "VPSUBQ";
	case I_VPSUBSB: return "VPSUBSB";
	case I_VPSUBSW: return "VPSUBSW";
	case I_VPSUBUSB: return "VPSUBUSB";
	case I_VPSUBUSW: return "VPSUBUSW";
	case I_VPSUBW: return "VPSUBW";
	case I_VPTEST: return "VPTEST";
	case I_VPUNPCKHBW: return "VPUNPCKHBW";
	case I_VPUNPCKHDQ: return "VPUNPCKHDQ";
	case I_VPUNPCKHQDQ: return "VPUNPCKHQDQ";
	case I_VPUNPCKHWD: return "VPUNPCKHWD";
	case I_VPUNPCKLBW: return "VPUNPCKLBW";
	case I_VPUNPCKLDQ: return "VPUNPCKLDQ";
	case I_VPUNPCKLQDQ: return "VPUNPCKLQDQ";
	case I_VPUNPCKLWD: return "VPUNPCKLWD";
	case I_VPXOR: return "VPXOR";
	case I_VRCPPS: return "VRCPPS";
	case I_VRCPSS: return "VRCPSS";
	case I_VROUNDPD: return "VROUNDPD";
	case I_VROUNDPS: return "VROUNDPS";
	case I_VROUNDSD: return "VROUNDSD";
	case I_VROUNDSS: return "VROUNDSS";
	case I_VRSQRTPS: return "VRSQRTPS";
	case I_VRSQRTSS: return "VRSQRTSS";
	case I_VSHUFPD: return "VSHUFPD";
	case I_VSHUFPS: return "VSHUFPS";
	case I_VSQRTPD: return "VSQRTPD";
	case I_VSQRTPS: return "VSQRTPS";
	case I_VSQRTSD: return "VSQRTSD";
	case I_VSQRTSS: return "VSQRTSS";
	case I_VSTMXCSR: return "VSTMXCSR";
	case I_VSUBPD: return "VSUBPD";
	case I_VSUBPS: return "VSUBPS";
	case I_VSUBSD: return "VSUBSD";
	case I_VSUBSS: return "VSUBSS";
	case I_VTESTPD: return "VTESTPD";
	case I_VTESTPS: return "VTESTPS";
	case I_VUCOMISD: return "VUCOMISD";
	case I_VUCOMISS: return "VUCOMISS";
	case I_VUNPCKHPD: return "VUNPCKHPD";
	case I_VUNPCKHPS: return "VUNPCKHPS";
	case I_VUNPCKLPD: return "VUNPCKLPD";
	case I_VUNPCKLPS: return "VUNPCKLPS";
	case I_VXORPD: return "VXORPD";
	case I_VXORPS: return "VXORPS";
	case I_VZEROALL: return "VZEROALL";
	case I_VZEROUPPER: return "VZEROUPPER";
	case I_WAIT: return "WAIT";
	case I_WBINVD: return "WBINVD";
	case I_WRFSBASE: return "WRFSBASE";
	case I_WRGSBASE: return "WRGSBASE";
	case I_WRMSR: return "WRMSR";
	case I_XABORT: return "XABORT";
	case I_XADD: return "XADD";
	case I_XBEGIN: return "XBEGIN";
	case I_XCHG: return "XCHG";
	case I_XEND: return "XEND";
	case I_XGETBV: return "XGETBV";
	case I_XLAT: return "XLAT";
	case I_XOR: return "XOR";
	case I_XORPD: return "XORPD";
	case I_XORPS: return "XORPS";
	case I_XRSTOR: return "XRSTOR";
	case I_XRSTOR64: return "XRSTOR64";
	case I_XSAVE: return "XSAVE";
	case I_XSAVE64: return "XSAVE64";
	case I_XSAVEOPT: return "XSAVEOPT";
	case I_XSAVEOPT64: return "XSAVEOPT64";
	case I_XSETBV: return "XSETBV";
	default: assert(false); return "???";
	}
}

void output_value(std::stringstream &output, int nibbles, uint32_t value)
{
	if (value < 0xA)
		// Show 0 upto 9 as 1 decimal digit
		output << std::setw(1) << value;
	else
		output << std::setfill('0') << std::setw(nibbles) << std::right << value << 'h';
}

void output_value_disp(std::stringstream &output, _DInst &info)
{
	uint32_t value = EmuX86_Distorm_read_disp(info);
	if (value != 0) {
		int nibbles = info.dispSize / 4;
		assert(nibbles == 2 || nibbles == 4 || nibbles == 8);
		if ((int32_t)value < 0) {
			output << '-';
			output_value(output, nibbles, value);
		}
		else {
			output << '+';
			output_value(output, nibbles, value);
		}
	}
}

void output_segment(std::stringstream &output, _DInst &info)
{
	if (SEGMENT_GET(info.segment) != R_NONE)
		output << Distorm_RegStrings[SEGMENT_GET(info.segment)] << ":";
}

void EmuX86_DistormLogInstruction(const uint8_t *Eip, _DInst &info, LOG_LEVEL log_level)
{
	std::stringstream output;

	output << "Disassembly : "
		<< std::setfill('0') << std::setw(8) << std::right << std::hex << std::uppercase << (xbox::addr_xt)Eip;
	for (int b = 0; b < MAX(7, info.size); b++) {
		if (b < info.size)
			//output << " " << std::setfill('0') << std::setw(2) << std::right << std::hex << ((uint8_t)0+Eip[b]); // add 2 hex nibbles, not chars
			output << " " << std::setw(2) << ((uint8_t)0+Eip[b]); // add 2 hex nibbles, not chars
		else
			output << "   ";
	}

	output << std::setfill(' ') << std::left << std::setw(1) << " ";
	if (info.flags & FLAG_LOCK)
		output << "LOCK ";

	if (info.flags & FLAG_REPNZ)
		output << "REPNZ ";

	if (info.flags & FLAG_REP) {
		switch (info.opcode) {
		case I_CMPS:
		case I_SCAS:
			output << "REPZ ";
			break;
		default:
			output << "REP ";
		}
	}

	output << std::setw(11) << Distorm_OpcodeString(info.opcode);
	for (int o = 0; o < 4 && info.ops[o].type != O_NONE; o++) {
		// Convert size in bits to (hexadecimal) nibble count and a size-indicator string
		int nr_nibbles;
		std::string size_str;

		switch (info.ops[o].size) {
		case 0: nr_nibbles = 8; size_str = ""; break;
		case 8: nr_nibbles = 2; size_str = "byte ptr "; break;
		case 16: nr_nibbles = 4; size_str = "word ptr "; break;
		case 32: nr_nibbles = 8; size_str = "dword ptr "; break;
		default: assert(false);
		}

		// Output register/operand separator
		output << std::setfill(' ') << std::setw(1) << std::right << ((o == 0) ? " " : ",");
		// Render operand to output
		switch (info.ops[o].type) {
		case O_REG:
			output << Distorm_RegStrings[info.ops[o].index];
			break;
		case O_IMM:
			output_value(output, nr_nibbles, info.imm.dword);
			break;
		case O_IMM1: // TODO : Needs test-case
			output_value(output, nr_nibbles, info.imm.ex.i1);
			break;
		case O_IMM2: // TODO : Needs test-case
			output_value(output, nr_nibbles, info.imm.ex.i2);
			break;
		case O_DISP:
			output << size_str;
			output_segment(output, info); // Was << "ds:", so no check on default
			output << "[";
			output_value(output, info.dispSize > 0 ? info.dispSize / 4 : 8, EmuX86_Distorm_read_disp(info));
			output << "]";
			break;
		case O_SMEM:
			output << size_str << "[";
			if (!SEGMENT_IS_DEFAULT(info.segment))
				output_segment(output, info);
			output << Distorm_RegStrings[info.ops[o].index];
			output_value_disp(output, info);
			output << "]";
			break;
		case O_MEM:
			output << size_str << "[";
			if (!SEGMENT_IS_DEFAULT(info.segment))
				output_segment(output, info);
			if (info.base != R_NONE)
				output << Distorm_RegStrings[info.base] << "+";
			output << Distorm_RegStrings[info.ops[o].index];
			if (info.scale >= 2) {
				output << "*";
				output_value(output, 1/*nibble*/, info.scale);
			}
			output_value_disp(output, info);
			output << "]";
			break;
		case O_PC:
			output_value(output, 8, (xbox::addr_xt)Eip + (xbox::addr_xt)INSTRUCTION_GET_TARGET(&info));
			break;
		case O_PTR: // TODO : Needs test-case
			output << "+" << std::setfill('0') << info.imm.ptr.seg << "/";
			output_value(output, nr_nibbles, info.imm.ptr.off);
			break;
		default:
			assert(false);
			output << "?";
			break;
		}
	}

	if (FLAG_GET_PRIVILEGED(info.flags))
		output << " // Privileged ring 0 instruction";

/*
#define FLAG_LOCK (1 << 0) // The instruction locks memory access.
#define FLAG_REPNZ (1 << 1) // The instruction is prefixed with a REPNZ.
#define FLAG_REP (1 << 2) // The instruction is prefixed with a REP, this can be a REPZ, it depends on the specific instruction.
#define FLAG_HINT_TAKEN (1 << 3) // Indicates there is a hint taken for Jcc instructions only.
#define FLAG_HINT_NOT_TAKEN (1 << 4) // Indicates there is a hint non-taken for Jcc instructions only.
#define FLAG_IMM_SIGNED (1 << 5) // The Imm value is signed extended (E.G in 64 bit decoding mode, a 32 bit imm is usually sign extended into 64 bit imm).
#define FLAG_DST_WR (1 << 6) // The destination operand is writable.
#define FLAG_GET_OPSIZE(flags) (((flags) >> 8) & 3)
#define FLAG_GET_ADDRSIZE(flags) (((flags) >> 10) & 3)
#define FLAG_GET_PREFIX(flags) ((flags) & 7) // To get the LOCK/REPNZ/REP prefixes.
*/

	EmuLog(log_level, output.str().c_str());
}

int EmuX86_OpcodeSize(uint8_t *Eip)
{
	_DInst info;
	if (EmuX86_DecodeOpcode((uint8_t*)Eip, info))
		return info.size;

	EmuLog(LOG_LEVEL::WARNING, "Error decoding opcode size at 0x%.8X", Eip);
	return 1;
}

bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e)
{
	// Decoded instruction information.
	// Opcode handler note : 
	// If an opcode or one of it's operand can't be decoded, that's a clear failure.
	// However, if for any reason, an opcode operand cannot be read from or written to,
	// that case may be logged, but it shouldn't fail the opcode handler.
	_DInst info;
	g_tls_isEmuX86Managed = true;
	DWORD StartingEip = e->ContextRecord->Eip;
	EmuLog(LOG_LEVEL::DEBUG, "Starting instruction emulation from 0x%08X", e->ContextRecord->Eip);

	// Execute op-codes until we hit an unhandled instruction, or an error occurs
	//while (true)
	// TODO: Find where the weird memory addresses come from when using the above case
	// There is obviously something wrong with one or more of our instruction implementations
	// For now, we only execute one instruction at a time...
	for (int x=0;x<1;x++)
	{
		if (!EmuX86_DecodeOpcode((uint8_t*)e->ContextRecord->Eip, info)) {
			EmuLog(LOG_LEVEL::WARNING, "Error decoding opcode at 0x%08X", e->ContextRecord->Eip);
			assert(false);
			return false;
		}

		LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) {
			EmuX86_DistormLogInstruction((uint8_t*)e->ContextRecord->Eip, info, LOG_LEVEL::DEBUG);
		}

		switch (info.opcode) { // Keep these cases alphabetically ordered and condensed
			case I_ADD:
				if (EmuX86_Opcode_ADD(e, info)) break;
				goto opcode_error;
			case I_AND:
				if (EmuX86_Opcode_AND(e, info)) break;
				goto opcode_error;
			case I_CALL:
				// RET and CALL always signify the end of a code block
				return true;
			case I_CDQ: // = 250 : 	Convert Doubleword to Quadword
				EmuX86_Opcode_CDQ(e, info);
				break;
			case I_CLI: { // = 502 : Disable all interrupts
				EmuX86_Opcode_CLI();
				break;
			}
			case I_CMP:
				if (EmuX86_Opcode_CMP(e, info)) break;
				goto opcode_error;
			case I_CMPXCHG:
				if (EmuX86_Opcode_CMPXCHG(e, info)) break;
				goto opcode_error;
			case I_CPUID:
				EmuX86_Opcode_CPUID(e, info);
				break;
			case I_DEC:
				if (EmuX86_Opcode_DEC(e, info)) break;
				goto opcode_error;
#if 0 // TODO : Implement EmuX86_Opcode_IMUL and enable this :
			case I_IMUL: { // = 117 : 	Signed Multiply
				if (EmuX86_Opcode_IMUL(e, info)) break;
				goto opcode_error;
			}
#endif
			case I_IN:
				if (EmuX86_Opcode_IN(e, info)) break;
				goto opcode_error;
			case I_INC:
				if (EmuX86_Opcode_INC(e, info)) break;
				goto opcode_error;
			case I_INVD: // = 555 : Flush internal caches; initiate flushing of external caches.
				break; // Privileged Level (Ring 0) Instruction. Causes a priviledge instruction exception - We can safely ignore this
			case I_INVLPG: { // = 1727
				// This instruction invalidates the TLB entry specified with the source operand. Since we don't emulate
				// the TLB yet, we can safely ignore this. Test case: Fable.
				break;
			}
			case I_JA: { // = 166 : Jump if above (CF=0 and ZF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_CF(e) && !EmuX86_HasFlag_ZF(e))) {
					continue;
				}
				break;
			}
			case I_JAE: { // = 147 : Jump if above or equal (CF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_CF(e))) {
					continue;
				}
				break;
			}
			case I_JB: { // = 143 : Jump if below (CF=1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_CF(e))) {
					continue;
				}
				break;
			}
			case I_JBE: { // = 161 : Jump if below or equal (CF=1 or ZF=1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_CF(e) || EmuX86_HasFlag_ZF(e))) {
					continue;
				}
				break;
			}
			case I_JCXZ: { // = 427 : Jump if CX register is 0.
				if (EmuX86_Opcode_Jcc(e, info, ((e->ContextRecord->Ecx & 0xFF) == 0))) {
					continue;
				}
				break;
			}
			case I_JECXZ: { // = 433 : Jump if ECX register is 0.
				if (EmuX86_Opcode_Jcc(e, info, e->ContextRecord->Ecx == 0)) {
					continue;
				}
				break;
			}
			case I_JG: { // = 202 : Jump if greater (ZF=0 and SF=OF).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_ZF(e) && (EmuX86_HasFlag_SF(e) == EmuX86_HasFlag_OF(e)))) {
					continue;
				}
				break;
			}
			case I_JGE: { // = 192 : Jump if greater or equal (SF=OF).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_SF(e) == EmuX86_HasFlag_OF(e))) {
					continue;
				}
				break;
			}
			case I_JL: { // = 188 : Jump if less (SF<>OF).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_SF(e) != EmuX86_HasFlag_OF(e))) {
					continue;
				}
				break;
			}
			case I_JLE: { // = 197 : Jump if less or equal (ZF=1 or SF<>OF).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_ZF(e) || (EmuX86_HasFlag_SF(e) != EmuX86_HasFlag_OF(e)))) {
					continue;
				}
				break;
			}
			case I_JMP: // = 462 : 	Jump
			case I_JMP_FAR: { // = 467 : Jump
				if (EmuX86_Opcode_JMP(e, info)) {
					continue;
				}
				break;
			}
			case I_JNO: { // = 138 : Jump if not overflow (OF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_OF(e))) {
					continue;
				}
				break;
			}
			case I_JNP: { // = 183 : Jump if not parity (PF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_PF(e))) {
					continue;
				}
				break;
			}
			case I_JNS: { // = 174 : Jump if not sign (SF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_SF(e))) {
					continue;
				}
				break;
			}
			case I_JNZ: { // = 156 : Jump if not zero (ZF=0).
				if (EmuX86_Opcode_Jcc(e, info, !EmuX86_HasFlag_ZF(e))) {
					continue;
				}
				break;
			}
			case I_JO: { // = 134 : Jump if overflow (OF=1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_OF(e))) {
					continue;
				}
				break;
			}
			case I_JP: { // = 179 : Jump if parity (PF=1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_PF(e))) {
					continue;
				}
				break;
			}
			case I_JS: { // = 170 : Jump if sign (SF=1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_SF(e))) {
					continue;
				}
				break;
			}
			case I_JZ: { // = 152 : Jump if zero (ZF = 1).
				if (EmuX86_Opcode_Jcc(e, info, EmuX86_HasFlag_ZF(e))) {
					continue;
				}
				break;
			}
			case I_LEA: { // = 223 : Load Effective Address
				if (EmuX86_Opcode_LEA(e, info)) break;
				goto opcode_error;
			}
			case I_LEAVE:
				// LEAVE often precedes RET - end of a code block
				return true;
			case I_LFENCE: { // = 4287 : Serializes load operations.
				__asm { lfence }; // emulate as-is (doesn't cause exceptions)
				break;
			}
			case I_MFENCE: { // = 4313 : Serializes load and store operations.
				__asm { mfence }; // emulate as-is (doesn't cause exceptions)
				break;
			}
			case I_MOV:
				if (EmuX86_Opcode_MOV(e, info)) break;
				goto opcode_error;
			case I_MOVSX:
				if (EmuX86_Opcode_MOVSX(e, info)) break;
				goto opcode_error;
			case I_MOVZX:
				if (EmuX86_Opcode_MOVZX(e, info)) break;
				goto opcode_error;
			case I_NEG:
				if (EmuX86_Opcode_NEG(e, info)) break;
				goto opcode_error;
			case I_NOP: 
				break;
			case I_NOT:
				if (EmuX86_Opcode_NOT(e, info)) break;
				goto opcode_error;
			case I_OR:
				if (EmuX86_Opcode_OR(e, info)) break;
				goto opcode_error;
			case I_OUT:
				if (EmuX86_Opcode_OUT(e, info)) break;
				goto opcode_error;
			case I_POP: { // = 22 : Pop a Value from the Stack
				if (EmuX86_Opcode_POP(e, info)) break;
				goto opcode_error;
			}
			case I_PUSH: { // = 16 : Push Word or Doubleword Onto the Stack
				if (EmuX86_Opcode_PUSH(e, info)) break;
				goto opcode_error;
			}
			 // TODO : case I_RDPMC: // = 607 : Read Performance-Monitoring Counters; Privileged Level (Ring 0) Instruction. Causes a priviledge instruction exception
			case I_RDTSC: // = 593 : Read Time-Stamp Counter
				EmuX86_Opcode_RDTSC(e);
				break;
			case I_RET:
				// RET and CALL always signify the end of a code block
				return true;
			case I_SAR: // = 1002 : Shift arithmetic right
				EmuX86_Opcode_SAR(e, info);
				break;
			case I_SBB:
				if (EmuX86_Opcode_SBB(e, info)) break;
				goto opcode_error;
			case I_SETA: { // Set byte if above (CF=0 and ZF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_CF(e) && !EmuX86_HasFlag_ZF(e))) break;
				goto opcode_error;
			}
			case I_SETAE: { // Set byte if above or equal (CF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_CF(e))) break;
				goto opcode_error;
			}
			case I_SETB: { // Set byte if below (CF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_CF(e))) break;
				goto opcode_error;
			}
			case I_SETBE: { // Set byte if below or equal (CF=1 or ZF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_CF(e) || EmuX86_HasFlag_ZF(e))) break;
				goto opcode_error;
			}
			case I_SETG: { // Set byte if greater (ZF=0 and SF=OF).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_ZF(e) && (EmuX86_HasFlag_SF(e) == EmuX86_HasFlag_OF(e)))) break;
				goto opcode_error;
			}
			case I_SETGE: { // Set byte if greater or equal (SF=OF).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_SF(e) == EmuX86_HasFlag_OF(e))) break;
				goto opcode_error;
			}
			case I_SETL: { // Set byte if less (SF<>OF).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_SF(e) != EmuX86_HasFlag_OF(e))) break;
				goto opcode_error;
			}
			case I_SETLE: { // Set byte if less or equal (ZF=1 or SF<>OF).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_ZF(e) || (EmuX86_HasFlag_SF(e) != EmuX86_HasFlag_OF(e)))) break;
				goto opcode_error;
			}
			case I_SETNO: { // Set byte if not overflow (OF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_OF(e))) break;
				goto opcode_error;
			}
			case I_SETNP: { // Set byte if not parity (PF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_PF(e))) break;
				goto opcode_error;
			}
			case I_SETNS: { // Set byte if not sign (SF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_SF(e))) break;
				goto opcode_error;
			}
			case I_SETNZ: { // Set byte if not zero (ZF=0).
				if (EmuX86_Opcode_SETcc(e, info, !EmuX86_HasFlag_ZF(e))) break;
				goto opcode_error;
			}
			case I_SETO: { // Set byte if overflow (OF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_OF(e))) break;
				goto opcode_error;
			}
			case I_SETP: { // Set byte if parity (PF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_PF(e))) break;
				goto opcode_error;
			}
			case I_SETS: { // Set byte if sign (SF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_SF(e))) break;
				goto opcode_error;
			}
			case I_SETZ: { // Set byte if zero (ZF=1).
				if (EmuX86_Opcode_SETcc(e, info, EmuX86_HasFlag_ZF(e))) break;
				goto opcode_error;
			}
			case I_SFENCE: { // = 4343 : Serializes store operations.
				__asm { sfence }; // emulate as-is (doesn't cause exceptions)
				break;
			}
			case I_SHL:
				if (EmuX86_Opcode_SHL(e, info)) break;
				goto opcode_error;
			case I_SHR:
				if (EmuX86_Opcode_SHR(e, info)) break;
				goto opcode_error;
			case I_STI: {
				// Enable all interrupts
				EmuX86_Opcode_STI();
				break;
			}
			case I_STOS: {
				// Handle REP prefix (if set, repeat ECX times)
				int counter = 1;
				if (FLAG_GET_PREFIX(info.flags) == FLAG_REP) {
					counter = e->ContextRecord->Ecx;
					// Make sure ECX is zero when the loop ends
					e->ContextRecord->Ecx = 0;
				}

				while (counter != 0) {
					if (EmuX86_Opcode_STOS(e, info)) {
						counter--;
						continue;
					}

					goto opcode_error;
				}
				break;
			}
			case I_SUB:
				if (EmuX86_Opcode_SUB(e, info)) break;
				goto opcode_error;
			case I_TEST:
				if (EmuX86_Opcode_TEST(e, info)) break;
				goto opcode_error;
			case I_WBINVD: // Write back and flush internal caches; initiate writing-back and flushing of external caches.
				break; // Privileged Level (Ring 0) Instruction. Causes a priviledge instruction exception - We can safely ignore this
			case I_WRMSR:
				// We do not emulate processor specific registers just yet
				// Some titles attempt to manually set the TSC via this instruction
				// This needs fixing eventually, but should be acceptible to ignore for now!
				// test-case : Chase: Hollywood Stunt Driver
				EmuLog(LOG_LEVEL::WARNING, "WRMSR instruction ignored");
				break;
			case I_XOR:
				if (EmuX86_Opcode_XOR(e, info)) break;
				goto opcode_error;
			default:
				EmuLog(LOG_LEVEL::DEBUG, "Unhandled instruction : %s (%u)", Distorm_OpcodeString(info.opcode), info.opcode);
				// HACK: If we hit an unhandled instruction, log and skip it
				e->ContextRecord->Eip += info.size;
				return true;
		} // switch info.opcode

		if (g_tls_isEmuX86Managed) {
			e->ContextRecord->Eip += info.size;
		}
		else {
			break;
		}
	} // while true

	return g_tls_isEmuX86Managed;

opcode_error:
	EmuLog(LOG_LEVEL::WARNING, "0x%08X: Error while handling instruction %s (%u)", e->ContextRecord->Eip, Distorm_OpcodeString(info.opcode), info.opcode);
	assert(false);
	return false;
}

void EmuX86_Init()
{
	EmuLog(LOG_LEVEL::DEBUG, "Initializing distorm version %d", distorm_version());

	// Initialize emulation exception to ensure they are front and last line of exception.
	g_ExceptionManager->EmuX86_Init();

	EmuX86_InitContextRecordOffsetByRegisterType();
	EmuX86_InitMemoryBackedRegisters();
}
