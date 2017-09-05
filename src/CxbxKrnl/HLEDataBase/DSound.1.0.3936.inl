// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.3936.cpp
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
// * DirectSoundCreate
// ******************************************************************
OOVPA_NO_XREF(DirectSoundCreate, 3936, 9)

        // DirectSoundCreate+0x23 : add eax, 8
        { 0x23, 0x83 },
        { 0x24, 0xC0 },
        { 0x25, 0x08 },

        // DirectSoundCreate+0x34 : push 0x1C
        { 0x34, 0x6A },
        { 0x35, 0x1C },

        // DirectSoundCreate+0x75 : sbb eax, eax
        { 0x75, 0x1B },
        { 0x76, 0xC0 },

        // DirectSoundCreate+0x9B : retn 0x0C
        { 0x9B, 0xC2 },
        { 0x9C, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_ServiceDeferredCommandsLow
// ******************************************************************
OOVPA_XREF(CMcpxAPU_ServiceDeferredCommandsLow, 3936, 12,

    XREF_DSDOWORKB,
    XRefZero)

        // CMcpxAPU_ServiceDeferredCommandsLow+0x06 : lea edi, [ebx+0x664]
        { 0x06, 0x8D },
        { 0x07, 0xBB },
        { 0x08, 0x64 },
        { 0x09, 0x06 },
        { 0x0A, 0x00 },
        { 0x0B, 0x00 },

        // CMcpxAPU_ServiceDeferredCommandsLow+0x20 : mov eax, [ecx]
        { 0x20, 0x8B },
        { 0x21, 0x01 },

        // CMcpxAPU_ServiceDeferredCommandsLow+0x33 : add edi, 4; dec ebp
        { 0x33, 0x83 },
        { 0x34, 0xC7 },
        { 0x35, 0x04 },
        { 0x36, 0x4D },
OOVPA_END;

// ******************************************************************
// * CDirectSound_DoWork
// ******************************************************************
OOVPA_XREF(CDirectSound_DoWork, 3936, 9,

    XREF_DSDOWORKA,
    XRefOne)

        // CDirectSound_DoWork+0x10 : call [CMcpxAPU_ServiceDeferredCommandsLow]
        XREF_ENTRY( 0x10, XREF_DSDOWORKB ),

        // CDirectSound_DoWork+0x08 : mov eax, [esp+0x08]
        { 0x08, 0x8B },
        { 0x09, 0x44 },
        { 0x0A, 0x24 },
        { 0x0B, 0x08 },

        // CDirectSound_DoWork+0x17 : jz +0x0B
        { 0x17, 0x74 },
        { 0x18, 0x0B },

        // CDirectSound_DoWork+0x24 : retn 0x04
        { 0x24, 0xC2 },
        { 0x25, 0x04 },
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
OOVPA_XREF(DirectSoundDoWork, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundDoWork+0x13 : call [XREF_DSDOWORKA]
        XREF_ENTRY( 0x13, XREF_DSDOWORKA ),

        // DirectSoundDoWork+0x0D : test eax, eax;jz +0x06
        { 0x0D, 0x85 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x74 },
        { 0x10, 0x06 },

        // DirectSoundDoWork+0x19 : pop esi; jz +0x0B
        { 0x19, 0x5E },
        { 0x1A, 0x74 },
        { 0x1B, 0x0B },

        // DirectSoundDoWork+0x27 : retn
        { 0x27, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundStream, 3936, 16,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        // CDirectSound_CreateSoundStream+0x2D : and esi, 0x7FF8FFF2
        { 0x2D, 0x81 },
        { 0x2E, 0xE6 },
        { 0x2F, 0xF2 },
        { 0x30, 0xFF },
        { 0x31, 0xF8 },
        { 0x32, 0x7F },

        // CDirectSound_CreateSoundStream+0x33 : add esi, 0x8007000E
        { 0x33, 0x81 },
        { 0x34, 0xC6 },
        { 0x35, 0x0E },
        { 0x36, 0x00 },
        { 0x37, 0x07 },
        { 0x38, 0x80 },

        // CDirectSound_CreateSoundStream+0x3C : js +0x16
        { 0x3C, 0x78 },
        { 0x3D, 0x16 },

        // CDirectSound_CreateSoundStream+0x72 : retn 0x10
        { 0x72, 0xC2 },
        { 0x73, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundStream, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: tidy up the xref names (are probably not accurate)

        // IDirectSound_CreateSoundStream+0x1D : call [CDirectSound::CreateSoundStream]
        XREF_ENTRY( 0x1D, XREF_DSCREATESOUNDSTREAM ),

        // IDirectSound_CreateSoundStream+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSound_CreateSoundStream+0x12 : and eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_CreateSoundStream+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundBuffer, 3936, 16,

    XREF_DSCREATESOUNDBUFFER,
    XRefZero)

    /*TODO: Need verification offset 0x00 is push ebp or different op code.
      This is needed to avoid false detection for 4039 and later XDK version.
        // CDirectSound_CreateSoundBuffer+0x00 : push ebp
        { 0x00, 0x55 },
    */
        // CDirectSound_CreateSoundBuffer+0x2D : and esi, 0x7FF8FFF2
        { 0x2D, 0x81 },
        { 0x2E, 0xE6 },
        { 0x2F, 0xF2 },
        { 0x30, 0xFF },
        { 0x31, 0xF8 },
        { 0x32, 0x7F },

        // CDirectSound_CreateSoundBuffer+0x33 : add esi, 0x8007000E
        { 0x33, 0x81 },
        { 0x34, 0xC6 },
        { 0x35, 0x0E },
        { 0x36, 0x00 },
        { 0x37, 0x07 },
        { 0x38, 0x80 },

        // CDirectSound_CreateSoundBuffer+0x3C : js +0x21
        { 0x3C, 0x78 },
        { 0x3D, 0x21 },

        // CDirectSound_CreateSoundBuffer+0x7D : retn 0x10
        { 0x7D, 0xC2 },
        { 0x7E, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(IDirectSound_CreateSoundBuffer, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: tidy up the xref names (are probably not accurate)

        // IDirectSound_CreateSoundBuffer+0x1D : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x1D, XREF_DSCREATESOUNDBUFFER ),

        // IDirectSound_CreateSoundBuffer+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSound_CreateSoundBuffer+0x12 : and eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_CreateSoundBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Release, 3936, 10)

        // IDirectSoundBuffer_Release+0x04 : lea ecx, [eax-0x1C]
        { 0x04, 0x8D },
        { 0x05, 0x48 },
        { 0x06, 0xE4 },

        // IDirectSoundBuffer_Release+0x07 : neg eax; sbb eax, eax
        { 0x07, 0xF7 },
        { 0x08, 0xD8 },
        { 0x09, 0x1B },
        { 0x0A, 0xC0 },

        // IDirectSoundBuffer_Release+0x10 : call dword ptr [ecx+0x08]
        { 0x10, 0xFF },
        { 0x11, 0x51 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetPitch
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetPitch, 3936, 11,

    XREF_DSBUFFERSETPITCHB,
    XRefZero)
        // CMcpxVoiceClient_SetPitch+0x21 : lea edi, [esi+0xB4]
        { 0x21, 0x8D },
        { 0x22, 0xBE },
        { 0x23, 0xB4 },
        { 0x24, 0x00 },

        // CMcpxVoiceClient_SetPitch+0x58 : movzx eax, word ptr [eax+2]
        { 0x58, 0x0F },
        { 0x59, 0xB7 },
        { 0x5A, 0x40 },
        { 0x5B, 0x02 },

        // CMcpxVoiceClient_SetPitch+0x5E : dec eax; sar eax, 1
        { 0x5E, 0x48 },
        { 0x5F, 0xD1 },
        { 0x60, 0xF8 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPitch, 3936, 12,

    XREF_DSBUFFERSETPITCHA,
    XRefOne)

        // CDirectSoundVoice_SetPitch+0x1B : call [CMcpxVoiceClient_SetPitch]
        XREF_ENTRY( 0x1B, XREF_DSBUFFERSETPITCHB ),

        // CDirectSoundVoice_SetPitch+0x07 : mov edx, [esp+0x10]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },

        // CDirectSoundVoice_SetPitch+0x11 : mov ecx, [eax+0x18]
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x13, 0x18 },

        // CDirectSoundVoice_SetPitch+0x23 : jz +0x0B
        { 0x23, 0x74 },
        { 0x24, 0x0B },

        // CDirectSoundVoice_SetPitch+0x34 : retn 0x08
        { 0x34, 0xC2 },
        { 0x35, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetPitch+0x15 : call [CDirectSoundVoice_SetPitch]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETPITCHA ),

        // IDirectSoundBuffer_SetPitch+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetPitch+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetPitch+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer::GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 3936, 13,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x10 : mov eax, [ebp+0x08]
        { 0x10, 0x8B },
        { 0x11, 0x45 },
        { 0x12, 0x08 },

        // CMcpxBuffer_GetStatus+0x16 : xor ecx, ecx; inc ecx
        { 0x16, 0x33 },
        { 0x17, 0xC9 },
        { 0x18, 0x41 },

        // CMcpxBuffer_GetStatus+0x1C : jz +0x17
        { 0x1C, 0x74 },
        { 0x1D, 0x17 },

        // CMcpxBuffer_GetStatus+0x2F : mov [eax], 5
        { 0x2F, 0xC7 },
        { 0x30, 0x00 },
        { 0x31, 0x05 },

        // CMcpxBuffer_GetStatus+0x48 : retn 0x04
        { 0x48, 0xC2 },
        { 0x49, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 3936, 10,

    XREF_CDirectSoundBuffer_GetStatus,
    XRefOne)

        // CDirectSoundBuffer_GetStatus+0x14 : call [CMcpxBuffer::GetStatus]
        XREF_ENTRY( 0x15, XREF_CMcpxBuffer_GetStatus),

        // CDirectSoundBuffer_GetStatus+0x07 : push [esp+0x10]
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },

        // CDirectSoundBuffer_GetStatus+0x11 : mov ecx, [eax+0x20]
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x13, 0x20 },

        // CDirectSoundBuffer_GetStatus+0x2E : retn 0x08
        { 0x2E, 0xC2 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_GetStatus+0x15 : call [CDirectSoundBuffer::GetStatus]
        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_GetStatus ),

        // IDirectSoundBuffer_GetStatus+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_GetStatus+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_GetStatus+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 3936, 11,

    XREF_DSBUFFERSETVOLUMEB,
    XRefZero)

        // CMcpxVoiceClient_SetVolume+0x26 : mov ecx, [esi+0xE0]
        { 0x26, 0x8B },
        { 0x27, 0x8E },
        { 0x28, 0xE0 },
        { 0x29, 0x00 },

        // CMcpxVoiceClient_SetVolume+0x2F : movzx ecx, word ptr [ecx+2]
        { 0x2F, 0x0F },
        { 0x30, 0xB7 },
        { 0x31, 0x49 },
        { 0x32, 0x02 },

        // CMcpxVoiceClient_SetVolume+0x35 : dec ecx; sar ecx, 1
        { 0x35, 0x49 },
        { 0x36, 0xD1 },
        { 0x37, 0xF9 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVolume, 3936, 12,

    XREF_DSBUFFERSETVOLUMEA,
    XRefOne)

        // CDirectSoundVoice_SetVolume+0x1E : call [CMcpxVoiceClient_SetVolume]
        XREF_ENTRY( 0x1E, XREF_DSBUFFERSETVOLUMEB ),

        // IDirectSoundBuffer_SetVolumeA+0x07 : mov ecx, [esp+0x0C]
        { 0x07, 0x8B },
        { 0x08, 0x4C },
        { 0x09, 0x24 },
        { 0x0A, 0x0C },

        // CDirectSoundVoice_SetVolume+0x11 : mov eax, [ecx+0x18]
        { 0x11, 0x8B },
        { 0x12, 0x41 },
        { 0x13, 0x18 },

        // CDirectSoundVoice_SetVolume+0x26 : jz +0x0B
        { 0x26, 0x74 },
        { 0x27, 0x0B },

        // CDirectSoundVoice_SetVolume+0x37 : retn 0x08
        { 0x37, 0xC2 },
        { 0x38, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetVolume+0x15 : call [CDirectSoundVoice_SetVolume]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETVOLUMEA ),

        // IDirectSoundBuffer_SetVolume+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetVolume+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetVolume+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 3936, 11,

    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XRefZero)

        // CMcpxBuffer_SetCurrentPosition+0x25 : mov eax, [esi+0x148]
        { 0x25, 0x8B },
        { 0x26, 0x86 },
        { 0x27, 0x48 },
        { 0x28, 0x01 },

        // CMcpxBuffer_SetCurrentPosition+0x4A : push edi; push 4
        { 0x4A, 0x57 },
        { 0x4B, 0x6A },
        { 0x4C, 0x04 },

        // CMcpxBuffer_SetCurrentPosition+0x71 : movzx eax, word ptr [eax+2]
        { 0x71, 0x0F },
        { 0x72, 0xB7 },
        { 0x73, 0x40 },
        { 0x74, 0x02 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 3936, 12,

    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XRefOne)

        // CDirectSoundBuffer_SetCurrentPosition+0x15 : call [CMcpxBuffer_SetCurrentPosition]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETCURRENTPOSITIONB ),

        // CDirectSoundBuffer_SetCurrentPosition+0x0D : mov eax, [esp+0x10]
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },

        // CDirectSoundBuffer_SetCurrentPosition+0x11 : mov ecx, [eax+0x20]
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x13, 0x20 },

        // CDirectSoundBuffer_SetCurrentPosition+0x1D : jz +0x0B
        { 0x1D, 0x74 },
        { 0x1E, 0x0B },

        // CDirectSoundBuffer_SetCurrentPosition+0x2E : retn 0x08
        { 0x2E, 0xC2 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetCurrentPosition+0x15 : call [CDirectSoundBuffer_SetCurrentPosition]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA ),

        // IDirectSoundBuffer_SetCurrentPosition+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetCurrentPosition+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetCurrentPosition+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetPlayRegion
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPlayRegion, 3936, 10,

    XREF_DSBUFFERSETPLAYREGIONA,
    XRefZero)

        // CDirectSoundBuffer_SetPlayRegion+0x20 : cmp edx, [ecx+0x54]
        { 0x20, 0x3B },
        { 0x21, 0x51 },
        { 0x22, 0x54 },

        // CDirectSoundBuffer_SetPlayRegion+0x2C : mov ecx, [esi+0x20]
        { 0x2C, 0x8B },
        { 0x2D, 0x4E },
        { 0x2E, 0x20 },

        // CDirectSoundBuffer_SetPlayRegion+0x38 : jz +0x0B
        { 0x38, 0x74 },
        { 0x39, 0x0B },

        // CDirectSoundBuffer_SetPlayRegion+0x4A : retn 0x0C
        { 0x4A, 0xC2 },
        { 0x4B, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPlayRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPlayRegion, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetPlayRegion+0x19 : call [CDirectSoundBuffer_SetPlayRegion]
        XREF_ENTRY( 0x19, XREF_DSBUFFERSETPLAYREGIONA ),

        // IDirectSoundBuffer_SetPlayRegion+0x04 : push [esp+0x0C]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetPlayRegion+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_SetPlayRegion+0x1D : retn 0x0C
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Lock
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Lock, 3936, 10,

    XREF_DSBUFFERLOCKA,
    XRefZero)

        // CDirectSoundBuffer_Lock+0x37 : mov eax, [eax+0x4C]
        { 0x37, 0x8B },
        { 0x38, 0x40 },
        { 0x39, 0x4C },

        // CDirectSoundBuffer_Lock+0x68 : mov [edi], ebx; jz +0x1F
        { 0x68, 0x89 },
        { 0x69, 0x1F },
        { 0x6A, 0x74 },
        { 0x6B, 0x1F },

        // CDirectSoundBuffer_Lock+0x85 : and dword ptr [edx], 0
        { 0x85, 0x83 },
        { 0x86, 0x22 },
        { 0x87, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Lock
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Lock, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Lock+0x28 : call [CDirectSoundBuffer_Lock]
        XREF_ENTRY( 0x28, XREF_DSBUFFERLOCKA ),

        // IDirectSoundBuffer_Lock+0x03 : push [esp+0x24]
        { 0x03, 0xFF },
        { 0x04, 0x75 },
        { 0x05, 0x24 },

        // IDirectSoundBuffer_Lock+0x11 : add eax, 0xFFFFFFE4
        { 0x11, 0x83 },
        { 0x12, 0xC0 },
        { 0x13, 0xE4 },

        // IDirectSoundBuffer_Lock+0x2D : retn 0x20
        { 0x2D, 0xC2 },
        { 0x2E, 0x20 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 3936, 12,

    XREF_DSBUFFERSETHEADROOMA,
    XRefOne)

    // WARNING: I am unsure if this is SetHeadroom

        // CDirectSoundVoice_SetHeadroom+0x23 : call [CMcpxVoiceClient_SetVolume]
        XREF_ENTRY( 0x23, XREF_DSBUFFERSETVOLUMEB ),

        // CDirectSoundVoice_SetHeadroom+0x07 : mov edx, [esp+0x0C]
        { 0x07, 0x8B },
        { 0x08, 0x54 },
        { 0x09, 0x24 },
        { 0x0A, 0x0C },

        // CDirectSoundVoice_SetHeadroom+0x11 : mov eax, [edx+0x18]
        { 0x11, 0x8B },
        { 0x12, 0x42 },
        { 0x13, 0x18 },

        // CDirectSoundVoice_SetHeadroom+0x2B : jz +0x0B
        { 0x2B, 0x74 },
        { 0x2C, 0x0B },

        // CDirectSoundVoice_SetHeadroom+0x3C : retn 0x08
        { 0x3C, 0xC2 },
        { 0x3D, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetHeadroom+0x15 : call [CDirectSoundVoice_SetHeadroom]
        XREF_ENTRY( 0x15, XREF_DSBUFFERSETHEADROOMA ),

        // IDirectSoundBuffer_SetHeadroom+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetHeadroom+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetHeadroom+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 3936, 9,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        // CDirectSoundBuffer_SetBufferData+0x22 : cmp ecx, [eax+0x4C]
        { 0x22, 0x3B },
        { 0x23, 0x48 },
        { 0x24, 0x4C },

        // CDirectSoundBuffer_SetBufferData+0x38 : mov ecx, [esi+0x1C]
        { 0x38, 0x8B },
        { 0x39, 0x4E },
        { 0x3A, 0x1C },

        // CDirectSoundBuffer_SetBufferData+0x71 : leave; retn 0x0C
        { 0x71, 0xC9 },
        { 0x72, 0xC2 },
        { 0x73, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetBufferData+0x18 : call [CDirectSoundBuffer_SetBufferData]
        XREF_ENTRY( 0x19, XREF_DSSETBUFFERDATA ),

        // IDirectSoundBuffer_SetBufferData+0x04 : push [esp+0x0C]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetBufferData+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_SetBufferData+0x1D : retn 0x0C
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetMixBins
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetMixBins, 3936, 15,

    XREF_DSSETMIXBINSB,
    XRefZero)

        // CMcpxVoiceClient_SetMixBins+0x1C : lea eax, [esi+0x84]
        { 0x1D, 0x86 },
        { 0x1E, 0x84 },
        { 0x21, 0x00 },

        // CMcpxVoiceClient_SetMixBins+0x2E : mov eax, [esi+0xE0]
        { 0x2E, 0x8B },
        { 0x30, 0xE0 },
        { 0x31, 0x00 },

        // CMcpxVoiceClient_SetMixBins+0x41 : jz +0x7F
        { 0x41, 0x74 },
        { 0x42, 0x7F },

        // CMcpxVoiceClient_SetMixBins+0x5A : mov ds:[0xFE8202F8], eax
        { 0x5A, 0xA3 },
        { 0x5B, 0xF8 },
        { 0x5C, 0x02 },
        { 0x5D, 0x82 },

        // CMcpxVoiceClient_SetMixBins+0xBF : jb +0x8C
        { 0xBF, 0x72 },
        { 0xC0, 0x8C },

        // CMcpxVoiceClient_SetMixBins+0xD6 : retn
        { 0xD6, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 3936, 9,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // CDirectSoundVoice_SetMixBins+0x28 : call [CMcpxVoiceClient::SetMixBins]
        XREF_ENTRY( 0x29, XREF_DSSETMIXBINSB ),

        // CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x18]
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x13, 0x18 },

        // CDirectSoundVoice_SetMixBins+0x25 : mov ecx, [esi+0x14]
        { 0x25, 0x8B },
        { 0x26, 0x4E },
        { 0x27, 0x14 },

        // CDirectSoundVoice_SetMixBins+0x55 : retn 0x08
        { 0x55, 0xC2 },
        { 0x56, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBins, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_SetFrequency+0x0C : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x0D, XREF_DSSETMIXBINSA ),

        // CDirectSoundStream_SetMixBins+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetMixBins+0x08 : add eax, 0x04
        { 0x08, 0x83 },
        { 0x09, 0xC0 },
        { 0x0A, 0x04 },

        // CDirectSoundStream_SetMixBins+0x11 : retn 0x08
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMixBins+0x14 : call [CDirectSoundVoice::SetMixBins]
        XREF_ENTRY( 0x15, XREF_DSSETMIXBINSA ), 

        // IDirectSoundBuffer_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 3936, 11,

	XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x12 : mov eax, [esi+8]
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x14, 0x08 },

        // CMcpxBuffer_GetCurrentPosition+0x1A : jnz +0x79
        { 0x1A, 0x75 },
        { 0x1B, 0x79 },

        // CMcpxBuffer_GetCurrentPosition+0x8E : div dword ptr [ecx+0x4C]
        { 0x8E, 0xF7 },
        { 0x8F, 0x71 },
        { 0x90, 0x4C },

        // CMcpxBuffer_GetCurrentPosition+0xC8 : retn 0x08
        { 0xC8, 0xC2 },
        { 0xC9, 0x08 },
        { 0xCA, 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition2, 3936, 11,

	XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x12 : mov eax, [esi+0x18]
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        { 0x14, 0x18 },

        // CMcpxBuffer_GetCurrentPosition+0x1A : jnz +0x79
        { 0x1A, 0x75 },
        { 0x1B, 0x79 },

        // CMcpxBuffer_GetCurrentPosition+0x8E : div dword ptr [ecx+0x4C]
        { 0x8E, 0xF7 },
        { 0x8F, 0x71 },
        { 0x90, 0x4C },

        // CMcpxBuffer_GetCurrentPosition+0xC8 : retn 0x08
        { 0xC8, 0xC2 },
        { 0xC9, 0x08 },
        { 0xCA, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 3936, 9,

    XREF_CDirectSoundBuffer_GetCurrentPosition,
    XRefOne)

        // CDirectSoundBuffer_GetCurrentPosition+0x19 : call [CMcpxBuffer::GetCurrentPosition]
        XREF_ENTRY( 0x19, XREF_CMcpxBuffer_GetCurrentPosition), 

        // CDirectSoundBuffer_GetCurrentPosition+0x0D : mov eax, [esp+0x10]
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },

        // CDirectSoundBuffer_GetCurrentPosition+0x21 : jz +0x0B
        { 0x21, 0x74 },
        { 0x22, 0x0B },

        // CDirectSoundBuffer_GetCurrentPosition+0x32 : retn 0x0C
        { 0x32, 0xC2 },
        { 0x33, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_GetCurrentPosition+0x19 : call [CDirectSoundBuffer::GetCurrentPosition]
        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_GetCurrentPosition), 

        // IDirectSoundBuffer_GetCurrentPosition+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_GetCurrentPosition+0x13 : sbb ecx, ecx
        { 0x13, 0x1B },
        { 0x14, 0xC9 },

        // IDirectSoundBuffer_GetCurrentPosition+0x15 : and ecx, eax
        { 0x15, 0x23 },
        { 0x16, 0xC8 },
OOVPA_END;
#if 0 //Moved to 3911
// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 3936, 7,

    XREF_CDirectSoundBuffer_Play,
    XRefZero)

        { 0x07, 0xFF },
        { 0x0C, 0xF0 },
        { 0x13, 0x24 },
        { 0x1A, 0x85 },
        { 0x25, 0xFF },
        { 0x2B, 0x8B },
        { 0x2F, 0xC2 },
OOVPA_END;
#endif
#if 0 // Moved to 3911
// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 3936, 16,

    XRefNoSaveIndex,
    XRefOne)

	// IDirectSoundBuffer_Play+0x1D : call CDirectSoundBuffer::Play
	XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_Play ),

        // IDirectSoundBuffer_Play+0x00 : push [esp+0x10]
        { 0x00, 0xFF },
        { 0x01, 0x74 },
        { 0x02, 0x24 },
        { 0x03, 0x10 },

	// IDirectSoundBuffer_Play+0x04 : mov eax, [esp+4+arg_0]
	{ 0x04, 0x8B },
	{ 0x05, 0x44 },
	{ 0x06, 0x24 },
	{ 0x07, 0x08 },

        // IDirectSoundBuffer_Play+0x12 : and eax, [0xFFFFFFE4]
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B },
        { 0x18, 0xC9 },

        // IDirectSoundBuffer_Play+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by another 3936 version
// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 3936, 12,

    XRefNoSaveIndex,
    XRefOne)

	// IDirectSoundBuffer_Play+0x1D : call CDirectSoundBuffer::Play
	XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_Play ),

        // IDirectSoundBuffer_Play+0x00 : push [esp+0x10]
        { 0x00, 0xFF },
        { 0x01, 0x74 },
        { 0x02, 0x24 },
        { 0x03, 0x10 },

        // IDirectSoundBuffer_Play+0x12 : and eax, [0xFFFFFFE4]
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xE4 },

        // IDirectSoundBuffer_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B },
        { 0x18, 0xC9 },

        // IDirectSoundBuffer_Play+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;
#endif
#if 0 // Was ?SetVolume@CDirectSoundVoice@@QAGJJ@Z, replaced by CDirectSoundVoice_SetVolume 3936 version
// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 3936, 10,

    XREF_DSSTREAMSETVOLUME,
    XRefZero)

    // TODO: re-use for IDirectSoundBuffer_SetVolume!

        // CMcpxVoiceClient_SetVolume+0x14 : sub edx, [eax+0x1C]
        { 0x14, 0x2B },
        { 0x15, 0x50 },
        { 0x16, 0x1C },

        // CMcpxVoiceClient_SetVolume+0x1A : mov ecx, [ecx+0x14]
        { 0x1A, 0x8B },
        { 0x1B, 0x49 },
        { 0x1C, 0x14 },

        // CMcpxVoiceClient_SetVolume+0x26 : jz +0x0B
        { 0x26, 0x74 },
        { 0x27, 0x0B },

        // CMcpxVoiceClient_SetVolume+0x37 : retn 0x08
        { 0x37, 0xC2 },
        { 0x38, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetVolume+0x0D : call [CMcpxVoiceClient_SetVolume]
        XREF_ENTRY( 0x0D, XREF_DSSTREAMSETVOLUME ),

        // CDirectSoundStream_SetVolume+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetVolume+0x08 : add eax, 0x04
        { 0x08, 0x83 },
        { 0x09, 0xC0 },
        { 0x0A, 0x04 },

        // CDirectSoundStream_SetVolume+0x11 : retn 0x08
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dParameters
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dParameters, 3936, 11,

    XREF_DSSTREAMSETALLPARAMETERS1B,
    XRefZero)

        { 0x02, 0x74 },
        { 0x08, 0x6A },
        { 0x09, 0x10 },
        { 0x0A, 0x59 },
        { 0x0B, 0x8D },
        { 0x0C, 0x78 },
        { 0x0D, 0x1C },
        { 0x0E, 0xF3 },
        { 0x14, 0x00 },
        { 0x18, 0x7C },
        { 0x1F, 0x09 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetAllParameters, 3936, 12,

    XREF_DSSTREAMSETALLPARAMETERS1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetAllParametersA!?

        // CDirectSoundVoice_SetAllParameters+0x25 : call [CMcpxVoiceClient_Set3dParameters]
        XREF_ENTRY( 0x1F, XREF_DSSTREAMSETALLPARAMETERS1B ),

        // CDirectSoundVoice_SetAllParameters+0x09 : mov eax, [esp+0x14]
        { 0x09, 0x8B },
        { 0x0A, 0x44 },
        { 0x0B, 0x24 },
        { 0x0C, 0x14 },

        // CDirectSoundVoice_SetAllParameters+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 },
        { 0x0E, 0xD0 },
        { 0x0F, 0x83 },
        { 0x10, 0xE0 },
        { 0x11, 0x01 },

        // CDirectSoundVoice_SetAllParameters+0x38 : retn 0x0C
        { 0x38, 0xC2 },
        { 0x39, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParameters, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetAllParameters+0x11 : call [CDirectSoundVoice_SetAllParameters]
        XREF_ENTRY( 0x11, XREF_DSSTREAMSETALLPARAMETERS1A ),

        // CDirectSoundStream_SetAllParameters+0x04 : push eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetAllParameters+0x0C : add eax, 0x04
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x04 },

        // CDirectSoundStream_SetAllParameters+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;
#if 0 // No longer used
// ******************************************************************
// * CMcpxVoiceClient_Set3dConeAngles
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dConeAngles, 3936, 12,

    XREF_DSSTREAMSETCONEANGLES1B,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesB!?

        // CMcpxVoiceClient_Set3dConeAngles+0x04 : or dword ptr [eax+0x80], 0x10
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0x80 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x10 },

        // CMcpxVoiceClient_Set3dConeAngles+0x10 : mov [ecx+0x38], eax
        { 0x10, 0x89 },
        { 0x11, 0x41 },
        { 0x12, 0x38 },

        // CMcpxVoiceClient_Set3dConeAngles+0x25 : retn 0x0C
        { 0x25, 0xC2 },
        { 0x26, 0x0C },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by CDirectSoundVoice_SetConeAngles 3936
// ******************************************************************
// * CDirectSoundVoice_SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeAngles, 3936, 12,

    XREF_DSSTREAMSETCONEANGLES1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesA!?

        // CDirectSoundVoice_SetConeAngles+0x23 : call [CMcpxVoiceClient_Set3dConeAngles]
        XREF_ENTRY( 0x23, XREF_DSSTREAMSETCONEANGLES1B ),

        // CDirectSoundVoice_SetConeAngles+0x09 : mov eax, [esp+0x18]
        { 0x09, 0x8B },
        { 0x0A, 0x44 },
        { 0x0B, 0x24 },
        { 0x0C, 0x18 },

        // CDirectSoundVoice_SetConeAngles+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 },
        { 0x0E, 0xD0 },
        { 0x0F, 0x83 },
        { 0x10, 0xE0 },
        { 0x11, 0x01 },

        // CDirectSoundVoice_SetConeAngles+0x3C : retn 0x10
        { 0x3C, 0xC2 },
        { 0x3D, 0x10 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundStream_SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeAngles, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeAngles+0x15 : call [CDirectSoundVoice_SetConeAngles]
        XREF_ENTRY( 0x15, XREF_DSVOICESETCONEANGLES ),

        // CDirectSoundStream_SetConeAngles+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetConeAngles+0x0C : add eax, 0x04
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x04 },

        // CDirectSoundStream_SetConeAngles+0x19 : retn 0x10
        { 0x19, 0xC2 },
        { 0x1A, 0x10 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dConeOutsideVolume, 3936, 12,

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeB!?

        // CMcpxVoiceClient_Set3dConeOutsideVolume+0x04 : or dword ptr [eax+0x80], 0x10
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0x80 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x10 },

        // CMcpxVoiceClient_Set3dConeOutsideVolume+0x10 : mov [ecx+0x4C], eax
        { 0x10, 0x89 },
        { 0x11, 0x41 },
        { 0x12, 0x4C },

        // CMcpxVoiceClient_Set3dConeOutsideVolume+0x1E : retn 0x08
        { 0x1E, 0xC2 },
        { 0x1F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 3936, 12,

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeA!?

        // CDirectSoundVoice_SetConeOutsideVolume+0x1F : call [CMcpxVoiceClient_Set3dConeOutsideVolume]
        XREF_ENTRY( 0x1F, XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B ),

        // CDirectSoundVoice_SetConeOutsideVolume+0x09 : mov eax, [esp+0x14]
        { 0x09, 0x8B },
        { 0x0A, 0x44 },
        { 0x0B, 0x24 },
        { 0x0C, 0x14 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 },
        { 0x0E, 0xD0 },
        { 0x0F, 0x83 },
        { 0x10, 0xE0 },
        { 0x11, 0x01 },

        // CDirectSoundVoice_SetConeOutsideVolume+0x38 : retn 0x0C
        { 0x38, 0xC2 },
        { 0x39, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeOutsideVolume+0x11 : call [CDirectSoundVoice_SetConeOutsideVolume]
        XREF_ENTRY( 0x11, XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A ),

        // CDirectSoundStream_SetConeOutsideVolume+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetConeOutsideVolume+0x0C : add eax, 0x04
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x04 },

        // CDirectSoundStream_SetConeOutsideVolume+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dMaxDistance
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dMaxDistance, 3936, 12,

    XREF_DSSTREAMSETMAXDISTANCE1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceC!?

        // CMcpxVoiceClient_Set3dMaxDistance+0x04 : or byte ptr [ecx+0x80], 0x04
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0x80 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x04 },

        // CMcpxVoiceClient_Set3dMaxDistance+0x10 : mov [ecx+0x54], eax
        { 0x10, 0x89 },
        { 0x11, 0x41 },
        { 0x12, 0x54 },

        // CMcpxVoiceClient_Set3dMaxDistance+0x1E : retn 0x08
        { 0x1E, 0xC2 },
        { 0x1F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 3936, 15,

    XREF_DSSTREAMSETMAXDISTANCE1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceB!?

        // CDirectSoundVoice_SetMaxDistance+0x23 : call [CMcpxVoiceClient_Set3dMaxDistance]
        XREF_ENTRY( 0x23, XREF_DSSTREAMSETMAXDISTANCE1C ),

        // CDirectSoundVoice_SetMaxDistance+0x0D : mov eax, [esp+0x14]
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x14 },

        // CDirectSoundVoice_SetMaxDistance+0x11 : not eax
        { 0x11, 0xF7 },
        { 0x12, 0xD0 },

        // CDirectSoundVoice_SetMaxDistance+0x13 : and eax, 1
        { 0x13, 0x83 },
        { 0x14, 0xE0 },
        { 0x15, 0x01 },

        // CDirectSoundVoice_SetMaxDistance+0x1C : mov ecx, [eax+0x14]
        { 0x1C, 0x8B },
        { 0x1D, 0x48 },
        { 0x1E, 0x14 },

        // CDirectSoundVoice_SetMaxDistance+0x3C : retn 0x0C
        { 0x3C, 0xC2 },
        { 0x3D, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistance, 3936, 10,

    XREF_DSSTREAMSETMAXDISTANCE1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceA!?

        // CDirectSoundStream_SetMaxDistance+0x15 : call [CDirectSoundVoice_SetMaxDistance]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETMAXDISTANCE1B ),

        // CDirectSoundStream_SetMaxDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // CDirectSoundStream_SetMaxDistance+0x0D : add eax, 0x04
        { 0x0D, 0x83 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x04 },

        // CDirectSoundStream_SetMaxDistance+0x19 : retn 0x0C
        { 0x19, 0xC2 },
        { 0x1A, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetMaxDistance, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMaxDistance+0x11 : call [CDirectSoundStream_SetMaxDistance]
        XREF_ENTRY( 0x11, XREF_DSSTREAMSETMAXDISTANCE1A ),

        // CDirectSoundStream_SetMaxDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // CDirectSoundStream_SetMaxDistance+0x0C : push [esp+0x0C]
        { 0x0C, 0xFF },
        { 0x0D, 0x74 },
        { 0x0E, 0x24 },
        { 0x0F, 0x0C },

        // CDirectSoundStream_SetMaxDistance+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dMinDistance
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dMinDistance, 3936, 12,

    XREF_DSSTREAMSETMINDISTANCE1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceC!?

        // CMcpxVoiceClient_Set3dMinDistance+0x04 : or byte ptr [ecx+0x80], 0x04
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0x80 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x04 },

        // CMcpxVoiceClient_Set3dMinDistance+0x10 : mov [ecx+0x50], eax
        { 0x10, 0x89 },
        { 0x11, 0x41 },
        { 0x12, 0x50 },

        // CMcpxVoiceClient_Set3dMinDistance+0x1E : retn 0x08
        { 0x1E, 0xC2 },
        { 0x1F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 3936, 15,

    XREF_DSSTREAMSETMINDISTANCE1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceB!?

        // CDirectSoundVoice_SetMinDistance+0x23 : call [CMcpxVoiceClient_Set3dMinDistance]
        XREF_ENTRY( 0x23, XREF_DSSTREAMSETMINDISTANCE1C ),

        // CDirectSoundVoice_SetMinDistance+0x0D : mov eax, [esp+0x14]
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x14 },

        // CDirectSoundVoice_SetMinDistance+0x11 : not eax
        { 0x11, 0xF7 },
        { 0x12, 0xD0 },

        // CDirectSoundVoice_SetMinDistance+0x13 : and eax, 1
        { 0x13, 0x83 },
        { 0x14, 0xE0 },
        { 0x15, 0x01 },

        // CDirectSoundVoice_SetMinDistance+0x1C : mov ecx, [eax+0x14]
        { 0x1C, 0x8B },
        { 0x1D, 0x48 },
        { 0x1E, 0x14 },

        // CDirectSoundVoice_SetMinDistance+0x3C : retn 0x0C
        { 0x3C, 0xC2 },
        { 0x3D, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 3936, 10,

    XREF_DSSTREAMSETMINDISTANCE1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceA!?

        // CDirectSoundStream_SetMinDistance+0x15 : call [CDirectSoundVoice_SetMinDistance]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETMINDISTANCE1B ),

        // CDirectSoundStream_SetMinDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // CDirectSoundStream_SetMinDistance+0x0D : add eax, 0x04
        { 0x0D, 0x83 },
        { 0x0E, 0xC0 },
        { 0x0F, 0x04 },

        // CDirectSoundStream_SetMinDistance+0x19 : retn 0x0C
        { 0x19, 0xC2 },
        { 0x1A, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetMinDistance, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMinDistance+0x11 : call [CDirectSoundStream_SetMinDistance]
        XREF_ENTRY( 0x11, XREF_DSSTREAMSETMINDISTANCE1A ),

        // CDirectSoundStream_SetMinDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // CDirectSoundStream_SetMinDistance+0x0C : push [esp+0x0C]
        { 0x0C, 0xFF },
        { 0x0D, 0x74 },
        { 0x0E, 0x24 },
        { 0x0F, 0x0C },

        // CDirectSoundStream_SetMinDistance+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dVelocity
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dVelocity, 3936, 12,

    XREF_DSSTREAMSETVELOCITY1C,
    XRefZero)

        // CMcpxVoiceClient_Set3dVelocity+0x09 : movsd; movsd; movsd
        { 0x09, 0xA5 },
        { 0x0A, 0xA5 },
        { 0x0B, 0xA5 },

        // CMcpxVoiceClient_Set3dVelocity+0x0C : or byte ptr [ecx+0x80], 0x40
        { 0x0C, 0x83 },
        { 0x0D, 0x89 },
        { 0x0E, 0x80 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x40 },

        // CMcpxVoiceClient_Set3dVelocity+0x25 : retn 0x08
        { 0x25, 0xC2 },
        { 0x26, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVelocity, 3936, 14,

    XREF_DSSTREAMSETVELOCITY1B,
    XRefOne)

        // CDirectSoundVoice_SetVelocity+0x35 : call [CMcpxVoiceClient_Set3dVelocity]
        XREF_ENTRY( 0x35, XREF_DSSTREAMSETVELOCITY1C ),

        // CDirectSoundVoice_SetVelocity+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B },
        { 0x22, 0x45 },
        { 0x23, 0x18 },

        // CDirectSoundVoice_SetVelocity+0x24 : not eax
        { 0x24, 0xF7 },
        { 0x25, 0xD0 },

        // CDirectSoundVoice_SetVelocity+0x26 : and eax, 1
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        // CDirectSoundVoice_SetVelocity+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B },
        { 0x32, 0x48 },
        { 0x33, 0x14 },

        // CDirectSoundVoice_SetVelocity+0x4F : retn 0x14
        { 0x4F, 0xC2 },
        { 0x50, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocity, 3936, 9,

    XREF_DSSTREAMSETVELOCITY1A,
    XRefOne)

        // CDirectSoundStream_SetVelocity+0x25 : call [CDirectSoundVoice_SetVelocity]
        XREF_ENTRY( 0x25, XREF_DSSTREAMSETVELOCITY1B ),

        // CDirectSoundStream_SetVelocity+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundStream_SetVelocity+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x0C },

        // CDirectSoundStream_SetVelocity+0x2A : retn 0x14
        { 0x2A, 0xC2 },
        { 0x2B, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetVelocity, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetVelocity+0x21 : call [CDirectSoundStream_SetVelocity]
        XREF_ENTRY( 0x21, XREF_DSSTREAMSETVELOCITY1A ),

        // CDirectSoundStream_SetVelocity+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundStream_SetVelocity+0x1A : fstp [esp]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // CDirectSoundStream_SetVelocity+0x26 : retn 0x14
        { 0x26, 0xC2 },
        { 0x27, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 3936, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        // CDirectSoundBuffer_SetVelocity+0x20 : call [CDirectSoundStream::SetVelocityB]
        XREF_ENTRY( 0x21, XREF_DSSTREAMSETVELOCITY1B ),

        // CDirectSoundBuffer_SetVelocity+0x06 : fld [ebp+arg_C]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundBuffer_SetVelocity+0x1A : fstp [esp+0x10+var_10]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // CDirectSoundBuffer_SetVelocity+0x26 : retn 0x14
        { 0x27, 0x14 },
        { 0x28, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVelocity, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetVelocity+0x2C : call [CDirectSoundBuffer::SetVelocity]
        XREF_ENTRY( 0x2D, XREF_DSBUFFERSETVELOCITY ),

        // IDirectSoundBuffer_SetVelocity+0x09 : mov eax, [ebp+arg_0]
        { 0x09, 0x8B },
        { 0x0A, 0x45 },
        { 0x0B, 0x08 },

        // IDirectSoundBuffer_SetVelocity+0x28 : fstp [esp+0x10+var_10]
        { 0x28, 0xD9 },
        { 0x29, 0x1C },
        { 0x2A, 0x24 },

        // IDirectSoundBuffer_SetVelocity+0x32 : retn 0x14
        { 0x33, 0x14 },
        { 0x34, 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dConeOrientation
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dConeOrientation, 3936, 11,

    XREF_DSSTREAMSETCONEORIENTATION1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationC!?

        // CMcpxVoiceClient_Set3dConeOrientation+0x0C : or byte ptr [ecx+0x80], 0x18
        { 0x0C, 0x83 },
        { 0x0D, 0x89 },
        { 0x0E, 0x80 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0x18 },

        // CMcpxVoiceClient_Set3dConeOrientation+0x1A : jz +0x07
        { 0x1A, 0x74 },
        { 0x1B, 0x07 },

        // CMcpxVoiceClient_Set3dConeOrientation+0x25 : retn 0x08
        { 0x25, 0xC2 },
        { 0x26, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeOrientation, 3936, 14,

    XREF_DSSTREAMSETCONEORIENTATION1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationB!?

        // CDirectSoundVoice_SetConeOrientation+0x35 : call [CMcpxVoiceClient_Set3dConeOrientation]
        XREF_ENTRY( 0x35, XREF_DSSTREAMSETCONEORIENTATION1C ),

        // CDirectSoundVoice_SetConeOrientation+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B },
        { 0x22, 0x45 },
        { 0x23, 0x18 },

        // CDirectSoundVoice_SetConeOrientation+0x24 : not eax
        { 0x24, 0xF7 },
        { 0x25, 0xD0 },

        // CDirectSoundVoice_SetConeOrientation+0x26 : and eax, 1
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        // CDirectSoundVoice_SetConeOrientation+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B },
        { 0x32, 0x48 },
        { 0x33, 0x14 },

        // CDirectSoundVoice_SetConeOrientation+0x4F : retn 0x14
        { 0x4F, 0xC2 },
        { 0x50, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientation, 3936, 9,

    XREF_DSSTREAMSETCONEORIENTATION1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationA!?

        // CDirectSoundStream_SetConeOrientation+0x25 : call [CDirectSoundVoice_SetConeOrientation]
        XREF_ENTRY( 0x25, XREF_DSSTREAMSETCONEORIENTATION1B ),

        // CDirectSoundStream_SetConeOrientation+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundStream_SetConeOrientation+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x0C },

        // CDirectSoundStream_SetConeOrientation+0x2A : retn 0x14
        { 0x2A, 0xC2 },
        { 0x2B, 0x14 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetConeOrientation
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetConeOrientation, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_SetConeOrientation+0x21 : call [CDirectSoundStream_SetConeOrientation]
        XREF_ENTRY( 0x21, XREF_DSSTREAMSETCONEORIENTATION1A ),

        // IDirectSoundStream_SetConeOrientation+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // IDirectSoundStream_SetConeOrientation+0x1A : fstp [esp]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // IDirectSoundStream_SetConeOrientation+0x26 : retn 0x14
        { 0x26, 0xC2 },
        { 0x27, 0x14 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dPosition
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dPosition, 3936, 11,

    XREF_DSSTREAMSETPOSITION1C,
    XRefZero)

        // CMcpxVoiceClient_Set3dPosition+0x0C : or byte ptr [ecx+0x80], 0xFF
        { 0x0C, 0x80 },
        { 0x0D, 0x89 },
        { 0x0E, 0x80 },
        { 0x0F, 0x00 },
        { 0x10, 0x00 },
        { 0x11, 0x00 },
        { 0x12, 0xFF },

        // CMcpxVoiceClient_Set3dPosition+0x1A : jz +0x07
        { 0x1A, 0x74 },
        { 0x1B, 0x07 },

        // CMcpxVoiceClient_Set3dPosition+0x25 : retn 0x08
        { 0x25, 0xC2 },
        { 0x26, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPosition, 3936, 14,

    XREF_DSSTREAMSETPOSITION1B,
    XRefOne)

        // CDirectSoundVoice_SetPosition+0x35 : call [CMcpxVoiceClient_Set3dPosition]
        XREF_ENTRY( 0x35, XREF_DSSTREAMSETPOSITION1C ),

        // CDirectSoundVoice_SetPosition+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B },
        { 0x22, 0x45 },
        { 0x23, 0x18 },

        // CDirectSoundVoice_SetPosition+0x24 : not eax
        { 0x24, 0xF7 },
        { 0x25, 0xD0 },

        // CDirectSoundVoice_SetPosition+0x26 : and eax, 1
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        // CDirectSoundVoice_SetPosition+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B },
        { 0x32, 0x48 },
        { 0x33, 0x14 },

        // CDirectSoundVoice_SetPosition+0x4F : retn 0x14
        { 0x4F, 0xC2 },
        { 0x50, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPosition, 3936, 9,

    XREF_DSSTREAMSETPOSITION1A,
    XRefOne)

        // CDirectSoundStream_SetPosition+0x25 : call [CDirectSoundVoice_SetPosition]
        XREF_ENTRY( 0x25, XREF_DSSTREAMSETPOSITION1B ),

        // CDirectSoundStream_SetPosition+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundStream_SetPosition+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x0C },

        // CDirectSoundStream_SetPosition+0x2A : retn 0x14
        { 0x2A, 0xC2 },
        { 0x2B, 0x14 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetPosition, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_SetPosition+0x21 : call [CDirectSoundStream_SetPosition]
        XREF_ENTRY( 0x21, XREF_DSSTREAMSETPOSITION1A ),

        // IDirectSoundStream_SetPosition+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // IDirectSoundStream_SetPosition+0x1A : fstp [esp]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // IDirectSoundStream_SetPosition+0x26 : retn 0x14
        { 0x26, 0xC2 },
        { 0x27, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 3936, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        // CDirectSoundBuffer_SetPosition+0x20 : call [CDirectSoundVoice_SetPosition]
        XREF_ENTRY( 0x21, XREF_DSSTREAMSETPOSITION1B ),

        // CDirectSoundBuffer_SetPosition+0x06 : fld [ebp+arg_C]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // CDirectSoundBuffer_SetPosition+0x1A : fstp [esp+0x10+var_10]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // CDirectSoundBuffer_SetPosition+0x26 : retn 0x14
        { 0x27, 0x14 },
        { 0x28, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPosition, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetPosition+0x2C : call [CDirectSoundBuffer::SetPosition]
        XREF_ENTRY( 0x2D, XREF_DSBUFFERSETPOSITION ),

        // IDirectSoundBuffer_SetPosition+0x09 : mov eax, [ebp+arg_0]
        { 0x09, 0x8B },
        { 0x0A, 0x45 },
        { 0x0B, 0x08 },

        // IDirectSoundBuffer_SetPosition+0x28 : fstp [esp+0x10+var_10]
        { 0x28, 0xD9 },
        { 0x29, 0x1C },
        { 0x2A, 0x24 },

        // IDirectSoundBuffer_SetPosition+0x32 : retn 0x14
        { 0x33, 0x14 },
        { 0x34, 0x00 },
OOVPA_END;

// ******************************************************************
// * XAudioCalculatePitch
// ******************************************************************
OOVPA_XREF(XAudioCalculatePitch, 3936, 11,

    XREF_DSSTREAMSETFREQUENCY1B,
    XRefZero)

        // XAudioCalculatePitch+0x08 : cmp esi, 0xBB80
        { 0x08, 0x81 },
        { 0x09, 0xFE },
        { 0x0A, 0x80 },
        { 0x0B, 0xBB },

        // XAudioCalculatePitch+0x1D : jmp +0x2B
        { 0x1D, 0xEB },
        { 0x1E, 0x2B },

        // XAudioCalculatePitch+0x1F : lea ecx, [ebp+0x08]
        { 0x1F, 0x8D },
        { 0x20, 0x4D },
        { 0x21, 0x08 },

        // XAudioCalculatePitch+0x4E : retn 0x08
        { 0x4E, 0xC2 },
        { 0x4F, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFrequency, 3936, 9,

    XREF_DSSTREAMSETFREQUENCY1A,
    XRefOne)

        // CDirectSoundVoice_SetFrequency+0x1F : call [XAudioCalculatePitch]
        XREF_ENTRY( 0x20, XREF_DSSTREAMSETFREQUENCY1B ),

        // CDirectSoundVoice_SetFrequency+0x15 : mov eax, [esi+0x18]
        { 0x15, 0x8B },
        { 0x16, 0x46 },
        { 0x17, 0x18 },

        // CDirectSoundVoice_SetFrequency+0x18 : mov eax, [eax+0x10]
        { 0x18, 0x8B },
        { 0x19, 0x40 },
        { 0x1A, 0x10 },

        // CDirectSoundVoice_SetFrequency+0x40 : retn 0x08
        { 0x40, 0xC2 },
        { 0x41, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequency, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetFrequency+0x0D : call [CDirectSoundVoice_SetFrequency]
        XREF_ENTRY( 0x0D, XREF_DSSTREAMSETFREQUENCY1A ),

        // CDirectSoundStream_SetFrequency+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetFrequency+0x08 : add eax, 0x04
        { 0x08, 0x83 },
        { 0x09, 0xC0 },
        { 0x0A, 0x04 },

        // CDirectSoundStream_SetFrequency+0x11 : retn 0x08
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFrequency
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x15 : call [CDirectSoundVoice_SetFrequency]
        XREF_ENTRY( 0x15, XREF_DSSTREAMSETFREQUENCY1A ), 

        // IDirectSoundBuffer_SetFrequency+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetFrequency+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B },
        { 0x10, 0xC9 },

        // IDirectSoundBuffer_SetFrequency+0x11 : and ecx, eax
        { 0x11, 0x23 },
        { 0x12, 0xC8 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::Set3dMode
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dMode, 3936, 12,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        // CMcpxVoiceClient_Set3dMode+0x04 : or byte ptr [ecx+0x80], 0xFF
        { 0x04, 0x80 },
        { 0x05, 0x89 },
        { 0x06, 0x80 },
        { 0x07, 0x00 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0xFF },

        // CMcpxVoiceClient_Set3dMode+0x10 : cmp [ecx+0x58], eax
        { 0x10, 0x89 },
        { 0x11, 0x41 },
        { 0x12, 0x58 },

        // CMcpxVoiceClient_Set3dMode+0x1E : retn 0x08
        { 0x1E, 0xC2 },
        { 0x1F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 3936, 10,

    XREF_DSBUFFERSETMODEA,
    XRefOne)

        // CDirectSoundVoice_SetMode+0x1E : call [CMcpxVoiceClient::Set3dMode]
        XREF_ENTRY( 0x1F, XREF_DSBUFFERSETMODEB ),

        // CDirectSoundVoice_SetMode+0x17 : push [esp+0x14]
        { 0x17, 0xFF },
        { 0x18, 0x74 },
        { 0x19, 0x24 },
        { 0x1A, 0x14 },

        // CDirectSoundVoice_SetMode+0x1B : mov ecx, [eax+0x14]
        { 0x1B, 0x8B },
        { 0x1C, 0x48 },
        { 0x1D, 0x14 },

        // CDirectSoundVoice_SetMode+0x38 : retn 0x0C
        { 0x38, 0xC2 },
        { 0x39, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMode
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMode, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetMode+0x18 : call [CDirectSoundVoice::SetMode]
        XREF_ENTRY( 0x19, XREF_DSBUFFERSETMODEA ),

        // IDirectSoundBuffer_SetMode+0x04 : push [esp+0x0C]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSoundBuffer_SetMode+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xE4 },

        // IDirectSoundBuffer_SetMode+0x1D : retn 0x0C
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;
#if 0 // No longer used, not available by 3936
// ******************************************************************
// * CDirectSoundStream_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetRolloffFactor, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetRolloffFactor+0x11 : call [IDirectSoundStream::SetRolloffFactor]
        XREF_ENTRY( 0x11, XREF_DSSTREAMSETROLLOFFFACTOR1A ),

        // CDirectSoundStream_SetRolloffFactor+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetRolloffFactor+0x0C : add eax, 0x04
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x04 },

        // CDirectSoundStream_SetRolloffFactor+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by CDirectSoundVoice_SetHeadroom 3936 version
// ******************************************************************
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 3936, 10,

    XREF_DSSETHEADROOMA,
    XRefZero)

        { 0x08, 0x54 },
        { 0x0C, 0x74 },
        { 0x10, 0xF8 },
        { 0x16, 0x1C },
        { 0x17, 0x2B },
        { 0x18, 0xCE },
        { 0x19, 0x01 },
        { 0x1A, 0x48 },
        { 0x3C, 0xC2 },
        { 0x3D, 0x08 },
OOVPA_END;
#endif

// ******************************************************************
// * CDirectSoundStream_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetHeadroom, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetHeadroom+0x0D : call [DirectSound_CDirectSoundVoice_SetHeadroom]
        XREF_ENTRY( 0x0D, XREF_DSBUFFERSETHEADROOMA ),

        // CDirectSoundStream_SetHeadroom+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // CDirectSoundStream_SetHeadroom+0x08 : add eax, 0x04
        { 0x08, 0x83 },
        { 0x09, 0xC0 },
        { 0x0A, 0x04 },

        // CDirectSoundStream_SetHeadroom+0x11 : retn 0x08
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetOrientation
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_SetOrientation, 3936, 13)

        // IDirectSound_SetOrientation+0x06 : fld [ebp+0x20]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x20 },

        // IDirectSound_SetOrientation+0x0C : sub esp, 0x18
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x18 },

        // IDirectSound_SetOrientation+0x18 : add eax, 0xFFFFFFF8
        { 0x18, 0x83 },
        { 0x19, 0xC0 },
        { 0x1A, 0xF8 },

        // IDirectSound_SetOrientation+0x1F : neg ecx
        { 0x1F, 0xF7 },
        { 0x20, 0xD9 },

        // IDirectSound_SetOrientation+0x47 : retn 0x20
        { 0x47, 0xC2 },
        { 0x48, 0x20 },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_Commit3dSettings
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Commit3dSettings, 3936, 11,

    XREF_COMMITDEFERREDSETTINGSB,
    XRefZero)

        // CMcpxAPU_Commit3dSettings+0x56 : fld dword ptr [eax+0x174]
        { 0x56, 0xD9 },
        { 0x57, 0x80 },
        { 0x58, 0x74 },
        { 0x59, 0x01 },
        { 0x5A, 0x00 },
        { 0x5B, 0x00 },

        // CMcpxAPU_Commit3dSettings+0xA8 : fsubp st(1), st
        { 0xA8, 0xDE },
        { 0xA9, 0xE9 },

        // CMcpxAPU_Commit3dSettings+0xD4 : mov esi, 0x118
        { 0xD4, 0xBE },
        { 0xD5, 0x18 },
        { 0xD6, 0x01 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 3936, 10,

    XREF_COMMITDEFERREDSETTINGSA,
    XRefOne)

        // CDirectSound_CommitDeferredSettings+0x12 : call [CMcpxAPU_Commit3dSettings]
        XREF_ENTRY( 0x12, XREF_COMMITDEFERREDSETTINGSB ),

        // CDirectSound_CommitDeferredSettings+0x0C : mov ecx, [eax+0x0C]
        { 0x0C, 0x8B },
        { 0x0D, 0x48 },
        { 0x0E, 0x0C },

        // CDirectSound_CommitDeferredSettings+0x0F : push 0
        { 0x0F, 0x6A },
        { 0x10, 0x00 },

        // CDirectSound_CommitDeferredSettings+0x19 : jz +0x0B
        { 0x19, 0x74 },
        { 0x1A, 0x0B },

        // CDirectSound_CommitDeferredSettings+0x28 : retn 0x04
        { 0x28, 0xC2 },
        { 0x29, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(IDirectSound_CommitDeferredSettings, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CommitDeferredSettings+0x11 : call [CDirectSound_CommitDeferredSettings]
        XREF_ENTRY( 0x11, XREF_COMMITDEFERREDSETTINGSA ), 

        // IDirectSound_CommitDeferredSettings+0x06 : and eax, 0xFFFFFFF8
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0xF8 },

        // IDirectSound_CommitDeferredSettings+0x09 : neg ecx
        { 0x09, 0xF7 },
        { 0x0A, 0xD9 },

        // IDirectSound_CommitDeferredSettings+0x15 : retn 0x04
        { 0x15, 0xC2 },
        { 0x16, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_Release, 3936, 10)

        // IDirectSound_Release+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D },
        { 0x05, 0x48 },
        { 0x06, 0xF8 },

        // IDirectSound_Release+0x07 : neg eax
        { 0x07, 0xF7 },
        { 0x08, 0xD8 },

        // IDirectSound_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF },
        { 0x11, 0x51 },
        { 0x12, 0x08 },

        // IDirectSound_Release+0x13 : retn 0x04
        { 0x13, 0xC2 },
        { 0x14, 0x04 },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_Set3dDistanceFactor
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dDistanceFactor, 3936, 13,

    XREF_SETDISTANCEFACTORB,
    XRefZero)

        // CMcpxAPU_Set3dDistanceFactor+0x04 : or dword ptr [ecx+0x01B4], 0x60
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0xB4 },
        { 0x07, 0x01 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x60 },

        // CMcpxAPU_Set3dDistanceFactor+0x0B : cmp [esp+0x08], 0
        { 0x0B, 0x83 },
        { 0x0C, 0x7C },
        { 0x0D, 0x24 },
        { 0x0E, 0x08 },

        // CMcpxAPU_Set3dDistanceFactor+0x12 : mov [ecx+0x0178], eax
        { 0x12, 0x78 },
        { 0x13, 0x01 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactor, 3936, 12,

    XREF_SETDISTANCEFACTORA,
    XRefOne)

        // CDirectSound_SetDistanceFactor+0x23 : call [CMcpxAPU_Set3dDistanceFactor]
        XREF_ENTRY( 0x23, XREF_SETDISTANCEFACTORB ),

        // CDirectSound_SetDistanceFactor+0x00 : push esi; push edi
        { 0x00, 0x56 },
        { 0x01, 0x57 },

        // CDirectSound_SetDistanceFactor+0x07 : fld [esp+0x10]
        { 0x07, 0xD9 },
        { 0x08, 0x44 },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },

        // CDirectSound_SetDistanceFactor+0x11 : not eax; and eax, 1
        { 0x11, 0xF7 },
        { 0x12, 0xD0 },
        { 0x13, 0x83 },
        { 0x14, 0xE0 },
        { 0x15, 0x01 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDistanceFactor, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative (may not be SetDistanceFactor)

        // IDirectSound_SetDistanceFactor+0x1D : call [CDirectSound_SetDistanceFactor]
        XREF_ENTRY( 0x1D, XREF_SETDISTANCEFACTORA ), 

        // IDirectSound_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 },

        // IDirectSound_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_Set3dRolloffFactor
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dRolloffFactor, 3936, 13,

    XREF_SETROLLOFFFACTORB,
    XRefZero)

        // CMcpxAPU_Set3dRolloffFactor+0x04 : or dword ptr [ecx+0x01B4], 0x60
        { 0x04, 0x83 },
        { 0x05, 0x89 },
        { 0x06, 0xB4 },
        { 0x07, 0x01 },
        { 0x08, 0x00 },
        { 0x09, 0x00 },
        { 0x0A, 0x04 },

        // CMcpxAPU_Set3dRolloffFactor+0x0B : cmp [esp+0x08], 0
        { 0x0B, 0x83 },
        { 0x0C, 0x7C },
        { 0x0D, 0x24 },
        { 0x0E, 0x08 },

        // CMcpxAPU_Set3dRolloffFactor+0x12 : mov [ecx+0x017C], eax
        { 0x12, 0x7C },
        { 0x13, 0x01 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 3936, 12,

    XREF_SETROLLOFFFACTORA,
    XRefOne)

        // CDirectSound_SetRolloffFactor+0x23 : call [CMcpxAPU_Set3dRolloffFactor]
        XREF_ENTRY( 0x23, XREF_SETROLLOFFFACTORB ),

        // CDirectSound_SetRolloffFactor+0x00 : push esi; push edi
        { 0x00, 0x56 },
        { 0x01, 0x57 },

        // CDirectSound_SetRolloffFactor+0x07 : fld [esp+0x10]
        { 0x07, 0xD9 },
        { 0x08, 0x44 },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },

        // CDirectSound_SetRolloffFactor+0x11 : not eax; and eax, 1
        { 0x11, 0xF7 },
        { 0x12, 0xD0 },
        { 0x13, 0x83 },
        { 0x14, 0xE0 },
        { 0x15, 0x01 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetRolloffFactor, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative (may not be SetRolloffFactor)

        // IDirectSound_SetRolloffFactor+0x1D : call [CDirectSound_SetRolloffFactor]
        XREF_ENTRY( 0x1D, XREF_SETROLLOFFFACTORA ), 

        // IDirectSound_SetRolloffFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetRolloffFactor+0x0C : push ecx
        { 0x0C, 0x51 },

        // IDirectSound_SetRolloffFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_SetRolloffFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 },
        { 0x22, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(CMcpxAPU_SetMixBinHeadroom, 3936, 12,

    XREF_DSSETMIXBINHEADROOMB,
    XRefZero)

        // CMcpxAPU_SetMixBinHeadroom+0x04 : cmp dword ptr ds:[0xFE820010], 4
        { 0x12, 0x83 },
        { 0x13, 0x3D },
        { 0x14, 0x10 },
        { 0x15, 0x00 },
        { 0x16, 0x82 },
        { 0x17, 0xFE },
        { 0x18, 0x04 },

        // CMcpxAPU_SetMixBinHeadroom+0x1F : and edx, 7
        { 0x1F, 0x83 },
        { 0x20, 0xE2 },
        { 0x21, 0x07 },

        // CMcpxAPU_SetMixBinHeadroom+0x2D : jl +0xD8
        { 0x2D, 0x7C },
        { 0x2E, 0xD8 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSound_SetMixBinHeadroom, 3936, 9,

    XREF_DSSETMIXBINHEADROOMA,
    XRefOne)

        // CDirectSound_SetMixBinHeadroom+0x19 : call [CMcpxAPU_SetMixBinHeadroom]
        XREF_ENTRY( 0x19, XREF_DSSETMIXBINHEADROOMB ),

        // CDirectSound_SetMixBinHeadroom+0x00 : push esi; push edi
        { 0x00, 0x56 },
        { 0x01, 0x57 },

        // CDirectSound_SetMixBinHeadroom+0x07 : push [esp+0x14]
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x09, 0x24 },
        { 0x0A, 0x14 },

        // CDirectSound_SetMixBinHeadroom+0x21 : jz +0x0B
        { 0x21, 0x74 },
        { 0x22, 0x0B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound_SetMixBinHeadroom, 3936, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetMixBinHeadroom+0x19 : call [CDirectSound_SetMixBinHeadroom]
        XREF_ENTRY( 0x19, XREF_DSSETMIXBINHEADROOMA ), 

        // IDirectSound_SetMixBinHeadroom+0x04 : push [esp+0x0C]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x0C },

        // IDirectSound_SetMixBinHeadroom+0x08 : mov ecx, eax
        { 0x08, 0x8B },
        { 0x09, 0xC8 },

        // IDirectSound_SetMixBinHeadroom+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 },
        { 0x0F, 0xC0 },
        { 0x10, 0xF8 },

        // IDirectSound_SetMixBinHeadroom+0x1D : retn 0x0C
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_Set3dPosition
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dPosition, 3936, 12,

    XREF_DSSETPOSITIONB,
    XRefZero)

        // CMcpxAPU_Set3dPosition+0x0C : movsd; movsd; movsd
        { 0x0C, 0xA5 },
        { 0x0D, 0xA5 },
        { 0x0E, 0xA5 },

        // CMcpxAPU_Set3dPosition+0x0F : or byte ptr [ecx+0x01B4], 0xFF
        { 0x0F, 0x80 },
        { 0x10, 0x89 },
        { 0x11, 0xB4 },
        { 0x12, 0x01 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0xFF },

        // CMcpxAPU_Set3dPosition+0x28 : retn 0x08
        { 0x28, 0xC2 },
        { 0x29, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPosition, 3936, 10,

    XREF_DSSETPOSITIONA,
    XRefOne)

        // CDirectSound_SetPosition+0x35 : call [CMcpxAPU_Set3dPosition]
        XREF_ENTRY( 0x35, XREF_DSSETPOSITIONB ),

        // CDirectSound_SetPosition+0x06 : push esi; push edi
        { 0x06, 0x56 },
        { 0x07, 0x57 },

        // CDirectSound_SetPosition+0x24 : not eax; and eax, 1
        { 0x24, 0xF7 },
        { 0x25, 0xD0 },
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        // CDirectSound_SetPosition+0x4F : retn 0x14
        { 0x4F, 0xC2 },
        { 0x50, 0x14 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSound_SetPosition, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetPosition+0x2D : call [CDirectSound_SetPosition]
        XREF_ENTRY( 0x2D, XREF_DSSETPOSITIONA ), 

        // IDirectSound_SetPosition+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // IDirectSound_SetPosition+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x0C },

        // IDirectSound_SetPosition+0x1B : neg ecx
        { 0x1B, 0xF7 },
        { 0x1C, 0xD9 },

        // IDirectSound_SetPosition+0x32 : retn 0x14
        { 0x32, 0xC2 },
        { 0x33, 0x14 },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU_Set3dVelocity
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dVelocity, 3936, 12,

    XREF_DSSETVELOCITYB,
    XRefZero)

        // CMcpxAPU_Set3dVelocity+0x0C : movsd; movsd; movsd
        { 0x0C, 0xA5 },
        { 0x0D, 0xA5 },
        { 0x0E, 0xA5 },

        // CMcpxAPU_Set3dVelocity+0x0F : or dword ptr [ecx+0x01B4], 0x40
        { 0x0F, 0x83 },
        { 0x10, 0x89 },
        { 0x11, 0xB4 },
        { 0x12, 0x01 },
        { 0x13, 0x00 },
        { 0x14, 0x00 },
        { 0x15, 0x40 },

        // CMcpxAPU_Set3dVelocity+0x28 : retn 0x08
        { 0x28, 0xC2 },
        { 0x29, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocity, 3936, 10,

    XREF_DSSETVELOCITYA,
    XRefOne)

        // CDirectSound_SetVelocity+0x35 : call [CMcpxAPU_Set3dVelocity]
        XREF_ENTRY( 0x35, XREF_DSSETVELOCITYB ),

        // CDirectSound_SetVelocity+0x06 : push esi; push edi
        { 0x06, 0x56 },
        { 0x07, 0x57 },

        // CDirectSound_SetVelocity+0x24 : not eax; and eax, 1
        { 0x24, 0xF7 },
        { 0x25, 0xD0 },
        { 0x26, 0x83 },
        { 0x27, 0xE0 },
        { 0x28, 0x01 },

        // CDirectSound_SetVelocity+0x4F : retn 0x14
        { 0x4F, 0xC2 },
        { 0x50, 0x14 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSound_SetVelocity, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_SetVelocity+0x2D : call [CDirectSound_SetVelocity]
        XREF_ENTRY( 0x2D, XREF_DSSETVELOCITYA ), 

        // IDirectSound_SetVelocity+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 },
        { 0x07, 0x45 },
        { 0x08, 0x14 },

        // IDirectSound_SetVelocity+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 },
        { 0x0D, 0xEC },
        { 0x0E, 0x0C },

        // IDirectSound_SetVelocity+0x1B : neg ecx
        { 0x1B, 0xF7 },
        { 0x1C, 0xD9 },

        // IDirectSound_SetVelocity+0x32 : retn 0x14
        { 0x32, 0xC2 },
        { 0x33, 0x14 },
OOVPA_END;

#if 0 // Was ?SetI3DL2Listener@CMcpxAPU@@QAEJABU_DSI3DL2LISTENER@@H@Z.
// ******************************************************************
// * CDirectSound::SetAllParametersB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersB, 3936, 13,

    XREF_DSSETALLPARAMETERSB,
    XRefZero)

        // CDirectSound_SetAllParametersB+0x08 : push 0x0C; pop ecx
        { 0x08, 0x6A },
        { 0x09, 0x0C },
        { 0x0A, 0x59 },

        // CDirectSound_SetAllParametersB+0x13 : or word ptr [ecx+0x01B4], 0x0180
        { 0x13, 0x66 },
        { 0x14, 0x81 },
        { 0x15, 0x88 },
        { 0x16, 0xB4 },
        { 0x17, 0x01 },
        { 0x18, 0x00 },
        { 0x1A, 0x80 },
        { 0x1B, 0x01 },

        // CDirectSound_SetAllParametersB+0x30 : retn 0x08
        { 0x30, 0xC2 },
        { 0x31, 0x08 },
OOVPA_END;
#endif
#if 0 // Was ?SetI3DL2Listener@CDirectSound@@QAGJPBU_DSI3DL2LISTENER@@K@Z. replaced by CDirectSound_SetI3DL2Listener.
// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 3936, 10,

    XREF_DSSETALLPARAMETERSA,
    XRefOne)

        // CDirectSound_SetAllParametersA+0x2F : call [CDirectSound::SetAllParametersB]
        XREF_ENTRY( 0x2F, XREF_DSSETALLPARAMETERSB ),

        // CDirectSound_SetAllParametersA+0x00 : push esi; push edi
        { 0x00, 0x56 },
        { 0x01, 0x57 },

        // CDirectSound_SetAllParametersA+0x21 : not ecx; and ecx, 1
        { 0x21, 0xF7 },
        { 0x22, 0xD1 },
        { 0x23, 0x83 },
        { 0x24, 0xE1 },
        { 0x25, 0x01 },

        // CDirectSound_SetAllParametersA+0x48 : retn 0x0C
        { 0x48, 0xC2 },
        { 0x49, 0x0C },
OOVPA_END;
#endif

// ******************************************************************
// * IDirectSound_DownloadEffectsImage
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_DownloadEffectsImage, 3936, 11)

        // IDirectSound_DownloadEffectsImage+0x03 : push [ebp+0x18]
        { 0x03, 0xFF },
        { 0x04, 0x75 },
        { 0x05, 0x18 },

        // IDirectSound_DownloadEffectsImage+0x06 : mov eax, [ebp+0x08]
        { 0x06, 0x8B },
        { 0x07, 0x45 },
        { 0x08, 0x08 },

        // IDirectSound_DownloadEffectsImage+0x11 : add eax, 0xFFFFFFF8
        { 0x11, 0x83 },
        { 0x12, 0xC0 },
        { 0x13, 0xF8 },

        // IDirectSound_DownloadEffectsImage+0x24 : retn 0x14
        { 0x24, 0xC2 },
        { 0x25, 0x14 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMode, 3936, 7,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMode+0x10 : call [CDirectSoundVoice::SetMode]
        XREF_ENTRY( 0x11, XREF_DSBUFFERSETMODEA ),

        // CDirectSoundStream_SetMode+0x0C : add eax, 4
        { 0x0C, 0x83 },
        { 0x0D, 0xC0 },
        { 0x0E, 0x04 },

        // CDirectSoundStream_SetMode+0x0F : push eax
        { 0x0F, 0x50 },

        // CDirectSoundBuffer8_SetMode+0x15 : retn 0x0C
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;
#if 0 //Moved to 3911
// ******************************************************************
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 3925, 9,

    XREF_CMcpxBuffer_Play,
    XRefZero)

        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x18, 0x75 },
        { 0x19, 0x08 },
        { 0x1A, 0x6A },
        { 0x1B, 0x02 },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
OOVPA_END;
#endif
// ******************************************************************
// * CMcpxAPU::Set3dDopplerFactor
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dDopplerFactor, 3936, 7,

    XREF_CMcpxAPU_Set3dDopplerFactor,
    XRefZero)

        { 0x03, 0x04 },
        { 0x08, 0x00 },
        { 0x0D, 0x24 },
        { 0x12, 0x80 },
        { 0x17, 0x07 },
        { 0x1F, 0x33 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 3936, 9,

    XREF_SETDOPPLERFACTOR,
    XRefOne)

        XREF_ENTRY( 0x23, XREF_CMcpxAPU_Set3dDopplerFactor ),

        { 0x07, 0xD9 },
        { 0x0E, 0x44 },
        { 0x16, 0x50 },
        { 0x1E, 0x0C },
        { 0x27, 0x85 },
        { 0x32, 0xFF },
        { 0x38, 0x8B },
        { 0x3E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetDopplerFactor
// ******************************************************************
OOVPA_XREF(IDirectSound_SetDopplerFactor, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_SETDOPPLERFACTOR ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CSensaura3d::GetFullHRTFFilterPair
// ******************************************************************
OOVPA_XREF(CSensaura3d_GetFullHRTFFilterPair, 3925, 7,

    XREF_CSensaura3d_GetFullHRTFFilterPair,
    XRefZero)

        { 0x1E, 0xEB },
        { 0x2E, 0x6A },
        { 0x3E, 0x40 },
        { 0x5E, 0x7E },
        { 0x7E, 0x1E },
        { 0xA2, 0xD9 },
        { 0xBE, 0x09 },
OOVPA_END;

// ******************************************************************
// * CSensaura3d::GetFullHRTFFilterPair
// ******************************************************************
OOVPA_XREF(CSensaura3d_GetFullHRTFFilterPair, 3936, 9,

    XREF_CSensaura3d_GetFullHRTFFilterPair,
    XRefZero)

        { 0x05, 0x0C },
        { 0x0E, 0xD9 },
        { 0x20, 0xEB },
        { 0x28, 0xD9 },
        { 0x30, 0x6A },
        { 0xAA, 0xEB },
        { 0xAB, 0x17 },
        { 0xAC, 0xD8 },
        { 0xAD, 0x05 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 3925, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_CSensaura3d_GetFullHRTFFilterPair ),

        { 0x05, 0x85 },
        { 0x06, 0xC0 },
        { 0x11, 0x74 },
        { 0x12, 0x0B },
        { 0x13, 0x68 },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::GetCaps
// ******************************************************************
OOVPA_XREF(CDirectSound_GetCaps, 3936, 8,

    XREF_CDirectSound_GetCaps,
    XRefZero)

        { 0x08, 0x8B },
        { 0x12, 0x8D },
        { 0x1C, 0x0C },
        { 0x26, 0xFF },
        { 0x34, 0x03 },
        { 0x3A, 0xDB },
        { 0x48, 0x8B },
        { 0x4E, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_GetCaps
// ******************************************************************
OOVPA_XREF(IDirectSound_GetCaps, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSound_GetCaps ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#if 0 // No longer used, replaced by CDirectSoundVoice_SetPitch 3936 version
// ******************************************************************
// * CDirectSoundVoice::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetPitch, 3936, 8,

    XREF_CDirectSoundVoice_SetPitch,
    XRefZero)

        { 0x07, 0x8B },
        { 0x0C, 0xF0 },
        { 0x13, 0x18 },
        { 0x1A, 0xE8 },
        { 0x21, 0x8B },
        { 0x2A, 0xFF },
        { 0x30, 0x8B },
        { 0x36, 0x00 },
OOVPA_END;
#endif
#if 0 // No longer used, replaced by another 3936 version
// ******************************************************************
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetPitch ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif
// ******************************************************************
// * CDirectSoundVoice::SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetConeAngles, 3936, 8,

    XREF_DSVOICESETCONEANGLES,
    XRefZero)

        { 0x07, 0x8B },
        { 0x0E, 0xD0 },
        { 0x16, 0x18 },
        { 0x1E, 0x18 },
        { 0x27, 0x85 },
        { 0x32, 0xFF },
        { 0x38, 0x8B },
        { 0x3E, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeAngles
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeAngles, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSVOICESETCONEANGLES ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSoundEnterCriticalSection
// ******************************************************************
OOVPA_XREF(DirectSoundEnterCriticalSection, 3936, 7,

    XREF_DirectSoundEnterCriticalSection,
    XRefZero)

        { 0x02, 0xB6 },
        { 0x06, 0x00 },
        { 0x0A, 0x74 },
        { 0x0E, 0xC3 },
        { 0x14, 0xFF },
        { 0x1A, 0x33 },
        { 0x1B, 0xC0 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 3936, 9,

    XREF_CDirectSoundBuffer_PlayEx,
    XRefOne)

        XREF_ENTRY( 0x03, XREF_DirectSoundEnterCriticalSection ),

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x0E, 0xE0 },
        { 0x16, 0x44 },
        { 0x1E, 0x48 },
        { 0x26, 0xF6 },
        { 0x30, 0xFF },
        { 0x36, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_PlayEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_PlayEx, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_PlayEx ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirectSound_AddRef, 3936, 12)

        // IDirectSound_AddRef+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D },
        { 0x05, 0x48 },
        { 0x06, 0xF8 },

        // IDirectSound_AddRef+0x07 : neg eax
        { 0x07, 0xF7 },
        { 0x08, 0xD8 },

        // IDirectSound_AddRef+0x09 : sbb eax, eax
        { 0x09, 0x1B },
        { 0x0A, 0xC0 },

        // IDirectSound_AddRef+0x0D : mov ecx, [eax]
        { 0x0D, 0x8B },
        { 0x0E, 0x08 },

        // IDirectSound_AddRef+0x10 : call dword ptr [ecx+4]
        { 0x10, 0xFF },
        { 0x11, 0x51 },
        { 0x12, 0x04 },
OOVPA_END;

// ******************************************************************
// * DirectSoundGetSampleTime
// ******************************************************************
OOVPA_NO_XREF(DirectSoundGetSampleTime, 3936, 6)

        { 0x00, 0xA1 },
        { 0x01, 0x0C },
        { 0x02, 0x20 },
        { 0x03, 0x80 },
        { 0x04, 0xFE },
        { 0x05, 0xC3 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_AddRef, 3936, 10)

        // IDirectSoundBuffer_AddRef+0x04 : lea ecx, [eax-0x1C]
        { 0x04, 0x8D },
        { 0x05, 0x48 },
        { 0x06, 0xE4 },

        // IDirectSoundBuffer_AddRef+0x07 : neg eax; sbb eax, eax
        { 0x07, 0xF7 },
        { 0x08, 0xD8 },
        { 0x09, 0x1B },
        { 0x0A, 0xC0 },

        // IDirectSoundBuffer_AddRef+0x10 : call dword ptr [ecx+0x04]
        { 0x10, 0xFF },
        { 0x11, 0x51 },
        { 0x12, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 3936, 9,

    XREF_CDirectSoundBuffer_SetMinDistance,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_DSSTREAMSETMINDISTANCE1B ),

        { 0x01, 0x74 },
        { 0x04, 0xD9 },
        { 0x07, 0x0C },
        { 0x0A, 0x1C },
        { 0x0D, 0x74 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetMinDistance ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 3936, 9,

    XREF_CDirectSoundBuffer_SetMaxDistance,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_DSSTREAMSETMAXDISTANCE1B ),

        { 0x01, 0x74 },
        { 0x04, 0xD9 },
        { 0x07, 0x0C },
        { 0x0A, 0x1C },
        { 0x0D, 0x74 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_SetMaxDistance ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::Commit3dSettings
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Commit3dSettings, 3936, 10,

    XREF_CMcpxVoiceClient_Commit3dSettings,
    XRefZero)

	// CMcpxVoiceClient::Commit3dSettings+0x17 : or [eax+0x80], ecx
	{ 0x17, 0x09 },
	{ 0x18, 0x88 },
	{ 0x19, 0x80 },
	{ 0x1A, 0x00 },
	// CMcpxVoiceClient::Commit3dSettings+0x2B : add eax, 0x80
	{ 0x2B, 0x05 },
	{ 0x2C, 0x80 },
	{ 0x2D, 0x00 },
	// CMcpxVoiceClient::Commit3dSettings+0x3D : cmpxchg [ecx], edx
	{ 0x3D, 0x0F },
	{ 0x3E, 0xB1 },
	{ 0x3F, 0x11 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetI3DL2Source, 3936, 7,

    XREF_CMcpxVoiceClient_SetI3DL2Source,
    XRefZero)

        { 0x04, 0x08 },
        { 0x0A, 0x59 },
        { 0x10, 0x80 },
        { 0x16, 0x80 },
        { 0x1C, 0x5F },
        { 0x22, 0x8B },
        { 0x29, 0x33 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetI3DL2Source, 3936, 9,

    XREF_CDirectSoundVoice_SetI3DL2Source,
    XRefOne)

        XREF_ENTRY( 0x1F, XREF_CMcpxVoiceClient_SetI3DL2Source ),

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x0E, 0xD0 },
        { 0x16, 0x10 },
        { 0x1E, 0xE8 },
        { 0x26, 0xF8 },
        { 0x2E, 0xFF },
        { 0x36, 0x5F },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetI3DL2Source, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundVoice_SetI3DL2Source ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 3936, 7,

    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XRefZero)

        { 0x0D, 0x65 },
        { 0x1C, 0x53 },
        { 0x2B, 0xC0 },
        { 0x3A, 0xC7 },
        { 0x49, 0xFC },
        { 0x58, 0x83 },
        { 0x67, 0x15 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CMcpxAPU::Set3dParameters
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Set3dParameters, 3936, 7,

    XREF_DSSETALLPARAMETERSB,
    XRefZero)

        { 0x05, 0x57 },
        { 0x0C, 0xB8 },
        { 0x13, 0x80 },
        { 0x1A, 0x83 },
        { 0x21, 0x74 },
        { 0x2C, 0x33 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParameters, 3936, 9,

    XREF_DSSETALLPARAMETERSA,
    XRefOne)

        XREF_ENTRY( 0x1F, XREF_DSSETALLPARAMETERSB ),

        { 0x00, 0x56 },
        { 0x07, 0x8B },
        { 0x0E, 0xD0 },
        { 0x16, 0x10 },
        { 0x1E, 0xE8 },
        { 0x26, 0xF8 },
        { 0x2E, 0xFF },
        { 0x36, 0x5F },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSound_SetAllParameters, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSSETALLPARAMETERSA ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 3936, 7,

    XREF_DSSETI3DL2LISTENER,
    XRefZero)

        { 0x08, 0xF8 },
        { 0x12, 0x0C },
        { 0x1C, 0x18 },
        { 0x26, 0x51 },
        { 0x33, 0x8B },
        { 0x3E, 0xFF },
        { 0x44, 0x5F },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(IDirectSound_SetI3DL2Listener, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_DSSETI3DL2LISTENER ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSound::EnableHeadphones
// ******************************************************************
OOVPA_XREF(CDirectSound_EnableHeadphones, 3936, 7,

    XREF_CDirectSound_EnableHeadphones,
    XRefZero)
        { 0x18, 0x8B },
        { 0x32, 0x0C },
        { 0x4F, 0x8B },
        { 0x66, 0x8B },
        { 0x80, 0xFF },
        { 0x9A, 0x76 },
        { 0xB4, 0x5B },
OOVPA_END;

// ******************************************************************
// * IDirectSound_EnableHeadphones
// ******************************************************************
OOVPA_XREF(IDirectSound_EnableHeadphones, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSound_EnableHeadphones ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes, 3936, 9,

    XREF_CDirectSoundStream_SetMixBinVolumes,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x01, 0x74 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0A, 0x24 },
        { 0x0D, 0xC0 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::SetI3DL2Source
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetI3DL2Source, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundVoice_SetI3DL2Source ),

	{ 0x01, 0x74 },
        { 0x04, 0x8B },
        { 0x07, 0x08 },
        { 0x0A, 0x24 },
        { 0x0D, 0xC0 },
        { 0x10, 0xE8 },
        { 0x15, 0xC2 },
        { 0x16, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetNotificationPositions
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetNotificationPositions, 3936, 7,

    XREF_CDirectSoundBuffer_SetNotificationPositions,
    XRefZero)

        { 0x09, 0x24 },
        { 0x14, 0x24 },
        { 0x1F, 0x74 },
        { 0x2A, 0x50 },
        { 0x35, 0xF6 },
        { 0x40, 0x07 },
        { 0x4D, 0x5F },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetNotificationPositions
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetNotificationPositions, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetNotificationPositions ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream::Pause
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_Pause, 3936, 6)

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x24 },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;

//TODO: Need another OOVPA reference since Halo has a match for CDirectSoundStream_SetConeOrientation signature.
// ******************************************************************
// * CDirectSoundBuffer::SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOrientation, 3936, 10,
    XREF_DSBUFFERSETCONEORIENTATION,
    XRefZero)

        // CDirectSoundBuffer_SetConeOrientation+0x1A : fstp [esp+8h]
        { 0x0C, 0xD9 },
        { 0x0D, 0x5C },
        { 0x0E, 0x24 },
        { 0x0F, 0x08 },

        // CDirectSoundBuffer_SetConeOrientation+0x1A : fstp [esp]
        { 0x1A, 0xD9 },
        { 0x1B, 0x1C },
        { 0x1C, 0x24 },

        // CDirectSoundBuffer_SetConeOrientation+0x26 : ret 14h
        { 0x26, 0xC2 },
        { 0x27, 0x14 },
        { 0x28, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeOrientation
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeOrientation, 3936, 9,

           XRefNoSaveIndex,
           XRefOne)

    // IDirectSoundBuffer_SetConeOrientation+0x2C : call [CDirectSoundBuffer::SetConeOrientation]
    XREF_ENTRY(0x2D, XREF_DSBUFFERSETCONEORIENTATION),

    // IDirectSoundBuffer_SetConeOrientation+0x06 : fld [ebp+arg_C]
    { 0x06, 0xD9 },
    { 0x07, 0x45 },
    { 0x08, 0x14 },

    // IDirectSoundBuffer_SetConeOrientation+0x28 : fstp [esp+10h+var_10]
    { 0x28, 0xD9 },
    { 0x29, 0x1C },
    { 0x2A, 0x24 },

    // IDirectSoundBuffer_SetConeOrientation+0x32 : retn 14h
    { 0x33, 0x14 },
    { 0x34, 0x00 }
OOVPA_END;

//TODO: Need another OOVPA reference since Halo has a match for CDirectSoundStream_SetConeOutsideVolume signature.
// ******************************************************************
// * CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOutsideVolume, 3936, 11,

           XREF_DSBUFFERSETCONEOUTSIDEVOLUME,
           XRefZero)

    // CDirectSoundBuffer_SetConeOutsideVolume+0x09 : mov eax,dword ptr [esp+14h]  
    { 0x09, 0x8B },
    { 0x0A, 0x44 },
    { 0x0B, 0x24 },
    { 0x0C, 0x14 },

    // CDirectSoundBuffer_SetConeOutsideVolume+0x17 : push [esp+0Ch+arg_0]
    { 0x17, 0xFF },
    { 0x18, 0x74 },
    { 0x19, 0x24 },
    { 0x1A, 0x14 },

    // CDirectSoundBuffer_SetConeOutsideVolume+0x38 : retn 0Ch
    { 0x38, 0xC2 },
    { 0x39, 0x0C },
    { 0x3A, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeOutsideVolume, 3936, 9,

           XRefNoSaveIndex,
           XRefOne)

    // IDirectSoundBuffer_SetConeOutsideVolume+0x18 : call [CDirectSoundBuffer::SetConeOutsideVolume]
    XREF_ENTRY(0x19, XREF_DSBUFFERSETCONEOUTSIDEVOLUME),

    // IDirectSoundBuffer_SetConeOutsideVolume+0x0A : push [esp+4+arg_4]
    { 0x0A, 0xFF },
    { 0x0B, 0x74 },
    { 0x0C, 0x24 },
    { 0x0D, 0x0C },

    // IDirectSoundBuffer_SetConeOutsideVolume+0x15 : and ecx, eax
    { 0x15, 0x23 },
    { 0x16, 0xC8 },

    // IDirectSoundBuffer_SetConeOutsideVolume+0x1D : retn 0Ch
    { 0x1E, 0x0C },
    { 0x1F, 0x00 }
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Unlock
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer_Unlock, 3936, 5)

    // IDirectSoundBuffer_Unlock+0x00 : xor eax, eax
    { 0x00, 0x33 },
    { 0x01, 0xC0 },

    // IDirectSoundBuffer_Unlock+0x02 : ret 14h
    { 0x02, 0xC2 },
    { 0x03, 0x14 },
    { 0x04, 0x00 }
OOVPA_END;

// ******************************************************************
// * DSound_3936
// ******************************************************************
OOVPATable DSound_3936[] = {

	REGISTER_OOVPA(DirectSoundCreate, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_ServiceDeferredCommandsLow, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_DoWork, 3936, XREF),
	REGISTER_OOVPA(DirectSoundDoWork, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetPitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_Lock, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetMixBins, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBins, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition, 3936, XREF),
	REGISTER_OOVPA(CMcpxBuffer_GetCurrentPosition2, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 3936, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 3911, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 3911, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 3911, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_Stop, 3911, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 3911, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetConeAngles, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dConeOutsideVolume, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOutsideVolume, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOutsideVolume, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dParameters, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dMaxDistance, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetMaxDistance, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetMaxDistance, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dMinDistance, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetMinDistance, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetMinDistance, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dVelocity, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVelocity, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVelocity, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dConeOrientation, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeOrientation, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetConeOrientation, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetConeOrientation, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dPosition, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetPosition, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundStream_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPosition, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(XAudioCalculatePitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFrequency, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Set3dMode, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetHeadroom, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Commit3dSettings, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_CommitDeferredSettings, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dDistanceFactor, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dRolloffFactor, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_SetMixBinHeadroom, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetMixBinHeadroom, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dPosition, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetPosition, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetPosition, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dVelocity, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dParameters, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetAllParameters, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMode, 3936, PATCH),
	REGISTER_OOVPA(CMcpxAPU_Set3dDopplerFactor, 3936, XREF),
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 3936, PATCH),
	REGISTER_OOVPA(CSensaura3d_GetFullHRTFFilterPair, 3925, XREF),
	REGISTER_OOVPA(CSensaura3d_GetFullHRTFFilterPair, 3936, XREF),
	REGISTER_OOVPA(DirectSoundUseFullHRTF, 3925, PATCH),
	REGISTER_OOVPA(CDirectSound_GetCaps, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_GetCaps, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundEnterCriticalSection, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(DirectSoundGetSampleTime, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_Commit3dSettings, 3936, XREF),
	REGISTER_OOVPA(CMcpxVoiceClient_SetI3DL2Source, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetI3DL2Source, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_EnableHeadphones, 3936, XREF),
	REGISTER_OOVPA(IDirectSound_EnableHeadphones, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetNotificationPositions, 3936, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 3936, PATCH),
    REGISTER_OOVPA(CDirectSoundBuffer_SetConeOrientation, 3936, XREF),
    REGISTER_OOVPA(IDirectSoundBuffer_SetConeOrientation, 3936, PATCH),
    REGISTER_OOVPA(CDirectSoundBuffer_SetConeOutsideVolume, 3936, XREF),
    REGISTER_OOVPA(IDirectSoundBuffer_SetConeOutsideVolume, 3936, PATCH),
    REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
    REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 3911, PATCH), // TODO: Add CDirectSoundBuffer_SetLoopRegion before 3935-ish
};

// ******************************************************************
// * DSound_3936_SIZE
// ******************************************************************
uint32 DSound_3936_SIZE = sizeof(DSound_3936);
