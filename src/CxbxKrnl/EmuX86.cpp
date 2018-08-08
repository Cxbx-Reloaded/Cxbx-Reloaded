// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuX86.cpp
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
// *  (c) 2016 Luke Usher <luke.usher@outlook.com>
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX CXBXR_MODULE::X86

// Link the library into our project.
#pragma comment(lib, "distorm.lib")

// Cxbx uses dynamic linking of distorm, which by default chooses for 64 bits offsets :
#define SUPPORT_64BIT_OFFSET

#include "distorm.h"
#include "mnemonics.h"

#include "CxbxKrnl.h"
#include "Emu.h" // For EmuWarning
#include "EmuX86.h"
#include "HLEIntercept.h" // for bLLE_GPU

#include <assert.h>
#include "devices\Xbox.h" // For g_PCIBus
#include <atomic>
#include "Logging.h"

extern uint32_t GetAPUTime();
extern std::atomic_bool g_bEnableAllInterrupts;

//
// Read & write handlers handlers for I/O
//

static int field_pin = 0;

uint32_t EmuX86_IORead(xbaddr addr, int size)
{
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

	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_IORead(0x%08X, %d) [Unhandled]", addr, size);
	return 0;
}

void EmuX86_IOWrite(xbaddr addr, uint32_t value, int size)
{
	// Pass the IO Write to the PCI Bus, this will handle devices with BARs set to IO addresses
	if (g_PCIBus->IOWrite(addr, value, size)) {
		return;
	}

	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_IOWrite(0x%08X, 0x%04X, %d) [Unhandled]", addr, value, size);
}

//
// Read & write handlers for pass-through access to (host committed, virtual) xbox memory
//
// Only allowed to be called outside our EmuException exception handler,
// to prevent recursive exceptions when accessing unallocated memory.
//

uint32_t EmuX86_Mem_Read(xbaddr addr, int size)
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
		return 0;
	}
}

void EmuX86_Mem_Write(xbaddr addr, uint32_t value, int size)
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
		return;
	}
}

uint32_t EmuFlash_Read32(xbaddr addr) // TODO : Move to EmuFlash.cpp
{
	uint32_t r;

	switch (addr) {
	case 0x78: // ROM_VERSION
		r = 0x90; // Luke's hardware revision 1.6 Xbox returns this (also since XboxKrnlVersion is set to 5838)
		break;
	default:
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Read32 FLASH_ROM (0x%.8X) [Unknown address]", addr);
		return -1;
	}

	DbgPrintf(LOG_PREFIX, "Read32 FLASH_ROM (0x%.8X) = 0x%.8X [HANDLED]\n", addr, r);
	return r;
}

//
// Read & write handlers for memory-mapped hardware devices
//

uint32_t EmuX86_Read(xbaddr addr, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_Read(0x%08X, %d) [Unaligned unimplemented]", addr, size);
		// LOG_UNIMPLEMENTED();
		return 0;
	}

	uint32_t value;

	if (addr >= XBOX_FLASH_ROM_BASE) { // 0xFFF00000 - 0xFFFFFFF
		value = EmuFlash_Read32(addr - XBOX_FLASH_ROM_BASE); // TODO : Make flash access size-aware
	} else if(addr == 0xFE80200C) {
		// TODO: Remove this once we have an LLE APU Device
		return GetAPUTime();
	} else {
		// Pass the Read to the PCI Bus, this will handle devices with BARs set to MMIO addresses
		if (g_PCIBus->MMIORead(addr, &value, size)) {
			return value;
		}

		if (g_bEmuException) {
			EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_Read(0x%08X, %d) [Unknown address]", addr, size);
			value = 0;
		} else {
			// Outside EmuException, pass the memory-access through to normal memory :
			value = EmuX86_Mem_Read(addr, size);
		}

		DbgPrintf(LOG_PREFIX, "Read(0x%08X, %d) = 0x%08X\n", addr, size, value);
	}

	return value;
}

void EmuX86_Write(xbaddr addr, uint32_t value, int size)
{
	if ((addr & (size - 1)) != 0) {
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_Write(0x%08X, 0x%08X, %d) [Unaligned unimplemented]", addr, value, size);
		// LOG_UNIMPLEMENTED();
		return;
	}

	if (addr >= XBOX_FLASH_ROM_BASE) { // 0xFFF00000 - 0xFFFFFFF
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_Write(0x%08X, 0x%08X) [FLASH_ROM]", addr, value);
		return;
	}

	// Pass the Write to the PCI Bus, this will handle devices with BARs set to MMIO addresses
	if (g_PCIBus->MMIOWrite(addr, value, size)) {
		return;
	}

	if (g_bEmuException) {
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "EmuX86_Write(0x%08X, 0x%08X) [Unknown address]", addr, value);
		return;
	}

	// Outside EmuException, pass the memory-access through to normal memory :
	DbgPrintf(LOG_PREFIX, "Write(0x%.8X, 0x%.8X, %d)\n", addr, value, size);
	EmuX86_Mem_Write(addr, value, size);
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
	// Unsupported by XBox CPU : R_CR0, R_UNUSED0, R_CR2, R_CR3, R_CR4, R_UNUSED1, R_UNUSED2, R_UNUSED3, R_CR8,
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

inline void * EmuX86_GetRegisterPointer(const LPEXCEPTION_POINTERS e, const uint8_t reg)
{
	int offset = ContextRecordOffsetByRegisterType[reg];
	if (offset > 0)
		return (void*)((uintptr_t)(e->ContextRecord) + offset);

	assert(false);
	return nullptr;
}

inline uint32_t EmuX86_GetRegisterValue32(const LPEXCEPTION_POINTERS e, const uint8_t reg)
{
	if (reg != R_NONE)
	{
		void* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr != nullptr)
			return *(uint32_t *)regptr;
	}

	return 0;
}

xbaddr EmuX86_Distorm_O_SMEM_Addr(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand)
{
	xbaddr base = EmuX86_GetRegisterValue32(e, info.ops[operand].index);

	return base + (uint32_t)info.disp;
}

xbaddr EmuX86_Distorm_O_MEM_Addr(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand)
{
	xbaddr base = EmuX86_GetRegisterValue32(e, info.base);

	uint32_t index = EmuX86_GetRegisterValue32(e, info.ops[operand].index);

	if (info.scale >= 2)
		return base + (index * info.scale) + (uint32_t)info.disp;
	else
		return base + index + (uint32_t)info.disp;
}

typedef struct {
	xbaddr addr;
	bool is_internal_addr; // If set, addr points to a CPU context (or Distorm immedate value) member (instead of Xbox memory)
	int size; // Expressed in bytes, not bits!
} OperandAddress;

bool EmuX86_Operand_Addr_ForReadOnly(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand, OperandAddress &opAddr)
{
	opAddr.size = info.ops[operand].size / 8; // Convert size in bits into bytes
	switch (info.ops[operand].type) {
	case O_NONE:
	{
		// ignore operand
		return false;
	}
	case O_REG:
		opAddr.is_internal_addr = true;
		opAddr.addr = (xbaddr)EmuX86_GetRegisterPointer(e, info.ops[operand].index);
		return true;
	{
	}
	case O_IMM:
	{
		opAddr.is_internal_addr = true;
		opAddr.addr = (xbaddr)(&info.imm);
		return true;
	}
	case O_IMM1:
	{
		opAddr.is_internal_addr = true;
		opAddr.addr = (xbaddr)(&info.imm.ex.i1);
		return true;
	}
	case O_IMM2:
	{
		opAddr.is_internal_addr = true;
		opAddr.addr = (xbaddr)(&info.imm.ex.i2);
		return true;
	}
	case O_DISP:
	{
		opAddr.is_internal_addr = false;
		// TODO : Does this operand require : opAddr.size = info.dispSize / 8; // ?
		opAddr.addr = (xbaddr)info.disp;
		return true;
	}
	case O_SMEM:
	{
		opAddr.is_internal_addr = false;
		// TODO : Does this operand require : opAddr.size = info.dispSize / 8; // ?
		opAddr.addr = EmuX86_Distorm_O_SMEM_Addr(e, info, operand);
		return true;
	}
	case O_MEM:
	{
		opAddr.is_internal_addr = false;
		// TODO : Does this operand require : opAddr.size = info.dispSize / 8; // ?
		opAddr.addr = EmuX86_Distorm_O_MEM_Addr(e, info, operand);
		return true;
	}
	case O_PC:
	{
		opAddr.is_internal_addr = false;
		opAddr.addr = (xbaddr)INSTRUCTION_GET_TARGET(&info);
		return true;
	}
	case O_PTR:
	{
		opAddr.is_internal_addr = false;
		opAddr.addr = (xbaddr)info.imm.ptr.off; // TODO : What about info.imm.ptr.seg ?
		return true;
	}
	default:
		// UNREACHABLE(info.ops[operand].type);
		return false;
	}

	return false;
}

bool EmuX86_Operand_Addr_ForReadWrite(const LPEXCEPTION_POINTERS e, const _DInst& info, const int operand, OperandAddress &opAddr)
{
	// Disallow write-access to immediate value adresses
	switch (info.ops[operand].type) {
	case O_IMM:
	case O_IMM1:
	case O_IMM2:
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Refused operand write-access to immedate value address!");
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
	// By Ron Jeffery, see http://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
	e->ContextRecord->EFlags ^= ((e->ContextRecord->EFlags ^ value) & mask);
}

#define BIT(flag, bit) ((static_cast<uint32>((bool)bit)) << (flag))
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
#define PFCalc(result) (0x6996 >> ((result ^ (result >> 4)) & 0xF)) & 1 // See http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
#define CFCalc(result) (result >> BitSize) & 1 // TODO : Instead of looking at an actual overflow bit, use high bit of (result XOR dest XOR src)
// Flags calculated : Overflow (for addition or subtraction), Sign, Zero, Adjust, Parity and Carry

// See http://x86.renejeschke.de/ for affected CPU flags per instruction

// Keep opcode emulations alphabetically ordered :

bool EmuX86_Opcode_ADD(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// ADD reads value from source :
	uint32_t dest = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &dest))
		return false;

	// ADD reads and writes the same operand :
	OperandAddress opAddr;
	if (!EmuX86_Operand_Addr_ForReadWrite(e, info, 0, OUT opAddr))
		return false;

	const uint32_t src = EmuX86_Addr_Read(opAddr);

	const uint64_t result = (uint64_t)dest + (uint64_t)src;

	// Write back the result
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	// The OF, SF, ZF, AF, CF, and PF flags are set according to the result.
	EmuX86_SetFlags_OSZAPC(e,
		/*EMUX86_EFLAG_OF*/OF_Add(result, src, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, src, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/CFCalc(result));

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

	// AND Destination with src
	uint32_t result = dest & src;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	// The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
	EmuX86_SetFlags_OSZPC(e,
		/*EMUX86_EFLAG_OF*/0, 
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/0);

	return true;
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

	// CMP Destination with src (cmp internally is a discarded subtract)
	uint64_t result = (uint64_t)dest - (uint64_t)src;

	// The CF, OF, SF, ZF, AF, and PF flags are set according to the result.
	EmuX86_SetFlags_OSZAPC(e, 
		/*EMUX86_EFLAG_OF*/OF_Sub(result, src, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, src, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/CFCalc(result));

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

	if (src == dest) {
		// Write the source value to the destination operand
		if (!EmuX86_Operand_Write(e, info, 0, src)) {
			return false;
		}
	} else	{
		// Write the dest value to the source operand
		if (!EmuX86_Operand_Write(e, info, 1, dest)) {
			return false;
		}
	}

	// Perform arithmatic operation for flag calculation
	uint64_t result = (uint64_t)dest - (uint64_t)src;

	// CF, PF, AF, SF, and OF are set according to the result
	EmuX86_SetFlags_OSZAPC(e, 
		/*EMUX86_EFLAG_OF*/OF_Sub(result, src, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, src, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/CFCalc(result));

	return true;
}

void EmuX86_Opcode_CPUID(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// This CPUID emulation is based on :
	// https://github.com/docbrown/vxb/wiki/Xbox-CPUID-Information
	// https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information and
	// http://www.sandpile.org/x86/cpuid.htm
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

	// DEC Destination to src 
	uint64_t result = (uint64_t)dest - (uint64_t)1;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	// The CF flag is not affected. The OF, SF, ZF, AF, and PF flags are set according to the result.
	EmuX86_SetFlags_OSZAP(e, 
		/*EMUX86_EFLAG_OF*/OF_Sub(result, 1, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, 1, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result));

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

	// INC Destination to src 
	uint64_t result = (uint64_t)dest + (uint64_t)1;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));
	
	// The CF flag is not affected. The OF, SF, ZF, AF, and PF flags are set according to the re
	EmuX86_SetFlags_OSZAP(e,
		/*EMUX86_EFLAG_OF*/OF_Add(result, 1, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, 1, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result));

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

	// TODO : Implement MOVZX zero-extension!

	// MOVZX writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : MOVZX instructions never update CPU flags

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

	// OR Destination with src
	uint32_t result = dest | src;

	// Write back the result
	EmuX86_Addr_Write(opAddr, result);

	// The OF and CF flags are cleared; the SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
	EmuX86_SetFlags_OSZPC(e,
		/*EMUX86_EFLAG_OF*/0,
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/0);

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

	// SUB Destination with src 
	uint64_t result = (uint64_t)dest - (uint64_t)src;

	// Write result back
	EmuX86_Addr_Write(opAddr, static_cast<uint32_t>(result));

	// The OF, SF, ZF, AF, PF, and CF flags are set according to the result.
	EmuX86_SetFlags_OSZAPC(e,
		/*EMUX86_EFLAG_OF*/OF_Sub(result, src, dest),
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_AF*/AFCalc(result, src, dest),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/CFCalc(result));

	return true;
}

bool EmuX86_Opcode_TEST(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TEST reads first value :
	uint32_t src = 0;
	if (!EmuX86_Operand_Read(e, info, 0, &src))
		return false;

	// TEST reads second value :
	uint32_t dest = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &dest))
		return false;

	// TEST performs bitwise AND between first and second value :
	uint32_t result = src & dest;

	// https://en.wikipedia.org/wiki/TEST_(x86_instruction)
	// The OF and CF flags are set to 0. The SF, ZF, and PF flags are set according to the result. The state of the AF flag is undefined.
	EmuX86_SetFlags_OSZPC(e,
		/*EMUX86_EFLAG_OF*/0,
		/*EMUX86_EFLAG_SF*/SFCalc(result),
		/*EMUX86_EFLAG_ZF*/ZFCalc(result),
		/*EMUX86_EFLAG_PF*/PFCalc(result),
		/*EMUX86_EFLAG_CF*/0);

	// result is thrown away

	return true;
}

void EmuX86_Opcode_CLI()
{
	g_bEnableAllInterrupts = false;
}

void EmuX86_Opcode_STI()
{
	g_bEnableAllInterrupts = true;
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

int EmuX86_OpcodeSize(uint8_t *Eip)
{
	_DInst info;
	if (EmuX86_DecodeOpcode((uint8_t*)Eip, info))
		return info.size;

	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Error decoding opcode size at 0x%.8X", Eip);
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
	if (!EmuX86_DecodeOpcode((uint8_t*)e->ContextRecord->Eip, OUT info)) {
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Error decoding opcode at 0x%08X", e->ContextRecord->Eip);
		return false;
	}

	switch (info.opcode) { // Keep these cases alphabetically ordered and condensed
	case I_ADD:
		if (EmuX86_Opcode_ADD(e, info)) break;
		goto opcode_error;
	case I_AND:
		if (EmuX86_Opcode_AND(e, info)) break;
		goto opcode_error;
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
	case I_IN:
		if (EmuX86_Opcode_IN(e, info)) break;
		goto opcode_error;
	case I_INC:
		if (EmuX86_Opcode_INC(e, info)) break;
		goto opcode_error;
	case I_INVD: // Flush internal caches; initiate flushing of external caches.
		break; // We can safely ignore this
	case I_MOV:
		if (EmuX86_Opcode_MOV(e, info)) break;
		goto opcode_error;
	case I_MOVSX:
		if (EmuX86_Opcode_MOVSX(e, info)) break;
		goto opcode_error;
	case I_MOVZX:			
		if (EmuX86_Opcode_MOVZX(e, info)) break;
		goto opcode_error;
	case I_OR:
		if (EmuX86_Opcode_OR(e, info)) break;
		goto opcode_error;
	case I_OUT:
		if (EmuX86_Opcode_OUT(e, info)) break;
		goto opcode_error;
	case I_SUB:
		if (EmuX86_Opcode_SUB(e, info)) break;
		goto opcode_error;
	case I_TEST:
		if (EmuX86_Opcode_TEST(e, info)) break;
		goto opcode_error;
	case I_WBINVD: // Write back and flush internal caches; initiate writing-back and flushing of external caches.
		break; // We can safely ignore this
	case I_WRMSR:
		// We do not emulate processor specific registers just yet
		// Some titles attempt to manually set the TSC via this instruction
		// This needs fixing eventually, but should be acceptible to ignore for now!
		// Chase: Hollywood Stunt Driver hits this
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "WRMSR instruction ignored");
		break;
	case I_CLI: {
		// Disable all interrupts
		EmuX86_Opcode_CLI();
		break;
	}
	case I_STI: {
		// Enable all interrupts
		EmuX86_Opcode_STI();
		break;
	}
	default:
		EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "Unhandled instruction : %u", info.opcode);
		e->ContextRecord->Eip += info.size;
		return false;
	}

	// When falling through here, the instruction was handled correctly,
	// skip over the instruction and continue execution :
	e->ContextRecord->Eip += info.size;
	return true;

opcode_error:
	EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, "0x%08X: Error while handling instruction %u", e->ContextRecord->Eip, info.opcode); // TODO : format decodedInstructions[0]
	e->ContextRecord->Eip += info.size;
	return false;
}

void EmuX86_Init()
{
	DbgPrintf(LOG_PREFIX, "Initializing distorm version %d\n", distorm_version());

	AddVectoredExceptionHandler(/*FirstHandler=*/ULONG(true), lleException);

	EmuX86_InitContextRecordOffsetByRegisterType();
}
