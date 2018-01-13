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
	bool read = m_Address & 0x01;
	uint8_t addr = (m_Address >> 1) & 0x7f;
	auto it = m_Devices.find(addr);
	if (it == m_Devices.end()) {
		m_Status |= GS_PRERR_STS;
		printf("SMBus::ExecuteTransaction: Invalid Device (Addr: 0x%X\t Read: %X\t Prot: 0x%X)\n", addr, read, prot);
		return;
	}

	SMDevice *pDevice = it->second;
	switch (prot) {
		case AMD756_QUICK:
			pDevice->QuickCommand(read);
			break;
		case AMD756_BYTE:
			if (read) {
				m_Data0 = pDevice->ReceiveByte();
			} else {
				pDevice->SendByte(m_Data0); // TODO : Was m_Command correct?
			}
			break;
		case AMD756_BYTE_DATA:
			if (read) {
				m_Data0 = pDevice->ReadByte(m_Command);
			} else {
				pDevice->WriteByte(m_Command, m_Data0);
			}
			break;
		case AMD756_WORD_DATA:
			if (read) {
				uint16_t val;
				val = pDevice->ReadWord(m_Command);
				m_Data0 = val & 0xFF;
				m_Data1 = val >> 8;
			} else {
				pDevice->WriteWord(m_Command, m_Data0);
			}
			break;
		case AMD756_BLOCK_DATA:
			if (read) {
				m_Data0 = pDevice->ReadBlock(m_Command, m_Data);
			} else {
				pDevice->WriteBlock(m_Command, m_Data, m_Data0);
			}
			break;
		default:
			m_Status |= GS_PRERR_STS;
			return;
	}

	m_Status |= GS_HCYC_STS;
}


uint32_t SMBus::IORead(int barIndex, uint32_t addr, unsigned size)
{
	if (barIndex != 1) {
		return 0;
	}

	// For now, make SMBus only support byte-reads
	if (size != 1) {
		return 0; // TODO : Set GS_PRERR_STS in m_Status too?
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

void SMBus::IOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size)
{
	if (barIndex != 1) {
		return;
	}

	// For now, make SMBus only support byte-reads
	if (size != 1) {
		return; // TODO : Set GS_PRERR_STS in m_Status too?
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