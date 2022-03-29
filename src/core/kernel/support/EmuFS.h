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
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUFS_H
#define EMUFS_H

#include <vector>
#include "common\xbe\Xbe.h"
#include <windows.h>

// Get Xbox's TIB StackBase address from thread's StackBase.
xbox::PVOID EmuGetTIBStackBase(xbox::PVOID ThreadStackBase);

// generate stack size reserved for xbox threads to write on.
// espBaseAddress will return aligned DOWN address, do not rely on return's size for offset usage.
xbox::dword_xt EmuGenerateStackSize(xbox::addr_xt& espBaseAddress, IN xbox::ulong_xt TlsDataSize);

// initialize fs segment selector emulation
extern void EmuInitFS();

// generate fs segment selector
template<bool IsHostThread = false>
void EmuGenerateFS(xbox::PETHREAD Ethread, unsigned XboxThreadStackBaseReserved = 0, unsigned XboxThreadStackSizeReserved = 0);
// free kpcr allocated for the thread
void EmuKeFreePcr();

void EmuKeSetPcr(xbox::KPCR *Pcr);
xbox::KPCR *_stdcall EmuKeGetPcr();

typedef struct
{
	std::vector<uint8_t> data;
	void* functionPtr;
}fs_instruction_t;

#endif
