// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuX86.cpp
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
#ifndef EMUX86_H
#define EMUX86_H

#include <cstdint>

#define EMUX86_EFLAG_CF 0
#define EMUX86_EFLAG_PF 2
#define EMUX86_EFLAG_AF 4
#define EMUX86_EFLAG_ZF 6
#define EMUX86_EFLAG_SF 7
#define EMUX86_EFLAG_TF 8
#define EMUX86_EFLAG_IF 9
#define EMUX86_EFLAG_DF 10
#define EMUX86_EFLAG_OF 11
#define EMUX86_EFLAG_IOPL1 12
#define EMUX86_EFLAG_IOPL2 13
#define EMUX86_EFLAG_NT 14
#define EMUX86_EFLAG_RF 16
#define EMUX86_EFLAG_VM 17
#define EMUX86_EFLAG_AC 18
#define EMUX86_EFLAG_VIF 19
#define EMUX86_EFLAG_VIP 20
#define EMUX86_EFLAG_ID 21

void EmuX86_Init();
int EmuX86_OpcodeSize(uint8_t *Eip);
bool EmuX86_DecodeException(LPEXCEPTION_POINTERS e);
uint32_t EmuX86_IORead32(xbaddr addr);
uint16_t EmuX86_IORead16(xbaddr addr);
uint8_t EmuX86_IORead8(xbaddr addr);
void EmuX86_IOWrite32(xbaddr addr, uint32_t value);
void EmuX86_IOWrite16(xbaddr addr, uint16_t value);
void EmuX86_IOWrite8(xbaddr addr, uint8_t value);

#endif
