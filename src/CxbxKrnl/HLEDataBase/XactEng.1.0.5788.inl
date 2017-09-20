// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XactEng.1.0.5788.inl
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
// * XactEng_5788
// ******************************************************************
OOVPATable XactEng_5788[] = {

	REGISTER_OOVPA(XACTEngineCreate, 5120, PATCH),
	REGISTER_OOVPA(XACTEngineDoWork, 4627, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterStreamedWaveBank, 5233, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterStreamedWaveBank, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_CreateSoundBank, 4928, XREF),
	REGISTER_OOVPA(IXACTEngine_CreateSoundBank, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_UnRegisterWaveBank, 5120, XREF),
	REGISTER_OOVPA(IXACTEngine_UnRegisterWaveBank, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_GetNotification, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_GetNotification, 4928, PATCH),
	REGISTER_OOVPA(XACT_CSoundBank_GetSoundCueIndexFromFriendlyName, 4928, XREF),
	REGISTER_OOVPA(IXACTSoundBank_GetSoundCueIndexFromFriendlyName, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterWaveBank, 5233, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterWaveBank, 5120, PATCH),
	REGISTER_OOVPA(XACT_CEngine_RegisterNotification, 4627, XREF),
	REGISTER_OOVPA(IXACTEngine_RegisterNotification, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_CreateSoundSource, 4928, XREF),
	REGISTER_OOVPA(IXACTEngine_CreateSoundSource, 4928, PATCH),
	REGISTER_OOVPA(XACT_CEngine_DownloadEffectsImage, 5344, XREF),
	REGISTER_OOVPA(IXACTEngine_DownloadEffectsImage, 5344, PATCH),
	REGISTER_OOVPA(XACT_CSoundBank_Play, 5558, XREF),
	REGISTER_OOVPA(IXACTSoundBank_PlayEx, 5344, XREF), // Not Implemented yet.

	// ******************************************************************
	// Provisional registration functions in XDK 5788
	// TODO: Need test cases
	// ******************************************************************
	REGISTER_OOVPA(IXACTSoundBank_Play, 4627, PATCH),
	// ******************************************************************
};

// ******************************************************************
// * XACTENG_5788_SIZE
// ******************************************************************
uint32 XactEng_5788_SIZE = sizeof(XactEng_5788);
