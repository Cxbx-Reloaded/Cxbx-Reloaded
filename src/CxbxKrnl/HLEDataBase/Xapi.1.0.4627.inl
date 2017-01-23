// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.4627.cpp
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
// * XFormatUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XFormatUtilityDrive, 4627, 7)

        { 0x10, 0x50 },
        { 0x23, 0xFF },
        { 0x34, 0xC0 },
        { 0x46, 0x45 },
        { 0x58, 0xFF },
        { 0x6D, 0x33 },
        { 0x7C, 0x40 },
OOVPA_END;

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriorityBoost, 4627, 10)

        // SetThreadPriorityBoost+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // SetThreadPriorityBoost+0x18 : jl +0x20
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x20 }, // (Offset,Value)-Pair #5

        // SetThreadPriorityBoost+0x1F : setnz al
        { 0x1F, 0x0F }, // (Offset,Value)-Pair #6
        { 0x20, 0x95 }, // (Offset,Value)-Pair #7
        { 0x21, 0xC0 }, // (Offset,Value)-Pair #8

        // SetThreadPriorityBoost+0x2C : mov ecx, [ebp+0x08]
        { 0x2C, 0x8B }, // (Offset,Value)-Pair #9
        { 0x2D, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(GetThreadPriority, 4627, 10)

        // GetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // GetThreadPriority+0x18 : jl +0x2B
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x2B }, // (Offset,Value)-Pair #5

        // GetThreadPriority+0x2F : cmp esi, 0xFFFFFFF0
        { 0x2F, 0x83 }, // (Offset,Value)-Pair #6
        { 0x30, 0xFE }, // (Offset,Value)-Pair #7
        { 0x31, 0xF0 }, // (Offset,Value)-Pair #8

        // GetThreadPriority+0x37 : mov ecx, [ebp+0x08]
        { 0x37, 0x8B }, // (Offset,Value)-Pair #9
        { 0x38, 0x4D }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 4627, 7,

    XREF_FCLOSEDEVICE,
    XRefZero)

        { 0x13, 0x1E },
        { 0x28, 0x75 },
        { 0x3D, 0x01 },
        { 0x52, 0xC6 },
        { 0x67, 0xFF },
        { 0x7C, 0x06 },
        { 0x91, 0x89 },
OOVPA_END;

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 4928, 11,

    XREF_FCLOSEDEVICE,
    XRefZero)

        { 0x13, 0x8B }, // (Offset,Value)-Pair #1
        { 0x14, 0x86 }, // (Offset,Value)-Pair #2
        { 0x15, 0xA3 }, // (Offset,Value)-Pair #3

        { 0x4C, 0x89 }, // (Offset,Value)-Pair #4
        { 0x4D, 0x86 }, // (Offset,Value)-Pair #5
        { 0x4E, 0x9E }, // (Offset,Value)-Pair #6

        { 0x87, 0x89 }, // (Offset,Value)-Pair #7
        { 0x88, 0x86 }, // (Offset,Value)-Pair #8
        { 0x89, 0xA7 }, // (Offset,Value)-Pair #9

        { 0x95, 0xC9 }, // (Offset,Value)-Pair #10
        { 0x96, 0xC3 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * XInputClose
// ******************************************************************
OOVPA_XREF(XInputClose, 4928, 8,

    XRefNoSaveIndex,
    XRefOne)

        // XInputClose+0x05 : call [fCloseDevice]
        { 0x05, XREF_FCLOSEDEVICE },  // (Offset,Value)-Pair #1

        // XInputClose+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B }, // (Offset,Value)-Pair #2
        { 0x01, 0x4C }, // (Offset,Value)-Pair #3
        { 0x02, 0x24 }, // (Offset,Value)-Pair #4
        { 0x03, 0x04 }, // (Offset,Value)-Pair #5

        // XInputClose+0x04 : call [fCloseDevice]
        { 0x04, 0xE8 }, // (Offset,Value)-Pair #6

        // XInputClose+0x09 : retn 0x04
        { 0x09, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 4928, 12)

        { 0x0E, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0F, 0x8A }, // (Offset,Value)-Pair #2
        { 0x10, 0xA3 }, // (Offset,Value)-Pair #3

        { 0x1A, 0x6A }, // (Offset,Value)-Pair #4
        { 0x1B, 0x57 }, // (Offset,Value)-Pair #5

        { 0x1D, 0xEB }, // (Offset,Value)-Pair #6
        { 0x1E, 0x3E }, // (Offset,Value)-Pair #7

        { 0x3A, 0x8B }, // (Offset,Value)-Pair #8
        { 0x3B, 0x8A }, // (Offset,Value)-Pair #9
        { 0x3C, 0xA3 }, // (Offset,Value)-Pair #10

        { 0x69, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x6A, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 4928, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0xAC }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0xE8 }, // (Offset,Value)-Pair #11
        { 0x92, 0x7B }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
/* Unreal Championship's version is different */
OOVPA_NO_XREF(XInputGetCapabilities2, 4928, 12)

		// mov     ebx, ds:KeRaiseIrqlToDpcLevel
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x1C }, // (Offset,Value)-Pair #3

		// test    byte ptr [esi+4], 2
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

		// rep stosd
        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

		// mov  edi, offset unknown_libname_2
        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0xCC }, // (Offset,Value)-Pair #11
        { 0x92, 0x5C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities3
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities3, 4928, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0xA4 }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0x8A }, // (Offset,Value)-Pair #11
        { 0x92, 0x2A }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
/* Unreal Championship's version is different */
OOVPA_NO_XREF(XInputGetCapabilities4, 4928, 12)

		// mov     ebx, ds:KeRaiseIrqlToDpcLevel
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0xDC }, // (Offset,Value)-Pair #3

		// test    byte ptr [esi+4], 2
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

		// rep stosd
        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

		// mov  edi, offset unknown_libname_2
        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0x8C }, // (Offset,Value)-Pair #11
        { 0x92, 0x8C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities5
// ******************************************************************
/* DOAV */
OOVPA_NO_XREF(XInputGetCapabilities5, 4928, 12)

		// mov     ebx, ds:KeRaiseIrqlToDpcLevel
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0xF4 }, // (Offset,Value)-Pair #3

		// test    byte ptr [esi+4], 2
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

		// rep stosd
        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

		// mov  edi, offset unknown_libname_2
        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0x17 }, // (Offset,Value)-Pair #11
        { 0x92, 0xB7 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities6
// ******************************************************************
/* Defender */
OOVPA_NO_XREF(XInputGetCapabilities6, 4928, 12)

		// mov     ebx, ds:KeRaiseIrqlToDpcLevel
        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x78 }, // (Offset,Value)-Pair #3

		// test    byte ptr [esi+4], 2
        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

		// rep stosd
        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

		// mov  edi, offset unknown_libname_2
        { 0x90, 0xBF }, // (Offset,Value)-Pair #10
        { 0x91, 0xE5 }, // (Offset,Value)-Pair #11
        { 0x92, 0xCF }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 4831, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x88 }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0xAF, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB0, 0x45 }, // (Offset,Value)-Pair #11
        { 0xB1, 0xE2 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities2, 4831, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x50 }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0xAF, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB0, 0x45 }, // (Offset,Value)-Pair #11
        { 0xB1, 0xE2 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities3
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities3, 4831, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x14 }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0xAF, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB0, 0x45 }, // (Offset,Value)-Pair #11
        { 0xB1, 0xE2 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities4
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities4, 4831, 12)

        { 0x0B, 0x8B }, // (Offset,Value)-Pair #1
        { 0x0C, 0x1D }, // (Offset,Value)-Pair #2
        { 0x0D, 0x9C }, // (Offset,Value)-Pair #3

        { 0x25, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x26, 0x46 }, // (Offset,Value)-Pair #5
        { 0x27, 0x04 }, // (Offset,Value)-Pair #6
        { 0x28, 0x02 }, // (Offset,Value)-Pair #7

        { 0x39, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x3A, 0xAB }, // (Offset,Value)-Pair #9

        { 0xAF, 0xC7 }, // (Offset,Value)-Pair #10
        { 0xB0, 0x45 }, // (Offset,Value)-Pair #11
        { 0xB1, 0xE2 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities5
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities5, 4831, 8)

		{ 0x22, 0xF6 },
		{ 0x23, 0x46 },
		{ 0x24, 0x04 },
		{ 0x25, 0x02 },
		{ 0x44, 0x6A },
		{ 0x45, 0x12 },
		{ 0xF1, 0xC2 },
		{ 0xF2, 0x08 },
OOVPA_END;

// ******************************************************************
// * GetOverlappedResult
// ******************************************************************
OOVPA_NO_XREF(GetOverlappedResult, 4627, 7)

        { 0x0B, 0x75 },
        { 0x18, 0xC0 },
        { 0x27, 0xEB },
        { 0x32, 0x00 },
        { 0x3F, 0xEB },
        { 0x4C, 0x89 },
        { 0x59, 0x56 },
OOVPA_END;

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 4627, 8)

        { 0x1E, 0x80 },
        { 0x3E, 0xC0 },
        { 0x5E, 0xFF },
        { 0x7E, 0xFC },
        { 0x9E, 0x08 },
        { 0xBE, 0x50 },
        { 0xDE, 0x05 },
        { 0xFE, 0x85 },
OOVPA_END;

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 4928, 12)

        // XLaunchNewImageA+0x03 : sub esp, 0x384
		{ 0x03, 0x81 },
		{ 0x04, 0xEC },
		{ 0x05, 0x84 },
		{ 0x06, 0x03 },
		{ 0x07, 0x00 },
		{ 0x08, 0x00 },
		// XLaunchNewImageA+0x33 : cmp al, 0x44
		{ 0x33, 0x3C },
		{ 0x34, 0x44 },
		// XLaunchNewImageA+0x41 : cmp byte ptr [esi+1], 0x3A
		{ 0x41, 0x80 },
		{ 0x42, 0x7E },
		{ 0x43, 0x02 },
		{ 0x44, 0x5C },
OOVPA_END;

// ******************************************************************
// * XGetLaunchInfo
// ******************************************************************
OOVPA_NO_XREF(XGetLaunchInfo, 4627, 7)

        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x8B },
        { 0x32, 0x30 },
        { 0x3F, 0x00 },
        { 0x4C, 0x83 },
        { 0x59, 0x5E },
OOVPA_END;

// ******************************************************************
// * XSetProcessQuantumLength
// ******************************************************************
OOVPA_NO_XREF(XSetProcessQuantumLength, 4627, 7)

        { 0x01, 0xA1 },
        { 0x04, 0x00 },
        { 0x07, 0x4C },
        { 0x0A, 0x8B },
        { 0x0D, 0x8D },
        { 0x10, 0x89 },
        { 0x13, 0xC2 },
OOVPA_END;

// ******************************************************************
// * timeSetEvent
// ******************************************************************
OOVPA_NO_XREF(timeSetEvent, 4627, 7)

        { 0x1E, 0x8D },
        { 0x3E, 0x89 },
        { 0x5E, 0x15 },
        { 0x7E, 0x3F },
        { 0x9E, 0x03 },
        { 0xBE, 0x32 },
        { 0xDE, 0x89 },
OOVPA_END;

// ******************************************************************
// * timeKillEvent
// ******************************************************************
OOVPA_NO_XREF(timeKillEvent, 4627, 8)

        { 0x0E, 0x8B },
        { 0x1A, 0xC2 },
        { 0x28, 0x8D },
        { 0x36, 0x56 },
        { 0x44, 0x00 },
        { 0x52, 0x00 },
        { 0x60, 0x5E },
        { 0x6E, 0x00 },
OOVPA_END;

// ******************************************************************
// * RaiseException
// ******************************************************************
OOVPA_NO_XREF(RaiseException, 4627, 7)

        { 0x09, 0x83 },
        { 0x14, 0x8B },
        { 0x1F, 0xC7 },
        { 0x2A, 0x10 },
        { 0x35, 0x89 },
        { 0x40, 0x5F },
        { 0x4B, 0xFF },
OOVPA_END;


// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA, 4627, 7)

        { 0x1E, 0x0F },
        { 0x3E, 0x56 },
        { 0x5E, 0xFF },
        { 0x7E, 0x8D },
        { 0x9E, 0x50 },
        { 0xBE, 0x66 },
        { 0xDE, 0xF0 },
OOVPA_END;

// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA, 4928, 7)

        { 0x1E, 0x0F },
        { 0x3E, 0x83 },
        { 0x5E, 0x03 },
        { 0x7E, 0x8D },
        { 0x9E, 0x00 },
        { 0xBE, 0x01 },
        { 0xDE, 0x45 },
OOVPA_END;

// ******************************************************************
// * XUnmountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XUnmountAlternateTitleA, 4627, 7)

        { 0x0A, 0x65 },
        { 0x16, 0xFF },
        { 0x23, 0x83 },
        { 0x2E, 0x45 },
        { 0x3A, 0x50 },
        { 0x46, 0x0B },
        { 0x52, 0x50 },
OOVPA_END;

// ******************************************************************
// * MoveFileA
// ******************************************************************
OOVPA_NO_XREF(MoveFileA, 4627, 7)

        { 0x11, 0x8D },
        { 0x24, 0xE8 },
        { 0x37, 0x50 },
        { 0x4A, 0x7D },
        { 0x5D, 0xFF },
        { 0x74, 0xFF },
        { 0x83, 0x33 },
OOVPA_END;

// ******************************************************************
// * XInputGetDeviceDescription
// ******************************************************************
OOVPA_NO_XREF(XInputGetDeviceDescription, 4831, 9)

		{ 0x03, 0x83 },
		{ 0x04, 0xEC },
		{ 0x05, 0x48 },
		{ 0x06, 0x83 },
		{ 0x07, 0x65 },
		{ 0x08, 0xF8 },
		{ 0x09, 0x00 },
		{ 0x34, 0x6A },
		{ 0x35, 0x06 },
OOVPA_END;

// ******************************************************************
// * XAPI_4627
// ******************************************************************
OOVPATable XAPI_4627[] = {

    // XFormatUtilityDrive
	OOVPA_TABLE_ENTRY(XFormatUtilityDrive, 4627),
	// SetThreadPriorityBoost
	OOVPA_TABLE_ENTRY(SetThreadPriorityBoost, 4627),
	// SetThreadPriority (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(SetThreadPriority, 3911),
	// GetThreadPriority
	OOVPA_TABLE_ENTRY(GetThreadPriority, 4627),
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(XRegisterThreadNotifyRoutine, 3911),
    // XMountUtilityDrive (* unchanged since 4432 *)
	OOVPA_TABLE_ENTRY(XMountUtilityDrive, 4432),
    // XInitDevices (* unchanged since 1.0.3911 *)
	OOVPA_TABLE_ENTRY(XInitDevices, 3911),
    // XGetDevices (* unchanged since 1.0.4134 *)
	OOVPA_TABLE_ENTRY(XGetDevices, 4134),
    // XInputOpen (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_ENTRY(XInputOpen, 4361),
    // XInputPoll (* unchanged since 4134 *)
	OOVPA_TABLE_ENTRY(XInputPoll, 4134),
	// XID_fCloseDevice
	OOVPA_TABLE_XREF(XID_fCloseDevice, 4627),
	// XID_fCloseDevice
	OOVPA_TABLE_XREF(XID_fCloseDevice, 4928),
	// XInputClose
	OOVPA_TABLE_ENTRY(XInputClose, 4928),
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4361),
    // XInputGetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_ENTRY(XInputGetState, 4361),
    // XInputGetState
	OOVPA_TABLE_ENTRY(XInputGetState, 4928),
    // XInputGetCapabilities
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4928),
	// XInputGetCapabilities2
	OOVPA_TABLE_PATCH(XInputGetCapabilities2, 4928, XInputGetCapabilities),
	// XInputGetCapabilities3
	OOVPA_TABLE_PATCH(XInputGetCapabilities3, 4928, XInputGetCapabilities),
	// XInputGetCapabilities4
	OOVPA_TABLE_PATCH(XInputGetCapabilities4, 4928, XInputGetCapabilities),
	// XInputGetCapabilities5
	OOVPA_TABLE_PATCH(XInputGetCapabilities5, 4928, XInputGetCapabilities),
    // XInputSetState (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_ENTRY(XInputSetState, 4361),
	// XInputSetState
	OOVPA_TABLE_ENTRY(XInputSetState, 4928),
    /* obsolete?
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
	OOVPA_TABLE_ENTRY(XapiThreadStartup, 4361),
    //*/
    //*/
	// XInputGetCapabilities
	OOVPA_TABLE_ENTRY(XInputGetCapabilities, 4831),
	// XInputGetCapabilities2
	OOVPA_TABLE_PATCH(XInputGetCapabilities2, 4831, XInputGetCapabilities),
	// XInputGetCapabilities3
	OOVPA_TABLE_PATCH(XInputGetCapabilities3, 4831, XInputGetCapabilities),
	// XInputGetCapabilities4
	OOVPA_TABLE_PATCH(XInputGetCapabilities4, 4831, XInputGetCapabilities),
	// XInputGetCapabilities5
	OOVPA_TABLE_PATCH(XInputGetCapabilities5, 4831, XInputGetCapabilities),
	// XInputGetCapabilities6
	OOVPA_TABLE_PATCH(XInputGetCapabilities6, 4928, XInputGetCapabilities),
    // +s
    /* not necessary?
    // XCalculateSignatureBeginEx
	OOVPA_TABLE_ENTRY(XCalculateSignatureBeginEx, 4627),
    // XCalculateSignatureBegin
	OOVPA_TABLE_ENTRY(XCalculateSignatureBegin, 4627),
    // XCalculateSignatureUpdate
	OOVPA_TABLE_ENTRY(XCalculateSignatureUpdate, 4627),
    // XCalculateSignatureEnd
	OOVPA_TABLE_ENTRY(XCalculateSignatureEnd, 4627), //*/
    // s+
	// CreateFiber (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(CreateFiber, 3911),
	// DeleteFiber (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(DeleteFiber, 3911),
	// GetTimeZoneInformation (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(GetTimeZoneInformation, 3911),
	// GetExitCodeThread (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(GetExitCodeThread, 3911),
	// GetOverlappedResult
	OOVPA_TABLE_ENTRY(GetOverlappedResult, 4627),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA, 4627, XLaunchNewImage),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA, 4928, XLaunchNewImage),
	// XGetLaunchInfo
	OOVPA_TABLE_ENTRY(XGetLaunchInfo, 4627),
	// XSetProcessQuantumLength
	OOVPA_TABLE_ENTRY(XSetProcessQuantumLength, 4627),
	// SignalObjectAndWait (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(SignalObjectAndWait, 3911),
	// timeSetEvent
	OOVPA_TABLE_ENTRY(timeSetEvent, 4627),
	// timeKillEvent
	OOVPA_TABLE_ENTRY(timeKillEvent, 4627),
	// RaiseException
	OOVPA_TABLE_ENTRY(RaiseException, 4627),
	// QueueUserAPC (* unchanged since 3911 *)
	OOVPA_TABLE_ENTRY(QueueUserAPC, 3911),
	// XMountAlternateTitleA
	OOVPA_TABLE_PATCH(XMountAlternateTitleA, 4627, XMountAlternateTitle),
	// XMountAlternateTitleA
	OOVPA_TABLE_PATCH(XMountAlternateTitleA, 4928, XMountAlternateTitle),
	// XUnmountAlternateTitleA
	OOVPA_TABLE_PATCH(XUnmountAlternateTitleA, 4627, XUnmountAlternateTitle),
	// XInputGetDeviceDescription
	OOVPA_TABLE_ENTRY(XInputGetDeviceDescription, 4831),
	// MoveFileA
	/*
	OOVPA_TABLE_ENTRY(MoveFileA, 4627),
	*/
};

// ******************************************************************
// * XAPI_4627_SIZE
// ******************************************************************
uint32 XAPI_4627_SIZE = sizeof(XAPI_4627);
