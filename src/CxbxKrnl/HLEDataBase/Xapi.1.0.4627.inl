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
SOOVPA<7> XFormatUtilityDrive_1_0_4627 =
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x10, 0x50 },
        { 0x23, 0xFF },
        { 0x34, 0xC0 },
        { 0x46, 0x45 },
        { 0x58, 0xFF },
        { 0x6D, 0x33 },
        { 0x7C, 0x40 },
    }
};

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
SOOVPA<10> SetThreadPriorityBoost_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
SOOVPA<10> GetThreadPriority_1_0_4627 =
{
    0,  // Large == 0
    10, // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * RtlFreeHeap
// ******************************************************************
SOOVPA<9> RtlFreeHeap_1_0_4627 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // RtlFreeHeap+0x1F : test edi, edi
        { 0x1F, 0x85 }, // (Offset,Value)-Pair #1
        { 0x20, 0xFF }, // (Offset,Value)-Pair #2

        // RtlFreeHeap+0x23 : mov al, 1
        { 0x23, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x24, 0x01 }, // (Offset,Value)-Pair #4

        // RtlFreeHeap+0x35 : mov eax, fs:[0x20]
        { 0x35, 0x64 }, // (Offset,Value)-Pair #5
        { 0x36, 0xA1 }, // (Offset,Value)-Pair #6
        { 0x37, 0x20 }, // (Offset,Value)-Pair #7

        // RtlFreeHeap+0x49 : push 0x0B
        { 0x49, 0x6A }, // (Offset,Value)-Pair #8
        { 0x4A, 0x0B }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * RtlReAllocateHeap
// ******************************************************************
SOOVPA<10> RtlReAllocateHeap_1_0_4627 =
{
    0,  // Large == 0
    10,  // Count == 10

    XRefNotSaved,
    XRefNotUsed,

    {
        // RtlReAllocateHeap+0x1C : test esi, esi
        { 0x1C, 0x85 }, // (Offset,Value)-Pair #1
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #2

        // RtlReAllocateHeap+0x26 : or [ebp+0x0C], ecx
        { 0x26, 0x09 }, // (Offset,Value)-Pair #3
        { 0x27, 0x4D }, // (Offset,Value)-Pair #4
        { 0x28, 0x0C }, // (Offset,Value)-Pair #5

        // RtlReAllocateHeap+0x61 : jz +0x06; add ecx, 0x10
        { 0x61, 0x74 }, // (Offset,Value)-Pair #6
        { 0x62, 0x06 }, // (Offset,Value)-Pair #7
        { 0x63, 0x83 }, // (Offset,Value)-Pair #8
        { 0x64, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x65, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
SOOVPA<7> XID_fCloseDevice_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_FCLOSEDEVICE,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x13, 0x1E },
        { 0x28, 0x75 },
        { 0x3D, 0x01 },
        { 0x52, 0xC6 },
        { 0x67, 0xFF },
        { 0x7C, 0x06 },
        { 0x91, 0x89 },
    }
};

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
SOOVPA<11> XID_fCloseDevice_1_0_4928 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_FCLOSEDEVICE,  // XRef Is  Saved
    0,                  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * XInputClose
// ******************************************************************
SOOVPA<8> XInputClose_1_0_4928 =
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,  // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * XInputGetState
// ******************************************************************
SOOVPA<12> XInputGetState_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
SOOVPA<12> XInputGetCapabilities_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
/* Unreal Championship's version is different */
SOOVPA<12> XInputGetCapabilities2_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities3
// ******************************************************************
SOOVPA<12> XInputGetCapabilities3_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
/* Unreal Championship's version is different */
SOOVPA<12> XInputGetCapabilities4_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities5
// ******************************************************************
/* DOAV */
SOOVPA<12> XInputGetCapabilities5_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities6
// ******************************************************************
/* Defender */
SOOVPA<12> XInputGetCapabilities6_1_0_4928 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
SOOVPA<12> XInputGetCapabilities_1_0_4831 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities2
// ******************************************************************
SOOVPA<12> XInputGetCapabilities2_1_0_4831 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities3
// ******************************************************************
SOOVPA<12> XInputGetCapabilities3_1_0_4831 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities4
// ******************************************************************
SOOVPA<12> XInputGetCapabilities4_1_0_4831 =
{
    0,  // Large == 0
    12, // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XInputGetCapabilities5
// ******************************************************************
SOOVPA<12> XInputGetCapabilities5_1_0_4831 =
{
    0,  // Large == 0
    8,	// Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
		{ 0x22, 0xF6 },
		{ 0x23, 0x46 },
		{ 0x24, 0x04 },
		{ 0x25, 0x02 },
		{ 0x44, 0x6A },
		{ 0x45, 0x12 },
		{ 0xF1, 0xC2 },
		{ 0xF2, 0x08 },
    }
};

// ******************************************************************
// * GetOverlappedResult
// ******************************************************************
SOOVPA<7> GetOverlappedResult_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x75 },
        { 0x18, 0xC0 },
        { 0x27, 0xEB },
        { 0x32, 0x00 },
        { 0x3F, 0xEB },
        { 0x4C, 0x89 },
        { 0x59, 0x56 },
    }
};

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
SOOVPA<8> XLaunchNewImageA_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x80 },
        { 0x3E, 0xC0 },
        { 0x5E, 0xFF },
        { 0x7E, 0xFC },
        { 0x9E, 0x08 },
        { 0xBE, 0x50 },
        { 0xDE, 0x05 },
        { 0xFE, 0x85 },
    }
};

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
SOOVPA<12> XLaunchNewImageA_1_0_4928 = 
{
    0,   // Large == 0
    12,  // Count == 12

    XRefNotSaved,
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * XGetLaunchInfo
// ******************************************************************
SOOVPA<7> XGetLaunchInfo_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x8B },
        { 0x32, 0x30 },
        { 0x3F, 0x00 },
        { 0x4C, 0x83 },
        { 0x59, 0x5E },
    }
};

// ******************************************************************
// * XSetProcessQuantumLength
// ******************************************************************
SOOVPA<7> XSetProcessQuantumLength_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x01, 0xA1 },
        { 0x04, 0x00 },
        { 0x07, 0x4C },
        { 0x0A, 0x8B },
        { 0x0D, 0x8D },
        { 0x10, 0x89 },
        { 0x13, 0xC2 },
    }
};

// ******************************************************************
// * timeSetEvent
// ******************************************************************
SOOVPA<7> timeSetEvent_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x8D },
        { 0x3E, 0x89 },
        { 0x5E, 0x15 },
        { 0x7E, 0x3F },
        { 0x9E, 0x03 },
        { 0xBE, 0x32 },
        { 0xDE, 0x89 },
    }
};

// ******************************************************************
// * timeKillEvent
// ******************************************************************
SOOVPA<8> timeKillEvent_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0E, 0x8B },
        { 0x1A, 0xC2 },
        { 0x28, 0x8D },
        { 0x36, 0x56 },
        { 0x44, 0x00 },
        { 0x52, 0x00 },
        { 0x60, 0x5E },
        { 0x6E, 0x00 },
    }
};

// ******************************************************************
// * RaiseException
// ******************************************************************
SOOVPA<7> RaiseException_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x09, 0x83 },
        { 0x14, 0x8B },
        { 0x1F, 0xC7 },
        { 0x2A, 0x10 },
        { 0x35, 0x89 },
        { 0x40, 0x5F },
        { 0x4B, 0xFF },
    }
};


// ******************************************************************
// * VirtualProtect
// ******************************************************************
SOOVPA<7> VirtualProtect_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    0/*XREF_VirtualProtect*/,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x04, 0x8D },
        { 0x0A, 0x24 },
        { 0x10, 0x10 },
        { 0x18, 0x85 },
        { 0x1C, 0x33 },
        { 0x22, 0xE8 },
        { 0x28, 0xC0 },
    }
};

// ******************************************************************
// * CreateSemaphoreA
// ******************************************************************
SOOVPA<7> CreateSemaphoreA_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0A, 0x74 },
        { 0x16, 0x50 },
        { 0x22, 0x10 },
        { 0x31, 0x85 },
        { 0x3A, 0x75 },
        { 0x4A, 0x8B },
        { 0x55, 0x33 },
    }
};

// ******************************************************************
// * ReleaseSemaphore
// ******************************************************************
SOOVPA<7> ReleaseSemaphore_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x03, 0x0C },
        { 0x08, 0xFF },
        { 0x0D, 0x15 },
        { 0x12, 0x85 },
        { 0x17, 0xC0 },
        { 0x1C, 0xE8 },
        { 0x21, 0x33 },
    }
};

// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
SOOVPA<7> XMountAlternateTitleA_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x0F },
        { 0x3E, 0x56 },
        { 0x5E, 0xFF },
        { 0x7E, 0x8D },
        { 0x9E, 0x50 },
        { 0xBE, 0x66 },
        { 0xDE, 0xF0 },
    }
};

// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
SOOVPA<7> XMountAlternateTitleA_1_0_4928 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x1E, 0x0F },
        { 0x3E, 0x83 },
        { 0x5E, 0x03 },
        { 0x7E, 0x8D },
        { 0x9E, 0x00 },
        { 0xBE, 0x01 },
        { 0xDE, 0x45 },
    }
};

// ******************************************************************
// * XUnmountAlternateTitleA
// ******************************************************************
SOOVPA<7> XUnmountAlternateTitleA_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x0A, 0x65 },
        { 0x16, 0xFF },
        { 0x23, 0x83 },
        { 0x2E, 0x45 },
        { 0x3A, 0x50 },
        { 0x46, 0x0B },
        { 0x52, 0x50 },
    }
};

// ******************************************************************
// * XGetFileCacheSize
// ******************************************************************
SOOVPA<6> XGetFileCacheSize_1_0_4627 = 
{
    0,  // Large == 0
    6,  // Count == 6

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x00, 0xFF },
        { 0x01, 0x15 },
        { 0x06, 0xC1 },
        { 0x07, 0xE0 },
        { 0x08, 0x0C },
        { 0x09, 0xC3 },
    }
};

// ******************************************************************
// * MoveFileA
// ******************************************************************
SOOVPA<7> MoveFileA_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x11, 0x8D },
        { 0x24, 0xE8 },
        { 0x37, 0x50 },
        { 0x4A, 0x7D },
        { 0x5D, 0xFF },
        { 0x74, 0xFF },
        { 0x83, 0x33 },
    }
};

// ******************************************************************
// * XInputGetDeviceDescription
// ******************************************************************
SOOVPA<9> XInputGetDeviceDescription_1_0_4831 = 
{
	0,	// Large == 0
	9,	// Count == 9

	XRefNotSaved,
	XRefNotUsed,

	{
		{ 0x03, 0x83 },
		{ 0x04, 0xEC },
		{ 0x05, 0x48 },
		{ 0x06, 0x83 },
		{ 0x07, 0x65 },
		{ 0x08, 0xF8 },
		{ 0x09, 0x00 },
		{ 0x34, 0x6A },
		{ 0x35, 0x06 },
	}
};

// ******************************************************************
// * XAPI_1_0_4627
// ******************************************************************
OOVPATable XAPI_1_0_4627[] =
{
    // XFormatUtilityDrive
    {
        (OOVPA*)&XFormatUtilityDrive_1_0_4627,

        XTL::EmuXFormatUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXFormatUtilityDrive"
        #endif
    },
    // SetThreadPriorityBoost
    {
        (OOVPA*)&SetThreadPriorityBoost_1_0_4627,

        XTL::EmuSetThreadPriorityBoost,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriorityBoost"
        #endif
    },
    // SetThreadPriority (* unchanged since 3911 *)
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    // GetThreadPriority
    {
        (OOVPA*)&GetThreadPriority_1_0_4627,

        XTL::EmuGetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuGetThreadPriority"
        #endif
    },
    // XRegisterThreadNotifyRoutine (* unchanged since 3911 *)
    {
        (OOVPA*)&XRegisterThreadNotifyRoutine_1_0_3911,

        XTL::EmuXRegisterThreadNotifyRoutine,

        #ifdef _DEBUG_TRACE
        "EmuXRegisterThreadNotifyRoutine"
        #endif
    },
    // QueryPerformanceCounter (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceCounter_1_0_4361,

        XTL::EmuQueryPerformanceCounter,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceCounter"
        #endif
    },
    // QueryPerformanceFrequency (* unchanged since 4361 *)
    {
        (OOVPA*)&QueryPerformanceFrequency_1_0_4361,

        XTL::EmuQueryPerformanceFrequency,

        #ifdef _DEBUG_TRACE
        "EmuQueryPerformanceFrequency"
        #endif
    },
    // XMountUtilityDrive (* unchanged since 4432 *)
    {
        (OOVPA*)&XMountUtilityDrive_1_0_4432,

        XTL::EmuXMountUtilityDrive,

        #ifdef _DEBUG_TRACE
        "EmuXMountUtilityDrive"
        #endif
    },
    // XInitDevices (* unchanged since 1.0.3911 *)
    {
        (OOVPA*)&XInitDevices_1_0_3911,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
    // XGetDevices (* unchanged since 1.0.4134 *)
    {
        (OOVPA*)&XGetDevices_1_0_4134,

        XTL::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices"
        #endif
    },
    // XInputOpen (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputOpen_1_0_4361,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XInputPoll (* unchanged since 4134 *)
    {
        (OOVPA*)&XInputPoll_1_0_4134,

        XTL::EmuXInputPoll,

        #ifdef _DEBUG_TRACE
        "EmuXInputPoll"
        #endif
    },
	// XID_fCloseDevice
    {
        (OOVPA*)&XID_fCloseDevice_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
		"XID_fCloseDevice (4627) (XREF)"
        #endif
    },
    // XID_fCloseDevice
    {
        (OOVPA*)&XID_fCloseDevice_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "XID_fCloseDevice (XREF)"
        #endif
    },
    // XInputClose
    {
        (OOVPA*)&XInputClose_1_0_4928,

        XTL::EmuXInputClose,

        #ifdef _DEBUG_TRACE
        "EmuXInputClose"
        #endif
    },
    // XInputGetCapabilities (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4361,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XInputGetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputGetState_1_0_4361,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputGetState
    {
        (OOVPA*)&XInputGetState_1_0_4928,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
	// XInputGetCapabilities2
    {
        (OOVPA*)&XInputGetCapabilities2_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities2"
        #endif
    },
	// XInputGetCapabilities3
    {
        (OOVPA*)&XInputGetCapabilities3_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities3"
        #endif
    },
	// XInputGetCapabilities4
    {
        (OOVPA*)&XInputGetCapabilities4_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities4"
        #endif
    },
	// XInputGetCapabilities5
    {
        (OOVPA*)&XInputGetCapabilities5_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
		"EmuXInputGetCapabilities(5)"
        #endif
    },
    // XInputSetState (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XInputSetState_1_0_4361,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
        #endif
    },
	// XInputSetState
    {
        (OOVPA*)&XInputSetState_1_0_4928,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
		"EmuXInputSetState (4928)"
        #endif
    },
    /* obsolete?
    // XapiThreadStartup (* unchanged since 1.0.4361 *)
    {
        (OOVPA*)&XapiThreadStartup_1_0_4361,

        XTL::EmuXapiThreadStartup,

        #ifdef _DEBUG_TRACE
        "EmuXapiThreadStartup"
        #endif
    },
    //*/
    //*/
	// XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_4831,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities_4831"
        #endif
    },
	// XInputGetCapabilities2
    {
        (OOVPA*)&XInputGetCapabilities2_1_0_4831,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities2_4831"
        #endif
    },
	// XInputGetCapabilities3
    {
        (OOVPA*)&XInputGetCapabilities3_1_0_4831,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities3_4831"
        #endif
    },
	// XInputGetCapabilities4
    {
        (OOVPA*)&XInputGetCapabilities4_1_0_4831,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities4_4831"
        #endif
    },
	// XInputGetCapabilities5
    {
        (OOVPA*)&XInputGetCapabilities5_1_0_4831,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities5_4831"
        #endif
    },
	// XInputGetCapabilities6
    {
        (OOVPA*)&XInputGetCapabilities6_1_0_4928,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities6_4928"
        #endif
    },
    // +s
    /* not necessary?
    // XCalculateSignatureBeginEx
    {
        (OOVPA*)&XCalculateSignatureBeginEx_1_0_4627,

        XTL::EmuXCalculateSignatureBeginEx,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureBeginEx (XREF&FUNC)"
        #endif
    },
    // XCalculateSignatureBegin
    {
        (OOVPA*)&XCalculateSignatureBegin_1_0_4627,

        XTL::EmuXCalculateSignatureBegin,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureBegin"
        #endif
    },
    // XCalculateSignatureUpdate
    {
        (OOVPA*)&XCalculateSignatureUpdate_1_0_4627,

        XTL::EmuXCalculateSignatureUpdate,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureUpdate"
        #endif
    },
    // XCalculateSignatureEnd
    {
        (OOVPA*)&XCalculateSignatureEnd_1_0_4627,

        XTL::EmuXCalculateSignatureEnd,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureEnd"
        #endif
    },
    //*/
    // s+
	// CreateFiber (* unchanged since 3911 *)
	{
		(OOVPA*)&CreateFiber_1_0_3911,

		XTL::EmuCreateFiber,

		#ifdef _DEBUG_TRACE
		"EmuCreateFiber"
		#endif
	},
	// DeleteFiber (* unchanged since 3911 *)
	{
		(OOVPA*)&DeleteFiber_1_0_3911,

		XTL::EmuDeleteFiber,

		#ifdef _DEBUG_TRACE
		"EmuDeleteFiber"
		#endif
	},
	// GetTimeZoneInformation (* unchanged since 3911 *)
    {
        (OOVPA*)&GetTimeZoneInformation_1_0_3911,

        XTL::EmuGetTimeZoneInformation,

        #ifdef _DEBUG_TRACE
        "GetTimeZoneInformation"
        #endif
    },
	// GetExitCodeThread (* unchanged since 3911 *)
    {
        (OOVPA*)&GetExitCodeThread_1_0_3911,

        XTL::EmuGetExitCodeThread,

        #ifdef _DEBUG_TRACE
        "EmuGetExitCodeThread"
        #endif
    },
	// GetOverlappedResult
    {
        (OOVPA*)&GetOverlappedResult_1_0_4627,

        XTL::EmuGetOverlappedResult,

        #ifdef _DEBUG_TRACE
        "EmuGetOverlappedResult"
        #endif
    },
	// XLaunchNewImageA
    {
        (OOVPA*)&XLaunchNewImageA_1_0_4627,
        XTL::EmuXLaunchNewImage,

        #ifdef _DEBUG_TRACE
		"EmuXLaunchNewImageA"
        #endif
    },
	// XLaunchNewImageA
    {
        (OOVPA*)&XLaunchNewImageA_1_0_4928,
        XTL::EmuXLaunchNewImage,

        #ifdef _DEBUG_TRACE
		"EmuXLaunchNewImageA"
        #endif
    },
	// XGetLaunchInfo
    {
        (OOVPA*)&XGetLaunchInfo_1_0_4627,
        XTL::EmuXGetLaunchInfo,

        #ifdef _DEBUG_TRACE
        "EmuXGetLaunchInfo"
        #endif
    },
	// XSetProcessQuantumLength
    {
        (OOVPA*)&XSetProcessQuantumLength_1_0_4627,
		XTL::EmuXSetProcessQuantumLength,

        #ifdef _DEBUG_TRACE
        "EmuXSetProcessQuantumLength"
        #endif
    },
	// SignalObjectAndWait (* unchanged since 3911 *)
	{
		(OOVPA*)&SignalObjectAndWait_1_0_3911, 
			
		XTL::EmuSignalObjectAndWait,

		#ifdef _DEBUG_TRACE
		"EmuSignalObjectAndWait"
		#endif
	},
	// timeSetEvent
    {
        (OOVPA*)&timeSetEvent_1_0_4627,
        XTL::EmutimeSetEvent,

        #ifdef _DEBUG_TRACE
        "EmutimeSetEvent"
        #endif
    },
    // timeKillEvent
    {
        (OOVPA*)&timeKillEvent_1_0_4627,
        XTL::EmutimeKillEvent,

        #ifdef _DEBUG_TRACE
        "EmutimeKillEvent"
        #endif
    },
	// RaiseException
    {
        (OOVPA*)&RaiseException_1_0_4627,
        XTL::EmuRaiseException,

        #ifdef _DEBUG_TRACE
        "EmuRaiseException"
        #endif
    },
	// QueueUserAPC (* unchanged since 3911 *)
	{ 
		(OOVPA*)&QueueUserAPC_1_0_3911,

		XTL::EmuQueueUserAPC,
			
		#ifdef _DEBUG_TRACE
		"EmuQueueUserAPC"
		#endif
	},
	// XMountAlternateTitleA
	{
		(OOVPA*)&XMountAlternateTitleA_1_0_4627,
		XTL::EmuXMountAlternateTitle,

		#ifdef _DEBUG_TRACE
		"XMountAlternateTitleA"
		#endif
	},
	// XMountAlternateTitleA
	{
		(OOVPA*)&XMountAlternateTitleA_1_0_4928,
		XTL::EmuXMountAlternateTitle,

		#ifdef _DEBUG_TRACE
		"XMountAlternateTitleA (4928)"
		#endif
	},
	// XUnmountAlternateTitleA
	{
		(OOVPA*)&XUnmountAlternateTitleA_1_0_4627,
	    XTL::EmuXUnmountAlternateTitle,

		#ifdef _DEBUG_TRACE
		"XUnmountAlternateTitleA"
		#endif
	},
	// GetFileAttributesA (* unchanged since 4134 *)
    {
        (OOVPA*)&GetFileAttributesA_1_0_4134,
        XTL::EmuGetFileAttributesA,

        #ifdef _DEBUG_TRACE
        "EmuGetFileAttributesA"
        #endif
    },
	// XGetFileCacheSize
    {
        (OOVPA*)&XGetFileCacheSize_1_0_4627,
        XTL::EmuXGetFileCacheSize,

        #ifdef _DEBUG_TRACE
        "XGetFileCacheSize"
        #endif
	},
	// XInputGetDeviceDescription
	{
        (OOVPA*)&XInputGetDeviceDescription_1_0_4831,
        XTL::EmuXInputGetDeviceDescription,

        #ifdef _DEBUG_TRACE
        "XInputGetDeviceDescription_4831"
        #endif
	},
	// MoveFileA
	/*{
		(OOVPA*)&MoveFileA_1_0_4627,
		XTL::EmuMoveFileA,

		#ifdef _DEBUG_TRACE
		"MoveFileA"
		#endif
	},*/
};

// ******************************************************************
// * XAPI_1_0_4627_SIZE
// ******************************************************************
uint32 XAPI_1_0_4627_SIZE = sizeof(XAPI_1_0_4627);
