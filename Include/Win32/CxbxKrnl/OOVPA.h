// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;; 
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['  
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P    
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,  
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->OOVPA.h
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
#ifndef OOVPA_H
#define OOVPA_H

// ******************************************************************
// * struct: OVPL
// ******************************************************************
struct OVPL
{
    uint16 Offset;
    uint08 Value;
};

// ******************************************************************
// * struct: OVPS
// ******************************************************************
struct OVPS
{
    uint08 Offset;
    uint08 Value;
};

// ******************************************************************
// * struct: OVP
// ******************************************************************
struct OVP
{
};

// ******************************************************************
// * struct: OOVPA
// ******************************************************************
struct OOVPA
{
    uint16  Count : 15; // Number of Order,Value Pairs
    uint16  Large : 1;  // Large (16 bit) or Small (8 bit) offsets?

    union OVPU
    {
        OVPL ovpl[];
        OVPS ovps[];
    }
	ovpu;
};

#endif
