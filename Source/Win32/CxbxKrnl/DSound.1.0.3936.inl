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
        "EmuCDirectSoundStream8_SetFrequency" 
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
        "EmuCDirectSoundStream8_SetRolloffFactor" 
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
