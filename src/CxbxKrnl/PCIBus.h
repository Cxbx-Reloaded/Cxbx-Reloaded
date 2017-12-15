#ifndef _PCIMANAGER_H_
#define _PCIMANAGER_H_

#include <cstdint>
#include <map>

#include "PCIDevice.h"

#define PORT_PCI_CONFIG_ADDRESS 0xCF8
#define PORT_PCI_CONFIG_DATA 0xCFC
#define PCI_CONFIG_REGISTER_MASK 0xFC

#define PCI_DEVFN(slot, func) ((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)	(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)	((devfn) & 0x07)
#define PCI_DEVID(bus, devfn)  ((((uint16_t)(bus)) << 8) | (devfn))
#define PCI_BUS_NUM(x) (((x) >> 8) & 0xff)

typedef struct {
	uint8_t registerNumber : 8;
	uint8_t functionNumber : 3;
	uint8_t deviceNumber : 5;
	uint8_t busNumber : 8;
	uint8_t reserved : 7;
	uint8_t enable : 1;
} PCIConfigAddressRegister;

class PCIBus {
public:
	void ConnectDevice(uint32_t deviceId, PCIDevice *pDevice);

	bool IORead(uint32_t addr, uint32_t* value, unsigned size);
	bool IOWrite(uint32_t addr, uint32_t value, unsigned size);

	bool MMIORead(uint32_t addr, uint32_t * data, unsigned size);
	bool MMIOWrite(uint32_t addr, uint32_t value, unsigned size);

	void Reset();
private:
	void IOWriteConfigAddress(uint32_t pData);
	void IOWriteConfigData(uint32_t pData);
	uint32_t IOReadConfigData();

	std::map<uint32_t, PCIDevice*> m_Devices;
	PCIConfigAddressRegister m_configAddressRegister;
};

extern PCIBus* g_PCIBus;

#endif