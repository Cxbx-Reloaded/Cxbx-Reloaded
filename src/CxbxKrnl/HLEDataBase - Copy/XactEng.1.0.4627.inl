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
// * XACTEngineCreate
// ******************************************************************
SOOVPA<11> XACTEngineCreate_1_0_4627 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
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
	}
};

// ******************************************************************
// * XACTEngineCreate
// ******************************************************************
SOOVPA<11> XACTEngineCreate_1_0_4928 =
{
    0,  // Large == 0
    11, // Count == 11

    XRefNotSaved,
    XRefNotUsed,

    {
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
	}
};

// ******************************************************************
// * XACT::CEngine::RegisterWaveBank
// ******************************************************************
SOOVPA<7> XACT_CEngine_RegisterWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_XACT_CEngine_RegisterWaveBank,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x11, 0x33 },
        { 0x24, 0xC6 },
        { 0x37, 0xE7 },
        { 0x4A, 0x75 },
        { 0x5D, 0x83 },
        { 0x70, 0x33 },
        { 0x84, 0xFF },
    }
};

// ******************************************************************
// * IXACTEngine_RegisterWaveBank
// ******************************************************************
SOOVPA<8> IXACTEngine_RegisterWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_XACT_CEngine_RegisterWaveBank },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * XACTEngineDoWork
// ******************************************************************
SOOVPA<7> XACTEngineDoWork_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XRefNotSaved,
    XRefNotUsed,

    {
        { 0x07, 0x8B },
        { 0x0D, 0x85 },
        { 0x13, 0x0C },
        { 0x1A, 0xCE },
        { 0x21, 0xFF },
        { 0x29, 0xFF },
        { 0x2F, 0x5F },
    }
};

// ******************************************************************
// * XACT::CEngine::RegisterStreamedWaveBank
// ******************************************************************
SOOVPA<8> XACT_CEngine_RegisterStreamedWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_RegisterStreamedWaveBank,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x10, 0x01 },
        { 0x23, 0x3B },
        { 0x34, 0x1B },
        { 0x46, 0x0C },
        { 0x58, 0x46 },
        { 0x6A, 0x41 },
        { 0x7C, 0x68 },
        { 0x8E, 0x0C },
    }
};

// ******************************************************************
// * XACT::CEngine::RegisterStreamedWaveBank
// ******************************************************************
SOOVPA<13> XACT_CEngine_RegisterStreamedWaveBank_1_0_4928 = 
{
    0,   // Large == 0
    13,  // Count == 13

    XREF_XACT_CEngine_RegisterStreamedWaveBank,  // Xref Is Saved
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * IXACTEngine_RegisterStreamedWaveBank 
// ******************************************************************
SOOVPA<9> IXACTEngine_RegisterStreamedWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_XACT_CEngine_RegisterStreamedWaveBank },

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
// * IXACTEngine_RegisterStreamedWaveBank 
// ******************************************************************
SOOVPA<10> IXACTEngine_RegisterStreamedWaveBank_1_0_4928 = 
{
    0,   // Large == 0
    10,  // Count == 10

    XRefNotSaved,
    1,

    {
		// IXACTEngine_RegisterStreamedWaveBank+0x22 : call XACT::CEngine::RegisterStreamedWaveBank
        { 0x23, XREF_XACT_CEngine_RegisterStreamedWaveBank },

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
    }
};

// ******************************************************************
// * XACT::CEngine::CreateSoundBank
// ******************************************************************
SOOVPA<8> XACT_CEngine_CreateSoundBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_CreateSoundBank,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0E, 0xA0 },
        { 0x1E, 0x8B },
        { 0x2E, 0xE6 },
        { 0x3E, 0x18 },
        { 0x4E, 0x7C },
        { 0x5E, 0xC7 },
        { 0x72, 0xFF },
        { 0x7E, 0x10 },
    }
};

// ******************************************************************
// * XACT::CEngine::CreateSoundBank
// ******************************************************************
SOOVPA<13> XACT_CEngine_CreateSoundBank_1_0_4928 = 
{
    0,   // Large == 0
    13,  // Count == 13

    XREF_XACT_CEngine_CreateSoundBank,  // Xref Is Saved
    XRefNotUsed,

    {
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
    }
};

// ******************************************************************
// * IXACTEngine_CreateSoundBank
// ******************************************************************
SOOVPA<8> IXACTEngine_CreateSoundBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x1D, XREF_XACT_CEngine_CreateSoundBank },

        { 0x03, 0x10 },
        { 0x08, 0xFF },
        { 0x0D, 0xC8 },
        { 0x12, 0x83 },
        { 0x17, 0x1B },
        { 0x1C, 0xE8 },
        { 0x21, 0xC2 },
    }
};

// ******************************************************************
// * IXACTEngine_CreateSoundBank
// ******************************************************************
SOOVPA<10> IXACTEngine_CreateSoundBank_1_0_4928 = 
{
    0,   // Large == 0
    10,  // Count == 10

    XRefNotSaved,
    1,

    {
        { 0x27, XREF_XACT_CEngine_CreateSoundBank },

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
    }
};

// ******************************************************************
// * XACT::CEngine::DownloadEffectsImage
// ******************************************************************
SOOVPA<7> XACT_CEngine_DownloadEffectsImage_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_XACT_CEngine_DownloadEffectsImage,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0B, 0x8B },
        { 0x12, 0xFF },
        { 0x1C, 0x46 },
        { 0x27, 0x85 },
        { 0x30, 0x89 },
        { 0x3A, 0x68 },
        { 0x45, 0x8B },
    }
};

// ******************************************************************
// * IXACTEngine_DownloadEffectsImage
// ******************************************************************
SOOVPA<9> IXACTEngine_DownloadEffectsImage_1_0_4627 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x1F, XREF_XACT_CEngine_DownloadEffectsImage },

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
// * XACT::CEngine::CreateSoundSource
// ******************************************************************
SOOVPA<8> XACT_CEngine_CreateSoundSource_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_CreateSoundSource,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x0A, 0x0F },
        { 0x14, 0x8B },
        { 0x1F, 0x50 },
        { 0x2A, 0x85 },
        { 0x35, 0x08 },
        { 0x40, 0x08 },
        { 0x4B, 0x15 },
        { 0x56, 0x0C },
    }
};

// ******************************************************************
// * IXACTEngine_CreateSoundSource
// ******************************************************************
SOOVPA<9> IXACTEngine_CreateSoundSource_1_0_4627 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_XACT_CEngine_CreateSoundSource },

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
// * XACT::CSoundBank::GetSoundCueIndexFromFriendlyName
// ******************************************************************
SOOVPA<7> XACT_CSoundBank_GetSoundCueIndexFromFriendlyName_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x12, 0x8B },
        { 0x22, 0x46 },
        { 0x34, 0x68 },
        { 0x46, 0x0C },
        { 0x58, 0x39 },
        { 0x6A, 0x45 },
        { 0x7E, 0xFF },
    }
};

// ******************************************************************
// * IXACTSoundBank_GetSoundCueIndexFromFriendlyName
// ******************************************************************
SOOVPA<9> IXACTSoundBank_GetSoundCueIndexFromFriendlyName_1_0_4627 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_XACT_CSoundBank_GetSoundCueIndexFromFriendlyName },

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
// * IXACTSoundBank_Play
// ******************************************************************
SOOVPA<8> IXACTSoundBank_Play_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    XRefNotUsed,

    {
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
// * XACT::CEngine::RegisterNotification
// ******************************************************************
SOOVPA<7> XACT_CEngine_RegisterNotification_1_0_4627 = 
{
    0,  // Large == 0
    7,  // Count == 7

    XREF_XACT_CEngine_RegisterNotification,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x8B },
        { 0x0C, 0x01 },
        { 0x13, 0xF0 },
        { 0x1A, 0xF6 },
        { 0x24, 0xFF },
        { 0x2A, 0x8B },
        { 0x2F, 0x08 },
    }
};

// ******************************************************************
// * IXACTEngine_RegisterNotification
// ******************************************************************
SOOVPA<8> IXACTEngine_RegisterNotification_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_XACT_CEngine_RegisterNotification },

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
// * XACT::CEngine::GetNotification
// ******************************************************************
SOOVPA<8> XACT_CEngine_GetNotification_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_GetNotification,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x11, 0x8D },
        { 0x24, 0x5A },
        { 0x37, 0xC0 },
        { 0x4A, 0x8B },
        { 0x5D, 0x04 },
        { 0x74, 0x8B },
        { 0x83, 0x40 },
        { 0x96, 0x0C },
    }
};

// ******************************************************************
// * IXACTEngine_GetNotification
// ******************************************************************
SOOVPA<9> IXACTEngine_GetNotification_1_0_4627 = 
{
    0,  // Large == 0
    9,  // Count == 9

    XRefNotSaved,
    1,

    {
        { 0x19, XREF_XACT_CEngine_GetNotification },

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
// * XACT::CEngine::UnRegisterWaveBank
// ******************************************************************
SOOVPA<8> XACT_CEngine_UnRegisterWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_UnRegisterWaveBank,  // Xref Is Saved
    XRefNotUsed,

    {
        { 0x07, 0x8B },
        { 0x10, 0x58 },
        { 0x19, 0x8B },
        { 0x22, 0x89 },
        { 0x2C, 0x8B },
        { 0x34, 0x5F },
        { 0x3D, 0xFF },
        { 0x46, 0x08 },
    }
};

// ******************************************************************
// * XACT::CEngine::UnRegisterWaveBank
// ******************************************************************
SOOVPA<8> XACT_CEngine_UnRegisterWaveBank_1_0_4928 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XREF_XACT_CEngine_UnRegisterWaveBank,  // Xref Is Saved
    XRefNotUsed,

    {
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
    }
};


// ******************************************************************
// * IXACTEngine_UnRegisterWaveBank
// ******************************************************************
SOOVPA<8> IXACTEngine_UnRegisterWaveBank_1_0_4627 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
        { 0x15, XREF_XACT_CEngine_UnRegisterWaveBank },

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
// * IXACTEngine_UnRegisterWaveBank
// ******************************************************************
SOOVPA<8> IXACTEngine_UnRegisterWaveBank_1_0_4928 = 
{
    0,  // Large == 0
    8,  // Count == 8

    XRefNotSaved,
    1,

    {
		// IXACTEngine_UnRegisterWaveBank+0x1E : call XACT::CEngine::UnRegisterWaveBank
        { 0x1F, XREF_XACT_CEngine_UnRegisterWaveBank },

		{ 0x07, 0xFF },
        { 0x0E, 0x8B },
        { 0x16, 0xF8 },
        { 0x1E, 0xE8 },
        { 0x26, 0xF8 },
        { 0x2E, 0xFF },
        { 0x36, 0x5F },
    }
};

// ******************************************************************
// * XactEng_1_0_4627
// ******************************************************************
OOVPATable XactEng_1_0_4627[] =
{
	// XACTEngineCreate
	{
		(OOVPA*) &XACTEngineCreate_1_0_4627, 
			
		XTL::EmuXACTEngineCreate,

		#ifdef _DEBUG_TRACE
		"EmuXACTEngineCreate"
		#endif
	},
	// XACTEngineCreate
	{
		(OOVPA*) &XACTEngineCreate_1_0_4928,
	
		XTL::EmuXACTEngineCreate,

		#ifdef _DEBUG_TRACE
		"EmuXACTEngineCreate_4928"
		#endif
	},
	// XACTEngineDoWork
	{
		(OOVPA*) &XACTEngineDoWork_1_0_4627,
	
		XTL::EmuXACTEngineDoWork,

		#ifdef _DEBUG_TRACE
		"EmuXACTEngineDoWork"
		#endif
	},
	// XACT_CEngine_RegisterWaveBank
	{
		(OOVPA*)&XACT_CEngine_RegisterWaveBank_1_0_4627, 0,

		#ifdef _DEBUG_TRACE
		"XACT::CEngine::RegisterWaveBank (XREF)"
		#endif
	},
	// IXACTEngine_RegisterWaveBank
	{
		(OOVPA*)&IXACTEngine_RegisterWaveBank_1_0_4627,

		XTL::EmuIXACTEngine_RegisterWaveBank,

		#ifdef _DEBUG_TRACE
		"EmuIXACTEngine_ResiterWaveBank"
		#endif
	},
	// XACT::CEngine::RegisterStreamedWaveBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_RegisterStreamedWaveBank_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::RegisterStreamedWaveBank (XREF)"
        #endif
    },
	// XACT::CEngine::RegisterStreamedWaveBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_RegisterStreamedWaveBank_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::RegisterStreamedWaveBank_4928 (XREF)"
        #endif
    },
	// IXACTEngine_RegisterStreamedWaveBank
    {
	    (OOVPA*)&IXACTEngine_RegisterStreamedWaveBank_1_0_4627,
        XTL::EmuIXACTEngine_RegisterStreamedWaveBank,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_RegisterStreamedWaveBank"
        #endif
    },
	// IXACTEngine_RegisterStreamedWaveBank
    {
	    (OOVPA*)&IXACTEngine_RegisterStreamedWaveBank_1_0_4928,
        XTL::EmuIXACTEngine_RegisterStreamedWaveBank,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_RegisterStreamedWaveBank_4928"
        #endif
    },
	// XACT::CEngine::CreateSoundBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_CreateSoundBank_1_0_4627, 0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::CreateSoundBank (XREF)"
        #endif
    },
	// XACT::CEngine::CreateSoundBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_CreateSoundBank_1_0_4928, 0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::CreateSoundBank_4928 (XREF)"
        #endif
    },
	// IXACTEngine_CreateSoundBank
    {
        (OOVPA*)&IXACTEngine_CreateSoundBank_1_0_4627,
        XTL::EmuIXACTEngine_CreateSoundBank,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_CreateSoundBank"
        #endif
    },
	// IXACTEngine_CreateSoundBank
    {
        (OOVPA*)&IXACTEngine_CreateSoundBank_1_0_4928,
        XTL::EmuIXACTEngine_CreateSoundBank,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_CreateSoundBank_4928"
        #endif
    },
	// XACT::CEngine::DownloadEffectsImage (XREF)
    {
        (OOVPA*)&XACT_CEngine_DownloadEffectsImage_1_0_4627,
        0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::DownloadEffectsImage (XREF)"
        #endif
    },
	// IXACTEngine_DownloadEffectsImage
    {
        (OOVPA*)&IXACTEngine_DownloadEffectsImage_1_0_4627,
        XTL::EmuIXACTEngine_DownloadEffectsImage,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_DownloadEffectsImage"
        #endif
    },
	// XACT::CEngine::CreateSoundSource (XREF)
    {
        (OOVPA*)&XACT_CEngine_CreateSoundSource_1_0_4627,
        0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::CreateSoundSource (XREF)"
        #endif
    },
	// IXACTEngine_CreateSoundSource
    {
        (OOVPA*)&IXACTEngine_CreateSoundSource_1_0_4627,
        XTL::EmuIXACTEngine_CreateSoundSource,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_CreateSoundSource"
        #endif
    },
	// XACT::CSoundBank::GetSoundCueIndexFromFriendlyName (XREF)
    {
        (OOVPA*)&XACT_CSoundBank_GetSoundCueIndexFromFriendlyName_1_0_4627,
        0,

        #ifdef _DEBUG_TRACE
        "XACT::CSoundBank::GetSoundCueIndexFromFriendlyName (XREF)"
        #endif
    },
	// IXACTSoundBank_GetSoundCueIndexFromFriendlyName
    {
        (OOVPA*)&IXACTSoundBank_GetSoundCueIndexFromFriendlyName_1_0_4627,
        XTL::EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName,

        #ifdef _DEBUG_TRACE
        "EmuIXACTSoundBank_GetSoundCueIndexFromFriendlyName"
        #endif
    },
	// IXACTSoundBank_Play
    {
        (OOVPA*)&IXACTSoundBank_Play_1_0_4627,
        XTL::EmuIXACTSoundBank_Play,

        #ifdef _DEBUG_TRACE
        "EmuIXACTSoundBank_Play"
        #endif
    },
	// XACT::CEngine::RegisterNotification
	{
		(OOVPA*)&XACT_CEngine_RegisterNotification_1_0_4627, 0,

		#ifdef _DEBUG_TRACE
		"XACT::CEngine::RegisterNotification (XREF)"
		#endif
	},
	// IXACTEngine_RegisterNotification
	{
		(OOVPA*)&IXACTEngine_RegisterNotification_1_0_4627, 
			
		XTL::EmuIXACTEngine_RegisterNotification,

		#ifdef _DEBUG_TRACE
		"EmuIXACTEngine_RegisterNotification (XREF)"
		#endif
	},
	// XACT::CEngine::GetNotification
    {
	    (OOVPA*)&XACT_CEngine_GetNotification_1_0_4627,
	    0,

	    #ifdef _DEBUG_TRACE
	    "XACT::CEngine::GetNotification (XREF)"
	    #endif
    },
	// IXACTEngine_GetNotification
    {
        (OOVPA*)&IXACTEngine_GetNotification_1_0_4627,
        XTL::EmuIXACTEngine_GetNotification,

        #ifdef _DEBUG_TRACE
        "EmuIXACTEngine_GetNotification"
        #endif
    },
	// XACT::CEngine::UnRegisterWaveBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_UnRegisterWaveBank_1_0_4627,
        0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::UnRegisterWaveBank (XREF)"
        #endif
    },
	// XACT::CEngine::UnRegisterWaveBank (XREF)
    {
        (OOVPA*)&XACT_CEngine_UnRegisterWaveBank_1_0_4928,
        0,

        #ifdef _DEBUG_TRACE
        "XACT::CEngine::UnRegisterWaveBank_4928 (XREF)"
        #endif
    },
	// IXACTEngine_UnRegisterWaveBank
    {
        (OOVPA*)&IXACTEngine_UnRegisterWaveBank_1_0_4627,
        XTL::EmuIXACTEngine_UnRegisterWaveBank,

        #ifdef _DEBUG_TRACE
        "IXACTEngine_UnRegisterWaveBank"
        #endif
    },
	// IXACTEngine_UnRegisterWaveBank
    {
        (OOVPA*)&IXACTEngine_UnRegisterWaveBank_1_0_4928,
        XTL::EmuIXACTEngine_UnRegisterWaveBank,

        #ifdef _DEBUG_TRACE
        "IXACTEngine_UnRegisterWaveBank_4928"
        #endif
    },
};

// ******************************************************************
// * XACTENG_1_0_4627_SIZE
// ******************************************************************
uint32 XactEng_1_0_4627_SIZE = sizeof(XactEng_1_0_4627);