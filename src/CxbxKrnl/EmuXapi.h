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

#include <dxerr8.h>

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
typedef unsigned short      WORD;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
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
// * POLLING_PARAMETERS_HANDLE
// ******************************************************************
typedef struct _POLLING_PARAMETERS_HANDLE
{
    XINPUT_POLLING_PARAMETERS *pPollingParameters;

    DWORD dwPort;
}
POLLING_PARAMETERS_HANDLE, *PPOLLING_PARAMETERS_HANDLE;

// ******************************************************************
// * XPP_DEVICE_TYPE
// ******************************************************************
typedef struct _XPP_DEVICE_TYPE
{
	ULONG CurrentConnected;
	ULONG ChangeConnected;
	ULONG PreviousConnected;
}
XPP_DEVICE_TYPE, *PXPP_DEVICE_TYPE;

// ******************************************************************
// * XDEVICE_PREALLOC_TYPE
// ******************************************************************
typedef struct _XDEVICE_PREALLOC_TYPE 
{
    PXPP_DEVICE_TYPE DeviceType;
    DWORD            dwPreallocCount;
} 
XDEVICE_PREALLOC_TYPE, *PXDEVICE_PREALLOC_TYPE;

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
// * offsets into analog button array
// ******************************************************************
#define XB_XINPUT_GAMEPAD_A                0
#define XB_XINPUT_GAMEPAD_B                1
#define XB_XINPUT_GAMEPAD_X                2
#define XB_XINPUT_GAMEPAD_Y                3
#define XB_XINPUT_GAMEPAD_BLACK            4
#define XB_XINPUT_GAMEPAD_WHITE            5
#define XB_XINPUT_GAMEPAD_LEFT_TRIGGER     6
#define XB_XINPUT_GAMEPAD_RIGHT_TRIGGER    7

// ******************************************************************
// * masks for digital buttons
// ******************************************************************
#define XB_XINPUT_GAMEPAD_DPAD_UP          0x00000001
#define XB_XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
#define XB_XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
#define XB_XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
#define XB_XINPUT_GAMEPAD_START            0x00000010
#define XB_XINPUT_GAMEPAD_BACK             0x00000020
#define XB_XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
#define XB_XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080

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
// * XBGAMEPAD
// ******************************************************************
struct XBGAMEPAD : public XINPUT_GAMEPAD
{
	FLOAT      fX1;
	FLOAT      fY1;
	FLOAT      fX2;
	FLOAT      fY2;
	WORD       wLastButtons;
	BOOL       bLastAnalogButtons[8];
	WORD       wPressedButtons;
	BOOL       bPressedAnalogButtons[8];
	XINPUT_RUMBLE	Rumble;
	XINPUT_FEEDBACK	Feedback;
	XINPUT_CAPABILITIES caps;
	HANDLE     hDevice;
	BOOL       bInserted;
	BOOL       bRemoved;
};

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
// * XTHREAD_NOTIFY_PROC
// ******************************************************************
typedef VOID (WINAPI *XTHREAD_NOTIFY_PROC)(BOOL fCreate);

// ******************************************************************
// * XTHREAD_NOTIFICATION
// ******************************************************************
typedef struct _XTHREAD_NOTIFICATION
{
    LIST_ENTRY          Reserved;
    XTHREAD_NOTIFY_PROC pfnNotifyRoutine;
}
XTHREAD_NOTIFICATION, *PXTHREAD_NOTIFICATION;

// ******************************************************************
// * XCALCSIG_SIGNATURE
// ******************************************************************
#define XCALCSIG_SIGNATURE_SIZE         20

typedef struct _XCALCSIG_SIGNATURE
{
    BYTE    Signature[XCALCSIG_SIGNATURE_SIZE];
}
XCALCSIG_SIGNATURE, *PXCALCSIG_SIGNATURE;

#define XCALCSIG_FLAG_NON_ROAMABLE  (0x00000001)

// ******************************************************************
// * LAUNCH_DATA
// ******************************************************************
#define MAX_LAUNCH_DATA_SIZE	1024 * 3

typedef struct _LAUNCH_DATA 
{
	BYTE	Data[MAX_LAUNCH_DATA_SIZE];
} 
LAUNCH_DATA, *PLAUNCH_DATA;

#define LDT_TITLE                 0
#define LDT_FROM_DASHBOARD        2
#define LDT_FROM_DEBUGGER_CMDLINE 3
#define LDT_FROM_UPDATE           4

// ******************************************************************
// * XGetDeviceEnumerationStatus flags
// ******************************************************************
#define XDEVICE_ENUMERATION_IDLE 0
#define XDEVICE_ENUMERATION_BUSY 1

// ******************************************************************
// * macro: EMUPATCH - marker on patches on original Xbox functions
// ******************************************************************
#define EMUPATCH(Name) EmuPatch_##Name

// ******************************************************************
// * patch: XFormatUtilityDrive
// ******************************************************************
BOOL WINAPI EMUPATCH(XFormatUtilityDrive)();

#if 0 // Handled by ExQueryNonVolatileSetting(XC_MAX_OS) returning XBOX_USER_SETTINGS
// ******************************************************************
// * patch: GetTimeZoneInformation
// ******************************************************************
DWORD WINAPI EMUPATCH(GetTimeZoneInformation)
(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
);
#endif

// ******************************************************************
// * patch: XMountUtilityDrive
// ******************************************************************
BOOL WINAPI EMUPATCH(XMountUtilityDrive)
(
    BOOL    fFormatClean
);

// ******************************************************************
// * patch: XInitDevices
// ******************************************************************
VOID WINAPI EMUPATCH(XInitDevices)
(
    DWORD					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
);

// ******************************************************************
// * patch: XGetDevices
// ******************************************************************
DWORD WINAPI EMUPATCH(XGetDevices)
(
    XPP_DEVICE_TYPE *DeviceType
);

// ******************************************************************
// * patch: XGetDeviceChanges
// ******************************************************************
BOOL WINAPI EMUPATCH(XGetDeviceChanges)
(
    PXPP_DEVICE_TYPE DeviceType,
    PDWORD           pdwInsertions,
    PDWORD           pdwRemovals
);

// ******************************************************************
// * patch: XInputOpen
// ******************************************************************
HANDLE WINAPI EMUPATCH(XInputOpen)
(
    IN PXPP_DEVICE_TYPE             DeviceType,
    IN DWORD                        dwPort,
    IN DWORD                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
);

// ******************************************************************
// * patch: XInputClose
// ******************************************************************
VOID WINAPI EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
);

// ******************************************************************
// * patch: XInputPoll
// ******************************************************************
DWORD WINAPI EMUPATCH(XInputPoll)
(
    IN HANDLE Device
);

// ******************************************************************
// * patch: XInputGetCapabilities
// ******************************************************************
DWORD WINAPI EMUPATCH(XInputGetCapabilities)
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
);

// ******************************************************************
// * patch: XInputGetState
// ******************************************************************
DWORD WINAPI EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
);

// ******************************************************************
// * patch: XInputSetState
// ******************************************************************
DWORD WINAPI EMUPATCH(XInputSetState)
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
);


// ******************************************************************
// * patch: CreateMutex
// ******************************************************************
HANDLE WINAPI EMUPATCH(CreateMutex)
(
    LPSECURITY_ATTRIBUTES   lpMutexAttributes,
    BOOL                    bInitialOwner,
    LPCSTR                  lpName
);

// ******************************************************************
// * patch: CloseHandle
// ******************************************************************
BOOL WINAPI EMUPATCH(CloseHandle)
(
    HANDLE hObject
);

// ******************************************************************
// * patch: SetThreadPriority
// ******************************************************************
BOOL WINAPI EMUPATCH(SetThreadPriority)
(
    HANDLE  hThread,
    int     nPriority
);

// ******************************************************************
// * patch: GetThreadPriority
// ******************************************************************
int WINAPI EMUPATCH(GetThreadPriority)
(
    HANDLE  hThread
);

// ******************************************************************
// * patch: SetThreadPriorityBoost
// ******************************************************************
BOOL WINAPI EMUPATCH(SetThreadPriorityBoost)
(
    HANDLE  hThread,
    BOOL    DisablePriorityBoost
);

// ******************************************************************
// * patch: GetExitCodeThread
// ******************************************************************
BOOL WINAPI EMUPATCH(GetExitCodeThread)
(
    HANDLE  hThread,
    LPDWORD lpExitCode
);

// ******************************************************************
// * patch: XapiThreadStartup
// ******************************************************************
VOID WINAPI EMUPATCH(XapiThreadStartup)
(
    DWORD dwDummy1,
    DWORD dwDummy2
);

/* Too High Level!
// ******************************************************************
// * patch: XapiSetupPerTitleDriveLetters
// ******************************************************************
NTSTATUS CDECL XapiSetupPerTitleDriveLetters(DWORD dwTitleId, LPCWSTR wszTitleName);
*/

// ******************************************************************
// * patch: XRegisterThreadNotifyRoutine
// ******************************************************************
VOID WINAPI EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
);

#if 0 // patch disabled
// ******************************************************************
// * patch: CreateFiber
// ******************************************************************
LPVOID WINAPI EMUPATCH(CreateFiber)
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
);
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: DeleteFiber
// ******************************************************************
VOID WINAPI EMUPATCH(DeleteFiber)
(
	LPVOID lpFiber
);
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: SwitchToFiber
// ******************************************************************
VOID WINAPI EMUPATCH(SwitchToFiber)
(
	LPVOID lpFiber 
);
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: ConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
);
#endif

#if 0 // patch disabled
// ******************************************************************
// * patch: XapiFiberStartup
// ******************************************************************
VOID WINAPI EMUPATCH(XapiFiberStartup)(DWORD dwDummy);
#endif

// ******************************************************************
// * patch: QueueUserAPC
// ******************************************************************
DWORD WINAPI EMUPATCH(QueueUserAPC)
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	DWORD   	dwData
);

#if 0 // Handled by WaitForSingleObject
// ******************************************************************
// * patch: GetOverlappedResult
// ******************************************************************
BOOL WINAPI EMUPATCH(GetOverlappedResult)
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	BOOL			bWait
);
#endif

// ******************************************************************
// * patch: XLaunchNewImageA
// ******************************************************************
DWORD WINAPI EMUPATCH(XLaunchNewImageA)
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
);

#if 0 // patch disabled
// ******************************************************************
// * patch: XGetLaunchInfo
// ******************************************************************
DWORD WINAPI EMUPATCH(XGetLaunchInfo)
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
);
#endif

// ******************************************************************
// * patch: XSetProcessQuantumLength
// ******************************************************************
VOID WINAPI EMUPATCH(XSetProcessQuantumLength)
(
    DWORD dwMilliseconds
);

// ******************************************************************
// * patch: SignalObjectAndWait
// ******************************************************************
DWORD WINAPI EMUPATCH(SignalObjectAndWait)
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	DWORD	dwMilliseconds,
	BOOL	bAlertable
);

// ******************************************************************
// * patch: timeSetEvent
// ******************************************************************
MMRESULT WINAPI EMUPATCH(timeSetEvent)
(
	UINT			uDelay,
	UINT			uResolution,
	LPTIMECALLBACK	fptc,
	DWORD			dwUser,
	UINT			fuEvent
);

// ******************************************************************
// * patch: timeKillEvent
// ******************************************************************
MMRESULT WINAPI EMUPATCH(timeKillEvent)
(
	UINT uTimerID  
);

// ******************************************************************
// * patch: RaiseException
// ******************************************************************
VOID WINAPI EMUPATCH(RaiseException)
(
	DWORD			dwExceptionCode,       // exception code
	DWORD			dwExceptionFlags,      // continuable exception flag
	DWORD			nNumberOfArguments,    // number of arguments
	CONST ULONG_PTR *lpArguments		   // array of arguments
);

// ******************************************************************
// * patch: lstrcmpiW
// ******************************************************************
int WINAPI EMUPATCH(lstrcmpiW)
(
	LPCWSTR lpString1,
	LPCWSTR lpString2
);

// ******************************************************************
// * patch: XMountMUA
// ******************************************************************
DWORD WINAPI EMUPATCH(XMountMUA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
);

// ******************************************************************
// * patch: XMountMURootA
// ******************************************************************
DWORD WINAPI EMUPATCH(XMountMURootA)
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
);

// ******************************************************************
// * patch: XMountAlternateTitleA
// ******************************************************************
DWORD WINAPI EMUPATCH(XMountAlternateTitleA)
(
	LPCSTR		lpRootPath,               
	DWORD		dwAltTitleId,               
	PCHAR		pchDrive               
);

// ******************************************************************
// * patch: XUnmountAlternateTitleA
// ******************************************************************
DWORD WINAPI EMUPATCH(XUnmountAlternateTitleA)(CHAR chDrive);

// ******************************************************************
// * patch: MoveFileA
// ******************************************************************
BOOL WINAPI EMUPATCH(MoveFileA)
(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
);

// ******************************************************************
// * patch: XGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI EMUPATCH(XGetDeviceEnumerationStatus)();

// ******************************************************************
// * patch: SwitchToThread
// ******************************************************************
BOOL WINAPI EMUPATCH(SwitchToThread)();

// ******************************************************************
// * patch: XInputGetDeviceDescription
// ******************************************************************
DWORD WINAPI EMUPATCH(XInputGetDeviceDescription)
(
    HANDLE	hDevice,
    PVOID	pDescription
);

// ******************************************************************
// * patch: ReadFileEx
// ******************************************************************
BOOL WINAPI EMUPATCH(ReadFileEx)
(
	HANDLE hFile,                                       // handle to file
	LPVOID lpBuffer,                                    // data buffer
	DWORD nNumberOfBytesToRead,                         // number of bytes to read
	LPOVERLAPPED lpOverlapped,                          // offset
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// ******************************************************************
// * patch: WriteFileEx
// ******************************************************************
BOOL WINAPI EMUPATCH(WriteFileEx)
(
	HANDLE hFile,                                       // handle to output file
	LPCVOID lpBuffer,                                   // data buffer
	DWORD nNumberOfBytesToWrite,                        // number of bytes to write
	LPOVERLAPPED lpOverlapped,                          // overlapped buffer
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// ******************************************************************
// * patch: OutputDebugStringA
// ******************************************************************
VOID WINAPI EMUPATCH(OutputDebugStringA)
(
	IN LPCSTR lpOutputString
);

// s+
/* not necessary?
// ******************************************************************
// * patch: XCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EMUPATCH(XCalculateSignatureBegin)
(
    DWORD dwFlags
);

// ******************************************************************
// * patch: XCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EMUPATCH(XCalculateSignatureBeginEx)
(
    DWORD dwFlags,
    DWORD dwAltTitleId
);

// ******************************************************************
// * patch: XCalculateSignatureUpdate
// ******************************************************************
DWORD WINAPI EMUPATCH(XCalculateSignatureUpdate)
(
  HANDLE        hCalcSig,
  const BYTE    *pbData,
  ULONG         cbData
);

// ******************************************************************
// * patch: XCalculateSignatureEnd
// ******************************************************************
DWORD WINAPI EMUPATCH(XCalculateSignatureEnd)
(
  HANDLE                hCalcSig,
  PXCALCSIG_SIGNATURE   pSignature
);
//*/
// +s

#endif
