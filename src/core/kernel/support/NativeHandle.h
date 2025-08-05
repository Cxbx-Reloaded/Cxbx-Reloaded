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
// *  (c) 2021 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#pragma once

#include <optional>

// Use GetObjectNativeHandle if certain object type is fully implemented, otherwise use GetNativeHandle instead.
template<bool NoConversion = false> std::optional<HANDLE> GetNativeHandle(xbox::HANDLE xhandle);

template<bool PartitionConversion = false> void RegisterXboxObject(xbox::PVOID xobject, HANDLE nhandle);
void RemoveXboxObject(xbox::PVOID xobject);
// Use GetObjectNativeHandle if certain object type is fully implemented, otherwise use GetNativeHandle instead.
template<bool PartitionConversion = true> std::optional<HANDLE> GetObjectNativeHandle(xbox::PVOID xobject);
