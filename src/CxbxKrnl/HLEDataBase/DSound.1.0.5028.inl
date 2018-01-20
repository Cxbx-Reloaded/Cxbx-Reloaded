// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5028.inl
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
// * CDirectSoundStream_Flush
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundStream_Flush, 5028, 1+8,

    XRefNoSaveIndex,
    XRefOne)

        //CDirectSoundStream_Flush+0x31 : call [CMcpxStream_Flush]
        XREF_ENTRY( 0x32, XREF_CMcpxStream_Flush ),

        { 0x00, 0x56 },

        { 0x28, 0x8B },
        { 0x2B, 0x08 },

        { 0x2C, 0x8B },
        { 0x2E, 0x24 },

        { 0x31, 0xE8 },

        //CDirectSoundStream_Flush+0x48 : ret 4
        { 0x48, 0xC2 },
        { 0x49, 0x04 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_FlushEx
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundStream_FlushEx, 5028, 16,

    XREF_CDirectSoundStream_FlushEx,
    XRefZero)

        { 0x00, 0x55 },

        { 0x24, 0xB8 },
        { 0x25, 0x05 },
        { 0x26, 0x40 },
        { 0x27, 0x00 },
        { 0x28, 0x80 },

        { 0x29, 0xEB },
        { 0x2A, 0x3A },
        { 0x2B, 0x83 },
        { 0x2C, 0x7D },

        { 0x36, 0x74 },
        { 0x37, 0x12 },

        { 0x3E, 0xFF },
        { 0x40, 0x0C },

        { 0x67, 0xC2 },
        { 0x68, 0x10 },
OOVPA_END;

// ******************************************************************
// * CMcpxVoiceClient_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CMcpxVoiceClient_GetVoiceProperties, 5028, 14,

    XREF_CMcpxVoiceClient_GetVoiceProperties,
    XRefZero)

        { 0x00, 0x55 },
        { 0x01, 0x8B },
        { 0x03, 0x83 },
        { 0x06, 0x83 },
        { 0x0A, 0xF6 },

        // CMcpxVoiceClient_GetVoiceProperties+0xB5 : shr esi,6
        { 0xB5, 0xC1 },
        { 0xB6, 0xEE },
        { 0xB7, 0x06 },

        // CMcpxVoiceClient_GetVoiceProperties+0x102 : mov dword ptr [edi],0FFFFD8F0h
        { 0x102, 0xC7 },
        { 0x103, 0x07 },
        { 0x104, 0xF0 },
        { 0x105, 0xD8 },
        { 0x106, 0xFF },
        { 0x107, 0xFF },

        // After offset 0x11C, lots of changes has occurred.
OOVPA_END;

// ******************************************************************
// * CDirectSoundVoice_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundVoice_GetVoiceProperties, 5028, 1+7,

    XREF_CDirectSoundVoice_GetVoiceProperties,
    XRefOne)

        XREF_ENTRY( 0x0C, XREF_CMcpxVoiceClient_GetVoiceProperties),

        { 0x00, 0x8B },
        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x08, 0x8B },
        { 0x0B, 0xE8 },
        { 0x10, 0xC2 },
        { 0x11, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundBuffer_GetVoiceProperties, 5028, 1+7,

    XREF_CDirectSoundBuffer_GetVoiceProperties,
    XRefOne)

        XREF_ENTRY( 0x32, XREF_CDirectSoundVoice_GetVoiceProperties ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x2C, 0x10 },
        { 0x30, 0x10 },
        { 0x31, 0xE8 },
        { 0x4B, 0xC2 },
        { 0x4C, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundBuffer_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(IDirectSoundBuffer_GetVoiceProperties, 5028, 1+7,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x15, XREF_CDirectSoundBuffer_GetVoiceProperties),

        { 0x02, 0x24 },
        { 0x06, 0x24 },
        { 0x0A, 0x83 },
        { 0x0E, 0xD9 },
        { 0x12, 0xC8 },
        { 0x19, 0xC2 },
        { 0x1A, 0x08 },
OOVPA_END;

// ******************************************************************
// * CDirectSoundStream_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(CDirectSoundStream_GetVoiceProperties, 5028, 1+7,

    XREF_CDirectSoundStream_GetVoiceProperties,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_GetVoiceProperties ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x2B, 0x08 },
        { 0x30, 0x10 },
        { 0x35, 0xE8 },
        { 0x4F, 0xC2 },
        { 0x50, 0x08 },
OOVPA_END;

// ******************************************************************
// * IDirectSoundStream_GetVoiceProperties
// ******************************************************************
// Generic OOVPA as of 5028 and newer
OOVPA_XREF(IDirectSoundStream_GetVoiceProperties, 5028, 1+1,

    XRefNoSaveIndex,
    XRefOne)

        // IDirectSoundStream_GetVoiceProperties+0x00 : jmp [CDirectSoundStream_GetVoiceProperties]
        XREF_ENTRY(0x01, XREF_CDirectSoundStream_GetVoiceProperties),

        // IDirectSoundStream_GetVoiceProperties+0x00 : jmp 0x........
        { 0x00, 0xE9 },
OOVPA_END;
