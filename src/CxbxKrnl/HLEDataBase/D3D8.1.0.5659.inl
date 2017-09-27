// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->D3D8.1.0.5659.cpp
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

// ******************************************************************
// * D3DDevice_SetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetMaterial, 5659, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x00 },
        { 0x0F, 0x0F },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetMaterial, 5659, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0x00 },
        { 0x0E, 0x0F },

        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_AddRef
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_AddRef, 5659, 10)

        // D3DDevice_AddRef+0x00 : mov eax, [addr]
        { 0x00, 0xA1 },

        // D3DDevice_AddRef+0x05 : mov ecx, [eax+0x0938]
        { 0x05, 0x8B },
        { 0x06, 0x88 },
        { 0x07, 0x38 },
        { 0x08, 0x09 },

        // D3DDevice_AddRef+0x0B : inc ecx
        { 0x0B, 0x41 },

        // D3DDevice_AddRef+0x0C : mov [eax+0x0938], ecx
        { 0x0C, 0x89 },
        { 0x0D, 0x88 },
        { 0x0E, 0x38 },
        { 0x0F, 0x09 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetBackMaterial, 5659, 11)

        { 0x04, 0x08 },

        { 0x0C, 0x81 },
        { 0x0D, 0xC7 },
        { 0x0E, 0x44 },
        { 0x0F, 0x0F },
        { 0x10, 0x00 },

        { 0x16, 0x00 },
        { 0x1F, 0x81 },
        { 0x22, 0x90 },
        { 0x2C, 0x5E },
        { 0x2E, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_GetBackMaterial
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_GetBackMaterial, 5659, 8)

        { 0x05, 0x56 },
        { 0x06, 0x57 },
        { 0x0A, 0x0C },

        { 0x0D, 0x44 },
        { 0x0E, 0x0F },

        { 0x12, 0x11 },
        { 0x16, 0xF3 },
        { 0x1A, 0xC2 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetVerticalBlankCallback
// ******************************************************************
OOVPA_XREF(D3DDevice_SetVerticalBlankCallback, 5659, 1+12,

    XRefNoSaveIndex,
    XRefOne)

        XREF_ENTRY( 0x06, XREF_D3DDEVICE ),

        // D3DDevice_SetVerticalBlankCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetVerticalBlankCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetVerticalBlankCallback+0x0A : mov [ecx+0x1DB4], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xB8 }, // B4 vs B8
        { 0x0D, 0x1D },

        // D3DDevice_SetVerticalBlankCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;

// ******************************************************************
// * D3DDevice_SetSwapCallback
// ******************************************************************
OOVPA_NO_XREF(D3DDevice_SetSwapCallback, 5659, 12)

        // D3DDevice_SetSwapCallback+0x00 : mov eax, [esp+0x04]
        { 0x00, 0x8B },
        { 0x01, 0x44 },
        { 0x02, 0x24 },
        { 0x03, 0x04 },

        // D3DDevice_SetSwapCallback+0x04 : mov ecx, [addr]
        { 0x04, 0x8B },
        { 0x05, 0x0D },

        // D3DDevice_SetSwapCallback+0x0A : mov [ecx+0x1DB4], eax
        { 0x0A, 0x89 },
        { 0x0B, 0x81 },
        { 0x0C, 0xB4 }, // B4 vs B8
        { 0x0D, 0x1D },

        // D3DDevice_SetSwapCallback+0x10 : retn 0x04
        { 0x10, 0xC2 },
        { 0x11, 0x04 },
OOVPA_END;
