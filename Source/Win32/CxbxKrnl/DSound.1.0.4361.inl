// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4627.cpp
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
        // IDirectSoundBuffer8_SetBufferData+0x48 : cmp ebx, [eax+0x00B8]
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
// * IDirectSoundBuffer8_SetCurrentPosition
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetCurrentPosition_1_0_4361 =
{
    0,  // Large == 0
    9,  // Count == 9

    // NOTE: This is actually intercepting CMcpxBuffer::SetCurrentPosition(ULONG)

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
        // IDirectSoundBuffer8_SetCurrentPosition+0x11 : mov al, [esi+12]
        { 0x11, 0x8A }, // (Offset,Value)-Pair #1
        { 0x12, 0x46 }, // (Offset,Value)-Pair #2
        { 0x13, 0x12 }, // (Offset,Value)-Pair #3

        // IDirectSoundBuffer8_SetCurrentPosition+0x5F : jb +0x10
        { 0x5F, 0x72 }, // (Offset,Value)-Pair #4
        { 0x60, 0x10 }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetCurrentPosition+0x86 : movzx eax, byte ptr [esi+0x64]
        { 0x86, 0x0F }, // (Offset,Value)-Pair #6
        { 0x87, 0xB6 }, // (Offset,Value)-Pair #7
        { 0x88, 0x46 }, // (Offset,Value)-Pair #8
        { 0x89, 0x64 }, // (Offset,Value)-Pair #9
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

        xdirectx::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
    // IDirectSound8::CreateSoundBuffer
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4361,

        xdirectx::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer" 
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData
    {
        (OOVPA*)&IDirectSoundBuffer8_SetBufferData_1_0_4361,

        xdirectx::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetBufferData" 
        #endif
    },
    // IDirectSoundBuffer8::SetPlayRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetPlayRegion_1_0_4361,

        xdirectx::EmuIDirectSoundBuffer8_SetPlayRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetPlayRegion" 
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegion_1_0_4361,

        xdirectx::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion" 
        #endif
    },
    // IDirectSoundBuffer8::SetVolume
    {
        (OOVPA*)&IDirectSoundBuffer8_SetVolume_1_0_4361,

        xdirectx::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetVolume" 
        #endif
    },
    // IDirectSoundBuffer8::SetCurrentPosition
    {
        (OOVPA*)&IDirectSoundBuffer8_SetCurrentPosition_1_0_4361,

        xdirectx::EmuIDirectSoundBuffer8_SetCurrentPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetCurrentPosition"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_4361_SIZE
// ******************************************************************
uint32 DSound_1_0_4361_SIZE = sizeof(DSound_1_0_4361);
