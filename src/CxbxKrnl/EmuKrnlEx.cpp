// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlEx.cpp
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
#include <xboxkrnl/xboxkrnl.h> // For ExAllocatePool, etc.
};

#include "Logging.h" // For LOG_FUNC()

#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuAlloc.h" // For CxbxFree(), CxbxMalloc(), etc.

using namespace xboxkrnl;

// ******************************************************************
// * 0x000E ExAllocatePool
// ******************************************************************
XBSYSAPI EXPORTNUM(14) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePool
(
	IN ULONG NumberOfBytes
)
{
	LOG_FUNC_ONE_ARG(NumberOfBytes);

	PVOID pRet = ExAllocatePoolWithTag(NumberOfBytes, (ULONG)"enoN"); // "None" in reverse?

	RETURN(pRet);
}

// ******************************************************************
// * 0x000F ExAllocatePoolWithTag
// ******************************************************************
// * Differences from NT: There is no PoolType field, as the XBOX
// * only has 1 pool, the non-paged pool.
// ******************************************************************
XBSYSAPI EXPORTNUM(15) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePoolWithTag
(
	IN SIZE_T NumberOfBytes,
	IN ULONG Tag
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(NumberOfBytes)
		LOG_FUNC_ARG(Tag)
		LOG_FUNC_END;

	// TODO: Actually implement this
	PVOID pRet = CxbxMalloc(NumberOfBytes);

	RETURN(pRet);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(16) xboxkrnl::POBJECT_TYPE xboxkrnl::ExEventObjectType = NULL;

// ******************************************************************
// * 0x0011 ExFreePool
// ******************************************************************
XBSYSAPI EXPORTNUM(17) VOID NTAPI xboxkrnl::ExFreePool
(
	IN PVOID	P
)
{
	LOG_FUNC_ONE_ARG(P);

	CxbxFree(P);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(22) xboxkrnl::POBJECT_TYPE xboxkrnl::ExMutantObjectType = NULL;

// ******************************************************************
// * 0x0017 ExQueryPoolBlockSize
// ******************************************************************
XBSYSAPI EXPORTNUM(23) xboxkrnl::ULONG NTAPI xboxkrnl::ExQueryPoolBlockSize
(
	IN PVOID PoolBlock
)
{
	LOG_FUNC_ONE_ARG(PoolBlock);

	// Not strictly correct, but it will do for now
	ULONG ret = MmQueryAllocationSize(PoolBlock);

	RETURN(ret);
}

// ******************************************************************
// * 0x0018 ExQueryNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(24) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExQueryNonVolatileSetting
(
	IN  EEPROM_INDEX        ValueIndex,
	OUT DWORD              *Type,
	OUT PUCHAR              Value,
	IN  SIZE_T              ValueLength,
	OUT PSIZE_T             ResultLength OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ValueIndex)
		LOG_FUNC_ARG_OUT(Type)
		LOG_FUNC_ARG_OUT(Value)
		LOG_FUNC_ARG(ValueLength)
		LOG_FUNC_ARG_OUT(ResultLength)
		LOG_FUNC_END;

	if (!Type || !Value)
		CxbxKrnlCleanup("Assertion in ExQueryNonVolatileSetting()");

	NTSTATUS ret = STATUS_SUCCESS;

	// handle eeprom read
	switch (ValueIndex)
	{
		// Factory Game Region
	case EEPROM_FACTORY_GAME_REGION:
	{
		// TODO: configurable region or autodetect of some sort
		if (ResultLength != 0)
			*ResultLength = 0x04;

		if (ValueLength >= 4) {
			*Type = 0x04;
			*Value = 0x01;  // North America
		}
	}
	break;

	// Factory AV Region
	case EEPROM_FACTORY_AV_REGION:
	{
		// TODO: configurable region or autodetect of some sort
		if (ResultLength != 0)
			*ResultLength = 0x04;

		*Type = 0x04;
		*Value = 0x01; // NTSC_M
	}
	break;

	// Language
	case EEPROM_LANGUAGE:
	{
		// TODO: configurable language or autodetect of some sort
		if (ResultLength != 0)
			*ResultLength = 0x04;

		*Type = 0x04;
		*Value = 0x01;  // English
	}
	break;

	// Video Flag
	case EEPROM_VIDEO:
	{
		// TODO: configurable video flags or autodetect of some sort
		if (ResultLength != 0)
			*ResultLength = 0x04;

		*Type = 0x04;
		*Value = 0x10;  // Letterbox
	}
	break;

	// Audio Flags
	case EEPROM_AUDIO:
	{
		if (ResultLength != 0)
			*ResultLength = 0x04;

		*Type = 0x04;
		*Value = 0;  // Stereo, no AC3, no DTS
	}
	break;

	case EEPROM_MISC:
	{
		if (ResultLength != 0)
			*ResultLength = 0x04;

		*Type = 0x04;
		*Value = 0;  // No automatic power down
	}
	break;

	case EEPROM_MAX_OS:
	{
		// This is called to return a complete XBOX_USER_SETTINGS structure
		//
		// One example is from XapipQueryTimeZoneInformation(), where it is used to
		// detect the local timezone information.

		// TODO
	}
	break;

	default:
		EmuWarning("ExQueryNonVolatileSetting unknown ValueIndex (%d)", ValueIndex);
		ret = STATUS_OBJECT_NAME_NOT_FOUND;
		break;
	}

	RETURN(ret);
}

// ******************************************************************
// * 0x0019 - ExReadWriteRefurbInfo
// ******************************************************************
XBSYSAPI EXPORTNUM(25) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExReadWriteRefurbInfo
(
	PVOID	Unknown1,
	DWORD	Unknown2,
	DWORD	Unknown3
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Unknown1)
		LOG_FUNC_ARG(Unknown2)
		LOG_FUNC_ARG(Unknown3)
		LOG_FUNC_END;

	// TODO: What does this do?
	LOG_IGNORED();

	RETURN(STATUS_SUCCESS);
}

// ******************************************************************
// * 0x001D - ExSaveNonVolatileSetting
// ******************************************************************
XBSYSAPI EXPORTNUM(29) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExSaveNonVolatileSetting
(
	IN  DWORD               ValueIndex,
	OUT DWORD              *Type,
	IN  PUCHAR              Value,
	IN  SIZE_T              ValueLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ValueIndex)
		LOG_FUNC_ARG_OUT(Type)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(ValueLength)
		LOG_FUNC_END;

	// TODO: Later.

	RETURN(STATUS_SUCCESS);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(30) xboxkrnl::POBJECT_TYPE xboxkrnl::ExSemaphoreObjectType = NULL;

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(31) xboxkrnl::POBJECT_TYPE xboxkrnl::ExTimerObjectType = NULL;

