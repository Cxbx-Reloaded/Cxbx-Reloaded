// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase.h
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
#ifndef HLEDATABASE_H
#define HLEDATABASE_H

#include "HLEDataBase\D3D8.OOVPA.h"

// ******************************************************************
// * szHLELastCompileTime
// ******************************************************************
extern "C" const char *szHLELastCompileTime;

//TODO: Need to remove these externs as v2 is no longer require it.
extern const char *Lib_D3D8;
extern const char *Lib_D3D8LTCG;
extern const char *Lib_D3DX8;
extern const char *Lib_DSOUND;
extern const char *Lib_XACTENG;
extern const char *Lib_XAPILIB;
extern const char *Lib_XGRAPHC;
extern const char *Lib_XNETS;
extern const char *Lib_XONLINE; //Is not a typo issue
extern const char *Lib_XONLINES;

// ******************************************************************
// * HLEDataBase
// ******************************************************************

#define PAIRSCANSEC_MAX 3

const struct PairScanLibSec {
    const char *library;
    const char *section[PAIRSCANSEC_MAX];
};

extern const struct HLEData
{
    const PairScanLibSec LibSec;

    OOVPATable     *OovpaTable;
    uint32          OovpaTableSize;
}
HLEDataBase[];

// ******************************************************************
// * HLEDataBaseCount
// ******************************************************************
extern const uint32 HLEDataBaseCount;

// ******************************************************************
// * XRefDataBaseOffset
// ******************************************************************
enum XRefDataBaseOffset
{
    XREF_D3DDEVICE=0,
    XREF_D3DDevice_SetRenderState_CullMode,
    XREF_D3D_RenderState_CullMode,
    XREF_D3D_TextureState_TexCoordIndex,
    XREF_D3D_RenderState_MultiSampleMode,
    XREF_D3D_RenderState_MultiSampleRenderTargetMode,
    XREF_D3D_RenderState_RopZCmpAlwaysRead,
    XREF_D3D_RenderState_RopZRead,
    XREF_D3D_RenderState_DoNotCullUncompressed,
    XREF_D3D_RenderState_StencilCullEnable,
    XREF_D3DDevice_SetLight,
    XREF_D3DDevice_LazySetStateVB,
    XREF_D3D_BlockOnResource,
    XREF_D3D_BlockOnTime,
    XREF_D3D_CreateStandAloneSurface,
    XREF_D3D_SetFence,
    XREF_D3D_SetTileNoWait,
	XREF_D3D_KickOffAndWaitForIdle,
	XREF_D3D_MakeRequestedSpace,
	XREF_D3D_CDevice_KickOff,
	XREF_D3DPalette_Lock2,
	XREF_D3DTexture_GetSurfaceLevel2,
	XREF_D3DDevice_CreatePalette2,
    XREF_D3DDevice_CreateIndexBuffer2,
    XREF_D3DDevice_CreateVertexBuffer2,
    XREF_D3DDevice_GetBackBuffer2,
    XREF_D3DDevice_GetDepthStencilSurface2,
    XREF_D3DDevice_GetRenderTarget2,
    XREF_D3DResource_AddRef,
    XREF_D3DVertexBuffer_Lock2,
    XREF_XnInit,
    XREF_XID_fCloseDevice,
    XREF_D3D_ClearStateBlockFlags,
    XREF_D3D_RecordStateBlock,
    XREF_Lock2DSurface,
    XREF_Lock3DSurface,
    XREF_CDirectSound_SetDistanceFactor,
    XREF_CMcpxAPU_Set3dDistanceFactor,
    XREF_CDirectSound_SetRolloffFactor,
    XREF_CMcpxAPU_Set3dRolloffFactor,
    XREF_CDirectSound_SetDopplerFactor,
	XREF_CMcpxBuffer_SetCurrentPosition,
	XREF_CDirectSoundBuffer_SetCurrentPosition,
	XREF_CMcpxBuffer_GetCurrentPosition,
	XREF_CDirectSoundBuffer_GetCurrentPosition,
    XREF_CDirectSoundBuffer_Play,
	XREF_CMcpxBuffer_Stop,
	XREF_CDirectSoundBuffer_Stop,
    XREF_CDirectSoundStream_FlushEx,
	XREF_CMcpxBuffer_SetBufferData,
	XREF_CDirectSoundBuffer_SetBufferData,
    XREF_CDirectSound_CreateSoundBuffer,
    XREF_CDirectSound_CreateSoundStream,
    XREF_CDirectSound_SetI3DL2Listener,
    XREF_CDirectSound_SetMixBinHeadroom,
    XREF_CMcpxAPU_SetMixBinHeadroom,
    XREF_CDirectSoundVoiceSettings_SetMixBins,
    XREF_CMcpxVoiceClient_SetMixBins,
    XREF_CDirectSoundVoice_SetMixBins,
    XREF_CDirectSoundBuffer_SetMixBins,
    XREF_CDirectSoundBuffer_SetMixBinVolumes,
    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XREF_CDirectSoundVoiceSettings_SetMixBinVolumes,
    XREF_CDirectSound_SetPosition,
    XREF_CMcpxAPU_Set3dPosition,
    XREF_CDirectSound_SetVelocity,
    XREF_CMcpxAPU_Set3dVelocity,
    XREF_CDirectSound_DoWork,
    XREF_CMcpxAPU_ServiceDeferredCommandsLow,
    XREF_CDirectSound_SetAllParameters,
    XREF_CMcpxAPU_Set3dParameters,
    XREF_CDirectSound_EnableHeadphones,
    XREF_CDirectSound_SynchPlayback,
    XREF_CMcpxAPU_SynchPlayback,
    XREF_CMcpxVoiceClient_SetPitch,
    XREF_CDirectSoundVoice_SetPitch,
    XREF_CDirectSoundBuffer_SetPitch,
    XREF_CDirectSoundStream_SetPitch,
    XREF_CMcpxVoiceClient_SetVolume,
    XREF_CDirectSoundBuffer_SetPlayRegion,
    XREF_CDirectSoundBuffer_Lock,
    XREF_CDirectSoundVoice_SetHeadroom,
	XREF_CDirectSoundBuffer_SetHeadroom,
    XREF_CDirectSoundStream_SetFrequency,
    XREF_CDirectSoundBuffer_SetFrequency,
    XREF_CDirectSoundVoice_SetFrequency,
    XREF_CMcpxVoiceClient_Set3dMaxDistance,
    XREF_CDirectSoundVoice_SetMaxDistance,
    XREF_CDirectSoundStream_SetMaxDistance,
    XREF_CDirectSoundStream_SetMinDistance,
    XREF_CDirectSoundVoice_SetMinDistance,
    XREF_CMcpxVoiceClient_Set3dMinDistance,
    XREF_CDirectSoundVoice_SetConeOutsideVolume,
    XREF_CMcpxVoiceClient_Set3dConeOutsideVolume,
    XREF_CDirectSoundVoice_SetAllParameters,
    XREF_CMcpxVoiceClient_Set3dParameters,
    XREF_CDirectSoundStream_SetVelocity,
    XREF_CDirectSoundVoice_SetVelocity,
    XREF_CMcpxVoiceClient_Set3dVelocity,
    XREF_CDirectSoundStream_SetConeOrientation,
    XREF_CDirectSoundVoice_SetConeOrientation,
    XREF_CMcpxVoiceClient_Set3dConeOrientation,
    XREF_CDirectSoundStream_SetPosition,
    XREF_CDirectSoundVoice_SetPosition,
    XREF_CMcpxVoiceClient_Set3dPosition,
    XREF_CMcpxStream_Flush,
    XREF_CMcpxStream_Discontinuity,
    XREF_XAudioCalculatePitch,
    XREF_D3D_CommonSetRenderTarget,
    XREF_CDirectSound_CommitDeferredSettings,
    XREF_CMcpxAPU_Commit3dSettings,
    XREF_CDirectSound_SetOrientation,
    XREF_IDirectSound_CreateSoundBuffer,
    XREF_IDirectSound_CreateSoundStream,
    // s+
    // DSOUND
    XREF_DSound_CRefCount_AddRef,
    XREF_DSound_CRefCount_Release,
    XREF_CDirectSoundVoice_SetRolloffCurve,
    XREF_CDirectSoundVoice_SetRolloffFactor,
    XREF_CDirectSoundBuffer_SetRolloffFactor,
    XREF_CDirectSoundStream_SetRolloffFactor,
    XREF_CDirectSoundVoice_SetDistanceFactor,
    XREF_CDirectSoundBuffer_SetDistanceFactor,
    XREF_CDirectSoundStream_SetDistanceFactor,
    XREF_CDirectSoundVoice_SetConeAngles,
    XREF_CDirectSoundVoice_SetDopplerFactor,
    XREF_CDirectSoundBuffer_SetDopplerFactor,
    XREF_CDirectSoundStream_SetDopplerFactor,
    XREF_CMcpxVoiceClient_Set3dMode,
    XREF_CDirectSoundVoice_SetMode,
    XREF_CDirectSoundBuffer_SetMode,
    XREF_CDirectSoundBuffer_SetMaxDistance,
    XREF_CDirectSoundBuffer_SetMinDistance,
    XREF_CDirectSoundBuffer_SetRolloffCurve,
    XREF_CDirectSoundBuffer_SetConeAngles,
    XREF_CDirectSoundBuffer_SetConeOrientation,
    XREF_CDirectSoundBuffer_SetConeOutsideVolume,
    XREF_CDirectSoundBuffer_SetPosition,
    XREF_CDirectSoundBuffer_SetVelocity,
    XREF_CDirectSoundBuffer_SetI3DL2Source,
    XREF_CDirectSoundVoice_SetFormat,
    XREF_CDirectSoundBuffer_SetFormat,
    XREF_CDirectSoundStream_SetFormat,
    XREF_CFullHRTFSource_GetCenterVolume,
    XREF_CHRTFSource_SetFullHRTF5Channel,
    XREF_CHRTFSource_SetLightHRTF5Channel,
    XREF_CHRTFSource_SetFullHRTF4Channel,
    XREF_CHRTFSource_SetLightHRTF4Channel,
    XREF_CMcpxVoiceClient_SetLFO,
    XREF_CDirectSoundVoice_SetLFO,
    XREF_CDirectSoundBuffer_SetLFO,
    XREF_CDirectSoundStream_SetLFO,
    XREF_CDirectSoundVoice_SetVolume,
    XREF_CDirectSoundBuffer_SetVolume,
    XREF_CDirectSoundStream_SetVolume,
    XREF_CDirectSoundBuffer_SetLoopRegion,
    XREF_CMcpxBuffer_Play,
    XREF_CDirectSoundBuffer_PlayEx,
    XREF_WaveFormat_CreateXboxAdpcmFormat,
    XREF_CDirectSoundVoice_CommitDeferredSettings,
    XREF_CDirectSoundBuffer_SetAllParameters,
	XREF_CDirectSound_GetOutputLevels,
	XREF_CMcpxBuffer_Pause,
	XREF_CDirectSoundBuffer_Pause,
    XREF_CMcpxBuffer_Pause_Ex,
    XREF_CDirectSoundBuffer_PauseEx,
	XREF_CMcpxBuffer_GetStatus,
	XREF_CDirectSoundBuffer_GetStatus,
    XREF_CMcpxStream_GetStatus,
    XREF_CDirectSoundStream_SetEG,
	XREF_CDirectSoundVoice_SetEG,
	XREF_CMcpxVoiceClient_SetEG,
	XREF_CMcpxVoiceClient_SetFilter,
	XREF_CDirectSoundVoice_SetFilter,
	XREF_CDirectSoundBuffer_SetFilter,
	XREF_CDirectSoundStream_SetFilter,
	XREF_CDirectSoundBuffer_StopEx,
	XREF_CMcpxBuffer_Stop2,
	XREF_CMcpxAPU_Set3dDopplerFactor,
	XREF_CSensaura3d_GetFullHRTFFilterPair,
	XREF_CSensaura3d_GetLiteHRTFFilterPair,
	XREF_CDirectSound_GetCaps,
	XREF_DirectSoundEnterCriticalSection,
	XREF_CMcpxVoiceClient_Commit3dSettings,
	XREF_CMcpxVoiceClient_SetI3DL2Source,
	XREF_CDirectSoundVoice_SetI3DL2Source,
	XREF_CDirectSoundStream_SetMixBinVolumes_12,
	XREF_CDirectSoundStream_SetMixBinVolumes_8,
    XREF_CDirectSoundStream_SetMixBins,
	XREF_CDirectSoundStream_SetHeadroom,
	XREF_CDirectSoundVoice_SetOutputBuffer,
	XREF_CDirectSoundBuffer_SetOutputBuffer,
	XREF_CDirectSoundBuffer_SetNotificationPositions,
	XREF_CDirectSoundBuffer_SetEG,
	XREF_CDirectSound_GetEffectData,
	XREF_CMcpxBuffer_Play2,
	XREF_CDirectSoundVoice_Use3DVoiceData,
	XREF_CDirectSoundBuffer_Use3DVoiceData,
    XREF_CDirectSoundStream_Use3DVoiceData,
	XREF_CDirectSound_SetEffectData,
    XREF_CDirectSound_GetSpeakerConfig,
    XREF_CDirectSound_DownloadEffectsImage,
    XREF_CDirectSound_CommitEffectData,
    XREF_CDirectSound_GetTime,
    XREF_CMcpxAPU_SetI3DL2Listener,
    XREF_CDirectSoundStream_SetOutputBuffer,
    XREF_CDirectSoundStream_SetAllParameters,
    XREF_CDirectSoundStream_SetConeAngles,
    XREF_CDirectSoundStream_SetConeOutsideVolume,
    XREF_CDirectSoundStream_SetMode,
    XREF_CDirectSoundStream_SetI3DL2Source,
    XREF_CMcpxStream_Pause,
    XREF_CDirectSoundStream_Pause,
    XREF_CMcpxStream_Pause_Ex,
    XREF_CDirectSoundStream_PauseEx,
    XREF_CDirectSoundVoice_GetVoiceProperties,
    XREF_CMcpxVoiceClient_GetVoiceProperties,
    XREF_CDirectSoundBuffer_GetVoiceProperties,
    XREF_CDirectSoundStream_GetVoiceProperties,
    XREF_CDirectSoundVoice_Set3DVoiceData,
    XREF_CDirectSoundBuffer_Set3DVoiceData,
    XREF_CDirectSoundStream_Set3DVoiceData,
    XREF_CDirectSound3DCalculator_GetVoiceData,
    XREF_CDirectSound3DCalculator_GetPanData,
    XREF_CDirectSound3DCalculator_GetMixBinVolumes,
    XREF_CDirectSound3DCalculator_Calculate3D,
    XREF_CDirectSound_UnmapBufferData,
    XREF_CDirectSound_MapBufferData,
	// XACT
	// +s
	XREF_XACT_CEngine_RegisterWaveBank,
	XREF_XACT_CEngine_RegisterStreamedWaveBank,
	XREF_XACT_CEngine_CreateSoundBank,
	XREF_XACT_CEngine_DownloadEffectsImage,
	XREF_XACT_CEngine_CreateSoundSource,
	XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName,
	XREF_XACT_CSoundBank_Play,
	XREF_XACT_CEngine_RegisterNotification,
	XREF_XACT_CEngine_GetNotification,
	XREF_XACT_CEngine_UnRegisterWaveBank,
	// XONLINE
	// +s
	XREF_XoUpdateLaunchNewImageInternal,
	XREF_CXo_XOnlineLogon,

    // XAPI
	XREF_XAPI_GetTypeInformation,

	XREF_COUNT // XREF_COUNT must always be last.
	// Also, if XREF_COUNT > sizeof(uint16), enlarge struct OOVPA.XRefSaveIndex (and Value somehow)
};

#define XREF_ADDR_UNDETERMINED -1
#define XREF_ADDR_NOT_FOUND ((xbaddr)nullptr)
#define XREF_ADDR_DERIVE 1

// ******************************************************************
// * XRefDataBase
// ******************************************************************
extern xbaddr XRefDataBase[XREF_COUNT];

#endif /*HLEDATABASE_H*/
