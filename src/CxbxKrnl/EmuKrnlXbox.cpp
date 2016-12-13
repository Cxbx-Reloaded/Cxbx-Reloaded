// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlXbox.cpp
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For XboxEEPROMKey, etc.
};

// ******************************************************************
// * 0x0141 - XboxEEPROMKey
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(321) xboxkrnl::XBOX_KEY_DATA xboxkrnl::XboxEEPROMKey = {};

// ******************************************************************
// * 0x0142 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) xboxkrnl::XBOX_HARDWARE_INFO xboxkrnl::XboxHardwareInfo =
{
	0xC0000035,
	0,0,0,0
};

// ******************************************************************
// * 0x0143 - XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) xboxkrnl::UCHAR xboxkrnl::XboxHDKey[16] =
{
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

// ******************************************************************
// * 0x0144 - XboxKrnlVersion
// ******************************************************************
// Xbox Kernel version (part of kernel thunk table)
XBSYSAPI EXPORTNUM(324) xboxkrnl::XBOX_KRNL_VERSION xboxkrnl::XboxKrnlVersion =
{
	// Default Cxbx to emulating kernel version 5838 :
	1, 0, 5838, 1 // TODO : Make this configurable
};

// ******************************************************************
// * 0x0145 - XboxSignatureKey
// ******************************************************************
XBSYSAPI EXPORTNUM(325) xboxkrnl::BYTE xboxkrnl::XboxSignatureKey[16] =
{
	// cxbx default saved game key
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ******************************************************************
// * 0x0161 - XboxLANKey
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(353) xboxkrnl::XBOX_KEY_DATA xboxkrnl::XboxLANKey = {};

// ******************************************************************
// * 0x0162 - XboxAlternateSignatureKeys
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(354) xboxkrnl::XBOX_KEY_DATA xboxkrnl::XboxAlternateSignatureKeys = {};

