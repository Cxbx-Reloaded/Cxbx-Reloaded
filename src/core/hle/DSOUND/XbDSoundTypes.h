// ******************************************************************
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
// *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *  (c) 2017-2019 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef XBDSOUNDTYPES_H
#define XBDSOUNDTYPES_H

#include <dsound.h> // TODO: FIXME after global XTL namespace issue is resolved.
// TODO: Port PC dsound.h to xbox edition here base on preivous research.
// TODO: Also need to use fixed size to able support cross-platform without extra work.
//       Such as uint32_t, uint16_t, etc.

#define WAVE_FORMAT_XBOX_ADPCM 0x0069

// XDSMIXBIN Flags
#define XDSMIXBIN_FRONT_LEFT        0
#define XDSMIXBIN_FRONT_RIGHT       1
#define XDSMIXBIN_FRONT_CENTER      2
#define XDSMIXBIN_LOW_FREQUENCY     3
#define XDSMIXBIN_BACK_LEFT         4
#define XDSMIXBIN_BACK_RIGHT        5
#define XDSMIXBIN_SPEAKERS_MAX      6 // Max count for speakers
// Other flags are used 

// ******************************************************************
// * X_DSMIXBINVOLUMEPAIR
// ******************************************************************
typedef struct _XDSMIXBINVOLUMEPAIR {
    DWORD       dwMixBin;
    LONG        lVolume;
} X_DSMIXBINVOLUMEPAIR, *X_LPDSMIXBINVOLUMEPAIR;

// ******************************************************************
// * X_DSMIXBINS
// ******************************************************************
typedef struct _XDSMIXBINS {
    DWORD                       dwCount;
    X_LPDSMIXBINVOLUMEPAIR     lpMixBinVolumePairs;
} X_DSMIXBINS, *X_LPDSMIXBINS;

// EmuIDirectSoundBuffer_Play flags
#define X_DSBPLAY_LOOPING       0x00000001
#define X_DSBPLAY_FROMSTART     0x00000002
#define X_DSBPLAY_SYNCHPLAYBACK 0x00000004

// EmuIDirectSoundBuffer_Pause flags
#define X_DSBPAUSE_RESUME             0x00000000
#define X_DSBPAUSE_PAUSE              0x00000001
#define X_DSBPAUSE_SYNCHPLAYBACK      0x00000002

// EmuIDirectSoundStream_Pause flags
#define X_DSSPAUSE_RESUME             0x00000000
#define X_DSSPAUSE_PAUSE              0x00000001
#define X_DSSPAUSE_SYNCHPLAYBACK      0x00000002
#define X_DSSPAUSE_PAUSENOACTIVATE    0x00000003

// EmuIDirectSoundStream_FlushEx flags
#define X_DSSFLUSHEX_IMMEDIATE        0x00000000
#define X_DSSFLUSHEX_ASYNC            0x00000001
#define X_DSSFLUSHEX_ENVELOPE         0x00000002
#define X_DSSFLUSHEX_ENVELOPE2        0x00000004

// EmuIDirectSoundStream_GetStatus flags
#define X_DSSSTATUS_READY             0x00000001
#define X_DSSSTATUS_PLAYING           0x00010000
#define X_DSSSTATUS_PAUSED            0x00020000
#define X_DSSSTATUS_STARVED           0x00040000
#define X_DSSSTATUS_ENVELOPECOMPLETE  0x00080000

// EmuIDirectSoundBuffer_GetStatus flags
#define X_DSBSTATUS_PLAYING           0x00000001
#define X_DSBSTATUS_PAUSED            0x00000002
#define X_DSBSTATUS_LOOPING           0x00000004

// EmuIDirectSoundBuffer_StopEx flags
#define X_DSBSTOPEX_IMMEDIATE         0x00000000
#define X_DSBSTOPEX_ENVELOPE          0x00000001
#define X_DSBSTOPEX_RELEASEWAVEFORM   0x00000002
#define X_DSBSTOPEX_ALL               (X_DSBSTOPEX_ENVELOPE | X_DSBSTOPEX_RELEASEWAVEFORM)

#define XTL_DSBCAPS_CTRL3D               0x00000010
#define XTL_DSBCAPS_CTRLFREQUENCY        0x00000020
#define XTL_DSBCAPS_CTRLVOLUME           0x00000080
#define XTL_DSBCAPS_CTRLPOSITIONNOTIFY   0x00000100
#define XTL_DSBCAPS_MIXIN                0x00002000
#define XTL_DSBCAPS_MUTE3DATMAXDISTANCE  0x00020000
#define XTL_DSBCAPS_LOCDEFER             0x00040000
#define XTL_DSBCAPS_FXIN                 0x00080000
#define XTL_DSBCAPS_FXIN2                0x00100000

// ******************************************************************
// * X_DSBUFFERDESC
// ******************************************************************
struct X_DSBUFFERDESC
{
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwBufferBytes;
    LPWAVEFORMATEX  lpwfxFormat;
    X_LPDSMIXBINS   lpMixBinsOutput;
    DWORD           dwInputMixBin;
};

// ******************************************************************
// * X_DSENVELOPEDESC
// ******************************************************************
struct X_DSENVOLOPEDESC
{
    DWORD           dwEnvelopGenerator;
    DWORD           dwMode;
    DWORD           dwDelay;
    DWORD           dwAttack;
    DWORD           dwHold;
    DWORD           dwDecay;
    DWORD           dwRelease;
    DWORD           dwSustain;
    LONG            lPitchScale;
    LONG            lFilterCutOff;
};

#define XTL_DSSCAPS_CTRL3D               0x00000010
#define XTL_DSSCAPS_CTRLFREQUENCY        0x00000020
#define XTL_DSSCAPS_CTRLVOLUME           0x00000080
#define XTL_DSSCAPS_MUTE3DATMAXDISTANCE  0x00020000
#define XTL_DSSCAPS_LOCDEFER             0x00040000
#define XTL_DSSCAPS_NOMERGE              0x20000000
#define XTL_DSSCAPS_ACCURATENOTIFY       0x40000000

typedef VOID(CALLBACK *LPFNXMOCALLBACK)(LPVOID pStreamContext, LPVOID pPacketContext, DWORD dwStatus);

// ******************************************************************
// * X_DSSTREAMDESC
// ******************************************************************
struct X_DSSTREAMDESC
{
    DWORD                       dwFlags;
    DWORD                       dwMaxAttachedPackets;
    LPWAVEFORMATEX              lpwfxFormat;
    LPFNXMOCALLBACK             lpfnCallback;
    LPVOID                      lpvContext;
    X_LPDSMIXBINS               lpMixBinsOutput;
};

// ******************************************************************
// * REFERENCE_TIME
// ******************************************************************
typedef LONGLONG REFERENCE_TIME, *PREFERENCE_TIME, *LPREFERENCE_TIME;

// ******************************************************************
// * XMEDIAPACKET
// ******************************************************************
typedef struct _XMEDIAPACKET
{
    LPVOID pvBuffer;
    DWORD dwMaxSize;
    PDWORD pdwCompletedSize;
    PDWORD pdwStatus;
    union {
        HANDLE  hCompletionEvent;
        PVOID  pContext;
    };
    PREFERENCE_TIME prtTimestamp; // Not supported in xbox
}
XMEDIAPACKET, *PXMEDIAPACKET, *LPXMEDIAPACKET;

#define XMP_STATUS_SUCCESS          S_OK
#define XMP_STATUS_PENDING          E_PENDING
#define XMP_STATUS_FLUSHED          E_ABORT
#define XMP_STATUS_FAILURE          E_FAIL
#define XMP_STATUS_RELEASE_CXBXR    0xFFFFFFFF

// ******************************************************************
// * XMEDIAINFO
// ******************************************************************
typedef struct _XMEDIAINFO
{
    DWORD  dwFlags;
    DWORD  dwInputSize;
    DWORD  dwOutputSize;
    DWORD  dwMaxLookahead;
}
XMEDIAINFO, *PXEIDIAINFO, *LPXMEDIAINFO;

// XMEDIAINFO Flags
#define XMO_STREAMF_FIXED_SAMPLE_SIZE           0x00000001      // The object supports only a fixed sample size
#define XMO_STREAMF_FIXED_PACKET_ALIGNMENT      0x00000002      // The object supports only a fixed packet alignment
#define XMO_STREAMF_INPUT_ASYNC                 0x00000004      // The object supports receiving input data asynchronously
#define XMO_STREAMF_OUTPUT_ASYNC                0x00000008      // The object supports providing output data asynchronously
#define XMO_STREAMF_IN_PLACE                    0x00000010      // The object supports in-place modification of data
#define XMO_STREAMF_MASK                        0x0000001F

// ******************************************************************
// * X_DSFILTERDESC
// ******************************************************************
struct X_DSFILTERDESC
{
    DWORD dwMode;
    DWORD dwQCoefficient;
    DWORD adwCoefficients[4];
};

// X_DSFILTERDESC modes
#define DSFILTER_MODE_BYPASS        0x00000000      // The filter is bypassed
#define DSFILTER_MODE_DLS2          0x00000001      // DLS2 mode
#define DSFILTER_MODE_PARAMEQ       0x00000002      // Parametric equalizer mode
#define DSFILTER_MODE_MULTI         0x00000003      // Multifunction mode

// ******************************************************************
// * DSLFODESC
// ******************************************************************
typedef struct _DSLFODESC
{
    DWORD dwLFO;
    DWORD dwDelay;
    DWORD dwDelta;
    LONG lPitchModulation;
    LONG lFilterCutOffRange;
    LONG lAmplitudeModulation;
}
DSLFODESC, *LPCDSLFODESC;

// ******************************************************************
// * XBOXADPCMWAVEFORMAT
// ******************************************************************
typedef struct xbox_adpcmwaveformat_tag
{
    WAVEFORMATEX    wfx;                    // WAVEFORMATEX data
    WORD            wSamplesPerBlock;       // Number of samples per encoded block.  It must be 64.
}
XBOXADPCMWAVEFORMAT, *PXBOXADPCMWAVEFORMAT, *LPXBOXADPCMWAVEFORMAT;

typedef const XBOXADPCMWAVEFORMAT *LPCXBOXADPCMWAVEFORMAT;

// ******************************************************************
// * X_DSOUTPUTLEVELS
// ******************************************************************
struct X_DSOUTPUTLEVELS
{
    DWORD dwAnalogLeftTotalPeak;    // analog peak
    DWORD dwAnalogRightTotalPeak;
    DWORD dwAnalogLeftTotalRMS;        // analog RMS
    DWORD dwAnalogRightTotalRMS;
    DWORD dwDigitalFrontLeftPeak;    // digital peak levels
    DWORD dwDigitalFrontCenterPeak;
    DWORD dwDigitalFrontRightPeak;
    DWORD dwDigitalBackLeftPeak;
    DWORD dwDigitalBackRightPeak;
    DWORD dwDigitalLowFrequencyPeak;
    DWORD dwDigitalFrontLeftRMS;    // digital RMS levels
    DWORD dwDigitalFrontCenterRMS;
    DWORD dwDigitalFrontRightRMS;
    DWORD dwDigitalBackLeftRMS;
    DWORD dwDigitalBackRightRMS;
    DWORD dwDigitalLowFrequencyRMS;
};

// ******************************************************************
// * X_DSCAPS
// ******************************************************************
struct X_DSCAPS
{                                                       
    DWORD dwFree2DBuffers;
    DWORD dwFree3DBuffers;
    DWORD dwFreeBufferSGEs;
    DWORD dwMemoryAllocated;
};

#define X_DSSPEAKER_STEREO          0x00000000
#define X_DSSPEAKER_MONO            0x00000001
#define X_DSSPEAKER_SURROUND        0x00000002
#define X_DSSPEAKER_ENABLE_AC3      0x00010000
#define X_DSSPEAKER_ENABLE_DTS      0x00020000

struct X_DS3DBUFFER {
    DWORD dwSize;
    D3DVECTOR vPosition;
    D3DVECTOR vVelocity;
    DWORD dwInsideConeAngle;
    DWORD  dwOutsideConeAngle;
    D3DVECTOR vConeOrientation;
    LONG lConeOutsideVolume;
    FLOAT flMinDistance;
    FLOAT flMaxDistance;
    DWORD dwMode;
    FLOAT flDistanceFactor;
    FLOAT flRolloffFactor;
    FLOAT flDopplerFactor;
};

struct X_DSI3DL2LISTENER {
    LONG lRoom;
    LONG lRoomHF;
    FLOAT flRoomRolloffFactor;
    FLOAT flDecayTime;
    FLOAT flDecayHFRatio;
    LONG  lReflections;
    FLOAT flReflectionsDelay;
    LONG  lReverb;
    FLOAT flReverbDelay;
    FLOAT flDiffusion;
    FLOAT flDensity;
    FLOAT flHFReference;
};

struct X_DSI3DL2OBSTRUCTION {
    LONG            lHFLevel;
    FLOAT           flLFRatio;
};

struct X_DSI3DL2OCCLUSION {
    LONG            lHFLevel;
    FLOAT           flLFRatio;
};

struct X_DSI3DL2BUFFER {
    LONG lDirect;
    LONG lDirectHF;
    LONG lRoom;
    LONG lRoomHF;
    FLOAT flRoomRolloffFactor;
    X_DSI3DL2OBSTRUCTION Obstruction;
    X_DSI3DL2OCCLUSION Occlusion;
};

typedef struct IDirectSoundStream IDirectSoundStream;
typedef IDirectSoundStream *LPDIRECTSOUNDSTREAM;

struct X_DSVOICEPROPS {
    DWORD dwMixBinCount;
    X_DSMIXBINVOLUMEPAIR MixBinVolumePairs[8];
    LONG lPitch;
    LONG l3DDistanceVolume;
    LONG l3DConeVolume;
    LONG l3DDopplerPitch;
    LONG lI3DL2DirectVolume;
    LONG lI3DL2RoomVolume;
};
#endif
