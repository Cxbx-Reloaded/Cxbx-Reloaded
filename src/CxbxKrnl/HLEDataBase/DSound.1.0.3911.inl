// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.3911.inl
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
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

// TODO: Known DSound OOVPA issue list for 3911 to 3936(?):
// * Following separater functions has exact asm codes as whole function are...
//   * IDirectSoundStream_SetVolume     & CDirectSoundStream_SetVolume
//   * IDirectSoundStream_SetPitch      & CDirectSoundStream_SetPitch
//   * IDirectSoundStream_SetLFO        & CDirectSoundStream_SetLFO
//   * IDirectSoundStream_SetEG         & CDirectSoundStream_SetEG
//   * IDirectSoundStream_SetFilter     & CDirectSoundStream_SetFilter
//   * IDirectSoundStream_SetHeadroom   & CDirectSoundStream_SetHeadroom
//   * IDirectSoundStream_SetFrequency  & CDirectSoundStream_SetFrequency
//   * IDirectSoundStream_SetMixBins    & CDirectSoundStream_SetMixBins



// ******************************************************************
// * DirectSoundEnterCriticalSection
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(DirectSoundEnterCriticalSection, 3911, 7,

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
// * DirectSoundCreate
// ******************************************************************
OOVPA_NO_XREF(DirectSoundCreate, 3911, 9)

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
OOVPA_XREF(CMcpxAPU_ServiceDeferredCommandsLow, 3911, 12,

    XREF_CMcpxAPU_ServiceDeferredCommandsLow,
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
OOVPA_XREF(CDirectSound_DoWork, 3911, 9,

    XREF_CDirectSound_DoWork,
    XRefOne)

        // CDirectSound_DoWork+0x0F : call [CMcpxAPU_ServiceDeferredCommandsLow]
        XREF_ENTRY( 0x10, XREF_CMcpxAPU_ServiceDeferredCommandsLow ),

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
OOVPA_XREF(DirectSoundDoWork, 3911, 9,

    XRefNoSaveIndex,
    XRefOne)

        // DirectSoundDoWork+0x12 : call [CDirectSound_DoWork]
        XREF_ENTRY( 0x13, XREF_CDirectSound_DoWork ),

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
// * CDirectSound::CreateSoundBuffer
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundBuffer, 3911, 17,

    XREF_CDirectSound_CreateSoundBuffer,
    XRefZero)

        // CDirectSound_CreateSoundBuffer+0x00 : push ebp
        { 0x00, 0x55 },

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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSound_CreateSoundBuffer, 3911, 12,

    XREF_IDirectSound_CreateSoundBuffer,
    XRefOne)

        // IDirectSound_CreateSoundBuffer+0x1C : call [CDirectSound::CreateSoundBuffer]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_CreateSoundBuffer ),

        // IDirectSound_CreateSoundBuffer+0x04 : mov eax, [esp+8]
        { 0x04, 0x8B },
        { 0x05, 0x44 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSound_CreateSoundBuffer+0x12 : add eax, 0xFFFFFFF8
        { 0x12, 0x83 },
        { 0x13, 0xC0 },
        { 0x14, 0xF8 },

        // IDirectSound_CreateSoundBuffer+0x17 : sbb ecx, ecx
        { 0x17, 0x1B },
        { 0x18, 0xC9 },

        // IDirectSound_CreateSoundBuffer+0x21 : retn 0x10
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;

// ******************************************************************
// * DirectSoundCreateBuffer
// ******************************************************************
OOVPA_XREF(DirectSoundCreateBuffer, 3911, 12,

           XRefNoSaveIndex,
           XRefOne)

    // DirectSoundCreateBuffer+0x27 : call [IDirectSound::CreateSoundBuffer]
    XREF_ENTRY(0x28, XREF_IDirectSound_CreateSoundBuffer),

    // DirectSoundCreateBuffer+0x04 : and [ebp-0x04], 0
    { 0x04, 0x83 },
    { 0x05, 0x65 },
    { 0x06, 0xFC },

    { 0x20, 0x0C },
    { 0x23, 0x08 },
    { 0x26, 0xFC },

    // DirectSoundCreateBuffer+0x2E : lea eax, [ebp-0x04]
    { 0x2E, 0x8D },
    { 0x2F, 0x45 },
    { 0x30, 0xFC },

    // DirectSoundCreateBuffer+0x3B : retn 0x08
    { 0x3B, 0xC2 },
    { 0x3C, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::CreateSoundStream
// ******************************************************************
OOVPA_XREF(CDirectSound_CreateSoundStream, 3911, 16,

    XREF_CDirectSound_CreateSoundStream,
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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSound_CreateSoundStream, 3911, 10,

    XREF_IDirectSound_CreateSoundStream,
    XRefOne)

        // IDirectSound_CreateSoundStream+0x1C : call [CDirectSound::CreateSoundStream]
        XREF_ENTRY( 0x1D, XREF_CDirectSound_CreateSoundStream ),

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
// * DirectSoundCreateStream
// ******************************************************************
OOVPA_XREF(DirectSoundCreateStream, 3911, 12,

           XRefNoSaveIndex,
           XRefOne)

    // DirectSoundCreateStream+0x27 : call [IDirectSound::CreateSoundStream]
    XREF_ENTRY(0x28, XREF_IDirectSound_CreateSoundStream ),

    // DirectSoundCreateStream+0x04 : and [ebp-0x04], 0
    { 0x04, 0x83 },
    { 0x05, 0x65 },
    { 0x06, 0xFC },

    { 0x20, 0x0C },
    { 0x23, 0x08 },
    { 0x26, 0xFC },

    // DirectSoundCreateStream+0x2E : lea eax, [ebp-0x04]
    { 0x2E, 0x8D },
    { 0x2F, 0x45 },
    { 0x30, 0xFC },

    // DirectSoundCreateStream+0x3B : retn 0x08
    { 0x3B, 0xC2 },
    { 0x3C, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_AddRef
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(IDirectSoundBuffer_AddRef, 3911, 11)

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
    
        // IDirectSoundBuffer_AddRef+0x13 : ret 4
        { 0x13, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Release
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(IDirectSoundBuffer_Release, 3911, 11)

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

        // IDirectSoundBuffer_Release+0x13 : ret 4
        { 0x13, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Unlock
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(IDirectSoundBuffer_Unlock, 3911, 5)

    // IDirectSoundBuffer_Unlock+0x00 : xor eax, eax
    { 0x00, 0x33 },
    { 0x01, 0xC0 },

    // IDirectSoundBuffer_Unlock+0x02 : ret 14h
    { 0x02, 0xC2 },
    { 0x03, 0x14 },
    { 0x04, 0x00 }
OOVPA_END;
#if 0 // Cannot be used since this OOVPA does produce false detection. Use it only for as a model understanding.
// ******************************************************************
// * IDirectSound_SetCooperativeLevel
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_NO_XREF(IDirectSound_SetCooperativeLevel, 3911, 5)

    // IDirectSound_SetCooperativeLevel+0x00 : xor eax, eax
    { 0x00, 0x33 },
    { 0x01, 0xC0 },

    // IDirectSound_SetCooperativeLevel+0x02 : ret 0Ch
    { 0x02, 0xC2 },
    { 0x03, 0x0C },
    { 0x04, 0x00 }
OOVPA_END;
#endif
// ******************************************************************
// * CMcpxVoiceClient_SetVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetVolume, 3911, 11,

    XREF_CMcpxVoiceClient_SetVolume,
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
OOVPA_XREF(CDirectSoundVoice_SetVolume, 3911, 11,

    XREF_CDirectSoundVoice_SetVolume,
    XRefOne)

        XREF_ENTRY( 0x1E, XREF_CMcpxVoiceClient_SetVolume ),

        //CDirectSoundVoice_SetVolume+0x00 : push esi
        { 0x00, 0x56 },

        //CDirectSoundVoice_SetVolume+0x11 : mov eax, dword ptr [ecx+18h]
        { 0x11, 0x8B },
        { 0x12, 0x41 },
        { 0x13, 0x18 },

        //CDirectSoundVoice_SetVolume+0x1A : mov ecx, dword ptr [ecx+14h]
        { 0x1A, 0x8B },
        { 0x1B, 0x49 },
        { 0x1C, 0x14 },

        //CDirectSoundVoice_SetVolume+0x1D : call CMcpxVoiceClient_SetVolume
        { 0x1D, 0xE8 },

        //CDirectSoundVoice_SetVolume+0x37 : ret 8
        { 0x37, 0xC2 },
        { 0x38, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetVolume, 3911, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetVolume+0x15 : call [CDirectSoundVoice_SetVolume]
        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetVolume),

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
// * CMcpxBuffer_Stop
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Stop, 3911, 10,

	XREF_CMcpxBuffer_Stop,
    XRefZero)

        // CMcpxBuffer_Stop+0x08 : or eax, dword ptr [esp+10h]
        { 0x08, 0x0B },
        { 0x09, 0x44 },
        { 0x0A, 0x24 },
        { 0x0B, 0x10 },

        // CMcpxBuffer_Stop+0x19 : push 3
        { 0x19, 0x6A },
        { 0x1A, 0x03 },

        // CMcpxBuffer_Stop+0x24 : test [esp+10h], 2
        { 0x24, 0x8B },
        { 0x25, 0xCE },

        // CMcpxBuffer_Stop+0x31 : retn 0x08
        { 0x31, 0xC2 },
        { 0x32, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_StopEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_StopEx, 3911, 11,

    XREF_CDirectSoundBuffer_StopEx,
    XRefOne)

        XREF_ENTRY( 0x39, XREF_CMcpxBuffer_Stop ),

        // CDirectSoundBuffer_StopEx+0x0F : test al, 3
        { 0x0F, 0xA8 },
        { 0x10, 0x03 },


        // CDirectSoundBuffer_StopEx+0x15 : and eax, 1
        { 0x15, 0x83 },
        { 0x16, 0xE0 },
        { 0x17, 0x01 },

        // CDirectSoundBuffer_StopEx+0x32 : push dword ptr [ebp+0Ch]
        { 0x32, 0xFF },
        { 0x33, 0x75 },
        { 0x34, 0x0C },

        // CDirectSoundBuffer_StopEx+0x53 : retn 0x10
        { 0x53, 0xC2 },
        { 0x54, 0x10 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_StopEx
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_StopEx, 3911, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_StopEx+0x1D : call [CDirectSoundBuffer::StopEx]
        XREF_ENTRY( 0x1D, XREF_CDirectSoundBuffer_StopEx ), 

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
        { 0x22, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Stop
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Stop, 3911, 13,

    XREF_CDirectSoundBuffer_Stop,
    XRefOne)

        XREF_ENTRY( 0x14, XREF_CDirectSoundBuffer_StopEx ),

        // CDirectSoundBuffer_Stop+0x07 : mov eax, 0x80004005
        { 0x07, 0x6A },
        { 0x08, 0x00 },
        { 0x09, 0x6A },
        { 0x0A, 0x00 },
        { 0x0B, 0x6A },
        { 0x0C, 0x00 },

        // CDirectSoundBuffer_Stop+0x1A : mov ebx, eax; jz +0x0B
        { 0x1A, 0x8B },
        { 0x1B, 0xD8 },
        { 0x1C, 0x74 },
        { 0x1D, 0x0B },

        // CDirectSoundBuffer_Stop+0x2D : retn 0x04
        { 0x2D, 0xC2 },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Stop
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_Stop, 3911, 12,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_Stop+0x11 : call [CDirectSoundBuffer::Stop]
        XREF_ENTRY( 0x11, XREF_CDirectSoundBuffer_Stop ), 

        // IDirectSoundBuffer_Stop+0x00 : mov eax, [esp+arg_0]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // IDirectSoundBuffer_Stop+0x04 : mov ecx, eax
        { 0x04, 0x8B },
        { 0x05, 0xC8 },

        // IDirectSoundBuffer_Stop+0x06 : add eax, 0x0FFFFFFE4
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0xE4 },

        // IDirectSoundBuffer_Stop+0x15 : retn 4
        { 0x16, 0x04 },
        { 0x17, 0x00 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetLoopRegion
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
//TODO: Need to make CDirectSoundBuffer_SetLoopRegion OOVPA for 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_SetLoopRegion, 3911, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_CDirectSoundBuffer_SetLoopRegion ),

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
// * CMcpxBuffer_Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play, 3911, 9,

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

// ******************************************************************
// * CDirectSoundBuffer_PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 3911, 9,

    XREF_CDirectSoundBuffer_PlayEx,
    XRefOne)

        XREF_ENTRY( 0x21, XREF_CMcpxBuffer_Play ),

        { 0x08, 0xF0 },

        { 0x13, 0x24 },
        { 0x17, 0x24 },
        { 0x1B, 0x24 },

        { 0x25, 0x85 },
        { 0x27, 0x8B },


        { 0x3A, 0xC2 },
        { 0x3B, 0x10 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 3911, 8,

    XREF_CDirectSoundBuffer_Play,
    XRefOne)

        XREF_ENTRY( 0x16, XREF_CDirectSoundBuffer_PlayEx ),

        { 0x07, 0xFF },
        { 0x0C, 0xF0 },
        { 0x13, 0x24 },
        { 0x1A, 0x85 },
        { 0x25, 0xFF },
        { 0x2B, 0x8B },
        { 0x2F, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_Play
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_Play, 3911, 16,

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

// ******************************************************************
// * IDirectSoundBuffer_PlayEx
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_PlayEx, 3911, 8,

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
// * CMcpxBuffer_GetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetCurrentPosition, 3911, 15,

    XREF_CMcpxBuffer_GetCurrentPosition,
    XRefZero)

        // CMcpxBuffer_GetCurrentPosition+0x12 : mov eax, [esi+XX]
        { 0x12, 0x8B },
        { 0x13, 0x46 },
        //{ 0x14, 0x18 }, //3911 has 0x18 while 3936 has 0x08

        // CMcpxBuffer_GetCurrentPosition+0x15 : and eax, 3
        { 0x15, 0x83 },
        { 0x16, 0xE0 },
        { 0x17, 0x03 },

        // CMcpxBuffer_GetCurrentPosition+0x18 : cmp al, 3
        { 0x18, 0x3C },
        { 0x19, 0x03 },

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
OOVPA_XREF(CDirectSoundBuffer_GetCurrentPosition, 3911, 9,

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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_GetCurrentPosition, 3911, 8,

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

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 3911, 13,

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
// * CDirectSoundBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_GetStatus, 3911, 10,

    XREF_CDirectSoundBuffer_GetStatus,
    XRefOne)

        // CDirectSoundBuffer_GetStatus+0x15 : call [CMcpxBuffer::GetStatus]
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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_GetStatus, 3911, 10,

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
// * CMcpxVoiceClient::SetFilter
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetFilter, 3911, 12,

    XREF_CMcpxVoiceClient_SetFilter,
    XRefZero)

        { 0x0E, 0xE8 },
        { 0x1F, 0x6A },

        { 0x73, 0x00 },
        { 0x74, 0xF6 },
        { 0x75, 0x47 },
        { 0x76, 0x0C },
        { 0x77, 0x10 },
        { 0x78, 0x8B },
        { 0x79, 0x7D },
        { 0x7A, 0xE0 },

        { 0xA6, 0x8B },
        { 0xA7, 0xE0 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetFilter
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFilter, 3911, 10,

    XREF_CDirectSoundVoice_SetFilter,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CMcpxVoiceClient_SetFilter ),

        { 0x01, 0x57 },
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x2E, 0xC2 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetFilter
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFilter, 3911, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFilter+0x15 : call [CDirectSoundVoice_SetFilter]
        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetFilter),

        // IDirectSoundBuffer_SetFilter+0x04 : push [esp+0x08]
        { 0x04, 0xFF },
        { 0x05, 0x74 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },

        // IDirectSoundBuffer_SetFilter+0x0A : add eax, 0xFFFFFFE4
        { 0x0A, 0x83 },
        { 0x0B, 0xC0 },
        { 0x0C, 0xE4 },

        // IDirectSoundBuffer_SetFilter+0x19 : retn 0x08
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetLFO
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetLFO, 3911, 12,

    XREF_CMcpxVoiceClient_SetLFO,
    XRefZero)

        { 0x10, 0x8B },
        { 0x21, 0xEB },

        { 0x7D, 0x75 },
        { 0x7E, 0x28 },
        { 0x7F, 0x39 },
        { 0x80, 0x05 },
        { 0x81, 0x10 },
        { 0x82, 0x00 },
        { 0x83, 0x82 },
        { 0x84, 0xFE },

        { 0xA1, 0x89 },
        { 0xB7, 0x6C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetLFO
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetLFO, 3911, 10,

    XREF_CDirectSoundVoice_SetLFO,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CMcpxVoiceClient_SetLFO ),

        { 0x01, 0x57 },
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x2E, 0xC2 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetLFO
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetLFO, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetLFO ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient::SetEG
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetEG, 3911, 12,

    XREF_CMcpxVoiceClient_SetEG,
    XRefZero)

        { 0x10, 0x8B },
        { 0x21, 0xEB },

        { 0x6B, 0x82 },
        { 0x6C, 0xFE },

        { 0x95, 0x8B },
        { 0x96, 0x96 },
        { 0x97, 0x8C },
        { 0x98, 0x00 },
        { 0x99, 0x00 },
        { 0x9A, 0x00 },
        { 0x9B, 0x89 },
        { 0x9C, 0x15 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetEG
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetEG, 3911, 10,

    XREF_CDirectSoundVoice_SetEG,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CMcpxVoiceClient_SetEG ),

        { 0x01, 0x57 },
        { 0x0D, 0x8B },
        { 0x0E, 0x44 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },
        { 0x11, 0x8B },
        { 0x12, 0x48 },
        { 0x2E, 0xC2 },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetEG
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetEG, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetEG ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#if 0 // Disabled due to both functions are not unique and unable to identify the difference.
// ******************************************************************
// * IDirectSoundStream_SetEG & CDirectSoundStream_SetEG
// ******************************************************************
OOVPA_XREF(IDirectSoundStream_SetEG, 3911, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0xD, XREF_CDirectSoundVoice_SetEG ),

        { 0x02, 0x24 },
        { 0x03, 0x08 },
        { 0x06, 0x24 },
        { 0x07, 0x08 },
        { 0x08, 0x83 },
        { 0x0C, 0xE8 },
        { 0x11, 0xC2 },
        { 0x12, 0x08 },
OOVPA_END;
#endif
// ******************************************************************
// * IDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
//Generic OOVPA as of 3911 and newer
OOVPA_XREF(IDirectSoundBuffer_SetOutputBuffer, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetOutputBuffer ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetMixBinVolumes, 3911, 8,

    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XRefOne)

        XREF_ENTRY( 0x54, XREF_CMcpxVoiceClient_SetVolume ),

        { 0x0D, 0x65 },
        { 0x1C, 0x53 },
        { 0x2B, 0xC0 },
        { 0x3A, 0xC7 },
        { 0x49, 0xFC },
        { 0x58, 0x83 },
        { 0x67, 0x15 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMixBinVolumes_12
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBinVolumes_12, 3911, 9,

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
// * CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes_12, 3911, 9,

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
// * CMcpxAPU_Commit3dSettings
// ******************************************************************
OOVPA_XREF(CMcpxAPU_Commit3dSettings, 3911, 11,

    XREF_CMcpxAPU_Commit3dSettings,
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
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 3911, 10,

    XREF_CDirectSound_CommitDeferredSettings,
    XRefOne)

        // CDirectSound_CommitDeferredSettings+0x12 : call [CMcpxAPU_Commit3dSettings]
        XREF_ENTRY( 0x12, XREF_CMcpxAPU_Commit3dSettings ),

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
//Generic OOVPA as of 3911 and newer
OOVPA_XREF(IDirectSound_CommitDeferredSettings, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CommitDeferredSettings+0x11 : call [CDirectSound_CommitDeferredSettings]
        XREF_ENTRY( 0x11, XREF_CDirectSound_CommitDeferredSettings ), 

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
// * CDirectSound_SetOrientation
// ******************************************************************
OOVPA_XREF(CDirectSound_SetOrientation, 3911, 13,

    XREF_CDirectSound_SetOrientation,
    XRefZero)

        // CDirectSound_SetOrientation+0x00 : push ebp
        { 0x00 , 0x55 },

        { 0x0F , 0x8B },
        { 0x11 , 0x0C },
        { 0x15 , 0x8B },
        { 0x17 , 0x10 },
        { 0x1B , 0x8B },
        { 0x1D , 0x14 },
        { 0x21 , 0x8B },
        { 0x23 , 0x18 },
        { 0x27 , 0x8B },
        { 0x29 , 0x1C },
        { 0x2D , 0x8B },
        { 0x2F , 0x20 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_SetOrientation
// ******************************************************************
//Generic OOVPA as of 3911 and newer
OOVPA_XREF(IDirectSound_SetOrientation, 3911, 15,
    XRefNoSaveIndex,
    XRefOne)

        // IDirectSound_CommitDeferredSettings+0x11 : call [CDirectSound_CommitDeferredSettings]
        XREF_ENTRY( 0x42, XREF_CDirectSound_SetOrientation ), 

        // IDirectSound_SetOrientation+0x00 : push ebp
        { 0x00 , 0x55 },

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
// * IDirectSound_AddRef
// ******************************************************************
//Generic OOVPA as of 3911 and newer
OOVPA_NO_XREF(IDirectSound_AddRef, 3911, 12)

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
// * IDirectSound_Release
// ******************************************************************
//Generic OOVPA as of 3911 and newer
OOVPA_NO_XREF(IDirectSound_Release, 3911, 10)

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
// * CMcpxVoiceClient_SetPitch
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetPitch, 3911, 11,

    XREF_CMcpxVoiceClient_SetPitch,
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
OOVPA_XREF(CDirectSoundVoice_SetPitch, 3911, 12,

    XREF_CDirectSoundVoice_SetPitch,
    XRefOne)

        // CDirectSoundVoice_SetPitch+0x1B : call [CMcpxVoiceClient_SetPitch]
        XREF_ENTRY( 0x1B, XREF_CMcpxVoiceClient_SetPitch ),

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
OOVPA_XREF(IDirectSoundBuffer_SetPitch, 3911, 10,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetPitch+0x15 : call [CDirectSoundVoice_SetPitch]
        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetPitch ),

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
// * XAudioCalculatePitch
// ******************************************************************
OOVPA_XREF(XAudioCalculatePitch, 3911, 11,

    XREF_XAudioCalculatePitch,
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
// * DirectSoundGetSampleTime
// ******************************************************************
OOVPA_NO_XREF(DirectSoundGetSampleTime, 3911, 6)

        { 0x00, 0xA1 },
        { 0x01, 0x0C },
        { 0x02, 0x20 },
        { 0x03, 0x80 },
        { 0x04, 0xFE },
        { 0x05, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CSensaura3d::GetFullHRTFFilterPair
// ******************************************************************
OOVPA_XREF(CSensaura3d_GetFullHRTFFilterPair, 3911, 7,

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
// * DirectSoundUseFullHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseFullHRTF, 3911, 8,

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
// * CSensaura3d_GetLiteHRTFFilterPair
// ******************************************************************
OOVPA_XREF(CSensaura3d_GetLiteHRTFFilterPair, 3911, 10,

    XREF_CSensaura3d_GetLiteHRTFFilterPair,
    XRefZero)

        //CSensaura3d_GetLightHRTFFilterPair+0x00 : push ebp
        { 0x00, 0x55 },

        //CSensaura3d_GetLightHRTFFilterPair+0x14 : push ecx
        { 0x15, 0x51 },

        //CSensaura3d_GetLightHRTFFilterPair+0x22 : push 3
        { 0x1F, 0x6A },
        { 0x20, 0x03 },

        //CSensaura3d_GetLightHRTFFilterPair+0x22 : idiv eax, ecx
        { 0x22, 0xF7 },
        { 0x23, 0xF9 },

        //CSensaura3d_GetLightHRTFFilterPair+0x24 : cmp dword ptr [ebp+10h], 0
        { 0x24, 0x83 },
        { 0x25, 0x7D },
        { 0x26, 0x10 },
        { 0x27, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSoundUseLightHRTF
// ******************************************************************
OOVPA_XREF(DirectSoundUseLightHRTF, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x0D, XREF_CSensaura3d_GetLiteHRTFFilterPair ),

        { 0x05, 0x85 },
        { 0x06, 0xC0 },
        { 0x11, 0x74 },
        { 0x12, 0x0B },
        { 0x13, 0x68 },
        { 0x18, 0xFF },
        { 0x1E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_SetFrequency
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetFrequency, 3911, 9,

    XREF_DSSTREAMSETFREQUENCY1A,
    XRefOne)

        // CDirectSoundVoice_SetFrequency+0x1F : call [XAudioCalculatePitch]
        XREF_ENTRY( 0x20, XREF_XAudioCalculatePitch ),

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
// * IDirectSoundBuffer_SetFrequency
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetFrequency, 3911, 8,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetFrequency+0x14 : call [CDirectSoundVoice_SetFrequency]
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
// * CMcpxVoiceClient::SetMixBins
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_SetMixBins, 3911, 15,

    XREF_DSSETMIXBINSB,
    XRefZero)

        // CMcpxVoiceClient_SetMixBins+0x1D : lea eax, [esi+0x84]
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
OOVPA_XREF(CDirectSoundVoice_SetMixBins, 3911, 9,

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
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMixBins, 3911, 8,

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
// * CDirectSoundVoice::SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetOutputBuffer, 3911, 7,

    XREF_CDirectSoundVoice_SetOutputBuffer,
    XRefZero)

        { 0x0B, 0xF7 },
        { 0x19, 0x8B },
        { 0x25, 0x3B },
        { 0x30, 0xE8 },
        { 0x35, 0x8B },
        { 0x46, 0x74 },
        { 0x51, 0xD8 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetOutputBuffer
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetOutputBuffer, 3911, 13,

    XREF_CDirectSoundBuffer_SetOutputBuffer,
    XRefOne)

        // CDirectSoundBuffer_SetOutputBuffer+0x11 : call [XREF_CDirectSoundVoice_SetOutputBuffer]
        XREF_ENTRY( 0x12, XREF_CDirectSoundVoice_SetOutputBuffer),

        // CDirectSoundBuffer_SetOutputBuffer+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSoundBuffer_SetOutputBuffer+0x07 : push [esp+0x10]
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x09, 0x24 },
        { 0x0A, 0x10 },

        // CDirectSoundBuffer_SetOutputBuffer+0x0D : push [esp+0x10]
        { 0x0D, 0xFF },
        { 0x0E, 0x74 },
        { 0x0F, 0x24 },
        { 0x10, 0x10 },

        // CDirectSoundBuffer_SetOutputBuffer+0x29 : pop edi
        { 0x29, 0x5F },

        // CDirectSoundBuffer_SetOutputBuffer+0x2B : retn 0x08
        { 0x2B, 0xC2 },
        { 0x2C, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 3911, 9,

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
OOVPA_XREF(IDirectSoundBuffer_SetBufferData, 3911, 10,

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
// * CDirectSoundBuffer::SetNotificationPositions
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetNotificationPositions, 3911, 7,

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
OOVPA_XREF(IDirectSoundBuffer_SetNotificationPositions, 3911, 9,

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
// * CDirectSoundVoice_SetHeadroom
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetHeadroom, 3911, 12,

    XREF_DSBUFFERSETHEADROOMA,
    XRefOne)

    // WARNING: I am unsure if this is SetHeadroom

        // CDirectSoundVoice_SetHeadroom+0x23 : call [CMcpxVoiceClient_SetVolume]
        XREF_ENTRY( 0x23, XREF_CMcpxVoiceClient_SetVolume ),

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
OOVPA_XREF(IDirectSoundBuffer_SetHeadroom, 3911, 10,

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

//TODO: Need to create XREF to CMcpxStream_Pause in order strengthen OOVPA chain.
// ******************************************************************
// * CDirectSoundStream_Pause
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_Pause, 3911, 6)

        { 0x01, 0x44 },
        { 0x04, 0xFF },
        { 0x07, 0x08 },
        { 0x0A, 0x24 },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Process
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_Process, 3911, 16)

        { 0x01, 0x8B },
        { 0x04, 0x08 },

        { 0x05, 0x8B },
        { 0x07, 0x24 },

        { 0x0D, 0x85 },
        { 0x0E, 0xC0 },

        //CDirectSoundStream_Process+0x11 : mov eax, 88780032h
        { 0x12, 0x32 },
        { 0x13, 0x00 },
        { 0x14, 0x78 },
        { 0x15, 0x88 },

        { 0x18, 0xFF },
        { 0x1B, 0x0C },

        { 0x1C, 0x8B },
        { 0x1E, 0x24 },

        { 0x25, 0xC2 },
        { 0x26, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_GetStatus
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_GetStatus, 3911, 13)

        { 0x08, 0x8B },
        { 0x0B, 0x08 },

        { 0x0C, 0x8B },
        { 0x0E, 0x24 },

        { 0x14, 0x8B },
        { 0x17, 0x0C },

        { 0x18, 0xF7 },
        { 0x1A, 0x1B },
        { 0x1C, 0xF7 },

        //CDirectSoundStream_GetStatus+0x20 : mov dword ptr [ecx], eax
        { 0x20, 0x89 },
        { 0x21, 0x01 },

        { 0x32, 0xC2 },
        { 0x33, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Flush
// ******************************************************************
OOVPA_XREF(CMcpxStream_Flush, 3911, 11,
    XREF_CMcpxStream_Flush,
    XRefZero)

        { 0x0D, 0x03 },
        { 0x0F, 0x03 },

        { 0x20, 0x33 },
        { 0x22, 0x33 },

        { 0x2D, 0x83 },
        { 0x2E, 0xFF },
        { 0x2F, 0x03 },

        { 0x4B, 0xE8 },

        { 0x62, 0xE8 },


        { 0x9D, 0xC9 },
        { 0x9E, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Flush
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Flush, 3911, 9,
    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Flush+0x10 : call [CMcpxStream_Flush]
        XREF_ENTRY( 0x11, XREF_CMcpxStream_Flush ),

        { 0x00, 0x56 },

        { 0x09, 0x8B },
        { 0x0C, 0x0C },

        { 0x0D, 0x8B },
        { 0x0F, 0x24 },

        { 0x10, 0xE8 },

        //CDirectSoundStream_Flush+0x2A : ret 4
        { 0x2A, 0xC2 },
        { 0x2B, 0x04 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Discontinuity
// ******************************************************************
OOVPA_XREF(CMcpxStream_Discontinuity, 3911, 12,
    XREF_CMcpxStream_Discontinuity,
    XRefOne)

        //CMcpxStream_Discontinuity+0x66 : call [CMcpxStream_Flush]
        XREF_ENTRY ( 0x67, XREF_CMcpxStream_Flush ),

        { 0x0D, 0x03 },
        { 0x0F, 0x03 },

        { 0x1B, 0x33 },
        { 0x1D, 0x8D },

        { 0x25, 0x0F },
        { 0x26, 0x94 },
        { 0x27, 0xC1 },

        { 0x64, 0x8B },

        { 0x66, 0xE8 },


        { 0x6E, 0xC9 },
        { 0x6F, 0xC3 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_Discontinuity
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_Discontinuity, 3911, 9,
    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Discontinuity+0x10 : call [CMcpxStream_Discontinuity]
        XREF_ENTRY( 0x11, XREF_CMcpxStream_Discontinuity ),

        { 0x00, 0x56 },

        { 0x09, 0x8B },
        { 0x0C, 0x0C },

        { 0x0D, 0x8B },
        { 0x0F, 0x24 },

        { 0x10, 0xE8 },

        //CDirectSoundStream_Discontinuity+0x2A : ret 4
        { 0x2A, 0xC2 },
        { 0x2B, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_GetInfo
// ******************************************************************
OOVPA_NO_XREF(CDirectSoundStream_GetInfo, 3911, 17)

        //CDirectSoundStream_GetInfo+0x00 : push ebx
        { 0x00, 0x53 },

        //CDirectSoundStream_GetInfo+0x0C : mov edi, dword ptr [esp + 10h]
        { 0x0C, 0x8B },
        { 0x0D, 0x7C },
        { 0x0E, 0x24 },
        { 0x0F, 0x10 },

        //CDirectSoundStream_GetInfo+0x16 : mov, dword ptr [edi + 24h]
        { 0x16, 0x8B },
        { 0x17, 0x4F },
        { 0x18, 0x24 },

        //CDirectSoundStream_GetInfo+0x23 : mov eax, dword ptr [edi + 20h]
        { 0x23, 0x8B },
        { 0x24, 0x47 },
        { 0x25, 0x20 },

        //CDirectSoundStream_GetInfo+0x2D : and dword ptr [esi + 8], 0
        { 0x2D, 0x83 },
        { 0x2E, 0x66 },
        { 0x2F, 0x08 },
        { 0x30, 0x00 },

        //CDirectSoundStream_GetInfo+0x48 : ret 8
        { 0x48, 0xC2 },
        { 0x49, 0x08 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_Set3dMaxDistance
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dMaxDistance, 3911, 12,

    XREF_CMcpxVoiceClient_Set3dMaxDistance,
    XRefZero)

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
OOVPA_XREF(CDirectSoundVoice_SetMaxDistance, 3911, 15,

    XREF_CDirectSoundVoice_SetMaxDistance,
    XRefOne)

        // CDirectSoundVoice_SetMaxDistance+0x23 : call [CMcpxVoiceClient_Set3dMaxDistance]
        XREF_ENTRY( 0x23, XREF_CMcpxVoiceClient_Set3dMaxDistance ),

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
// * CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 3911, 9,

    XREF_CDirectSoundBuffer_SetMaxDistance,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundVoice_SetMaxDistance ),

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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 3911, 8,

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
// * CDirectSoundStream_SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMaxDistance, 3911, 10,

    XREF_CDirectSoundStream_SetMaxDistance,
    XRefOne)

        // CDirectSoundStream_SetMaxDistance+0x15 : call [CDirectSoundVoice_SetMaxDistance]
        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetMaxDistance ),

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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundStream_SetMaxDistance, 3911, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMaxDistance+0x11 : call [CDirectSoundStream_SetMaxDistance]
        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_SetMaxDistance ),

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
OOVPA_XREF(CMcpxVoiceClient_Set3dMinDistance, 3911, 12,

    XREF_CMcpxVoiceClient_Set3dMinDistance,
    XRefZero)

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
OOVPA_XREF(CDirectSoundVoice_SetMinDistance, 3911, 15,

    XREF_CDirectSoundVoice_SetMinDistance,
    XRefOne)

        // CDirectSoundVoice_SetMinDistance+0x23 : call [CMcpxVoiceClient_Set3dMinDistance]
        XREF_ENTRY( 0x23, XREF_CMcpxVoiceClient_Set3dMinDistance ),

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
// * CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 3911, 9,

    XREF_CDirectSoundBuffer_SetMinDistance,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSoundVoice_SetMinDistance ),

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
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 3911, 8,

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
// * CDirectSoundStream_SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMinDistance, 3911, 10,

    XREF_CDirectSoundStream_SetMinDistance,
    XRefOne)

        // CDirectSoundStream_SetMinDistance+0x15 : call [CDirectSoundVoice_SetMinDistance]
        XREF_ENTRY( 0x15, XREF_CDirectSoundVoice_SetMinDistance ),

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
// * IDirectSoundStream_SetMinDistance
// ******************************************************************
//Generic OOVPA as of 3911 and newer.
OOVPA_XREF(IDirectSoundStream_SetMinDistance, 3911, 11,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetMinDistance+0x11 : call [CDirectSoundStream_SetMinDistance]
        XREF_ENTRY( 0x11, XREF_CDirectSoundStream_SetMinDistance ),

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
// * CMcpxVoiceClient_Set3dConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CMcpxVoiceClient_Set3dConeOutsideVolume, 3911, 12,

    XREF_CMcpxVoiceClient_Set3dConeOutsideVolume,
    XRefZero)

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
OOVPA_XREF(CDirectSoundVoice_SetConeOutsideVolume, 3911, 12,

    XREF_CDirectSoundVoice_SetConeOutsideVolume,
    XRefOne)

        // CDirectSoundVoice_SetConeOutsideVolume+0x1F : call [CMcpxVoiceClient_Set3dConeOutsideVolume]
        XREF_ENTRY( 0x1F, XREF_CMcpxVoiceClient_Set3dConeOutsideVolume ),

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
// * CDirectSoundBuffer::SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetConeOutsideVolume, 3911, 2,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundBuffer_SetConeOutsideVolume+0x00 : jmp CDirectSoundVoice_SetConeOutsideVolume
        XREF_ENTRY( 0x1, XREF_CDirectSoundVoice_SetConeOutsideVolume ),

        // CDirectSoundBuffer_SetConeOutsideVolume+0x00 : jmp
        { 0x00, 0xE9 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetConeOutsideVolume, 3911, 9,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundBuffer_SetConeOutsideVolume+0x18 : call [CDirectSoundVoice_SetConeOutsideVolume]
        XREF_ENTRY(0x19, XREF_CDirectSoundVoice_SetConeOutsideVolume ),

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
// * CDirectSoundStream_SetConeOutsideVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetConeOutsideVolume, 3911, 10,

    XRefNoSaveIndex,
    XRefOne)

        // CDirectSoundStream_SetConeOutsideVolume+0x11 : call [CDirectSoundVoice_SetConeOutsideVolume]
        XREF_ENTRY( 0x11, XREF_CDirectSoundVoice_SetConeOutsideVolume ),

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
