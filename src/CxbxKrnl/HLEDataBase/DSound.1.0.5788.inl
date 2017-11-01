// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5788.inl
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

#if 0 // Replaced with generic OOVPA 5455
// ******************************************************************
// * DirectSound::CDirectSound::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 5788, 7,

    XREF_CDirectSound_CommitDeferredSettings,
    XRefZero)

        { 0x12, 0xC0 },
        { 0x27, 0xB8 },
        { 0x3A, 0x75 },
        { 0x52, 0x8D },
        { 0x62, 0x8B },
        { 0x76, 0x89 },
        { 0x8E, 0xFF },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CMcpxBuffer::SetCurrentPosition
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetCurrentPosition, 5788, 7,

    XREF_CMcpxBuffer_SetCurrentPosition,
    XRefZero)

        { 0x1D, 0x00 },
        { 0x3C, 0x66 },
        { 0x5B, 0x00 },
        { 0x7C, 0x8D },
        { 0x99, 0xD1 },
        { 0xB8, 0x89 },
        { 0xDB, 0x5F },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetCurrentPosition
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetCurrentPosition, 5788, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_SetCurrentPosition ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;
#endif

#if 0 // Duplicate OOVPA, replacing with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetMaxDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMaxDistance, 5788, 1+7,

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
#endif
#if 0 // Duplicate OOVPA, replacing with generic OOVPA 3911
// ******************************************************************
// * IDirectSoundBuffer_SetMinDistance
// ******************************************************************
OOVPA_XREF(IDirectSoundBuffer_SetMinDistance, 5788, 1+7,

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
#endif
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
OOVPA_XREF(CDirectSoundBuffer_SetVolume, 5788, 1+7,

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

#if 0 // Replaced by generic 4039
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetMixBinVolumes
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetMixBinVolumes_8, 5788, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetMixBinVolumes ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
OOVPA_END;
#endif

#if 0 // Replaced with OOVPA 4831
// ******************************************************************
// * DirectSound::CMcpxStream::Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 5788, 7,

    XREF_CMcpxStream_Pause,
    XRefZero)

        { 0x10, 0x07 },
        { 0x23, 0x0F },
        { 0x34, 0x83 },
        { 0x46, 0xF8 },
        { 0x58, 0x03 },
        { 0x6A, 0xBB },
        { 0x7C, 0x00 },
OOVPA_END;
#endif
