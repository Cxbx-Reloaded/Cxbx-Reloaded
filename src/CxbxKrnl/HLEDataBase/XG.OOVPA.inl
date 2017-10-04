// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;	 .,;;  ;;;'';;'   `;;;	 .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o	   oP"``"Yo	 _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDatabase->XG.OOVPA.inl
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license	or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not	write to the Free Software Foundation	Inc.,
// *  59 Temple Place - Suite 330	Bostom	MA 02111-1307	USA.
// *
// *  (c) 2017 jarupxx
// *
// *  All rights reserved
// *
// ******************************************************************

// TODO: Known Xgraphics OOVPA issue list
// * 4034 is not verified each OOVPA.
// * Following OOVPA revision are not verified
//   * XGCompressRect (4242, 4721, 5028, 5120)
//   * XGSetIndexBufferHeader (4039)
//   * XGSetVertexBufferHeader (4039)
//   * XGSwizzleBox (4242)
//   * XGUnswizzleBox (4039)
//   * XGWriteSurfaceOrTextureToXPR (4242, 5028)
// NOTE: Known Xgraphics OOVPA not included in initial revision.
//   * XFONT_OpenBitmapFontFromMemory (4361)

#ifndef XGRAPHC_OOVPA_INL
#define XGRAPHC_OOVPA_INL

#include "../OOVPA.h"

#include "HLEDataBase/XG.1.0.3911.inl"
#include "HLEDataBase/XG.1.0.4034.inl"
#include "HLEDataBase/XG.1.0.4134.inl"
#include "HLEDataBase/XG.1.0.4361.inl"
#include "HLEDataBase/XG.1.0.4432.inl"
#include "HLEDataBase/XG.1.0.4627.inl"
#include "HLEDataBase/XG.1.0.5028.inl"
#include "HLEDataBase/XG.1.0.5233.inl"
#include "HLEDataBase/XG.1.0.5344.inl"
#include "HLEDataBase/XG.1.0.5558.inl"
#include "HLEDataBase/XG.1.0.5788.inl"
#include "HLEDataBase/XG.1.0.5849.inl"

// ******************************************************************
// * XGRAPHC_OOVPA
// ******************************************************************
OOVPATable XGRAPHC_OOVPAV2[] = {

	REGISTER_OOVPAS(XFONT_OpenBitmapFontFromMemory, XREF, 4361),
	REGISTER_OOVPAS(XGCompressRect, XREF, 3911),
	REGISTER_OOVPAS(XGIsSwizzledFormat, PATCH, 3911),
	REGISTER_OOVPAS(XGSetIndexBufferHeader, XREF, 3911, 4134/*4361*/),
	REGISTER_OOVPAS(XGSetTextureHeader, PATCH, 3911),
	REGISTER_OOVPAS(XGSetVertexBufferHeader, XREF, 3911, 4134/*4361*/),
	REGISTER_OOVPAS(XGSwizzleBox, PATCH, 3911),
	REGISTER_OOVPAS(XGSwizzleRect, UNPATCHED, 3911),
	REGISTER_OOVPAS(XGUnswizzleBox, UNPATCHED, 3911),
	REGISTER_OOVPAS(XGUnswizzleRect, UNPATCHED, 3911),
	REGISTER_OOVPAS(XGWriteSurfaceOrTextureToXPR, PATCH, 3911),
};

// ******************************************************************
// * XGRAPHC_OOVPA_SIZE
// ******************************************************************
uint32 XGRAPHC_OOVPA_SIZEV2 = sizeof(XGRAPHC_OOVPAV2);

#endif
