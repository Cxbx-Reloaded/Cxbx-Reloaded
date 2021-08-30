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
