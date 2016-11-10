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

// For thread_local, see : http://en.cppreference.com/w/cpp/language/storage_duration
extern thread_local const DWORD _CurrentThreadId;

// TODO : Use Boost.Format http://www.boost.org/doc/libs/1_53_0/libs/format/index.html
extern thread_local std::string _logPrefix;

#define LOG_FUNC_BEGIN \
	do { if (_DEBUG_TRACE) if(g_bPrintfOn) { \
		if (_logPrefix.empty()) { \
			std::stringstream tmp; \
			tmp << __FILENAME__ << " (0x" << std::hex << std::uppercase << _CurrentThreadId << "): "; \
			_logPrefix = tmp.str(); \
		}; \
		bool had_args = false; \
		std::stringstream msg; \
		msg << _logPrefix << __func__ << "(";

// LOG_FUNC_ARG_OUT writes output via all available ostream << operator overloads, adding detail where possible
#define LOG_FUNC_ARG(arg) \
		had_args = true; \
		msg << "\n   " << std::setw(18) << std::left << std::setfill(' ') << #arg << " : " << arg;

// LOG_FUNC_ARG_OUT prevents expansion of types, by only rendering as a pointer
#define LOG_FUNC_ARG_OUT(arg) \
		msg << "\n   " << std::setw(18) << std::left << std::setfill(' ') << #arg << " : 0x" << (void*)arg;

#define LOG_FUNC_END \
		if (had_args) msg << "\n"; \
		msg << ");\n"; \
		std::cout << msg.str(); \
	} } while (0)

// Short hand defines :

// Log function without arguments
#define LOG_FUNC() LOG_FUNC_BEGIN LOG_FUNC_END

// Log function with one argument
#define LOG_FUNC_ONE_ARG(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG(arg) LOG_FUNC_END 

// Log function with one out argument
#define LOG_FUNC_ONE_ARG_OUT(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG_OUT(arg) LOG_FUNC_END 


// TODO : 
// LPCSTR
// PLARGE_INTEGER
// PXDEVICE_PREALLOC_TYPE
// PXPP_DEVICE_TYPE
// PXINPUT_CAPABILITIES
// PXINPUT_STATE
// PXTHREAD_NOTIFICATION -> pfnNotifyRoutine
#endif _LOGGING_H