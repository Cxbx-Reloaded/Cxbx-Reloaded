// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xfile_def.h
// *
// * note : internal typedefs / structs / etc for xfile
// *
// ******************************************************************
#ifndef XFILE_DEF_H
#define XFILE_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif

// DEVICE_TYPEs (I took a guess as to which the XBOX might have.)
#define FILE_DEVICE_CD_ROM                      (0x00000002)
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM          (0x00000003)
#define FILE_DEVICE_CONTROLLER                  (0x00000004)
#define FILE_DEVICE_DISK                        (0x00000007)
#define FILE_DEVICE_DISK_FILE_SYSTEM            (0x00000008)
#define FILE_DEVICE_FILE_SYSTEM                 (0x00000009)
#define FILE_DEVICE_NULL                        (0x00000015)
#define FILE_DEVICE_SCREEN                      (0x0000001c)
#define FILE_DEVICE_SOUND                       (0x0000001d)
#define FILE_DEVICE_UNKNOWN                     (0x00000022)
#define FILE_DEVICE_VIDEO                       (0x00000023)
#define FILE_DEVICE_VIRTUAL_DISK                (0x00000024)
#define FILE_DEVICE_FULLSCREEN_VIDEO            (0x00000034)

#define FILE_ATTRIBUTE_READONLY                 (0x00000001)
#define FILE_ATTRIBUTE_HIDDEN                   (0x00000002)
#define FILE_ATTRIBUTE_SYSTEM                   (0x00000004)
#define FILE_ATTRIBUTE_DIRECTORY                (0x00000010)
#define FILE_ATTRIBUTE_ARCHIVE                  (0x00000020)
#define FILE_ATTRIBUTE_NORMAL                   (0x00000080)
#define FILE_ATTRIBUTE_TEMPORARY                (0x00000100)
#define FILE_FLAG_WRITE_THROUGH                 (0x80000000)
#define FILE_FLAG_RANDOM_ACCESS                 (0x10000000)

// NtCreateFile/NtOpenFile stuff
#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

// Flags for OBJECT_ATTRIBUTES::Attributes      
#define OBJ_INHERIT                             (0x00000002L)
#define OBJ_PERMANENT                           (0x00000010L)
#define OBJ_EXCLUSIVE                           (0x00000020L)
#define OBJ_CASE_INSENSITIVE                    (0x00000040L)
#define OBJ_OPENIF                              (0x00000080L)
#define OBJ_OPENLINK                            (0x00000100L)
#define OBJ_KERNEL_HANDLE                       (0x00000200L)
#define OBJ_VALID_ATTRIBUTES                    (0x000003F2L)

// ******************************************************************
// * File pointer information (SetFilePointer, etc)
// ******************************************************************
typedef struct _FILE_POSITION_INFORMATION
{
    LARGE_INTEGER   CurrentByteOffset;
}
FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

// ******************************************************************
// * Access times and normal attributes (only set is known to be
// * supported - use FILE_NETWORK_OPEN_INFORMATION if you want to
// * query this information.
// ******************************************************************
typedef struct _FILE_BASIC_INFORMATION
{
    LARGE_INTEGER   CreationTime;
    LARGE_INTEGER   LastAccessTime;
    LARGE_INTEGER   LastWriteTime;
    LARGE_INTEGER   ChangeTime;
    ULONG           FileAttributes;
}
FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

// ******************************************************************
// * Access times and normal attributes
// ******************************************************************
typedef struct _FILE_NETWORK_OPEN_INFORMATION 
{
    LARGE_INTEGER CreationTime;             // 000 Time file was created
    LARGE_INTEGER LastAccessTime;           // 008 Time file was last accessed
    LARGE_INTEGER LastWriteTime;            // 010 Time file was last opened for writing?
    LARGE_INTEGER ChangeTime;               // 018 Time file was last changed?
    LARGE_INTEGER AllocationSize;           // 020 Size of the file in the file system (including slack space)
    LARGE_INTEGER EndOfFile;                // 028 What we'd normally call the file size
    ULONG FileAttributes;                   // 030 File attributes
    ULONG Unknown;                          // 034 Unknown
}
FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

LARGE_INTEGER AddU64(PLARGE_INTEGER A, PLARGE_INTEGER B);

#ifdef  __cplusplus
};
#endif

#endif // XFILE_DEF_H
