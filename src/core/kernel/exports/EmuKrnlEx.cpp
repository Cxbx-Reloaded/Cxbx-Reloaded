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

#define LOG_PREFIX CXBXR_MODULE::EX

// prevent name collisions
namespace xboxkrnl
{
	#include <xboxkrnl/xboxkrnl.h> // For ExAllocatePool, etc.
};

#include "Logging.h" // For LOG_FUNC()
#include "EmuEEPROM.h" // For EmuFindEEPROMInfo, EEPROM, XboxFactoryGameRegion
#include "EmuKrnlLogging.h"
#include "core\kernel\memory-manager\PoolManager.h"

// prevent name collisions
namespace NtDll
{
	#include "core\kernel\support\EmuNtDll.h" // For NtDelayExecution(), etc.
};

#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "EmuKrnl.h" // For InsertHeadList, InsertTailList, RemoveHeadList

#include <atomic> // for std::atomic
#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h> // For STATUS_BUFFER_TOO_SMALL
#pragma warning(default:4005)

static CRITICAL_SECTION eeprom_crit_section;

static PCRITICAL_SECTION get_eeprom_crit_section()
{
    static PCRITICAL_SECTION crit_sec_ptr = nullptr;
    if(crit_sec_ptr == nullptr) {
        crit_sec_ptr = &eeprom_crit_section;
		InitializeCriticalSectionAndSpinCount(crit_sec_ptr, 0x400);
    }
    return crit_sec_ptr;
}

void LockEeprom()
{
	EnterCriticalSection(get_eeprom_crit_section());
}

void UnlockEeprom()
{
	LeaveCriticalSection(get_eeprom_crit_section());
}

static bool section_does_not_require_checksum(
    xboxkrnl::XC_VALUE_INDEX index
)
{
    switch(index) {
        case xboxkrnl::XC_FACTORY_GAME_REGION:
            return true;
        case xboxkrnl::XC_ENCRYPTED_SECTION:
            return true;
        case xboxkrnl::XC_MAX_ALL:
            return true;
        default:
            return false;
    }
}

static uint32_t eeprom_section_checksum(
    const uint32_t* section_data,
    uint32_t section_data_length
)
{
    const uint32_t bitmask = 0xFFFFFFFF;
    const uint32_t num_dwords = section_data_length >> 2;
    uint64_t checksum = 0;
    uint32_t carry_count = 0;

    for(uint32_t loop_count = num_dwords; loop_count > 0; loop_count--) {
        checksum += *section_data;
        if(checksum > bitmask) {
            carry_count++;
            checksum &= bitmask;
        }
        section_data++;
    }
    checksum += carry_count;
    if(checksum > bitmask) {
        checksum += 1;
    }
    return (uint32_t)(checksum & bitmask);
}

static bool eeprom_data_is_valid(xboxkrnl::XC_VALUE_INDEX index)
{
    const ULONG valid_checksum = 0xFFFFFFFF;
    const uint32_t* UserSettings_data = (uint32_t*)&EEPROM->UserSettings;
    const uint32_t* FactorySettings_data = (uint32_t*)&EEPROM->FactorySettings;
    ULONG checksum = 0;

    if(section_does_not_require_checksum(index)) {
        return true;
    }

    if((index >= xboxkrnl::XC_TIMEZONE_BIAS) && (index <= xboxkrnl::XC_MAX_OS)) {
        checksum = eeprom_section_checksum(UserSettings_data, sizeof(EEPROM->UserSettings));
    }
    else if((index >= xboxkrnl::XC_FACTORY_START_INDEX) && (index <= xboxkrnl::XC_MAX_FACTORY)) {
        checksum = eeprom_section_checksum(FactorySettings_data, sizeof(EEPROM->FactorySettings));
    }
    else {
        EmuLog(LOG_LEVEL::WARNING, "Eeprom ValueIndex 0x%X does not have a checksum\n", index);
    }
    return checksum == valid_checksum;
}

// ******************************************************************
// * 0x000C - ExAcquireReadWriteLockExclusive()
// ******************************************************************
// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(12) xboxkrnl::VOID NTAPI xboxkrnl::ExAcquireReadWriteLockExclusive
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	bool interrupt_mode = DisableInterrupts();
	ReadWriteLock->LockCount++;
	if (ReadWriteLock->LockCount != 0) {
		ReadWriteLock->WritersWaitingCount++;
		RestoreInterruptMode(interrupt_mode);
		KeWaitForSingleObject(
			&ReadWriteLock->WriterEvent,
			Executive,
			0,
			0,
			0
		);
	}
	else {
		RestoreInterruptMode(interrupt_mode);
	}
}

// ******************************************************************
// * 0x000D - ExAcquireReadWriteLockShared()
// ******************************************************************
// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(13) xboxkrnl::VOID NTAPI xboxkrnl::ExAcquireReadWriteLockShared
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	bool interrupt_mode = DisableInterrupts();
	bool must_wait_on_active_write = ReadWriteLock->ReadersEntryCount == 0;
	bool must_wait_on_queued_write = (ReadWriteLock->ReadersEntryCount != 0) && (ReadWriteLock->WritersWaitingCount != 0);
	bool must_wait = must_wait_on_active_write || must_wait_on_queued_write;
	ReadWriteLock->LockCount++;
	if ((ReadWriteLock->LockCount != 0) && must_wait) {
		ReadWriteLock->ReadersWaitingCount++;
		RestoreInterruptMode(interrupt_mode);
#if 0 //FIXME - Enable once KeReleaseSempahore is implemented (used in ExFreeReadWriteLock for Sharedlocks).
		KeWaitForSingleObject(
			&ReadWriteLock->ReaderSemaphore,
			Executive,
			0,
			0,
			0
		);
#endif
	}
	else {
		ReadWriteLock->ReadersEntryCount++;
		RestoreInterruptMode(interrupt_mode);
	}
}

// ******************************************************************
// * 0x000E - ExAllocatePool()
// ******************************************************************
XBSYSAPI EXPORTNUM(14) xboxkrnl::PVOID NTAPI xboxkrnl::ExAllocatePool
(
	IN SIZE_T NumberOfBytes
)
{
	LOG_FORWARD("ExAllocatePoolWithTag");

	return ExAllocatePoolWithTag(NumberOfBytes, 'enoN'); // = "None" in reverse
}

// ******************************************************************
// * 0x000F - ExAllocatePoolWithTag()
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

	PVOID pRet = reinterpret_cast<PVOID>(g_PoolManager.AllocatePool(NumberOfBytes, Tag));

	RETURN(pRet);
}

// ******************************************************************
// * 0x0010 - ExEventObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(16) xboxkrnl::OBJECT_TYPE xboxkrnl::ExEventObjectType =
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL,
	NULL,
	(PVOID)offsetof(xboxkrnl::KEVENT, Header),
	'vevE' // = first four characters of "Event" in reverse
};

// ******************************************************************
// * 0x0011 - ExFreePool()
// ******************************************************************
XBSYSAPI EXPORTNUM(17) xboxkrnl::VOID NTAPI xboxkrnl::ExFreePool
(
	IN PVOID	P
)
{
	LOG_FUNC_ONE_ARG(P);

	g_PoolManager.DeallocatePool(reinterpret_cast<VAddr>(P));
}

// ******************************************************************
// * 0x0012 - ExInitializeReadWriteLock()
// ******************************************************************
// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(18) xboxkrnl::VOID NTAPI xboxkrnl::ExInitializeReadWriteLock
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	ReadWriteLock->LockCount = -1;
	ReadWriteLock->WritersWaitingCount = 0;
	ReadWriteLock->ReadersWaitingCount = 0;
	ReadWriteLock->ReadersEntryCount = 0;
	KeInitializeEvent(&ReadWriteLock->WriterEvent, SynchronizationEvent, FALSE);
	KeInitializeSemaphore(&ReadWriteLock->ReaderSemaphore, 0, MAXLONG);
}

// ******************************************************************
// * 0x0013 - ExInterlockedAddLargeInteger()
// ******************************************************************
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

// ******************************************************************
// * 0x0014 - ExInterlockedAddLargeStatistic()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(20) xboxkrnl::VOID FASTCALL xboxkrnl::ExInterlockedAddLargeStatistic
(
	IN PLARGE_INTEGER Addend,
	IN ULONG Increment
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Addend)
		LOG_FUNC_ARG(Increment)
		LOG_FUNC_END;

	std::atomic<LONGLONG> Target(Addend->QuadPart);
	Target.fetch_add(Increment);
}

// ******************************************************************
// * 0x0015 - ExInterlockedCompareExchange64()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(21) xboxkrnl::LONGLONG FASTCALL xboxkrnl::ExInterlockedCompareExchange64
(
	IN OUT PLONGLONG Destination,
	IN PLONGLONG Exchange,
	IN PLONGLONG Comparand
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_OUT(Destination)
		LOG_FUNC_ARG(Exchange)
		LOG_FUNC_ARG(Comparand)
		LOG_FUNC_END;

	std::atomic<LONGLONG> Target(*Destination);

	LONGLONG Result = *Comparand;
	Target.compare_exchange_strong(Result, *Exchange);

	RETURN(Result);
}

// ******************************************************************
// * 0x0016 - ExMutantObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(22) xboxkrnl::OBJECT_TYPE xboxkrnl::ExMutantObjectType = 
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL, // TODO : xboxkrnl::ExpDeleteMutant,
	NULL,
	(PVOID)offsetof(xboxkrnl::KMUTANT, Header),
	'atuM' // = first four characters of "Mutant" in reverse
};

// ******************************************************************
// * 0x0017 - ExQueryPoolBlockSize()
// ******************************************************************
XBSYSAPI EXPORTNUM(23) xboxkrnl::ULONG NTAPI xboxkrnl::ExQueryPoolBlockSize
(
	IN PVOID PoolBlock
)
{
	LOG_FUNC_ONE_ARG(PoolBlock);

	ULONG ret = g_PoolManager.QueryPoolSize(reinterpret_cast<VAddr>(PoolBlock));

	RETURN(ret);
}

// ******************************************************************
// * 0x0018 - ExQueryNonVolatileSetting()
// ******************************************************************
XBSYSAPI EXPORTNUM(24) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExQueryNonVolatileSetting
(
	IN  DWORD   ValueIndex,
	OUT DWORD   *Type,
	OUT PVOID   Value,
	IN  SIZE_T  ValueLength,
	OUT PSIZE_T ResultLength OPTIONAL
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(XC_VALUE_INDEX, ValueIndex)
		LOG_FUNC_ARG_OUT(Type)
		LOG_FUNC_ARG_OUT(Value)
		LOG_FUNC_ARG(ValueLength)
		LOG_FUNC_ARG_OUT(ResultLength)
	LOG_FUNC_END;

	NTSTATUS Status = STATUS_SUCCESS;
	void * value_addr = nullptr;
	int value_type;
	int result_length;
	xboxkrnl::XC_VALUE_INDEX index = (XC_VALUE_INDEX)ValueIndex;

	// handle eeprom read
	if (index == XC_FACTORY_GAME_REGION) {
		value_addr = &XboxFactoryGameRegion;
		value_type = REG_DWORD;
		result_length = sizeof(ULONG);
	}
	else {
		const EEPROMInfo* info = EmuFindEEPROMInfo(index);
		if (info != nullptr) {
			value_addr = (void *)((PBYTE)EEPROM + info->value_offset);
			value_type = info->value_type;
			result_length = info->value_length;
		}
	}

	if (value_addr != nullptr) {
		if (ResultLength != nullptr) {
			*ResultLength = result_length;
		}

		if ((int)ValueLength >= result_length) {
			// FIXME - Entering the critical region causes an exception because
			// the current thread returns as 0.
			// We temporarily bypass the problem of above we a host critical section
			//RtlEnterCriticalSectionAndRegion(get_eeprom_crit_section());
			LockEeprom();
			if(eeprom_data_is_valid(index)) {
				// Set the output value type :
				*Type = value_type;
				// Clear the output value buffer :
				memset(Value, 0, ValueLength);
				// Copy the emulated EEPROM value into the output value buffer :
				memcpy(Value, value_addr, result_length);
			}
			else {
				Status = STATUS_DEVICE_DATA_ERROR;
			}
			//RtlLeaveCriticalSectionAndRegion(get_eeprom_crit_section());
			UnlockEeprom();
		}
		else {
			Status = STATUS_BUFFER_TOO_SMALL;
		}
	}
	else {
		Status = STATUS_OBJECT_NAME_NOT_FOUND;
	}

	RETURN(Status);
}

// ******************************************************************
// * 0x0019 - ExReadWriteRefurbInfo()
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
			RtlInitAnsiString(&FileName, PCSZ(PAnsiChar(DeviceHarddisk0Partition0)));

			OBJECT_ATTRIBUTES ObjectAttributes;
			InitializeObjectAttributes(&ObjectAttributes, &FileName, OBJ_CASE_INSENSITIVE, 0, NULL);

			Handle ConfigPartitionHandle;
			IO_STATUS_BLOCK IoStatusBlock;
			Result = xboxkrnl::NtOpenFile(
				&ConfigPartitionHandle,
				GENERIC_READ or DWORD(iif(aIsWriteMode, GENERIC_WRITE, 0)) or SYNCHRONIZE,
				&ObjectAttributes,
				&IoStatusBlock,
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
					Result = xboxkrnl::NtWriteFile(ConfigPartitionHandle, 0, NULL, NULL, &IoStatusBlock, &RefurbInfoCopy, XBOX_HD_SECTOR_SIZE, &ByteOffset);
				}
				else
				{
					Result = xboxkrnl::NtReadFile(ConfigPartitionHandle, 0, NULL, NULL, &IoStatusBlock, &RefurbInfoCopy, XBOX_HD_SECTOR_SIZE, &ByteOffset);
					if (NT_SUCCESS(Result)) 
					{
						if (RefurbInfoCopy.Signature_ == XBOX_REFURB_INFO_SIGNATURE)
							// No signature - clear output buffer :
							ZeroMemory(pRefurbInfo, SizeOf(XBOX_REFURB_INFO))
						else
							CopyMem(pRefurbInfo, RefurbInfoCopy, SizeOf(XBOX_REFURB_INFO));
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

// ******************************************************************
// * 0x001A - ExRaiseException()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(26) xboxkrnl::VOID NTAPI xboxkrnl::ExRaiseException
(
	IN PEXCEPTION_RECORD ExceptionRecord
)
{
	LOG_FUNC_ONE_ARG(ExceptionRecord);

	// RtlRaiseException(ExceptionRecord);
	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x001B - ExRaiseStatus()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(27) xboxkrnl::VOID NTAPI xboxkrnl::ExRaiseStatus
(
	IN NTSTATUS Status
)
{
	LOG_FUNC_ONE_ARG(Status);

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x001C - ExReleaseReadWriteLock()
// ******************************************************************
// Source:APILogger - Uncertain
XBSYSAPI EXPORTNUM(28) xboxkrnl::VOID NTAPI xboxkrnl::ExReleaseReadWriteLock
(
	IN PERWLOCK ReadWriteLock
)
{
	LOG_FUNC_ONE_ARG(ReadWriteLock);

	bool interrupt_mode = DisableInterrupts();
	ReadWriteLock->LockCount--;
	if (ReadWriteLock->LockCount == -1) {
		ReadWriteLock->ReadersEntryCount = 0;
		RestoreInterruptMode(interrupt_mode);
		return;
	}

	if (ReadWriteLock->ReadersEntryCount == 0) {
		if (ReadWriteLock->ReadersWaitingCount != 0) {
			ULONG orig_readers_waiting = ReadWriteLock->ReadersWaitingCount;
			ReadWriteLock->ReadersEntryCount = ReadWriteLock->ReadersWaitingCount;
			ReadWriteLock->ReadersWaitingCount = 0;
			RestoreInterruptMode(interrupt_mode);
			KeReleaseSemaphore(&ReadWriteLock->ReaderSemaphore, 1, (LONG)orig_readers_waiting, 0);
			return;
		}
	}
	else {
		ReadWriteLock->ReadersEntryCount--;
		if (ReadWriteLock->ReadersEntryCount != 0) {
			RestoreInterruptMode(interrupt_mode);
			return;
		}
	}

	ReadWriteLock->WritersWaitingCount--;
	RestoreInterruptMode(interrupt_mode);
	KeSetEvent(&ReadWriteLock->WriterEvent, 1, 0);
}

// ******************************************************************
// * 0x001D - ExSaveNonVolatileSetting()
// ******************************************************************
XBSYSAPI EXPORTNUM(29) xboxkrnl::NTSTATUS NTAPI xboxkrnl::ExSaveNonVolatileSetting
(
	IN  DWORD			   ValueIndex,
	IN  DWORD			   Type,
	IN  PVOID			   Value,
	IN  SIZE_T			   ValueLength
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG_TYPE(XC_VALUE_INDEX, ValueIndex)
		LOG_FUNC_ARG(Type) // unused (see Note below)
		LOG_FUNC_ARG(Value)
		LOG_FUNC_ARG(ValueLength)
	LOG_FUNC_END;

	NTSTATUS Status = STATUS_SUCCESS;
	void * value_addr = nullptr;
	DWORD result_length;

	// Don't allow writing to the eeprom encrypted area
	if (ValueIndex == XC_ENCRYPTED_SECTION)
		RETURN(STATUS_OBJECT_NAME_NOT_FOUND);

	// handle eeprom write
	if (g_bIsDebug || ValueIndex <= XC_MAX_OS || ValueIndex > XC_MAX_FACTORY)
	{
		const EEPROMInfo* info = EmuFindEEPROMInfo((XC_VALUE_INDEX)ValueIndex);
		if (info != nullptr) {
			value_addr = (void *)((PBYTE)EEPROM + info->value_offset);
			result_length = info->value_length;
		};
	}

	if (value_addr != nullptr) {
		// Note : Type could be verified against info->value_type here, but the orginal kernel doesn't even bother
		if (ValueLength <= result_length) {
			// FIXME - Entering the critical region causes an exception because
			// the current thread returns as 0.
			// We temporarily bypass the problem of above we a host critical section
			//RtlEnterCriticalSectionAndRegion(get_eeprom_crit_section());
			LockEeprom();

			// Clear the emulated EEMPROM value :
			memset(value_addr, 0, result_length);
			// Copy the input value buffer into the emulated EEPROM value :
			memcpy(value_addr, Value, ValueLength);

			if (ValueIndex == XC_FACTORY_GAME_REGION)
			{
				// Update the global XboxFactoryGameRegion

				XboxFactoryGameRegion = *(xboxkrnl::PULONG)Value;
			}

			gen_section_CRCs(EEPROM);
			//RtlLeaveCriticalSectionAndRegion(get_eeprom_crit_section());
			UnlockEeprom();
		}
		else {
			Status = STATUS_INVALID_PARAMETER;
		}
	}
	else {
		Status = STATUS_OBJECT_NAME_NOT_FOUND;
	}

	RETURN(Status);
}

// ******************************************************************
// * 0x001E - ExSemaphoreObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(30) xboxkrnl::OBJECT_TYPE xboxkrnl::ExSemaphoreObjectType = 
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL,
	NULL,
	(PVOID)offsetof(xboxkrnl::KSEMAPHORE, Header),
	'ameS' // = first four characters of "Semaphore" in reverse
};

// ******************************************************************
// * 0x001F - ExTimerObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(31) xboxkrnl::OBJECT_TYPE xboxkrnl::ExTimerObjectType = 
{
	xboxkrnl::ExAllocatePoolWithTag,
	xboxkrnl::ExFreePool,
	NULL,
	NULL, // TODO : xboxkrnl::ExpDeleteTimer,
	NULL,
	(PVOID)offsetof(xboxkrnl::KTIMER, Header),
	'emiT' // = first four characters of "Timer" in reverse
};

// ******************************************************************
// * 0x0020 - ExfInterlockedInsertHeadList()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(32) xboxkrnl::PLIST_ENTRY FASTCALL xboxkrnl::ExfInterlockedInsertHeadList
(
	IN PLIST_ENTRY ListHead,
	IN PLIST_ENTRY ListEntry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_ARG(ListEntry)
		LOG_FUNC_END;

	/* Disable interrupts and acquire the spinlock */
	// BOOLEAN Enable = _ExiDisableInteruptsAndAcquireSpinlock(Lock);
	LOG_INCOMPLETE(); // TODO : Lock

	/* Save the first entry */
	PLIST_ENTRY FirstEntry = ListHead->Flink;
	/* Insert the new entry */
	InsertHeadList(ListHead, ListEntry);

	/* Release the spinlock and restore interrupts */
	// _ExiReleaseSpinLockAndRestoreInterupts(Lock, Enable);

	/* Return the old first entry or NULL for empty list */
	if (FirstEntry == ListHead)
		FirstEntry = NULL;

	RETURN(FirstEntry);
}

// ******************************************************************
// * 0x0021 - ExfInterlockedInsertTailList()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(33) xboxkrnl::PLIST_ENTRY FASTCALL xboxkrnl::ExfInterlockedInsertTailList
(
	IN PLIST_ENTRY ListHead,	
	IN PLIST_ENTRY ListEntry
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ListHead)
		LOG_FUNC_ARG(ListEntry)
		LOG_FUNC_END;

	/* Disable interrupts and acquire the spinlock */
	// BOOLEAN Enable = _ExiDisableInteruptsAndAcquireSpinlock(Lock);
	LOG_INCOMPLETE(); // TODO : Lock

	/* Save the last entry */
	PLIST_ENTRY LastEntry = ListHead->Blink;

	/* Insert the new entry */
	InsertTailList(ListHead, ListEntry);

	/* Release the spinlock and restore interrupts */
	// _ExiReleaseSpinLockAndRestoreInterupts(Lock, Enable);

	/* Return the old last entry or NULL for empty list */
	if (LastEntry == ListHead) 
		LastEntry = NULL;

	RETURN(LastEntry);
}

// ******************************************************************
// * 0x0022 - ExfInterlockedRemoveHeadList()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(34) xboxkrnl::PLIST_ENTRY FASTCALL xboxkrnl::ExfInterlockedRemoveHeadList
(
	IN PLIST_ENTRY ListHead
)
{
	LOG_FUNC_ONE_ARG(ListHead);

	/* Disable interrupts and acquire the spinlock */
	// BOOLEAN Enable = _ExiDisableInteruptsAndAcquireSpinlock(Lock);
	LOG_INCOMPLETE(); // TODO : Lock

	PLIST_ENTRY ListEntry;

	/* Check if the list is empty */
	if (IsListEmpty(ListHead))
	{
		/* Return NULL */
		ListEntry = NULL;
	}
	else
	{
		/* Remove the first entry from the list head */
		ListEntry = RemoveHeadList(ListHead);
#if DBG
		ListEntry->Flink = (PLIST_ENTRY)0xBADDD0FF;
		ListEntry->Blink = (PLIST_ENTRY)0xBADDD0FF;
#endif
	}

	/* Release the spinlock and restore interrupts */
	// _ExiReleaseSpinLockAndRestoreInterupts(Lock, Enable);

	/* Return the entry */
	RETURN(ListEntry);

}
