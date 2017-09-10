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

#include "HLEDataBase/Xapi.1.0.3911.h"
#include "HLEDataBase/Xapi.1.0.4034.h"
#include "HLEDataBase/Xapi.1.0.4134.h"
#include "HLEDataBase/Xapi.1.0.4361.h"
#include "HLEDataBase/Xapi.1.0.4432.h"
#include "HLEDataBase/Xapi.1.0.4627.h"
#include "HLEDataBase/Xapi.1.0.5028.h"
#include "HLEDataBase/Xapi.1.0.5233.h"
#include "HLEDataBase/Xapi.1.0.5344.h"
#include "HLEDataBase/Xapi.1.0.5558.h"
#include "HLEDataBase/Xapi.1.0.5788.h"
#include "HLEDataBase/Xapi.1.0.5849.h"
#include "HLEDataBase/D3D8.1.0.3925.h"
#include "HLEDataBase/D3D8.1.0.4034.h"
#include "HLEDataBase/D3D8.1.0.4134.h"
#include "HLEDataBase/D3D8.1.0.4361.h"
#include "HLEDataBase/D3D8.1.0.4432.h"
#include "HLEDataBase/D3D8.1.0.4627.h"
#include "HLEDataBase/D3D8.1.0.5028.h"
#include "HLEDataBase/D3D8.1.0.5233.h"
#include "HLEDataBase/D3D8.1.0.5344.h"
#include "HLEDataBase/D3D8.1.0.5558.h"
#include "HLEDataBase/D3D8.1.0.5788.h"
#include "HLEDataBase/D3D8.1.0.5849.h"
#include "HLEDataBase/DSound.1.0.3936.h"
#include "HLEDataBase/DSound.1.0.4134.h"
#include "HLEDataBase/DSound.1.0.4361.h"
#include "HLEDataBase/DSound.1.0.4432.h"
#include "HLEDataBase/DSound.1.0.4627.h"
#include "HLEDataBase/DSound.1.0.5028.h"
#include "HLEDataBase/DSound.1.0.5233.h"
#include "HLEDataBase/DSound.1.0.5344.h"
#include "HLEDataBase/DSound.1.0.5558.h"
#include "HLEDataBase/DSound.1.0.5788.h"
#include "HLEDataBase/DSound.1.0.5849.h"
#include "HLEDataBase/XG.1.0.3911.h"
#include "HLEDataBase/XG.1.0.4034.h"
#include "HLEDataBase/XG.1.0.4361.h"
#include "HLEDataBase/XG.1.0.4432.h"
#include "HLEDataBase/XG.1.0.4627.h"
#include "HLEDataBase/XG.1.0.5028.h"
#include "HLEDataBase/XG.1.0.5233.h"
#include "HLEDataBase/XG.1.0.5344.h"
#include "HLEDataBase/XG.1.0.5558.h"
#include "HLEDataBase/XG.1.0.5788.h"
#include "HLEDataBase/XG.1.0.5849.h"
#include "HLEDataBase/XNet.1.0.3911.h"
#include "HLEDataBase/XOnline.1.0.4361.h"
#include "HLEDataBase/XOnline.1.0.4627.h"
#include "HLEDataBase/XOnline.1.0.5028.h"
#include "HLEDataBase/XOnline.1.0.5233.h"
#include "HLEDataBase/XOnline.1.0.5344.h"
#include "HLEDataBase/XOnline.1.0.5558.h"
#include "HLEDataBase/XOnline.1.0.5788.h"
#include "HLEDataBase/XOnline.1.0.5849.h"
#include "HLEDataBase/XactEng.1.0.4627.h"
#include "HLEDataBase/XactEng.1.0.4928.h"
#include "HLEDataBase/XactEng.1.0.5233.h"
#include "HLEDataBase/XactEng.1.0.5344.h"
#include "HLEDataBase/XactEng.1.0.5558.h"
#include "HLEDataBase/XactEng.1.0.5788.h"
#include "HLEDataBase/XactEng.1.0.5849.h"

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
extern const struct HLEData
{
    const char *Library;

    uint16      BuildVersion;
    OOVPATable *OovpaTable;
    uint32      OovpaTableSize;
}
HLEDataBase[];

#define PAIRSCANSEC_MAX 2

const struct PairScanLibSec {
    const char *library;
    const char *section[PAIRSCANSEC_MAX];
};

extern const struct HLEDataV2
{
    const PairScanLibSec LibSec;

    OOVPATable  *OovpaTable;
    uint32          OovpaTableSize;
}
HLEDataBaseV2[];

// ******************************************************************
// * HLEDataBaseCount
// ******************************************************************
extern const uint32 HLEDataBaseCount;

extern const uint32 HLEDataBaseCountV2;

// ******************************************************************
// * XRefDataBaseOffset
// ******************************************************************
enum XRefDataBaseOffset
{
    XREF_D3DDEVICE=0,
    XREF_SETRENDERSTATE_CULLMODE,
	XREF_D3DRS_CULLMODE,
	XREF_D3DTSS_TEXCOORDINDEX,
    XREF_D3DRS_MULTISAMPLEMODE,
    XREF_D3DRS_MULTISAMPLERENDERTARGETMODE,
    XREF_D3DRS_ROPZCMPALWAYSREAD,
    XREF_D3DRS_ROPZREAD,
    XREF_D3DRS_DONOTCULLUNCOMPRESSED,
    XREF_D3DRS_STENCILCULLENABLE,
    XREF_D3DCD_LAZYSETSTATEVB,
    XREF_D3D_BlockOnResource,
    XREF_D3D_BLOCKONTIME,
    XREF_D3D_CreateStandAloneSurface,
    XREF_D3D_SETFENCE,
	XREF_D3D_KickOffAndWaitForIdle,
	XREF_D3D_MakeRequestedSpace,
	XREF_D3D_KickOff,
	XREF_D3DPalette_Lock2,
    XREF_XNINIT,
    XREF_FCLOSEDEVICE,
    XREF_CLEARSTATEBLOCKFLAGS,
    XREF_RECORDSTATEBLOCK,
    XREF_SETDISTANCEFACTORA,
    XREF_SETDISTANCEFACTORB,
    XREF_SETROLLOFFFACTORA,
    XREF_SETROLLOFFFACTORB,
    XREF_SETDOPPLERFACTOR,
    XREF_SETBUFFERDATA,
	XREF_CMcpxBuffer_SetCurrentPosition,
	XREF_CDirectSoundBuffer_SetCurrentPosition,
	XREF_CMcpxBuffer_GetCurrentPosition,
	XREF_CDirectSoundBuffer_GetCurrentPosition,
    XREF_CDirectSoundBuffer_Play,
	XREF_CMcpxBuffer_Stop,
	XREF_CDirectSoundBuffer_Stop,
    XREF_DSFLUSHEX2,
	XREF_DSSETBUFFERDATA,
	XREF_CMcpxBuffer_SetBufferData,
	XREF_CDirectSoundBuffer_SetBufferData,
    XREF_DSCREATESOUNDBUFFER,
    XREF_DSCREATESOUNDSTREAM,
    XREF_DSSTREAMPAUSE,
    XREF_DSSETI3DL2LISTENER,
    XREF_DSSETMIXBINHEADROOMA,
    XREF_DSSETMIXBINHEADROOMB,
    XREF_DSSETMIXBINSA,
    XREF_DSSETMIXBINSB,
    XREF_DSSETMIXBINSC,
    XREF_CDirectSoundBuffer_SetMixBinVolumes,
    XREF_CDirectSoundVoice_SetMixBinVolumes,
    XREF_CDirectSoundVoiceSettings_SetMixBinVolumes,
    XREF_DSSETPOSITIONA,
    XREF_DSSETPOSITIONB,
    XREF_DSSETVELOCITYA,
    XREF_DSSETVELOCITYB,
    XREF_DSDOWORKA,
    XREF_DSDOWORKB,
    XREF_DSSETALLPARAMETERSA,
    XREF_DSSETALLPARAMETERSB,
    XREF_DSSETHEADROOMA,
    XREF_DSENABLEHEADPHONES,
    XREF_DSSYNCHPLAYBACKA,
    XREF_DSSYNCHPLAYBACKB,
    XREF_DSSETI3DL2SOURCE1A,
    XREF_DSSETI3DL2SOURCE2A,
    XREF_DSBUFFERSETPITCHA,
    XREF_DSBUFFERSETPITCHB,
    XREF_DSBUFFERSETVOLUMEA,
    XREF_CMcpxVoiceClient_SetVolume,
    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XREF_DSBUFFERSETLOOPREGIONA,
    XREF_DSBUFFERSETPLAYREGIONA,
    XREF_DSBUFFERLOCKA,
    XREF_DSBUFFERSETHEADROOMA,
    XREF_DSBUFFERSETFREQUENCYA,
    XREF_CDirectSoundVoice_SetFrequency,
    XREF_CMcpxVoiceClient_Set3dMaxDistance,
    XREF_CDirectSoundVoice_SetMaxDistance,
    XREF_CDirectSoundStream_SetMaxDistance,
    XREF_CDirectSoundStream_SetMinDistance,
    XREF_CDirectSoundVoice_SetMinDistance,
    XREF_CMcpxVoiceClient_Set3dMinDistance,
    XREF_DSSTREAMSETCONEANGLES1A,
    XREF_DSSTREAMSETCONEANGLES1B,
    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A,
    XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B,
    XREF_DSSTREAMSETALLPARAMETERS1A,
    XREF_DSSTREAMSETALLPARAMETERS1B,
    XREF_DSSTREAMSETALLPARAMETERS1C,
    XREF_DSSTREAMSETVELOCITY1A,
    XREF_DSSTREAMSETVELOCITY1B,
    XREF_DSSTREAMSETVELOCITY1C,
    XREF_DSSTREAMSETCONEORIENTATION1A,
    XREF_DSSTREAMSETCONEORIENTATION1B,
    XREF_DSSTREAMSETCONEORIENTATION1C,
    XREF_DSSTREAMSETPOSITION1A,
    XREF_DSSTREAMSETPOSITION1B,
    XREF_DSSTREAMSETPOSITION1C,
    XREF_DSSTREAMSETFREQUENCY1A,
    XREF_DSSTREAMSETFREQUENCY1B,
    XREF_DSSTREAMSETROLLOFFFACTOR1A,
    XREF_DSSTREAMSETROLLOFFFACTOR1B,
    XREF_GET2DSURFACEDESCB,
    XREF_COMMONSETRENDERTARGET,
    XREF_COMMITDEFERREDSETTINGSA,
    XREF_COMMITDEFERREDSETTINGSB,
    // s+
    // DSOUND
    XREF_DSVOICESETMAXDISTANCE,
    XREF_DSVOICESETMINDISTANCE,
    XREF_DSVOICESETROLLOFFCURVE,
    XREF_DSVOICESETROLLOFFFACTOR,
    XREF_DSVOICESETDISTANCEFACTOR,
    XREF_DSVOICESETCONEANGLES,
    XREF_DSVOICESETCONEORIENTATION,
    XREF_DSVOICESETCONEOUTSIDEVOLUME,
    XREF_DSVOICESETPOSITION,
    XREF_DSVOICESETVELOCITY,
	XREF_CDirectSoundVoice_SetDopplerFactor,
    XREF_DSVOICESETI3DL2SOURCE,
    XREF_DSBUFFERSETMODEA,
    XREF_DSBUFFERSETMODEB,
    XREF_DSBUFFERSETMAXDISTANCE,
    XREF_DSBUFFERSETMINDISTANCE,
    XREF_DSBUFFERSETROLLOFFCURVE,
    XREF_DSBUFFERSETROLLOFFFACTOR,
    XREF_DSBUFFERSETDISTANCEFACTOR,
    XREF_DSBUFFERSETCONEANGLES,
    XREF_DSBUFFERSETCONEORIENTATION,
    XREF_DSBUFFERSETCONEOUTSIDEVOLUME,
    XREF_DSBUFFERSETPOSITION,
    XREF_DSBUFFERSETVELOCITY,
    XREF_DSBUFFERSETDOPPLERFACTOR,
    XREF_DSBUFFERSETI3DL2SOURCE,
    XREF_CDirectSoundVoice_SetFormat,
    XREF_CDirectSoundBuffer_SetFormat,
    XREF_CFullHRTFSource_GetCenterVolume,
    XREF_CHRTFSource_SetFullHRTF5Channel,
    XREF_CMcpxVoiceClient_SetLFO,
    XREF_CDirectSoundVoice_SetLFO,
    XREF_CDirectSoundBuffer_SetLFO,
    XREF_CDirectSoundVoice_SetVolume,
    XREF_CDirectSoundBuffer_SetVolume,
    XREF_CDirectSoundBuffer_SetLoopRegion,
    XREF_CMcpxBuffer_Play,
    XREF_CDirectSoundBuffer_PlayEx,
    XREF_WaveFormat_CreateXboxAdpcmFormat,
    XREF_CDirectSoundVoice_SetConeAngles,
    XREF_CDirectSoundBuffer_SetConeAngles,
    XREF_CDirectSoundVoice_CommitDeferredSettings,
    XREF_CDirectSoundVoice_SetAllParameters,
    XREF_CDirectSoundBuffer_SetAllParameters,
	XREF_CDirectSound_EnableHeadphones,
	XREF_CDirectSoundVoice_SetHeadroom,
	XREF_CDirectSoundBuffer_SetHeadroom,
	XREF_CDirectSound_GetOutputLevels,
	XREF_CMcpxBuffer_Pause,
	XREF_CDirectSoundBuffer_Pause,
	XREF_CMcpxBuffer_GetStatus,
	XREF_CDirectSoundBuffer_GetStatus,
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
	XREF_CDirectSound_GetCaps,
	XREF_CDirectSoundVoice_SetPitch,
	XREF_DirectSoundEnterCriticalSection,
	XREF_CDirectSoundBuffer_SetMinDistance,
	XREF_CDirectSoundBuffer_SetMaxDistance,
	XREF_CMcpxVoiceClient_Commit3dSettings,
	XREF_CMcpxVoiceClient_SetI3DL2Source,
	XREF_CDirectSoundVoice_SetI3DL2Source,
	XREF_CDirectSoundStream_SetMixBinVolumes,
	XREF_CDirectSoundStream_SetHeadroom,
	XREF_CDirectSoundVoice_SetOutputBuffer,
	XREF_CDirectSoundBuffer_SetOutputBuffer,
	XREF_CDirectSoundBuffer_SetNotificationPositions,
	XREF_CDirectSoundBuffer_SetEG,
	XREF_CDirectSound_GetEffectData,
	XREF_CMcpxBuffer_Play2,
	XREF_CDirectSoundVoice_Use3DVoiceData,
	XREF_CDirectSoundBuffer_Use3DVoiceData,
	XREF_CDirectSound_SetEffectData,
	// XACT
	// +s
	XREF_XACT_CEngine_RegisterWaveBank,
	XREF_XACT_CEngine_RegisterStreamedWaveBank,
	XREF_XACT_CEngine_CreateSoundBank,
	XREF_XACT_CEngine_DownloadEffectsImage,
	XREF_XACT_CEngine_CreateSoundSource,
	XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName,
	XREF_XACT_CEngine_RegisterNotification,
	XREF_XACT_CEngine_GetNotification,
	XREF_XACT_CEngine_UnRegisterWaveBank,
	// XONLINE
	// +s
	XREF_XoUpdateLaunchNewImageInternal,
	XREF_CXo_XOnlineLogon,

    // XAPI
    /* not necessary?
    XREF_XAPIXCALCULATESIGNATUREBEGINEX
    //*/
    // +s
//	XREF_XLoadSectionByHandle,
//	XREF_XFreeSectionByHandle,

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
