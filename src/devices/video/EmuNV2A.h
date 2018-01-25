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

#define NV_PMC_SIZE                 0x001000
#define _NV_PFIFO_SIZE               0x002000 // Underscore prefix to prevent clash with NV_PFIFO_SIZE
#define NV_PVIDEO_SIZE              0x001000
#define NV_PTIMER_SIZE              0x001000
#define NV_PFB_SIZE                 0x001000
#define NV_PGRAPH_SIZE              0x002000
#define NV_PCRTC_SIZE               0x001000
#define NV_PRAMDAC_SIZE             0x001000
#define NV_PRAMIN_ADDR   0x00700000
#define NV_PRAMIN_SIZE              0x100000

typedef volatile DWORD *PPUSH;

typedef struct {
	DWORD Ignored[0x10];
	PPUSH Put; // On Xbox1, this field is only written to by the CPU (the GPU uses this as a trigger to start executing from the given address)
	PPUSH Get; // On Xbox1, this field is only read from by the CPU (the GPU reflects in here where it is/stopped executing)
	PPUSH Reference; // TODO : xbaddr / void* / DWORD ? 
	DWORD Ignored2[0x7ED];
} Nv2AControlDma;

uint32_t EmuNV2A_Read(xbaddr addr, int size);
void EmuNV2A_Write(xbaddr addr, uint32_t value, int size);

#define PUSH_TYPE_MASK         0x00000002 // 2 bits
#define PUSH_TYPE_SHIFT        0
#define PUSH_TYPE_METHOD        0 // method
#define PUSH_TYPE_JMP_FAR       1 // jump far
#define PUSH_TYPE_CALL_FAR      2 // call far
#define PUSH_TYPE_METHOD_UNUSED 3 // method (unused)
#define PUSH_METHOD_MASK       0x00001FFC // 12 bits
#define PUSH_METHOD_SHIFT      0 // Dxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)
#define PUSH_SUBCH_MASK        0x0000E000 // 3 bits
#define PUSH_SUBCH_SHIFT       13
#define PUSH_COUNT_MASK        0x1FFC0000 // 11 bits
#define PUSH_COUNT_SHIFT       18
#define PUSH_INSTR_MASK        0xE0000000 // 3 bits
#define PUSH_INSTR_SHIFT       29
#define PUSH_INSTR_IMM_INCR     0 // immediate, increment
#define PUSH_INSTR_JMP_NEAR     1 // near jump
#define PUSH_INSTR_IMM_NOINC    2 // immediate, no-increment
#define PUSH_ADDR_FAR_MASK     0xFFFFFFFC // 30 bits
#define PUSH_ADDR_FAR_SHIFT    0
#define PUSH_ADDR_NEAR_MASK    0x1FFFFFFC // 27 bits
#define PUSH_ADDR_NEAR_SHIFT   0 // Cxbx note : Not 2, because methods are actually DWORD offsets (and thus defined with increments of 4)

#define PUSH_TYPE(dwPushCommand) ((dwPushCommand & PUSH_TYPE_MASK) >> PUSH_TYPE_SHIFT)
#define PUSH_METHOD(dwPushCommand) ((dwPushCommand & PUSH_METHOD_MASK) >> PUSH_METHOD_SHIFT)
#define PUSH_SUBCH(dwPushCommand) ((dwPushCommand & PUSH_SUBCH_MASK) >> PUSH_SUBCH_SHIFT)
#define PUSH_COUNT(dwPushCommand) ((dwPushCommand & PUSH_COUNT_MASK) >> PUSH_COUNT_SHIFT)
#define PUSH_INSTR(dwPushCommand) ((dwPushCommand & PUSH_INSTR_MASK) >> PUSH_INSTR_SHIFT)
#define PUSH_ADDR_FAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_FAR_MASK) >> PUSH_ADDR_FAR_SHIFT)
#define PUSH_ADDR_NEAR(dwPushCommand) ((dwPushCommand & PUSH_ADDR_NEAR_MASK) >> PUSH_ADDR_NEAR_SHIFT)

#define PUSH_METHOD_MAX ((PUSH_METHOD_MASK | 3) >> PUSH_METHOD_SHIFT) // = 8191
#define PUSH_SUBCH_MAX (PUSH_SUBCH_MASK >> PUSH_SUBCH_SHIFT) // = 7
#define PUSH_COUNT_MAX (PUSH_COUNT_MASK >> PUSH_COUNT_SHIFT) // = 2047

// Decode push buffer conmmand (inverse of D3DPUSH_ENCODE)
inline void D3DPUSH_DECODE(const DWORD dwPushCommand, DWORD &dwMethod, DWORD &dwSubCh, DWORD &dwCount)
{
	dwMethod = PUSH_METHOD(dwPushCommand);
	dwSubCh = PUSH_SUBCH(dwPushCommand);
	dwCount = PUSH_COUNT(dwPushCommand);
}

void EmuNV2A_Init();

void InitOpenGLContext();

#endif
