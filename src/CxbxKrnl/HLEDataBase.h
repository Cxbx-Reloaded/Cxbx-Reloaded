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
#include "HLEDataBase/Xapi.1.0.5233.h"
#include "HLEDataBase/Xapi.1.0.5558.h"
#include "HLEDataBase/Xapi.1.0.5849.h"
#include "HLEDataBase/D3D8.1.0.3925.h"
#include "HLEDataBase/D3D8.1.0.4034.h"
#include "HLEDataBase/D3D8.1.0.4134.h"
#include "HLEDataBase/D3D8.1.0.4361.h"
#include "HLEDataBase/D3D8.1.0.4432.h"
#include "HLEDataBase/D3D8.1.0.4627.h"
#include "HLEDataBase/D3D8.1.0.5233.h"
#include "HLEDataBase/D3D8.1.0.5558.h"
#include "HLEDataBase/D3D8.1.0.5849.h"
#include "HLEDataBase/D3D8LTCG.1.0.5849.h"
#include "HLEDataBase/DSound.1.0.3936.h"
#include "HLEDataBase/DSound.1.0.4134.h"
#include "HLEDataBase/DSound.1.0.4361.h"
#include "HLEDataBase/DSound.1.0.4432.h"
#include "HLEDataBase/DSound.1.0.4627.h"
#include "HLEDataBase/DSound.1.0.5233.h"
#include "HLEDataBase/DSound.1.0.5558.h"
#include "HLEDataBase/DSound.1.0.5849.h"
#include "HLEDataBase/XG.1.0.3911.h"
#include "HLEDataBase/XG.1.0.4034.h"
#include "HLEDataBase/XG.1.0.4361.h"
#include "HLEDataBase/XG.1.0.4432.h"
#include "HLEDataBase/XG.1.0.4627.h"
#include "HLEDataBase/XG.1.0.5233.h"
#include "HLEDataBase/XG.1.0.5558.h"
#include "HLEDataBase/XG.1.0.5849.h"
#include "HLEDataBase/XNet.1.0.3911.h"
#include "HLEDataBase/XOnline.1.0.4361.h"
#include "HLEDataBase/XOnline.1.0.4627.h"
#include "HLEDataBase/XOnline.1.0.5233.h"
#include "HLEDataBase/XOnline.1.0.5558.h"
#include "HLEDataBase/XOnline.1.0.5849.h"
#include "HLEDataBase/XactEng.1.0.4627.h"

// ******************************************************************
// * szHLELastCompileTime
// ******************************************************************
extern "C" const char *szHLELastCompileTime;

extern const char *Lib_D3D8;
extern const char *Lib_D3D8LTCG;
extern const char *Lib_D3DX8;
extern const char *Lib_DSOUND;
extern const char *Lib_XACTENG;
extern const char *Lib_XAPILIB;
extern const char *Lib_XGRAPHC;
extern const char *Lib_XNETS;
extern const char *Lib_XONLINE; // TODO : Typo for XONLINES?
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
    XREF_DXSRSCULLMODE,
    XREF_D3DRS_MULTISAMPLEMODE,
    XREF_D3DRS_MULTISAMPLERENDERTARGETMODE,
    XREF_D3DRS_ROPZCMPALWAYSREAD,
    XREF_D3DRS_ROPZREAD,
    XREF_D3DRS_DONOTCULLUNCOMPRESSED,
    XREF_D3DRS_STENCILCULLENABLE,
    XREF_D3DCD_LAZYSETSTATEVB,
    XREF_D3D_BLOCKONTIME,
    XREF_D3D_SETFENCE,
	XREF_D3D_KickOffAndWaitForIdle,
	XREF_D3D_MakeRequestedSpace,
	XREF_D3D_KickOff,
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
    XREF_SETCURRENTPOSITION,
    XREF_SETCURRENTPOSITION2,
    XREF_GETCURRENTPOSITION,
    XREF_GETCURRENTPOSITION2,
    XREF_DSOUNDPLAY,
    XREF_DSOUNDPLAY2,
    XREF_DSOUNDSTOP,
    XREF_DSOUNDSTOP2,
    XREF_DSFLUSHEX2,
    XREF_DSSETBUFFERDATA,
    XREF_DSSETBUFFERDATA2,
    XREF_DSCREATESOUNDBUFFER,
    XREF_DSCREATESOUNDSTREAM,
    XREF_DSSTREAMPAUSE,
    XREF_DSSTREAMSETVOLUME,
    XREF_DSSETI3DL2LISTENER,
    XREF_DSSETMIXBINHEADROOMA,
    XREF_DSSETMIXBINHEADROOMB,
    XREF_DSSETMIXBINSA,
    XREF_DSSETMIXBINSB,
    XREF_DSSETMIXBINSC,
    XREF_DSSETMIXBINVOLUMESA,
    XREF_DSSETMIXBINVOLUMESB,
    XREF_DSSETMIXBINVOLUMESC,
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
    XREF_DSBUFFERGETSTATUSA,
    XREF_DSBUFFERGETSTATUSB,
    XREF_DSBUFFERSETVOLUMEA,
    XREF_DSBUFFERSETVOLUMEB,
    XREF_DSBUFFERSETCURRENTPOSITIONA,
    XREF_DSBUFFERSETCURRENTPOSITIONB,
    XREF_DSBUFFERSETLOOPREGIONA,
    XREF_DSBUFFERSETPLAYREGIONA,
    XREF_DSBUFFERLOCKA,
    XREF_DSBUFFERSETHEADROOMA,
    XREF_DSBUFFERSETFREQUENCYA,
    XREF_DSBUFFERSETFREQUENCYB,
    XREF_DSSTREAMSETMAXDISTANCE1A,
    XREF_DSSTREAMSETMAXDISTANCE1B,
    XREF_DSSTREAMSETMAXDISTANCE1C,
    XREF_DSSTREAMSETMINDISTANCE1A,
    XREF_DSSTREAMSETMINDISTANCE1B,
    XREF_DSSTREAMSETMINDISTANCE1C,
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
    XREF_DSVOICESETDOPPLERFACTOR,
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
    XREF_DirectSound_CDirectSoundVoice_SetFormat,
    XREF_DirectSound_CDirectSoundBuffer_SetFormat,
    XREF_DirectSound_CFullHRTFSource_GetCenterVolume,
    XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel,
    XREF_D3D_BlockOnResource,
    XREF_DirectSound_CMcpxVoiceClient_SetLFO,
    XREF_DirectSound_CDirectSoundVoice_SetLFO,
    XREF_DirectSound_CDirectSoundBuffer_SetLFO,
    XREF_DirectSound_CDirectSoundVoice_SetVolume,
    XREF_DirectSound_CDirectSoundBuffer_SetVolume,
    XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion,
    XREF_DirectSound_CMcpxBuffer_Play,
    XREF_DirectSound_CDirectSoundBuffer_PlayEx,
    XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat,
    XREF_DirectSound_CDirectSoundVoice_SetConeAngles,
    XREF_DirectSound_CDirectSoundBuffer_SetConeAngles,
    XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings,
    XREF_DirectSound_CDirectSoundVoice_SetAllParameters,
    XREF_DirectSound_CDirectSoundBuffer_SetAllParameters,
	XREF_DirectSound_CDirectSound_EnableHeadphones,
	XREF_DirectSound_CDirectSoundVoice_SetHeadroom,
	XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,
	XREF_DirectSound_CDirectSound_GetOutputLevels,
	XREF_DirectSound_CMcpxBuffer_Pause,
	XREF_DirectSound_CDirectSoundBuffer_Pause,
	XREF_DirectSound_CMcpxBuffer_GetStatus,
	XREF_DirectSound_CDirectSoundBuffer_GetStatus,
	XREF_DirectSound_CDirectSoundVoice_SetEG,
	XREF_DirectSound_CMcpxVoiceClient_SetEG,
	XREF_DirectSound_CMcpxVoiceClient_SetFilter,
	XREF_DirectSound_CDirectSoundVoice_SetFilter,
	XREF_DirectSound_CDirectSoundBuffer_SetFilter,
	XREF_DirectSound_CDirectSoundStream_SetFilter,
	XREF_DirectSound_CDirectSoundBuffer_StopEx,
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
	XREF_CDirectSoundVoice_SetMixBinVolumes,
	XREF_CDirectSoundStream_SetMixBinVolumes,
	XREF_DirectSound_CDirectSoundStream_SetHeadroom,
	XREF_DirectSound_CDirectSoundVoice_SetOutputBuffer,
	XREF_DirectSound_CDirectSoundBuffer_SetOutputBuffer,
	XREF_DirectSound_CDirectSoundBuffer_SetNotificationPositions,
	XREF_DirectSound_CDirectSoundBuffer_SetEG,
	XREF_DirectSound_CDirectSound_GetEffectData,
	XREF_CDirectSoundBuffer_SetNotificationPositions,
	XREF_DirectSound_CMcpxBuffer_Play2,
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
	// Also, if XREF_COUNT > sizeof(byte), enlarge struct OOVPA.XRefSaveIndex (and Value somehow)
};

#define XREF_UNKNOWN -1

// ******************************************************************
// * XRefDataBase
// ******************************************************************
extern uint32 XRefDataBase[XREF_COUNT];

#endif /*HLEDATABASE_H*/
