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
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2019      RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#pragma once

#include <string>
#include <SDL_stdinc.h>

// Depending on what type of UTF-8 string is input, otherwise will use native by default.
static std::wstring utf8_to_utf16(const char* utf8_string)
{
	wchar_t* tempUTF16 = reinterpret_cast<wchar_t*>(SDL_iconv_string(/*to:*/"UTF-16", /*from:*/"UTF-8", utf8_string, strlen(utf8_string) + 1));
	std::wstring strUTF16 = tempUTF16;
	// Don't forget to free allocated string from SDL library.
	SDL_free(tempUTF16);

	return strUTF16;
};

// Depending on what type of UTF-16 string is input, otherwise will use native by default.
static std::string utf16_to_ascii(const wchar_t* utf16_string)
{
	char* tempASCII = SDL_iconv_string(/*to:*/"ASCII", /*from:*/"UTF-16", reinterpret_cast<const char*>(utf16_string), wcslen(utf16_string)*2 + 2);
	const std::string strASCII = tempASCII;
	// Don't forget to free allocated string from SDL library.
	SDL_free(tempASCII);

	return strASCII;
}

// Enforce to expect little endian UTF-16 string is input.
static std::string utf16le_to_ascii(const wchar_t* utf16le_string)
{
	char* tempASCII = SDL_iconv_string(/*to:*/"ASCII", /*from:*/"UTF-16LE", reinterpret_cast<const char*>(utf16le_string), wcslen(utf16le_string)*2 + 2);
	const std::string strASCII = tempASCII;
	// Don't forget to free allocated string from SDL library.
	SDL_free(tempASCII);

	return strASCII;
}
