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

#include <vector>  // Needed for EmuDSound.h file, must be outside of XTL namespace.

namespace XTL
{
    #include "core/HLE/XAPI/Xapi.h"
    #include "EmuD3D8.h"
    #include "EmuD3D8\Convert.h"
    #include "EmuD3D8\VertexBuffer.h"
    #include "EmuD3D8\PushBuffer.h"
    #include "EmuD3D8\VertexShader.h"
	#include "EmuD3D8\PixelShader.h"
    #include "EmuD3D8\State.h"
    #include "core/HLE/XAPI/OHCI/XInput/DInput.h"
    #include "core/HLE/DSOUND/DirectSound/DirectSound.hpp"
    #include "core/HLE/XONLINE/XOnline.h"
    #include "core/HLE/XGRAPHIC/XGraphic.h"
	#include "core/HLE/XACTENG/XactEng.h"
	#include "core/HLE/XAPI/OHCI/XInput/XInput.h"
}

extern XTL::IDirect3DDevice    *g_pD3DDevice;
extern DWORD                    g_CurrentXboxVertexShaderHandle;
extern XTL::X_PixelShader*		g_D3DActivePixelShader;
extern BOOL                     g_bIsFauxFullscreen;

#endif
