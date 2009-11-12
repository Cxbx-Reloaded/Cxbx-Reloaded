// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Cxbx.h
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

/*! CxbxKrnl exports, others import */
#ifndef _CXBXKRNL_INTERNAL
#define CXBXKRNL_API __declspec(dllimport)
#else
#define CXBXKRNL_API __declspec(dllexport)
#endif

/*! \name primitive typedefs */
/*! \{ */
typedef signed int     sint;
typedef unsigned int   uint;
typedef char           int08;
typedef short          int16;
typedef long           int32;
typedef unsigned char  uint08;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef signed char    sint08;
typedef signed short   sint16;
typedef signed long    sint32;
/*! \} */

/*! define this to track vertex buffers */
#define _DEBUG_TRACK_VB
/*! define this to track vertex shaders */
#define _DEBUG_TRACK_VS
/*! define this to track push buffers */
#define _DEBUG_TRACK_PB
/*! define this to track memory allocations */
#ifdef _DEBUG
#define _DEBUG_ALLOC
#endif
/*! define this to trace intercepted function calls */
#ifdef _DEBUG
#define _DEBUG_TRACE
#endif
/*! define this to trace warnings */
#define _DEBUG_WARNINGS

/*! define this to dump textures that have been set */
//#define _DEBUG_DUMP_TEXTURE_SETTEXTURE "C:\\xbox\\_textures\\"
/*! define this to dump textures that are registered */
//#define _DEBUG_DUMP_TEXTURE_REGISTER   "C:\\xbox\\_textures\\"

/*! version string dependent on trace flag */
#ifndef _DEBUG_TRACE
#define _CXBX_VERSION "0.8.1-Beta"
#else
#define _CXBX_VERSION "0.8.1-Beta-Trace"
#endif

/*! debug mode choices */
enum DebugMode { DM_NONE, DM_CONSOLE, DM_FILE };

/*! maximum number of threads cxbx can handle */
#define MAXIMUM_XBOX_THREADS 256

/*! runtime DbgPrintf toggle boolean */
extern volatile bool g_bPrintfOn;

/*! DbgPrintf enabled if _DEBUG_TRACE is set */
#ifdef _DEBUG_TRACE
#define DbgPrintf if(g_bPrintfOn) printf
#else
inline void null_func(...) { }
#define DbgPrintf null_func
#endif

#endif
