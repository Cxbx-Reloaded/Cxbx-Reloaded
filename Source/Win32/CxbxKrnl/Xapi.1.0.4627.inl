// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4627.cpp
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

// ******************************************************************
// * XAPI_1_0_4627
// ******************************************************************
OOVPATable XAPI_1_0_4627[] =
{
    // XInputOpen (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputOpen_1_0_4361,

        xboxkrnl::EmuXXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXXInputOpen"
        #endif
    },
    // XInitDevices (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInitDevices_1_0_4361,

        xboxkrnl::EmuXXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXXInitDevices"
        #endif
    },
    // XGetDevices (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XGetDevices_1_0_4361,

        xboxkrnl::EmuXXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXXGetDevices" 
        #endif
    },
    // CreateThread (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&CreateThread_1_0_4361,

        xboxkrnl::EmuXCreateThread,

        #ifdef _DEBUG_TRACE
        "EmuXCreateThread" 
        #endif
    },
    // CloseHandle (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&CloseHandle_1_0_4361,

        xboxkrnl::EmuXCloseHandle,

        #ifdef _DEBUG_TRACE
        "EmuXCloseHandle" 
        #endif
    },
    // XapiInitProcess (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiInitProcess_1_0_4361,

        xboxkrnl::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess" 
        #endif
    },
    // XapiBootToDash (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiBootDash_1_0_4361,

        xboxkrnl::EmuXapiBootDash,

        #ifdef _DEBUG_TRACE
        "EmuXapiBootDash"
        #endif
    },
    // __rtinit (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&__rtinit_1_0_4361,

        xboxkrnl::EmuX__rtinit,

        #ifdef _DEBUG_TRACE
        "EmuX__rtinit",
        #endif
    },
    // __cinit (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&__cinit_1_0_4361,

        xboxkrnl::EmuX__cinit,

        #ifdef _DEBUG_TRACE
        "EmuX__cinit",
        #endif
    },
};

// ******************************************************************
// * XAPI_1_0_4627_SIZE
// ******************************************************************
uint32 XAPI_1_0_4627_SIZE = sizeof(XAPI_1_0_4627);
