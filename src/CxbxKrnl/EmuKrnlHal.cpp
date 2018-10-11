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
// *   Cxbx->Win32->CxbxKrnl->EmuKrnlHal.cpp
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

#define LOG_PREFIX CXBXR_MODULE::HAL

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For HalReadSMCTrayState, etc.
};

#include <Shlwapi.h> // For PathRemoveFileSpec()
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup, CxbxConvertArgToString, and CxbxExec
#include "Emu.h" // For EmuLog(LOG_PREFIX, LOG_LEVEL::WARNING, )
#include "EmuKrnl.h"
#include "EmuX86.h" // HalReadWritePciSpace needs this
#include "EmuShared.h"
#include "EmuFile.h" // For FindNtSymbolicLinkObjectByDriveLetter
#include "Common\EmuEEPROM.h" // For EEPROM
#include "devices\Xbox.h" // For g_SMBus, SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER
#include "devices\SMCDevice.h" // For SMC_COMMAND_SCRATCH

#include <algorithm> // for std::replace
#include <locale>
#include <codecvt>

// See the links below for the details about the kernel structure LIST_ENTRY and the related functions
// https://www.codeproject.com/Articles/800404/Understanding-LIST-ENTRY-Lists-and-Its-Importance
// https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/singly-and-doubly-linked-lists
#define LIST_ENTRY_DEFINE_HEAD(ListHead) xboxkrnl::LIST_ENTRY (ListHead) = { &(ListHead), &(ListHead) }
#define LIST_ENTRY_ACCESS_RECORD(address, type, field) \
((type*)((UCHAR*)(address) - (ULONG)(&((type*)0)->field)))


volatile DWORD HalInterruptRequestRegister = APC_LEVEL | DISPATCH_LEVEL;
HalSystemInterrupt HalSystemInterrupts[MAX_BUS_INTERRUPT_LEVEL + 1];

// variables used by the SMC to know a reset / shutdown is pending
uint8_t ResetOrShutdownCommandCode = 0;
uint32_t ResetOrShutdownDataValue = 0;

// global list of routines executed during a reboot
LIST_ENTRY_DEFINE_HEAD(ShutdownRoutineList);

// ******************************************************************
// * Declaring this in a header causes errors with xboxkrnl
// * namespace, so we must declare it within any file that uses it
// ******************************************************************
xboxkrnl::KPCR* WINAPI KeGetPcr();

// ******************************************************************
// * 0x0009 - HalReadSMCTrayState()
// ******************************************************************
XBSYSAPI EXPORTNUM(9) xboxkrnl::VOID NTAPI xboxkrnl::HalReadSMCTrayState
(
	DWORD*	State,
	DWORD*	Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

#define TRAY_CLOSED_MEDIA_PRESENT 96
#define TRAY_CLOSED_NO_MEDIA 64
#define TRAY_OPEN 16

	// TODO: Make this configurable?
	// TODO: What is the count parameter for??

	if (Count)
		*Count = 1;

	// Pretend the tray is open
	// TRAY_CLOSED_NO_MEDIA causes Dashboard to call DeviceIoControl, which we do not implement
	// TRAY_CLOSED_MEDIA_PRESENT causes Dashboard to attempt to launch media, causing errors.
	*State = TRAY_OPEN;

	//	*Count = 1;
}

// ******************************************************************
// * 0x0026 - HalClearSoftwareInterrupt()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(38) xboxkrnl::VOID FASTCALL xboxkrnl::HalClearSoftwareInterrupt
(
	KIRQL Request
)
{
	LOG_FUNC_ONE_ARG_TYPE(KIRQL_TYPE, Request);

	// Mask out this interrupt request
	DWORD InterruptMask = 1 << Request;
	HalInterruptRequestRegister &= ~InterruptMask;
}

// ******************************************************************
// * 0x0027 - HalDisableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(39) xboxkrnl::VOID NTAPI xboxkrnl::HalDisableSystemInterrupt
(
	IN ULONG BusInterruptLevel
)
{
	LOG_FUNC_ONE_ARG(BusInterruptLevel);

	HalSystemInterrupts[BusInterruptLevel].Disable();
}

// ******************************************************************
// * 0x0028 - HalDiskCachePartitionCount
// ******************************************************************
// This specifies the number of Cache partitions available for game data caching
// On real hardware, there are three, generally known as X, Y and Z in homebrew
XBSYSAPI EXPORTNUM(40) xboxkrnl::ULONG xboxkrnl::HalDiskCachePartitionCount = 3; 

// ******************************************************************
// * 0x0029 - HalDiskModelNumber
// ******************************************************************
// Source:OpenXDK  TODO : Fill this with something sensible
XBSYSAPI EXPORTNUM(41) xboxkrnl::PANSI_STRING xboxkrnl::HalDiskModelNumber = 0;

// ******************************************************************
// * 0x002A - HalDiskSerialNumber
// ******************************************************************
// Source:OpenXDK  TODO : Fill this with something sensible
XBSYSAPI EXPORTNUM(42) xboxkrnl::PANSI_STRING xboxkrnl::HalDiskSerialNumber = 0;	

// ******************************************************************
// * 0x002B - HalEnableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(43) xboxkrnl::VOID NTAPI xboxkrnl::HalEnableSystemInterrupt
(
	IN ULONG BusInterruptLevel,
	IN KINTERRUPT_MODE InterruptMode
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BusInterruptLevel)
		LOG_FUNC_ARG(InterruptMode)
		LOG_FUNC_END;

	HalSystemInterrupts[BusInterruptLevel].Enable();
	HalSystemInterrupts[BusInterruptLevel].SetInterruptMode(InterruptMode);
}

#ifdef _DEBUG_TRACE
// Source : Xbox Linux
char *IRQNames[MAX_BUS_INTERRUPT_LEVEL + 1] =
{
	"<unknown>",
	"USB0", // IRQ 1 USB Controller: nVidia Corporation nForce USB Controller (rev d4) (prog-if 10 [OHCI])
	"<unknown>",
	"GPU", // IRQ 3 VGA compatible controller: nVidia Corporation: Unknown device 02a0 (rev a1) (prog-if 00 [VGA])
	"NIC", // Network Interface Card IRQ 4 Ethernet controller: nVidia Corporation nForce Ethernet Controller (rev d2)
	"APU", // IRQ 5 APU
	"ACI", // IRQ 6 Multimedia audio controller: nVidia Corporation nForce Audio (rev d2)
	"<unknown>",
	"<unknown>",
	"USB1", // IRQ 9 USB Controller : nVidia Corporation nForce USB Controller(rev d4) (prog - if 10[OHCI])
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"IDE", // IRQ 14
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>",
	"<unknown>"
};
#endif

// ******************************************************************
// * 0x002C - HalGetInterruptVector()
// ******************************************************************
XBSYSAPI EXPORTNUM(44) xboxkrnl::ULONG NTAPI xboxkrnl::HalGetInterruptVector
(
	IN ULONG   BusInterruptLevel,
	OUT PKIRQL  Irql
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BusInterruptLevel)
		LOG_FUNC_ARG_OUT(Irql)
		LOG_FUNC_END;

	// Note : blueshogun added this HalGetInterruptVector mock-up for
	// Virtua Cop 3 (Chihiro) and assumed Xbox games need not emulate this.
	// However, even something as simple as the OpenXDK XInput library uses this,
	// PLUS Cxbx will execute it's own preemptive thread-switching, after which
	// interrupt handling must be implememented too, including this API.

	ULONG dwVector = 0;

	if((BusInterruptLevel >=0) && (BusInterruptLevel <= MAX_BUS_INTERRUPT_LEVEL))
	{
		dwVector = IRQ2VECTOR(BusInterruptLevel);
		if(Irql)
			*Irql = (KIRQL)VECTOR2IRQL(dwVector);

#ifdef _DEBUG_TRACE
		DBG_PRINTF("HalGetInterruptVector(): Interrupt vector requested for %d (%s)\n", 
			BusInterruptLevel, IRQNames[BusInterruptLevel]);
#endif
	}

	RETURN(dwVector);
}

// ******************************************************************
// * 0x002D - HalReadSMBusValue()
// ******************************************************************
XBSYSAPI EXPORTNUM(45) xboxkrnl::NTSTATUS NTAPI xboxkrnl::HalReadSMBusValue
(
	IN  UCHAR               Address,
	IN  UCHAR               Command,
	IN  BOOLEAN             ReadWord,
	OUT PULONG              DataValue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(Command)
		LOG_FUNC_ARG(ReadWord)
		LOG_FUNC_ARG_OUT(DataValue)
		LOG_FUNC_END;

	// TODO : Prevent interrupts

	NTSTATUS Status = STATUS_SUCCESS;

	// ergo720: the or 1 on the address is necessary because I have seen that UnleashX and RDX dashboard pass 0x20 instead of the
	// expected 0x21 to this function when reading cpu and m/b temperatures

	g_SMBus->IOWrite(1, SMB_HOST_ADDRESS, Address | 1);
	g_SMBus->IOWrite(1, SMB_HOST_COMMAND, Command);
	if (ReadWord)
		g_SMBus->IOWrite(1, SMB_GLOBAL_ENABLE, AMD756_WORD_DATA | GE_HOST_STC);
	else
		g_SMBus->IOWrite(1, SMB_GLOBAL_ENABLE, AMD756_BYTE_DATA | GE_HOST_STC);
	// Note : GE_HOST_STC triggers ExecuteTransaction, which reads the command from the specified address

	// Check if the command was executed successfully
	if (g_SMBus->IORead(1, SMB_GLOBAL_STATUS) & GS_PRERR_STS) {
		Status = STATUS_UNSUCCESSFUL;
	}
	else {
		*DataValue = g_SMBus->IORead(1, SMB_HOST_DATA);
		if (ReadWord)
			*DataValue |= g_SMBus->IORead(1, SMB_HOST_DATA + 1) << 8;
	}

	// TODO : Reenable interrupts

	RETURN(Status);
}

// ******************************************************************
// * 0x002E - HalReadWritePCISpace()
// ******************************************************************
// Source:OpenXDK
XBSYSAPI EXPORTNUM(46) xboxkrnl::VOID NTAPI xboxkrnl::HalReadWritePCISpace
(
	IN ULONG   BusNumber,
	IN ULONG   SlotNumber,
	IN ULONG   RegisterNumber,
	IN PVOID   Buffer,
	IN ULONG   Length,
	IN BOOLEAN WritePCISpace
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(BusNumber)
		LOG_FUNC_ARG(SlotNumber)
		LOG_FUNC_ARG(RegisterNumber)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Length)
		LOG_FUNC_ARG(WritePCISpace)
		LOG_FUNC_END;

	// TODO: Disable Interrupt Processing
	
	PCI_SLOT_NUMBER PCISlotNumber;
	PCI_TYPE1_CFG_BITS CfgBits;

	PCISlotNumber.u.AsULONG = SlotNumber;
	CfgBits.u.AsULONG = 0;
	CfgBits.u.bits.BusNumber = BusNumber;
	CfgBits.u.bits.DeviceNumber = PCISlotNumber.u.bits.DeviceNumber;
	CfgBits.u.bits.FunctionNumber = PCISlotNumber.u.bits.FunctionNumber;
	CfgBits.u.bits.Enable = 1;

	const int B = sizeof(uint8_t); // Byte
	const int W = sizeof(uint16_t); // Word
	const int L = sizeof(uint32_t); // Long
	const UCHAR RegisterDataSizes[4][4] = {
		{L, B, W, W},
		{B, B, B, B},
		{W, B, W, W},
		{B, B, B, B}
	};

	while (Length > 0) {
		int ByteOffset = RegisterNumber % sizeof(ULONG);
		int Size = RegisterDataSizes[RegisterNumber % sizeof(ULONG)][Length % sizeof(ULONG)];

		EmuX86_IOWrite((xbaddr)PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG, sizeof(uint32_t));

		if (WritePCISpace) {
			EmuX86_IOWrite(PCI_TYPE1_DATA_PORT, *((PUCHAR)Buffer), Size);
		} else {
			uint32_t value = EmuX86_IORead(PCI_TYPE1_DATA_PORT, Size);
			// TODO : Could memcpy(Buffer, &value, Size) the following (for all endianesses)?
			switch (Size) {
			case sizeof(uint8_t): // Byte
				*((PUCHAR)Buffer) = value;
				break;
			case sizeof(uint16_t): // Word
				*((PUSHORT)Buffer) = value;
				break;
			case sizeof(uint32_t): // Long
				*((PULONG)Buffer) = value;
				break;
			}
		}
		
		RegisterNumber += Size;
		Buffer = (PUCHAR)Buffer + Size;
		Length -= Size;
	}

	// TODO: Enable Interrupt Processing
}

// ******************************************************************
// * 0x002F - HalRegisterShutdownNotification()
// ******************************************************************
XBSYSAPI EXPORTNUM(47) xboxkrnl::VOID NTAPI xboxkrnl::HalRegisterShutdownNotification
(
	IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
	IN BOOLEAN Register
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(ShutdownRegistration)
		LOG_FUNC_ARG(Register)
	LOG_FUNC_END;

	PLIST_ENTRY ListEntry;
	KIRQL OldIrql;

	OldIrql = KeRaiseIrqlToDpcLevel();

	if (Register)
	{
		ListEntry = ShutdownRoutineList.Flink;
		while (ListEntry != &ShutdownRoutineList)
		{
			if (ShutdownRegistration->Priority > LIST_ENTRY_ACCESS_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry)->Priority)
			{
				InsertTailList(ListEntry, &ShutdownRegistration->ListEntry);
				break;
			}
			ListEntry = ListEntry->Flink;
		}

		if (ListEntry == &ShutdownRoutineList)
		{
			InsertTailList(ListEntry, &ShutdownRegistration->ListEntry);
		}
	}
	else
	{
		ListEntry = ShutdownRoutineList.Flink;
		while (ListEntry != &ShutdownRoutineList)
		{
			if (ShutdownRegistration == LIST_ENTRY_ACCESS_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry))
			{
				RemoveEntryList(&ShutdownRegistration->ListEntry);
				break;
			}
			ListEntry = ListEntry->Flink;
		}
	}

	KfLowerIrql(OldIrql);
}

// Table of Software Interrupt Priority Valu	es
uint8_t SoftwareInterruptLookupTable[] = {
	0, 0, 1, 1, 2, 2, 2, 2
};

// ******************************************************************
// * 0x0030 - HalRequestSoftwareInterrupt()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(48) xboxkrnl::VOID FASTCALL xboxkrnl::HalRequestSoftwareInterrupt
(
	IN KIRQL Request
)
{
	LOG_FUNC_ONE_ARG_TYPE(KIRQL_TYPE, Request);

	DWORD InterruptMask = 1 << Request;
	bool interrupt_flag = DisableInterrupts();

	// Set this interrupt request bit:
	HalInterruptRequestRegister |= InterruptMask;

	// Get current IRQL
	PKPCR Pcr = KeGetPcr();
	KIRQL CurrentIrql = (KIRQL)Pcr->Irql;

	// Get pending Software Interrupts (by masking of the HW interrupt bits)
	uint8_t SoftwareInterrupt = HalInterruptRequestRegister & 3;

	// Get the highest pending software interrupt level
	KIRQL SoftwareIrql = SoftwareInterruptLookupTable[SoftwareInterrupt];
	
	if (SoftwareIrql > CurrentIrql) {
		// TODO: This is not completely correct, but it fixes an issue where DPCQueue's weren't running
		CallSoftwareInterrupt(Request);
	}

	RestoreInterruptMode(interrupt_flag);
}

// ******************************************************************
// * 0x0031 - HalReturnToFirmware()
// ******************************************************************
XBSYSAPI EXPORTNUM(49) xboxkrnl::VOID DECLSPEC_NORETURN NTAPI xboxkrnl::HalReturnToFirmware
(
	RETURN_FIRMWARE Routine
)
{
	LOG_FUNC_ONE_ARG(Routine);

	switch (Routine) {
	case ReturnFirmwareHalt:
		CxbxKrnlCleanup(LOG_PREFIX, "Emulated Xbox is halted");
		break;

	case ReturnFirmwareReboot:
		LOG_UNIMPLEMENTED(); // fall through
	case ReturnFirmwareQuickReboot:
	{
		if (xboxkrnl::LaunchDataPage == NULL)
			LOG_UNIMPLEMENTED();
		else
		{
			// Save the launch data page to disk for later.
			// (Note : XWriteTitleInfoNoReboot does this too)
			// Commented out because XLaunchNewImage is disabled!
			// MmPersistContiguousMemory((PVOID)xboxkrnl::LaunchDataPage, sizeof(LAUNCH_DATA_PAGE), TRUE);

			{
				// ergo720: I tested this with Tenchu and Dead or Alive Ultimate, both of which register a single shutdown
				// routine with HalRegisterShutdownNotification. The routines are correctly registered but when invoked they
				// cause a crash. It's because these routines are registered by and act upon the Xbox hardware, but most of
				// it is not LLEd enough and so, until then, we don't try to execute the shutdown routines

				#if 0
				KIRQL OldIrql;
				PLIST_ENTRY ListEntry;
				PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration;

				while (true)
				{
					OldIrql = KeRaiseIrqlToDpcLevel();

					ListEntry = RemoveHeadList(&ShutdownRoutineList);

					KfLowerIrql(OldIrql);

					if (ListEntry == &ShutdownRoutineList)
						break;

					ShutdownRegistration = LIST_ENTRY_ACCESS_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry);
					ShutdownRegistration->NotificationRoutine(ShutdownRegistration);
				}
				#endif
			}


			std::string TitlePath = xboxkrnl::LaunchDataPage->Header.szLaunchPath;
			char szWorkingDirectoy[MAX_PATH];

			// If the title path starts with a semicolon, remove it
			if (TitlePath.length() > 0 && TitlePath[0] == ';') {
				TitlePath.erase(0, 1);
			}

			// If the title path was an empty string, we need to launch the dashboard
			if (TitlePath.length() == 0) {
				TitlePath = DeviceHarddisk0Partition2 + "\\xboxdash.xbe";
			}

			std::string XbePath = TitlePath;
			// Convert Xbox XBE Path to Windows Path
			{
				HANDLE rootDirectoryHandle = nullptr;
				std::wstring wXbePath;
				// We pretend to come from NtCreateFile to force symbolic link resolution
				CxbxConvertFilePath(TitlePath, wXbePath, &rootDirectoryHandle, "NtCreateFile");

				// Convert Wide String as returned by above to a string, for XbePath
				XbePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wXbePath);

				// If the rootDirectoryHandle is not null, we have a relative path
				// We need to prepend the path of the root directory to get a full DOS path
				if (rootDirectoryHandle != nullptr) {
					char directoryPathBuffer[MAX_PATH];
					GetFinalPathNameByHandle(rootDirectoryHandle, directoryPathBuffer, MAX_PATH, VOLUME_NAME_DOS);
					XbePath = directoryPathBuffer + std::string("\\") + XbePath;

					// Trim \\?\ from the output string, as we want the raw DOS path, not NT path
					// We can do this always because GetFinalPathNameByHandle ALWAYS returns this format
					// Without exception
					XbePath.erase(0, 4);
				}
			}

			// Determine Working Directory
			{
				strncpy_s(szWorkingDirectoy, XbePath.c_str(), MAX_PATH);
				PathRemoveFileSpec(szWorkingDirectoy);
			}

			// Relaunch Cxbx, to load another Xbe
			{
				bool bMultiXbe = true;
				int QuickReboot;
				g_EmuShared->GetBootFlags(&QuickReboot);
				QuickReboot |= BOOT_QUICK_REBOOT;
				g_EmuShared->SetBootFlags(&QuickReboot);

				// Some titles (Xbox Dashboard and retail/demo discs) use ";" as a current directory path seperator
				// This process is handled during initialization. No speical handling here required.

				std::string szProcArgsBuffer;
				CxbxConvertArgToString(szProcArgsBuffer, szFilePath_CxbxReloaded_Exe, XbePath.c_str(), CxbxKrnl_hEmuParent, CxbxKrnl_DebugMode, CxbxKrnl_DebugFileName.c_str());

				if (!CxbxExec(szProcArgsBuffer, nullptr, false)) {
					CxbxKrnlCleanup(LOG_PREFIX, "Could not launch %s", XbePath.c_str());
				}
			}
		}
		break;
	};

	case ReturnFirmwareHard:
		LOG_UNIMPLEMENTED();
		break;

	case ReturnFirmwareFatal:
	{
		// NOTE: the error code is displayed by ExDisplayFatalError by other code paths so we need to change our corresponding
		// paths if we want to emulate all the possible fatal errors

		xboxkrnl::HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_SCRATCH, 0, SMC_SCRATCH_DISPLAY_FATAL_ERROR);

		std::string szProcArgsBuffer;
		CxbxConvertArgToString(szProcArgsBuffer, szFilePath_CxbxReloaded_Exe, szFilePath_Xbe, CxbxKrnl_hEmuParent, CxbxKrnl_DebugMode, CxbxKrnl_DebugFileName.c_str());

		if (!CxbxExec(szProcArgsBuffer, nullptr, false)) {
			CxbxKrnlCleanup(LOG_PREFIX, "Could not launch %s", szFilePath_Xbe);
		}
		break;
	}

	case ReturnFirmwareAll:
		LOG_UNIMPLEMENTED();
		break;

	default:
		LOG_UNIMPLEMENTED();
	}

	EmuShared::Cleanup();
	ExitProcess(EXIT_SUCCESS);
}

// ******************************************************************
// * 0x0032 - HalWriteSMBusValue()
// ******************************************************************
XBSYSAPI EXPORTNUM(50) xboxkrnl::NTSTATUS NTAPI xboxkrnl::HalWriteSMBusValue
(
	IN  UCHAR               Address,
	IN  UCHAR               Command,
	IN  BOOLEAN             WriteWord,
	IN  ULONG               DataValue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(Command)
		LOG_FUNC_ARG(WriteWord)
		LOG_FUNC_ARG(DataValue)
		LOG_FUNC_END;

	// TODO : Prevent interrupts

	NTSTATUS Status = STATUS_SUCCESS;

	g_SMBus->IOWrite(1, SMB_HOST_ADDRESS, Address);
	g_SMBus->IOWrite(1, SMB_HOST_COMMAND, Command);
	g_SMBus->IOWrite(1, SMB_HOST_DATA, DataValue & 0xFF);
	if (WriteWord) {
		g_SMBus->IOWrite(1, SMB_HOST_DATA + 1, (DataValue >> 8) & 0xFF);
		g_SMBus->IOWrite(1, SMB_GLOBAL_ENABLE, AMD756_WORD_DATA | GE_HOST_STC);
	}
	else
		g_SMBus->IOWrite(1, SMB_GLOBAL_ENABLE, AMD756_BYTE_DATA | GE_HOST_STC);
		// Note : GE_HOST_STC triggers ExecuteTransaction, which writes the command to the specified address

	// Check if the command was executed successfully
	if (g_SMBus->IORead(1, SMB_GLOBAL_STATUS) & GS_PRERR_STS) {
		Status = STATUS_UNSUCCESSFUL;
	}

	// TODO : Reenable interrupts

	RETURN(Status);
}

// ******************************************************************
// * 0x0149 - READ_PORT_BUFFER_UCHAR()
// ******************************************************************
XBSYSAPI EXPORTNUM(329) xboxkrnl::VOID NTAPI xboxkrnl::READ_PORT_BUFFER_UCHAR
(
	IN PUCHAR Port,
	IN PUCHAR Buffer,
	IN ULONG  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = (uint8_t)EmuX86_IORead((xbaddr)Port, sizeof(uint8_t));
}

// ******************************************************************
// * 0x014A - READ_PORT_BUFFER_USHORT()
// ******************************************************************
XBSYSAPI EXPORTNUM(330) xboxkrnl::VOID NTAPI xboxkrnl::READ_PORT_BUFFER_USHORT
(
	IN PUSHORT Port,
	IN PUSHORT Buffer,
	IN ULONG   Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = (uint16_t)EmuX86_IORead((xbaddr)Port, sizeof(uint16_t));
}

// ******************************************************************
// * 0x014B - READ_PORT_BUFFER_ULONG()
// ******************************************************************
XBSYSAPI EXPORTNUM(331) xboxkrnl::VOID NTAPI xboxkrnl::READ_PORT_BUFFER_ULONG
(
	IN PULONG Port,
	IN PULONG Buffer,
	IN ULONG  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = EmuX86_IORead((xbaddr)Port, sizeof(uint32_t));
}

// ******************************************************************
// * 0x014C - WRITE_PORT_BUFFER_UCHAR()
// ******************************************************************
XBSYSAPI EXPORTNUM(332) xboxkrnl::VOID NTAPI xboxkrnl::WRITE_PORT_BUFFER_UCHAR
(
	IN PUCHAR Port,
	IN PUCHAR Buffer,
	IN ULONG  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbaddr)Port, *Buffer++, sizeof(uint8_t));
}

// ******************************************************************
// * 0x014D - WRITE_PORT_BUFFER_USHORT()
// ******************************************************************
XBSYSAPI EXPORTNUM(333) xboxkrnl::VOID NTAPI xboxkrnl::WRITE_PORT_BUFFER_USHORT
(
	IN PUSHORT Port,
	IN PUSHORT Buffer,
	IN ULONG   Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbaddr)Port, *Buffer++, sizeof(uint16_t));
}

// ******************************************************************
// * 0x014E - WRITE_PORT_BUFFER_ULONG()
// ******************************************************************
XBSYSAPI EXPORTNUM(334) xboxkrnl::VOID NTAPI xboxkrnl::WRITE_PORT_BUFFER_ULONG
(
	IN PULONG Port,
	IN PULONG Buffer,
	IN ULONG  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbaddr)Port, *Buffer++, sizeof(uint32_t));
}

// ******************************************************************
// * 0x0164 - HalBootSMCVideoMode
// ******************************************************************
// TODO: Verify this!
XBSYSAPI EXPORTNUM(356) xboxkrnl::DWORD xboxkrnl::HalBootSMCVideoMode = 1; // TODO : AV_PACK_STANDARD?

// ******************************************************************
// * 0x0166 - HalIsResetOrShutdownPending()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(358) xboxkrnl::BOOLEAN NTAPI xboxkrnl::HalIsResetOrShutdownPending
(
)
{
	LOG_FUNC();

	BOOLEAN ret = FALSE;

	if (ResetOrShutdownCommandCode != 0) { ret = TRUE; }

	RETURN(ret);
}

// ******************************************************************
// * 0x0168 - HalInitiateShutdown()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(360) xboxkrnl::NTSTATUS NTAPI xboxkrnl::HalInitiateShutdown
(
)
{
	LOG_FUNC();
	
	ResetOrShutdownCommandCode = SMC_COMMAND_RESET;
	ResetOrShutdownDataValue = SMC_RESET_ASSERT_SHUTDOWN;
	xboxkrnl::HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, ResetOrShutdownCommandCode, 0, ResetOrShutdownDataValue);

	RETURN(S_OK);
}

// ******************************************************************
// * 0x016D - HalEnableSecureTrayEject()
// ******************************************************************
// Notifies the SMBUS that ejecting the DVD-ROM should not reset the system.
// Note that HalEnableSecureTrayEject can't really be called directly...
//
// New to the XBOX.
// Source:XBMC Undocumented.h
XBSYSAPI EXPORTNUM(365) xboxkrnl::VOID NTAPI xboxkrnl::HalEnableSecureTrayEject
(
)
{
	LOG_FUNC();

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x016E - HalWriteSMCScratchRegister()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(366) xboxkrnl::NTSTATUS NTAPI xboxkrnl::HalWriteSMCScratchRegister
(
	IN DWORD ScratchRegister
)
{
	LOG_FUNC_ONE_ARG(ScratchRegister);

//	HalpSMCScratchRegister = ScratchRegister;

	NTSTATUS Res = HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_SCRATCH, /*WordFlag:*/false, ScratchRegister);

	RETURN(Res);
}
