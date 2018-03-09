// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlAv.cpp
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
#define _XBOXKRNL_DEFEXTRN_

#define LOG_PREFIX "KRNL"

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For AvGetSavedDataAddress, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "EmuShared.h" // For g_EmuShared

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

#include "Emu.h" // For EmuWarning()


// Global Variable(s)
PVOID g_pPersistedData = NULL; // (?? What is this used for?)

ULONG AvQueryAvCapabilities()
{
	// This is the only AV mode we currently emulate, so we can hardcode the return value
	// TODO: Once we add the ability to change av pack, read HalSmcVideoMode) and convert it to a AV_PACK_*
	ULONG avpack = AV_PACK_HDTV;
	ULONG type;
	ULONG resultSize;

	// First, read the factory AV settings
	ULONG avRegion;
	NTSTATUS result = xboxkrnl::ExQueryNonVolatileSetting(
		xboxkrnl::XC_FACTORY_AV_REGION,
		&type,
		&avRegion,
		sizeof(ULONG),
		&resultSize);

	// If this failed, default to AV_STANDARD_NTSC_M | AV_FLAGS_60Hz
	if (result != STATUS_SUCCESS || resultSize != sizeof(ULONG)) {
		avRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
	}

	// Read the user-configurable (via the dashboard) settings
	ULONG userSettings;
	result = xboxkrnl::ExQueryNonVolatileSetting(
		xboxkrnl::XC_VIDEO,
		&type,
		&userSettings,
		sizeof(ULONG),
		&resultSize);

	// If this failed, default to no user-options set
	if (result != STATUS_SUCCESS || resultSize != sizeof(ULONG)) {
		userSettings = 0;
	}

	return avpack | (avRegion & (AV_STANDARD_MASK | AV_REFRESH_MASK)) | (userSettings & ~(AV_STANDARD_MASK | AV_PACK_MASK));
}

// ******************************************************************
// * 0x0001 - AvGetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(1) xboxkrnl::PVOID NTAPI xboxkrnl::AvGetSavedDataAddress(void)
{
	LOG_FUNC();

	RETURN(AvSavedDataAddress);
}

// ******************************************************************
// * 0x0002 - AvSendTVEncoderOption()
// ******************************************************************
XBSYSAPI EXPORTNUM(2) xboxkrnl::VOID NTAPI xboxkrnl::AvSendTVEncoderOption
(
	IN  PVOID   RegisterBase,
	IN  ULONG   Option,
	IN  ULONG   Param,
	OUT ULONG   *Result
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(RegisterBase)
		LOG_FUNC_ARG(Option)
		LOG_FUNC_ARG(Param)
		LOG_FUNC_ARG_OUT(Result)
		LOG_FUNC_END;

	//if (RegisterBase == NULL)
	//	RegisterBase = (void *)NV20_REG_BASE_KERNEL;

	switch (Option) {
	case AV_OPTION_MACROVISION_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ENABLE_CC:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_DISABLE_CC:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_SEND_CC_DATA:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_CC_STATUS:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_AV_CAPABILITIES:
		*Result = AvQueryAvCapabilities();
		break;
	case AV_OPTION_BLANK_SCREEN:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_MACROVISION_COMMIT:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_FLICKER_FILTER:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ZERO_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_QUERY_MODE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_ENABLE_LUMA_FILTER:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_GUESS_FIELD:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_ENCODER_TYPE:
		LOG_UNIMPLEMENTED();
		break;
	case AV_QUERY_MODE_TABLE_VERSION:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_CGMS:
		LOG_UNIMPLEMENTED();
		break;
	case AV_OPTION_WIDESCREEN:
		LOG_UNIMPLEMENTED();
		break;
	default:
		// do nothing
		break;
	}
}

// ******************************************************************
// * 0x0003 - AvSetDisplayMode()
// ******************************************************************
XBSYSAPI EXPORTNUM(3) xboxkrnl::ULONG NTAPI xboxkrnl::AvSetDisplayMode
(
	IN  PVOID   RegisterBase,
	IN  ULONG   Step,
	IN  ULONG   Mode,
	IN  ULONG   Format,
	IN  ULONG   Pitch,
	IN  ULONG   FrameBuffer
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(RegisterBase)
		LOG_FUNC_ARG(Step)
		LOG_FUNC_ARG(Mode)
		LOG_FUNC_ARG(Format)
		LOG_FUNC_ARG(Pitch)
		LOG_FUNC_ARG(FrameBuffer)
		LOG_FUNC_END;

	ULONG result = S_OK;

	LOG_UNIMPLEMENTED();

	RETURN(result);
}

// ******************************************************************
// * 0x0004 - AvSetSavedDataAddress()
// ******************************************************************
XBSYSAPI EXPORTNUM(4) xboxkrnl::VOID NTAPI xboxkrnl::AvSetSavedDataAddress
(
	IN  PVOID   Address
)
{
	LOG_FUNC_ONE_ARG(Address);

	AvSavedDataAddress = Address;

	// NOTE: it shouldn't be needed to call MmPersistContiguousMemory since it should have been called by other routines
	// on the supplied address

	if (Address)
	{
		g_EmuShared->SetFrameBufferAddress((uintptr_t*)Address);
		DbgPrintf("KNRL: Persisting FrameBuffer\n");
	}
	else
	{
		uintptr_t* pTemp = NULL;
		g_EmuShared->SetFrameBufferAddress(pTemp);
		DbgPrintf("KNRL: Forgetting FrameBuffer\n");
	}
}