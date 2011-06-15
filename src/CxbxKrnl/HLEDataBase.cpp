// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#undef FIELD_OFFSET     // prevent macro redefinition warnings
#include <windows.h>

extern "C" const char *szHLELastCompileTime = __TIMESTAMP__;

#include "Emu.h"
#include "EmuXTL.h"
#include "HLEDataBase.h"
#include "HLEDataBase/Xapi.1.0.3911.inl"
#include "HLEDataBase/Xapi.1.0.4034.inl"
#include "HLEDataBase/Xapi.1.0.4134.inl"
#include "HLEDataBase/Xapi.1.0.4361.inl"
#include "HLEDataBase/Xapi.1.0.4432.inl"
#include "HLEDataBase/Xapi.1.0.4627.inl"
#include "HLEDataBase/Xapi.1.0.5233.inl"
#include "HLEDataBase/Xapi.1.0.5558.inl"
#include "HLEDataBase/Xapi.1.0.5849.inl"
#include "HLEDataBase/D3D8.1.0.3925.inl"
#include "HLEDataBase/D3D8.1.0.4034.inl"
#include "HLEDataBase/D3D8.1.0.4134.inl"
#include "HLEDataBase/D3D8.1.0.4361.inl"
#include "HLEDataBase/D3D8.1.0.4432.inl"
#include "HLEDataBase/D3D8.1.0.4627.inl"
#include "HLEDataBase/D3D8.1.0.5233.inl"
#include "HLEDataBase/D3D8.1.0.5558.inl"
#include "HLEDataBase/D3D8.1.0.5849.inl"
#include "HLEDataBase/D3D8LTCG.1.0.5849.inl"
#include "HLEDataBase/DSound.1.0.3936.inl"
#include "HLEDataBase/DSound.1.0.4134.inl"
#include "HLEDataBase/DSound.1.0.4361.inl"
#include "HLEDataBase/DSound.1.0.4432.inl"
#include "HLEDataBase/DSound.1.0.4627.inl"
#include "HLEDataBase/DSound.1.0.5233.inl"
#include "HLEDataBase/DSound.1.0.5558.inl"
#include "HLEDataBase/DSound.1.0.5849.inl"
#include "HLEDataBase/XG.1.0.3911.inl"
#include "HLEDataBase/XG.1.0.4034.inl"
#include "HLEDataBase/XG.1.0.4361.inl"
#include "HLEDataBase/XG.1.0.4432.inl"
#include "HLEDataBase/XG.1.0.4627.inl"
#include "HLEDataBase/XG.1.0.5233.inl"
#include "HLEDataBase/XG.1.0.5558.inl"
#include "HLEDataBase/XG.1.0.5849.inl"
#include "HLEDataBase/XNet.1.0.3911.inl"
#include "HLEDataBase/XOnline.1.0.4361.inl"
#include "HLEDataBase/XOnline.1.0.4627.inl"
#include "HLEDataBase/XOnline.1.0.5233.inl"
#include "HLEDataBase/XOnline.1.0.5849.inl"
#include "HLEDataBase/XactEng.1.0.4627.inl"

// ******************************************************************
// * HLEDataBase
// ******************************************************************
HLEData HLEDataBase[] =
{
    // Xapilib Version 1.0.3911
    {
        "XAPILIB",
        1, 0, 3911,
        XAPI_1_0_3911,
        XAPI_1_0_3911_SIZE
    },
    // Xapilib Version 1.0.4034
    {
        "XAPILIB",
        1, 0, 4034,
        XAPI_1_0_4034,
        XAPI_1_0_4034_SIZE
    },
    // Xapilib Version 1.0.4134
    {
        "XAPILIB",
        1, 0, 4134,
        XAPI_1_0_4134,
        XAPI_1_0_4134_SIZE
    },
    // Xapilib Version 1.0.4361
    {
        "XAPILIB",
        1, 0, 4361,
        XAPI_1_0_4361,
        XAPI_1_0_4361_SIZE
    },
    // Xapilib Version 1.0.4432
    {
        "XAPILIB",
        1, 0, 4432,
        XAPI_1_0_4432,
        XAPI_1_0_4432_SIZE
    },
    // Xapilib Version 1.0.4627
    {
        "XAPILIB",
        1, 0, 4627,
        XAPI_1_0_4627,
        XAPI_1_0_4627_SIZE
    },
    // Xapilib Version 1.0.5233
    {
        "XAPILIB",
        1, 0, 5233,
        XAPI_1_0_5233,
        XAPI_1_0_5233_SIZE
    },
    // Xapilib Version 1.0.5558
    {
        "XAPILIB",
        1, 0, 5558,
        XAPI_1_0_5558,
        XAPI_1_0_5558_SIZE
    },
    // Xapilib Version 1.0.5849
    {
        "XAPILIB",
        1, 0, 5849,
        XAPI_1_0_5849,
        XAPI_1_0_5849_SIZE
    },
    // D3D8 Version 1.0.3925
    {
        "D3D8",
        1, 0, 3925,
        D3D8_1_0_3925,
        D3D8_1_0_3925_SIZE
    },
    // D3D8 Version 1.0.4034
    {
        "D3D8",
        1, 0, 4034,
        D3D8_1_0_4034,
        D3D8_1_0_4034_SIZE
    },
    // D3D8 Version 1.0.4134
    {
        "D3D8",
        1, 0, 4134,
        D3D8_1_0_4134,
        D3D8_1_0_4134_SIZE
    },
    // D3D8 Version 1.0.4361
    {
        "D3D8",
        1, 0, 4361,
        D3D8_1_0_4361,
        D3D8_1_0_4361_SIZE
    },
    // D3D8 Version 1.0.4432
    {
        "D3D8",
        1, 0, 4432,
        D3D8_1_0_4432,
        D3D8_1_0_4432_SIZE
    },
    // D3D8 Version 1.0.4627
    {
        "D3D8",
        1, 0, 4627,
        D3D8_1_0_4627,
        D3D8_1_0_4627_SIZE
    },
    // D3D8 Version 1.0.5233
    {
        "D3D8",
        1, 0, 5233,
        D3D8_1_0_5233,
        D3D8_1_0_5233_SIZE
    },
    // D3D8 Version 1.0.5558
    {
        "D3D8",
        1, 0, 5558,
        D3D8_1_0_5558,
        D3D8_1_0_5558_SIZE
    },
    // D3D8 Version 1.0.5849
    {
        "D3D8",
        1, 0, 5849,
        D3D8_1_0_5849,
        D3D8_1_0_5849_SIZE
    },
	// D3D8LTCG Version 1.0.5849
    {
        "D3D8LTCG",
        1, 0, 5849,
        D3D8LTCG_1_0_5849,
        D3D8LTCG_1_0_5849_SIZE
    },
    // DSound Version 1.0.3936
    {
        "DSOUND",
        1, 0, 3936,
        DSound_1_0_3936,
        DSound_1_0_3936_SIZE
    },
	// DSound Version 1.0.4134
    {
        "DSOUND",
        1, 0, 4134,
        DSound_1_0_4134,
        DSound_1_0_4134_SIZE
    },
    // DSound Version 1.0.4361
    {
        "DSOUND",
        1, 0, 4361,
        DSound_1_0_4361,
        DSound_1_0_4361_SIZE
    },
    // DSound Version 1.0.4432
    {
        "DSOUND",
        1, 0, 4432,
        DSound_1_0_4432,
        DSound_1_0_4432_SIZE
    },
    // DSound Version 1.0.4627
    {
        "DSOUND",
        1, 0, 4627,
        DSound_1_0_4627,
        DSound_1_0_4627_SIZE
    },
    // DSound Version 1.0.5233
    {
        "DSOUND",
        1, 0, 5233,
        DSound_1_0_5233,
        DSound_1_0_5233_SIZE
    },
    // DSound Version 1.0.5558
    {
        "DSOUND",
        1, 0, 5558,
        DSound_1_0_5558,
        DSound_1_0_5558_SIZE
    },
    // DSound Version 1.0.5849
    {
        "DSOUND",
        1, 0, 5849,
        DSound_1_0_5849,
        DSound_1_0_5849_SIZE
    },
	// XG Version 1.0.3911
    {
        "XGRAPHC",
        1, 0, 3911,
        XG_1_0_3911,
        XG_1_0_3911_SIZE
    },
	// XG Version 1.0.4034
    {
        "XGRAPHC",
        1, 0, 4034,
        XG_1_0_4034,
        XG_1_0_4034_SIZE
    },
    // XG Version 1.0.4361
    {
        "XGRAPHC",
        1, 0, 4361,
        XG_1_0_4361,
        XG_1_0_4361_SIZE
    },
	// XG Version 1.0.4432
    {
        "XGRAPHC",
        1, 0, 4432,
        XG_1_0_4432,
        XG_1_0_4432_SIZE
    },
    // XG Version 1.0.4627
    {
        "XGRAPHC",
        1, 0, 4627,
        XG_1_0_4627,
        XG_1_0_4627_SIZE
    },
    // XG Version 1.0.5233
    {
        "XGRAPHC",
        1, 0, 5233,
        XG_1_0_5233,
        XG_1_0_5233_SIZE
    },
    // XG Version 1.0.5558
    {
        "XGRAPHC",
        1, 0, 5558,
        XG_1_0_5558,
        XG_1_0_5558_SIZE
    },
	// XG Version 1.0.5849
    {
        "XGRAPHC",
        1, 0, 5849,
        XG_1_0_5849,
        XG_1_0_5849_SIZE
    },
    // XNet Version 1.0.3911
    {
        "XNETS",
        1, 0, 3911,
        XNet_1_0_3911,
        XNet_1_0_3911_SIZE
    },
    // XOnline Version 1.0.4361
    {
        "XONLINE",
        1, 0, 4361,
        XOnline_1_0_4361,
        XOnline_1_0_4361_SIZE
    },
	// XOnline(s) Version 1.0.4627
	// TODO: Verify differences between XONLINE and XONLINES (if any)
    {
        "XONLINES",
        1, 0, 4627,
        XOnline_1_0_4627,
        XOnline_1_0_4627_SIZE
    },
	// XOnline(s) Version 1.0.5233
	{
        "XONLINES",
        1, 0, 5233,
        XOnline_1_0_5233,
        XOnline_1_0_5233_SIZE
    },
	// XOnline(s) Version 1.0.5849
	{
        "XONLINES",
        1, 0, 5849,
        XOnline_1_0_5849,
        XOnline_1_0_5849_SIZE
    },
	// XactEng Version 1.0.4627
	{
		"XACTENG",
		1, 0, 4627,
		XactEng_1_0_4627,
		XactEng_1_0_4627_SIZE
	},
};

// ******************************************************************
// * HLEDataBaseSize
// ******************************************************************
extern uint32 HLEDataBaseSize = sizeof(HLEDataBase);

// ******************************************************************
// * XRefDataBase
// ******************************************************************
extern uint32 XRefDataBase[] =
{
    -1, // XREF_D3DDEVICE
    -1, // XREF_DXSRSCULLMODE
    -1, // XREF_D3DRS_MULTISAMPLEMODE
    -1, // XREF_D3DRS_MULTISAMPLERENDERTARGETMODE
    -1, // XREF_D3DRS_ROPZCMPALWAYSREAD
    -1, // XREF_D3DRS_ROPZREAD
    -1, // XREF_D3DRS_DONOTCULLUNCOMPRESSED
    -1, // XREF_D3DRS_STENCILCULLENABLE
    -1, // XREF_D3DCD_LAZYSETSTATEVB
    -1, // XREF_D3D_BLOCKONTIME
    -1, // XREF_D3D_SETFENCE
	-1, // XREF_D3D_MakeRequestedSpace
    -1, // XREF_XNINIT
    -1, // XREF_FCLOSEDEVICE
    -1, // XREF_CLEARSTATEBLOCKFLAGS
    -1, // XREF_RECORDSTATEBLOCK
    -1, // XREF_SETDISTANCEFACTORA
    -1, // XREF_SETDISTANCEFACTORB
    -1, // XREF_SETROLLOFFFACTORA
    -1, // XREF_SETROLLOFFFACTORB
    -1, // XREF_SETDOPPLERFACTOR
    -1, // XREF_SETBUFFERDATA
    -1, // XREF_SETCURRENTPOSITION
    -1, // XREF_SETCURRENTPOSITION2
    -1, // XREF_GETCURRENTPOSITION
    -1, // XREF_GETCURRENTPOSITION2
    -1, // XREF_DSOUNDPLAY
    -1, // XREF_DSOUNDPLAY2
    -1, // XREF_DSOUNDSTOP
    -1, // XREF_DSOUNDSTOP2
    -1, // XREF_DSFLUSHEX2
    -1, // XREF_DSSETBUFFERDATA
    -1, // XREF_DSSETBUFFERDATA2
    -1, // XREF_DSCREATESOUNDBUFFER
    -1, // XREF_DSCREATESOUNDSTREAM
    -1, // XREF_DSSTREAMPAUSE
    -1, // XREF_DSSTREAMSETVOLUME
    -1, // XREF_DSSETI3DL2LISTENER
    -1, // XREF_DSSETMIXBINHEADROOMA
    -1, // XREF_DSSETMIXBINHEADROOMB
    -1, // XREF_DSSETMIXBINSA
    -1, // XREF_DSSETMIXBINSB
    -1, // XREF_DSSETMIXBINSC
    -1, // XREF_DSSETMIXBINVOLUMESA
    -1, // XREF_DSSETMIXBINVOLUMESB
    -1, // XREF_DSSETMIXBINVOLUMESC
    -1, // XREF_DSSETPOSITIONA
    -1, // XREF_DSSETPOSITIONB
    -1, // XREF_DSSETVELOCITYA
    -1, // XREF_DSSETVELOCITYB
    -1, // XREF_DSDOWORKA
    -1, // XREF_DSDOWORKB
    -1, // XREF_DSSETALLPARAMETERSA
    -1, // XREF_DSSETALLPARAMETERSB
    -1, // XREF_DSSETHEADROOMA
    -1, // XREF_DSENABLEHEADPHONES
    -1, // XREF_DSSYNCHPLAYBACKA
    -1, // XREF_DSSYNCHPLAYBACKB
    -1, // XREF_DSSETI3DL2SOURCE1A (Stream)
    -1, // XREF_DSSETI3DL2SOURCE2A (Buffer)
    -1, // XREF_DSBUFFERSETPITCHA (Buffer)
    -1, // XREF_DSBUFFERSETPITCHB (Buffer)
    -1, // XREF_DSBUFFERGETSTATUSA (Buffer)
    -1, // XREF_DSBUFFERGETSTATUSB (Buffer)
    -1, // XREF_DSBUFFERSETVOLUMEA (Buffer)
    -1, // XREF_DSBUFFERSETVOLUMEB (Buffer)
    -1, // XREF_DSBUFFERSETCURRENTPOSITIONA (Buffer)
    -1, // XREF_DSBUFFERSETCURRENTPOSITIONB (Buffer)
    -1, // XREF_DSBUFFERSETLOOPREGIONA (Buffer)
    -1, // XREF_DSBUFFERSETPLAYREGIONA (Buffer)
    -1, // XREF_DSBUFFERLOCKA (Buffer)
    -1, // XREF_DSBUFFERSETHEADROOMA (Buffer)
    -1, // XREF_DSBUFFERSETFREQUENCYA (Buffer)
    -1, // XREF_DSBUFFERSETFREQUENCYB
    -1, // XREF_DSSTREAMSETMAXDISTANCE1A (Stream)
    -1, // XREF_DSSTREAMSETMAXDISTANCE1B
    -1, // XREF_DSSTREAMSETMAXDISTANCE1C
    -1, // XREF_DSSTREAMSETMINDISTANCE1A (Stream)
    -1, // XREF_DSSTREAMSETMINDISTANCE1B
    -1, // XREF_DSSTREAMSETMINDISTANCE1C
    -1, // XREF_DSSTREAMSETCONEANGLES1A
    -1, // XREF_DSSTREAMSETCONEANGLES1B
    -1, // XREF_DSSTREAMSETCONEOUTSIDEVOLUME1A (Stream)
    -1, // XREF_DSSTREAMSETCONEOUTSIDEVOLUME1B
    -1, // XREF_DSSTREAMSETALLPARAMETERS1A (Stream)
    -1, // XREF_DSSTREAMSETALLPARAMETERS1B
    -1, // XREF_DSSTREAMSETALLPARAMETERS1C
    -1, // XREF_DSSTREAMSETVELOCITY1A (Stream)
    -1, // XREF_DSSTREAMSETVELOCITY1B
    -1, // XREF_DSSTREAMSETVELOCITY1C
    -1, // XREF_DSSTREAMSETCONEORIENTATION1A (Stream)
    -1, // XREF_DSSTREAMSETCONEORIENTATION1B
    -1, // XREF_DSSTREAMSETCONEORIENTATION1C
    -1, // XREF_DSSTREAMSETPOSITION1A (Stream)
    -1, // XREF_DSSTREAMSETPOSITION1B
    -1, // XREF_DSSTREAMSETPOSITION1C
    -1, // XREF_DSSTREAMSETFREQUENCY1A (Stream)
    -1, // XREF_DSSTREAMSETFREQUENCY1B
    -1, // XREF_DSSTREAMSETROLLOFFFACTOR1A (Stream)
    -1, // XREF_DSSTREAMSETROLLOFFFACTOR1B
    -1, // XREF_GET2DSURFACEDESCB
    -1, // XREF_COMMONSETRENDERTARGET
    -1, // XREF_COMMITDEFERREDSETTINGSA
    -1, // XREF_COMMITDEFERREDSETTINGSB
    // s+
    -1, // XREF_DSVOICESETMAXDISTANCE
    -1, // XREF_DSVOICESETMINDISTANCE
    -1, // XREF_DSVOICESETROLLOFFCURVE
    -1, // XREF_DSVOICESETROLLOFFFACTOR
    -1, // XREF_DSVOICESETDISTANCEFACTOR
    -1, // XREF_DSVOICESETCONEANGLES
    -1, // XREF_DSVOICESETCONEORIENTATION
    -1, // XREF_DSVOICESETCONEOUTSIDEVOLUME
    -1, // XREF_DSVOICESETPOSITION
    -1, // XREF_DSVOICESETVELOCITY
    -1, // XREF_DSVOICESETDOPPLERFACTOR
    -1, // XREF_DSVOICESETI3DL2SOURCE
    -1, // XREF_DSBUFFERSETMODEA
    -1, // XREF_DSBUFFERSETMODEB
    -1, // XREF_DSBUFFERSETMAXDISTANCE
    -1, // XREF_DSBUFFERSETMINDISTANCE
    -1, // XREF_DSBUFFERSETROLLOFFCURVE
    -1, // XREF_DSBUFFERSETROLLOFFFACTOR
    -1, // XREF_DSBUFFERSETDISTANCEFACTOR
    -1, // XREF_DSBUFFERSETCONEANGLES
    -1, // XREF_DSBUFFERSETCONEORIENTATION
    -1, // XREF_DSBUFFERSETCONEOUTSIDEVOLUME
    -1, // XREF_DSBUFFERSETPOSITION
    -1, // XREF_DSBUFFERSETVELOCITY
    -1, // XREF_DSBUFFERSETDOPPLERFACTOR
    -1, // XREF_DSBUFFERSETI3DL2SOURCE
    -1, // XREF_DirectSound_CDirectSoundVoice_SetFormat
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetFormat
    -1, // XREF_DirectSound_CFullHRTFSource_GetCenterVolume
    -1, // XREF_DirectSound_CHRTFSource_SetFullHRTF5Channel
    -1, // XREF_D3D_BlockOnResource
    -1, // XREF_DirectSound_CMcpxVoiceClient_SetLFO
    -1, // XREF_DirectSound_CDirectSoundVoice_SetLFO
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetLFO
    -1, // XREF_DirectSound_CDirectSoundVoice_SetVolume
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetVolume
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetLoopRegion
    -1, // XREF_DirectSound_CMcpxBuffer_Play
    -1, // XREF_DirectSound_CDirectSoundBuffer_PlayEx
    -1, // XREF_DirectSound_WaveFormat_CreateXboxAdpcmFormat
    -1, // XREF_DirectSound_CDirectSoundVoice_SetConeAngles
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetConeAngles
    -1, // XREF_DirectSound_CDirectSoundVoice_CommitDeferredSettings
    -1, // XREF_DirectSound_CDirectSoundVoice_SetAllParameters
    -1, // XREF_DirectSound_CDirectSoundBuffer_SetAllParameters
	-1, // XREF_DirectSound_CDirectSound_EnableHeadphones
	-1, // XREF_DirectSound_CDirectSoundVoice_SetHeadroom
	-1, // XREF_DirectSound_CDirectSoundBuffer_SetHeadroom
	-1, // XREF_DirectSound_CDirectSound_GetOutputLevels
	-1, // XREF_DirectSound_CMcpxBuffer_Pause
	-1, // XREF_DirectSound_CDirectSoundBuffer_Pause
	-1, // XREF_DirectSound_CMcpxBuffer_GetStatus
	-1, // XREF_DirectSound_CDirectSoundBuffer_GetStatus
	-1, // XREF_DirectSound_CDirectSoundVoice_SetEG
	-1, // XREF_DirectSound_CMcpxVoiceClient_SetEG
	-1,	// XREF_DirectSound_CMcpxVoiceClient_SetFilter
	-1,	// XREF_DirectSound_CDirectSoundVoice_SetFilter
	-1,	// XREF_DirectSound_CDirectSoundBuffer_SetFilter
	-1,	// XREF_DirectSound_CDirectSoundStream_SetFilter
	-1, // XREF_DirectSound_CDirectSoundBuffer_StopEx
	-1,	// XREF_CMcpxBuffer_Stop2
	-1, // XREF_CMcpxAPU_Set3dDopplerFactor
	-1, // XREF_CSensaura3d_GetFullHRTFFilterPair
	-1, // XREF_CDirectSound_GetCaps
	-1, // XREF_CDirectSoundVoice_SetPitch
	-1, // XREF_DirectSoundEnterCriticalSection
	-1, // XREF_CDirectSoundBuffer_SetMinDistance
	-1, // XREF_CDirectSoundBuffer_SetMaxDistance
	-1, // XREF_CMcpxVoiceClient_Commit3dSettings
	-1,	// XREF_CMcpxVoiceClient_SetI3DL2Source
	-1,	// XREF_CDirectSoundVoice_SetI3DL2Source
	-1,	// XREF_CDirectSoundVoice_SetMixBinVolumes
	-1, // XREF_CDirectSoundStream_SetMixBinVolumes
	-1, // XREF_DirectSound_CDirectSoundStream_SetHeadroom
	-1, // XREF_DirectSound_CDirectSoundVoice_SetOutputBuffer
	-1, // XREF_DirectSound_CDirectSoundBuffer_SetOutputBuffer
	-1, // XREF_DirectSound_CDirectSoundBuffer_SetNotificationPositions
	-1, // XREF_DirectSound_CDirectSoundBuffer_SetEG
	-1, // XREF_DirectSound_CDirectSound_GetEffectData
	// XACT
	-1, // XREF_XACT_CEngine_RegisterWaveBank
	-1, // XREF_XACT_CEngine_RegisterStreamedWaveBank
	-1, // XREF_XACT_CEngine_CreateSoundBank
	-1, // XREF_XACT_CEngine_DownloadEffectsImage
	-1, // XREF_XACT_CEngine_CreateSoundSource
	-1, // XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName
	-1, // XREF_XACT_CEngine_RegisterNotification
	-1, // XREF_XACT_CEngine_GetNotification
	-1, // XREF_XACT_CEngine_UnRegisterWaveBank
	// XONLINE
	-1, // XREF_XoUpdateLaunchNewImageInternal
	-1, // XREF_CXo_XOnlineLogon
	-1,
    /* not necessary?
    -1, //XREF_XAPIXCALCULATESIGNATUREBEGINEX
    //*/
    // +s
};

// ******************************************************************
// * track XRef location
// ******************************************************************
extern uint32 UnResolvedXRefs = sizeof(XRefDataBase)/sizeof(uint32);

// ******************************************************************
// * Search Speed Optimization
// ******************************************************************
extern bool bXRefFirstPass = true;
