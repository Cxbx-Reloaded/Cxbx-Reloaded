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

XBSYSAPI VOID *HalReadSMCTrayState;
XBSYSAPI VOID *HalClearSoftwareInterrupt;
XBSYSAPI VOID *HalDisableSystemInterrupt;
XBSYSAPI VOID *HalDiskCachePartitionCount;
XBSYSAPI VOID *HalDiskModelNumber;
XBSYSAPI VOID *HalDiskSerialNumber;
XBSYSAPI VOID *HalEnableSystemInterrupt;
XBSYSAPI VOID *HalGetInterruptVector;

// ******************************************************************
// * HalReadSMBusValue
// ******************************************************************
XBSYSAPI EXPORTNUM(45) VOID NTAPI HalReadSMBusValue
(
    ULONG   BusNumber,
    ULONG   SlotNumber,
    ULONG   RegisterNumber,
    PVOID   Buffer,
    ULONG   Length,
    BOOLEAN WritePCISpace
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

XBSYSAPI VOID *HalRegisterShutdownNotification;
XBSYSAPI VOID *HalRequestSoftwareInterrupt;

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
XBSYSAPI EXPORTNUM(50) ULONG NTAPI HalWriteSMBusValue
(
    UCHAR   Address,
    UCHAR   Command,
    BOOLEAN WordFlag,
    ULONG   Value
);

// ******************************************************************
// * HalBootSMCVideoMode
// ******************************************************************
XBSYSAPI EXPORTNUM(356) DWORD HalBootSMCVideoMode;

XBSYSAPI VOID *HalIsResetOrShutdownPending;
XBSYSAPI VOID *HalInitiateShutdown;
XBSYSAPI VOID *HalEnableSecureTrayEject;
XBSYSAPI VOID *HalWriteSMCScratchRegister;

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

#endif


