// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXTL.h
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
#ifndef EMUXTL_H
#define EMUXTL_H

namespace XTL
{
    #include "EmuXapi.h"
    #include "EmuD3D8.h"
    #include "EmuD3D8\Convert.h"
    #include "EmuD3D8\VertexBuffer.h"
    #include "EmuD3D8\PushBuffer.h"
    #include "EmuD3D8\VertexShader.h"
    #include "EmuD3D8\State.h"
    #include "EmuDInput.h"
    #include "EmuDSound.h"
//	#include "EmuXBAudio\XboxAdpcm.h"
//	#include "EmuXBAudio\XSoundBank.h"
//	#include "EmuXBAudio\XWaveBank.h"
    #include "EmuXOnline.h"
    #include "EmuXG.h"
	#include "EmuXactEng.h"
}

extern XTL::LPDIRECT3DDEVICE8   g_pD3DDevice8;
extern DWORD                    g_CurrentVertexShader;
extern BOOL                     g_bFakePixelShaderLoaded;
extern BOOL                     g_bIsFauxFullscreen;

#endif
