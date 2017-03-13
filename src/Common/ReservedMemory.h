// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Common->ReservedMemory.h
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef RESERVEDMEMORY_H
#define RESERVEDMEMORY_H

#if defined(__cplusplus)
#pragma once
extern "C"
{
#endif

#include "CxbxKrnl/EmuShared.h" // For EMU_MAX_MEMORY_SIZE and OPCODE_NOP_90

// The following code claims 0x0001000 + 128 MB;
// First, declare the '.text' section again :
#pragma section(".text") // Note : 'read,write,execute' would cause a warning
// Then place the following variable into the '.text' section :
__declspec(allocate(".text"))
// This variable *MUST* be this large, for it to take up address space
// so that all other code and data in this module are placed outside of the
// maximum emulated memory range.
unsigned char emulated_memory_placeholder[EMU_MAX_MEMORY_SIZE]; // = { OPCODE_NOP_90 };
// TODO : Try to get the same result without enlarging the executable by 128 MB!

#if defined(__cplusplus)
}
#endif

#endif // RESERVEDMEMORY_H