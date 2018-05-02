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
// *   src->devices->PCIDevice.h
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

#ifndef _PCIDEVICE_H_
#define _PCIDEVICE_H_

#include <cstdint>
#include <map>

#define	PCI_BAR_TYPE_IO			1
#define PCI_BAR_TYPE_MEMORY		0

#define PCI_CONFIG_DEVICE				0x00
#define PCI_CONFIG_BAR_0                0x10
#define PCI_CONFIG_BAR_1                0x14
#define PCI_CONFIG_BAR_2                0x18
#define PCI_CONFIG_BAR_3                0x1C
#define PCI_CONFIG_BAR_4                0x20
#define PCI_CONFIG_BAR_5                0x24


#define PCI_VENDOR_ID_NVIDIA			0x10DE

// MMIO addresses and sizes for the various xbox devices
#define NV2A_ADDR                               0xFD000000
#define NV2A_SIZE                               0x01000000
#define NV2A_PRAMIN_ADDR                        0xFD700000
#define NV2A_PRAMIN_SIZE                        0x100000
#define NV2A_USER_ADDR                          0xFD800000
#define NV2A_USER_SIZE                          0x800000
#define APU_BASE                                0xFE800000
#define APU_SIZE                                0x80000
#define AC97_BASE                               0xFEC00000
#define AC97_SIZE                               0x1000
#define USB0_BASE                               0xFED00000
#define USB1_BASE                               0xFED08000
#define USB_SIZE                                0x1000
#define NVNet_BASE                              0xFEF00000
#define NVNet_SIZE                              0x400
#define BIOS_BASE                               0xFF000000 // this takes into account that the bios covers the top 16 MiB of memory
#define BIOS_XBOX_SIZE                          0xFFFE00
#define BIOS_CHIHIRO_SIZE                       0x1000000
#define MCPX_BASE                               0xFFFFFE00
#define MCPX_SIZE                               0x200

class PCIDevice;

typedef struct
{

} PCIBarMemory;

typedef struct
{

} PCIBarIO;

typedef struct {
	union {
		struct {
			uint32_t type : 1;
			uint32_t locatable : 2;
			uint32_t prefetchable : 1;
			uint32_t address : 28;
		}Memory;

		struct {
			uint32_t type : 1;
			uint32_t reserved : 1;
			uint32_t address : 30;
		}IO;	

		struct {
			uint32_t type : 1;
			uint32_t other : 31;
		}Raw ;

		uint32_t value;
	};
	
} PCIBarRegister;

typedef struct {
	uint32_t size;
	PCIBarRegister reg;
	int index;
	PCIDevice* pDevice;
} PCIBar;

class PCIDevice {
	// PCI Device Interface
public:
	virtual void Init(unsigned int address) = 0;
	virtual void Reset() = 0;
	virtual uint32_t IORead(int barIndex, uint32_t port, unsigned size) = 0;
	virtual void IOWrite(int barIndex, uint32_t port, uint32_t value, unsigned size) = 0;
	virtual uint32_t MMIORead(int barIndex, uint32_t addr, unsigned size) = 0;
	virtual void MMIOWrite(int barIndex, uint32_t addr, uint32_t value, unsigned size) = 0;

	// PCI Device Implementation
public:
	bool GetIOBar(uint32_t port, PCIBar* bar);
	bool GetMMIOBar(uint32_t addr, PCIBar * bar);
	bool RegisterBAR(int index, uint32_t size, uint32_t defaultValue);
	bool UpdateBAR(int index, uint32_t newValue);
	uint32_t ReadConfigRegister(uint32_t reg);
	void WriteConfigRegister(uint32_t reg, uint32_t value);
protected:
	std::map<int, PCIBar> m_BAR;
	uint16_t m_DeviceId;
	uint16_t m_VendorId;
/* Unused?
private:

	static uint64_t MMIOBarRead(struct uc_struct* uc, void* pBar, uint64_t addr, unsigned size);
	static void MMIOBarWrite(struct uc_struct* uc, void* pBar, uint64_t addr, uint64_t value, unsigned size);
*/
};

#endif