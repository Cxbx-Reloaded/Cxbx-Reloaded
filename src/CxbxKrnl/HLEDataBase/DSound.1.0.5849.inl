// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.5849.cpp
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
// * DirectSound::CDirectSound::SetRolloffFactor
// Xbe Explorer generated pattern, derived from address $00439F47 in "SpiderMan 2" :
// 56E8........833DD4........0FB6F0741685F6740B68E03B4400FF15...... 00 0000 005F ? SetRolloffFactor@CDirectSound@DirectSound@@QAGJMK@Z ^ 0002R ? DirectSoundEnterCriticalSection@@YGHXZ ^0009D _g_fDirectSoundInFinalRelease ^ 001DD __imp__RtlLeaveCriticalSection@4 ^ 0046R ? CommitDeferredSettings@CDirectSound@DirectSound@@QAGJXZ ^ 0055D __imp__RtlLeaveCriticalSection@4
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 5849, 20,

    XREF_SETROLLOFFFACTORA,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0xE8 },
        { 0x06, 0x83 },
        { 0x07, 0x3D },
        { 0x0D, 0x0F },
        { 0x0E, 0xB6 },
		{ 0x0F, 0xF0 },
		{ 0x10, 0x74 },
		{ 0x11, 0x16 },
		{ 0x12, 0x85 },
		{ 0x13, 0xF6 },
		{ 0x14, 0x74 },
		{ 0x15, 0x0B },
		{ 0x16, 0x68 },
		{ 0x1A, 0x00 },
		{ 0x1B, 0xFF },
		{ 0x1C, 0x15 },
		{ 0x35, 0x6C },
		{ 0x3C, 0x10 },
		{ 0x5C, 0xC2 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 5849, 12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_DSBUFFERSETPITCHB ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
	{ 0x4F, 0xC2 },
	{ 0x50, 0x08 },
	{ 0x51, 0x00 },
OOVPA_END;

// ******************************************************************
// * DSound_5849
// ******************************************************************
OOVPATable DSound_5849[] = {

	REGISTER_OOVPA(DirectSoundCreate, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_CreateSoundBuffer, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPlayRegion, 5558, XREF),
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
	REGISTER_OOVPA(CDirectSoundVoiceSettings_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMixBinVolumes, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMixBinVolumes2, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetMixBinVolumes2, 5788, PATCH),
	REGISTER_OOVPA(IDirectSound_CreateSoundBuffer, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFrequency, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFrequency, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetVolume, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetVolume, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetVolume, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetVolume, 4134, PATCH),
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
	REGISTER_OOVPA(IDirectSoundBuffer_Stop, 4134, PATCH), // (Possibly weak, but quite OK for 4627 DSOUND)
	REGISTER_OOVPA(IDirectSoundBuffer_Release, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetHeadroom, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetHeadroom, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetHeadroom, 5558, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_Lock, 5558, PATCH),
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
	REGISTER_OOVPA(CDirectSound_CommitDeferredSettings, 5788, PATCH),
	REGISTER_OOVPA(DirectSoundCreateBuffer, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetCurrentPosition, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetCurrentPosition, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_GetCurrentPosition, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_GetCurrentPosition, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetLoopRegion, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetLoopRegion, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetRolloffFactor, 5849, XREF),
	REGISTER_OOVPA(IDirectSound_SetRolloffFactor, 4134, PATCH), // TODO : Use 5344?
	REGISTER_OOVPA(CDirectSound_SetDopplerFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSound_SetDopplerFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetPitch, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetPitch, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetPitch, 4627, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 4361, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_PlayEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_PlayEx, 3936, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetRolloffFactor, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetRolloffFactor, 5788, PATCH),
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
	REGISTER_OOVPA(CDirectSound_CreateSoundStream, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_CreateSoundStream, 5558, PATCH),
	REGISTER_OOVPA(DirectSoundCreateStream, 5788, PATCH),
	REGISTER_OOVPA(CMcpxStream_Pause, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundStream_Pause, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_FlushEx, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundStream_FlushEx, 4627, PATCH),
	REGISTER_OOVPA(DirectSoundDoWork, 5558, PATCH),
	REGISTER_OOVPA(CDirectSound_SynchPlayback, 5558, PATCH),
	REGISTER_OOVPA(XAudioDownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(IDirectSound_SetEffectData, 5344, PATCH),
	REGISTER_OOVPA(CMemoryManager_PoolAlloc, 5788, XREF),
	REGISTER_OOVPA(XFileCreateMediaObjectAsync, 5788, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFormat, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetPitch, 5849, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetHeadroom, 5558, PATCH),
	REGISTER_OOVPA(CMcpxBuffer_Play2, 5788, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Play, 5788, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Play, 5558, PATCH),
	REGISTER_OOVPA(IDirectSound_AddRef, 3936, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_AddRef, 3936, PATCH),
	REGISTER_OOVPA(CDirectSound_SetDistanceFactorA, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetDistanceFactor, 4134, PATCH),
	REGISTER_OOVPA(CDirectSound_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(IDirectSound_SetAllParameters, 3936, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetFilter, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetFilter, 4134, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetFilter, 4627, PATCH),
	REGISTER_OOVPA(IDirectSoundBuffer_SetFilter, 4134, PATCH),
	REGISTER_OOVPA(CMcpxVoiceClient_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundVoice_SetEG, 4627, XREF),
	REGISTER_OOVPA(CDirectSoundStream_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundBuffer_SetEG, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetEG, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetMode, 5344, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetMode, 5233, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetMode, 5233, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetI3DL2Source, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetI3DL2Source, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_SetAllParameters, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetAllParameters, 4134, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetAllParameters, 4134, PATCH),
	REGISTER_OOVPA(CDirectSoundVoice_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_Use3DVoiceData, 5558, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_Use3DVoiceData, 5558, XREF),
	REGISTER_OOVPA(CDirectSoundBuffer_SetNotificationPositions, 4627, XREF),
	REGISTER_OOVPA(IDirectSoundBuffer_SetNotificationPositions, 4627, PATCH),
	REGISTER_OOVPA(CDirectSoundStream_SetFrequency, 5233, PATCH)
};

// ******************************************************************
// * DSound_5849_SIZE
// ******************************************************************
uint32 DSound_5849_SIZE = sizeof(DSound_5849);
