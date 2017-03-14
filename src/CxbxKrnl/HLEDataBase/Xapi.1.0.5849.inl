// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.5849.cpp
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
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 5849, 8)

        { 0x22, 0x6A },
        { 0x3E, 0x01 },
        { 0x5E, 0x75 },
        { 0x7E, 0x5E },
        { 0x9E, 0x7D },
        { 0xBE, 0x51 },
        { 0xDE, 0x00 },
        { 0xFE, 0x53 },
OOVPA_END;

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceChanges, 5849, 7)

        { 0x0C, 0x75 },
        { 0x1A, 0x53 },
        { 0x28, 0xF7 },
        { 0x36, 0x56 },
        { 0x44, 0x89 },
        { 0x52, 0xC8 },
        { 0x60, 0x5F },
OOVPA_END;

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(GetThreadPriority, 5849, 7)

        { 0x0D, 0xFF },
        { 0x16, 0x85 },
        { 0x1F, 0x15 },
        { 0x2A, 0x04 },
        { 0x35, 0xF1 },
        { 0x40, 0x8B },
        { 0x4B, 0xB8 },
OOVPA_END;

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriorityBoost, 5849, 7)

        { 0x07, 0xFF },
        { 0x10, 0xFF },
        { 0x19, 0x20 },
        { 0x22, 0x50 },
        { 0x2C, 0x8B },
        { 0x35, 0x33 },
        { 0x40, 0x33 },
OOVPA_END;

// ******************************************************************
// * XGetSectionSize
// ******************************************************************
OOVPA_NO_XREF(XGetSectionSize, 5849, 5)

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x04, 0x8B },
        { 0x06, 0x08 },
        { 0x08, 0x04 },
OOVPA_END;

//Xbe Explorer generated pattern, derived from address $0027927D in "EA SPORTS™ RUGBY 2005" :
//558BEC83EC14536880153200FF15........8B0D........33DB3BCB7548538D 1F 4046 0124 _timeSetEvent@20 ^ 000ED __imp__RtlEnterCriticalSection@4 ^ 0014D _XapiMmTimerThreadState ^ 003FR _CreateThread@24 ^ 0047D _timeSetEvent@20+0112 ^ 004DR _CloseHandle@4 ^ 005CD __imp__KeWaitForSingleObject@20 ^ 0062D _XapiMmTimerThreadState ^0082D _timeSetEvent@20+010F ^0090D _XapiMmTimerSerialNumber ^0097D _XapiMmTimerSerialNumber ^ 00C0R __allmul ^ 00CCD __imp__KeQueryInterruptTime@0 ^00E7D __imp__KeSetTimerEx@20 ^ 00ECD _XapiMmTimerThreadState ^ 00FBD __imp__KeSetEvent@12 ^0119D __imp__RtlLeaveCriticalSection@4
OOVPA_NO_XREF(timeSetEvent, 5849, 9)
	{0x00, 0x55},
	{0x01, 0x8B},
	{0x02, 0xEC},
	{0x04, 0xEC},
	{0x08, 0x80},
	{0x0D, 0x15},
	{0x12, 0x8B},
	{0x13, 0x0D},
	{0x18, 0x33}		
OOVPA_END;

//; Xbe Explorer generated pattern, derived from address $002793A1 in "EA SPORTS™ RUGBY 2005" :
//5657BF8015320057FF15........8B54240C8B0D........0FB7C24885C9743E 1D AD12 006F _timeKillEvent@4 ^ 000AD __imp__RtlEnterCriticalSection@4 ^ 0014D _XapiMmTimerThreadState ^ 003DD __imp__KeCancelTimer@4 ^ 0046D _XapiMmTimerThreadState ^ 0056D __imp__KeSetEvent@12 ^ 0064D __imp__RtlLeaveCriticalSection@4
OOVPA_NO_XREF(timeKillEvent, 5849, 10)
	{ 0x00, 0x56 },
	{ 0x01, 0x57 },
	{ 0x02, 0xBF },
	{ 0x09, 0x15 },
	{ 0x0E, 0x8B },
	{ 0x0F, 0x54 },
	{ 0x10, 0x24 },
	{ 0x13, 0x0D },
	{ 0x18, 0x0F },
	{ 0x1F, 0x3E }
OOVPA_END;


// ******************************************************************
// * RaiseException
// ******************************************************************
OOVPA_NO_XREF(RaiseException, 5849, 7)

        { 0x09, 0x83 },
        { 0x14, 0x8B },
        { 0x1F, 0xC7 },
        { 0x2A, 0x10 },
        { 0x35, 0x89 },
        { 0x40, 0x5F },
        { 0x4B, 0xFF },
OOVPA_END;

// ******************************************************************
// * XGetDeviceEnumerationStatus
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceEnumerationStatus, 5849, 7)

        { 0x04, 0x15 },
        { 0x0A, 0x35 },
        { 0x10, 0x09 },
        { 0x17, 0x00 },
        { 0x1C, 0x46 },
        { 0x25, 0x8B },
        { 0x28, 0xC3 },
OOVPA_END;

// ******************************************************************
// * SwitchToThread
// ******************************************************************
OOVPA_NO_XREF(SwitchToThread, 5849, 6)

        { 0x01, 0x15 },
        { 0x06, 0x33 },
        { 0x07, 0xC9 },
        { 0x0A, 0x00 },
        { 0x0D, 0x0F },
        { 0x10, 0x8B },
OOVPA_END;

// ******************************************************************
// * XAPI_5849
// ******************************************************************
OOVPATable XAPI_5849[] = {

	REGISTER_OOVPA(SetThreadPriority, 3911, PATCH),
	REGISTER_OOVPA(XMountUtilityDrive, 4432, PATCH),
	REGISTER_OOVPA(XInitDevices, 5233, PATCH),
	REGISTER_OOVPA(XGetDevices, 4134, PATCH),
	REGISTER_OOVPA(XInputOpen, 4361, PATCH),
	REGISTER_OOVPA(XInputGetCapabilities, 5558, PATCH),
	REGISTER_OOVPA(XInputGetState, 5558, PATCH),
	REGISTER_OOVPA(XGetDeviceChanges, 5849, PATCH),
	// REGISTER_OOVPA(XapiThreadStartup, 4361, PATCH), // obsolete?
	REGISTER_OOVPA(QueueUserAPC, 3911, PATCH),
	REGISTER_OOVPA(GetThreadPriority, 5849, PATCH),
	REGISTER_OOVPA(SetThreadPriorityBoost, 5849, PATCH),
	// REGISTER_OOVPA(GetThreadPriorityBoost, 5849, PATCH),
	REGISTER_OOVPA(timeSetEvent, 5849, PATCH),
	REGISTER_OOVPA(timeKillEvent, 5849, PATCH),
	REGISTER_OOVPA(RaiseException, 5849, PATCH),
	REGISTER_OOVPA(XLaunchNewImage, 5558, ALIAS, XLaunchNewImageA),
	REGISTER_OOVPA(XInputSetState, 5233, PATCH),
	REGISTER_OOVPA(XGetDeviceEnumerationStatus, 5849, PATCH),
	// REGISTER_OOVPA(SwitchToThread, 5849, PATCH),
	REGISTER_OOVPA(XFormatUtilityDrive, 4627, PATCH),
	REGISTER_OOVPA(CreateFiber, 3911, DISABLED),
	REGISTER_OOVPA(DeleteFiber, 3911, DISABLED),
	REGISTER_OOVPA(SwitchToFiber, 3911, DISABLED),
	REGISTER_OOVPA(ConvertThreadToFiber, 3911, DISABLED),
};

// ******************************************************************
// * XAPI_5849_SIZE
// ******************************************************************
uint32 XAPI_5849_SIZE = sizeof(XAPI_5849);
