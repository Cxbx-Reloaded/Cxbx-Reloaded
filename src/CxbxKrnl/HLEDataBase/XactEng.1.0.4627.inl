// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XactEng.1.0.4627.inl
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx is free software; you can redistribute it
// *  and/or modify it under the terms of the GNU General Public
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
// * XACTEngineCreate
// ******************************************************************
OOVPA_NO_XREF(XACTEngineCreate, 4627, 11)

		// XACTEngineCreate+0x0C : movzx ebx, al
		{ 0x0C, 0x0F },
		{ 0x0D, 0xB6 },
		{ 0x0E, 0xD8 },

        // XACTEngineCreate+0x30 : push edi
		{ 0x30, 0x57 },

		// XACTEngineCreate+0x31 : push 0x108
		{ 0x31, 0x68 },
		{ 0x32, 0x08 },
		{ 0x33, 0x01 },
		{ 0x34, 0x00 },
		{ 0x35, 0x00 },

		// XACTEngineCreate+0x9A : retn 0x8
		{ 0x9A, 0xC2 },
		{ 0x9B, 0x08 },
OOVPA_END;

// ******************************************************************
// * XACTEngineCreate
// ******************************************************************
OOVPA_NO_XREF(XACTEngineCreate, 4928, 11)

		// XACTEngineCreate+0x09 : movzx ebx, al
		{ 0x09, 0x0F },
		{ 0x0A, 0xB6 },
		{ 0x0B, 0xD8 },

        // XACTEngineCreate+0x2B : push edi
		{ 0x2B, 0x57 },

		// XACTEngineCreate+0x2C : push 0x120
		{ 0x2C, 0x68 },
		{ 0x2D, 0x20 },
		{ 0x2E, 0x01 },
		{ 0x2F, 0x00 },
		{ 0x30, 0x00 },

		// XACTEngineCreate+0x9E : retn 0x8
		{ 0x9E, 0xC2 },
		{ 0x9F, 0x08 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterWaveBank, 4627, 7,

    XREF_XACT_CEngine_RegisterWaveBank,
    XRefZero)

        { 0x11, 0x33 },
        { 0x24, 0xC6 },
        { 0x37, 0xE7 },
        { 0x4A, 0x75 },
        { 0x5D, 0x83 },
        { 0x70, 0x33 },
        { 0x84, 0xFF },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_RegisterWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_RegisterWaveBank, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_XACT_CEngine_RegisterWaveBank ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * XACTEngineDoWork
// ******************************************************************
OOVPA_NO_XREF(XACTEngineDoWork, 4627, 7)

        { 0x07, 0x8B },
        { 0x0D, 0x85 },
        { 0x13, 0x0C },
        { 0x1A, 0xCE },
        { 0x21, 0xFF },
        { 0x29, 0xFF },
        { 0x2F, 0x5F },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterStreamedWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterStreamedWaveBank, 4627, 8,

    XREF_XACT_CEngine_RegisterStreamedWaveBank,
    XRefZero)

        { 0x10, 0x01 },
        { 0x23, 0x3B },
        { 0x34, 0x1B },
        { 0x46, 0x0C },
        { 0x58, 0x46 },
        { 0x6A, 0x41 },
        { 0x7C, 0x68 },
        { 0x8E, 0x0C },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterStreamedWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterStreamedWaveBank, 4928, 13,

    XREF_XACT_CEngine_RegisterStreamedWaveBank,
    XRefZero)

        // XACT_CEngine_RegisterStreamedWaveBank+0x07 : push 0x7C
		{ 0x07, 0x6A }, // (Offset,Value)-Pair #1
		{ 0x08, 0x7C }, // (Offset,Value)-Pair #2
		// XACT_CEngine_RegisterStreamedWaveBank+0x30 : add esi, 0x8007000E
		{ 0x30, 0x81 }, // (Offset,Value)-Pair #3
		{ 0x31, 0xC7 }, // (Offset,Value)-Pair #4
		{ 0x32, 0x0E }, // (Offset,Value)-Pair #5
		{ 0x33, 0x00 }, // (Offset,Value)-Pair #6
		{ 0x34, 0x07 }, // (Offset,Value)-Pair #7
		{ 0x35, 0x80 }, // (Offset,Value)-Pair #8
		// XACT_CEngine_RegisterStreamedWaveBank+0x50 : add ecx, 0x44
		{ 0x50, 0x83 }, // (Offset,Value)-Pair #9
		{ 0x51, 0xC1 }, // (Offset,Value)-Pair #10
		{ 0x52, 0x44 }, // (Offset,Value)-Pair #11
		// XACT_CEngine_RegisterStreamedWaveBank+0x71 : retn 0x0C
		{ 0x71, 0xC2 }, // (Offset,Value)-Pair #12
		{ 0x72, 0x0C }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_RegisterStreamedWaveBank, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_XACT_CEngine_RegisterStreamedWaveBank ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_RegisterStreamedWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_RegisterStreamedWaveBank, 4928, 10,

    XRefNoSaveIndex,
    XRefOne)

		// IXACTEngine_RegisterStreamedWaveBank+0x22 : call XACT::CEngine::RegisterStreamedWaveBank
        XREF_ENTRY( 0x23, XREF_XACT_CEngine_RegisterStreamedWaveBank ),

		// IXACTEngine_RegisterStreamedWaveBank+0x0B : movzx esi, al
		{ 0x0B, 0x0F },
		{ 0x0C, 0xB6 },
		{ 0x0D, 0xF0 },
		// IXACTEngine_RegisterStreamedWaveBank+0x0E : mov eax, [esp+0x0C+4]
		{ 0x0E, 0x8B },
		{ 0x0F, 0x44 },
		{ 0x10, 0x24 },
		{ 0x11, 0x10 },
		// IXACTEngine_RegisterStreamedWaveBank+0x3C : retn 0x0C
		{ 0x3C, 0xC2 },
		{ 0x3D, 0x0C },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::CreateSoundBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_CreateSoundBank, 4627, 8,

    XREF_XACT_CEngine_CreateSoundBank,
    XRefZero)

        { 0x0E, 0xA0 },
        { 0x1E, 0x8B },
        { 0x2E, 0xE6 },
        { 0x3E, 0x18 },
        { 0x4E, 0x7C },
        { 0x5E, 0xC7 },
        { 0x72, 0xFF },
        { 0x7E, 0x10 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::CreateSoundBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_CreateSoundBank, 4928, 13,

    XREF_XACT_CEngine_CreateSoundBank,
    XRefZero)
		// XACT::CEngine::CreateSoundBank+0x02 : push 0x34
		{ 0x02, 0x6A }, // (Offset,Value)-Pair #1
		{ 0x03, 0x34 }, // (Offset,Value)-Pair #2
		// XACT::CEngine::CreateSoundBank+0x2B : add edi, 0x8007000E
		{ 0x2B, 0x81 }, // (Offset,Value)-Pair #3
		{ 0x2C, 0xC7 }, // (Offset,Value)-Pair #4
		{ 0x2D, 0x0E }, // (Offset,Value)-Pair #5
		{ 0x2E, 0x00 }, // (Offset,Value)-Pair #6
		{ 0x2F, 0x07 }, // (Offset,Value)-Pair #7
		{ 0x30, 0x80 }, // (Offset,Value)-Pair #8
		// XACT::CEngine::CreateSoundBank+0x4F : add ecx, 0x4C
		{ 0x4F, 0x83 }, // (Offset,Value)-Pair #9
		{ 0x50, 0xC1 }, // (Offset,Value)-Pair #10
		{ 0x51, 0x4C }, // (Offset,Value)-Pair #11
		// XACT::CEngine::CreateSoundBank+0x73 : retn 0x10
		{ 0x73, 0xC2 }, // (Offset,Value)-Pair #12
		{ 0x74, 0x10 }, // (Offset,Value)-Pair #13
OOVPA_END;

// ******************************************************************
// * IXACTEngine_CreateSoundBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_CreateSoundBank, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1D, XREF_XACT_CEngine_CreateSoundBank ),

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_CreateSoundBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_CreateSoundBank, 4928, 10,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x27, XREF_XACT_CEngine_CreateSoundBank ),

		// IXACTEngine_CreateSoundBank+0x07 : push [ebp+8+0x10]
		{ 0x07, 0xFF },
		{ 0x08, 0x74 },
		{ 0x09, 0x24 },
		{ 0x0A, 0x18 },
        // IXACTEngine_CreateSoundBank+0x0B : movzx esi, al
		{ 0x0B, 0x0F },
		{ 0x0C, 0xB6 },
		{ 0x0D, 0xF0 },
		// IXACTEngine_CreateSoundBank+0x40 : retn 0x10
		{ 0x40, 0xC2 },
		{ 0x41, 0x10 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::DownloadEffectsImage
// ******************************************************************
OOVPA_XREF(XACT_CEngine_DownloadEffectsImage, 4627, 7,

    XREF_XACT_CEngine_DownloadEffectsImage,
    XRefZero)
        { 0x0B, 0x8B },
        { 0x12, 0xFF },
        { 0x1C, 0x46 },
        { 0x27, 0x85 },
        { 0x30, 0x89 },
        { 0x3A, 0x68 },
        { 0x45, 0x8B },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_DownloadEffectsImage
// ******************************************************************
OOVPA_XREF(IXACTEngine_DownloadEffectsImage, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x1F, XREF_XACT_CEngine_DownloadEffectsImage ),

        { 0x03, 0xFF },
        { 0x08, 0x08 },
        { 0x0D, 0xC8 },
        { 0x12, 0xC0 },
        { 0x17, 0xF7 },
        { 0x1C, 0xC8 },
        { 0x23, 0x5D },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::CreateSoundSource
// ******************************************************************
OOVPA_XREF(XACT_CEngine_CreateSoundSource, 4627, 8,

    XREF_XACT_CEngine_CreateSoundSource,
    XRefZero)
        { 0x0A, 0x0F },
        { 0x14, 0x8B },
        { 0x1F, 0x50 },
        { 0x2A, 0x85 },
        { 0x35, 0x08 },
        { 0x40, 0x08 },
        { 0x4B, 0x15 },
        { 0x56, 0x0C },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_CreateSoundSource
// ******************************************************************
OOVPA_XREF(IXACTEngine_CreateSoundSource, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_XACT_CEngine_CreateSoundSource ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * XACT::CSoundBank::GetSoundCueIndexFromFriendlyName
// ******************************************************************
OOVPA_XREF(XACT_CSoundBank_GetSoundCueIndexFromFriendlyName, 4627, 7,

    XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName,
    XRefZero)
        { 0x12, 0x8B },
        { 0x22, 0x46 },
        { 0x34, 0x68 },
        { 0x46, 0x0C },
        { 0x58, 0x39 },
        { 0x6A, 0x45 },
        { 0x7E, 0xFF },
OOVPA_END;

// ******************************************************************
// * IXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
OOVPA_XREF(IXACTSoundBank_GetSoundCueIndexFromFriendlyName, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * IXACTSoundBank_Play
// ******************************************************************
OOVPA_NO_XREF(IXACTSoundBank_Play, 4627, 8)

        { 0x03, 0xFF },
        { 0x08, 0x08 },
        { 0x0D, 0xC8 },
        { 0x12, 0xC0 },
        { 0x17, 0xF7 },
        { 0x1C, 0xC8 },
        { 0x23, 0x5D },
        { 0x26, 0x00 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterNotification
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterNotification, 4627, 7,

    XREF_XACT_CEngine_RegisterNotification,
    XRefZero)
        { 0x07, 0x8B },
        { 0x0C, 0x01 },
        { 0x13, 0xF0 },
        { 0x1A, 0xF6 },
        { 0x24, 0xFF },
        { 0x2A, 0x8B },
        { 0x2F, 0x08 },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_RegisterNotification
// ******************************************************************
OOVPA_XREF(IXACTEngine_RegisterNotification, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_XACT_CEngine_RegisterNotification ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::GetNotification
// ******************************************************************
OOVPA_XREF(XACT_CEngine_GetNotification, 4627, 8,

    XREF_XACT_CEngine_GetNotification,
    XRefZero)

        { 0x11, 0x8D },
        { 0x24, 0x5A },
        { 0x37, 0xC0 },
        { 0x4A, 0x8B },
        { 0x5D, 0x04 },
        { 0x74, 0x8B },
        { 0x83, 0x40 },
        { 0x96, 0x0C },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_GetNotification
// ******************************************************************
OOVPA_XREF(IXACTEngine_GetNotification, 4627, 9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x19, XREF_XACT_CEngine_GetNotification ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0xFF },
        { 0x0E, 0x83 },
        { 0x12, 0xD9 },
        { 0x16, 0xC8 },
        { 0x1D, 0xC2 },
        { 0x1E, 0x0C },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::UnRegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_UnRegisterWaveBank, 4627, 8,

    XREF_XACT_CEngine_UnRegisterWaveBank,
    XRefZero)

        { 0x07, 0x8B },
        { 0x10, 0x58 },
        { 0x19, 0x8B },
        { 0x22, 0x89 },
        { 0x2C, 0x8B },
        { 0x34, 0x5F },
        { 0x3D, 0xFF },
        { 0x46, 0x08 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::UnRegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_UnRegisterWaveBank, 4928, 8,

    XREF_XACT_CEngine_UnRegisterWaveBank,
    XRefZero)

    	// XACT_CEngine_UnRegisterWaveBank+0x06 : lea eax, [ecx+0x58]
    	{ 0x06, 0x8D },
    	{ 0x07, 0x41 },
    	{ 0x08, 0x58 },
    	// XACT_CEngine_UnRegisterWaveBank+0x28 : lea edi, [ebx+0x4C]
    	{ 0x28, 0x8D },
    	{ 0x29, 0x7B },
    	{ 0x2A, 0x4C },
    	// XACT_CEngine_UnRegisterWaveBank+0xBF : retn 0x8
    	{ 0xBF, 0xC2 },
    	{ 0xC0, 0x08 },
OOVPA_END;


// ******************************************************************
// * IXACTEngine_UnRegisterWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_UnRegisterWaveBank, 4627, 8,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_XACT_CEngine_UnRegisterWaveBank ),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_UnRegisterWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_UnRegisterWaveBank, 4928, 8,

    XRefNoSaveIndex,
    XRefOne)

	// IXACTEngine_UnRegisterWaveBank+0x1E : call XACT::CEngine::UnRegisterWaveBank
        XREF_ENTRY( 0x1F, XREF_XACT_CEngine_UnRegisterWaveBank ),

	{ 0x07, 0xFF },
        { 0x0E, 0x8B },
        { 0x16, 0xF8 },
        { 0x1E, 0xE8 },
        { 0x26, 0xF8 },
        { 0x2E, 0xFF },
        { 0x36, 0x5F },
OOVPA_END;

// ******************************************************************
// * XactEng_4627
// ******************************************************************
OOVPATable XactEng_4627[] = {

	REGISTER_OOVPA(XACTEngineCreate, 4627, PATCH),
	REGISTER_OOVPA(XACTEngineCreate, 4928, PATCH),
	REGISTER_OOVPA(XACTEngineDoWork, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterWaveBank, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterWaveBank, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterStreamedWaveBank, 4627, XREF),
	REGISTER_OOVPA(XACT_CEngine_RegisterStreamedWaveBank, 4928, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterStreamedWaveBank, 4627, PATCH),
	REGISTER_OOVPA(IXACTEngine_RegisterStreamedWaveBank, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_CreateSoundBank, 4627, XREF),
	REGISTER_OOVPA(XACT_CEngine_CreateSoundBank, 4928, XREF),
	REGISTER_OOVPA(IXACTEngine_CreateSoundBank, 4627, PATCH),
	REGISTER_OOVPA(IXACTEngine_CreateSoundBank, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_DownloadEffectsImage, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_DownloadEffectsImage, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_CreateSoundSource, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_CreateSoundSource, 4627, PATCH),
	REGISTER_OOVPA(XACT_CSoundBank_GetSoundCueIndexFromFriendlyName, 4627, XREF),
	REGISTER_OOVPA(IXACTSoundBank_GetSoundCueIndexFromFriendlyName, 4627, PATCH),
	REGISTER_OOVPA(IXACTSoundBank_Play, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterNotification, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterNotification, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_GetNotification, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_GetNotification, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_UnRegisterWaveBank, 4627, XREF),
	REGISTER_OOVPA(XACT_CEngine_UnRegisterWaveBank, 4928, XREF),
	REGISTER_OOVPA(IXACTEngine_UnRegisterWaveBank, 4627, PATCH),
	REGISTER_OOVPA(IXACTEngine_UnRegisterWaveBank, 4928, PATCH),
};

// ******************************************************************
// * XACTENG_4627_SIZE
// ******************************************************************
uint32 XactEng_4627_SIZE = sizeof(XactEng_4627);
