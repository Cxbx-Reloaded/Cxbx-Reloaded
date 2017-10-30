// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.5849.inl
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

#if 0 // Moved to 5455
// ******************************************************************
// * timeSetEvent
// ******************************************************************
//Xbe Explorer generated pattern, derived from address $0027927D in "EA SPORTSÅERUGBY 2005" :
//558BEC83EC14536880153200FF15........8B0D........33DB3BCB7548538D 1F 4046 0124 _timeSetEvent@20 ^ 000ED __imp__RtlEnterCriticalSection@4 ^ 0014D _XapiMmTimerThreadState ^ 003FR _CreateThread@24 ^ 0047D _timeSetEvent@20+0112 ^ 004DR _CloseHandle@4 ^ 005CD __imp__KeWaitForSingleObject@20 ^ 0062D _XapiMmTimerThreadState ^0082D _timeSetEvent@20+010F ^0090D _XapiMmTimerSerialNumber ^0097D _XapiMmTimerSerialNumber ^ 00C0R __allmul ^ 00CCD __imp__KeQueryInterruptTime@0 ^00E7D __imp__KeSetTimerEx@20 ^ 00ECD _XapiMmTimerThreadState ^ 00FBD __imp__KeSetEvent@12 ^0119D __imp__RtlLeaveCriticalSection@4
//Improved pattern using timeSetEvent in "Kingdom Under Fire: The Crusaders" at address $00221F2E
OOVPA_NO_XREF(timeSetEvent, 5849, 10)
	{0x00, 0x55},
	{0x01, 0x8B},
	{0x02, 0xEC},
	{0x04, 0xEC},
	{0x0D, 0x15},
	{0x12, 0x8B},
	{0x13, 0x0D},
	{0x18, 0x33},
	{0x19, 0xDB},
	{0x1A, 0x3B}
OOVPA_END;
#endif
#if 0 // No longer used, replaced by generic 3911 version
//; Xbe Explorer generated pattern, derived from address $002793A1 in "EA SPORTSÅERUGBY 2005" :
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
#endif
#if 0 // No longer used, replaced by generic 4831 version
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
#endif
#if 0 // No longer used, replaced by generic 3911 version
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
#endif
