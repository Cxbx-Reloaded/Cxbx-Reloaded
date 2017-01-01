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
OOVPA_NO_XREF(GetExitCodeThread_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * RtlCreateHeap
// ******************************************************************
OOVPA_NO_XREF(RtlCreateHeap_1_0_3911, 10)

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
OOVPA_END;

// ******************************************************************
// * RtlAllocateHeap
// ******************************************************************
OOVPA_NO_XREF(RtlAllocateHeap_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * RtlFreeHeap
// ******************************************************************
OOVPA_NO_XREF(RtlFreeHeap_1_0_3911, 10)

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
OOVPA_END;

// ******************************************************************
// * RtlReAllocateHeap
// ******************************************************************
OOVPA_NO_XREF(RtlReAllocateHeap_1_0_3911, 10)

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
OOVPA_END;

// ******************************************************************
// * RtlSizeHeap
// ******************************************************************
OOVPA_NO_XREF(RtlSizeHeap_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * XInitDevices
// ******************************************************************
// * NOTE: We are actually intercepting USBD_Init, because
// *       XInitDevices Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(XInitDevices_1_0_3911, 10)

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
OOVPA_END;

// ******************************************************************
// * CreateMutex
// ******************************************************************
OOVPA_NO_XREF(CreateMutex_1_0_3911, 13)

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
OOVPA_END;

// ******************************************************************
// * CreateThread
// ******************************************************************
OOVPA_NO_XREF(CreateThread_1_0_3911, 8)

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
        { 0x52, 0x18 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * SetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriority_1_0_3911, 10)

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
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess_1_0_3911, 7)

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
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess_1_0_3950, 7)

        // XapiInitProcess+0x03 : sub esp, 30h
        { 0x05, 0x30 }, // (Offset,Value)-Pair #1

        // XapiInitProcess+0x0F : push 0x0C
        { 0x10, 0x6A }, // (Offset,Value)-Pair #2
        { 0x11, 0x0C }, // (Offset,Value)-Pair #3

        // XapiInitProcess+0x17 : repe stosd
        { 0x18, 0xF3 }, // (Offset,Value)-Pair #4
        { 0x19, 0xAB }, // (Offset,Value)-Pair #5

        // XapiInitProcess+0x42 : jnz +0x0A
        { 0x43, 0x75 }, // (Offset,Value)-Pair #6
        { 0x44, 0x0A }, // (Offset,Value)-Pair #7
OOVPA_END;

// ******************************************************************
// * XapiBootDash
// ******************************************************************
OOVPA_NO_XREF(XapiBootDash_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * XRegisterThreadNotifyRoutine
// ******************************************************************
OOVPA_NO_XREF(XRegisterThreadNotifyRoutine_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * GetTimeZoneInformation
// ******************************************************************
OOVPA_NO_XREF(GetTimeZoneInformation_1_0_3911, 7)

        { 0x05, 0x28 },
        { 0x2E, 0x28 },
        { 0x4F, 0x59 },
        { 0x56, 0xAB },
        { 0x8C, 0xC0 },
        { 0xB9, 0x80 },
        { 0xF7, 0x99 },
OOVPA_END;

/* not necessary?
// ******************************************************************
// * XCalculateSignatureBegin
// ******************************************************************
OOVPA_NO_XREF(XCalculateSignatureBegin_1_0_3911, 10)

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
OOVPA_END;
*/

// ******************************************************************
// * XGetDevices
// ******************************************************************
OOVPA_NO_XREF(XGetDevices_1_0_3911, 14)

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
OOVPA_END;

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceChanges_1_0_3911, 8)

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
OOVPA_END;

// ******************************************************************
// * XInputOpen
// ******************************************************************
OOVPA_NO_XREF(XInputOpen_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice_1_0_3911, 10,

    XREF_FCLOSEDEVICE,
    XRefZero)

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
OOVPA_END;

// ******************************************************************
// * XInputClose
// ******************************************************************
OOVPA_XREF(XInputClose_1_0_3911, 8,

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
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities_1_0_3911, 14)

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
OOVPA_END;

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState_1_0_3911, 13)

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
OOVPA_END;

// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState_1_0_3911, 12)

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
OOVPA_END;

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriorityBoost_1_0_3911, 10)

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
OOVPA_NO_XREF(GetThreadPriority_1_0_3911, 10)

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

//// ******************************************************************
//// * XGetDevices
//// ******************************************************************
//OOVPA_NO_XREF(XGetDevices_1_0_3911, 14)
//
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
//OOVPA_END;

// ******************************************************************
// * CreateFiber
// ******************************************************************
OOVPA_NO_XREF(CreateFiber_1_0_3911, 11)

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
OOVPA_END;

// ******************************************************************
// * DeleteFiber
// ******************************************************************
OOVPA_NO_XREF(DeleteFiber_1_0_3911, 9)

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
OOVPA_END;

// ******************************************************************
// * SwitchToFiber
// ******************************************************************
OOVPA_NO_XREF(SwitchToFiber_1_0_3911, 8)

        { 0x06, 0x64 },
        { 0x0E, 0x44 },
        { 0x16, 0xFF },
        { 0x1E, 0x91 },
        { 0x26, 0x61 },
        { 0x31, 0x64 },
        { 0x36, 0x00 },
        { 0x3E, 0x00 },
OOVPA_END;

// ******************************************************************
// * ConvertThreadToFiber
// ******************************************************************
OOVPA_NO_XREF(ConvertThreadToFiber_1_0_3911, 7)

        { 0x05, 0x64 },
        { 0x0C, 0x8B },
        { 0x13, 0x8D },
        { 0x1A, 0x01 },
        { 0x21, 0x8B },
        { 0x28, 0x89 },
        { 0x31, 0x8B },
OOVPA_END;

//// ******************************************************************
//// * XInputGetCapabilities
//// ******************************************************************
//OOVPA_NO_XREF(XInputGetCapabilities_1_0_3911, 14)
//
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
//OOVPA_END;

// ******************************************************************
// * SignalObjectAndWait
// ******************************************************************
OOVPA_NO_XREF(SignalObjectAndWait_1_0_3911, 8)

        { 0x07, 0x75 },
        { 0x12, 0x8B },
        { 0x19, 0x01 },
        { 0x26, 0x85 },
        { 0x2B, 0x7D },
        { 0x34, 0x00 },
        { 0x3F, 0x83 },
        { 0x46, 0x00 },
OOVPA_END;

// ******************************************************************
// * QueueUserAPC
// ******************************************************************
OOVPA_NO_XREF(QueueUserAPC_1_0_3911, 7)

        { 0x03, 0x74 },
        { 0x08, 0x24 },
        { 0x0F, 0xFF },
        { 0x12, 0x18 },
        { 0x19, 0x33 },
        { 0x1C, 0xC0 },
        { 0x21, 0xC1 },
OOVPA_END;

// ******************************************************************
// * lstrcmpiW
// ******************************************************************
OOVPA_NO_XREF(lstrcmpiW_1_0_3911, 8)

        { 0x07, 0x56 },
        { 0x0F, 0x01 },
        { 0x10, 0xE8 },
        { 0x19, 0x39 },
        { 0x22, 0x0E },
        { 0x2D, 0x59 },
        { 0x34, 0xEB },
        { 0x3D, 0x03 },
OOVPA_END;

// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA_1_0_3911, 7)

        { 0x1E, 0x0F },
        { 0x3E, 0x56 },
        { 0x5E, 0xFF },
        { 0x7E, 0x8D },
        { 0x9E, 0x50 },
        { 0xBE, 0x66 },
        { 0xDE, 0xF0 },
OOVPA_END;

// ******************************************************************
// * XUnmountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XUnmountAlternateTitleA_1_0_3911, 7)

        { 0x0A, 0x65 },
        { 0x16, 0xFF },
        { 0x23, 0x83 },
        { 0x2E, 0x45 },
        { 0x3A, 0x50 },
        { 0x46, 0x0B },
        { 0x52, 0x50 },
OOVPA_END;

// ******************************************************************
// * XMountMUA
// ******************************************************************
OOVPA_NO_XREF(XMountMUA_1_0_3911, 7)

        { 0x1E, 0x0C },
        { 0x3E, 0x66 },
        { 0x61, 0x85 },
        { 0x7E, 0x8D },
        { 0xA2, 0x0F },
        { 0xBE, 0x50 },
        { 0xDE, 0x74 },
OOVPA_END;

// ******************************************************************
// * CloseHandle
// ******************************************************************
OOVPA_NO_XREF(CloseHandle_1_0_3911, 8)

        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x0A, 0x85 },
        { 0x0B, 0xC0 },
        { 0x0E, 0x33 },
        { 0x12, 0x08 },
        { 0x19, 0x33 },
        { 0x1A, 0xC0 },
OOVPA_END;

// ******************************************************************
// * ExitThread
// ******************************************************************
OOVPA_NO_XREF(ExitThread_1_0_3911, 5)

        { 0x01, 0x00 },
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x0A, 0x04 },
        { 0x11, 0xCC },
OOVPA_END;

// ******************************************************************
// * VirtualAlloc
// ******************************************************************
OOVPA_NO_XREF(VirtualAlloc_1_0_3911, 7)

        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x45 },
        { 0x19, 0x85 },
        { 0x1C, 0x05 },
        { 0x22, 0x50 },
        { 0x28, 0x33 },
OOVPA_END;

// ******************************************************************
// * VirtualFree
// ******************************************************************
OOVPA_NO_XREF(VirtualFree_1_0_3911, 7)

        { 0x06, 0x80 },
        { 0x0E, 0x07 },
        { 0x16, 0xFF },
        { 0x1E, 0x45 },
        { 0x27, 0x85 },
        { 0x2E, 0xEB },
        { 0x36, 0x33 },
OOVPA_END;

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA_1_0_3911, 8)

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
// * XGetLaunchInfo
// ******************************************************************
OOVPA_NO_XREF(XGetLaunchInfo_1_0_3911, 7)

        { 0x0B, 0x8B },
        { 0x18, 0x15 },
        { 0x25, 0x8B },
        { 0x32, 0x30 },
        { 0x3F, 0x00 },
        { 0x4C, 0x83 },
        { 0x59, 0x5E },
OOVPA_END;

// ******************************************************************
// * XGetSectionHandleA
//	558BEC83EC10538B1D........5657FF75088D45F050FFD38B351C0101008B3D
// ******************************************************************
OOVPA_NO_XREF(XGetSectionHandleA_1_0_3911, 12)

		{ 0x00, 0x55 },
		{ 0x01, 0x8B },
		{ 0x02, 0xEC },
		{ 0x03, 0x83 },
		{ 0x04, 0xEC },
		{ 0x05, 0x10 },
		{ 0x06, 0x53 },
		{ 0x07, 0x8B },
		{ 0x08, 0x1D },

		{ 0x0D, 0x56 },
		{ 0x0E, 0x57 },
		{ 0x0F, 0xFF }
OOVPA_END;

// ******************************************************************
// * XLoadSectionByHandle
//  568B74240856FF15........85C07D0A50E8........33C0EB038B46045EC204
// ******************************************************************
OOVPA_NO_XREF(XLoadSectionByHandle_1_0_3911, 11)

		// XLoadSectionByHandle+0x01 : mov esi, [esp+4+arg_0]
		{ 0x01, 0x8B },
		{ 0x02, 0x74 },
		{ 0x03, 0x24 },
		{ 0x04, 0x08 },

		// XLoadSectionByHandle+0x0C : test eax, eax
		{ 0x0C, 0x85 },
		{ 0x0D, 0xC0 },

		// XLoadSectionByHandle+0x1A : mov eax, [esi+4]
		{ 0x1A, 0x8B },
		{ 0x1B, 0x46 },
		{ 0x1C, 0x04 },

		// XLoadSectionByHandle+0x1E : retn 4
		{ 0x1E, 0xC2 },
		{ 0x1F, 0x04 }
OOVPA_END;

// ******************************************************************
// * XFreeSectionByHandle
// FF742404FF15........85C07D0A50E8........33C0EB0333C040C20400....
// ******************************************************************
OOVPA_NO_XREF(XFreeSectionByHandle_1_0_3911, 11)

		// XFreeSectionByHandle+0x00 : push esp
		{ 0x00, 0xFF },
		{ 0x01, 0x74 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },

		// XFreeSectionByHandle+0x0A : test eax, eax
		{ 0x0A, 0x85 },
		{ 0x0B, 0xC0 },

		// XFreeSectionByHandle+0x18 : xor eax, eax
		{ 0x18, 0x33 },
		{ 0x19, 0xC0 },

		// XFreeSectionByHandle+0x1A : inc eax
		{ 0x1A, 0x40 },

		// XFreeSectionByHandle+0x1B : retn 4
		{ 0x1B, 0xC2 },
		{ 0x1C, 0x04 }
OOVPA_END;

// ******************************************************************
// * XAutoPowerDownResetTimer
// ******************************************************************
OOVPA_NO_XREF(XAutoPowerDownResetTimer_1_0_3911, 6)

        { 0x02, 0x59 },
        { 0x08, 0x51 },
        { 0x0A, 0x00 },
        { 0x0E, 0x50 },
        { 0x14, 0xFF },
        { 0x1A, 0xC3 },
OOVPA_END;

// ******************************************************************
// * XMountMURootA
// ******************************************************************
OOVPA_NO_XREF(XMountMURootA_1_0_3911, 7)

        { 0x1E, 0x0C },
        { 0x3E, 0x00 },
        { 0x61, 0x8B },
        { 0x7E, 0x00 },
        { 0x9E, 0x00 },
        { 0xBE, 0xFF },
        { 0xDE, 0xFF },
OOVPA_END;

// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive_1_0_3911, 7)

        { 0x1C, 0x85 },
        { 0x3A, 0x8D },
        { 0x58, 0x10 },
        { 0x76, 0xF8 },
        { 0x94, 0xF8 },
        { 0xB3, 0x8B },
        { 0xD0, 0xC0 },
OOVPA_END;

// ******************************************************************
// * GetFileAttributesA
// ******************************************************************
OOVPA_NO_XREF(GetFileAttributesA_1_0_3911, 7)

        { 0x08, 0x08 },
        { 0x13, 0x8D },
        { 0x1C, 0x50 },
        { 0x26, 0xFF },
        { 0x30, 0x15 },
        { 0x3A, 0x45 },
        { 0x44, 0x83 },
OOVPA_END;

// ******************************************************************
// * ReadFileEx@20
// ******************************************************************
OOVPA_NO_XREF(ReadFileEx_1_0_3911, 8)

        { 0x09, 0x48 },
        { 0x14, 0x8D },
        { 0x1F, 0xFF },
        { 0x2A, 0xFF },
		{ 0x2F, 0x00 },
        { 0x35, 0x00 },
        { 0x40, 0x50 },
        { 0x4B, 0xC0 },
OOVPA_END;

// ******************************************************************
// * WriteFileEx
// ******************************************************************
OOVPA_NO_XREF(WriteFileEx_1_0_3911, 8)

        { 0x09, 0x48 },
        { 0x14, 0x8D },
        { 0x1F, 0xFF },
        { 0x2A, 0xFF },
		{ 0x2F, 0xFC },
        { 0x35, 0x00 },
        { 0x40, 0x50 },
        { 0x4B, 0xC0 },
OOVPA_END;

// ******************************************************************
// * XAPI_1_0_3911
// ******************************************************************
OOVPATable XAPI_1_0_3911[] = {

    // GetExitCodeThread
	OOVPA_TABLE_PATCH(GetExitCodeThread_1_0_3911, XTL::EmuGetExitCodeThread),
    //*/
    // XInitDevices
	OOVPA_TABLE_PATCH(XInitDevices_1_0_3911, XTL::EmuXInitDevices), /* Too High Level
    // CreateMutex
	OOVPA_TABLE_PATCH(CreateMutex_1_0_3911, XTL::EmuCreateMutex), //*/
    /* Too High Level */
    // CreateThread
    /*
	OOVPA_TABLE_PATCH(CreateThread_1_0_3911, XTL::EmuCreateThread),
	*/
    // SetThreadPriority
	OOVPA_TABLE_PATCH(SetThreadPriority_1_0_3911, XTL::EmuSetThreadPriority), //*/
    // GetTimeZoneInformation
	OOVPA_TABLE_PATCH(GetTimeZoneInformation_1_0_3911, XTL::EmuGetTimeZoneInformation),
    // XRegisterThreadNotifyRoutine
	OOVPA_TABLE_PATCH(XRegisterThreadNotifyRoutine_1_0_3911, XTL::EmuXRegisterThreadNotifyRoutine),
    /*
    // XCalculateSignatureBegin
	OOVPA_TABLE_PATCH(XCalculateSignatureBegin_1_0_3911, XTL::EmuXCalculateSignatureBegin),
    */
    // XGetDevices
	OOVPA_TABLE_PATCH(XGetDevices_1_0_3911, XTL::EmuXGetDevices),
    // XGetDeviceChanges
	OOVPA_TABLE_PATCH(XGetDeviceChanges_1_0_3911, XTL::EmuXGetDeviceChanges),
    // XInputOpen
	OOVPA_TABLE_PATCH(XInputOpen_1_0_3911, XTL::EmuXInputOpen),
    // XID_fCloseDevice
	OOVPA_TABLE_XREF(XID_fCloseDevice_1_0_3911),
    // XInputClose
	OOVPA_TABLE_PATCH(XInputClose_1_0_3911, XTL::EmuXInputClose),
    // XInputGetCapabilities
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_3911, XTL::EmuXInputGetCapabilities),
    // XInputGetState
	OOVPA_TABLE_PATCH(XInputGetState_1_0_3911, XTL::EmuXInputGetState),
    // XInputSetState
	OOVPA_TABLE_PATCH(XInputSetState_1_0_3911, XTL::EmuXInputSetState),
	// SetThreadPriorityBoost
	OOVPA_TABLE_PATCH(SetThreadPriorityBoost_1_0_3911, XTL::EmuSetThreadPriorityBoost),
	// GetThreadPriority
	OOVPA_TABLE_PATCH(GetThreadPriority_1_0_3911, XTL::EmuGetThreadPriority),
	// XGetDevices
	OOVPA_TABLE_PATCH(XGetDevices_1_0_3911, XTL::EmuXGetDevices),
	// CreateFiber
	OOVPA_TABLE_PATCH(CreateFiber_1_0_3911, XTL::EmuCreateFiber),
	// DeleteFiber
	OOVPA_TABLE_PATCH(DeleteFiber_1_0_3911, XTL::EmuDeleteFiber),
	// SwitchToFiber
	OOVPA_TABLE_PATCH(SwitchToFiber_1_0_3911, XTL::EmuSwitchToFiber),
	// ConvertThreadToFiber
	OOVPA_TABLE_PATCH(ConvertThreadToFiber_1_0_3911, XTL::EmuConvertThreadToFiber),
	// XInputGetCapabilities
	OOVPA_TABLE_PATCH(XInputGetCapabilities_1_0_3911, XTL::EmuXInputGetCapabilities),
	// SignalObjectAndWait
	OOVPA_TABLE_PATCH(SignalObjectAndWait_1_0_3911, XTL::EmuSignalObjectAndWait),
	// QueueUserAPC
	OOVPA_TABLE_PATCH(QueueUserAPC_1_0_3911, XTL::EmuQueueUserAPC),
	// lstrcmpiW
	/*
	OOVPA_TABLE_PATCH(lstrcmpiW_1_0_3911, XTL::EmulstrcmpiW),
	*/
	// XMountAlternateTitleA
	OOVPA_TABLE_PATCH(XMountAlternateTitleA_1_0_3911, XTL::EmuXMountAlternateTitle),
	// XUnmountAlternateTitleA
	OOVPA_TABLE_PATCH(XUnmountAlternateTitleA_1_0_3911, XTL::EmuXUnmountAlternateTitle),
	// XMountMUA
	OOVPA_TABLE_PATCH(XMountMUA_1_0_3911, XTL::EmuXMountMUA),
	// XLaunchNewImageA
	OOVPA_TABLE_PATCH(XLaunchNewImageA_1_0_3911, XTL::EmuXLaunchNewImage),
	// XGetLaunchInfo
	OOVPA_TABLE_PATCH(XGetLaunchInfo_1_0_3911, XTL::EmuXGetLaunchInfo),
	// XAutoPowerDownResetTimer
	OOVPA_TABLE_PATCH(XAutoPowerDownResetTimer_1_0_3911, XTL::EmuXAutoPowerDownResetTimer),
	// XMountMURootA
	OOVPA_TABLE_PATCH(XMountMURootA_1_0_3911, XTL::EmuXMountMURootA),
	// XMountUtilityDrive
	OOVPA_TABLE_PATCH(XMountUtilityDrive_1_0_3911, XTL::EmuXMountUtilityDrive),
	// GetFileAttributesA
	OOVPA_TABLE_PATCH(GetFileAttributesA_1_0_3911, XTL::EmuGetFileAttributesA),
	// ReadFileEx
	/*
	OOVPA_TABLE_PATCH(ReadFileEx_1_0_3911, XTL::EmuReadFileEx),
	// WriteFileEx
	OOVPA_TABLE_PATCH(WriteFileEx_1_0_3911, XTL::EmuWriteFileEx),
	*/
	// CloseHandle
    /*
	OOVPA_TABLE_PATCH(CloseHandle_1_0_3911, XTL::EmuCloseHandle),
	*/
	// ExitThread
    /*
	OOVPA_TABLE_PATCH(ExitThread_1_0_3911, XTL::EmuExitThread),
	*/
	//// VirtualAlloc@16
    //OOVPA_TABLE_PATCH(VirtualAlloc_1_0_3911, //   XTL::EmuVirtualAlloc), //// VirtualFree@12
    //OOVPA_TABLE_PATCH(VirtualFree_1_0_3911, //   XTL::EmuVirtualFree),
};

// ******************************************************************
// * XAPI_1_0_3911_SIZE
// ******************************************************************
uint32 XAPI_1_0_3911_SIZE = sizeof(XAPI_1_0_3911);
