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

bool PCIBus::IORead(uint32_t addr, uint32_t* data)
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		PCIBar bar;
		if (it->second->GetIOBar(addr, &bar)) {
			*data = it->second->IORead(bar.index, addr - bar.reg.IO.address);
			return true;
		}
	}

	return false;
}

bool PCIBus::IOWrite(uint32_t addr, uint32_t value)
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		PCIBar bar;
		if (it->second->GetIOBar(addr, &bar)) {
			it->second->IOWrite(bar.index, addr - bar.reg.IO.address, value);
			return true;
		}
	}

	return false;
}

bool PCIBus::MMIORead(uint32_t addr, uint32_t* data, unsigned size)
{
	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		PCIBar bar;
		if (it->second->GetMMIOBar(addr, &bar)) {
			*data = it->second->MMIORead(bar.index, addr - bar.reg.Memory.address, size);
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
			it->second->MMIOWrite(bar.index, addr - bar.reg.IO.address, value, size);
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