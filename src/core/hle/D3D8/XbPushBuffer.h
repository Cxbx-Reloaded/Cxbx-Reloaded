// ******************************************************************
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
#ifndef XBPUSHBUFFER_H
#define XBPUSHBUFFER_H

#include "core/hle/D3D8/XbVertexBuffer.h" // for CxbxDrawContext

extern void CxbxDrawIndexed(CxbxDrawContext &DrawContext);
extern void CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext);

extern void EmuExecutePushBuffer
(
	xbox::X_D3DPushBuffer       *pPushBuffer,
	xbox::X_D3DFixup            *pFixup
);

extern void EmuExecutePushBufferRaw
(
    void *pPushData,
    uint32_t uSizeInBytes,
    uint32_t **p_dma_get,
    uint32_t **p_dma_put,
	uint8_t *dma
);

#endif
