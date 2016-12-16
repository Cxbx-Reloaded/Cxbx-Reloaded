// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuNtDll.h
// *
// *  This file is part of the Cxbx project.
// *
// *  Cxbx and Cxbe are free software; you can redistribute them
// *  and/or modify them under the terms of the GNU General Public
// *  License as published by the Free Software Foundation; either
// *  version 2 of the license, or (at your option) any later version.
// *
// *  This program is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// *  GNU General Public License for more details.
// *
// *  You should have recieved a copy of the GNU General Public License
// *  along with this program; see the file COPYING.
// *  If not, write to the Free Software Foundation, Inc.,
// *  59 Temple Place - Suite 330, Bostom, MA 02111-1307, USA.
// *
// *  (c) 2002-2003 Aaron Robinson <caustik@caustik.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef EMUNTDLL_H
#define EMUNTDLL_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <ctype.h>  // winnt ntndis

// ******************************************************************
// * fixes some compiler errors and warnings
// ******************************************************************
#ifdef VOID
#undef VOID
#endif

// ******************************************************************
// * dll import/export
// ******************************************************************
#define DECLSPEC_IMPORT __declspec(dllimport)
#define DECLSPEC_EXPORT __declspec(dllexport)

// ******************************************************************
// * cxbx_krnl exports, others import
// ******************************************************************
#define NTSYSAPI     DECLSPEC_IMPORT

// ******************************************************************
// * CONST
// ******************************************************************
#define CONST               const

// ******************************************************************
// * Basic types
// ******************************************************************
typedef char                CHAR, CCHAR;
typedef short               SHORT;
typedef long                LONG;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef unsigned char       BOOLEAN;
typedef unsigned short      USHORT;
typedef unsigned short      WORD;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef unsigned long       SIZE_T, *PSIZE_T;
typedef unsigned long       ACCESS_MASK;
typedef unsigned long       PHYSICAL_ADDRESS;
typedef long                INT_PTR;
typedef int                 BOOL;


typedef unsigned long       UINT32;

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
typedef ULONG              *ULONG_PTR;
typedef ACCESS_MASK        *PACCESS_MASK;
typedef LONG               *PLONG, *LONG_PTR;
typedef INT_PTR            *PINT_PTR;
typedef void                VOID;
typedef VOID               *PVOID;
typedef void               *HANDLE;
typedef HANDLE             *PHANDLE;
typedef wchar_t             WCHAR, *PWCHAR;
typedef WCHAR              *LPWCH, *PWCH;
typedef CONST WCHAR        *LPCWCH, *PCWCH;
typedef WCHAR              *NWPSTR;
typedef WCHAR              *LPWSTR, *PWSTR;
typedef CONST WCHAR        *LPCWSTR, *PCWSTR;

// ******************************************************************
// * NTSTATUS
// ******************************************************************
typedef long                            NTSTATUS;
typedef unsigned __int64                ULONGLONG;

#define NT_SUCCESS(Status)              ((NTSTATUS) (Status) >= 0)

#define DIRECTORY_QUERY 1
#define DIRECTORY_TRAVERSE 2
#define DIRECTORY_CREATE_OBJECT 4
#define DIRECTORY_CREATE_SUBDIRECTORY 8


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

#define VOLATILE            volatile

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
	Spare2,
	Spare3,
	Spare4,
	Spare5,
	WrCalloutStack,
	WrKernel,
	WrResource,
	WrPushLock,
	WrMutex,
	WrQuantumEnd,
	WrDispatchInt,
	WrPreempted,
	WrYieldExecution,
	WrFastMutex,
	WrGuardedMutex,
	WrRundown,
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
// * LARGE_INTEGER
// ******************************************************************
typedef struct _LARGE_INTEGER
{
    DWORD   LowPart;
    LONG    HighPart;
}
LARGE_INTEGER, *PLARGE_INTEGER;

// ******************************************************************
// * LDT_ENTRY
// ******************************************************************
typedef struct _LDT_ENTRY
{
    WORD    LimitLow;
    WORD    BaseLow;

    union
    {
        struct
        {
            BYTE    BaseMid;
            BYTE    Flags1;     // Declare as bytes to avoid alignment
            BYTE    Flags2;     // Problems.
            BYTE    BaseHi;
        }
        Bytes;

        struct
        {
            DWORD   BaseMid : 8;
            DWORD   Type : 5;
            DWORD   Dpl : 2;
            DWORD   Pres : 1;
            DWORD   LimitHi : 4;
            DWORD   Sys : 1;
            DWORD   Reserved_0 : 1;
            DWORD   Default_Big : 1;
            DWORD   Granularity : 1;
            DWORD   BaseHi : 8;
        }
        Bits;

    }
    HighWord;
}
LDT_ENTRY, *PLDT_ENTRY;

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
// * RTL_HEAP_DEFINITION
// ******************************************************************
typedef struct _RTL_HEAP_DEFINITION
{
    ULONG   Length;
    ULONG   Unknown1;
    ULONG   Unknown2;
    ULONG   Unknown3;
    ULONG   Unknown4;
    ULONG   Unknown5;
    ULONG   Unknown6;
    ULONG   Unknown7;
    ULONG   Unknown8;
    ULONG   Unknown9;
    ULONG   Unknown10;
    ULONG   Unknown11;
}
RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

// ******************************************************************
// * RTL_CRITICAL_SECTION
// ******************************************************************
typedef struct _RTL_CRITICAL_SECTION
{
    DWORD               Unknown[4];                                     // 0x00
    LONG                LockCount;                                      // 0x10
    LONG                RecursionCount;                                 // 0x14
    HANDLE              OwningThread;                                   // 0x18
    HANDLE              LockSemaphore;
    DWORD               Reserved;
}
RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

// ******************************************************************
// * Valid values for the Attributes field
// ******************************************************************
#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

// ******************************************************************
// * UNICODE_STRING
// ******************************************************************
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
}
UNICODE_STRING, *PUNICODE_STRING;

typedef const UNICODE_STRING *PCUNICODE_STRING;

#define UNICODE_NULL ((WCHAR)0) // winnt

// ******************************************************************
// * OBJECT_ATTRIBUTES
// ******************************************************************
typedef struct _OBJECT_ATTRIBUTES
{
    ULONG           Length;
    HANDLE          RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG           Attributes;
    PVOID           SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID           SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
}
OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

// ******************************************************************
// * InitializeObjectAttributes
// *
// * VOID
// * InitializeObjectAttributes(
// *     OUT POBJECT_ATTRIBUTES p,
// *     IN PUNICODE_STRING n,
// *     IN ULONG a,
// *     IN HANDLE r,
// *     IN PSECURITY_DESCRIPTOR s
// *     )
// *
// ******************************************************************
#define InitializeObjectAttributes( p, n, a, r, s )     \
{                                                       \
    (p)->Length = sizeof( NtDll::OBJECT_ATTRIBUTES );   \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
}

// ******************************************************************
// * IO_STATUS_BLOCK
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
// * MEMORY_BASIC_INFORMATION
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
// * MEMORY_INFORMATION_CLASS
// ******************************************************************
typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation
}
MEMORY_INFORMATION_CLASS;

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
// * OBJECT_WAIT_TYPE
// ******************************************************************
typedef enum _OBJECT_WAIT_TYPE
{
    WaitAllObject,
    WaitAnyObject
}
OBJECT_WAIT_TYPE;

// ******************************************************************
// * CREATE_FILE_TYPE
// ******************************************************************
typedef enum _CREATE_FILE_TYPE
{
    CreateFileTypeNone,
    CreateFileTypeNamedPipe,
    CreateFileTypeMailslot
}
CREATE_FILE_TYPE;

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
    FileDirectoryInformation=1,
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
// * FS_INFORMATION_CLASS
// ******************************************************************
typedef enum _FS_INFORMATION_CLASS
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
    WCHAR           FileName[1];        // Offset: 0x40
}
FILE_DIRECTORY_INFORMATION;

// ******************************************************************
// * FILE_NETWORK_OPEN_INFORMATION
// ******************************************************************
typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   ChangeTime;
    LARGE_INTEGER   AllocationSize;
    LARGE_INTEGER   EndOfFile;
    ULONG           FileAttributes;
}
FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

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
// * KSYSTEM_TIME
// ******************************************************************
typedef struct _KSYSTEM_TIME {
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

// ******************************************************************
// * KUSER_SHARED_DATA
// ******************************************************************
typedef struct _KUSER_SHARED_DATA
{
	/* Current low 32-bit of tick count and tick count multiplier.
	* N.B. The tick count is updated each time the clock ticks.
	*/
	ULONG VOLATILE TickCountLow;
	UINT32 TickCountMultiplier;

	/* Current 64-bit interrupt time in 100ns units. */
	KSYSTEM_TIME VOLATILE InterruptTime;

	/* Current 64-bit system time in 100ns units. */
	KSYSTEM_TIME VOLATILE SystemTime;

	/* Current 64-bit time zone bias. */
	KSYSTEM_TIME VOLATILE TimeZoneBias;
}
KUSER_SHARED_DATA, *PKUSER_SHARED_DATA;
// This is only the top of the actual definition. For the complete version,
// see http://processhacker.sourceforge.net/doc/ntexapi_8h_source.html


// ******************************************************************
// * KeDelayExecutionThread
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_KeDelayExecutionThread)
(
    IN KPROCESSOR_MODE  WaitMode,
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   Interval
);

// ******************************************************************
// * KeQuerySystemTime
// ******************************************************************
typedef VOID (NTAPI *FPTR_KeQuerySystemTime)
(
    PLARGE_INTEGER CurrentTime
);

// ******************************************************************
// * RtlCreateHeap
// ******************************************************************
typedef PVOID (NTAPI *FPTR_RtlCreateHeap)
(
    IN ULONG   Flags,
    IN PVOID   Base OPTIONAL,
    IN ULONG   Reserve OPTIONAL,
    IN ULONG   Commit,
    IN PVOID   Lock OPTIONAL,
    IN PVOID   RtlHeapParams OPTIONAL
);

// ******************************************************************
// * RtlAllocateHeap
// ******************************************************************
typedef PVOID (NTAPI *FPTR_RtlAllocateHeap)
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN SIZE_T dwBytes
);

// ******************************************************************
// * RtlFreeHeap
// ******************************************************************
typedef BOOL (NTAPI *FPTR_RtlFreeHeap)
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
);

// ******************************************************************
// * RtlReAllocateHeap
// ******************************************************************
typedef PVOID (NTAPI *FPTR_RtlReAllocateHeap)
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem,
    IN DWORD  dwBytes
);

// ******************************************************************
// * RtlSizeHeap
// ******************************************************************
typedef SIZE_T (NTAPI *FPTR_RtlSizeHeap)
(
    IN HANDLE hHeap,
    IN DWORD  dwFlags,
    IN PVOID  lpMem
);

// ******************************************************************
// * RtlNtStatusToDosError
// ******************************************************************
typedef ULONG (NTAPI *FPTR_RtlNtStatusToDosError)(NTSTATUS Status);

// ******************************************************************
// * RtlTimeToTimeFields
// ******************************************************************
typedef BOOLEAN (NTAPI *FPTR_RtlTimeFieldsToTime)
(
    IN  PTIME_FIELDS    TimeFields,
    OUT PLARGE_INTEGER  Time
);

// ******************************************************************
// * RtlTimeToTimeFields
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlTimeToTimeFields)
(
    IN  PLARGE_INTEGER  Time,
    OUT PTIME_FIELDS    TimeFields
);

// ******************************************************************
// * RtlTryEnterCriticalSection
// ******************************************************************
typedef BOOL (NTAPI *FPTR_RtlTryEnterCriticalSection)
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * RtlInitAnsiString
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlInitAnsiString)
(
  IN OUT PANSI_STRING   DestinationString,
  IN     PCSZ           SourceString
);

// ******************************************************************
// * RtlInitUnicodeString
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlInitUnicodeString)
(
  IN OUT PUNICODE_STRING DestinationString,
  IN     PCWSTR          SourceString
);

// ******************************************************************
// * RtlAnsiStringToUnicodeString
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_RtlAnsiStringToUnicodeString)
(
    IN OUT PUNICODE_STRING DestinationString,
    IN     PANSI_STRING    SourceString,
    IN     BOOLEAN         AllocateDestinationString
);

// ******************************************************************
// * RtlAppendStringToString
// ******************************************************************
typedef NTSTATUS(NTAPI *FPTR_RtlAppendStringToString)
(
	IN OUT PSTRING    Destination,
	IN     PSTRING    Source
);

// ******************************************************************
// * RtlAppendUnicodeStringToString
// ******************************************************************
typedef NTSTATUS(NTAPI *FPTR_RtlAppendUnicodeStringToString)
(
	IN OUT PUNICODE_STRING  Destination,
	IN     PUNICODE_STRING  Source
);

// ******************************************************************
// * RtlAppendUnicodeToString
// ******************************************************************
typedef NTSTATUS(NTAPI *FPTR_RtlAppendUnicodeToString)
(
	IN OUT PUNICODE_STRING  Destination,
	IN     PCWSTR  Source
);

// ******************************************************************
// * RtlCharToInteger
// ******************************************************************
typedef NTSTATUS(NTAPI *FPTR_RtlCharToInteger)
(
	IN     PCSZ   String,
	IN     ULONG  Base OPTIONAL,
	OUT    PULONG Value
);

// ******************************************************************
// * RtlUnicodeStringToAnsiString
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_RtlUnicodeStringToAnsiString)
(
    IN OUT PANSI_STRING    DestinationString,
    IN     PUNICODE_STRING SourceString,
    IN     BOOLEAN         AllocateDestinationString
);

// ******************************************************************
// * RtlFreeAnsiString
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlFreeAnsiString)
(
  IN OUT PANSI_STRING   AnsiString
);

// ******************************************************************
// * RtlEqualString
// ******************************************************************
typedef BOOLEAN (NTAPI *FPTR_RtlEqualString)
(
	IN PSTRING			String1,
	IN PSTRING			String2,
	IN BOOLEAN			CaseSensitive
);

// ******************************************************************
// * NtDelayExecution
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtDelayExecution)
(
    IN BOOLEAN          Alertable,
    IN PLARGE_INTEGER   DelayInterval
);

// ******************************************************************
// * NtDuplicateObject
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtDuplicateObject)
(
    IN  HANDLE          SourceProcessHandle,
    IN  HANDLE          SourceHandle,
    IN  HANDLE          TargetProcessHandle,
    OUT PHANDLE         TargetHandle,
    IN  ACCESS_MASK     DesiredAccess OPTIONAL,
    IN  ULONG           Attributes,
    IN  ULONG           Options
);

#define DUPLICATE_SAME_ATTRIBUTES   0x00000004

// ******************************************************************
// * NtFlushBuffersFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtFlushBuffersFile)
(
    IN  HANDLE              FileHandle,
    OUT PIO_STATUS_BLOCK    IoStatusBlock
);

// ******************************************************************
// * NtSetLdtEntries
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtSetLdtEntries)
(
    IN USHORT       Selector1,
    IN LDT_ENTRY    Descriptor1,
    IN USHORT       Selector2,
    IN LDT_ENTRY    Descriptor2
);

// ******************************************************************
// * NtAllocateVirtualMemory
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtAllocateVirtualMemory)
(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID            *BaseAddress,
    IN ULONG                ZeroBits,
    IN OUT PULONG           RegionSize,
    IN ULONG                AllocationType,
    IN ULONG                Protect
);

// ******************************************************************
// * NtFreeVirtualMemory
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtFreeVirtualMemory)
(
    IN HANDLE               ProcessHandle,
    IN PVOID               *BaseAddress,
    IN OUT PULONG           RegionSize,
    IN ULONG                FreeType
);

// ******************************************************************
// * NtQueryVirtualMemory
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueryVirtualMemory)
(
    IN  HANDLE                      ProcessHandle,
    IN  PVOID                       BaseAddress,
    IN  MEMORY_INFORMATION_CLASS    MemoryInformationClass,
    OUT PVOID                       Buffer,
    IN  ULONG                       Length,
    OUT PULONG                      ResultLength OPTIONAL
);

// ******************************************************************
// * RtlInitializeCriticalSection
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlInitializeCriticalSection)
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * RtlEnterCriticalSection
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlEnterCriticalSection)
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * RtlLeaveCriticalSection
// ******************************************************************
typedef VOID (NTAPI *FPTR_RtlLeaveCriticalSection)
(
    IN PRTL_CRITICAL_SECTION CriticalSection
);

// ******************************************************************
// * NtWaitForSingleObject
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtWaitForSingleObject)
(
    IN HANDLE               ObjectHandle,
    IN BOOLEAN              Alertable,
    IN PLARGE_INTEGER       TimeOut
);

// ******************************************************************
// * NtWaitForMultipleObjects
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtWaitForMultipleObjects)
(
    IN ULONG                ObjectCount,
    IN PHANDLE              ObjectsArray,
    IN OBJECT_WAIT_TYPE     WaitType,
    IN BOOLEAN              Alertable,
    IN PLARGE_INTEGER       TimeOut OPTIONAL
);

// ******************************************************************
// * NtCreateEvent
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtCreateEvent)
(
    OUT PHANDLE             EventHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  EVENT_TYPE          EventType,
    IN  BOOLEAN             InitialState
);

// ******************************************************************
// * NtCreateMutant
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtCreateMutant)
(
    OUT PHANDLE             MutantHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  BOOLEAN             InitialOwner
);

// ******************************************************************
// * NtReleaseMutant
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtReleaseMutant)
(
    IN  HANDLE              MutantHandle,
    OUT PLONG               PreviousCount OPTIONAL
);

// ******************************************************************
// * NtCreateSemaphore
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtCreateSemaphore)
(
    OUT PHANDLE             SemaphoreHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  ULONG               InitialCount,
    IN  ULONG               MaximumCount
);

// ******************************************************************
// * NtReleaseSemaphore
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtReleaseSemaphore)
(
    IN  HANDLE              SemaphoreHandle,
    IN  ULONG               ReleaseCount,
    OUT PULONG              PreviousCount OPTIONAL
);

// ******************************************************************
// * NtCreateFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtCreateFile)
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               CreateDisposition,
    IN  ULONG               CreateOptions,
    IN  PVOID               EaBuffer OPTIONAL,
    IN  ULONG               EaLength
);

// ******************************************************************
// * NtCreateDirectoryObject
// ******************************************************************
typedef NTSTATUS(NTAPI *FPTR_NtCreateDirectoryObject)
(
	OUT PHANDLE            DirectoryHandle,
	IN  ACCESS_MASK        DesiredAccess,
	IN  POBJECT_ATTRIBUTES ObjectAttributes
);


// ******************************************************************
// * NtClearEvent
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtClearEvent)
(
    IN HANDLE               EventHandle
);

// ******************************************************************
// * NtClose
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtClose)
(
    IN HANDLE               Handle
);

// ******************************************************************
// * NtOpenFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtOpenFile)
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  ULONG               ShareAccess,
    IN  ULONG               OpenOptions
);

// ******************************************************************
// * NtReadFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtReadFile)
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  HANDLE          Event OPTIONAL,
    IN  PVOID           ApcRoutine OPTIONAL,
    IN  PVOID           ApcContext,
    OUT PVOID           IoStatusBlock,
    OUT PVOID           Buffer,
    IN  ULONG           Length,
    IN  PLARGE_INTEGER  ByteOffset OPTIONAL,
    IN  PULONG          Key OPTIONAL
);

// ******************************************************************
// * NtWriteFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtWriteFile)
(
    IN  HANDLE          FileHandle,            // TODO: correct paramters
    IN  HANDLE          Event OPTIONAL,
    IN  PVOID           ApcRoutine OPTIONAL,
    IN  PVOID           ApcContext,
    OUT PVOID           IoStatusBlock,
    OUT PVOID           Buffer,
    IN  ULONG           Length,
    IN  PLARGE_INTEGER  ByteOffset OPTIONAL,
    IN  PULONG          Key OPTIONAL
);

// ******************************************************************
// * NtYieldExecution
// ******************************************************************
typedef VOID (NTAPI *FPTR_NtYieldExecution)();

// ******************************************************************
// * NtSetInformationFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtSetInformationFile)
(
    IN  HANDLE  FileHandle,            // TODO: correct paramters
    OUT PVOID   IoStatusBlock,
    IN  PVOID   FileInformation,
    IN  ULONG   Length,
    IN  ULONG   FileInformationClass
);

// ******************************************************************
// * NtSuspendThread
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtSuspendThread)
(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  PreviousSuspendCount OPTIONAL
);

// ******************************************************************
// * NtResumeThread
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtResumeThread)
(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  SuspendCount OPTIONAL
);

// ******************************************************************
// * NtSetEvent
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtSetEvent)
(
    IN  HANDLE  EventHandle,
    OUT PLONG   PreviousState OPTIONAL
);

// ******************************************************************
// * NtQueryDirectoryFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueryDirectoryFile)
(
    IN  HANDLE                  FileHandle,
    IN  HANDLE                  Event OPTIONAL,
    IN  PIO_APC_ROUTINE         ApcRoutine OPTIONAL,
    IN  PVOID                   ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK        IoStatusBlock,
    OUT PVOID                   FileInformation,
    IN  ULONG                   Length,
    IN  FILE_INFORMATION_CLASS  FileInformationClass,
    IN  BOOLEAN                 ReturnSingleEntry,
    IN  PUNICODE_STRING         FileMask OPTIONAL,
    IN  BOOLEAN                 RestartScan
);

// ******************************************************************
// * NtQueryFullAttributesFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueryFullAttributesFile)
(
    IN  POBJECT_ATTRIBUTES          ObjectAttributes,
    OUT PVOID                       Attributes
);

// ******************************************************************
// * NtQueryInformationFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueryInformationFile)
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
    IN  ULONG                       Length,
    IN  FILE_INFORMATION_CLASS      FileInfo
);

// ******************************************************************
// * NtQueryVolumeInformationFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueryVolumeInformationFile)
(
    IN  HANDLE                      FileHandle,
    OUT PIO_STATUS_BLOCK            IoStatusBlock,
    OUT PFILE_FS_SIZE_INFORMATION   FileInformation,
    IN  ULONG                       Length,
    IN  FS_INFORMATION_CLASS        FsInformationClass
);

// ******************************************************************
// * IoCreateFile
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_IoCreateFile)
(
    OUT PHANDLE             FileHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK    IoStatusBlock,
    IN  PLARGE_INTEGER      AllocationSize OPTIONAL,
    IN  ULONG               FileAttributes,
    IN  ULONG               ShareAccess,
    IN  ULONG               Disposition,
    IN  ULONG               CreateOptions,
    IN  PVOID               EaBuffer OPTIONAL,
    IN  ULONG               EaLength,
    IN  CREATE_FILE_TYPE    CreateFileType,
    IN  PVOID               ExtraCreateParameters OPTIONAL,
    IN  ULONG               Options
);

// ******************************************************************
// * NtQueueApcThread
// ******************************************************************
typedef NTSTATUS (NTAPI *FPTR_NtQueueApcThread)
(
	IN HANDLE               ThreadHandle,
	IN PIO_APC_ROUTINE      ApcRoutine,
	IN PVOID                ApcRoutineContext OPTIONAL,
	IN PIO_STATUS_BLOCK     ApcStatusBlock OPTIONAL,
	IN ULONG                ApcReserved OPTIONAL 
);

// ******************************************************************
// * RtlDestroyHeap
// ******************************************************************
typedef PVOID (NTAPI *FPTR_RtlDestroyHeap)
(
    IN PVOID				HeapHandle
);

// ******************************************************************
// * Exported API
// ******************************************************************
#define EXTERN(API)  extern FPTR_##API API

// Note : Keep EXTERN's sorted, to ease future sync's (and compares with IMPORT's):
/*
EXTERN(InterlockedCompareExchange);
EXTERN(InterlockedDecrement);
EXTERN(InterlockedExchange);
EXTERN(InterlockedExchangeAdd);
EXTERN(InterlockedFlushSList);
EXTERN(InterlockedIncrement);
EXTERN(InterlockedPopEntrySList);
EXTERN(InterlockedPushEntrySList);
*/
EXTERN(NtAllocateVirtualMemory);
EXTERN(NtClearEvent);
EXTERN(NtClose);
EXTERN(NtCreateDirectoryObject);
EXTERN(NtCreateEvent);
EXTERN(NtCreateFile);
EXTERN(NtCreateMutant);
EXTERN(NtCreateSemaphore);
EXTERN(NtDelayExecution);
EXTERN(NtDuplicateObject);
EXTERN(NtFlushBuffersFile);
EXTERN(NtFreeVirtualMemory);
EXTERN(NtQueryDirectoryFile);
EXTERN(NtQueryFullAttributesFile);
EXTERN(NtQueryInformationFile);
EXTERN(NtQueryVirtualMemory);
EXTERN(NtQueryVolumeInformationFile);
EXTERN(NtQueueApcThread);
EXTERN(NtReadFile);
EXTERN(NtReleaseMutant);
EXTERN(NtReleaseSemaphore);
EXTERN(NtResumeThread);
EXTERN(NtSetEvent);
EXTERN(NtSetInformationFile);
EXTERN(NtSetLdtEntries);
EXTERN(NtSuspendThread);
EXTERN(NtWaitForMultipleObjects);
EXTERN(NtWaitForSingleObject);
EXTERN(NtWriteFile);
EXTERN(NtYieldExecution);
EXTERN(RtlAllocateHeap);
EXTERN(RtlAnsiStringToUnicodeString);
EXTERN(RtlAppendStringToString);
EXTERN(RtlAppendUnicodeStringToString);
EXTERN(RtlAppendUnicodeToString);
EXTERN(RtlCharToInteger);
EXTERN(RtlCreateHeap);
EXTERN(RtlDestroyHeap);
EXTERN(RtlEnterCriticalSection);
EXTERN(RtlEqualString);
EXTERN(RtlFreeAnsiString);
EXTERN(RtlFreeHeap);
EXTERN(RtlInitAnsiString);
EXTERN(RtlInitializeCriticalSection);
EXTERN(RtlInitUnicodeString);
EXTERN(RtlLeaveCriticalSection);
EXTERN(RtlNtStatusToDosError);
EXTERN(RtlReAllocateHeap);
EXTERN(RtlSizeHeap);
EXTERN(RtlTimeFieldsToTime);
EXTERN(RtlTimeToTimeFields);
EXTERN(RtlTryEnterCriticalSection);
EXTERN(RtlUnicodeStringToAnsiString);

#if defined(__cplusplus)
}
#endif

#endif
