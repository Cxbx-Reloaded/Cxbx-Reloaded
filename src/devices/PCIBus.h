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

#ifndef _PCIMANAGER_H_
#define _PCIMANAGER_H_

#include <cstdint>
#include <map>

#include "PCIDevice.h"

#define PORT_PCI_CONFIG_ADDRESS 0xCF8
#define PORT_PCI_CONFIG_DATA 0xCFC
#define PCI_CONFIG_REGISTER_MASK 0xFC

#define PCI_DEVFN(slot, func) ((((slot) & 0x1f) << 3) | ((func) & 0x07))

#define PCI_SLOT(devfn)	(((devfn) >> 3) & 0x1f) // 5 bits (PCIConfigAddressRegister.deviceNumber)
#define PCI_FUNC(devfn)	((devfn) & 0x07) // 3 bits (PCIConfigAddressRegister.functionNumber)

#define PCI_DEVID(bus, devfn)  ((((uint16_t)(bus)) << 8) | (devfn))

#define PCI_BUS_NUM(x) (((x) >> 8) & 0xff)

typedef struct {
	uint8_t registerNumber : 8;
	uint8_t functionNumber : 3; // PCI_FUNC
	uint8_t deviceNumber : 5; // PCI_SLOT
	uint8_t busNumber : 8; // PCI_BUS_NUM
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

#endif
