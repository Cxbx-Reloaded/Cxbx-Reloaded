#include <cstdio>

#include "SMBus.h"

void SMBus::Init()
{
	PCIBarRegister r;
	r.Raw.type = PCI_BAR_TYPE_IO;
	r.IO.address = 0xC000;
	RegisterBAR(1, 32, r.value);

	m_DeviceId = 0x01B4;
	m_VendorId = PCI_VENDOR_ID_NVIDIA;
}
	
void SMBus::Reset()
{
	m_Status = 0;

	for (auto it = m_Devices.begin(); it != m_Devices.end(); ++it) {
		it->second->Reset();
	}
}

void SMBus::ConnectDevice(uint8_t addr, SMDevice *pDevice)
{

	if (m_Devices.find(addr) != m_Devices.end()) {
		printf("PCIBus: Attempting to connect two devices to the same device address\n");
		return;
	}

	m_Devices[addr] = pDevice;
	pDevice->Init();
}

void SMBus::ExecuteTransaction()
{
	uint8_t prot = m_Control & GE_CYC_TYPE_MASK;
	uint8_t read = m_Address & 0x01;
	uint8_t addr = (m_Address >> 1) & 0x7f;

	switch (prot) {
		case AMD756_QUICK:
			QuickCommand(addr, read);
			break;
		case AMD756_BYTE:
			if (read) {
				m_Data0 = ReceiveByte(addr);
			} else {
				SendByte(addr, m_Command);
			}
			break;
		case AMD756_BYTE_DATA:
			if (read) {
				m_Data0 = ReadByte(addr, m_Command);
			} else {
				WriteByte(addr, m_Command, m_Data0);
			}
			break;
		case AMD756_WORD_DATA:
			if (read) {
				uint16_t val;
				val = ReadWord(addr, m_Command);
				m_Data0 = val;
				m_Data1 = val >> 8;
			} else {
				WriteWord(addr, m_Command, m_Data0);
			}
			break;
		case AMD756_BLOCK_DATA:
			if (read) {
				m_Data0 = ReadBlock(addr, m_Command, m_Data);
			} else {
				WriteBlock(addr, m_Command, m_Data, m_Data0);
			}
			break;
		default:
			m_Status |= GS_PRERR_STS;
			return;
	}

	m_Status |= GS_HCYC_STS;
}

void SMBus::QuickCommand(uint8_t addr, int read)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->QuickCommand(read);
	}

	printf("SMBus::QuickCommand: Invalid Device (Addr: 0x%X)\n", addr);
}

uint8_t SMBus::ReceiveByte(uint8_t addr)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->ReceiveByte();
	}

	printf("SMBus::ReceiveByte: Invalid Device (Addr: 0x%X)\n", addr);
	return 0;
}

uint8_t SMBus::ReadByte(uint8_t addr, uint8_t command)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->ReadByte(command);
	}

	printf("SMBus::ReadByte: Invalid Device Read (Addr: 0x%X\t Command: %X)\n", addr, command);
	return 0;
}

uint16_t SMBus::ReadWord(uint8_t addr, uint8_t command)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->ReadWord(command);
	}

	printf("SMBus::ReadWord: Invalid Device Read (Addr: 0x%X\t Command: %X)\n", addr, command);
	return 0;
}

int SMBus::ReadBlock(uint8_t addr, uint8_t command, uint8_t *data)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->ReadBlock(command, data);
	}

	printf("SMBus::ReadBlock: Invalid Device Read (Addr: 0x%X\t Command: %X\t Data: 0x%X)\n", addr, command, data);
	return 0;
}

void SMBus::SendByte(uint8_t addr, uint8_t data)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->SendByte(data);
	}

	printf("SMBus::SendByte: Invalid Device (Addr: 0x%X\t Data: 0x%02X)\n", addr, data);
}

void SMBus::WriteByte(uint8_t addr, uint8_t command, uint8_t value)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->WriteByte(command, value);
	}

	printf("SMBus::WriteByte: Invalid Device Write (Addr: 0x%X\t Command: %X\t Value: 0x%02X)\n", addr, command, value);
}

void SMBus::WriteWord(uint8_t addr, uint8_t command, uint16_t value)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->WriteWord(command, value);
	}

	printf("SMBus::WriteWord: Invalid Device Write (Addr: 0x%X\t Command: %X\t Value: 0x%04X)\n", addr, command, value);
}

void SMBus::WriteBlock(uint8_t addr, uint8_t command, uint8_t* data, int length)
{
	auto it = m_Devices.find(addr);
	if (it != m_Devices.end()) {
		return it->second->WriteBlock(command, data, length);
	}

	printf("SMBus::WriteBlock: Invalid Device Write (Addr: 0x%X\t Command: %X\t Data: 0x%X\t Length: %d)\n", addr, command, data, length);
}

uint32_t SMBus::IORead(int barIndex, uint32_t addr)
{
	if (barIndex != 1) {
		return 0;
	}

	uint32_t value;
	addr &= 0x3f;

	switch (addr) {
		case SMB_GLOBAL_STATUS:
			value = m_Status;
			break;
		case SMB_GLOBAL_ENABLE:
			value = m_Control & 0x1f;
			break;
		case SMB_HOST_COMMAND:
			value = m_Command;
			break;
		case SMB_HOST_ADDRESS:
			value = m_Address;
			break;
		case SMB_HOST_DATA:
			value = m_Data0;
			break;
		case SMB_HOST_DATA + 1:
			value = m_Data1;
			break;
		case SMB_HOST_BLOCK_DATA:
			value = m_Data[m_Index++];
			if (m_Index > 31) {
				m_Index = 0;
			}
			break;
		default:
			value = 0;
			break;
	}

	return value;
}

void SMBus::IOWrite(int barIndex, uint32_t addr, uint32_t value)
{
	if (barIndex != 1) {
		return;
	}

	addr &= 0x3f;
	switch (addr) {
		case SMB_GLOBAL_STATUS:
			// If a new status is being set and interrupts are enabled, trigger an interrupt
			if ((m_Control & GE_HCYC_EN) && ((value & GS_CLEAR_STS) & (~(m_Status & GS_CLEAR_STS)))) {
				// TODO: RaiseIRQ
			} else {
				// TODO LowerIRQ
			}

			if (value & GS_CLEAR_STS) {
				m_Status = 0;
				m_Index = 0;
			} else if (value & GS_HCYC_STS) {
				m_Status = GS_HCYC_STS;
				m_Index = 0;
			} else {
				m_Status = GS_HCYC_STS;
				m_Index = 0;
			}

			break;
		case SMB_GLOBAL_ENABLE:
			m_Control = value;
			if (value & GE_ABORT) {
				m_Status |= GS_ABRT_STS;
			}

			if (value & GE_HOST_STC) {
				ExecuteTransaction();

				if ((value & GE_HCYC_EN) && (m_Status & GS_CLEAR_STS)) {
					// TODO: RaiseIRQ
				}
			}

			break;
		case SMB_HOST_COMMAND:
			m_Command = value;
			break;
		case SMB_HOST_ADDRESS:
			m_Address = value;
			break;
		case SMB_HOST_DATA:
			m_Data0 = value;
			break;
		case SMB_HOST_DATA + 1:
			m_Data1 = value;
			break;
		case SMB_HOST_BLOCK_DATA:
			m_Data[m_Index++] = value;
			if (m_Index > 31) {
				m_Index = 0;
			}
			break;
		default:
			break;
	}
}

uint32_t SMBus::MMIORead(int barIndex, uint32_t addr, unsigned size)
{
	return 0;
}

void SMBus::MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{

}