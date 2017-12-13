#ifndef _SMBUS_H_
#define _SMBUS_H_

#include "PCIDevice.h"
#include "SMDevice.h"

#include <map>

#define SMB_ADDR_OFFSET			0xE0
#define SMB_IOSIZE				16
#define SMB_IRQ					11

#define SMB_GLOBAL_STATUS       0x0
#define SMB_GLOBAL_ENABLE       0x2
#define SMB_HOST_ADDRESS        0x4
#define SMB_HOST_DATA           0x6
#define SMB_HOST_COMMAND        0x8
#define SMB_HOST_BLOCK_DATA     0x9
#define SMB_HAS_DATA            0xA
#define SMB_HAS_DEVICE_ADDRESS  0xC
#define SMB_HAS_HOST_ADDRESS    0xE
#define SMB_SNOOP_ADDRESS       0xF

/* AMD756 constants */
#define AMD756_QUICK        0x00
#define AMD756_BYTE         0x01
#define AMD756_BYTE_DATA    0x02
#define AMD756_WORD_DATA    0x03
#define AMD756_PROCESS_CALL 0x04
#define AMD756_BLOCK_DATA   0x05

#define GS_ABRT_STS (1 << 0)
#define GS_COL_STS  (1 << 1)
#define GS_PRERR_STS    (1 << 2)
#define GS_HST_STS  (1 << 3)
#define GS_HCYC_STS (1 << 4)
#define GS_TO_STS   (1 << 5)
#define GS_SMB_STS  (1 << 11)

#define GS_CLEAR_STS    (GS_ABRT_STS | GS_COL_STS | GS_PRERR_STS |  GS_HCYC_STS | GS_TO_STS )

#define GE_CYC_TYPE_MASK    (7)
#define GE_HOST_STC     (1 << 3)

#define GE_HCYC_EN      (1 << 4)
#define GE_ABORT        (1 << 5)

class SMBus : public PCIDevice {
	public:
		using PCIDevice::PCIDevice;

		// PCI Functions
		void Init();
		void Reset();
		uint32_t IORead(int barIndex, uint32_t addr);
		void IOWrite(int barIndex, uint32_t addr, uint32_t data);
		uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size);
		void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size);

		// Misc
		void ConnectDevice(uint8_t addr, SMDevice *device);

	private:
		uint8_t m_Status;
		uint8_t m_Control;
		uint8_t m_Command;
		uint8_t m_Address;
		uint8_t m_Data0;
		uint8_t m_Data1;
		uint8_t m_Data[32];
		uint8_t m_Index;

		void ExecuteTransaction();

		void QuickCommand(uint8_t addr, bool read);
		uint8_t ReceiveByte(uint8_t addr);
		uint8_t ReadByte(uint8_t addr, uint8_t command);
		uint16_t ReadWord(uint8_t addr, uint8_t command);
		int ReadBlock(uint8_t addr, uint8_t command, uint8_t *data);

		void SendByte(uint8_t addr, uint8_t data);
		void WriteByte(uint8_t addr, uint8_t command, uint8_t value);
		void WriteWord(uint8_t addr, uint8_t command, uint16_t value);
		void WriteBlock(uint8_t addr, uint8_t command, uint8_t* data, int length);

		std::map<uint8_t, SMDevice*> m_Devices;
};

extern SMBus* g_SMBus;

#endif