// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXxapi.h
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
#ifndef EMUXXAPI_H
#define EMUXXAPI_H

#include "EmuX.h"

// ******************************************************************
// * calling conventions
// ******************************************************************
#define WINAPI              __stdcall

// ******************************************************************
// * basic types
// ******************************************************************
typedef int                 BOOL;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef LONG                HRESULT;

// ******************************************************************
// * PTHREAD_START_ROUTINE / LPTHREAD_START_ROUTINE
// ******************************************************************
typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)
(
    LPVOID lpThreadParameter
);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

// ******************************************************************
// * XINPUT_POLLING_PARAMETERS
// ******************************************************************
typedef struct _XINPUT_POLLING_PARAMETERS
{
    BYTE       fAutoPoll        : 1;
    BYTE       fInterruptOut    : 1;
    BYTE       ReservedMBZ1     : 6;
    BYTE       bInputInterval;
    BYTE       bOutputInterval;
    BYTE       ReservedMBZ2;
}
XINPUT_POLLING_PARAMETERS, *PXINPUT_POLLING_PARAMETERS;

// ******************************************************************
// * XPP_DEVICE_TYPE
// ******************************************************************
typedef struct _XPP_DEVICE_TYPE
{
    ULONG Reserved[3];
}
XPP_DEVICE_TYPE, *PXPP_DEVICE_TYPE;

// ******************************************************************
// * XINPUT_GAMEPAD
// ******************************************************************
typedef struct _XINPUT_GAMEPAD
{
    WORD    wButtons;
    BYTE    bAnalogButtons[8];
    SHORT   sThumbLX;
    SHORT   sThumbLY;
    SHORT   sThumbRX;
    SHORT   sThumbRY;
}
XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

// ******************************************************************
// * XINPUT_RUMBLE
// ******************************************************************
typedef struct _XINPUT_RUMBLE
{
    WORD   wLeftMotorSpeed;
    WORD   wRightMotorSpeed;
}
XINPUT_RUMBLE, *PXINPUT_RUMBLE;

// ******************************************************************
// * XINPUT_CAPABILITIES
// ******************************************************************
typedef struct _XINPUT_CAPABILITIES
{
    BYTE SubType;
    WORD Reserved;

    union  
    {
        XINPUT_GAMEPAD Gamepad;
    }
    In;

    union
    {
        XINPUT_RUMBLE Rumble;
    }
    Out;
}
XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

// ******************************************************************
// * Device SubTypes
// ******************************************************************
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD              0x01
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT          0x02
#define XINPUT_DEVSUBTYPE_GC_WHEEL                0x10
#define XINPUT_DEVSUBTYPE_GC_ARCADE_STICK         0x20
#define XINPUT_DEVSUBTYPE_GC_DIGITAL_ARCADE_STICK 0x21
#define XINPUT_DEVSUBTYPE_GC_FLIGHT_STICK         0x30
#define XINPUT_DEVSUBTYPE_GC_SNOWBOARD            0x40

// ******************************************************************
// * XINPUT_STATE
// ******************************************************************
typedef struct _XINPUT_STATE
{
    DWORD dwPacketNumber;

    union
    {
        XINPUT_GAMEPAD Gamepad;
    };
}
XINPUT_STATE, *PXINPUT_STATE;

// ******************************************************************
// * func: EmuXXInitDevices
// ******************************************************************
VOID WINAPI EmuXXInitDevices
(
    DWORD   Unknown1,
    PVOID   Unknown2
);

// ******************************************************************
// * func: EmuXXGetDevices
// ******************************************************************
DWORD WINAPI EmuXXGetDevices
(
    XPP_DEVICE_TYPE *DeviceType
);

// ******************************************************************
// * func: EmuXXInputOpen
// ******************************************************************
HANDLE WINAPI EmuXXInputOpen
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
);

// ******************************************************************
// * func: EmuXXInputGetCapabilities
// ******************************************************************
DWORD WINAPI EmuXXInputGetCapabilities
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
);

// ******************************************************************
// * func: EmuXInputGetState
// ******************************************************************
DWORD WINAPI EmuXXInputGetState
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
);

// ******************************************************************
// * func: EmuXCreateThread
// ******************************************************************
HANDLE WINAPI EmuXCreateThread
(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    DWORD                   dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
);

// ******************************************************************
// * func: EmuXCloseHandle
// ******************************************************************
BOOL WINAPI EmuXCloseHandle
(
    HANDLE hObject
);

// ******************************************************************
// * func: EmuXapiInitProcess
// ******************************************************************
VOID WINAPI EmuXapiInitProcess();

// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI EmuXapiBootDash(uint32 UnknownA, uint32 UnknownB, uint32 UnknownC);

// ******************************************************************
// * func: EmuX__rtinit
// ******************************************************************
VOID EmuX__rtinit();

// ******************************************************************
// * func: EmuX__cinit
// ******************************************************************
VOID EmuX__cinit();

#endif
