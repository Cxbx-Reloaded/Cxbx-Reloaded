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

#include "xboxkrnl/xboxkrnl_types.h"

namespace xbox
{

// ******************************************************************
// * HalReadSMCTrayState
// ******************************************************************
XBSYSAPI EXPORTNUM(9) NTSTATUS NTAPI HalReadSMCTrayState
(
	dword_t*	State,
	dword_t*	Count
);

XBSYSAPI EXPORTNUM(38) void_t FASTCALL HalClearSoftwareInterrupt
(
	KIRQL Request
);

// ******************************************************************
// * 0x0027 - HalDisableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(39) void_t NTAPI HalDisableSystemInterrupt
(
	IN ulong_t BusInterruptLevel
);

XBSYSAPI EXPORTNUM(40) ulong_t HalDiskCachePartitionCount;
XBSYSAPI EXPORTNUM(41) PANSI_STRING HalDiskModelNumber;
XBSYSAPI EXPORTNUM(42) PANSI_STRING HalDiskSerialNumber;

// ******************************************************************
// * 0x002B - HalEnableSystemInterrupt()
// ******************************************************************
XBSYSAPI EXPORTNUM(43) void_t NTAPI HalEnableSystemInterrupt
(
	IN ulong_t BusInterruptLevel,
	IN KINTERRUPT_MODE InterruptMode
);

// ******************************************************************
// * HalGetInterruptVector
// ******************************************************************
XBSYSAPI EXPORTNUM(44) ulong_t  NTAPI HalGetInterruptVector
(
    IN ulong_t   BusInterruptLevel,
    OUT PKIRQL Irql
);

// ******************************************************************
// * HalReadSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(45) NTSTATUS NTAPI HalReadSMBusValue
(
    IN uchar_t   Address,
    IN uchar_t   Command,
    IN boolean_t WriteWord,
    OUT PULONG DataValue
);

// ******************************************************************
// * HalReadWritePCISpace
// ******************************************************************
XBSYSAPI EXPORTNUM(46) void_t NTAPI HalReadWritePCISpace
(
  IN ulong_t   BusNumber,
  IN ulong_t   SlotNumber,
  IN ulong_t   RegisterNumber,
  IN PVOID   Buffer,
  IN ulong_t   Length,
  IN boolean_t WritePCISpace
);

typedef void_t (*PHAL_SHUTDOWN_NOTIFICATION)(
    IN struct _HAL_SHUTDOWN_REGISTRATION *ShutdownRegistration
);

typedef struct _HAL_SHUTDOWN_REGISTRATION {
    PHAL_SHUTDOWN_NOTIFICATION NotificationRoutine;
    long_t Priority;
    LIST_ENTRY ListEntry;
} HAL_SHUTDOWN_REGISTRATION, *PHAL_SHUTDOWN_REGISTRATION;

XBSYSAPI EXPORTNUM(47) void_t NTAPI HalRegisterShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN boolean_t Register
);

XBSYSAPI EXPORTNUM(46) void_t FASTCALL HalRequestSoftwareInterrupt
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
XBSYSAPI EXPORTNUM(49) void_t DECLSPEC_NORETURN NTAPI HalReturnToFirmware
(
    RETURN_FIRMWARE Routine
);

// ******************************************************************
// * HalWriteSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(50) NTSTATUS NTAPI HalWriteSMBusValue
(
    uchar_t   Address,
    uchar_t   Command,
    boolean_t WriteWord,
    ulong_t   DataValue
);

// ******************************************************************
// * READ_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(329) void_t NTAPI READ_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    IN PUCHAR Buffer,
    IN ulong_t  Count
);

// ******************************************************************
// * READ_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(330) void_t NTAPI READ_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    IN PUSHORT Buffer,
    IN ulong_t   Count
);

// ******************************************************************
// * READ_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(331) void_t NTAPI READ_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    IN PULONG Buffer,
    IN ulong_t  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(332) void_t NTAPI WRITE_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    IN PUCHAR Buffer,
    IN ulong_t  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(333) void_t NTAPI WRITE_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    IN PUSHORT Buffer,
    IN ulong_t   Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(334) void_t NTAPI WRITE_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    IN PULONG Buffer,
    IN ulong_t  Count
);

// ******************************************************************
// * HalBootSMCVideoMode
// ******************************************************************
XBSYSAPI EXPORTNUM(356) dword_t HalBootSMCVideoMode;

XBSYSAPI EXPORTNUM(358) boolean_t NTAPI HalIsResetOrShutdownPending
(
);

XBSYSAPI EXPORTNUM(360) NTSTATUS NTAPI HalInitiateShutdown
(
);

XBSYSAPI EXPORTNUM(365) void_t NTAPI HalEnableSecureTrayEject
(
);

XBSYSAPI EXPORTNUM(366) NTSTATUS NTAPI HalWriteSMCScratchRegister
(
	IN dword_t ScratchRegister
);

}

#endif


