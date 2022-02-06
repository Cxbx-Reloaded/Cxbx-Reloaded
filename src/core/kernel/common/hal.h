// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : hal.h
// *
// * note : XBox Kernel *Hardware Abstraction Layer* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_HAL_H
#define XBOXKRNL_HAL_H

#include "types.h"

namespace xbox
{

// ******************************************************************
// * HalReadSMCTrayState
// ******************************************************************
XBSYSAPI EXPORTNUM(9) ntstatus_xt NTAPI HalReadSMCTrayState
(
	dword_xt*	State,
	dword_xt*	Count
);

XBSYSAPI EXPORTNUM(38) void_xt FASTCALL HalClearSoftwareInterrupt
(
	KIRQL Request
);

// ******************************************************************
// * 0x0027 - HalDisableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(39) void_xt NTAPI HalDisableSystemInterrupt
(
	IN ulong_xt BusInterruptLevel
);

XBSYSAPI EXPORTNUM(40) ulong_xt HalDiskCachePartitionCount;
XBSYSAPI EXPORTNUM(41) PANSI_STRING HalDiskModelNumber;
XBSYSAPI EXPORTNUM(42) PANSI_STRING HalDiskSerialNumber;

// ******************************************************************
// * 0x002B - HalEnableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(43) void_xt NTAPI HalEnableSystemInterrupt
(
	IN ulong_xt BusInterruptLevel,
	IN KINTERRUPT_MODE InterruptMode
);

// ******************************************************************
// * HalGetInterruptVector
// ******************************************************************
XBSYSAPI EXPORTNUM(44) ulong_xt  NTAPI HalGetInterruptVector
(
    IN ulong_xt   BusInterruptLevel,
    OUT PKIRQL Irql
);

// ******************************************************************
// * HalReadSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(45) ntstatus_xt NTAPI HalReadSMBusValue
(
    IN uchar_xt   Address,
    IN uchar_xt   Command,
    IN boolean_xt WriteWord,
    OUT PULONG DataValue
);

// ******************************************************************
// * HalReadWritePCISpace
// ******************************************************************
XBSYSAPI EXPORTNUM(46) void_xt NTAPI HalReadWritePCISpace
(
  IN ulong_xt   BusNumber,
  IN ulong_xt   SlotNumber,
  IN ulong_xt   RegisterNumber,
  IN PVOID   Buffer,
  IN ulong_xt   Length,
  IN boolean_xt WritePCISpace
);

typedef void_xt (*PHAL_SHUTDOWN_NOTIFICATION)(
    IN struct _HAL_SHUTDOWN_REGISTRATION *ShutdownRegistration
);

typedef struct _HAL_SHUTDOWN_REGISTRATION {
    PHAL_SHUTDOWN_NOTIFICATION NotificationRoutine;
    long_xt Priority;
    LIST_ENTRY ListEntry;
} HAL_SHUTDOWN_REGISTRATION, *PHAL_SHUTDOWN_REGISTRATION;

XBSYSAPI EXPORTNUM(47) void_xt NTAPI HalRegisterShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN boolean_xt Register
);

XBSYSAPI EXPORTNUM(46) void_xt FASTCALL HalRequestSoftwareInterrupt
(
	IN KIRQL Request
);

// ******************************************************************
// * HalReturnToFirmware
// ******************************************************************
// *
// * Reboot / Shutdown / Etc
// *
// ******************************************************************
XBSYSAPI EXPORTNUM(49) void_xt DECLSPEC_NORETURN NTAPI HalReturnToFirmware
(
    RETURN_FIRMWARE Routine
);

// ******************************************************************
// * HalWriteSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(50) ntstatus_xt NTAPI HalWriteSMBusValue
(
    uchar_xt   Address,
    uchar_xt   Command,
    boolean_xt WriteWord,
    ulong_xt   DataValue
);

// ******************************************************************
// * READ_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(329) void_xt NTAPI READ_PORT_BUFFER_UCHAR
(
    IN dword_xt Port,
    IN PUCHAR Buffer,
    IN ulong_xt  Count
);

// ******************************************************************
// * READ_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(330) void_xt NTAPI READ_PORT_BUFFER_USHORT
(
    IN dword_xt Port,
    IN PUSHORT Buffer,
    IN ulong_xt   Count
);

// ******************************************************************
// * READ_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(331) void_xt NTAPI READ_PORT_BUFFER_ULONG
(
    IN dword_xt Port,
    IN PULONG Buffer,
    IN ulong_xt  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(332) void_xt NTAPI WRITE_PORT_BUFFER_UCHAR
(
    IN dword_xt Port,
    IN PUCHAR Buffer,
    IN ulong_xt  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(333) void_xt NTAPI WRITE_PORT_BUFFER_USHORT
(
    IN dword_xt Port,
    IN PUSHORT Buffer,
    IN ulong_xt   Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(334) void_xt NTAPI WRITE_PORT_BUFFER_ULONG
(
    IN dword_xt Port,
    IN PULONG Buffer,
    IN ulong_xt  Count
);

// ******************************************************************
// * HalBootSMCVideoMode
// ******************************************************************
XBSYSAPI EXPORTNUM(356) dword_xt HalBootSMCVideoMode;

XBSYSAPI EXPORTNUM(358) boolean_xt NTAPI HalIsResetOrShutdownPending
(
);

XBSYSAPI EXPORTNUM(360) ntstatus_xt NTAPI HalInitiateShutdown
(
);

XBSYSAPI EXPORTNUM(365) void_xt NTAPI HalEnableSecureTrayEject
(
);

XBSYSAPI EXPORTNUM(366) ntstatus_xt NTAPI HalWriteSMCScratchRegister
(
	IN dword_xt ScratchRegister
);

}

#endif


