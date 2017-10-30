// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->Xapi.1.0.3911.inl
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
OOVPA_NO_XREF(GetExitCodeThread, 3911, 11)

        // GetExitCodeThread+0x03 : lea eax, [ebp+0x08]
        { 0x03, 0x8D },
        { 0x04, 0x45 },
        { 0x05, 0x08 },

        // GetExitCodeThread+0x1A : mov ecx, dword ptr [ebp+0x08]
        { 0x1A, 0x8B },
        { 0x1B, 0x4D },
        { 0x1C, 0x08 },

        // GetExitCodeThread+0x2B : mov eax, 0x0103
        { 0x2B, 0xB8 },
        { 0x2C, 0x03 },
        { 0x2D, 0x01 },

        // GetExitCodeThread+0x49 : retn 0x08
        { 0x49, 0xC2 },
        { 0x4A, 0x08 },
OOVPA_END;

// ******************************************************************
// * XInitDevices
// ******************************************************************
// * NOTE: We are actually intercepting USBD_Init, because
// *       XInitDevices Simply redirects to that function
// ******************************************************************
OOVPA_NO_XREF(XInitDevices, 3911, 10)

        // XInitDevices+0x03 : push 0xB4
        { 0x03, 0x68 },
        { 0x04, 0xB4 },

        // XInitDevices+0x10 : jmp +0x13
        { 0x10, 0x74 },
        { 0x11, 0x13 },

        // XInitDevices+0x5B : movzx eax, byte ptr [esi+0xA1]
        { 0x5B, 0x0F },
        { 0x5C, 0xB6 },
        { 0x5D, 0x86 },
        { 0x5E, 0xA1 },

        // XInitDevices+0x8B : retn 8
        { 0x8B, 0xC2 },
        { 0x8C, 0x08 },
OOVPA_END;

// ******************************************************************
// * CreateMutex
// ******************************************************************
OOVPA_NO_XREF(CreateMutex, 3911, 13)

        // CreateMutex+0x03 : sub esp, 0x14
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },

        // CreateMutex+0x32 : cmp eax, 0x40000000
        { 0x32, 0x3D },
        { 0x33, 0x00 },
        { 0x34, 0x00 },
        { 0x35, 0x00 },
        { 0x36, 0x40 },

        // CreateMutex+0x39 : push 0xB7
        { 0x39, 0x68 },
        { 0x3A, 0xB7 },

        // CreateMutex+0x47 : mov eax, [ebp+0x10]
        { 0x47, 0x8B },
        { 0x48, 0x45 },
        { 0x49, 0x10 },
OOVPA_END;

// ******************************************************************
// * CreateThread
// ******************************************************************
OOVPA_NO_XREF(CreateThread, 3911, 21)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x8B },
        { 0x04, 0x45 },
        { 0x05, 0x0C },
        { 0x06, 0x85 },
        { 0x07, 0xC0 },
        { 0x08, 0x75 },
        { 0x09, 0x05 },
        { 0x0A, 0xA1 },
        { 0x0B, 0x30 },
        { 0x0C, 0x01 },
        { 0x0D, 0x01 },
        { 0x0E, 0x00 },
        { 0x0F, 0x8B },
        { 0x10, 0x4D },
        { 0x11, 0x18 },
        { 0x12, 0x68 },

        { 0x32, 0x50 },
        { 0x33, 0x6A },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * CreateThread
// ******************************************************************
OOVPA_NO_XREF(CreateThread, 3911, 8)

        // CreateThread+0x0A : mov eax, ds:10130h
        { 0x0A, 0xA1 },
        { 0x0B, 0x30 },
        { 0x0C, 0x01 },

        // CreateThread+0x1C : and ecx, 0xFFFFFF01
        { 0x1C, 0x81 },
        { 0x1D, 0xE1 },
        { 0x1E, 0x01 },

        // CreateThread+0x51 : retn 0x18
        { 0x51, 0xC2 },
        { 0x52, 0x18 },
OOVPA_END;
#endif

// ******************************************************************
// * SetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriority, 3911, 10)

        // SetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF },
        { 0x0E, 0x75 },
        { 0x0F, 0x08 },

        // SetThreadPriority+0x18 : jl +0x2C
        { 0x18, 0x7C },
        { 0x19, 0x2C },

        // SetThreadPriority+0x22 : push 0x10
        { 0x22, 0x6A },
        { 0x23, 0x10 },

        // SetThreadPriority+0x26 : cmp eax, 0xFFFFFFF1
        { 0x26, 0x83 },
        { 0x27, 0xF8 },
        { 0x28, 0xF1 },
OOVPA_END;

// ******************************************************************
// * OutputDebugStringA
// ******************************************************************
OOVPA_NO_XREF(OutputDebugStringA, 3911, 32)
		{ 0x00, 0x55 },
		{ 0x01, 0x8B },
		{ 0x02, 0xEC },
		{ 0x03, 0x51 },
		{ 0x04, 0x51 },
		{ 0x05, 0x8B },
		{ 0x06, 0x45 },
		{ 0x07, 0x08 },
		{ 0x08, 0x89 },
		{ 0x09, 0x45 },
		{ 0x0A, 0xFC },
		{ 0x0B, 0x8D },
		{ 0x0C, 0x48 },
		{ 0x0D, 0x01 },
		{ 0x0E, 0x8A },
		{ 0x0F, 0x10 },
		{ 0x10, 0x40 },
		{ 0x11, 0x84 },
		{ 0x12, 0xD2 },
		{ 0x13, 0x75 },
		{ 0x14, 0xF9 },
		{ 0x15, 0x2B },
		{ 0x16, 0xC1 },
		{ 0x17, 0x66 },
		{ 0x18, 0x89 },
		{ 0x19, 0x45 },
		{ 0x1A, 0xF8 },
		{ 0x1B, 0x8B },
		{ 0x1C, 0x45 },
		{ 0x1D, 0xF8 },
		{ 0x1E, 0x40 },
		{ 0x1F, 0x66 },
OOVPA_END;

// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 3911, 7)

        // XapiInitProcess+0x03 : sub esp, 30h
        { 0x05, 0x30 },

        // XapiInitProcess+0x0F : push 0x0C
        { 0x0F, 0x6A },
        { 0x10, 0x0C },

        // XapiInitProcess+0x17 : repe stosd
        { 0x17, 0xF3 },
        { 0x18, 0xAB },

        // XapiInitProcess+0x42 : jnz +0x0A
        { 0x42, 0x75 },
        { 0x43, 0x0A },
OOVPA_END;

#if 0 // Moved to 3950
// ******************************************************************
// * XapiInitProcess
// ******************************************************************
OOVPA_NO_XREF(XapiInitProcess, 3950, 7)

        // XapiInitProcess+0x03 : sub esp, 30h
        { 0x05, 0x30 },

        // XapiInitProcess+0x0F : push 0x0C
        { 0x10, 0x6A },
        { 0x11, 0x0C },

        // XapiInitProcess+0x17 : repe stosd
        { 0x18, 0xF3 },
        { 0x19, 0xAB },

        // XapiInitProcess+0x42 : jnz +0x0A
        { 0x43, 0x75 },
        { 0x44, 0x0A },
OOVPA_END;
#endif

// ******************************************************************
// * XapiBootDash
// ******************************************************************
OOVPA_NO_XREF(XapiBootDash, 3911, 11)

        // XapiBootDash+0x03 : sub esp, 0x0C00
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x06, 0x0C },

        // XapiBootDash+0x09 : mov eax, ds:0x10118
        { 0x09, 0xA1 },
        { 0x0A, 0x18 },
        { 0x0B, 0x01 },
        { 0x0C, 0x01 },

        // XapiBootDash+0x25 : repe stosd
        { 0x25, 0xF3 },
        { 0x26, 0xAB },

        // XapiBootDash+0x59 : retn 0x0C
        { 0x59, 0xC2 },
        { 0x5A, 0x0C },
OOVPA_END;

// ******************************************************************
// * XRegisterThreadNotifyRoutine
// ******************************************************************
OOVPA_NO_XREF(XRegisterThreadNotifyRoutine, 3911, 11)

        // XRegisterThreadNotifyRoutine+0x0D : cmp [esp+0x0C], 0
        { 0x0D, 0x83 },
        { 0x0E, 0x7C },
        { 0x0F, 0x24 },
        { 0x10, 0x0C },

        // XRegisterThreadNotifyRoutine+0x12 : jz +0x1C
        { 0x12, 0x74 },
        { 0x13, 0x1C },

        // XRegisterThreadNotifyRoutine+0x24 : mov [eax+4], ecx
        { 0x24, 0x89 },
        { 0x25, 0x48 },
        { 0x26, 0x04 },

        // XRegisterThreadNotifyRoutine+0x46 : retn 0x08
        { 0x46, 0xC2 },
        { 0x47, 0x08 },
OOVPA_END;

// ******************************************************************
// * GetTimeZoneInformation
// ******************************************************************
OOVPA_NO_XREF(GetTimeZoneInformation, 3911, 7)

        { 0x05, 0x28 },
        { 0x2E, 0x28 },
        { 0x4F, 0x59 },
        { 0x56, 0xAB },
        { 0x8C, 0xC0 },
        { 0xB9, 0x80 },
        { 0xF7, 0x99 },
OOVPA_END;

// not necessary?
// ******************************************************************
// * XCalculateSignatureBegin
// ******************************************************************
OOVPA_NO_XREF(XCalculateSignatureBegin, 3911, 10)

        // XCalculateSignatureBegin+0x01 : push 0x7C; push 0
        { 0x01, 0x6A },
        { 0x02, 0x7C },
        { 0x03, 0x6A },
        { 0x04, 0x00 },

        // XCalculateSignatureBegin+0x10 : push 0x08
        { 0x10, 0x6A },
        { 0x11, 0x08 },

        // XCalculateSignatureBegin+0x2E : push 0x10
        { 0x2E, 0x6A },
        { 0x2F, 0x10 },

        // XCalculateSignatureBegin+0x3B : retn 0x04
        { 0x3B, 0xC2 },
        { 0x3C, 0x04 },
OOVPA_END;

// ******************************************************************
// * XGetDevices
// ******************************************************************
OOVPA_NO_XREF(XGetDevices, 3911, 14)

        // XGetDevices+0x07 : mov edx, [esp+arg_0]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x08 },

        // XGetDevices+0x0D : and [edx+4], 0
        { 0x0D, 0x83 },
        { 0x0E, 0x62 },
        { 0x0F, 0x04 },
        { 0x10, 0x00 },

        // XGetDevices+0x11 : mov cl, al
        { 0x11, 0x8A },
        { 0x12, 0xC8 },

        // XGetDevices+0x16 : call KfLowerIrql
        { 0x16, 0xFF },
        { 0x17, 0x15 },

        // XGetDevices+0x1F : retn 4
        { 0x1F, 0xC2 },
        { 0x20, 0x04 },
OOVPA_END;

// ******************************************************************
// * XGetDeviceChanges
// ******************************************************************
OOVPA_NO_XREF(XGetDeviceChanges, 3911, 8)

        // XGetDeviceChanges+0x07 : xor eax, eax
        { 0x07, 0x33 },
        { 0x08, 0xC0 },

        // XGetDeviceChanges+0x33 : not edx
        { 0x33, 0xF7 },
        { 0x34, 0xD2 },

        // XGetDeviceChanges+0x42 : or edx, edi
        { 0x42, 0x0B },
        { 0x43, 0xD7 },

        // XGetDeviceChanges+0x51 : mov cl, al
        { 0x51, 0x8A },
        { 0x52, 0xC8 },
OOVPA_END;

// ******************************************************************
// * XInputOpen
// ******************************************************************
OOVPA_NO_XREF(XInputOpen, 3911, 11)

        // XInputOpen+0x20 : jmp +0x0B
        { 0x20, 0xEB },
        { 0x21, 0x0B },

        // XInputOpen+0x29 : jnz +0x3D
        { 0x29, 0x75 },
        { 0x2A, 0x3D },

        // XInputOpen+0x4A : add edx, 0x10
        { 0x4A, 0x83 },
        { 0x4B, 0xC2 },
        { 0x4C, 0x10 },

        // XInputOpen+0x66 : jmp +0x09
        { 0x66, 0xEB },
        { 0x67, 0x09 },

        // XInputOpen+0x68 : push 0x57
        { 0x68, 0x6A },
        { 0x69, 0x57 },
OOVPA_END;

// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 3911, 16,

    XREF_XID_fCloseDevice,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x53 },
        { 0x07, 0x56 },
        { 0x08, 0x8B },
        { 0x09, 0xF1 },
        { 0x0A, 0xFF },
        { 0x0B, 0x15 },

        { 0x3E, 0x8D },
        { 0x3F, 0x45 },

        { 0x44, 0x8D },
        { 0x45, 0x45 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XID_fCloseDevice
// ******************************************************************
OOVPA_XREF(XID_fCloseDevice, 3911, 10,

    XREF_XID_fCloseDevice,
    XRefZero)

        // XID_fCloseDevice+0x19 : jz +0x5C
        { 0x19, 0x74 },
        { 0x1A, 0x5C },

        // XID_fCloseDevice+0x1B : cmp [esi+0x00A3], 1
        { 0x1B, 0x80 },
        { 0x1D, 0xA3 },
        { 0x21, 0x01 },

        // XID_fCloseDevice+0x91 : mov [esi+0x00A4], eax
        { 0x95, 0x89 },
        { 0x96, 0x86 },
        { 0x97, 0xA4 },

        // XID_fCloseDevice+0xA3 : leave; retn
        { 0xA3, 0xC9 },
        { 0xA4, 0xC3 },
OOVPA_END;
#endif

// ******************************************************************
// * XInputClose
// ******************************************************************
OOVPA_XREF(XInputClose, 3911, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        // XInputClose+0x05 : call [fCloseDevice]
        XREF_ENTRY( 0x05, XREF_XID_fCloseDevice ),

        // XInputClose+0x00 : mov ecx, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x4C },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // XInputClose+0x04 : call [fCloseDevice]
        { 0x04, 0xE8 },

        // XInputClose+0x09 : retn 0x04
        { 0x09, 0xC2 },
        { 0x0A, 0x04 },
OOVPA_END;

// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 3911, 13)

        { 0x00, 0x55 },
        { 0x0F, 0x15 },
        { 0x1E, 0x0F },
        { 0x1F, 0x84 },

        { 0x36, 0x8B },
        { 0x37, 0xFA },
        { 0x38, 0xF3 },
        { 0x39, 0xAB },
        { 0x3A, 0xAA },
        { 0x3B, 0x8A },
        { 0x3C, 0x46 },
        { 0x3D, 0x0B },
        { 0x3E, 0x88 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XInputGetCapabilities
// ******************************************************************
OOVPA_NO_XREF(XInputGetCapabilities, 3911, 14)

        // XInputGetCapabilities+0x03 : sub esp, 0x48
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x48 },

        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
        { 0x24, 0xF6 },
        { 0x25, 0x46 },
        { 0x26, 0x04 },
        { 0x27, 0x02 },

        // XInputGetCapabilities+0x3A : stosb
        { 0x3A, 0xAA },

        // XInputGetCapabilities+0x8A : mov [ebp+var_48], 0x30
        { 0x8A, 0xC6 },
        { 0x8B, 0x45 },
        { 0x8C, 0xB8 },
        { 0x8D, 0x30 },

        // XInputGetCapabilities+0xFB : cmp [eax], ebx
        { 0xFB, 0x39 },
        { 0xFC, 0x18 },
OOVPA_END;
#endif

// ******************************************************************
// * XInputGetState
// ******************************************************************
OOVPA_NO_XREF(XInputGetState, 3911, 13)

        // XInputGetState+0x0E : cmp byte ptr [edx+0x0A3], 1
        { 0x0E, 0x80 },
        { 0x0F, 0xBA },
        { 0x10, 0xA3 },
        { 0x14, 0x01 },

        // XInputGetState+0x17 : push 0x57
        { 0x17, 0x6A },
        { 0x18, 0x57 },

        // XInputGetState+0x1A : jmp +0x46
        { 0x1A, 0xEB },
        { 0x1B, 0x46 },

        // XInputGetState+0x28 : mov ebx, 0x048F
        { 0x28, 0xBB },
        { 0x29, 0x8F },
        { 0x2A, 0x04 },

        // XInputGetState+0x6E : retn 8
        { 0x6E, 0xC2 },
        { 0x6F, 0x08 },
OOVPA_END;

// ******************************************************************
// * XInputSetState
// ******************************************************************
OOVPA_NO_XREF(XInputSetState, 3911, 12)

        // XInputSetState+0x04 : lea eax, [ecx+0x0A3]
        { 0x04, 0x8D },
        { 0x05, 0x81 },
        { 0x06, 0xA3 },

        // XInputSetState+0x0F : push 0x57
        { 0x0F, 0x6A },
        { 0x10, 0x57 },

        // XInputSetState+0x12 : jmp +0x21
        { 0x12, 0xEB },
        { 0x13, 0x21 },

        // XInputSetState+0x2D : mov [edx+0x41], al
        { 0x2D, 0x88 },
        { 0x2E, 0x42 },
        { 0x2F, 0x41 },

        // XInputSetState+0x35 : retn 0x08
        { 0x35, 0xC2 },
        { 0x36, 0x08 },
OOVPA_END;

// ******************************************************************
// * SetThreadPriorityBoost
// ******************************************************************
OOVPA_NO_XREF(SetThreadPriorityBoost, 3911, 10) // generic version

        // SetThreadPriorityBoost+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF },
        { 0x0E, 0x75 },
        { 0x0F, 0x08 },

        // SetThreadPriorityBoost+0x18 : jl +0x20
        { 0x18, 0x7C },
        { 0x19, 0x20 },

        // SetThreadPriorityBoost+0x1F : setnz al
        { 0x1F, 0x0F },
        { 0x20, 0x95 },
        { 0x21, 0xC0 },

        // SetThreadPriorityBoost+0x2C : mov ecx, [ebp+0x08]
        { 0x2C, 0x8B },
        { 0x2D, 0x4D },
OOVPA_END;

// ******************************************************************
// * GetThreadPriority
// ******************************************************************
OOVPA_NO_XREF(GetThreadPriority, 3911, 10)

        // GetThreadPriority+0x0D : push [ebp+0x08]
        { 0x0D, 0xFF },
        { 0x0E, 0x75 },
        { 0x0F, 0x08 },

        // GetThreadPriority+0x18 : jl +0x2B
        { 0x18, 0x7C },
        { 0x19, 0x2B },

        // GetThreadPriority+0x2F : cmp esi, 0xFFFFFFF0
        { 0x2F, 0x83 },
        { 0x30, 0xFE },
        { 0x31, 0xF0 },

        // GetThreadPriority+0x37 : mov ecx, [ebp+0x08]
        { 0x37, 0x8B },
        { 0x38, 0x4D },
OOVPA_END;

//// ******************************************************************
//// * XGetDevices
//// ******************************************************************
//OOVPA_NO_XREF(XGetDevices, 3911, 14)
//
//        // XGetDevices+0x07 : mov edx, [esp+arg_0]
//        { 0x07, 0x8B },
//        { 0x08, 0x54 },
//        { 0x09, 0x24 },
//        { 0x0A, 0x08 },
//
//        // XGetDevices+0x0D : and [edx+4], 0
//        { 0x0D, 0x83 },
//        { 0x0E, 0x62 },
//        { 0x0F, 0x04 },
//        { 0x10, 0x00 },
//
//        // XGetDevices+0x11 : mov cl, al
//        { 0x11, 0x8A },
//        { 0x12, 0xC8 },
//
//        // XGetDevices+0x16 : call KfLowerIrql
//        { 0x16, 0xFF },
//        { 0x17, 0x15 },
//
//        // XGetDevices+0x1F : retn 4
//        { 0x1F, 0xC2 },
//        { 0x20, 0x04 },
//OOVPA_END;

// ******************************************************************
// * CreateFiber
// ******************************************************************
OOVPA_NO_XREF(CreateFiber, 3911, 32)
		{ 0x00, 0x8B },
		{ 0x01, 0x44 },
		{ 0x02, 0x24 },
		{ 0x03, 0x04 },
		{ 0x04, 0x56 },
		{ 0x05, 0x57 },
		{ 0x06, 0x33 },
		{ 0x07, 0xFF },
		{ 0x08, 0x3B },
		{ 0x09, 0xC7 },
		{ 0x0A, 0x75 },
		{ 0x0B, 0x05 },
		{ 0x0C, 0xA1 },
		{ 0x0D, 0x30 },
		{ 0x0E, 0x01 },
		{ 0x0F, 0x01 },
		{ 0x10, 0x00 },
		{ 0x11, 0xB9 },
		{ 0x12, 0x00 },
		{ 0x13, 0x30 },
		{ 0x14, 0x00 },
		{ 0x15, 0x00 },
		{ 0x16, 0x3B },
		{ 0x17, 0xC1 },
		{ 0x18, 0x73 },
		{ 0x19, 0x02 },
		{ 0x1A, 0x8B },
		{ 0x1B, 0xC1 },
		{ 0x1C, 0x8D },
		{ 0x1D, 0xB0 },
		{ 0x1E, 0xFF },
		{ 0x1F, 0x0F },
OOVPA_END;

// ******************************************************************
// * DeleteFiber
// ******************************************************************
OOVPA_NO_XREF(DeleteFiber, 3911, 15) 
	{ 0x00, 0x8B },
	{ 0x01, 0x44 },
	{ 0x02, 0x24 },
	{ 0x03, 0x04 },
	{ 0x04, 0xFF },
	{ 0x05, 0x70 },
	{ 0x06, 0x08 },
	{ 0x07, 0xFF },
	{ 0x08, 0x70 },
	{ 0x09, 0x04 },
	{ 0x0A, 0xFF },
	{ 0x0B, 0x15 },

	{ 0x10, 0xC2 },
	{ 0x11, 0x04 },
	{ 0x12, 0x00 },
OOVPA_END;

// ******************************************************************
// * SwitchToFiber
// ******************************************************************
OOVPA_NO_XREF(SwitchToFiber, 3911, 28)
	{ 0x00, 0x8B },
	{ 0x01, 0x15 },

    { 0x06, 0x64 },
	{ 0x07, 0x8B },
	{ 0x08, 0x0D },
	{ 0x09, 0x04 },
	{ 0x0A, 0x00 },
	{ 0x0B, 0x00 },
	{ 0x0C, 0x00 },
	{ 0x0D, 0x8B },
	{ 0x0E, 0x44 },
	{ 0x0F, 0x24 },
	{ 0x10, 0x04 },
	{ 0x11, 0x55 },
	{ 0x12, 0x56 },
	{ 0x13, 0x57 },
	{ 0x14, 0x53 },
	{ 0x15, 0x64 },
	{ 0x16, 0xFF },
	{ 0x17, 0x35 },
	{ 0x18, 0x00 },
	{ 0x19, 0x00 },
	{ 0x1A, 0x00 },
	{ 0x1B, 0x00 },
	{ 0x1C, 0x8B },
	{ 0x1D, 0x14 },
	{ 0x1E, 0x91 },
	{ 0x1F, 0x8B },
OOVPA_END;

// ******************************************************************
// * ConvertThreadToFiber
// ******************************************************************
OOVPA_NO_XREF(ConvertThreadToFiber, 3911, 20)
	{ 0x00, 0xA1 },
    
	{ 0x05, 0x64 },
	{ 0x06, 0x8B },
	{ 0x07, 0x0D },

    { 0x0C, 0x8B },
	{ 0x0D, 0x14 },
	{ 0x0E, 0x81 },
	{ 0x0F, 0x8B },
	{ 0x10, 0x44 },
	{ 0x11, 0x24 },
	{ 0x12, 0x04 },
	{ 0x13, 0x8D },
	{ 0x14, 0x8A },

	{ 0x19, 0x89 },
    { 0x1A, 0x01 },
	{ 0x1B, 0x64 },
	{ 0x1C, 0xA1 },
	{ 0x1D, 0x28 },
	{ 0x1E, 0x00 },
	{ 0x1F, 0x00 },
OOVPA_END;

//// ******************************************************************
//// * XInputGetCapabilities
//// ******************************************************************
//OOVPA_NO_XREF(XInputGetCapabilities, 3911, 14)
//
//        // XInputGetCapabilities+0x03 : sub esp, 0x48
//        { 0x03, 0x83 },
//        { 0x04, 0xEC },
//        { 0x05, 0x48 },
//
//        // XInputGetCapabilities+0x24 : test byte ptr [esi+4], 2
//        { 0x24, 0xF6 },
//        { 0x25, 0x46 },
//        { 0x26, 0x04 },
//        { 0x27, 0x02 },
//
//        // XInputGetCapabilities+0x3A : stosb
//        { 0x3A, 0xAA },
//
//        // XInputGetCapabilities+0x8A : mov [ebp+var_48], 0x30
//        { 0x8A, 0xC6 },
//        { 0x8B, 0x45 },
//        { 0x8C, 0xB8 },
//        { 0x8D, 0x30 },
//
//        // XInputGetCapabilities+0xFB : cmp [eax], ebx
//        { 0xFB, 0x39 },
//        { 0xFC, 0x18 },
//OOVPA_END;

// ******************************************************************
// * SignalObjectAndWait
// ******************************************************************
OOVPA_NO_XREF(SignalObjectAndWait, 3911, 8) // generic version

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
OOVPA_NO_XREF(QueueUserAPC, 3911, 7) // generic version

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
OOVPA_NO_XREF(lstrcmpiW, 3911, 8)

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
OOVPA_NO_XREF(XMountAlternateTitleA, 3911, 13)

        { 0x04, 0xEC },

        { 0x0F, 0x18 },
        { 0x10, 0x01 },
        { 0x11, 0x01 },
        { 0x12, 0x00 },
        { 0x13, 0x53 },
        { 0x14, 0x8A },

        { 0x30, 0x39 },
        { 0x31, 0x55 },
        { 0x32, 0x0C },
        { 0x33, 0x74 },
        { 0x34, 0x09 },

        { 0x3D, 0xEC },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XMountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XMountAlternateTitleA, 3911, 7)

        { 0x1E, 0x0F },
        { 0x3E, 0x56 },
        { 0x5E, 0xFF },
        { 0x7E, 0x8D },
        { 0x9E, 0x50 },
        { 0xBE, 0x66 },
        { 0xDE, 0xF0 },
OOVPA_END;
#endif

// ******************************************************************
// * XUnmountAlternateTitleA
// ******************************************************************
OOVPA_NO_XREF(XUnmountAlternateTitleA, 3911, 7)

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
OOVPA_NO_XREF(XMountMUA, 3911, 7) // generic version

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
OOVPA_NO_XREF(CloseHandle, 3911, 8)

        { 0x02, 0x24 },
        { 0x03, 0x04 },
        { 0x0A, 0x85 },
        { 0x0B, 0xC0 },
        { 0x0E, 0x33 },
        { 0x12, 0x08 },
        { 0x19, 0x33 },
        { 0x1A, 0xC0 },
OOVPA_END;

// Generic OOVPA as of 3911 and newer.
// ******************************************************************
// * ExitThread
// ******************************************************************
OOVPA_NO_XREF(ExitThread, 3911, 10)

        { 0x00, 0x6A },
        { 0x01, 0x00 },
        { 0x02, 0xE8 },
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x09, 0x24 },
        { 0x0A, 0x04 },
        { 0x0B, 0xFF },
        { 0x0C, 0x15 },
        { 0x11, 0xCC },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * ExitThread
// ******************************************************************
OOVPA_NO_XREF(ExitThread, 3911, 5)

        { 0x01, 0x00 },
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x0A, 0x04 },
        { 0x11, 0xCC },
OOVPA_END;
#endif

// ******************************************************************
// * XLaunchNewImageA
// ******************************************************************
OOVPA_NO_XREF(XLaunchNewImageA, 3911, 8)

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
OOVPA_NO_XREF(XGetLaunchInfo, 3911, 7)

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
OOVPA_NO_XREF(XGetSectionHandleA, 3911, 12)

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
OOVPA_NO_XREF(XLoadSectionByHandle, 3911, 11)

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
OOVPA_NO_XREF(XFreeSectionByHandle, 3911, 11)

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
// Assembly line at 0x00 and 0x09 are unique. It will prevent any false detection it might find in the future.
OOVPA_NO_XREF(XAutoPowerDownResetTimer, 3911, 12)

        //XAutoPowerDownResetTimer+0x00 : push 0FFFFFFCDh
        { 0x00, 0x6A },
        { 0x01, 0xCD },

        //XAutoPowerDownResetTimer+0x02 : pop ecx
        { 0x02, 0x59 },

        //XAutoPowerDownResetTimer+0x08 : push ecx
        { 0x08, 0x51 },

        //XAutoPowerDownResetTimer+0x09 : mov eax, 0B5659000h
        { 0x09, 0xB8 },
        { 0x0A, 0x00 },
        { 0x0B, 0x90 },
        { 0x0C, 0x65 },
        { 0x0D, 0xB5 },

        //XAutoPowerDownResetTimer+0x0E : push eax
        { 0x0E, 0x50 },

        { 0x14, 0xFF },

        //XAutoPowerDownResetTimer+0x1A : ret
        { 0x1A, 0xC3 },
OOVPA_END;

// ******************************************************************
// * XMountMURootA
// ******************************************************************
OOVPA_NO_XREF(XMountMURootA, 3911, 7) // generic version

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
OOVPA_NO_XREF(XMountUtilityDrive, 3911, 26)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x01 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x53 },
        { 0x0A, 0x56 },
        { 0x0B, 0x57 },
        { 0x0C, 0x8D },
        { 0x0D, 0x45 },
        { 0x0E, 0xF4 },
        { 0x0F, 0x50 },
        { 0x10, 0x8D },
        { 0x11, 0x45 },
        { 0x12, 0xFC },
        { 0x13, 0x50 },
        { 0x14, 0xFF },
        { 0x15, 0x75 },
        { 0x16, 0x08 },
        { 0x17, 0xE8 },

        { 0x56, 0x83 },
        { 0x57, 0xC4 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XMountUtilityDrive
// ******************************************************************
OOVPA_NO_XREF(XMountUtilityDrive, 3911, 7)

        { 0x1C, 0x85 },
        { 0x3A, 0x8D },
        { 0x58, 0x10 },
        { 0x76, 0xF8 },
        { 0x94, 0xF8 },
        { 0xB3, 0x8B },
        { 0xD0, 0xC0 },
OOVPA_END;
#endif

// ******************************************************************
// * ReadFileEx@20
// ******************************************************************
OOVPA_NO_XREF(ReadFileEx, 3911, 8)

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
OOVPA_NO_XREF(WriteFileEx, 3911, 8)

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
// * XInputPoll
// ******************************************************************
OOVPA_NO_XREF(XInputPoll, 3911, 14)

        { 0x00, 0x53 },
        { 0x01, 0x56 },
        { 0x02, 0x33 },
        { 0x03, 0xF6 },
        { 0x04, 0xFF },
        { 0x05, 0x15 },

        { 0x18, 0x04 },
        { 0x19, 0x02 },
        { 0x1A, 0x75 },
        { 0x1B, 0x29 },
        { 0x1C, 0xF6 },
        { 0x1D, 0x80 },
        { 0x1E, 0xA2 },
        { 0x1F, 0x00 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * XInputPoll
// ******************************************************************
OOVPA_NO_XREF(XInputPoll, 3911, 10)

        { 0x16, 0xF6 },
        { 0x17, 0x41 },
        { 0x18, 0x04 },
        { 0x19, 0x02 },

        { 0x25, 0x39 },
        { 0x26, 0x70 },
        { 0x27, 0x04 },

        { 0x3A, 0x83 },
        { 0x3B, 0xC0 },
        { 0x3C, 0x52 },
OOVPA_END;
#endif

// ******************************************************************
// * timeSetEvent
// ******************************************************************
OOVPA_NO_XREF(timeSetEvent, 3911, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x14 },
        { 0x06, 0x53 },
        { 0x07, 0x68 },

        { 0x1B, 0xCB },
        { 0x1C, 0x75 },

        { 0x2A, 0x45 },
        { 0x55, 0x53 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * timeSetEvent
// ******************************************************************
OOVPA_NO_XREF(timeSetEvent, 3911, 7)

        { 0x1E, 0x8D },
        { 0x3E, 0x89 },
        { 0x5E, 0x15 },
        { 0x7E, 0x3F },
        { 0x9E, 0x03 },
        { 0xBE, 0x32 },
        { 0xDE, 0x89 },
OOVPA_END;
#endif

// ******************************************************************
// * timeKillEvent
// ******************************************************************
OOVPA_NO_XREF(timeKillEvent, 3911, 12)

        { 0x02, 0xBF },
        { 0x13, 0x0D },

        { 0x18, 0x0F },
        { 0x19, 0xB7 },
        { 0x1A, 0xC2 },
        { 0x1B, 0x48 },
        { 0x1C, 0x85 },
        { 0x1D, 0xC9 },
        { 0x1E, 0x74 },
        { 0x1F, 0x3E },

        { 0x4A, 0x6A },
        { 0x55, 0x15 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 3911 version
// ******************************************************************
// * timeKillEvent
// ******************************************************************
OOVPA_NO_XREF(timeKillEvent, 3911, 8)

        { 0x0E, 0x8B },
        { 0x1A, 0xC2 },
        { 0x28, 0x8D },
        { 0x36, 0x56 },
        { 0x44, 0x00 },
        { 0x52, 0x00 },
        { 0x60, 0x5E },
        { 0x6E, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * GetOverlappedResult
// ******************************************************************
OOVPA_NO_XREF(GetOverlappedResult, 3911, 7)

        { 0x0B, 0x75 },
        { 0x18, 0xC0 },
        { 0x27, 0xEB },
        { 0x32, 0x00 },
        { 0x3F, 0xEB },
        { 0x4C, 0x89 },
        { 0x59, 0x56 },
OOVPA_END;

// ******************************************************************
// * RaiseException
// ******************************************************************
OOVPA_NO_XREF(RaiseException, 3911, 7)

        { 0x09, 0x83 },
        { 0x14, 0x8B },
        { 0x1F, 0xC7 },
        { 0x2A, 0x10 },
        { 0x35, 0x89 },
        { 0x40, 0x5F },
        { 0x4B, 0xFF },
OOVPA_END;

// Generic OOVPA as of 3911 and newer.
// ******************************************************************
// * SwitchToThread
// ******************************************************************
OOVPA_NO_XREF(SwitchToThread, 3911, 15)

        { 0x00, 0xFF },
        { 0x01, 0x15 },
        { 0x06, 0x33 },
        { 0x07, 0xC9 },
        { 0x08, 0x3D },
        { 0x09, 0x24 },
        { 0x0A, 0x00 },
        { 0x0B, 0x00 },
        { 0x0C, 0x40 },
        { 0x0D, 0x0F },
        { 0x0E, 0x95 },
        { 0x0F, 0xC1 },
        { 0x10, 0x8B },
        { 0x11, 0xC1 },
        { 0x12, 0xC3 },
OOVPA_END;

// ******************************************************************
// * XapiThreadStartup
// ******************************************************************
// Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(XapiThreadStartup, 3911, 17)

        { 0x00, 0x6A },
        { 0x01, 0x18 },
        { 0x02, 0x68 },
        { 0x07, 0xE8 },
        { 0x0C, 0x83 },
        { 0x0D, 0x65 },
        { 0x0E, 0xFC },
        { 0x0F, 0x00 },
        { 0x10, 0x64 },
        { 0x11, 0xA1 },
        { 0x12, 0x28 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x00 },
        { 0x16, 0x89 },
        { 0x17, 0x45 },
        // { 0x18, 0xE4 }, 3911 0xE4 vs 5558 0xE0
        
        { 0x1F, 0x89 },
OOVPA_END;

// Generic OOVPA as of 3911 and newer.
// ******************************************************************
// * MoveFileA
// ******************************************************************
OOVPA_NO_XREF(MoveFileA, 3911, 12)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x02, 0xEC },
        { 0x03, 0x83 },
        { 0x04, 0xEC },
        { 0x05, 0x2C },
        { 0x06, 0x56 },
        { 0x07, 0x8B },
        { 0x08, 0x35 },

        { 0x1F, 0x8D },
        { 0x93, 0xC2 },
        { 0x94, 0x08 },
OOVPA_END;

// Generic OOVPA as of 3911 and newer.
// ******************************************************************
// * XapiFiberStartup
// ******************************************************************
OOVPA_NO_XREF(XapiFiberStartup, 3911, 14)

        { 0x00, 0x6A },
        { 0x01, 0x08 },

        { 0x25, 0xFF },
        { 0x26, 0x30 },
        { 0x27, 0xFF },
        { 0x28, 0x55 },
        { 0x29, 0x08 },
        { 0x2A, 0xEB },
        { 0x2B, 0x0C },
        { 0x2C, 0xFF },
        { 0x2D, 0x75 },
        { 0x2E, 0xEC },
        { 0x2F, 0xE8 },

        { 0x44, 0xCC },
OOVPA_END;
