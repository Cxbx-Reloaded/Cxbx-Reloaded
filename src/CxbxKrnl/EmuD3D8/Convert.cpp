// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuD3D8->Convert.cpp
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
// *  (c) 2002-2004 Aaron Robinson <caustik@caustik.com>
// *                Kingofc <kingofc@freenet.de>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "CxbxKrnl/Emu.h"
#include "CxbxKrnl/EmuXTL.h"

BOOL XTL::EmuXBFormatIsSwizzled(X_D3DFORMAT Format, DWORD *pBPP)
{
    switch(Format)
    {
        case 0x00:
        case 0x01:
        case 0x0B:
            *pBPP = 1;
            return TRUE;
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x1A:
            *pBPP = 2;
            return TRUE;
        case 0x06:
        case 0x07:
            *pBPP = 4;
            return TRUE;
    }

    return FALSE;
}

BOOL XTL::EmuXBFormatIsLinear(X_D3DFORMAT Format)
{
    switch(Format)
    {
        case 0x10: // X_D3DFMT_LIN_A1R5G5B5
        case 0x11: // X_D3DFMT_LIN_R5G6B5
        case 0x12: // X_D3DFMT_LIN_A8R8G8B8
        case 0x16: // X_D3DFMT_LIN_R8B8
        case 0x1D: // X_D3DFMT_LIN_A4R4G4B4
        case 0x1E: // X_D3DFMT_LIN_X8R8G8B8
        case 0x2E: // X_D3DFMT_LIN_D24S8
        case 0x30: // X_D3DFMT_LIN_D16
        case 0x3F: // X_D3DFMT_LIN_A8B8G8R8
            return TRUE;
    }

    return FALSE;
}

XTL::D3DFORMAT XTL::EmuXB2PC_D3DFormat(X_D3DFORMAT Format)
{
    switch(Format)
    {
        case 0x00: // Swizzled   (X_D3DFMT_L8)
            return D3DFMT_L8;

        case 0x01: // Swizzled   (X_D3DFMT_AL8) // NOTE: Hack: Alpha ignored, basically
        {
            EmuWarning("X_D3DFMT_AL8 -> D3DFMT_L8");
            return D3DFMT_L8;
        }
        case 0x10: // Linear     (X_D3DFMT_LIN_A1R5G5B5)
        case 0x02: // Swizzled   (X_D3DFMT_A1R5G5B5)
            return D3DFMT_A1R5G5B5;

        case 0x03: // Swizzled   (X_D3DFMT_X1R5G5B5)
            return D3DFMT_X1R5G5B5;

        case 0x1A: // Swizzled   (X_D3DFMT_A8L8)
        {
            EmuWarning("X_D3DFMT_A8L8 -> D3DFMT_R5G6B5");
            return D3DFMT_R5G6B5;   // NOTE: HACK: Totally and utterly wrong :)
        }
        case 0x19: // Swizzled   (X_D3DFMT_A8)
            return D3DFMT_A8;

        case 0x1D: // Linear     (X_D3DFMT_LIN_A4R4G4B4)
        case 0x04: // Swizzled   (X_D3DFMT_A4R4G4B4)
            return D3DFMT_A4R4G4B4;

        case 0x11: // Linear     (X_D3DFMT_LIN_R5G6B5)
        case 0x05: // Swizzled   (X_D3DFMT_R5G6B5)
            return D3DFMT_R5G6B5;

        case 0x12: // Linear     (X_D3DFMT_LIN_A8R8G8B8)
        case 0x06: // Swizzled   (X_D3DFMT_A8R8G8B8)
            return D3DFMT_A8R8G8B8;

        case 0x16: // Linear     (X_D3DFMT_LIN_R8B8)
        {
            EmuWarning("X_D3DFMT_LIN_R8B8 -> D3DFMT_R5G6B5");
            return D3DFMT_R5G6B5;   // NOTE: HACK: Totally and utterly wrong :)
        }
        case 0x3F: // Linear     (X_D3DFMT_LIN_A8B8G8R8)
        {
            EmuWarning("X_D3DFMT_LIN_A8B8G8R8 -> D3DFMT_A8R8G8B8");
            return D3DFMT_A8R8G8B8; // NOTE: HACK: R<->B Swapped!
        }

        case 0x1E: // Linear     (X_D3DFMT_LIN_X8R8G8B8)
        case 0x07: // Swizzled   (X_D3DFMT_X8R8G8B8)
            return D3DFMT_X8R8G8B8;

        case 0x0B: // Swizzled   (X_D3DFMT_P8)
            return D3DFMT_P8;

        case 0x0C: // Compressed (X_D3DFMT_DXT1)
            return D3DFMT_DXT1;

        case 0x0E: // Compressed (X_D3DFMT_DXT2)
            return D3DFMT_DXT2;

        case 0x0F: // Compressed (X_D3DFMT_DXT3)
            return D3DFMT_DXT3;

        case 0x24: // Swizzled   (X_D3DFMT_YUV2)
            return D3DFMT_YUY2;

        case 0x2E: // Linear     (X_D3DFMT_LIN_D24S8)
        case 0x2A: // Swizzled   (X_D3DFMT_D24S8)
            return D3DFMT_D24S8;

        case 0x2B: // Swizzled   (X_D3DFMT_F24S8)
        {
            EmuWarning("X_D3DFMT_F24S8 -> D3DFMT_D24S8");
            return D3DFMT_D24S8;    // NOTE: Hack!! PC does not have D3DFMT_F24S8 (Float vs Int)
        }

        case 0x30: // Linear     (X_D3DFMT_LIN_D16)
        case 0x2C: // Swizzled   (X_D3DFMT_D16)
            return D3DFMT_D16;

        case 0x27: // Swizzled   (X_D3DFMT_L6V5U5)
            return D3DFMT_L6V5U5;

        case 0x28: // Swizzled   (X_D3DFMT_V8U8)
            return D3DFMT_V8U8;

        case 0x33: // Swizzled   (X_D3DFMT_V16U16)
            return D3DFMT_V16U16;

        case 0x64:
            return D3DFMT_VERTEXDATA;
    }

    CxbxKrnlCleanup("EmuXB2PC_D3DFormat: Unknown Format (0x%.08X)", Format);

    return (D3DFORMAT)Format;
}

XTL::X_D3DFORMAT XTL::EmuPC2XB_D3DFormat(D3DFORMAT Format)
{
    switch(Format)
    {
        case D3DFMT_YUY2:
            return 0x24;
        case D3DFMT_R5G6B5:
            return 0x11;        // Linear
//            return 0x05;      // Swizzled
        case D3DFMT_D24S8:
            return 0x2A;
        case D3DFMT_DXT3:
            return 0x0F;
        case D3DFMT_DXT2:
            return 0x0E;
        case D3DFMT_DXT1:
            return 0x0C;
        case D3DFMT_A1R5G5B5:   // Linear (X_D3DFMT_LIN_A1R5G5B5)
            return 0x10;
        case D3DFMT_X8R8G8B8:
            return 0x1E;        // Linear (X_D3DFMT_LIN_X8R8G8B8)
//            return 0x07;      // Swizzled
        case D3DFMT_A8R8G8B8:
//            return 0x12;      // Linear (X_D3DFMT_LIN_A8R8G8B8)
            return 0x06;

        case D3DFMT_A4R4G4B4:
            return 0x1D;        // Linear
//            return 0x04;      // Swizzled
    }

    CxbxKrnlCleanup("EmuPC2XB_D3DFormat: Unknown Format (%d)", Format);

    return Format;
}

DWORD XTL::EmuXB2PC_D3DLock(DWORD Flags)
{
    DWORD NewFlags = 0;

    // Need to convert the flags, TODO: fix the xbox extensions
    if(Flags & X_D3DLOCK_NOFLUSH)
    {
        NewFlags ^= 0;
    }
    if(Flags & X_D3DLOCK_NOOVERWRITE)
    {
        NewFlags ^= D3DLOCK_NOOVERWRITE;
    }
    if(Flags & X_D3DLOCK_TILED)
    {
        NewFlags ^= 0;
    }
    if(Flags & X_D3DLOCK_READONLY)
    {
        NewFlags ^= D3DLOCK_READONLY;
    }

    return NewFlags;
}

// lookup table for converting vertex count to primitive count
UINT XTL::EmuD3DVertexToPrimitive[11][2] =
{
    {0, 0},
    {1, 0},
    {2, 0},
    {1, 1},
    {1, 1},
    {3, 0},
    {1, 2},
    {1, 2},
    {4, 0},
    {2, 2},
    {0, 0},
};

// conversion table for xbox->pc primitive types
XTL::D3DPRIMITIVETYPE XTL::EmuPrimitiveTypeLookup[] =
{
    /* NULL                 = 0         */ (XTL::D3DPRIMITIVETYPE)0,
    /* D3DPT_POINTLIST      = 1,        */ XTL::D3DPT_POINTLIST,
    /* D3DPT_LINELIST       = 2,        */ XTL::D3DPT_LINELIST,
    /* D3DPT_LINELOOP       = 3,  Xbox  */ XTL::D3DPT_LINESTRIP,
    /* D3DPT_LINESTRIP      = 4,        */ XTL::D3DPT_LINESTRIP,
    /* D3DPT_TRIANGLELIST   = 5,        */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_TRIANGLESTRIP  = 6,        */ XTL::D3DPT_TRIANGLESTRIP,
    /* D3DPT_TRIANGLEFAN    = 7,        */ XTL::D3DPT_TRIANGLEFAN,
    /* D3DPT_QUADLIST       = 8,  Xbox  */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_QUADSTRIP      = 9,  Xbox  */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_POLYGON        = 10, Xbox  */ XTL::D3DPT_TRIANGLELIST,
    /* D3DPT_MAX            = 11,       */ (XTL::D3DPRIMITIVETYPE)11
};

// render state conversion table
CONST DWORD XTL::EmuD3DRenderStateSimpleEncoded[174] =
{
    // WARNING: This lookup table strongly binds us to an SDK with these
    // specific #define values for D3DRS_*. Make VERY sure that you have
    // the correct lookup values;
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 0
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 2
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 4
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 6
    X_D3DRSSE_UNK,  0x0004037c,     // 8  - D3DRS_SHADEMODE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 10
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 12
    0x0004035c,     0x00040300,     // 14 - D3DRS_ZWRITEENABLE, D3DRS_ALPHATESTENABLE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 16
    X_D3DRSSE_UNK,  0x00040344,     // 18
    0x00040348,     X_D3DRSSE_UNK,  // 20 - D3DRS_DESTBLEND
    X_D3DRSSE_UNK,  0x00040354,     // 22 - , D3DRS_ZFUNC
    0x00040340,     0x0004033c,     // 24 - D3DRS_ALPHAREF, D3DRS_ALPHAFUNC
    0x00040310,     0x00040304,     // 26 - D3DRS_DITHERENABLE, D3DRS_ALPHABLENDENABLE
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 28
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 30
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 32
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 34
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 36
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 38
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 40
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 42
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 44
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 46
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 48
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 50
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 52
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 54
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 56
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 58
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 60
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 62
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 64
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 66
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 68
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 70
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 72
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 74
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 76
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 78
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 80
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 82
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 84
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 86
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 88
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 90
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 92
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 94
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 96
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 98
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 100
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 102
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 104
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 106
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 108
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 110
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 112
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 114
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 116
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 118
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 120
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 122
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 124
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 126
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 128
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 130
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 132
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 134
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 136
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 138
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 140
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 142
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 144
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 146
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 148
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 150
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 152
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 154
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 156
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 158
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 160
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 162
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 164
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 166
    0x00040358,     X_D3DRSSE_UNK,  // 168 - D3DRS_COLORWRITEENABLE
    X_D3DRSSE_UNK,  0x00040350,     // 170
    X_D3DRSSE_UNK,  X_D3DRSSE_UNK,  // 172
};
