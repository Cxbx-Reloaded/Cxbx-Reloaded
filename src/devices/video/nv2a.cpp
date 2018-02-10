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
// *   src->devices->video->nv2a.cpp
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
// *  (c) 2017-2018 Luke Usher <luke.usher@outlook.com>
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "NV2A"

#include "CxbxKrnl\CxbxKrnl.h" // For XBOX_MEMORY_SIZE, DWORD, etc
#include "EmuNV2A.h" // For now, use EmuNV2A

#include "nv2a.h"

/* NV2ADevice */

// PCI Device functions

void NV2ADevice::Init()
{
	PCIBarRegister r;

	// Register Memory bar :
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = NV2A_ADDR >> 4;
	RegisterBAR(0, NV2A_SIZE, r.value);

	// Register physical memory on bar 1
	r.Memory.address = 0;
	RegisterBAR(1, XBOX_MEMORY_SIZE, r.value); // TODO : Read g_PhysicalMemory->Size
	/* LukeUsher commented at https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/pull/882#discussion_r162871029

	This is not right: I should have done a better review ;)
	The starting address here is the physical address in the Xbox memory the VRAM should be placed at,
	and should point to the TILED memory region ((0xF0000000 >> 4))

	This maps VRAM address 0 to 0xF0000000. (this is what our TILED memory is: VRAM accessed via the GPU device)
	*/

	m_DeviceId = 0x02A5;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;

	// For now, forward to EmuNv2A
	EmuNV2A_Init();
}

void NV2ADevice::Reset()
{
}

uint32_t NV2ADevice::IORead(int barIndex, uint32_t port, unsigned size)
{
	return 0;
}

void NV2ADevice::IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size)
{
}

uint32_t NV2ADevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{ 
	switch (barIndex) {
	case 0:
		uint32_t value;
		// For now, forward to EmuNV2A
		{
			// Access NV2A regardless weither HLE is disabled or not (ignoring bLLE_GPU)
			value = EmuNV2A_Read(addr, size);
			// Note : EmuNV2A_Read does it's own logging
		}
		// TODO : call block handler
		return value;
	case 1:
		return 0; // TODO : access physical memory
	}

	// TODO : Log unexpected bar access
	return 0;
}

void NV2ADevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	switch (barIndex) {
	case 0:
		// For now, forward to EmuNV2A
		{
			// Access NV2A regardless weither HLE is disabled or not (ignoring bLLE_GPU)
			EmuNV2A_Write(addr, value, size);
			// Note : EmuNV2A_Write does it's own logging
		}
		// TODO : call block handler
		return;
	case 1:
		// TODO : access physical memory
		return;
	}

	// TODO : Log unexpected bar access
}
