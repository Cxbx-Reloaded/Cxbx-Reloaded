// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XactEng.1.0.5120.inl
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

// *****************************************************************
// * XACTEngineCreate
// ******************************************************************
OOVPA_NO_XREF(XACTEngineCreate, 5120, 12)

        { 0x11, 0x85 },
        { 0x29, 0xEB },

        { 0x5F, 0x24 },
        { 0x60, 0xFF },
        { 0x61, 0x74 },
        { 0x62, 0x24 },
        { 0x63, 0x10 },
        { 0x64, 0x8B },
        { 0x65, 0xCF },
        { 0x66, 0xE8 },

        { 0xA2, 0xC2 },
        { 0xA3, 0x08 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::UnRegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_UnRegisterWaveBank, 5120, 13,

    XREF_XACT_CEngine_UnRegisterWaveBank,
    XRefZero)


        { 0x0E, 0x51 },
        { 0x1F, 0x00 },

        { 0x25, 0x8B },
        { 0x26, 0x7C },
        { 0x27, 0x24 },
        { 0x28, 0x10 },
        { 0x29, 0x83 },
        { 0x2A, 0xC7 },
        { 0x2B, 0x4C },
        { 0x2C, 0x8B },
        { 0x2D, 0x37 },

        { 0x45, 0xF6 },
        { 0x62, 0xC2 },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterWaveBank, 5120, 12,

    XREF_XACT_CEngine_RegisterWaveBank,
    XRefZero)

        { 0x04, 0x14 }, //RegisterStreamedWaveBank 0x10 vs RegisterWaveBank 0x14
        { 0x17, 0x3B },

        { 0x37, 0x32 },
        { 0x38, 0xFF },
        { 0x39, 0x74 },
        { 0x3A, 0x24 },
        { 0x3B, 0x18 },
        { 0x3C, 0x8B },
        { 0x3D, 0xCE },
        { 0x3E, 0xFF },

        { 0x79, 0xC2 },
        { 0x7A, 0x10 },
OOVPA_END;

// ******************************************************************
// * IXACTEngine_RegisterWaveBank
// ******************************************************************
OOVPA_XREF(IXACTEngine_RegisterWaveBank, 5120, 1+9,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x27, XREF_XACT_CEngine_RegisterWaveBank ),

        // IXACTEngine_RegisterWaveBank+0x07 : push [ebp+8+0x10]
        { 0x07, 0xFF },
        { 0x08, 0x74 },
        { 0x09, 0x24 },
        { 0x0A, 0x18 },

        // IXACTEngine_RegisterWaveBank+0x0B : movzx esi, al
        { 0x0B, 0x0F },
        { 0x0C, 0xB6 },
        { 0x0D, 0xF0 },

        // IXACTEngine_RegisterWaveBank+0x40 : retn 0x10
        { 0x40, 0xC2 },
        { 0x41, 0x10 },
OOVPA_END;