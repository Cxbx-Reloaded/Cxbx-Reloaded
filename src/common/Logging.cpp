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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#include <windows.h> // for PULONG

#include "Logging.h"
#include "common\Settings.hpp"
#include "EmuShared.h"

// For thread_local, see : https://en.cppreference.com/w/cpp/language/storage_duration
// TODO : Use Boost.Format https://www.boost.org/doc/libs/1_53_0/libs/format/index.html
thread_local std::string _logThreadPrefix;

std::atomic_bool g_EnabledModules[to_underlying(CXBXR_MODULE::MAX)] = { false };
const char* g_EnumModules2String[to_underlying(CXBXR_MODULE::MAX)] = {
	"CXBXR   ",
	"XBE     ",
	"INIT    ",
	"VMEM    ",
	"PMEM    ",
	"GUI     ",
	"EEPR    ",
	"RSA     ",
	"POOLMEM ",
	"D3D8    ",
	"D3DST   ",
	"D3DCVT  ",
	"DSOUND  ",
	"XAPI    ",
	"XACT    ",
	"XGRP    ",
	"XONLINE ",
	"FS      ",
	"PSHB    ",
	"PXSH    ",
	"VTXSH   ",
	"VTXB    ",
	"DINP    ",
	"XINP    ",
	"SDL     ",
	"FILE    ",
	"X86     ",
	"HLE     ",
	"NET     ",
	"MCPX    ",
	"NV2A    ",
	"SMC     ",
	"OHCI    ",
	"USB     ",
	"HUB     ",
	"XIDCTRL ",
	"ADM     ",
	"INPSYS  ",
	"KRNL    ",
	"LOG     ",
	"XBOX    ",
	"XBDM    ",
	"AV      ",
	"DBG     ",
	"EX      ",
	"FSC     ",
	"HAL     ",
	"IO      ",
	"KD      ",
	"KE      ",
	"KI      ",
	"MM      ",
	"NT      ",
	"OB      ",
	"PS      ",
	"RTL     ",
	"XC      ",
	"XE      ",
};
std::atomic_int g_CurrentLogLevel = to_underlying(LOG_LEVEL::INFO);

const char log_debug[] = "DEBUG: ";
const char log_info[]  = "INFO : ";
const char log_warn[]  = "WARN : ";
const char log_error[] = "ERROR: ";
const char log_fatal[] = "FATAL: ";
const char log_unkwn[] = "???? : ";

// print out a warning message to the kernel debug log file
void NTAPI EmuLogEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, const char *szWarningMessage, ...)
{
	if (szWarningMessage == NULL) {
		return;
	}

	LOG_CHECK_ENABLED_EX(cxbxr_module, level) {
		if (g_bPrintfOn) {

			va_list argp;
			const char* level_str;

			LOG_THREAD_INIT;

			switch (level) {
				default:
					level_str = log_unkwn;
					break;
				case LOG_LEVEL::DEBUG:
					level_str = log_debug;
					break;
				case LOG_LEVEL::INFO:
					level_str = log_info;
					break;
				case LOG_LEVEL::WARNING:
					level_str = log_warn;
					break;
				case LOG_LEVEL::ERROR2:
					level_str = log_error;
					break;
				case LOG_LEVEL::FATAL:
					level_str = log_fatal;
					break;
			}

			std::cout << _logThreadPrefix << level_str
				<< g_EnumModules2String[to_underlying(cxbxr_module)];

			va_start(argp, szWarningMessage);

			vfprintf(stdout, szWarningMessage, argp);

			va_end(argp);

			fprintf(stdout, "\n");

			fflush(stdout);

		}
	}
}

// Set up the logging variables for the GUI process
inline void log_get_settings()
{
	log_set_config(g_Settings->m_core.LogLevel, g_Settings->m_core.LoggedModules);
}

inline void log_sync_config()
{
	int LogLevel;
	unsigned int LoggedModules[NUM_INTEGERS_LOG];
	g_EmuShared->GetLogLv(&LogLevel);
	g_EmuShared->GetLogModules(LoggedModules);
	log_set_config(LogLevel, LoggedModules);
}

void log_set_config(int LogLevel, unsigned int* LoggedModules)
{
	g_CurrentLogLevel = LogLevel;
	for (unsigned int index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::MAX); index++) {
		if (LoggedModules[index / 32] & (1 << (index % 32))) {
			g_EnabledModules[index] = true;
		}
		else {
			g_EnabledModules[index] = false;
		}
	}
}

// Generate active log filter output.
void log_generate_active_filter_output(const CXBXR_MODULE cxbxr_module)
{
	LOG_THREAD_INIT;
	std::string generic_output_str = _logThreadPrefix + g_EnumModules2String[to_underlying(cxbxr_module)];

	std::cout << generic_output_str << "Current log level: " << g_CurrentLogLevel << std::endl;

	generic_output_str.append("Active log filter: ");
	for (unsigned int index = to_underlying(CXBXR_MODULE::CXBXR); index < to_underlying(CXBXR_MODULE::MAX); index++) {
		if (g_EnabledModules[index]) {
			std::cout << generic_output_str << g_EnumModules2String[index] << "\n";
		}
	}
	std::cout << std::flush;
}

const bool needs_escape(const wint_t _char)
{
	// Escaping is needed for control characters,
	// for double quote, and for backslash :
	return iswcntrl(_char) || (_char == '"') || (_char == '\\');
}

inline void output_char(std::ostream& os, char c)
{
	if (needs_escape((int)c))
	{
		switch (c)
		{
			// Render escaped double quote as \", and escaped backslash as \\ :
		case '"': os << "\\\""; break;
		case '\\': os << "\\\\"; break;
			// See https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences
		case '\a': os << "\\t"; break;
		case '\b': os << "\\b"; break;
		case '\f': os << "\\f"; break;
		case '\n': os << "\\n"; break;
		case '\r': os << "\\r"; break;
		case '\t': os << "\\t"; break;
		case '\v': os << "\\v"; break;
			// All other to-escape-characters are rendered as hexadecimal :
		default: os << "\\x" << std::setfill('0') << std::setw(2) << std::right << std::hex << std::uppercase << (int)c;
		}
	}
	else
		os << c;
}

inline void output_wchar(std::ostream& os, wchar_t c)
{
	if (needs_escape((wint_t)c))
	{
		switch (c)
		{
			// Render escaped double quote as \", and escaped backslash as \\ :
		case '"': os << "\\\""; break;
		case '\\': os << "\\\\"; break;
			// See https://en.wikipedia.org/wiki/Escape_sequences_in_C#Table_of_escape_sequences
		case '\a': os << "\\t"; break;
		case '\b': os << "\\b"; break;
		case '\f': os << "\\f"; break;
		case '\n': os << "\\n"; break;
		case '\r': os << "\\r"; break;
		case '\t': os << "\\t"; break;
		case '\v': os << "\\v"; break;
			// All other to-escape-characters are rendered as hexadecimal :
		default: os << "\\x" << std::setfill('0') << std::setw(4) << std::right << std::hex << std::uppercase << (wint_t)c;
		}
	}
	else
		os << c;
}

LOG_SANITIZE_HEADER(hex1, uint8_t)
{
	return os << hexstring8 << (int)container.value;
}

LOG_SANITIZE_HEADER(hex2, uint16_t)
{
	return os << hexstring16 << (int)container.value;
}

LOG_SANITIZE_HEADER(hex4, uint32_t)
{
	return os << hexstring32 << (int)container.value;
}

LOG_SANITIZE_HEADER(sanitized_char, char)
{
	output_char(os, container.value);
	return os;
}

LOG_SANITIZE_HEADER(sanitized_wchar, wchar_t)
{
	output_wchar(os, container.value);
	return os;
}

LOG_SANITIZE_HEADER(sanitized_char_pointer, char *)
{
	char *v = container.value;

	os << "(char *)";
	if (v == nullptr)
		return os << "NULL";

	bool needsEscaping = false;
	int max_length = container.max;
	while (*v && max_length--)
		if (needs_escape(*v++))
		{
			needsEscaping = true;
			break;
		}

	v = container.value;
	os << hexstring32 << (uint32_t)v << " = \"";
	max_length = container.max;
	if (needsEscaping)
	{
		while (*v && max_length--)
			output_char(os, *v++);
	}
	else {
		while (*v && max_length--) {
			os << *v++;
		}
	}

	return os << "\"";
}

LOG_SANITIZE_HEADER(sanitized_wchar_pointer, wchar_t *)
{
	wchar_t *v = container.value;

	os << "(wchar *)";
	if (v == nullptr)
		return os << "NULL";

	bool needsEscaping = false;
	int max_length = container.max;
	while (*v && max_length--)
		if (needs_escape(*v++))
		{
			needsEscaping = true;
			break;
		}

	v = container.value;
	os << hexstring32 << (uint32_t)v << " = \"";
	max_length = container.max;
	if (needsEscaping)
	{
		while (*v && max_length--)
			output_wchar(os, *v++);
	}
	else
#if 0
		os << v; // TODO : FIXME - VS2015 doesn''t render this string (instead, it shows a hexadecimal memory address)
#else // For now, render unicode as ANSI (replacing non-printables with '?')
	{
		while (*v && max_length--) {
			output_char(os, *v <= 0xFF ? (char)*v : '?');
			v++;
		}
	}
#endif
	return os << "\"";
}

LOGRENDER_HEADER_BY_REF(PVOID)
{
	return os << hex4((uint32_t)value);
}
