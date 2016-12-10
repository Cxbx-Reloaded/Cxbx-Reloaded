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

// ******************************************************************
// * HalReadSMCTrayState
// ******************************************************************
XBSYSAPI EXPORTNUM(9) VOID NTAPI HalReadSMCTrayState
(
	DWORD*	State,
	DWORD*	Count
);

XBSYSAPI EXPORTNUM(38) VOID FASTCALL HalClearSoftwareInterrupt
(
	KIRQL Request
);

XBSYSAPI EXPORTNUM(39) VOID NTAPI HalDisableSystemInterrupt
(
	ULONG Vector,
	KIRQL Irql
);

XBSYSAPI EXPORTNUM(40) ULONG HalDiskCachePartitionCount;
XBSYSAPI EXPORTNUM(41) PANSI_STRING HalDiskModelNumber;
XBSYSAPI EXPORTNUM(42) PANSI_STRING HalDiskSerialNumber;
XBSYSAPI EXPORTNUM(43) BOOLEAN NTAPI HalEnableSystemInterrupt
(
	ULONG Vector,
	KIRQL Irql,
	KINTERRUPT_MODE InterruptMode
);

// ******************************************************************
// * HalGetInterruptVector
// ******************************************************************
XBSYSAPI EXPORTNUM(44) ULONG  NTAPI HalGetInterruptVector
(
    IN ULONG   InterruptLevel,
    OUT PKIRQL Irql
);

// ******************************************************************
// * HalReadSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(45) NTSTATUS NTAPI HalReadSMBusValue
(
    IN UCHAR   Address,
    IN UCHAR   Command,
    IN BOOLEAN WriteWord,
    OUT PULONG DataValue
);

// ******************************************************************
// * HalReadWritePCISpace
// ******************************************************************
XBSYSAPI EXPORTNUM(46) VOID NTAPI HalReadWritePCISpace
(
  IN ULONG   BusNumber,
  IN ULONG   SlotNumber,
  IN ULONG   RegisterNumber,
  IN PVOID   Buffer,
  IN ULONG   Length,
  IN BOOLEAN WritePCISpace
);

typedef VOID (*PHAL_SHUTDOWN_NOTIFICATION)(
    IN struct _HAL_SHUTDOWN_REGISTRATION *ShutdownRegistration
);

typedef struct {
    PHAL_SHUTDOWN_NOTIFICATION NotificationRoutine;
    LONG Priority;
    LIST_ENTRY ListEntry;
} HAL_SHUTDOWN_REGISTRATION, *PHAL_SHUTDOWN_REGISTRATION;

// TODO : NTAPI or FASTCALL ?
XBSYSAPI EXPORTNUM(47) VOID HalRegisterShutdownNotification(
    IN PHAL_SHUTDOWN_REGISTRATION ShutdownRegistration,
    IN BOOLEAN Register
);

XBSYSAPI EXPORTNUM(46) VOID FASTCALL HalRequestSoftwareInterrupt
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
XBSYSAPI EXPORTNUM(49) VOID DECLSPEC_NORETURN HalReturnToFirmware
(
    RETURN_FIRMWARE Routine
);

// ******************************************************************
// * HalWriteSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(50) NTSTATUS NTAPI HalWriteSMBusValue
(
    UCHAR   Address,
    UCHAR   Command,
    BOOLEAN WriteWord,
    ULONG   DataValue
);

// ******************************************************************
// * READ_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(329) VOID NTAPI READ_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    IN PUCHAR Buffer,
    IN ULONG  Count
);

// ******************************************************************
// * READ_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(330) VOID NTAPI READ_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    IN PUSHORT Buffer,
    IN ULONG   Count
);

// ******************************************************************
// * READ_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(331) VOID NTAPI READ_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    IN PULONG Buffer,
    IN ULONG  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_UCHAR
// ******************************************************************
XBSYSAPI EXPORTNUM(332) VOID NTAPI WRITE_PORT_BUFFER_UCHAR
(
    IN PUCHAR Port,
    IN PUCHAR Buffer,
    IN ULONG  Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_USHORT
// ******************************************************************
XBSYSAPI EXPORTNUM(333) VOID NTAPI WRITE_PORT_BUFFER_USHORT
(
    IN PUSHORT Port,
    IN PUSHORT Buffer,
    IN ULONG   Count
);

// ******************************************************************
// * WRITE_PORT_BUFFER_ULONG
// ******************************************************************
XBSYSAPI EXPORTNUM(334) VOID NTAPI WRITE_PORT_BUFFER_ULONG
(
    IN PULONG Port,
    IN PULONG Buffer,
    IN ULONG  Count
);

// ******************************************************************
// * HalBootSMCVideoMode
// ******************************************************************
XBSYSAPI EXPORTNUM(356) DWORD HalBootSMCVideoMode;

XBSYSAPI EXPORTNUM(358) BOOLEAN NTAPI HalIsResetOrShutdownPending
(
);

XBSYSAPI EXPORTNUM(360) NTSTATUS NTAPI HalInitiateShutdown
(
);

XBSYSAPI EXPORTNUM(365) VOID NTAPI HalEnableSecureTrayEject
(
);

XBSYSAPI EXPORTNUM(366) NTSTATUS NTAPI HalWriteSMCScratchRegister
(
	IN DWORD ScratchRegister
);

#endif


