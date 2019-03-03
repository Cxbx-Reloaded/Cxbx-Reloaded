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
// *  (c) 2018 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include "PCIDevice.h"

bool PCIDevice::GetIOBar(uint32_t port, PCIBar* bar)
{
	for (auto it = m_BAR.begin(); it != m_BAR.end(); ++it) {
		if (it->second.reg.Raw.type == PCI_BAR_TYPE_IO && (port >= it->second.reg.IO.address) && (port < it->second.reg.IO.address + it->second.size)) {
			*bar = it->second;
			return true;
		}
	}

	return false;
}

bool PCIDevice::GetMMIOBar(uint32_t addr, PCIBar* bar)
{
	for (auto it = m_BAR.begin(); it != m_BAR.end(); ++it) {
		if (it->second.reg.Raw.type == PCI_BAR_TYPE_MEMORY && (addr >= (it->second.reg.Memory.address << 4)) && (addr < (it->second.reg.Memory.address << 4) + it->second.size)) {
			*bar = it->second;
			return true;
		}
	}

	return false;
}

bool PCIDevice::RegisterBAR(int index, uint32_t size, uint32_t defaultValue)
{
	if (m_BAR.find(index) != m_BAR.end()) {
		printf("PCIDevice::RegisterBar: Trying to register a BAR that is already allocated (index: %d)\n", index);
		return false;
	}

	PCIBar bar;
	bar.size = size;
	bar.pDevice = this;
	bar.reg.value = defaultValue;
	bar.index = index;
	m_BAR[index] = bar;

	return true;
}

bool PCIDevice::UpdateBAR(int index, uint32_t newValue)
{
	auto it = m_BAR.find(index);
	if (it == m_BAR.end()) {
		printf("PCIDevice::UpdateBAR: Trying to update a BAR that does not exist (index: %d, value 0x%08X)\n", index, newValue);
		return false;
	}

	it->second.reg.value = newValue;

	return true;
}

uint32_t PCIDevice::ReadConfigRegister(uint32_t reg)
{
	switch (reg) {
		case PCI_CONFIG_DEVICE:
			return (m_DeviceId << 16) | m_VendorId;
		case PCI_CONFIG_BAR_0:
		case PCI_CONFIG_BAR_1:
		case PCI_CONFIG_BAR_2:
		case PCI_CONFIG_BAR_3:
		case PCI_CONFIG_BAR_4:
		case PCI_CONFIG_BAR_5:
		{
			int barIndex = (reg - PCI_CONFIG_BAR_0) / 4;
			auto it = m_BAR.find(barIndex);
			if (it == m_BAR.end()) {
				printf("PCIDevice::ReadConfigRegister: Trying to Read a BAR that does not exist (index: %d)\n", barIndex);
				return 0xFFFFFFFF;
			}

			return it->second.reg.value;
		}
		default:
			printf("PCIDevice::ReadConfigRegister: Unhandled Register %X\n", reg);
			break;
	}

	return 0;
}

void PCIDevice::WriteConfigRegister(uint32_t reg, uint32_t value)
{
	switch (reg) {
		case PCI_CONFIG_BAR_0:
		case PCI_CONFIG_BAR_1:
		case PCI_CONFIG_BAR_2:
		case PCI_CONFIG_BAR_3:
		case PCI_CONFIG_BAR_4:
		case PCI_CONFIG_BAR_5:
		{
			int barIndex = (reg - PCI_CONFIG_BAR_0) / 4;
			UpdateBAR(barIndex, value);
			break;
		}
		default:
			printf("PCIDevice::WriteConfigRegister: Unhandled Register %X\n", reg);
			break;
	}
}