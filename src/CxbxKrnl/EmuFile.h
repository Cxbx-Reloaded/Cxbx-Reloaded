// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFile.h
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
#ifndef EMUFILE_H
#define EMUFILE_H

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace xboxkrnl
{
    #include <xboxkrnl/xboxkrnl.h>
};

#include <vector>
#include <cstdio>
#include <string>
#include <memory>

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "Emu.h"

// TODO : Move to a better suited file
//std::ostream& operator<<(std::ostream& os, const NtDll::NTSTATUS& value);

extern const std::string DrivePrefix;
extern const std::string DriveSerial;
extern const std::string DriveCdRom0;
extern const std::string DriveMbfs;
extern const std::string DriveMbcom;
extern const std::string DriveMbrom;
extern const std::string DriveA;
extern const std::string DriveC;
extern const std::string DriveD;
extern const std::string DriveE;
extern const std::string DriveF;
extern const std::string DriveS;
extern const std::string DriveT;
extern const std::string DriveU;
extern const std::string DriveV;
extern const std::string DriveW;
extern const std::string DriveX;
extern const std::string DriveY;
extern const std::string DriveZ;
extern const std::string DevicePrefix;
extern const std::string DeviceCdrom0;
extern const std::string DeviceHarddisk0;
extern const std::string DeviceHarddisk0PartitionPrefix;
extern const std::string DeviceHarddisk0Partition0;
extern const std::string DeviceHarddisk0Partition1;
extern const std::string DeviceHarddisk0Partition2;
extern const std::string DeviceHarddisk0Partition3;
extern const std::string DeviceHarddisk0Partition4;
extern const std::string DeviceHarddisk0Partition5;
extern const std::string DeviceHarddisk0Partition6;
extern const std::string DeviceHarddisk0Partition7;
extern const std::string DeviceHarddisk0Partition8;
extern const std::string DeviceHarddisk0Partition9;
extern const std::string DeviceHarddisk0Partition10;
extern const std::string DeviceHarddisk0Partition11;
extern const std::string DeviceHarddisk0Partition12;
extern const std::string DeviceHarddisk0Partition13;
extern const std::string DeviceHarddisk0Partition14;
extern const std::string DeviceHarddisk0Partition15;
extern const std::string DeviceHarddisk0Partition16;
extern const std::string DeviceHarddisk0Partition17;
extern const std::string DeviceHarddisk0Partition18;
extern const std::string DeviceHarddisk0Partition19;
extern const std::string DeviceHarddisk0Partition20;
extern const char CxbxDefaultXbeDriveLetter;
extern int CxbxDefaultXbeDriveIndex;

extern std::string CxbxBasePath;
extern HANDLE CxbxBasePathHandle;

const size_t XboxFileInfoStructSizes[xboxkrnl::FileMaximumInformation] = {
	0,                                                    // (index 0)
	sizeof(xboxkrnl::FILE_DIRECTORY_INFORMATION),         // FileDirectoryInformation
	sizeof(xboxkrnl::FILE_DIRECTORY_INFORMATION),         // FileFullDirectoryInformation
	sizeof(xboxkrnl::FILE_DIRECTORY_INFORMATION),         // FileBothDirectoryInformation
	sizeof(xboxkrnl::FILE_BASIC_INFORMATION),             // FileBasicInformation
	sizeof(xboxkrnl::FILE_STANDARD_INFORMATION),          // FileStandardInformation
	sizeof(xboxkrnl::FILE_INTERNAL_INFORMATION),          // FileInternalInformation
	sizeof(xboxkrnl::FILE_EA_INFORMATION),                // FileEaInformation
	sizeof(xboxkrnl::FILE_ACCESS_INFORMATION),            // FileAccessInformation
	sizeof(xboxkrnl::FILE_NAME_INFORMATION),              // FileNameInformation
	sizeof(xboxkrnl::FILE_RENAME_INFORMATION),            // FileRenameInformation
	sizeof(xboxkrnl::FILE_LINK_INFORMATION),              // FileLinkInformation
	sizeof(xboxkrnl::FILE_DIRECTORY_INFORMATION),         // FileNamesInformation
	sizeof(xboxkrnl::FILE_DISPOSITION_INFORMATION),       // FileDispositionInformation
	sizeof(xboxkrnl::FILE_POSITION_INFORMATION),          // FilePositionInformation
	sizeof(xboxkrnl::FILE_FULL_EA_INFORMATION),           // FileFullEaInformation
	sizeof(xboxkrnl::FILE_MODE_INFORMATION),              // FileModeInformation
	sizeof(xboxkrnl::FILE_ALIGNMENT_INFORMATION),         // FileAlignmentInformation
	sizeof(xboxkrnl::FILE_ALL_INFORMATION),               // FileAllInformation
	sizeof(xboxkrnl::FILE_ALLOCATION_INFORMATION),        // FileAllocationInformation
	sizeof(xboxkrnl::FILE_END_OF_FILE_INFORMATION),       // FileEndOfFileInformation
	sizeof(xboxkrnl::FILE_NAME_INFORMATION),              // FileAlternateNameInformation
	sizeof(xboxkrnl::FILE_STREAM_INFORMATION),            // FileStreamInformation
	sizeof(xboxkrnl::FILE_PIPE_INFORMATION),              // FilePipeInformation
	sizeof(xboxkrnl::FILE_PIPE_LOCAL_INFORMATION),        // FilePipeLocalInformation
	sizeof(xboxkrnl::FILE_PIPE_REMOTE_INFORMATION),       // FilePipeRemoteInformation
	sizeof(xboxkrnl::FILE_MAILSLOT_QUERY_INFORMATION),    // FileMailslotQueryInformation
	sizeof(xboxkrnl::FILE_MAILSLOT_SET_INFORMATION),      // FileMailslotSetInformation
	sizeof(xboxkrnl::FILE_COMPRESSION_INFORMATION),       // FileCompressionInformation
	0,                                                    // FileCopyOnWriteInformation
	sizeof(xboxkrnl::FILE_COMPLETION_INFORMATION),        // FileCompletionInformation
	sizeof(xboxkrnl::FILE_MOVE_CLUSTER_INFORMATION),      // FileMoveClusterInformation
	0,                                                    // FileQuotaInformation
	sizeof(xboxkrnl::FILE_REPARSE_POINT_INFORMATION),     // FileReparsePointInformation
	sizeof(xboxkrnl::FILE_NETWORK_OPEN_INFORMATION),      // FileNetworkOpenInformation
	sizeof(xboxkrnl::FILE_DIRECTORY_INFORMATION),         // FileObjectIdInformation
	sizeof(xboxkrnl::FILE_TRACKING_INFORMATION),          // FileTrackingInformation
	0,                                                    // FileOleDirectoryInformation
	0,                                                    // FileContentIndexInformation
	0,                                                    // FileInheritContentIndexInformation
	0                                                     // FileOleInformation
};


class EmuNtObject;

struct NativeObjectAttributes {
	wchar_t wszObjectName[160];
	NtDll::UNICODE_STRING    NtUnicodeString;
	NtDll::OBJECT_ATTRIBUTES NtObjAttr;
	// This is what should be passed on to Windows
	// after CxbxObjectAttributesToNT() has been called :
	NtDll::POBJECT_ATTRIBUTES NtObjAttrPtr;
};

NTSTATUS CxbxObjectAttributesToNT(xboxkrnl::POBJECT_ATTRIBUTES ObjectAttributes, NativeObjectAttributes& nativeObjectAttributes, std::string aFileAPIName = "");
NTSTATUS CxbxConvertFilePath(std::string RelativeXboxPath, OUT std::wstring &RelativeHostPath, OUT NtDll::HANDLE *RootDirectory, std::string aFileAPIName = "");

// ******************************************************************
// * Wrapper of a handle object
// ******************************************************************
class EmuHandle
{
public:
	EmuHandle(EmuNtObject* ntObject);
	NTSTATUS NtClose();
	NTSTATUS NtDuplicateObject(PHANDLE TargetHandle, DWORD Options);
	EmuNtObject* NtObject;
};

// ******************************************************************
// * An NT fake object
// ******************************************************************
class EmuNtObject
{
public:
	EmuNtObject();
	HANDLE NewHandle();
	NTSTATUS NtClose();
	EmuNtObject* NtDuplicateObject(DWORD Options);
protected:
	virtual ~EmuNtObject() {};
private:
	ULONG RefCount;

};

// ******************************************************************
// * Emulated symbolic link handle
// ******************************************************************
class EmuNtSymbolicLinkObject : public EmuNtObject {
public:
	char DriveLetter;
	std::string SymbolicLinkName;
	bool IsHostBasedPath;
	std::string XboxSymbolicLinkPath;
	std::string HostSymbolicLinkPath;
	HANDLE RootDirectoryHandle;
	NTSTATUS Init(std::string aSymbolicLinkName, std::string aFullPath);
	~EmuNtSymbolicLinkObject();
};

struct XboxDevice {
	std::string XboxDevicePath;
	std::string HostDevicePath;
	HANDLE HostRootHandle;
};

// ******************************************************************
// * is Handle a 'special' emulated handle?
// ******************************************************************
bool IsEmuHandle(HANDLE Handle);
EmuHandle* HandleToEmuHandle(HANDLE Handle);
HANDLE EmuHandleToHandle(EmuHandle* emuHandle);

CHAR* NtStatusToString(IN NTSTATUS Status);

int CxbxRegisterDeviceHostPath(std::string XboxFullPath, std::string HostDevicePath, bool IsFile = false);
int CxbxDeviceIndexByDevicePath(const char *XboxDevicePath);
XboxDevice *CxbxDeviceByDevicePath(const std::string XboxDevicePath);

char SymbolicLinkToDriveLetter(std::string aSymbolicLinkName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDriveLetter(const char DriveLetter);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByName(std::string SymbolicLinkName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDevice(std::string DeviceName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByRootHandle(HANDLE Handle);
void CleanupSymbolicLinks();

HANDLE CxbxGetDeviceNativeRootHandle(std::string XboxFullPath);
NTSTATUS CxbxCreateSymbolicLink(std::string SymbolicLinkName, std::string FullPath);
bool CxbxMountUtilityDrive(bool formatClean);
bool CxbxIsUtilityDrive(NtDll::HANDLE RootDirectory);

std::wstring string_to_wstring(std::string const & src);
std::wstring PUNICODE_STRING_to_wstring(NtDll::PUNICODE_STRING const & src);
std::string PSTRING_to_string(xboxkrnl::PSTRING const & src);
void copy_string_to_PSTRING_to(std::string const & src, const xboxkrnl::PSTRING & dest);

static int NtFileDirectoryInformationSize = sizeof(NtDll::FILE_DIRECTORY_INFORMATION) - 1;
static int NtPathBufferSize = MAX_PATH * sizeof(wchar_t);

// Deletes structs created by the converters
void _CxbxPVOIDDeleter(PVOID *ptr);

// Creates a PVOID variable named var which takes the given value
// and is automatically deleted when it goes out of scope
#define SMART_PVOID(var, value, orig)                     \
	PVOID var = value;                                    \
	std::shared_ptr<PVOID> __var_shared_ptr;              \
	if (NULL == var)                                      \
	{                                                     \
		__var_shared_ptr = NULL;                          \
		var = orig;                                       \
	}                                                     \
	else                                                  \
		__var_shared_ptr = std::shared_ptr<PVOID>(&var, _CxbxPVOIDDeleter);

// Converts an Xbox FileInformation struct to the NT equivalent.
// Used by NtSetInformationFile.
#define XboxToNTFileInformation(var, i, c, l)  SMART_PVOID(var, _XboxToNTFileInformation(i, c, l), i)
PVOID _XboxToNTFileInformation
(
	IN  PVOID xboxFileInformation,
	IN  ULONG FileInformationClass,
	OUT ULONG *Length
);

// Converts an NT FileInformation struct to the Xbox equivalent.
// Used by NtQueryInformationFile and NtQueryDirectoryFile
NTSTATUS NTToXboxFileInformation
(
	IN  PVOID nativeFileInformation,
	OUT PVOID xboxFileInformation,
	IN  ULONG FileInformationClass,
	IN  ULONG Length
);
#endif
