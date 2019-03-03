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
// *  (c) 2018 Luke Usher <luke.usher@outlook.coM>
// *
// *  All rights reserved
// *
// ******************************************************************

#include <cstdio>

#include "AC97Device.h"

void AC97Device::Init()
{
	PCIBarRegister r;
	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = AC97_BASE >> 4;
	RegisterBAR(0, AC97_SIZE, r.value);

	m_DeviceId = 0x01B1;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;
}
	
void AC97Device::Reset()
{

}

uint32_t AC97Device::IORead(int barIndex, uint32_t addr, unsigned size)
{
	printf("AC97Device: Unimplemented IORead %X\n", addr);
	return 0;
}

void AC97Device::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	printf("AC97Device: Unimplemented IOWrite %X\n", addr);
}

uint32_t AC97Device::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	printf("AC97Device: Unimplemented MMIORead %X\n", addr);
	return 0;
}

void AC97Device::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	printf("AC97Device: Unimplemented MMIOWrite %X\n", addr);
}
