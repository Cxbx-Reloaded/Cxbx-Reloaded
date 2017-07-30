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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h> // For HalReadSMCTrayState, etc.
};

#include <Shlwapi.h> // For PathRemoveFileSpec()
#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()
#include "EmuX86.h" // HalReadWritePciSpace needs this
#include "EmuEEPROM.h" // For EEPROM
#include "EmuShared.h"
#include "EmuFile.h" // For FindNtSymbolicLinkObjectByDriveLetter

// prevent name collisions
namespace NtDll
{
#include "EmuNtDll.h"
};

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
	LOG_FUNC_ONE_ARG(Request);

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED(); // TODO : Once thread-switching works, make system interrupts work too
}

// ******************************************************************
// * 0x0028 - HalDiskCachePartitionCount
// ******************************************************************
// This enables Partition3..7  Source:OpenXDK  TODO : Make this configurable
XBSYSAPI EXPORTNUM(40) xboxkrnl::ULONG xboxkrnl::HalDiskCachePartitionCount = 4; // Was 3

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

	LOG_UNIMPLEMENTED(); // TODO : Once thread-switching works, make system interrupts work too
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
		DbgPrintf("HalGetInterruptVector(): Interrupt vector requested for %d (%s)!\n", 
			BusInterruptLevel, IRQNames[BusInterruptLevel]);
#endif
	}

	RETURN(dwVector);
}

#define SMC_SLAVE_ADDRESS 0x20
#define SMBUS_SMC_WRITE SMC_SLAVE_ADDRESS // = 0x20
#define SMBUS_SMC_READ (SMC_SLAVE_ADDRESS || 1) // = 0x21

#define EEPROM_ADDRESS 0xA8
#define SMBUS_EEPROM_WRITE EEPROM_ADDRESS // = 0xA8
#define SMBUS_EEPROM_READ (EEPROM_ADDRESS || 1) // = 0xA9

#define SMBUS_TV_ENCODER_ID_CONEXANT 0x8A
#define SMBUS_TV_ENCODER_ID_CONEXANT_WRITE SMBUS_TV_ENCODER_ID_CONEXANT // = 0x8A
#define SMBUS_TV_ENCODER_ID_CONEXANT_READ (SMBUS_TV_ENCODER_ID_CONEXANT_WRITE || 1) // = 0x8B

#define SMBUS_TV_ENCODER_ID_FOCUS 0xD4
#define SMBUS_TV_ENCODER_ID_FOCUS_WRITE SMBUS_TV_ENCODER_ID_FOCUS // = 0xD4
#define SMBUS_TV_ENCODER_ID_FOCUS_READ (SMBUS_TV_ENCODER_ID_FOCUS_WRITE || 1) // = 0xD5

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

	NTSTATUS Status = STATUS_SUCCESS;

	switch (Address) {
	case SMBUS_EEPROM_READ: {
		if (ReadWord)
			*DataValue = *((PWORD)(((PBYTE)EEPROM) + Command));
		else
			*DataValue = *(((PBYTE)EEPROM) + Command);

		break;
	}
	default:
		// TODO : Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
		// Resources : http://pablot.com/misc/fancontroller.cpp
		// https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
		LOG_INCOMPLETE();
		Status = STATUS_UNSUCCESSFUL; // TODO : Faked. Figure out the real error status
	}

	RETURN(Status );
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

	// TODO: Verify this calculation is actually correct
	size_t Size = Length / sizeof(ULONG);
	ULONG RegisterByteOffset = 0;

	while (Length > 0) {
		switch (Size) {
		case 4:
			CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);
			EmuX86_IOWrite32((xbaddr)PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);

			if (WritePCISpace) {
				EmuX86_IOWrite32((xbaddr)PCI_TYPE1_DATA_PORT, *((PULONG)Buffer));
			}
			else {
				*((PULONG)Buffer) = EmuX86_IORead32((xbaddr)PCI_TYPE1_DATA_PORT);
			}
			break;
		case 2:
			RegisterByteOffset = RegisterNumber % sizeof(ULONG);
			CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

			EmuX86_IOWrite32((xbaddr)PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);

			if (WritePCISpace) {
				EmuX86_IOWrite16((xbaddr)PCI_TYPE1_DATA_PORT + RegisterByteOffset, *((PUSHORT)Buffer));
			}
			else {
				*((PUSHORT)Buffer) = EmuX86_IORead16((xbaddr)PCI_TYPE1_DATA_PORT + RegisterByteOffset);
			}
			break;
		case 1: {
			RegisterByteOffset = RegisterNumber % sizeof(ULONG);
			CfgBits.u.bits.RegisterNumber = RegisterNumber / sizeof(ULONG);

			EmuX86_IOWrite32((xbaddr)PCI_TYPE1_ADDR_PORT, CfgBits.u.AsULONG);

			if (WritePCISpace) {
				EmuX86_IOWrite8((xbaddr)PCI_TYPE1_DATA_PORT + RegisterByteOffset, *((PUCHAR)Buffer));
			}
			else {
				*((PUCHAR)Buffer) = EmuX86_IORead8((xbaddr)PCI_TYPE1_DATA_PORT + RegisterByteOffset);
			}
		}
			break;
		}

		RegisterNumber += Size;
		Buffer = (PUCHAR)Buffer + Size;
		Length -= Size;
	}

	
	// TODO: Enable Interrupt Processing1
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

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0030 - HalRequestSoftwareInterrupt()
// ******************************************************************
// Source:ReactOS
XBSYSAPI EXPORTNUM(48) xboxkrnl::VOID FASTCALL xboxkrnl::HalRequestSoftwareInterrupt
(
	IN KIRQL Request
)
{
	LOG_FUNC_ONE_ARG(Request);

	LOG_UNIMPLEMENTED();
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
		CxbxKrnlCleanup("Emulated Xbox is halted");
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
			MmPersistContiguousMemory((PVOID)xboxkrnl::LaunchDataPage, sizeof(LAUNCH_DATA_PAGE), TRUE);

			char *lpTitlePath = xboxkrnl::LaunchDataPage->Header.szLaunchPath;
			char szXbePath[MAX_PATH];
			char szWorkingDirectoy[MAX_PATH];

			// Convert Xbox XBE Path to Windows Path
			{
				EmuNtSymbolicLinkObject* symbolicLink = FindNtSymbolicLinkObjectByDriveLetter(lpTitlePath[0]);
				snprintf(szXbePath, MAX_PATH, "%s%s", symbolicLink->HostSymbolicLinkPath.c_str(), &lpTitlePath[2]);
			}

			// Determine Working Directory
			{
				strncpy_s(szWorkingDirectoy, szXbePath, MAX_PATH);
				PathRemoveFileSpec(szWorkingDirectoy);
			}

			// Relaunch Cxbx, to load another Xbe
			{
				char szArgsBuffer[4096];

				snprintf(szArgsBuffer, 4096, "/load \"%s\" %u %d \"%s\"", szXbePath, CxbxKrnl_hEmuParent, CxbxKrnl_DebugMode, CxbxKrnl_DebugFileName);
				if ((int)ShellExecute(NULL, "open", szFilePath_CxbxReloaded_Exe, szArgsBuffer, szWorkingDirectoy, SW_SHOWDEFAULT) <= 32)
					CxbxKrnlCleanup("Could not launch %s", lpTitlePath);
			}
		}
		break;
	};

	case ReturnFirmwareHard:
		LOG_UNIMPLEMENTED();
		break;

	case ReturnFirmwareFatal:
		CxbxPopupMessage("Emulated Xbox hit a fatal error (might be called by XapiBootToDash from within dashboard)");
		break;

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

	NTSTATUS Status = STATUS_SUCCESS;

	switch (Address) {
	case SMBUS_EEPROM_WRITE: {
		if (WriteWord)
			*((PWORD)(((PBYTE)EEPROM) + Command)) = (WORD)DataValue;
		else
			*(((PBYTE)EEPROM) + Command) = (BYTE)DataValue;

		break;
	}
	default:
		// TODO : Handle other SMBUS Addresses, like PIC_ADDRESS, XCALIBUR_ADDRESS
		// Resources : http://pablot.com/misc/fancontroller.cpp
		// https://github.com/JayFoxRox/Chihiro-Launcher/blob/master/hook.h
		LOG_INCOMPLETE();
		Status = STATUS_UNSUCCESSFUL; // TODO : Faked. Figure out the real error status
	}

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
		*Buffer++ = EmuX86_IORead8((xbaddr)Port);
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
		*Buffer++ = EmuX86_IORead16((xbaddr)Port);
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
		*Buffer++ = EmuX86_IORead32((xbaddr)Port);
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
		EmuX86_IOWrite8((xbaddr)Port, *Buffer++);
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
		EmuX86_IOWrite16((xbaddr)Port, *Buffer++);
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
		EmuX86_IOWrite32((xbaddr)Port, *Buffer++);
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

	LOG_UNIMPLEMENTED();

	RETURN(FALSE);
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

	LOG_UNIMPLEMENTED();

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

	LOG_UNIMPLEMENTED();

/* TODO
	HalpSMCScratchRegister = ScratchRegister;

	// TODO : Is this the way we need to set the value?
	return HalWriteSMBusValue(SMC_ADDRESS, SMC_COMMAND_SCRATCH, WordFlag: False, ScratchRegister);
*/
	
	RETURN(S_OK);
}
