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

// ******************************************************************
// * kernel exports, others either import or link locally
// ******************************************************************
#define XBSYSAPI DECLSPEC_IMPORT
#ifdef _XBOXKRNL_INTERNAL_
#undef  XBSYSAPI
#define XBSYSAPI DECLSPEC_EXPORT
#endif
#ifdef _XBOXKRNL_DEFEXTRN_
#undef  XBSYSAPI
#define XBSYSAPI extern
#endif

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
typedef unsigned short      WCHAR;

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
typedef LONG               *PLONG, *LONG_PTR;
typedef ULONG              *ULONG_PTR;
typedef INT_PTR            *PINT_PTR;
typedef VOID               *PVOID, *LPVOID;
typedef void               *HANDLE;
typedef HANDLE             *PHANDLE;

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
// * calling conventions
// ******************************************************************
#define NTAPI               __stdcall
#define CDECL               __cdecl
#define INLINE              __inline
#define DECLSPEC_NORETURN   __declspec(noreturn)

// ******************************************************************
// * documentation purposes only
// ******************************************************************
#define EXPORTNUM(a)
#define UNALIGNED
#define OPTIONAL
#define IN
#define OUT

// ******************************************************************
// * KPROCESSOR_MODE
// ******************************************************************
typedef CCHAR KPROCESSOR_MODE;

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
// *
// * NOTE: Non-standard call. Similar to stdcall, but first argument
// *       must be located at ebp+4 before calling.
// *
// *       This is different from the NT version: 2 parameters as
// *       opposed to 1.
// *
// ******************************************************************
typedef VOID (NTAPI *PKSTART_ROUTINE)
(
    IN PVOID StartContext1,
    IN PVOID StartContext2
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
    UCHAR               Number;             // 0x02
    UCHAR               Importance;         // 0x03
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
// * RTL_CRITICAL_SECTION
// ******************************************************************
typedef struct _RTL_CRITICAL_SECTION
{
    DWORD               Unknown[4];                                     // 0x00
    LONG                LockCount;                                      // 0x10
    LONG                RecursionCount;                                 // 0x14
    ULONG               OwningThread;                                   // 0x18
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
    EEPROM_MISC = 0x11
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


