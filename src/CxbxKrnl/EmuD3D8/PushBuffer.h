// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->PushBuffer.h
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
#ifndef PUSHBUFFER_H
#define PUSHBUFFER_H

extern int DxbxFVF_GetTextureSize(DWORD dwFVF, int aTextureIndex);
extern UINT DxbxFVFToVertexSizeInBytes(DWORD dwFVF, BOOL bIncludeTextures);

extern void CxbxDrawIndexed(CxbxDrawContext &DrawContext, INDEX16 *pIndexData);
extern void CxbxDrawPrimitiveUP(CxbxDrawContext &DrawContext);

extern void EmuExecutePushBuffer
(
    X_D3DPushBuffer       *pPushBuffer,
    X_D3DFixup            *pFixup
);

extern void EmuExecutePushBufferRaw
(
    DWORD                 *pdwPushData,
	ULONG					dwSize
);

extern void DbgDumpPushBuffer
( 
	DWORD*				  PBData, 
	DWORD				  dwSize 
);

// primary push buffer
extern uint32  g_dwPrimaryPBCount;
extern uint32 *g_pPrimaryPB;

// push buffer debugging
extern bool g_bStepPush;
extern bool g_bSkipPush;
extern bool g_bBrkPush;

#endif