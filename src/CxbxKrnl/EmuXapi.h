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
// * pfRtlCreateHeap
// ******************************************************************
typedef PVOID (WINAPI *pfRtlCreateHeap)
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN PVOID   Lock OPTIONAL,
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
    ULONG Reserved[3];
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
// * func: EmuXFormatUtilityDrive
// ******************************************************************
BOOL WINAPI EmuXFormatUtilityDrive();

// ******************************************************************
// * func: EmuGetTimeZoneInformation
// ******************************************************************
DWORD WINAPI EmuGetTimeZoneInformation
(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
);

// ******************************************************************
// * func: EmuRtlCreateHeap
// ******************************************************************
PVOID WINAPI EmuRtlCreateHeap
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN PVOID   Lock OPTIONAL,
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
// * func: EmuRtlReAllocateHeap
// ******************************************************************
PVOID WINAPI EmuRtlReAllocateHeap
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem,
    IN SIZE_T dwBytes
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
    DWORD					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
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
// * func: EmuXInputPoll
// ******************************************************************
DWORD WINAPI EmuXInputPoll
(
    IN HANDLE Device
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
// * func: EmuXInputSetState
// ******************************************************************
DWORD WINAPI EmuXInputSetState
(
    IN     HANDLE           hDevice,
    IN OUT PXINPUT_FEEDBACK pFeedback
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
// * func: EmuGetThreadPriority
// ******************************************************************
int WINAPI EmuGetThreadPriority
(
    HANDLE  hThread
);

// ******************************************************************
// * func: EmuSetThreadPriorityBoost
// ******************************************************************
BOOL WINAPI EmuSetThreadPriorityBoost
(
    HANDLE  hThread,
    BOOL    DisablePriorityBoost
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
// * func: EmuXRegisterThreadNotifyRoutine
// ******************************************************************
VOID WINAPI EmuXRegisterThreadNotifyRoutine
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
);

// ******************************************************************
// * func: EmuCreateFiber
// ******************************************************************
LPVOID WINAPI EmuCreateFiber
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
);

// ******************************************************************
// * func: EmuDeleteFiber
// ******************************************************************
VOID WINAPI EmuDeleteFiber
(
	LPVOID lpFiber
);

// ******************************************************************
// * func: EmuSwitchToFiber
// ******************************************************************
VOID WINAPI EmuSwitchToFiber
(
	LPVOID lpFiber 
);

// ******************************************************************
// * func: EmuConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI EmuConvertThreadToFiber
(
	LPVOID lpParameter
);

// ******************************************************************
// * func: EmuXapiFiberStartup
// ******************************************************************
VOID WINAPI EmuXapiFiberStartup(DWORD dwDummy);

// ******************************************************************
// * func: EmuRtlDestroyHeap
// ******************************************************************
PVOID WINAPI EmuRtlDestroyHeap
(
    IN HANDLE HeapHandle
);

// ******************************************************************
// * func: EmuQueueUserAPC
// ******************************************************************
DWORD WINAPI EmuQueueUserAPC
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	DWORD   	dwData
);

// ******************************************************************
// * func: EmuGetOverlappedResult
// ******************************************************************
BOOL WINAPI EmuGetOverlappedResult
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	BOOL			bWait
);

// ******************************************************************
// * func: EmuXLaunchNewImage
// ******************************************************************
DWORD WINAPI EmuXLaunchNewImage
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
);

// ******************************************************************
// * func: EmuXGetLaunchInfo
// ******************************************************************
DWORD WINAPI EmuXGetLaunchInfo
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
);

// ******************************************************************
// * func: EmuXSetProcessQuantumLength
// ******************************************************************
VOID WINAPI EmuXSetProcessQuantumLength
(
    DWORD dwMilliseconds
);

// ******************************************************************
// * func: EmuXGetFileCacheSize
// ******************************************************************
DWORD WINAPI EmuXGetFileCacheSize();

// ******************************************************************
// * func: EmuSignalObjectAndWait
// ******************************************************************
DWORD WINAPI EmuSignalObjectAndWait
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	DWORD	dwMilliseconds,
	BOOL	bAlertable
);

// ******************************************************************
// * func: EmuPulseEvent
// ******************************************************************
BOOL WINAPI EmuPulseEvent( HANDLE hEvent );

// ******************************************************************
// * func: EmuCreateSemaphore
// ******************************************************************
HANDLE WINAPI EmuCreateSemaphore
(
	LPVOID	lpSemaphoreAttributes, 
	LONG	lInitialCount,
	LONG	lMaximumCount,
	LPSTR	lpName
);

// ******************************************************************
// * func: EmuReleaseSemaphore
// ******************************************************************
BOOL WINAPI EmuReleaseSemaphore
(
	HANDLE	hSemaphore,
	LONG	lReleaseCount,
	LPLONG	lpPreviousCount
);

// ******************************************************************
// * func: timeSetEvent
// ******************************************************************
MMRESULT WINAPI EmutimeSetEvent
(
	UINT			uDelay,
	UINT			uResolution,
	LPTIMECALLBACK	fptc,
	DWORD			dwUser,
	UINT			fuEvent
);

// ******************************************************************
// * func: timeKillEvent
// ******************************************************************
MMRESULT WINAPI EmutimeKillEvent
(
	UINT uTimerID  
);

// ******************************************************************
// * func: EmuRaiseException
// ******************************************************************
VOID WINAPI EmuRaiseException
(
	DWORD			dwExceptionCode,       // exception code
	DWORD			dwExceptionFlags,      // continuable exception flag
	DWORD			nNumberOfArguments,    // number of arguments
	CONST ULONG_PTR *lpArguments		   // array of arguments
);

// ******************************************************************
// * func: EmuGetFileAttributesA
// ******************************************************************
DWORD WINAPI EmuGetFileAttributesA
(
	LPCSTR			lpFileName    // name of file or directory
);

// ******************************************************************
// * func: EmuVirtualProtect
// ******************************************************************
BOOL WINAPI EmuVirtualProtect
(
	LPVOID	lpAddress,       // region of committed pages
	SIZE_T	dwSize,          // size of the region
	DWORD	flNewProtect,    // desired access protection
	PDWORD	lpflOldProtect   // old protection
);

// ******************************************************************
// * func: EmulstrcmpiW
// ******************************************************************
int WINAPI EmulstrcmpiW
(
	LPCWSTR lpString1,
	LPCWSTR lpString2
);

// ******************************************************************
// * func: EmuXMountMUA
// ******************************************************************
DWORD WINAPI EmuXMountMUA
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
);

// ******************************************************************
// * func: EmuXMountMURootA
// ******************************************************************
DWORD WINAPI EmuXMountMURootA
(
	DWORD dwPort,                  
	DWORD dwSlot,                  
	PCHAR pchDrive               
);

// ******************************************************************
// * func: EmuCreateWaitableTimerA
// ******************************************************************
HANDLE WINAPI EmuCreateWaitableTimerA
(
	LPVOID					lpTimerAttributes, // SD
	BOOL					bManualReset,      // reset type
	LPCSTR					lpTimerName        // object name
);

// ******************************************************************
// * func: EmuSetWaitableTimer
// ******************************************************************
BOOL WINAPI EmuSetWaitableTimer
(
	HANDLE				hTimer,                     // handle to timer
	const LARGE_INTEGER *pDueTime,					// timer due time
	LONG				lPeriod,                    // timer interval
	PTIMERAPCROUTINE	pfnCompletionRoutine,		// completion routine
	LPVOID				lpArgToCompletionRoutine,   // completion routine parameter
	BOOL				fResume                     // resume state
);

// ******************************************************************
// * func: EmuXMountAlternateTitle
// ******************************************************************
DWORD WINAPI EmuXMountAlternateTitle
(
	LPCSTR		lpRootPath,               
	DWORD		dwAltTitleId,               
	PCHAR		pchDrive               
);

// ******************************************************************
// * func: EmuXUnmountAlternateTitle
// ******************************************************************
DWORD WINAPI EmuXUnmountAlternateTitle(CHAR chDrive);

// ******************************************************************
// * func: EmuVirtualAlloc
// ******************************************************************
LPVOID WINAPI EmuVirtualAlloc
(
	LPVOID lpAddress,        
	SIZE_T dwSize,           
	DWORD flAllocationType,  
	DWORD flProtect          
);

// ******************************************************************
// * func: EmuVirtualAlloc
// ******************************************************************
BOOL WINAPI EmuVirtualFree
(
	LPVOID lpAddress,   
	SIZE_T dwSize,      
	DWORD dwFreeType    
);

// ******************************************************************
// * func: EmuMoveFileA
// ******************************************************************
BOOL WINAPI EmuMoveFileA
(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
);

// ******************************************************************
// * func: EmuXGetDeviceEnumerationStatus
// ******************************************************************
DWORD WINAPI EmuXGetDeviceEnumerationStatus();

// ******************************************************************
// * func: EmuSwitchToThread
// ******************************************************************
BOOL WINAPI EmuSwitchToThread();

// ******************************************************************
// * func: EmuXInputGetDeviceDescription
// ******************************************************************
DWORD WINAPI EmuXInputGetDeviceDescription
(
    HANDLE	hDevice,
    PVOID	pDescription
);

// ******************************************************************
// * func: EmuXAutoPowerDownResetTimer
// ******************************************************************
int WINAPI EmuXAutoPowerDownResetTimer();

// ******************************************************************
// * func: EmuReadFileEx
// ******************************************************************
BOOL WINAPI EmuReadFileEx
(
	HANDLE hFile,                                       // handle to file
	LPVOID lpBuffer,                                    // data buffer
	DWORD nNumberOfBytesToRead,                         // number of bytes to read
	LPOVERLAPPED lpOverlapped,                          // offset
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// ******************************************************************
// * func: EmuWriteFileEx
// ******************************************************************
BOOL WINAPI EmuWriteFileEx
(
	HANDLE hFile,                                       // handle to output file
	LPCVOID lpBuffer,                                   // data buffer
	DWORD nNumberOfBytesToWrite,                        // number of bytes to write
	LPOVERLAPPED lpOverlapped,                          // overlapped buffer
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// s+
/* not necessary?
// ******************************************************************
// * func: EmuXCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EmuXCalculateSignatureBegin
(
    DWORD dwFlags
);

// ******************************************************************
// * func: EmuXCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EmuXCalculateSignatureBeginEx
(
    DWORD dwFlags,
    DWORD dwAltTitleId
);

// ******************************************************************
// * func: EmuXCalculateSignatureUpdate
// ******************************************************************
DWORD WINAPI EmuXCalculateSignatureUpdate
(
  HANDLE        hCalcSig,
  const BYTE    *pbData,
  ULONG         cbData
);

// ******************************************************************
// * func: EmuXCalculateSignatureEnd
// ******************************************************************
DWORD WINAPI EmuXCalculateSignatureEnd
(
  HANDLE                hCalcSig,
  PXCALCSIG_SIGNATURE   pSignature
);
//*/
// +s

#endif
