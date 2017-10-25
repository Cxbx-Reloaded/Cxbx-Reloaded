// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->DSound.1.0.5849.inl
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
// * DirectSound::CDirectSound::SetRolloffFactor
// Xbe Explorer generated pattern, derived from address $00439F47 in "SpiderMan 2" :
// 56E8........833DD4........0FB6F0741685F6740B68E03B4400FF15...... 00 0000 005F ? SetRolloffFactor@CDirectSound@DirectSound@@QAGJMK@Z ^ 0002R ? DirectSoundEnterCriticalSection@@YGHXZ ^0009D _g_fDirectSoundInFinalRelease ^ 001DD __imp__RtlLeaveCriticalSection@4 ^ 0046R ? CommitDeferredSettings@CDirectSound@DirectSound@@QAGJXZ ^ 0055D __imp__RtlLeaveCriticalSection@4
// ******************************************************************
OOVPA_XREF(CDirectSound_SetRolloffFactor, 5849, 20,

    XREF_CDirectSound_SetRolloffFactor,
    XRefZero)

        { 0x00, 0x56 },
        { 0x01, 0xE8 },
        { 0x06, 0x83 },
        { 0x07, 0x3D },
        { 0x0D, 0x0F },
        { 0x0E, 0xB6 },
		{ 0x0F, 0xF0 },
		{ 0x10, 0x74 },
		{ 0x11, 0x16 },
		{ 0x12, 0x85 },
		{ 0x13, 0xF6 },
		{ 0x14, 0x74 },
		{ 0x15, 0x0B },
		{ 0x16, 0x68 },
		{ 0x1A, 0x00 },
		{ 0x1B, 0xFF },
		{ 0x1C, 0x15 },
		{ 0x35, 0x6C },
		{ 0x3C, 0x10 },
		{ 0x5C, 0xC2 },
OOVPA_END;
#if 0 // Duplicate OOVPA base on 4134
// ******************************************************************
// * DirectSound::CDirectSoundStream::SetPitch
// ******************************************************************
OOVPA_XREF(CDirectSoundStream_SetPitch, 5849, 12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x36, XREF_CDirectSoundVoice_SetPitch ),

        { 0x00, 0x56 },
        { 0x0C, 0x00 },
        { 0x14, 0x74 },
        { 0x21, 0xB8 },
        { 0x2A, 0x24 },
        { 0x35, 0xE8 },
        { 0x40, 0x68 },
        { 0x4B, 0x8B },
	{ 0x4F, 0xC2 },
	{ 0x50, 0x08 },
	{ 0x51, 0x00 },
OOVPA_END;
#endif
