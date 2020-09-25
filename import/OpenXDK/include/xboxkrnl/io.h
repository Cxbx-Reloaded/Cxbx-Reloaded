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

#include "xboxkrnl/xboxkrnl_types.h"

namespace xbox
{

// ******************************************************************
// * 0x003B - IoAllocateIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(59) PVOID NTAPI IoAllocateIrp
(
	IN cchar_t StackSize
);

// ******************************************************************
// * 0x003C - IoBuildAsynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(60) PVOID NTAPI IoBuildAsynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
);

// ******************************************************************
// * 0x003D - IoBuildDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(61) PVOID NTAPI IoBuildDeviceIoControlRequest
(
	IN ULONG IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength OPTIONAL,
	IN boolean_t InternalDeviceIoControl,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
);

// ******************************************************************
// * 0x003E - IoBuildSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(62) PVOID NTAPI IoBuildSynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock
);

// ******************************************************************
// * 0x003F - IoCheckShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(63) NTSTATUS NTAPI IoCheckShareAccess
(
	IN ACCESS_MASK DesiredAccess,
	IN ULONG DesiredShareAccess,
	OUT PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess,
	IN boolean_t Update
);

// ******************************************************************
// * 0x0040 - IoCompletionObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(64) OBJECT_TYPE IoCompletionObjectType;

// ******************************************************************
// * 0x0041 - IoCreateDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(65) NTSTATUS NTAPI IoCreateDevice
(
	IN  PDRIVER_OBJECT		DriverObject,
	IN  ULONG				DeviceExtensionSize,
	IN  PSTRING				DeviceName OPTIONAL,
	IN  ULONG				DeviceType,
	IN  boolean_t				Exclusive,
	OUT PDEVICE_OBJECT*		DeviceObject
);

// ******************************************************************
// * 0x0042 - IoCreateFile()
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
// * 0x0043 - IoCreateSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(67) NTSTATUS NTAPI IoCreateSymbolicLink
(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
);

// ******************************************************************
// * 0x0044 - IoDeleteDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(68) void_t NTAPI IoDeleteDevice
(
	IN PDEVICE_OBJECT irql
);

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(69) NTSTATUS NTAPI IoDeleteSymbolicLink
(
    IN PSTRING SymbolicLinkName
);

// ******************************************************************
// * 0x0046 - IoDeviceObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(70) OBJECT_TYPE IoDeviceObjectType;

// ******************************************************************
// * 0x0047 - IoFileObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(71) OBJECT_TYPE IoFileObjectType;

// ******************************************************************
// * 0x0048 - IoFreeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(72) void_t NTAPI IoFreeIrp
(
	IN PIRP Irp
);

// ******************************************************************
// * 0x0049 - IoInitializeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(73) PVOID NTAPI IoInitializeIrp
(
	IN PIRP Irp,
	IN USHORT PacketSize,
	IN cchar_t StackSize
);

// ******************************************************************
// * 0x004A - IoInvalidDeviceRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(74) NTSTATUS NTAPI IoInvalidDeviceRequest
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

// ******************************************************************
// * 0x004B - IoQueryFileInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(75) NTSTATUS NTAPI IoQueryFileInformation
(
	IN PFILE_OBJECT FileObject,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN ULONG Length,
	OUT PVOID FileInformation,
	OUT PULONG ReturnedLength
);

// ******************************************************************
// * 0x004C - IoQueryVolumeInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(76) NTSTATUS NTAPI IoQueryVolumeInformation
(
	IN PFILE_OBJECT FileObject,
	IN FS_INFORMATION_CLASS FsInformationClass,
	IN ULONG Length,
	OUT PVOID FsInformation,
	OUT PULONG ReturnedLength
);

// ******************************************************************
// * 0x004D - IoQueueThreadIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(77) void_t NTAPI IoQueueThreadIrp
(
	IN PIRP Irp
);

// ******************************************************************
// * 0x004E - IoRemoveShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(78) void_t NTAPI IoRemoveShareAccess
(
	IN PFILE_OBJECT FileObject,
	IN PSHARE_ACCESS ShareAccess
);

// ******************************************************************
// * 0x004F - IoSetIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(79) NTSTATUS NTAPI IoSetIoCompletion
(
	IN PKQUEUE IoCompletion,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN NTSTATUS IoStatus,
	IN ULONG IoStatusInformation
);

// ******************************************************************
// * 0x0050 - IoSetShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(80) cchar_t NTAPI IoSetShareAccess
(
	IN ULONG DesiredAccess,
	IN ULONG DesiredShareAccess,
	IN PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess
);

// ******************************************************************
// * 0x0051 - IoStartNextPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(81) void_t NTAPI IoStartNextPacket
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * 0x0052 - IoStartNextPacketByKey()
// ******************************************************************
XBSYSAPI EXPORTNUM(82) void_t NTAPI IoStartNextPacketByKey
(
	IN PDEVICE_OBJECT DeviceObject,
	IN ULONG Key
);

// ******************************************************************
// * 0x0053 - IoStartPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(83) void_t NTAPI IoStartPacket
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	OUT PULONG Key OPTIONAL
);

// ******************************************************************
// * 0x0054 - IoSynchronousDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(84) NTSTATUS NTAPI IoSynchronousDeviceIoControlRequest
(
	IN ULONG IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnedOutputBufferLength OPTIONAL,
	IN boolean_t InternalDeviceIoControl
);

// ******************************************************************
// * 0x0055 - IoSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(85) NTSTATUS NTAPI IoSynchronousFsdRequest
(
	IN ULONG MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ULONG Length,
	IN PLARGE_INTEGER StartingOffset OPTIONAL
);

// ******************************************************************
// * 0x0056 - IofCallDriver()
// ******************************************************************
XBSYSAPI EXPORTNUM(86) NTSTATUS FASTCALL IofCallDriver
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

// ******************************************************************
// * 0x0057 - IofCompleteRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(87) void_t FASTCALL IofCompleteRequest
(
	IN PIRP Irp,
	IN cchar_t PriorityBoost
);

// ******************************************************************
// * 0x005A - IoDismountVolume()
// ******************************************************************
XBSYSAPI EXPORTNUM(90) NTSTATUS NTAPI IoDismountVolume
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * 0x005B - IoDismountVolumeByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(91) NTSTATUS NTAPI IoDismountVolumeByName
(
	IN PSTRING VolumeName
);

// ******************************************************************
// * 0x0167 - IoMarkIrpMustComplete()
// ******************************************************************
XBSYSAPI EXPORTNUM(359) cchar_t NTAPI IoMarkIrpMustComplete
(
	IN PIRP Irp
);

}

#endif


