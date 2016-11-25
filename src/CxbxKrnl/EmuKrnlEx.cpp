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

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(12) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExAcquireReadWriteLockExclusive
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	// KeWaitForSingleObject
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(13) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExAcquireReadWriteLockShared
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	// KeWaitForSingleObject
	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// ******************************************************************
// * 0x000E ExAllocatePool
// ******************************************************************
XBSYSAPI EXPORTNUM(14) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePool
(
	IN SIZE_T NumberOfBytes
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

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(18) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExInitializeReadWriteLock
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
}

// Source:ReactOS https://doxygen.reactos.org/d0/d35/ntoskrnl_2ex_2interlocked_8c_source.html#l00062
XBSYSAPI EXPORTNUM(19) xboxkrnl::LARGE_INTEGER NTAPI xboxkrnl::ExInterlockedAddLargeInteger
(
	IN OUT PLARGE_INTEGER Addend,
	IN LARGE_INTEGER Increment,
	IN OUT PKSPIN_LOCK Lock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Addend)
// TODO : operator<<(LARGE_INTERGER) enables 		LOG_FUNC_ARG(Increment)
		LOG_FUNC_ARG(Lock)
		LOG_FUNC_END;

	LARGE_INTEGER OldValue;
// TODO :	BOOLEAN Enable;

	/* Disable interrupts and acquire the spinlock */
// TODO :	Enable = _ExiDisableInterruptsAndAcquireSpinlock(Lock);

	/* Save the old value */
	OldValue.QuadPart = Addend->QuadPart;

	/* Do the operation */
	Addend->QuadPart += Increment.QuadPart;

	/* Release the spinlock and restore interrupts */
	// TODO :	_ExiReleaseSpinLockAndRestoreInterrupts(Lock, Enable);

	/* Return the old value */
	return OldValue; // TODO : operator<<(LARGE_INTERGER) enables RETURN(OldValue);
}

// Source:ReactOS
XBSYSAPI EXPORTNUM(20) VOID __fastcall xboxkrnl::ExInterlockedAddLargeStatistic
(
	PLARGE_INTEGER Addend,
	ULONG Increment
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Addend)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
}

// Source:ReactOS
XBSYSAPI EXPORTNUM(21) xboxkrnl::LONGLONG __fastcall xboxkrnl::ExInterlockedCompareExchange64
(
	OUT PLONGLONG Destination,
	PLONGLONG Exchange,
	PLONGLONG Comparand,
	PKSPIN_LOCK Lock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Destination)
		LOG_FUNC_ARG(Exchange)
		LOG_FUNC_ARG(Comparand)
		LOG_FUNC_ARG(Lock)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(0);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(22) xboxkrnl::POBJECT_TYPE xboxkrnl::ExMutantObjectType = NULL;

// ******************************************************************
// * 0x0017 ExQueryPoolBlockSize
// ******************************************************************
XBSYSAPI EXPORTNUM(23) xboxkrnl::ULONG NTAPI xboxkrnl::ExQueryPoolBlockSize
(
	IN PVOID PoolBlock
	// TODO : Add 'OUT PBOOLEAN QuotaCharged' ?
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
	IN OUT PXBOX_REFURB_INFO	pRefurbInfo,
	IN ULONG	dwBufferSize,
	IN BOOLEAN	bIsWriteMode
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pRefurbInfo)
		LOG_FUNC_ARG(dwBufferSize)
		LOG_FUNC_ARG(bIsWriteMode)
		LOG_FUNC_END;

	NTSTATUS Result = STATUS_SUCCESS;

/* TODO: Port this Dxbx code :
	if (pRefurbInfo)
	{
		if dwBufferSize <> SizeOf(XBOX_REFURB_INFO) then
			Result = STATUS_INVALID_PARAMETER
		else
		{
			// Open partition 0 directly :
			_STRING FileName;
			RtlInitAnsiString(@FileName, PCSZ(PAnsiChar(DeviceHarddisk0Partition0)));

			OBJECT_ATTRIBUTES ObjectAttributes;
			InitializeObjectAttributes(@ObjectAttributes, @FileName, OBJ_CASE_INSENSITIVE, 0, NULL);

			Handle ConfigPartitionHandle;
			IO_STATUS_BLOCK IoStatusBlock;
			Result = xboxkrnl_NtOpenFile(
				@ConfigPartitionHandle,
				GENERIC_READ or DWORD(iif(aIsWriteMode, GENERIC_WRITE, 0)) or SYNCHRONIZE,
				@ObjectAttributes,
				@IoStatusBlock,
				FILE_SHARE_READ or FILE_SHARE_WRITE,
				FILE_SYNCHRONOUS_IO_ALERT);
			if (NT_SUCCESS(Result))
			{
				LARGE_INTEGER ByteOffset;
				ByteOffset.QuadPart = XBOX_REFURB_INFO_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

				XBOX_REFURB_INFO RefurbInfoCopy;
				if (bIsWriteMode)
				{
					RefurbInfoCopy = *pRefurbInfo;
					RefurbInfoCopy.Signature_ = XBOX_REFURB_INFO_SIGNATURE;
					Result = xboxkrnl_NtWriteFile(ConfigPartitionHandle, 0, NULL, NULL, @IoStatusBlock, @RefurbInfoCopy, XBOX_HD_SECTOR_SIZE, @ByteOffset);
				}
				else
				{
					Result = xboxkrnl::NtReadFile(ConfigPartitionHandle, 0, NULL, NULL, @IoStatusBlock, @RefurbInfoCopy, XBOX_HD_SECTOR_SIZE, @ByteOffset);
					if (NT_SUCCESS(Result)) 
					{
						if (RefurbInfoCopy.Signature_ == XBOX_REFURB_INFO_SIGNATURE)
							// No signature - clear output buffer :
							ZeroMemory(pRefurbInfo, SizeOf(XBOX_REFURB_INFO))
						else
							*pRefurbInfo = RefurbInfoCopy;
					}
				}

				NtClose(ConfigPartitionHandle);
			}
		}
	}
	else
		Result = STATUS_UNSUCCESSFUL; // This may never happen!
*/

	LOG_IGNORED();

	RETURN(Result);
}

// Source:ReactOS
XBSYSAPI EXPORTNUM(26) VOID NTAPI xboxkrnl::ExRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
)
{
	LOG_FUNC_ONE_ARG(ExceptionRecord);

	// RtlRaiseException(ExceptionRecord);
	LOG_UNIMPLEMENTED();
}


// Source:ReactOS
XBSYSAPI EXPORTNUM(27) VOID NTAPI xboxkrnl::ExRaiseStatus
(
	IN NTSTATUS Status
)
{
	LOG_FUNC_ONE_ARG(Status);

	LOG_UNIMPLEMENTED();
}

// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(28) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExReleaseReadWriteLock
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	LOG_UNIMPLEMENTED();

	RETURN(S_OK);
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
	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
}

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(30) xboxkrnl::POBJECT_TYPE xboxkrnl::ExSemaphoreObjectType = NULL;

// TODO : What should we initialize this to?
XBSYSAPI EXPORTNUM(31) xboxkrnl::POBJECT_TYPE xboxkrnl::ExTimerObjectType = NULL;

// Source:ReactOS
XBSYSAPI EXPORTNUM(32) xboxkrnl::PLIST_ENTRY __fastcall xboxkrnl::ExfInterlockedInsertHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_ARG(ListEntry)
		LOG_FUNC_ARG(Lock)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(ListHead);
}


// Source:ReactOS
XBSYSAPI EXPORTNUM(33) xboxkrnl::PLIST_ENTRY __fastcall xboxkrnl::ExfInterlockedInsertTailList
(
	IN PLIST_ENTRY ListHead,	
	IN PLIST_ENTRY ListEntry,
	IN PKSPIN_LOCK Lock
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_ARG(ListEntry)
		LOG_FUNC_ARG(Lock)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(ListHead);
}


// Source:ReactOS
XBSYSAPI EXPORTNUM(34) xboxkrnl::PLIST_ENTRY __fastcall xboxkrnl::ExfInterlockedRemoveHeadList
(
	IN PKSPIN_LOCK Lock,
	IN PLIST_ENTRY ListHead
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Lock)
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(ListHead);
}
