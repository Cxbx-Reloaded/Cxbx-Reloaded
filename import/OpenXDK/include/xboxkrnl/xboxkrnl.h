// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xboxkrnl.h
// *
// * note : XBox Kernel Declarations
// *
// ******************************************************************
#ifndef XBOXKRNL_H
#define XBOXKRNL_H

#if defined(__cplusplus)
extern "C"
{
#endif

// ******************************************************************
// * dll import/export
// ******************************************************************
#define DECLSPEC_IMPORT __declspec(dllimport)
#define DECLSPEC_EXPORT __declspec(dllexport)
#define DECLSPEC_EXTERN extern

// ******************************************************************
// * kernel exports, others either import or link locally
// ******************************************************************
#define XBSYSAPI DECLSPEC_IMPORT

#ifdef _XBOXKRNL_INTERNAL_
#undef  XBSYSAPI
#define XBSYSAPI DECLSPEC_EXPORT
#define KRNL(API) API
#endif
#ifdef _XBOXKRNL_DEFEXTRN_
#undef  XBSYSAPI
#define XBSYSAPI DECLSPEC_EXTERN
// The KRNL macro prevents naming collisions
#define KRNL(API) KRNL##API
#endif
#define RESTRICTED_POINTER
//TODO : When #define RESTRICTED_POINTER __restrict

// ******************************************************************
// * Null
// ******************************************************************
#ifndef NULL
#define NULL                0
#endif

// ******************************************************************
// * TRUE / FALSE
// ******************************************************************
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

// ******************************************************************
// * CONST
// ******************************************************************
#define CONST               const

// ******************************************************************
// * VOLATILE
// ******************************************************************
#undef VOLATILE
#define VOLATILE            volatile

// ******************************************************************
// * VOID
// ******************************************************************
#ifndef VOID
	typedef void                VOID;
	//#define VOID                void
#endif

// ******************************************************************
// * Basic types
// ******************************************************************
typedef char                CHAR, CCHAR;
typedef short               SHORT, CSHORT;
typedef long                LONG;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef unsigned char       BOOLEAN;
typedef unsigned short      USHORT;
typedef unsigned short      WORD;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef unsigned long       SIZE_T, *PSIZE_T;
typedef unsigned long       ACCESS_MASK, *PACCESS_MASK;
typedef unsigned long       PHYSICAL_ADDRESS;
typedef long                INT_PTR;
typedef signed __int64      LONGLONG;
typedef unsigned __int64    ULONGLONG;
typedef wchar_t             WCHAR;

// ******************************************************************
// * Pointer types
// ******************************************************************
typedef CHAR               *PCHAR;
typedef CHAR               *PCSZ;
typedef BYTE               *PBYTE;
typedef BOOLEAN            *PBOOLEAN;
typedef UCHAR              *PUCHAR;
typedef USHORT             *PUSHORT;
typedef ULONG              *PULONG;
typedef DWORD              *PDWORD, *LPDWORD;
typedef ACCESS_MASK        *PACCESS_MASK;
typedef LONG               *PLONG;
typedef long               *PINT_PTR;
typedef VOID               *PVOID, *LPVOID;
typedef void               *HANDLE;
typedef HANDLE             *PHANDLE;

// Additional basic and pointer types :
typedef __int32				LONG_PTR; // TODO : Update this declaration for 64 bit
typedef unsigned __int32	ULONG_PTR; // TODO : Update this declaration for 64 bit

typedef LONGLONG            *PLONGLONG;

// ******************************************************************
// ANSI (Multi-byte Character) types
// ******************************************************************
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;

typedef /*_Null_terminated_*/ CONST WCHAR *LPCWSTR, *PCWSTR;


// ******************************************************************
// * LPSECURITY_ATTRIBUTES
// ******************************************************************
typedef void* LPSECURITY_ATTRIBUTES;

// ******************************************************************
// * NTSTATUS
// ******************************************************************
typedef long                            NTSTATUS;

#define NT_SUCCESS(Status)              ((NTSTATUS) (Status) >= 0)
#define STATUS_SUCCESS                   ((DWORD   )0x00000000L)
#ifndef STATUS_PENDING
#define STATUS_PENDING                   ((DWORD   )0x00000103L)
#endif
#define STATUS_TIMER_RESUME_IGNORED      ((DWORD   )0x40000025L)
#define STATUS_UNSUCCESSFUL              ((DWORD   )0xC0000001)
#define STATUS_UNRECOGNIZED_MEDIA        ((DWORD   )0xC0000014)
#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)
#endif
#define STATUS_ALERTED                   ((DWORD   )0x00000101)
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)
// The SCSI input buffer was too large (not necessarily an error!)
#define STATUS_DATA_OVERRUN              ((DWORD   )0xC000003CL)
#define STATUS_INVALID_IMAGE_FORMAT      ((DWORD   )0xC000007BL)
#define STATUS_INSUFFICIENT_RESOURCES    ((DWORD   )0xC000009AL)
#define STATUS_TOO_MANY_SECRETS          ((DWORD   )0xC0000156L)
#define STATUS_XBE_REGION_MISMATCH       ((DWORD   )0xC0050001L)
#define STATUS_XBE_MEDIA_MISMATCH        ((DWORD   )0xC0050002L)
#define STATUS_OBJECT_NAME_NOT_FOUND     ((DWORD   )0xC0000034L)
#define STATUS_OBJECT_NAME_COLLISION     ((DWORD   )0xC0000035L)

// ******************************************************************
// * PAGE Masks
// ******************************************************************
#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define PAGE_GUARD             0x100
#define PAGE_NOCACHE           0x200
#define PAGE_WRITECOMBINE      0x400

// ******************************************************************
// * memory
// ******************************************************************

// Define virtual base and alternate virtual base of kernel.
#define KSEG0_BASE                  0x80000000

// Define virtual base addresses for physical memory windows.
#define MM_SYSTEM_PHYSICAL_MAP      KSEG0_BASE

#define MM_HIGHEST_PHYSICAL_PAGE    0x07FFF
#define MM_64M_PHYSICAL_PAGE        0x04000
#define MM_INSTANCE_PHYSICAL_PAGE   0x03FE0 // Chihiro arcade should use 0x07FF0

#define MM_INSTANCE_PAGE_COUNT      16

#define PAGE_SHIFT                  12

// Convert a physical frame number to its corresponding physical address.
#define MI_CONVERT_PFN_TO_PHYSICAL(Pfn) \
	((PCHAR)MM_SYSTEM_PHYSICAL_MAP + ((ULONG)(Pfn) << PAGE_SHIFT))

#define KERNEL_STACK_SIZE			12288 /* = 0x03000 */

#define PSP_MAX_CREATE_THREAD_NOTIFY 16 /* TODO : Should be 8 */

// ******************************************************************
// * calling conventions
// ******************************************************************
#define NTAPI               __stdcall
#ifndef CDECL
#define CDECL               __cdecl
#endif
#define FASTCALL            __fastcall
#define INLINE              __inline
#define DECLSPEC_NORETURN   __declspec(noreturn)

// ******************************************************************
// * documentation purposes only
// ******************************************************************
#define EXPORTNUM(ordinal)
#define IN
#define OUT
#define OPTIONAL
#define UNALIGNED

// ******************************************************************
// * KPROCESSOR_MODE
// ******************************************************************
typedef CCHAR KPROCESSOR_MODE;

// ******************************************************************
// * KWAIT_REASON
// ******************************************************************
typedef enum _KWAIT_REASON {
	Executive,
	FreePage,
	PageIn,
	PoolAllocation,
	DelayExecution,
	Suspended,
	UserRequest,
	WrExecutive,
	WrFreePage,
	WrPageIn,
	WrPoolAllocation,
	WrDelayExecution,
	WrSuspended,
	WrUserRequest,
	WrEventPair,
	WrQueue,
	WrLpcReceive,
	WrLpcReply,
	WrVirtualMemory,
	WrPageOut,
	WrRendezvous,
	WrFsCacheIn,
	WrFsCacheOut,
	Spare4,
	Spare5,
	Spare6,
	WrKernel,
	MaximumWaitReason
} KWAIT_REASON;

// ******************************************************************
// * MODE
// ******************************************************************
typedef enum _MODE
{
    KernelMode,
    UserMode,
    MaximumMode
}
MODE;

// ******************************************************************
// * WAIT_TYPE
// ******************************************************************
typedef enum _WAIT_TYPE
{
    WaitAll = 0,
    WaitAny = 1
}
WAIT_TYPE;

// ******************************************************************
// * LARGE_INTEGER
// ******************************************************************
typedef union _LARGE_INTEGER
{
    struct
    {
        DWORD   LowPart;
        LONG    HighPart;
    }
    u;

    LONGLONG QuadPart;
}
LARGE_INTEGER, *PLARGE_INTEGER;

// ******************************************************************
// * ULARGE_INTEGER
// ******************************************************************
typedef union _ULARGE_INTEGER
{
    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    }
    u1;

    struct
    {
        DWORD LowPart;
        DWORD HighPart;
    }
    u;

    ULONGLONG QuadPart;
}
ULARGE_INTEGER, *PULARGE_INTEGER;

// ******************************************************************
// * STRING
// ******************************************************************
typedef struct _STRING
{
    USHORT  Length;
    USHORT  MaximumLength;
    PCHAR   Buffer;
}
STRING, ANSI_STRING, *PSTRING, *PANSI_STRING;

typedef STRING OBJECT_STRING;

// ******************************************************************
// * UNICODE_STRING
// ******************************************************************
typedef struct _UNICODE_STRING
{
    USHORT  Length;
    USHORT  MaximumLength;
    USHORT *Buffer;
}
UNICODE_STRING, *PUNICODE_STRING;

// ******************************************************************
// * LIST_ENTRY
// ******************************************************************
typedef struct _LIST_ENTRY
{
    struct _LIST_ENTRY *Flink;
    struct _LIST_ENTRY *Blink;
}
LIST_ENTRY, *PLIST_ENTRY;

typedef struct _SINGLE_LIST_ENTRY {
	struct _SINGLE_LIST_ENTRY  *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY, SLIST_ENTRY, *PSLIST_ENTRY;

/*
 * Disabled as Cxbx-Reloaded does not support Win64 compilation
 * Win64 is not possible while using direct code execution, unless we add
 * X86-X64 translation
#if defined(_WIN64)

//
// The type SINGLE_LIST_ENTRY is not suitable for use with SLISTs.  For
// WIN64, an entry on an SLIST is required to be 16-byte aligned, while a
// SINGLE_LIST_ENTRY structure has only 8 byte alignment.
//
// Therefore, all SLIST code should use the SLIST_ENTRY type instead of the
// SINGLE_LIST_ENTRY type.
//

#pragma warning(push)
#pragma warning(disable:4324)   // structure padded due to align()

typedef struct DECLSPEC_ALIGN(16) _SLIST_ENTRY {
	struct _SLIST_ENTRY *Next;
} SLIST_ENTRY, *PSLIST_ENTRY;

#pragma warning(pop)

#else

typedef _SLIST_ENTRY {
	struct _SLIST_ENTRY *Next;
} SLIST_ENTRY, *PSLIST_ENTRY;

#endif // _WIN64

*/
typedef union _SLIST_HEADER {
	ULONGLONG Alignment;
	struct {
		SLIST_ENTRY Next;
		WORD   Depth;
		WORD   CpuId;
	} DUMMYSTRUCTNAME;
} SLIST_HEADER, *PSLIST_HEADER;

// ******************************************************************
// * FILE_FS_SIZE_INFORMATION
// ******************************************************************
typedef struct _FILE_FS_SIZE_INFORMATION
{
    LARGE_INTEGER   TotalAllocationUnits;
    LARGE_INTEGER   AvailableAllocationUnits;
    ULONG           SectorsPerAllocationUnit;
    ULONG           BytesPerSector;
}
FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

// ******************************************************************
// * FILE_INFORMATION_CLASS
// ******************************************************************
typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation        = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileTrackingInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
}
FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

// ******************************************************************
// * CreateDisposition Values for NtCreateFile
// ******************************************************************
#define FILE_SUPERSEDE                          0x00000000
#define FILE_OPEN                               0x00000001
#define FILE_CREATE                             0x00000002
#define FILE_OPEN_IF                            0x00000003
#define FILE_OVERWRITE                          0x00000004
#define FILE_OVERWRITE_IF                       0x00000005
#define FILE_MAXIMUM_DISPOSITION                0x00000005

// ******************************************************************
// * CreateOption Values for NtCreateFile
// ******************************************************************
// FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT is what CreateFile
// uses for most things when translating to NtCreateFile.
#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080
#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_FOR_RECOVERY                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800
#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000
#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000
#define FILE_COPY_STRUCTURED_STORAGE            0x00000041
#define FILE_STRUCTURED_STORAGE                 0x00000441
#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036

// ******************************************************************
// * OBJECT_ATTRIBUTES
// ******************************************************************
typedef struct _OBJECT_ATTRIBUTES
{
    HANDLE  RootDirectory;
    PSTRING ObjectName;
    ULONG   Attributes;
}
OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

// ******************************************************************
// * OBJECT_TYPE
// ******************************************************************
typedef struct _OBJECT_TYPE
{
	/* TODO : Declare missing types and enable corresponding members :
	OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
	*/
	PVOID DefaultObject;
	ULONG PoolTag;
}
OBJECT_TYPE, *POBJECT_TYPE;

// Source : DXBX
typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

// Source : DXBX
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
}
FILETIME, *PFILETIME;

// Source : DXBX (Xbox Refurb Info)
typedef struct _XBOX_REFURB_INFO
{
	DWORD Signature;
	DWORD PowerCycleCount;
	FILETIME FirstBootTime;
}
XBOX_REFURB_INFO, *PXBOX_REFURB_INFO;

#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

// Source : DXBX
typedef struct _EXCEPTION_RECORD
{
	DWORD ExceptionCode;
	DWORD ExceptionFlags;
	_EXCEPTION_RECORD *ExceptionRecord;
	VOID *ExceptionAddress;
	DWORD NumberParameters;
	ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
}
EXCEPTION_RECORD, *PEXCEPTION_RECORD;

// ******************************************************************
// * FSINFOCLASS
// ******************************************************************
typedef enum _FSINFOCLASS
{
    FileFsVolumeInformation       = 1,
    FileFsLabelInformation,      // 2
    FileFsSizeInformation,       // 3
    FileFsDeviceInformation,     // 4
    FileFsAttributeInformation,  // 5
    FileFsControlInformation,    // 6
    FileFsFullSizeInformation,   // 7
    FileFsObjectIdInformation,   // 8
    FileFsMaximumInformation
}
FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

// ******************************************************************
// * FILE_DIRECTORY_INFORMATION
// ******************************************************************
typedef struct _FILE_DIRECTORY_INFORMATION
{
    ULONG           NextEntryOffset;
    ULONG           FileIndex;
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   ChangeTime;
    LARGE_INTEGER   EndOfFile;
    LARGE_INTEGER   AllocationSize;
    ULONG           FileAttributes;
    ULONG           FileNameLength;
    CHAR            FileName[1];        // Offset: 0x40
}
FILE_DIRECTORY_INFORMATION;

// ******************************************************************
// * KSYSTEM_TIME
// ******************************************************************
typedef struct _KSYSTEM_TIME
{
	/* 0x00 */ ULONG LowPart;
	/* 0x04 */ LONG High1Time;
	/* 0x08 */ LONG High2Time;
} // Size = 0x0C
KSYSTEM_TIME, *PKSYSTEM_TIME;

// ******************************************************************
// * MM_STATISTICS
// ******************************************************************
typedef struct _MM_STATISTICS
{
    ULONG   Length;
    ULONG   TotalPhysicalPages;
    ULONG   AvailablePages;
    ULONG   VirtualMemoryBytesCommitted;
    ULONG   VirtualMemoryBytesReserved;
    ULONG   CachePagesCommitted;
    ULONG   PoolPagesCommitted;
    ULONG   StackPagesCommitted;
    ULONG   ImagePagesCommitted;
}
MM_STATISTICS, *PMM_STATISTICS;

// ******************************************************************
// * IO_STATUS_BLOCK *Same as Win2k/XP*
// ******************************************************************
typedef struct _IO_STATUS_BLOCK
{
    union
    {
        NTSTATUS Status;
        PVOID    Pointer;
    }
    u1;

    ULONG_PTR Information;
}
IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

// ******************************************************************
// * IO_APC_ROUTINE
// ******************************************************************
typedef VOID (NTAPI *PIO_APC_ROUTINE)
(
    IN PVOID            ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG            Reserved
);

// ******************************************************************
// * MEMORY_BASIC_INFORMATION *Same as Win2k/XP*
// ******************************************************************
typedef struct _MEMORY_BASIC_INFORMATION
{
    PVOID   BaseAddress;
    PVOID   AllocationBase;
    DWORD   AllocationProtect;
    SIZE_T  RegionSize;
    DWORD   State;
    DWORD   Protect;
    DWORD   Type;
}
MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

// ******************************************************************
// * EVENT_TYPE
// ******************************************************************
typedef enum _EVENT_TYPE
{
    NotificationEvent = 0,
    SynchronizationEvent
}
EVENT_TYPE;

// ******************************************************************
// * BUS_DATA_TYPE
// ******************************************************************
typedef enum _BUS_DATA_TYPE
{
    ConfigurationSpaceUndefined = 0xFF,
    Cmos                        = 0x0,
    EisaConfiguration           = 0x1,
    Pos                         = 0x2,
    CbusConfiguration           = 0x3,
    PCIConfiguration            = 0x4,
    VMEConfiguration            = 0x5,
    NuBusConfiguration          = 0x6,
    PCMCIAConfiguration         = 0x7,
    MPIConfiguration            = 0x8,
    MPSAConfiguration           = 0x9,
    PNPISAConfiguration         = 0xA,
    SgiInternalConfiguration    = 0xB,
    MaximumBusDataType          = 0xC,
}
BUS_DATA_TYPE;

// ******************************************************************
// * PCI_SLOT_NUMBER
// ******************************************************************
typedef struct _PCI_SLOT_NUMBER
{
    union
    {
        struct
        {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   Reserved:24;
        }
        bits;

        ULONG   AsULONG;
    }u;
}
PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER;

#define PCI_TYPE0_ADDRESSES             6
#define PCI_TYPE1_ADDRESSES             2
#define PCI_TYPE2_ADDRESSES             5

// ******************************************************************
// * PCI_COMMON_CONFIG
// ******************************************************************
typedef struct _PCI_COMMON_CONFIG
{
    USHORT  VendorID;                   // 0x00 (ro)
    USHORT  DeviceID;                   // 0x02 (ro)
    USHORT  Command;                    // 0x04 Device control
    USHORT  Status;                     // 0x06
    UCHAR   RevisionID;                 // 0x08 (ro)
    UCHAR   ProgIf;                     // 0x09 (ro)
    UCHAR   SubClass;                   // 0x0A (ro)
    UCHAR   BaseClass;                  // 0x0B (ro)
    UCHAR   CacheLineSize;              // 0x0C (ro+)
    UCHAR   LatencyTimer;               // 0x0D (ro+)
    UCHAR   HeaderType;                 // 0x0E (ro)
    UCHAR   BIST;                       // 0x0F Built in self test

    union
    {
        struct _PCI_HEADER_TYPE_0
        {
            ULONG   BaseAddresses[PCI_TYPE0_ADDRESSES]; // 0x10
            ULONG   CIS;
            USHORT  SubVendorID;
            USHORT  SubSystemID;
            ULONG   ROMBaseAddress;
            UCHAR   CapabilitiesPtr;
            UCHAR   Reserved1[3];
            ULONG   Reserved2;
            UCHAR   InterruptLine;      //
            UCHAR   InterruptPin;       // (ro)
            UCHAR   MinimumGrant;       // (ro)
            UCHAR   MaximumLatency;     // (ro)
        }
        type0;
    }u;

    UCHAR DeviceSpecific[192];

}
PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;

#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))

#define PCI_COMMON_HDR_LENGTH (FIELD_OFFSET (PCI_COMMON_CONFIG, DeviceSpecific))

#define PCI_MAX_DEVICES                     32
#define PCI_MAX_FUNCTION                    8
#define PCI_MAX_BRIDGE_NUMBER               0xFF
#define PCI_INVALID_VENDORID                0xFFFF

#define PCI_VENDOR_NVIDIA_CORPORATION       0x10DE

#define PCI_USB0_DEVICE_ID                  2
#define PCI_USB0_FUNCTION_ID                0
#define PCI_USB0_IRQ                        1
#define PCI_USB0_REGISTER_BASE              0xFED00000
#define PCI_USB0_REGISTER_LENGTH            0x00001000
#define PCI_USB0_OHCI_CONTROLLER            0x01C2

// ******************************************************************
// * RETURN_FIRMWARE
// ******************************************************************
typedef enum _RETURN_FIRMWARE
{
    ReturnFirmwareHalt          = 0x0,
    ReturnFirmwareReboot        = 0x1,
    ReturnFirmwareQuickReboot   = 0x2,
    ReturnFirmwareHard          = 0x3,
    ReturnFirmwareFatal         = 0x4,
    ReturnFirmwareAll           = 0x5
}
RETURN_FIRMWARE, *LPRETURN_FIRMWARE;

// ******************************************************************
// * LAUNCH_DATA_HEADER
// ******************************************************************
typedef struct _LAUNCH_DATA_HEADER
{
    DWORD   dwLaunchDataType;
    DWORD   dwTitleId;
    char    szLaunchPath[520];
    DWORD   dwFlags;
}
LAUNCH_DATA_HEADER, *PLAUNCH_DATA_HEADER;

// ******************************************************************
// * LAUNCH_DATA_PAGE
// ******************************************************************
typedef struct _LAUNCH_DATA_PAGE
{
    LAUNCH_DATA_HEADER  Header;
    UCHAR               Pad[492];
    UCHAR               LaunchData[3072];
}
LAUNCH_DATA_PAGE, *PLAUNCH_DATA_PAGE;

// ******************************************************************
// * DISPATCHER_HEADER
// ******************************************************************
typedef struct _DISPATCHER_HEADER
{
    UCHAR       Type;           // 0x00
    UCHAR       Absolute;       // 0x01
    UCHAR       Size;           // 0x02
    UCHAR       Inserted;       // 0x03
    LONG        SignalState;    // 0x04
    LIST_ENTRY  WaitListHead;   // 0x08
}
DISPATCHER_HEADER;

typedef struct _KEVENT {
	DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;

typedef struct _KSEMAPHORE {
	DISPATCHER_HEADER Header;
	LONG Limit;
} KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

typedef struct _ERWLOCK {
	LONG LockCount;
	ULONG WritersWaitingCount;
	ULONG ReadersWaitingCount;
	ULONG ReadersEntryCount;
	KEVENT WriterEvent;
	KSEMAPHORE ReaderSemaphore;
} ERWLOCK, *PERWLOCK;

typedef struct _KDEVICE_QUEUE {
	CSHORT Type;
	UCHAR Size;
	BOOLEAN Busy;
	LIST_ENTRY DeviceListHead;
} KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

typedef struct _DEVICE_OBJECT {
	CSHORT Type;
	USHORT Size;
	LONG ReferenceCount;
	struct _DRIVER_OBJECT *DriverObject;
	struct _DEVICE_OBJECT *MountedOrSelfDevice;
	struct _IRP *CurrentIrp;
	ULONG Flags;
	PVOID DeviceExtension;
	UCHAR DeviceType;
	UCHAR StartIoFlags;
	CCHAR StackSize;
	BOOLEAN DeletePending;
	ULONG SectorSize;
	ULONG AlignmentRequirement;
	KDEVICE_QUEUE DeviceQueue;
	KEVENT DeviceLock;
	ULONG StartIoKey;
} DEVICE_OBJECT, *PDEVICE_OBJECT;

// ******************************************************************
// * TIMER_TYPE
// ******************************************************************
typedef enum _TIMER_TYPE
{
    NotificationTimer     = 0,
    SynchronizationTimer  = 1
}
TIMER_TYPE;

// ******************************************************************
// * KTIMER (Timer Object)
// ******************************************************************
typedef struct _KTIMER
{
    DISPATCHER_HEADER   Header;           // 0x00
    ULARGE_INTEGER      DueTime;          // 0x10
    LIST_ENTRY          TimerListEntry;   // 0x18
    struct _KDPC       *Dpc;              // 0x20
    LONG                Period;           // 0x24
}
KTIMER, *PKTIMER;

// ******************************************************************
// * PKSTART_ROUTINE
// ******************************************************************
typedef VOID (NTAPI *PKSTART_ROUTINE)
(
    IN PVOID StartContext
);

// ******************************************************************
// * PKSYSTEM_ROUTINE
// ******************************************************************
// *
// * NOTE: Non-standard call. Similar to stdcall, but first argument
// *       must be located at ebp+4 before calling.
// *
// *       This is different from the NT version: 2 parameters as
// *       opposed to 1.
// *
// ******************************************************************
typedef VOID (*PKSYSTEM_ROUTINE)
(
	IN PKSTART_ROUTINE StartRoutine OPTIONAL,
	IN PVOID StartContext OPTIONAL
);

struct _KDPC;

// ******************************************************************
// * PKDEFERRED_ROUTINE
// ******************************************************************
typedef VOID (*PKDEFERRED_ROUTINE)
(
    IN struct _KDPC *Dpc,
    IN PVOID         DeferredContext,
    IN PVOID         SystemArgument1,
    IN PVOID         SystemArgument2
);

// ******************************************************************
// * KDPC (Deferred Procedure Call (DPC) Object)
// ******************************************************************
typedef struct _KDPC
{
    CSHORT              Type;               // 0x00
    BOOLEAN             Inserted;           // 0x02
    BYTE                Padding;            // 0x03
    LIST_ENTRY          DpcListEntry;       // 0x04
    PKDEFERRED_ROUTINE  DeferredRoutine;    // 0x0C
    PVOID               DeferredContext;
    PVOID               SystemArgument1;
    PVOID               SystemArgument2;
}
KDPC, *PKDPC;

// ******************************************************************
// * KOBJECTS
// ******************************************************************
typedef enum _KOBJECTS
{
    DpcObject = 0x13,
}
KOBJECTS, *PKOBJECTS;

// ******************************************************************
// * KINTERRUPR
// ******************************************************************
typedef struct _KINTERRUPT
{
	unsigned char     UnknownA[0x0C];
	unsigned char     KIRQL;
	unsigned char     PaddingA[0x03];
	unsigned char     UnknownB[0x08];
	unsigned char     ISR[0x58];
}
KINTERRUPT, *PKINTERRUPT;

// ******************************************************************
// * PKSERVICE_ROUTINE
// ******************************************************************
typedef void* PKSERVICE_ROUTINE;

typedef CHAR KIRQL;
typedef KIRQL* PKIRQL;

// ******************************************************************
// * KINTERRUPT_MODE
// ******************************************************************
typedef enum _KINTERRUPT_MODE
{
	LevelSensitive,
	Latched,
}
KINTERRUPT_MODE;

// ******************************************************************
// * RTL_CRITICAL_SECTION
// ******************************************************************
typedef struct _RTL_CRITICAL_SECTION
{
    DWORD               Unknown[4];                                     // 0x00
    LONG                LockCount;                                      // 0x10
    LONG                RecursionCount;                                 // 0x14
    HANDLE              OwningThread;                                   // 0x18
}
RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

// ******************************************************************
// * NT_TIB
// ******************************************************************
typedef struct _NT_TIB
{
    struct _EXCEPTION_REGISTRATION_RECORD  *ExceptionList;          // 0x00
    PVOID                                   StackBase;              // 0x04
    PVOID                                   StackLimit;             // 0x08
    PVOID                                   SubSystemTib;           // 0x0C
    union
    {
        PVOID FiberData;                                            // 0x10 for TIB
        ULONG Version;                                              // 0x10 for TEB (?)
    }
    u_a;
    PVOID                                   ArbitraryUserPointer;   // 0x14
    struct _NT_TIB                         *Self;                   // 0x18
}
NT_TIB, *PNT_TIB;

// ******************************************************************
// * KTHREAD
// ******************************************************************
// *
// * NOTE: INCOMPLETE!!
// *
// ******************************************************************
typedef struct _KTHREAD
{
    UCHAR           UnknownA[0x28];
    PVOID           TlsData;        // 0x28
    UCHAR           UnknownB[0xE4]; // 0x2C
}
KTHREAD, *PKTHREAD;

// ******************************************************************
// * ETHREAD
// ******************************************************************
// *
// * NOTE: INCOMPLETE!!
// *
// ******************************************************************
typedef struct _ETHREAD
{
    struct _KTHREAD Tcb;
    UCHAR           UnknownA[0x1C]; // 0x110
    DWORD           UniqueThread;   // 0x12C
}
ETHREAD, *PETHREAD;

// ******************************************************************
// * PCREATE_THREAD_NOTIFY_ROUTINE
// ******************************************************************
typedef VOID(*PCREATE_THREAD_NOTIFY_ROUTINE)
(
	IN PETHREAD Thread,
	IN HANDLE ThreadId,
	IN BOOLEAN Create
);

// ******************************************************************
// * KPCRB
// ******************************************************************
// *
// * NOTE: INCOMPLETE!!
// *
// ******************************************************************
typedef struct _KPRCB
{
    struct _KTHREAD* CurrentThread;                                 // 0x00, KPCR : 0x28
    struct _KTHREAD* NextThread;                                    // 0x04, KPCR : 0x2C
    struct _KTHREAD* IdleThread;                                    // 0x08, KPCR : 0x30

    // This is the total size of the structure (presumably)
    UCHAR            Unknown[0x250];                                // 0x0C, KPCR : 0x34
}
KPRCB, *PKPRCB;

// ******************************************************************
// * KPCR
// ******************************************************************
// *
// * NOTE: KPCR is the structure which exists at the FS: segment.
// *
// ******************************************************************
typedef struct _KPCR
{
    struct _NT_TIB  NtTib;                                          // 0x00
    struct _KPCR   *SelfPcr;                                        // 0x1C
    struct _KPRCB  *Prcb;                                           // 0x20
    UCHAR           Irql;                                           // 0x24
    struct _KPRCB   PrcbData;                                       // 0x28
}
KPCR, *PKPCR;

// ******************************************************************
// * EEPROM_INDEX
// ******************************************************************
typedef enum _EEPROM_INDEX
{
    EEPROM_LANGUAGE				= 0x7,
    EEPROM_VIDEO				= 0x8,
    EEPROM_AUDIO				= 0x9,
    EEPROM_P_CONTROL_GAMES		= 0xA,
    EEPROM_P_CONTROL_PASSWORD	= 0xB,
    EEPROM_P_CONTROL_MOVIES		= 0xC,
    EEPROM_ONLINE_IP_ADDRESS	= 0xD,
    EEPROM_ONLINE_DNS_ADDRESS	= 0xE,
    EEPROM_ONLINE_DEFAULT_GATEWAY_ADDRESS = 0xF,
    EEPROM_ONLINE_SUBNET_ADDRESS= 0x10,
    EEPROM_MISC					= 0x11,
    EEPROM_DVD_REGION			= 0x12,
    EEPROM_MAX_OS				= 0xFF,
    // Break
    EEPROM_FACTORY_SERIAL_NUMBER= 0x100,
    EEPROM_FACTORY_ETHERNET_ADDR= 0x101,
    EEPROM_FACTORY_ONLINE_KEY	= 0x102,
    EEPROM_FACTORY_AV_REGION	= 0x103,
    EEPROM_FACTORY_GAME_REGION	= 0x104,
    EEPROM_MAX_FACTORY			= 0x1FF,
    // Break
    EEPROM_ENCRYPTED_SECTION	= 0xFFFE,
    EEPROM_MAX_ALL				= 0xFFFF
}
EEPROM_INDEX, *PEEPROM_INDEX;

// ******************************************************************
// * XBOX_HARDWARE_INFO
// ******************************************************************
typedef struct _XBOX_HARDWARE_INFO
{
    ULONG Flags;
    UCHAR Unknown1;
    UCHAR Unknown2;
    UCHAR Unknown3;
    UCHAR Unknown4;
}
XBOX_HARDWARE_INFO;

const int XBOX_KEY_LENGTH = 16;
typedef UCHAR XBOX_KEY_DATA[XBOX_KEY_LENGTH];

// ******************************************************************
// * TIME_FIELDS
// ******************************************************************
typedef struct _TIME_FIELDS
{
    USHORT  Year;
    USHORT  Month;
    USHORT  Day;
    USHORT  Hour;
    USHORT  Minute;
    USHORT  Second;
    USHORT  Millisecond;
    USHORT  Weekday;
}
TIME_FIELDS, *PTIME_FIELDS;

// ******************************************************************
// * READ_REGISTER_UCHAR
// ******************************************************************
// *
// * Use this to access I/O mapped memory. Just a good standard.
// *
// ******************************************************************
INLINE static UCHAR READ_REGISTER_UCHAR(PUCHAR Address)
{
    return *(volatile UCHAR *)Address;
}

// ******************************************************************
// * READ_REGISTER_USHORT
// ******************************************************************
// *
// * Use this to access I/O mapped memory. Just a good standard.
// *
// ******************************************************************
INLINE static USHORT READ_REGISTER_USHORT(PUSHORT Address)
{
    return *(volatile USHORT *)Address;
}

// ******************************************************************
// * READ_REGISTER_ULONG
// ******************************************************************
// *
// * Use this to access I/O mapped memory. Just a good standard.
// *
// ******************************************************************
INLINE static ULONG READ_REGISTER_ULONG(PULONG Address)
{
    return *(volatile ULONG *)Address;
}

// ******************************************************************
// * WRITE_REGISTER_UCHAR
// ******************************************************************
// *
// * Use this to access I/O mapped memory (without this, writing a
// * value and then reading it back can produce an incorrect result
// * because the write may not be completed yet.)
// *
// ******************************************************************
static VOID WRITE_REGISTER_UCHAR(PVOID Address, UCHAR Value)
{
    __asm
    {
        mov edx, Address
        mov ah, Value
        mov [edx], ah
        lock or Address, edx
    };
}

// ******************************************************************
// * WRITE_REGISTER_USHORT
// ******************************************************************
// *
// * Use this to access I/O mapped memory (without this, writing a
// * value and then reading it back can produce an incorrect result
// * because the write may not be completed yet.)
// *
// ******************************************************************
static VOID WRITE_REGISTER_USHORT(PVOID Address, USHORT Value)
{
    __asm
    {
        mov edx, Address
        mov ax, Value
        mov [edx], ax
        lock or Address, edx
    };
}

// ******************************************************************
// * WRITE_REGISTER_ULONG
// ******************************************************************
// *
// * Use this to access I/O mapped memory (without this, writing a
// * value and then reading it back can produce an incorrect result
// * because the write may not be completed yet.)
// *
// ******************************************************************
static VOID WRITE_REGISTER_ULONG(PVOID Address, ULONG Value)
{
    __asm
    {
        mov edx, Address
        mov eax, Value
        mov [edx], eax
        lock or Address, edx
    };
}

typedef struct _SCSI_PASS_THROUGH_DIRECT {
	USHORT Length;
	UCHAR ScsiStatus;
	UCHAR PathId;
	UCHAR TargetId;
	UCHAR Lun;
	UCHAR CdbLength;
	UCHAR SenseInfoLength;
	UCHAR DataIn;
	ULONG DataTransferLength;
	ULONG TimeOutValue;
	PVOID DataBuffer;
	ULONG SenseInfoOffset;
	UCHAR Cdb[16];
}SCSI_PASS_THROUGH_DIRECT, *PSCSI_PASS_THROUGH_DIRECT;

typedef struct _MODE_PARAMETER_HEADER10 {
	UCHAR ModeDataLength[2];
	UCHAR MediumType;
	UCHAR DeviceSpecificParameter;
	UCHAR Reserved[2];
	UCHAR BlockDescriptorLength[2];
}MODE_PARAMETER_HEADER10, *PMODE_PARAMETER_HEADER10;

typedef struct _DVDX2_AUTHENTICATION_PAGE {
	UCHAR Unknown[2];
    UCHAR PartitionArea;
    UCHAR CDFValid;
    UCHAR Authentication;
	UCHAR Unknown2[3];
	ULONG Unknown3[3];
} DVDX2_AUTHENTICATION_PAGE, *PDVDX2_AUTHENTICATION_PAGE;

typedef struct _DVDX2_AUTHENTICATION {
    MODE_PARAMETER_HEADER10 Header;
    DVDX2_AUTHENTICATION_PAGE AuthenticationPage;
} DVDX2_AUTHENTICATION, *PDVDX2_AUTHENTICATION;

// ******************************************************************
// * Debug
// ******************************************************************
#include "dbg.h"

// ******************************************************************
// * Executive
// ******************************************************************
#include "ex.h"

// ******************************************************************
// * Hardware Abstraction Layer
// ******************************************************************
#include "hal.h"

// ******************************************************************
// * I/O Manager
// ******************************************************************
#include "io.h"

// ******************************************************************
// * Kernel
// ******************************************************************
#include "kernel.h"

// ******************************************************************
// * Memory Manager
// ******************************************************************
#include "mm.h"

// ******************************************************************
// * NT
// ******************************************************************
#include "nt.h"

// ******************************************************************
// * Object Manager
// ******************************************************************
#include "ob.h"

// ******************************************************************
// * Process Structure
// ******************************************************************
#include "ps.h"

// ******************************************************************
// * Run-time Library
// ******************************************************************
#include "rtl.h"

// ******************************************************************
// * XBox
// ******************************************************************
#include "xbox.h"

#if defined(__cplusplus)
}
#endif

#endif


