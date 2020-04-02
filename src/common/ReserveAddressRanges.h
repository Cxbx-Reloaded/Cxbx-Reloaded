// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of Cxbx-Reloaded.
// *
// *  Cxbx-Reloaded is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// *  (c) 2017-2019 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <cstdint> // For uint32_t

// First block section will be devkit region (256MiB) = 262144KiB / 64 KiB = 4096 bits needed -> 4096 / 32 (sizeof(uint32_t)) = 128 entries blocks_reserved array.
// Next block section will be system region (512MiB) = 524288KiB / 64KiB = 8192 bits needed -> 8192 / 32 (sizeof(uint32_t)) = 256 entries of blocks_reserved array.
// 1 bit per block_size (64KiB).
typedef uint32_t blocks_reserved_t[384]; // 384 = 128 (devkit region) + 256 (system region)

inline constexpr uint32_t BLOCK_REGION_DEVKIT_INDEX_BEGIN = 0;
inline constexpr uint32_t BLOCK_REGION_DEVKIT_INDEX_END   = 4096;
inline constexpr uint32_t BLOCK_REGION_SYSTEM_INDEX_BEGIN = 4096;
inline constexpr uint32_t BLOCK_REGION_SYSTEM_INDEX_END   = 12288;

extern bool ReserveAddressRanges(const unsigned int system, blocks_reserved_t blocks_reserved);

extern void FreeAddressRanges(const unsigned int system, unsigned int release_systems, blocks_reserved_t blocks_reserved);

extern bool AttemptReserveAddressRanges(unsigned int* p_reserved_systems, blocks_reserved_t blocks_reserved);

extern bool isSystemFlagSupport(unsigned int reserved_systems, unsigned int assign_system);
