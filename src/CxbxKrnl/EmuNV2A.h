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

typedef struct {
	DWORD Ignored[0x10];
	DWORD* Put; // On Xbox1, this field is only written to by the CPU (the GPU uses this as a trigger to start executing from the given address)
	DWORD* Get; // On Xbox1, this field is only read from by the CPU (the GPU reflects in here where it is/stopped executing)
	DWORD Reference; // TODO : xbaddr / void* / DWORD ? 
	DWORD Ignored2[0x7ED];
} Nv2AControlDma;

uint32_t EmuNV2A_Read(xbaddr addr, int size);
void EmuNV2A_Write(xbaddr addr, uint32_t value, int size);

void InitOpenGLContext();

#endif
