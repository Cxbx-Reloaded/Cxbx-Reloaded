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

#define LOG_PREFIX CXBXR_MODULE::HAL

#include <core\kernel\exports\xboxkrnl.h> // For HalReadSMCTrayState, etc.
#include <Shlwapi.h> // For PathRemoveFileSpec()
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnl.h" // For InitializeListHead(), etc.
#include "EmuKrnlLogging.h"
#include "core\kernel\init\CxbxKrnl.h" // For CxbxKrnlCleanup, and CxbxExec
#include "core\kernel\support\Emu.h" // For EmuLog(LOG_LEVEL::WARNING, )
#include "EmuKrnl.h"
#include "devices\x86\EmuX86.h" // HalReadWritePciSpace needs this
#include "EmuShared.h"
#include "core\kernel\support\EmuFile.h" // For FindNtSymbolicLinkObjectByDriveLetter
#include "common\EmuEEPROM.h" // For EEPROM
#include "devices\Xbox.h" // For g_SMBus, SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER
#include "devices\SMCDevice.h" // For SMC_COMMAND_SCRATCH
#include "common/util/strConverter.hpp" // for utf16_to_ascii
#include "core\kernel\memory-manager\VMManager.h"

#include <algorithm> // for std::replace
#include <locale>
#include <codecvt>

volatile DWORD HalInterruptRequestRegister = APC_LEVEL | DISPATCH_LEVEL;
HalSystemInterrupt HalSystemInterrupts[MAX_BUS_INTERRUPT_LEVEL + 1];

// variables used by the SMC to know a reset / shutdown is pending
uint8_t ResetOrShutdownCommandCode = 0;
uint32_t ResetOrShutdownDataValue = 0;

// global list of routines executed during a reboot
xbox::LIST_ENTRY ShutdownRoutineList = { &ShutdownRoutineList , &ShutdownRoutineList }; // see InitializeListHead()


// ******************************************************************
// * Declaring this in a header causes errors with xboxkrnl
// * namespace, so we must declare it within any file that uses it
// ******************************************************************
xbox::KPCR* WINAPI KeGetPcr();

#define TRAY_CLOSED_MEDIA_PRESENT 0x60
#define TRAY_CLOSED_NO_MEDIA 0x40
#define TRAY_OPEN 0x10
#define INIT_TRAY_STATE 0xFFFFFFFF
static DWORD g_TrayState = INIT_TRAY_STATE;
static DWORD g_TrayCount = 0;

// ******************************************************************
// * 0x0009 - HalReadSMCTrayState()
// ******************************************************************
XBSYSAPI EXPORTNUM(9) xbox::ntstatus_xt NTAPI xbox::HalReadSMCTrayState
(
	dword_xt*	State,
	dword_xt*	Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(State)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	UCHAR orig_irql = KeRaiseIrqlToDpcLevel();
	DWORD TrayState = g_TrayState;
	DWORD orig_TrayCount = g_TrayCount;
	KfLowerIrql(orig_irql);

	NTSTATUS ret = xbox::status_success;
	if (TrayState == INIT_TRAY_STATE) {
		ret = HalReadSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_TRAY_STATE, 0, (PULONG)&TrayState);
		// If bit 31 = 1, then there is an error so do not execute this code.
		if ((ret >> 31) == 0) {
			TrayState &= 0x70;
			if ((TrayState != TRAY_CLOSED_MEDIA_PRESENT) && (TrayState != TRAY_CLOSED_NO_MEDIA)) {
				TrayState = TRAY_OPEN;
			}
			orig_irql = KeRaiseIrqlToDpcLevel();
			if (orig_TrayCount != g_TrayCount) {
				g_TrayState = TrayState;
			}
			KfLowerIrql(orig_irql);
		}
	}
	// FIXME: Pretend the tray is open
	// TRAY_CLOSED_NO_MEDIA causes Dashboard to call DeviceIoControl, which we do not implement
	// TRAY_CLOSED_MEDIA_PRESENT causes Dashboard to attempt to launch media, causing errors.
	//*State = TrayState;
	*State = TRAY_OPEN;
	if (Count) {
		*Count = orig_TrayCount;
	}
	RETURN(ret);
}

// ******************************************************************
// * 0x0026 - HalClearSoftwareInterrupt()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(38) xbox::void_xt FASTCALL xbox::HalClearSoftwareInterrupt
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
XBSYSAPI EXPORTNUM(39) xbox::void_xt NTAPI xbox::HalDisableSystemInterrupt
(
	IN ulong_xt BusInterruptLevel
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
XBSYSAPI EXPORTNUM(40) xbox::ulong_xt xbox::HalDiskCachePartitionCount = 3; 

// ******************************************************************
// * 0x0029 - HalDiskModelNumber
// ******************************************************************
// Source:OpenXDK  TODO : Fill this with something sensible
XBSYSAPI EXPORTNUM(41) xbox::PANSI_STRING xbox::HalDiskModelNumber = 0;

// ******************************************************************
// * 0x002A - HalDiskSerialNumber
// ******************************************************************
// Source:OpenXDK  TODO : Fill this with something sensible
XBSYSAPI EXPORTNUM(42) xbox::PANSI_STRING xbox::HalDiskSerialNumber = 0;	

// ******************************************************************
// * 0x002B - HalEnableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(43) xbox::void_xt NTAPI xbox::HalEnableSystemInterrupt
(
	IN ulong_xt BusInterruptLevel,
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
XBSYSAPI EXPORTNUM(44) xbox::ulong_xt NTAPI xbox::HalGetInterruptVector
(
	IN ulong_xt   BusInterruptLevel,
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
		EmuLog(LOG_LEVEL::DEBUG, "HalGetInterruptVector(): Interrupt vector requested for %d (%s)", 
			BusInterruptLevel, IRQNames[BusInterruptLevel]);
#endif
	}

	RETURN(dwVector);
}

// ******************************************************************
// * 0x002D - HalReadSMBusValue()
// ******************************************************************
XBSYSAPI EXPORTNUM(45) xbox::ntstatus_xt NTAPI xbox::HalReadSMBusValue
(
	IN  uchar_xt               Address,
	IN  uchar_xt               Command,
	IN  boolean_xt             ReadWord,
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

	NTSTATUS Status = xbox::status_success;

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
		Status = xbox::status_unsuccessful;
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
XBSYSAPI EXPORTNUM(46) xbox::void_xt NTAPI xbox::HalReadWritePCISpace
(
	IN ulong_xt   BusNumber,
	IN ulong_xt   SlotNumber,
	IN ulong_xt   RegisterNumber,
	IN PVOID   Buffer,
	IN ulong_xt   Length,
	IN boolean_xt WritePCISpace
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

		EmuX86_IOWrite((xbox::addr_xt)PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG, sizeof(uint32_t));

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
XBSYSAPI EXPORTNUM(47) xbox::void_xt NTAPI xbox::HalRegisterShutdownNotification
(
	IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
	IN boolean_xt Register
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
			if (ShutdownRegistration->Priority > CONTAINING_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry)->Priority)
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
			if (ShutdownRegistration == CONTAINING_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry))
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
XBSYSAPI EXPORTNUM(48) xbox::void_xt FASTCALL xbox::HalRequestSoftwareInterrupt
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
XBSYSAPI EXPORTNUM(49) xbox::void_xt DECLSPEC_NORETURN NTAPI xbox::HalReturnToFirmware
(
	RETURN_FIRMWARE Routine
)
{
	LOG_FUNC_ONE_ARG(Routine);

	bool is_reboot = false;

	switch (Routine) {
	case ReturnFirmwareHalt:
		CxbxKrnlCleanup("Emulated Xbox is halted");
		break;

	case ReturnFirmwareReboot:
		LOG_UNIMPLEMENTED();
		[[fallthrough]];

	case ReturnFirmwareQuickReboot:
	{
		if (xbox::LaunchDataPage == NULL)
			LOG_UNIMPLEMENTED();
		else
		{
			// Save the launch data page to disk for later.
			// (Note : XWriteTitleInfoNoReboot does this too)
			// Commented out because XLaunchNewImage is disabled!
			// MmPersistContiguousMemory((PVOID)xbox::LaunchDataPage, sizeof(LAUNCH_DATA_PAGE), TRUE);

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

					ShutdownRegistration = CONTAINING_RECORD(ListEntry, HAL_SHUTDOWN_REGISTRATION, ListEntry);
					ShutdownRegistration->NotificationRoutine(ShutdownRegistration);
				}
				#endif
			}


			std::string TitlePath = xbox::LaunchDataPage->Header.szLaunchPath;

			// If the title path starts with a semicolon, remove it
			if (TitlePath.length() > 0 && TitlePath[0] == ';') {
				TitlePath.erase(0, 1);
			}

			// If the title path was an empty string, we need to launch the dashboard
			if (TitlePath.length() == 0) {
				TitlePath = DeviceHarddisk0Partition2 + "\\xboxdash.xbe";
			}

			std::string& XbePath = CxbxConvertXboxToHostPath(TitlePath);

			// Relaunch Cxbx, to load another Xbe
			{
				int QuickReboot;
				g_EmuShared->GetBootFlags(&QuickReboot);
				QuickReboot |= BOOT_QUICK_REBOOT;
				g_EmuShared->SetBootFlags(&QuickReboot);
				is_reboot = true;

				g_VMManager.SavePersistentMemory();

				// Some titles (Xbox Dashboard and retail/demo discs) use ";" as a current directory path seperator
				// This process is handled during initialization. No special handling here required.

				CxbxLaunchNewXbe(XbePath);

			}
		}
		break;
	};

	case ReturnFirmwareHard:
		LOG_UNIMPLEMENTED();
		break;

	case ReturnFirmwareFatal:
	{
		xbox::HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_SCRATCH, 0, SMC_SCRATCH_DISPLAY_FATAL_ERROR);

		g_VMManager.SavePersistentMemory();

		CxbxLaunchNewXbe(szFilePath_Xbe);
		break;
	}

	case ReturnFirmwareAll:
		LOG_UNIMPLEMENTED();
		break;

	default:
		LOG_UNIMPLEMENTED();
	}

	CxbxKrnlShutDown(is_reboot);
	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
}

// ******************************************************************
// * 0x0032 - HalWriteSMBusValue()
// ******************************************************************
XBSYSAPI EXPORTNUM(50) xbox::ntstatus_xt NTAPI xbox::HalWriteSMBusValue
(
	IN  uchar_xt               Address,
	IN  uchar_xt               Command,
	IN  boolean_xt             WriteWord,
	IN  ulong_xt               DataValue
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Address)
		LOG_FUNC_ARG(Command)
		LOG_FUNC_ARG(WriteWord)
		LOG_FUNC_ARG(DataValue)
		LOG_FUNC_END;

	// TODO : Prevent interrupts

	NTSTATUS Status = xbox::status_success;

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
		Status = xbox::status_unsuccessful;
	}

	// TODO : Reenable interrupts

	RETURN(Status);
}

// ******************************************************************
// * 0x0149 - READ_PORT_BUFFER_UCHAR()
// ******************************************************************
XBSYSAPI EXPORTNUM(329) xbox::void_xt NTAPI xbox::READ_PORT_BUFFER_UCHAR
(
	IN PUCHAR Port,
	IN PUCHAR Buffer,
	IN ulong_xt  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = (uint8_t)EmuX86_IORead((xbox::addr_xt)Port, sizeof(uint8_t));
}

// ******************************************************************
// * 0x014A - READ_PORT_BUFFER_USHORT()
// ******************************************************************
XBSYSAPI EXPORTNUM(330) xbox::void_xt NTAPI xbox::READ_PORT_BUFFER_USHORT
(
	IN PUSHORT Port,
	IN PUSHORT Buffer,
	IN ulong_xt   Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = (uint16_t)EmuX86_IORead((xbox::addr_xt)Port, sizeof(uint16_t));
}

// ******************************************************************
// * 0x014B - READ_PORT_BUFFER_ULONG()
// ******************************************************************
XBSYSAPI EXPORTNUM(331) xbox::void_xt NTAPI xbox::READ_PORT_BUFFER_ULONG
(
	IN PULONG Port,
	IN PULONG Buffer,
	IN ulong_xt  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		*Buffer++ = EmuX86_IORead((xbox::addr_xt)Port, sizeof(uint32_t));
}

// ******************************************************************
// * 0x014C - WRITE_PORT_BUFFER_UCHAR()
// ******************************************************************
XBSYSAPI EXPORTNUM(332) xbox::void_xt NTAPI xbox::WRITE_PORT_BUFFER_UCHAR
(
	IN PUCHAR Port,
	IN PUCHAR Buffer,
	IN ulong_xt  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbox::addr_xt)Port, *Buffer++, sizeof(uint8_t));
}

// ******************************************************************
// * 0x014D - WRITE_PORT_BUFFER_USHORT()
// ******************************************************************
XBSYSAPI EXPORTNUM(333) xbox::void_xt NTAPI xbox::WRITE_PORT_BUFFER_USHORT
(
	IN PUSHORT Port,
	IN PUSHORT Buffer,
	IN ulong_xt   Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbox::addr_xt)Port, *Buffer++, sizeof(uint16_t));
}

// ******************************************************************
// * 0x014E - WRITE_PORT_BUFFER_ULONG()
// ******************************************************************
XBSYSAPI EXPORTNUM(334) xbox::void_xt NTAPI xbox::WRITE_PORT_BUFFER_ULONG
(
	IN PULONG Port,
	IN PULONG Buffer,
	IN ulong_xt  Count
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Port)
		LOG_FUNC_ARG(Buffer)
		LOG_FUNC_ARG(Count)
		LOG_FUNC_END;

	while (Count-- > 0)
		EmuX86_IOWrite((xbox::addr_xt)Port, *Buffer++, sizeof(uint32_t));
}

// ******************************************************************
// * 0x0164 - HalBootSMCVideoMode
// ******************************************************************
// TODO: This should be set to whichever AV Pack is read from the SMC at boot time
XBSYSAPI EXPORTNUM(356) xbox::dword_xt xbox::HalBootSMCVideoMode = 1; // TODO : AV_PACK_STANDARD?

// ******************************************************************
// * 0x0166 - HalIsResetOrShutdownPending()
// ******************************************************************
// Source:Dxbx
XBSYSAPI EXPORTNUM(358) xbox::boolean_xt NTAPI xbox::HalIsResetOrShutdownPending
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
XBSYSAPI EXPORTNUM(360) xbox::ntstatus_xt NTAPI xbox::HalInitiateShutdown
(
)
{
	LOG_FUNC();
	
	ResetOrShutdownCommandCode = SMC_COMMAND_RESET;
	ResetOrShutdownDataValue = SMC_RESET_ASSERT_SHUTDOWN;
	xbox::HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, ResetOrShutdownCommandCode, 0, ResetOrShutdownDataValue);

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
XBSYSAPI EXPORTNUM(365) xbox::void_xt NTAPI xbox::HalEnableSecureTrayEject
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
XBSYSAPI EXPORTNUM(366) xbox::ntstatus_xt NTAPI xbox::HalWriteSMCScratchRegister
(
	IN dword_xt ScratchRegister
)
{
	LOG_FUNC_ONE_ARG(ScratchRegister);

//	HalpSMCScratchRegister = ScratchRegister;

	NTSTATUS Res = HalWriteSMBusValue(SMBUS_ADDRESS_SYSTEM_MICRO_CONTROLLER, SMC_COMMAND_SCRATCH, /*WordFlag:*/false, ScratchRegister);

	RETURN(Res);
}
