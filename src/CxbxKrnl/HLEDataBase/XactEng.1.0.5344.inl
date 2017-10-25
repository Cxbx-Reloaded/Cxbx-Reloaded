// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XactEng.1.0.5344.inl
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
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// ******************************************************************
// * XACT::CEngine::DownloadEffectsImage
// ******************************************************************
OOVPA_XREF(XACT_CEngine_DownloadEffectsImage, 5344, 12,

    XREF_XACT_CEngine_DownloadEffectsImage,
    XRefZero)

        { 0x02, 0xEC },

        { 0x08, 0x46 },
        { 0x09, 0x10 },
        { 0x0A, 0x57 },
        { 0x0B, 0x8B },
        { 0x0C, 0x7D },
        { 0x0D, 0x18 },
        { 0x0E, 0x57 },
        { 0x0F, 0xFF },

        { 0x1E, 0x85 },
        { 0x2A, 0xC2 },
        { 0x2B, 0x14 },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_DownloadEffectsImage
// ******************************************************************
OOVPA_XREF(IXACTEngine_DownloadEffectsImage, 5344, 11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x29, XREF_XACT_CEngine_DownloadEffectsImage ),

        { 0x05, 0xE8 },

        { 0x19, 0x8B },
        { 0x1A, 0xC8 },
        { 0x1B, 0xFF },
        { 0x1C, 0x75 },
        { 0x1D, 0x0C },
        { 0x1E, 0x83 },
        { 0x1F, 0xC0 },

        { 0x43, 0xC2 },
        { 0x44, 0x14 },
OOVPA_END;

// ******************************************************************
// * XACT::CSoundBank::Play
// ******************************************************************
OOVPA_XREF(XACT_CSoundBank_Play, 5344, 12,

    XREF_XACT_CSoundBank_Play,
    XRefZero)

        { 0x08, 0x57 },
        { 0x1F, 0x89 },

        { 0x7B, 0x89 },
        { 0x7C, 0x45 },
        { 0x7D, 0xFC },
        { 0x7E, 0xEB },
        { 0x7F, 0x53 },
        { 0x80, 0x3B },
        { 0x81, 0xC7 },
        { 0x82, 0x74 },

        { 0x9B, 0x68 },
        { 0xA5, 0x83 },
OOVPA_END;

// ******************************************************************
// * IXACTSoundBank_PlayEx
// ******************************************************************
OOVPA_XREF(IXACTSoundBank_PlayEx, 5344, 11,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x17, XREF_XACT_CSoundBank_Play ),

        { 0x01, 0x57 },
        { 0x0E, 0xFF },
        { 0x0F, 0x74 },
        { 0x10, 0x24 },
        { 0x11, 0x14 },
        { 0x1B, 0x85 },
        { 0x1C, 0xF6 },
        { 0x1D, 0x8B },
        { 0x30, 0xC2 },
        { 0x31, 0x0C },
OOVPA_END;
