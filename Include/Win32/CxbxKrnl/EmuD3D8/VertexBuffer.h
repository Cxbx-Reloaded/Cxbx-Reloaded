// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->VertexBuffer.h
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
#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Cxbx.h"

#ifdef _DEBUG_TRACK_VB

struct VBNode
{
	XTL::IDirect3DVertexBuffer8 *vb;
	VBNode						*next;
};

extern bool g_bVBSkipStream;

extern class VBTracker
{
	public:
		VBTracker() : m_head(0), m_tail(0) {}
	   ~VBTracker();

	    // insert a ptr
		void insert(XTL::IDirect3DVertexBuffer8 *pVB);

		// remove a ptr
		void remove(XTL::IDirect3DVertexBuffer8 *pVB);

		// check for existance of ptr
		bool exists(XTL::IDirect3DVertexBuffer8 *pVB);

        // for traversal
        VBNode *getHead() { return m_head; }

    private:
		// list of "live" vertex buffers for debugging purposes
		VBNode *m_head;
		VBNode *m_tail;
}
g_VBTrackTotal, g_VBTrackDisable;

#endif

// fixup xbox extensions to be compatible with PC direct3d
extern UINT EmuFixupVerticesA
(
    DWORD                           PrimitiveType,
    UINT                           &PrimitiveCount,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8,
    UINT                            dwOffset,
    PVOID                           pVertexStreamZeroData,
    UINT                            uiVertexStreamZeroStride, 
    PVOID                          *ppNewVertexStreamZeroData
);

// fixup xbox extensions to be compatible with PC direct3d
extern VOID EmuFixupVerticesB
(
    UINT                            nStride,
    XTL::IDirect3DVertexBuffer8   *&pOrigVertexBuffer8,
    XTL::IDirect3DVertexBuffer8   *&pHackVertexBuffer8
);

#endif