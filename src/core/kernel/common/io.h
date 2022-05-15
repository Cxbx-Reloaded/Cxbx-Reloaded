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

#include "types.h"

namespace xbox
{

// ******************************************************************
// * 0x003B - IoAllocateIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(59) PVOID NTAPI IoAllocateIrp
(
	IN cchar_xt StackSize
);

// ******************************************************************
// * 0x003C - IoBuildAsynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(60) PVOID NTAPI IoBuildAsynchronousFsdRequest
(
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
);

// ******************************************************************
// * 0x003D - IoBuildDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(61) PVOID NTAPI IoBuildDeviceIoControlRequest
(
	IN ulong_xt IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ulong_xt InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ulong_xt OutputBufferLength OPTIONAL,
	IN boolean_xt InternalDeviceIoControl,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
);

// ******************************************************************
// * 0x003E - IoBuildSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(62) PVOID NTAPI IoBuildSynchronousFsdRequest
(
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
	OUT PLARGE_INTEGER StartingOffset OPTIONAL,
	IN PKEVENT Event,
	OUT PIO_STATUS_BLOCK IoStatusBlock
);

// ******************************************************************
// * 0x003F - IoCheckShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(63) ntstatus_xt NTAPI IoCheckShareAccess
(
	IN access_mask_xt DesiredAccess,
	IN ulong_xt DesiredShareAccess,
	OUT PFILE_OBJECT FileObject,
	OUT PSHARE_ACCESS ShareAccess,
	IN boolean_xt Update
);

// ******************************************************************
// * 0x0040 - IoCompletionObjectType
// ******************************************************************
XBSYSAPI EXPORTNUM(64) OBJECT_TYPE IoCompletionObjectType;

// ******************************************************************
// * 0x0041 - IoCreateDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(65) ntstatus_xt NTAPI IoCreateDevice
(
	IN  PDRIVER_OBJECT		DriverObject,
	IN  ulong_xt				DeviceExtensionSize,
	IN  PSTRING				DeviceName OPTIONAL,
	IN  ulong_xt				DeviceType,
	IN  boolean_xt				Exclusive,
	OUT PDEVICE_OBJECT*		DeviceObject
);

// ******************************************************************
// * 0x0042 - IoCreateFile()
// ******************************************************************
XBSYSAPI EXPORTNUM(66) ntstatus_xt NTAPI IoCreateFile
(
    OUT PHANDLE             FileHandle,
    IN  access_mask_xt         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize,
    IN  ulong_xt               FileAttributes,
    IN  ulong_xt               ShareAccess,
    IN  ulong_xt               Disposition,
    IN  ulong_xt               CreateOptions,
    IN  ulong_xt               Options
);

// ******************************************************************
// * 0x0043 - IoCreateSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(67) ntstatus_xt NTAPI IoCreateSymbolicLink
(
    IN PSTRING SymbolicLinkName,
    IN PSTRING DeviceName
);

// ******************************************************************
// * 0x0044 - IoDeleteDevice()
// ******************************************************************
XBSYSAPI EXPORTNUM(68) void_xt NTAPI IoDeleteDevice
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * 0x0045 - IoDeleteSymbolicLink()
// ******************************************************************
XBSYSAPI EXPORTNUM(69) ntstatus_xt NTAPI IoDeleteSymbolicLink
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
XBSYSAPI EXPORTNUM(72) void_xt NTAPI IoFreeIrp
(
	IN PIRP Irp
);

// ******************************************************************
// * 0x0049 - IoInitializeIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(73) PVOID NTAPI IoInitializeIrp
(
	IN PIRP Irp,
	IN ushort_xt PacketSize,
	IN cchar_xt StackSize
);

// ******************************************************************
// * 0x004A - IoInvalidDeviceRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(74) ntstatus_xt NTAPI IoInvalidDeviceRequest
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

// ******************************************************************
// * 0x004B - IoQueryFileInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(75) ntstatus_xt NTAPI IoQueryFileInformation
(
	IN PFILE_OBJECT FileObject,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN ulong_xt Length,
	OUT PVOID FileInformation,
	OUT PULONG ReturnedLength
);

// ******************************************************************
// * 0x004C - IoQueryVolumeInformation()
// ******************************************************************
XBSYSAPI EXPORTNUM(76) ntstatus_xt NTAPI IoQueryVolumeInformation
(
	IN PFILE_OBJECT FileObject,
	IN FS_INFORMATION_CLASS FsInformationClass,
	IN ulong_xt Length,
	OUT PVOID FsInformation,
	OUT PULONG ReturnedLength
);

// ******************************************************************
// * 0x004D - IoQueueThreadIrp()
// ******************************************************************
XBSYSAPI EXPORTNUM(77) void_xt NTAPI IoQueueThreadIrp
(
	IN PIRP Irp
);

// ******************************************************************
// * 0x004E - IoRemoveShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(78) void_xt NTAPI IoRemoveShareAccess
(
	IN PFILE_OBJECT FileObject,
	IN PSHARE_ACCESS ShareAccess
);

// ******************************************************************
// * 0x004F - IoSetIoCompletion()
// ******************************************************************
XBSYSAPI EXPORTNUM(79) ntstatus_xt NTAPI IoSetIoCompletion
(
	IN PKQUEUE IoCompletion,
	IN PVOID KeyContext,
	IN PVOID ApcContext,
	IN ntstatus_xt IoStatus,
	IN ulong_xt IoStatusInformation
);

// ******************************************************************
// * 0x0050 - IoSetShareAccess()
// ******************************************************************
XBSYSAPI EXPORTNUM(80) void_xt NTAPI IoSetShareAccess
(
	IN access_mask_xt DesiredAccess,
	IN ulong_xt       DesiredShareAccess,
	IN PFILE_OBJECT   FileObject,
	OUT PSHARE_ACCESS ShareAccess
);

// ******************************************************************
// * 0x0051 - IoStartNextPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(81) void_xt NTAPI IoStartNextPacket
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * 0x0052 - IoStartNextPacketByKey()
// ******************************************************************
XBSYSAPI EXPORTNUM(82) void_xt NTAPI IoStartNextPacketByKey
(
	IN PDEVICE_OBJECT DeviceObject,
	IN ulong_xt Key
);

// ******************************************************************
// * 0x0053 - IoStartPacket()
// ******************************************************************
XBSYSAPI EXPORTNUM(83) void_xt NTAPI IoStartPacket
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	OUT PULONG Key OPTIONAL
);

// ******************************************************************
// * 0x0054 - IoSynchronousDeviceIoControlRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(84) ntstatus_xt NTAPI IoSynchronousDeviceIoControlRequest
(
	IN ulong_xt IoControlCode,
	IN PDEVICE_OBJECT DeviceObject,
	IN PVOID InputBuffer OPTIONAL,
	IN ulong_xt InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ulong_xt OutputBufferLength,
	OUT PULONG ReturnedOutputBufferLength OPTIONAL,
	IN boolean_xt InternalDeviceIoControl
);

// ******************************************************************
// * 0x0055 - IoSynchronousFsdRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(85) ntstatus_xt NTAPI IoSynchronousFsdRequest
(
	IN ulong_xt MajorFunction,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID Buffer OPTIONAL,
	IN ulong_xt Length,
	IN PLARGE_INTEGER StartingOffset OPTIONAL
);

// ******************************************************************
// * 0x0056 - IofCallDriver()
// ******************************************************************
XBSYSAPI EXPORTNUM(86) ntstatus_xt FASTCALL IofCallDriver
(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);

// ******************************************************************
// * 0x0057 - IofCompleteRequest()
// ******************************************************************
XBSYSAPI EXPORTNUM(87) void_xt FASTCALL IofCompleteRequest
(
	IN PIRP Irp,
	IN cchar_xt PriorityBoost
);

// ******************************************************************
// * 0x005A - IoDismountVolume()
// ******************************************************************
XBSYSAPI EXPORTNUM(90) ntstatus_xt NTAPI IoDismountVolume
(
	IN PDEVICE_OBJECT DeviceObject
);

// ******************************************************************
// * 0x005B - IoDismountVolumeByName()
// ******************************************************************
XBSYSAPI EXPORTNUM(91) ntstatus_xt NTAPI IoDismountVolumeByName
(
	IN PSTRING VolumeName
);

// ******************************************************************
// * 0x0167 - IoMarkIrpMustComplete()
// ******************************************************************
XBSYSAPI EXPORTNUM(359) cchar_xt NTAPI IoMarkIrpMustComplete
(
	IN PIRP Irp
);

}

#endif


