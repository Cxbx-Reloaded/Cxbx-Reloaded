// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5788.cpp
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
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData, 5788, 9,

	XREF_CMcpxBuffer_SetBufferData,
    XRefZero)

        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B }, // (Offset,Value)-Pair #1
        { 0x1F, 0xC3 }, // (Offset,Value)-Pair #2

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 }, // (Offset,Value)-Pair #3
        { 0x21, 0x0C }, // (Offset,Value)-Pair #4

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xBC], ecx
        { 0x45, 0x89 }, // (Offset,Value)-Pair #5
        { 0x46, 0x8E }, // (Offset,Value)-Pair #6
        { 0x47, 0xBC }, // (Offset,Value)-Pair #7

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 }, // (Offset,Value)-Pair #8
        { 0x8A, 0x08 }, // (Offset,Value)-Pair #9
OOVPA_END;
#if 0 // Used CMcpxBuffer_Play 4831
// ******************************************************************
// * DirectSound::CMcpxBuffer::Play
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Play2, 5788, 7,

    XREF_CMcpxBuffer_Play2,
    XRefZero)

	{ 0x17, 0x43 },
	{ 0x2D, 0xF6 },
	{ 0x2E, 0xC3 },
	{ 0x2F, 0x04 },
        { 0xAD, 0xC2 },
	{ 0xAE, 0x04 },
	{ 0xAF, 0x00 },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::Play
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_Play, 5788, 9,

    XREF_CDirectSoundBuffer_Play,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x2E, 0x20 },
        { 0x33, 0x18 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 5788, 7,

    XREF_COMMITDEFERREDSETTINGSA,
    XRefZero)

        { 0x12, 0xC0 },
        { 0x27, 0xB8 },
        { 0x3A, 0x75 },
        { 0x52, 0x8D },
        { 0x62, 0x8B },
        { 0x76, 0x89 },
        { 0x8E, 0xFF },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 5788, 7,

    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XRefZero)

        { 0x1D, 0x00 },
        { 0x3C, 0x66 },
        { 0x5B, 0x00 },
        { 0x7C, 0x8D },
        { 0x99, 0xD1 },
        { 0xB8, 0x89 },
        { 0xDB, 0x5F },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetCurrentPosition, 5788, 8,

    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XRefOne)

        XREF_ENTRY( 0x35, XREF_DSBUFFERSETCURRENTPOSITIONB ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x39, 0x85 },
        { 0x44, 0xFF },
        { 0x4B, 0xC7 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_DSBUFFERSETCURRENTPOSITIONA ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 5788, 9,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x6C },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetDopplerFactor
// ******************************************************************
OOVPA_XREF(CDirectSound_SetDopplerFactor, 5788, 9,

    XREF_SETDOPPLERFACTOR,
    XRefZero)

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x08 },
        { 0x35, 0x70 },
        { 0x3A, 0x48 },
        { 0x4A, 0x85 },
        { 0x53, 0xFF },
        { 0x5E, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMaxDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMaxDistance, 5788, 8,

    XREF_DSBUFFERSETMAXDISTANCE,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETMAXDISTANCE ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMinDistance
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetMinDistance, 5788, 8,

    XREF_DSBUFFERSETMINDISTANCE,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETMINDISTANCE ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMAXDISTANCE ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETMINDISTANCE ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;
#if 0 // Not 5788
// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetVolume, 5788, 7,

    XREF_CDirectSoundVoice_SetVolume,
    XRefZero)

        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif
#if 0 // Used 5233
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetVolume
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 5788, 8,

    XREF_CDirectSoundBuffer_SetVolume,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_SetVolume ),

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
OOVPA_END;
#endif
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetVolume
// ******************************************************************
// * Not until we need it!
//OOVPA_XREF(CDirectSoundStream_SetVolume, 5788, 9,
//
//    XREF_CDirectSoundStream_SetVolume,
//    XRefOne)
//
//        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetVolume ),
//
//        { 0x00, 0x56 },
//        { 0x0C, 0x00 },
//        { 0x14, 0x74 },
//        { 0x21, 0xB8 },
//        { 0x2A, 0x24 },
//        { 0x35, 0xE8 },
//        { 0x40, 0x68 },
//        { 0x4B, 0x8B },
//OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::PlayEx
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_PlayEx, 5788, 8,

    XREF_CDirectSoundBuffer_PlayEx,
    XRefOne)

        XREF_ENTRY( 0x3D, XREF_CMcpxBuffer_Play2 ),

        { 0x0C, 0x00 },
        { 0x16, 0x68 },
        { 0x22, 0x05 },
        { 0x2E, 0x20 },
        { 0x3A, 0x24 },
        { 0x46, 0x0B },
        { 0x52, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_SetRolloffFactor, 5788, 7,

    XREF_DSVOICESETROLLOFFFACTOR,
    XRefZero)

        { 0x05, 0x41 },
        { 0x0C, 0x00 },
        { 0x13, 0x48 },
        { 0x1A, 0x00 },
        { 0x21, 0xF6 },
        { 0x28, 0x51 },
        { 0x2F, 0xC0 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetRolloffFactor
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetRolloffFactor, 5788, 8,

    XREF_DSBUFFERSETROLLOFFFACTOR,
    XRefOne)

        XREF_ENTRY( 0x3A, XREF_DSVOICESETROLLOFFFACTOR ),

        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xFF },
        { 0x40, 0x8B },
        { 0x4F, 0x8B },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_SetRolloffFactor
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetRolloffFactor, 5788, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_DSBUFFERSETROLLOFFFACTOR ),

        { 0x03, 0x0C },
        { 0x08, 0x8B },
        { 0x0D, 0x8B },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IDirectSound_CreateSoundStream
// ******************************************************************
//OOVPA_XREF(IDirectSound_CreateSoundStream, 5788, 8,
//
//    XRefNoSaveIndex,
//    XRefOne)
//
//        XREF_ENTRY( 0x1D, XREF_CDirectSound_CreateSoundStream ),
//
//        { 0x03, 0x10 },
//        { 0x08, 0xFF },
//        { 0x0D, 0xC8 },
//        { 0x12, 0x83 },
//        { 0x17, 0x1B },
//        { 0x1C, 0xE8 },
//        { 0x21, 0xC2 },
//OOVPA_END;

// ******************************************************************
// * DirectSoundCreateStream
// ******************************************************************
OOVPA_XREF(DirectSoundCreateStream, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x2F, XREF_DSCREATESOUNDSTREAM ),

        { 0x09, 0x56 },
        { 0x14, 0x45 },
        { 0x1F, 0x8B },
        { 0x2A, 0xFF },
        { 0x35, 0x85 },
        { 0x40, 0xFF },
        { 0x4E, 0x5F },
        { 0x56, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes2, 5788, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSSETMIXBINVOLUMESB ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxStream::Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 5788, 7,

    XREF_DSSTREAMPAUSE,
    XRefZero)

        { 0x10, 0x07 },
        { 0x23, 0x0F },
        { 0x34, 0x83 },
        { 0x46, 0xF8 },
        { 0x58, 0x03 },
        { 0x6A, 0xBB },
        { 0x7C, 0x00 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::FlushEx
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_FlushEx, 5788, 7,

    XREF_DSFLUSHEX2,
    XRefZero)

        { 0x0F, 0x00 },
        { 0x1E, 0xFF },
        { 0x28, 0x80 },
        { 0x36, 0x74 },
        { 0x46, 0x8B },
        { 0x52, 0xFF },
        { 0x62, 0x8B },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
OOVPA_NO_XREF(CMemoryManager_PoolAlloc, 5788, 10)

	{ 0x24, 0x83 },
	{ 0x25, 0x7C },
	{ 0x26, 0x24 },
	{ 0x27, 0x10 },
	{ 0x28, 0x00 },
	{ 0x37, 0xF3 },
	{ 0x38, 0xAB },
	{ 0x44, 0xC2 },
	{ 0x45, 0x0C },
	{ 0x46, 0x00 },
OOVPA_END;

// ******************************************************************
// * XFileCreateMediaObjectAsync
// ******************************************************************
OOVPA_NO_XREF(XFileCreateMediaObjectAsync, 5788, 8)

        { 0x0A, 0xC0 },
        { 0x16, 0xF8 },
        { 0x22, 0xE6 },
        { 0x2E, 0x20 },
        { 0x3D, 0x8B },
        { 0x49, 0x8B },
        { 0x52, 0x06 },
        { 0x5E, 0x0C },
OOVPA_END;

// ******************************************************************
// * Rollback support signature(s)
// ******************************************************************
#define CDirectSoundBuffer_SetBufferData_5788 CDirectSoundBuffer_SetBufferData_4134
#define IDirectSoundBuffer_SetBufferData_5788 IDirectSoundBuffer_SetBufferData_4134
#define XAudioDownloadEffectsImage_5788 XAudioDownloadEffectsImage_4627

// ******************************************************************
// * DSound_5788
// ******************************************************************
OOVPATable DSound_5788[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetBufferData, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetBufferData, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetBufferData, 4134, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_GetStatus, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_GetStatus, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetStatus, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetI3DL2Listener, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetI3DL2Listener, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFormat, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFormat, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFormat, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetConeAngles, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetConeAngles, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetConeAngles, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffCurve, 5344, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffCurve, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetRolloffCurve, 5558, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffCurve, 4627, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4039, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 4134, PATCH),
	REGISTER_OOVPA(IDirectSound_Release, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_DownloadEffectsImage, 3936, PATCH),
	REGISTER_OOVPA(IDirectSound_SetOrientation, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMaxDistance, 5344, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMaxDistance, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMaxDistance, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMinDistance, 5344, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMinDistance, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMinDistance, 5788, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Stop, 4361, XREF),
	REGISTER_OOVPA(CMcpxBuffer_Stop2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_StopEx, 4361, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_StopEx, 4361, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 3936, PATCH), // (Possibly weak, but quite OK for 4627 DSOUND)
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH),
	REGISTER_OOVPA(CFullHRTFSource_GetCenterVolume, 5558, XREF),
	REGISTER_OOVPA(CHRTFSource_SetFullHRTF5Channel, 5558, XREF),
	REGISTER_OOVPA(DirectSoundUseFullHRTF, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetHeadroom, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBins, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBins, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBins, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBins, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBins, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_SetMixBinHeadroom, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetMixBinHeadroom, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_SetPosition, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetPosition, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_SetVelocity, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetVelocity, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDistanceFactor, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDistanceFactor, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDistanceFactor, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 5788, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4242, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 5788, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetLFO, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLFO, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLFO, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPlayRegion, 4361, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactor, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH), // TODO : Use 5344?
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 4039, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 3936, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 4134, XREF),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 5233, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVolume, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4039, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetVolume, 4134, PATCH), // This is actually the XREF we are using
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFrequency, 5233, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetDopplerFactor, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetDopplerFactor, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetDopplerFactor, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPosition, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPosition, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPosition, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetVelocity, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVelocity, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVelocity, 5558, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Pause, 4831, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Pause, 4928, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Pause, 4928, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 5788, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_FlushEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 4627, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_SynchPlayback, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_EnableHeadphones, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_EnableHeadphones, 5558, PATCH),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(CDirectSound_GetOutputLevels, 5558, XREF),
	REGISTER_OOVPA(CDirectSound_GetOutputLevels, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_GetOutputLevels, 5558, PATCH),
	REGISTER_OOVPA(IDirectSound_SetEffectData, 5344, PATCH),
	REGISTER_OOVPA(CMemoryManager_PoolAlloc, 5788, XREF),
	REGISTER_OOVPA(XFileCreateMediaObjectAsync, 5788, PATCH),
	REGISTER_OOVPA(WaveFormat_CreateXboxAdpcmFormat, 5344, XREF),
	REGISTER_OOVPA(XAudioCreateAdpcmFormat, 5344, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFormat, 4242, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetHeadroom, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 5558, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 5558, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetEG, 4627, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetFilter, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFilter, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 5344, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMode, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 5233, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Use3DVoiceData, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetNotificationPositions, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Unlock, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_GetCaps, 4361, XREF),
	REGISTER_OOVPA(IDirectSound_GetCaps, 3936, PATCH),
};

// ******************************************************************
// * DSound_5788_SIZE
// ******************************************************************
uint32 DSound_5788_SIZE = sizeof(DSound_5788);
