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
// *   Cxbx->Common->CxbxCommon.h
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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef CXBX_COMMON_H
#define CXBX_COMMON_H

#include "Cxbx.h"
#include <stdint.h>
#include <assert.h>

/* This is a linux struct for vectored I/O. See readv() and writev() */
struct IoVec
{
	void* Iov_Base;  // Starting address
	size_t Iov_Len;  // Number of bytes to transfer
};

struct IOVector
{
	IoVec* IoVecStruct;
	int IoVecNumber;      // number of I/O buffers supplied
	int AllocNumber;      // number of IoVec structs currently allocated
	size_t Size;          // total size of all I/O buffers supplied
};

uint64_t Muldiv64(uint64_t a, uint32_t b, uint32_t c);

void IoVecReset(IOVector* qiov);
void IoVecAdd(IOVector* qiov, void* base, size_t len);
size_t IoVecTobuffer(const IoVec* iov, const unsigned int iov_cnt, size_t offset, void *buf, size_t bytes);
size_t IoVecFromBuffer(const IoVec* iov, unsigned int iov_cnt, size_t offset, void* buf, size_t bytes);

void WriteDwords(xbaddr Paddr, uint32_t* Buffer, int Number);
void GetDwords(xbaddr Paddr, uint32_t* Buffer, int Number);
void GetWords(xbaddr Paddr, uint16_t* Buffer, int Number);
void WriteWords(xbaddr Paddr, uint16_t* Buffer, int Number);

#define GET_WORD_LOW(value) (uint8_t)((value) & 0xFF)
#define GET_WORD_HIGH(value) (uint8_t)(((value) >> 8) & 0xFF)

#endif
