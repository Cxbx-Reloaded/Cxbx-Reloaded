// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XactEng.1.0.5233.inl
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
// * XACT::CEngine::RegisterStreamedWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterStreamedWaveBank, 5233, 11,

    XREF_XACT_CEngine_RegisterStreamedWaveBank,
    XRefZero)

        { 0x04, 0x10 }, //RegisterStreamedWaveBank 0x10 vs RegisterWaveBank 0x14
        { 0x25, 0x8B },

        { 0x3A, 0x2E }, //RegisterStreamedWaveBank 0x2E vs RegisterWaveBank 0x32
        { 0x3B, 0xFF },
        { 0x3C, 0x74 },
        { 0x3D, 0x24 },
        { 0x3E, 0x14 },
        { 0x3F, 0x8B },
        { 0x40, 0xCE },
        { 0x41, 0xE8 },

        { 0x67, 0xEB },
OOVPA_END;

// ******************************************************************
// * XACT::CEngine::RegisterWaveBank
// ******************************************************************
OOVPA_XREF(XACT_CEngine_RegisterWaveBank, 5233, 12,

    XREF_XACT_CEngine_RegisterWaveBank,
    XRefZero)

        { 0x04, 0x14 }, //RegisterStreamedWaveBank 0x10 vs RegisterWaveBank 0x14
        { 0x1A, 0x3B },

        { 0x3A, 0x32 },
        { 0x3B, 0xFF },
        { 0x3C, 0x74 },
        { 0x3D, 0x24 },
        { 0x3E, 0x18 },
        { 0x3F, 0x8B },
        { 0x40, 0xCE },
        { 0x41, 0xFF },

        { 0x7C, 0xC2 },
        { 0x7D, 0x10 },
OOVPA_END;
