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
SOOVPA<9> DirectSoundCreate_1_0_3936 =
{
    0,  // Large == 0
    9,  // Count == 9

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::CreateSoundStream
// ******************************************************************
SOOVPA<16> CDirectSound_CreateSoundStream_1_0_3936 =
{
    0,  // Large == 0
    16, // Count == 16

    XREF_DSCREATESOUNDSTREAM,   // XRef Is  Saved
    0,                          // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_CreateStream
// ******************************************************************
SOOVPA<10> IDirectSound8_CreateStream_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: tidy up the xref names (are probably not accurate)

    {
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
    }
};


// ******************************************************************
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
SOOVPA<16> CDirectSound_CreateSoundBuffer_1_0_3936 =
{
    0,  // Large == 0
    16, // Count == 16

    XREF_DSCREATESOUNDBUFFER,   // XRef Is  Saved
    0,                          // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_CreateBuffer
// ******************************************************************
SOOVPA<10> IDirectSound8_CreateBuffer_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: tidy up the xref names (are probably not accurate)

    {
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
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetLoopRegionA
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer8_SetLoopRegionA_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    XREF_DSBUFFERSETLOOPREGIONA,// XRef Is  Saved
    0,                          // XRef Not Used

    {
        // IDirectSoundBuffer8_SetLoopRegionA+0x22 : cmp ecx, [eax+0x4C]
        { 0x22, 0x3B }, // (Offset,Value)-Pair #1
        { 0x23, 0x48 }, // (Offset,Value)-Pair #2
        { 0x24, 0x4C }, // (Offset,Value)-Pair #3
        
        // IDirectSoundBuffer8_SetLoopRegionA+0x38 : mov ecx, [esi+0x1C]
        { 0x38, 0x8B }, // (Offset,Value)-Pair #4
        { 0x39, 0x4E }, // (Offset,Value)-Pair #5
        { 0x3A, 0x1C }, // (Offset,Value)-Pair #6

        // IDirectSoundBuffer8_SetLoopRegionA+0x71 : leave; retn 0x0C
        { 0x71, 0xC9 }, // (Offset,Value)-Pair #7
        { 0x72, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x73, 0x0C }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_SetLoopRegion
// ******************************************************************
SOOVPA<10> IDirectSoundBuffer8_SetLoopRegion_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
        // IDirectSoundBuffer8_SetLoopRegion+0x19 : call [IDirectSoundBuffer8_SetLoopRegionA]
        { 0x19, XREF_DSBUFFERSETLOOPREGIONA }, // (Offset,Value)-Pair #1

        // IDirectSoundBuffer8_SetLoopRegion+0x04 : push [esp+0x0C]
        { 0x04, 0xFF }, // (Offset,Value)-Pair #2
        { 0x05, 0x74 }, // (Offset,Value)-Pair #3
        { 0x06, 0x24 }, // (Offset,Value)-Pair #4
        { 0x07, 0x0C }, // (Offset,Value)-Pair #5

        // IDirectSoundBuffer8_SetLoopRegion+0x0E : add eax, 0xFFFFFFE4
        { 0x0E, 0x83 }, // (Offset,Value)-Pair #6
        { 0x0F, 0xC0 }, // (Offset,Value)-Pair #7
        { 0x10, 0xE4 }, // (Offset,Value)-Pair #8

        // IDirectSoundBuffer8_SetLoopRegion+0x1D : retn 0x0C
        { 0x1D, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x1E, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * IDirectSoundBuffer8_Play
// ******************************************************************
SOOVPA<11> IDirectSoundBuffer8_Play_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
SOOVPA<10> CMcpxVoiceClient_SetVolume_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSTREAMSETVOLUME, // XRef Is  Saved
    0,                      // XRef Not Used

    // TODO: re-use for IDirectSoundBuffer_SetVolume!

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVolume
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetVolume_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetAllParametersB
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetAllParametersB_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMSETALLPARAMETERS1B,  // XRef Is  Saved
    0,                                // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetAllParametersB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetAllParametersA
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetAllParametersA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETALLPARAMETERS1A, // XRef Is Saved
    1,                               // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetAllParametersA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetAllParameters
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetAllParameters_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeAnglesB
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetConeAnglesB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETCONEANGLES1B, // XRef Is  Saved
    0,                            // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeAnglesA
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetConeAnglesA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETCONEANGLES1A, // XRef Is Saved
    1,                               // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetConeAnglesA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeAngles
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetConeAngles_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolumeB
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetConeOutsideVolumeB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B, // XRef Is  Saved
    0,                                   // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolumeA
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetConeOutsideVolumeA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A, // XRef Is Saved
    1,                                   // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetConeOutsideVolumeA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetConeOutsideVolume_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceC
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetMaxDistanceC_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETMAXDISTANCE1C, // XRef Is  Saved
    0,                             // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceC!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceB
// ******************************************************************
SOOVPA<15> CDirectSoundStream_SetMaxDistanceB_1_0_3936 =
{
    0,  // Large == 0
    15, // Count == 15

    XREF_DSSTREAMSETMAXDISTANCE1B, // XRef Is Saved
    1,                                 // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMaxDistanceA
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetMaxDistanceA_1_0_3936 =
{
    0, // Large == 0
    10,// Count == 10

    XREF_DSSTREAMSETMAXDISTANCE1A, // XRef Is Saved
    1,                             // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetMaxDistanceA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetMaxDistance_1_0_3936 =
{
    0, // Large == 0
    11,// Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceC
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetMinDistanceC_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETMINDISTANCE1C, // XRef Is  Saved
    0,                             // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceC!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceB
// ******************************************************************
SOOVPA<15> CDirectSoundStream_SetMinDistanceB_1_0_3936 =
{
    0,  // Large == 0
    15, // Count == 15

    XREF_DSSTREAMSETMINDISTANCE1B, // XRef Is Saved
    1,                                 // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMinDistanceA
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetMinDistanceA_1_0_3936 =
{
    0, // Large == 0
    10,// Count == 10

    XREF_DSSTREAMSETMINDISTANCE1A, // XRef Is Saved
    1,                             // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetMinDistanceA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetMinDistance_1_0_3936 =
{
    0, // Large == 0
    11,// Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVelocityC
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetVelocityC_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETVELOCITY1C, // XRef Is  Saved
    0,                          // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetVelocityC!?

    {
        // CDirectSoundStream_SetVelocityC+0x0C : or byte ptr [ecx+0x80], 0x40
        { 0x0C, 0x83 }, // (Offset,Value)-Pair #1
        { 0x0D, 0x89 }, // (Offset,Value)-Pair #2
        { 0x0E, 0x80 }, // (Offset,Value)-Pair #3
        { 0x0F, 0x00 }, // (Offset,Value)-Pair #4
        { 0x10, 0x00 }, // (Offset,Value)-Pair #5
        { 0x11, 0x00 }, // (Offset,Value)-Pair #6
        { 0x12, 0x40 }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetVelocityC+0x09 : movsd; movsd; movsd
        { 0x09, 0xA5 }, // (Offset,Value)-Pair #8
        { 0x0A, 0xA5 }, // (Offset,Value)-Pair #9
        { 0x0B, 0xA5 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetVelocityC+0x25 : retn 0x08
        { 0x25, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x26, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVelocityB
// ******************************************************************
SOOVPA<14> CDirectSoundStream_SetVelocityB_1_0_3936 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_DSSTREAMSETVELOCITY1B, // XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetVelocityB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVelocityA
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetVelocityA_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    XREF_DSSTREAMSETVELOCITY1A, // XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetVelocityA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetVelocity
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetVelocity_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationC
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetConeOrientationC_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMSETCONEORIENTATION1C, // XRef Is  Saved
    0,                                 // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationC!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationB
// ******************************************************************
SOOVPA<14> CDirectSoundStream_SetConeOrientationB_1_0_3936 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_DSSTREAMSETCONEORIENTATION1B, // XRef Is Saved
    1,                                 // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOrientationA
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetConeOrientationA_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    XREF_DSSTREAMSETCONEORIENTATION1A, // XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetConeOrientationA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetConeOrientation
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetConeOrientation_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetPositionC
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetPositionC_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMSETPOSITION1C, // XRef Is  Saved
    0,                          // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetPositionC!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetPositionB
// ******************************************************************
SOOVPA<14> CDirectSoundStream_SetPositionB_1_0_3936 =
{
    0,  // Large == 0
    14, // Count == 14

    XREF_DSSTREAMSETPOSITION1B, // XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetPositionB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetPositionA
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetPositionA_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    XREF_DSSTREAMSETPOSITION1A, // XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetPositionA!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetPosition
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetPosition_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetFrequencyB
// ******************************************************************
SOOVPA<11> CDirectSoundStream_SetFrequencyB_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSTREAMSETFREQUENCY1B,// XRef Is  Saved
    0,                          // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetFrequencyB!?

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetFrequencyA
// ******************************************************************
SOOVPA<9> CDirectSoundStream_SetFrequencyA_1_0_3936 =
{
    0, // Large == 0
    9, // Count == 9

    XREF_DSSTREAMSETFREQUENCY1A,// XRef Is Saved
    1,                          // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetFrequencyA!?

    {
        // CDirectSoundStream_SetFrequencyA+0x20 : mov eax, [esi+0x18]
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetFrequency
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetFrequency_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSoundStream_SetRolloffFactorB
// ******************************************************************
SOOVPA<12> CDirectSoundStream_SetRolloffFactorB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSTREAMSETROLLOFFFACTOR1B,// XRef Is  Saved
    0,                              // XRef Not Used

    // TODO: re-use for CDirectSoundBuffer_SetRolloffFactorB!?

    {
        // CDirectSoundStream_SetRolloffFactorB+0x04 : or byte ptr [ecx+0x80], 0xFF
        { 0x04, 0x80 }, // (Offset,Value)-Pair #1
        { 0x05, 0x89 }, // (Offset,Value)-Pair #2
        { 0x06, 0x80 }, // (Offset,Value)-Pair #3
        { 0x07, 0x00 }, // (Offset,Value)-Pair #4
        { 0x08, 0x00 }, // (Offset,Value)-Pair #5
        { 0x09, 0x00 }, // (Offset,Value)-Pair #6
        { 0x0A, 0xFF }, // (Offset,Value)-Pair #7

        // CDirectSoundStream_SetRolloffFactorB+0x10 : cmp [ecx+0x58], eax
        { 0x10, 0x89 }, // (Offset,Value)-Pair #8
        { 0x11, 0x41 }, // (Offset,Value)-Pair #9
        { 0x12, 0x58 }, // (Offset,Value)-Pair #10

        // CDirectSoundStream_SetRolloffFactorB+0x1E : retn 0x08
        { 0x1E, 0xC2 }, // (Offset,Value)-Pair #11
        { 0x1F, 0x08 }, // (Offset,Value)-Pair #12
    }
};

// ******************************************************************
// * CDirectSoundStream_SetRolloffFactorA
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetRolloffFactorA_1_0_3936 =
{
    0, // Large == 0
    10,// Count == 10

    XREF_DSSTREAMSETROLLOFFFACTOR1A,// XRef Is Saved
    1,                              // XRef Is Used

    // TODO: re-use for CDirectSoundBuffer_SetRolloffFactorA!?

    {
        // CDirectSoundStream_SetRolloffFactorA+0x1F : mov eax, [esi+0x18]
        { 0x1F, XREF_DSSTREAMSETROLLOFFFACTOR1B }, // (Offset,Value)-Pair #1

        // CDirectSoundStream_SetRolloffFactorA+0x17 : push [esp+0x14]
        { 0x17, 0xFF }, // (Offset,Value)-Pair #2
        { 0x18, 0x74 }, // (Offset,Value)-Pair #3
        { 0x19, 0x24 }, // (Offset,Value)-Pair #4
        { 0x1A, 0x14 }, // (Offset,Value)-Pair #5

        // CDirectSoundStream_SetRolloffFactorA+0x1B : mov ecx, [eax+0x14]
        { 0x1B, 0x8B }, // (Offset,Value)-Pair #6
        { 0x1C, 0x48 }, // (Offset,Value)-Pair #7
        { 0x1D, 0x14 }, // (Offset,Value)-Pair #8

        // CDirectSoundStream_SetRolloffFactorA+0x38 : retn 0x0C
        { 0x38, 0xC2 }, // (Offset,Value)-Pair #9
        { 0x39, 0x0C }, // (Offset,Value)-Pair #10
    }
};

// ******************************************************************
// * CDirectSoundStream_SetRolloffFactor
// ******************************************************************
SOOVPA<10> CDirectSoundStream_SetRolloffFactor_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundStream_SetI3DL2SourceA
// ******************************************************************
SOOVPA<11> IDirectSoundStream_SetI3DL2SourceA_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSETI3DL2SOURCE1A,  // XRef Is  Saved
    0,                        // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundStream_SetI3DL2Source
// ******************************************************************
SOOVPA<10> IDirectSoundStream_SetI3DL2Source_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: Tentative! (most likely incorrect, actually!)

    {
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
    }
};

// ******************************************************************
// * IDirectSoundStream_SetHeadroomA
// ******************************************************************
SOOVPA<11> IDirectSoundStream_SetHeadroomA_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_DSSETHEADROOMA,  // XRef Is  Saved
    0,                    // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * IDirectSoundStream_SetHeadroom
// ******************************************************************
SOOVPA<10> IDirectSoundStream_SetHeadroom_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: Tentative! (most likely incorrect, actually!)
    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetOrientation
// ******************************************************************
SOOVPA<13> IDirectSound8_SetOrientation_1_0_3936 =
{
    0,  // Large == 0
    13, // Count == 13

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::CommitDeferredSettingsB
// ******************************************************************
SOOVPA<11> CDirectSound_CommitDeferredSettingsB_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    XREF_COMMITDEFERREDSETTINGSB, // XRef Is Saved
    0,                            // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::CommitDeferredSettingsA
// ******************************************************************
SOOVPA<10> CDirectSound_CommitDeferredSettingsA_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_COMMITDEFERREDSETTINGSA, // XRef Is Saved
    1,                            // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound_CommitDeferredSettings
// ******************************************************************
SOOVPA<8> CDirectSound_CommitDeferredSettings_1_0_3936 =
{
    0, // Large == 0
    8, // Count == 8

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetDistanceFactorB
// ******************************************************************
SOOVPA<13> CDirectSound_SetDistanceFactorB_1_0_3936 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_SETDISTANCEFACTORB,// XRef Is Saved
    0,                      // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetDistanceFactorA
// ******************************************************************
SOOVPA<12> CDirectSound_SetDistanceFactorA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 11

    XREF_SETDISTANCEFACTORA,// XRef Is Saved
    1,                      // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetDistanceFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetDistanceFactor_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: Tentative (may not be SetDistanceFactor)

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetRolloffFactorB
// ******************************************************************
SOOVPA<13> CDirectSound_SetRolloffFactorB_1_0_3936 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_SETROLLOFFFACTORB, // XRef Is Saved
    0,                      // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetRolloffFactorA
// ******************************************************************
SOOVPA<12> CDirectSound_SetRolloffFactorA_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 11

    XREF_SETROLLOFFFACTORA, // XRef Is Saved
    1,                      // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetRolloffFactor
// ******************************************************************
SOOVPA<11> IDirectSound8_SetRolloffFactor_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    // TODO: Tentative (may not be SetRolloffFactor)

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetMixBinHeadroomB
// ******************************************************************
SOOVPA<12> CDirectSound_SetMixBinHeadroomB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSETMIXBINHEADROOMB, // XRef Is Saved
    0,                         // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetMixBinHeadroomA
// ******************************************************************
SOOVPA<9> CDirectSound_SetMixBinHeadroomA_1_0_3936 =
{
    0,  // Large == 0
    9,  // Count == 9

    XREF_DSSETMIXBINHEADROOMA,  // XRef Is Saved
    1,                          // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetMixBinHeadroom
// ******************************************************************
SOOVPA<12> IDirectSound8_SetMixBinHeadroom_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetPositionB
// ******************************************************************
SOOVPA<12> CDirectSound_SetPositionB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSETPOSITIONB,    // XRef Is Saved
    0,                      // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetPositionA
// ******************************************************************
SOOVPA<10> CDirectSound_SetPositionA_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSETPOSITIONA,    // XRef Is Saved
    1,                      // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetPosition
// ******************************************************************
SOOVPA<11> IDirectSound8_SetPosition_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetVelocityB
// ******************************************************************
SOOVPA<12> CDirectSound_SetVelocityB_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    XREF_DSSETVELOCITYB,    // XRef Is Saved
    0,                      // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetVelocityA
// ******************************************************************
SOOVPA<10> CDirectSound_SetVelocityA_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSETVELOCITYA,    // XRef Is Saved
    1,                      // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetVelocity
// ******************************************************************
SOOVPA<11> IDirectSound8_SetVelocity_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetAllParametersB
// ******************************************************************
SOOVPA<13> CDirectSound_SetAllParametersB_1_0_3936 =
{
    0,  // Large == 0
    13, // Count == 13

    XREF_DSSETALLPARAMETERSB,   // XRef Is Saved
    0,                          // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
SOOVPA<10> CDirectSound_SetAllParametersA_1_0_3936 =
{
    0,  // Large == 0
    10, // Count == 10

    XREF_DSSETALLPARAMETERSA,   // XRef Is Saved
    1,                          // XRef Is Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_SetAllParameters
// ******************************************************************
SOOVPA<12> IDirectSound8_SetAllParameters_1_0_3936 =
{
    0,  // Large == 0
    12, // Count == 12

    -1, // XRef Not Saved
    1,  // XRef Is  Used

    {
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
    }
};

// ******************************************************************
// * IDirectSound8_DownloadEffectsImage
// ******************************************************************
SOOVPA<11> IDirectSound8_DownloadEffectsImage_1_0_3936 =
{
    0,  // Large == 0
    11, // Count == 11

    -1, // XRef Not Saved
    0,  // XRef Not Used

    {
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
    }
};

// ******************************************************************
// * DirectSoundCreate_1_0_3936
// ******************************************************************
OOVPATable DSound_1_0_3936[] =
{
    // DirectSoundCreate
    {
        (OOVPA*)&DirectSoundCreate_1_0_3936,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate" 
        #endif
    },
    // CDirectSound_CreateSoundStream
    {
        (OOVPA*)&CDirectSound_CreateSoundStream_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundStream (XREF)" 
        #endif
    },
    // IDirectSound8_CreateStream
    {
        (OOVPA*)&IDirectSound8_CreateStream_1_0_3936,

        XTL::EmuIDirectSound8_CreateStream,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateStream" 
        #endif
    },
    // CDirectSound_CreateSoundBuffer
    {
        (OOVPA*)&CDirectSound_CreateSoundBuffer_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::CreateSoundBuffer (XREF)" 
        #endif
    },
    // IDirectSound8_CreateBuffer
    {
        (OOVPA*)&IDirectSound8_CreateBuffer_1_0_3936,

        XTL::EmuIDirectSound8_CreateBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateBuffer" 
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion (XREF)
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegionA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetLoopRegionA (XRef)"
        #endif
    },
    // IDirectSoundBuffer8::SetLoopRegion
    {
        (OOVPA*)&IDirectSoundBuffer8_SetLoopRegion_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_SetLoopRegion,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetLoopRegion" 
        #endif
    },
    // IDirectSoundBuffer8::Play
    {
        (OOVPA*)&IDirectSoundBuffer8_Play_1_0_3936,

        XTL::EmuIDirectSoundBuffer8_Play,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Play" 
        #endif
    },
    // CMcpxVoiceClient_SetVolume
    {
        (OOVPA*)&CMcpxVoiceClient_SetVolume_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CMcpxVoiceClient::SetVolume (XREF)" 
        #endif
    },
    // CDirectSoundStream_SetVolume
    {
        (OOVPA*)&CDirectSoundStream_SetVolume_1_0_3936, 
            
        XTL::EmuCDirectSoundStream_SetVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVolume" 
        #endif
    },
    // CDirectSoundStream::SetConeAnglesB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeAnglesB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeAnglesB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeAnglesA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeAnglesA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeAnglesA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeAngles
    {
        (OOVPA*)&CDirectSoundStream_SetConeAngles_1_0_3936,

        XTL::EmuCDirectSoundStream_SetConeAngles,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetConeAngles" 
        #endif
    },
    // CDirectSoundStream::SetConeOutsideVolumeB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeOutsideVolumeB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeOutsideVolumeB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeOutsideVolumeA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeOutsideVolumeA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeOutsideVolumeA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeOutsideVolume
    {
        (OOVPA*)&CDirectSoundStream_SetConeOutsideVolume_1_0_3936,

        XTL::EmuCDirectSoundStream_SetConeOutsideVolume,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetConeOutsideVolume" 
        #endif
    },
    // CDirectSoundStream::SetAllParametersB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetAllParametersB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetAllParametersB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetAllParametersA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetAllParametersA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetAllParametersA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetAllParameters
    {
        (OOVPA*)&CDirectSoundStream_SetAllParameters_1_0_3936,

        XTL::EmuCDirectSoundStream_SetAllParameters,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetAllParameters" 
        #endif
    },
    // CDirectSoundStream::SetMaxDistanceC (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMaxDistanceC_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMaxDistanceC (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMaxDistanceB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMaxDistanceB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMaxDistanceB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMaxDistanceA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMaxDistanceA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMaxDistanceA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMaxDistance
    {
        (OOVPA*)&CDirectSoundStream_SetMaxDistance_1_0_3936,

        XTL::EmuCDirectSoundStream_SetMaxDistance,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetMaxDistance" 
        #endif
    },
    // CDirectSoundStream::SetMinDistanceC (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMinDistanceC_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMinDistanceC (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMinDistanceB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMinDistanceB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMinDistanceB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMinDistanceA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetMinDistanceA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetMinDistanceA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetMinDistance
    {
        (OOVPA*)&CDirectSoundStream_SetMinDistance_1_0_3936,

        XTL::EmuCDirectSoundStream_SetMinDistance,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetMinDistance" 
        #endif
    },
    // CDirectSoundStream::SetVelocityC (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetVelocityC_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetVelocityC (XRef)"
        #endif
    },
    // CDirectSoundStream::SetVelocityB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetVelocityB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetVelocityB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetVelocityA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetVelocityA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetVelocityA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetVelocity
    {
        (OOVPA*)&CDirectSoundStream_SetVelocity_1_0_3936,

        XTL::EmuCDirectSoundStream_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetVelocity" 
        #endif
    },
    // CDirectSoundStream::SetConeOrientationC (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeOrientationC_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeOrientationC (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeOrientationB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeOrientationB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeOrientationB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeOrientationA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetConeOrientationA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetConeOrientationA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetConeOrientation
    {
        (OOVPA*)&CDirectSoundStream_SetConeOrientation_1_0_3936,

        XTL::EmuCDirectSoundStream_SetConeOrientation,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetConeOrientation" 
        #endif
    },
    // CDirectSoundStream::SetPositionC (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetPositionC_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetPositionC (XRef)"
        #endif
    },
    // CDirectSoundStream::SetPositionB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetPositionB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetPositionB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetPositionA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetPositionA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetPositionA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetPosition
    {
        (OOVPA*)&CDirectSoundStream_SetPosition_1_0_3936,

        XTL::EmuCDirectSoundStream_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetPosition" 
        #endif
    },
    // CDirectSoundStream::SetFrequencyB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetFrequencyB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetFrequencyB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetFrequencyA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetFrequencyA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetFrequencyA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetFrequency
    {
        (OOVPA*)&CDirectSoundStream_SetFrequency_1_0_3936,

        XTL::EmuCDirectSoundStream_SetFrequency,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetFrequency" 
        #endif
    },
    // CDirectSoundStream::SetRolloffFactorB (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetRolloffFactorB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetRolloffFactorB (XRef)"
        #endif
    },
    // CDirectSoundStream::SetRolloffFactorA (XREF)
    {
        (OOVPA*)&CDirectSoundStream_SetRolloffFactorA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundStream_SetRolloffFactorA (XRef)"
        #endif
    },
    // CDirectSoundStream::SetRolloffFactor
    {
        (OOVPA*)&CDirectSoundStream_SetRolloffFactor_1_0_3936,

        XTL::EmuCDirectSoundStream_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSoundStream_SetRolloffFactor" 
        #endif
    },
    // IDirectSoundStream::SetI3DL2SourceA (XREF)
    {
        (OOVPA*)&IDirectSoundStream_SetI3DL2SourceA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "IDirectSoundStream_SetI3DL2SourceA (XRef)"
        #endif
    },
    // IDirectSoundStream::SetI3DL2Source
    {
        (OOVPA*)&IDirectSoundStream_SetI3DL2Source_1_0_3936,

        XTL::EmuIDirectSoundStream_SetI3DL2Source,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundStream_SetI3DL2Source" 
        #endif
    },
    // IDirectSoundStream::SetHeadroomA (XREF)
    {
        (OOVPA*)&IDirectSoundStream_SetHeadroomA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "IDirectSoundStream_SetHeadroomA (XRef)"
        #endif
    },
    // IDirectSoundStream::SetHeadroom
    {
        (OOVPA*)&IDirectSoundStream_SetHeadroom_1_0_3936,

        XTL::EmuIDirectSoundStream_SetHeadroom,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundStream_SetHeadroom" 
        #endif
    },
    // IDirectSound8::SetOrientation
    {
        (OOVPA*)&IDirectSound8_SetOrientation_1_0_3936,

        XTL::EmuIDirectSound8_SetOrientation,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetOrientation" 
        #endif
    },
    // CDirectSound::CommitDeferredSettingsB (XREF)
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettingsB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_CommitDeferredSettingsB (XRef)"
        #endif
    },
    // CDirectSound::CommitDeferredSettingsA (XREF)
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettingsA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_CommitDeferredSettingsA (XRef)"
        #endif
    },
    // CDirectSound::CommitDeferredSettings
    {
        (OOVPA*)&CDirectSound_CommitDeferredSettings_1_0_3936,

        XTL::EmuCDirectSound_CommitDeferredSettings,

        #ifdef _DEBUG_TRACE
        "EmuCDirectSound_CommitDeferredSettings" 
        #endif
    },
    // CDirectSound::SetDistanceFactorB (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorB (XRef)"
        #endif
    },
    // CDirectSound::SetDistanceFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetDistanceFactorA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetDistanceFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetDistanceFactor
    {
        (OOVPA*)&IDirectSound8_SetDistanceFactor_1_0_3936,

        XTL::EmuIDirectSound8_SetDistanceFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetDistanceFactor" 
        #endif
    },
    // CDirectSound::SetRolloffFactorB (XREF)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactorB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetRolloffFactorB (XRef)"
        #endif
    },
    // CDirectSound::SetRolloffFactorA (XREF)
    {
        (OOVPA*)&CDirectSound_SetRolloffFactorA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetRolloffFactorA (XRef)"
        #endif
    },
    // IDirectSound8::SetRolloffFactor
    {
        (OOVPA*)&IDirectSound8_SetRolloffFactor_1_0_3936,

        XTL::EmuIDirectSound8_SetRolloffFactor,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetRolloffFactor" 
        #endif
    },
    // CDirectSound::SetMixBinHeadroomB (XREF)
    {
        (OOVPA*)&CDirectSound_SetMixBinHeadroomB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetMixBinHeadroomB (XRef)"
        #endif
    },
    // CDirectSound::SetMixBinHeadroomA (XREF)
    {
        (OOVPA*)&CDirectSound_SetMixBinHeadroomA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetMixBinHeadroomA (XRef)"
        #endif
    },
    // IDirectSound8::SetMixBinHeadroom
    {
        (OOVPA*)&IDirectSound8_SetMixBinHeadroom_1_0_3936,

        XTL::EmuIDirectSound8_SetMixBinHeadroom,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetMixBinHeadroom" 
        #endif
    },
    // CDirectSound::SetPositionB (XREF)
    {
        (OOVPA*)&CDirectSound_SetPositionB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetPositionB (XRef)"
        #endif
    },
    // CDirectSound::SetPositionA (XREF)
    {
        (OOVPA*)&CDirectSound_SetPositionA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetPositionA (XRef)"
        #endif
    },
    // IDirectSound8::SetPosition
    {
        (OOVPA*)&IDirectSound8_SetPosition_1_0_3936,

        XTL::EmuIDirectSound8_SetPosition,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetPosition" 
        #endif
    },
    // CDirectSound::SetVelocityB (XREF)
    {
        (OOVPA*)&CDirectSound_SetVelocityB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetVelocityB (XRef)"
        #endif
    },
    // CDirectSound::SetVelocityA (XREF)
    {
        (OOVPA*)&CDirectSound_SetVelocityA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetVelocityA (XRef)"
        #endif
    },
    // IDirectSound8::SetVelocity
    {
        (OOVPA*)&IDirectSound8_SetVelocity_1_0_3936,

        XTL::EmuIDirectSound8_SetVelocity,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetVelocity" 
        #endif
    },
    // CDirectSound::SetVelocityB (XREF)
    {
        (OOVPA*)&CDirectSound_SetAllParametersB_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetAllParametersB (XRef)"
        #endif
    },
    // CDirectSound::SetAllParametersA (XREF)
    {
        (OOVPA*)&CDirectSound_SetAllParametersA_1_0_3936, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound_SetAllParametersA (XRef)"
        #endif
    },
    // IDirectSound8::SetAllParameters
    {
        (OOVPA*)&IDirectSound8_SetAllParameters_1_0_3936,

        XTL::EmuIDirectSound8_SetAllParameters,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetAllParameters" 
        #endif
    },
    // IDirectSound8::DownloadEffectsImage
    {
        (OOVPA*)&IDirectSound8_DownloadEffectsImage_1_0_3936,

        XTL::EmuIDirectSound8_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_DownloadEffectsImage" 
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_3936_SIZE
// ******************************************************************
uint32 DSound_1_0_3936_SIZE = sizeof(DSound_1_0_3936);
