// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuX86.h
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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2016 Luke Usher <luke.usher@outlook.com>
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUNV2A_H
#define EMUNV2A_H

// Valid after PCI init :
#define NV20_REG_BASE_KERNEL 0xFD000000

#define NV2A_ADDR  0xFD000000
#define NV2A_SIZE             0x01000000

#define NV_PMC_ADDR      0x00000000
#define NV_PMC_SIZE                 0x001000
#define NV_PBUS_ADDR     0x00001000
#define NV_PBUS_SIZE                0x001000
#define NV_PFIFO_ADDR    0x00002000
#define NV_PFIFO_SIZE               0x002000
#define NV_PRMA_ADDR     0x00007000
#define NV_PRMA_SIZE                0x001000
#define NV_PVIDEO_ADDR   0x00008000
#define NV_PVIDEO_SIZE              0x001000
#define NV_PTIMER_ADDR   0x00009000
#define NV_PTIMER_SIZE              0x001000
#define NV_PCOUNTER_ADDR 0x0000A000
#define NV_PCOUNTER_SIZE            0x001000
#define NV_PVPE_ADDR     0x0000B000
#define NV_PVPE_SIZE                0x001000
#define NV_PTV_ADDR      0x0000D000
#define NV_PTV_SIZE                 0x001000
#define NV_PRMFB_ADDR    0x000A0000
#define NV_PRMFB_SIZE               0x020000
#define NV_PRMVIO_ADDR   0x000C0000
#define NV_PRMVIO_SIZE              0x001000
#define NV_PFB_ADDR      0x00100000
#define NV_PFB_SIZE                 0x001000
#define NV_PSTRAPS_ADDR  0x00101000
#define NV_PSTRAPS_SIZE             0x001000
#define NV_PGRAPH_ADDR   0x00400000
#define NV_PGRAPH_SIZE              0x002000
#define NV_PCRTC_ADDR    0x00600000
#define NV_PCRTC_SIZE               0x001000
#define NV_PRMCIO_ADDR   0x00601000
#define NV_PRMCIO_SIZE              0x001000
#define NV_PRAMDAC_ADDR  0x00680000
#define NV_PRAMDAC_SIZE             0x001000
#define NV_PRMDIO_ADDR   0x00681000
#define NV_PRMDIO_SIZE              0x001000
#define NV_PRAMIN_ADDR   0x00700000
#define NV_PRAMIN_SIZE              0x100000
#define NV_USER_ADDR     0x00800000
#define NV_USER_SIZE                0x800000

typedef struct {
	DWORD Ignored[0x10];
	DWORD* Put; // On Xbox1, this field is only written to by the CPU (the GPU uses this as a trigger to start executing from the given address)
	DWORD* Get; // On Xbox1, this field is only read from by the CPU (the GPU reflects in here where it is/stopped executing)
	DWORD Reference; // TODO : xbaddr / void* / DWORD ? 
	DWORD Ignored2[0x7ED];
} Nv2AControlDma;

uint32_t EmuNV2A_Read(xbaddr addr, int size);
void EmuNV2A_Write(xbaddr addr, uint32_t value, int size);
void EmuNV2A_Init();

void InitOpenGLContext();

#endif
