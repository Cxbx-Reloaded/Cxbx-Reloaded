// ******************************************************************
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
extern "C"
{
#endif

#include "EmuShared.h" // For XBE_MAX_VA, XBE_IMAGE_BASE and CXBX_BASE_OF_CODE

// The following code reserves virtual addresses from 0x00011000 upwards;
#define VM_PLACEHOLDER_SIZE (XBE_MAX_VA - XBE_IMAGE_BASE - CXBX_BASE_OF_CODE)

// First, declare the '.text' section again :
#pragma section(".text") // Note : 'read,write,execute' would cause a warning
// Then place the following variable into the '.text' section :
__declspec(allocate(".text"))
// This variable *MUST* be this large, for it to take up address space
// so that all other code and data in this module are placed outside of the
// maximum virtual memory range.
unsigned char virtual_memory_placeholder[VM_PLACEHOLDER_SIZE]; // = { OPCODE_NOP_90 };
// TODO : Try to get the same result without enlarging our executable by 128 MB!


#if defined(__cplusplus)
}
#endif

#endif // RESERVEDMEMORY_H
