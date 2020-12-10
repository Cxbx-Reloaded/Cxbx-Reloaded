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

#include <windows.h>

typedef const char* (CDECL* LPFN_WINEGETVERSION)(void);
LPFN_WINEGETVERSION wine_get_version;

static bool CheckForWine() {
	HMODULE hNtDll = GetModuleHandle("ntdll.dll");
	if (hNtDll != nullptr) {
		wine_get_version = (LPFN_WINEGETVERSION)GetProcAddress(hNtDll, "wine_get_version");
		// If wine is found, store pointer to the function for later call.
		if (wine_get_version) {
			return true;
		}
	}
	return false;
}

bool isWineEnv()
{
	static bool bIsWine = CheckForWine();
	return bIsWine;
}

const char* getWineVersion()
{
	return wine_get_version();
}
