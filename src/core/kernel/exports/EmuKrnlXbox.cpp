// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************

#define LOG_PREFIX CXBXR_MODULE::XBOX


#include <core\kernel\exports\xboxkrnl.h> // For XboxEEPROMKey, etc.
#include "Logging.h"

// Certificate Key
// Not exported but used to generate per-title keys
xbox::XBOX_KEY_DATA xbox::XboxCertificateKey = { 0 };

// ******************************************************************
// * 0x0141 - XboxEEPROMKey
// ******************************************************************
// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(321) xbox::XBOX_KEY_DATA xbox::XboxEEPROMKey = { 0 };

// ******************************************************************
// * 0x0142 - XboxHardwareInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(322) xbox::XBOX_HARDWARE_INFO xbox::XboxHardwareInfo =
{
	0xC0000031, // Flags: 1=INTERNAL_USB, 2=DEVKIT, 4=MACROVISION, 8=CHIHIRO
	0xA2, // GpuRevision, byte read from NV2A first register, at 0xFD0000000 - see NV_PMC_BOOT_0
	0xD3, // McpRevision, Retail 1.6 - see https://github.com/JayFoxRox/xqemu-jfr/wiki/MCPX-and-bootloader
	0, // unknown
	0 // unknown
};

// ******************************************************************
// * 0x0143 - XboxHDKey
// ******************************************************************
XBSYSAPI EXPORTNUM(323) xbox::XBOX_KEY_DATA xbox::XboxHDKey = { 0 };

// ******************************************************************
// * 0x0144 - XboxKrnlVersion
// ******************************************************************
// Xbox Kernel version (part of kernel thunk table)
XBSYSAPI EXPORTNUM(324) xbox::XBOX_KRNL_VERSION xbox::XboxKrnlVersion =
{
	// Default Cxbx to emulating kernel version 5838 :
	1, 0, 5838, 1 // TODO : Make this configurable
};

// ******************************************************************
// * 0x0145 - XboxSignatureKey
// Generated in SetupPerTitleKeys() using the Certificate Key and the
// XBE's Signature Key
// ******************************************************************
XBSYSAPI EXPORTNUM(325) xbox::XBOX_KEY_DATA xbox::XboxSignatureKey = { 0 };

// ******************************************************************
// * 0x0161 - XboxLANKey
// Generated in SetupPerTitleKeys() using the Certificate Key and the
// XBE's LAN Key
// ******************************************************************
XBSYSAPI EXPORTNUM(353) xbox::XBOX_KEY_DATA xbox::XboxLANKey = { 0 };

// ******************************************************************
// * 0x0162 - XboxAlternateSignatureKeys
// Generated in SetupPerTitleKeys() using the Certificate Key and the
// XBE's Alternate Signature Keys Key
// ******************************************************************
XBSYSAPI EXPORTNUM(354) xbox::XBOX_KEY_DATA xbox::XboxAlternateSignatureKeys[ALTERNATE_SIGNATURE_COUNT] = { 0 };
