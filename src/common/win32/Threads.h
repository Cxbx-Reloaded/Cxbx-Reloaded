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

#pragma once

#include <memory>

void SetCurrentThreadName(const char* szThreadName);

// A helper class to pin game/other threads to specific CPU cores
// Implemented different depending on the host OS, so exposes itself as an interface
// If "All Cores Hack" is enabled (or the host system is single core), an empty implementation is used
class AffinityPolicy
{
public:
	~AffinityPolicy() = default;

	virtual void SetAffinityXbox(void* thread) const = 0;
	virtual void SetAffinityOther(void* thread) const = 0;

	void SetAffinityXbox() const;
	void SetAffinityOther() const;

	static std::unique_ptr<AffinityPolicy> InitPolicy();
};

extern std::unique_ptr<AffinityPolicy> g_AffinityPolicy;
