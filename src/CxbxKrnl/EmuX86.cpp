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
		EmuWarning("EmuX86: 0x%08X: %s Ignored\n", e->ContextRecord->Eip, formatter.formatInstruction(info));
		e->ContextRecord->Eip += info.length;
		return true;
	}

	return false;
}
