// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->XG.1.0.5344.cpp
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
// * XG_5344
// ******************************************************************
OOVPATable XG_5344[] = {

	REGISTER_OOVPA(XGIsSwizzledFormat, 3911, PATCH),
	// REGISTER_OOVPA(XGSwizzleRect, 3911, DISABLED), // TODO : Uncomment
	// REGISTER_OOVPA(XGUnswizzleRect, 3911, DISABLED), // TODO : Uncomment
	REGISTER_OOVPA(XGSwizzleBox, 3911, PATCH),
	REGISTER_OOVPA(XGWriteSurfaceOrTextureToXPR, 3911, PATCH),
	REGISTER_OOVPA(XGSetTextureHeader, 3911, PATCH),
	REGISTER_OOVPA(XGSetVertexBufferHeader, 4361, XREF),
	REGISTER_OOVPA(XGSetIndexBufferHeader, 4361, XREF),
	REGISTER_OOVPA(XGCompressRect, 3911, XREF),
	REGISTER_OOVPA(XGUnswizzleBox, 3911, DISABLED),
	REGISTER_OOVPA(XFONT_OpenBitmapFontFromMemory, 4361, XREF),
};

// ******************************************************************
// * XG_5344_SIZE
// ******************************************************************
uint32 XG_5344_SIZE = sizeof(XG_5344);
