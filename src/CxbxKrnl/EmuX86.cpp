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

_DecodeResult distorm_decompose64(_CodeInfo* ci, _DInst result[], unsigned int maxInstructions, unsigned int* usedInstructionsCount);

uint32_t EmuX86_IORead32(uint32_t addr)
{
	EmuWarning("EmuX86_IORead32(0x%08X) Not Implemented", addr);
	return 0;
}

uint16_t EmuX86_IORead16(uint32_t addr)
{
	EmuWarning("EmuX86_IORead16(0x%08X) Not Implemented", addr);
	return 0;
}

uint8_t EmuX86_IORead8(uint32_t addr)
{
	EmuWarning("EmuX86_IORead8(0x%08X) Not Implemented", addr);
	return 0;
}

void EmuX86_IOWrite32(uint32_t addr, uint32_t value)
{
	EmuWarning("EmuX86_IOWrite32(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_IOWrite16(uint32_t addr, uint16_t value)
{
	EmuWarning("EmuX86_IOWrite16(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_IOWrite8(uint32_t addr, uint8_t value)
{
	EmuWarning("EmuX86_IOWrite8(0x%08X, 0x%02X) [Unknown address]", addr, value);
}

uint32_t EmuX86_Read32(uint32_t addr)
{
	uint32_t value = 0;
	if (addr >= NV2A_ADDR && addr < NV2A_ADDR + NV2A_SIZE)
		value = EmuNV2A_Read32(addr - NV2A_ADDR);
	else
		EmuWarning("EmuX86_Read32(0x%08X) = 0x%08X [Unknown address]", addr, value);

	return value;
}

uint16_t EmuX86_Read16(uint32_t addr)
{
	EmuWarning("EmuX86_Read16(0x%08X) Forwarding to EmuX86_Read32...", addr);
	uint16_t value;
	if (addr & 2)
		value = (uint16_t)(EmuX86_Read32(addr - 2) >> 16);
	else
		value = (uint16_t)EmuX86_Read32(addr);

	EmuWarning("EmuX86_Read16(0x%08X) = 0x%04X", addr, value);
	return value;
}

uint8_t EmuX86_Read8(uint32_t addr)
{
	EmuWarning("EmuX86_Read8(0x%08X) Forwarding to EmuX86_Read16...", addr);
	uint8_t value;
	if (addr & 1)
		value = (uint8_t)(EmuX86_Read16(addr - 1) >> 8);
	else
		value = (uint8_t)EmuX86_Read16(addr);

	EmuWarning("EmuX86_Read8(0x%08X) = 0x%02X", addr, value);
	return value;
}

void EmuX86_Write32(uint32_t addr, uint32_t value)
{
	if (addr >= NV2A_ADDR && addr < NV2A_ADDR + NV2A_SIZE) {
		EmuNV2A_Write32(addr - NV2A_ADDR, value);
		return;
	}

	EmuWarning("EmuX86_Write32(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_Write16(uint32_t addr, uint16_t value)
{
	EmuWarning("EmuX86_Write16(0x%08X, 0x%04X) [Unknown address]", addr, value);
}

void EmuX86_Write8(uint32_t addr, uint8_t value)
{
	EmuWarning("EmuX86_Write8(0x%08X, 0x%02X) [Unknown address]", addr, value);
}

inline DWORD* EmuX86_GetRegisterPointer(LPEXCEPTION_POINTERS e, uint8_t reg)
{
	switch (reg) {
		case R_AL: case R_AH:	case R_AX: case R_EAX:
			return &e->ContextRecord->Eax;
		case R_BL: case R_BH:	case R_BX: case R_EBX:
			return &e->ContextRecord->Ebx;
		case R_CL: case R_CH:	case R_CX: case R_ECX:
			return &e->ContextRecord->Ecx;
		case R_DL: case R_DH: case R_DX: case R_EDX:
			return &e->ContextRecord->Edx;
		case R_EDI:
			return &e->ContextRecord->Edi;
		case R_ESI:
			return &e->ContextRecord->Esi;
		case R_EBP:
			return &e->ContextRecord->Ebp;
	}

	return nullptr;
}

inline bool EmuX86_GetRegisterValue(uint32_t* output, LPEXCEPTION_POINTERS e, uint8_t reg)
{
	uint32_t value = 0;
	if (reg != R_NONE)
	{
		DWORD* regptr = EmuX86_GetRegisterPointer(e, reg);
		if (regptr == nullptr)
			return false;

		value = *regptr;
	}

	*output = value;
	return true;
}

uint32_t EmuX86_Distorm_O_SMEM_Addr(LPEXCEPTION_POINTERS e, _DInst& info, int operand)
{
	uint32_t base;
	EmuX86_GetRegisterValue(&base, e, info.ops[operand].index);

	return base + info.disp;
}

uint32_t EmuX86_Distorm_O_MEM_Addr(LPEXCEPTION_POINTERS e, _DInst& info, int operand)
{
	uint32_t base = 0;
	EmuX86_GetRegisterValue(&base, e, info.base);

	uint32_t index = 0;
	EmuX86_GetRegisterValue(&index, e, info.ops[operand].index);

	if (info.scale >= 2)
		return base + (index * info.scale) + info.disp;
	else
		return base + index + info.disp;
}

void EmuX86_ReadAddr(uint32_t srcAddr, uint16_t size, OUT uint32_t *value)
{
	switch (size) {
	case 8:
		*value = EmuX86_Read8(srcAddr);
		break;
	case 16:
		*value = EmuX86_Read16(srcAddr);
		break;
	case 32:
		*value = EmuX86_Read32(srcAddr);
		break;
	default:
		// TODO : Handle other sizes?
		break;
	}
}

void EmuX86_WriteAddr(uint32_t destAddr, uint16_t size, uint32_t value)
{
	switch (size) {
	case 8:
		EmuX86_Write8(destAddr, value & 0xFF);
		break;
	case 16:
		EmuX86_Write16(destAddr, value & 0xFFFF);
		break;
	case 32:
		EmuX86_Write32(destAddr, value);
		break;
	default:
		// TODO : Handle other sizes?
		break;
	}
}

bool EmuX86_ReadValueFromSource(LPEXCEPTION_POINTERS e, _DInst& info, int operand, OUT uint32_t *value)
{
	switch (info.ops[operand].type) {
	case O_NONE:
	{
		// ignore operand
	}
	case O_REG:
	{
		if (!EmuX86_GetRegisterValue(value, e, info.ops[operand].index))
			return false;

		break;
	}
	case O_IMM:
	{
		switch (info.ops[operand].size) {
		case 8:
			*value = info.imm.byte;
			break;
		case 16:
			*value = info.imm.word;
			break;
		case 32:
			*value = info.imm.dword;
			break;
			// TODO : Handle other sizes?
		}
		break;
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
		uint32_t srcAddr = info.disp;
		EmuX86_ReadAddr(srcAddr, info.ops[operand].size, value);
		break;
	}
	case O_SMEM:
	{
		uint32_t srcAddr = EmuX86_Distorm_O_SMEM_Addr(e, info, operand);
		EmuX86_ReadAddr(srcAddr, info.ops[operand].size, value);
		break;
	}
	case O_MEM:
	{
		uint32_t srcAddr = EmuX86_Distorm_O_MEM_Addr(e, info, operand);
		EmuX86_ReadAddr(srcAddr, info.ops[operand].size, value);
		break;
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

	return true;
}

bool EmuX86_WriteValueToDestination(LPEXCEPTION_POINTERS e, _DInst& info, int operand, uint32_t value)
{
	switch (info.ops[operand].type) {
	case O_NONE:
	{
		// ignore operand
	}
	case O_REG:
	{
		DWORD* pDstReg = EmuX86_GetRegisterPointer(e, info.ops[operand].index);
		if (pDstReg == nullptr)
			return false;

		switch (info.ops[operand].size) {
		case 8:
			*((uint8_t*)pDstReg + 3) = value & 0xFF;
			break;
		case 16:
			*((uint16_t*)pDstReg + 2) = value & 0xFFFF;
			break;
		case 32:
			*pDstReg = value;
			break;
		default:
			return false;
		}
		break;
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
		uint32_t destAddr = info.disp;
		EmuX86_WriteAddr(destAddr, info.ops[operand].size, value);
		break;
	}
	case O_SMEM:
	{
		uint32_t destAddr = EmuX86_Distorm_O_SMEM_Addr(e, info, operand);
		EmuX86_WriteAddr(destAddr, info.ops[operand].size, value);
		break;
	}
	case O_MEM:
	{
		uint32_t destAddr = EmuX86_Distorm_O_MEM_Addr(e, info, operand);
		EmuX86_WriteAddr(destAddr, info.ops[operand].size, value);
		break;
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

	return true;
}

bool EmuX86_MOV(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TODO : Test this refactoring

	// MOV reads value from source :
	uint32_t value = 0;
	if (!EmuX86_ReadValueFromSource(e, info, 1, &value))
		return false;

	// MOV writes value to destination :
	if (!EmuX86_WriteValueToDestination(e, info, 0, value))
		return false;

	// Note : MOV instructions never update CPU flags

	return true;
}

bool EmuX86_MOVZX(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TODO : Test this refactoring

	// MOV reads value from source :
	uint32_t value = 0;
	if (!EmuX86_ReadValueFromSource(e, info, 1, &value))
		return false;

	// MOV writes value to destination :
	if (!EmuX86_WriteValueToDestination(e, info, 0, value))
		return false;

	// Note : MOV instructions never update CPU flags

	return true;
}

inline void EmuX86_SetFlag(LPEXCEPTION_POINTERS e, int flag, int value)
{
	e->ContextRecord->EFlags ^= (-value ^ e->ContextRecord->EFlags) & (1 << flag);
}

bool EmuX86_TEST(LPEXCEPTION_POINTERS e, _DInst& info)
{
	// TEST reads first value :
	uint32_t result = 0;
	if (!EmuX86_ReadValueFromSource(e, info, 0, &result))
		return false;

	// TEST reads second value :
	uint32_t value = 0;
	if (!EmuX86_ReadValueFromSource(e, info, 1, &value))
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

bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e)
{
	// Only decode instructions within Xbox memory space
	if (e->ContextRecord->Eip > XBOX_MEMORY_SIZE || e->ContextRecord->Eip < 0x10000) {
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
		EmuWarning("EmuX86: Error decoding opcode at 0x%08X\n", e->ContextRecord->Eip);
	}
	else
	{
		switch (info.opcode)
		{
		case I_MOV:
			if (EmuX86_MOV(e, info)) {
				break;
			}

			goto unimplemented_opcode;
		case I_MOVZX:			
			if (EmuX86_MOVZX(e, info)) {
				break;
			}

			goto unimplemented_opcode;
		case I_TEST:
			if (EmuX86_TEST(e, info)) {
				break;
			}

			goto unimplemented_opcode;
		case I_WBINVD:
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
