// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXapi.h
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
#ifndef EMUXAPI_H
#define EMUXAPI_H

// ******************************************************************
// * calling conventions
// ******************************************************************
#define WINAPI              __stdcall

// ******************************************************************
// * Basic types
// ******************************************************************
typedef char                CHAR, CCHAR;
typedef short               SHORT, CSHORT;
typedef long                LONG;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef unsigned char       BOOLEAN;
typedef unsigned short      USHORT;
typedef	unsigned short		WORD;
typedef unsigned long       ULONG;
typedef	unsigned long       DWORD;
typedef unsigned long       SIZE_T, *PSIZE_T;
typedef unsigned long       ACCESS_MASK, *PACCESS_MASK;
typedef unsigned long       PHYSICAL_ADDRESS;
typedef long                INT_PTR;
typedef unsigned __int64    ULONGLONG;

// xapi
typedef int                 BOOL;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef LONG                HRESULT;

// ******************************************************************
// * Pointer types
// ******************************************************************
typedef CHAR               *PCHAR;
typedef CHAR               *PCSZ;
typedef BYTE               *PBYTE;
typedef BOOLEAN            *PBOOLEAN;
typedef UCHAR              *PUCHAR;
typedef USHORT             *PUSHORT;
typedef ULONG              *PULONG;
typedef DWORD              *LPDWORD;
typedef ACCESS_MASK        *PACCESS_MASK;
typedef LONG               *LONG_PTR;
typedef ULONG              *ULONG_PTR;
typedef INT_PTR            *PINT_PTR;
#ifndef VOID
#define VOID void
#endif
typedef VOID               *PVOID, *LPVOID;
typedef void               *HANDLE;
typedef HANDLE             *PHANDLE;

// ******************************************************************
// * documentation purposes only
// ******************************************************************
#define EXPORTNUM(a)
#define UNALIGNED
#define OPTIONAL
#define IN
#define OUT

// ******************************************************************
// * LPSECURITY_ATTRIBUTES
// ******************************************************************
typedef void* LPSECURITY_ATTRIBUTES;

// ******************************************************************
// * PTHREAD_START_ROUTINE / LPTHREAD_START_ROUTINE
// ******************************************************************
typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)
(
    LPVOID lpThreadParameter
);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

// ******************************************************************
// * pfRtlCreateHeap
// ******************************************************************
typedef PVOID (WINAPI *pfRtlCreateHeap)
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN BOOLEAN Lock OPTIONAL,
    IN PVOID   RtlHeapParams OPTIONAL
);

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
// * XINPUT_FEEDBACK_HEADER
// ******************************************************************
typedef struct _XINPUT_FEEDBACK_HEADER
{
    DWORD           dwStatus;
    HANDLE OPTIONAL hEvent;
    BYTE            Reserved[58];
}
XINPUT_FEEDBACK_HEADER, *PXINPUT_FEEDBACK_HEADER;

// ******************************************************************
// * XINPUT_FEEDBACK
// ******************************************************************
typedef struct _XINPUT_FEEDBACK
{
    XINPUT_FEEDBACK_HEADER Header;

    union
    {
        XINPUT_RUMBLE Rumble;
    };
}
XINPUT_FEEDBACK, *PXINPUT_FEEDBACK;

// ******************************************************************
// * RTL_HEAP_PARAMETERS
// ******************************************************************
typedef struct _RTL_HEAP_PARAMETERS
{
    ULONG   Length;
    BYTE    Unknown[0x2C];
}
RTL_HEAP_PARAMETERS;

// ******************************************************************
// * func: EmuRtlCreateHeap
// ******************************************************************
PVOID WINAPI EmuRtlCreateHeap
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN BOOLEAN Lock OPTIONAL,
    IN PVOID   RtlHeapParams OPTIONAL
);

// ******************************************************************
// * func: EmuRtlAllocateHeap
// ******************************************************************
PVOID WINAPI EmuRtlAllocateHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN SIZE_T dwBytes
);

// ******************************************************************
// * func: EmuRtlFreeHeap
// ******************************************************************
BOOL WINAPI EmuRtlFreeHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
);

// ******************************************************************
// * func: EmuRtlSizeHeap
// ******************************************************************
SIZE_T WINAPI EmuRtlSizeHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
);

// ******************************************************************
// * func: XapiUnknownBad1
// ******************************************************************
// NOTE: This does some hard disk verification and other things
VOID WINAPI EmuXapiUnknownBad1
(
    IN DWORD dwUnknown
);

// ******************************************************************
// * func: EmuQueryPerformanceCounter
// ******************************************************************
BOOL WINAPI EmuQueryPerformanceCounter
(
    PLARGE_INTEGER lpPerformanceCount
);

// ******************************************************************
// * func: EmuQueryPerformanceCounter
// ******************************************************************
BOOL WINAPI EmuQueryPerformanceFrequency
(
    PLARGE_INTEGER lpFrequency
);

// ******************************************************************
// * func: EmuXMountUtilityDrive
// ******************************************************************
BOOL WINAPI EmuXMountUtilityDrive
(
    BOOL    fFormatClean
);

// ******************************************************************
// * func: EmuXInitDevices
// ******************************************************************
VOID WINAPI EmuXInitDevices
(
    DWORD   Unknown1,
    PVOID   Unknown2
);

// ******************************************************************
// * func: EmuXGetDevices
// ******************************************************************
DWORD WINAPI EmuXGetDevices
(
    XPP_DEVICE_TYPE *DeviceType
);

// ******************************************************************
// * func: EmuXGetDeviceChanges
// ******************************************************************
BOOL WINAPI EmuXGetDeviceChanges
(
    PXPP_DEVICE_TYPE DeviceType,
    PDWORD           pdwInsertions,                  
    PDWORD           pdwRemovals                     
);

// ******************************************************************
// * func: EmuXInputOpen
// ******************************************************************
HANDLE WINAPI EmuXInputOpen
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
);

// ******************************************************************
// * func: EmuXInputClose
// ******************************************************************
VOID WINAPI EmuXInputClose
(
    IN HANDLE hDevice
);

// ******************************************************************
// * func: EmuXInputGetCapabilities
// ******************************************************************
DWORD WINAPI EmuXInputGetCapabilities
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
);

// ******************************************************************
// * func: EmuXInputGetState
// ******************************************************************
DWORD WINAPI EmuXInputGetState
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
);

// ******************************************************************
// * func: EmuXInputGetState
// ******************************************************************
DWORD WINAPI EmuXInputSetState
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
);

// ******************************************************************
// * func: EmuCreateThread
// ******************************************************************
HANDLE WINAPI EmuCreateThread
(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    DWORD                   dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    LPVOID                  lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
);

// ******************************************************************
// * func: EmuCreateMutex
// ******************************************************************
HANDLE WINAPI EmuCreateMutex
(
    LPSECURITY_ATTRIBUTES   lpMutexAttributes,
    BOOL                    bInitialOwner,
    LPCSTR                  lpName
);

// ******************************************************************
// * func: EmuCloseHandle
// ******************************************************************
BOOL WINAPI EmuCloseHandle
(
    HANDLE hObject
);

// ******************************************************************
// * func: EmuSetThreadPriority
// ******************************************************************
BOOL WINAPI EmuSetThreadPriority
(
    HANDLE  hThread,
    int     nPriority
);

// ******************************************************************
// * func: EmuGetExitCodeThread
// ******************************************************************
BOOL WINAPI EmuGetExitCodeThread
(
    HANDLE  hThread,
    LPDWORD lpExitCode
);

// ******************************************************************
// * func: EmuXapiInitProcess
// ******************************************************************
VOID WINAPI EmuXapiInitProcess();

// ******************************************************************
// * data: EmuXapiProcessHeap
// ******************************************************************
extern PVOID* EmuXapiProcessHeap;

// ******************************************************************
// * func: g_pRtlCreateHeap
// ******************************************************************
extern pfRtlCreateHeap g_pRtlCreateHeap;

// ******************************************************************
// * func: EmuXapiThreadStartup
// ******************************************************************
VOID WINAPI EmuXapiThreadStartup
(
    DWORD dwDummy1,
    DWORD dwDummy2
);

/* Too High Level!
// ******************************************************************
// * func: XapiSetupPerTitleDriveLetters
// ******************************************************************
NTSTATUS CDECL XapiSetupPerTitleDriveLetters(DWORD dwTitleId, LPCWSTR wszTitleName);
*/

// ******************************************************************
// * func: EmuXapiBootDash
// ******************************************************************
VOID WINAPI EmuXapiBootDash(DWORD UnknownA, DWORD UnknownB, DWORD UnknownC);

// ******************************************************************
// * func: EmuXCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EmuXCalculateSignatureBegin(DWORD dwFlags);

#endif
