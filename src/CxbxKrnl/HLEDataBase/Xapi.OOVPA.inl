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

// Titles which did compiled with full library
//   [LibV] Title Name                       |  Verify   |  Comments
//-------------------------------------------------------------------
// * [3911] Cel Damage                       |   100%    | have 38/45 library.
// * [3950] Shikigami no Shiro               |   100%    | have 35/45 library.
// * [4034] Dark Summit                      |   100%    | have 21/45 library.
// * [4039] New Legends                      |   100%    | have 30/45 library.
// * [4134] Double-S.T.E.A.L                 |   100%    | have 39/47 library.
// * [4242] NFL Blitz 2002                   |   100%    | have 46/48 library.
// * [4361] ESPN NBA 2Night 2002             |   100%    | have 33/48 library.
// * [4432] RedCard 2003                     |   100%    | have 46/48 library.
// * [4531] NHL HITZ 2003                    |   100%    | have 41/48 library.
// * [4627] MLB SlugFest 2003                |   100%    | have 46/48 library.
// * [4721] Terminator - Dawn of Fate        |   100%    | have 41/48 library.
// * [4831] Whacked                          |   100%    | have 42/50 library.
// * [4928] Drihoo                           |   100%    | have 42/50 library.
// * [5028] Shikigami no Shiro Evolution     |   100%    | have 39/50 library.
// * [5120] N.U.D.E.@                        |   100%    | have 40/50 library.
// * [5233] Evil Dead                        |   100%    | have 39/50 library.
// * [5344] Starsky & Hutch                  |   100%    | have 43/50 library.
// * [5455] NCAA Football 2004               |   100%    | have 44/50 library.
// * [5558] XIII                             |   100%    | have 42/50 library.
// * [5659] LMA Manager 2004                 |   100%    | have 42/50 library.
// * [5788] All-Star Baseball 2005           |   100%    | have 42/50 library.
// * [5849] Nickelodeon Tak 2                |   100%    | have 40/50 library.

// TODO: Known Xapi OOVPA issue list
// * Verification needed: Function Name ( Revision )
//   * ConvertThreadToFiber (3950, 4034, 4039, 4134, 4531, 4721, 5028, 5455)
//   * CreateFiber (3950, 4034, 4039, 4134, 4531, 4721, 5028, 5455)
//   * DeleteFiber (3950, 4034, 4039, 4134, 4531, 4721, 5028, 5455)
//   * ExitThread (4034)
//   * GetExitCodeThread (4034)
//   * GetOverlappedResult (4034)
//   * GetThreadPriority (4034)
//   * GetTimeZoneInformation (4034)
//   * GetTypeInformation (4134)
//   * MoveFileA (4034)
//   * QueueUserAPC (4034)
//   * SetThreadPriority (4034)
//   * SetThreadPriorityBoost (4034)
//   * SignalObjectAndWait (4034)
//   * SwitchToFiber (3950, 4034, 4039, 4134, 4531, 4721, 5028, 5455)
//   * SwitchToThread (4034)
//   * XCalculateSignatureBegin (4034)
//   * XGetDeviceChanges (4034)
//   * XInputGetCapabilities (4034)
//   * XInputGetDeviceDescription (4928, 5028, 5120, 5233, 5455)
//   * XInputPoll (4034)
//   * XMountAlternateTitleA (4034)
//   * XMountMURootA (3950, 4034, 4039, 4134, 5028, 5120, 5233)
//   * XapiFiberStartup (3950, 4034, 4039, 4134, 4531, 4721, 5028, 5455)
//   * timeKillEvent (3950, 4034, 5028, 5233)
//   * timeSetEvent (4034, 5233)
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
	// REGISTER_OOVPAS(ReadFileEx, UNPATCHED, 3911),// Has exact asm codes _WriteFileEx@20
	// REGISTER_OOVPAS(WriteFileEx, UNPATCHED, 3911), // Has exact asm codes _ReadFileEx@20
	// REGISTER_OOVPAS(lstrcmpiW, UNPATCHED, 3911), // TODO: need XREF _xCompareStringW@20 (Has exact asm codes as lstrcmpiW)
	// REGISTER_OOVPAS(CloseHandle, UNPATCHED, 3911),// Has exact same asm codes as _ResetEvent@4
	REGISTER_OOVPAS(CreateMutex, UNPATCHED, 3911), // Too High Level (from 3911's comment)
	REGISTER_OOVPAS(CreateThread, UNPATCHED, 3911), // Too High Level (from 3911's comment)
	REGISTER_OOVPAS(ExitThread, UNPATCHED, 3911),//
	REGISTER_OOVPAS(MoveFileA, UNPATCHED, 3911),
	REGISTER_OOVPAS(SwitchToThread, UNPATCHED, 3911),
	REGISTER_OOVPAS(XCalculateSignatureBegin, UNPATCHED, 3911, 4039),
	REGISTER_OOVPAS(XapiBootDash, UNPATCHED, 3911), // obsolete (from 4721's comment)
	REGISTER_OOVPAS(XapiInitProcess, UNPATCHED, 3911, 3950, 4242, 4831, 5028), // obsolete, Too High Level (from 4721's comment)
	REGISTER_OOVPAS(XapiThreadStartup, UNPATCHED, 3911), // obsolete? (from 4627, 5028, 5558, 5788, 5849's comment) // obsolete (from 4721's comment)
	REGISTER_OOVPAS(ConvertThreadToFiber, UNPATCHED, 3911),
	REGISTER_OOVPAS(CreateFiber, UNPATCHED, 3911),
	REGISTER_OOVPAS(DeleteFiber, UNPATCHED, 3911),
	REGISTER_OOVPAS(GetExitCodeThread, PATCH, 3911),
	REGISTER_OOVPAS(GetOverlappedResult, PATCH, 3911),
	REGISTER_OOVPAS(GetThreadPriority, PATCH, 3911/*, 5788*/),
	REGISTER_OOVPAS(GetTimeZoneInformation, UNPATCHED, 3911),
	REGISTER_OOVPAS(GetTypeInformation, XREF, 4134), // TODO: Actually introduced in some unknown XDK between 4134 and 4361
	REGISTER_OOVPAS(OutputDebugStringA, PATCH, 3911),
	REGISTER_OOVPAS(QueueUserAPC, PATCH, 3911),
	REGISTER_OOVPAS(RaiseException, PATCH, 3911),
	REGISTER_OOVPAS(SetThreadPriority, PATCH, 3911),
	REGISTER_OOVPAS(SetThreadPriorityBoost, PATCH, 3911/*, 5788*/),
	REGISTER_OOVPAS(SignalObjectAndWait, PATCH, 3911),
	REGISTER_OOVPAS(SwitchToFiber, UNPATCHED, 3911),
	REGISTER_OOVPAS(XAutoPowerDownResetTimer, UNPATCHED, 3911), // Just calls KeSetTimer (from 3911's comment)
	REGISTER_OOVPAS(XFormatUtilityDrive, PATCH, 4242),
	REGISTER_OOVPAS(XGetDeviceChanges, PATCH, 3911/*, 5233*/), // Was PATCH (from 5788's comment)
	REGISTER_OOVPAS(XGetDeviceEnumerationStatus, PATCH, 4831),
	REGISTER_OOVPAS(XGetDevices, PATCH, 3911),
	REGISTER_OOVPAS(XGetLaunchInfo, UNPATCHED, 3911),
	REGISTER_OOVPAS(XID_fCloseDevice, XREF, 3911/*, 4242, 4627*/, 4831/*, 5455*/),
	REGISTER_OOVPAS(XInitDevices, PATCH, 3911, 5120),
	REGISTER_OOVPAS(XInputClose, PATCH, 3911/*, 5455*/),
	REGISTER_OOVPAS(XInputGetCapabilities, PATCH, 3911/*, 4242*/, 4831/*, 5233, 5455*/),
	REGISTER_OOVPAS(XInputGetDeviceDescription, PATCH, 4831),
	REGISTER_OOVPAS(XInputGetState, PATCH, 3911/*, 4134*/, 4242/*, 4831*/, 5455),
	REGISTER_OOVPAS(XInputOpen, PATCH, 3911/*, 4134*/, 4242),
	REGISTER_OOVPAS(XInputPoll, PATCH, 3911),
	REGISTER_OOVPAS(XInputSetState, PATCH, 3911, 4242, 4831/*, 5233*/),
	REGISTER_OOVPAS(XLaunchNewImageA, PATCH, 3911, 4721, 5344/*, 5558*/),
	REGISTER_OOVPAS(XMountAlternateTitleA, PATCH, 3911, 5028, 5455),
	REGISTER_OOVPAS(XMountMUA, PATCH, 3911, 4242),
	REGISTER_OOVPAS(XMountMURootA, PATCH, 3911, 4242),
	REGISTER_OOVPAS(XMountUtilityDrive, PATCH, 3911/*, 4134*/, 4432),
	REGISTER_OOVPAS(XRegisterThreadNotifyRoutine, PATCH, 3911),
	REGISTER_OOVPAS(XSetProcessQuantumLength, PATCH, 4134),
	REGISTER_OOVPAS(XUnmountAlternateTitleA, PATCH, 3911),
	REGISTER_OOVPAS(XapiFiberStartup, UNPATCHED, 3911),
	REGISTER_OOVPAS(timeKillEvent, PATCH, 3911/*, 5849*/),
	REGISTER_OOVPAS(timeSetEvent, PATCH, 3911/*, 5455*/),
};

// ******************************************************************
// * XAPILIB_OOVPA_SIZE
// ******************************************************************
uint32 XAPILIB_OOVPA_SIZEV2 = sizeof(XAPILIB_OOVPAV2);

#endif
