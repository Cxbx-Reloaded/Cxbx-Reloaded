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
// *  (c) 2020 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#include "Types.hpp"
#include "common/AddressRanges.h"
#include "common/util/cliConfig.hpp"

const char* GetSystemTypeToStr(unsigned int system)
{
	if (system == SYSTEM_CHIHIRO) {
		return cli_config::system_chihiro;
	}

	if (system == SYSTEM_DEVKIT) {
		return cli_config::system_devkit;
	}

	if (system == SYSTEM_XBOX) {
		return cli_config::system_retail;
	}

	return nullptr;
}

const char* GetXbeTypeToStr(XbeType xbe_type)
{
	if (xbe_type == XbeType::xtChihiro) {
		return "chihiro";
	}

	if (xbe_type == XbeType::xtDebug) {
		return "debug";
	}

	return "retail";
}
