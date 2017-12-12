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