// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->XactEng.OOVPA.inl
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

// TODO: Known XactEng OOVPA issue list
// * 4627 and 5028 are not verified each OOVPA. (Last known revision found is 4928. revision 5028 is obscure)
// * Following OOVPA revision are not verified
//   * XACT_CEngine_CreateSoundSource (5455)
//   * XACT_CEngine_DownloadEffectsImage (5455)
//   * XACT_CSoundBank_GetSoundCueIndexFromFriendlyName (5455)
//   * XACT_CSoundBank_Play (5455)
//   * IXACTEngine_CreateSoundSource (5455)
//   * IXACTEngine_DownloadEffectsImage (5455)
//   * IXACTEngine_RegisterWaveBank (5455)
//   * IXACTSoundBank_GetSoundCueIndexFromFriendlyName (5455)
//   * IXACTSoundBank_Play (4928 to 5849)

#ifndef XACTENG_OOVPA_INL
#define XACTENG_OOVPA_INL

#include "../OOVPA.h"

#include "XactEng.1.0.4627.inl"
#include "XactEng.1.0.4928.inl"
#include "XactEng.1.0.5120.inl"
#include "XactEng.1.0.5233.inl"
#include "XactEng.1.0.5344.inl"
#include "XactEng.1.0.5558.inl"
#include "XactEng.1.0.5788.inl"
#include "XactEng.1.0.5849.inl"


// ******************************************************************
// * XACTENG_OOVPA
// ******************************************************************
OOVPATable XACTENG_OOVPAV2[] = {

	REGISTER_OOVPAS(XACTEngineCreate, UNPATCHED, 4627, 4928, 5120),
	REGISTER_OOVPAS(XACTEngineDoWork, UNPATCHED, 4627),
	REGISTER_OOVPAS(XACT_CEngine_CreateSoundBank, XREF, 4627, 4928),
	REGISTER_OOVPAS(XACT_CEngine_CreateSoundSource, XREF, 4627, 4928),
	REGISTER_OOVPAS(XACT_CEngine_DownloadEffectsImage, XREF, 4627, 5344),
	REGISTER_OOVPAS(XACT_CEngine_GetNotification, XREF, 4627),
	REGISTER_OOVPAS(XACT_CEngine_RegisterNotification, XREF, 4627),
	REGISTER_OOVPAS(XACT_CEngine_RegisterStreamedWaveBank, XREF, 4627, 4928, 5233),
	REGISTER_OOVPAS(XACT_CEngine_RegisterWaveBank, XREF, 4627, 5120, 5233),
	REGISTER_OOVPAS(XACT_CEngine_UnRegisterWaveBank, XREF, 4627, 4928, 5120),
	REGISTER_OOVPAS(XACT_CSoundBank_GetSoundCueIndexFromFriendlyName, XREF, 4627, 4928),
	REGISTER_OOVPAS(XACT_CSoundBank_Play, XREF, 5344, 5558),
	REGISTER_OOVPAS(IXACTEngine_CreateSoundBank, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTEngine_CreateSoundSource, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTEngine_DownloadEffectsImage, UNPATCHED, 4627, 5344),
	REGISTER_OOVPAS(IXACTEngine_GetNotification, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTEngine_RegisterNotification, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTEngine_RegisterStreamedWaveBank, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTEngine_RegisterWaveBank, UNPATCHED, 4627, 5120),
	REGISTER_OOVPAS(IXACTEngine_UnRegisterWaveBank, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTSoundBank_GetSoundCueIndexFromFriendlyName, UNPATCHED, 4627, 4928),
	REGISTER_OOVPAS(IXACTSoundBank_Play, UNPATCHED, 4627),
	REGISTER_OOVPAS(IXACTSoundBank_PlayEx, UNPATCHED, 5344), // Not Implemented yet.
};

// ******************************************************************
// * XACTENG_OOVPA_SIZE
// ******************************************************************
uint32 XACTENG_OOVPA_SIZEV2 = sizeof(XACTENG_OOVPAV2);

#endif
