// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->Xapi.1.0.3911.cpp
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
// * GetExitCodeThread
// ******************************************************************
SOOVPA<11> GetExitCodeThread_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // GetExitCodeThread+0x03 : lea eax, [ebp+0x08]
        { 0x03, 0x8D }, // (Offset,Value)-Pair #1
        { 0x04, 0x45 }, // (Offset,Value)-Pair #2
        { 0x05, 0x08 }, // (Offset,Value)-Pair #3

        // GetExitCodeThread+0x1A : mov ecx, dword ptr [ebp+0x08]
        { 0x1A, 0x8B }, // (Offset,Value)-Pair #4
        { 0x1B, 0x4D }, // (Offset,Value)-Pair #5
        { 0x1C, 0x08 }, // (Offset,Value)-Pair #6

        // GetExitCodeThread+0x2B : mov eax, 0x0103
        { 0x2B, 0xB8 }, // (Offset,Value)-Pair #7
        { 0x2C, 0x03 }, // (Offset,Value)-Pair #8
        { 0x2D, 0x01 }, // (Offset,Value)-Pair #9

        // GetExitCodeThread+0x49 : retn 0x08
        { 0x49, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4A, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * RtlCreateHeap
// ******************************************************************
SOOVPA<10> RtlCreateHeap_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlCreateHeap+0x13 : push 0x0C
        { 0x13, 0x6A }, // (Offset,Value)-Pair #1
        { 0x14, 0x0C }, // (Offset,Value)-Pair #2

        // RtlCreateHeap+0x1B : rep stosd
        { 0x1B, 0xF3 }, // (Offset,Value)-Pair #3
        { 0x1C, 0xAB }, // (Offset,Value)-Pair #4

        // RtlCreateHeap+0x4E : retn
        { 0x4E, 0xC3 }, // (Offset,Value)-Pair #5

        // RtlCreateHeap+0x8D : jnz +0x08
        { 0x8D, 0x75 }, // (Offset,Value)-Pair #6
        { 0x8E, 0x08 }, // (Offset,Value)-Pair #7

        // RtlCreateHeap+0xA4 : cmp [ebp-38h], esi
        { 0xA4, 0x39 }, // (Offset,Value)-Pair #8
        { 0xA5, 0x75 }, // (Offset,Value)-Pair #9
        { 0xA6, 0xC8 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * RtlAllocateHeap
// ******************************************************************
SOOVPA<11> RtlAllocateHeap_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlAllocateHeap+0x1E : or ecx, [esi+0x18]
        { 0x1E, 0x0B }, // (Offset,Value)-Pair #1
        { 0x1F, 0x4E }, // (Offset,Value)-Pair #2
        { 0x20, 0x18 }, // (Offset,Value)-Pair #3

        // RtlAllocateHeap+0x29 : jnz +0x01
        { 0x29, 0x75 }, // (Offset,Value)-Pair #4
        { 0x2A, 0x01 }, // (Offset,Value)-Pair #5

        // RtlAllocateHeap+0x2B : inc eax
        { 0x2B, 0x40 }, // (Offset,Value)-Pair #6

        // RtlAllocateHeap+0x73 : sub eax, 0x08
        { 0x73, 0x83 }, // (Offset,Value)-Pair #7
        { 0x74, 0xE8 }, // (Offset,Value)-Pair #8
        { 0x75, 0x08 }, // (Offset,Value)-Pair #9

        // RtlAllocateHeap+0xA5 : shl edi, cl
        { 0xA5, 0xD3 }, // (Offset,Value)-Pair #10
        { 0xA6, 0xE7 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * RtlFreeHeap
// ******************************************************************
SOOVPA<10> RtlFreeHeap_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlFreeHeap+0x1F : test ecx, ecx
        { 0x1F, 0x85 }, // (Offset,Value)-Pair #1
        { 0x20, 0xC9 }, // (Offset,Value)-Pair #2

        // RtlFreeHeap+0x23 : mov al, 1
        { 0x23, 0xB0 }, // (Offset,Value)-Pair #3
        { 0x24, 0x01 }, // (Offset,Value)-Pair #4

        // RtlFreeHeap+0x30 : lea edi, [ecx-0x10]
        { 0x30, 0x8D }, // (Offset,Value)-Pair #5
        { 0x31, 0x79 }, // (Offset,Value)-Pair #6
        { 0x32, 0xF0 }, // (Offset,Value)-Pair #7

        // RtlFreeHeap+0x7B : and ecx, 0x10
        { 0x7B, 0x83 }, // (Offset,Value)-Pair #8
        { 0x7C, 0xE1 }, // (Offset,Value)-Pair #9
        { 0x7D, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * RtlReAllocateHeap
// ******************************************************************
SOOVPA<10> RtlReAllocateHeap_1_0_3911 =
{
    0,  // Large == 0
    10,  // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlReAllocateHeap+0x1C : test esi, esi
        { 0x1C, 0x85 }, // (Offset,Value)-Pair #1
        { 0x1D, 0xF6 }, // (Offset,Value)-Pair #2

        // RtlReAllocateHeap+0x26 : mov edx, [ebp+0x14]
        { 0x26, 0x8B }, // (Offset,Value)-Pair #3
        { 0x27, 0x55 }, // (Offset,Value)-Pair #4
        { 0x28, 0x14 }, // (Offset,Value)-Pair #5

        // RtlReAllocateHeap+0x5E : jz +0x06; add ecx, 0x10
        { 0x5E, 0x74 }, // (Offset,Value)-Pair #6
        { 0x5F, 0x06 }, // (Offset,Value)-Pair #7
        { 0x60, 0x83 }, // (Offset,Value)-Pair #8
        { 0x61, 0xC1 }, // (Offset,Value)-Pair #9
        { 0x62, 0x10 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * RtlSizeHeap
// ******************************************************************
SOOVPA<11> RtlSizeHeap_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // RtlSizeHeap+0x04 : mov al, [ecx-0x0B]
        { 0x04, 0x8A }, // (Offset,Value)-Pair #1
        { 0x05, 0x41 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF5 }, // (Offset,Value)-Pair #3

        // RtlSizeHeap+0x07 : test al, 1
        { 0x07, 0xA8 }, // (Offset,Value)-Pair #4
        { 0x08, 0x01 }, // (Offset,Value)-Pair #5

        // RtlSizeHeap+0x14 : movzx edx, word ptr [ecx-0x10]
        { 0x14, 0x0F }, // (Offset,Value)-Pair #6
        { 0x15, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x16, 0x51 }, // (Offset,Value)-Pair #8
        { 0x17, 0xF0 }, // (Offset,Value)-Pair #9

        // RtlSizeHeap+0x2A : sub eax, ecx
        { 0x2A, 0x2B }, // (Offset,Value)-Pair #10
        { 0x2B, 0xC1 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XInitDevices
// ******************************************************************
// * NOTE: We are actually intercepting USBD_Init, because
// *       XInitDevices Simply redirects to that function
// ******************************************************************
SOOVPA<10> XInitDevices_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInitDevices+0x03 : push 0xB4
        { 0x03, 0x68 }, // (Offset,Value)-Pair #1
        { 0x04, 0xB4 }, // (Offset,Value)-Pair #2

        // XInitDevices+0x10 : jmp +0x13
        { 0x10, 0x74 }, // (Offset,Value)-Pair #3
        { 0x11, 0x13 }, // (Offset,Value)-Pair #4

        // XInitDevices+0x5B : movzx eax, byte ptr [esi+0xA1]
        { 0x5B, 0x0F }, // (Offset,Value)-Pair #5
        { 0x5C, 0xB6 }, // (Offset,Value)-Pair #6
        { 0x5D, 0x86 }, // (Offset,Value)-Pair #7
        { 0x5E, 0xA1 }, // (Offset,Value)-Pair #8

        // XInitDevices+0x8B : retn 8
        { 0x8B, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x8C, 0x08 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * CreateMutex
// ******************************************************************
SOOVPA<13> CreateMutex_1_0_3911 =
{
    0,  // Large == 0
    13,  // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // CreateMutex+0x03 : sub esp, 0x14
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x14 }, // (Offset,Value)-Pair #3

        // CreateMutex+0x32 : cmp eax, 0x40000000
        { 0x32, 0x3D }, // (Offset,Value)-Pair #4
        { 0x33, 0x00 }, // (Offset,Value)-Pair #5
        { 0x34, 0x00 }, // (Offset,Value)-Pair #6
        { 0x35, 0x00 }, // (Offset,Value)-Pair #7
        { 0x36, 0x40 }, // (Offset,Value)-Pair #8

        // CreateMutex+0x39 : push 0xB7
        { 0x39, 0x68 }, // (Offset,Value)-Pair #9
        { 0x3A, 0xB7 }, // (Offset,Value)-Pair #10

        // CreateMutex+0x47 : mov eax, [ebp+0x10]
        { 0x47, 0x8B }, // (Offset,Value)-Pair #11
        { 0x48, 0x45 }, // (Offset,Value)-Pair #12
        { 0x49, 0x10 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * CreateThread
// ******************************************************************
SOOVPA<8> CreateThread_1_0_3911 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // CreateThread+0x0A : mov eax, ds:10130h
        { 0x0A, 0xA1 }, // (Offset,Value)-Pair #1
        { 0x0B, 0x30 }, // (Offset,Value)-Pair #2
        { 0x0C, 0x01 }, // (Offset,Value)-Pair #3

        // CreateThread+0x1C : and ecx, 0xFFFFFF01
        { 0x1C, 0x81 }, // (Offset,Value)-Pair #4
        { 0x1D, 0xE1 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x01 }, // (Offset,Value)-Pair #6

        // CreateThread+0x51 : retn 0x18
        { 0x51, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x52, 0x18 }  // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * SetThreadPriority
// ******************************************************************
SOOVPA<10> SetThreadPriority_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // SetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF }, // (Offset,Value)-Pair #1
        { 0x0E, 0x75 }, // (Offset,Value)-Pair #2
        { 0x0F, 0x08 }, // (Offset,Value)-Pair #3

        // SetThreadPriority+0x18 : jl +0x2C
        { 0x18, 0x7C }, // (Offset,Value)-Pair #4
        { 0x19, 0x2C }, // (Offset,Value)-Pair #5

        // SetThreadPriority+0x22 : push 0x10
        { 0x22, 0x6A }, // (Offset,Value)-Pair #6
        { 0x23, 0x10 }, // (Offset,Value)-Pair #7

        // SetThreadPriority+0x26 : cmp eax, 0xFFFFFFF1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #8
        { 0x27, 0xF8 }, // (Offset,Value)-Pair #9
        { 0x28, 0xF1 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
SOOVPA<7> XapiInitProcess_1_0_3911 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XapiInitProcess+0x03 : sub esp, 30h
        { 0x05, 0x30 }, // (Offset,Value)-Pair #1

        // XapiInitProcess+0x0F : push 0x0C
        { 0x0F, 0x6A }, // (Offset,Value)-Pair #2
        { 0x10, 0x0C }, // (Offset,Value)-Pair #3

        // XapiInitProcess+0x17 : repe stosd
        { 0x17, 0xF3 }, // (Offset,Value)-Pair #4
        { 0x18, 0xAB }, // (Offset,Value)-Pair #5

        // XapiInitProcess+0x42 : jnz +0x0A
        { 0x42, 0x75 }, // (Offset,Value)-Pair #6
        { 0x43, 0x0A }, // (Offset,Value)-Pair #7
    }
};

// ******************************************************************
// * XapiBootDash
// ******************************************************************
SOOVPA<11> XapiBootDash_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XapiBootDash+0x03 : sub esp, 0x0C00
        { 0x03, 0x81 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x06, 0x0C }, // (Offset,Value)-Pair #3

        // XapiBootDash+0x09 : mov eax, ds:0x10118
        { 0x09, 0xA1 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x18 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x01 }, // (Offset,Value)-Pair #6
        { 0x0C, 0x01 }, // (Offset,Value)-Pair #7

        // XapiBootDash+0x25 : repe stosd
        { 0x25, 0xF3 }, // (Offset,Value)-Pair #8
        { 0x26, 0xAB }, // (Offset,Value)-Pair #9

        // XapiBootDash+0x59 : retn 0x0C
        { 0x59, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x5A, 0x0C }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XRegisterThreadNotifyRoutine
// ******************************************************************
SOOVPA<11> XRegisterThreadNotifyRoutine_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XRegisterThreadNotifyRoutine+0x0D : cmp [esp+0x0C], 0
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0E, 0x7C }, // (Offset,Value)-Pair #2
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #3
        { 0x10, 0x0C }, // (Offset,Value)-Pair #4

        // XRegisterThreadNotifyRoutine+0x12 : jz +0x1C
        { 0x12, 0x74 }, // (Offset,Value)-Pair #5
        { 0x13, 0x1C }, // (Offset,Value)-Pair #6

        // XRegisterThreadNotifyRoutine+0x24 : mov [eax+4], ecx
        { 0x24, 0x89 }, // (Offset,Value)-Pair #7
        { 0x25, 0x48 }, // (Offset,Value)-Pair #8
        { 0x26, 0x04 }, // (Offset,Value)-Pair #9

        // XRegisterThreadNotifyRoutine+0x46 : retn 0x08
        { 0x46, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x47, 0x08 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * GetTimeZoneInformation
// ******************************************************************
SOOVPA<7> GetTimeZoneInformation_1_0_3911 =
{
    0,  // Large == 0
    7,  // Count == 7

    -1, // Xref Not Saved
    0,  // Xref Not Used

    {
        { 0x05, 0x28 },
        { 0x2E, 0x28 },
        { 0x4F, 0x59 },
        { 0x56, 0xAB },
        { 0x8C, 0xC0 },
        { 0xB9, 0x80 },
        { 0xF7, 0x99 },
    }
};

/* not necessary?
// ******************************************************************
// * XCalculateSignatureBegin
// ******************************************************************
SOOVPA<10> XCalculateSignatureBegin_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XCalculateSignatureBegin+0x01 : push 0x7C; push 0
        { 0x01, 0x6A }, // (Offset,Value)-Pair #1
        { 0x02, 0x7C }, // (Offset,Value)-Pair #2
        { 0x03, 0x6A }, // (Offset,Value)-Pair #3
        { 0x04, 0x00 }, // (Offset,Value)-Pair #4

        // XCalculateSignatureBegin+0x10 : push 0x08
        { 0x10, 0x6A }, // (Offset,Value)-Pair #5
        { 0x11, 0x08 }, // (Offset,Value)-Pair #6

        // XCalculateSignatureBegin+0x2E : push 0x10
        { 0x2E, 0x6A }, // (Offset,Value)-Pair #7
        { 0x2F, 0x10 }, // (Offset,Value)-Pair #8

        // XCalculateSignatureBegin+0x3B : retn 0x04
        { 0x3B, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x3C, 0x04 }, // (Offset,Value)-Pair #10
    }
};
*/

// ******************************************************************
// * XGetDevices
// ******************************************************************
SOOVPA<14> XGetDevices_1_0_3911 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XGetDevices+0x07 : mov edx, [esp+arg_0]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
        { 0x08, 0x54 }, // (Offset,Value)-Pair #2
        { 0x09, 0x24 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x08 }, // (Offset,Value)-Pair #4

        // XGetDevices+0x0D : and [edx+4], 0
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0E, 0x62 }, // (Offset,Value)-Pair #6
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #7
        { 0x10, 0x00 }, // (Offset,Value)-Pair #8

        // XGetDevices+0x11 : mov cl, al
        { 0x11, 0x8A }, // (Offset,Value)-Pair #9
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #10

        // XGetDevices+0x16 : call KfLowerIrql
        { 0x16, 0xFF }, // (Offset,Value)-Pair #11
        { 0x17, 0x15 }, // (Offset,Value)-Pair #12

        // XGetDevices+0x1F : retn 4
        { 0x1F, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x20, 0x04 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
SOOVPA<8> XGetDeviceChanges_1_0_3911 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XGetDeviceChanges+0x07 : xor eax, eax
        { 0x07, 0x33 }, // (Offset,Value)-Pair #1
        { 0x08, 0xC0 }, // (Offset,Value)-Pair #2

        // XGetDeviceChanges+0x33 : not edx
        { 0x33, 0xF7 }, // (Offset,Value)-Pair #3
        { 0x34, 0xD2 }, // (Offset,Value)-Pair #4

        // XGetDeviceChanges+0x42 : or edx, edi
        { 0x42, 0x0B }, // (Offset,Value)-Pair #5
        { 0x43, 0xD7 }, // (Offset,Value)-Pair #6

        // XGetDeviceChanges+0x51 : mov cl, al
        { 0x51, 0x8A }, // (Offset,Value)-Pair #7
        { 0x52, 0xC8 }, // (Offset,Value)-Pair #8
    }
};

// ******************************************************************
// * XInputOpen
// ******************************************************************
SOOVPA<11> XInputOpen_1_0_3911 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputOpen+0x20 : jmp +0x0B
        { 0x20, 0xEB }, // (Offset,Value)-Pair #1
        { 0x21, 0x0B }, // (Offset,Value)-Pair #2

        // XInputOpen+0x29 : jnz +0x3D
        { 0x29, 0x75 }, // (Offset,Value)-Pair #3
        { 0x2A, 0x3D }, // (Offset,Value)-Pair #4

        // XInputOpen+0x4A : add edx, 0x10
        { 0x4A, 0x83 }, // (Offset,Value)-Pair #5
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #6
        { 0x4C, 0x10 }, // (Offset,Value)-Pair #7

        // XInputOpen+0x66 : jmp +0x09
        { 0x66, 0xEB }, // (Offset,Value)-Pair #8
        { 0x67, 0x09 }, // (Offset,Value)-Pair #9

        // XInputOpen+0x68 : push 0x57
        { 0x68, 0x6A }, // (Offset,Value)-Pair #10
        { 0x69, 0x57 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
SOOVPA<10> XID_fCloseDevice_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_FCLOSEDEVICE,  // XRef Is  Saved
    0,                  // XRef Not Used

    {
        // XID_fCloseDevice+0x19 : jz +0x5C
        { 0x19, 0x74 }, // (Offset,Value)-Pair #1
        { 0x1A, 0x5C }, // (Offset,Value)-Pair #2

        // XID_fCloseDevice+0x1B : cmp [esi+0x00A3], 1
        { 0x1B, 0x80 }, // (Offset,Value)-Pair #3
        { 0x1D, 0xA3 }, // (Offset,Value)-Pair #4
        { 0x21, 0x01 }, // (Offset,Value)-Pair #5

        // XID_fCloseDevice+0x91 : mov [esi+0x00A4], eax
        { 0x95, 0x89 }, // (Offset,Value)-Pair #6
        { 0x96, 0x86 }, // (Offset,Value)-Pair #7
        { 0x97, 0xA4 }, // (Offset,Value)-Pair #8

        // XID_fCloseDevice+0xA3 : leave; retn
        { 0xA3, 0xC9 }, // (Offset,Value)-Pair #9
        { 0xA4, 0xC3 }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * XInputClose
// ******************************************************************
SOOVPA<8> XInputClose_1_0_3911 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
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
// * XInputGetCapabilities
// ******************************************************************
SOOVPA<14> XInputGetCapabilities_1_0_3911 =
{
    0,  // Large == 0
    14, // Count == 14

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputGetCapabilities+0x03 : sub esp, 0x48
        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
        { 0x05, 0x48 }, // (Offset,Value)-Pair #3

        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
        { 0x24, 0xF6 }, // (Offset,Value)-Pair #4
        { 0x25, 0x46 }, // (Offset,Value)-Pair #5
        { 0x26, 0x04 }, // (Offset,Value)-Pair #6
        { 0x27, 0x02 }, // (Offset,Value)-Pair #7

        // XInputGetCapabilities+0x3A : stosb
        { 0x3A, 0xAA }, // (Offset,Value)-Pair #8

        // XInputGetCapabilities+0x8A : mov [ebp+var_48], 0x30
        { 0x8A, 0xC6 }, // (Offset,Value)-Pair #9
        { 0x8B, 0x45 }, // (Offset,Value)-Pair #10
        { 0x8C, 0xB8 }, // (Offset,Value)-Pair #11
        { 0x8D, 0x30 }, // (Offset,Value)-Pair #12

        // XInputGetCapabilities+0xFB : cmp [eax], ebx
        { 0xFB, 0x39 }, // (Offset,Value)-Pair #13
        { 0xFC, 0x18 }, // (Offset,Value)-Pair #14
    }
};

// ******************************************************************
// * XInputGetState
// ******************************************************************
SOOVPA<13> XInputGetState_1_0_3911 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputGetState+0x0E : cmp byte ptr [edx+0x0A3], 1
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #1
        { 0x0F, 0xBA }, // (Offset,Value)-Pair #2
        { 0x10, 0xA3 }, // (Offset,Value)-Pair #3
        { 0x14, 0x01 }, // (Offset,Value)-Pair #4

        // XInputGetState+0x17 : push 0x57
        { 0x17, 0x6A }, // (Offset,Value)-Pair #5
        { 0x18, 0x57 }, // (Offset,Value)-Pair #6

        // XInputGetState+0x1A : jmp +0x46
        { 0x1A, 0xEB }, // (Offset,Value)-Pair #7
        { 0x1B, 0x46 }, // (Offset,Value)-Pair #8

        // XInputGetState+0x28 : mov ebx, 0x048F
        { 0x28, 0xBB }, // (Offset,Value)-Pair #9
        { 0x29, 0x8F }, // (Offset,Value)-Pair #10
        { 0x2A, 0x04 }, // (Offset,Value)-Pair #11

        // XInputGetState+0x6E : retn 8
        { 0x6E, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x6F, 0x08 }, // (Offset,Value)-Pair #13
    }
};

// ******************************************************************
// * XInputSetState
// ******************************************************************
SOOVPA<12> XInputSetState_1_0_3911 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x81 }, // (Offset,Value)-Pair #2
        { 0x06, 0xA3 }, // (Offset,Value)-Pair #3

        // XInputSetState+0x0F : push 0x57
        { 0x0F, 0x6A }, // (Offset,Value)-Pair #4
        { 0x10, 0x57 }, // (Offset,Value)-Pair #5

        // XInputSetState+0x12 : jmp +0x21
        { 0x12, 0xEB }, // (Offset,Value)-Pair #6
        { 0x13, 0x21 }, // (Offset,Value)-Pair #7

        // XInputSetState+0x2D : mov [edx+0x41], al
        { 0x2D, 0x88 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x42 }, // (Offset,Value)-Pair #9
        { 0x2F, 0x41 }, // (Offset,Value)-Pair #10

        // XInputSetState+0x35 : retn 0x08
        { 0x35, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x36, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
SOOVPA<10> SetThreadPriorityBoost_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

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
SOOVPA<10> GetThreadPriority_1_0_3911 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    0,  // XRef Not Used

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

//// ******************************************************************
//// * XGetDevices
//// ******************************************************************
//SOOVPA<14> XGetDevices_1_0_3911 =
//{
//    0,  // Large == 0
//    14, // Count == 14
//
//    -1, // XRef Not Saved
//    0,  // XRef Not Used
//
//    {
//        // XGetDevices+0x07 : mov edx, [esp+arg_0]
//        { 0x07, 0x8B }, // (Offset,Value)-Pair #1
//        { 0x08, 0x54 }, // (Offset,Value)-Pair #2
//        { 0x09, 0x24 }, // (Offset,Value)-Pair #3
//        { 0x0A, 0x08 }, // (Offset,Value)-Pair #4
//
//        // XGetDevices+0x0D : and [edx+4], 0
//        { 0x0D, 0x83 }, // (Offset,Value)-Pair #5
//        { 0x0E, 0x62 }, // (Offset,Value)-Pair #6
//        { 0x0F, 0x04 }, // (Offset,Value)-Pair #7
//        { 0x10, 0x00 }, // (Offset,Value)-Pair #8
//
//        // XGetDevices+0x11 : mov cl, al
//        { 0x11, 0x8A }, // (Offset,Value)-Pair #9
//        { 0x12, 0xC8 }, // (Offset,Value)-Pair #10
//
//        // XGetDevices+0x16 : call KfLowerIrql
//        { 0x16, 0xFF }, // (Offset,Value)-Pair #11
//        { 0x17, 0x15 }, // (Offset,Value)-Pair #12
//
//        // XGetDevices+0x1F : retn 4
//        { 0x1F, 0xC2 }, // (Offset,Value)-Pair #13
//        { 0x20, 0x04 }, // (Offset,Value)-Pair #14
//    }
//};

// ******************************************************************
// * CreateFiber
// ******************************************************************
SOOVPA<11> CreateFiber_1_0_3911 = 
{
	0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
		// CreateFiber+0x11 : mov ecx, 0x3000
		{ 0x11, 0xB9 },
		{ 0x12, 0x00 },
		{ 0x13, 0x30 },
		{ 0x14, 0x00 },
		{ 0x15, 0x00 },

		// CreateFiber+0x2A : call ds:MmCreateKernelStack
		{ 0x2A, 0xFF },
		{ 0x2B, 0x15 },
		{ 0x2C, 0xF8 },
		{ 0x2D, 0xD1 },
		{ 0x2E, 0x1E },
		{ 0x2F, 0x00 }
	}
};

// ******************************************************************
// * DeleteFiber
// ******************************************************************
SOOVPA<9> DeleteFiber_1_0_3911 = 
{
	0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
		// DeleteFiber+0x0A : call ds:MmDeleteKernelStack
		{ 0x0A, 0xFF },
		{ 0x0B, 0x15 },
		{ 0x0C, 0xFC },
		{ 0x0D, 0xD1 },
		{ 0x0E, 0x1E },
		{ 0x0F, 0x00 },

		// DeleteFiber+0x10 : retn 4
		{ 0x10, 0xC2 },
		{ 0x11, 0x04 },
		{ 0x12, 0x00 },
	}
};

//// ******************************************************************
//// * XInputGetCapabilities
//// ******************************************************************
//SOOVPA<14> XInputGetCapabilities_1_0_3911 =
//{
//    0,  // Large == 0
//    14, // Count == 14
//
//    -1, // XRef Not Saved
//    0,  // XRef Not Used
//
//    {
//        // XInputGetCapabilities+0x03 : sub esp, 0x48
//        { 0x03, 0x83 }, // (Offset,Value)-Pair #1
//        { 0x04, 0xEC }, // (Offset,Value)-Pair #2
//        { 0x05, 0x48 }, // (Offset,Value)-Pair #3
//
//        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
//        { 0x24, 0xF6 }, // (Offset,Value)-Pair #4
//        { 0x25, 0x46 }, // (Offset,Value)-Pair #5
//        { 0x26, 0x04 }, // (Offset,Value)-Pair #6
//        { 0x27, 0x02 }, // (Offset,Value)-Pair #7
//
//        // XInputGetCapabilities+0x3A : stosb
//        { 0x3A, 0xAA }, // (Offset,Value)-Pair #8
//
//        // XInputGetCapabilities+0x8A : mov [ebp+var_48], 0x30
//        { 0x8A, 0xC6 }, // (Offset,Value)-Pair #9
//        { 0x8B, 0x45 }, // (Offset,Value)-Pair #10
//        { 0x8C, 0xB8 }, // (Offset,Value)-Pair #11
//        { 0x8D, 0x30 }, // (Offset,Value)-Pair #12
//
//        // XInputGetCapabilities+0xFB : cmp [eax], ebx
//        { 0xFB, 0x39 }, // (Offset,Value)-Pair #13
//        { 0xFC, 0x18 }, // (Offset,Value)-Pair #14
//    }
//};

// ******************************************************************
// * XLoadSectionA
// ******************************************************************
SOOVPA<10> XLoadSectionA_1_0_3911 = 
{
	0,	// Large == 0
	10,	// Count == 10

	-1,	// XRef not saved
	0,	// XRef not used

	{
		// XLoadSectionA+0x0F : call XLoadSectionByHandle
//		{ 0x10, XREF_XLoadSectionByHandle },

		// XLoadSectionA+0x0: push dword ptr [esp+4]
		{ 0x00, 0xFF },
		{ 0x01, 0x74 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },

		// XLoadSectionA+0x9: cmp eax, 0xFFFFFFFF
		{ 0x09, 0x83 },
		{ 0x0A, 0xF8 },
		{ 0x0B, 0xFF },

		// XLoadSectionA+0xE: push eax
		{ 0x0E, 0x50 },

		// XLoadSectionA+0x18: ret 4
		{ 0x18, 0xC2 },
		{ 0x19, 0x04 },
	}
};

// ******************************************************************
// * XFreeSectionA
// ******************************************************************
SOOVPA<11> XFreeSectionA_1_0_3911 = 
{
	0,	// Large == 0
	11,	// Count == 11

	-1,	// XRef not saved
	0,	// XRef not used

	{
		// XFreeSectionA+0x0F : call XFreeSectionByHandle
//		{ 0x10, XREF_XFreeSectionByHandle },

		// XLoadSectionA+0x0: push dword ptr [esp+4]
		{ 0x00, 0xFF },
		{ 0x01, 0x74 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },

		// XLoadSectionA+0x9: cmp eax, 0xFFFFFFFF
		{ 0x09, 0x83 },
		{ 0x0A, 0xF8 },
		{ 0x0B, 0xFF },

		// XLoadSectionA+0x14: jz 0x2
		{ 0x14, 0xEB },
		{ 0x15, 0x02 },

		// XLoadSectionA+0x18: ret 4
		{ 0x18, 0xC2 },
		{ 0x19, 0x04 },
	}
};

// ******************************************************************
// * XAPI_1_0_3911
// ******************************************************************
OOVPATable XAPI_1_0_3911[] =
{
    // GetExitCodeThread
    {
        (OOVPA*)&GetExitCodeThread_1_0_3911,

        XTL::EmuGetExitCodeThread,

        #ifdef _DEBUG_TRACE
        "EmuGetExitCodeThread"
        #endif
    },
    // RtlCreateHeap
    {
        (OOVPA*)&RtlCreateHeap_1_0_3911,

        XTL::EmuRtlCreateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlCreateHeap"
        #endif
    },
    // RtlAllocateHeap
    {
        (OOVPA*)&RtlAllocateHeap_1_0_3911,

        XTL::EmuRtlAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlAllocateHeap"
        #endif
    },
    // RtlFreeHeap
    {
        (OOVPA*)&RtlFreeHeap_1_0_3911,

        XTL::EmuRtlFreeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlFreeHeap"
        #endif
    },
    //*
    // RtlReAllocateHeap
    {
        (OOVPA*)&RtlReAllocateHeap_1_0_3911,

        XTL::EmuRtlReAllocateHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlReAllocateHeap"
        #endif
    },
    //*/
    //*
    // RtlSizeHeap
    {
        (OOVPA*)&RtlSizeHeap_1_0_3911,

        XTL::EmuRtlSizeHeap,

        #ifdef _DEBUG_TRACE
        "EmuRtlSizeHeap"
        #endif
    },
    //*/
    // XInitDevices
    {
        (OOVPA*)&XInitDevices_1_0_3911,

        XTL::EmuXInitDevices,

        #ifdef _DEBUG_TRACE
        "EmuXInitDevices"
        #endif
    },
    /* Too High Level
    // CreateMutex
    {
        (OOVPA*)&CreateMutex_1_0_3911,

        XTL::EmuCreateMutex,

        #ifdef _DEBUG_TRACE
        "EmuCreateMutex"
        #endif
    },
    //*/
    /* Too High Level
    // CreateThread
    {
        (OOVPA*)&CreateThread_1_0_3911,

        XTL::EmuCreateThread,

        #ifdef _DEBUG_TRACE
        "EmuCreateThread"
        #endif
    },
    */
    // SetThreadPriority
    {
        (OOVPA*)&SetThreadPriority_1_0_3911,

        XTL::EmuSetThreadPriority,

        #ifdef _DEBUG_TRACE
        "EmuSetThreadPriority"
        #endif
    },
    //* Too High Level
    // XapiInitProcess
    {
        (OOVPA*)&XapiInitProcess_1_0_3911,

        XTL::EmuXapiInitProcess,

        #ifdef _DEBUG_TRACE
        "EmuXapiInitProcess"
        #endif
    },
    //*/
    // GetTimeZoneInformation
    {
        (OOVPA*)&GetTimeZoneInformation_1_0_3911,

        XTL::EmuGetTimeZoneInformation,

        #ifdef _DEBUG_TRACE
        "GetTimeZoneInformation"
        #endif
    },
    // XapiBootToDash
    {
        (OOVPA*)&XapiBootDash_1_0_3911,

        XTL::EmuXapiBootDash,

        #ifdef _DEBUG_TRACE
        "EmuXapiBootDash"
        #endif
    },
    // XRegisterThreadNotifyRoutine
    {
        (OOVPA*)&XRegisterThreadNotifyRoutine_1_0_3911,

        XTL::EmuXRegisterThreadNotifyRoutine,

        #ifdef _DEBUG_TRACE
        "EmuXRegisterThreadNotifyRoutine"
        #endif
    },
    /*
    // XCalculateSignatureBegin
    {
        (OOVPA*)&XCalculateSignatureBegin_1_0_3911,

        XTL::EmuXCalculateSignatureBegin,

        #ifdef _DEBUG_TRACE
        "EmuXCalculateSignatureBegin"
        #endif
    },
    */
    // XGetDevices
    {
        (OOVPA*)&XGetDevices_1_0_3911,

        XTL::EmuXGetDevices,

        #ifdef _DEBUG_TRACE
        "EmuXGetDevices"
        #endif
    },
    // XGetDeviceChanges
    {
        (OOVPA*)&XGetDeviceChanges_1_0_3911,

        XTL::EmuXGetDeviceChanges,

        #ifdef _DEBUG_TRACE
        "EmuXGetDeviceChanges"
        #endif
    },
    // XInputOpen
    {
        (OOVPA*)&XInputOpen_1_0_3911,

        XTL::EmuXInputOpen,

        #ifdef _DEBUG_TRACE
        "EmuXInputOpen"
        #endif
    },
    // XID_fCloseDevice
    {
        (OOVPA*)&XID_fCloseDevice_1_0_3911, 0,

        #ifdef _DEBUG_TRACE
        "XID_fCloseDevice (XREF)"
        #endif
    },
    // XInputClose
    {
        (OOVPA*)&XInputClose_1_0_3911,

        XTL::EmuXInputClose,

        #ifdef _DEBUG_TRACE
        "EmuXInputClose"
        #endif
    },
    // XInputGetCapabilities
    {
        (OOVPA*)&XInputGetCapabilities_1_0_3911,

        XTL::EmuXInputGetCapabilities,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetCapabilities"
        #endif
    },
    // XInputGetState
    {
        (OOVPA*)&XInputGetState_1_0_3911,

        XTL::EmuXInputGetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputGetState"
        #endif
    },
    // XInputSetState
    {
        (OOVPA*)&XInputSetState_1_0_3911,

        XTL::EmuXInputSetState,

        #ifdef _DEBUG_TRACE
        "EmuXInputSetState"
        #endif
    },
	// SetThreadPriorityBoost
	{
		(OOVPA*)&SetThreadPriorityBoost_1_0_3911,

		XTL::EmuSetThreadPriorityBoost,

		#ifdef _DEBUG_TRACE
		"EmuSetThreadPriorityBoost"
		#endif
	},
	// GetThreadPriority
	{
		(OOVPA*)&GetThreadPriority_1_0_3911,

		XTL::EmuGetThreadPriority,

		#ifdef _DEBUG_TRACE
		"EmuGetThreadPriority"
		#endif
	},
	// XGetDevices
	{
		(OOVPA*)&XGetDevices_1_0_3911,

		XTL::EmuXGetDevices,

		#ifdef _DEBUG_TRACE
		"EmuXGetDevices"
		#endif
	},
	// CreateFiber
	{
		(OOVPA*)&CreateFiber_1_0_3911,

		XTL::EmuCreateFiber,

		#ifdef _DEBUG_TRACE
		"EmuCreateFiber"
		#endif
	},
	// DeleteFiber
	{
		(OOVPA*)&DeleteFiber_1_0_3911,

		XTL::EmuDeleteFiber,

		#ifdef _DEBUG_TRACE
		"EmuDeleteFiber"
		#endif
	},
	// XInputGetCapabilities
	{
		(OOVPA*)&XInputGetCapabilities_1_0_3911,

		XTL::EmuXInputGetCapabilities,

		#ifdef _DEBUG_TRACE
		"EmuXInputGetCapabilities"
		#endif
	},
	// XLoadSectionA
	{
		(OOVPA*)&XLoadSectionA_1_0_3911, 
			
		XTL::EmuXLoadSectionA,

		#ifdef _DEBUG_TRACE
		"EmuXLoadSectionA"
		#endif
	},
	// XFreeSectionA
	{
		(OOVPA*)&XFreeSectionA_1_0_3911, 
			
		XTL::EmuXFreeSectionA,

		#ifdef _DEBUG_TRACE
		"EmuXFreeSectionA"
		#endif
	},
};

// ******************************************************************
// * XAPI_1_0_3911_SIZE
// ******************************************************************
uint32 XAPI_1_0_3911_SIZE = sizeof(XAPI_1_0_3911);
