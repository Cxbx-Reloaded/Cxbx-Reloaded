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
SOOVPA<9> DirectSoundCreate_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_CreateSoundBuffer
// ******************************************************************
SOOVPA<9> IDirectSound8_CreateSoundBuffer_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetBufferData
// ******************************************************************
SOOVPA<10> IDirectSoundBuffer8_SetBufferData_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    // NOTE: This is actually intercepting CDirectSoundBuffer::SetBufferData(void*, ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirectSoundBuffer8_SetBufferData+0x4E : cmp ebx, [eax+0x00B8]
        { 0x4E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x4F, 0x98 }, // (Offset,Value)-Pair #2
        { 0x50, 0xB8 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetBufferData+0x8A : mov ecx, [esi+0x20]
        { 0x8A, 0x8B }, // (Offset,Value)-Pair #4
        { 0x8B, 0x4E }, // (Offset,Value)-Pair #5
        { 0x8C, 0x20 }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetBufferData+0x99 : jz +0x0B
        { 0x99, 0x74 }, // (Offset,Value)-Pair #7
        { 0x9A, 0x0B }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 }, // (Offset,Value)-Pair #9
        { 0xAC, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetPlayRegion
// ******************************************************************
SOOVPA<10> IDirectSoundBuffer8_SetPlayRegion_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    // NOTE: This is actually intercepting CDirectSoundBuffer::SetPlayRegion(ULONG, ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
SOOVPA<11> IDirectSoundBuffer8_SetLoopRegion_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    // NOTE: This is actually intercepting CDirectSoundBuffer::SetLoopRegion(ULONG, ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetVolume
// ******************************************************************
SOOVPA<11> IDirectSoundBuffer8_SetVolume_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    // NOTE: This is actually intercepting CDirectSoundBuffer::SetVolume(ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CMcpxBuffer_SetCurrentPosition
// ******************************************************************
SOOVPA<9> CMcpxBuffer_SetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_SETCURRENTPOSITION,    // XRef Is  Saved
    0,                          // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
SOOVPA<10> CDirectSoundBuffer_SetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_SETCURRENTPOSITION2,   // XRef Is Saved
    1,                          // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_SetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
SOOVPA<11> CMcpxBuffer_GetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_GETCURRENTPOSITION,    // XRef Is  Saved
    0,                          // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundBuffer_GetCurrentPosition
// ******************************************************************
SOOVPA<10> CDirectSoundBuffer_GetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_GETCURRENTPOSITION2,   // XRef Is Saved
    1,                          // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_GetCurrentPosition
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer8_GetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    8,  // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CMcpxBuffer_Play
// ******************************************************************
SOOVPA<11> CMcpxBuffer_Play_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSOUNDPLAY,    // XRef Is  Saved
    0,                  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundBuffer_Play
// ******************************************************************
SOOVPA<10> CDirectSoundBuffer_Play_1_0_4361 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSOUNDPLAY2,   // XRef Is Saved
    1,                  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_Play
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_Play_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSoundBuffer8_GetCurrentPosition+0x1D : call [CDirectSoundBuffer::Play]
        { 0x1D, XREF_DSOUNDPLAY2 },  // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_GetCurrentPosition+0x0E : push [esp+0x10]
        { 0x0E, 0xFF }, // (Offset,Value)-Pair #2
        { 0x0F, 0x74 }, // (Offset,Value)-Pair #3
        { 0x10, 0x24 }, // (Offset,Value)-Pair #4
        { 0x11, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_GetCurrentPosition+0x17 : sbb ecx, ecx
        { 0x17, 0x1B }, // (Offset,Value)-Pair #6
        { 0x18, 0xC9 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_GetCurrentPosition+0x19 : and ecx, eax
        { 0x19, 0x23 }, // (Offset,Value)-Pair #8
        { 0x1A, 0xC8 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_Stop
// ******************************************************************
SOOVPA<11> IDirectSoundBuffer8_Stop_1_0_4361 =
{
    0,  // Large == 0
    11, // Count == 11

    // NOTE: This is actually intercepting CMcpxBuffer::Stop(ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirectSoundBuffer8_Stop+0x28 : cmp ax, 0x0203
        { 0x28, 0x66 }, // (Offset,Value)-Pair #1
        { 0x29, 0x3D }, // (Offset,Value)-Pair #2
        { 0x2A, 0x03 }, // (Offset,Value)-Pair #3
        { 0x2B, 0x02 }, // (Offset,Value)-Pair #4

        // IDirectSoundBuffer8_Stop+0x43 : mov eax, [esi+0x90]
        { 0x43, 0x8B }, // (Offset,Value)-Pair #5
        { 0x44, 0x86 }, // (Offset,Value)-Pair #6
        { 0x45, 0x90 }, // (Offset,Value)-Pair #7

        // IDirectSoundBuffer8_Stop+0x76 : test byte ptr[eax+0x0A], 4
        { 0x76, 0xF6 }, // (Offset,Value)-Pair #8
        { 0x77, 0x40 }, // (Offset,Value)-Pair #9
        { 0x78, 0x0A }, // (Offset,Value)-Pair #10
        { 0x79, 0x04 }, // (Offset,Value)-Pair #11
    }
};

// ******************************************************************
// * DirectSoundCreate_1_0_4361
// ******************************************************************
OOVPATable DSound_1_0_4361[] =
{
    // DirectSoundCreate
    {
        (OOVPA*)&DirectSoundCreate_1_0_4361,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
    // IDirectSound8::CreateSoundBuffer
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4361,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer" 
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData
    {
        (OOVPA*)&IDirectSoundBuffer8_SetBufferData_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetBufferData" 
        #endif
    },
    // IDirectSoundBuffer8::SetPlayRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPlayRegion_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetPlayRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPlayRegion" 
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegion_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion" 
        #endif
    },
    // IDirectSoundBuffer8::SetVolume
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVolume_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetVolume" 
        #endif
    },
    // CMcpxBuffer::SetCurrentPosition
    {
        (OOVPA*)&CMcpxBuffer_SetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetCurrentPosition
    {
        (OOVPA*)&CDirectSoundBuffer_SetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer8_SetCurrentPosition_1_0_4361, 
            
        XTL::EmuIDirectSoundBuffer8_SetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetCurrentPosition"
        #endif
    },
    // CMcpxBuffer::GetCurrentPosition
    {
        (OOVPA*)&CMcpxBuffer_GetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_GetCurrentPosition (XREF)"
        #endif
    },
    // CDirectSoundBuffer::GetCurrentPosition
    {
        (OOVPA*)&CDirectSoundBuffer_GetCurrentPosition_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_GetCurrentPosition (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::GetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer8_GetCurrentPosition_1_0_4361, 
            
        XTL::EmuIDirectSoundBuffer8_GetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_GetCurrentPosition"
        #endif
    },
    // IDirectSoundBuffer8::Stop
    {
        (OOVPA*)&IDirectSoundBuffer8_Stop_1_0_4361,

        XTL::EmuIDirectSoundBuffer8_Stop,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Stop"
        #endif
    },
    // CMcpxBuffer::Play
    {
        (OOVPA*)&CMcpxBuffer_Play_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxBuffer_Play (XREF)"
        #endif
    },
    // CDirectSoundBuffer::Play
    {
        (OOVPA*)&CDirectSoundBuffer_Play_1_0_4361, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_Play (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::Play
    {
        (OOVPA*)&IDirectSoundBuffer8_Play_1_0_4361, 
            
        XTL::EmuIDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_Play"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_4361_SIZE
// ******************************************************************
uint32 DSound_1_0_4361_SIZE = sizeof(DSound_1_0_4361);
