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
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
        { 0x23, 0x83 }, // (Offset,Value)-Pair #1
        { 0x24, 0xC0 }, // (Offset,Value)-Pair #2
        { 0x25, 0x08 }, // (Offset,Value)-Pair #3

        // DirectSoundCreate+0x34 : push 0x1C
        { 0x34, 0x6A }, // (Offset,Value)-Pair #4
        { 0x35, 0x1C }, // (Offset,Value)-Pair #5

        // DirectSoundCreate+0x75 : sbb eax, eax
        { 0x75, 0x1B }, // (Offset,Value)-Pair #6
        { 0x76, 0xC0 }, // (Offset,Value)-Pair #7

        // DirectSoundCreate+0x9B : retn 0x0C
        { 0x9B, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x9C, 0x0C }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWorkB
// ******************************************************************
OOVPA_XREF(DirectSoundDoWorkB, 3936, 12,

    XREF_DSDOWORKB,
    XRefZero)

        // DirectSoundDoWorkB+0x06 : lea edi, [ebx+0x664]
        { 0x06, 0x8D }, // (Offset,Value)-Pair #1
        { 0x07, 0xBB }, // (Offset,Value)-Pair #2
        { 0x08, 0x64 }, // (Offset,Value)-Pair #3
        { 0x09, 0x06 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #5
        { 0x0B, 0x00 }, // (Offset,Value)-Pair #6

        // DirectSoundDoWorkB+0x20 : mov eax, [ecx]
        { 0x20, 0x8B }, // (Offset,Value)-Pair #7
        { 0x21, 0x01 }, // (Offset,Value)-Pair #8

        // DirectSoundDoWorkB+0x33 : add edi, 4; dec ebp
        { 0x33, 0x83 }, // (Offset,Value)-Pair #9
        { 0x34, 0xC7 }, // (Offset,Value)-Pair #10
        { 0x35, 0x04 }, // (Offset,Value)-Pair #11
        { 0x36, 0x4D }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWorkA
// ******************************************************************
OOVPA_XREF(DirectSoundDoWorkA, 3936, 9,

    XREF_DSDOWORKA,
    XRefOne)

        // DirectSoundDoWorkA+0x10 : call [DirectSoundDoWorkB]
        { 0x10, XREF_DSDOWORKB }, // (Offset,Value)-Pair #1

        // DirectSoundDoWorkA+0x08 : mov eax, [esp+0x08]
        { 0x08, 0x8B }, // (Offset,Value)-Pair #2
        { 0x09, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0A, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x08 }, // (Offset,Value)-Pair #5

        // DirectSoundDoWorkA+0x17 : jz +0x0B
        { 0x17, 0x74 }, // (Offset,Value)-Pair #6
        { 0x18, 0x0B }, // (Offset,Value)-Pair #7

        // DirectSoundDoWorkA+0x24 : retn 0x04
        { 0x24, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x25, 0x04 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * DirectSoundDoWork
// ******************************************************************
OOVPA_XREF(DirectSoundDoWork, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundDoWork+0x13 : call [XREF_DSDOWORKA]
        { 0x13, XREF_DSDOWORKA }, // (Offset,Value)-Pair #1

        // DirectSoundDoWork+0x0D : test eax, eax;jz +0x06
        { 0x0D, 0x85 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #4
        { 0x10, 0x06 }, // (Offset,Value)-Pair #5

        // DirectSoundDoWork+0x19 : pop esi; jz +0x0B
        { 0x19, 0x5E }, // (Offset,Value)-Pair #6
        { 0x1A, 0x74 }, // (Offset,Value)-Pair #7
        { 0x1B, 0x0B }, // (Offset,Value)-Pair #8

        // DirectSoundDoWork+0x27 : retn
        { 0x27, 0xC3 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundStream, 3936, 16,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        // CDirectSound_CreateSoundStream+0x2D : and esi, 0x7FF8FFF2
        { 0x2D, 0x81 }, // (Offset,Value)-Pair #1
        { 0x2E, 0xE6 }, // (Offset,Value)-Pair #2
        { 0x2F, 0xF2 }, // (Offset,Value)-Pair #3
        { 0x30, 0xFF }, // (Offset,Value)-Pair #4
        { 0x31, 0xF8 }, // (Offset,Value)-Pair #5
        { 0x32, 0x7F }, // (Offset,Value)-Pair #6

        // CDirectSound_CreateSoundStream+0x33 : add esi, 0x8007000E
        { 0x33, 0x81 }, // (Offset,Value)-Pair #7
        { 0x34, 0xC6 }, // (Offset,Value)-Pair #8
        { 0x35, 0x0E }, // (Offset,Value)-Pair #9
        { 0x36, 0x00 }, // (Offset,Value)-Pair #10
        { 0x37, 0x07 }, // (Offset,Value)-Pair #11
        { 0x38, 0x80 }, // (Offset,Value)-Pair #12

        // CDirectSound_CreateSoundStream+0x3C : js +0x16
        { 0x3C, 0x78 }, // (Offset,Value)-Pair #13
        { 0x3D, 0x16 }, // (Offset,Value)-Pair #14

        // CDirectSound_CreateSoundStream+0x72 : retn 0x10
        { 0x72, 0xC2 }, // (Offset,Value)-Pair #15
        { 0x73, 0x10 }, // (Offset,Value)-Pair #16
OOVPA_END;

// ******************************************************************
// * IDirectSound8_CreateStream
// ******************************************************************
OOVPA_XREF(IDirectSound8_CreateStream, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: tidy up the xref names (are probably not accurate)

        // IDirectSound8_CreateStream+0x1D : call [CDirectSound::CreateSoundStream]
        { 0x1D, XREF_DSCREATESOUNDSTREAM }, // (Offset,Value)-Pair #1

        // IDirectSound8_CreateStream+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSound8_CreateStream+0x12 : and eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #6
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_CreateStream+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x22, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundBuffer, 3936, 16,

    XREF_DSCREATESOUNDBUFFER,
    XRefZero)

        // CDirectSound_CreateSoundBuffer+0x2D : and esi, 0x7FF8FFF2
        { 0x2D, 0x81 }, // (Offset,Value)-Pair #1
        { 0x2E, 0xE6 }, // (Offset,Value)-Pair #2
        { 0x2F, 0xF2 }, // (Offset,Value)-Pair #3
        { 0x30, 0xFF }, // (Offset,Value)-Pair #4
        { 0x31, 0xF8 }, // (Offset,Value)-Pair #5
        { 0x32, 0x7F }, // (Offset,Value)-Pair #6

        // CDirectSound_CreateSoundBuffer+0x33 : add esi, 0x8007000E
        { 0x33, 0x81 }, // (Offset,Value)-Pair #7
        { 0x34, 0xC6 }, // (Offset,Value)-Pair #8
        { 0x35, 0x0E }, // (Offset,Value)-Pair #9
        { 0x36, 0x00 }, // (Offset,Value)-Pair #10
        { 0x37, 0x07 }, // (Offset,Value)-Pair #11
        { 0x38, 0x80 }, // (Offset,Value)-Pair #12

        // CDirectSound_CreateSoundBuffer+0x3C : js +0x21
        { 0x3C, 0x78 }, // (Offset,Value)-Pair #13
        { 0x3D, 0x21 }, // (Offset,Value)-Pair #14

        // CDirectSound_CreateSoundBuffer+0x7D : retn 0x10
        { 0x7D, 0xC2 }, // (Offset,Value)-Pair #15
        { 0x7E, 0x10 }, // (Offset,Value)-Pair #16
OOVPA_END;

// ******************************************************************
// * IDirectSound8_CreateBuffer
// ******************************************************************
OOVPA_XREF(IDirectSound8_CreateBuffer, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: tidy up the xref names (are probably not accurate)

        // IDirectSound8_CreateBuffer+0x1D : call [CDirectSound::CreateSoundStream]
        { 0x1D, XREF_DSCREATESOUNDBUFFER }, // (Offset,Value)-Pair #1

        // IDirectSound8_CreateBuffer+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSound8_CreateBuffer+0x12 : and eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #6
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_CreateBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x22, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer8_Release, 3936, 10)

        // IDirectSoundBuffer8_Release+0x04 : lea ecx, [eax-0x1C]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xE4 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_Release+0x07 : neg eax; sbb eax, eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5
        { 0x09, 0x1B }, // (Offset,Value)-Pair #6
        { 0x0A, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_Release+0x10 : call dword ptr [ecx+0x08]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #8
        { 0x11, 0x51 }, // (Offset,Value)-Pair #9
        { 0x12, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPitchB
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPitchB, 3936, 11,

    XREF_DSBUFFERSETPITCHB,
    XRefZero)
        // IDirectSoundBuffer8_SetPitchB+0x21 : lea edi, [esi+0xB4]
        { 0x21, 0x8D }, // (Offset,Value)-Pair #1
        { 0x22, 0xBE }, // (Offset,Value)-Pair #2
        { 0x23, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x24, 0x00 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetPitchB+0x58 : movzx eax, word ptr [eax+2]
        { 0x58, 0x0F }, // (Offset,Value)-Pair #5
        { 0x59, 0xB7 }, // (Offset,Value)-Pair #6
        { 0x5A, 0x40 }, // (Offset,Value)-Pair #7
        { 0x5B, 0x02 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetPitchB+0x5E : dec eax; sar eax, 1
        { 0x5E, 0x48 }, // (Offset,Value)-Pair #9
        { 0x5F, 0xD1 }, // (Offset,Value)-Pair #10
        { 0x60, 0xF8 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPitchA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPitchA, 3936, 12,

    XREF_DSBUFFERSETPITCHA,
    XRefOne)

        // IDirectSoundBuffer8_SetPitchA+0x1B : call [IDirectSoundBuffer8_SetPitchB]
        { 0x1B, XREF_DSBUFFERSETPITCHB }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetPitchA+0x07 : mov edx, [esp+0x10]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #2
        { 0x08, 0x54 }, // (Offset,Value)-Pair #3
        { 0x09, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetPitchA+0x11 : mov ecx, [eax+0x18]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #6
        { 0x12, 0x48 }, // (Offset,Value)-Pair #7
        { 0x13, 0x18 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetPitchA+0x23 : jz +0x0B
        { 0x23, 0x74 }, // (Offset,Value)-Pair #9
        { 0x24, 0x0B }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetPitchA+0x34 : retn 0x08
        { 0x34, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x35, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPitch, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetPitch+0x15 : call [IDirectSoundBuffer8_SetPitchA]
        { 0x15, XREF_DSBUFFERSETPITCHA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetPitch+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetPitch+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetPitch+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer::GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 3936, 13,

    XREF_DSBUFFERGETSTATUSB,
    XRefZero)

        // CMcpxBuffer_GetStatus+0x10 : mov eax, [ebp+0x08]
        { 0x10, 0x8B }, // (Offset,Value)-Pair #1
        { 0x11, 0x45 }, // (Offset,Value)-Pair #2
        { 0x12, 0x08 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_GetStatus+0x16 : xor ecx, ecx; inc ecx
        { 0x16, 0x33 }, // (Offset,Value)-Pair #4
        { 0x17, 0xC9 }, // (Offset,Value)-Pair #5
        { 0x18, 0x41 }, // (Offset,Value)-Pair #6

        // CMcpxBuffer_GetStatus+0x1C : jz +0x17
        { 0x1C, 0x74 }, // (Offset,Value)-Pair #7
        { 0x1D, 0x17 }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetStatus+0x2F : mov [eax], 5
        { 0x2F, 0xC7 }, // (Offset,Value)-Pair #9
        { 0x30, 0x00 }, // (Offset,Value)-Pair #10
        { 0x31, 0x05 }, // (Offset,Value)-Pair #11

        // CMcpxBuffer_GetStatus+0x48 : retn 0x04
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x49, 0x04 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 3936, 10,

    XREF_DSBUFFERGETSTATUSA,
    XRefOne)

        // CDirectSoundBuffer_GetStatus+0x14 : call [CMcpxBuffer::GetStatus]
        { 0x15, XREF_DSBUFFERGETSTATUSB }, // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_GetStatus+0x07 : push [esp+0x10]
        { 0x07, 0xFF }, // (Offset,Value)-Pair #2
        { 0x08, 0x74 }, // (Offset,Value)-Pair #3
        { 0x09, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #5

        // CDirectSoundBuffer_GetStatus+0x11 : mov ecx, [eax+0x20]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #6
        { 0x12, 0x48 }, // (Offset,Value)-Pair #7
        { 0x13, 0x20 }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_GetStatus+0x2E : retn 0x08
        { 0x2E, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x2F, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_GetStatus
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_GetStatus, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_GetStatus+0x15 : call [CDirectSoundBuffer::GetStatus]
        { 0x15, XREF_DSBUFFERGETSTATUSA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_GetStatus+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_GetStatus+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_GetStatus+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetVolumeB
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVolumeB, 3936, 11,

    XREF_DSBUFFERSETVOLUMEB,
    XRefZero)

        // IDirectSoundBuffer8_SetVolumeB+0x26 : mov ecx, [esi+0xE0]
        { 0x26, 0x8B }, // (Offset,Value)-Pair #1
        { 0x27, 0x8E }, // (Offset,Value)-Pair #2
        { 0x28, 0xE0 }, // (Offset,Value)-Pair #3
        { 0x29, 0x00 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetVolumeB+0x2F : movzx ecx, word ptr [ecx+2]
        { 0x2F, 0x0F }, // (Offset,Value)-Pair #5
        { 0x30, 0xB7 }, // (Offset,Value)-Pair #6
        { 0x31, 0x49 }, // (Offset,Value)-Pair #7
        { 0x32, 0x02 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetVolumeB+0x35 : dec ecx; sar ecx, 1
        { 0x35, 0x49 }, // (Offset,Value)-Pair #9
        { 0x36, 0xD1 }, // (Offset,Value)-Pair #10
        { 0x37, 0xF9 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetVolumeA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVolumeA, 3936, 12,

    XREF_DSBUFFERSETVOLUMEA,
    XRefOne)

        // IDirectSoundBuffer8_SetVolumeA+0x1E : call [IDirectSoundBuffer8_SetVolumeB]
        { 0x1E, XREF_DSBUFFERSETVOLUMEB }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetVolumeA+0x07 : mov ecx, [esp+0x0C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #2
        { 0x08, 0x4C }, // (Offset,Value)-Pair #3
        { 0x09, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetVolumeA+0x11 : mov eax, [ecx+0x18]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #6
        { 0x12, 0x41 }, // (Offset,Value)-Pair #7
        { 0x13, 0x18 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetVolumeA+0x26 : jz +0x0B
        { 0x26, 0x74 }, // (Offset,Value)-Pair #9
        { 0x27, 0x0B }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetVolumeA+0x37 : retn 0x08
        { 0x37, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x38, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetVolume+0x15 : call [IDirectSoundBuffer8_SetVolumeA]
        { 0x15, XREF_DSBUFFERSETVOLUMEA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetVolume+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetVolume+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetVolume+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPositionB
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetCurrentPositionB, 3936, 11,

    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XRefZero)

        // IDirectSoundBuffer8_SetCurrentPositionB+0x25 : mov eax, [esi+0x148]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #1
        { 0x26, 0x86 }, // (Offset,Value)-Pair #2
        { 0x27, 0x48 }, // (Offset,Value)-Pair #3
        { 0x28, 0x01 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetCurrentPositionB+0x4A : push edi; push 4
        { 0x4A, 0x57 }, // (Offset,Value)-Pair #5
        { 0x4B, 0x6A }, // (Offset,Value)-Pair #6
        { 0x4C, 0x04 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_SetCurrentPositionB+0x71 : movzx eax, word ptr [eax+2]
        { 0x71, 0x0F }, // (Offset,Value)-Pair #8
        { 0x72, 0xB7 }, // (Offset,Value)-Pair #9
        { 0x73, 0x40 }, // (Offset,Value)-Pair #10
        { 0x74, 0x02 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPositionA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetCurrentPositionA, 3936, 12,

    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XRefOne)

        // IDirectSoundBuffer8_SetCurrentPositionA+0x15 : call [IDirectSoundBuffer8_SetCurrentPositionB]
        { 0x15, XREF_DSBUFFERSETCURRENTPOSITIONB }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetCurrentPositionA+0x0D : mov eax, [esp+0x10]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0E, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #4
        { 0x10, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetCurrentPositionA+0x11 : mov ecx, [eax+0x20]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #6
        { 0x12, 0x48 }, // (Offset,Value)-Pair #7
        { 0x13, 0x20 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetCurrentPositionA+0x1D : jz +0x0B
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1E, 0x0B }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetCurrentPositionA+0x2E : retn 0x08
        { 0x2E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2F, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetCurrentPosition, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetCurrentPosition+0x15 : call [IDirectSoundBuffer8_SetCurrentPositionA]
        { 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetCurrentPosition+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetCurrentPosition+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetCurrentPosition+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegionA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPlayRegionA, 3936, 10,

    XREF_DSBUFFERSETPLAYREGIONA,
    XRefZero)

        // IDirectSoundBuffer8_SetPlayRegionA+0x20 : cmp edx, [ecx+0x54]
        { 0x20, 0x3B }, // (Offset,Value)-Pair #1
        { 0x21, 0x51 }, // (Offset,Value)-Pair #2
        { 0x22, 0x54 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetPlayRegionA+0x2C : mov ecx, [esi+0x20]
        { 0x2C, 0x8B }, // (Offset,Value)-Pair #4
        { 0x2D, 0x4E }, // (Offset,Value)-Pair #5
        { 0x2E, 0x20 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetPlayRegionA+0x38 : jz +0x0B
        { 0x38, 0x74 }, // (Offset,Value)-Pair #7
        { 0x39, 0x0B }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetPlayRegionA+0x4A : retn 0x0C
        { 0x4A, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4B, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPlayRegion, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetPlayRegion+0x19 : call [IDirectSoundBuffer8_SetPlayRegionA]
        { 0x19, XREF_DSBUFFERSETPLAYREGIONA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetPlayRegion+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetPlayRegion+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetPlayRegion+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_LockA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_LockA, 3936, 10,

    XREF_DSBUFFERLOCKA,
    XRefZero)

        // IDirectSoundBuffer8_LockA+0x37 : mov eax, [eax+0x4C]
        { 0x37, 0x8B }, // (Offset,Value)-Pair #1
        { 0x38, 0x40 }, // (Offset,Value)-Pair #2
        { 0x39, 0x4C }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_LockA+0x68 : mov [edi], ebx; jz +0x1F
        { 0x68, 0x89 }, // (Offset,Value)-Pair #4
        { 0x69, 0x1F }, // (Offset,Value)-Pair #5
        { 0x6A, 0x74 }, // (Offset,Value)-Pair #6
        { 0x6B, 0x1F }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_LockA+0x85 : and dword ptr [edx], 0
        { 0x85, 0x83 }, // (Offset,Value)-Pair #8
        { 0x86, 0x22 }, // (Offset,Value)-Pair #9
        { 0x87, 0x00 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Lock
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Lock, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_Lock+0x28 : call [IDirectSoundBuffer8_LockA]
        { 0x28, XREF_DSBUFFERLOCKA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_Lock+0x03 : push [esp+0x24]
        { 0x03, 0xFF }, // (Offset,Value)-Pair #2
        { 0x04, 0x75 }, // (Offset,Value)-Pair #3
        { 0x05, 0x24 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_Lock+0x11 : add eax, 0xFFFFFFE4
        { 0x11, 0x83 }, // (Offset,Value)-Pair #5
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x13, 0xE4 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_Lock+0x2D : retn 0x20
        { 0x2D, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x20 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetHeadroomA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetHeadroomA, 3936, 12,

    XREF_DSBUFFERSETHEADROOMA,
    XRefOne)

    // WARNING: I am unsure if this is SetHeadroom

        // IDirectSoundBuffer8_SetHeadroomA+0x23 : call [IDirectSoundBuffer8_SetVolumeB]
        { 0x23, XREF_DSBUFFERSETVOLUMEB }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetHeadroomA+0x07 : mov edx, [esp+0x0C]
        { 0x07, 0x8B }, // (Offset,Value)-Pair #2
        { 0x08, 0x54 }, // (Offset,Value)-Pair #3
        { 0x09, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0A, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetHeadroomA+0x11 : mov eax, [edx+0x18]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #6
        { 0x12, 0x42 }, // (Offset,Value)-Pair #7
        { 0x13, 0x18 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetHeadroomA+0x2B : jz +0x0B
        { 0x2B, 0x74 }, // (Offset,Value)-Pair #9
        { 0x2C, 0x0B }, // (Offset,Value)-Pair #10

        // IDirectSoundBuffer8_SetHeadroomA+0x3C : retn 0x08
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x3D, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetHeadroom, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetHeadroom+0x15 : call [IDirectSoundBuffer8_SetHeadroomA]
        { 0x15, XREF_DSBUFFERSETHEADROOMA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetHeadroom+0x04 : push [esp+0x08]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetHeadroom+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetHeadroom+0x19 : retn 0x08
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferDataA
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetBufferDataA, 3936, 9,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        // IDirectSoundBuffer8_SetBufferDataA+0x22 : cmp ecx, [eax+0x4C]
        { 0x22, 0x3B }, // (Offset,Value)-Pair #1
        { 0x23, 0x48 }, // (Offset,Value)-Pair #2
        { 0x24, 0x4C }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetBufferDataA+0x38 : mov ecx, [esi+0x1C]
        { 0x38, 0x8B }, // (Offset,Value)-Pair #4
        { 0x39, 0x4E }, // (Offset,Value)-Pair #5
        { 0x3A, 0x1C }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetBufferDataA+0x71 : leave; retn 0x0C
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #7
        { 0x72, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x73, 0x0C }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetBufferData, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetBufferData+0x18 : call [IDirectSoundBuffer8_SetBufferDataA]
        { 0x19, XREF_DSSETBUFFERDATA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetBufferData+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetBufferData+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetBufferData+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetMixBins
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetMixBins, 3936, 15,

    XREF_DSSETMIXBINSB,
    XRefZero)

        // CMcpxVoiceClient_SetMixBins+0x1C : lea eax, [esi+0x84]
        { 0x1D, 0x86 }, // (Offset,Value)-Pair #1
        { 0x1E, 0x84 }, // (Offset,Value)-Pair #2
        { 0x21, 0x00 }, // (Offset,Value)-Pair #3

        // CMcpxVoiceClient_SetMixBins+0x2E : mov eax, [esi+0xE0]
        { 0x2E, 0x8B }, // (Offset,Value)-Pair #4
        { 0x30, 0xE0 }, // (Offset,Value)-Pair #5
        { 0x31, 0x00 }, // (Offset,Value)-Pair #6

        // CMcpxVoiceClient_SetMixBins+0x41 : jz +0x7F
        { 0x41, 0x74 }, // (Offset,Value)-Pair #7
        { 0x42, 0x7F }, // (Offset,Value)-Pair #8

        // CMcpxVoiceClient_SetMixBins+0x5A : mov ds:[0xFE8202F8], eax
        { 0x5A, 0xA3 }, // (Offset,Value)-Pair #9
        { 0x5B, 0xF8 }, // (Offset,Value)-Pair #10
        { 0x5C, 0x02 }, // (Offset,Value)-Pair #11
        { 0x5D, 0x82 }, // (Offset,Value)-Pair #12

        // CMcpxVoiceClient_SetMixBins+0xBF : jb +0x8C
        { 0xBF, 0x72 }, // (Offset,Value)-Pair #13
        { 0xC0, 0x8C }, // (Offset,Value)-Pair #14

        // CMcpxVoiceClient_SetMixBins+0xD6 : retn
        { 0xD6, 0xC3 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 3936, 9,

    XREF_DSSETMIXBINSA,
    XRefOne)

        // CDirectSoundVoice_SetMixBins+0x28 : call [CMcpxVoiceClient::SetMixBins]
        { 0x29, XREF_DSSETMIXBINSB }, // (Offset,Value)-Pair #1

        // CDirectSoundVoice_SetMixBins+0x11 : mov ecx, [esi+0x18]
        { 0x11, 0x8B }, // (Offset,Value)-Pair #2
        { 0x12, 0x4E }, // (Offset,Value)-Pair #3
        { 0x13, 0x18 }, // (Offset,Value)-Pair #4

        // CDirectSoundVoice_SetMixBins+0x25 : mov ecx, [esi+0x14]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #5
        { 0x26, 0x4E }, // (Offset,Value)-Pair #6
        { 0x27, 0x14 }, // (Offset,Value)-Pair #7

        // CDirectSoundVoice_SetMixBins+0x55 : retn 0x08
        { 0x55, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x56, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetMixBins, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_SetFrequency+0x0C : call [CDirectSoundVoice::SetMixBins]
        { 0x0D, XREF_DSSETMIXBINSA }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_SetMixBins+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundStream_SetMixBins+0x08 : add eax, 0x04
        { 0x08, 0x83 }, // (Offset,Value)-Pair #6
        { 0x09, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8

        // IDirectSoundStream_SetMixBins+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x12, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetMixBins, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetMixBins+0x14 : call [CDirectSoundVoice::SetMixBins]
        { 0x15, XREF_DSSETMIXBINSA },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetMixBins+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetMixBins+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #5
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetMixBins+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 3936, 11,

    XREF_GETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x12 : mov eax, [esi+8]
        { 0x12, 0x8B }, // (Offset,Value)-Pair #1
        { 0x13, 0x46 }, // (Offset,Value)-Pair #2
        { 0x14, 0x08 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_GetCurrentPosition+0x1A : jnz +0x79
        { 0x1A, 0x75 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x79 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_GetCurrentPosition+0x8E : div dword ptr [ecx+0x4C]
        { 0x8E, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x8F, 0x71 }, // (Offset,Value)-Pair #7
        { 0x90, 0x4C }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetCurrentPosition+0xC8 : retn 0x08
        { 0xC8, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xC9, 0x08 }, // (Offset,Value)-Pair #10
        { 0xCA, 0x00 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition2, 3936, 11,

    XREF_GETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x12 : mov eax, [esi+0x18]
        { 0x12, 0x8B }, // (Offset,Value)-Pair #1
        { 0x13, 0x46 }, // (Offset,Value)-Pair #2
        { 0x14, 0x18 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_GetCurrentPosition+0x1A : jnz +0x79
        { 0x1A, 0x75 }, // (Offset,Value)-Pair #4
        { 0x1B, 0x79 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_GetCurrentPosition+0x8E : div dword ptr [ecx+0x4C]
        { 0x8E, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x8F, 0x71 }, // (Offset,Value)-Pair #7
        { 0x90, 0x4C }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetCurrentPosition+0xC8 : retn 0x08
        { 0xC8, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xC9, 0x08 }, // (Offset,Value)-Pair #10
        { 0xCA, 0x00 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 3936, 9,

    XREF_GETCURRENTPOSITION2,
    XRefOne)

        // CDirectSoundBuffer_GetCurrentPosition+0x19 : call [CMcpxBuffer::GetCurrentPosition]
        { 0x19, XREF_GETCURRENTPOSITION },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_GetCurrentPosition+0x0D : mov eax, [esp+0x10]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0E, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #4
        { 0x10, 0x10 }, // (Offset,Value)-Pair #5

        // CDirectSoundBuffer_GetCurrentPosition+0x21 : jz +0x0B
        { 0x21, 0x74 }, // (Offset,Value)-Pair #6
        { 0x22, 0x0B }, // (Offset,Value)-Pair #7

        // CDirectSoundBuffer_GetCurrentPosition+0x32 : retn 0x0C
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x33, 0x0C }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_GetCurrentPosition, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_GetCurrentPosition+0x19 : call [CDirectSoundBuffer::GetCurrentPosition]
        { 0x19, XREF_GETCURRENTPOSITION2 },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_GetCurrentPosition+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_GetCurrentPosition+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #5
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_GetCurrentPosition+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #7
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 3936, 7,

    XREF_DSOUNDPLAY,
    XRefZero)

        { 0x07, 0xFF },
        { 0x0C, 0xF0 },
        { 0x13, 0x24 },
        { 0x1A, 0x85 },
        { 0x25, 0xFF },
        { 0x2B, 0x8B },
        { 0x2F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Play, 3936, 16,

    XRefNoSaveIndex,
    XRefOne)

	// IDirectSoundBuffer8_Play+0x1D : call CDirectSoundBuffer::Play
	{ 0x1D, XREF_DSOUNDPLAY },

        // IDirectSoundBuffer8_Play+0x00 : push [esp+0x10]
        { 0x00, 0xFF }, // (Offset,Value)-Pair #1
        { 0x01, 0x74 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

	// IDirectSoundBuffer8_Play+0x04 : mov eax, [esp+4+arg_0]
	{ 0x04, 0x8B }, // (Offset,Value)-Pair #5
	{ 0x05, 0x44 }, // (Offset,Value)-Pair #6
	{ 0x06, 0x24 }, // (Offset,Value)-Pair #7
	{ 0x07, 0x08 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_Play+0x12 : and eax, [0xFFFFFFE4]
        { 0x12, 0x83 }, // (Offset,Value)-Pair #9
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #10
        { 0x14, 0xE4 }, // (Offset,Value)-Pair #11

        // IDirectSoundBuffer8_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #12
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #13

        // IDirectSoundBuffer8_Play+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x22, 0x10 }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Play2
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Play2, 3936, 12,

    XRefNoSaveIndex,
    XRefOne)

	// IDirectSoundBuffer8_Play+0x1D : call CDirectSoundBuffer::Play
	{ 0x1D, XREF_DSOUNDPLAY },

        // IDirectSoundBuffer8_Play+0x00 : push [esp+0x10]
        { 0x00, 0xFF }, // (Offset,Value)-Pair #1
        { 0x01, 0x74 }, // (Offset,Value)-Pair #2
        { 0x02, 0x24 }, // (Offset,Value)-Pair #3
        { 0x03, 0x10 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_Play+0x12 : and eax, [0xFFFFFFE4]
        { 0x12, 0x83 }, // (Offset,Value)-Pair #5
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #6
        { 0x14, 0xE4 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #8
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #9

        // IDirectSoundBuffer8_Play+0x21 : retn 0x10
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x10 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop, 3936, 12,

    XREF_DSOUNDSTOP,
    XRefZero)

        // CDirectSoundBuffer_Stop+0x07 : mov eax, 0x80004005
        { 0x07, 0x6A }, // (Offset,Value)-Pair #1
        { 0x08, 0x00 }, // (Offset,Value)-Pair #2
        { 0x09, 0x6A }, // (Offset,Value)-Pair #3
        { 0x0A, 0x00 }, // (Offset,Value)-Pair #4
        { 0x0B, 0x6A }, // (Offset,Value)-Pair #5
        { 0x0C, 0x00 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_Stop+0x1A : mov ebx, eax; jz +0x0B
        { 0x1A, 0x8B }, // (Offset,Value)-Pair #7
        { 0x1B, 0xD8 }, // (Offset,Value)-Pair #8
        { 0x1C, 0x74 }, // (Offset,Value)-Pair #9
        { 0x1D, 0x0B }, // (Offset,Value)-Pair #10

        // CDirectSoundBuffer_Stop+0x2D : retn 0x04
        { 0x2D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x2E, 0x04 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Stop
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Stop, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_Stop+0x11 : call [CDirectSoundBuffer::Stop]
        { 0x11, XREF_DSOUNDSTOP },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_Stop+0x06 : and eax, 0xFFFFFFE4
        { 0x06, 0x83 }, // (Offset,Value)-Pair #2
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x08, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_Stop+0x0B : sbb ecx, ecx
        { 0x0B, 0x1B }, // (Offset,Value)-Pair #5
        { 0x0C, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_Stop+0x0D : and ecx, eax
        { 0x0D, 0x23 }, // (Offset,Value)-Pair #7
        { 0x0E, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 3936, 10,

    XREF_DSSTREAMSETVOLUME,
    XRefZero)

    // TODO: re-use for IDirectSoundBuffer_SetVolume!

        // CMcpxVoiceClient_SetVolume+0x14 : sub edx, [eax+0x1C]
        { 0x14, 0x2B }, // (Offset,Value)-Pair #1
        { 0x15, 0x50 }, // (Offset,Value)-Pair #2
        { 0x16, 0x1C }, // (Offset,Value)-Pair #3

        // CMcpxVoiceClient_SetVolume+0x1A : mov ecx, [ecx+0x14]
        { 0x1A, 0x8B }, // (Offset,Value)-Pair #4
        { 0x1B, 0x49 }, // (Offset,Value)-Pair #5
        { 0x1C, 0x14 }, // (Offset,Value)-Pair #6

        // CMcpxVoiceClient_SetVolume+0x26 : jz +0x0B
        { 0x26, 0x74 }, // (Offset,Value)-Pair #7
        { 0x27, 0x0B }, // (Offset,Value)-Pair #8

        // CMcpxVoiceClient_SetVolume+0x37 : retn 0x08
        { 0x37, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x38, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetVolume+0x0D : call [IDirectSoundStream::SetVolume]
        { 0x0D, XREF_DSSTREAMSETVOLUME }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetVolume+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetVolume+0x08 : add eax, 0x04
        { 0x08, 0x83 }, // (Offset,Value)-Pair #6
        { 0x09, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetVolume+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x12, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetAllParametersB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParametersB, 3936, 11,

    XREF_DSSTREAMSETALLPARAMETERS1B,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetAllParametersB!?

        // CDirectSoundStream_SetAllParametersB+0x10 : or byte ptr [eax+0x80], 0x80
        { 0x10, 0x80 }, // (Offset,Value)-Pair #1
        { 0x11, 0x88 }, // (Offset,Value)-Pair #2
        { 0x12, 0x80 }, // (Offset,Value)-Pair #3
        { 0x13, 0x00 }, // (Offset,Value)-Pair #4
        { 0x14, 0x00 }, // (Offset,Value)-Pair #5
        { 0x15, 0x00 }, // (Offset,Value)-Pair #6
        { 0x16, 0x80 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetAllParametersB+0x20 : push 0
        { 0x20, 0x6A }, // (Offset,Value)-Pair #8
        { 0x21, 0x00 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetAllParametersB+0x2B : retn 0x08
        { 0x2B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x2C, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParametersA, 3936, 12,

    XREF_DSSTREAMSETALLPARAMETERS1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetAllParametersA!?

        // CDirectSoundStream_SetAllParametersA+0x25 : call [IDirectSoundStream::SetAllParametersB]
        { 0x1F, XREF_DSSTREAMSETALLPARAMETERS1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetAllParametersA+0x09 : mov eax, [esp+0x14]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetAllParametersA+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xD0 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #8
        { 0x10, 0xE0 }, // (Offset,Value)-Pair #9
        { 0x11, 0x01 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetAllParametersA+0x38 : retn 0x0C
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetAllParameters
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetAllParameters, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetAllParameters+0x11 : call [IDirectSoundStream::SetAllParametersA]
        { 0x11, XREF_DSSTREAMSETALLPARAMETERS1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetAllParameters+0x04 : push eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetAllParameters+0x0C : add eax, 0x04
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetAllParameters+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeAnglesB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeAnglesB, 3936, 12,

    XREF_DSSTREAMSETCONEANGLES1B,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesB!?

        // CDirectSoundStream_SetConeAnglesB+0x04 : or dword ptr [eax+0x80], 0x10
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetConeAnglesB+0x10 : mov [ecx+0x38], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x38 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetConeAnglesB+0x25 : retn 0x0C
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x26, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeAnglesA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeAnglesA, 3936, 12,

    XREF_DSSTREAMSETCONEANGLES1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesA!?

        // CDirectSoundStream_SetConeAnglesA+0x23 : call [IDirectSoundStream::SetConeAnglesB]
        { 0x23, XREF_DSSTREAMSETCONEANGLES1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeAnglesA+0x09 : mov eax, [esp+0x18]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x18 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetConeAnglesA+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xD0 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #8
        { 0x10, 0xE0 }, // (Offset,Value)-Pair #9
        { 0x11, 0x01 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetConeAnglesA+0x3C : retn 0x10
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x3D, 0x10 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeAngles
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeAngles, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeAngles+0x15 : call [IDirectSoundStream::SetConeAnglesA]
        { 0x15, XREF_DSSTREAMSETCONEANGLES1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeAngles+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetConeAngles+0x0C : add eax, 0x04
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetConeAngles+0x19 : retn 0x10
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolumeB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolumeB, 3936, 12,

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeB!?

        // CDirectSoundStream_SetConeOutsideVolumeB+0x04 : or dword ptr [eax+0x80], 0x10
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetConeOutsideVolumeB+0x10 : mov [ecx+0x4C], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x4C }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetConeOutsideVolumeB+0x1E : retn 0x08
        { 0x1E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolumeA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolumeA, 3936, 12,

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeA!?

        // CDirectSoundStream_SetConeOutsideVolumeA+0x1F : call [IDirectSoundStream::SetConeOutsideVolumeB]
        { 0x1F, XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeOutsideVolumeA+0x09 : mov eax, [esp+0x14]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x24 }, // (Offset,Value)-Pair #4
        { 0x0C, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetConeOutsideVolumeA+0x0D : not eax; and eax, 1
        { 0x0D, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xD0 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #8
        { 0x10, 0xE0 }, // (Offset,Value)-Pair #9
        { 0x11, 0x01 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetConeOutsideVolumeA+0x38 : retn 0x0C
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x39, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolume, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeOutsideVolume+0x11 : call [IDirectSoundStream::SetConeOutsideVolumeA]
        { 0x11, XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeOutsideVolume+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetConeOutsideVolume+0x0C : add eax, 0x04
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetConeOutsideVolume+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceC
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistanceC, 3936, 12,

    XREF_DSSTREAMSETMAXDISTANCE1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceC!?

        // CDirectSoundStream_SetMaxDistanceC+0x04 : or byte ptr [ecx+0x80], 0x04
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetMaxDistanceC+0x10 : mov [ecx+0x54], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x54 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetMaxDistanceC+0x1E : retn 0x08
        { 0x1E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistanceB, 3936, 15,

    XREF_DSSTREAMSETMAXDISTANCE1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceB!?

        // CDirectSoundStream_SetMaxDistanceB+0x23 : call [IDirectSoundStream::SetMaxDistanceC]
        { 0x23, XREF_DSSTREAMSETMAXDISTANCE1C }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMaxDistanceB+0x0D : mov eax, [esp+0x14]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0E, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #4
        { 0x10, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMaxDistanceB+0x11 : not eax
        { 0x11, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x12, 0xD0 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetMaxDistanceB+0x13 : and eax, 1
        { 0x13, 0x83 }, // (Offset,Value)-Pair #8
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #9
        { 0x15, 0x01 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetMaxDistanceB+0x1C : mov ecx, [eax+0x14]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #11
        { 0x1D, 0x48 }, // (Offset,Value)-Pair #12
        { 0x1E, 0x14 }, // (Offset,Value)-Pair #13

        // CDirectSoundStream_SetMaxDistanceB+0x3C : retn 0x0C
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x3D, 0x0C }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistanceA, 3936, 10,

    XREF_DSSTREAMSETMAXDISTANCE1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceA!?

        // CDirectSoundStream_SetMaxDistanceA+0x15 : call [IDirectSoundStream::SetMaxDistanceB]
        { 0x15, XREF_DSSTREAMSETMAXDISTANCE1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMaxDistanceA+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMaxDistanceA+0x0D : add eax, 0x04
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetMaxDistanceA+0x19 : retn 0x0C
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistance, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMaxDistance+0x11 : call [IDirectSoundStream::SetMaxDistanceA]
        { 0x11, XREF_DSSTREAMSETMAXDISTANCE1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMaxDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMaxDistance+0x0C : push [esp+0x0C]
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #6
        { 0x0D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #8
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetMaxDistance+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x16, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceC
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistanceC, 3936, 12,

    XREF_DSSTREAMSETMINDISTANCE1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceC!?

        // CDirectSoundStream_SetMinDistanceC+0x04 : or byte ptr [ecx+0x80], 0x04
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetMinDistanceC+0x10 : mov [ecx+0x50], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x50 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetMinDistanceC+0x1E : retn 0x08
        { 0x1E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistanceB, 3936, 15,

    XREF_DSSTREAMSETMINDISTANCE1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceB!?

        // CDirectSoundStream_SetMinDistanceB+0x23 : call [IDirectSoundStream::SetMinDistanceC]
        { 0x23, XREF_DSSTREAMSETMINDISTANCE1C }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMinDistanceB+0x0D : mov eax, [esp+0x14]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0E, 0x44 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x24 }, // (Offset,Value)-Pair #4
        { 0x10, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMinDistanceB+0x11 : not eax
        { 0x11, 0xF7 }, // (Offset,Value)-Pair #6
        { 0x12, 0xD0 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetMinDistanceB+0x13 : and eax, 1
        { 0x13, 0x83 }, // (Offset,Value)-Pair #8
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #9
        { 0x15, 0x01 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetMinDistanceB+0x1C : mov ecx, [eax+0x14]
        { 0x1C, 0x8B }, // (Offset,Value)-Pair #11
        { 0x1D, 0x48 }, // (Offset,Value)-Pair #12
        { 0x1E, 0x14 }, // (Offset,Value)-Pair #13

        // CDirectSoundStream_SetMinDistanceB+0x3C : retn 0x0C
        { 0x3C, 0xC2 }, // (Offset,Value)-Pair #14
        { 0x3D, 0x0C }, // (Offset,Value)-Pair #15
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistanceA, 3936, 10,

    XREF_DSSTREAMSETMINDISTANCE1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceA!?

        // CDirectSoundStream_SetMinDistanceA+0x15 : call [IDirectSoundStream::SetMinDistanceB]
        { 0x15, XREF_DSSTREAMSETMINDISTANCE1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMinDistanceA+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMinDistanceA+0x0D : add eax, 0x04
        { 0x0D, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0E, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0F, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetMinDistanceA+0x19 : retn 0x0C
        { 0x19, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMinDistance+0x11 : call [IDirectSoundStream::SetMinDistanceA]
        { 0x11, XREF_DSSTREAMSETMINDISTANCE1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMinDistance+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetMinDistance+0x0C : push [esp+0x0C]
        { 0x0C, 0xFF }, // (Offset,Value)-Pair #6
        { 0x0D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x24 }, // (Offset,Value)-Pair #8
        { 0x0F, 0x0C }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetMinDistance+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x16, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocityC
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocityC, 3936, 12,

    XREF_DSSTREAMSETVELOCITY1C,
    XRefZero)

        // CDirectSoundStream_SetVelocityC+0x09 : movsd; movsd; movsd
        { 0x09, 0xA5 }, // (Offset,Value)-Pair #8
        { 0x0A, 0xA5 }, // (Offset,Value)-Pair #9
        { 0x0B, 0xA5 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetVelocityC+0x0C : or byte ptr [ecx+0x80], 0x40
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x40 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetVelocityC+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x26, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocityB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocityB, 3936, 14,

    XREF_DSSTREAMSETVELOCITY1B,
    XRefOne)

        // CDirectSoundStream_SetVelocityB+0x35 : call [IDirectSoundStream::SetVelocityC]
        { 0x35, XREF_DSSTREAMSETVELOCITY1C }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetVelocityB+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B }, // (Offset,Value)-Pair #2
        { 0x22, 0x45 }, // (Offset,Value)-Pair #3
        { 0x23, 0x18 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetVelocityB+0x24 : not eax
        { 0x24, 0xF7 }, // (Offset,Value)-Pair #5
        { 0x25, 0xD0 }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_SetVelocityB+0x26 : and eax, 1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #7
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x28, 0x01 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetVelocityB+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #10
        { 0x32, 0x48 }, // (Offset,Value)-Pair #11
        { 0x33, 0x14 }, // (Offset,Value)-Pair #12

        // CDirectSoundStream_SetVelocityB+0x4F : retn 0x14
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x50, 0x14 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocityA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocityA, 3936, 9,

    XREF_DSSTREAMSETVELOCITY1A,
    XRefOne)

        // CDirectSoundStream_SetVelocityA+0x25 : call [IDirectSoundStream::SetVelocityB]
        { 0x25, XREF_DSSTREAMSETVELOCITY1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetVelocityA+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetVelocityA+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #6
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetVelocityA+0x2A : retn 0x14
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetVelocity, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetVelocity+0x21 : call [IDirectSoundStream::SetVelocityA]
        { 0x21, XREF_DSSTREAMSETVELOCITY1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetVelocity+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetVelocity+0x1A : fstp [esp]
        { 0x1A, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #6
        { 0x1C, 0x24 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetVelocity+0x26 : retn 0x14
        { 0x26, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x27, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetVelocity
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVelocity, 3936, 9,

    XREF_DSBUFFERSETVELOCITY,
    XRefOne)

        // CDirectSoundBuffer_SetVelocity+0x20 : call [CDirectSoundStream::SetVelocityB]
        { 0x21, XREF_DSSTREAMSETVELOCITY1B }, // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetVelocity+0x06 : fld [ebp+arg_C]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundBuffer_SetVelocity+0x1A : fstp [esp+0x10+var_10]
        { 0x1A, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #6
        { 0x1C, 0x24 }, // (Offset,Value)-Pair #7

        // CDirectSoundBuffer_SetVelocity+0x26 : retn 0x14
        { 0x27, 0x14 }, // (Offset,Value)-Pair #8
        { 0x28, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetVelocity, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetVelocity+0x2C : call [CDirectSoundBuffer::SetVelocity]
        { 0x2D, XREF_DSBUFFERSETVELOCITY }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetVelocity+0x09 : mov eax, [ebp+arg_0]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x45 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x08 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetVelocity+0x28 : fstp [esp+0x10+var_10]
        { 0x28, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x29, 0x1C }, // (Offset,Value)-Pair #6
        { 0x2A, 0x24 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_SetVelocity+0x32 : retn 0x14
        { 0x33, 0x14 }, // (Offset,Value)-Pair #8
        { 0x34, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationC
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientationC, 3936, 11,

    XREF_DSSTREAMSETCONEORIENTATION1C,
    XRefZero)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationC!?

        // CDirectSoundStream_SetConeOrientationC+0x0C : or byte ptr [ecx+0x80], 0x18
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x18 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetConeOrientationC+0x1A : jz +0x07
        { 0x1A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x1B, 0x07 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetConeOrientationC+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x26, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientationB, 3936, 14,

    XREF_DSSTREAMSETCONEORIENTATION1B,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationB!?

        // CDirectSoundStream_SetConeOrientationB+0x35 : call [IDirectSoundStream::SetConeOrientationC]
        { 0x35, XREF_DSSTREAMSETCONEORIENTATION1C }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeOrientationB+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B }, // (Offset,Value)-Pair #2
        { 0x22, 0x45 }, // (Offset,Value)-Pair #3
        { 0x23, 0x18 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetConeOrientationB+0x24 : not eax
        { 0x24, 0xF7 }, // (Offset,Value)-Pair #5
        { 0x25, 0xD0 }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_SetConeOrientationB+0x26 : and eax, 1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #7
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x28, 0x01 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetConeOrientationB+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #10
        { 0x32, 0x48 }, // (Offset,Value)-Pair #11
        { 0x33, 0x14 }, // (Offset,Value)-Pair #12

        // CDirectSoundStream_SetConeOrientationB+0x4F : retn 0x14
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x50, 0x14 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientationA, 3936, 9,

    XREF_DSSTREAMSETCONEORIENTATION1A,
    XRefOne)

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationA!?

        // CDirectSoundStream_SetConeOrientationA+0x25 : call [IDirectSoundStream::SetConeOrientationB]
        { 0x25, XREF_DSSTREAMSETCONEORIENTATION1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeOrientationA+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetConeOrientationA+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #6
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetConeOrientationA+0x2A : retn 0x14
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetConeOrientation
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOrientation, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeOrientation+0x21 : call [IDirectSoundStream::SetConeOrientationA]
        { 0x21, XREF_DSSTREAMSETCONEORIENTATION1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetConeOrientation+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetConeOrientation+0x1A : fstp [esp]
        { 0x1A, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #6
        { 0x1C, 0x24 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetConeOrientation+0x26 : retn 0x14
        { 0x26, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x27, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPositionC
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPositionC, 3936, 11,

    XREF_DSSTREAMSETPOSITION1C,
    XRefZero)

        // CDirectSoundStream_SetPositionC+0x0C : or byte ptr [ecx+0x80], 0xFF
        { 0x0C, 0x80 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0xFF }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetPositionC+0x1A : jz +0x07
        { 0x1A, 0x74 }, // (Offset,Value)-Pair #8
        { 0x1B, 0x07 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetPositionC+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x26, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPositionB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPositionB, 3936, 14,

    XREF_DSSTREAMSETPOSITION1B,
    XRefOne)

        // CDirectSoundStream_SetPositionB+0x35 : call [IDirectSoundStream::SetPositionC]
        { 0x35, XREF_DSSTREAMSETPOSITION1C }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetPositionB+0x21 : mov eax, [ebp+0x18]
        { 0x21, 0x8B }, // (Offset,Value)-Pair #2
        { 0x22, 0x45 }, // (Offset,Value)-Pair #3
        { 0x23, 0x18 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetPositionB+0x24 : not eax
        { 0x24, 0xF7 }, // (Offset,Value)-Pair #5
        { 0x25, 0xD0 }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_SetPositionB+0x26 : and eax, 1
        { 0x26, 0x83 }, // (Offset,Value)-Pair #7
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #8
        { 0x28, 0x01 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetPositionB+0x31 : mov ecx, [eax+0x14]
        { 0x31, 0x8B }, // (Offset,Value)-Pair #10
        { 0x32, 0x48 }, // (Offset,Value)-Pair #11
        { 0x33, 0x14 }, // (Offset,Value)-Pair #12

        // CDirectSoundStream_SetPositionB+0x4F : retn 0x14
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x50, 0x14 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPositionA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPositionA, 3936, 9,

    XREF_DSSTREAMSETPOSITION1A,
    XRefOne)

        // CDirectSoundStream_SetPositionA+0x25 : call [IDirectSoundStream::SetPositionB]
        { 0x25, XREF_DSSTREAMSETPOSITION1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetPositionA+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetPositionA+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #6
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetPositionA+0x2A : retn 0x14
        { 0x2A, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x2B, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPosition, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetPosition+0x21 : call [IDirectSoundStream::SetPositionA]
        { 0x21, XREF_DSSTREAMSETPOSITION1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetPosition+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetPosition+0x1A : fstp [esp]
        { 0x1A, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #6
        { 0x1C, 0x24 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetPosition+0x26 : retn 0x14
        { 0x26, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x27, 0x14 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetPosition, 3936, 9,

    XREF_DSBUFFERSETPOSITION,
    XRefOne)

        // CDirectSoundBuffer_SetPosition+0x20 : call [CDirectSoundStream::PositionB]
        { 0x21, XREF_DSSTREAMSETPOSITION1B }, // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetPosition+0x06 : fld [ebp+arg_C]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // CDirectSoundBuffer_SetPosition+0x1A : fstp [esp+0x10+var_10]
        { 0x1A, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x1B, 0x1C }, // (Offset,Value)-Pair #6
        { 0x1C, 0x24 }, // (Offset,Value)-Pair #7

        // CDirectSoundBuffer_SetPosition+0x26 : retn 0x14
        { 0x27, 0x14 }, // (Offset,Value)-Pair #8
        { 0x28, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetPosition, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetPosition+0x2C : call [CDirectSoundBuffer::SetPosition]
        { 0x2D, XREF_DSBUFFERSETPOSITION }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetPosition+0x09 : mov eax, [ebp+arg_0]
        { 0x09, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0A, 0x45 }, // (Offset,Value)-Pair #3
        { 0x0B, 0x08 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetPosition+0x28 : fstp [esp+0x10+var_10]
        { 0x28, 0xD9 }, // (Offset,Value)-Pair #5
        { 0x29, 0x1C }, // (Offset,Value)-Pair #6
        { 0x2A, 0x24 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_SetPosition+0x32 : retn 0x14
        { 0x33, 0x14 }, // (Offset,Value)-Pair #8
        { 0x34, 0x00 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetFrequencyB
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequencyB, 3936, 11,

    XREF_DSSTREAMSETFREQUENCY1B,
    XRefZero)

        // CDirectSoundStream_SetFrequencyB+0x08 : cmp esi, 0xBB80
        { 0x08, 0x81 }, // (Offset,Value)-Pair #1
        { 0x09, 0xFE }, // (Offset,Value)-Pair #2
        { 0x0A, 0x80 }, // (Offset,Value)-Pair #3
        { 0x0B, 0xBB }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetFrequencyB+0x1D : jmp +0x2B
        { 0x1D, 0xEB }, // (Offset,Value)-Pair #5
        { 0x1E, 0x2B }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_SetFrequencyB+0x1F : lea ecx, [ebp+0x08]
        { 0x1F, 0x8D }, // (Offset,Value)-Pair #7
        { 0x20, 0x4D }, // (Offset,Value)-Pair #8
        { 0x21, 0x08 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_SetFrequencyB+0x4E : retn 0x08
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4F, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetFrequencyA
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequencyA, 3936, 9,

    XREF_DSSTREAMSETFREQUENCY1A,
    XRefOne)

        // CDirectSoundStream_SetFrequencyA+0x1F : call [CDirectSoundStream::SetFrequencyB]
        { 0x20, XREF_DSSTREAMSETFREQUENCY1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetFrequencyA+0x15 : mov eax, [esi+0x18]
        { 0x15, 0x8B }, // (Offset,Value)-Pair #2
        { 0x16, 0x46 }, // (Offset,Value)-Pair #3
        { 0x17, 0x18 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetFrequencyA+0x18 : mov eax, [eax+0x10]
        { 0x18, 0x8B }, // (Offset,Value)-Pair #5
        { 0x19, 0x40 }, // (Offset,Value)-Pair #6
        { 0x1A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetFrequencyA+0x40 : retn 0x08
        { 0x40, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x41, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetFrequency, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetFrequency+0x0D : call [IDirectSoundStream::SetFrequency]
        { 0x0D, XREF_DSSTREAMSETFREQUENCY1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetFrequency+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetFrequency+0x08 : add eax, 0x04
        { 0x08, 0x83 }, // (Offset,Value)-Pair #6
        { 0x09, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetFrequency+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x12, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetFrequency
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetFrequency, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetFrequency+0x15 : call [IDirectSoundStream::SetFrequency]
        { 0x15, XREF_DSSTREAMSETFREQUENCY1A },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetFrequency+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetFrequency+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #5
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetFrequency+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::Set3dMode
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dMode, 3936, 12,

    XREF_DSBUFFERSETMODEB,
    XRefZero)

        // CMcpxVoiceClient_Set3dMode+0x04 : or byte ptr [ecx+0x80], 0xFF
        { 0x04, 0x80 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0xFF }, // (Offset,Value)-Pair #7

        // CMcpxVoiceClient_Set3dMode+0x10 : cmp [ecx+0x58], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x58 }, // (Offset,Value)-Pair #10

        // CMcpxVoiceClient_Set3dMode+0x1E : retn 0x08
        { 0x1E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMode, 3936, 10,

    XREF_DSBUFFERSETMODEA,
    XRefOne)

        // CDirectSoundVoice_SetMode+0x1E : call [CMcpxVoiceClient::Set3dMode]
        { 0x1F, XREF_DSBUFFERSETMODEB }, // (Offset,Value)-Pair #1

        // CDirectSoundVoice_SetMode+0x17 : push [esp+0x14]
        { 0x17, 0xFF }, // (Offset,Value)-Pair #2
        { 0x18, 0x74 }, // (Offset,Value)-Pair #3
        { 0x19, 0x24 }, // (Offset,Value)-Pair #4
        { 0x1A, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundVoice_SetMode+0x1B : mov ecx, [eax+0x14]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #6
        { 0x1C, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1D, 0x14 }, // (Offset,Value)-Pair #8

        // CDirectSoundVoice_SetMode+0x38 : retn 0x0C
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetMode
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetMode, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetMode+0x18 : call [CDirectSoundVoice::SetMode]
        { 0x19, XREF_DSBUFFERSETMODEA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetMode+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetMode+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetMode+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

/*
// ******************************************************************
// * CDirectSoundStream_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetRolloffFactor, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetRolloffFactor+0x11 : call [IDirectSoundStream::SetRolloffFactor]
        { 0x11, XREF_DSSTREAMSETROLLOFFFACTOR1A }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetRolloffFactor+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetRolloffFactor+0x0C : add eax, 0x04
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetRolloffFactor+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;
*/
// ******************************************************************
// * IDirectSoundStream_SetI3DL2SourceA
// ******************************************************************
/*OOVPA_XREF(IDirectSoundStream_SetI3DL2SourceA, 3936, 11,

    XREF_DSSETI3DL2SOURCE1A,
    XRefZero)

        // IDirectSoundStream_SetI3DL2SourceA+0x07 : call [addr]
        { 0x07, 0xE8 }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_SetI3DL2SourceA+0x16 : push 0x24
        { 0x16, 0x6A }, // (Offset,Value)-Pair #2
        { 0x17, 0x24 }, // (Offset,Value)-Pair #3

        // IDirectSoundStream_SetI3DL2SourceA+0x23 : shl ebx, cl
        { 0x23, 0xD3 }, // (Offset,Value)-Pair #4
        { 0x24, 0xE3 }, // (Offset,Value)-Pair #5

        // IDirectSoundStream_SetI3DL2SourceA+0x4A : cmp [ebp-0x04], 0x20
        { 0x4A, 0x83 }, // (Offset,Value)-Pair #6
        { 0x4B, 0x7D }, // (Offset,Value)-Pair #7
        { 0x4C, 0xFC }, // (Offset,Value)-Pair #8
        { 0x4D, 0x20 }, // (Offset,Value)-Pair #9

        // IDirectSoundStream_SetI3DL2SourceA+0x71 : retn 0x0C
        { 0x71, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x72, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetI3DL2Source
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetI3DL2Source, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative! (most likely incorrect, actually!)

        // IDirectSoundStream_SetI3DL2SourceB+0x11 : call [IDirectSoundStream_SetI3DL2SourceA]
        { 0x11, XREF_DSSETI3DL2SOURCE1A }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_SetI3DL2SourceB+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundStream_SetI3DL2SourceB+0x0C : add eax, 0x04
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #8

        // IDirectSoundStream_SetI3DL2SourceB+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;
*/

/*
// ******************************************************************
// * IDirectSoundStream_Unknown1
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_Unknown1, 3936, 9,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative! (most likely incorrect, actually!)

        // IDirectSoundStream_Unknown1A+0x38 : call [IDirectSoundStream_SetI3DL2SourceA]
        { 0x38, XREF_DSSETI3DL2SOURCE1A }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_Unknown1A+0x17 : cmp edi, [ecx+0x20]
        { 0x17, 0x3B }, // (Offset,Value)-Pair #2
        { 0x18, 0x79 }, // (Offset,Value)-Pair #3
        { 0x19, 0x20 }, // (Offset,Value)-Pair #4

        // IDirectSoundStream_Unknown1A+0x25 : mov ecx, [esi+0x14]
        { 0x25, 0x8B }, // (Offset,Value)-Pair #5
        { 0x26, 0x4E }, // (Offset,Value)-Pair #6
        { 0x27, 0x14 }, // (Offset,Value)-Pair #7

        // IDirectSoundStream_Unknown1A+0x55 : retn 0x08
        { 0x55, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x56, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;
//*/
// ******************************************************************
// * IDirectSoundStream_SetHeadroomA
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetHeadroomA, 3936, 11,

    XREF_DSSETHEADROOMA,
    XRefZero)

        // IDirectSoundStream_SetHeadroomA+0x09 : call [addr]
        { 0x09, 0xE8 }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_SetHeadroomA+0x17 : cmp edi, [ecx+0x20]
        { 0x17, 0x3B }, // (Offset,Value)-Pair #2
        { 0x18, 0x79 }, // (Offset,Value)-Pair #3
        { 0x19, 0x20 }, // (Offset,Value)-Pair #4

        // IDirectSoundStream_SetHeadroomA+0x1D : jz +0x1F
        { 0x1D, 0x74 }, // (Offset,Value)-Pair #5
        { 0x1E, 0x1F }, // (Offset,Value)-Pair #6

        // IDirectSoundStream_SetHeadroomA+0x3E : cmp [ebp-0x04], 0
        { 0x3E, 0x83 }, // (Offset,Value)-Pair #7
        { 0x3F, 0x7D }, // (Offset,Value)-Pair #8
        { 0x40, 0xFC }, // (Offset,Value)-Pair #9

        // IDirectSoundStream_SetHeadroomA+0x55 : retn 0x08
        { 0x55, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x56, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_SetHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetHeadroom, 3936, 10,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative! (most likely incorrect, actually!)

        // IDirectSoundStream_SetHeadroomB+0x0D : call [IDirectSoundStream_SetHeadroomA]
        { 0x0D, XREF_DSSETHEADROOMA }, // (Offset,Value)-Pair #1

        // IDirectSoundStream_SetHeadroomB+0x04 : mov eax, [esp+0x08]
        { 0x04, 0x8B }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x08 }, // (Offset,Value)-Pair #5

        // IDirectSoundStream_SetHeadroomB+0x08 : add eax, 0x04
        { 0x08, 0x83 }, // (Offset,Value)-Pair #6
        { 0x09, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #8

        // IDirectSoundStream_SetHeadroomB+0x11 : retn 0x08
        { 0x11, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x12, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetOrientation
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_SetOrientation, 3936, 13)

        // IDirectSound8_SetOrientation+0x06 : fld [ebp+0x20]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x07, 0x45 }, // (Offset,Value)-Pair #2
        { 0x08, 0x20 }, // (Offset,Value)-Pair #3

        // IDirectSound8_SetOrientation+0x0C : sub esp, 0x18
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #4
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #5
        { 0x0E, 0x18 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetOrientation+0x18 : add eax, 0xFFFFFFF8
        { 0x18, 0x83 }, // (Offset,Value)-Pair #7
        { 0x19, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x1A, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetOrientation+0x1F : neg ecx
        { 0x1F, 0xF7 }, // (Offset,Value)-Pair #10
        { 0x20, 0xD9 }, // (Offset,Value)-Pair #11

        // IDirectSound8_SetOrientation+0x47 : retn 0x20
        { 0x47, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x48, 0x20 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CDirectSound::CommitDeferredSettingsB
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettingsB, 3936, 11,

    XREF_COMMITDEFERREDSETTINGSB,
    XRefZero)

        // CDirectSound_CommitDeferredSettingsB+0x56 : fld dword ptr [eax+0x174]
        { 0x56, 0xD9 }, // (Offset,Value)-Pair #1
        { 0x57, 0x80 }, // (Offset,Value)-Pair #2
        { 0x58, 0x74 }, // (Offset,Value)-Pair #3
        { 0x59, 0x01 }, // (Offset,Value)-Pair #4
        { 0x5A, 0x00 }, // (Offset,Value)-Pair #5
        { 0x5B, 0x00 }, // (Offset,Value)-Pair #6

        // CDirectSound_CommitDeferredSettingsB+0xA8 : fsubp st(1), st
        { 0xA8, 0xDE }, // (Offset,Value)-Pair #7
        { 0xA9, 0xE9 }, // (Offset,Value)-Pair #8

        // CDirectSound_CommitDeferredSettingsB+0xD4 : mov esi, 0x118
        { 0xD4, 0xBE }, // (Offset,Value)-Pair #9
        { 0xD5, 0x18 }, // (Offset,Value)-Pair #10
        { 0xD6, 0x01 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::CommitDeferredSettingsA
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettingsA, 3936, 10,

    XREF_COMMITDEFERREDSETTINGSA,
    XRefOne)

        // CDirectSound_CommitDeferredSettingsA+0x12 : call [CDirectSound::CommitDeferredSettingsB]
        { 0x12, XREF_COMMITDEFERREDSETTINGSB }, // (Offset,Value)-Pair #1

        // CDirectSound_CommitDeferredSettingsA+0x0C : mov ecx, [eax+0x0C]
        { 0x0C, 0x8B }, // (Offset,Value)-Pair #2
        { 0x0D, 0x48 }, // (Offset,Value)-Pair #3
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #4

        // CDirectSound_CommitDeferredSettingsA+0x0F : push 0
        { 0x0F, 0x6A }, // (Offset,Value)-Pair #5
        { 0x10, 0x00 }, // (Offset,Value)-Pair #6

        // CDirectSound_CommitDeferredSettingsA+0x19 : jz +0x0B
        { 0x19, 0x74 }, // (Offset,Value)-Pair #7
        { 0x1A, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSound_CommitDeferredSettingsA+0x28 : retn 0x04
        { 0x28, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x29, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSound_CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSound_CommitDeferredSettings+0x11 : call [CDirectSound::CommitDeferredSettingsA]
        { 0x11, XREF_COMMITDEFERREDSETTINGSA },  // (Offset,Value)-Pair #1

        // CDirectSound_CommitDeferredSettings+0x06 : and eax, 0xFFFFFFF8
        { 0x06, 0x83 }, // (Offset,Value)-Pair #2
        { 0x07, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x08, 0xF8 }, // (Offset,Value)-Pair #4

        // CDirectSound_CommitDeferredSettings+0x09 : neg ecx
        { 0x09, 0xF7 }, // (Offset,Value)-Pair #5
        { 0x0A, 0xD9 }, // (Offset,Value)-Pair #6

        // CDirectSound_CommitDeferredSettings+0x15 : retn 0x04
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #7
        { 0x16, 0x04 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * IDirectSound8_Release
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_Release, 3936, 10)

        // IDirectSound8_Release+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_Release+0x07 : neg eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5

        // IDirectSound8_Release+0x10 : call dword ptr [ecx+8]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #6
        { 0x11, 0x51 }, // (Offset,Value)-Pair #7
        { 0x12, 0x08 }, // (Offset,Value)-Pair #8

        // IDirectSound8_Release+0x13 : retn 0x04
        { 0x13, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x14, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactorB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactorB, 3936, 13,

    XREF_SETDISTANCEFACTORB,
    XRefZero)

        // CDirectSound_SetDistanceFactorB+0x04 : or dword ptr [ecx+0x01B4], 0x60
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x07, 0x01 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x60 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetDistanceFactorB+0x0B : cmp [esp+0x08], 0
        { 0x0B, 0x83 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x7C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #11

        // CDirectSound_SetDistanceFactorB+0x12 : mov [ecx+0x0178], eax
        { 0x12, 0x78 }, // (Offset,Value)-Pair #12
        { 0x13, 0x01 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDistanceFactorA, 3936, 12,

    XREF_SETDISTANCEFACTORA,
    XRefOne)

        // CDirectSound_SetDistanceFactorA+0x23 : call [CDirectSound::SetDistanceFactorB]
        { 0x23, XREF_SETDISTANCEFACTORB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetDistanceFactorA+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #2
        { 0x01, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetDistanceFactorA+0x07 : fld [esp+0x10]
        { 0x07, 0xD9 }, // (Offset,Value)-Pair #4
        { 0x08, 0x44 }, // (Offset,Value)-Pair #5
        { 0x09, 0x24 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetDistanceFactorA+0x11 : not eax; and eax, 1
        { 0x11, 0xF7 }, // (Offset,Value)-Pair #8
        { 0x12, 0xD0 }, // (Offset,Value)-Pair #9
        { 0x13, 0x83 }, // (Offset,Value)-Pair #10
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #11
        { 0x15, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetDistanceFactor, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative (may not be SetDistanceFactor)

        // IDirectSound8_SetDistanceFactor+0x1D : call [CDirectSound::SetDistanceFactorA]
        { 0x1D, XREF_SETDISTANCEFACTORA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetDistanceFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetDistanceFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetDistanceFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetDistanceFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetRolloffFactorB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactorB, 3936, 13,

    XREF_SETROLLOFFFACTORB,
    XRefZero)

        // CDirectSound_SetRolloffFactorB+0x04 : or dword ptr [ecx+0x01B4], 0x60
        { 0x04, 0x83 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0xB4 }, // (Offset,Value)-Pair #3
        { 0x07, 0x01 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x04 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetRolloffFactorB+0x0B : cmp [esp+0x08], 0
        { 0x0B, 0x83 }, // (Offset,Value)-Pair #8
        { 0x0C, 0x7C }, // (Offset,Value)-Pair #9
        { 0x0D, 0x24 }, // (Offset,Value)-Pair #10
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #11

        // CDirectSound_SetRolloffFactorB+0x12 : mov [ecx+0x017C], eax
        { 0x12, 0x7C }, // (Offset,Value)-Pair #12
        { 0x13, 0x01 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetRolloffFactorA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactorA, 3936, 12,

    XREF_SETROLLOFFFACTORA,
    XRefOne)

        // CDirectSound_SetRolloffFactorA+0x23 : call [CDirectSound::SetRolloffFactorB]
        { 0x23, XREF_SETROLLOFFFACTORB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetRolloffFactorA+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #2
        { 0x01, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetRolloffFactorA+0x07 : fld [esp+0x10]
        { 0x07, 0xD9 }, // (Offset,Value)-Pair #4
        { 0x08, 0x44 }, // (Offset,Value)-Pair #5
        { 0x09, 0x24 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x10 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetRolloffFactorA+0x11 : not eax; and eax, 1
        { 0x11, 0xF7 }, // (Offset,Value)-Pair #8
        { 0x12, 0xD0 }, // (Offset,Value)-Pair #9
        { 0x13, 0x83 }, // (Offset,Value)-Pair #10
        { 0x14, 0xE0 }, // (Offset,Value)-Pair #11
        { 0x15, 0x01 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetRolloffFactor, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

    // TODO: Tentative (may not be SetRolloffFactor)

        // IDirectSound8_SetRolloffFactor+0x1D : call [CDirectSound::SetRolloffFactorA]
        { 0x1D, XREF_SETROLLOFFFACTORA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetRolloffFactor+0x04 : fld [esp+0x0C]
        { 0x04, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x05, 0x44 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetRolloffFactor+0x0C : push ecx
        { 0x0C, 0x51 }, // (Offset,Value)-Pair #6

        // IDirectSound8_SetRolloffFactor+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 }, // (Offset,Value)-Pair #7
        { 0x13, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x14, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetRolloffFactor+0x21 : retn 0x0C
        { 0x21, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x22, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetMixBinHeadroomB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetMixBinHeadroomB, 3936, 12,

    XREF_DSSETMIXBINHEADROOMB,
    XRefZero)

        // CDirectSound_SetMixBinHeadroomB+0x04 : cmp dword ptr ds:[0xFE820010], 4
        { 0x12, 0x83 }, // (Offset,Value)-Pair #1
        { 0x13, 0x3D }, // (Offset,Value)-Pair #2
        { 0x14, 0x10 }, // (Offset,Value)-Pair #3
        { 0x15, 0x00 }, // (Offset,Value)-Pair #4
        { 0x16, 0x82 }, // (Offset,Value)-Pair #5
        { 0x17, 0xFE }, // (Offset,Value)-Pair #6
        { 0x18, 0x04 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetMixBinHeadroomB+0x1F : and edx, 7
        { 0x1F, 0x83 }, // (Offset,Value)-Pair #8
        { 0x20, 0xE2 }, // (Offset,Value)-Pair #9
        { 0x21, 0x07 }, // (Offset,Value)-Pair #10

        // CDirectSound_SetMixBinHeadroomB+0x2D : jl +0xD8
        { 0x2D, 0x7C }, // (Offset,Value)-Pair #11
        { 0x2E, 0xD8 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetMixBinHeadroomA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetMixBinHeadroomA, 3936, 9,

    XREF_DSSETMIXBINHEADROOMA,
    XRefOne)

        // CDirectSound_SetMixBinHeadroomA+0x19 : call [CDirectSound::SetMixBinHeadroomB]
        { 0x19, XREF_DSSETMIXBINHEADROOMB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetMixBinHeadroomA+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #2
        { 0x01, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetMixBinHeadroomA+0x07 : push [esp+0x14]
        { 0x07, 0xFF }, // (Offset,Value)-Pair #4
        { 0x08, 0x74 }, // (Offset,Value)-Pair #5
        { 0x09, 0x24 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x14 }, // (Offset,Value)-Pair #7

        // CDirectSound_SetMixBinHeadroomA+0x21 : jz +0x0B
        { 0x21, 0x74 }, // (Offset,Value)-Pair #8
        { 0x22, 0x0B }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetMixBinHeadroom
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetMixBinHeadroom, 3936, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetMixBinHeadroom+0x19 : call [CDirectSound::SetMixBinHeadroomA]
        { 0x19, XREF_DSSETMIXBINHEADROOMA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetMixBinHeadroom+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetMixBinHeadroom+0x08 : mov ecx, eax
        { 0x08, 0x8B }, // (Offset,Value)-Pair #6
        { 0x09, 0xC8 }, // (Offset,Value)-Pair #7

        // IDirectSound8_SetMixBinHeadroom+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #8
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #9
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #10

        // IDirectSound8_SetMixBinHeadroom+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetPositionB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPositionB, 3936, 12,

    XREF_DSSETPOSITIONB,
    XRefZero)

        // CDirectSound_SetPositionB+0x0C : movsd; movsd; movsd
        { 0x0C, 0xA5 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xA5 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xA5 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetPositionB+0x0F : or byte ptr [ecx+0x01B4], 0xFF
        { 0x0F, 0x80 }, // (Offset,Value)-Pair #4
        { 0x10, 0x89 }, // (Offset,Value)-Pair #5
        { 0x11, 0xB4 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7
        { 0x13, 0x00 }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9
        { 0x15, 0xFF }, // (Offset,Value)-Pair #10

        // CDirectSound_SetPositionB+0x28 : retn 0x08
        { 0x28, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x29, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetPositionA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetPositionA, 3936, 10,

    XREF_DSSETPOSITIONA,
    XRefOne)

        // CDirectSound_SetPositionA+0x35 : call [CDirectSound::SetPositionB]
        { 0x35, XREF_DSSETPOSITIONB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetPositionA+0x06 : push esi; push edi
        { 0x06, 0x56 }, // (Offset,Value)-Pair #2
        { 0x07, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetPositionA+0x24 : not eax; and eax, 1
        { 0x24, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x25, 0xD0 }, // (Offset,Value)-Pair #5
        { 0x26, 0x83 }, // (Offset,Value)-Pair #6
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #7
        { 0x28, 0x01 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetPositionA+0x4F : retn 0x14
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x50, 0x14 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetPosition
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetPosition, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetPosition+0x2D : call [CDirectSound::SetPositionA]
        { 0x2D, XREF_DSSETPOSITIONA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetPosition+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // IDirectSound8_SetPosition+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #6
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #7

        // IDirectSound8_SetPosition+0x1B : neg ecx
        { 0x1B, 0xF7 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xD9 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetPosition+0x32 : retn 0x14
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x33, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetVelocityB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocityB, 3936, 12,

    XREF_DSSETVELOCITYB,
    XRefZero)

        // CDirectSound_SetVelocityB+0x0C : movsd; movsd; movsd
        { 0x0C, 0xA5 }, // (Offset,Value)-Pair #1
        { 0x0D, 0xA5 }, // (Offset,Value)-Pair #2
        { 0x0E, 0xA5 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetVelocityB+0x0F : or dword ptr [ecx+0x01B4], 0x40
        { 0x0F, 0x83 }, // (Offset,Value)-Pair #4
        { 0x10, 0x89 }, // (Offset,Value)-Pair #5
        { 0x11, 0xB4 }, // (Offset,Value)-Pair #6
        { 0x12, 0x01 }, // (Offset,Value)-Pair #7
        { 0x13, 0x00 }, // (Offset,Value)-Pair #8
        { 0x14, 0x00 }, // (Offset,Value)-Pair #9
        { 0x15, 0x40 }, // (Offset,Value)-Pair #10

        // CDirectSound_SetVelocityB+0x28 : retn 0x08
        { 0x28, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x29, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetVelocityA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetVelocityA, 3936, 10,

    XREF_DSSETVELOCITYA,
    XRefOne)

        // CDirectSound_SetVelocityA+0x35 : call [CDirectSound::SetVelocityB]
        { 0x35, XREF_DSSETVELOCITYB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetVelocityA+0x06 : push esi; push edi
        { 0x06, 0x56 }, // (Offset,Value)-Pair #2
        { 0x07, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetVelocityA+0x24 : not eax; and eax, 1
        { 0x24, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x25, 0xD0 }, // (Offset,Value)-Pair #5
        { 0x26, 0x83 }, // (Offset,Value)-Pair #6
        { 0x27, 0xE0 }, // (Offset,Value)-Pair #7
        { 0x28, 0x01 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetVelocityA+0x4F : retn 0x14
        { 0x4F, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x50, 0x14 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetVelocity
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetVelocity, 3936, 11,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetVelocity+0x2D : call [CDirectSound::SetVelocityA]
        { 0x2D, XREF_DSSETVELOCITYA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetVelocity+0x06 : fld [ebp+0x14]
        { 0x06, 0xD9 }, // (Offset,Value)-Pair #2
        { 0x07, 0x45 }, // (Offset,Value)-Pair #3
        { 0x08, 0x14 }, // (Offset,Value)-Pair #4

        // IDirectSound8_SetVelocity+0x0C : sub esp, 0x0C
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #5
        { 0x0D, 0xEC }, // (Offset,Value)-Pair #6
        { 0x0E, 0x0C }, // (Offset,Value)-Pair #7

        // IDirectSound8_SetVelocity+0x1B : neg ecx
        { 0x1B, 0xF7 }, // (Offset,Value)-Pair #8
        { 0x1C, 0xD9 }, // (Offset,Value)-Pair #9

        // IDirectSound8_SetVelocity+0x32 : retn 0x14
        { 0x32, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x33, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParametersB
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersB, 3936, 13,

    XREF_DSSETALLPARAMETERSB,
    XRefZero)

        // CDirectSound_SetAllParametersB+0x08 : push 0x0C; pop ecx
        { 0x08, 0x6A }, // (Offset,Value)-Pair #1
        { 0x09, 0x0C }, // (Offset,Value)-Pair #2
        { 0x0A, 0x59 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetAllParametersB+0x13 : or word ptr [ecx+0x01B4], 0x0180
        { 0x13, 0x66 }, // (Offset,Value)-Pair #4
        { 0x14, 0x81 }, // (Offset,Value)-Pair #5
        { 0x15, 0x88 }, // (Offset,Value)-Pair #6
        { 0x16, 0xB4 }, // (Offset,Value)-Pair #7
        { 0x17, 0x01 }, // (Offset,Value)-Pair #8
        { 0x18, 0x00 }, // (Offset,Value)-Pair #9
        { 0x1A, 0x80 }, // (Offset,Value)-Pair #10
        { 0x1B, 0x01 }, // (Offset,Value)-Pair #11

        // CDirectSound_SetAllParametersB+0x30 : retn 0x08
        { 0x30, 0xC2 }, // (Offset,Value)-Pair #12
        { 0x31, 0x08 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 3936, 10,

    XREF_DSSETALLPARAMETERSA,
    XRefOne)

        // CDirectSound_SetAllParametersA+0x2F : call [CDirectSound::SetAllParametersB]
        { 0x2F, XREF_DSSETALLPARAMETERSB }, // (Offset,Value)-Pair #1

        // CDirectSound_SetAllParametersA+0x00 : push esi; push edi
        { 0x00, 0x56 }, // (Offset,Value)-Pair #2
        { 0x01, 0x57 }, // (Offset,Value)-Pair #3

        // CDirectSound_SetAllParametersA+0x21 : not ecx; and ecx, 1
        { 0x21, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x22, 0xD1 }, // (Offset,Value)-Pair #5
        { 0x23, 0x83 }, // (Offset,Value)-Pair #6
        { 0x24, 0xE1 }, // (Offset,Value)-Pair #7
        { 0x25, 0x01 }, // (Offset,Value)-Pair #8

        // CDirectSound_SetAllParametersA+0x48 : retn 0x0C
        { 0x48, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x49, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSound8_SetAllParameters
// ******************************************************************
OOVPA_XREF(IDirectSound8_SetAllParameters, 3936, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound8_SetAllParameters+0x19 : call [CDirectSound::SetAllParametersA]
        { 0x19, XREF_DSSETALLPARAMETERSA },  // (Offset,Value)-Pair #1

        // IDirectSound8_SetAllParameters+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSound8_SetAllParameters+0x0E : add eax, 0xFFFFFFF8
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #8

        // IDirectSound8_SetAllParameters+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #9
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #10

        // IDirectSound8_SetAllParameters+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * IDirectSound8_DownloadEffectsImage
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_DownloadEffectsImage, 3936, 11)

        // IDirectSound8_DownloadEffectsImage+0x03 : push [ebp+0x18]
        { 0x03, 0xFF }, // (Offset,Value)-Pair #1
        { 0x04, 0x75 }, // (Offset,Value)-Pair #2
        { 0x05, 0x18 }, // (Offset,Value)-Pair #3

        // IDirectSound8_DownloadEffectsImage+0x06 : mov eax, [ebp+0x08]
        { 0x06, 0x8B }, // (Offset,Value)-Pair #4
        { 0x07, 0x45 }, // (Offset,Value)-Pair #5
        { 0x08, 0x08 }, // (Offset,Value)-Pair #6

        // IDirectSound8_DownloadEffectsImage+0x11 : add eax, 0xFFFFFFF8
        { 0x11, 0x83 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC0 }, // (Offset,Value)-Pair #8
        { 0x13, 0xF8 }, // (Offset,Value)-Pair #9

        // IDirectSound8_DownloadEffectsImage+0x24 : retn 0x14
        { 0x24, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x25, 0x14 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_SetMode
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMode, 3936, 7,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMode+0x10 : call [CDirectSoundVoice::SetMode]
        { 0x11, XREF_DSBUFFERSETMODEA }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetMode+0x0C : add eax, 4
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0E, 0x04 }, // (Offset,Value)-Pair #4

        // CDirectSoundStream_SetMode+0x0F : push eax
        { 0x0F, 0x50 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer8_SetMode+0x15 : retn 0x0C
        { 0x15, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x16, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
OOVPA_XREF(DirectSound_CMcpxBuffer_Play, 3925, 8,

    XREF_DirectSound_CMcpxBuffer_Play,
    XRefZero)

        { 0x06, 0x56 },
        { 0x0E, 0xF1 },
        { 0x16, 0xFF },
        { 0x1D, 0x03 },
        { 0x1E, 0xFF },
        { 0x26, 0x75 },
        { 0x2F, 0x8B },
        { 0x36, 0xC2 },
OOVPA_END;

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

        { 0x23, XREF_CMcpxAPU_Set3dDopplerFactor },

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

        { 0x1D, XREF_SETDOPPLERFACTOR },

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
OOVPA_XREF(CSensaura3d_GetFullHRTFFilterPair, 3936, 7,

    XREF_CSensaura3d_GetFullHRTFFilterPair,
    XRefZero)

        { 0x1E, 0xEB },
        { 0x3E, 0x40 },
        { 0x5E, 0x7E },
        { 0x7E, 0x1E },
        { 0xA2, 0xD9 },
        { 0xBE, 0x09 },
        { 0xDE, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x0D, XREF_CSensaura3d_GetFullHRTFFilterPair },

        { 0x05, 0x85 },
        { 0x06, 0xC0 },
        { 0x11, 0x74 },
        { 0x12, 0x0B },
        { 0x13, 0x68 },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseFullHRTF2
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF2, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x01, XREF_DirectSoundEnterCriticalSection },

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

        { 0x15, XREF_CDirectSound_GetCaps },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

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

// ******************************************************************
// * IDirectSoundBuffer_SetPitch
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 3936, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x15, XREF_CDirectSoundVoice_SetPitch },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

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

        { 0x1D, XREF_DSVOICESETCONEANGLES },

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

    XREF_DirectSound_CDirectSoundBuffer_PlayEx,
    XRefOne)

        { 0x03, XREF_DirectSoundEnterCriticalSection },

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

        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_PlayEx },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSound8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_AddRef, 3936, 12)

        // IDirectSound8_AddRef+0x04 : lea ecx, [eax-8]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xF8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_AddRef+0x07 : neg eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5

        // IDirectSound8_AddRef+0x09 : sbb eax, eax
        { 0x09, 0x1B }, // (Offset,Value)-Pair #6
        { 0x0A, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirectSound8_AddRef+0x0D : mov ecx, [eax]
        { 0x0D, 0x8B }, // (Offset,Value)-Pair #8
        { 0x0E, 0x08 }, // (Offset,Value)-Pair #9

        // IDirectSound8_AddRef+0x10 : call dword ptr [ecx+4]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #10
        { 0x11, 0x51 }, // (Offset,Value)-Pair #11
        { 0x12, 0x04 }, // (Offset,Value)-Pair #12
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
// * IDirectSoundBuffer8_AddRef
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer8_AddRef, 3936, 10)

        // IDirectSoundBuffer8_AddRef+0x04 : lea ecx, [eax-0x1C]
        { 0x04, 0x8D }, // (Offset,Value)-Pair #1
        { 0x05, 0x48 }, // (Offset,Value)-Pair #2
        { 0x06, 0xE4 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_AddRef+0x07 : neg eax; sbb eax, eax
        { 0x07, 0xF7 }, // (Offset,Value)-Pair #4
        { 0x08, 0xD8 }, // (Offset,Value)-Pair #5
        { 0x09, 0x1B }, // (Offset,Value)-Pair #6
        { 0x0A, 0xC0 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_AddRef+0x10 : call dword ptr [ecx+0x04]
        { 0x10, 0xFF }, // (Offset,Value)-Pair #8
        { 0x11, 0x51 }, // (Offset,Value)-Pair #9
        { 0x12, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 3936, 9,

    XREF_CDirectSoundBuffer_SetMinDistance,
    XRefOne)

        { 0x11, XREF_DSSTREAMSETMINDISTANCE1B },

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

        { 0x1D, XREF_CDirectSoundBuffer_SetMinDistance },

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

        { 0x11, XREF_DSSTREAMSETMAXDISTANCE1B },

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

        { 0x1D, XREF_CDirectSoundBuffer_SetMaxDistance },

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

        { 0x1F, XREF_CMcpxVoiceClient_SetI3DL2Source },

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

        { 0x19, XREF_CDirectSoundVoice_SetI3DL2Source },

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

        { 0x19, XREF_CDirectSoundVoice_SetMixBinVolumes },

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

        { 0x1F, XREF_DSSETALLPARAMETERSB },

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

        { 0x19, XREF_DSSETALLPARAMETERSA },

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

        { 0x19, XREF_DSSETI3DL2LISTENER },

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

    XREF_DirectSound_CDirectSound_EnableHeadphones,
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

        { 0x15, XREF_DirectSound_CDirectSound_EnableHeadphones },

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

        { 0x11, XREF_CDirectSoundVoice_SetMixBinVolumes },

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

        { 0x11, XREF_CDirectSoundVoice_SetI3DL2Source },

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

        { 0x19, XREF_CDirectSoundBuffer_SetNotificationPositions },

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

// ******************************************************************
// * DirectSoundCreate, 3936
// ******************************************************************
OOVPATable DSound_3936[] = {

    // DirectSoundCreate
	OOVPA_TABLE_ENTRY(DirectSoundCreate, 3936),
    // DirectSoundDoWorkB (XREF)
	OOVPA_TABLE_XREF(DirectSoundDoWorkB, 3936),
    // DirectSoundDoWorkA (XREF)
	OOVPA_TABLE_XREF(DirectSoundDoWorkA, 3936),
	// DirectSoundDoWork
	OOVPA_TABLE_ENTRY(DirectSoundDoWork, 3936),
    // CDirectSound_CreateSoundStream
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundStream, 3936),
	// IDirectSound8_CreateStream
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateStream, 3936),
    // CDirectSound_CreateSoundBuffer
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundBuffer, 3936),
	// IDirectSound8_CreateBuffer
	OOVPA_TABLE_ENTRY(IDirectSound8_CreateBuffer, 3936),
	// IDirectSoundBuffer8::Release
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Release, 3936),
    // IDirectSoundBuffer8::SetPitchB (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetPitchB, 3936),
    // IDirectSoundBuffer8::SetPitchA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetPitchA, 3936),
	// IDirectSoundBuffer8::SetPitch
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPitch, 3936),
    // CMcpxBuffer::GetStatus (XREF)
	OOVPA_TABLE_XREF(CMcpxBuffer_GetStatus, 3936),
    // CDirectSoundBuffer::GetStatus (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_GetStatus, 3936),
	// IDirectSoundBuffer8::GetStatus
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_GetStatus, 3936),
    // IDirectSoundBuffer8::SetVolumeB (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetVolumeB, 3936),
    // IDirectSoundBuffer8::SetVolumeA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetVolumeA, 3936),
	// IDirectSoundBuffer8::SetVolume
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVolume, 3936),
    // IDirectSoundBuffer8::SetCurrentPositionB (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetCurrentPositionB, 3936),
    // IDirectSoundBuffer8::SetCurrentPositionA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetCurrentPositionA, 3936),
	// IDirectSoundBuffer8::SetCurrentPosition
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetCurrentPosition, 3936),
    // IDirectSoundBuffer8::SetPlayRegionA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetPlayRegionA, 3936),
	// IDirectSoundBuffer8::SetPlayRegion
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPlayRegion, 3936),
    // IDirectSoundBuffer8::LockA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_LockA, 3936),
	// IDirectSoundBuffer8::Lock
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Lock, 3936),
    // IDirectSoundBuffer8::SetHeadroomA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetHeadroomA, 3936),
	// IDirectSoundBuffer8::SetHeadroom
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetHeadroom, 3936),
    // IDirectSoundBuffer8::SetBufferDataA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundBuffer8_SetBufferDataA, 3936),
	// IDirectSoundBuffer8::SetBufferData
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetBufferData, 3936),
    // CMcpxVoiceClient::SetMixBins (XRef)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetMixBins, 3936),
    // CDirectSoundVoice::SetMixBins (XRef)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBins, 3936),
	// IDirectSoundStream::SetMixBins
	OOVPA_TABLE_PATCH(IDirectSoundStream_SetMixBins, 3936, CDirectSoundStream_SetMixBins),
	// IDirectSoundBuffer8::SetMixBins
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMixBins, 3936),
    // CMcpxBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(CMcpxBuffer_GetCurrentPosition, 3936),
	// CMcpxBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(CMcpxBuffer_GetCurrentPosition2, 3936),
    // CDirectSoundBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(CDirectSoundBuffer_GetCurrentPosition, 3936),
	// IDirectSoundBuffer8::GetCurrentPosition
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_GetCurrentPosition, 3936),
	// CDirectSoundBuffer_Play
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Play, 3936),
	// IDirectSoundBuffer8::Play
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Play, 3936),
	// IDirectSoundBuffer8::Play
   /* {
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_Play2, 3936, IDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
		"EmuIDirectSoundBuffer8_Play [YESSSS! IT WORKED!]"
        #endif
    },*/
    // CDirectSoundBuffer::Stop
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Stop, 3936),
	// IDirectSoundBuffer8::Stop
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_Stop, 3936),
    // CMcpxVoiceClient_SetVolume
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetVolume, 3936),
	// CDirectSoundStream_SetVolume
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetVolume, 3936),
    // CDirectSoundStream::SetConeAnglesB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeAnglesB, 3936),
    // CDirectSoundStream::SetConeAnglesA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeAnglesA, 3936),
	// CDirectSoundStream::SetConeAngles
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetConeAngles, 3936),
    // CDirectSoundStream::SetConeOutsideVolumeB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeOutsideVolumeB, 3936),
    // CDirectSoundStream::SetConeOutsideVolumeA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeOutsideVolumeA, 3936),
	// CDirectSoundStream::SetConeOutsideVolume
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetConeOutsideVolume, 3936),
    //// CDirectSoundStream::SetAllParametersB (XREF)
    //OOVPA_TABLE_XREF(CDirectSoundStream_SetAllParametersB, 3936),
    //// CDirectSoundStream::SetAllParametersA (XREF)
    //OOVPA_TABLE_XREFCDirectSoundStream_SetAllParametersA, 3936),
	//// CDirectSoundStream::SetAllParameters
    //OOVPA_TABLE_ENTRY(CDirectSoundStream_SetAllParameters, 3936),
    // CDirectSoundStream::SetMaxDistanceC (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMaxDistanceC, 3936),
    // CDirectSoundStream::SetMaxDistanceB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMaxDistanceB, 3936),
    // CDirectSoundStream::SetMaxDistanceA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMaxDistanceA, 3936),
	// CDirectSoundStream::SetMaxDistance
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetMaxDistance, 3936),
    // CDirectSoundStream::SetMinDistanceC (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMinDistanceC, 3936),
    // CDirectSoundStream::SetMinDistanceB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMinDistanceB, 3936),
    // CDirectSoundStream::SetMinDistanceA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetMinDistanceA, 3936),
	// CDirectSoundStream::SetMinDistance
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetMinDistance, 3936),
    // CDirectSoundStream::SetVelocityC (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetVelocityC, 3936),
    // CDirectSoundStream::SetVelocityB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetVelocityB, 3936),
    // CDirectSoundStream::SetVelocityA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetVelocityA, 3936),
	// CDirectSoundStream::SetVelocity
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetVelocity, 3936),
    // CDirectSoundBuffer::SetVelocity (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetVelocity, 3936),
	// IDirectSoundBuffer8_SetVelocity
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetVelocity, 3936),
    // CDirectSoundStream::SetConeOrientationC (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeOrientationC, 3936),
    // CDirectSoundStream::SetConeOrientationB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeOrientationB, 3936),
    // CDirectSoundStream::SetConeOrientationA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetConeOrientationA, 3936),
	// CDirectSoundStream::SetConeOrientation
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetConeOrientation, 3936),
    // CDirectSoundStream::SetPositionC (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetPositionC, 3936),
    // CDirectSoundStream::SetPositionB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetPositionB, 3936),
    // CDirectSoundStream::SetPositionA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetPositionA, 3936),
	// CDirectSoundStream::SetPosition
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetPosition, 3936),
    // CDirectSoundBuffer::SetPosition (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetPosition, 3936),
	// IDirectSoundBuffer8_SetPosition
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetPosition, 3936),
    // CDirectSoundStream::SetFrequencyB (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetFrequencyB, 3936),
    // CDirectSoundStream::SetFrequencyA (XREF)
	OOVPA_TABLE_XREF(CDirectSoundStream_SetFrequencyA, 3936),
	// CDirectSoundStream::SetFrequency
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetFrequency, 3936),
	// IDirectSoundBuffer8::SetFrequency
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetFrequency, 3936),
    // CMcpxVoiceClient::Set3dMode (XREF)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_Set3dMode, 3936),
    // CDirectSoundVoice::SetMode (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMode, 3936),
	// IDirectSoundBuffer8::SetMode
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_SetMode, 3936),
    // IDirectSoundStream::SetI3DL2SourceA (XREF)
    /*
	OOVPA_TABLE_XREF(IDirectSoundStream_SetI3DL2SourceA, 3936),
	// IDirectSoundStream::SetI3DL2Source
	OOVPA_TABLE_ENTRY(IDirectSoundStream_SetI3DL2Source, 3936),
	*/
    /*
    // IDirectSoundStream::Unknown1
	OOVPA_TABLE_ENTRY(IDirectSoundStream_Unknown1, 3936),
	*/
    // IDirectSoundStream::SetHeadroomA (XREF)
	OOVPA_TABLE_XREF(IDirectSoundStream_SetHeadroomA, 3936),
	// IDirectSoundStream::SetHeadroom
	OOVPA_TABLE_ENTRY(IDirectSoundStream_SetHeadroom, 3936),
		// IDirectSound8::SetOrientation
	OOVPA_TABLE_ENTRY(IDirectSound8_SetOrientation, 3936),
    // CDirectSound::CommitDeferredSettingsB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_CommitDeferredSettingsB, 3936),
    // CDirectSound::CommitDeferredSettingsA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_CommitDeferredSettingsA, 3936),
	// CDirectSound::CommitDeferredSettings
	OOVPA_TABLE_ENTRY(CDirectSound_CommitDeferredSettings, 3936),
	// IDirectSound8::Release
	OOVPA_TABLE_ENTRY(IDirectSound8_Release, 3936),
    // CDirectSound::SetDistanceFactorB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorB, 3936),
    // CDirectSound::SetDistanceFactorA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetDistanceFactorA, 3936),
	// IDirectSound8::SetDistanceFactor
	OOVPA_TABLE_ENTRY(IDirectSound8_SetDistanceFactor, 3936),
    // CDirectSound::SetRolloffFactorB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetRolloffFactorB, 3936),
    // CDirectSound::SetRolloffFactorA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetRolloffFactorA, 3936),
	// IDirectSound8::SetRolloffFactor
	OOVPA_TABLE_ENTRY(IDirectSound8_SetRolloffFactor, 3936),
    // CDirectSound::SetMixBinHeadroomB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetMixBinHeadroomB, 3936),
    // CDirectSound::SetMixBinHeadroomA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetMixBinHeadroomA, 3936),
	// IDirectSound8::SetMixBinHeadroom
	OOVPA_TABLE_ENTRY(IDirectSound8_SetMixBinHeadroom, 3936),
    // CDirectSound::SetPositionB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetPositionB, 3936),
    // CDirectSound::SetPositionA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetPositionA, 3936),
	// IDirectSound8::SetPosition
	OOVPA_TABLE_ENTRY(IDirectSound8_SetPosition, 3936),
    // CDirectSound::SetVelocityB (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetVelocityB, 3936),
    // CDirectSound::SetVelocityA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetVelocityA, 3936),
	// IDirectSound8::SetVelocity
	OOVPA_TABLE_ENTRY(IDirectSound8_SetVelocity, 3936),
    // CDirectSound::SetAllParametersB (XREF)
	OOVPA_TABLE_XREF(CMcpxAPU_Set3dParameters, 3936),
    // CDirectSound::SetAllParametersA (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetAllParameters, 3936),
	// IDirectSound8::SetAllParameters
	OOVPA_TABLE_PATCH(IDirectSound_SetAllParameters, 3936, IDirectSound8_SetAllParameters),
	// IDirectSound8::DownloadEffectsImage
	OOVPA_TABLE_ENTRY(IDirectSound8_DownloadEffectsImage, 3936),
	// CDirectSoundStream::SetMode
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetMode, 3936),
	// DirectSound::CMcpxBuffer::Play (XREF)
    /*
	OOVPA_TABLE_XREF(DirectSound_CMcpxBuffer_Play, 4361),
	*/
	// CMcpxAPU::Set3dDopplerFactor (XREF)
	OOVPA_TABLE_XREF(CMcpxAPU_Set3dDopplerFactor, 3936),
	// CDirectSound::SetDopplerFactor (XREF)
	OOVPA_TABLE_XREF(CDirectSound_SetDopplerFactor, 3936),
	// IDirectSound_SetDopplerFactor
	OOVPA_TABLE_PATCH(IDirectSound_SetDopplerFactor, 3936, IDirectSound8_SetDopplerFactor),
	// CSensaura3d::GetFullHRTFFilterPair (XREF)
	OOVPA_TABLE_XREF(CSensaura3d_GetFullHRTFFilterPair, 3936),
	// DirectSoundUseFullHRTF
	OOVPA_TABLE_ENTRY(DirectSoundUseFullHRTF, 3936),
	// CDirectSound::GetCaps (XREF)
	OOVPA_TABLE_XREF(CDirectSound_GetCaps, 3936),
	// IDirectSound_GetCaps
	OOVPA_TABLE_PATCH(IDirectSound_GetCaps, 3936, IDirectSound8_GetCaps),
	// CDirectSoundVoice::SetConeAngles (XREF)
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetConeAngles, 3936),
	// IDirectSoundBuffer_SetConeAngles
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetConeAngles, 3936, IDirectSoundBuffer8_SetConeAngles),
	// DirectSoundEnterCriticalSection (XREF)
	OOVPA_TABLE_XREF(DirectSoundEnterCriticalSection, 3936),
	// DirectSoundUseFullHRTF
	OOVPA_TABLE_PATCH(DirectSoundUseFullHRTF2, 3936, DirectSoundUseFullHRTF),
	// DirectSound::CDirectSoundBuffer::PlayEx (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_PlayEx, 3936),
	// IDirectSoundBuffer_PlayEx
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_PlayEx, 3936, IDirectSoundBuffer8_PlayEx),
	// IDirectSound8::AddRef
	OOVPA_TABLE_ENTRY(IDirectSound8_AddRef, 3936),
	// DirectSoundGetSampleTime
	OOVPA_TABLE_ENTRY(DirectSoundGetSampleTime, 3936),
	// IDirectSoundBuffer_AddRef
	OOVPA_TABLE_ENTRY(IDirectSoundBuffer8_AddRef, 3936),
	// CDirectSoundBuffer::SetMinDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMinDistance, 3936),
	// IDirectSoundBuffer_SetMinDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMinDistance, 3936, IDirectSoundBuffer8_SetMinDistance),
	// CDirectSoundBuffer::SetMaxDistance (XREF)
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetMaxDistance, 3936),
	// IDirectSoundBuffer_SetMaxDistance
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMaxDistance, 3936, IDirectSoundBuffer8_SetMaxDistance),
	// CMcpxVoiceClient::Commit3dSettings (XREF)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_Commit3dSettings, 3936),
	// CMcpxVoiceClient::SetI3DL2Source (XREF)
	OOVPA_TABLE_XREF(CMcpxVoiceClient_SetI3DL2Source, 3936),
	// CDirectSoundVoice::SetI3DL2Source (XREF)
	OOVPA_TABLE_PATCH(CDirectSoundVoice_SetI3DL2Source, 3936, CDirectSoundStream_SetI3DL2Source),
	// IDirectSoundBuffer_SetI3DL2Source
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetI3DL2Source, 3936, IDirectSoundBuffer8_SetI3DL2Source),
	// CDirectSoundVoice::SetMixBinVolumes
	OOVPA_TABLE_XREF(CDirectSoundVoice_SetMixBinVolumes, 3936),
	// IDirectSoundBuffer_SetMixBinVolumes
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetMixBinVolumes, 3936, IDirectSoundBuffer8_SetMixBinVolumes),
	// CDirectSound_SetI3DL2Listener
	OOVPA_TABLE_XREF(CDirectSound_SetI3DL2Listener, 3936),
	// IDirectSound8_SetI3DL2Listener
	OOVPA_TABLE_PATCH(IDirectSound_SetI3DL2Listener, 3936, IDirectSound8_SetI3DL2Listener),
	// DirectSound::CDirectSound::EnableHeadphones (XREF)
	OOVPA_TABLE_XREF(CDirectSound_EnableHeadphones, 3936),
	// EmuIDirectSound_EnableHeadphones
	OOVPA_TABLE_ENTRY(IDirectSound_EnableHeadphones, 3936),
	// CDirectSoundStream::SetMixBinVolumes
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetMixBinVolumes, 3936),
	// CDirectSoundStream::SetI3DL2Source
    /*
	OOVPA_TABLE_ENTRY(CDirectSoundStream_SetI3DL2Source, 3936),
    */
	// CDirectSoundBuffer_SetNotificationPositions
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetNotificationPositions, 3936),
	// IDirectSoundBuffer_SetNotificationPositions
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_SetNotificationPositions, 3936, IDirectSoundBuffer8_SetNotificationPositions),
	// CDirectSoundStream::Pause
	OOVPA_TABLE_ENTRY(CDirectSoundStream_Pause, 3936),
};

// ******************************************************************
// * DSound_3936_SIZE
// ******************************************************************
uint32 DSound_3936_SIZE = sizeof(DSound_3936);
