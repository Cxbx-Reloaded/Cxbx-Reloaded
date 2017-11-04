// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5455.inl
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
// *
// *  All rights reserved
// *
// ******************************************************************


// ******************************************************************
// * CDirectSound::CommitDeferredSettings
// ******************************************************************
// Generic OOVPA as of 5455 and newer
OOVPA_XREF(CDirectSound_CommitDeferredSettings, 5455, 18,

    XREF_CDirectSound_CommitDeferredSettings,
    XRefZero)

        // CDirectSound_CommitDeferredSettings+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSound_CommitDeferredSettings+0x10 : movzx eax, al
        { 0x10, 0x0F },
        { 0x11, 0xB6 },
        { 0x12, 0xC0 },

        // CDirectSound_CommitDeferredSettings+0x27 : mov eax, 0x80004005
        { 0x27, 0xB8 },
        { 0x28, 0x05 },
        { 0x29, 0x40 },
        { 0x2B, 0x80 },

        // CDirectSound_CommitDeferredSettings+0x3C : mov eax, dword ptr [esi+8]
        { 0x3C, 0x8B }, 
        { 0x3D, 0x46 },
        { 0x3E, 0x08 },

        // CDirectSound_CommitDeferredSettings+0x52 : lea ebx, [esi+10h]
        { 0x52, 0x8D },
        { 0x53, 0x5E },
        { 0x54, 0x10 },

        // CDirectSound_CommitDeferredSettings+0x68 : lea ebx, [esi+10h]
        { 0x68, 0x8B },
        { 0x69, 0x46 },
        { 0x6A, 0x08 },

        // CDirectSound_CommitDeferredSettings+0x96 : leave
        { 0x96, 0xC9 },
OOVPA_END;

#if 0 // Moved to 4134
// ******************************************************************
// * DirectSound::CDirectSoundBuffer::SetBufferData
// ******************************************************************
OOVPA_XREF(CDirectSoundBuffer_SetBufferData, 5455, 19,

    XREF_CDirectSoundBuffer_SetBufferData,
    XRefZero)

        // CDirectSoundBuffer_SetBufferData+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSoundBuffer_SetBufferData+0x0F : cmp dword ptr [ebp+10h],edi
        { 0x0F, 0x39 },
        { 0x10, 0x7D },
        { 0x11, 0x10 },

        // CDirectSoundBuffer_SetBufferData+0x12 : mov dword ptr [ebp+0Ch],edi
        { 0x12, 0x89 },
        { 0x13, 0x7D },
        { 0x14, 0x10 },

        // CDirectSoundBuffer_SetBufferData+0x4B : mov eax,dword ptr [esi+1Ch]
        { 0x4B, 0x8B },
        { 0x4C, 0x46 },
        { 0x4D, 0x1C },

        // CDirectSoundBuffer_SetBufferData+0x64 : mov ecx,dword ptr [esi+20h]
        { 0x64, 0x8B },
        { 0x65, 0x4E },
        { 0x66, 0x20 },

        // CDirectSoundBuffer_SetBufferData+0x8A : mov ecx,dword ptr [esi+20h]
        { 0x8A, 0x8B },
        { 0x8B, 0x4E },
        { 0x8C, 0x20 },

        // CDirectSoundBuffer_SetBufferData+0xAB : retn 0x0C
        { 0xAB, 0xC2 },
        { 0xAC, 0x0C },
        { 0xAD, 0x00 },
OOVPA_END;
#endif

// ******************************************************************
// * DirectSound::CDirectSound::EnableHeadphones
// ******************************************************************
// Generic OOVPA as of 5455 and newer
OOVPA_XREF(CDirectSound_EnableHeadphones, 5455, 17,

    XREF_CDirectSound_EnableHeadphones,
    XRefZero)

        // CDirectSound_EnableHeadphones+0x00 : push ebp
        { 0x00, 0x55 },

        // CDirectSound_EnableHeadphones+0x16 : movzx eax, al
        { 0x16, 0x0F },
        { 0x17, 0xB6 },
        { 0x18, 0xC0 },

        // CDirectSound_EnableHeadphones+0x3E : shr e__, 0x1F
        { 0x3E, 0xC1 },
        //{ 0x3F, 0xE8 }, // 5344 0xEA vs 5455 0xE8
        { 0x40, 0x1F },

        // CDirectSound_EnableHeadphones+0x53 : mov edi, 0x80000000
        { 0x53, 0xBF },
        { 0x54, 0x00 },
        { 0x55, 0x00 },
        { 0x56, 0x00 },
        { 0x57, 0x80 },

        // CDirectSound_EnableHeadphones+0xC4 : cmp dword ptr [ebp+0x0C],0
        { 0xC4, 0x83 },
        { 0xC5, 0x7D },
        { 0xC6, 0x0C },
        { 0xC7, 0x00 },

        // CDirectSound_EnableHeadphones+0x117 : leave
        //{ 0x117, 0xC9 },
        { 0x118, 0xC2 },
        { 0x119, 0x08 },
OOVPA_END;

// ******************************************************************
// * DSound_CRefCount_Release
// ******************************************************************
// Generic OOVPA as of 5455 and newer
OOVPA_XREF(DSound_CRefCount_Release, 5455, 10,

    XREF_DSound_CRefCount_Release,
    XRefZero)

        { 0x00, 0x8B },

        // DSound_CRefCount_Release+0x0B : dec eax
        { 0x0B, 0x48 },

        // DSound_CRefCount_Release+0x0E : mov dword ptr [ecx+4], eax
        { 0x0C, 0x89 },
        { 0x0D, 0x41 },
        { 0x0E, 0x04 },

        // DSound_CRefCount_Release+0x1D : mov, dword ptr [ecx+4]
        { 0x1B, 0x8B },
        { 0x1C, 0x41 },
        { 0x1D, 0x04 },

        // DSound_CRefCount_Release+0x20 : retn 0x04
        { 0x1E, 0xC2 },
        { 0x1F, 0x04 },
OOVPA_END;

#if 0 // Replaced with generic OOVPA 5344
// ******************************************************************
// * CMcpxStream_Flush
// ******************************************************************
OOVPA_XREF(CMcpxStream_Flush, 5455, 11,
    XREF_CMcpxStream_Flush,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },

        { 0x12, 0x33 },

        { 0x38, 0x83 },
        { 0x39, 0xFF },
        { 0x3A, 0x06 },

        { 0x4B, 0xE8 },

        { 0x62, 0xE8 },

        { 0xD1, 0xC9 },
        { 0xD2, 0xC2 },
        { 0xD3, 0x04 },
OOVPA_END;
#endif

// ******************************************************************
// * CMcpxStream_Discontinuity
// ******************************************************************
OOVPA_XREF(CMcpxStream_Discontinuity, 5455, 9,
    XREF_CMcpxStream_Discontinuity,
    XRefZero)

        // CMcpxStream_Discontinuity+0x00 : push esi; push edi
        { 0x00, 0x56 },
        { 0x01, 0x57 },

        // CMcpxStream_Discontinuity+0x12 : mov dx,0x800
        { 0x12, 0x66 },
        { 0x13, 0xBA },
        { 0x14, 0x00 },
        { 0x15, 0x08 },

        // CMcpxStream_Discontinuity+0x23 : call Stop@CMcpxStream@DirectSound@@QAEJK@Z
        { 0x21, 0xE8 },

        // CMcpxStream_Discontinuity+0x2D : pop esi; ret
        { 0x2B, 0x5E },
        { 0x2C, 0xC3 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CDirectSound::SetI3DL2Listener
// ******************************************************************
OOVPA_XREF(CDirectSound_SetI3DL2Listener, 5455, 20,

    XREF_CDirectSound_SetI3DL2Listener,
    XRefZero)

        // CDirectSound_SetI3DL2Listener+0x00 : push esi
        { 0x00, 0x56 },

        // CDirectSound_SetI3DL2Listener+0x3A : mov edi,88780032h (unchanged since 3911)
        { 0x3A, 0xBF },
        //{ 0x3B, 0x32 },
        //{ 0x3C, 0x00 },
        //{ 0x3D, 0x78 },
        { 0x3E, 0x88 },

        // CDirectSound_SetI3DL2Listener+0x54 : mov ebx,dword ptr [ecx+4]
        { 0x54, 0x8B },
        { 0x55, 0x59 },
        { 0x56, 0x04 },

        // CDirectSound_SetI3DL2Listener+0x57 : mov edx,dword ptr [eax+8]
        { 0x57, 0x8B },
        { 0x58, 0x50 },
        { 0x59, 0x08 },

        // CDirectSound_SetI3DL2Listener+0x9C : mov ebx,dword ptr [ecx+1Ch]
        { 0x9C, 0x8B },
        { 0x9D, 0x59 },
        { 0x9E, 0x1C },

        // CDirectSound_SetI3DL2Listener+0xDB : or word ptr [ecx+__h],0FFFh
        { 0xDB, 0x66 },
        { 0xDC, 0x81 },
        { 0xDD, 0x89 },
        //{ 0xDE, 0x80 }, vs 5344 0x84
        { 0xDF, 0x00 },
        //{ 0xE0, 0x00 },
        //{ 0xE1, 0x00 },
        { 0xE2, 0xFF },
        { 0xE3, 0x0F },

        // CDirectSound_SetI3DL2Listener+0x105 : retn 0x0C
        { 0x105, 0xC2 },
        { 0x106, 0x0C },
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoiceSettings::SetMixBins
// ******************************************************************
OOVPA_XREF(CDirectSoundVoiceSettings_SetMixBins, 5455, 7,

    XREF_CDirectSoundVoiceSettings_SetMixBins,
    XRefZero)

        { 0x1B, 0xEB },
        { 0x32, 0x8B },
        { 0x4C, 0x7C },
        { 0x66, 0x00 },
        { 0x84, 0xF6 },
        { 0x9A, 0xB4 },
        { 0xB4, 0x00 },
OOVPA_END;
// ******************************************************************
// * CDirectSoundVoice::CommitDeferredSettings
// ******************************************************************
OOVPA_XREF(CDirectSoundVoice_CommitDeferredSettings, 5455, 6,

    XREF_CDirectSoundVoice_CommitDeferredSettings,
    XRefZero)

        { 0x01, 0x44 },
        { 0x04, 0x8B },
        { 0x07, 0xE8 },
        { 0x0C, 0x33 },
        { 0x0D, 0xC0 },
        { 0x10, 0x00 },
OOVPA_END;

// ******************************************************************
// * CMcpxBuffer_SetBufferData
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_SetBufferData, 5455, 9,

	XREF_CMcpxBuffer_SetBufferData,
    XRefZero)

        // CMcpxBuffer_SetBufferData+0x1E : cmp eax, ebx
        { 0x1E, 0x3B },
        { 0x1F, 0xC3 },

        // CMcpxBuffer_SetBufferData+0x20 : jz +0x0C
        { 0x20, 0x74 },
        { 0x21, 0x0C },

        // CMcpxBuffer_SetBufferData+0x45 : mov [esi+0xBC], ecx
        { 0x45, 0x89 },
        { 0x46, 0x8E },
        { 0x47, 0xBC },

        // CMcpxBuffer_SetBufferData+0x89 : retn 0x08
        { 0x89, 0xC2 },
        { 0x8A, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMemoryManager::PoolAlloc
// ******************************************************************
OOVPA_NO_XREF(DSound_CMemoryManager_PoolAlloc, 5455, 11)

	{ 0x00, 0x56 },
	{ 0x24, 0x83 },
	{ 0x25, 0x7C },
	{ 0x26, 0x24 },
	{ 0x27, 0x10 },
	{ 0x28, 0x00 },
	{ 0x37, 0xF3 },
	{ 0x38, 0xAB },
	{ 0x44, 0xC2 },
	{ 0x45, 0x0C },
	{ 0x46, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSound_GetSpeakerConfig
// ******************************************************************
// Generic OOVPA as of 5455 and newer
OOVPA_XREF(CDirectSound_GetSpeakerConfig, 5455, 14,

    XREF_CDirectSound_GetSpeakerConfig,
    XRefZero)

        { 0x00, 0xE8 },

        { 0x0B, 0x00 },
        { 0x14, 0x0B },
        { 0x20, 0xB8 },
        { 0x30, 0x08 },

        { 0x31, 0x81 },
        //{ 0x32, 0xE1 }, (This value has changed, commented out for generic support.)
        { 0x33, 0xFF },
        { 0x34, 0xFF },
        { 0x35, 0xFF },
        { 0x36, 0x7F },

        { 0x3C, 0x0B },

        { 0x48, 0x33 },

        { 0x4A, 0xC2 },
        { 0x4B, 0x08 },
OOVPA_END;
