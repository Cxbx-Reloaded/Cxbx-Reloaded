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
// *  (c) 2019 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef _LOGGING_H
#define _LOGGING_H

#include <windows.h> // For DWORD
#include <sstream> // For std::stringstream
#include <iostream> // For std::cout
#include <iomanip> // For std::setw
#include <atomic> // For atomic_bool and atomic_uint
#include "common\util\CxbxUtil.h" // For g_bPrintfOn and to_underlying

// NOTE: using ERROR2 since windows.h imports an ERROR macro which would conflict otherwise
typedef enum class _LOG_LEVEL {
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR2,
	FATAL,
	MAX,
}LOG_LEVEL;

typedef enum class _CXBXR_MODULE: unsigned int {
	// general
	CXBXR = 0,
	XBE,
	INIT,
	VMEM,
	PMEM,
	GUI,
	EEPR,
	RSA,
	POOLMEM,
	D3D8,
	D3DST,
	D3DCVT,
	DSOUND,
	XAPI,
	XACT,
	XGRP,
	XONLINE,
	FS,
	PSHB,
	PXSH,
	VTXSH,
	VSHCACHE,
	VTXB,
	DINP,
	XINP,
	SDL,
	FILE,
	X86,
	HLE,
	NET,
	MCPX,
	NV2A,
	SMC,
	OHCI,
	USB,
	HUB,
	XIDCTRL,
	ADM,
	INPSYS,
	DSBUFFER,
	DSSTREAM,
	DS3DCALC,
	XMO,
	RINP,
	JVS,
	// kernel
	KRNL,
	LOG,
	XBOX,
	XBDM,
	AV,
	DBG,
	EX,
	FSC,
	HAL,
	IO,
	KD,
	KE,
	KI,
	MM,
	NT,
	OB,
	PS,
	RTL,
	XC,
	XE,
	// max
	MAX,
}CXBXR_MODULE;

extern std::atomic_bool g_EnabledModules[to_underlying(CXBXR_MODULE::MAX)];
extern const char* g_EnumModules2String[to_underlying(CXBXR_MODULE::MAX)];
extern std::atomic_int g_CurrentLogLevel;
extern std::atomic_bool g_CurrentLogPopupTestCase;

// print out a log message to the console or kernel debug log file if level is high enough
void EmuLogEx(CXBXR_MODULE cxbxr_module, LOG_LEVEL level, const char *szWarningMessage, ...);
void EmuLogInit(LOG_LEVEL level, const char *szWarningMessage, ...);

#define EmuLog(level, fmt, ...) EmuLogEx(LOG_PREFIX, level, fmt, ##__VA_ARGS__)

extern inline void log_get_settings();

extern inline void log_sync_config();

void log_set_config(int LogLevel, unsigned int* LoggedModules, bool LogPopupTestCase);

void log_generate_active_filter_output(const CXBXR_MODULE cxbxr_module);

// Use emulation environment to manage popup messages
// If log_init_popup_msg is not called at earliest point of emulation.
// Then users will have a chance of popup message appear during start of emulation in full screen.
void log_init_popup_msg();

typedef enum class _PopupIcon {
	Unknown = 0,
	Question,
	Info,
	Warning,
	Error
} PopupIcon;

typedef enum class _PopupButtons {
	Unknown = 0,
	Ok,
	OkCancel,
	AbortRetryIgnore,
	YesNoCancel,
	YesNo,
	RetryCancel
} PopupButtons;

typedef enum class _PopupReturn {
	Unknown = 0,
	Ok,
	Cancel,
	Abort,
	Retry,
	Ignore,
	Yes,
	No
} PopupReturn;

PopupReturn PopupCustomEx(const void* hwnd, const CXBXR_MODULE cxbxr_module, const LOG_LEVEL level, const PopupIcon icon, const PopupButtons buttons, const PopupReturn ret_default, const char* message, ...);

#define PopupCustom(hwnd, level, icon, buttons, ret_default, fmt, ...) PopupCustomEx(hwnd, LOG_PREFIX, level, icon, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupQuestionEx(hwnd, level, buttons, ret_default, fmt, ...) PopupCustom(hwnd, level, PopupIcon::Question, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupQuestion(hwnd, fmt, ...) PopupQuestionEx(hwnd, LOG_LEVEL::INFO, PopupButtons::YesNoCancel, PopupReturn::Cancel, fmt, ## __VA_ARGS__)
#define PopupInfoEx(hwnd, buttons, ret_default, fmt, ...) PopupCustom(hwnd, LOG_LEVEL::INFO, PopupIcon::Info, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupInfo(hwnd, fmt, ...) (void)PopupInfoEx(hwnd, PopupButtons::Ok, PopupReturn::Ok, fmt, ## __VA_ARGS__)
#define PopupWarningEx(hwnd, buttons, ret_default, fmt, ...) PopupCustom(hwnd, LOG_LEVEL::WARNING, PopupIcon::Warning, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupWarning(hwnd, fmt, ...) (void)PopupWarningEx(hwnd, PopupButtons::Ok, PopupReturn::Ok, fmt, ## __VA_ARGS__)
#define PopupErrorEx(hwnd, buttons, ret_default, fmt, ...) PopupCustom(hwnd, LOG_LEVEL::ERROR2, PopupIcon::Error, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupError(hwnd, fmt, ...) (void)PopupErrorEx(hwnd, PopupButtons::Ok, PopupReturn::Ok, fmt, ## __VA_ARGS__)
#define PopupFatalEx(hwnd, buttons, ret_default, fmt, ...) PopupCustom(hwnd, LOG_LEVEL::FATAL, PopupIcon::Error, buttons, ret_default, fmt, ## __VA_ARGS__)
#define PopupFatal(hwnd, fmt, ...) (void)PopupFatalEx(hwnd, PopupButtons::Ok, PopupReturn::Ok, fmt, ## __VA_ARGS__)

// For LOG_TEST_CASE
extern inline void EmuLogOutputEx(const CXBXR_MODULE cxbxr_module, const LOG_LEVEL level, const char *szWarningMessage, ...);

//
// __FILENAME__
//

// From https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
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
inline T _log_sanitize(T value, int ignored_length = 0)
{
	return value;
}

#if 0 // TODO FIXME : Disabled for now, as this is incorrectly called for INT types too
// Convert booleans to strings properly
inline const char * _log_sanitize(BOOL value, int ignored_length = 0)
{
	return value ? "TRUE" : "FALSE";
}
#endif

// Macro to ease declaring a _log_sanitize overload (invokeable via C) for type T
#define LOG_SANITIZE_HEADER(C, T)                     \
std::ostream& operator<<(                             \
    std::ostream& os,                                 \
    const Sane##C& container)                         \

#define LOG_SANITIZE(C, T)                            \
struct Sane##C                                        \
{                                                     \
  T value;                                            \
  int max;                                            \
  Sane##C(T _v, int _m = 80) : value(_v), max(_m) { } \
};                                                    \
                                                      \
inline Sane##C C(T value, int max = 80)               \
{                                                     \
    return Sane##C(value, max);                       \
}                                                     \
                                                      \
inline Sane##C _log_sanitize(T value, int max = 80)   \
{                                                     \
    return C(value, max);                             \
}                                                     \
                                                      \
extern LOG_SANITIZE_HEADER(C, T)                      \

// Hex output (type safe)
// https://stackoverflow.com/questions/673240/how-do-i-print-an-unsigned-char-as-hex-in-c-using-ostream
LOG_SANITIZE(hex1, uint8_t);
LOG_SANITIZE(hex2, uint16_t);
LOG_SANITIZE(hex4, uint32_t);

// Character output (escaped into a C-string representation)
// https://en.wikipedia.org/wiki/Escape_sequences_in_C
LOG_SANITIZE(sanitized_char, char);
LOG_SANITIZE(sanitized_wchar, wchar_t);
LOG_SANITIZE(sanitized_char_pointer, char *);
LOG_SANITIZE(sanitized_wchar_pointer, wchar_t *);


//
// Function (and argument) logging defines
//

constexpr const int str_length(const char* str) {
	return str_end(str) - str;
}

constexpr const char* str_skip_prefix(const char* str, const char *prefix) {
	return (*str == *prefix) ? str_skip_prefix(str + 1, prefix + 1) : str;
}

constexpr const char* remove_prefix(const char* str, const char *prefix) {
	return (str_skip_prefix(str, prefix) == str + str_length(prefix)) ? str_skip_prefix(str, prefix) : str;
}

static const char* xbox_prefix = "xbox::";
static const char* emupatch_prefix = "EmuPatch_"; // See #define EMUPATCH

constexpr const char* remove_emupatch_prefix(const char* str) {
	// return an empty string when str isn't given
	// skip xbox:: and/or EmuPatch_ prefix if present
	return remove_prefix(remove_prefix(str, xbox_prefix), emupatch_prefix);
}

#define LOG_ARG_START "\n   " << std::setfill(' ') << std::setw(20) << std::left 
#define LOG_ARG_OUT_START "\n OUT " << std::setfill(' ') << std::setw(18) << std::left 

#ifndef LOG_PREFIX
#define LOG_PREFIX __FILENAME__
#endif // LOG_PREFIX

// For thread_local, see : https://en.cppreference.com/w/cpp/language/storage_duration
// TODO : Use Boost.Format https://www.boost.org/doc/libs/1_53_0/libs/format/index.html
extern thread_local std::string _logThreadPrefix;

// Checks if this log should be printed or not
#define LOG_CHECK_ENABLED_EX(cxbxr_module, level) \
	if (g_EnabledModules[to_underlying(cxbxr_module)] && to_underlying(level) >= g_CurrentLogLevel)

// Checks if this log should be printed or not
#define LOG_CHECK_ENABLED(level) \
	LOG_CHECK_ENABLED_EX(LOG_PREFIX, level)

#define LOG_THREAD_INIT \
	if (_logThreadPrefix.length() == 0) { \
		std::stringstream tmp; \
		tmp << "[" << hexstring16 << GetCurrentThreadId() << "] "; \
		_logThreadPrefix = tmp.str(); \
    }

#define LOG_FUNC_INIT(func) \
	static thread_local std::string _logFuncPrefix; \
	if (_logFuncPrefix.length() == 0) {	\
		std::stringstream tmp; \
		tmp << g_EnumModules2String[to_underlying(LOG_PREFIX)] << (func != nullptr ? remove_emupatch_prefix(func) : ""); \
		_logFuncPrefix = tmp.str(); \
	}

#define LOG_INIT \
	LOG_THREAD_INIT \
	LOG_FUNC_INIT(__func__)

#define LOG_FINIT \
	_logFuncPrefix.clear(); // Reset prefix, to show caller changes

#define LOG_FUNC_BEGIN_NO_INIT \
	do { if(g_bPrintfOn) { \
		bool _had_arg = false; \
		std::stringstream msg; \
		msg << _logThreadPrefix << _logFuncPrefix << "(";

#define LOG_FUNC_BEGIN \
		LOG_INIT \
		LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { \
			LOG_FUNC_BEGIN_NO_INIT

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
			if (_had_arg) msg << "\n"; \
			msg << ");\n"; \
			std::cout << msg.str(); \
		} } while (0); \
	}

#define LOG_FUNC_BEGIN_ARG_RESULT_NO_INIT \
	do { if(g_bPrintfOn) { \
		bool _had_arg = false; \
		std::stringstream msg; \
		msg << _logThreadPrefix << _logFuncPrefix << " returns OUT {";

#define LOG_FUNC_BEGIN_ARG_RESULT \
		LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { \
			LOG_FUNC_BEGIN_ARG_RESULT_NO_INIT

// LOG_FUNC_ARG_RESULT writes output via all available ostream << operator overloads, sanitizing and adding detail where possible
#define LOG_FUNC_ARG_RESULT(arg) \
		_had_arg = true; \
		msg << LOG_ARG_START << "*"#arg << " : "; \
		if (arg != nullptr) { \
			msg << _log_sanitize(*arg); \
		} else { \
			msg << "NOT SET"; \
		}

// LOG_FUNC_ARG_RESULT_TYPE writes result output using the overloaded << operator of the given type
#define LOG_FUNC_ARG_RESULT_TYPE(type, arg) \
		_had_arg = true; \
		msg << LOG_ARG_START << "*"#arg << " : "; \
		if (arg != nullptr) { \
			msg << (type)*arg; \
		} else { \
			msg << "NOT SET"; \
		}

// LOG_FUNC_END_ARG_RESULT closes off function and optional argument result logging
#define LOG_FUNC_END_ARG_RESULT \
			if (_had_arg) msg << "\n"; \
			msg << "};\n"; \
			std::cout << msg.str(); \
		} } while (0); \
	}

// LOG_FUNC_RESULT logs the function return result
#define LOG_FUNC_RESULT(r) \
	std::cout << _logThreadPrefix << _logFuncPrefix << " returns " << _log_sanitize(r) << "\n";

// LOG_FUNC_RESULT_TYPE logs the function return result using the overloaded << operator of the given type
#define LOG_FUNC_RESULT_TYPE(type, r) \
	std::cout << _logThreadPrefix << _logFuncPrefix << " returns " << (type)r << "\n";

// LOG_FORWARD indicates that an api is implemented by a forward to another API
#define LOG_FORWARD(api) \
	LOG_INIT \
	LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { \
		do { if(g_bPrintfOn) { \
			std::cout << _logThreadPrefix << _logFuncPrefix << " forwarding to "#api"...\n"; \
		} } while (0); \
	}

// LOG_IGNORED indicates that Cxbx consiously ignores an api
#define LOG_IGNORED() \
	do { \
		static bool b_echoOnce = true; \
			if(g_bPrintfOn && b_echoOnce) { \
				LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
					LOG_THREAD_INIT \
					LOG_FUNC_INIT(__func__) \
					std::cout << _logThreadPrefix << "WARN: " << _logFuncPrefix << " ignored!\n"; \
					b_echoOnce = false; \
				} \
			} \
		} while(0)

// LOG_UNIMPLEMENTED indicates that Cxbx is missing an implementation of an api
#define LOG_UNIMPLEMENTED() \
	do { \
		static bool b_echoOnce = true; \
			if(g_bPrintfOn && b_echoOnce) { \
				LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
					LOG_THREAD_INIT \
					LOG_FUNC_INIT(__func__) \
					std::cout << _logThreadPrefix << "WARN: " << _logFuncPrefix << " unimplemented!\n"; \
					b_echoOnce = false; \
				} \
			} \
		} while(0)

// LOG_INCOMPLETE indicates that Cxbx is missing part of an implementation of an api
#define LOG_INCOMPLETE() \
	do { \
		static bool b_echoOnce = true; \
			if(g_bPrintfOn && b_echoOnce) { \
				LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
					LOG_THREAD_INIT \
					LOG_FUNC_INIT(__func__) \
					std::cout << _logThreadPrefix << "WARN: " << _logFuncPrefix << " incomplete!\n"; \
					b_echoOnce = false; \
				} \
			} \
		} while(0)

// LOG_NOT_SUPPORTED indicates that Cxbx cannot implement (part of) an api
#define LOG_NOT_SUPPORTED() \
	do { \
		static bool b_echoOnce = true; \
			if(g_bPrintfOn && b_echoOnce) { \
				LOG_CHECK_ENABLED(LOG_LEVEL::INFO) { \
					LOG_THREAD_INIT \
					LOG_FUNC_INIT(__func__) \
					std::cout << _logThreadPrefix << "WARN: " << _logFuncPrefix << " not supported!\n"; \
					b_echoOnce = false; \
				} \
			} \
		} while(0)

//
// Short hand function logging defines :
//

// Log function without arguments
#define LOG_FUNC() LOG_FUNC_BEGIN LOG_FUNC_END

// Log function with one argument
#define LOG_FUNC_ONE_ARG(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG(arg) LOG_FUNC_END 

// Log function with one typed argument
#define LOG_FUNC_ONE_ARG_TYPE(type, arg) LOG_FUNC_BEGIN LOG_FUNC_ARG_TYPE(type, arg) LOG_FUNC_END 

// Log function with one out argument
#define LOG_FUNC_ONE_ARG_OUT(arg) LOG_FUNC_BEGIN LOG_FUNC_ARG_OUT(arg) LOG_FUNC_END 

// RETURN logs the given result and then returns it (so this should appear last in functions)
#define RETURN(r) do { if (g_bPrintfOn) LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { LOG_FUNC_RESULT(r) } return r; } while (0)

// RETURN_TYPE logs the given typed result and then returns it (so this should appear last in functions)
#define RETURN_TYPE(type, r) do { if (g_bPrintfOn) LOG_CHECK_ENABLED(LOG_LEVEL::DEBUG) { LOG_FUNC_RESULT_TYPE(type, r) } return r; } while (0)

//
// Headers for rendering enums, flags and (pointer-to-)types :
//

// Macro to ease declaration of a render function per Type:
#define LOGRENDER_HEADER_BY_REF(Type) std::ostream& operator<<(std::ostream& os, const Type& value)
#define LOGRENDER_HEADER_BY_PTR(Type) std::ostream& operator<<(std::ostream& os, const Type *value)

#define TYPE2PCHAR(Type) Type##ToPCHAR
#define TYPE2PCHAR_HEADER(Type) const char * TYPE2PCHAR(Type)(const Type &value)

// Macro for implementation of rendering any Type-ToString :
#define LOGRENDER_TYPE(Type) LOGRENDER_HEADER_BY_REF(Type) \
{ return os << "("#Type")" << hex4((int)value) << " = " << TYPE2PCHAR(Type)(value); }

// Macro's for Enum-ToString conversions :
#define ENUM2STR_HEADER(EnumType) extern TYPE2PCHAR_HEADER(EnumType); LOGRENDER_HEADER_BY_REF(EnumType);
#define ENUM2STR_START(EnumType) TYPE2PCHAR_HEADER(EnumType) { switch (value) {
#define ENUM2STR_CASE(a) case a: return #a;
#define ENUM2STR_END(EnumType) default: return "Unknown_"#EnumType; } }
#define ENUM2STR_END_and_LOGRENDER(EnumType) ENUM2STR_END(EnumType) LOGRENDER_TYPE(EnumType)

// Macro's for Flags-ToString conversions :
#define FLAGS2STR_HEADER(FlagType) LOGRENDER_HEADER_BY_REF(FlagType);
#define FLAGS2STR_START(FlagType) std::string TYPE2PCHAR(FlagType)(const FlagType &value) { std::stringstream ss;
#define FLAG2STR(f) if (((uint32_t)value & f) == f) ss << #f << "|";
#define FLAG2STR_MASK(f) ss << #f"=" << (value & f) << "|";
#define FLAGS2STR_END std::string res = ss.str(); if (!res.empty()) res.pop_back(); return res; }
#define FLAGS2STR_END_and_LOGRENDER(FlagType) FLAGS2STR_END LOGRENDER_TYPE(FlagType)

// Macro's for Struct-member rendering :
#define LOGRENDER_MEMBER_ARRAY_OFFSET(Name, Offset) << LOG_ARG_START << (Name) << "[" << (Offset) << "]: {"
#define LOGRENDER_MEMBER_NAME_VALUE(Name, Value) << LOG_ARG_START << (Name) << "  : " << (Value)
#define LOGRENDER_MEMBER_NAME_TYPE_PTR(Name, Type, Value) << LOG_ARG_START << (Name) << "  : " << hex4((uint32_t)Value) << " -> " << (Type) << "* {"
#define LOGRENDER_MEMBER_NAME_TYPE_VALUE(Name, Type, Value) << LOG_ARG_START << (Name) << "  : " << (Type)(Value)
#define LOGRENDER_MEMBER_NAME(Member) << LOG_ARG_START << "."#Member << "  : "
#define LOGRENDER_MEMBER(Member) LOGRENDER_MEMBER_NAME_VALUE("."#Member, value.Member)
#define LOGRENDER_MEMBER_TYPE(Type, Member) LOGRENDER_MEMBER_NAME(Member) << (Type)value.Member
#define LOGRENDER_MEMBER_SANITIZED(Member, MemberType, Length) LOGRENDER_MEMBER_NAME(Member) << _log_sanitize((MemberType)value.Member, Length)

// Macro to ease declaration of two render functions, for type and pointer-to-type :
#define LOGRENDER_HEADER(Type) LOGRENDER_HEADER_BY_PTR(Type); LOGRENDER_HEADER_BY_REF(Type);

// Macro's for dynamic array structure rendering :
template<class A, class B>
static std::string
LogRenderGenerate_Array(std::string MemberStr, A MemberValue, B Count) {
	B counter = 0;
	std::ostringstream strGenerator;
	while (counter < Count) {
		strGenerator LOGRENDER_MEMBER_ARRAY_OFFSET(MemberStr, counter)
			<< *MemberValue << "}";
		MemberValue++;
		counter++;
	}
	strGenerator << "}";
	return strGenerator.str();
}
#define LOGRENDER_MEMBER_ARRAY_TYPE(Type, Member, Count) LOGRENDER_MEMBER_NAME_TYPE_PTR("."#Member, #Type, value.Member) << LogRenderGenerate_Array("."#Member, (Type*)value.Member, value.Count)

// Traits to switch ostreams to our preferred rendering of hexadecimal values
template <class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
hexstring8(std::basic_ostream<_CharT, _Traits>&os)
{
	// std::noshowbase is not neccessary to set (it's the default, and we never use std::showbase)
	return os << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << std::uppercase;
}

template <class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
hexstring16(std::basic_ostream<_CharT, _Traits>&os)
{
	return os << "0x" << std::setfill('0') << std::setw(4) << std::right << std::hex << std::uppercase;
}

template <class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
hexstring32(std::basic_ostream<_CharT, _Traits>&os)
{
	return os << "0x" << std::setfill('0') << std::setw(8) << std::right << std::hex << std::uppercase;
}

// Macro combining pointer-to-type implementation and type rendering header :
#define LOGRENDER(Type)                                         \
LOGRENDER_HEADER_BY_PTR(Type)                                   \
{                                                               \
	os << hexstring32 << reinterpret_cast<uint32_t>(value);     \
	if (value)                                                  \
		os << " -> "#Type"* {" << *value << "}";                \
                                                                \
	return os;                                                  \
}                                                               \
                                                                \
LOGRENDER_HEADER_BY_REF(Type)                                   \

//
// An example type rendering, for PVOID
//

LOGRENDER_HEADER_BY_REF(PVOID);

#endif _LOGGING_H
