// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->DSound.1.0.4134.cpp
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
// * DirectSoundCreate
// ******************************************************************
SOOVPA<9> DirectSoundCreate_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // DirectSoundCreate+0x0B : movzx esi, al
        { 0x0B, 0x0F }, // (Offset,Value)-Pair #1
        { 0x0C, 0xB6 }, // (Offset,Value)-Pair #2
        { 0x0D, 0xF0 }, // (Offset,Value)-Pair #3

        // DirectSoundCreate+0x12 : call [addr]
        { 0x12, 0xE8 }, // (Offset,Value)-Pair #4

        // DirectSoundCreate+0x1B : jl +0x13
        { 0x1B, 0x7C }, // (Offset,Value)-Pair #5
        { 0x1C, 0x13 }, // (Offset,Value)-Pair #6

        // DirectSoundCreate+0x27 : sbb ecx, ecx
        { 0x27, 0x1B }, // (Offset,Value)-Pair #7
        { 0x28, 0xC9 }, // (Offset,Value)-Pair #8

        // DirectSoundCreate+0x43 : leave
        { 0x43, 0xC9 }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * IDirectSound8_CreateSoundBuffer
// ******************************************************************
SOOVPA<9> IDirectSound8_CreateSoundBuffer_1_0_4134 =
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    XRefNotUsed,

    {
        // IDirectSound8_CreateSoundBuffer+0x2A : push 0x24
        { 0x2A, 0x6A }, // (Offset,Value)-Pair #1
        { 0x2B, 0x24 }, // (Offset,Value)-Pair #2

        // IDirectSound8_CreateSoundBuffer+0x3C : call [addr]
        { 0x3C, 0xE8 }, // (Offset,Value)-Pair #3

        // IDirectSound8_CreateSoundBuffer+0x4A : and esi, 0x7FF8FFF2
        { 0x4A, 0x81 }, // (Offset,Value)-Pair #4
        { 0x4B, 0xE6 }, // (Offset,Value)-Pair #5
        { 0x4C, 0xF2 }, // (Offset,Value)-Pair #6
        { 0x4D, 0xFF }, // (Offset,Value)-Pair #7
        { 0x4E, 0xF8 }, // (Offset,Value)-Pair #8
        { 0x4F, 0x7F }, // (Offset,Value)-Pair #9
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetHeadroom
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetHeadroom_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetHeadroom,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x03, 0x04 },
        { 0x08, 0x48 },
        { 0x0D, 0x24 },
        { 0x12, 0x48 },
        { 0x17, 0x8B },
        { 0x1F, 0x5E },
        { 0x21, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetHeadroom
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetHeadroom,  // Xref Is Saved
    1,

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetHeadroom },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetHeadroom
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetHeadroom_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetHeadroom },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_Lock
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_Lock_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x04, 0x75 },
        { 0x0A, 0x75 },
        { 0x10, 0x1C },
        { 0x16, 0x18 },
        { 0x1C, 0x1B },
        { 0x22, 0xC8 },
        { 0x2C, 0x5D },
        { 0x2E, 0x20 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetVolume
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoice_SetVolume_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DirectSound_CDirectSoundVoice_SetVolume,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x02, 0x24 },
        { 0x06, 0x10 },
        { 0x0A, 0x08 },
        { 0x0E, 0x89 },
        { 0x12, 0x49 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetVolume
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetVolume_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSoundBuffer_SetVolume,  // Xref Is Saved
    1,

    {
        { 0x32, XREF_DirectSound_CDirectSoundVoice_SetVolume },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetVolume
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetVolume_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DirectSound_CDirectSoundBuffer_SetVolume },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::SetI3DL2Listener
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSound_SetI3DL2Listener_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETI3DL2LISTENER,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x1E, 0x15 },
        { 0x3E, 0x88 },
        { 0x5E, 0x5A },
        { 0x7E, 0x89 },
        { 0x9E, 0x20 },
        { 0xBE, 0x00 },
        { 0xDE, 0x75 },
    }
};

// ******************************************************************
// * IDirectSound_SetI3DL2Listener
// ******************************************************************
SOOVPA<9> IDirectSound_SetI3DL2Listener_1_0_4134 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_DSSETI3DL2LISTENER },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundBuffer_SetBufferData_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETBUFFERDATA2,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x14, 0x0C },
        { 0x2A, 0xB6 },
        { 0x40, 0xB8 },
        { 0x56, 0x8B },
        { 0x6C, 0x8B },
        { 0x82, 0x7C },
        { 0x98, 0x5E },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetBufferData
// ******************************************************************
SOOVPA<9> IDirectSoundBuffer_SetBufferData_1_0_4134 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_DSSETBUFFERDATA2 },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
    }
};

// ******************************************************************
// * DirectSound::CDirectSound::GetEffectData
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSound_GetEffectData_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DirectSound_CDirectSound_GetEffectData,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0A, 0x3D },
        { 0x16, 0xF6 },
        { 0x24, 0xB8 },
        { 0x2E, 0x8B },
        { 0x3A, 0x14 },
        { 0x46, 0x85 },
        { 0x52, 0x15 },
        { 0x5E, 0x00 },
    }
};

// ******************************************************************
// * IDirectSound_GetEffectData
// ******************************************************************
SOOVPA<9> IDirectSound_GetEffectData_1_0_4134 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x1F, XREF_DirectSound_CDirectSound_GetEffectData },

        { 0x03, 0xFF },
        { 0x08, 0x08 },
        { 0x0D, 0xC8 },
        { 0x12, 0xC0 },
        { 0x17, 0xF7 },
        { 0x1C, 0xC8 },
        { 0x23, 0x5D },
        { 0x26, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
SOOVPA<7> DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_4134 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_DSSETMIXBINSC,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0E, 0xB8 },
        { 0x22, 0x83 },
        { 0x2E, 0x71 },
        { 0x3E, 0xB6 },
        { 0x4E, 0x41 },
        { 0x5E, 0x00 },
        { 0x6E, 0x00 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundVoice::SetMixBins
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundVoice_SetMixBins_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSB,  // Xref Is Saved
    1,

    {
        { 0x0D, XREF_DSSETMIXBINSC },

        { 0x02, 0x74 },
        { 0x06, 0x74 },
        { 0x0A, 0x4E },
        { 0x11, 0x8B },
        { 0x12, 0x4E },
        { 0x19, 0x5E },
        { 0x1A, 0xC2 },
    }
};

// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetMixBins
// ******************************************************************
SOOVPA<8> DirectSound_CDirectSoundBuffer_SetMixBins_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_DSSETMIXBINSA,  // Xref Is Saved
    1,

    {
        { 0x32, XREF_DSSETMIXBINSB },

        { 0x0C, 0x00 },
        { 0x12, 0x85 },
        { 0x1C, 0x15 },
        { 0x26, 0xEB },
        { 0x30, 0x10 },
        { 0x3A, 0x74 },
        { 0x47, 0x8B },
    }
};

// ******************************************************************
// * IDirectSoundBuffer_SetMixBins
// ******************************************************************
SOOVPA<8> IDirectSoundBuffer_SetMixBins_1_0_4134 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_DSSETMIXBINSA },

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
    }
};

// ******************************************************************
// * DirectSoundCreate_1_0_4134
// ******************************************************************
OOVPATable DSound_1_0_4134[] =
{
    // DirectSoundCreate
    {
        (OOVPA*)&DirectSoundCreate_1_0_4134,

        XTL::EmuDirectSoundCreate,

        #ifdef _DEBUG_TRACE
        "EmuDirectSoundCreate"
        #endif
    },
	// IDirectSound8::AddRef (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_AddRef_1_0_3936,

        XTL::EmuIDirectSound8_AddRef,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_AddRef"
        #endif
    },
	// IDirectSound8::Release (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_Release_1_0_3936,

        XTL::EmuIDirectSound8_Release,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_Release"
        #endif
    },
	// IDirectSound8::DownloadEffectsImage (* unchanged since 3936 *)
    {
        (OOVPA*)&IDirectSound8_DownloadEffectsImage_1_0_3936,

        XTL::EmuIDirectSound8_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_DownloadEffectsImage"
        #endif
    },
	// IDirectSound8::CreateSoundBuffer
    {
        (OOVPA*)&IDirectSound8_CreateSoundBuffer_1_0_4134,

        XTL::EmuIDirectSound8_CreateSoundBuffer,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateSoundBuffer"
        #endif
    },
	// DirectSound::CDirectSoundVoice::SetHeadroom (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundVoice_SetHeadroom_1_0_4134,
		0,

		#ifdef _DEBUG_TRACE
		"CDirectSoundVoice::SetHeadroom (XREF)"
		#endif
	},
	// DirectSound::CDirectSoundBuffer::SetHeadroom (XREF)
	{
		(OOVPA*)&DirectSound_CDirectSoundBuffer_SetHeadroom_1_0_4134,
		0,

		#ifdef _DEBUG_TRACE
		"CDirectSoundBuffer::SetHeadroom (XREF)"
		#endif
	},
	// IDirectSoundBuffer_SetHeadroom
	{
		(OOVPA*)&IDirectSoundBuffer_SetHeadroom_1_0_4134,
		XTL::EmuIDirectSoundBuffer8_SetHeadroom,

		#ifdef _DEBUG_TRACE
		"EmuIDirectSoundBuffer8_SetHeadRoom"
		#endif
	},
    // IDirectSoundBuffer8::Lock
    {
        (OOVPA*)&IDirectSoundBuffer_Lock_1_0_4134,

        XTL::EmuIDirectSoundBuffer8_Lock,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_Lock"
        #endif
    },
	// DirectSound::CDirectSoundVoice::SetVolume (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetVolume_1_0_4134,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetVolume (XREF)"
        #endif
    },
	// DirectSound::CDirectSoundBuffer::SetVolume (XREF)
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetVolume_1_0_4134,0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetVolume (XREF)"
        #endif
    },
	// IDirectSoundBuffer_SetVolume (XREF)
    {
        (OOVPA*)&IDirectSoundBuffer_SetVolume_1_0_4134,
		XTL::EmuIDirectSoundBuffer8_SetVolume,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetVolume"
        #endif
    },
	// CDirectSound_SetI3DL2Listener
    {
        (OOVPA*)&DirectSound_CDirectSound_SetI3DL2Listener_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::SetI3DL2Listener (XREF)"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound_SetI3DL2Listener_1_0_4134,

        XTL::EmuIDirectSound8_SetI3DL2Listener,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_SetI3DL2Listener"
        #endif
    },
	// CDirectSound_GetEffectData
    {
        (OOVPA*)&DirectSound_CDirectSound_GetEffectData_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSound::GetEffectData"
        #endif
    },
    // IDirectSound8_SetI3DL2Listener
    {
        (OOVPA*)&IDirectSound_GetEffectData_1_0_4134,

        XTL::EmuIDirectSound8_GetEffectData,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_GetEffectData"
        #endif
    },
	// CDirectSoundBuffer::SetBufferData
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetBufferData_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer_SetBufferData (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetBufferData
    {
        (OOVPA*)&IDirectSoundBuffer_SetBufferData_1_0_4134,

        XTL::EmuIDirectSoundBuffer8_SetBufferData,

        #ifdef _DEBUG_TRACE
        "IDirectSoundBuffer8_SetBufferData"
        #endif
    },
    // IDirectSound8_CreateStream
    {
        (OOVPA*)&IDirectSound8_CreateStream_1_0_3936,

        XTL::EmuIDirectSound8_CreateStream,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSound8_CreateStream"
        #endif
    },      
	// CDirectSoundVoiceSettings::SetMixBins
    {
        (OOVPA*)&DirectSound_CDirectSoundVoiceSettings_SetMixBins_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoiceSettings::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundVoice::SetMixBins
    {
        (OOVPA*)&DirectSound_CDirectSoundVoice_SetMixBins_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundVoice::SetMixBins (XREF)"
        #endif
    },
    // CDirectSoundBuffer::SetMixBins
    {
        (OOVPA*)&DirectSound_CDirectSoundBuffer_SetMixBins_1_0_4134, 0,

        #ifdef _DEBUG_TRACE
        "CDirectSoundBuffer::SetMixBins (XREF)"
        #endif
    },
    // IDirectSoundBuffer8::SetMixBins
    {
        (OOVPA*)&IDirectSoundBuffer_SetMixBins_1_0_4134,

        XTL::EmuIDirectSoundBuffer8_SetMixBins,

        #ifdef _DEBUG_TRACE
        "EmuIDirectSoundBuffer8_SetMixBins"
        #endif
    },
};

// ******************************************************************
// * DSound_1_0_4134_SIZE
// ******************************************************************
uint32 DSound_1_0_4134_SIZE = sizeof(DSound_1_0_4134);
