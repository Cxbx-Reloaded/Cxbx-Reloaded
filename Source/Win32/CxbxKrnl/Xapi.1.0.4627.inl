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
// * RtlFreeHeap
// ******************************************************************
SOOVPA<9> RtlFreeHeap_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    {
        // RtlFreeHeap+0x1F : test edi, edi
        { 0x1F, 0x85 }, // (Offset,Value)-Pair #1
        { 0x20, 0xFF }, // (Offset,Value)-Pair #2

        // RtlFreeHeap+0x23 : mov al, 1
        { 0x23, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x24, 0x01 }, // (Offset,Value)-Pair #4

        // RtlFreeHeap+0x35 : mov eax, fs:[0x20]
        { 0x35, 0x64 }, // (Offset,Value)-Pair #5
        { 0x36, 0xA1 }, // (Offset,Value)-Pair #6
        { 0x37, 0x20 }, // (Offset,Value)-Pair #7

        // RtlFreeHeap+0x49 : push 0x0B
        { 0x49, 0x6A }, // (Offset,Value)-Pair #8
        { 0x4A, 0x0B }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * XAPI_1_0_4627
// ******************************************************************
OOVPATable XAPI_1_0_4627[] =
{
    // RtlCreateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    {
        (OOVPA*)&RtlCreateHeap_1_0_4361,

        xapi::EmuRtlCreateHeap,

        #ifdef _DEBUG_TRACE
        "RtlCreateHeap"
        #endif
    },
    // RtlAllocateHeap (* unchanged since 1.0.4361 *) (* OR FARTHER *)
    {
        (OOVPA*)&RtlAllocateHeap_1_0_4361,

        xapi::EmuRtlAllocateHeap,

        #ifdef _DEBUG_TRACE
        "RtlAllocateHeap"
        #endif
    },
    // RtlFreeHeap
    {
        (OOVPA*)&RtlFreeHeap_1_0_4627,

        xapi::EmuRtlFreeHeap,

        #ifdef _DEBUG_TRACE
        "RtlFreeHeap"
        #endif
    },
    // XInitDevices (* unchanged since 1.0.4034 *)
    {
        (OOVPA*)&XInitDevices_1_0_4034,

        xapi::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
    // XGetDevices (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XGetDevices_1_0_4361,

        xapi::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices" 
        #endif
    },
    // XInputOpen (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputOpen_1_0_4361,

        xapi::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4361,

        xapi::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XInputGetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetState_1_0_4361,

        xapi::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiThreadStartup_1_0_4361,

        xapi::EmuXapiThreadStartup,

        #ifdef _DEBUG_TRACE
        "XapiThreadStartup"
        #endif
    },
    //* Too High Level
    // XapiInitProcess (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiInitProcess_1_0_4361,

        xapi::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess" 
        #endif
    },
    //*/
    // XapiBootToDash (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&XapiBootDash_1_0_3911,

        xapi::EmuXapiBootDash,

        #ifdef _DEBUG_TRACE
        "EmuXapiBootDash"
        #endif
    },
};

// ******************************************************************
// * XAPI_1_0_4627_SIZE
// ******************************************************************
uint32 XAPI_1_0_4627_SIZE = sizeof(XAPI_1_0_4627);
