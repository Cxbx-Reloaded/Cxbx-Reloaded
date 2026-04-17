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
// *  All rights reserved
// *
// ******************************************************************
#ifndef INDEXBUFFERCONVERT_H
#define INDEXBUFFERCONVERT_H

#include "core\hle\D3D8\XbD3D8Types.h"

// Triangle fan to triangle list conversion
UINT FanToTriangleVertexCount(UINT NrOfFanVertices);
void CxbxConvertTriFanToTriangleListIndices(INDEX16* pFanIndexData, unsigned uNrOfFanVertices, INDEX16* pTriangleIndexData);
INDEX16* CxbxCreateTriFanToTriangleListIndexData(INDEX16* pFanIndexData, unsigned FanVertexCount);

// Winding order query (reads X_D3DRS_FRONTFACE)
bool CxbxGetClockWiseWindingOrder();

// Quad list to triangle list conversion
UINT QuadToTriangleVertexCount(UINT NrOfQuadVertices);
void CxbxConvertQuadListToTriangleListIndices(INDEX16* pXboxQuadIndexData, unsigned uNrOfTriangleIndices, INDEX16* pTriangleIndexData);
INDEX16* CxbxCreateQuadListToTriangleListIndexData(INDEX16* pXboxQuadIndexData, unsigned QuadVertexCount);

// Quad list index data cache (grown on demand)
INDEX16 *CxbxAssureQuadListIndexData(UINT NrOfQuadIndices);

// Release helpers
void CxbxReleaseQuadListToTriangleListIndexData(void* pHostIndexData);

// Quad-to-triangle D3D index buffer management
void CxbxAssureQuadListD3DIndexBuffer(UINT NrOfQuadIndices);

// Line loop closing-line helpers
void CxbxDrawIndexedClosingLine(INDEX16 LowIndex, INDEX16 HighIndex);
void CxbxDrawIndexedClosingLineUP(INDEX16 LowIndex, INDEX16 HighIndex, void *pHostVertexStreamZeroData, UINT uiHostVertexStreamZeroStride);

#endif // INDEXBUFFERCONVERT_H
