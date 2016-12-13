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

#include "Logging.h" // For LOG_FUNC()
#include "EmuKrnlLogging.h"
#include "CxbxKrnl.h" // For CxbxKrnlCleanup
#include "Emu.h" // For EmuWarning()

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
// Source:ReactOS
XBSYSAPI EXPORTNUM(39) xboxkrnl::VOID NTAPI xboxkrnl::HalDisableSystemInterrupt
(
	ULONG Vector,
	KIRQL Irql
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Vector)
		LOG_FUNC_ARG(Irql)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();
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
// Source:ReactOS
XBSYSAPI EXPORTNUM(43) xboxkrnl::BOOLEAN NTAPI xboxkrnl::HalEnableSystemInterrupt
(
	ULONG Vector,
	KIRQL Irql,
	KINTERRUPT_MODE InterruptMode
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(Vector)
		LOG_FUNC_ARG(Irql)
		LOG_FUNC_ARG(InterruptMode)
		LOG_FUNC_END;

	LOG_UNIMPLEMENTED();

	RETURN(FALSE);
}

// ******************************************************************
// * 0x002C - HalGetInterruptVector()
// ******************************************************************
XBSYSAPI EXPORTNUM(44) xboxkrnl::ULONG  NTAPI xboxkrnl::HalGetInterruptVector
(
	IN ULONG   InterruptLevel,
	OUT PKIRQL  Irql
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(InterruptLevel)
		LOG_FUNC_ARG_OUT(Irql)
		LOG_FUNC_END;

	// I'm only adding this for Virtua Cop 3 (Chihiro). Xbox games need not emulate this.

	EmuWarning("HalGetInterruptVector(): If this is NOT a Chihiro game, tell blueshogun!");

	RETURN(1);
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

	if (ReadWord) {
		// Write UCHAR
	}
	else {
		// Write BYTE
	}

	RETURN(STATUS_SUCCESS);
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

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x002F - HalRegisterShutdownNotification()
// ******************************************************************
XBSYSAPI EXPORTNUM(47) xboxkrnl::VOID xboxkrnl::HalRegisterShutdownNotification
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
XBSYSAPI EXPORTNUM(49) xboxkrnl::VOID DECLSPEC_NORETURN xboxkrnl::HalReturnToFirmware
(
	RETURN_FIRMWARE Routine
)
{
	LOG_FUNC_ONE_ARG(Routine);
	CxbxKrnlCleanup("Xbe has rebooted : HalReturnToFirmware(%d)", Routine);
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

	// TODO: Later.
	LOG_UNIMPLEMENTED();

	RETURN(STATUS_SUCCESS);
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

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED();
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

	LOG_UNIMPLEMENTED();
}

// ******************************************************************
// * 0x0164 - HalBootSMCVideoMode
// ******************************************************************
// TODO: Verify this!
XBSYSAPI EXPORTNUM(356) xboxkrnl::DWORD xboxkrnl::HalBootSMCVideoMode = 1;

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
// Note that this function can't really be called directly...
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
