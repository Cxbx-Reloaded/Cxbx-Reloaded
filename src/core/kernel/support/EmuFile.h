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


#include <core\kernel\exports\xboxkrnl.h>
#include <filesystem>
#include "core\kernel\init\CxbxKrnl.h"
#include <vector>
#include <cstdio>
#include <string>
#include <memory>
#include <unordered_set>
#include <shared_mutex>

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

#define CHIHIRO_MBCOM_HANDLE (HANDLE)0x4D434F3D // MCOM in ascii

extern const std::string MediaBoardRomFile;
extern const std::string MediaBoardSegaBoot0;
extern const std::string MediaBoardSegaBoot1;
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
extern const std::string DriveG;
extern const std::string DriveH;
extern const std::string DriveI;
extern const std::string DriveJ;
extern const std::string DriveK;
extern const std::string DriveL;
extern const std::string DriveM;
extern const std::string DriveZ;
extern const std::string DevicePrefix;
extern const std::string DeviceCdrom0;
extern const std::string DeviceHarddisk0;
extern const std::string MUPrefix;
extern const std::string DeviceMUPrefix;
extern const std::string PartitionPrefix;
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
extern const std::string DeviceMU0;
extern const std::string DeviceMU1;
extern const std::string DeviceMU2;
extern const std::string DeviceMU3;
extern const std::string DeviceMU4;
extern const std::string DeviceMU5;
extern const std::string DeviceMU6;
extern const std::string DeviceMU7;
constexpr char CxbxAutoMountDriveLetter = 'D'; // TODO: Make this useful, likely need to be "D:" string. Check in EmuFile or various Emu prefix source files.

inline constexpr xbox::ulong_xt fsctl_dismount_volume = 0x00090020;
inline constexpr xbox::ulong_xt fsctl_read_fatx_metadata = 0x0009411C;
inline constexpr xbox::ulong_xt fsctl_write_fatx_metadata = 0x00098120;

inline constexpr std::size_t mu_max_name_lenght = 32 * sizeof(xbox::wchar_xt); // MU names are in wide chars
extern std::string g_DiskBasePath;
extern std::string g_MuBasePath;
extern std::string g_TitleMountPath;

// TODO: Need verify if matches with xbox kernel and anything else.
static const size_t IopQueryOperationLength[xbox::FileMaximumInformation + 1] = {
	0,                                                    // (index 0, unused)
	sizeof(xbox::FILE_DIRECTORY_INFORMATION),             // FileDirectoryInformation
	sizeof(xbox::FILE_DIRECTORY_INFORMATION),             // FileFullDirectoryInformation
	sizeof(xbox::FILE_DIRECTORY_INFORMATION),             // FileBothDirectoryInformation
	sizeof(xbox::FILE_BASIC_INFORMATION),                 // FileBasicInformation
	sizeof(xbox::FILE_STANDARD_INFORMATION),              // FileStandardInformation
	sizeof(xbox::FILE_INTERNAL_INFORMATION),              // FileInternalInformation
	sizeof(xbox::FILE_EA_INFORMATION),                    // FileEaInformation
	sizeof(xbox::FILE_ACCESS_INFORMATION),                // FileAccessInformation
	sizeof(xbox::FILE_NAME_INFORMATION),                  // FileNameInformation
	sizeof(xbox::FILE_RENAME_INFORMATION),                // FileRenameInformation
	sizeof(xbox::FILE_LINK_INFORMATION),                  // FileLinkInformation
	sizeof(xbox::FILE_DIRECTORY_INFORMATION),             // FileNamesInformation
	sizeof(xbox::FILE_DISPOSITION_INFORMATION),           // FileDispositionInformation
	sizeof(xbox::FILE_POSITION_INFORMATION),              // FilePositionInformation
	sizeof(xbox::FILE_FULL_EA_INFORMATION),               // FileFullEaInformation
	sizeof(xbox::FILE_MODE_INFORMATION),                  // FileModeInformation
	sizeof(xbox::FILE_ALIGNMENT_INFORMATION),             // FileAlignmentInformation
	sizeof(xbox::FILE_ALL_INFORMATION),                   // FileAllInformation
	sizeof(xbox::FILE_ALLOCATION_INFORMATION),            // FileAllocationInformation
	sizeof(xbox::FILE_END_OF_FILE_INFORMATION),           // FileEndOfFileInformation
	sizeof(xbox::FILE_NAME_INFORMATION),                  // FileAlternateNameInformation
	sizeof(xbox::FILE_STREAM_INFORMATION),                // FileStreamInformation
	sizeof(xbox::FILE_PIPE_INFORMATION),                  // FilePipeInformation
	sizeof(xbox::FILE_PIPE_LOCAL_INFORMATION),            // FilePipeLocalInformation
	sizeof(xbox::FILE_PIPE_REMOTE_INFORMATION),           // FilePipeRemoteInformation
	sizeof(xbox::FILE_MAILSLOT_QUERY_INFORMATION),        // FileMailslotQueryInformation
	sizeof(xbox::FILE_MAILSLOT_SET_INFORMATION),          // FileMailslotSetInformation
	sizeof(xbox::FILE_COMPRESSION_INFORMATION),           // FileCompressionInformation
	sizeof(xbox::FILE_DIRECTORY_INFORMATION),             // FileObjectIdInformation
	sizeof(xbox::FILE_COMPLETION_INFORMATION),            // FileCompletionInformation
	sizeof(xbox::FILE_MOVE_CLUSTER_INFORMATION),          // FileMoveClusterInformation
	0,                                                    // FileQuotaInformation
	sizeof(xbox::FILE_REPARSE_POINT_INFORMATION),         // FileReparsePointInformation
	sizeof(xbox::FILE_NETWORK_OPEN_INFORMATION),          // FileNetworkOpenInformation
	sizeof(xbox::FILE_ATTRIBUTE_TAG_INFORMATION),         // FileAttributeTagInformation
	sizeof(xbox::FILE_TRACKING_INFORMATION),              // FileTrackingInformation
	0xFF                                                  // FileMaximumInformation
};

struct NativeObjectAttributes {
	wchar_t wszObjectName[160];
	NtDll::UNICODE_STRING    NtUnicodeString;
	NtDll::OBJECT_ATTRIBUTES NtObjAttr;
	// This is what should be passed on to Windows
	// after CxbxObjectAttributesToNT() has been called :
	NtDll::POBJECT_ATTRIBUTES NtObjAttrPtr;
};

NTSTATUS CxbxObjectAttributesToNT(xbox::POBJECT_ATTRIBUTES ObjectAttributes, NativeObjectAttributes& nativeObjectAttributes, std::string aFileAPIName = "", bool partitionHeader = false);

std::string CxbxrDiskDeviceByHostPath(const std::filesystem::path& HostDiskDevicePath); // From EmuDisk for exposure.

typedef struct _fatx_volume_metadata {
	xbox::dword_xt offset;
	xbox::dword_xt length;
	xbox::PVOID buffer;
} fatx_volume_metadata, *pfatx_volume_metadata;

const CHAR* NtStatusToString(IN NTSTATUS Status);

class io_mu_metadata
{
public:
	io_mu_metadata(const std::wstring_view root_path);
	~io_mu_metadata();
	void read(const xbox::dword_xt PartitionNumber, std::size_t offset, char *buff, std::size_t size);
	void write(const xbox::dword_xt PartitionNumber, std::size_t offset, const char *buff, std::size_t size);
	void flush(const wchar_t lett);

private:
	char *m_buff[8];
	const std::wstring m_root_path;
	std::shared_mutex m_rw_lock;
};
extern io_mu_metadata *g_io_mu_metadata;

void CxbxrSetupDrives(std::filesystem::path& CdRomPath, int BootFlags);
void CxbxCreatePartitionHeaderFile(const std::filesystem::path& filename, bool partition0 = false, std::size_t size = 512 * ONE_KB);

std::string CxbxConvertXboxToHostPath(const std::string_view XboxDevicePath);
bool CxbxrIsPathInsideEmuDisk(const std::filesystem::path& path);
bool CxbxrIsPathInsideEmuMu(const std::filesystem::path& path);

std::wstring string_to_wstring(std::string const & src);
std::wstring PUNICODE_STRING_to_wstring(NtDll::PUNICODE_STRING const & src);
std::string PSTRING_to_string(xbox::PSTRING const & src);
void copy_string_to_PSTRING_to(std::string const & src, const xbox::PSTRING & dest);

static int NtFileDirectoryInformationSize = sizeof(NtDll::FILE_DIRECTORY_INFORMATION) - 1;
static int NtPathBufferSize = MAX_PATH * sizeof(wchar_t);

// Deletes structs created by the converters
void _CxbxPVOIDDeleter(PVOID *ptr);

// NOTE: Was used with XboxToNTFileInformation except it is removed as not needed anymore.
// Creates a PVOID variable named var which takes the given value
// and is automatically deleted when it goes out of scope
#define SMART_PVOID(var, value, orig)                     \
	PVOID var = value;                                    \
	std::shared_ptr<PVOID> __var_shared_ptr;              \
	if (nullptr == var)                                   \
	{                                                     \
		__var_shared_ptr = nullptr;                       \
		var = orig;                                       \
	}                                                     \
	else                                                  \
		__var_shared_ptr = std::shared_ptr<PVOID>(&var, _CxbxPVOIDDeleter);

// Converts an NT FileInformation struct to the Xbox equivalent.
// Used by NtQueryInformationFile and NtQueryDirectoryFile
NTSTATUS NTToXboxFileInformation
(
	IN  PVOID nativeFileInformation,
	OUT PVOID xboxFileInformation,
	IN  ULONG FileInformationClass,
	IN  ULONG Length
);

// Xbox Partition Information
typedef struct
{
	UCHAR Name[16];
	ULONG Flags;
	ULONG LBAStart;
	ULONG LBASize;
	ULONG Reserved;
} XboxPartitionTableEntry;

typedef struct
{
	UCHAR	Magic[16];
	char	Res0[32];
	XboxPartitionTableEntry	TableEntries[14];
} XboxPartitionTable;

inline constexpr xbox::ulong_xt fatx_name_length = 32;
inline constexpr xbox::ulong_xt fatx_online_data_length = 2048;
inline constexpr xbox::ulong_xt fatx_reserved_length = 1968;

inline constexpr xbox::ulong_xt fatx_signature = 'XTAF';

typedef struct _FATX_SUPERBLOCK
{
	xbox::ulong_xt Signature;
	xbox::ulong_xt VolumeID;
	xbox::ulong_xt ClusterSize;
	xbox::ulong_xt FatCopies;
	xbox::wchar_xt Name[fatx_name_length];
	xbox::uchar_xt OnlineData[fatx_online_data_length];
	xbox::uchar_xt Unused[fatx_reserved_length];
} FATX_SUPERBLOCK;

static_assert(sizeof(FATX_SUPERBLOCK) == PAGE_SIZE);

XboxPartitionTable CxbxGetPartitionTable();
FATX_SUPERBLOCK CxbxGetFatXSuperBlock(int partitionNumber);

// From EmuDisk source file
bool EmuDiskFormatPartition(xbox::dword_xt PartitionNumber);

// Ensures that an original IoStatusBlock gets passed to the completion callback
// Used by NtReadFile and NtWriteFile
using CxbxIoDispatcherContext = std::tuple<xbox::PIO_STATUS_BLOCK, xbox::PIO_APC_ROUTINE, PVOID>;

void NTAPI CxbxIoApcDispatcher
(
	PVOID                  ApcContext,
	xbox::PIO_STATUS_BLOCK IoStatusBlock,
	xbox::ulong_xt         Reserved
);

void CxbxLaunchNewXbe(const std::string& XbePath);

#endif
