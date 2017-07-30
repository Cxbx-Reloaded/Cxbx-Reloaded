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
typedef unsigned __int64    QUAD; // 8 byte aligned 8 byte long

// ******************************************************************
// * Pointer types
// ******************************************************************
typedef CHAR               *PCHAR;
typedef char               *PSZ;
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
typedef QUAD                *PQUAD;

// ******************************************************************
// ANSI (Multi-byte Character) types
// ******************************************************************
typedef CHAR *PCHAR, *LPCH, *PCH;
typedef CONST CHAR *LPCCH, *PCCH;
typedef WCHAR *LPWSTR, *PWSTR;

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
// * Registry value types
// ******************************************************************
// Used in ExQueryNonVolatileSetting and ExSaveNonVolatileSetting
#define REG_NONE                    ( 0 )   // No defined value type.
#define REG_SZ                      ( 1 )   // A null - terminated string. This will be either a Unicode or an ANSI string, depending on whether you use the Unicode or ANSI functions.
#define REG_EXPAND_SZ               ( 2 )   // A null - terminated string that contains unexpanded references to environment variables (for example, "%PATH%"). It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions. To expand the environment variable references, use the ExpandEnvironmentStrings function.
#define REG_BINARY                  ( 3 )   // Binary data in any form.
#define REG_DWORD                   ( 4 )   // A 32 - bit number.
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // A 32 - bit number in little - endian format. Windows is designed to run on little - endian computer architectures. Therefore, this value is defined as REG_DWORD in the Windows header files.
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // A 32 - bit number in big - endian format. Some UNIX systems support big - endian architectures.
#define REG_LINK                    ( 6 )   // A null - terminated Unicode string that contains the target path of a symbolic link that was created by calling the RegCreateKeyEx function with REG_OPTION_CREATE_LINK.
#define REG_MULTI_SZ                ( 7 )   // A sequence of null - terminated strings, terminated by an empty string(\0). String1\0String2\0String3\0LastString\0\0								 // The first \0 terminates the first string, the second to the last \0 terminates the last string, and the final \0 terminates the sequence.Note that the final terminator must be factored into the length of the string.
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )

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
// * MEM Masks
// ******************************************************************
#define MEM_COMMIT                  0x1000      
#define MEM_RESERVE                 0x2000      
#define MEM_DECOMMIT                0x4000      
#define MEM_RELEASE                 0x8000      
#define MEM_FREE                    0x10000     
#define MEM_PRIVATE                 0x20000     
#define MEM_MAPPED                  0x40000     
//#define MEM_RESET                 0x80000     
#define MEM_TOP_DOWN                0x100000     
#define MEM_WRITE_WATCH             0x200000    
#define MEM_PHYSICAL                0x400000    
#define MEM_NOZERO                  0x800000 // Replaces MEM_ROTATE on WinXP+
//#define MEM_IMAGE                 0x1000000 // Declare like below, to prevent warning C4005: 'MEM_IMAGE': macro redefinition
#define SEC_IMAGE                   0x1000000     
#define MEM_IMAGE                   SEC_IMAGE    

// ******************************************************************
// * memory
// ******************************************************************

#define PAGE_SHIFT                  12

// Xbox pages are (1 << 12) = 0x00001000 = 4096 bytes in size.
#define PAGE_SIZE                   (1 << PAGE_SHIFT)
#define PAGE_MASK                   (PAGE_SIZE - 1)

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
	Executive = 0,
	FreePage = 1,
	PageIn = 2,
	PoolAllocation = 3,
	DelayExecution = 4,
	Suspended = 5,
	UserRequest = 6,
	WrExecutive = 7,
	WrFreePage = 8,
	WrPageIn = 9,
	WrPoolAllocation = 10,
	WrDelayExecution = 11,
	WrSuspended = 12,
	WrUserRequest = 13,
	WrEventPair = 14,
	WrQueue = 15,
	WrLpcReceive = 16,
	WrLpcReply = 17,
	WrVirtualMemory = 18,
	WrPageOut = 19,
	WrRendezvous = 20,
	WrFsCacheIn = 21,
	WrFsCacheOut = 22,
	Spare4 = 23,
	Spare5 = 24,
	Spare6 = 25,
	WrKernel = 26,
	MaximumWaitReason = 27
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

// ******************************************************************
// * OBJECT_STRING
// ******************************************************************
typedef STRING OBJECT_STRING, *POBJECT_STRING;

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
// ******************************************************************
// * SLIST_HEADER
// ******************************************************************
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
// * FILE_FS_VOLUME_INFORMATION
// ******************************************************************
typedef struct _FILE_FS_VOLUME_INFORMATION {
	LARGE_INTEGER	VolumeCreationTime;
	ULONG			VolumeSerialNumber;
	ULONG			VolumeLabelLength;
	BOOLEAN			SupportsObjects;
	CHAR			VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

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
// * File attribute flags
// ******************************************************************
#define FILE_ATTRIBUTE_READONLY                 0x00000001
#define FILE_ATTRIBUTE_HIDDEN                   0x00000002
#define FILE_ATTRIBUTE_SYSTEM                   0x00000004
#define FILE_ATTRIBUTE_DIRECTORY                0x00000010
#define FILE_ATTRIBUTE_ARCHIVE                  0x00000020
#define FILE_ATTRIBUTE_DEVICE                   0x00000040
#define FILE_ATTRIBUTE_NORMAL                   0x00000080
#define FILE_ATTRIBUTE_TEMPORARY                0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE              0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT            0x00000400
#define FILE_ATTRIBUTE_COMPRESSED               0x00000800
#define FILE_ATTRIBUTE_OFFLINE                  0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED      0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED                0x00004000

#define FILE_ATTRIBUTE_CONTENT_INDEXED          FILE_ATTRIBUTE_NOT_CONTENT_INDEXED

#define FILE_ATTRIBUTE_VALID_FLAGS              0x00007fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS          0x000031a7


// ******************************************************************
// * GENERIC_MAPPING
// ******************************************************************
typedef struct _GENERIC_MAPPING
{
	ACCESS_MASK GenericRead;
	ACCESS_MASK GenericWrite;
	ACCESS_MASK GenericExecute;
	ACCESS_MASK GenericAll;
}
GENERIC_MAPPING, *PGENERIC_MAPPING;

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
typedef PVOID(NTAPI *OB_ALLOCATE_METHOD)(
	IN SIZE_T NumberOfBytes,
	IN ULONG Tag
	);

typedef VOID(NTAPI *OB_FREE_METHOD)(
	IN PVOID Pointer
	);

typedef VOID(NTAPI *OB_CLOSE_METHOD)(
	IN PVOID Object,
	IN ULONG SystemHandleCount
	);

typedef VOID(NTAPI *OB_DELETE_METHOD)(
	IN PVOID Object
	);

typedef NTSTATUS(NTAPI *OB_PARSE_METHOD)(
	IN PVOID ParseObject,
	IN struct _OBJECT_TYPE *ObjectType,
	IN ULONG Attributes,
	IN OUT POBJECT_STRING CompleteName,
	IN OUT POBJECT_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	OUT PVOID *Object
	);

typedef struct _OBJECT_TYPE
{
	OB_ALLOCATE_METHOD AllocateProcedure;
    OB_FREE_METHOD FreeProcedure;
    OB_CLOSE_METHOD CloseProcedure;
    OB_DELETE_METHOD DeleteProcedure;
    OB_PARSE_METHOD ParseProcedure;
	PVOID DefaultObject;
	ULONG PoolTag;
}
OBJECT_TYPE, *POBJECT_TYPE;

// ******************************************************************
// * OBJECT_HEADER
// ******************************************************************
typedef struct _OBJECT_HEADER {
	LONG PointerCount;
	LONG HandleCount;
	POBJECT_TYPE Type;
	ULONG Flags;
	QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

// Source : DXBX
typedef ULONG_PTR KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

// ******************************************************************
// * FILETIME
// ******************************************************************
// Source : DXBX
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
}
FILETIME, *PFILETIME;

// ******************************************************************
// * XBOX_REFURB_INFO
// ******************************************************************
// Source : DXBX (Xbox Refurb Info)
typedef struct _XBOX_REFURB_INFO
{
	DWORD Signature;
	DWORD PowerCycleCount;
	FILETIME FirstBootTime;
}
XBOX_REFURB_INFO, *PXBOX_REFURB_INFO;

#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

// ******************************************************************
// * EXCEPTION_RECORD
// ******************************************************************
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
// * FILE_RENAME_INFORMATION
// ******************************************************************
typedef struct _FILE_RENAME_INFORMATION
{
	BOOLEAN         ReplaceIfExists;
	HANDLE          RootDirectory;
	OBJECT_STRING   FileName;
}
FILE_RENAME_INFORMATION;

// ******************************************************************
// * FILE_LINK_INFORMATION
// ******************************************************************
typedef struct _FILE_LINK_INFORMATION {
	BOOLEAN         ReplaceIfExists;
	HANDLE          RootDirectory;
	ULONG           FileNameLength;
	CHAR            FileName[1];
} FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;

// ******************************************************************
// * FILE_NETWORK_OPEN_INFORMATION
// ******************************************************************
typedef struct _FILE_NETWORK_OPEN_INFORMATION {
	LARGE_INTEGER   CreationTime;
	LARGE_INTEGER   LastAccessTime;
	LARGE_INTEGER   LastWriteTime;
	LARGE_INTEGER   ChangeTime;
	LARGE_INTEGER   AllocationSize;
	LARGE_INTEGER   EndOfFile;
	ULONG           FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

// ******************************************************************
// * FILE_FULL_EA_INFORMATION
// ******************************************************************
typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

// ******************************************************************
// * FILE_BASIC_INFORMATION
// ******************************************************************
typedef struct _FILE_BASIC_INFORMATION {
	LARGE_INTEGER   CreationTime;
	LARGE_INTEGER   LastAccessTime;
	LARGE_INTEGER   LastWriteTime;
	LARGE_INTEGER   ChangeTime;
	ULONG           FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

// ******************************************************************
// * FILE_STANDARD_INFORMATION
// ******************************************************************
typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER   AllocationSize;
	LARGE_INTEGER   EndOfFile;
	ULONG           NumberOfLinks;
	BOOLEAN         DeletePending;
	BOOLEAN         Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

// ******************************************************************
// * FILE_INTERNAL_INFORMATION
// ******************************************************************
typedef struct _FILE_INTERNAL_INFORMATION {
	LARGE_INTEGER   IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

// ******************************************************************
// * FILE_EA_INFORMATION
// ******************************************************************
typedef struct _FILE_EA_INFORMATION {
	ULONG           EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

// ******************************************************************
// * FILE_ACCESS_INFORMATION
// ******************************************************************
typedef struct _FILE_ACCESS_INFORMATION {
	ACCESS_MASK     AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

// ******************************************************************
// * FILE_POSITION_INFORMATION
// ******************************************************************
typedef struct _FILE_POSITION_INFORMATION {
	LARGE_INTEGER   CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

// ******************************************************************
// * FILE_MODE_INFORMATION
// ******************************************************************
typedef struct _FILE_MODE_INFORMATION {
	ULONG           Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

// ******************************************************************
// * FILE_ALIGNMENT_INFORMATION
// ******************************************************************
typedef struct _FILE_ALIGNMENT_INFORMATION {
	ULONG           AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

// ******************************************************************
// * FILE_NAME_INFORMATION
// ******************************************************************
typedef struct _FILE_NAME_INFORMATION {
	ULONG           FileNameLength;
	CHAR            FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

// ******************************************************************
// * FILE_ALL_INFORMATION
// ******************************************************************
typedef struct _FILE_ALL_INFORMATION {
	FILE_BASIC_INFORMATION     BasicInformation;
	FILE_STANDARD_INFORMATION  StandardInformation;
	FILE_INTERNAL_INFORMATION  InternalInformation;
	FILE_EA_INFORMATION        EaInformation;
	FILE_ACCESS_INFORMATION    AccessInformation;
	FILE_POSITION_INFORMATION  PositionInformation;
	FILE_MODE_INFORMATION      ModeInformation;
	FILE_ALIGNMENT_INFORMATION AlignmentInformation;
	FILE_NAME_INFORMATION      NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

// ******************************************************************
// * FILE_DISPOSITION_INFORMATION
// ******************************************************************
typedef struct _FILE_DISPOSITION_INFORMATION {
	BOOLEAN         DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

// ******************************************************************
// * FILE_ALLOCATION_INFORMATION
// ******************************************************************
typedef struct _FILE_ALLOCATION_INFORMATION {
	LARGE_INTEGER   AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

// ******************************************************************
// * FILE_COMPRESSION_INFORMATION
// ******************************************************************
typedef struct _FILE_COMPRESSION_INFORMATION {
	LARGE_INTEGER   CompressedFileSize;
	USHORT          CompressionFormat;
	UCHAR           CompressionUnitShift;
	UCHAR           ChunkShift;
	UCHAR           ClusterShift;
	UCHAR           Reserved[3];
} FILE_COMPRESSION_INFORMATION, *PFILE_COMPRESSION_INFORMATION;

// ******************************************************************
// * FILE_END_OF_FILE_INFORMATION
// ******************************************************************
typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER   EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

// ******************************************************************
// * FILE_MOVE_CLUSTER_INFORMATION
// ******************************************************************
typedef struct _FILE_MOVE_CLUSTER_INFORMATION {
	ULONG ClusterCount;
	HANDLE RootDirectory;
	ULONG FileNameLength;
	CHAR FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION, *PFILE_MOVE_CLUSTER_INFORMATION;

// ******************************************************************
// * FILE_STREAM_INFORMATION
// ******************************************************************
typedef struct _FILE_STREAM_INFORMATION {
	ULONG           NextEntryOffset;
	ULONG           StreamNameLength;
	LARGE_INTEGER   StreamSize;
	LARGE_INTEGER   StreamAllocationSize;
	CHAR            StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

// ******************************************************************
// * FILE_TRACKING_INFORMATION
// ******************************************************************
typedef struct _FILE_TRACKING_INFORMATION {
	HANDLE          DestinationFile;
	ULONG           ObjectInformationLength;
	CHAR            ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

// ******************************************************************
// * FILE_COMPLETION_INFORMATION
// ******************************************************************
typedef struct _FILE_COMPLETION_INFORMATION {
	HANDLE          Port;
	PVOID           Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

// ******************************************************************
// * FILE_PIPE_INFORMATION
// ******************************************************************
typedef struct _FILE_PIPE_INFORMATION {
	ULONG           ReadMode;
	ULONG           CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

// ******************************************************************
// * FILE_PIPE_LOCAL_INFORMATION
// ******************************************************************
typedef struct _FILE_PIPE_LOCAL_INFORMATION {
	ULONG           NamedPipeType;
	ULONG           NamedPipeConfiguration;
	ULONG           MaximumInstances;
	ULONG           CurrentInstances;
	ULONG           InboundQuota;
	ULONG           ReadDataAvailable;
	ULONG           OutboundQuota;
	ULONG           WriteQuotaAvailable;
	ULONG           NamedPipeState;
	ULONG           NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

// ******************************************************************
// * FILE_PIPE_REMOTE_INFORMATION
// ******************************************************************
typedef struct _FILE_PIPE_REMOTE_INFORMATION {
	LARGE_INTEGER   CollectDataTime;
	ULONG           MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

// ******************************************************************
// * FILE_MAILSLOT_QUERY_INFORMATION
// ******************************************************************
typedef struct _FILE_MAILSLOT_QUERY_INFORMATION {
	ULONG           MaximumMessageSize;
	ULONG           MailslotQuota;
	ULONG           NextMessageSize;
	ULONG           MessagesAvailable;
	LARGE_INTEGER   ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION, *PFILE_MAILSLOT_QUERY_INFORMATION;

// ******************************************************************
// * FILE_MAILSLOT_SET_INFORMATION
// ******************************************************************
typedef struct _FILE_MAILSLOT_SET_INFORMATION {
	PLARGE_INTEGER  ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION, *PFILE_MAILSLOT_SET_INFORMATION;

// ******************************************************************
// * FILE_REPARSE_POINT_INFORMATION
// ******************************************************************
typedef struct _FILE_REPARSE_POINT_INFORMATION {
	LONGLONG        FileReference;
	ULONG           Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

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
	};

    ULONG_PTR Information;
}
IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

// ******************************************************************
// * PIO_APC_ROUTINE
// ******************************************************************
typedef VOID (NTAPI *PIO_APC_ROUTINE)
(
    IN PVOID            ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG            Reserved
);

// ******************************************************************
// * PTIMER_APC_ROUTINE *Same as Win2k/XP*
// ******************************************************************
typedef VOID(NTAPI *PTIMER_APC_ROUTINE)
(
	IN PVOID	TimerContext,
	IN ULONG	TimerLowValue,
	IN LONG		TimerHighValue
);

// ******************************************************************
// * TIMER_BASIC_INFORMATION *Same as Win2k/XP*
// ******************************************************************
typedef struct _TIMER_BASIC_INFORMATION
{
	LARGE_INTEGER TimeRemaining;
	BOOLEAN SignalState;
}
TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;

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
// * EVENT_TYPE - same as Windows
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

#define PCI_TYPE1_ADDR_PORT     ((PULONG) 0xCF8)
#define PCI_TYPE1_DATA_PORT     0xCFC

// ******************************************************************
// * PCI_TYPE1_CFG_BITS
// ******************************************************************
typedef struct _PCI_TYPE1_CFG_BITS {
    union {
        struct {
            ULONG   Reserved1:2;
            ULONG   RegisterNumber:6;
            ULONG   FunctionNumber:3;
            ULONG   DeviceNumber:5;
            ULONG   BusNumber:8;
            ULONG   Reserved2:7;
            ULONG   Enable:1;
        } bits;

        ULONG   AsULONG;
    } u;
} PCI_TYPE1_CFG_BITS, *PPCI_TYPE1_CFG_BITS;

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

typedef LONG KPRIORITY;

// ******************************************************************
// * KEVENT
// ******************************************************************
typedef struct _KEVENT
{
	DISPATCHER_HEADER Header;
}
//KEVENT, *PKEVENT, *RESTRICTED_POINTER PRKEVENT;
KEVENT, *PKEVENT, *PRKEVENT; // even with undefined RESTRICTED_POINTER, this doesn't compile

// ******************************************************************
// EVENT_BASIC_INFORMATION - same as Windows
// ******************************************************************
typedef struct _EVENT_BASIC_INFORMATION
{
	EVENT_TYPE EventType;
	LONG EventState;
}
EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

// ******************************************************************
// KSEMAPHORE
// ******************************************************************
typedef struct _KSEMAPHORE
{
	DISPATCHER_HEADER Header;
	LONG Limit;
}
KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

// ******************************************************************
// SEMAPHORE_BASIC_INFORMATION - same as Windows
// ******************************************************************
typedef struct _SEMAPHORE_BASIC_INFORMATION
{
	LONG CurrentCount;
	LONG MaximumCount;
}
SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

// ******************************************************************
// MUTANT_BASIC_INFORMATION - same as Windows
// ******************************************************************
typedef struct _MUTANT_BASIC_INFORMATION
{
	LONG CurrentCount;
	BOOLEAN OwnedByCaller;
	BOOLEAN AbandonedState;
}
MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

// ******************************************************************
// ERWLOCK
// ******************************************************************
typedef struct _ERWLOCK
{
	LONG LockCount;
	ULONG WritersWaitingCount;
	ULONG ReadersWaitingCount;
	ULONG ReadersEntryCount;
	KEVENT WriterEvent;
	KSEMAPHORE ReaderSemaphore;
}
ERWLOCK, *PERWLOCK;

// ******************************************************************
// KDEVICE_QUEUE
// ******************************************************************
typedef struct _KDEVICE_QUEUE
{
	CSHORT Type;                // 0x00
	UCHAR Size;                 // 0x02
	BOOLEAN Busy;               // 0x04
	LIST_ENTRY DeviceListHead;  // 0x08
}
KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

typedef struct _KDEVICE_QUEUE_ENTRY 
{
	LIST_ENTRY DeviceListEntry;
	ULONG SortKey;
	BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY, *RESTRICTED_POINTER PRKDEVICE_QUEUE_ENTRY;

// ******************************************************************
// FILE_SEGMENT_ELEMENT
// ******************************************************************
typedef PVOID PFILE_SEGMENT_ELEMENT;

// ******************************************************************
// IRP
// ******************************************************************
typedef struct _IRP
{
	CSHORT                 Type;                // 0x00
	WORD                   Size;                // 0x02
	ULONG                  Flags;               // 0x04
	LIST_ENTRY             ThreadListEntry;     // 0x08
	IO_STATUS_BLOCK        IoStatus;            // 0x10
	CHAR                   StackCount;          // 0x18
	CHAR                   CurrentLocation;	    // 0x19
	UCHAR                  PendingReturned;     // 0x1A
	UCHAR                  Cancel;              // 0x1B
	PIO_STATUS_BLOCK       UserIosb;            // 0x1C
	PKEVENT                UserEvent;           // 0x20
	ULONGLONG              Overlay;	            // 0x28
	PVOID                  UserBuffer;          // 0x30
	PFILE_SEGMENT_ELEMENT  SegmentArray;        // 0x34
	ULONG                  LockedBufferLength;  // 0x38
	ULONGLONG              Tail;                // 0x3C
}
IRP, *PIRP;

// ******************************************************************
// DEVICE_OBJECT
// ******************************************************************
typedef struct _DEVICE_OBJECT
{
	CSHORT Type;
	USHORT Size;
	LONG ReferenceCount;
	struct _DRIVER_OBJECT *DriverObject;
	struct _DEVICE_OBJECT *MountedOrSelfDevice;
	PIRP CurrentIrp;
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
}
DEVICE_OBJECT, *PDEVICE_OBJECT;

// ******************************************************************
// PDRIVER_OBJECT
// ******************************************************************
typedef VOID *PDRIVER_OBJECT;

// ******************************************************************
// * IO_COMPLETION_CONTEXT
// ******************************************************************
typedef struct _IO_COMPLETION_CONTEXT
{
	PVOID Port;
	PVOID Key;
} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT;

// ******************************************************************
// * FILE_OBJECT
// ******************************************************************
typedef struct _FILE_OBJECT {
	CSHORT                    Type;               // 0x00

	BYTE                      DeletePending : 1;  // 0x02
	BYTE                      ReadAccess : 1;     // 0x02
	BYTE                      WriteAccess : 1;    // 0x02
	BYTE                      DeleteAccess : 1;   // 0x02
	BYTE                      SharedRead : 1;     // 0x02
	BYTE                      SharedWrite : 1;    // 0x02
	BYTE                      SharedDelete : 1;   // 0x02
	BYTE                      Reserved : 1;       // 0x02

	BYTE                      Flags;              // 0x03
	PDEVICE_OBJECT            DeviceObject;       // 0x04
	PVOID                     FsContext;          // 0x08
	PVOID                     FsContext2;         // 0x0C
	NTSTATUS                  FinalStatus;        // 0x10
	LARGE_INTEGER             CurrentByteOffset;  // 0x14
	struct _FILE_OBJECT *     RelatedFileObject;  // 0x1C
	PIO_COMPLETION_CONTEXT    CompletionContext;  // 0x20
	LONG                      LockCount;          // 0x24
	KEVENT                    Lock;               // 0x28
	KEVENT                    Event;              // 0x38
} FILE_OBJECT, *PFILE_OBJECT;

// ******************************************************************
// * SHARE_ACCESS
// ******************************************************************
typedef struct _SHARE_ACCESS {
	BYTE OpenCount;
	BYTE Readers;
	BYTE Writers;
	BYTE Deleters;
	BYTE SharedRead;
	BYTE SharedWrite;
	BYTE SharedDelete;
} SHARE_ACCESS, *PSHARE_ACCESS;

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
	union
	{
		UCHAR           Number;             // 0x02
		BOOLEAN         Inserted;           // 0x02
	};
	UCHAR               Importance;         // 0x03
    LIST_ENTRY          DpcListEntry;       // 0x04
    PKDEFERRED_ROUTINE  DeferredRoutine;    // 0x0C
    PVOID               DeferredContext;
    PVOID               SystemArgument1;
    PVOID               SystemArgument2;
}
KDPC, *PKDPC;

// ******************************************************************
// * KFLOATING_SAVE
// ******************************************************************
// See NtDll::FLOATING_SAVE_AREA
typedef struct _KFLOATING_SAVE
{
	ULONG   ControlWord;
	ULONG   StatusWord;
	// NtDll contains ULONG TagWord here
	ULONG   ErrorOffset;
	ULONG   ErrorSelector;
	ULONG   DataOffset;
	ULONG   DataSelector;
	ULONG   Cr0NpxState; // NtDll has RegisterArea[SIZE_OF_80387_REGISTERS];
	ULONG   Spare1; // NtDll calls this Spare0
}
KFLOATING_SAVE, *PKFLOATING_SAVE;

// ******************************************************************
// * KOBJECTS
// ******************************************************************
typedef enum _KOBJECTS
{
	MutantObject = 2,
	QueueObject = 4,
	SemaphoreObject = 5,
	TimerNotificationObject = 8,
	TimerSynchronizationObject = 9,
	ApcObject = 0x12,
	DpcObject = 0x13,
}
KOBJECTS, *PKOBJECTS;

// ******************************************************************
// * PKNORMAL_ROUTINE
// ******************************************************************
typedef VOID (*PKNORMAL_ROUTINE)
(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
);

// ******************************************************************
// * PKKERNEL_ROUTINE
// ******************************************************************
typedef VOID (*PKKERNEL_ROUTINE)
(
	IN struct _KAPC *Apc,
	IN OUT PKNORMAL_ROUTINE *NormalRoutine,
	IN OUT PVOID *NormalContext,
	IN OUT PVOID *SystemArgument1,
	IN OUT PVOID *SystemArgument2
);

// ******************************************************************
// * PKRUNDOWN_ROUTINE
// ******************************************************************
typedef VOID (*PKRUNDOWN_ROUTINE)
(
	IN struct _KAPC *Apc
);

// ******************************************************************
// * PKSYNCHRONIZE_ROUTINE
// ******************************************************************
typedef BOOLEAN (*PKSYNCHRONIZE_ROUTINE)
(
	IN PVOID SynchronizeContext
);

// ******************************************************************
// * PKSERVICE_ROUTINE
// ******************************************************************
typedef BOOLEAN (*PKSERVICE_ROUTINE)
(
	IN struct _KINTERRUPT *Interrupt,
	IN PVOID ServiceContext
);

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
// * IRQ (Interrupt ReQuest) Priority Levels
// ******************************************************************
#define DISPATCH_LEVEL 2
#define PROFILE_LEVEL 27

#define DISPATCH_SIZE 22

// ******************************************************************
// * KINTERRUPR
// ******************************************************************
typedef struct _KINTERRUPT
{
	/* 0x00= 0 */ PKSERVICE_ROUTINE ServiceRoutine;
	/* 0x04= 4 */ PVOID ServiceContext;
	/* 0x08= 8 */ ULONG BusInterruptLevel;
	/* 0x0C=12 */ ULONG Irql; // Was : unsigned char     KIRQL; unsigned char     PaddingA[0x03];
	/* 0x10=16 */ BOOLEAN Connected;
	/* 0x11=17 */ BOOLEAN ShareVector;
	/* 0x12=18 */ KINTERRUPT_MODE Mode;
	/* 0x14=20 */ ULONG ServiceCount;
	/* 0x18=24 */ ULONG DispatchCode[DISPATCH_SIZE]; // Same as old : unsigned char ISR[0x58];
}
KINTERRUPT, *PKINTERRUPT;

// ******************************************************************
// * IRQL (Interrupt ReQuest Level) (* same as on win *)
// ******************************************************************
typedef UCHAR KIRQL, *PKIRQL;

// ******************************************************************
// * PS_STATISTICS
// ******************************************************************
typedef struct _PS_STATISTICS
{
	ULONG Length;
	ULONG ThreadCount;
	ULONG HandleCount;
}
PS_STATISTICS, *PPS_STATISTICS;

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

// 	typedef struct _KGDTENTRY
// 	{
// 		WORD LimitLow;
// 		WORD BaseLow;
// 		ULONG HighWord;
// 	}
//  KGDTENTRY, *PKGDTENTRY;
// 
// 	typedef struct _KIDTENTRY
// 	{
// 		WORD Offset;
// 		WORD Selector;
// 		WORD Access;
// 		WORD ExtendedOffset;
// 	}
//  KIDTENTRY, *PKIDTENTRY;
// 
// 	typedef struct _KEXECUTE_OPTIONS
// 	{
// 		ULONG ExecuteDisable: 1;
// 		ULONG ExecuteEnable: 1;
// 		ULONG DisableThunkEmulation: 1;
// 		ULONG Permanent: 1;
// 		ULONG ExecuteDispatchEnable: 1;
// 		ULONG ImageDispatchEnable: 1;
// 		ULONG Spare: 2;
// 	}
//  KEXECUTE_OPTIONS, *PKEXECUTE_OPTIONS;

// ******************************************************************
// * KPROCESS
// ******************************************************************
typedef struct _KPROCESS
{
	/* 0x0/0 */ LIST_ENTRY ReadyListHead;
	/* 0x8/8 */ LIST_ENTRY ThreadListHead;
	/* 0x10/16 */ ULONG StackCount;
	/* 0x14/20 */ ULONG ThreadQuantum;
	/* 0x18/24 */ CHAR BasePriority;
	/* 0x19/25 */ CHAR DisableBoost;
	/* 0x1A/26 */ CHAR DisableQuantum;
	/* 0x1B/27 */ CHAR _padding;
}
KPROCESS, *PKPROCESS;

// ******************************************************************
// * KAPC_STATE
// ******************************************************************
typedef struct _KAPC_STATE
{
	LIST_ENTRY ApcListHead[2];
	PKPROCESS Process;
	UCHAR KernelApcInProgress;
	UCHAR KernelApcPending;
	UCHAR UserApcPending;
	UCHAR ApcQueueable;
}
KAPC_STATE, *PKAPC_STATE;

// ******************************************************************
// * KGATE
// ******************************************************************
typedef struct _KGATE
{
	DISPATCHER_HEADER Header;
}
KGATE, *PKGATE;

// ******************************************************************
// * KMUTANT
// ******************************************************************
typedef struct _KMUTANT {
	DISPATCHER_HEADER Header;
	LIST_ENTRY MutantListEntry;
	struct _KTHREAD *RESTRICTED_POINTER OwnerThread;
	BOOLEAN Abandoned;
} KMUTANT, *PKMUTANT, *RESTRICTED_POINTER PRKMUTANT;

// ******************************************************************
// * KQUEUE
// ******************************************************************
typedef struct _KQUEUE
{
	DISPATCHER_HEADER Header;
	LIST_ENTRY EntryListHead;
	ULONG CurrentCount;
	ULONG MaximumCount;
	LIST_ENTRY ThreadListHead;
}
KQUEUE, *PKQUEUE, *RESTRICTED_POINTER PRKQUEUE;

// ******************************************************************
// * EXCEPTION_DISPOSITION
// ******************************************************************
typedef enum _EXCEPTION_DISPOSITION
{
	ExceptionContinueExecution = 0,
	ExceptionContinueSearch = 1,
	ExceptionNestedException = 2,
	ExceptionCollidedUnwind = 3
}
EXCEPTION_DISPOSITION, *PEXCEPTION_DISPOSITION;

// ******************************************************************
// * EXCEPTION_REGISTRATION_RECORD
// ******************************************************************
typedef struct _EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD; // forward

typedef struct _EXCEPTION_REGISTRATION_RECORD
{
	PEXCEPTION_REGISTRATION_RECORD Next;
	PEXCEPTION_DISPOSITION Handler;
}
EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

// ******************************************************************
// * KTRAP_FRAME
// ******************************************************************
typedef struct _KTRAP_FRAME
{
	ULONG DbgEbp;
	ULONG DbgEip;
	ULONG DbgArgMark;
	ULONG DbgArgPointer;
	WORD TempSegCs;
	UCHAR Logging;
	UCHAR Reserved;
	ULONG TempEsp;
	ULONG Dr0;
	ULONG Dr1;
	ULONG Dr2;
	ULONG Dr3;
	ULONG Dr6;
	ULONG Dr7;
	ULONG SegGs;
	ULONG SegEs;
	ULONG SegDs;
	ULONG Edx;
	ULONG Ecx;
	ULONG Eax;
	ULONG PreviousPreviousMode;
	PEXCEPTION_REGISTRATION_RECORD ExceptionList;
	ULONG SegFs;
	ULONG Edi;
	ULONG Esi;
	ULONG Ebx;
	ULONG Ebp;
	ULONG ErrCode;
	ULONG Eip;
	ULONG SegCs;
	ULONG EFlags;
	ULONG HardwareEsp;
	ULONG HardwareSegSs;
	ULONG V86Es;
	ULONG V86Ds;
	ULONG V86Fs;
	ULONG V86Gs;
}
KTRAP_FRAME, *PKTRAP_FRAME;

typedef struct _KTHREAD KTHREAD, *PKTHREAD; // forward

// ******************************************************************
// * KWAIT_BLOCK
// ******************************************************************

typedef struct _KWAIT_BLOCK
{
	LIST_ENTRY WaitListEntry;
	PKTHREAD Thread;
	PVOID Object;
	struct _KWAIT_BLOCK *NextWaitBlock;
	WORD WaitKey;
	UCHAR WaitType;
	UCHAR SpareByte;
}
KWAIT_BLOCK, *PKWAIT_BLOCK;

// ******************************************************************
// * KAPC
// ******************************************************************
typedef struct _KAPC
{
	/* 0x0/0 */ USHORT Type;
	/* 0x2/2 */ KPROCESSOR_MODE ApcMode;
	/* 0x3/3 */ BOOLEAN Inserted;
	/* 0x4/4 */ PKTHREAD Thread;
	/* 0x8/8 */ LIST_ENTRY ApcListEntry;
	/* 0x10/16 */ PKKERNEL_ROUTINE KernelRoutine;
	/* 0x14/20 */ PKRUNDOWN_ROUTINE RundownRoutine;
	/* 0x18/24 */ PKNORMAL_ROUTINE NormalRoutine;
	/* 0x1C/28 */ PVOID NormalContext;
	/* 0x20/32 */ PVOID SystemArgument1;
	/* 0x24/36 */ PVOID SystemArgument2;
}
KAPC, *PKAPC, *RESTRICTED_POINTER PRKAPC;

// ******************************************************************
// * KTHREAD
// ******************************************************************
// *
// * NOTE: INCOMPLETE!!
// *
// ******************************************************************
typedef struct _KTHREAD
{
	/* 0x0/0 */ DISPATCHER_HEADER Header;
	/* 0x10/16 */ LIST_ENTRY MutantListHead;
	/* 0x18/24 */ unsigned long KernelTime;
	/* 0x1C/28 */ void *StackBase;
	/* 0x20/32 */ void *StackLimit;
	/* 0x24/36 */ void *KernelStack;
	/* 0x28/40 */ void *TlsData;
	/* 0x2C/44 */ CHAR State;
	/* 0x2D/45 */ CHAR Alerted[2];
	/* 0x2F/47 */ CHAR Alertable;
	/* 0x30/48 */ CHAR NpxState;
	/* 0x31/49 */ CHAR Saturation;
	/* 0x32/50 */ CHAR Priority;
	/* 0x33/51 */ CHAR Padding;
	/* 0x34/52 */ KAPC_STATE ApcState;
	/* 0x4C/76 */ ULONG ContextSwitches;
	/* 0x50/80 */ ULONG WaitStatus;
	/* 0x54/84 */ CHAR WaitIrql;
	/* 0x55/85 */ CHAR WaitMode;
	/* 0x56/86 */ CHAR WaitNext;
	/* 0x57/87 */ CHAR WaitReason;
	/* 0x58/88 */ PVOID WaitBlockList;
	/* 0x5C/92 */ LIST_ENTRY WaitListEntry;
	/* 0x64/100 */ ULONG WaitTime;
	/* 0x68/104 */ ULONG KernelApcDisable;
	/* 0x6C/108 */ ULONG Quantum;
	/* 0x70/112 */ CHAR BasePriority;
	/* 0x71/113 */ CHAR DecrementCount;
	/* 0x72/114 */ CHAR PriorityDecrement;
	/* 0x73/115 */ CHAR DisableBoost;
	/* 0x74/116 */ CHAR NpxIrql;
	/* 0x75/117 */ CHAR SuspendCount;
	/* 0x76/118 */ CHAR Preempted;
	/* 0x77/119 */ CHAR HasTerminated;
	/* 0x78/120 */ PVOID Queue;
	/* 0x7C/124 */ LIST_ENTRY QueueListEntry;
	/* 0x88/136 */ UCHAR rsvd1[4];
	/* 0x88/136 */ KTIMER Timer;
	/* 0xB0/176 */ KWAIT_BLOCK TimerWaitBlock;
	/* 0xC8/200 */ KAPC SuspendApc;
	/* 0xF0/240 */ KSEMAPHORE SuspendSemaphore;
	/* 0x104/260 */ LIST_ENTRY ThreadListEntry;
	/* 0x10C/268 */ UCHAR _padding[4];
}
KTHREAD, *PKTHREAD, *RESTRICTED_POINTER PRKTHREAD;

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
// * KPRCB (Kernel PRocesor Control Block)
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

	ULONG            Unknown1[7];                                   // 0x0C, KPCR : 0x34

	LIST_ENTRY       DpcListHead;                                   // 0x28, KPCR : 0x50
	ULONG            DpcRoutineActive;                              // 0x30, KPCR : 0x58

    // This completes the total size of the structure (presumably)
    UCHAR            Unknown[0x224];                            
}
KPRCB, *PKPRCB;


// ******************************************************************
// * KPCR (Kernel Processor Control Region)
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
// * XC_VALUE_INDEX
// ******************************************************************
typedef enum _XC_VALUE_INDEX
{
	XC_TIMEZONE_BIAS         = 0,
	XC_TZ_STD_NAME           = 1,
	XC_TZ_STD_DATE           = 2,
	XC_TZ_STD_BIAS           = 3,
	XC_TZ_DLT_NAME           = 4,
	XC_TZ_DLT_DATE           = 5,
	XC_TZ_DLT_BIAS           = 6,
	XC_LANGUAGE				 = 7,
    XC_VIDEO				 = 8,
    XC_AUDIO				 = 9,
    XC_P_CONTROL_GAMES		 = 0xA,
    XC_P_CONTROL_PASSWORD	 = 0xB,
    XC_P_CONTROL_MOVIES		 = 0xC,
    XC_ONLINE_IP_ADDRESS	 = 0xD,
    XC_ONLINE_DNS_ADDRESS	 = 0xE,
    XC_ONLINE_DEFAULT_GATEWAY_ADDRESS = 0xF,
    XC_ONLINE_SUBNET_ADDRESS = 0x10,
    XC_MISC					 = 0x11,
    XC_DVD_REGION			 = 0x12,
    XC_MAX_OS				 = 0xFF,
    // Break
	XC_FACTORY_START_INDEX   = 0x100,
	XC_FACTORY_SERIAL_NUMBER = XC_FACTORY_START_INDEX,
    XC_FACTORY_ETHERNET_ADDR = 0x101,
    XC_FACTORY_ONLINE_KEY	 = 0x102,
    XC_FACTORY_AV_REGION	 = 0x103,
    XC_FACTORY_GAME_REGION	 = 0x104,
    XC_MAX_FACTORY			 = 0x1FF,
    // Break
    XC_ENCRYPTED_SECTION	 = 0xFFFE,
    XC_MAX_ALL				 = 0xFFFF
}
XC_VALUE_INDEX, *PXC_VALUE_INDEX;

// ******************************************************************
// * XBOX_HARDWARE_INFO
// ******************************************************************
typedef struct _XBOX_HARDWARE_INFO
{
    ULONG Flags;
    UCHAR GpuRevision;
	UCHAR McpRevision;
    UCHAR Unknown3;
    UCHAR Unknown4;
}
XBOX_HARDWARE_INFO;

const int XBOX_KEY_LENGTH = 16;
const int ALTERNATE_SIGNATURE_COUNT = 16;
typedef UCHAR XBOX_KEY_DATA[XBOX_KEY_LENGTH];

// ******************************************************************
// * XBOX_ENCRYPTED_SETTINGS
// ******************************************************************
typedef struct _XBOX_ENCRYPTED_SETTINGS
{
	UCHAR Checksum[20];
	UCHAR Confounder[8];
	UCHAR HDKey[XBOX_KEY_LENGTH];
	ULONG GameRegion;
}
XBOX_ENCRYPTED_SETTINGS;

// ******************************************************************
// * XBOX_FACTORY_SETTINGS
// ******************************************************************
typedef struct _XBOX_FACTORY_SETTINGS
{
	ULONG Checksum;
	UCHAR SerialNumber[12];
	UCHAR EthernetAddr[6];
	UCHAR Reserved1[2];
	UCHAR OnlineKey[16];
	ULONG AVRegion;
	ULONG Reserved2;
}
XBOX_FACTORY_SETTINGS;

// ******************************************************************
// * XBOX_TIMEZONE_DATE
// ******************************************************************
typedef struct _XBOX_TIMEZONE_DATE
{
	UCHAR Month;
	UCHAR Day;
	UCHAR DayOfWeek;
	UCHAR Hour;
}
XBOX_TIMEZONE_DATE;

// ******************************************************************
// * XBOX_USER_SETTINGS
// ******************************************************************
#define TIME_ZONE_NAME_LENGTH 4
typedef struct _XBOX_USER_SETTINGS
{
	ULONG Checksum;
	LONG TimeZoneBias;
	CHAR TimeZoneStdName[TIME_ZONE_NAME_LENGTH];
	CHAR TimeZoneDltName[TIME_ZONE_NAME_LENGTH];
	ULONG Reserved1[2];
	XBOX_TIMEZONE_DATE TimeZoneStdDate;
	XBOX_TIMEZONE_DATE TimeZoneDltDate;
	ULONG Reserved2[2];
	LONG TimeZoneStdBias;
	LONG TimeZoneDltBias;
	ULONG Language;
	ULONG VideoFlags;
	ULONG AudioFlags;
	ULONG ParentalControlGames;
	ULONG ParentalControlPassword;
	ULONG ParentalControlMovies;
	ULONG OnlineIpAddress;
	ULONG OnlineDnsAddress;
	ULONG OnlineDefaultGatewayAddress;
	ULONG OnlineSubnetMask;
	ULONG MiscFlags;
	ULONG DvdRegion;
}
XBOX_USER_SETTINGS;

// ******************************************************************
// * XBOX_EEPROM
// ******************************************************************
typedef struct _XBOX_EEPROM
{
	XBOX_ENCRYPTED_SETTINGS EncryptedSettings;
	XBOX_FACTORY_SETTINGS FactorySettings;
	XBOX_USER_SETTINGS UserSettings;
	UCHAR Unused[58];
	UCHAR UEMInfo[4];
	UCHAR Reserved1[2];
}
XBOX_EEPROM;

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

/*
typedef enum _KINTERRUPT_POLARITY
{
	InterruptPolarityUnknown = 0,
	InterruptActiveHigh = 1,
	InterruptActiveLow = 2
}
KINTERRUPT_POLARITY;

typedef struct _OWNER_ENTRY
{
	ULONG OwnerThread;
	union
	{
		LONG OwnerCount;
		ULONG TableSize;
	};
}
OWNER_ENTRY, *POWNER_ENTRY;

typedef struct _ERESOURCE
{
	LIST_ENTRY SystemResourcesList;
	POWNER_ENTRY OwnerTable;
	SHORT ActiveCount;
	WORD Flag;
	PKSEMAPHORE SharedWaiters;
	PKEVENT ExclusiveWaiters;
	OWNER_ENTRY OwnerEntry;
	ULONG ActiveEntries;
	ULONG ContentionCount;
	ULONG NumberOfSharedWaiters;
	ULONG NumberOfExclusiveWaiters;
	union
	{
		PVOID Address;
		ULONG CreatorBackTraceIndex;
	};
	ULONG SpinLock;
}
ERESOURCE, *PERESOURCE;

typedef enum _POOL_TYPE
{
	NonPagedPool = 0,
	PagedPool = 1,
	NonPagedPoolMustSucceed = 2,
	DontUseThisType = 3,
	NonPagedPoolCacheAligned = 4,
	PagedPoolCacheAligned = 5,
	NonPagedPoolCacheAlignedMustS = 6,
	MaxPoolType = 7,
	NonPagedPoolSession = 32,
	PagedPoolSession = 33,
	NonPagedPoolMustSucceedSession = 34,
	DontUseThisTypeSession = 35,
	NonPagedPoolCacheAlignedSession = 36,
	PagedPoolCacheAlignedSession = 37,
	NonPagedPoolCacheAlignedMustSSession = 38
}
POOL_TYPE;

typedef struct _GENERIC_MAPPING
{
	ULONG GenericRead;
	ULONG GenericWrite;
	ULONG GenericExecute;
	ULONG GenericAll;
}
GENERIC_MAPPING, *PGENERIC_MAPPING;

typedef struct _OBJECT_TYPE_INITIALIZER
{
	WORD Length;
	UCHAR ObjectTypeFlags;
	ULONG CaseInsensitive: 1;
	ULONG UnnamedObjectsOnly: 1;
	ULONG UseDefaultObject: 1;
	ULONG SecurityRequired: 1;
	ULONG MaintainHandleCount: 1;
	ULONG MaintainTypeList: 1;
	ULONG ObjectTypeCode;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	POOL_TYPE PoolType;
	ULONG DefaultPagedPoolCharge;
	ULONG DefaultNonPagedPoolCharge;
	PVOID DumpProcedure;
	LONG * OpenProcedure;
	PVOID CloseProcedure;
	PVOID DeleteProcedure;
	LONG * ParseProcedure;
	LONG * SecurityProcedure;
	LONG * QueryNameProcedure;
	UCHAR * OkayToCloseProcedure;
}
OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _EX_PUSH_LOCK
{
	union
	{
		ULONG Locked: 1;
		ULONG Waiting: 1;
		ULONG Waking: 1;
		ULONG MultipleShared: 1;
		ULONG Shared: 28;
		ULONG Value;
		PVOID Ptr;
	};
}
EX_PUSH_LOCK, *PEX_PUSH_LOCK;

typedef struct _OBJECT_TYPE
{
	ERESOURCE Mutex;
	LIST_ENTRY TypeList;
	UNICODE_STRING Name;
	PVOID DefaultObject;
	ULONG Index;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	OBJECT_TYPE_INITIALIZER TypeInfo;
	ULONG Key;
	EX_PUSH_LOCK ObjectLocks[32];
}
OBJECT_TYPE, *POBJECT_TYPE;
*/

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
// * SCSI_PASS_THROUGH_DIRECT
// ******************************************************************
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

// ******************************************************************
// * MODE_PARAMETER_HEADER10
// ******************************************************************
typedef struct _MODE_PARAMETER_HEADER10 {
	UCHAR ModeDataLength[2];
	UCHAR MediumType;
	UCHAR DeviceSpecificParameter;
	UCHAR Reserved[2];
	UCHAR BlockDescriptorLength[2];
}MODE_PARAMETER_HEADER10, *PMODE_PARAMETER_HEADER10;

// ******************************************************************
// * DVDX2_AUTHENTICATION_PAGE
// ******************************************************************
typedef struct _DVDX2_AUTHENTICATION_PAGE {
	UCHAR Unknown[2];
    UCHAR PartitionArea;
    UCHAR CDFValid;
    UCHAR Authentication;
	UCHAR Unknown2[3];
	ULONG Unknown3[3];
} DVDX2_AUTHENTICATION_PAGE, *PDVDX2_AUTHENTICATION_PAGE;

// ******************************************************************
// * DVDX2_AUTHENTICATION
// ******************************************************************
typedef struct _DVDX2_AUTHENTICATION {
    MODE_PARAMETER_HEADER10 Header;
    DVDX2_AUTHENTICATION_PAGE AuthenticationPage;
} DVDX2_AUTHENTICATION, *PDVDX2_AUTHENTICATION;

// ******************************************************************
// * XBEIMAGE_SECTION
// ******************************************************************
// Section headers - Source: XBMC
// See Xbe.h struct SectionHeader
typedef struct _XBE_SECTION // Was _XBE_SECTIONHEADER
{
	ULONG Flags;
	PVOID VirtualAddress; // Virtual address (where this section loads in RAM)
	ULONG VirtualSize; // Virtual size (size of section in RAM; after FileSize it's 00'd)
	ULONG FileAddress; // File address (where in the file from which this section comes)
	ULONG FileSize; // File size (size of the section in the XBE file)
	PCSZ SectionName; // Pointer to section name
	ULONG SectionReferenceCount; // Section reference count - when >= 1, section is loaded
	ULONG HeadReferenceCount; // Pointer to head shared page reference count
	ULONG TailReferenceCount; // Pointer to tail shared page reference count
	BYTE ShaHash[20];         // SHA hash.  Hash DWORD containing FileSize, then hash section.
}
XBEIMAGE_SECTION, *PXBEIMAGE_SECTION;

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


