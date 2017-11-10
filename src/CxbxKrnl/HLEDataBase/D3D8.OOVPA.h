// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->D3D8.OOVPA.h
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
// *  (c) 2017 jarupxx
// *  (c) 2017 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef D3D8_OOVPA_H
#define D3D8_OOVPA_H

#include "../OOVPA.h"

extern LOOVPA<1 + 11> D3DDevice_SetTextureState_TexCoordIndex_3911;
extern LOOVPA<2 + 15> D3DDevice_SetRenderState_CullMode_3911;

extern LOOVPA<1 + 12> D3DDevice_SetStreamSource_3911;

extern LOOVPA<1 + 10> D3DDevice_SetTextureState_TexCoordIndex_4034;
extern LOOVPA<2 + 14> D3DDevice_SetRenderState_CullMode_4034;

extern LOOVPA<1 + 14> D3DDevice_SetStreamSource_4034;

extern LOOVPA<1 + 10> D3DDevice_SetTextureState_TexCoordIndex_4242;

extern LOOVPA<1 + 10> D3DDevice_SetTextureState_TexCoordIndex_4627;

#endif