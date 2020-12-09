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

#include "xbox_types.h"

namespace xbox {

// TODO: Also need to use fixed size to able support cross-platform without extra work.
//       Such as uint32_t, uint16_t, etc.

typedef struct _XWAVEFORMATEX
{
    uint16_t    wFormatTag;
    uint16_t    nChannels;
    uint32_t    nSamplesPerSec;
    uint32_t    nAvgBytesPerSec;
    uint16_t    nBlockAlign;
    uint16_t    wBitsPerSample;
    uint16_t    cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *LPWAVEFORMATEX;

typedef const WAVEFORMATEX *LPCWAVEFORMATEX;




#define WAVE_FORMAT_ADPCM 0x0002
#define WAVE_FORMAT_XBOX_ADPCM 0x0069
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

// Xbox SGE Memory layout
#define                             XTL_DS_SGE_COUNT_MAX 2047
#define                             XTL_DS_SGE_PAGE_MAX (4 * ONE_KB)
#define                             XTL_DS_SGE_SIZE_MAX (XTL_DS_SGE_COUNT_MAX * XTL_DS_SGE_PAGE_MAX)

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
    dword_xt       dwMixBin;
    long_xt        lVolume;
} X_DSMIXBINVOLUMEPAIR, *X_LPDSMIXBINVOLUMEPAIR;

// ******************************************************************
// * X_DSMIXBINS
// ******************************************************************
// Revision 2 (4039+)
typedef struct _XDSMIXBINS {
    dword_xt                       dwCount;
    X_LPDSMIXBINVOLUMEPAIR     lpMixBinVolumePairs;
} X_DSMIXBINS, *X_LPDSMIXBINS;

// A convenience union combining Revision 1 (3911-4034) and Revision 2 (4039+) mixbin function parameters
union X_DSMIXBINBUNION {
    dword_xt      dwMixBinMask;
    X_LPDSMIXBINS pMixBins;
};

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

// Generic frequency range
#define XTL_DSXFREQUENCY_ORIGINAL        0x00000000
//#define XTL_DSGFREQUENCY_MIN             0x00000???
//#define XTL_DSGFREQUENCY_MAX             0x000?????

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
    dword_xt           dwSize;
    dword_xt           dwFlags;
    dword_xt           dwBufferBytes;
    LPWAVEFORMATEX  lpwfxFormat;
    X_DSMIXBINBUNION   mixBinsOutput;
    dword_xt           dwInputMixBin;
};

// ******************************************************************
// * X_DSENVELOPEDESC
// ******************************************************************
struct X_DSENVOLOPEDESC
{
    dword_xt           dwEnvelopGenerator;
    dword_xt           dwMode;
    dword_xt           dwDelay;
    dword_xt           dwAttack;
    dword_xt           dwHold;
    dword_xt           dwDecay;
    dword_xt           dwRelease;
    dword_xt           dwSustain;
    long_xt            lPitchScale;
    long_xt            lFilterCutOff;
};

#define XTL_DSSCAPS_CTRL3D               0x00000010
#define XTL_DSSCAPS_CTRLFREQUENCY        0x00000020
#define XTL_DSSCAPS_CTRLVOLUME           0x00000080
#define XTL_DSSCAPS_MUTE3DATMAXDISTANCE  0x00020000
#define XTL_DSSCAPS_LOCDEFER             0x00040000
#define XTL_DSSCAPS_NOMERGE              0x20000000
#define XTL_DSSCAPS_ACCURATENOTIFY       0x40000000

typedef xbox::void_xt(XCALLBACK *LPFNXMOCALLBACK)(LPVOID pStreamContext, LPVOID pPacketContext, dword_xt dwStatus);

// ******************************************************************
// * X_DSSTREAMDESC
// ******************************************************************
struct X_DSSTREAMDESC
{
    dword_xt                       dwFlags;
    dword_xt                       dwMaxAttachedPackets;
    LPWAVEFORMATEX              lpwfxFormat;
    LPFNXMOCALLBACK             lpfnCallback;
    LPVOID                      lpvContext;
    X_DSMIXBINBUNION               mixBinsOutput;
};

// ******************************************************************
// * REFERENCE_TIME
// ******************************************************************
typedef longlong_xt REFERENCE_TIME, *PREFERENCE_TIME, *LPREFERENCE_TIME;

// ******************************************************************
// * XMEDIAPACKET
// ******************************************************************
typedef struct _XMEDIAPACKET
{
    LPVOID pvBuffer;
    dword_xt dwMaxSize;
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

// ******************************************************************
// * XMEDIAINFO
// ******************************************************************
typedef struct _XMEDIAINFO
{
    dword_xt  dwFlags;
    dword_xt  dwInputSize;
    dword_xt  dwOutputSize;
    dword_xt  dwMaxLookahead;
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
    dword_xt dwMode;
    dword_xt dwQCoefficient;
    dword_xt adwCoefficients[4];
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
    dword_xt dwLFO;
    dword_xt dwDelay;
    dword_xt dwDelta;
    long_xt lPitchModulation;
    long_xt lFilterCutOffRange;
    long_xt lAmplitudeModulation;
}
DSLFODESC, *LPCDSLFODESC;

// ******************************************************************
// * XBOXADPCMWAVEFORMAT
// ******************************************************************
typedef struct xbox_adpcmwaveformat_tag
{
    WAVEFORMATEX    wfx;                    // WAVEFORMATEX data
    word_xt            wSamplesPerBlock;       // Number of samples per encoded block.  It must be 64.
}
XBOXADPCMWAVEFORMAT, *PXBOXADPCMWAVEFORMAT, *LPXBOXADPCMWAVEFORMAT;

typedef const XBOXADPCMWAVEFORMAT *LPCXBOXADPCMWAVEFORMAT;

// ******************************************************************
// * X_DSOUTPUTLEVELS
// ******************************************************************
struct X_DSOUTPUTLEVELS
{
    dword_xt dwAnalogLeftTotalPeak;    // analog peak
    dword_xt dwAnalogRightTotalPeak;
    dword_xt dwAnalogLeftTotalRMS;        // analog RMS
    dword_xt dwAnalogRightTotalRMS;
    dword_xt dwDigitalFrontLeftPeak;    // digital peak levels
    dword_xt dwDigitalFrontCenterPeak;
    dword_xt dwDigitalFrontRightPeak;
    dword_xt dwDigitalBackLeftPeak;
    dword_xt dwDigitalBackRightPeak;
    dword_xt dwDigitalLowFrequencyPeak;
    dword_xt dwDigitalFrontLeftRMS;    // digital RMS levels
    dword_xt dwDigitalFrontCenterRMS;
    dword_xt dwDigitalFrontRightRMS;
    dword_xt dwDigitalBackLeftRMS;
    dword_xt dwDigitalBackRightRMS;
    dword_xt dwDigitalLowFrequencyRMS;
};

// ******************************************************************
// * X_DSCAPS
// ******************************************************************
struct X_DSCAPS
{                                                       
    dword_xt dwFree2DBuffers;
    dword_xt dwFree3DBuffers;
    dword_xt dwFreeBufferSGEs;
    dword_xt dwMemoryAllocated;
};

#define X_DSSPEAKER_STEREO          0x00000000
#define X_DSSPEAKER_MONO            0x00000001
#define X_DSSPEAKER_SURROUND        0x00000002
#define X_DSSPEAKER_ENABLE_AC3      0x00010000
#define X_DSSPEAKER_ENABLE_DTS      0x00020000

struct X_DS3DBUFFER {
    dword_xt dwSize;
    D3DVECTOR vPosition;
    D3DVECTOR vVelocity;
    dword_xt dwInsideConeAngle;
    dword_xt  dwOutsideConeAngle;
    D3DVECTOR vConeOrientation;
    long_xt lConeOutsideVolume;
    float_xt flMinDistance;
    float_xt flMaxDistance;
    dword_xt dwMode;
    float_xt flDistanceFactor;
    float_xt flRolloffFactor;
    float_xt flDopplerFactor;
};

struct X_DSI3DL2LISTENER {
    long_xt lRoom;
    long_xt lRoomHF;
    float_xt flRoomRolloffFactor;
    float_xt flDecayTime;
    float_xt flDecayHFRatio;
    long_xt  lReflections;
    float_xt flReflectionsDelay;
    long_xt  lReverb;
    float_xt flReverbDelay;
    float_xt flDiffusion;
    float_xt flDensity;
    float_xt flHFReference;
};

struct X_DSI3DL2OBSTRUCTION {
    long_xt            lHFLevel;
    float_xt           flLFRatio;
};

struct X_DSI3DL2OCCLUSION {
    long_xt            lHFLevel;
    float_xt           flLFRatio;
};

struct X_DSI3DL2BUFFER {
    long_xt lDirect;
    long_xt lDirectHF;
    long_xt lRoom;
    long_xt lRoomHF;
    float_xt flRoomRolloffFactor;
    X_DSI3DL2OBSTRUCTION Obstruction;
    X_DSI3DL2OCCLUSION Occlusion;
};

typedef struct IDirectSoundStream IDirectSoundStream;
typedef IDirectSoundStream *LPDIRECTSOUNDSTREAM;

struct X_DSVOICEPROPS {
    dword_xt dwMixBinCount;
    X_DSMIXBINVOLUMEPAIR MixBinVolumePairs[8];
    long_xt lPitch;
    long_xt l3DDistanceVolume;
    long_xt l3DConeVolume;
    long_xt l3DDopplerPitch;
    long_xt lI3DL2DirectVolume;
    long_xt lI3DL2RoomVolume;
};

} // end of namespace xbox

#endif
