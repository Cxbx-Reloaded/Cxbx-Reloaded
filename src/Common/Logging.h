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

#include <windows.h> // For DWORD
#include <sstream> // For std::stringstream
#include <iostream> // For std::cout
#include <iomanip> // For std::setw
#include "Cxbx.h" // For g_bPrintfOn

//
// __FILENAME__
//

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

//
// Character escaping functions
//

extern const bool needs_escape(const wint_t _char);
extern inline void output_char(std::ostream& os, char c);
extern inline void output_wchar(std::ostream& os, wchar_t c);

//
// Data sanitization functions
//

// By default, sanitization functions simply return the given argument
// (type and value) which results in calls to standard output writers.
template<class T>
inline T _log_sanitize(T value)
{
	return value;
}

// Convert booleans to strings properly
inline const char * _log_sanitize(BOOL value)
{
	return value ? "TRUE" : "FALSE";
}

// Macro to ease declaring a _log_sanitize overload (invokeable via C) for type T
#define LOG_SANITIZE_OVERLOAD(C, T)     \
struct Sane##C                          \
{                                       \
  T value;                              \
  Sane##C(T _value) : value(_value) { } \
};                                      \
                                        \
inline Sane##C C(T value)               \
{                                       \
    return Sane##C(value);              \
}                                       \
                                        \
inline Sane##C _log_sanitize(T value)   \
{                                       \
    return C(value);                    \
}                                       \
                                        \
inline std::ostream& operator<<(        \
    std::ostream& os,                   \
	const Sane##C& container)

// Hex output (type safe)
// http://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
LOG_SANITIZE_OVERLOAD(hex1, uint8_t)
{
	return os << "0x" << std::hex << std::uppercase << (int)container.value;
}

LOG_SANITIZE_OVERLOAD(hex2, uint16_t)
{
	return os << "0x" << std::hex << std::uppercase << (int)container.value;
}

LOG_SANITIZE_OVERLOAD(hex4, uint32_t)
{
	return os << "0x" << std::hex << std::uppercase << (int)container.value;
}

// Character output (escaped into a C-string representation)
// https://en.wikipedia.org/wiki/Escape_sequences_in_C
LOG_SANITIZE_OVERLOAD(sanitized_char, char)
{
	output_char(os, container.value);
	return os;
}

LOG_SANITIZE_OVERLOAD(sanitized_wchar, wchar_t)
{
	output_wchar(os, container.value);
	return os;
}

LOG_SANITIZE_OVERLOAD(sanitized_char_pointer, char *)
{
	char *v = container.value;

	os << "(char *)";
	if (v == nullptr)
		return os << "NULL";

	bool needsEscaping = false;

	while (*v)
		if (needs_escape(*v++))
		{
			needsEscaping = true;
			break;
		}

	v = container.value;
	os << "0x" << std::hex << std::uppercase << (void *)v << " = \"";
	if (needsEscaping)
	{
		while (*v)
			output_char(os, *v++);
	}
	else
		os << v;

	return os << "\"";
}

LOG_SANITIZE_OVERLOAD(sanitized_wchar_pointer, wchar_t *)
{
	wchar_t *v = container.value;

	os << "(wchar *)";
	if (v == nullptr)
		return os << "NULL";

	bool needsEscaping = false;

	while (*v)
		if (needs_escape(*v++))
		{
			needsEscaping = true;
			break;
		}

	v = container.value;
	os << "0x" << std::hex << std::uppercase << (void *)v << " = \"";
	if (needsEscaping)
	{
		while (*v)
			output_wchar(os, *v++);
	}
	else
		os << v;

	return os << "\"";
}

//
// Logging defines
//

// For thread_local, see : http://en.cppreference.com/w/cpp/language/storage_duration
// TODO : Use Boost.Format http://www.boost.org/doc/libs/1_53_0/libs/format/index.html
extern thread_local std::string _logPrefix;

#define LOG_ARG_START "\n   " << std::setw(20)
#define LOG_ARG_OUT_START "\n OUT " << std::setw(18)

#ifdef _DEBUG_TRACE
	#define LOG_FUNC_BEGIN \
		do { if(g_bPrintfOn) { \
			bool _had_arg = false; \
			std::stringstream tmp; \
			tmp << "[" << hex2((uint16_t)GetCurrentThreadId()) << "] " << __FILENAME__ << ": "; \
			_logPrefix = tmp.str(); \
			std::stringstream msg; \
			msg << _logPrefix << __func__ << std::left << "(";

	// LOG_FUNC_ARG writes output via all available ostream << operator overloads, sanitizing and adding detail where possible
	#define LOG_FUNC_ARG(arg) \
			_had_arg = true; \
			msg << LOG_ARG_START << #arg << " : " << _log_sanitize(arg);

	// LOG_FUNC_ARG_TYPE writes output using the overloaded << operator of the given type
	#define LOG_FUNC_ARG_TYPE(type, arg) \
			_had_arg = true; \
			msg << LOG_ARG_START << #arg << " : " << (type)arg;

	// LOG_FUNC_ARG_OUT prevents expansion of types, by only rendering as a pointer
	#define LOG_FUNC_ARG_OUT(arg) \
			_had_arg = true; \
			msg << LOG_ARG_OUT_START << #arg << " : " << hex4((uint32_t)arg);

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
	#define LOG_FUNC_ARG_TYPE(type, arg)
	#define LOG_FUNC_ARG_OUT(arg)
	#define LOG_FUNC_END
	#define LOG_FUNC_RESULT(r)
#endif

//
// Short hand defines :
//

// Log function without arguments
#define LOG_FUNC() LOG_FUNC_BEGIN LOG_FUNC_END

// Log function with one argument
#define LOG_FUNC_ONE_ARG(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG(arg) LOG_FUNC_END 

// Log function with one typed argument
#define LOG_FUNC_ONE_ARG_TYPE(type, arg) LOG_FUNC_BEGIN LOG_FUNC_ARG_TYPE(type, arg) LOG_FUNC_END 

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

// LOG_INCOMPLETE indicates that Cxbx is missing part of an implementation of an api
#define LOG_INCOMPLETE() \
	do { if(g_bPrintfOn) { \
		std::cout << _logPrefix << __func__ << " incomplete!\n"; \
	} } while (0)

// RETURN logs the given result and then returns it (so this should appear last in functions)
#define RETURN(r) do { LOG_FUNC_RESULT(r) return r; } while (0)

#endif _LOGGING_H