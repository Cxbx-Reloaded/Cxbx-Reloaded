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
#ifndef XAPI_H
#define XAPI_H

#include "xbox_types.h"

namespace xbox {

// ******************************************************************
// * calling conventions
// ******************************************************************
#define WINAPI              __stdcall

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
// * XPP_DEVICE_INPUTSTATE_DESC
// ******************************************************************
typedef struct _XPP_DEVICE_INPUTSTATE_DESC
{
    unsigned char ucSize;       //size of InputState, doesn't include the dwPacketNumber, seems DWORD aligned.
    unsigned char * pInputState;//pointer to InputState
    unsigned char ucUnknown[3]; //for DWORD align
}
XPP_DEVICE_INPUTSTATE_DESC, *PXPP_DEVICE_INPUTSTATE_DESC;

// ******************************************************************
// * XPP_DEVICE_FEEDBACK_DESC
// ******************************************************************
typedef struct _XPP_DEVICE_FEEDBACK_DESC
{
    unsigned char ucSize;       //size of Feedback, not include Feedback_Header,  seems DWORD aligned.
    unsigned char * pFeedback;  //pointer to Feedback
    unsigned char ucUnknown[3]; //for DWORD align
}
XPP_DEVICE_FEEDBACK_DESC, *PXPP_DEVICE_FEEDBACK_DESC;

// ******************************************************************
// * XID_TYPE_INFORMATION
// ******************************************************************
typedef struct _XID_TYPE_INFORMATION
{
	UCHAR				ucType;
    BYTE				bRemainingHandles;
	UCHAR				ucUnknown[2];//probably for DWORD align
	PXPP_DEVICE_TYPE    XppType;//pointer to DeviceType structure.
    PXPP_DEVICE_INPUTSTATE_DESC pInputStateDesc;//pointer to InputStateDesc structure
    PXPP_DEVICE_FEEDBACK_DESC pFeedbackDesc;//pointer to FeedbackDesc structure
    DWORD *             pConstant;//always 0x0801
    void *              pFunction;//unknown function for device related process
    DWORD				dwEndZero;//last DWROD, always 0
} XID_TYPE_INFORMATION, *PXID_TYPE_INFORMATION;

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
// * XINPUT_GAMEPAD for xbox, xbox's GAMEPAD struct differs from PC's
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
// * X_SBC_GAMEPAD for xbox SteelBatalion GAMEPAD struc 
// ******************************************************************
typedef struct _X_SBC_GAMEPAD {
    WORD    wButtons[3];
    SHORT   sAimingX;
    SHORT   sAimingY;
    SHORT   sRotationLever;//maybe only high byte was used.
    SHORT   sSightChangeX;
    SHORT   sSightChangeY;
    WORD    wLeftPedal;//maybe only high byte was used.
    WORD    wMiddlePedal;//maybe only high byte was used.
    WORD    wRightPedal;//maybe only high byte was used.
    UCHAR   ucTunerDial;//low nibble, The 9 o'clock postion is 0, and the 6 o'clock position is 12. The blank area between the 6 and 9 o'clock positions is 13, 14, and 15 clockwise.
    UCHAR   ucGearLever;//GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
}
X_SBC_GAMEPAD, *PX_SBC_GAMEPAD;

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
#include "AlignPrefix1.h"
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
#include "AlignPosfix1.h"
XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

// ******************************************************************
// * Device XBOX Input Device Types 
// ******************************************************************
// all game controller use 0x01 GAMEPAD device type. then specify the subtype in returned Capabilities when XInputGetCapabilities called.
#define XINPUT_DEVTYPE_GAMEPAD              0x01
// SteelBatalion controller is the only one with special device type other than 1.
#define XINPUT_DEVTYPE_STEELBATALION        0x80

// ******************************************************************
// * Device XBOX Input Device SubTypes, for use in XINPUT_CAPABILITIES
// ******************************************************************
//general GAMEPAD uses subtype 0x01.
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD              0x01
//SteelBatallion controller uses subtype 0x02
#define XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT          0x02
#define XINPUT_DEVSUBTYPE_GC_WHEEL                0x10
#define XINPUT_DEVSUBTYPE_GC_ARCADE_STICK         0x20
#define XINPUT_DEVSUBTYPE_GC_DIGITAL_ARCADE_STICK 0x21
#define XINPUT_DEVSUBTYPE_GC_FLIGHT_STICK         0x30
#define XINPUT_DEVSUBTYPE_GC_SNOWBOARD            0x40
#define XINPUT_DEVSUBTYPE_GC_LIGHTGUN             0x50
#define XINPUT_DEVSUBTYPE_GC_RADIO_FLIGHT_CONTROL 0x60
#define XINPUT_DEVSUBTYPE_GC_FISHING_ROD          0x70
#define XINPUT_DEVSUBTYPE_GC_DANCEPAD             0x80

// ******************************************************************
// * XINPUT_STATE for xbox, xbox uses different Gamepad struce.
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
#include "AlignPrefix1.h"
typedef struct _XINPUT_FEEDBACK_HEADER
{
    DWORD             dwStatus;
    HANDLE OPTIONAL   hEvent;
    BYTE              Unknown1[4];
    PVOID             IoCompletedEvent; // PKEVENT really
    BYTE              Unknown2[50];
}
#include "AlignPosfix1.h"
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
typedef xbox::void_t (WINAPI *XTHREAD_NOTIFY_PROC)(BOOL fCreate);

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
xbox::void_t WINAPI EMUPATCH(XInitDevices)
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
xbox::void_t WINAPI EMUPATCH(XInputClose)
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
xbox::void_t WINAPI EMUPATCH(XapiThreadStartup)
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
xbox::void_t WINAPI EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    BOOL                    fRegister
);

// ******************************************************************
// * patch: CreateFiber
// ******************************************************************
LPVOID WINAPI EMUPATCH(CreateFiber)
(
	DWORD					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
);

// ******************************************************************
// * patch: DeleteFiber
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(DeleteFiber)
(
	LPVOID lpFiber
);

// ******************************************************************
// * patch: SwitchToFiber
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(SwitchToFiber)
(
	LPVOID lpFiber 
);

// ******************************************************************
// * patch: ConvertThreadToFiber
// ******************************************************************
LPVOID WINAPI EMUPATCH(ConvertThreadToFiber)
(
	LPVOID lpParameter
);

// ******************************************************************
// * patch: XapiFiberStartup
// ******************************************************************
xbox::void_t WINAPI EMUPATCH(XapiFiberStartup)(DWORD dwDummy);

// ******************************************************************
// * patch: QueryPerformanceCounter
// ******************************************************************
BOOL WINAPI EMUPATCH(QueryPerformanceCounter)
(
	LARGE_INTEGER *lpPerformanceCount
);

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
xbox::void_t WINAPI EMUPATCH(XSetProcessQuantumLength)
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
xbox::void_t WINAPI EMUPATCH(RaiseException)
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
/*DWORD WINAPI EMUPATCH(XMountAlternateTitleA)
(
	LPCSTR lpRootPath,
	DWORD  dwAltTitleId,
	PCHAR  pchDrive
);*/

// ******************************************************************
// * patch: XUnmountAlternateTitleA
// ******************************************************************
//DWORD WINAPI EMUPATCH(XUnmountAlternateTitleA)(CHAR chDrive);

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
xbox::void_t WINAPI EMUPATCH(OutputDebugStringA)
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

} // end of namespace xbox

#endif
