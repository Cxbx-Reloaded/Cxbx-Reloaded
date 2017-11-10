// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->HLEDataBase->XOnline.1.0.4627.inl
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

#if 0 // Belongs in XNet lib
// ******************************************************************
// * XnInit
// ******************************************************************
OOVPA_XREF(XnInit, 4627, 12,

    XREF_XnInit,
    XRefZero)

         XnInit+0x03 : sub esp, 0x0218
        { 0x03, 0x81 },
        { 0x04, 0xEC },
        { 0x05, 0x18 },
        { 0x06, 0x02 },

         XnInit+0x19 : cmpxchg [ecx], edx
        { 0x19, 0x0F },
        { 0x1A, 0xB1 },
        { 0x1B, 0x11 },

         XnInit+0x3C : push 0x4454454E
        { 0x3C, 0x68 },
        { 0x3D, 0x4E },
        { 0x3E, 0x45 },
        { 0x3F, 0x54 },
        { 0x40, 0x44 },
OOVPA_END;
#endif

#if 0 // Moved to XNet 4627
// ******************************************************************
// * XNetGetEthernetLinkStatus
// ******************************************************************
OOVPA_NO_XREF(XNetGetEthernetLinkStatus, 4627, 8)

        { 0x08, 0x33 },
        { 0x10, 0x8A },
        { 0x19, 0x00 },
        { 0x22, 0x56 },
        { 0x2B, 0xFF },
        { 0x34, 0xF0 },
        { 0x3D, 0x24 },
        { 0x46, 0x5B },
OOVPA_END;
#endif

// ******************************************************************
// * XoUpdateLaunchNewImageInternal
// ******************************************************************
OOVPA_XREF(XoUpdateLaunchNewImageInternal, 4627, 11,

    XREF_XoUpdateLaunchNewImageInternal,
    XRefZero)

        { 0x00, 0x55 },
        { 0x03, 0x81 },

        { 0x58, 0xEB },
        { 0x59, 0x04 },
        { 0x5A, 0x83 },
        { 0x5B, 0x63 },
        { 0x5C, 0x10 },
        { 0x5D, 0x00 },
        { 0x5E, 0xF6 },
        { 0x5F, 0x45 },

        { 0x65, 0xE8 },
OOVPA_END;

#if 0 // No longer used, replaced by generic 4627 version
// ******************************************************************
// * XoUpdateLaunchNewImageInternal
// ******************************************************************
OOVPA_XREF(XoUpdateLaunchNewImageInternal, 4627, 8,

    XREF_XoUpdateLaunchNewImageInternal,
    XRefZero)

        { 0x1E, 0xDB },
        { 0x3E, 0x1B },
        { 0x5E, 0xF6 },
        { 0x81, 0x83 },
        { 0x9E, 0xE8 },
        { 0xBE, 0x50 },
        { 0xDE, 0xE8 },
        { 0xFE, 0x15 },
OOVPA_END;
#endif

// ******************************************************************
// * CXo::XOnlineLogon
// ******************************************************************
OOVPA_XREF(CXo_XOnlineLogon, 4627, 13,

    XREF_CXo_XOnlineLogon,
    XRefZero)

        { 0x02, 0xEC },

        { 0x17, 0x04 },
        { 0x1E, 0xE8 },

        { 0x3B, 0x8B },
        { 0x3C, 0x4D },
        { 0x3D, 0xFC },
        { 0x3E, 0x68 },
        { 0x3F, 0x58 },
        { 0x40, 0x4F },
        { 0x41, 0x4E },
        { 0x42, 0x73 },
        { 0x43, 0x68 },
        { 0x44, 0x68 },
OOVPA_END;
