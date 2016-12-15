// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->EmuKrnlLogging.cpp
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
#include <xboxkrnl/xboxkrnl.h>
};


//#include <cstdio>
//#include <cctype>
//#include <clocale>
//
//#include <windows.h> // for DWORD
//#include <sstream> // for std::stringstream
//#include <iostream> // For std::cout
//#include <iomanip> // For std::setw

#include "Logging.h"
#include "EmuKrnlLogging.h"

std::ostream& operator<<(std::ostream& os, const PULONG& value)
{
	os << hex4((uint32_t)value);
	if (value)
		os << " (*value: " << hex4(*value) << ")";

	return os;
}

// Macro's for Xbox Enum-ToString conversions :
#define ENUM2STR_START(EnumType) const char * EnumType##ToString(const xboxkrnl::EnumType value) { switch (value) {
#define ENUM2STR_CASE(a) case xboxkrnl::a: return #a;
#define ENUM2STR_END(EnumType) default: return "Unknown_"#EnumType; } }

// Xbox Enum-ToString conversions :

ENUM2STR_START(BUS_DATA_TYPE)
	ENUM2STR_CASE(ConfigurationSpaceUndefined)
	ENUM2STR_CASE(Cmos)
	ENUM2STR_CASE(EisaConfiguration)
	ENUM2STR_CASE(Pos)
	ENUM2STR_CASE(CbusConfiguration)
	ENUM2STR_CASE(PCIConfiguration)
	ENUM2STR_CASE(VMEConfiguration)
	ENUM2STR_CASE(NuBusConfiguration)
	ENUM2STR_CASE(PCMCIAConfiguration)
	ENUM2STR_CASE(MPIConfiguration)
	ENUM2STR_CASE(MPSAConfiguration)
	ENUM2STR_CASE(PNPISAConfiguration)
	ENUM2STR_CASE(SgiInternalConfiguration)
	ENUM2STR_CASE(MaximumBusDataType)
ENUM2STR_END(BUS_DATA_TYPE)

ENUM2STR_START(EEPROM_INDEX)
	ENUM2STR_CASE(EEPROM_LANGUAGE)
	ENUM2STR_CASE(EEPROM_VIDEO)
	ENUM2STR_CASE(EEPROM_AUDIO)
	ENUM2STR_CASE(EEPROM_P_CONTROL_GAMES)
	ENUM2STR_CASE(EEPROM_P_CONTROL_PASSWORD)
	ENUM2STR_CASE(EEPROM_P_CONTROL_MOVIES)
	ENUM2STR_CASE(EEPROM_ONLINE_IP_ADDRESS)
	ENUM2STR_CASE(EEPROM_ONLINE_DNS_ADDRESS)
	ENUM2STR_CASE(EEPROM_ONLINE_DEFAULT_GATEWAY_ADDRESS)
	ENUM2STR_CASE(EEPROM_ONLINE_SUBNET_ADDRESS)
	ENUM2STR_CASE(EEPROM_MISC)
	ENUM2STR_CASE(EEPROM_DVD_REGION)
	ENUM2STR_CASE(EEPROM_MAX_OS)
	ENUM2STR_CASE(EEPROM_FACTORY_SERIAL_NUMBER)
	ENUM2STR_CASE(EEPROM_FACTORY_ETHERNET_ADDR)
	ENUM2STR_CASE(EEPROM_FACTORY_ONLINE_KEY)
	ENUM2STR_CASE(EEPROM_FACTORY_AV_REGION)
	ENUM2STR_CASE(EEPROM_FACTORY_GAME_REGION)
	ENUM2STR_CASE(EEPROM_MAX_FACTORY)
	ENUM2STR_CASE(EEPROM_ENCRYPTED_SECTION)
	ENUM2STR_CASE(EEPROM_MAX_ALL)
ENUM2STR_END(EEPROM_INDEX)

ENUM2STR_START(EVENT_TYPE)
	ENUM2STR_CASE(NotificationEvent)
	ENUM2STR_CASE(SynchronizationEvent)
ENUM2STR_END(EVENT_TYPE)

ENUM2STR_START(FILE_INFORMATION_CLASS)
	ENUM2STR_CASE(FileDirectoryInformation)
	ENUM2STR_CASE(FileFullDirectoryInformation)
	ENUM2STR_CASE(FileBothDirectoryInformation)
	ENUM2STR_CASE(FileBasicInformation)
	ENUM2STR_CASE(FileStandardInformation)
	ENUM2STR_CASE(FileInternalInformation)
	ENUM2STR_CASE(FileEaInformation)
	ENUM2STR_CASE(FileAccessInformation)
	ENUM2STR_CASE(FileNameInformation)
	ENUM2STR_CASE(FileRenameInformation)
	ENUM2STR_CASE(FileLinkInformation)
	ENUM2STR_CASE(FileNamesInformation)
	ENUM2STR_CASE(FileDispositionInformation)
	ENUM2STR_CASE(FilePositionInformation)
	ENUM2STR_CASE(FileFullEaInformation)
	ENUM2STR_CASE(FileModeInformation)
	ENUM2STR_CASE(FileAlignmentInformation)
	ENUM2STR_CASE(FileAllInformation)
	ENUM2STR_CASE(FileAllocationInformation)
	ENUM2STR_CASE(FileEndOfFileInformation)
	ENUM2STR_CASE(FileAlternateNameInformation)
	ENUM2STR_CASE(FileStreamInformation)
	ENUM2STR_CASE(FilePipeInformation)
	ENUM2STR_CASE(FilePipeLocalInformation)
	ENUM2STR_CASE(FilePipeRemoteInformation)
	ENUM2STR_CASE(FileMailslotQueryInformation)
	ENUM2STR_CASE(FileMailslotSetInformation)
	ENUM2STR_CASE(FileCompressionInformation)
	ENUM2STR_CASE(FileCopyOnWriteInformation)
	ENUM2STR_CASE(FileCompletionInformation)
	ENUM2STR_CASE(FileMoveClusterInformation)
	ENUM2STR_CASE(FileQuotaInformation)
	ENUM2STR_CASE(FileReparsePointInformation)
	ENUM2STR_CASE(FileNetworkOpenInformation)
	ENUM2STR_CASE(FileObjectIdInformation)
	ENUM2STR_CASE(FileTrackingInformation)
	ENUM2STR_CASE(FileOleDirectoryInformation)
	ENUM2STR_CASE(FileContentIndexInformation)
	ENUM2STR_CASE(FileInheritContentIndexInformation)
	ENUM2STR_CASE(FileOleInformation)
	ENUM2STR_CASE(FileMaximumInformation)
ENUM2STR_END(FILE_INFORMATION_CLASS)

ENUM2STR_START(FS_INFORMATION_CLASS)
	ENUM2STR_CASE(FileFsVolumeInformation)
	ENUM2STR_CASE(FileFsLabelInformation)
	ENUM2STR_CASE(FileFsSizeInformation)
	ENUM2STR_CASE(FileFsDeviceInformation)
	ENUM2STR_CASE(FileFsAttributeInformation)
	ENUM2STR_CASE(FileFsControlInformation)
	ENUM2STR_CASE(FileFsFullSizeInformation)
	ENUM2STR_CASE(FileFsObjectIdInformation)
	ENUM2STR_CASE(FileFsMaximumInformation)
ENUM2STR_END(FS_INFORMATION_CLASS)

ENUM2STR_START(KINTERRUPT_MODE)
	ENUM2STR_CASE(LevelSensitive)
	ENUM2STR_CASE(Latched)
ENUM2STR_END(KINTERRUPT_MODE)

ENUM2STR_START(KWAIT_REASON)
	ENUM2STR_CASE(Executive)
	ENUM2STR_CASE(FreePage)
	ENUM2STR_CASE(PageIn)
	ENUM2STR_CASE(PoolAllocation)
	ENUM2STR_CASE(DelayExecution)
	ENUM2STR_CASE(Suspended)
	ENUM2STR_CASE(UserRequest)
	ENUM2STR_CASE(WrExecutive)
	ENUM2STR_CASE(WrFreePage)
	ENUM2STR_CASE(WrPageIn)
	ENUM2STR_CASE(WrPoolAllocation)
	ENUM2STR_CASE(WrDelayExecution)
	ENUM2STR_CASE(WrSuspended)
	ENUM2STR_CASE(WrUserRequest)
	ENUM2STR_CASE(WrEventPair)
	ENUM2STR_CASE(WrQueue)
	ENUM2STR_CASE(WrLpcReceive)
	ENUM2STR_CASE(WrLpcReply)
	ENUM2STR_CASE(WrVirtualMemory)
	ENUM2STR_CASE(WrPageOut)
	ENUM2STR_CASE(WrRendezvous)
	ENUM2STR_CASE(WrFsCacheIn)
	ENUM2STR_CASE(WrFsCacheOut)
	ENUM2STR_CASE(Spare4)
	ENUM2STR_CASE(Spare5)
	ENUM2STR_CASE(Spare6)
	ENUM2STR_CASE(WrKernel)
	ENUM2STR_CASE(MaximumWaitReason)
ENUM2STR_END(KWAIT_REASON)

ENUM2STR_START(KOBJECTS)
	ENUM2STR_CASE(DpcObject)
ENUM2STR_END(KOBJECTS)

ENUM2STR_START(MODE)
	ENUM2STR_CASE(KernelMode)
	ENUM2STR_CASE(UserMode)
	ENUM2STR_CASE(MaximumMode)
ENUM2STR_END(MODE)

ENUM2STR_START(RETURN_FIRMWARE)
	ENUM2STR_CASE(ReturnFirmwareHalt)
	ENUM2STR_CASE(ReturnFirmwareReboot)
	ENUM2STR_CASE(ReturnFirmwareQuickReboot)
	ENUM2STR_CASE(ReturnFirmwareHard)
	ENUM2STR_CASE(ReturnFirmwareFatal)
	ENUM2STR_CASE(ReturnFirmwareAll)
ENUM2STR_END(RETURN_FIRMWARE)

ENUM2STR_START(TIMER_TYPE)
	ENUM2STR_CASE(NotificationTimer)
	ENUM2STR_CASE(SynchronizationTimer)
ENUM2STR_END(TIMER_TYPE)

ENUM2STR_START(WAIT_TYPE)
	ENUM2STR_CASE(WaitAll)
	ENUM2STR_CASE(WaitAny)
ENUM2STR_END(WAIT_TYPE)

// Macro for implementation of rendering EnumToString :
#define LOGRENDER_ENUM(EnumType) LOGRENDER_HEADER(EnumType) \
{ return os << "("#EnumType") " << EnumType##ToString(value) << " = " << hex4((int)value); }

// Xbox enum renderers :
LOGRENDER_ENUM(BUS_DATA_TYPE)
LOGRENDER_ENUM(EEPROM_INDEX)
LOGRENDER_ENUM(EVENT_TYPE)
LOGRENDER_ENUM(FILE_INFORMATION_CLASS)
LOGRENDER_ENUM(FS_INFORMATION_CLASS)
LOGRENDER_ENUM(KINTERRUPT_MODE)
LOGRENDER_ENUM(KWAIT_REASON)
LOGRENDER_ENUM(KOBJECTS)
LOGRENDER_ENUM(MODE)
LOGRENDER_ENUM(RETURN_FIRMWARE)
LOGRENDER_ENUM(TIMER_TYPE)
LOGRENDER_ENUM(WAIT_TYPE)

#undef ENUM_START
#undef ENUM_CASE
#undef ENUM_END
#undef RENDER_ENUM

#define LOGRENDER_MEMBER(Member) << "\n   ."#Member": " << value.Member

LOGRENDER_HEADER(BOOLEAN)
{
	return os << (BOOL)value;
}

// Macro combining pointer-to-type implementation and type rendering header :
#define LOGRENDER_TYPE(Type)                         \
LOGRENDER_HEADER(P##Type)                            \
{                                                    \
	os << "0x" << (void*)value;                      \
	if (value)                                       \
		os << " -> P"#Type" {" << *value << "}";     \
                                                     \
	return os;                                       \
}                                                    \
                                                     \
LOGRENDER_HEADER(Type)                               \

// Render Xbox types :
LOGRENDER_TYPE(LARGE_INTEGER)
{
	return os 
		LOGRENDER_MEMBER(QuadPart);
}

LOGRENDER_TYPE(MM_STATISTICS)
{
	return os
		LOGRENDER_MEMBER(Length)
		LOGRENDER_MEMBER(TotalPhysicalPages)
		LOGRENDER_MEMBER(AvailablePages)
		LOGRENDER_MEMBER(VirtualMemoryBytesCommitted)
		LOGRENDER_MEMBER(VirtualMemoryBytesCommitted)
		LOGRENDER_MEMBER(VirtualMemoryBytesReserved)
		LOGRENDER_MEMBER(CachePagesCommitted)
		LOGRENDER_MEMBER(PoolPagesCommitted)
		LOGRENDER_MEMBER(StackPagesCommitted)
		LOGRENDER_MEMBER(ImagePagesCommitted);
}

LOGRENDER_TYPE(OBJECT_ATTRIBUTES)
{
	return os
		LOGRENDER_MEMBER(RootDirectory)
		LOGRENDER_MEMBER(ObjectName)
		LOGRENDER_MEMBER(Attributes);
}

LOGRENDER_TYPE(STRING)
{
	return os
		LOGRENDER_MEMBER(Length)
		LOGRENDER_MEMBER(MaximumLength)
		LOGRENDER_MEMBER(Buffer);
}

LOGRENDER_TYPE(UNICODE_STRING)
{
	return os
		LOGRENDER_MEMBER(Length)
		LOGRENDER_MEMBER(MaximumLength)
		LOGRENDER_MEMBER(Buffer);
}

#undef LOGRENDER_MEMBER
#undef LOGRENDER_TYPE
#undef LOGRENDER_HEADER
