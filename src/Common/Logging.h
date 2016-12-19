// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Logging.h
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef _LOGGING_H
#define _LOGGING_H

#pragma once

#include "Cxbx.h"

// From http://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
constexpr const char* str_end(const char *str) {
	return *str ? str_end(str + 1) : str;
}

constexpr bool str_slant(const char *str) {
	return *str == '\\' ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char* r_slant(const char* str) {
	return *str == '\\' ? (str + 1) : r_slant(str - 1);
}
constexpr const char* file_name(const char* str) {
	return str_slant(str) ? r_slant(str_end(str)) : str;
}

#define __FILENAME__ file_name(__FILE__)

#include <windows.h> // for DWORD
#include <sstream> // for std::stringstream
#include <iostream> // For std::cout
#include <iomanip> // For std::setw

//
// Hex output (type safe)
//
// http://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
//

struct Hex1Struct
{
	uint8_t v;
	Hex1Struct(uint8_t _v) : v(_v) { }
};

inline Hex1Struct hex1(uint8_t _v)
{
	return Hex1Struct(_v);
}

inline std::ostream& operator<<(std::ostream& os, const Hex1Struct& hs)
{
	return os << "0x" << std::hex << std::uppercase << (int)hs.v;
}

struct Hex2Struct
{
	uint16_t v;
	Hex2Struct(uint16_t _v) : v(_v) { }
};

inline Hex2Struct hex2(uint16_t _v)
{
	return Hex2Struct(_v);
}

inline std::ostream& operator<<(std::ostream& os, const Hex2Struct& hs)
{
	return os << "0x" << std::hex << std::uppercase << (int)hs.v;
}

struct Hex4Struct
{
	uint32_t v;
	Hex4Struct(uint32_t _v) : v(_v) { }
};

inline Hex4Struct hex4(uint32_t _v)
{
	return Hex4Struct(_v);
}

inline std::ostream& operator<<(std::ostream& os, const Hex4Struct& hs)
{
	return os << "0x" << std::hex << std::uppercase << (int)hs.v;
}

//
// Data sanitization functions
//

// Default sanitization functions simply returns the given argument
template<class T>
inline T _log_sanitize(T arg) { return arg; }

// Sanitize C-style strings by converting NULL to "<nullptr>" to prevent null dereference
inline const char * _log_sanitize(const char *arg) { return (NULL == arg) ? "<nullptr>" : arg; }
inline const wchar_t * _log_sanitize(const wchar_t *arg) { return (NULL == arg) ? L"<nullptr>" : arg; }

// Convert booleans to strings properly
inline const char * _log_sanitize(BOOL value) { return value ? "TRUE" : "FALSE"; }
inline const char * _log_sanitize(BOOLEAN value) { return value ? "TRUE" : "FALSE"; }

//
// Logging defines
//

// For thread_local, see : http://en.cppreference.com/w/cpp/language/storage_duration
extern thread_local const DWORD _CurrentThreadId;

// TODO : Use Boost.Format http://www.boost.org/doc/libs/1_53_0/libs/format/index.html
extern thread_local std::string _logPrefix;


#ifdef _DEBUG_TRACE
	#define LOG_FUNC_BEGIN \
		do { if(g_bPrintfOn) { \
			bool _had_arg = false; \
			if (_logPrefix.empty()) { \
				std::stringstream tmp; \
				tmp << __FILENAME__ << " (" << hex2((uint16_t)_CurrentThreadId) << "): "; \
				_logPrefix = tmp.str(); \
			}; \
			std::stringstream msg; \
			msg << _logPrefix << __func__ << "(";

	// LOG_FUNC_ARG_OUT writes output via all available ostream << operator overloads, adding detail where possible
	#define LOG_FUNC_ARG(arg) \
			_had_arg = true; \
			msg << "\n   " << std::setw(25) << std::left << std::setfill(' ') << #arg << " : " << _log_sanitize(arg);

	// LOG_FUNC_ARG_OUT prevents expansion of types, by only rendering as a pointer
	#define LOG_FUNC_ARG_OUT(arg) \
			_had_arg = true; \
			msg << "\n OUT " << std::setw(23) << std::left << std::setfill(' ') << #arg << " : " << hex4((uint32_t)arg);

	// LOG_FUNC_END closes off function and optional argument logging
	#define LOG_FUNC_END \
			if (_had_arg) msg << '\n'; \
			msg << ");\n"; \
			std::cout << msg.str(); \
		} } while (0)

	// LOG_FUNC_RESULT logs the function return result
	#define LOG_FUNC_RESULT(r) \
		std::cout << _logPrefix << __func__ << " returns " << r << "\n";
#else
	#define LOG_FUNC_BEGIN 
	#define LOG_FUNC_ARG(arg)
	#define LOG_FUNC_ARG_OUT(arg)
	#define LOG_FUNC_END
	#define LOG_FUNC_RESULT(r)
#endif

// Short hand defines :
// Log function without arguments
#define LOG_FUNC() LOG_FUNC_BEGIN LOG_FUNC_END

// Log function with one argument
#define LOG_FUNC_ONE_ARG(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG(arg) LOG_FUNC_END 

// Log function with one out argument
#define LOG_FUNC_ONE_ARG_OUT(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG_OUT(arg) LOG_FUNC_END 

// LOG_FORWARD indicates that an api is implemented by a forward to another API
#define LOG_FORWARD(api) \
	do { if(g_bPrintfOn) { \
		std::cout << _logPrefix << __func__ << " forwarding to "#api"...\n"; \
	} } while (0)

// LOG_IGNORED indicates that Cxbx consiously ignores an api
#define LOG_IGNORED() \
	do { if(g_bPrintfOn) { \
		std::cout << _logPrefix << __func__ << " ignored!\n"; \
	} } while (0)

// LOG_UNIMPLEMENTED indicates that Cxbx is missing an implementation of an api
#define LOG_UNIMPLEMENTED() \
	do { if(g_bPrintfOn) { \
		std::cout << _logPrefix << __func__ << " unimplemented!\n"; \
	} } while (0)

// RETURN logs the given result and then returns it (so this should appear last in functions)
#define RETURN(r) do { LOG_FUNC_RESULT(r) return r; } while (0)

#endif _LOGGING_H