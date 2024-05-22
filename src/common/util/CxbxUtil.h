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
// *  (c) 2018      ergo720
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef CXBXUTIL_H
#define CXBXUTIL_H

#include <algorithm>
#include <stdexcept>
#include "xbox_types.h"
#include "Cxbx.h"
#include <stdint.h>
#include <assert.h>
#include <string>
#include <type_traits>
#include <vector>
#include "std_extend.hpp" // for ARRAY_SIZE

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

bool WriteDwords(xbox::addr_xt Paddr, uint32_t* Buffer, int Number);
bool GetDwords(xbox::addr_xt Paddr, uint32_t* Buffer, int Number);
bool GetWords(xbox::addr_xt Paddr, uint16_t* Buffer, int Number);
bool WriteWords(xbox::addr_xt Paddr, uint16_t* Buffer, int Number);
bool Memory_R(void* Addr, void* Buf, size_t Num);
bool Memory_W(void* Addr, void* Buf, size_t Num);
bool Memory_RW(void* Addr, void* Buf, size_t Num, bool bIsWrite);

void unix2dos(std::string& string);
std::string StripSpaces(const std::string& str);
std::string StripQuotes(const std::string& str);

// Retrieves the underlying integer value of a scoped enumerator. It allows to avoid using static_cast every time
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

#define GET_WORD_LOW(value) (uint8_t)((value) & 0xFF)
#define GET_WORD_HIGH(value) (uint8_t)(((value) >> 8) & 0xFF)

/*! round dwValue to the nearest multiple of dwMult */
static uint32_t RoundUp(uint32_t dwValue, uint32_t dwMult)
{
    if(dwMult == 0) { return dwValue; }

	int remainder = dwValue % dwMult;
	if (remainder == 0)
		return dwValue;

	return dwValue + dwMult - remainder;
}

constexpr std::size_t longest_str(const std::vector<std::string_view> &vec)
{
	if (!vec.empty()) {
		return std::max_element(vec.begin(), vec.end(),
			[](const auto &a, const auto &b) {
				return a.length() < b.length();
			})->length();
	}
	else {
		throw std::logic_error("No strings to compare!");
	}
}

#endif
