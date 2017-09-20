// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;	 .,;;  ;;;'';;'   `;;;	 .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o	   oP"``"Yo	 _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->XAPI.OOVPA.inl
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license	or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not	write to the Free Software Foundation	Inc.,
// *  59 Temple Place - Suite 330	Bostom	MA 02111-1307	USA.
// *
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// TODO: Known Xapi OOVPA issue list
// * 4034 is not verified each OOVPA.
// * Following OOVPA revision are not verified
//   * XMountMUA (3950)
//   * XMountMURootA (3950, 4039, 5028, 5120, 5233)
//   * timeKillEvent (3950, 5028, 5233)
//   * timeSetEvent (5233)
//   * SignalObjectAndWait (4039)
//   * QueueUserAPC (4039)
//   * XMountAlternateTitleA (4039)
//   * XInputPoll (4039)
//   * XInputGetDeviceDescription (4928, 5028, 5120, 5233, 5455)
//   * ConvertThreadToFiber (4039, 4134, 4531, 4721, 5028, 5455)
//   * CreateFiber (4039, 4134, 4531, 4721, 5028, 5455)
//   * DeleteFiber (4039, 4134, 4531, 4721, 5028, 5455)
//   * SwitchToFiber (4039, 4134, 4531, 4721, 5028, 5455)
//   * XapiFiberStartup (4039, 4134, 4531, 4721, 5028, 5455)
// * Following OOVPA are invalid
//   * ReadFileEx (3911) // Has exact asm codes _WriteFileEx@20
//   * WriteFileEx (3911) // Has exact asm codes _ReadFileEx@20
//   * lstrcmpiW (3911) // Necessary XREF _xCompareStringW@20
// * Following OOVPA are missing (Obsolete?)
//   * GetThreadPriorityBoost
//   * RtlAllocateHeap
//   * RtlCreateHeap
//   * RtlDestroyHeap
//   * RtlFreeHeap
//   * RtlReAllocateHeap
//   * RtlSizeHeap
//   * XCalculateSignatureBeginEx
//   * XCalculateSignatureEnd
//   * XCalculateSignatureUpdate
// NOTE: Known Xapi OOVPA not included in initial revision.
//   * XFormatUtilityDrive (4242)
//   * XSetProcessQuantumLength (4134)
//   * XInputGetDeviceDescription (4831)
//   * XGetDeviceEnumerationStatus (4831)

#ifndef XAPI_OOVPA_INL
#define XAPI_OOVPA_INL

#include "../OOVPA.h"

#include "Xapi.1.0.3911.inl"
#include "Xapi.1.0.3950.inl"
#include "Xapi.1.0.4034.inl"
#include "Xapi.1.0.4039.inl"
#include "Xapi.1.0.4134.inl"
#include "Xapi.1.0.4242.inl"
#include "Xapi.1.0.4361.inl"
#include "Xapi.1.0.4432.inl"
#include "Xapi.1.0.4627.inl"
#include "Xapi.1.0.4721.inl"
#include "Xapi.1.0.4831.inl"
#include "Xapi.1.0.5028.inl"
#include "Xapi.1.0.5120.inl"
#include "Xapi.1.0.5233.inl"
#include "Xapi.1.0.5344.inl"
#include "Xapi.1.0.5455.inl"
#include "Xapi.1.0.5558.inl"
#include "Xapi.1.0.5788.inl"
#include "Xapi.1.0.5659.inl"
#include "Xapi.1.0.5849.inl"

// ******************************************************************
// * XAPILIB_OOVPA
// ******************************************************************
OOVPATable XAPILIB_OOVPAV2[] = {

	// REGISTER_OOVPAS(GetThreadPriorityBoost, PATCH, 5788),
	// REGISTER_OOVPAS(GetThreadPriorityBoost, PATCH, 5849),
	// REGISTER_OOVPAS(RtlAllocateHeap, PATCH, 3911), // obsolete (* unchanged since 1.0.4361 *) (* OR FARTHER *) (from 4721's comment)
	// REGISTER_OOVPAS(RtlCreateHeap, PATCH, 3911), // obsolete, (* unchanged since 1.0.4361 *) (* OR FARTHER *) (from 4721's comment)
	// REGISTER_OOVPAS(RtlDestroyHeap, PATCH, 4627), // obsolete (from 4721's comment)
	// REGISTER_OOVPAS(RtlFreeHeap, PATCH, 4627), // obsolete (from 4721's comment)
	// REGISTER_OOVPAS(RtlReAllocateHeap, PATCH, 4627),
	// REGISTER_OOVPAS(RtlSizeHeap, PATCH, 4627), // obsolete (from 4721's comment)
	// REGISTER_OOVPAS(XCalculateSignatureBegin, PATCH, 4627),
	// REGISTER_OOVPAS(XCalculateSignatureBeginEx, PATCH, 4627), // +s, not necessary? (from 4627, 5028's comment)
	// REGISTER_OOVPAS(XCalculateSignatureEnd, PATCH, 4627), // s+ (from 4627, 5028's comment)
	// REGISTER_OOVPAS(XCalculateSignatureUpdate, PATCH, 4627),
	// REGISTER_OOVPAS(ReadFileEx, DISABLED, 3911),// Has exact asm codes _WriteFileEx@20
	// REGISTER_OOVPAS(WriteFileEx, DISABLED, 3911), // Has exact asm codes _ReadFileEx@20
	// REGISTER_OOVPAS(lstrcmpiW, DISABLED, 3911), // TODO: necessary XREF _xCompareStringW@20
	REGISTER_OOVPAS(CloseHandle, DISABLED, 3911),
	REGISTER_OOVPAS(CreateMutex, DISABLED, 3911), // Too High Level (from 3911's comment)
	REGISTER_OOVPAS(CreateThread, DISABLED, 3911), // Too High Level (from 3911's comment)
	REGISTER_OOVPAS(ExitThread, DISABLED, 3911),//
	REGISTER_OOVPAS(MoveFileA, DISABLED, 3911),
	REGISTER_OOVPAS(SwitchToThread, DISABLED, 3911),
	REGISTER_OOVPAS(XCalculateSignatureBegin, DISABLED, 3911, 4039),
	REGISTER_OOVPAS(XapiBootDash, DISABLED, 3911), // obsolete (from 4721's comment)
	REGISTER_OOVPAS(XapiInitProcess, DISABLED, 3911, 3950, 4242, 4831, 5028), // obsolete, Too High Level (from 4721's comment)
	REGISTER_OOVPAS(XapiThreadStartup, DISABLED, 3911), // obsolete? (from 4627, 5028, 5558, 5788, 5849's comment) // obsolete (from 4721's comment)
	REGISTER_OOVPAS(ConvertThreadToFiber, DISABLED, 3911),
	REGISTER_OOVPAS(CreateFiber, DISABLED, 3911),
	REGISTER_OOVPAS(DeleteFiber, DISABLED, 3911),
	REGISTER_OOVPAS(GetExitCodeThread, PATCH, 3911),
	REGISTER_OOVPAS(GetOverlappedResult, PATCH, 3911),
	REGISTER_OOVPAS(GetThreadPriority, PATCH, 3911, 5788),
	REGISTER_OOVPAS(GetTimeZoneInformation, DISABLED, 3911),
	REGISTER_OOVPAS(OutputDebugStringA, PATCH, 3911),
	REGISTER_OOVPAS(QueueUserAPC, PATCH, 3911),
	REGISTER_OOVPAS(RaiseException, PATCH, 3911),
	REGISTER_OOVPAS(SetThreadPriority, PATCH, 3911),
	REGISTER_OOVPAS(SetThreadPriorityBoost, PATCH, 3911, 5788),
	REGISTER_OOVPAS(SignalObjectAndWait, PATCH, 3911),
	REGISTER_OOVPAS(SwitchToFiber, DISABLED, 3911),
	REGISTER_OOVPAS(XAutoPowerDownResetTimer, DISABLED, 3911), // Just calls KeSetTimer (from 3911's comment)
	REGISTER_OOVPAS(XFormatUtilityDrive, PATCH, 4242),
	REGISTER_OOVPAS(XGetDeviceChanges, PATCH, 3911, 5233), // Was PATCH (from 5788's comment)
	REGISTER_OOVPAS(XGetDeviceEnumerationStatus, PATCH, 4831),
	REGISTER_OOVPAS(XGetDevices, PATCH, 3911),
	REGISTER_OOVPAS(XGetLaunchInfo, DISABLED, 3911),
	REGISTER_OOVPAS(XID_fCloseDevice, XREF, 3911, 4242, 4627, 4831, 5455),
	REGISTER_OOVPAS(XInitDevices, PATCH, 3911, 5120),
	REGISTER_OOVPAS(XInputClose, PATCH, 3911, 5455),
	REGISTER_OOVPAS(XInputGetCapabilities, PATCH, 3911, 4242, 4831, 5233, 5455),
	REGISTER_OOVPAS(XInputGetDeviceDescription, PATCH, 4831),
	REGISTER_OOVPAS(XInputGetState, PATCH, 3911, 4134, 4242, 4831, 5455),
	REGISTER_OOVPAS(XInputOpen, PATCH, 3911, 4134, 4242),
	REGISTER_OOVPAS(XInputPoll, PATCH, 3911),
	REGISTER_OOVPAS(XInputSetState, PATCH, 3911, 4242, 4831, 5233),
	REGISTER_OOVPAS(XLaunchNewImageA, PATCH, 3911, 4721, 5344, 5558),
	REGISTER_OOVPAS(XMountAlternateTitleA, PATCH, 3911, 5028, 5455),
	REGISTER_OOVPAS(XMountMUA, PATCH, 3911, 4242),
	REGISTER_OOVPAS(XMountMURootA, PATCH, 3911, 4242),
	REGISTER_OOVPAS(XMountUtilityDrive, PATCH, 3911, 4134, 4432), // TODO: This needs to be verified on 4361, not just 4242! (from 4361's comment)
	REGISTER_OOVPAS(XRegisterThreadNotifyRoutine, PATCH, 3911),
	REGISTER_OOVPAS(XSetProcessQuantumLength, PATCH, 4134),
	REGISTER_OOVPAS(XUnmountAlternateTitleA, PATCH, 3911),
	REGISTER_OOVPAS(XapiFiberStartup, DISABLED, 3911),
	REGISTER_OOVPAS(timeKillEvent, PATCH, 3911, 5849),
	REGISTER_OOVPAS(timeSetEvent, PATCH, 3911, 5455),
};

// ******************************************************************
// * XAPILIB_OOVPA_SIZE
// ******************************************************************
uint32 XAPILIB_OOVPA_SIZEV2 = sizeof(XAPILIB_OOVPAV2);

#endif
