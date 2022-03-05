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

#include "APUDevice.h"

extern uint32_t GetAPUTime();

// TODO: Everything :P
// TODO: Audio Processing/Thread

#define APU_VP_BASE 0x20000
#define APU_VP_SIZE 0x10000

#define APU_GP_BASE 0x30000
#define APU_GP_SIZE 0x10000

#define APU_EP_BASE 0x50000
#define APU_EP_SIZE 0x10000

void APUDevice::Init()
{
	PCIBarRegister r;
	r.Raw.type = PCI_BAR_TYPE_IO;
	r.IO.address = 0xD000 >> 4;
	RegisterBAR(0, 256, r.value);

	r.Raw.type = PCI_BAR_TYPE_IO;
	r.IO.address = 0xD200 >> 4;
	RegisterBAR(0, 128, r.value);

	r.Raw.type = PCI_BAR_TYPE_MEMORY;
	r.Memory.address = APU_BASE >> 4;
	RegisterBAR(2, APU_SIZE, r.value);

	m_DeviceId = 0x01B0;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;
}

void APUDevice::Reset()
{

}

uint32_t APUDevice::IORead(int barIndex, uint32_t addr, unsigned size)
{
	printf("APUDevice: Unimplemented IORead %X\n", addr);
	return 0;
}

void APUDevice::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	printf("APUDevice: Unimplemented IOWrite %X\n", addr);
}

uint32_t APUDevice::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
		return VPRead(addr - APU_VP_BASE, size);
	}

	if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
		return GPRead(addr - APU_GP_BASE, size);
	}

	if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
		return EPRead(addr - APU_EP_BASE, size);
	}

	switch (addr) {
		case 0x200C: return GetAPUTime();
	}

	printf("APUDevice: Unimplemented MMIORead %X\n", addr);
	return 0;
}

void APUDevice::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	if (addr >= APU_VP_BASE && addr < APU_VP_BASE + APU_VP_SIZE) {
		VPWrite(addr - APU_VP_BASE, value, size);
		return;
	}

	if (addr >= APU_GP_BASE && addr < APU_GP_BASE + APU_GP_SIZE) {
		GPWrite(addr - APU_GP_BASE, value, size);
		return;
	}

	if (addr >= APU_EP_BASE && addr < APU_EP_BASE + APU_EP_SIZE) {
		EPWrite(addr - APU_EP_BASE, value, size);
		return;
	}

	printf("APUDevice: Unimplemented MMIOWrite %X\n", addr);
}


uint32_t APUDevice::GPRead(uint32_t addr, unsigned size)
{
	printf("APUDevice: Unimplemented GP MMIORead %X\n", addr);
	return 0;
}

void APUDevice::GPWrite(uint32_t addr, uint32_t value, unsigned size)
{
	printf("APUDevice: Unimplemented GP MMIOWrite %X\n", addr);
}


uint32_t APUDevice::VPRead(uint32_t addr, unsigned size)
{
	switch (addr) {
		case 0x10: return 0x80; // HACK: Pretend the FIFO is always empty, bypasses hangs when APU isn't fully implemented
	}

	printf("APUDevice: Unimplemented VP MMIORead %X\n", addr);
	return 0;
}

void APUDevice::VPWrite(uint32_t addr, uint32_t value, unsigned size)
{
	printf("APUDevice: Unimplemented VP MMIOWrite %X\n", addr);
}


uint32_t APUDevice::EPRead(uint32_t addr, unsigned size)
{
	printf("APUDevice: Unimplemented EP MMIORead %X\n", addr);
	return 0;
}

void APUDevice::EPWrite(uint32_t addr, uint32_t value, unsigned size)
{
	printf("APUDevice: Unimplemented EP MMIOWrite %X\n", addr);
}
