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
// *   Cxbx->Common->CxbxCommon.cpp
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

// The intent of this file is to add general functions which are not kernel specific (for those CxbxKrnl.h should be used instead)

#include "CxbxCommon.h"
#include "CxbxKrnl\CxbxKrnl.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


// Acknowledgment: some the functions present are from XQEMU (GPLv2)
// https://xqemu.com/


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

// ergo720: note that GetDwords and WriteDwords will reliably work only if the allocation was served by MapViewOfFileEx and not
// by VirtualAlloc (see comment on OHCI_ReadHCCA for more details). Once LLE CPU and MMU are implemented, this will no
// longer be the case. Also note that the physical pages can be modified while being read/written

// Read an array of DWORDs in memory
void GetDwords(xbaddr Paddr, uint32_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		std::memcpy(Buffer, reinterpret_cast<void*>(Paddr + CONTIGUOUS_MEMORY_BASE), 4); // dropped little -> big endian conversion from XQEMU
	}
}

// Write an array of DWORDs in memory
void WriteDwords(xbaddr Paddr, uint32_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		std::memcpy(reinterpret_cast<void*>(Paddr + CONTIGUOUS_MEMORY_BASE), Buffer, 4); // dropped big -> little endian conversion from XQEMU
	}
}

// Read an array of WORDs in memory
void GetWords(xbaddr Paddr, uint16_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		std::memcpy(Buffer, reinterpret_cast<void*>(Paddr + CONTIGUOUS_MEMORY_BASE), 2); // dropped little -> big endian conversion from XQEMU
	}
}

// Write an array of WORDs in memory
void WriteWords(xbaddr Paddr, uint16_t* Buffer, int Number)
{
	for (int i = 0; i < Number; i++, Buffer++, Paddr += sizeof(*Buffer)) {
		std::memcpy(reinterpret_cast<void*>(Paddr + CONTIGUOUS_MEMORY_BASE), Buffer, 2); // dropped big -> little endian conversion from XQEMU
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
