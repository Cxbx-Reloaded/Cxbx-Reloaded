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
// *   Cxbx->devices->USBController->OHCI.cpp
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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#include "OHCI.h"
#include "..\CxbxKrnl\CxbxKrnl.h"

#define USB_HZ 12000000


typedef enum _USB_SPEED
{
	USB_SPEED_MASK_LOW =  1 << 0,
	USB_SPEED_MASK_FULL = 1 << 1,
}
USB_SPEED;

// global pointers to the two USB host controllers available on the Xbox
OHCI* g_pHostController1 = nullptr;
OHCI* g_pHostController2 = nullptr;


OHCI::OHCI(USBDevice* UsbObj)
{
	UsbInstance = UsbObj;

	for (int i = 0; i < 2; i++) {
		UsbInstance->USB_RegisterPort(&Registers.RhPort[i].Port, this, i, USB_SPEED_MASK_LOW | USB_SPEED_MASK_FULL);
	}
	USB_PacketInit(&UsbPacket);

	// Create the end-of-frame timer. Let's try a factor of 50 (1 virtual ms -> 50 real ms)
	pEndOfFrameTimer = Timer_Create(OHCI_FrameBoundaryWrapper, this, 50);

	UsbFrameTime = 1000000ULL; // 1 ms
	TicksPerUsbTick = 1000000000ULL / USB_HZ; // 83

	// Do a hardware reset
	OHCI_StateReset();
}

void OHCI::OHCI_FrameBoundaryWrapper(void* pVoid)
{
	static_cast<OHCI*>(pVoid)->OHCI_FrameBoundaryWorker();
}

void OHCI::OHCI_StateReset()
{
	// The usb state can be USB_Suspend if it is a software reset, and USB_Reset if it is a hardware
	// reset or cold boot

	// TODO: stop all the list processing here

	// Reset all registers
	// Remark: the standard says that RemoteWakeupConnected bit should be set during POST, cleared during hw reset
	// and ignored during a sw reset. However, VBox sets it on hw reset and XQEMU clears it. Considering that the Xbox
	// doesn't do POST, I will clear it.
	Registers.HcControl = 0;
	Registers.HcControl &= ~OHCI_CTL_HCFS;
	Registers.HcControl |= Reset;
	Registers.HcCommandStatus = 0;
	Registers.HcInterruptStatus = 0;
	Registers.HcInterruptEnable = OHCI_INTR_MASTER_INTERRUPT_ENABLED; // enable interrupts

	Registers.HcHCCA = 0;
	Registers.HcPeriodCurrentED = 0;
	Registers.HcControlHeadED = Registers.HcControlCurrentED = 0;
	Registers.HcBulkHeadED = Registers.HcBulkCurrentED = 0;
	Registers.HcDoneHead = 0;

	Registers.HcFmInterval = 0;
	Registers.HcFmInterval |= (0x2778 << 16); // TBD according to the standard, using what XQEMU sets (FSLargestDataPacket)
	Registers.HcFmInterval |= 0x2EDF; // bit-time of a frame. 1 frame = 1 ms (FrameInterval)
	Registers.HcFmRemaining = 0;
	Registers.HcFmNumber = 0;
	Registers.HcPeriodicStart = 0;

	Registers.HcRhDescriptorA = OHCI_RHA_NPS | 2; // The xbox lacks the hw to switch off the power on the ports and has 2 ports per HC
	Registers.HcRhDescriptorB = 0; // The attached devices are removable and use PowerSwitchingMode to control the power on the ports
	for (int i = 0; i < 2; i++)
	{
		OHCIPort* Port = &Registers.RhPort[i];
		Port->HcRhPortStatus = 0;
		//if (Port->port.device && Port->port.device->attached) {
			//usb_port_reset(&Port->port);
		//}
	}

	DbgPrintf("Usb-Ohci: Reset\n");
}

void OHCI::USB_PacketInit(USBPacket* packet)
{
	IOVector* vec = &packet->IoVec;
	vec->IoVec = new IoVec;
	vec->IoVecNumber = 0;
	vec->AllocNumber = 1;
	vec->Size = 0;
}
