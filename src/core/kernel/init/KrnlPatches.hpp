// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *  This file is part of the Cxbx-Reloaded project.
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
#pragma once

// NOTE: This file is intended to be used only with Cxbxkrnl.cpp file.
// NOTE2: If need to extern function/variable, please do in Cxbxkrnl.h file.

static void CxbxrKrnlSetupDummyHeader() {
	// TODO : The following seems to cause a crash when booting the game "Forza Motorsport",
	// according to https://github.com/Cxbx-Reloaded/Cxbx-Reloaded/issues/101#issuecomment-277230140

	// Create a fake kernel header for XapiRestrictCodeSelectorLimit
	// Thanks advancingdragon / DirtBox
	PDUMMY_KERNEL DummyKernel = (PDUMMY_KERNEL)XBOX_KERNEL_BASE;
	memset(DummyKernel, 0, sizeof(DUMMY_KERNEL));

	// XapiRestrictCodeSelectorLimit only checks these fields.
	DummyKernel->DosHeader.e_lfanew = sizeof(IMAGE_DOS_HEADER); // RVA of NtHeaders
	DummyKernel->FileHeader.SizeOfOptionalHeader = 0;
	DummyKernel->FileHeader.NumberOfSections = 1;
	// as long as this doesn't start with "INIT"
	strncpy_s((PSTR)DummyKernel->SectionHeader.Name, 8, "DONGS", 8);
	EmuLogInit(LOG_LEVEL::INFO, "Initialized dummy kernel image header.");
}

// TODO: If possible, maybe make this as optional patch when kernel emulation is fully done.
static void ApplyMediaPatches()
{
	// Patch the XBE Header to allow running from all media types
	g_pCertificate->dwAllowedMedia |= 0
		| XBEIMAGE_MEDIA_TYPE_HARD_DISK
		| XBEIMAGE_MEDIA_TYPE_DVD_X2
		| XBEIMAGE_MEDIA_TYPE_DVD_CD
		| XBEIMAGE_MEDIA_TYPE_CD
		| XBEIMAGE_MEDIA_TYPE_DVD_5_RO
		| XBEIMAGE_MEDIA_TYPE_DVD_9_RO
		| XBEIMAGE_MEDIA_TYPE_DVD_5_RW
		| XBEIMAGE_MEDIA_TYPE_DVD_9_RW
		;
	// Patch the XBE Header to allow running on all regions
	g_pCertificate->dwGameRegion = 0
		| XBEIMAGE_GAME_REGION_MANUFACTURING
		| XBEIMAGE_GAME_REGION_NA
		| XBEIMAGE_GAME_REGION_JAPAN
		| XBEIMAGE_GAME_REGION_RESTOFWORLD
		;
	// Patch the XBE Security Flag
	// This field is only present if the Xbe Size is >= than our Certificate Structure
	// This works as our structure is large enough to fit the newer certificate size,
	// while dwSize is the actual size of the certificate in the Xbe.
	// Source: Various Hacked Kernels
	if (g_pCertificate->dwSize >= sizeof(Xbe::Certificate)) {
		g_pCertificate->dwSecurityFlags &= ~1;
	}
}
