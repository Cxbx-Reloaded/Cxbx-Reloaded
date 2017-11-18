// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.4831.inl
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
// CMcpxAPU::SynchPlayback
// ******************************************************************
// Generic OOVPA as of 4831 and newer
OOVPA_XREF(CMcpxAPU_SynchPlayback, 4831, 9,

    XREF_CMcpxAPU_SynchPlayback,
    XRefZero)
        { 0x00, 0x55 },

        //{ 0x0C, 0x74 }, // vs 5344 0x3C
        { 0x1A, 0x1A },
        { 0x25, 0x53 },
        { 0x37, 0xF2 },
        //{ 0x58, 0xBC }, // vs 5344 0x84

        { 0x7F, 0x47 },
        { 0x80, 0x04 },

        { 0x84, 0x43 },

        { 0x9E, 0x64 },
        { 0xFF, 0x00 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SynchPlayback
// ******************************************************************
OOVPA_XREF(CDirectSound_SynchPlayback, 4831, 1+9,

    XREF_CDirectSound_SynchPlayback,
    XRefOne)

        XREF_ENTRY( 0x08, XREF_CMcpxAPU_SynchPlayback ),

        // CDirectSound_SynchPlayback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // CDirectSound_SynchPlayback+0x04 : mov ecx, [eax+0x0C]
        { 0x04, 0x8B },
        { 0x05, 0x48 },
        { 0x06, 0x0C },

        // CDirectSound_SynchPlayback+0x0C : retn 0x04
        { 0x0C, 0xC2 },
        { 0x0D, 0x04 },
OOVPA_END;

// ******************************************************************
// * IDirectSound::SynchPlayback
// ******************************************************************
// Generic OOVPA as of 4831 and newer
OOVPA_XREF(IDirectSound_SynchPlayback, 4831, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x11, XREF_CDirectSound_SynchPlayback ),

        // IDirectSound_SynchPlayback+0x06 : add eax, 0xFFFFFFF8
        { 0x06, 0x83 },
        { 0x07, 0xC0 },
        { 0x08, 0xF8 },

        // IDirectSound_SynchPlayback+0x0B : sbb ecx, ecx
        { 0x0B, 0x1B },
        { 0x0C, 0xC9 },

        // IDirectSound_SynchPlayback+0x15 : retn 0x04
        { 0x15, 0xC2 },
        { 0x16, 0x04 },
OOVPA_END;
