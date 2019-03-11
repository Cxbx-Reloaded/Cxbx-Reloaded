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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

// Acknowledgment: some of the functions present are from XQEMU (GPLv2)
// https://xqemu.com/

// swap_endianess is extracted from mbedtls_mpi_read_binary used in the file bignum.h of ReactOS

/**
* \file bignum.h
*
* \brief  Multi-precision integer library
*
*  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
*  SPDX-License-Identifier: GPL-2.0
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License along
*  with this program; if not, write to the Free Software Foundation, Inc.,
*  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
*  This file is part of mbed TLS (https://tls.mbed.org)
*/

// The intent of this file is to add general functions which are not kernel specific (for those CxbxKrnl.h should be used instead)

#include "common\util\CxbxUtil.h"
#include "core\kernel\init\CxbxKrnl.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


// Disable a compiler warning relative to uint64_t -> uint32_t conversions in Muldiv64. This function is taken from
// XQEMU so it should be safe regardless
#pragma warning(push)
#pragma warning(disable: 4244)

// Compute (a*b)/c with a 96 bit intermediate result
uint64_t Muldiv64(uint64_t a, uint32_t b, uint32_t c)
{
	union {
		uint64_t ll;
		struct {
			uint32_t low, high;
		} l;
	} u, res;
	uint64_t rl, rh;

	u.ll = a;
	rl = (uint64_t)u.l.low * (uint64_t)b;
	rh = (uint64_t)u.l.high * (uint64_t)b;
	rh += (rl >> 32);
	res.l.high = rh / c;
	res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
	return res.ll;
}

#pragma warning(pop)

void IoVecReset(IOVector* qiov)
{
	assert(qiov->AllocNumber != -1);

	qiov->IoVecNumber = 0;
	qiov->Size = 0;
}

void IoVecAdd(IOVector* qiov, void* base, size_t len)
{
	assert(qiov->AllocNumber != -1);

	if (qiov->IoVecNumber == qiov->AllocNumber) {
		qiov->AllocNumber = 2 * qiov->AllocNumber + 1;
		qiov->IoVecStruct = static_cast<IoVec*>(std::realloc(qiov->IoVecStruct, qiov->AllocNumber * sizeof(IOVector)));
	}
	qiov->IoVecStruct[qiov->IoVecNumber].Iov_Base = base;
	qiov->IoVecStruct[qiov->IoVecNumber].Iov_Len = len;
	qiov->Size += len;
	++qiov->IoVecNumber;
}

// This takes "iov_cnt" of "iov" buffers as input and copies sequentially their contents to the "buf" output buffer.
// "offset" indicates the offset inside "bytes" (total lenght of "iov" buffers) where the copy is going to start.
// "offset" must be less than "bytes" or else the assertion will fail. "done" is the number of bytes actually copied
size_t IoVecTobuffer(const IoVec* iov, const unsigned int iov_cnt, size_t offset, void* buf, size_t bytes)
{
	size_t done;
	unsigned int i;
	for (i = 0, done = 0; (offset || done < bytes) && i < iov_cnt; i++) {
		if (offset < iov[i].Iov_Len) {
			size_t len = MIN(iov[i].Iov_Len - offset, bytes - done);
			std::memcpy(static_cast<uint8_t*>(buf) + done, static_cast<uint8_t*>(iov[i].Iov_Base) + offset, len);
			done += len;
			offset = 0;
		}
		else {
			offset -= iov[i].Iov_Len;
		}
	}
	assert(offset == 0);
	return done;
}

// This does the opposite of IoVecTobuffer: it takes "buf" as input and copies sequentially its contents to the
// "iov" output buffers.
size_t IoVecFromBuffer(const IoVec* iov, unsigned int iov_cnt, size_t offset, void* buf, size_t bytes)
{
	size_t done;
	unsigned int i;
	for (i = 0, done = 0; (offset || done < bytes) && i < iov_cnt; i++) {
		if (offset < iov[i].Iov_Len) {
			size_t len = MIN(iov[i].Iov_Len - offset, bytes - done);
			std::memcpy(static_cast<uint8_t*>(iov[i].Iov_Base) + offset, static_cast<uint8_t*>(buf) + done, len);
			done += len;
			offset = 0;
		}
		else {
			offset -= iov[i].Iov_Len;
		}
	}
	assert(offset == 0);
	return done;
}

// Read an array of DWORDs in memory
bool GetDwords(xbaddr Paddr, uint32_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		// dropped little -> big endian conversion from XQEMU
		if (Memory_R(reinterpret_cast<void*>(Paddr), Buffer, 4)) {
			return true;
		}
	}
	return false;
}

// Write an array of DWORDs in memory
bool WriteDwords(xbaddr Paddr, uint32_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		// dropped big -> little endian conversion from XQEMU
		if (Memory_W(reinterpret_cast<void*>(Paddr), Buffer, 4)) {
			return true;
		}
	}
	return false;
}

// Read an array of WORDs in memory
bool GetWords(xbaddr Paddr, uint16_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		// dropped little -> big endian conversion from XQEMU
		if (Memory_R(reinterpret_cast<void*>(Paddr), Buffer, 2)) {
			return true;
		}
	}
	return false;
}

// Write an array of WORDs in memory
bool WriteWords(xbaddr Paddr, uint16_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		// dropped big -> little endian conversion from XQEMU
		if (Memory_W(reinterpret_cast<void*>(Paddr), Buffer, 2)) {
			return true;
		}
	}
	return false;
}

bool Memory_R(void* Addr, void* Buf, size_t Num)
{
	bool Error = false;

	if (Num != 0) {
		if (Addr == nullptr) {
			Error = true;
		}
		else {
			std::memcpy(Buf, Addr, Num);
		}
	}
	return Error;
}

bool Memory_W(void* Addr, void* Buf, size_t Num)
{
	bool Error = false;

	if (Num != 0) {
		if (Addr == nullptr) {
			Error = true;
		}
		else {
			std::memcpy(Addr, Buf, Num);
		}
	}
	return Error;
}

bool Memory_RW(void* Addr, void* Buf, size_t Num, bool bIsWrite)
{
	if (bIsWrite) {
		return Memory_W(Addr, Buf, Num);
	}
	else {
		return Memory_R(Addr, Buf, Num);
	}
}

// Converts LF to CRLF line endings
void unix2dos(std::string& string)
{
	size_t position = 0;
	while (true) {
		position = string.find('\n', position);
		if (position == std::string::npos) {
			break;
		}
		if (position != 0 && string.compare(position - 1, 2U, "\r\n") == 0) {
			position++;
			continue;
		}
		string.insert(position, 1, '\r');
		position += 2;
	}
}

void swap_endianess(const unsigned char* in_buf, unsigned char* out_buf, size_t size)
{
	size_t i, j, n;
	uint32_t* out_buf_uint = (uint32_t*)out_buf;

	memset(out_buf_uint, 0, size);

	for (n = 0; n < size; n++)
		if (in_buf[n] != 0)
			break;

	for (i = size, j = 0; i > n; i--, j++) {
		out_buf_uint[j / 4] |= ((uint32_t)in_buf[i - 1]) << ((j % 4) << 3);
	}
}
