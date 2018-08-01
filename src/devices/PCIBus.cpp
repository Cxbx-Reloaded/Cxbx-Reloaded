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
// *   src->devices->PCIBus.h
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

#include "PCIBus.h"
#include <cstdio>

void PCIBus::ConnectDevice(uint32_t deviceId, PCIDevice *pDevice)
{
	if (m_Devices.find(deviceId) != m_Devices.end()) {
		printf("PCIBus: Attempting to connect two devices to the same device address\n");
		return;
	}

	m_Devices[deviceId] = pDevice;
	pDevice->Init();
}

void PCIBus::IOWriteConfigAddress(uint32_t pData) 
{
	memcpy(&m_configAddressRegister, &pData, sizeof(PCIConfigAddressRegister));
}

uint32_t PCIBus::IOReadConfigData()
{
	auto it = m_Devices.find(PCI_DEVID(m_configAddressRegister.busNumber, m_configAddressRegister.deviceNumber));
	if (it != m_Devices.end()) {
		return it->second->ReadConfigRegister(m_configAddressRegister.registerNumber & PCI_CONFIG_REGISTER_MASK);
	}

	printf("PCIBus::IOReadConfigData: Invalid Device Write (Bus: %d\t Slot: %d\t Function: %d)\n", m_configAddressRegister.busNumber,
		PCI_SLOT(m_configAddressRegister.deviceNumber),
		PCI_FUNC(m_configAddressRegister.deviceNumber));

	// Unpopulated PCI slots return 0xFFFFFFFF
	return 0xFFFFFFFF;
}

void PCIBus::IOWriteConfigData(uint32_t pData) {
	auto it = m_Devices.find(PCI_DEVID(m_configAddressRegister.busNumber, m_configAddressRegister.deviceNumber));
	if (it != m_Devices.end()) {
		it->second->WriteConfigRegister(m_configAddressRegister.registerNumber & PCI_CONFIG_REGISTER_MASK, pData);
		return;
	}

	printf("PCIBus::IOWriteConfigData: Invalid Device Write (Bus: %d\t Slot: %d\t Function: %d)\n", m_configAddressRegister.busNumber, 
		PCI_SLOT(m_configAddressRegister.deviceNumber), 
		PCI_FUNC(m_configAddressRegister.deviceNumber));
}

bool PCIBus::IORead(uint32_t addr, uint32_t* data, unsigned size)
{
	switch (addr) {
	case PORT_PCI_CONFIG_DATA: // 0xCFC
		if (size == sizeof(uint32_t)) {
			*data = IOReadConfigData();
			return true;
		} // TODO : else log wrong size-access?
		break;
	default:
		for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
			PCIBar bar;
			if (it->second->GetIOBar(addr, &bar)) {
				*data = it->second->IORead(bar.index, addr - bar.reg.IO.address, size);
				return true;
			}
		}
	}

	return false;
}

bool PCIBus::IOWrite(uint32_t addr, uint32_t value, unsigned size)
{
	switch (addr) {
	case PORT_PCI_CONFIG_ADDRESS: // 0xCF8
		if (size == sizeof(uint32_t)) {
			IOWriteConfigAddress(value);
			return true;
		} // TODO : else log wrong size-access?
		break;
	case PORT_PCI_CONFIG_DATA: // 0xCFC
		if (size == sizeof(uint32_t)) {
			IOWriteConfigData(value);
			return true; // TODO : Should IOWriteConfigData() success/failure be returned?
		} // TODO : else log wrong size-access?
		break;
	default:
		for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
			PCIBar bar;
			if (it->second->GetIOBar(addr, &bar)) {
				it->second->IOWrite(bar.index, addr - bar.reg.IO.address, value, size);
				return true;
			}
		}
	}

	return false;
}

bool PCIBus::MMIORead(uint32_t addr, uint32_t* data, unsigned size)
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		PCIBar bar;
		if (it->second->GetMMIOBar(addr, &bar)) {
			*data = it->second->MMIORead(bar.index, addr - (bar.reg.Memory.address << 4), size);
			return true;
		}
	}

	return false;
}

bool PCIBus::MMIOWrite(uint32_t addr, uint32_t value, unsigned size)
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		PCIBar bar;
		if (it->second->GetMMIOBar(addr, &bar)) {
			it->second->MMIOWrite(bar.index, addr - (bar.reg.Memory.address << 4), value, size);
			return true;
		}
	}

	return false;
}

void PCIBus::Reset()
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		it->second->Reset();
	}
}
