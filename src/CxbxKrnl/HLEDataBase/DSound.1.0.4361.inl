// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4361.cpp
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
OOVPA_NO_XREF(DirectSoundCreate_1_0_4361, 9)

        // DirectSoundCreate+0x0B : movzx esi, al
        { 0x0B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xF0 }, // (Offset,Value)-Pair #3

        // DirectSoundCreate+0x12 : call [addr]
        { 0x12, 0xE8 }, // (Offset,Value)-Pair #4

        // DirectSoundCreate+0x1B : jl +0x13
        { 0x1B, 0x7C }, // (Offset,Value)-Pair #5
        { 0x1C, 0x13 }, // (Offset,Value)-Pair #6

        // DirectSoundCreate+0x27 : sbb ecx, ecx
        { 0x27, 0x1B }, // (Offset,Value)-Pair #7
        { 0x28, 0xC9 }, // (Offset,Value)-Pair #8

        // DirectSoundCreate+0x43 : leave
        { 0x43, 0xC9 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * IDirectSound8_CreateSoundBuffer
// ******************************************************************
OOVPA_NO_XREF(IDirectSound8_CreateSoundBuffer_1_0_4361, 9)

        // IDirectSound8_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #2

        // IDirectSound8_CreateSoundBuffer+0x3C : call [addr]
        { 0x3C, 0xE8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_CreateSoundBuffer+0x4A : and esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #4
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #6
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #7
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundStream_1_0_4361, 14,

    XREF_DSCREATESOUNDSTREAM,
    XRefZero)

        // CDirectSound_CreateSoundStream+0x23 : mov eax, 0x80004005
        { 0x23, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x24, 0x05 }, // (Offset,Value)-Pair #2
        { 0x25, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x80 }, // (Offset,Value)-Pair #4

        // CDirectSound_CreateSoundStream+0x2A : push 0x28
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #5
        { 0x2B, 0x28 }, // (Offset,Value)-Pair #6

        // CDirectSound_CreateSoundStream+0x4A : add esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #7
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #8
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #10
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #11
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #12

        // CDirectSound_CreateSoundStream+0x8E : retn 0x10
        { 0x8E, 0xC2 }, // (Offset,Value)-Pair #13
        { 0x8F, 0x10 }, // (Offset,Value)-Pair #14
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
OOVPA_XREF(DirectSoundCreateStream_1_0_4361, 12,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundCreateStream+0x2F : call [CDirectSound::CreateSoundStream]
        { 0x2F, XREF_DSCREATESOUNDSTREAM }, // (Offset,Value)-Pair #1

        // DirectSoundCreateStream+0x04 : and [ebp-0x04], 0
        { 0x04, 0x83 }, // (Offset,Value)-Pair #2
        { 0x05, 0x65 }, // (Offset,Value)-Pair #3
        { 0x06, 0xFC }, // (Offset,Value)-Pair #4

        // DirectSoundCreateStream+0x08 : push ebx; push esi; push edi
        { 0x08, 0x53 }, // (Offset,Value)-Pair #5
        { 0x09, 0x56 }, // (Offset,Value)-Pair #6
        { 0x0A, 0x57 }, // (Offset,Value)-Pair #7

        // DirectSoundCreateStream+0x3C : call dword ptr [eax+8]
        { 0x3C, 0xFF }, // (Offset,Value)-Pair #8
        { 0x3D, 0x50 }, // (Offset,Value)-Pair #9
        { 0x3E, 0x08 }, // (Offset,Value)-Pair #10

        // DirectSoundCreateStream+0x54 : retn 0x08
        { 0x54, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x55, 0x08 }, // (Offset,Value)-Pair #12
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause_1_0_4361, 11,

    XREF_DSSTREAMPAUSE,
    XRefZero)

        // CMcpxStream_Pause+0x1E : or eax, 4
        { 0x1E, 0x83 }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC8 }, // (Offset,Value)-Pair #2
        { 0x20, 0x04 }, // (Offset,Value)-Pair #3

        // CMcpxStream_Pause+0x21 : jmp +0x0D
        { 0x21, 0xEB }, // (Offset,Value)-Pair #4
        { 0x22, 0x0D }, // (Offset,Value)-Pair #5

        // CMcpxStream_Pause+0x29 : movzx eax, word ptr [esi+0x12]
        { 0x29, 0x0F }, // (Offset,Value)-Pair #6
        { 0x2A, 0xB7 }, // (Offset,Value)-Pair #7
        { 0x2B, 0x46 }, // (Offset,Value)-Pair #8
        { 0x2C, 0x12 }, // (Offset,Value)-Pair #9

        // CMcpxStream_Pause+0x44 : retn 0x04
        { 0x44, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x45, 0x04 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Pause
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Pause_1_0_4361, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_Pause+0x35 : call [CMcpxStream::Pause]
        { 0x35, XREF_DSSTREAMPAUSE }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_Pause+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundStream_Pause+0x2C : mov ecx, [eax+0x24]
        { 0x2C, 0x8B }, // (Offset,Value)-Pair #7
        { 0x2D, 0x48 }, // (Offset,Value)-Pair #8
        { 0x2E, 0x24 }, // (Offset,Value)-Pair #9

        // CDirectSoundStream_Pause+0x4E : retn 0x08
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4F, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer8_SetPlayRegion_1_0_4361, 10)
    // NOTE: This is actually intercepting CDirectSoundBuffer::SetPlayRegion(ULONG, ULONG)

        // IDirectSoundBuffer8_SetPlayRegion+0x10 : movzx edi, al
        { 0x10, 0x0F }, // (Offset,Value)-Pair #1
        { 0x11, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x12, 0xF8 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetPlayRegion+0x17 : jz +0x0B
        { 0x17, 0x74 }, // (Offset,Value)-Pair #4
        { 0x18, 0x0B }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetPlayRegion+0x46 : mov esi, 0x88780032
        { 0x46, 0xBE }, // (Offset,Value)-Pair #6
        { 0x47, 0x32 }, // (Offset,Value)-Pair #7
        { 0x48, 0x00 }, // (Offset,Value)-Pair #8
        { 0x49, 0x78 }, // (Offset,Value)-Pair #9
        { 0x4A, 0x88 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer8_SetLoopRegion_1_0_4361, 11)
    // NOTE: This is actually intercepting CDirectSoundBuffer::SetLoopRegion(ULONG, ULONG)

        // IDirectSoundBuffer8_SetLoopRegion+0x24 : mov eax, 0x80004005
        { 0x24, 0xB8 }, // (Offset,Value)-Pair #1
        { 0x25, 0x05 }, // (Offset,Value)-Pair #2
        { 0x26, 0x40 }, // (Offset,Value)-Pair #3
        { 0x27, 0x00 }, // (Offset,Value)-Pair #4
        { 0x28, 0x80 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetLoopRegion+0x29 : jmp +0x55
        { 0x29, 0xEB }, // (Offset,Value)-Pair #6
        { 0x2A, 0x55 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_SetLoopRegion+0x56 : sub ecx, esi
        { 0x56, 0x2B }, // (Offset,Value)-Pair #8
        { 0x57, 0xCE }, // (Offset,Value)-Pair #9

        // IDirectSoundBuffer8_SetLoopRegion+0x82 : retn 0x0C
        { 0x82, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x83, 0x0C }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetVolume
// ******************************************************************
OOVPA_NO_XREF(IDirectSoundBuffer8_SetVolume_1_0_4361, 11)
    // NOTE: This is actually intercepting CDirectSoundBuffer::SetVolume(ULONG)

        // IDirectSoundBuffer8_SetVolume+0x0D : movzx esi, al
        { 0x0D, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0E, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xF0 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetVolume+0x26 : jmp +0x22
        { 0x26, 0xEB }, // (Offset,Value)-Pair #4
        { 0x27, 0x22 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetVolume+0x2D : push [esp+0x10]
        { 0x2D, 0xFF }, // (Offset,Value)-Pair #6
        { 0x2E, 0x74 }, // (Offset,Value)-Pair #7
        { 0x2F, 0x24 }, // (Offset,Value)-Pair #8
        { 0x30, 0x10 }, // (Offset,Value)-Pair #9

        // IDirectSoundBuffer8_SetVolume+0x4B : retn 0x08
        { 0x4B, 0xC2 }, // (Offset,Value)-Pair #10
        { 0x4C, 0x08 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData_1_0_4361, 9,

    XREF_DSSETBUFFERDATA,
    XRefZero)

        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC3 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 }, // (Offset,Value)-Pair #3
        { 0x21, 0x0C }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xB8], ecx
        { 0x45, 0x89 }, // (Offset,Value)-Pair #5
        { 0x46, 0x8E }, // (Offset,Value)-Pair #6
        { 0x47, 0xB8 }, // (Offset,Value)-Pair #7

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x8A, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData_1_0_4361, 10,

    XREF_DSSETBUFFERDATA2,
    XRefOne)

        // CDirectSoundBuffer_SetBufferData+0x7A : call [CMcpxBuffer::SetBufferData]
        { 0x7A, XREF_DSSETBUFFERDATA },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetBufferData+0x40 : mov eax, 0x80004005
        { 0x40, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x41, 0x05 }, // (Offset,Value)-Pair #3
        { 0x42, 0x40 }, // (Offset,Value)-Pair #4
        { 0x43, 0x00 }, // (Offset,Value)-Pair #5
        { 0x44, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_SetBufferData+0x5F : jz +0x33
        { 0x5F, 0x74 }, // (Offset,Value)-Pair #7
        { 0x60, 0x33 }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAC, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferData
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetBufferData_1_0_4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetBufferData+0x19 : call [CDirectSoundBuffer::SetBufferData]
        { 0x19, XREF_DSSETBUFFERDATA2 },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetBufferData+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetBufferData+0x13 : sbb ecx, ecx
        { 0x13, 0x1B }, // (Offset,Value)-Pair #5
        { 0x14, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetBufferData+0x15 : and ecx, eax
        { 0x15, 0x23 }, // (Offset,Value)-Pair #7
        { 0x16, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition_1_0_4361, 9,

    XREF_SETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_SetCurrentPosition+0x11 : mov al, [esi+12]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x46 }, // (Offset,Value)-Pair #2
        { 0x13, 0x12 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_SetCurrentPosition+0x5F : jb +0x10
        { 0x5F, 0x72 }, // (Offset,Value)-Pair #4
        { 0x60, 0x10 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_SetCurrentPosition+0x86 : movzx eax, byte ptr [esi+0x64]
        { 0x86, 0x0F }, // (Offset,Value)-Pair #6
        { 0x87, 0xB6 }, // (Offset,Value)-Pair #7
        { 0x88, 0x46 }, // (Offset,Value)-Pair #8
        { 0x89, 0x64 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition_1_0_4361, 10,

    XREF_SETCURRENTPOSITION2,
    XRefOne)

        // CDirectSoundBuffer_SetCurrentPosition+0x35 : call [CMcpxBuffer::SetCurrentPosition]
        { 0x35, XREF_SETCURRENTPOSITION },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_SetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_SetCurrentPosition+0x3D : jz +0x0B
        { 0x3D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_SetCurrentPosition+0x3D : retn 0x08
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x08 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_SetCurrentPosition_1_0_4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_SetCurrentPosition+0x15 : call [CDirectSoundBuffer::SetCurrentPosition]
        { 0x15, XREF_SETCURRENTPOSITION2 },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetCurrentPosition+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 }, // (Offset,Value)-Pair #2
        { 0x0B, 0xC0 }, // (Offset,Value)-Pair #3
        { 0x0C, 0xE4 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_SetCurrentPosition+0x0F : sbb ecx, ecx
        { 0x0F, 0x1B }, // (Offset,Value)-Pair #5
        { 0x10, 0xC9 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetCurrentPosition+0x11 : and ecx, eax
        { 0x11, 0x23 }, // (Offset,Value)-Pair #7
        { 0x12, 0xC8 }, // (Offset,Value)-Pair #8
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition_1_0_4361, 11,

    XREF_GETCURRENTPOSITION,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x0E : lea ecx, [ebp-0x08]
        { 0x0E, 0x8D }, // (Offset,Value)-Pair #1
        { 0x0F, 0x4D }, // (Offset,Value)-Pair #2
        { 0x10, 0xF8 }, // (Offset,Value)-Pair #3

        // CMcpxBuffer_GetCurrentPosition+0x31 : jz +0x02
        { 0x31, 0x74 }, // (Offset,Value)-Pair #4
        { 0x32, 0x02 }, // (Offset,Value)-Pair #5

        // CMcpxBuffer_GetCurrentPosition+0xA0 : div dword ptr [ecx+0xBC]
        { 0xA0, 0xF7 }, // (Offset,Value)-Pair #6
        { 0xA1, 0xB1 }, // (Offset,Value)-Pair #7
        { 0xA2, 0xBC }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_GetCurrentPosition+0xD6 : retn 0x08
        { 0xD6, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xD7, 0x08 }, // (Offset,Value)-Pair #10
        { 0xD8, 0x00 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition_1_0_4361, 10,

    XREF_GETCURRENTPOSITION2,
    XRefOne)

        // CDirectSoundBuffer_GetCurrentPosition+0x39 : call [CMcpxBuffer::GetCurrentPosition]
        { 0x39, XREF_GETCURRENTPOSITION },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_GetCurrentPosition+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_GetCurrentPosition+0x41 : jz +0x0B
        { 0x41, 0x74 }, // (Offset,Value)-Pair #7
        { 0x42, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_GetCurrentPosition+0x3D : retn 0x08
        { 0x52, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x53, 0x0C }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play_1_0_4361, 11,

    XREF_DSOUNDPLAY,
    XRefZero)

        // CMcpxBuffer_Play+0x0E : cmp ebx, edi
        { 0x0E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x0F, 0xDF }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_Play+0x1C : jz +0x09
        { 0x1C, 0x74 }, // (Offset,Value)-Pair #3
        { 0x1D, 0x09 }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_Play+0x30 : test byte ptr [esi+0x12], 2
        { 0x30, 0xF6 }, // (Offset,Value)-Pair #5
        { 0x31, 0x46 }, // (Offset,Value)-Pair #6
        { 0x32, 0x12 }, // (Offset,Value)-Pair #7
        { 0x33, 0x02 }, // (Offset,Value)-Pair #8

        // CMcpxBuffer_Play+0x71 : call dword ptr [eax+0x18]
        { 0x71, 0xFF }, // (Offset,Value)-Pair #9
        { 0x72, 0x50 }, // (Offset,Value)-Pair #10
        { 0x73, 0x18 }, // (Offset,Value)-Pair #11
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play_1_0_4361, 10,

    XREF_DSOUNDPLAY2,
    XRefOne)

        // CDirectSoundBuffer_Play+0x35 : call [CMcpxBuffer::Play]
        { 0x35, XREF_DSOUNDPLAY },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_Play+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_Play+0x3D : jz +0x0B
        { 0x3D, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3E, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_Play+0x4E : retn 0x10
        { 0x4E, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4F, 0x10 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Play_1_0_4361, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_Play+0x1D : call [CDirectSoundBuffer::Play]
        { 0x1D, XREF_DSOUNDPLAY2 },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_Play+0x0E : push [esp+0x10]
        { 0x0E, 0xFF }, // (Offset,Value)-Pair #2
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #3
        { 0x10, 0x24 }, // (Offset,Value)-Pair #4
        { 0x11, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_Play+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #6
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_Play+0x19 : and ecx, eax
        { 0x19, 0x23 }, // (Offset,Value)-Pair #8
        { 0x1A, 0xC8 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_Stop
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop_1_0_4361, 9,

    XREF_DSOUNDSTOP,
    XRefZero)

        // CMcpxBuffer_Stop+0x0E : cmp al, 3
        { 0x0E, 0x3C }, // (Offset,Value)-Pair #1
        { 0x0F, 0x03 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_Stop+0x1C : cmp ax, 0x0203
        { 0x28, 0x66 }, // (Offset,Value)-Pair #3
        { 0x29, 0x3D }, // (Offset,Value)-Pair #4
        { 0x2A, 0x03 }, // (Offset,Value)-Pair #5
        { 0x2B, 0x02 }, // (Offset,Value)-Pair #6

        // CMcpxBuffer_Stop+0x70 : mov eax, [esi+0x90]
        { 0x70, 0x8B }, // (Offset,Value)-Pair #7
        { 0x71, 0x86 }, // (Offset,Value)-Pair #8
        { 0x72, 0x90 }, // (Offset,Value)-Pair #9
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop_1_0_4361, 10,

    XREF_DSOUNDSTOP2,
    XRefOne)

        // CDirectSoundBuffer_Stop+0x33 : call [CMcpxBuffer::Stop]
        { 0x33, XREF_DSOUNDSTOP },  // (Offset,Value)-Pair #1

        // CDirectSoundBuffer_Stop+0x21 : mov eax, 0x80004005
        { 0x21, 0xB8 }, // (Offset,Value)-Pair #2
        { 0x22, 0x05 }, // (Offset,Value)-Pair #3
        { 0x23, 0x40 }, // (Offset,Value)-Pair #4
        { 0x24, 0x00 }, // (Offset,Value)-Pair #5
        { 0x25, 0x80 }, // (Offset,Value)-Pair #6

        // CDirectSoundBuffer_Stop+0x3B : jz +0x0B
        { 0x3B, 0x74 }, // (Offset,Value)-Pair #7
        { 0x3C, 0x0B }, // (Offset,Value)-Pair #8

        // CDirectSoundBuffer_Stop+0x4C : retn 0x04
        { 0x4C, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x4D, 0x04 }, // (Offset,Value)-Pair #10
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer8_Stop
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer8_Stop_1_0_4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer8_Stop+0x11 : call [CDirectSoundBuffer::Stop]
        { 0x11, XREF_DSOUNDSTOP2 },  // (Offset,Value)-Pair #1

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
// * DirectSound::CDirectSoundVoiceSettings::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4361, 7,

    XREF_DSSETMIXBINVOLUMESC,
    XRefZero)

        { 0x03, 0x24 },
        { 0x08, 0x16 },
        { 0x0D, 0x46 },
        { 0x12, 0x8B },
        { 0x17, 0x42 },
        { 0x1C, 0x3B },
        { 0x21, 0x5E },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundVoice_SetMixBinVolumes_1_0_4361, 8,

    XREF_DSSETMIXBINVOLUMESB,
    XRefOne)

        { 0x0D, XREF_DSSETMIXBINVOLUMESC },

        { 0x02, 0x74 },
        { 0x06, 0x74 },
        { 0x0A, 0x4E },
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x19, 0x5E },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_SetMixBinVolumes_1_0_4361, 8,

    XREF_DSSETMIXBINVOLUMESA,
    XRefOne)

        { 0x32, XREF_DSSETMIXBINVOLUMESB },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_PlayEx_1_0_4361, 8,

    XREF_DirectSound_CDirectSoundBuffer_PlayEx,
    XRefOne)

        { 0x3D, XREF_DSOUNDPLAY },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_PlayEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_PlayEx_1_0_4361, 8,

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
// * CMcpxBuffer::Stop2
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop2_1_0_4361, 10,	

	XREF_CMcpxBuffer_Stop2,
	XRefZero)

		// CMcpxBuffer_Stop2+0x03 : mov eax, [ebp+arg_0]
		{ 0x03, 0x8B },
		{ 0x04, 0x45 },
		{ 0x05, 0x08 },
		// CMcpxBuffer_Stop2+0x14 : mov eax, [esi]
		{ 0x14, 0x8B },
		{ 0x15, 0x06 },
		// CMcpxBuffer_Stop2+0x25 : push [ebp+arg_8]
		{ 0x25, 0xFF },
		{ 0x26, 0x75 },
		{ 0x27, 0x10 },
		// CMcpxBuffer_Stop2+0x36 : retn 0xC
		{ 0x36, 0xC2 },
		{ 0x37, 0x0C },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::StopEx
// ******************************************************************
OOVPA_XREF(DirectSound_CDirectSoundBuffer_StopEx_1_0_4361, 8,

    XREF_DirectSound_CDirectSoundBuffer_StopEx,
    XRefOne)

        { 0x3D, XREF_CMcpxBuffer_Stop2 },

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_StopEx_1_0_4361, 8,

    XRefNoSaveIndex,
    XRefOne)

        { 0x1D, XREF_DirectSound_CDirectSoundBuffer_StopEx },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreate_1_0_4361
// ******************************************************************
OOVPATable DSound_1_0_4361[] = {

    // DirectSoundCreate
	OOVPA_TABLE_PATCH(DirectSoundCreate_1_0_4361, XTL::EmuDirectSoundCreate),
    // CDirectSound_CreateSoundStream
	OOVPA_TABLE_XREF(CDirectSound_CreateSoundStream_1_0_4361),
	// IDirectSound8_CreateStream
	OOVPA_TABLE_PATCH(IDirectSound8_CreateStream_1_0_3936, XTL::EmuIDirectSound8_CreateStream),
	// DirectSoundCreateStream
	OOVPA_TABLE_PATCH(DirectSoundCreateStream_1_0_4361, XTL::EmuDirectSoundCreateStream),
    // CMcpxStream::Pause
	OOVPA_TABLE_XREF(CMcpxStream_Pause_1_0_4361),
	// CDirectSoundStream::Pause
	OOVPA_TABLE_PATCH(CDirectSoundStream_Pause_1_0_4361, XTL::EmuCDirectSoundStream_Pause),
	// IDirectSound8::AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSound8_AddRef_1_0_3936, XTL::EmuIDirectSound8_AddRef),
	// IDirectSound8::CreateSoundBuffer
	OOVPA_TABLE_PATCH(IDirectSound8_CreateSoundBuffer_1_0_4361, XTL::EmuIDirectSound8_CreateSoundBuffer),
	// IDirectSoundBuffer8::SetPlayRegion
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetPlayRegion_1_0_4361, XTL::EmuIDirectSoundBuffer8_SetPlayRegion),
	// IDirectSoundBuffer8::SetLoopRegion
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetLoopRegion_1_0_4361, XTL::EmuIDirectSoundBuffer8_SetLoopRegion),
	// IDirectSoundBuffer8::SetVolume
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetVolume_1_0_4361, XTL::EmuIDirectSoundBuffer8_SetVolume),
    // CMcpxBuffer::SetBufferData
	OOVPA_TABLE_XREF(CMcpxBuffer_SetBufferData_1_0_4361),
    // CDirectSoundBuffer::SetBufferData
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetBufferData_1_0_4361),
	// IDirectSoundBuffer8::SetBufferData
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetBufferData_1_0_4361, XTL::EmuIDirectSoundBuffer8_SetBufferData),
    // CMcpxBuffer::SetCurrentPosition
	OOVPA_TABLE_XREF(CMcpxBuffer_SetCurrentPosition_1_0_4361),
    // CDirectSoundBuffer::SetCurrentPosition
	OOVPA_TABLE_XREF(CDirectSoundBuffer_SetCurrentPosition_1_0_4361),
	// IDirectSoundBuffer8::SetCurrentPosition
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_SetCurrentPosition_1_0_4361, XTL::EmuIDirectSoundBuffer8_SetCurrentPosition),
    // CMcpxBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(CMcpxBuffer_GetCurrentPosition_1_0_4361),
    // CDirectSoundBuffer::GetCurrentPosition
	OOVPA_TABLE_XREF(CDirectSoundBuffer_GetCurrentPosition_1_0_4361),
	// IDirectSoundBuffer8::GetCurrentPosition (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_GetCurrentPosition_1_0_3936, XTL::EmuIDirectSoundBuffer8_GetCurrentPosition),
    // CMcpxBuffer::Play
	OOVPA_TABLE_XREF(CMcpxBuffer_Play_1_0_4361),
    // CDirectSoundBuffer::Play
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Play_1_0_4361),
	// IDirectSoundBuffer8::Play
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_Play_1_0_4361, XTL::EmuIDirectSoundBuffer8_Play),
    // CMcpxBuffer::Stop
	OOVPA_TABLE_XREF(CMcpxBuffer_Stop_1_0_4361),
	// CMcpxBuffer_Stop2
	OOVPA_TABLE_XREF(CMcpxBuffer_Stop2_1_0_4361),
    // CDirectSoundBuffer::Stop
	OOVPA_TABLE_XREF(CDirectSoundBuffer_Stop_1_0_4361),
	// IDirectSoundBuffer8::Stop
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_Stop_1_0_4361, XTL::EmuIDirectSoundBuffer8_Stop),
	// CDirectSoundVoiceSettings_SetMixBinVolumes
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoiceSettings_SetMixBinVolumes_1_0_4361),
	// CDirectSoundVoice_SetMixBinVolumes
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundVoice_SetMixBinVolumes_1_0_4361),
    // CDirectSoundBuffer_SetMixBinVolumes
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_SetMixBinVolumes_1_0_4361),
	// DirectSound::CDirectSoundBuffer::PlayEx (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_PlayEx_1_0_4361),
	// IDirectSoundBuffer_PlayEx
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_PlayEx_1_0_4361, XTL::EmuIDirectSoundBuffer8_PlayEx),
	// DirectSound::CDirectSoundBuffer::StopEx (XREF)
	OOVPA_TABLE_XREF(DirectSound_CDirectSoundBuffer_StopEx_1_0_4361),
	// IDirectSoundBuffer_StopEx
	OOVPA_TABLE_PATCH(IDirectSoundBuffer_StopEx_1_0_4361, XTL::EmuIDirectSoundBuffer8_StopEx),
	// IDirectSoundBuffer_AddRef (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSoundBuffer8_AddRef_1_0_3936, XTL::EmuIDirectSoundBuffer8_AddRef),
	// IDirectSound8::Release (* unchanged since 3936 *)
	OOVPA_TABLE_PATCH(IDirectSound8_Release_1_0_3936, XTL::EmuIDirectSound8_Release),
};

// ******************************************************************
// * DSound_1_0_4361_SIZE
// ******************************************************************
uint32 DSound_1_0_4361_SIZE = sizeof(DSound_1_0_4361);
