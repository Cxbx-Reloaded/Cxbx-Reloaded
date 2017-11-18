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

// ******************************************************************
// * CMcpxBuffer_GetStatus
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_GetStatus, 4831, 13,

	XREF_CMcpxBuffer_GetStatus,
    XRefZero)

        { 0x00, 0x0F },

        { 0x16, 0x6A },
        { 0x17, 0x00 },
        { 0x18, 0x5A },

        { 0x1C, 0x42 },
        { 0x1D, 0xF6 },
        { 0x1E, 0xC4 },
        { 0x1F, 0x02 },

        { 0x29, 0xB9 },
        { 0x2A, 0x01 },
        { 0x2B, 0x80 },

        { 0x3F, 0xC2 },
        { 0x40, 0x04 },
OOVPA_END;

// ******************************************************************
// * public: long __thiscall DirectSound::CMcpxBuffer::Play(unsigned long)
// ******************************************************************
// Generic OOVPA as of 4831 and newer
OOVPA_XREF(CMcpxBuffer_Play, 4831, 11,

    XREF_CMcpxBuffer_Play,
    XRefZero)

        // CMcpxBuffer_Play+0x00 : push ebx
        { 0x00, 0x53 },

        // CMcpxBuffer_Play+0x32 : test byte ptr [esi+12h], 42h
        { 0x32, 0xF6 },
        { 0x33, 0x46 },
        { 0x34, 0x12 },
        { 0x35, 0x42 },

        // CMcpxBuffer_Play+0x4D : test byte ptr [eax+9], 20h
        { 0x4D, 0xF6 },
        { 0x4E, 0x40 },
        { 0x4F, 0x09 },
        { 0x50, 0x20 },

        // CMcpxBuffer_Play+0x71 : xor eax, eax
        { 0x71, 0x33 },
        { 0x72, 0xC0 },
OOVPA_END;

// ******************************************************************
// * CMcpxStream_Pause
// ******************************************************************
OOVPA_XREF(CMcpxStream_Pause, 4831, 11,

    XREF_CMcpxStream_Pause,
    XRefZero)

        // CMcpxStream_Pause+0x17 : and ax, 0xFFDF
        { 0x17, 0x66 },
        { 0x18, 0x25 },
        { 0x19, 0xDF },

        // CMcpxStream_Pause+0x32 : jmp +0x56
        { 0x32, 0xEB },
        { 0x33, 0x56 },

        // CMcpxStream_Pause+0x39 : movzx eax, word ptr [esi+0x12]
        { 0x39, 0x0F },
        { 0x3A, 0xB7 },
        { 0x3B, 0x46 },
        { 0x3C, 0x12 },

        // CMcpxStream_Pause+0x8B : retn 0x04
        { 0x8B, 0xC2 },
        { 0x8C, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSound::SetAllParametersA
// ******************************************************************
OOVPA_XREF(CDirectSound_SetAllParametersA, 4831, 10,

    XREF_CDirectSound_SetAllParameters,
    XRefZero)

        { 0x16, 0x68 },
        { 0x17, 0x08 },
        { 0x18, 0x6E },
        { 0x19, 0x18 },

        { 0x2F, 0xD9 },
        { 0x30, 0x41 },
        { 0x31, 0x04 },

        { 0x87, 0x8B },
        { 0x88, 0x50 },
        { 0x89, 0x08 },
OOVPA_END;

// ******************************************************************
// * DirectSound::CMcpxBuffer::Pause
// ******************************************************************
OOVPA_XREF(CMcpxBuffer_Pause, 4831, 7,

    XREF_CMcpxBuffer_Pause,
    XRefZero)

        { 0x0D, 0x4D },
        { 0x1C, 0x44 },
        { 0x2B, 0x8B },
        { 0x3A, 0x75 },
        { 0x49, 0x7D },
        { 0x58, 0xEB },
        { 0x67, 0xE8 },
OOVPA_END;
