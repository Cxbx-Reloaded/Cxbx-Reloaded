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
#ifndef CXBX_H
#define CXBX_H

#include <cstdint>

#define FUNC_EXPORTS __pragma(comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__))

/*! xbaddr is the type of a physical address */
typedef uint32_t xbaddr;

/*! xbnullptr is the type of null pointer address*/
#define xbnullptr nullptr

/*! xbnull is the type of null address or value*/
#define xbnull  0

#ifdef _DEBUG
/*! define this to track memory allocations */
//#define _DEBUG_ALLOC
#endif
/*! define this to trace intercepted function calls */
#ifdef _DEBUG
#define _DEBUG_TRACE 1
#endif
/*! define this to trace vertex shader constants */
#define _DEBUG_TRACK_VS_CONST
/*! define this to print current configuration at kernel startup */
#define _DEBUG_PRINT_CURRENT_CONF

/*! define this to dump textures that have been set */
//#define _DEBUG_DUMP_TEXTURE_SETTEXTURE "D:\\xbox\\_textures\\"
/*! define this to dump textures that are registered */
//#define _DEBUG_DUMP_TEXTURE_REGISTER   "D:\\cxbx\\_textures\\"

extern bool g_bIntegrityChecking;
#ifdef _DEBUG
//extern void CxbxCheckIntegrity();
#define CXBX_CHECK_INTEGRITY() //CxbxCheckIntegrity()
#else
#define CXBX_CHECK_INTEGRITY()
#endif

/*! debug mode choices */
enum DebugMode { DM_NONE, DM_CONSOLE, DM_FILE };

/*! debugger enable state */
enum DebuggerState { debuggerOff, debuggerOn };

/*! type of Xbe */
enum XbeType { xtRetail, xtDebug, xtChihiro };

extern XbeType g_XbeType;

/*! indicates emulation of an Chihiro (arcade, instead of Xbox console) executable */
extern bool g_bIsChihiro;

/*! indicates emulation of a Debug xbe executable */
extern bool g_bIsDebug;

/*! indicates emulation of a Retail xbe executable*/
extern bool g_bIsRetail;

/*! indicates ability to save on exit (needed for settings reset) */
extern bool g_SaveOnExit;

/*! runtime logging toggle boolean */
extern volatile bool g_bPrintfOn;

#if WIN32
#include "Win32\Threads.h"
#define CxbxSetThreadName(Name) SetCurrentThreadName(Name)
#else
#define CxbxSetThreadName(Name)
#endif

#include <filesystem>

#endif
