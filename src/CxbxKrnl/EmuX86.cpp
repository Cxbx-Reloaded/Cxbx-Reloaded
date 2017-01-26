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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// Link the library into our project.
#pragma comment(lib, "distorm.lib")

// Cxbx uses dynamic linking of distorm, which by default chooses for 64 bits offsets :
#define SUPPORT_64BIT_OFFSET

#include "distorm.h"
#include "mnemonics.h"

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuX86.h"
#include "EmuNV2A.h"
#include "HLEIntercept.h" // for bLLE_GPU

uint32_t EmuX86_IORead32(xbaddr addr)
{
	EmuWarning("EmuX86_IORead32(0x%08X) Not Implemented", addr);
	return 0;
}

uint16_t EmuX86_IORead16(xbaddr addr)
{
	EmuWarning("EmuX86_IORead16(0x%08X) Not Implemented", addr);
	return 0;
}

uint8_t EmuX86_IORead8(xbaddr addr)
{
	EmuWarning("EmuX86_IORead8(0x%08X) Not Implemented", addr);
	return 0;
}

void EmuX86_IOWrite32(xbaddr addr, uint32_t value)
{
	EmuWarning("EmuX86_IOWrite32(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_IOWrite16(xbaddr addr, uint16_t value)
{
	EmuWarning("EmuX86_IOWrite16(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_IOWrite8(xbaddr addr, uint8_t value)
{
	EmuWarning("EmuX86_IOWrite8(0x%08X, 0x%02X) [Unknown address]", addr, value);
}

uint32_t EmuX86_Read32(xbaddr addr)
{
	// TODO : Should we assert ((addr & 3) == 0) here?
	uint32_t value = 0;
	if (addr >= NV2A_ADDR && addr < NV2A_ADDR + NV2A_SIZE) {
		if (!bLLE_GPU) {
			EmuWarning("EmuX86_Read32(0x%08X) Unexpected NV2A access, missing a HLE patch. Please notify https://github.com/Cxbx-Reloaded/Cxbx-Reloaded which title raised this!", addr);
		} else
			value = EmuNV2A_Read32(addr - NV2A_ADDR);
	} else
		EmuWarning("EmuX86_Read32(0x%08X) = 0x%08X [Unknown address]", addr, value);

	return value;
}

uint16_t EmuX86_Read16(xbaddr addr)
{
	DbgPrintf("EmuX86_Read16(0x%08X) Forwarding to EmuX86_Read32...", addr);
	uint16_t value;
	if (addr & 2)
		value = (uint16_t)(EmuX86_Read32(addr - 2) >> 16);
	else
		value = (uint16_t)EmuX86_Read32(addr);

	DbgPrintf("EmuX86_Read16(0x%08X) = 0x%04X", addr, value);
	return value;
}

uint8_t EmuX86_Read8(xbaddr addr)
{
	DbgPrintf("EmuX86_Read8(0x%08X) Forwarding to EmuX86_Read16...", addr);
	uint8_t value;
	if (addr & 1)
		value = (uint8_t)(EmuX86_Read16(addr - 1) >> 8);
	else
		value = (uint8_t)EmuX86_Read16(addr);

	DbgPrintf("EmuX86_Read8(0x%08X) = 0x%02X", addr, value);
	return value;
}

void EmuX86_Write32(xbaddr addr, uint32_t value)
{
	if (addr >= NV2A_ADDR && addr < NV2A_ADDR + NV2A_SIZE) {
		if (!bLLE_GPU) {
			EmuWarning("EmuX86_Write32(0x%08X, 0x%04X) Unexpected NV2A access, missing a HLE patch. Please notify Cxbx project which title raised this!", addr, value);
		} else
			EmuNV2A_Write32(addr - NV2A_ADDR, value);

		return;
	}

	EmuWarning("EmuX86_Write32(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_Write16(xbaddr addr, uint16_t value)
{
	EmuWarning("EmuX86_Write16(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_Write8(xbaddr addr, uint8_t value)
{
	EmuWarning("EmuX86_Write8(0x%08X, 0x%02X) [Unknown address]", addr, value);
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

inline void * EmuX86_GetRegisterPointer(LPEXCEPTION_POINTERS e, uint8_t reg)
{
	int offset = ContextRecordOffsetByRegisterType[reg];
	if (offset > 0)
		return (void*)((uintptr_t)(e->ContextRecord) + offset);

	return nullptr;
}

inline uint32_t EmuX86_GetRegisterValue32(LPEXCEPTION_POINTERS e, uint8_t reg)
{
	if (reg != R_NONE)
	{
		void* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr != nullptr)
			return *(uint32_t *)regptr;
	}

	return 0;
}

inline uint16_t EmuX86_GetRegisterValue16(LPEXCEPTION_POINTERS e, uint8_t reg)
{
	if (reg != R_NONE)
	{
		void* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr != nullptr)
			return *(uint16_t *)regptr;
	}

	return 0;
}

inline uint8_t EmuX86_GetRegisterValue8(LPEXCEPTION_POINTERS e, uint8_t reg)
{
	if (reg != R_NONE)
	{
		void* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr != nullptr)
			return *(uint8_t *)regptr;
	}

	return 0;
}

xbaddr EmuX86_Distorm_O_SMEM_Addr(LPEXCEPTION_POINTERS e, _DInst& info, int operand)
{
	xbaddr base = EmuX86_GetRegisterValue32(e, info.ops[operand].index);

	return base + info.disp;
}

xbaddr EmuX86_Distorm_O_MEM_Addr(LPEXCEPTION_POINTERS e, _DInst& info, int operand)
{
	xbaddr base = EmuX86_GetRegisterValue32(e, info.base);

	uint32_t index = EmuX86_GetRegisterValue32(e, info.ops[operand].index);

	if (info.scale >= 2)
		return base + (index * info.scale) + info.disp;
	else
		return base + index + info.disp;
}

void EmuX86_Addr_Read(xbaddr srcAddr, uint16_t size, OUT uint32_t *value)
{
	switch (size) {
	case 8:
		*value = EmuX86_Read8(srcAddr);
		return;
	case 16:
		*value = EmuX86_Read16(srcAddr);
		return;
	case 32:
		*value = EmuX86_Read32(srcAddr);
	}
}

void EmuX86_Addr_Write(xbaddr destAddr, uint16_t size, uint32_t value)
{
	switch (size) {
	case 8:
		EmuX86_Write8(destAddr, value & 0xFF);
		return;
	case 16:
		EmuX86_Write16(destAddr, value & 0xFFFF);
		return;
	case 32:
		EmuX86_Write32(destAddr, value);
	}
}

bool EmuX86_Operand_Read(LPEXCEPTION_POINTERS e, _DInst& info, int operand, OUT uint32_t *value)
{
	switch (info.ops[operand].type) {
	case O_NONE:
	{
		// ignore operand
		return true;
	}
	case O_REG:
	{
		void* regAddr = EmuX86_GetRegisterPointer(e, info.ops[operand].index);
		if (regAddr == 0)
			return false;

		switch (info.ops[operand].size) {
		case 8:
			*value = *((uint8_t*)regAddr);
			return true;
		case 16:
			*value = *((uint16_t*)regAddr);
			return true;
		case 32:
			*value = *((uint32_t*)regAddr);
			return true;
		}
		return false;
	}
	case O_IMM:
	{
		switch (info.ops[operand].size) {
		case 8:
			*value = info.imm.byte;
			return true;
		case 16:
			*value = info.imm.word;
			return true;
		case 32:
			*value = info.imm.dword;
			return true;
		}
		return false;
	}
	case O_IMM1:
	{
		// TODO
		return false;
	}
	case O_IMM2:
	{
		// TODO
		return false;
	}
	case O_DISP:
	{
		xbaddr srcAddr = info.disp;
		EmuX86_Addr_Read(srcAddr, info.ops[operand].size, value);
		return true;
	}
	case O_SMEM:
	{
		xbaddr srcAddr = EmuX86_Distorm_O_SMEM_Addr(e, info, operand);
		EmuX86_Addr_Read(srcAddr, info.ops[operand].size, value);
		return true;
	}
	case O_MEM:
	{
		xbaddr srcAddr = EmuX86_Distorm_O_MEM_Addr(e, info, operand);
		EmuX86_Addr_Read(srcAddr, info.ops[operand].size, value);
		return true;
	}
	case O_PC:
	{
		// TODO
		return false;
	}
	case O_PTR:
	{
		// TODO
		return false;
	}
	default:
		return false;
	}

	return false;
}

bool EmuX86_Operand_Write(LPEXCEPTION_POINTERS e, _DInst& info, int operand, uint32_t value)
{
	switch (info.ops[operand].type) {
	case O_NONE:
	{
		// ignore operand
		return true;
	}
	case O_REG:
	{
		void* regAddr = EmuX86_GetRegisterPointer(e, info.ops[operand].index);
		if (regAddr == nullptr)
			return false;

		switch (info.ops[operand].size) {
		case 8:
			*((uint8_t*)regAddr) = (uint8_t)value;
			return true;
		case 16:
			*((uint16_t*)regAddr) = (uint16_t)value;
			return true;
		case 32:
			*((uint32_t*)regAddr) = value;
			return true;
		default:
			return false;
		}
		return false;
	}
	case O_IMM:
	{
		// TODO
		return false;
	}
	case O_IMM1:
	{
		// TODO
		return false;
	}
	case O_IMM2:
	{
		// TODO
		return false;
	}
	case O_DISP:
	{
		xbaddr destAddr = info.disp;
		EmuX86_Addr_Write(destAddr, info.ops[operand].size, value);
		return true;
	}
	case O_SMEM:
	{
		xbaddr destAddr = EmuX86_Distorm_O_SMEM_Addr(e, info, operand);
		EmuX86_Addr_Write(destAddr, info.ops[operand].size, value);
		return true;
	}
	case O_MEM:
	{
		xbaddr destAddr = EmuX86_Distorm_O_MEM_Addr(e, info, operand);
		EmuX86_Addr_Write(destAddr, info.ops[operand].size, value);
		return true;
	}
	case O_PC:
	{
		// TODO
		return false;
	}
	case O_PTR:
	{
		// TODO
		return false;
	}
	default:
		return false;
	}

	return false;
}

bool EmuX86_Opcode_ADD(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// ADD reads value from source :
	uint32_t value;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// ADD reads and writes destination :
	xbaddr addr;
	// TODO : Implement EmuX86_Operand_Addr, then enable the following line :
	// if (!EmuX86_Operand_Addr(e, info, 0, &addr))
		return false;

	// TODO : Do this better
	switch (info.ops[0].size) {
	case 8: {
		uint8_t current = EmuX86_Read8(addr);
		EmuX86_Write8(addr, (uint8_t)current + value);
		return true;
	}
	case 16: {
		uint16_t current = EmuX86_Read16(addr);
		EmuX86_Write16(addr, (uint16_t)current + value);
		return true;
	}
	case 32: {
		uint32_t current = EmuX86_Read32(addr);
		EmuX86_Write32(addr, (uint32_t)current + value);
		return true;
	}
	default:
		return false;
	}

	// TODO : update CPU flags

	return true;
}

bool  EmuX86_Opcode_MOV(LPEXCEPTION_POINTERS e, _DInst& info)
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

bool  EmuX86_Opcode_MOVZX(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// MOVZX reads value from source :
	uint32_t value = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// TODO : Implement MOVZX zero-extension!

	// MOVZX writes value to destination :
	if (!EmuX86_Operand_Write(e, info, 0, value))
		return false;

	// Note : MOV instructions never update CPU flags

	return true;
}

inline void EmuX86_SetFlag(LPEXCEPTION_POINTERS e, int flag, int value)
{
	e->ContextRecord->EFlags ^= (-value ^ e->ContextRecord->EFlags) & (1 << flag);
}

bool  EmuX86_Opcode_TEST(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TEST reads first value :
	uint32_t result = 0;
	if (!EmuX86_Operand_Read(e, info, 0, &result))
		return false;

	// TEST reads second value :
	uint32_t value = 0;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// TEST performs bitwise AND between first and second value :
	result &= value;

	// https://en.wikipedia.org/wiki/TEST_(x86_instruction)
	// Set CF/OF to 0
	EmuX86_SetFlag(e, EMUX86_EFLAG_CF, 0);
	EmuX86_SetFlag(e, EMUX86_EFLAG_OF, 0);
	EmuX86_SetFlag(e, EMUX86_EFLAG_SF, result >> 31);
	EmuX86_SetFlag(e, EMUX86_EFLAG_ZF, result == 0 ? 1 : 0);
	// Set Parity flag, based on "Compute parity in parallel" method from
	// http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
	uint32_t v = 255 & result;
	v ^= v >> 4;
	v &= 0xf;
	EmuX86_SetFlag(e, EMUX86_EFLAG_PF, (0x6996 >> v) & 1);

	// result is thrown away

	return true;
}

void  EmuX86_Opcode_CPUID(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// This CPUID emulation is based on :
	// https://github.com/docbrown/vxb/wiki/Xbox-CPUID-Information
	// https://github.com/docbrown/vxb/wiki/Xbox-Hardware-Information and
	// http://www.sandpile.org/x86/cpuid.htm
	switch (e->ContextRecord->Eax) // simpler than EmuX86_GetRegisterValue32(e, R_EAX)
	{
	case 0: // CPUID Function 0, return the maximum supported standard level and vendor ID string
	{
		// Maximum supported standard level
		e->ContextRecord->Eax = 2;
		// "GenuineIntel" Intel processor
		e->ContextRecord->Ebx = (ULONG)'uneG';
		e->ContextRecord->Edx = (ULONG)'Ieni';
		e->ContextRecord->Ecx = (ULONG)'letn';
		return;
	}
	case 1: // CPUID Function 1, Return the processor type / family / model / stepping and feature flags
	{
		// Family 6, Model 8, Stepping 10
		e->ContextRecord->Eax = 0x68a;
		e->ContextRecord->Ebx = 0;
		e->ContextRecord->Ecx = 0;
		// Feature Flags 
		e->ContextRecord->Edx = 0x383F9FF; // FPU, VME, DE, PSE, TSC, MSR, PAE, MCE, CX8, SEP, MTRR, PGE, MCA, CMOV, PAT, PSE36, MMX, FXSR, SSE
		return;
	}
	case 2: // CPUID Function 2, Return the processor configuration descriptors
	{
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
}

bool  EmuX86_Opcode_OUT(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// OUT will address the first operand :
	uint32_t addr;
	if (!EmuX86_Operand_Read(e, info, 0, &addr))
		return false;

	uint32_t value;
	if (!EmuX86_Operand_Read(e, info, 1, &value))
		return false;

	// OUT does an I/O write on the address, using the value from the second operand :
	switch (info.ops[1].size) {
	case 8: {
		EmuX86_IOWrite8(addr, (uint8_t)value);
		return true;
	}
	case 16: {
		EmuX86_IOWrite16(addr, (uint16_t)value);
		return true;
	}
	case 32: {
		EmuX86_IOWrite32(addr, value);
		return true;
	}
	default:
		return false;
	}
	return false;
}

bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e)
{
	// Only decode instructions within Xbox memory space
	if (e->ContextRecord->Eip > EMU_MAX_MEMORY_SIZE || e->ContextRecord->Eip < XBOX_BASE_ADDR) {
		return false;
	}

	// Decoded instruction information.
	_DInst info;
	unsigned int decodedInstructionsCount = 0;

	_CodeInfo ci;
	ci.code = (uint8_t*)e->ContextRecord->Eip;
	ci.codeLen = 20;
	ci.codeOffset = 0;
	ci.dt = (_DecodeType)Decode32Bits;
	ci.features = DF_NONE;

	// Checking for DECRES_SUCCESS won't work, since we're passing distorm_decompose
	// a codeLen big enough to decode any instruction-length, plus distorm doesn't
	// halt cleanly after reaching maxInstructions 1. So instead, just call distorm :
	distorm_decompose(&ci, &info, /*maxInstructions=*/1, &decodedInstructionsCount);
	// and check if it successfully decoded one instruction :
	if (decodedInstructionsCount != 1)
	{
		EmuWarning("EmuX86: Error decoding opcode at 0x%08X", e->ContextRecord->Eip);
	}
	else
	{
		switch (info.opcode) // Keep these cases alphabetically ordered
		{
		case I_ADD:
			if (EmuX86_Opcode_ADD(e, info))
				break;

			goto unimplemented_opcode;
		case I_CPUID:
			EmuX86_Opcode_CPUID(e, info);
			break;
		case I_INVD: // Flush internal caches; initiate flushing of external caches.
			 // We can safely ignore this
			break;
		case I_MOV:
			if (EmuX86_Opcode_MOV(e, info))
				break;

			goto unimplemented_opcode;
		case I_MOVZX:			
			if (EmuX86_Opcode_MOVZX(e, info))
				break;

			goto unimplemented_opcode;
		case I_OUT:
			if (EmuX86_Opcode_OUT(e, info))
				break;

			goto unimplemented_opcode;
		case I_TEST:
			if (EmuX86_Opcode_TEST(e, info))
				break;

			goto unimplemented_opcode;
		case I_WBINVD: // Write back and flush internal caches; initiate writing-back and flushing of external caches.
			// We can safely ignore this
			break;
		default:
			goto unimplemented_opcode;
		}

		// When falling through here, the instruction was handled correctly,
		// skip over the instruction and continue execution :
		e->ContextRecord->Eip += info.size;
		return true;

unimplemented_opcode:
		EmuWarning("EmuX86: 0x%08X: Not Implemented\n", e->ContextRecord->Eip);	// TODO : format decodedInstructions[0]
		e->ContextRecord->Eip += info.size;
	}
	
	return false;
}

void EmuX86_Init()
{
	DbgPrintf("EmuX86: Initializing distorm version %d\n", distorm_version());
	EmuX86_InitContextRecordOffsetByRegisterType();
}

