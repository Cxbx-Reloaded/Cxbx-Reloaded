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

#include <Zydis.hpp>

#include "CxbxKrnl.h"
#include "Emu.h"
#include "EmuX86.h"
#include "EmuNV2A.h"

uint32_t EmuX86_Read32(uint32_t addr)
{
	if (addr >= 0xFD000000 && addr <= 0xFE000000) {
		return EmuNV2A_Read32(addr - 0xFD000000);
	}

	EmuWarning("EmuX86_Read32: Unknown Read Address %08X", addr);
	return 0;
}

void EmuX86_Write32(uint32_t addr, uint32_t value)
{
	if (addr >= 0xFD000000 && addr <= 0xFE000000) {
		EmuNV2A_Write32(addr - 0xFD000000, value);
		return;
	}

	EmuWarning("EmuX86_Write32: Unknown Write Address %08X", addr);
}

bool EmuX86_MOV(LPEXCEPTION_POINTERS e, Zydis::InstructionInfo& info)
{
	if (info.operand[0].type == Zydis::OperandType::REGISTER && info.operand[1].type == Zydis::OperandType::MEMORY) 
	{
		switch (info.operand[0].base) {
			case Zydis::Register::EAX:
				e->ContextRecord->Eax = EmuX86_Read32(info.operand[1].lval.udword);
				return true;
			case Zydis::Register::EBX:
				e->ContextRecord->Ebx = EmuX86_Read32(info.operand[1].lval.udword);
				return true;
			case Zydis::Register::ECX:
				e->ContextRecord->Ebx = EmuX86_Read32(info.operand[1].lval.udword);
				return true;
			default:
				return false;
		}
	}
	else if (info.operand[0].type == Zydis::OperandType::MEMORY && info.operand[1].type == Zydis::OperandType::REGISTER)
	{
		switch (info.operand[1].base) {
			case Zydis::Register::EAX:
				EmuX86_Write32(info.operand[1].lval.udword, e->ContextRecord->Eax);
				return true;
			case Zydis::Register::EBX:
				EmuX86_Write32(info.operand[1].lval.udword, e->ContextRecord->Ebx);
				return true;
			case Zydis::Register::ECX:
				EmuX86_Write32(info.operand[1].lval.udword, e->ContextRecord->Ecx);
				return true;
			default:
				return false;
		}
	}
	else if (info.operand[0].type == Zydis::OperandType::MEMORY && info.operand[1].type == Zydis::OperandType::IMMEDIATE)
	{
		EmuX86_Write32(info.operand[0].lval.udword, info.operand[1].lval.udword);
		return true;
	}
}

bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e)
{
	// Only decode instructions within Xbox memory space
	if (e->ContextRecord->Eip > XBOX_MEMORY_SIZE || e->ContextRecord->Eip < 0x10000) {
		return false;
	}

	Zydis::MemoryInput input((uint8_t*)e->ContextRecord->Eip, XBOX_MEMORY_SIZE - e->ContextRecord->Eip);
	Zydis::InstructionInfo info;
	Zydis::InstructionDecoder decoder;
	decoder.setDisassemblerMode(Zydis::DisassemblerMode::M32BIT);
	decoder.setDataSource(&input);
	decoder.setInstructionPointer(e->ContextRecord->Eip);
	Zydis::IntelInstructionFormatter formatter;

	// Decode a single instruction
	decoder.decodeInstruction(info);
	
	if (info.flags & Zydis::IF_ERROR_MASK)
	{
		EmuWarning("EmuX86: Error decoding opcode at 0x%08X\n", e->ContextRecord->Eip);
	}
	else
	{
		DbgPrintf("EmuX86: 0x%08X: %s\n", e->ContextRecord->Eip, formatter.formatInstruction(info));

		switch (info.mnemonic) 
		{	
			case Zydis::InstructionMnemonic::MOV:
				if (EmuX86_MOV(e, info)) {
					break;
				}
			default:
				EmuWarning("EmuX86: 0x%08X: %s Not Implemented\n", e->ContextRecord->Eip, formatter.formatInstruction(info));
				e->ContextRecord->Eip += info.length;
				return false;
		}

		e->ContextRecord->Eip += info.length;
		return true;
	}

	return false;
}
