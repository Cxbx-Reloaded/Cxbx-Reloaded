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
typedef xbox::dword_xt (WINAPI *PTHREAD_START_ROUTINE)
(
    LPVOID lpThreadParameter
);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

// ******************************************************************
// * XINPUT_POLLING_PARAMETERS
// ******************************************************************
typedef struct _XINPUT_POLLING_PARAMETERS
{
    xbox::byte_xt       fAutoPoll        : 1;
    xbox::byte_xt       fInterruptOut    : 1;
    xbox::byte_xt       ReservedMBZ1     : 6;
    xbox::byte_xt       bInputInterval;
    xbox::byte_xt       bOutputInterval;
    xbox::byte_xt       ReservedMBZ2;
}
XINPUT_POLLING_PARAMETERS, *PXINPUT_POLLING_PARAMETERS;

// ******************************************************************
// * XPP_DEVICE_TYPE
// ******************************************************************
typedef struct _XPP_DEVICE_TYPE
{
	xbox::ulong_xt CurrentConnected;
	xbox::ulong_xt ChangeConnected;
	xbox::ulong_xt PreviousConnected;
}
XPP_DEVICE_TYPE, *PXPP_DEVICE_TYPE;

// ******************************************************************
// * XPP_DEVICE_INPUTSTATE_DESC
// ******************************************************************
typedef struct _XPP_DEVICE_INPUTSTATE_DESC
{
    unsigned char ucSize;       //size of InputState, doesn't include the dwPacketNumber, seems xbox::dword_xt aligned.
    unsigned char * pInputState;//pointer to InputState
    unsigned char ucUnknown[3]; //for xbox::dword_xt align
}
XPP_DEVICE_INPUTSTATE_DESC, *PXPP_DEVICE_INPUTSTATE_DESC;

// ******************************************************************
// * XPP_DEVICE_FEEDBACK_DESC
// ******************************************************************
typedef struct _XPP_DEVICE_FEEDBACK_DESC
{
    unsigned char ucSize;       //size of Feedback, not include Feedback_Header,  seems xbox::dword_xt aligned.
    unsigned char * pFeedback;  //pointer to Feedback
    unsigned char ucUnknown[3]; //for xbox::dword_xt align
}
XPP_DEVICE_FEEDBACK_DESC, *PXPP_DEVICE_FEEDBACK_DESC;

// ******************************************************************
// * XID_TYPE_INFORMATION
// ******************************************************************
typedef struct _XID_TYPE_INFORMATION
{
	xbox::uchar_xt				ucType;
    xbox::byte_xt				bRemainingHandles;
	xbox::uchar_xt				ucUnknown[2];//probably for xbox::dword_xt align
	PXPP_DEVICE_TYPE    XppType;//pointer to DeviceType structure.
    PXPP_DEVICE_INPUTSTATE_DESC pInputStateDesc;//pointer to InputStateDesc structure
    PXPP_DEVICE_FEEDBACK_DESC pFeedbackDesc;//pointer to FeedbackDesc structure
    xbox::dword_xt *             pConstant;//always 0x0801
    void *              pFunction;//unknown function for device related process
    xbox::dword_xt				dwEndZero;//last DWROD, always 0
} XID_TYPE_INFORMATION, *PXID_TYPE_INFORMATION;

// ******************************************************************
// * XDEVICE_PREALLOC_TYPE
// ******************************************************************
typedef struct _XDEVICE_PREALLOC_TYPE 
{
    PXPP_DEVICE_TYPE DeviceType;
    xbox::dword_xt            dwPreallocCount;
} 
XDEVICE_PREALLOC_TYPE, *PXDEVICE_PREALLOC_TYPE;

// ******************************************************************
// * XINPUT_GAMEPAD for xbox, xbox's GAMEPAD struct differs from PC's
// ******************************************************************
typedef struct _XINPUT_GAMEPAD
{
    xbox::word_xt    wButtons;
    xbox::byte_xt    bAnalogButtons[8];
    xbox::short_xt   sThumbLX;
    xbox::short_xt   sThumbLY;
    xbox::short_xt   sThumbRX;
    xbox::short_xt   sThumbRY;
}
XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

// ******************************************************************
// * X_SBC_GAMEPAD for xbox SteelBattalion GAMEPAD struc 
// ******************************************************************
typedef struct _SBC_GAMEPAD {
    xbox::word_xt    wButtons[3];
    xbox::short_xt   sAimingX;
    xbox::short_xt   sAimingY;
    xbox::short_xt   sRotationLever;//maybe only high byte was used.
    xbox::short_xt   sSightChangeX;
    xbox::short_xt   sSightChangeY;
    xbox::word_xt    wLeftPedal;//maybe only high byte was used.
    xbox::word_xt    wMiddlePedal;//maybe only high byte was used.
    xbox::word_xt    wRightPedal;//maybe only high byte was used.
    xbox::uchar_xt   ucTunerDial;//low nibble, The 9 o'clock postion is 0, and the 6 o'clock position is 12. The blank area between the 6 and 9 o'clock positions is 13, 14, and 15 clockwise.
    xbox::uchar_xt   ucGearLever;//GearLever 1~5 for gear 1~5, 7~13 for gear R,N,1~5, 15 for gear R. we use the continues range from 7~13
}
SBC_GAMEPAD, *PSBC_GAMEPAD;

// ergo720: from the description of the above gear levers, the gear to use is probably interpreted by looking at the first 3 bits of ucGearLever.
// Remove this when this theory is confirmed to be true or false
/*
0000 0000  -> should be N ??
0000 0001  1
0000 0010  2
0000 0011  3
0000 0100  4
0000 0101  5
0000 0110
0000 0111  R
0000 1000  N
0000 1001  1
0000 1010  2
0000 1011  3
0000 1100  4
0000 1101  5
0000 1110
0000 1111  R
*/

// ******************************************************************
// * XINPUT_RUMBLE
// ******************************************************************
typedef struct _XINPUT_RUMBLE
{
    xbox::word_xt   wLeftMotorSpeed;
    xbox::word_xt   wRightMotorSpeed;
}
XINPUT_RUMBLE, *PXINPUT_RUMBLE;

// ******************************************************************
// * XINPUT_CAPABILITIES
// ******************************************************************
#include "AlignPrefix1.h"
typedef struct _XINPUT_CAPABILITIES
{
    xbox::byte_xt SubType;
    xbox::word_xt Reserved;

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
// SteelBattalion controller is the only one with special device type other than 1.
#define XINPUT_DEVTYPE_STEELBATTALION        0x80

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
    xbox::dword_xt dwPacketNumber;

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
    xbox::dword_xt             dwStatus;
    HANDLE OPTIONAL   hEvent;
    xbox::byte_xt              Unknown1[4];
    PVOID             IoCompletedEvent; // PKEVENT really
    xbox::byte_xt              Unknown2[50];
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
    xbox::ulong_xt   Length;
    xbox::byte_xt    Unknown[0x2C];
}
RTL_HEAP_PARAMETERS;

// ******************************************************************
// * XTHREAD_NOTIFY_PROC
// ******************************************************************
typedef xbox::void_xt (WINAPI *XTHREAD_NOTIFY_PROC)(bool_xt fCreate);

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
    xbox::byte_xt    Signature[XCALCSIG_SIGNATURE_SIZE];
}
XCALCSIG_SIGNATURE, *PXCALCSIG_SIGNATURE;

#define XCALCSIG_FLAG_NON_ROAMABLE  (0x00000001)

// ******************************************************************
// * LAUNCH_DATA
// ******************************************************************
#define MAX_LAUNCH_DATA_SIZE	1024 * 3

typedef struct _LAUNCH_DATA 
{
	xbox::byte_xt	Data[MAX_LAUNCH_DATA_SIZE];
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
xbox::bool_xt WINAPI EMUPATCH(XFormatUtilityDrive)();

#if 0 // Handled by ExQueryNonVolatileSetting(XC_MAX_OS) returning XBOX_USER_SETTINGS
// ******************************************************************
// * patch: GetTimeZoneInformation
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(GetTimeZoneInformation)
(
    OUT LPTIME_ZONE_INFORMATION lpTimeZoneInformation
);
#endif

// ******************************************************************
// * patch: XMountUtilityDrive
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(XMountUtilityDrive)
(
    bool_xt    fFormatClean
);

// ******************************************************************
// * patch: XInitDevices
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XInitDevices)
(
    dword_xt					dwPreallocTypeCount,
	PXDEVICE_PREALLOC_TYPE	PreallocTypes
);

// ******************************************************************
// * patch: XGetDevices
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XGetDevices)
(
    XPP_DEVICE_TYPE *DeviceType
);

// ******************************************************************
// * patch: XGetDeviceChanges
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(XGetDeviceChanges)
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
    IN dword_xt                        dwPort,
    IN dword_xt                        dwSlot,
    IN PXINPUT_POLLING_PARAMETERS   pPollingParameters OPTIONAL
);

// ******************************************************************
// * patch: XInputClose
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XInputClose)
(
    IN HANDLE hDevice
);

// ******************************************************************
// * patch: XInputPoll
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XInputPoll)
(
    IN HANDLE Device
);

// ******************************************************************
// * patch: XInputGetCapabilities
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XInputGetCapabilities)
(
    IN  HANDLE               hDevice,
    OUT PXINPUT_CAPABILITIES pCapabilities
);

// ******************************************************************
// * patch: XInputGetState
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XInputGetState)
(
    IN  HANDLE         hDevice,
    OUT PXINPUT_STATE  pState
);

// ******************************************************************
// * patch: XInputSetState
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XInputSetState)
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
    bool_xt                    bInitialOwner,
    LPCSTR                  lpName
);

// ******************************************************************
// * patch: CloseHandle
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(CloseHandle)
(
    HANDLE hObject
);

// ******************************************************************
// * patch: SetThreadPriority
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(SetThreadPriority)
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
xbox::bool_xt WINAPI EMUPATCH(SetThreadPriorityBoost)
(
    HANDLE  hThread,
    bool_xt    DisablePriorityBoost
);

// ******************************************************************
// * patch: GetExitCodeThread
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(GetExitCodeThread)
(
    HANDLE  hThread,
    LPDWORD lpExitCode
);

// ******************************************************************
// * patch: XapiThreadStartup
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XapiThreadStartup)
(
    dword_xt dwDummy1,
    dword_xt dwDummy2
);

/* Too High Level!
// ******************************************************************
// * patch: XapiSetupPerTitleDriveLetters
// ******************************************************************
NTSTATUS CDECL XapiSetupPerTitleDriveLetters(dword_xt dwTitleId, LPCWSTR wszTitleName);
*/

// ******************************************************************
// * patch: XRegisterThreadNotifyRoutine
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XRegisterThreadNotifyRoutine)
(
    PXTHREAD_NOTIFICATION   pThreadNotification,
    bool_xt                    fRegister
);

// ******************************************************************
// * patch: CreateFiber
// ******************************************************************
LPVOID WINAPI EMUPATCH(CreateFiber)
(
	dword_xt					dwStackSize,
	LPFIBER_START_ROUTINE	lpStartRoutine,
	LPVOID					lpParameter
);

// ******************************************************************
// * patch: DeleteFiber
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(DeleteFiber)
(
	LPVOID lpFiber
);

// ******************************************************************
// * patch: SwitchToFiber
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(SwitchToFiber)
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
xbox::void_xt WINAPI EMUPATCH(XapiFiberStartup)(dword_xt dwDummy);

// ******************************************************************
// * patch: QueryPerformanceCounter
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(QueryPerformanceCounter)
(
	LARGE_INTEGER *lpPerformanceCount
);

// ******************************************************************
// * patch: QueueUserAPC
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(QueueUserAPC)
(
	PAPCFUNC	pfnAPC,
	HANDLE		hThread,
	dword_xt   	dwData
);

#if 0 // Handled by WaitForSingleObject
// ******************************************************************
// * patch: GetOverlappedResult
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(GetOverlappedResult)
(
	HANDLE			hFile,
	LPOVERLAPPED	lpOverlapped,
	LPDWORD			lpNumberOfBytesTransferred,
	bool_xt			bWait
);
#endif

// ******************************************************************
// * patch: XLaunchNewImageA
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XLaunchNewImageA)
(
	LPCSTR			lpTitlePath,
	PLAUNCH_DATA	pLaunchData
);

#if 0 // patch disabled
// ******************************************************************
// * patch: XGetLaunchInfo
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XGetLaunchInfo)
(
	PDWORD			pdwLaunchDataType,
	PLAUNCH_DATA	pLaunchData
);
#endif

// ******************************************************************
// * patch: XSetProcessQuantumLength
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(XSetProcessQuantumLength)
(
    dword_xt dwMilliseconds
);

// ******************************************************************
// * patch: SignalObjectAndWait
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(SignalObjectAndWait)
(
	HANDLE	hObjectToSignal,
	HANDLE	hObjectToWaitOn,
	dword_xt	dwMilliseconds,
	bool_xt	bAlertable
);

// ******************************************************************
// * patch: timeSetEvent
// ******************************************************************
MMRESULT WINAPI EMUPATCH(timeSetEvent)
(
	uint_xt			uDelay,
	uint_xt			uResolution,
	LPTIMECALLBACK	fptc,
	dword_xt			dwUser,
	uint_xt			fuEvent
);

// ******************************************************************
// * patch: timeKillEvent
// ******************************************************************
MMRESULT WINAPI EMUPATCH(timeKillEvent)
(
	uint_xt uTimerID  
);

// ******************************************************************
// * patch: RaiseException
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(RaiseException)
(
	dword_xt			dwExceptionCode,       // exception code
	dword_xt			dwExceptionFlags,      // continuable exception flag
	dword_xt			nNumberOfArguments,    // number of arguments
	CONST ulong_ptr_xt *lpArguments		   // array of arguments
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
xbox::dword_xt WINAPI EMUPATCH(XMountMUA)
(
	dword_xt dwPort,
	dword_xt dwSlot,
	PCHAR pchDrive
);

// ******************************************************************
// * patch: XMountMURootA
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XMountMURootA)
(
	dword_xt dwPort,
	dword_xt dwSlot,
	PCHAR pchDrive
);

// ******************************************************************
// * patch: XMountAlternateTitleA
// ******************************************************************
/*xbox::dword_xt WINAPI EMUPATCH(XMountAlternateTitleA)
(
	LPCSTR lpRootPath,
	dword_xt  dwAltTitleId,
	PCHAR  pchDrive
);*/

// ******************************************************************
// * patch: XUnmountAlternateTitleA
// ******************************************************************
//xbox::dword_xt WINAPI EMUPATCH(XUnmountAlternateTitleA)(char_xt chDrive);

// ******************************************************************
// * patch: MoveFileA
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(MoveFileA)
(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
);

// ******************************************************************
// * patch: XGetDeviceEnumerationStatus
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XGetDeviceEnumerationStatus)();

// ******************************************************************
// * patch: SwitchToThread
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(SwitchToThread)();

// ******************************************************************
// * patch: XInputGetDeviceDescription
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XInputGetDeviceDescription)
(
    HANDLE	hDevice,
    PVOID	pDescription
);

// ******************************************************************
// * patch: ReadFileEx
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(ReadFileEx)
(
	HANDLE hFile,                                       // handle to file
	LPVOID lpBuffer,                                    // data buffer
	dword_xt nNumberOfBytesToRead,                         // number of bytes to read
	LPOVERLAPPED lpOverlapped,                          // offset
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// ******************************************************************
// * patch: WriteFileEx
// ******************************************************************
xbox::bool_xt WINAPI EMUPATCH(WriteFileEx)
(
	HANDLE hFile,                                       // handle to output file
	LPCVOID lpBuffer,                                   // data buffer
	dword_xt nNumberOfBytesToWrite,                        // number of bytes to write
	LPOVERLAPPED lpOverlapped,                          // overlapped buffer
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

// ******************************************************************
// * patch: OutputDebugStringA
// ******************************************************************
xbox::void_xt WINAPI EMUPATCH(OutputDebugStringA)
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
    dword_xt dwFlags
);

// ******************************************************************
// * patch: XCalculateSignatureBegin
// ******************************************************************
HANDLE WINAPI EMUPATCH(XCalculateSignatureBeginEx)
(
    dword_xt dwFlags,
    dword_xt dwAltTitleId
);

// ******************************************************************
// * patch: XCalculateSignatureUpdate
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XCalculateSignatureUpdate)
(
  HANDLE        hCalcSig,
  const xbox::byte_xt    *pbData,
  ulong_xt         cbData
);

// ******************************************************************
// * patch: XCalculateSignatureEnd
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XCalculateSignatureEnd)
(
  HANDLE                hCalcSig,
  PXCALCSIG_SIGNATURE   pSignature
);
//*/
// +s

} // end of namespace xbox

#endif
