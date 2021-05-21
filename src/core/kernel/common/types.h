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

#include "xbox_types.h"

namespace xbox
{

// ******************************************************************
// * dll import/export
// ******************************************************************
#define DECLSPEC_IMPORT __declspec(dllimport)
#define DECLSPEC_EXPORT __declspec(dllexport)
#define DECLSPEC_EXTERN extern

// ******************************************************************
// * kernel exports, others either import or link locally
// ******************************************************************
#define XBSYSAPI DECLSPEC_EXTERN
// The KRNL macro prevents naming collisions
#define KRNL(API) KRNL##API
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
// * LPSECURITY_ATTRIBUTES
// ******************************************************************
typedef void* LPSECURITY_ATTRIBUTES;

inline bool nt_success(ntstatus_xt status) { return status >= 0; }
inline constexpr dword_xt status_success = 0x00000000L;
inline constexpr dword_xt status_abandoned = 0x00000080L;
inline constexpr dword_xt status_mutant_limit_exceeded = 0xC0000191L;
inline constexpr dword_xt status_pending = 0x00000103L;
inline constexpr dword_xt status_timer_resume_ignored = 0x40000025L;
inline constexpr dword_xt status_buffer_overflow = 0x80000005L;
inline constexpr dword_xt status_unsuccessful = 0xC0000001;
inline constexpr dword_xt status_invalid_handle = 0xC0000008;
inline constexpr dword_xt status_unrecognized_media = 0xC0000014;
inline constexpr dword_xt status_no_memory = 0xC0000017L;
inline constexpr dword_xt status_buffer_too_small = 0xC0000023L;
inline constexpr dword_xt status_invalid_parameter_2 = 0xC00000F0L;
inline constexpr dword_xt status_alerted = 0x00000101;
inline constexpr dword_xt status_user_apc = 0x000000C0L;
// The SCSI input buffer was too large (not necessarily an error!)
inline constexpr dword_xt status_data_overrun = 0xC000003CL;
inline constexpr dword_xt status_semaphore_limit_exceeded = 0xC0000047L;
inline constexpr dword_xt status_invalid_image_format = 0xC000007BL;
inline constexpr dword_xt status_insufficient_resources = 0xC000009AL;
inline constexpr dword_xt status_too_many_secrets = 0xC0000156L;
inline constexpr dword_xt status_xbe_region_mismatch = 0xC0050001L;
inline constexpr dword_xt status_xbe_media_mismatch = 0xC0050002L;
inline constexpr dword_xt status_object_name_invalid = 0xC0000033L;
inline constexpr dword_xt status_object_name_not_found = 0xC0000034L;
inline constexpr dword_xt status_object_name_collision = 0xC0000035L;
inline constexpr dword_xt status_file_is_a_directory = 0xC00000BAL;
inline constexpr dword_xt status_end_of_file = 0xC0000011L;
inline constexpr dword_xt status_invalid_page_protection = 0xC0000045L;
inline constexpr dword_xt status_conflicting_addresses = 0xC0000018L;
inline constexpr dword_xt status_unable_to_free_vm = 0xC000001AL;
inline constexpr dword_xt status_free_vm_not_at_base = 0xC000009FL;
inline constexpr dword_xt status_memory_not_allocated = 0xC00000A0L;
inline constexpr dword_xt status_not_committed = 0xC000002DL;

// ******************************************************************
// * Registry value types
// ******************************************************************
// Used in ExQueryNonVolatileSetting and ExSaveNonVolatileSetting
inline constexpr int_xt reg_none                       = 0;   // No defined value type.
inline constexpr int_xt reg_sz                         = 1;   // A null - terminated string. This will be either a Unicode or an ANSI string, depending on whether you use the Unicode or ANSI functions.
inline constexpr int_xt reg_expand_sz                  = 2;   // A null - terminated string that contains unexpanded references to environment variables (for example, "%PATH%"). It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions. To expand the environment variable references, use the ExpandEnvironmentStrings function.
inline constexpr int_xt reg_binary                     = 3;   // Binary data in any form.
inline constexpr int_xt reg_dword                      = 4;   // A 32 - bit number.
inline constexpr int_xt reg_dword_little_endian        = 4;   // A 32 - bit number in little - endian format. Windows is designed to run on little - endian computer architectures. Therefore, this value is defined as REG_DWORD in the Windows header files.
inline constexpr int_xt reg_dword_big_endian           = 5;   // A 32 - bit number in big - endian format. Some UNIX systems support big - endian architectures.
inline constexpr int_xt reg_link                       = 6;   // A null - terminated Unicode string that contains the target path of a symbolic link that was created by calling the RegCreateKeyEx function with REG_OPTION_CREATE_LINK.
inline constexpr int_xt reg_multi_sz                   = 7;   // A sequence of null - terminated strings, terminated by an empty string(\0). String1\0String2\0String3\0LastString\0\0								 // The first \0 terminates the first string, the second to the last \0 terminates the last string, and the final \0 terminates the sequence.Note that the final terminator must be factored into the length of the string.
inline constexpr int_xt reg_resource_list              = 8;   // Resource list in the resource map
inline constexpr int_xt reg_full_resource_descriptor   = 9;   // Resource list in the hardware description
inline constexpr int_xt reg_resource_requirements_list = 10;

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
typedef cchar_xt KPROCESSOR_MODE;

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
        dword_xt   LowPart;
        long_xt    HighPart;
    }
    u;

    longlong_xt QuadPart;
}
LARGE_INTEGER, *PLARGE_INTEGER;

// ******************************************************************
// * ULARGE_INTEGER
// ******************************************************************
typedef union _ULARGE_INTEGER
{
    struct
    {
        dword_xt LowPart;
        dword_xt HighPart;
    }
    u1;

    struct
    {
        dword_xt LowPart;
        dword_xt HighPart;
    }
    u;

    ulonglong_xt QuadPart;
}
ULARGE_INTEGER, *PULARGE_INTEGER;

// ******************************************************************
// * STRING
// ******************************************************************
typedef struct _STRING
{
    ushort_xt  Length;
    ushort_xt  MaximumLength;
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
    ushort_xt  Length;
    ushort_xt  MaximumLength;
    ushort_xt *Buffer;
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
// * SLIST_ENTRY
// ******************************************************************
typedef struct _SINGLE_LIST_ENTRY {
	struct _SINGLE_LIST_ENTRY  *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY, SLIST_ENTRY, *PSLIST_ENTRY;

typedef union _SLIST_HEADER {
	ulonglong_xt Alignment;
	struct {
		SINGLE_LIST_ENTRY Next;
		ushort_xt Depth;
		ushort_xt Sequence;
	};
} SLIST_HEADER, *PSLIST_HEADER;

#define QUERY_DEPTH_SLIST(_listhead_) (xbox::ushort_xt)(_listhead_)->Depth

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
// * FILE_FS_SIZE_INFORMATION
// ******************************************************************
typedef struct _FILE_FS_SIZE_INFORMATION
{
    LARGE_INTEGER   TotalAllocationUnits;
    LARGE_INTEGER   AvailableAllocationUnits;
    ulong_xt           SectorsPerAllocationUnit;
    ulong_xt           BytesPerSector;
}
FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

// ******************************************************************
// * FILE_FS_VOLUME_INFORMATION
// ******************************************************************
typedef struct _FILE_FS_VOLUME_INFORMATION {
	LARGE_INTEGER	VolumeCreationTime;
	ulong_xt			VolumeSerialNumber;
	ulong_xt			VolumeLabelLength;
	boolean_xt			SupportsObjects;
	char_xt			VolumeLabel[1];
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
	access_mask_xt GenericRead;
	access_mask_xt GenericWrite;
	access_mask_xt GenericExecute;
	access_mask_xt GenericAll;
}
GENERIC_MAPPING, *PGENERIC_MAPPING;

// ******************************************************************
// * OBJECT_ATTRIBUTES
// ******************************************************************
typedef struct _OBJECT_ATTRIBUTES
{
    HANDLE  RootDirectory;
    PSTRING ObjectName;
    ulong_xt   Attributes;
}
OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

// ******************************************************************
// * OBJECT_TYPE
// ******************************************************************
typedef PVOID(NTAPI *OB_ALLOCATE_METHOD)(
	IN size_xt NumberOfBytes,
	IN ulong_xt Tag
	);

typedef void_xt(NTAPI *OB_FREE_METHOD)(
	IN PVOID Pointer
	);

typedef void_xt(NTAPI *OB_CLOSE_METHOD)(
	IN PVOID Object,
	IN ulong_xt SystemHandleCount
	);

typedef void_xt(NTAPI *OB_DELETE_METHOD)(
	IN PVOID Object
	);

typedef ntstatus_xt(NTAPI *OB_PARSE_METHOD)(
	IN PVOID ParseObject,
	IN struct _OBJECT_TYPE *ObjectType,
	IN ulong_xt Attributes,
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
	ulong_xt PoolTag;
}
OBJECT_TYPE, *POBJECT_TYPE;

// ******************************************************************
// * OBJECT_HEADER
// ******************************************************************
typedef struct _OBJECT_HEADER {
	long_xt PointerCount;
	long_xt HandleCount;
	POBJECT_TYPE Type;
	ulong_xt Flags;
	quad_xt Body;
} OBJECT_HEADER, *POBJECT_HEADER;

// Source : DXBX
typedef ulong_ptr_xt KSPIN_LOCK;
typedef KSPIN_LOCK *PKSPIN_LOCK;

// ******************************************************************
// * FILETIME
// ******************************************************************
// Source : DXBX
typedef struct _FILETIME
{
	dword_xt dwLowDateTime;
	dword_xt dwHighDateTime;
}
FILETIME, *PFILETIME;

// ******************************************************************
// * XBOX_REFURB_INFO
// ******************************************************************
// Source : DXBX (Xbox Refurb Info)
typedef struct _XBOX_REFURB_INFO
{
	dword_xt Signature;
	dword_xt PowerCycleCount;
	FILETIME FirstBootTime;
}
XBOX_REFURB_INFO, *PXBOX_REFURB_INFO;

#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

#define ALIGN_DOWN(length, type) ((ulong_xt)(length) & ~(sizeof(type) - 1))
#define ALIGN_UP(length, type) (ALIGN_DOWN(((ulong_xt)(length) + sizeof(type) - 1), type))
#define ALIGN_DOWN_POINTER(address, type) ((PVOID)((ulong_ptr_xt)(address) & ~((ulong_ptr_xt)sizeof(type) - 1)))
#define ALIGN_UP_POINTER(address, type) (ALIGN_DOWN_POINTER(((ulong_ptr_xt)(address) + sizeof(type) - 1), type))

// ******************************************************************
// * EXCEPTION_RECORD
// ******************************************************************
// Source : DXBX
typedef struct _EXCEPTION_RECORD
{
	dword_xt ExceptionCode;
	dword_xt ExceptionFlags;
	_EXCEPTION_RECORD *ExceptionRecord;
	void_xt *ExceptionAddress;
	dword_xt NumberParameters;
	ulong_ptr_xt ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
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
	ulong_xt           NextEntryOffset;
	ulong_xt           FileIndex;
	LARGE_INTEGER   CreationTime;
	LARGE_INTEGER   LastAccessTime;
	LARGE_INTEGER   LastWriteTime;
	LARGE_INTEGER   ChangeTime;
	LARGE_INTEGER   EndOfFile;
	LARGE_INTEGER   AllocationSize;
	ulong_xt           FileAttributes;
	ulong_xt           FileNameLength;
	char_xt            FileName[1];        // Offset: 0x40
}
FILE_DIRECTORY_INFORMATION;

// ******************************************************************
// * FILE_RENAME_INFORMATION
// ******************************************************************
typedef struct _FILE_RENAME_INFORMATION
{
	boolean_xt         ReplaceIfExists;
	HANDLE          RootDirectory;
	OBJECT_STRING   FileName;
}
FILE_RENAME_INFORMATION;

// ******************************************************************
// * FILE_LINK_INFORMATION
// ******************************************************************
typedef struct _FILE_LINK_INFORMATION {
	boolean_xt         ReplaceIfExists;
	HANDLE          RootDirectory;
	ulong_xt           FileNameLength;
	char_xt            FileName[1];
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
	ulong_xt           FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

// ******************************************************************
// * FILE_FULL_EA_INFORMATION
// ******************************************************************
typedef struct _FILE_FULL_EA_INFORMATION {
	ulong_xt NextEntryOffset;
	uchar_xt Flags;
	uchar_xt EaNameLength;
	ushort_xt EaValueLength;
	char_xt EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

// ******************************************************************
// * FILE_BASIC_INFORMATION
// ******************************************************************
typedef struct _FILE_BASIC_INFORMATION {
	LARGE_INTEGER   CreationTime;
	LARGE_INTEGER   LastAccessTime;
	LARGE_INTEGER   LastWriteTime;
	LARGE_INTEGER   ChangeTime;
	ulong_xt           FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

// ******************************************************************
// * FILE_STANDARD_INFORMATION
// ******************************************************************
typedef struct _FILE_STANDARD_INFORMATION {
	LARGE_INTEGER   AllocationSize;
	LARGE_INTEGER   EndOfFile;
	ulong_xt           NumberOfLinks;
	boolean_xt         DeletePending;
	boolean_xt         Directory;
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
	ulong_xt           EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

// ******************************************************************
// * FILE_ACCESS_INFORMATION
// ******************************************************************
typedef struct _FILE_ACCESS_INFORMATION {
	access_mask_xt     AccessFlags;
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
	ulong_xt           Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

// ******************************************************************
// * FILE_ALIGNMENT_INFORMATION
// ******************************************************************
typedef struct _FILE_ALIGNMENT_INFORMATION {
	ulong_xt           AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

// ******************************************************************
// * FILE_NAME_INFORMATION
// ******************************************************************
typedef struct _FILE_NAME_INFORMATION {
	ulong_xt           FileNameLength;
	char_xt            FileName[1];
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
	boolean_xt         DeleteFile;
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
	ushort_xt          CompressionFormat;
	uchar_xt           CompressionUnitShift;
	uchar_xt           ChunkShift;
	uchar_xt           ClusterShift;
	uchar_xt           Reserved[3];
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
	ulong_xt ClusterCount;
	HANDLE RootDirectory;
	ulong_xt FileNameLength;
	char_xt FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION, *PFILE_MOVE_CLUSTER_INFORMATION;

// ******************************************************************
// * FILE_STREAM_INFORMATION
// ******************************************************************
typedef struct _FILE_STREAM_INFORMATION {
	ulong_xt           NextEntryOffset;
	ulong_xt           StreamNameLength;
	LARGE_INTEGER   StreamSize;
	LARGE_INTEGER   StreamAllocationSize;
	char_xt            StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

// ******************************************************************
// * FILE_TRACKING_INFORMATION
// ******************************************************************
typedef struct _FILE_TRACKING_INFORMATION {
	HANDLE          DestinationFile;
	ulong_xt           ObjectInformationLength;
	char_xt            ObjectInformation[1];
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
	ulong_xt           ReadMode;
	ulong_xt           CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

// ******************************************************************
// * FILE_PIPE_LOCAL_INFORMATION
// ******************************************************************
typedef struct _FILE_PIPE_LOCAL_INFORMATION {
	ulong_xt           NamedPipeType;
	ulong_xt           NamedPipeConfiguration;
	ulong_xt           MaximumInstances;
	ulong_xt           CurrentInstances;
	ulong_xt           InboundQuota;
	ulong_xt           ReadDataAvailable;
	ulong_xt           OutboundQuota;
	ulong_xt           WriteQuotaAvailable;
	ulong_xt           NamedPipeState;
	ulong_xt           NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

// ******************************************************************
// * FILE_PIPE_REMOTE_INFORMATION
// ******************************************************************
typedef struct _FILE_PIPE_REMOTE_INFORMATION {
	LARGE_INTEGER   CollectDataTime;
	ulong_xt           MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

// ******************************************************************
// * FILE_MAILSLOT_QUERY_INFORMATION
// ******************************************************************
typedef struct _FILE_MAILSLOT_QUERY_INFORMATION {
	ulong_xt           MaximumMessageSize;
	ulong_xt           MailslotQuota;
	ulong_xt           NextMessageSize;
	ulong_xt           MessagesAvailable;
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
	longlong_xt        FileReference;
	ulong_xt           Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

// ******************************************************************
// * KSYSTEM_TIME
// ******************************************************************
typedef struct _KSYSTEM_TIME
{
	/* 0x00 */ ulong_xt LowPart;
	/* 0x04 */ long_xt High1Time;
	/* 0x08 */ long_xt High2Time;
} // Size = 0x0C
KSYSTEM_TIME, *PKSYSTEM_TIME;

// ******************************************************************
// * MM_STATISTICS
// ******************************************************************
typedef struct _MM_STATISTICS
{
    ulong_xt   Length;
    ulong_xt   TotalPhysicalPages;
    ulong_xt   AvailablePages;
    ulong_xt   VirtualMemoryBytesCommitted;
    ulong_xt   VirtualMemoryBytesReserved;
    ulong_xt   CachePagesCommitted;
    ulong_xt   PoolPagesCommitted;
    ulong_xt   StackPagesCommitted;
    ulong_xt   ImagePagesCommitted;
}
MM_STATISTICS, *PMM_STATISTICS;

// ******************************************************************
// * IO_STATUS_BLOCK *Same as Win2k/XP*
// ******************************************************************
typedef struct _IO_STATUS_BLOCK
{
    union
    {
        ntstatus_xt Status;
        PVOID    Pointer;
	};

    ulong_ptr_xt Information;
}
IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

// ******************************************************************
// * PIO_APC_ROUTINE
// ******************************************************************
typedef void_xt (NTAPI *PIO_APC_ROUTINE)
(
    IN PVOID            ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ulong_xt            Reserved
);

// ******************************************************************
// * PTIMER_APC_ROUTINE *Same as Win2k/XP*
// ******************************************************************
typedef void_xt(NTAPI *PTIMER_APC_ROUTINE)
(
	IN PVOID	TimerContext,
	IN ulong_xt	TimerLowValue,
	IN long_xt		TimerHighValue
);

// ******************************************************************
// * TIMER_BASIC_INFORMATION *Same as Win2k/XP*
// ******************************************************************
typedef struct _TIMER_BASIC_INFORMATION
{
	LARGE_INTEGER TimeRemaining;
	boolean_xt SignalState;
}
TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;

// ******************************************************************
// * MEMORY_BASIC_INFORMATION *Same as Win2k/XP*
// ******************************************************************
typedef struct _MEMORY_BASIC_INFORMATION
{
    PVOID   BaseAddress;
    PVOID   AllocationBase;
    dword_xt   AllocationProtect;
    size_xt  RegionSize;
    dword_xt   State;
    dword_xt   Protect;
    dword_xt   Type;
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
            ulong_xt   DeviceNumber:5;
            ulong_xt   FunctionNumber:3;
            ulong_xt   Reserved:24;
        }
        bits;

        ulong_xt   AsULONG;
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
            ulong_xt   Reserved1:2;
            ulong_xt   RegisterNumber:6;
            ulong_xt   FunctionNumber:3;
            ulong_xt   DeviceNumber:5;
            ulong_xt   BusNumber:8;
            ulong_xt   Reserved2:7;
            ulong_xt   Enable:1;
        } bits;

        ulong_xt   AsULONG;
    } u;
} PCI_TYPE1_CFG_BITS, *PPCI_TYPE1_CFG_BITS;

// ******************************************************************
// * PCI_COMMON_CONFIG
// ******************************************************************
typedef struct _PCI_COMMON_CONFIG
{
    ushort_xt  VendorID;                   // 0x00 (ro)
    ushort_xt  DeviceID;                   // 0x02 (ro)
    ushort_xt  Command;                    // 0x04 Device control
    ushort_xt  Status;                     // 0x06
    uchar_xt   RevisionID;                 // 0x08 (ro)
    uchar_xt   ProgIf;                     // 0x09 (ro)
    uchar_xt   SubClass;                   // 0x0A (ro)
    uchar_xt   BaseClass;                  // 0x0B (ro)
    uchar_xt   CacheLineSize;              // 0x0C (ro+)
    uchar_xt   LatencyTimer;               // 0x0D (ro+)
    uchar_xt   HeaderType;                 // 0x0E (ro)
    uchar_xt   BIST;                       // 0x0F Built in self test

    union
    {
        struct _PCI_HEADER_TYPE_0
        {
            ulong_xt   BaseAddresses[PCI_TYPE0_ADDRESSES]; // 0x10
            ulong_xt   CIS;
            ushort_xt  SubVendorID;
            ushort_xt  SubSystemID;
            ulong_xt   ROMBaseAddress;
            uchar_xt   CapabilitiesPtr;
            uchar_xt   Reserved1[3];
            ulong_xt   Reserved2;
            uchar_xt   InterruptLine;      //
            uchar_xt   InterruptPin;       // (ro)
            uchar_xt   MinimumGrant;       // (ro)
            uchar_xt   MaximumLatency;     // (ro)
        }
        type0;
    }u;

    uchar_xt DeviceSpecific[192];

}
PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;

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
    dword_xt   dwLaunchDataType;
    dword_xt   dwTitleId;
    char    szLaunchPath[520];
    dword_xt   dwFlags;
}
LAUNCH_DATA_HEADER, *PLAUNCH_DATA_HEADER;

// ******************************************************************
// * LAUNCH_DATA_PAGE
// ******************************************************************
typedef struct _LAUNCH_DATA_PAGE
{
    LAUNCH_DATA_HEADER  Header;
    uchar_xt               Pad[492];
    uchar_xt               LaunchData[3072];
}
LAUNCH_DATA_PAGE, *PLAUNCH_DATA_PAGE;

// ******************************************************************
// * DASH_LAUNCH_DATA
// ******************************************************************
typedef struct _DASH_LAUNCH_DATA
{
	dword_xt dwReason;
	dword_xt dwContext;
	dword_xt dwParameter1;
	dword_xt dwParameter2;
	byte_xt  Reserved[3072 - 16];
}
DASH_LAUNCH_DATA, *PDASH_LAUNCH_DATA;

// ******************************************************************
// * Persisted Frame Buffer Address
// ******************************************************************
extern PVOID AvSavedDataAddress;

// ******************************************************************
// * DISPATCHER_HEADER
// ******************************************************************
typedef struct _DISPATCHER_HEADER
{
    uchar_xt       Type;           // 0x00
    uchar_xt       Absolute;       // 0x01
    uchar_xt       Size;           // 0x02
    uchar_xt       Inserted;       // 0x03
    long_xt        SignalState;    // 0x04
    LIST_ENTRY  WaitListHead;   // 0x08
}
DISPATCHER_HEADER;

typedef long_xt KPRIORITY;

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
	long_xt EventState;
}
EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

// ******************************************************************
// KSEMAPHORE
// ******************************************************************
typedef struct _KSEMAPHORE
{
	DISPATCHER_HEADER Header; // 0x00
	long_xt Limit;               // 0x10
}                             // 0x14
KSEMAPHORE, *PKSEMAPHORE, *RESTRICTED_POINTER PRKSEMAPHORE;

// ******************************************************************
// SEMAPHORE_BASIC_INFORMATION - same as Windows
// ******************************************************************
typedef struct _SEMAPHORE_BASIC_INFORMATION
{
	long_xt CurrentCount;
	long_xt MaximumCount;
}
SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

// ******************************************************************
// MUTANT_BASIC_INFORMATION - same as Windows
// ******************************************************************
typedef struct _MUTANT_BASIC_INFORMATION
{
	long_xt CurrentCount;
	boolean_xt OwnedByCaller;
	boolean_xt AbandonedState;
}
MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

// ******************************************************************
// ERWLOCK
// ******************************************************************
typedef struct _ERWLOCK
{
	long_xt LockCount;             // 0x00
	ulong_xt WritersWaitingCount;  // 0x04
	ulong_xt ReadersWaitingCount;  // 0x08
	ulong_xt ReadersEntryCount;    // 0x0C
	KEVENT WriterEvent;         // 0x10
	KSEMAPHORE ReaderSemaphore; // 0x20
}                               // 0x34
ERWLOCK, *PERWLOCK;

// ******************************************************************
// KDEVICE_QUEUE
// ******************************************************************
typedef struct _KDEVICE_QUEUE
{
	cshort_xt Type;                // 0x00
	uchar_xt Size;                 // 0x02
	boolean_xt Busy;               // 0x04
	LIST_ENTRY DeviceListHead;  // 0x08
}
KDEVICE_QUEUE, *PKDEVICE_QUEUE, *RESTRICTED_POINTER PRKDEVICE_QUEUE;

typedef struct _KDEVICE_QUEUE_ENTRY 
{
	LIST_ENTRY DeviceListEntry;
	ulong_xt SortKey;
	boolean_xt Inserted;
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
	cshort_xt                 Type;                // 0x00
	word_xt                   Size;                // 0x02
	ulong_xt                  Flags;               // 0x04
	LIST_ENTRY             ThreadListEntry;     // 0x08
	IO_STATUS_BLOCK        IoStatus;            // 0x10
	char_xt                   StackCount;          // 0x18
	char_xt                   CurrentLocation;	    // 0x19
	uchar_xt                  PendingReturned;     // 0x1A
	uchar_xt                  Cancel;              // 0x1B
	PIO_STATUS_BLOCK       UserIosb;            // 0x1C
	PKEVENT                UserEvent;           // 0x20
	ulonglong_xt              Overlay;	            // 0x28
	PVOID                  UserBuffer;          // 0x30
	PFILE_SEGMENT_ELEMENT  SegmentArray;        // 0x34
	ulong_xt                  LockedBufferLength;  // 0x38
	ulonglong_xt              Tail;                // 0x3C
}
IRP, *PIRP;

// ******************************************************************
// DEVICE_OBJECT
// ******************************************************************
typedef struct _DEVICE_OBJECT
{
	cshort_xt Type;
	ushort_xt Size;
	long_xt ReferenceCount;
	struct _DRIVER_OBJECT *DriverObject;
	struct _DEVICE_OBJECT *MountedOrSelfDevice;
	PIRP CurrentIrp;
	ulong_xt Flags;
	PVOID DeviceExtension;
	uchar_xt DeviceType;
	uchar_xt StartIoFlags;
	cchar_xt StackSize;
	boolean_xt DeletePending;
	ulong_xt SectorSize;
	ulong_xt AlignmentRequirement;
	KDEVICE_QUEUE DeviceQueue;
	KEVENT DeviceLock;
	ulong_xt StartIoKey;
}
DEVICE_OBJECT, *PDEVICE_OBJECT;

// ******************************************************************
// PDRIVER_OBJECT
// ******************************************************************
typedef void_xt *PDRIVER_OBJECT;

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
	cshort_xt                    Type;               // 0x00

	byte_xt                      DeletePending : 1;  // 0x02
	byte_xt                      ReadAccess : 1;     // 0x02
	byte_xt                      WriteAccess : 1;    // 0x02
	byte_xt                      DeleteAccess : 1;   // 0x02
	byte_xt                      SharedRead : 1;     // 0x02
	byte_xt                      SharedWrite : 1;    // 0x02
	byte_xt                      SharedDelete : 1;   // 0x02
	byte_xt                      Reserved : 1;       // 0x02

	byte_xt                      Flags;              // 0x03
	PDEVICE_OBJECT            DeviceObject;       // 0x04
	PVOID                     FsContext;          // 0x08
	PVOID                     FsContext2;         // 0x0C
	ntstatus_xt                  FinalStatus;        // 0x10
	LARGE_INTEGER             CurrentByteOffset;  // 0x14
	struct _FILE_OBJECT *     RelatedFileObject;  // 0x1C
	PIO_COMPLETION_CONTEXT    CompletionContext;  // 0x20
	long_xt                      LockCount;          // 0x24
	KEVENT                    Lock;               // 0x28
	KEVENT                    Event;              // 0x38
} FILE_OBJECT, *PFILE_OBJECT;

// ******************************************************************
// * SHARE_ACCESS
// ******************************************************************
typedef struct _SHARE_ACCESS {
	byte_xt OpenCount;
	byte_xt Readers;
	byte_xt Writers;
	byte_xt Deleters;
	byte_xt SharedRead;
	byte_xt SharedWrite;
	byte_xt SharedDelete;
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
    long_xt                Period;           // 0x24
}
KTIMER, *PKTIMER;

// ******************************************************************
// * PKSTART_ROUTINE
// ******************************************************************
typedef void_xt (NTAPI *PKSTART_ROUTINE)
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
typedef void_xt (*PKSYSTEM_ROUTINE)
(
	IN PKSTART_ROUTINE StartRoutine OPTIONAL,
	IN PVOID StartContext OPTIONAL
);

struct _KDPC;

// ******************************************************************
// * PKDEFERRED_ROUTINE
// ******************************************************************
typedef void_xt (__stdcall *PKDEFERRED_ROUTINE)
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
    cshort_xt              Type;               // 0x00
	union
	{
		uchar_xt           Number;             // 0x02
		boolean_xt         Inserted;           // 0x02
	};
	uchar_xt               Importance;         // 0x03
    LIST_ENTRY          DpcListEntry;       // 0x04
    PKDEFERRED_ROUTINE  DeferredRoutine;    // 0x0C
    PVOID               DeferredContext;
    PVOID               SystemArgument1;
    PVOID               SystemArgument2;
}
KDPC, *PKDPC;

// ******************************************************************
// * DPC queue entry structure
// ******************************************************************
typedef struct _DPC_QUEUE_ENTRY
{
	PKDPC Dpc;
	PKDEFERRED_ROUTINE Routine;
	PVOID Context;
}
DPC_QUEUE_ENTRY, *PDPC_QUEUE_ENTRY;

// ******************************************************************
// * KFLOATING_SAVE
// ******************************************************************
// See NtDll::FLOATING_SAVE_AREA
typedef struct _KFLOATING_SAVE
{
	ulong_xt   ControlWord;
	ulong_xt   StatusWord;
	// NtDll contains ulong_xt TagWord here
	ulong_xt   ErrorOffset;
	ulong_xt   ErrorSelector;
	ulong_xt   DataOffset;
	ulong_xt   DataSelector;
	ulong_xt   Cr0NpxState; // NtDll has RegisterArea[SIZE_OF_80387_REGISTERS];
	ulong_xt   Spare1; // NtDll calls this Spare0
}
KFLOATING_SAVE, *PKFLOATING_SAVE;

#define DISPATCHER_OBJECT_TYPE_MASK 0x7
// ******************************************************************
// * KOBJECTS
// ******************************************************************
typedef enum _KOBJECTS
{
	EventSynchronizationObject = 1,
	MutantObject = 2,
	QueueObject = 4,
	SemaphoreObject = 5,
	ThreadObject = 6,
	TimerNotificationObject = 8,
	TimerSynchronizationObject = 9,
	ApcObject = 0x12,
	DpcObject = 0x13,
	DeviceQueueObject = 0x14,
}
KOBJECTS, *PKOBJECTS;

// ******************************************************************
// * PKNORMAL_ROUTINE
// ******************************************************************
typedef void_xt (*PKNORMAL_ROUTINE)
(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
);

// ******************************************************************
// * PKKERNEL_ROUTINE
// ******************************************************************
typedef void_xt (*PKKERNEL_ROUTINE)
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
typedef void_xt (*PKRUNDOWN_ROUTINE)
(
	IN struct _KAPC *Apc
);

// ******************************************************************
// * PKSYNCHRONIZE_ROUTINE
// ******************************************************************
typedef boolean_xt (*PKSYNCHRONIZE_ROUTINE)
(
	IN PVOID SynchronizeContext
);

// ******************************************************************
// * PKSERVICE_ROUTINE
// ******************************************************************
typedef boolean_xt (*PKSERVICE_ROUTINE)
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
#define PASSIVE_LEVEL  0
#define APC_LEVEL      1
#define DISPATCH_LEVEL 2
#define PROFILE_LEVEL  26
#define CLOCK1_LEVEL   28
#define CLOCK2_LEVEL   28
#define SYNC_LEVEL     28
#define IPI_LEVEL      29
#define POWER_LEVEL    30
#define HIGH_LEVEL     31
#define CLOCK_LEVEL    CLOCK2_LEVEL

#define DISPATCH_SIZE 22

// ******************************************************************
// * KeBugCheck BugCheckCode's :
// ******************************************************************
#define IRQL_NOT_GREATER_OR_EQUAL 0x00000009
#define IRQL_NOT_LESS_OR_EQUAL 0x0000000A
#define TRAP_CAUSE_UNKNOWN 0x00000012

// ******************************************************************
// * KINTERRUPR
// ******************************************************************
typedef struct _KINTERRUPT
{
	/* 0x00= 0 */ PKSERVICE_ROUTINE ServiceRoutine;
	/* 0x04= 4 */ PVOID ServiceContext;
	/* 0x08= 8 */ ulong_xt BusInterruptLevel;
	/* 0x0C=12 */ ulong_xt Irql; // Was : unsigned char     KIRQL; unsigned char     PaddingA[0x03];
	/* 0x10=16 */ boolean_xt Connected;
	/* 0x11=17 */ boolean_xt ShareVector;
	/* 0x12=18 */ KINTERRUPT_MODE Mode;
	/* 0x14=20 */ ulong_xt ServiceCount;
	/* 0x18=24 */ ulong_xt DispatchCode[DISPATCH_SIZE]; // Same as old : unsigned char ISR[0x58];
}
KINTERRUPT, *PKINTERRUPT;

// ******************************************************************
// * IRQL (Interrupt ReQuest Level) (* same as on win *)
// ******************************************************************
typedef uchar_xt KIRQL, *PKIRQL;

// ******************************************************************
// * PS_STATISTICS
// ******************************************************************
typedef struct _PS_STATISTICS
{
	ulong_xt Length;
	ulong_xt ThreadCount;
	ulong_xt HandleCount;
}
PS_STATISTICS, *PPS_STATISTICS;

// ******************************************************************
// * RTL_CRITICAL_SECTION
// ******************************************************************
typedef struct _RTL_CRITICAL_SECTION
{
	DISPATCHER_HEADER   Event;                                          // 0x00
    long_xt                LockCount;                                      // 0x10
    long_xt                RecursionCount;                                 // 0x14
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
        ulong_xt Version;                                              // 0x10 for TEB (?)
    }
    u_a;
    PVOID                                   ArbitraryUserPointer;   // 0x14
    struct _NT_TIB                         *Self;                   // 0x18
}
NT_TIB, *PNT_TIB;

// 	typedef struct _KGDTENTRY
// 	{
// 		word_xt LimitLow;
// 		word_xt BaseLow;
// 		ulong_xt HighWord;
// 	}
//  KGDTENTRY, *PKGDTENTRY;
// 
// 	typedef struct _KIDTENTRY
// 	{
// 		word_xt Offset;
// 		word_xt Selector;
// 		word_xt Access;
// 		word_xt ExtendedOffset;
// 	}
//  KIDTENTRY, *PKIDTENTRY;
// 
// 	typedef struct _KEXECUTE_OPTIONS
// 	{
// 		ulong_xt ExecuteDisable: 1;
// 		ulong_xt ExecuteEnable: 1;
// 		ulong_xt DisableThunkEmulation: 1;
// 		ulong_xt Permanent: 1;
// 		ulong_xt ExecuteDispatchEnable: 1;
// 		ulong_xt ImageDispatchEnable: 1;
// 		ulong_xt Spare: 2;
// 	}
//  KEXECUTE_OPTIONS, *PKEXECUTE_OPTIONS;

// ******************************************************************
// * KPROCESS
// ******************************************************************
typedef struct _KPROCESS
{
	/* 0x0/0 */ LIST_ENTRY ReadyListHead;
	/* 0x8/8 */ LIST_ENTRY ThreadListHead;
	/* 0x10/16 */ ulong_xt StackCount;
	/* 0x14/20 */ ulong_xt ThreadQuantum;
	/* 0x18/24 */ char_xt BasePriority;
	/* 0x19/25 */ char_xt DisableBoost;
	/* 0x1A/26 */ char_xt DisableQuantum;
	/* 0x1B/27 */ char_xt _padding;
}
KPROCESS, *PKPROCESS;

// ******************************************************************
// * KAPC_STATE
// ******************************************************************
typedef struct _KAPC_STATE
{
	LIST_ENTRY ApcListHead[2];
	PKPROCESS Process;
	uchar_xt KernelApcInProgress;
	uchar_xt KernelApcPending;
	uchar_xt UserApcPending;
	uchar_xt ApcQueueable;
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
	boolean_xt Abandoned;
} KMUTANT, *PKMUTANT, *RESTRICTED_POINTER PRKMUTANT;

// ******************************************************************
// * KQUEUE
// ******************************************************************
typedef struct _KQUEUE
{
	DISPATCHER_HEADER Header;
	LIST_ENTRY EntryListHead;
	ulong_xt CurrentCount;
	ulong_xt MaximumCount;
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
typedef struct _EXCEPTION_REGISTRATION_RECORD
{
	struct _EXCEPTION_REGISTRATION_RECORD *Next; // Don't forward declare PEXCEPTION_REGISTRATION_RECORD to avoid conflict with winnt.h
	PEXCEPTION_DISPOSITION Handler;
}
EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

// ******************************************************************
// * KTRAP_FRAME
// ******************************************************************
typedef struct _KTRAP_FRAME
{
	ulong_xt DbgEbp;
	ulong_xt DbgEip;
	ulong_xt DbgArgMark;
	ulong_xt DbgArgPointer;
	word_xt TempSegCs;
	uchar_xt Logging;
	uchar_xt Reserved;
	ulong_xt TempEsp;
	ulong_xt Dr0;
	ulong_xt Dr1;
	ulong_xt Dr2;
	ulong_xt Dr3;
	ulong_xt Dr6;
	ulong_xt Dr7;
	ulong_xt SegGs;
	ulong_xt SegEs;
	ulong_xt SegDs;
	ulong_xt Edx;
	ulong_xt Ecx;
	ulong_xt Eax;
	ulong_xt PreviousPreviousMode;
	PEXCEPTION_REGISTRATION_RECORD ExceptionList;
	ulong_xt SegFs;
	ulong_xt Edi;
	ulong_xt Esi;
	ulong_xt Ebx;
	ulong_xt Ebp;
	ulong_xt ErrCode;
	ulong_xt Eip;
	ulong_xt SegCs;
	ulong_xt EFlags;
	ulong_xt HardwareEsp;
	ulong_xt HardwareSegSs;
	ulong_xt V86Es;
	ulong_xt V86Ds;
	ulong_xt V86Fs;
	ulong_xt V86Gs;
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
	word_xt WaitKey;
	uchar_xt WaitType;
	uchar_xt SpareByte;
}
KWAIT_BLOCK, *PKWAIT_BLOCK;

// ******************************************************************
// * KAPC
// ******************************************************************
typedef struct _KAPC
{
	/* 0x0/0 */ ushort_xt Type;
	/* 0x2/2 */ KPROCESSOR_MODE ApcMode;
	/* 0x3/3 */ boolean_xt Inserted;
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
	/* 0x2C/44 */ char_xt State;
	/* 0x2D/45 */ char_xt Alerted[2];
	/* 0x2F/47 */ char_xt Alertable;
	/* 0x30/48 */ char_xt NpxState;
	/* 0x31/49 */ char_xt Saturation;
	/* 0x32/50 */ char_xt Priority;
	/* 0x33/51 */ char_xt Padding;
	/* 0x34/52 */ KAPC_STATE ApcState;
	/* 0x4C/76 */ ulong_xt ContextSwitches;
	/* 0x50/80 */ ulong_xt WaitStatus;
	/* 0x54/84 */ char_xt WaitIrql;
	/* 0x55/85 */ char_xt WaitMode;
	/* 0x56/86 */ char_xt WaitNext;
	/* 0x57/87 */ char_xt WaitReason;
	/* 0x58/88 */ PKWAIT_BLOCK WaitBlockList;
	/* 0x5C/92 */ LIST_ENTRY WaitListEntry;
	/* 0x64/100 */ ulong_xt WaitTime;
	/* 0x68/104 */ ulong_xt KernelApcDisable;
	/* 0x6C/108 */ ulong_xt Quantum;
	/* 0x70/112 */ char_xt BasePriority;
	/* 0x71/113 */ char_xt DecrementCount;
	/* 0x72/114 */ char_xt PriorityDecrement;
	/* 0x73/115 */ char_xt DisableBoost;
	/* 0x74/116 */ char_xt NpxIrql;
	/* 0x75/117 */ char_xt SuspendCount;
	/* 0x76/118 */ char_xt Preempted;
	/* 0x77/119 */ char_xt HasTerminated;
	/* 0x78/120 */ PVOID Queue;
	/* 0x7C/124 */ LIST_ENTRY QueueListEntry;
	/* 0x88/136 */ uchar_xt rsvd1[4];
	/* 0x88/136 */ KTIMER Timer;
	/* 0xB0/176 */ KWAIT_BLOCK TimerWaitBlock;
	/* 0xC8/200 */ KAPC SuspendApc;
	/* 0xF0/240 */ KSEMAPHORE SuspendSemaphore;
	/* 0x104/260 */ LIST_ENTRY ThreadListEntry;
	/* 0x10C/268 */ uchar_xt _padding[4];
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
    uchar_xt           UnknownA[0x1C]; // 0x110
    dword_xt           UniqueThread;   // 0x12C
}
ETHREAD, *PETHREAD;

// ******************************************************************
// * PCREATE_THREAD_NOTIFY_ROUTINE
// ******************************************************************
typedef void_xt(*PCREATE_THREAD_NOTIFY_ROUTINE)
(
	IN PETHREAD Thread,
	IN HANDLE ThreadId,
	IN boolean_xt Create
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

	ulong_xt            Unknown1[7];                                   // 0x0C, KPCR : 0x34

	LIST_ENTRY       DpcListHead;                                   // 0x28, KPCR : 0x50
	ulong_xt            DpcRoutineActive;                              // 0x30, KPCR : 0x58

    // This completes the total size of the structure (presumably)
    uchar_xt            Unknown[0x224];                            
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
    uchar_xt           Irql;                                           // 0x24
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
    ulong_xt Flags;
    uchar_xt GpuRevision;
	uchar_xt McpRevision;
    uchar_xt Unknown3;
    uchar_xt Unknown4;
}
XBOX_HARDWARE_INFO;

const int XBOX_KEY_LENGTH = 16;
const int ALTERNATE_SIGNATURE_COUNT = 16;
typedef uchar_xt XBOX_KEY_DATA[XBOX_KEY_LENGTH];

// ******************************************************************
// * XBOX_ENCRYPTED_SETTINGS
// ******************************************************************
typedef struct _XBOX_ENCRYPTED_SETTINGS
{
	uchar_xt Checksum[20];
	uchar_xt Confounder[8];
	uchar_xt HDKey[XBOX_KEY_LENGTH];
	ulong_xt GameRegion;
}
XBOX_ENCRYPTED_SETTINGS;

// ******************************************************************
// * XBOX_FACTORY_SETTINGS
// ******************************************************************
typedef struct _XBOX_FACTORY_SETTINGS
{
	ulong_xt Checksum;
	uchar_xt SerialNumber[12];
	uchar_xt EthernetAddr[6];
	uchar_xt Reserved1[2];
	uchar_xt OnlineKey[16];
	ulong_xt AVRegion;
	ulong_xt Reserved2;
}
XBOX_FACTORY_SETTINGS;

// ******************************************************************
// * XBOX_TIMEZONE_DATE
// ******************************************************************
typedef struct _XBOX_TIMEZONE_DATE
{
	uchar_xt Month;
	uchar_xt Day;
	uchar_xt DayOfWeek;
	uchar_xt Hour;
}
XBOX_TIMEZONE_DATE;

// ******************************************************************
// * XBOX_USER_SETTINGS
// ******************************************************************
#define TIME_ZONE_NAME_LENGTH 4
typedef struct _XBOX_USER_SETTINGS
{
	ulong_xt Checksum;
	long_xt TimeZoneBias;
	char_xt TimeZoneStdName[TIME_ZONE_NAME_LENGTH];
	char_xt TimeZoneDltName[TIME_ZONE_NAME_LENGTH];
	ulong_xt Reserved1[2];
	XBOX_TIMEZONE_DATE TimeZoneStdDate;
	XBOX_TIMEZONE_DATE TimeZoneDltDate;
	ulong_xt Reserved2[2];
	long_xt TimeZoneStdBias;
	long_xt TimeZoneDltBias;
	ulong_xt Language;
	ulong_xt VideoFlags;
	ulong_xt AudioFlags;
	ulong_xt ParentalControlGames;
	ulong_xt ParentalControlPassword;
	ulong_xt ParentalControlMovies;
	ulong_xt OnlineIpAddress;
	ulong_xt OnlineDnsAddress;
	ulong_xt OnlineDefaultGatewayAddress;
	ulong_xt OnlineSubnetMask;
	ulong_xt MiscFlags;
	ulong_xt DvdRegion;
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
	uchar_xt Unused[58];
	uchar_xt UEMInfo[4];
	uchar_xt Reserved1[2];
}
XBOX_EEPROM;

// ******************************************************************
// * XBOX_UEM_INFO
// ******************************************************************
typedef struct _XBOX_UEM_INFO
{
	uchar_xt ErrorCode;
	uchar_xt Unused;
	ushort_xt History;
}
XBOX_UEM_INFO;

// ******************************************************************
// * Xbox UEM (fatal error) codes
// ******************************************************************
#define FATAL_ERROR_NONE                0x00
#define FATAL_ERROR_CORE_DIGITAL        0x01
#define FATAL_ERROR_BAD_EEPROM          0x02
#define FATAL_ERROR_UNUSED1             0x03
#define FATAL_ERROR_BAD_RAM             0x04
#define FATAL_ERROR_HDD_NOT_LOCKED      0x05
#define FATAL_ERROR_HDD_CANNOT_UNLOCK   0x06
#define FATAL_ERROR_HDD_TIMEOUT         0x07
#define FATAL_ERROR_HDD_NOT_FOUND       0x08
#define FATAL_ERROR_HDD_BAD_CONFIG      0x09
#define FATAL_ERROR_DVD_TIMEOUT         0x0A
#define FATAL_ERROR_DVD_NOT_FOUND       0x0B
#define FATAL_ERROR_DVD_BAD_CONFIG      0x0C
#define FATAL_ERROR_XBE_DASH_GENERIC    0x0D
#define FATAL_ERROR_XBE_DASH_ERROR      0x0E
#define FATAL_ERROR_UNUSED2             0x0F
#define FATAL_ERROR_XBE_DASH_SETTINGS   0x10
#define FATAL_ERROR_UNUSED3             0x11
#define FATAL_ERROR_UNUSED4             0x12
#define FATAL_ERROR_UNUSED5             0x13
#define FATAL_ERROR_XBE_DASH_X2_PASS    0x14
#define FATAL_ERROR_REBOOT_ROUTINE      0x15
#define FATAL_ERROR_RESERVED            0xFF

// ******************************************************************
// * TIME_FIELDS
// ******************************************************************
typedef struct _TIME_FIELDS
{
    ushort_xt  Year;
    ushort_xt  Month;
    ushort_xt  Day;
    ushort_xt  Hour;
    ushort_xt  Minute;
    ushort_xt  Second;
    ushort_xt  Millisecond;
    ushort_xt  Weekday;
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
	ulong_xt OwnerThread;
	union
	{
		long_xt OwnerCount;
		ulong_xt TableSize;
	};
}
OWNER_ENTRY, *POWNER_ENTRY;

typedef struct _ERESOURCE
{
	LIST_ENTRY SystemResourcesList;
	POWNER_ENTRY OwnerTable;
	short_xt ActiveCount;
	word_xt Flag;
	PKSEMAPHORE SharedWaiters;
	PKEVENT ExclusiveWaiters;
	OWNER_ENTRY OwnerEntry;
	ulong_xt ActiveEntries;
	ulong_xt ContentionCount;
	ulong_xt NumberOfSharedWaiters;
	ulong_xt NumberOfExclusiveWaiters;
	union
	{
		PVOID Address;
		ulong_xt CreatorBackTraceIndex;
	};
	ulong_xt SpinLock;
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
	ulong_xt GenericRead;
	ulong_xt GenericWrite;
	ulong_xt GenericExecute;
	ulong_xt GenericAll;
}
GENERIC_MAPPING, *PGENERIC_MAPPING;

typedef struct _OBJECT_TYPE_INITIALIZER
{
	word_xt Length;
	uchar_xt ObjectTypeFlags;
	ulong_xt CaseInsensitive: 1;
	ulong_xt UnnamedObjectsOnly: 1;
	ulong_xt UseDefaultObject: 1;
	ulong_xt SecurityRequired: 1;
	ulong_xt MaintainHandleCount: 1;
	ulong_xt MaintainTypeList: 1;
	ulong_xt ObjectTypeCode;
	ulong_xt InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ulong_xt ValidAccessMask;
	POOL_TYPE PoolType;
	ulong_xt DefaultPagedPoolCharge;
	ulong_xt DefaultNonPagedPoolCharge;
	PVOID DumpProcedure;
	long_xt * OpenProcedure;
	PVOID CloseProcedure;
	PVOID DeleteProcedure;
	long_xt * ParseProcedure;
	long_xt * SecurityProcedure;
	long_xt * QueryNameProcedure;
	uchar_xt * OkayToCloseProcedure;
}
OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _EX_PUSH_LOCK
{
	union
	{
		ulong_xt Locked: 1;
		ulong_xt Waiting: 1;
		ulong_xt Waking: 1;
		ulong_xt MultipleShared: 1;
		ulong_xt Shared: 28;
		ulong_xt Value;
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
	ulong_xt Index;
	ulong_xt TotalNumberOfObjects;
	ulong_xt TotalNumberOfHandles;
	ulong_xt HighWaterNumberOfObjects;
	ulong_xt HighWaterNumberOfHandles;
	OBJECT_TYPE_INITIALIZER TypeInfo;
	ulong_xt Key;
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
INLINE static uchar_xt READ_REGISTER_UCHAR(PUCHAR Address)
{
    return *(volatile uchar_xt *)Address;
}

// ******************************************************************
// * READ_REGISTER_USHORT
// ******************************************************************
// *
// * Use this to access I/O mapped memory. Just a good standard.
// *
// ******************************************************************
INLINE static ushort_xt READ_REGISTER_USHORT(PUSHORT Address)
{
    return *(volatile ushort_xt *)Address;
}

// ******************************************************************
// * READ_REGISTER_ULONG
// ******************************************************************
// *
// * Use this to access I/O mapped memory. Just a good standard.
// *
// ******************************************************************
INLINE static ulong_xt READ_REGISTER_ULONG(PULONG Address)
{
    return *(volatile ulong_xt *)Address;
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
static void_xt WRITE_REGISTER_UCHAR(PVOID Address, uchar_xt Value)
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
static void_xt WRITE_REGISTER_USHORT(PVOID Address, ushort_xt Value)
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
static void_xt WRITE_REGISTER_ULONG(PVOID Address, ulong_xt Value)
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
	ushort_xt Length;
	uchar_xt ScsiStatus;
	uchar_xt PathId;
	uchar_xt TargetId;
	uchar_xt Lun;
	uchar_xt CdbLength;
	uchar_xt SenseInfoLength;
	uchar_xt DataIn;
	ulong_xt DataTransferLength;
	ulong_xt TimeOutValue;
	PVOID DataBuffer;
	ulong_xt SenseInfoOffset;
	uchar_xt Cdb[16];
}SCSI_PASS_THROUGH_DIRECT, *PSCSI_PASS_THROUGH_DIRECT;

// ******************************************************************
// * MODE_PARAMETER_HEADER10
// ******************************************************************
typedef struct _MODE_PARAMETER_HEADER10 {
	uchar_xt ModeDataLength[2];
	uchar_xt MediumType;
	uchar_xt DeviceSpecificParameter;
	uchar_xt Reserved[2];
	uchar_xt BlockDescriptorLength[2];
}MODE_PARAMETER_HEADER10, *PMODE_PARAMETER_HEADER10;

// ******************************************************************
// * DVDX2_AUTHENTICATION_PAGE
// ******************************************************************
typedef struct _DVDX2_AUTHENTICATION_PAGE {
	uchar_xt Unknown[2];
    uchar_xt PartitionArea;
    uchar_xt CDFValid;
    uchar_xt Authentication;
	uchar_xt Unknown2[3];
	ulong_xt Unknown3[3];
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
	ulong_xt Flags;
	PVOID VirtualAddress; // Virtual address (where this section loads in RAM)
	ulong_xt VirtualSize; // Virtual size (size of section in RAM; after FileSize it's 00'd)
	ulong_xt FileAddress; // File address (where in the file from which this section comes)
	ulong_xt FileSize; // File size (size of the section in the XBE file)
	PCSZ SectionName; // Pointer to section name
	ulong_xt SectionReferenceCount; // Section reference count - when >= 1, section is loaded
	PUSHORT HeadReferenceCount; // Pointer to head shared page reference counter
	PUSHORT TailReferenceCount; // Pointer to tail shared page reference counter
	byte_xt ShaHash[20];         // SHA hash.  Hash dword_xt containing FileSize, then hash section.
}
XBEIMAGE_SECTION, *PXBEIMAGE_SECTION;

// From Windows Driver Development Kit
typedef enum _MEDIA_TYPE {
	Unknown,                // Format is unknown
	F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
	F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
	F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
	F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
	F3_720_512,             // 3.5",  720KB,  512 bytes/sector
	F5_360_512,             // 5.25", 360KB,  512 bytes/sector
	F5_320_512,             // 5.25", 320KB,  512 bytes/sector
	F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
	F5_180_512,             // 5.25", 180KB,  512 bytes/sector
	F5_160_512,             // 5.25", 160KB,  512 bytes/sector
	RemovableMedia,         // Removable media other than floppy
	FixedMedia,             // Fixed hard disk media
	F3_120M_512,            // 3.5", 120M Floppy
	F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
	F5_640_512,             // 5.25",  640KB,  512 bytes/sector
	F5_720_512,             // 5.25",  720KB,  512 bytes/sector
	F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
	F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
	F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
	F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
	F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
	F8_256_128,             // 8",     256KB,  128 bytes/sector
	F3_200Mb_512,           // 3.5",   200M Floppy (HiFD)
	F3_240M_512,            // 3.5",   240Mb Floppy (HiFD)
	F3_32M_512              // 3.5",   32Mb Floppy
} MEDIA_TYPE, *PMEDIA_TYPE;

typedef struct _DISK_GEOMETRY {
	LARGE_INTEGER Cylinders;
	MEDIA_TYPE MediaType;
	dword_xt TracksPerCylinder;
	dword_xt SectorsPerTrack;
	dword_xt BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

// From nxdk
#pragma pack(push, 1)
typedef struct _FLOATING_SAVE_AREA
{
    word_xt ControlWord;
    word_xt StatusWord;
    word_xt TagWord;
    word_xt ErrorOpcode;
    dword_xt ErrorOffset;
    dword_xt ErrorSelector;
    dword_xt DataOffset;
    dword_xt DataSelector;
    dword_xt MXCsr;
    dword_xt Reserved2;
    byte_xt RegisterArea[128];
    byte_xt XmmRegisterArea[128];
    byte_xt Reserved4[224];
    dword_xt Cr0NpxState;
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;
#pragma pack(pop)

typedef struct _CONTEXT
{
    dword_xt ContextFlags; // 0x00
    FLOATING_SAVE_AREA FloatSave; // 0x04
    dword_xt Edi; // 0x208
    dword_xt Esi; // 0x20C
    dword_xt Ebx; // 0x210
    dword_xt Edx; // 0x214
    dword_xt Ecx; // 0x218
    dword_xt Eax; // 0x21C
    dword_xt Ebp; // 0x220
    dword_xt Eip; // 0x224
    dword_xt SegCs; // 0x228
    dword_xt EFlags; // 0x22C
    dword_xt Esp; // 0x230
    dword_xt SegSs; // 0x234
} CONTEXT, *PCONTEXT;

// This is modeled around the Windows definition
typedef struct _IO_COMPLETION_BASIC_INFORMATION {
	long_xt Depth;
} IO_COMPLETION_BASIC_INFORMATION, *PIO_COMPLETION_BASIC_INFORMATION;

typedef void_xt(*PIDE_INTERRUPT_ROUTINE) (void);

typedef void_xt(*PIDE_FINISHIO_ROUTINE) (void);

typedef boolean_xt(*PIDE_POLL_RESET_COMPLETE_ROUTINE) (void);

typedef void_xt(*PIDE_TIMEOUT_EXPIRED_ROUTINE) (void);

typedef void_xt(*PIDE_START_PACKET_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
);

typedef void_xt(*PIDE_START_NEXT_PACKET_ROUTINE) (void);

typedef struct _IDE_CHANNEL_OBJECT
{
    PIDE_INTERRUPT_ROUTINE InterruptRoutine;
    PIDE_FINISHIO_ROUTINE FinishIoRoutine;
    PIDE_POLL_RESET_COMPLETE_ROUTINE PollResetCompleteRoutine;
    PIDE_TIMEOUT_EXPIRED_ROUTINE TimeoutExpiredRoutine;
    PIDE_START_PACKET_ROUTINE StartPacketRoutine;
    PIDE_START_NEXT_PACKET_ROUTINE StartNextPacketRoutine;
    KIRQL InterruptIrql;
    boolean_xt ExpectingBusMasterInterrupt;
    boolean_xt StartPacketBusy;
    boolean_xt StartPacketRequested;
    uchar_xt Timeout;
    uchar_xt IoRetries;
    uchar_xt MaximumIoRetries;
    PIRP CurrentIrp;
    KDEVICE_QUEUE DeviceQueue;
    ulong_xt PhysicalRegionDescriptorTablePhysical;
    KDPC TimerDpc;
    KDPC FinishDpc;
    KTIMER Timer;
    KINTERRUPT InterruptObject;
} IDE_CHANNEL_OBJECT, *PIDE_CHANNEL_OBJECT;

}

#endif


