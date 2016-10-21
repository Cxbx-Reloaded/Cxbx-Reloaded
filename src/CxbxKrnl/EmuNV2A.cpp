// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuNV2A.cpp
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
#include "EmuNV2A.h"
#include "nv2a_int.h" // from https://github.com/espes/xqemu/tree/xbox/hw/xbox

uint32_t EmuNV2A_PMC_Read32(uint32_t addr)
{
	switch (addr) {
	default:
		EmuWarning("EmuNV2A_PMC_Read32: Unknown Read Address %08X", addr);
	}
	
	return 0;
}

uint32_t EmuNV2A_PBUS_Read32(uint32_t addr)
{
	switch (addr) {
	case NV_PBUS_PCI_NV_0:
		return 0x10de;	// PCI_VENDOR_ID_NVIDIA	
		break;
	default:
		EmuWarning("EmuNV2A_PBUS_Read32: Unknown Read Address %08X", addr);
	}

	return 0;
}


uint32_t EmuNV2A_Read32(uint32_t addr)
{
	if (addr <= 0x1000) {
		return EmuNV2A_PMC_Read32(addr);
	}
	else if (addr <= 0x2000) {
		return EmuNV2A_PBUS_Read32(addr - 0x1000);
	}

	EmuWarning("EmuNV2A_Read32: Unknown Read Address %08X", addr);
	return 0;
}

void EmuNV2A_Write32(uint32_t addr, uint32_t value)
{
	EmuWarning("EmuNV2A_Write32: Unknown Write Address %08X (value %08X)", addr, value);
}
