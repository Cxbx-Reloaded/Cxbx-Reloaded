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

// ******************************************************************
// * cxbxkrnl exports, others import
// ******************************************************************
#ifndef _CXBXKRNL_INTERNAL
#define CXBXKRNL_API __declspec(dllimport)
#else
#define CXBXKRNL_API __declspec(dllexport)
#endif

// ******************************************************************
// * Caustik's favorite typedefs
// ******************************************************************
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

// ******************************************************************
// * Define this to trace intercepted function calls
// ******************************************************************
//#define _DEBUG_TRACE
//#define _DEBUG_WARNINGS

// ******************************************************************
// * Version information
// ******************************************************************
#ifndef _DEBUG_TRACE
#define _CXBX_VERSION "0.7.8c"
#else
#define _CXBX_VERSION "0.7.8c-Trace"
#endif

// ******************************************************************
// * Round up dwValue to nearest multiple of dwMult
// ******************************************************************
static uint32 RoundUp(uint32 dwValue, uint32 dwMult)
{
    if(dwMult == 0)
        return dwValue;

    return dwValue - (dwValue-1)%dwMult + (dwMult - 1);
}

// ******************************************************************
// * Different debug mode types. Debug information is directed to
// * Either a visible console screen, or to an external file
// ******************************************************************
enum DebugMode
{
    DM_NONE,
    DM_CONSOLE,
    DM_FILE
};

#endif
