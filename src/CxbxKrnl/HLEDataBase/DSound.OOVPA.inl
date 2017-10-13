// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->DSound.OOVPA.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// Titles which did compiled with full libary version
//   [LibV] Title Name          |  Verify   |   Comments
//-------------------------------------------------------------------
// * [3925] Cel Damage          |   100%    | Contain full library.
// * [3936] Silent Hill 2       |   100%    | Contain full library.
// * [4039] Nightcaster         |   100%    | Only has 90% of the library compiled with xbe build.
// * [4134] RaceX (Demo)        |     -%    | Only has a few library.
// * [4134] Blood Omen 2        |    80%    | Does not have full library.
// * [4134] JSRF                |     1%    | Does not have Stream class. DS and Buffer might be full.

// TODO: Known DSound OOVPA issue list
// * 3911 to 5933: Cannot make OOVPAs
//   * IDirectSound_SetCooperativeLevel     (Xbox doesn't use it, can't make OOVPA for it)
//   * IDirectSound_Compact                 (Xbox doesn't use it, can't make OOVPA for it)
//   * IDirectSoundBuffer_Restore           (Xbox doesn't use it, can't make OOVPA for it)
//   * IDirectSoundBuffer_Unlock            (Xbox doesn't use it, however can make OOVPA; NOTE: OOVPA is registered for it)
//   * IDirectSoundStream_AddRef            (Using IUnknown_AddRef)
//   * IDirectSoundStream_Release           (Using IUnknown_Release)
// * 3911 to ????: These functions does not exist until newer XDK revision
//   * IDirectSound_GetOutputLevels
//   * IDirectSound_SynchPlayback
//   * IDirectSoundBuffer_GetVoiceProperties
//   * IDirectSoundBuffer_SetFormat
//   * IDirectSoundBuffer_SetDistanceFactor
//   * IDirectSoundBuffer_SetDopplerFactor
//   * IDirectSoundBuffer_SetRolloffFactor
//   * IDirectSoundBuffer_SetRolloffCurve
//   * IDirectSoundBuffer_Pause
//   * IDirectSoundBuffer_PauseEx
//   * IDirectSoundBuffer_SetPlayRegion         (Introduce in 4039, last known earliest revision)
//   * IDirectSoundStream_FlushEx
//   * IDirectSoundStream_GetVoiceProperties
//   * IDirectSoundStream_PauseEx
//   * IDirectSoundStream_SetDistanceFactor
//   * IDirectSoundStream_SetDopplerFactor
//   * IDirectSoundStream_SetFormat
//   * IDirectSoundStream_SetRolloffCurve
//   * IDirectSoundStream_SetRolloffFactor
//   * DirectSoundDumpMemoryUsage
//   * DirectSoundGetSampleTime
//   * DirectSoundOverrideSpeakerConfig
//   * XAudioDownloadEffectsImage
//   * XAudioGetSpeakerConfig
//   * XAudioSetEffectData
//   * DirectSoundUseFullHRTF4Channel
//   * DirectSoundUseLightHRTF4Channel
// * 3911 to 3936: Following separater functions has exact asm codes as whole function are...
//   * IDirectSoundStream_SetVolume     & CDirectSoundStream_SetVolume
//   * IDirectSoundStream_SetPitch      & CDirectSoundStream_SetPitch
//   * IDirectSoundStream_SetLFO        & CDirectSoundStream_SetLFO
//   * IDirectSoundStream_SetEG         & CDirectSoundStream_SetEG
//   * IDirectSoundStream_SetFilter     & CDirectSoundStream_SetFilter
//   * IDirectSoundStream_SetHeadroom   & CDirectSoundStream_SetHeadroom
//   * IDirectSoundStream_SetFrequency  & CDirectSoundStream_SetFrequency
//   * IDirectSoundStream_SetMixBins    & CDirectSoundStream_SetMixBins
// * Need to include func export support for IDirectSoundStream_Setxxxxx from above.
// * 3911 - Not part of API, yet is a non-member function : (might be useful)
//   * GetFormatSize
//   * CopyFormat
//   * CompareFormats
//   * CalculateXboxAdpcmAlignment
//   * IsValidPcmFormat
//   * IsValidXboxAdpcmFormat
//   * CopyFormatAlloc
// * List of functions might need to be register:
//   * IDirectSoundBuffer_QueryInterface
//   * IDirectSoundBuffer_QueryInterfaceC
// * List of internal functions are not register:
//   * CDirectSoundBuffer_Release   (Is unique, however need multiple OOVPAs to register all revisions)
//     * Using XREF_DS_CRefCount_Release
//   * CDirectSound_Release         (Is unique, however need multiple OOVPAs to register all revisions)
//     * Using XREF_DS_CRefCount_Release
// * 4039 verification needed:
//   * DirectSoundCreateBuffer
//   * DirectSoundCreateStream
//   * DirectSoundUseLightHRTF
//   * IDirectSound_CommitEffectData
//   * IDirectSound_GetEffectData
//   * IDirectSound_GetSpeakerConfig
//   * IDirectSound_GetTime (It is generic)
//   * IDirectSound_SetAllParameters
//   * IDirectSound_SetEffectData
//   * IDirectSound_SetMixBinHeadroom
//   * IDirectSoundBuffer_PlayEx
//   * IDirectSoundBuffer_SetNotificationPositions
//   * CDirectSoundStream_SetFormat
//   * CDirectSoundStream_SetHeadroom
//   * CDirectSoundStream_SetI3DL2Source
//   * CDirectSoundStream_SetRolloffCurve
//   * CDirectSoundStream_SetRolloffFactor
//   * IDirectSoundStream_SetHeadroom
// * 4134 verification needed: (Verifying with Blood Omen 2 title is done, need different title to find the remaining...)
//   * CDirectSoundBuffer_SetEG
//   * IDirectSoundBuffer_SetEG
//   * CMcpxVoiceClient_SetLFO
//   * CDirectSoundStream_SetAllParameters
//   * CDirectSoundStream_SetConeAngles
//   * CDirectSoundStream_SetConeOrientation
//   * CDirectSoundStream_SetConeOutsideVolume
//   * CDirectSoundStream_SetFilter
//   * CDirectSoundStream_SetFrequency
//   * CDirectSoundStream_SetHeadroom
//   * CDirectSoundStream_SetLFO
//   * CDirectSoundStream_SetMaxDistance
//   * CDirectSoundStream_SetMinDistance
//   * CDirectSoundStream_SetMixBinVolumes_8
//   * CDirectSoundStream_SetMixBins
//   * CDirectSoundStream_SetMode
//   * CDirectSoundStream_SetOutputBuffer
//   * CDirectSoundStream_SetPitch
//   * CDirectSoundStream_SetVelocity
//   * CDirectSoundBuffer_PlayEx
//   * CDirectSoundBuffer_SetConeOutsideVolume
//   * CDirectSoundBuffer_SetCurrentPosition
//   * CDirectSoundBuffer_SetMaxDistance
//   * CDirectSoundBuffer_SetMinDistance
//   * CDirectSoundBuffer_SetNotificationPositions
//   * CDirectSoundBuffer_SetVelocity
//   * CDirectSoundBuffer_StopEx
//   * CDirectSound_CommitEffectData
//   * CDirectSound_EnableHeadphones
//   * CDirectSound_GetCaps
//   * CDirectSound_GetEffectData
//   * CDirectSound_GetSpeakerConfig
//   * CDirectSound_SetDistanceFactor
//   * CDirectSound_SetDopplerFactor
//   * CDirectSound_SetEffectData
//   * CDirectSound_SetMixBinHeadroom
//   * CDirectSound_SetOrientation
//   * CDirectSound_SetPosition
//   * CDirectSound_SetRolloffFactor
//   * CDirectSound_SetVelocity
//   * DirectSoundCreateBuffer
//   * DirectSoundUseLightHRTF
//   * DirectSoundGetSampleTime
//   * IsValidFormat
//   * XAudioCreateAdpcmFormat
//   * XAudioCreatePcmFormat
//   * IDirectSound_CreateSoundStream (CDirectSound_CreateSoundStream is found btw)
// * CDirectSound_SetAllParameters and CDirectSound_SetAllParametersA currently separated
//   * Need to review what's the difference and why is it necessary to be separated.
//   * It also have various revisions, we should be able to narrow it down to remove duplicates.


#ifndef DSOUND_OOVPA_INL
#define DSOUND_OOVPA_INL

#include "../OOVPA.h"

#include "DSound.1.0.3911.inl"
#include "DSound.1.0.3936.inl"
#include "DSound.1.0.4039.inl"
#include "DSound.1.0.4134.inl"
#include "DSound.1.0.4361.inl"
#include "DSound.1.0.4432.inl"
#include "DSound.1.0.4627.inl"
#include "DSound.1.0.5028.inl"
#include "DSound.1.0.5233.inl"
#include "DSound.1.0.5344.inl"
#include "DSound.1.0.5558.inl"
#include "DSound.1.0.5788.inl"
#include "DSound.1.0.5849.inl"


// ******************************************************************
// * DSOUND_OOVPA
// ******************************************************************
OOVPATable DSound_OOVPAV2[] = {

    REGISTER_OOVPAS(XAudioCalculatePitch, XREF, 3911, 4039),
    REGISTER_OOVPAS(DirectSoundEnterCriticalSection, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Commit3dSettings, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_ServiceDeferredCommandsLow, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dDistanceFactor, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dDopplerFactor, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dParameters, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dPosition, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dRolloffFactor, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_Set3dVelocity, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_SetI3DL2Listener, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_SetMixBinHeadroom, XREF, 3911),
    REGISTER_OOVPAS(CMcpxAPU_SynchPlayback, XREF, 5233),
    REGISTER_OOVPAS(CMcpxBuffer_GetCurrentPosition, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CMcpxBuffer_GetStatus, XREF, 3911, 4039, 4134, 4721, 4831, 5558),
    REGISTER_OOVPAS(CMcpxBuffer_Pause, XREF, 4831),
    REGISTER_OOVPAS(CMcpxBuffer_Play, XREF, 3911, 4361, 4721, 4831), // NOTE: ?Play@CMcpxBuffer@DirectSound@@QAEJK@Z
    REGISTER_OOVPAS(CMcpxBuffer_Play2, XREF, 4361, 5558), // NOTE: ?Play@CMcpxBuffer@DirectSound@@QAEJ_JK@Z
    REGISTER_OOVPAS(CMcpxBuffer_SetBufferData, XREF, 4134, 5788),
    REGISTER_OOVPAS(CMcpxBuffer_SetCurrentPosition, XREF, 3911, 4039, 4134, 5558, 5788),
    REGISTER_OOVPAS(CMcpxBuffer_Stop, XREF, 3911, 4134, 4242), // NOTE: ?Stop@CMcpxBuffer@DirectSound@@QAEJK@Z
    REGISTER_OOVPAS(CMcpxBuffer_Stop2, XREF, 4361), // NOTE: ?Stop@CMcpxBuffer@DirectSound@@QAEJ_JK@Z
    REGISTER_OOVPAS(CMcpxStream_Discontinuity, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CMcpxStream_Flush, XREF, 3911, 3936, 4039, 4134),
    REGISTER_OOVPAS(CMcpxStream_GetStatus, XREF, 4134),
    REGISTER_OOVPAS(CMcpxStream_Pause, XREF, 3911, 4039, 4134, /*4361,*/ 4831, 5788),
    REGISTER_OOVPAS(CSensaura3d_GetFullHRTFFilterPair, XREF, 3911, 3936),
    REGISTER_OOVPAS(CSensaura3d_GetLiteHRTFFilterPair, XREF, 3911, 3936),
    REGISTER_OOVPAS(CMcpxVoiceClient_Commit3dSettings, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dConeOrientation, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dConeOutsideVolume, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dMaxDistance, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dMinDistance, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dMode, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dParameters, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dPosition, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_Set3dVelocity, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetEG, XREF, 3911, 4039, 4134, 4242),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetFilter, XREF, 3911, 4039, 4134 /*, 5344, 5558*/),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetI3DL2Source, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetLFO, XREF, 3911, 4039, 4242),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetMixBins, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetPitch, XREF, 3911),
    REGISTER_OOVPAS(CMcpxVoiceClient_SetVolume, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(DS_CRefCount_AddRef, XREF, 3911, 4039),
    REGISTER_OOVPAS(DS_CRefCount_Release, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoiceSettings_SetMixBins, XREF, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoiceSettings_SetMixBinVolumes, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_CommitDeferredSettings, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetAllParameters, XREF, 3911, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeAngles, XREF, 3911, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeOrientation, XREF, 3911, 4039, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetConeOutsideVolume, XREF, 3911, 4039, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetDistanceFactor, XREF, 4134, 4361, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetDopplerFactor, XREF, 4134, 4361, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetEG, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFilter, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFormat, XREF, 4242, 4721, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetFrequency, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundVoice_SetHeadroom, XREF, 3911, 4039, 4134, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetI3DL2Source, XREF, 3911, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetLFO, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMaxDistance, XREF, 3911, 4039, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMinDistance, XREF, 3911, 4039, 4134, 4361, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMixBins, XREF, 3911, 4039, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMixBinVolumes, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetMode, XREF, 3911, 4039, 4134, 5233, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetOutputBuffer, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetPitch, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundVoice_SetPosition, XREF, 3911, 4039, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundVoice_SetRolloffCurve, XREF, 4627, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetRolloffFactor, XREF, 4134, 4361, 5344, 5788), // s+ (from 4134's comment)
    REGISTER_OOVPAS(CDirectSoundVoice_SetVelocity, XREF, 3911, 4039, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundVoice_SetVolume, XREF, 3911, 4039, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundVoice_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_GetCurrentPosition, XREF, 3911, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_GetStatus, XREF, 3911, 4039, 4134, 4721),
    REGISTER_OOVPAS(CDirectSoundBuffer_Lock, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_Pause, XREF, 4928),
    REGISTER_OOVPAS(CDirectSoundBuffer_Play, XREF, 3911, 4039, 4134, 4361, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_PlayEx, XREF, 3911, 4361, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetAllParameters, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetBufferData, XREF, 3911, 4039, 4134, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeAngles, XREF, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeOrientation, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetConeOutsideVolume, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetCurrentPosition, XREF, 3911, 4039, 4134, 5558, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetDistanceFactor, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetDopplerFactor, XREF, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetEG, XREF, 4039, 4242),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFilter, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFormat, XREF, 4242, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetFrequency, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetHeadroom, XREF, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetI3DL2Source, XREF, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetLFO, XREF, 4039, 4242),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetLoopRegion, XREF, 3911, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMaxDistance, XREF, 3911, 4039, 4134, 5344, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMinDistance, XREF, 3911, 4039, 4134, 5344, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMixBins, XREF, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMixBinVolumes, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetMode, XREF, 4039, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetNotificationPositions, XREF, 3911, 4627), // Uncommenting these fixes dashboard 4920 (from 4627 comment)
    REGISTER_OOVPAS(CDirectSoundBuffer_SetOutputBuffer, XREF, 3911, 4039, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPitch, XREF, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPlayRegion, XREF, 4039, 4361, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetPosition, XREF, 3911, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetRolloffCurve, XREF, 4627),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetRolloffFactor, XREF, 4134, 5788),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetVelocity, XREF, 3911, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSoundBuffer_SetVolume, XREF, 4039, 4134, 5233),
    REGISTER_OOVPAS(CDirectSoundBuffer_Stop, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundBuffer_StopEx, XREF, 3911, 4039, 4361),
    REGISTER_OOVPAS(CDirectSoundBuffer_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_AddRef, PATCH, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundStream_Discontinuity, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_Flush, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_FlushEx, XREF, 4627, 5233, 5788),
    REGISTER_OOVPAS(CDirectSoundStream_GetInfo, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_GetStatus, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_Pause, PATCH, 3911, 4039, 4134 /*, 4361, 5558*/),
    REGISTER_OOVPAS(CDirectSoundStream_Process, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_Release, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetAllParameters, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeAngles, PATCH, 3911, 4039),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOrientation, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetConeOutsideVolume, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetEG, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetFilter, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetFormat, PATCH, 4242),
    REGISTER_OOVPAS(CDirectSoundStream_SetFrequency, PATCH, 3911, 4039, 5233),
    REGISTER_OOVPAS(CDirectSoundStream_SetHeadroom, PATCH, 3911, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetI3DL2Source, PATCH, 3911, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetLFO, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetMaxDistance, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetMinDistance, PATCH, 3911, 4039, 4134, 5344),
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBins, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBinVolumes_12, PATCH, 3911), //This revision is only used in XDK 3911 to 3936.
    REGISTER_OOVPAS(CDirectSoundStream_SetMixBinVolumes_8, PATCH, 4039), //Then it has changed in XDK 4039 and higher.
    REGISTER_OOVPAS(CDirectSoundStream_SetMode, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetOutputBuffer, PATCH, 3911, 4039, 4627),
    REGISTER_OOVPAS(CDirectSoundStream_SetPitch, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetPosition, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetRolloffCurve, XREF, 4627, 5558),
    REGISTER_OOVPAS(CDirectSoundStream_SetRolloffFactor, PATCH, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetVelocity, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_SetVolume, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSoundStream_Use3DVoiceData, XREF, 5558),
    REGISTER_OOVPAS(CDirectSound_CommitDeferredSettings, XREF, 3911, 4039, 4134, 5344, 5558, 5788),
    REGISTER_OOVPAS(CDirectSound_CommitEffectData, XREF, 3911),
    REGISTER_OOVPAS(CDirectSound_CreateSoundBuffer, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSound_CreateSoundStream, XREF, 3911, 4039, 4134, 5558),
    REGISTER_OOVPAS(CDirectSound_DownloadEffectsImage, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSound_DoWork, XREF, 3911, 4039),
    REGISTER_OOVPAS(CDirectSound_EnableHeadphones, XREF, 3911, 4039, 4627, 5233, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_GetCaps, XREF, 3911, 4039, 4361),
    REGISTER_OOVPAS(CDirectSound_GetEffectData, XREF, 3911),
    REGISTER_OOVPAS(CDirectSound_GetOutputLevels, XREF, 4627, 5558),
    REGISTER_OOVPAS(CDirectSound_GetSpeakerConfig, PATCH, 3911, 4627),
    REGISTER_OOVPAS(CDirectSound_GetTime, XREF, 3911),
    REGISTER_OOVPAS(CDirectSound_SetAllParameters, XREF, 3911, 4134, 4831, 5558),
    REGISTER_OOVPAS(CDirectSound_SetAllParametersA, XREF, 4627, 4721, 4831),
    REGISTER_OOVPAS(CDirectSound_SetDistanceFactor, XREF, 3911, 4039, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetDopplerFactor, XREF, 3911, 4039, 4134, 4627, 5344, 5558, 5788),
    REGISTER_OOVPAS(CDirectSound_SetEffectData, XREF, 3911, 4134),
    REGISTER_OOVPAS(CDirectSound_SetI3DL2Listener, XREF, 3911, 4039, 4134, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetMixBinHeadroom, XREF, 3911, 4627, 5558),
    REGISTER_OOVPAS(CDirectSound_SetOrientation, XREF, 3911, 4039, 4134),
    REGISTER_OOVPAS(CDirectSound_SetPosition, XREF, 3911, 4039, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SetRolloffFactor, XREF, 3911, 4039, 4134, 5344, 5558, 5788, 5849),
    REGISTER_OOVPAS(CDirectSound_SetVelocity, XREF, 3911, 4039, 4134, 4627, 5344, 5558),
    REGISTER_OOVPAS(CDirectSound_SynchPlayback, XREF, 5233, 5344, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_AddRef, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_GetCurrentPosition, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_GetStatus, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_Lock, PATCH, 3911, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_Pause, PATCH, 4928),
    REGISTER_OOVPAS(IDirectSoundBuffer_Play, PATCH, 3911, 4361, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_PlayEx, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_Release, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetAllParameters, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetBufferData, PATCH, 3911, 4134, 5558, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeAngles, PATCH, 3911, 4039, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeOrientation, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetConeOutsideVolume, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetCurrentPosition, PATCH, 3911 /*, 4134, 5558, 5788*/),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetDistanceFactor, PATCH, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetDopplerFactor, PATCH, 4134, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetEG, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFilter, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFormat, PATCH, 4242, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetFrequency, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetHeadroom, PATCH, 3911, 4039, 5344, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetI3DL2Source, PATCH, 3911, 4039, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetLFO, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetLoopRegion, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMaxDistance, PATCH, 3911 /*,5344, 5788*/),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMinDistance, PATCH, 3911 /*, 5558, 5788*/),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBins, PATCH, 3911, 4039 /*, 4134, 5558*/),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBinVolumes_12, PATCH, 3911), //This revision is only used in XDK 3911 to 3936.
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMixBinVolumes_8, PATCH, 4039), //Then it has changed in XDK 4039 and higher.
    REGISTER_OOVPAS(IDirectSoundBuffer_SetMode, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetNotificationPositions, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetOutputBuffer, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPitch, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPlayRegion, PATCH, 4039, 4361),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetPosition, PATCH, 3911, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetRolloffCurve, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetRolloffFactor, PATCH, 4134, 5788),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVelocity, PATCH, 3911, 5558),
    REGISTER_OOVPAS(IDirectSoundBuffer_SetVolume, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundBuffer_Stop, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_StopEx, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_Unlock, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundBuffer_Use3DVoiceData, PATCH, 5558),
    REGISTER_OOVPAS(IDirectSoundStream_FlushEx, PATCH, 4627),
    REGISTER_OOVPAS(IDirectSoundStream_Pause, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetAllParameters, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetConeAngles, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetConeOrientation, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetConeOutsideVolume, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetEG, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetLFO, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetFilter, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetFrequency, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetHeadroom, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetI3DL2Source, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetMaxDistance, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetMinDistance, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetMixBins, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetMixBinVolumes_12, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, see more note for CDirectSoundStream_SetMixBinVolumes_12
    REGISTER_OOVPAS(IDirectSoundStream_SetMixBinVolumes_8, UNPATCHED, 4039), //NOTE: 4039 and newer only perform a jmp.
    REGISTER_OOVPAS(IDirectSoundStream_SetMode, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetOutputBuffer, UNPATCHED, 3911), //NOTE: 3911 only perform a jmp, later XDK revision may need a patch?
    REGISTER_OOVPAS(IDirectSoundStream_SetPitch, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSoundStream_SetPosition, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetVelocity, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSoundStream_SetVolume, PATCH, 3911, 4039),
    REGISTER_OOVPAS(IDirectSound_AddRef, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_CommitDeferredSettings, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_CommitEffectData, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_CreateSoundBuffer, PATCH, 3911 /*3936, 4134, 4361*/),
    REGISTER_OOVPAS(IDirectSound_CreateSoundStream, PATCH, 3911 /*5558*/),
    REGISTER_OOVPAS(IDirectSound_DownloadEffectsImage, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_EnableHeadphones, PATCH, 3911 /*, 3936, 4627, 5233, 5558*/),
    REGISTER_OOVPAS(IDirectSound_GetCaps, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_GetEffectData, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_GetOutputLevels, PATCH, 4627, 5558),
    REGISTER_OOVPAS(IDirectSound_GetSpeakerConfig, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_GetTime, UNPATCHED, 3911),
    REGISTER_OOVPAS(IDirectSound_Release, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_SetAllParameters, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_SetDistanceFactor, PATCH, 3911 /*, 4134*/),
    REGISTER_OOVPAS(IDirectSound_SetDopplerFactor, PATCH, 3911 /*, 4134*/),
    REGISTER_OOVPAS(IDirectSound_SetEffectData, PATCH, 3911 /*, 4134, 5344*/),
    REGISTER_OOVPAS(IDirectSound_SetI3DL2Listener, PATCH, 3911 /*, 4134*/),
    REGISTER_OOVPAS(IDirectSound_SetMixBinHeadroom, PATCH, 3911 /*, 4627, 5558*/),
    REGISTER_OOVPAS(IDirectSound_SetOrientation, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_SetPosition, PATCH, 3911),
    REGISTER_OOVPAS(IDirectSound_SetRolloffFactor, PATCH, 3911, 4134/*, 5344*/),
    REGISTER_OOVPAS(IDirectSound_SetVelocity, PATCH, 3911 /*, 5558*/),
    REGISTER_OOVPAS(IDirectSound_SynchPlayback, PATCH, 5233),

    REGISTER_OOVPAS(CFullHRTFSource_GetCenterVolume, XREF, 4039, 4134, 5558),
    REGISTER_OOVPAS(CHRTFSource_SetFullHRTF5Channel, XREF, 4039, 5558),

    REGISTER_OOVPAS(DirectSoundCreate, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(DirectSoundCreateBuffer, PATCH, 3911, 4039, 4242),
    REGISTER_OOVPAS(DirectSoundCreateStream, PATCH, 3911, 4134, 5788),
    REGISTER_OOVPAS(DirectSoundDoWork, PATCH, 3911, 4134),
    REGISTER_OOVPAS(DirectSoundGetSampleTime, PATCH, 3911, 4361),
    REGISTER_OOVPAS(DirectSoundUseFullHRTF, PATCH, 3911, 4039, 4134),
    REGISTER_OOVPAS(DirectSoundUseLightHRTF, PATCH, 3911),

    REGISTER_OOVPAS(WaveFormat_CreateXboxAdpcmFormat, XREF, 5344),
    REGISTER_OOVPAS(XAudioDownloadEffectsImage, PATCH, 4627, 5558, 5788),
    REGISTER_OOVPAS(IsValidFormat, UNPATCHED, 3911),
    REGISTER_OOVPAS(XAudioCreatePcmFormat, UNPATCHED, 3911),
    REGISTER_OOVPAS(XAudioCreateAdpcmFormat, PATCH, 3911, 5344),
    REGISTER_OOVPAS(XFileCreateMediaObjectAsync, PATCH, 5788),
    REGISTER_OOVPAS(XFileCreateMediaObjectEx, PATCH, 4627, 5028),
    REGISTER_OOVPAS(XWaveFileCreateMediaObject, PATCH, 4627),

    REGISTER_OOVPAS(DS_CMemoryManager_PoolAlloc, XREF, 4134, 5788), // For reference purpose only, does not have XREF value.
};

// ******************************************************************
// * DSOUND_OOVPA_SIZE
// ******************************************************************
uint32 DSound_OOVPA_SIZEV2 = sizeof(DSound_OOVPAV2);

#endif
