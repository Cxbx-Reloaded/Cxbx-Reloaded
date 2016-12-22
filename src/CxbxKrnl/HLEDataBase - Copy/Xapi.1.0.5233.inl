// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5233.cpp
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
// * XInitDevices
// ******************************************************************
// * NOTE: We are actually intercepting USBD_Init, because
// *       XInitDevices Simply redirects to that function
// ******************************************************************
SOOVPA<7> XInitDevices_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0E, 0x75 },
        { 0x20, 0xBE },
        { 0x2E, 0xC0 },
        { 0x3E, 0xEC },
        { 0x50, 0x0F },
        { 0x5F, 0xE8 },
        { 0x70, 0x5F },
    }
};

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
SOOVPA<8> XInputGetCapabilities_1_0_5233 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1D, 0x85 },
        { 0x3C, 0x8A },
        { 0x5B, 0x00 },
        { 0x85, 0x45 },
        { 0xA0, 0xD4 },
        { 0xBB, 0x02 },
        { 0xDA, 0x8D },
        { 0xF9, 0x46 },
    }
};

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
SOOVPA<7> XGetDeviceChanges_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0C, 0x75 },
        { 0x1A, 0x53 },
        { 0x28, 0xF7 },
        { 0x36, 0x56 },
        { 0x44, 0x89 },
        { 0x52, 0xC8 },
        { 0x60, 0x5F },
    }
};

// ******************************************************************
// * XInputSetState
// ******************************************************************
SOOVPA<7> XInputSetState_1_0_5233 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x05, 0x81 },
        { 0x0C, 0xF6 },
        { 0x13, 0x57 },
        { 0x1A, 0x08 },
        { 0x21, 0x8B },
        { 0x28, 0x88 },
        { 0x30, 0xC2 },
    }
};

// ******************************************************************
// * XGetFileCacheSize
// ******************************************************************
SOOVPA<6> XGetFileCacheSize_1_0_5344 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0xFF },
        { 0x01, 0x15 },
        { 0x06, 0xC1 },
        { 0x07, 0xE0 },
        { 0x08, 0x0C },
        { 0x09, 0xC3 },
    }
};

// ******************************************************************
// * XAPI_1_0_5233
// ******************************************************************
OOVPATable XAPI_1_0_5233[] =
{
    // GetTimeZoneInformation (* unchanged since 3911 *)
    {
        (OOVPA*)&GetTimeZoneInformation_1_0_3911,

        XTL::EmuGetTimeZoneInformation,

        #ifdef _DEBUG_TRACE
        "GetTimeZoneInformation"
        #endif
    },
    // SetThreadPriority (* unchanged since 3911 *)
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    // SetThreadPriorityBoost (* unchanged since 4627 *)
    {
        (OOVPA*)&SetThreadPriorityBoost_1_0_4627,

        XTL::EmuSetThreadPriorityBoost,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriorityBoost"
        #endif
    },
	// GetThreadPriority (* unchanged since 4627 *)
    {
        (OOVPA*)&GetThreadPriority_1_0_4627,

        XTL::EmuGetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuGetThreadPriority"
        #endif
    },
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
    {
        (OOVPA*)&XRegisterThreadNotifyRoutine_1_0_3911,

        XTL::EmuXRegisterThreadNotifyRoutine,

        #ifdef _DEBUG_TRACE
        "EmuXRegisterThreadNotifyRoutine"
        #endif
    },
  
    // QueryPerformanceCounter (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceCounter_1_0_4361,

        XTL::EmuQueryPerformanceCounter,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceCounter"
        #endif
    },
    // QueryPerformanceFrequency (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceFrequency_1_0_4361,

        XTL::EmuQueryPerformanceFrequency,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceFrequency"
        #endif
    },
    // XMountUtilityDrive (* unchanged since 4432 *)
    {
        (OOVPA*)&XMountUtilityDrive_1_0_4432,

        XTL::EmuXMountUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXMountUtilityDrive"
        #endif
    },
    // XInitDevices
    {
        (OOVPA*)&XInitDevices_1_0_5233,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
    // XGetDevices (* unchanged since 1.0.4134 *)
    {
        (OOVPA*)&XGetDevices_1_0_4134,

        XTL::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices"
        #endif
    },
    // XGetDeviceChanges
    {
        (OOVPA*)&XGetDeviceChanges_1_0_5233,

        XTL::EmuXGetDeviceChanges,

        #ifdef _DEBUG_TRACE
        "EmuXGetDeviceChanges"
        #endif
    },
    // XInputOpen (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputOpen_1_0_4361,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_5233,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XID_fCloseDevice (* unchanged since 1.0.4928 *)
    {
        (OOVPA*)&XID_fCloseDevice_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "XID_fCloseDevice (XREF)"
        #endif
    },
    // XInputClose (* unchanged since 1.0.4928 *)
    {
        (OOVPA*)&XInputClose_1_0_4928,

        XTL::EmuXInputClose,

        #ifdef _DEBUG_TRACE
        "EmuXInputClose"
        #endif
    },
    // XInputGetState (* unchanged since 1.0.4928 *)
    {
        (OOVPA*)&XInputGetState_1_0_4928,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputSetState
    {
        (OOVPA*)&XInputSetState_1_0_5233,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
        #endif
    },
	// XGetFileCacheSize
    {
        (OOVPA*)&XGetFileCacheSize_1_0_5344,
        XTL::EmuXGetFileCacheSize,

        #ifdef _DEBUG_TRACE
        "XGetFileCacheSize"
        #endif
	},
	// QueueUserAPC (* unchanged since 3911 *)
	{ 
		(OOVPA*)&QueueUserAPC_1_0_3911,

		XTL::EmuQueueUserAPC,
			
		#ifdef _DEBUG_TRACE
		"EmuQueueUserAPC"
		#endif
	},
	// GetFileAttributesA (* unchanged since 4134 *)
    {
        (OOVPA*)&GetFileAttributesA_1_0_4134,
        XTL::EmuGetFileAttributesA,

        #ifdef _DEBUG_TRACE
        "EmuGetFileAttributesA"
        #endif
    },
};

// ******************************************************************
// * XAPI_1_0_5233_SIZE
// ******************************************************************
uint32 XAPI_1_0_5233_SIZE = sizeof(XAPI_1_0_5233);
