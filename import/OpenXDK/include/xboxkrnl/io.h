// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : io.h
// *
// * note : XBox Kernel *I/O Manager* Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_IO_H
#define XBOXKRNL_IO_H

XBSYSAPI VOID *IoAllocateIrp;
XBSYSAPI VOID *IoBuildAsynchronousFsdRequest;
XBSYSAPI VOID *IoBuildDeviceIoControlRequest;
XBSYSAPI VOID *IoBuildSynchronousFsdRequest;
XBSYSAPI VOID *IoCheckShareAccess;

XBSYSAPI EXPORTNUM(64) OBJECT_TYPE IoCompletionObjectType;

XBSYSAPI EXPORTNUM(65) NTSTATUS NTAPI IoCreateDevice
(
	IN  PDRIVER_OBJECT		DriverObject,
	IN  ULONG				DeviceExtensionSize,
	IN  PSTRING				DeviceName  OPTIONAL,
	IN  ULONG				DeviceType,
	IN  BOOLEAN				Exclusive,
	OUT PDEVICE_OBJECT*		DeviceObject
);

// ******************************************************************
// * IoCreateFile
// ******************************************************************
XBSYSAPI EXPORTNUM(66) NTSTATUS NTAPI IoCreateFile
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               Disposition,
    IN  ULONG               CreateOptions,
    IN  ULONG               Options
);

// ******************************************************************
// * IoCreateSymbolicLink
// ******************************************************************
XBSYSAPI EXPORTNUM(67) NTSTATUS NTAPI IoCreateSymbolicLink
(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
);

XBSYSAPI VOID *IoDeleteDevice;

// ******************************************************************
// * IoDeleteSymbolicLink
// ******************************************************************
XBSYSAPI EXPORTNUM(69) NTSTATUS NTAPI IoDeleteSymbolicLink
(
    IN PSTRING SymbolicLinkName
);

XBSYSAPI EXPORTNUM(70) OBJECT_TYPE IoDeviceObjectType;
XBSYSAPI EXPORTNUM(71) OBJECT_TYPE IoFileObjectType;

XBSYSAPI VOID *IoFreeIrp;
XBSYSAPI VOID *IoInitializeIrp;
XBSYSAPI VOID *IoInvalidDeviceRequest;
XBSYSAPI VOID *IoQueryFileInformation;
XBSYSAPI VOID *IoQueryVolumeInformation;
XBSYSAPI VOID *IoQueueThreadIrp;
XBSYSAPI VOID *IoRemoveShareAccess;
XBSYSAPI VOID *IoSetIoCompletion;
XBSYSAPI VOID *IoSetShareAccess;
XBSYSAPI VOID *IoStartNextPacket;
XBSYSAPI VOID *IoStartNextPacketByKey;
XBSYSAPI VOID *IoStartPacket;
XBSYSAPI VOID *IoSynchronousDeviceIoControlRequest;
XBSYSAPI VOID *IoSynchronousFsdRequest;
XBSYSAPI VOID *IofCallDriver;
XBSYSAPI VOID *IofCompleteRequest;

XBSYSAPI EXPORTNUM(90) NTSTATUS NTAPI IoDismountVolume
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * IoDismountVolumeByName
// ******************************************************************
XBSYSAPI EXPORTNUM(91) NTSTATUS NTAPI IoDismountVolumeByName
(
	IN PSTRING VolumeName
);

XBSYSAPI VOID *IoMarkIrpMustComplete;

#endif


