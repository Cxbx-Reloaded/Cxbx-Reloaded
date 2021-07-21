// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
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

#define LOG_PREFIX CXBXR_MODULE::FILE

#include "EmuFile.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <Shlobj.h>
#include <Shlwapi.h>
#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include "Logging.h"
#include "common/util/strConverter.hpp" // utf16_to_ascii
#include "common/util/cliConfig.hpp"
#include "common/CxbxDebugger.h"
#include "EmuShared.h"

#include <filesystem>

// partition emulation directory handles
HANDLE g_hCurDir_hack = NULL; // HACK: We should not be depending on this variable. Instead, we should fix/implement Ob/Io objects such as IoCreateDevice.

HANDLE g_DiskBasePathHandle;
std::string g_DiskBasePath;
std::string g_MuBasePath;

// Default Xbox Partition Table
#define PE_PARTFLAGS_IN_USE	0x80000000
#define XBOX_SWAPPART1_LBA_START		0x400
#define XBOX_SWAPPART_LBA_SIZE			0x177000
#define XBOX_SWAPPART2_LBA_START		(XBOX_SWAPPART1_LBA_START + XBOX_SWAPPART_LBA_SIZE)
#define XBOX_SWAPPART3_LBA_START		(XBOX_SWAPPART2_LBA_START + XBOX_SWAPPART_LBA_SIZE)

#define XBOX_SYSPART_LBA_START			(XBOX_SWAPPART3_LBA_START + XBOX_SWAPPART_LBA_SIZE)
#define XBOX_SYSPART_LBA_SIZE			0xfa000

#define XBOX_MUSICPART_LBA_START		(XBOX_SYSPART_LBA_START + XBOX_SYSPART_LBA_SIZE)
#define XBOX_MUSICPART_LBA_SIZE			0x9896b0

XboxPartitionTable BackupPartTbl =
{
	{'*', '*', '*', '*', 'P', 'A', 'R', 'T', 'I', 'N', 'F', 'O', '*', '*', '*', '*'},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{
		{{'X', 'B', 'O', 'X', ' ', 'S', 'H', 'E', 'L', 'L', ' ', ' ', ' ', ' ', ' ', ' '}, PE_PARTFLAGS_IN_USE, XBOX_MUSICPART_LBA_START, XBOX_MUSICPART_LBA_SIZE, 0},
		{{'X', 'B', 'O', 'X', ' ', 'D', 'A', 'T', 'A', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, PE_PARTFLAGS_IN_USE, XBOX_SYSPART_LBA_START, XBOX_SYSPART_LBA_SIZE, 0},
		{{'X', 'B', 'O', 'X', ' ', 'G', 'A', 'M', 'E', ' ', 'S', 'W', 'A', 'P', ' ', '1'}, PE_PARTFLAGS_IN_USE, XBOX_SWAPPART1_LBA_START, XBOX_SWAPPART_LBA_SIZE, 0},
		{{'X', 'B', 'O', 'X', ' ', 'G', 'A', 'M', 'E', ' ', 'S', 'W', 'A', 'P', ' ', '2'}, PE_PARTFLAGS_IN_USE, XBOX_SWAPPART2_LBA_START, XBOX_SWAPPART_LBA_SIZE, 0},
		{{'X', 'B', 'O', 'X', ' ', 'G', 'A', 'M', 'E', ' ', 'S', 'W', 'A', 'P', ' ', '3'}, PE_PARTFLAGS_IN_USE, XBOX_SWAPPART3_LBA_START, XBOX_SWAPPART_LBA_SIZE, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
		{{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0, 0, 0},
	}
};

io_mu_metadata *g_io_mu_metadata = nullptr;

io_mu_metadata::io_mu_metadata(const std::wstring_view root_path) : m_root_path(root_path)
{
	for (unsigned i = 0; i < 8; ++i) {
		m_buff[i] = new char[sizeof(FATX_SUPERBLOCK)];
		assert(m_buff[i] != nullptr);
		std::wstring path = m_root_path + static_cast<wchar_t>(L'F' + i) + L".bin";
		std::fstream fs(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		if (!fs.is_open()) {
			CxbxKrnlCleanup("%s: could not open MU bin file at \"%ls\"!", __func__, path.c_str());
		}
		fs.seekg(0);
		fs.read(m_buff[i], sizeof(FATX_SUPERBLOCK));
		// if the signature is not "fatx" or we read less bytes than expected, then we assume the bin file is either corrupted or
		// unformatted, so we reformat it now
		FATX_SUPERBLOCK *volume = reinterpret_cast<FATX_SUPERBLOCK *>(m_buff[i]);
		if ((fs.gcount() != sizeof(FATX_SUPERBLOCK)) || (volume->Signature != fatx_signature)) {
			volume->Signature = fatx_signature;
			volume->VolumeID = 0x11223344 + i;
			volume->ClusterSize = 32;
			volume->FatCopies = 1;
			std::memset(volume->Name, 0, mu_max_name_lenght);
			std::memset(volume->OnlineData, 0, fatx_online_data_length);
			std::memset(volume->Unused, 0xFF, fatx_reserved_length);
			fs.write(m_buff[i], sizeof(FATX_SUPERBLOCK));
		}
	}
}

io_mu_metadata::~io_mu_metadata()
{
	std::unique_lock<std::shared_mutex> lck(m_rw_lock);
	for (unsigned i = 0; i < 8; ++i) {
		std::wstring path = m_root_path + static_cast<wchar_t>(L'F' + i) + L".bin";
		std::ofstream ofs(path, std::ios_base::out | std::ios_base::binary);
		if (!ofs.is_open()) {
			EmuLog(LOG_LEVEL::ERROR2, "%s: could not open MU bin file at \"%ls\"!", __func__, path.c_str());
			delete[] m_buff[i];
			continue;
		}
		ofs.seekp(0);
		ofs.write(m_buff[i], sizeof(FATX_SUPERBLOCK));
		delete[] m_buff[i];
	}
}

void io_mu_metadata::read(const wchar_t lett, std::size_t offset, char *buff, std::size_t size)
{
	std::shared_lock<std::shared_mutex> lck(m_rw_lock); // allows for concurrent reads
	std::memcpy(buff, m_buff[lett - L'F'] + offset, size);
}

void io_mu_metadata::write(const wchar_t lett, std::size_t offset, const char *buff, std::size_t size)
{
	std::unique_lock<std::shared_mutex> lck(m_rw_lock); // blocks when there is rw in progress
	std::memcpy(m_buff[lett - L'F'] + offset, buff, size);
}

void io_mu_metadata::flush(const wchar_t lett)
{
	std::unique_lock<std::shared_mutex> lck(m_rw_lock);
	std::wstring path = m_root_path + lett + L".bin";
	std::ofstream ofs(path, std::ios_base::out | std::ios_base::binary);
	if (!ofs.is_open()) {
		EmuLog(LOG_LEVEL::ERROR2, "%s: could not open MU bin file at \"%ls\"!", __func__, path.c_str());
		return;
	}
	ofs.seekp(0);
	ofs.write(m_buff[lett - L'F'], sizeof(FATX_SUPERBLOCK));
	ofs.flush();
}

// NOTE: root_path input must already had called canonical function for optimization purpose.
static bool CxbxrIsPathInsideRootPath(const std::filesystem::path& path, const std::filesystem::path& root_path)
{
	std::error_code rootError, finalError;
	const std::filesystem::path rootPath = std::filesystem::canonical(root_path, rootError); // TODO: Replace rootPath to root_path when possible.
	const std::filesystem::path finalPath = std::filesystem::canonical(path, finalError);
	if (rootError || finalError) {
		return false;
	}
	auto match = std::mismatch(rootPath.begin(), rootPath.end(), finalPath.begin(), finalPath.end());
	return match.first == rootPath.end();
}

bool CxbxrIsPathInsideEmuDisk(const std::filesystem::path& path)
{
	return CxbxrIsPathInsideRootPath(path, g_DiskBasePath);
}

static bool CxbxrIsPathInsideEmuMu(const std::filesystem::path& path)
{
	return CxbxrIsPathInsideRootPath(path, g_MuBasePath);
}

DeviceType CxbxrGetDeviceTypeFromHandle(HANDLE hFile)
{
	const std::filesystem::path path = CxbxGetFinalPathNameByHandle(hFile);

	if (CxbxrIsPathInsideEmuDisk(path)) {
		return DeviceType::Harddisk0;
	}

	if (CxbxrIsPathInsideEmuMu(path)) {
		return DeviceType::MU;
	}

	EmuDirPath hybrid_path;
	FindEmuDirPathByDevice(DeviceCdrom0, hybrid_path);
	if (!hybrid_path.HostDirPath.empty() && CxbxrIsPathInsideRootPath(path, hybrid_path.HostDirPath)) {
		return DeviceType::Cdrom0;
	}

	return DeviceType::Invalid;
}

void CxbxCreatePartitionHeaderFile(std::string filename, bool partition0 = false, std::size_t size = 512 * ONE_KB)
{
	HANDLE hf = CreateFile(filename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (!hf) {
		CxbxKrnlCleanup("CxbxCreatePartitionHeaderFile Failed\nUnable to create file: %s (%s)", filename.c_str());
		return;
	}

	// If this is partition 0, install the partiton table
	if (partition0) {
		DWORD NumberOfBytesWritten = 0;
		WriteFile(hf, &BackupPartTbl, sizeof(XboxPartitionTable), &NumberOfBytesWritten, 0);
	}

	SetFilePointer(hf, size, 0, FILE_BEGIN);
	SetEndOfFile(hf);
	CloseHandle(hf);
}

XboxPartitionTable CxbxGetPartitionTable()
{
	XboxPartitionTable table;
	FILE* fp = fopen((g_DiskBasePath + "Partition0.bin").c_str(), "rb");
	if (fp == nullptr) {
		CxbxKrnlCleanup("CxbxGetPartitionTable Failed:\nUnable to open file: %s", (g_DiskBasePath + "Partition0.bin").c_str());
	}

	fread(&table, sizeof(XboxPartitionTable), 1, fp);
	fclose(fp);

	// If the partition table is not valid, format it
	// This allows recovery from corrupted partition tables
	// Or invalid partition tables left behind from previous versions
	// of Cxbx-Reloaded
	if (memcmp(table.Magic, BackupPartTbl.Magic, 16) != 0) {
		DeleteFile((g_DiskBasePath + "Partition0.bin").c_str());
		CxbxCreatePartitionHeaderFile(g_DiskBasePath + "Partition0.bin", true);
		memcpy(&table, &BackupPartTbl, sizeof(XboxPartitionTable));
	}

	return table;
}

FATX_SUPERBLOCK CxbxGetFatXSuperBlock(int partitionNumber)
{
	FATX_SUPERBLOCK superblock;
	std::stringstream ss;

	ss << g_DiskBasePath << "Partition" << partitionNumber << ".bin";
	FILE* fp = fopen(ss.str().c_str(), "rb");
	fread(&superblock, sizeof(FATX_SUPERBLOCK), 1, fp);
	fclose(fp);
	return superblock;
}

std::wstring CxbxGetFinalPathNameByHandle(HANDLE hFile)
{
	constexpr size_t INITIAL_BUF_SIZE = MAX_PATH;
	std::wstring path(INITIAL_BUF_SIZE, '\0');

	DWORD size = GetFinalPathNameByHandleW(hFile, path.data(), INITIAL_BUF_SIZE, VOLUME_NAME_DOS);
	if (size == 0) {
		CxbxKrnlCleanup("CxbxGetPartitionNumberFromHandle Failed:\nUnable to determine path for HANDLE 0x%08X", hFile);
	}

	// If the function fails because lpszFilePath is too small to hold the string plus the terminating null character,
	// the return value is the required buffer size, in TCHARs. This value includes the size of the terminating null character.
	if (size >= INITIAL_BUF_SIZE) {
		path.resize(size);
		size = GetFinalPathNameByHandleW(hFile, path.data(), size, VOLUME_NAME_DOS);
	}
	path.resize(size);

	return path;
}

static int CxbxGetPartitionNumber(const std::wstring_view path)
{
	const std::wstring_view partitionString = L"\\EmuDisk\\Partition";
	const size_t pos = path.rfind(partitionString);
	if (pos == std::string::npos) {
		return 0;
	}
	const std::wstring_view partitionNumberString = path.substr(pos + partitionString.length(), 1);

	// wcstol returns 0 on non-numeric characters, so we don't need to error check here
	return wcstol(partitionNumberString.data(), nullptr, 0);
}

int CxbxGetPartitionNumberFromPath(const std::wstring_view path)
{
	return CxbxGetPartitionNumber(path);
}

int CxbxGetPartitionNumberFromHandle(HANDLE hFile)
{
	// Get which partition number is being accessed, by parsing the filename and extracting the last portion 
	const std::wstring path = CxbxGetFinalPathNameByHandle(hFile);

	return CxbxGetPartitionNumber(path);
}

std::filesystem::path CxbxGetPartitionDataPathFromHandle(HANDLE hFile)
{
	// Get which partition number is being accessed, by parsing the filename and extracting the last portion 
	const std::wstring path = CxbxGetFinalPathNameByHandle(hFile);

	std::wstring partitionPath;
	const std::wstring_view partitionString = L"\\EmuDisk\\Partition";
	const size_t pos = path.rfind(partitionString);
	if (pos != std::string::npos) {
		partitionPath = path.substr(0, path.find(partitionString) + partitionString.length() + 1);
	}
	return partitionPath;
}

void CxbxFormatPartitionByHandle(HANDLE hFile)
{
	const std::filesystem::path partitionPath = CxbxGetPartitionDataPathFromHandle(hFile);

	// Sanity check, make sure we are actually deleting something within the Cxbx-Reloaded folder
	if (!CxbxrIsPathInsideEmuDisk(partitionPath)) {
		EmuLog(LOG_LEVEL::WARNING, "Attempting to format a path that is not within a Cxbx-Reloaded data folder... Ignoring!\n");
		return;
	}


	// Format the partition, by iterating through the contents and removing all files/folders within
	// Previously, we deleted and re-created the folder, but that caused permission issues for some users
	std::error_code er;
	for (const auto& directoryEntry : std::filesystem::directory_iterator(partitionPath, er)) {
		if (!er) {
			std::filesystem::remove_all(directoryEntry, er);
		}
	}

	printf("Formatted EmuDisk Partition%d\n", CxbxGetPartitionNumberFromHandle(hFile));
}

void NTAPI CxbxIoApcDispatcher(PVOID ApcContext, xbox::PIO_STATUS_BLOCK /*IoStatusBlock*/, xbox::ulong_xt Reserved)
{
	CxbxIoDispatcherContext* cxbxContext = reinterpret_cast<CxbxIoDispatcherContext*>(ApcContext);
	std::get<xbox::PIO_APC_ROUTINE>(*cxbxContext)(
		std::get<LPVOID>(*cxbxContext),std::get<xbox::PIO_STATUS_BLOCK>(*cxbxContext), Reserved);
	delete cxbxContext;
}

const std::string MediaBoardRomFile = "Chihiro\\fpr21042_m29w160et.bin";
const std::string MediaBoardSegaBoot0 = "Chihiro\\SEGABOOT_MBROM0.XBE";
const std::string MediaBoardSegaBoot1 = "Chihiro\\SEGABOOT_MBROM1.XBE";
const std::string DrivePrefix = "\\??\\";
const std::string DriveSerial = DrivePrefix + "serial:";
const std::string DriveCdRom0 = DrivePrefix + "CdRom0:"; // CD-ROM device
const std::string DriveMbfs = "mbfs:"; // media board's file system area device
const std::string DriveMbcom = "mbcom:"; // media board's communication area device
const std::string DriveMbrom0 = "mbrom0:"; // media board's boot ROM device (first image)
const std::string DriveMbrom1 = "mbrom1:"; // media board's boot ROM device (second image)
const std::string DriveA = DrivePrefix + "A:"; // A: could be CDROM
const std::string DriveC = DrivePrefix + "C:"; // C: is HDD0
const std::string DriveD = DrivePrefix + "D:"; // D: is DVD Player
const std::string DriveE = DrivePrefix + "E:";
const std::string DriveF = DrivePrefix + "F:"; // MU port 0, slot top
const std::string DriveG = DrivePrefix + "G:"; // MU port 0, slot bottom
const std::string DriveH = DrivePrefix + "H:"; // MU port 1, slot top
const std::string DriveI = DrivePrefix + "I:"; // MU port 1, slot bottom
const std::string DriveJ = DrivePrefix + "J:"; // MU port 2, slot top
const std::string DriveK = DrivePrefix + "K:"; // MU port 2, slot bottom
const std::string DriveL = DrivePrefix + "L:"; // MU port 3, slot top
const std::string DriveM = DrivePrefix + "M:"; // MU port 3, slot bottom
const std::string DriveS = DrivePrefix + "S:";
const std::string DriveT = DrivePrefix + "T:"; // T: is Title persistent data region
const std::string DriveU = DrivePrefix + "U:"; // U: is User persistent data region
const std::string DriveV = DrivePrefix + "V:";
const std::string DriveW = DrivePrefix + "W:";
const std::string DriveX = DrivePrefix + "X:";
const std::string DriveY = DrivePrefix + "Y:"; // Y: is Dashboard volume (contains "xboxdash.xbe" and "XDASH" folder + contents)
const std::string DriveZ = DrivePrefix + "Z:"; // Z: is Title utility data region
const std::string DevicePrefix = "\\Device";
const std::string DeviceCdrom0 = DevicePrefix + "\\CdRom0";
const std::string DeviceHarddisk0 = DevicePrefix + "\\Harddisk0";
const std::string DeviceMU = DevicePrefix + "\\MU_";
const std::string DeviceHarddisk0PartitionPrefix = DevicePrefix + "\\Harddisk0\\partition";
const std::string DeviceHarddisk0Partition0 = DeviceHarddisk0PartitionPrefix + "0"; // Contains raw config sectors (like XBOX_REFURB_INFO) + entire hard disk
const std::string DeviceHarddisk0Partition1 = DeviceHarddisk0PartitionPrefix + "1"; // Data partition. Contains TDATA and UDATA folders.
const std::string DeviceHarddisk0Partition2 = DeviceHarddisk0PartitionPrefix + "2"; // Shell partition. Contains Dashboard (cpxdash.xbe, evoxdash.xbe or xboxdash.xbe)
const std::string DeviceHarddisk0Partition3 = DeviceHarddisk0PartitionPrefix + "3"; // First cache partition. Contains cache data (from here up to largest number)
const std::string DeviceHarddisk0Partition4 = DeviceHarddisk0PartitionPrefix + "4";
const std::string DeviceHarddisk0Partition5 = DeviceHarddisk0PartitionPrefix + "5";
const std::string DeviceHarddisk0Partition6 = DeviceHarddisk0PartitionPrefix + "6";
const std::string DeviceHarddisk0Partition7 = DeviceHarddisk0PartitionPrefix + "7";
const std::string DeviceHarddisk0Partition8 = DeviceHarddisk0PartitionPrefix + "8";
const std::string DeviceHarddisk0Partition9 = DeviceHarddisk0PartitionPrefix + "9";
const std::string DeviceHarddisk0Partition10 = DeviceHarddisk0PartitionPrefix + "10";
const std::string DeviceHarddisk0Partition11 = DeviceHarddisk0PartitionPrefix + "11";
const std::string DeviceHarddisk0Partition12 = DeviceHarddisk0PartitionPrefix + "12";
const std::string DeviceHarddisk0Partition13 = DeviceHarddisk0PartitionPrefix + "13";
const std::string DeviceHarddisk0Partition14 = DeviceHarddisk0PartitionPrefix + "14";
const std::string DeviceHarddisk0Partition15 = DeviceHarddisk0PartitionPrefix + "15";
const std::string DeviceHarddisk0Partition16 = DeviceHarddisk0PartitionPrefix + "16";
const std::string DeviceHarddisk0Partition17 = DeviceHarddisk0PartitionPrefix + "17";
const std::string DeviceHarddisk0Partition18 = DeviceHarddisk0PartitionPrefix + "18";
const std::string DeviceHarddisk0Partition19 = DeviceHarddisk0PartitionPrefix + "19";
const std::string DeviceHarddisk0Partition20 = DeviceHarddisk0PartitionPrefix + "20"; // 20 = Largest possible partition number
const std::string DeviceMU0 = DeviceMU + "0";
const std::string DeviceMU1 = DeviceMU + "1";
const std::string DeviceMU2 = DeviceMU + "2";
const std::string DeviceMU3 = DeviceMU + "3";
const std::string DeviceMU4 = DeviceMU + "4";
const std::string DeviceMU5 = DeviceMU + "5";
const std::string DeviceMU6 = DeviceMU + "6";
const std::string DeviceMU7 = DeviceMU + "7"; // 7 = Largest possible mu number

EmuNtSymbolicLinkObject* NtSymbolicLinkObjects['Z' - 'A' + 1];
std::vector<XboxDevice> Devices;

EmuHandle::EmuHandle(EmuNtObject* ntObject)
{
	NtObject = ntObject;
}

std::unordered_set<EmuHandle*> EmuHandle::EmuHandleLookup = {};
std::shared_mutex EmuHandle::EmuHandleLookupLock = {};

EmuHandle* EmuHandle::CreateEmuHandle(EmuNtObject* ntObject) {
	auto emuHandle = new EmuHandle(ntObject);

	// Register EmuHandle
	{
		std::unique_lock scopedLock(EmuHandleLookupLock);
		EmuHandleLookup.insert(emuHandle);
	}

	return emuHandle;
}

NTSTATUS EmuHandle::NtClose()
{
	auto status = NtObject->NtClose();

	// Unregister the handle
	if (status == STATUS_SUCCESS) {
		std::unique_lock scopedLock(EmuHandleLookupLock);
		EmuHandleLookup.erase(this);
	}

	return status;
}

bool EmuHandle::IsEmuHandle(HANDLE Handle)
{
	std::shared_lock scopedLock(EmuHandleLookupLock);
	auto iter = EmuHandleLookup.find((EmuHandle*) Handle);
	return !(iter == EmuHandleLookup.end());
}

NTSTATUS EmuHandle::NtDuplicateObject(PHANDLE TargetHandle, DWORD Options)
{
	*TargetHandle = NtObject->NtDuplicateObject(Options)->NewHandle();
	return xbox::status_success;
}

EmuNtObject::EmuNtObject()
{
	RefCount = 1;
}

HANDLE EmuNtObject::NewHandle()
{
	RefCount++;
	return EmuHandle::CreateEmuHandle(this);
}

NTSTATUS EmuNtObject::NtClose()
{
	if (--RefCount <= 0) {
		delete this;
	}

	return xbox::status_success;
}

EmuNtObject* EmuNtObject::NtDuplicateObject(DWORD Options)
{
	RefCount++;
	return this;
}

std::wstring string_to_wstring(std::string const & src)
{
	std::wstring result = std::wstring(src.length(), L' ');
	std::copy(src.begin(), src.end(), result.begin());
	return result;
}

std::wstring PUNICODE_STRING_to_wstring(NtDll::PUNICODE_STRING const & src)
{
	if (src == nullptr) {
		return L"";
	}

	return std::wstring(src->Buffer, src->Length / sizeof(NtDll::WCHAR));
}

std::string PSTRING_to_string(xbox::PSTRING const & src)
{
	if (src == nullptr) {
		return "";
	}

	return std::string(src->Buffer, src->Length);
}

void copy_string_to_PSTRING_to(std::string const & src, const xbox::PSTRING & dest)
{
	dest->Length = (USHORT)src.size();
	memcpy(dest->Buffer, src.c_str(), src.size());
}

void replace_all(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

NTSTATUS CxbxConvertFilePath(
	std::string RelativeXboxPath,
	OUT std::wstring &RelativeHostPath,
	IN OUT NtDll::HANDLE *RootDirectory,
	std::string aFileAPIName,
	bool partitionHeader)
{
	std::string OriginalPath = RelativeXboxPath;
	std::string RelativePath = RelativeXboxPath;
	std::string XboxFullPath;
	std::string HostPath;
	EmuNtSymbolicLinkObject* NtSymbolicLinkObject = nullptr;
	EmuDirPath find_path;

	// Always trim '\??\' off :
	if (RelativePath.compare(0, DrivePrefix.length(), DrivePrefix.c_str()) == 0)
		RelativePath.erase(0, 4);

	// Check if we were called from a File-handling API :
	if (!aFileAPIName.empty()) {
		if (RelativePath.compare(DriveMbrom0) == 0) {
			*RootDirectory = g_DiskBasePathHandle;
			HostPath = g_DiskBasePath;
			RelativePath = MediaBoardSegaBoot0;
		}
		else if (RelativePath.compare(DriveMbrom1) == 0) {
			*RootDirectory = g_DiskBasePathHandle;
			HostPath = g_DiskBasePath;
			RelativePath = MediaBoardSegaBoot1;
		}
		else if (!partitionHeader) {
			// Check if the path starts with a volume indicator :
			if ((RelativePath.length() >= 2) && (RelativePath[1] == ':')) {
				// Look up the symbolic link information using the drive letter :
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByDriveLetter(RelativePath[0]);
				RelativePath.erase(0, 2); // Remove 'C:'

				// If the remaining path starts with a ':', remove it (to prevent errors) :
				if ((RelativePath.length() > 0) && (RelativePath[0] == ':'))
					RelativePath.erase(0, 1);  // xbmp needs this, as it accesses 'e::\'
			}
			else if (RelativePath[0] == '$') {
				if (RelativePath.compare(0, 5, "$HOME") == 0) // "xbmp" needs this
				{
					NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir_hack);
					RelativePath.erase(0, 5); // Remove '$HOME'
				}
				else
					CxbxKrnlCleanup(("Unsupported path macro : " + OriginalPath).c_str());
			}
			// Check if the path starts with a relative path indicator :
			else if (RelativePath[0] == '.') {// "4x4 Evo 2" needs this
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir_hack);
				RelativePath.erase(0, 1); // Remove the '.'
			}
			else {
				// TODO : How should we handle accesses to the serial: (?semi-)volume?
				if (RelativePath.compare(0, 7, "serial:") == 0) {
					return STATUS_UNRECOGNIZED_VOLUME;
				}

				// TODO: CDROM0: need access to raw file handle which doesn't exist in file system.
				//       Similar concept with serial: and perhaps mediaboards.
				// Raw handle access to the CDROM0:
				/*if (RelativePath.compare(0, 7, "CDROM0:") == 0) {
					// we should have a return and likely forward to special handler function, including serial: above.
					return ?;
				}*/

				// The path seems to be a device path, look it up :
				FindEmuDirPathByDevice(RelativePath, find_path);
				// Fixup RelativePath path here
				if (!find_path.HostDirPath.empty()) {
					RelativePath.erase(0, find_path.XboxDirPath.length()); // Remove '\Device\Harddisk0\Partition2'
				}
			}

			if (NtSymbolicLinkObject != nullptr || !find_path.HostDirPath.empty()) {
				// If found, then we can skip misc checks below.
			}
			// Check if the path accesses a partition from Harddisk0 :
			else if (_strnicmp(RelativePath.c_str(), DeviceHarddisk0PartitionPrefix.c_str(), DeviceHarddisk0PartitionPrefix.length()) == 0) {
				XboxFullPath = RelativePath;
				// Remove Harddisk0 prefix, in the hope that the remaining path might work :
				RelativePath.erase(0, DeviceHarddisk0.length() + 1);
				// And set Root to the folder containing the partition-folders :
				*RootDirectory = g_DiskBasePathHandle;
				HostPath = g_DiskBasePath;
			}
			// NOTE: RootDirectory cannot be ignored.
			// Any special handling for it should be done below.
			else if (*RootDirectory == nullptr) {
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir_hack);
			}
			else if (*RootDirectory == ObDosDevicesDirectory()) {
				// This is a special handle that tells the API that this is a DOS device
				// We can safely remove it and forward to the Xbe directory.
				// Test case GTA3
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir_hack);
			}
			else if (*RootDirectory == ObWin32NamedObjectsDirectory()) {
				// NOTE: A handle of -4 on the Xbox signifies the path should be in the BaseNamedObjects namespace.
				// This handle doesn't exist on Windows, so we prefix the name instead. (note from LukeUsher)
				// Handle special root directory constants
				*RootDirectory = NULL;

				if (OriginalPath.size() == 0){
					RelativePath = "\\BaseNamedObjects";
				} else {
					RelativePath = "\\BaseNamedObjects\\" + OriginalPath;
				}
			}
			// else {} // NOTE: Allow RootDirectory handle to take control of relative path.
			// Test-case: Turok Evolution

			if (NtSymbolicLinkObject != nullptr) {
				HostPath = NtSymbolicLinkObject->HostSymbolicLinkPath;

				XboxFullPath = NtSymbolicLinkObject->XboxSymbolicLinkPath;

				// If accessing a partition as a directly, set the root directory handle and keep relative path as is
				*RootDirectory = NtSymbolicLinkObject->RootDirectoryHandle;
			}
			else if (!find_path.HostDirPath.empty()) {
				HostPath = find_path.HostDirPath;
				XboxFullPath = find_path.XboxDirPath;
				*RootDirectory = find_path.HostDirHandle;
			}
		} else {
			*RootDirectory = g_DiskBasePathHandle;
			HostPath = g_DiskBasePath;
			RelativePath = RelativeXboxPath.substr(DeviceHarddisk0.length()) + ".bin";
		}

		// If the remaining path starts with a '\', remove it (to prevent working in a native root) :
		if ((RelativePath.length() > 0) && (RelativePath[0] == '\\')) {
			RelativePath.erase(0, 1);
			// And if needed, add it to the host path instead :
			if (HostPath.back() != '\\')
				HostPath.append(1, '\\');
		}

		// Lastly, remove any '\\' sequences in the string (this should fix the problem with Azurik game saves)
		replace_all( RelativePath, "\\\\", "\\" );

		if (g_bPrintfOn) {
			EmuLog(LOG_LEVEL::DEBUG, "%s Corrected path...", aFileAPIName.c_str());
			EmuLog(LOG_LEVEL::DEBUG, "  Org:\"%s\"", OriginalPath.c_str());
			if (_strnicmp(HostPath.c_str(), g_DiskBasePath.c_str(), g_DiskBasePath.length()) == 0) {
				EmuLog(LOG_LEVEL::DEBUG, "  New:\"$CxbxPath\\%s%s\"", (HostPath.substr(g_DiskBasePath.length(), std::string::npos)).c_str(), RelativePath.c_str());
			}
			else
				EmuLog(LOG_LEVEL::DEBUG, "  New:\"$XbePath\\%s\"", RelativePath.c_str());
		}
	}
	else
	{
		// For non-file API calls, prefix with '\??\' again :
		RelativePath = DrivePrefix + RelativePath;
		*RootDirectory = 0;
	}

	// Convert the relative path to unicode
	RelativeHostPath = string_to_wstring(RelativePath);

	return xbox::status_success;
}

NTSTATUS CxbxObjectAttributesToNT(
	xbox::POBJECT_ATTRIBUTES ObjectAttributes, 
	OUT NativeObjectAttributes& nativeObjectAttributes, 
	const std::string aFileAPIName,
	bool partitionHeader)
{
	if (ObjectAttributes == NULL)
	{
		// When the pointer is nil, make sure we pass nil to Windows too :
		nativeObjectAttributes.NtObjAttrPtr = nullptr;
		return xbox::status_success;
	}

	// Pick up the ObjectName, and let's see what to make of it :
	std::string ObjectName = "";
	if (ObjectAttributes->ObjectName != NULL) {
		ObjectName = PSTRING_to_string(ObjectAttributes->ObjectName);
	}
	std::wstring RelativeHostPath;
	NtDll::HANDLE RootDirectory = ObjectAttributes->RootDirectory;

	// Is there a filename API given?
	if (aFileAPIName.size() > 0) {
		// Then interpret the ObjectName as a filename, and update it to host relative :
		NTSTATUS result = CxbxConvertFilePath(ObjectName, /*OUT*/RelativeHostPath, /*IN OUT*/&RootDirectory, aFileAPIName, partitionHeader);
		if (FAILED(result)) {
			return result;
		}
	}
	else {
		// When not called from a file-handling API, just convert the ObjectName to a wide string :
		RelativeHostPath = string_to_wstring(ObjectName);
	}

	// Copy the wide string to the unicode string
	wcscpy_s(nativeObjectAttributes.wszObjectName, RelativeHostPath.c_str());
	NtDll::RtlInitUnicodeString(&nativeObjectAttributes.NtUnicodeString, nativeObjectAttributes.wszObjectName);
	// And initialize the NT ObjectAttributes with that :
	InitializeObjectAttributes(&nativeObjectAttributes.NtObjAttr, &nativeObjectAttributes.NtUnicodeString, ObjectAttributes->Attributes, RootDirectory, NULL);
	// ObjectAttributes are given, so make sure the pointer we're going to pass to Windows is assigned :
	nativeObjectAttributes.NtObjAttrPtr = &nativeObjectAttributes.NtObjAttr;

	return xbox::status_success;
}

int CxbxDeviceIndexByDevicePath(const char *XboxDevicePath)
{
	for (size_t i = 0; i < Devices.size(); i++)
		if (_strnicmp(XboxDevicePath, Devices[i].XboxDevicePath.c_str(), Devices[i].XboxDevicePath.length()) == 0)
			return(i);

	return -1;
}

int CxbxDeviceIndexByHostPath(const char * HostDevicePath)
{
	for (size_t i = 0; i < Devices.size(); i++)
		if (_strnicmp(HostDevicePath, Devices[i].HostDevicePath.c_str(), Devices[i].HostDevicePath.length()) == 0)
			return(i);

	return -1;
}

XboxDevice *CxbxDeviceByDevicePath(const std::string_view XboxDevicePath)
{
	int DeviceIndex = CxbxDeviceIndexByDevicePath(XboxDevicePath.data());
	if (DeviceIndex >= 0)
		return &Devices[DeviceIndex];

	return nullptr;
}

XboxDevice *CxbxDeviceByHostPath(const std::string_view HostDevicePath)
{
	int DeviceIndex = CxbxDeviceIndexByHostPath(HostDevicePath.data());
	if (DeviceIndex >= 0)
		return &Devices[DeviceIndex];

	return nullptr;
}

// Convert Xbox XBE Path to Host Path
std::string CxbxConvertXboxToHostPath(const std::string_view XboxDevicePath)
{
	HANDLE rootDirectoryHandle = nullptr;
	std::wstring wXbePath;
	// We pretend to come from NtCreateFile to force symbolic link resolution
	CxbxConvertFilePath(XboxDevicePath.data(), wXbePath, &rootDirectoryHandle, "NtCreateFile");

	// Convert Wide String as returned by above to a string, for XbePath
	std::string XbePath = utf16_to_ascii(wXbePath.c_str());

	// If the rootDirectoryHandle is not null, we have a relative path
	// We need to prepend the path of the root directory to get a full DOS path
	if (rootDirectoryHandle != nullptr) {
		char directoryPathBuffer[MAX_PATH];
		GetFinalPathNameByHandle(rootDirectoryHandle, directoryPathBuffer, MAX_PATH, VOLUME_NAME_DOS);
		XbePath = directoryPathBuffer + std::string("\\") + XbePath;

		// Trim \\?\ from the output string, as we want the raw DOS path, not NT path
		// We can do this always because GetFinalPathNameByHandle ALWAYS returns this format
		// Without exception
		XbePath.erase(0, 4);
	}

	return XbePath;
}

int CxbxRegisterDeviceHostPath(const std::string_view XboxDevicePath, std::string HostDevicePath, bool IsFile, std::size_t size)
{
	XboxDevice newDevice;
	newDevice.XboxDevicePath = XboxDevicePath;
	newDevice.HostDevicePath = HostDevicePath;

	bool succeeded{ false };

	// All HDD and MU partitions have a .bin file to allow direct file io on the partition info
	if (_strnicmp(XboxDevicePath.data(), DeviceHarddisk0PartitionPrefix.c_str(), DeviceHarddisk0PartitionPrefix.length()) == 0 ||
		_strnicmp(XboxDevicePath.data(), DeviceMU.c_str(), DeviceMU.length()) == 0) {
		std::string partitionHeaderPath = HostDevicePath + ".bin";
		if (!std::filesystem::exists(partitionHeaderPath)) {
			CxbxCreatePartitionHeaderFile(partitionHeaderPath, XboxDevicePath == DeviceHarddisk0Partition0, size);
		}

		succeeded = true;
	}

	// If this path is not a raw file partition, create the directory for it
	if (!IsFile) {
		std::error_code error; // We do not want filesystem to throw an exception on directory creation. Instead, listen for return value to fail.
		succeeded = std::filesystem::exists(HostDevicePath) || std::filesystem::create_directory(HostDevicePath, error);
	}

	if (succeeded) {
		newDevice.HostRootHandle = CreateFile(newDevice.HostDevicePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		Devices.push_back(newDevice);
		return static_cast<int>(Devices.size()) - 1;
	}

	EmuLog(LOG_LEVEL::FATAL, "Failed to register host device (%s)\n", HostDevicePath.c_str());

	return -1;
}


xbox::ntstatus_xt CxbxCreateSymbolicLink(std::string SymbolicLinkName, std::string FullPath)
{
	xbox::ntstatus_xt result = 0;
	EmuNtSymbolicLinkObject* SymbolicLinkObject = FindNtSymbolicLinkObjectByName(SymbolicLinkName);
	
	// If symbolic link exist, return object name collsion. Do NOT delete existing symlink object!
	if (SymbolicLinkObject != NULL) {
		return xbox::status_object_name_collision;
	}

	// Now (re)create a symbolic link object, and initialize it with the new definition :
	SymbolicLinkObject = new EmuNtSymbolicLinkObject();
	result = SymbolicLinkObject->Init(SymbolicLinkName, FullPath);

	if (result != xbox::status_success) {
		SymbolicLinkObject->NtClose();
	}
	// TODO: Remove whole else if statement below, see g_hCurDir_hack's comment for remark.
	else if (SymbolicLinkObject->DriveLetter == CxbxAutoMountDriveLetter) {
		g_hCurDir_hack = SymbolicLinkObject->RootDirectoryHandle;
	}

	return result;
}


NTSTATUS EmuNtSymbolicLinkObject::Init(std::string aSymbolicLinkName, std::string aFullPath)
{
	NTSTATUS result = STATUS_OBJECT_NAME_INVALID;
	int i = 0;
	int DeviceIndex = -1;

	// If aFullPath is an empty string, set it to the CD-ROM drive
	// This should work for all titles, as CD-ROM is mapped to the current working directory
	// This fixes the issue where titles crash after being launched from the update.xbe
	if (aFullPath.length() == 0) {
		aFullPath = DeviceCdrom0;
	}

	DriveLetter = SymbolicLinkToDriveLetter(aSymbolicLinkName);
	if (DriveLetter >= 'A' && DriveLetter <= 'Z')
	{
		result = xbox::status_object_name_collision;
		if (FindNtSymbolicLinkObjectByDriveLetter(DriveLetter) == NULL)
		{
			// Look up the partition in the list of pre-registered devices :
			result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?

			// If aFullPath starts with a Drive letter, find the originating path and substitute that
			if (aFullPath[1] == ':' && aFullPath[0] >= 'A' && aFullPath[0] <= 'Z') {
				EmuNtSymbolicLinkObject* DriveLetterLink = FindNtSymbolicLinkObjectByDriveLetter(aFullPath[0]);
				if (DriveLetterLink != NULL) {
					aFullPath = DriveLetterLink->XboxSymbolicLinkPath;
				}
			}

 		    // Make a distinction between Xbox paths (starting with '\Device'...) and host paths :
			IsHostBasedPath = _strnicmp(aFullPath.c_str(), DevicePrefix.c_str(), DevicePrefix.length()) != 0;
			if (IsHostBasedPath) {
				DeviceIndex = CxbxDeviceIndexByHostPath(aFullPath.c_str());
			}
			else {
				DeviceIndex = CxbxDeviceIndexByDevicePath(aFullPath.c_str());
			}

			if (DeviceIndex >= 0)
			{
				result = xbox::status_success;
				SymbolicLinkName = aSymbolicLinkName;
				if (IsHostBasedPath) {
					// Handle the case where a sub folder of the partition is mounted (instead of it's root) :
					std::string ExtraPath = aFullPath.substr(Devices[DeviceIndex].HostDevicePath.length(), std::string::npos);

					XboxSymbolicLinkPath = Devices[DeviceIndex].XboxDevicePath + ExtraPath;
					HostSymbolicLinkPath = aFullPath;

				}
				else {
					XboxSymbolicLinkPath = aFullPath;
					HostSymbolicLinkPath = Devices[DeviceIndex].HostDevicePath;
					// Handle the case where a sub folder of the partition is mounted (instead of it's root) :
					std::string ExtraPath = aFullPath.substr(Devices[DeviceIndex].XboxDevicePath.length(), std::string::npos);

					if (!ExtraPath.empty()) {
						HostSymbolicLinkPath = HostSymbolicLinkPath + ExtraPath;
					}
				}

				RootDirectoryHandle = CreateFile(HostSymbolicLinkPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				if (RootDirectoryHandle == INVALID_HANDLE_VALUE)
				{
					result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?
					CxbxKrnlCleanup((std::string("Could not map ") + HostSymbolicLinkPath).c_str());
				}
				else
				{
					NtSymbolicLinkObjects[DriveLetter - 'A'] = this;
					EmuLog(LOG_LEVEL::DEBUG, "Linked \"%s\" to \"%s\" (residing at \"%s\")", aSymbolicLinkName.c_str(), aFullPath.c_str(), HostSymbolicLinkPath.c_str());
				}
			}
		}
	}

	return result;
}

 EmuNtSymbolicLinkObject::~EmuNtSymbolicLinkObject()
{
	if (DriveLetter >= 'A' && DriveLetter <= 'Z') {
		NtSymbolicLinkObjects[DriveLetter - 'A'] = NULL;
		NtDll::NtClose(RootDirectoryHandle);
		// TODO: Remove whole if statement below, see g_hCurDir_hack's comment for remark.
		if (DriveLetter == CxbxAutoMountDriveLetter) {
			g_hCurDir_hack = NULL;
		}
	}
}

char SymbolicLinkToDriveLetter(std::string SymbolicLinkName)
{
	char result = '\0';
	// SymbolicLinkName must look like this : "\??\D:"
	if ((SymbolicLinkName.size() == 6) && (SymbolicLinkName[0] == '\\') && (SymbolicLinkName[1] == '?') && (SymbolicLinkName[2] == '?') && (SymbolicLinkName[3] == '\\') && (SymbolicLinkName[5] == ':'))
	{
		result = SymbolicLinkName[4];
		if (result >= 'A' && result <= 'Z')
			return result;

		if (result >= 'a' && result <= 'z') {
			return result + 'A' - 'a';
		}
	}
	
	return NULL;
}

EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDriveLetter(const char DriveLetter)
{
	if (DriveLetter >= 'A' && DriveLetter <= 'Z')
		return NtSymbolicLinkObjects[DriveLetter - 'A'];
		
	if (DriveLetter >= 'a' && DriveLetter <= 'z')
		return NtSymbolicLinkObjects[DriveLetter - 'a'];

	return NULL;
}

EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByName(std::string SymbolicLinkName)
{
	return FindNtSymbolicLinkObjectByDriveLetter(SymbolicLinkToDriveLetter(SymbolicLinkName));
}


void FindEmuDirPathByDevice(std::string DeviceName, EmuDirPath& hybrid_path)
{
	for (auto device = Devices.begin(); device != Devices.end(); device++) {
		if (_strnicmp(DeviceName.c_str(), device->XboxDevicePath.c_str(), device->XboxDevicePath.length()) == 0) {
			hybrid_path.XboxDirPath = device->XboxDevicePath;
			hybrid_path.HostDirPath = device->HostDevicePath;
			hybrid_path.HostDirHandle = device->HostRootHandle;
			return;
		}
	}

	hybrid_path.HostDirPath = "";
}


EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByRootHandle(const HANDLE Handle)
{
	for (char DriveLetter = 'A'; DriveLetter <= 'Z'; DriveLetter++)
	{
		EmuNtSymbolicLinkObject* result = NtSymbolicLinkObjects[DriveLetter - 'A'];
		if ((result != NULL) && (Handle == result->RootDirectoryHandle))
			return result;
	}
	
	return NULL;
}


void _CxbxPVOIDDeleter(PVOID *ptr)
{
	if (*ptr) {
		xbox::ExFreePool(*ptr);
	}
}

// ----------------------------------------------------------------------------
// Xbox to NT converters
// ----------------------------------------------------------------------------

NtDll::FILE_LINK_INFORMATION * _XboxToNTLinkInfo(xbox::FILE_LINK_INFORMATION *xboxLinkInfo, ULONG *Length)
{
	// Convert the path from Xbox to native
	std::string originalFileName(xboxLinkInfo->FileName, xboxLinkInfo->FileNameLength);
	std::wstring convertedFileName;
	NtDll::HANDLE RootDirectory = nullptr;
	NTSTATUS res = CxbxConvertFilePath(originalFileName, /*OUT*/convertedFileName, /*IN OUT*/&RootDirectory, "NtSetInformationFile");
	// TODO : handle if(FAILED(res))

	// Build the native FILE_LINK_INFORMATION struct
	*Length = sizeof(NtDll::FILE_LINK_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
	NtDll::FILE_LINK_INFORMATION *ntLinkInfo = (NtDll::FILE_LINK_INFORMATION *)xbox::ExAllocatePool(*Length);
	ntLinkInfo->ReplaceIfExists = xboxLinkInfo->ReplaceIfExists;
	ntLinkInfo->RootDirectory = RootDirectory;
	ntLinkInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
	wmemcpy_s(ntLinkInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());
	
	return ntLinkInfo;
}

NtDll::FILE_RENAME_INFORMATION * _XboxToNTRenameInfo(xbox::FILE_RENAME_INFORMATION *xboxRenameInfo, ULONG *Length)
{
	// Convert the path from Xbox to native
	std::string originalFileName(xboxRenameInfo->FileName.Buffer, xboxRenameInfo->FileName.Length);
	std::wstring convertedFileName;
	NtDll::HANDLE RootDirectory = nullptr;
	NTSTATUS res = CxbxConvertFilePath(originalFileName, /*OUT*/convertedFileName, /*IN OUT*/&RootDirectory, "NtSetInformationFile");
	// TODO : handle if(FAILED(res))

	// Build the native FILE_RENAME_INFORMATION struct
	*Length = sizeof(NtDll::FILE_RENAME_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
	NtDll::FILE_RENAME_INFORMATION *ntRenameInfo = (NtDll::FILE_RENAME_INFORMATION *)xbox::ExAllocatePool(*Length);
	ntRenameInfo->ReplaceIfExists = xboxRenameInfo->ReplaceIfExists;
	ntRenameInfo->RootDirectory = RootDirectory;
	ntRenameInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
	wmemcpy_s(ntRenameInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());

	return ntRenameInfo;
}

// ----------------------------------------------------------------------------
// NT to Xbox converters - common functions
// ----------------------------------------------------------------------------

void _ConvertXboxBasicInfo(xbox::FILE_BASIC_INFORMATION *xboxBasicInfo)
{
	// Fix up attributes
	xboxBasicInfo->FileAttributes &= FILE_ATTRIBUTE_VALID_FLAGS;
}

NTSTATUS _ConvertXboxNameInfo(NtDll::FILE_NAME_INFORMATION *ntNameInfo, xbox::FILE_NAME_INFORMATION *xboxNameInfo, int convertedFileNameLength, ULONG Length)
{
	// Convert the file name to ANSI in-place
	xboxNameInfo->FileNameLength = convertedFileNameLength;

	// Check if the new file name fits within the given struct size and copy as many chars as possible if not
	int maxFileNameLength = Length - sizeof(xbox::FILE_NAME_INFORMATION);
	size_t convertedChars;
	wcstombs_s(&convertedChars, xboxNameInfo->FileName, maxFileNameLength, ntNameInfo->FileName, ntNameInfo->FileNameLength);

	// Return the appropriate result depending on whether the string was fully copied
	return convertedChars == ntNameInfo->FileNameLength / sizeof(NtDll::WCHAR)
		? xbox::status_success
		: xbox::status_buffer_overflow;
}

// ----------------------------------------------------------------------------
// NT to Xbox converters
// ----------------------------------------------------------------------------

NTSTATUS _NTToXboxNetOpenInfo(NtDll::FILE_NETWORK_OPEN_INFORMATION *ntNetOpenInfo, xbox::FILE_NETWORK_OPEN_INFORMATION *xboxNetOpenInfo, ULONG Length)
{
	// Copy everything from the NT struct
	memcpy_s(xboxNetOpenInfo, Length, ntNetOpenInfo, sizeof(NtDll::FILE_NETWORK_OPEN_INFORMATION));

	// Fix up attributes
	xboxNetOpenInfo->FileAttributes &= FILE_ATTRIBUTE_VALID_FLAGS;

	return xbox::status_success;
}

NTSTATUS _NTToXboxBasicInfo(NtDll::FILE_BASIC_INFORMATION *ntBasicInfo, xbox::FILE_BASIC_INFORMATION *xboxBasicInfo, ULONG Length)
{
	// Copy everything from the NT struct
	memcpy_s(xboxBasicInfo, sizeof(xbox::FILE_BASIC_INFORMATION), ntBasicInfo, sizeof(NtDll::FILE_BASIC_INFORMATION));

	_ConvertXboxBasicInfo(xboxBasicInfo);

	return xbox::status_success;
}

NTSTATUS _NTToXboxNameInfo(NtDll::FILE_NAME_INFORMATION *ntNameInfo, xbox::FILE_NAME_INFORMATION *xboxNameInfo, ULONG Length)
{
	// TODO: the FileName probably needs to be converted back to an Xbox path in some cases
	// Determine new file name length
	size_t convertedFileNameLength = ntNameInfo->FileNameLength / sizeof(NtDll::WCHAR);

	// Clean up the Xbox FILE_NAME_INFORMATION struct
	ZeroMemory(xboxNameInfo, Length);

	// Convert file name and return the result
	return _ConvertXboxNameInfo(ntNameInfo, xboxNameInfo, convertedFileNameLength, Length);
}

NTSTATUS _NTToXboxAllInfo(NtDll::FILE_ALL_INFORMATION *ntAllInfo, xbox::FILE_ALL_INFORMATION *xboxAllInfo, ULONG Length)
{
	// TODO: the FileName probably needs to be converted back to an Xbox path in some cases
	// Determine new file name length
	size_t convertedFileNameLength = ntAllInfo->NameInformation.FileNameLength / sizeof(NtDll::WCHAR);

	// Copy everything from the NT struct
	memcpy_s(xboxAllInfo, Length, ntAllInfo, sizeof(NtDll::FILE_ALL_INFORMATION));

	// Convert NT structs to Xbox where needed and return the result
	_ConvertXboxBasicInfo(&xboxAllInfo->BasicInformation);
	return _ConvertXboxNameInfo(&ntAllInfo->NameInformation, &xboxAllInfo->NameInformation, convertedFileNameLength, Length);
}

// ----------------------------------------------------------------------------
// File information struct converters
// ----------------------------------------------------------------------------

PVOID _XboxToNTFileInformation
(
	IN  PVOID xboxFileInformation,
	IN  ULONG FileInformationClass,
	OUT ULONG *Length
)
{
	// The following classes of file information structs are identical between platforms:
	//   FileBasicInformation
	//   FileDispositionInformation
	//   FileEndOfFileInformation
	//   FileLinkInformation
	//   FilePositionInformation

	PVOID result = NULL;

	switch (FileInformationClass)
	{
		case xbox::FileLinkInformation:
		{
			xbox::FILE_LINK_INFORMATION *xboxLinkInfo = reinterpret_cast<xbox::FILE_LINK_INFORMATION *>(xboxFileInformation);
			result = _XboxToNTLinkInfo(xboxLinkInfo, Length);
			break;
		}
		case xbox::FileRenameInformation:
		{
			xbox::FILE_RENAME_INFORMATION *xboxRenameInfo = reinterpret_cast<xbox::FILE_RENAME_INFORMATION *>(xboxFileInformation);
			result = _XboxToNTRenameInfo(xboxRenameInfo, Length);
			break;
		}
		default:
		{
			result = NULL;
			break;
		}
	}

	return result;
}

NTSTATUS NTToXboxFileInformation
(
	IN  PVOID nativeFileInformation,
	OUT PVOID xboxFileInformation,
	IN  ULONG FileInformationClass,
	IN  ULONG Length
)
{
	// The following classes of file information structs are identical between platforms:
	//   FileAccessInformation
	//   FileAlignmentInformation
	//   FileEaInformation
	//   FileInternalInformation
	//   FileModeInformation
	//   FilePositionInformation
	//   FileStandardInformation
	//   FileReparsePointInformation

	NTSTATUS result = xbox::status_success;

	switch (FileInformationClass)
	{
		case NtDll::FileAllInformation:
		{
			NtDll::FILE_ALL_INFORMATION *ntAllInfo = reinterpret_cast<NtDll::FILE_ALL_INFORMATION *>(nativeFileInformation);
			xbox::FILE_ALL_INFORMATION *xboxAllInfo = reinterpret_cast<xbox::FILE_ALL_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxAllInfo(ntAllInfo, xboxAllInfo, Length);
			break;
		}
		case NtDll::FileBasicInformation:
		{
			NtDll::FILE_BASIC_INFORMATION *ntBasicInfo = reinterpret_cast<NtDll::FILE_BASIC_INFORMATION *>(nativeFileInformation);
			xbox::FILE_BASIC_INFORMATION *xboxBasicInfo = reinterpret_cast<xbox::FILE_BASIC_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxBasicInfo(ntBasicInfo, xboxBasicInfo, Length);
			break;
		}
		case NtDll::FileNameInformation:
		{
			NtDll::FILE_NAME_INFORMATION *ntNameInfo = reinterpret_cast<NtDll::FILE_NAME_INFORMATION *>(nativeFileInformation);
			xbox::FILE_NAME_INFORMATION *xboxNameInfo = reinterpret_cast<xbox::FILE_NAME_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxNameInfo(ntNameInfo, xboxNameInfo, Length);
			break;
		}
		case NtDll::FileNetworkOpenInformation:
		{
			NtDll::FILE_NETWORK_OPEN_INFORMATION *ntNetOpenInfo = reinterpret_cast<NtDll::FILE_NETWORK_OPEN_INFORMATION *>(nativeFileInformation);
			xbox::FILE_NETWORK_OPEN_INFORMATION *xboxNetOpenInfo = reinterpret_cast<xbox::FILE_NETWORK_OPEN_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxNetOpenInfo(ntNetOpenInfo, xboxNetOpenInfo, Length);
			break;
		}
		case NtDll::FileBothDirectoryInformation:
		{
			// TODO: handle differences
			// - Xbox reuses the FILE_DIRECTORY_INFORMATION struct, which is mostly identical to FILE_BOTH_DIR_INFORMATION
			// - NextEntryOffset might be a problem
			// - NT has extra fields before FileName:
			//   - ULONG EaSize
			//   - CCHAR ShortNameLength
			//   - WCHAR ShortName[12]
			// - FileName on Xbox uses single-byte chars, NT uses wide chars

			//break;
		}
		case NtDll::FileDirectoryInformation:
		{
			// TODO: handle differences
			// - NextEntryOffset might be a problem
			// - FileName on Xbox uses single-byte chars, NT uses wide chars

			//break;
		}
		case NtDll::FileFullDirectoryInformation:
		{
			// TODO: handle differences
			// - Xbox reuses the FILE_DIRECTORY_INFORMATION struct, which is mostly identical to FILE_FULL_DIR_INFORMATION
			// - NextEntryOffset might be a problem
			// - NT has one extra field before FileName:
			//   - ULONG EaSize
			// - FileName on Xbox uses single-byte chars, NT uses wide chars

			//break;
		}
		case NtDll::FileNamesInformation:
		{
			// TODO: handle differences
			// - NextEntryOffset might be a problem
			// - FileName on Xbox uses single-byte chars, NT uses wide chars

			//break;
		}
		case NtDll::FileObjectIdInformation:
		{
			// TODO: handle differences
			// - The LONGLONG FileReference field from NT can be ignored
			// - ExtendedInfo is an union on NT, but is otherwise identical

			//break;
		}
		default:
		{
			// No differences between structs; a simple copy should suffice
			memcpy_s(xboxFileInformation, Length, nativeFileInformation, Length);
			result = xbox::status_success;
			break;
		}
	}

	return result;
}

void CxbxLaunchNewXbe(const std::string& XbePath) {
	cli_config::SetLoad(XbePath);

	bool Debugging{ false };
	g_EmuShared->GetDebuggingFlag(&Debugging);

	if (Debugging)
	{
		std::string cliCommands;
		if (!cli_config::GenCMD(cliCommands))
		{
			CxbxKrnlCleanup("Could not launch %s", XbePath.c_str());
		}

		CxbxDebugger::ReportNewTarget(cliCommands.c_str());

		// The debugger will execute this process
	}
	else
	{
		if (const auto &err = CxbxExec(false, nullptr, false))
		{
			CxbxKrnlCleanup("Could not launch %s\n\nThe reason was: %s", XbePath.c_str(), err->c_str());
		}
	}

	// This is a requirement to have shared memory buffers remain alive and transfer to new emulation process.
	unsigned int retryAttempt = 0;
	unsigned int curProcID = 0;
	unsigned int oldProcID = GetCurrentProcessId();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		g_EmuShared->GetKrnlProcID(&curProcID);
		// Break when new emulation process has take over.
		if (curProcID != oldProcID) {
			break;
		}
		retryAttempt++;
		// Terminate after 5 seconds of failure.
		if (retryAttempt >= (5 * (1000 / 100))) {
			PopupError(nullptr, "Could not reboot; New emulation process did not take over.");
			break;
		}
	}
}

// TODO: FS_INFORMATION_CLASS and its related structs most likely need to be converted too

// TODO : Move to a better suited file
/* TODO : Also, fix C2593: "'operator <<' is ambiguous" for this
std::ostream& operator<<(std::ostream& os, const NtDll::NTSTATUS& value)
{
	os << hex4((uint32_t)value) << " = " << NtStatusToString(value);

	return os;
}
*/

// TODO : Create (and use) an Xbox version of this too
CHAR* NtStatusToString(IN NTSTATUS Status)
{
#define _CASE(s) case s: return #s;

	switch (Status)
	{
		// Note : Keep all cases sorted, for easier maintenance
		_CASE(DBG_APP_NOT_IDLE);
		_CASE(DBG_CONTINUE);
		_CASE(DBG_CONTROL_BREAK);
		_CASE(DBG_CONTROL_C);
		_CASE(DBG_EXCEPTION_HANDLED);
		_CASE(DBG_EXCEPTION_NOT_HANDLED);
		_CASE(DBG_NO_STATE_CHANGE);
		_CASE(DBG_PRINTEXCEPTION_C);
		_CASE(DBG_REPLY_LATER);
		_CASE(DBG_RIPEXCEPTION);
		_CASE(DBG_TERMINATE_PROCESS);
		_CASE(DBG_TERMINATE_THREAD);
		_CASE(DBG_UNABLE_TO_PROVIDE_HANDLE);
		_CASE(EPT_NT_CANT_CREATE);
		_CASE(EPT_NT_CANT_PERFORM_OP);
		_CASE(EPT_NT_INVALID_ENTRY);
		_CASE(EPT_NT_NOT_REGISTERED);
		_CASE(RPC_NT_ADDRESS_ERROR);
		_CASE(RPC_NT_ALREADY_LISTENING);
		_CASE(RPC_NT_ALREADY_REGISTERED);
		_CASE(RPC_NT_BAD_STUB_DATA);
		_CASE(RPC_NT_BINDING_HAS_NO_AUTH);
		_CASE(RPC_NT_BINDING_INCOMPLETE);
		_CASE(RPC_NT_BYTE_COUNT_TOO_SMALL);
		_CASE(RPC_NT_CALL_CANCELLED);
		_CASE(RPC_NT_CALL_FAILED);
		_CASE(RPC_NT_CALL_FAILED_DNE);
		_CASE(RPC_NT_CALL_IN_PROGRESS);
		_CASE(RPC_NT_CANNOT_SUPPORT);
		_CASE(RPC_NT_CANT_CREATE_ENDPOINT);
		_CASE(RPC_NT_COMM_FAILURE);
		_CASE(RPC_NT_DUPLICATE_ENDPOINT);
		_CASE(RPC_NT_ENTRY_ALREADY_EXISTS);
		_CASE(RPC_NT_ENTRY_NOT_FOUND);
		_CASE(RPC_NT_ENUM_VALUE_OUT_OF_RANGE);
		_CASE(RPC_NT_FP_DIV_ZERO);
		_CASE(RPC_NT_FP_OVERFLOW);
		_CASE(RPC_NT_FP_UNDERFLOW);
		_CASE(RPC_NT_GROUP_MEMBER_NOT_FOUND);
		_CASE(RPC_NT_INCOMPLETE_NAME);
		_CASE(RPC_NT_INTERFACE_NOT_FOUND);
		_CASE(RPC_NT_INTERNAL_ERROR);
		_CASE(RPC_NT_INVALID_ASYNC_CALL);
		_CASE(RPC_NT_INVALID_ASYNC_HANDLE);
		_CASE(RPC_NT_INVALID_AUTH_IDENTITY);
		_CASE(RPC_NT_INVALID_BINDING);
		_CASE(RPC_NT_INVALID_BOUND);
		_CASE(RPC_NT_INVALID_ENDPOINT_FORMAT);
		_CASE(RPC_NT_INVALID_ES_ACTION);
		_CASE(RPC_NT_INVALID_NAF_ID);
		_CASE(RPC_NT_INVALID_NAME_SYNTAX);
		_CASE(RPC_NT_INVALID_NETWORK_OPTIONS);
		_CASE(RPC_NT_INVALID_NET_ADDR);
		_CASE(RPC_NT_INVALID_OBJECT);
		_CASE(RPC_NT_INVALID_PIPE_OBJECT);
		_CASE(RPC_NT_INVALID_PIPE_OPERATION);
		_CASE(RPC_NT_INVALID_RPC_PROTSEQ);
		_CASE(RPC_NT_INVALID_STRING_BINDING);
		_CASE(RPC_NT_INVALID_STRING_UUID);
		_CASE(RPC_NT_INVALID_TAG);
		_CASE(RPC_NT_INVALID_TIMEOUT);
		_CASE(RPC_NT_INVALID_VERS_OPTION);
		_CASE(RPC_NT_MAX_CALLS_TOO_SMALL);
		_CASE(RPC_NT_NAME_SERVICE_UNAVAILABLE);
		_CASE(RPC_NT_NOTHING_TO_EXPORT);
		_CASE(RPC_NT_NOT_ALL_OBJS_UNEXPORTED);
		_CASE(RPC_NT_NOT_CANCELLED);
		_CASE(RPC_NT_NOT_LISTENING);
		_CASE(RPC_NT_NOT_RPC_ERROR);
		_CASE(RPC_NT_NO_BINDINGS);
		_CASE(RPC_NT_NO_CALL_ACTIVE);
		_CASE(RPC_NT_NO_CONTEXT_AVAILABLE);
		_CASE(RPC_NT_NO_ENDPOINT_FOUND);
		_CASE(RPC_NT_NO_ENTRY_NAME);
		_CASE(RPC_NT_NO_INTERFACES);
		_CASE(RPC_NT_NO_MORE_BINDINGS);
		_CASE(RPC_NT_NO_MORE_ENTRIES);
		_CASE(RPC_NT_NO_MORE_MEMBERS);
		_CASE(RPC_NT_NO_PRINC_NAME);
		_CASE(RPC_NT_NO_PROTSEQS);
		_CASE(RPC_NT_NO_PROTSEQS_REGISTERED);
		_CASE(RPC_NT_NULL_REF_POINTER);
		_CASE(RPC_NT_OBJECT_NOT_FOUND);
		_CASE(RPC_NT_OUT_OF_RESOURCES);
		_CASE(RPC_NT_PIPE_CLOSED);
		_CASE(RPC_NT_PIPE_DISCIPLINE_ERROR);
		_CASE(RPC_NT_PIPE_EMPTY);
		_CASE(RPC_NT_PROCNUM_OUT_OF_RANGE);
		_CASE(RPC_NT_PROTOCOL_ERROR);
		_CASE(RPC_NT_PROTSEQ_NOT_FOUND);
		_CASE(RPC_NT_PROTSEQ_NOT_SUPPORTED);
		_CASE(RPC_NT_SEC_PKG_ERROR);
		_CASE(RPC_NT_SEND_INCOMPLETE);
		_CASE(RPC_NT_SERVER_TOO_BUSY);
		_CASE(RPC_NT_SERVER_UNAVAILABLE);
		_CASE(RPC_NT_SS_CANNOT_GET_CALL_HANDLE);
		_CASE(RPC_NT_SS_CHAR_TRANS_OPEN_FAIL);
		_CASE(RPC_NT_SS_CHAR_TRANS_SHORT_FILE);
		_CASE(RPC_NT_SS_CONTEXT_DAMAGED);
		_CASE(RPC_NT_SS_CONTEXT_MISMATCH);
		_CASE(RPC_NT_SS_HANDLES_MISMATCH);
		_CASE(RPC_NT_SS_IN_NULL_CONTEXT);
		_CASE(RPC_NT_STRING_TOO_LONG);
		_CASE(RPC_NT_TYPE_ALREADY_REGISTERED);
		_CASE(RPC_NT_UNKNOWN_AUTHN_LEVEL);
		_CASE(RPC_NT_UNKNOWN_AUTHN_SERVICE);
		_CASE(RPC_NT_UNKNOWN_AUTHN_TYPE);
		_CASE(RPC_NT_UNKNOWN_AUTHZ_SERVICE);
		_CASE(RPC_NT_UNKNOWN_IF);
		_CASE(RPC_NT_UNKNOWN_MGR_TYPE);
		_CASE(RPC_NT_UNSUPPORTED_AUTHN_LEVEL);
		_CASE(RPC_NT_UNSUPPORTED_NAME_SYNTAX);
		_CASE(RPC_NT_UNSUPPORTED_TRANS_SYN);
		_CASE(RPC_NT_UNSUPPORTED_TYPE);
		_CASE(RPC_NT_UUID_LOCAL_ONLY);
		_CASE(RPC_NT_UUID_NO_ADDRESS);
		_CASE(RPC_NT_WRONG_ES_VERSION);
		_CASE(RPC_NT_WRONG_KIND_OF_BINDING);
		_CASE(RPC_NT_WRONG_PIPE_VERSION);
		_CASE(RPC_NT_WRONG_STUB_VERSION);
		_CASE(RPC_NT_ZERO_DIVIDE);
		_CASE(STATUS_ABANDONED_WAIT_0);
		_CASE(STATUS_ABANDONED_WAIT_63);
		_CASE(STATUS_ABIOS_INVALID_COMMAND);
		_CASE(STATUS_ABIOS_INVALID_LID);
		_CASE(STATUS_ABIOS_INVALID_SELECTOR);
		_CASE(STATUS_ABIOS_LID_ALREADY_OWNED);
		_CASE(STATUS_ABIOS_LID_NOT_EXIST);
		_CASE(STATUS_ABIOS_NOT_LID_OWNER);
		_CASE(STATUS_ABIOS_NOT_PRESENT);
		_CASE(STATUS_ABIOS_SELECTOR_NOT_AVAILABLE);
		_CASE(STATUS_ACCESS_DENIED);
		_CASE(STATUS_ACCESS_VIOLATION);
		_CASE(STATUS_ACCOUNT_DISABLED);
		_CASE(STATUS_ACCOUNT_EXPIRED);
		_CASE(STATUS_ACCOUNT_LOCKED_OUT);
		_CASE(STATUS_ACCOUNT_RESTRICTION);
		_CASE(STATUS_ACPI_ACQUIRE_GLOBAL_LOCK);
		_CASE(STATUS_ACPI_ADDRESS_NOT_MAPPED);
		_CASE(STATUS_ACPI_ALREADY_INITIALIZED);
		_CASE(STATUS_ACPI_ASSERT_FAILED);
		_CASE(STATUS_ACPI_FATAL);
		_CASE(STATUS_ACPI_HANDLER_COLLISION);
		_CASE(STATUS_ACPI_INCORRECT_ARGUMENT_COUNT);
		_CASE(STATUS_ACPI_INVALID_ACCESS_SIZE);
		_CASE(STATUS_ACPI_INVALID_ARGTYPE);
		_CASE(STATUS_ACPI_INVALID_ARGUMENT);
		_CASE(STATUS_ACPI_INVALID_DATA);
		_CASE(STATUS_ACPI_INVALID_EVENTTYPE);
		_CASE(STATUS_ACPI_INVALID_INDEX);
		_CASE(STATUS_ACPI_INVALID_MUTEX_LEVEL);
		_CASE(STATUS_ACPI_INVALID_OBJTYPE);
		_CASE(STATUS_ACPI_INVALID_OPCODE);
		_CASE(STATUS_ACPI_INVALID_REGION);
		_CASE(STATUS_ACPI_INVALID_SUPERNAME);
		_CASE(STATUS_ACPI_INVALID_TABLE);
		_CASE(STATUS_ACPI_INVALID_TARGETTYPE);
		_CASE(STATUS_ACPI_MUTEX_NOT_OWNED);
		_CASE(STATUS_ACPI_MUTEX_NOT_OWNER);
		_CASE(STATUS_ACPI_NOT_INITIALIZED);
		_CASE(STATUS_ACPI_POWER_REQUEST_FAILED);
		_CASE(STATUS_ACPI_REG_HANDLER_FAILED);
		_CASE(STATUS_ACPI_RS_ACCESS);
		_CASE(STATUS_ACPI_STACK_OVERFLOW);
		_CASE(STATUS_ADAPTER_HARDWARE_ERROR);
		_CASE(STATUS_ADDRESS_ALREADY_ASSOCIATED);
		_CASE(STATUS_ADDRESS_ALREADY_EXISTS);
		_CASE(STATUS_ADDRESS_CLOSED);
		_CASE(STATUS_ADDRESS_NOT_ASSOCIATED);
		_CASE(STATUS_AGENTS_EXHAUSTED);
		_CASE(xbox::status_alerted);
		_CASE(STATUS_ALIAS_EXISTS);
		_CASE(STATUS_ALLOCATE_BUCKET);
		_CASE(STATUS_ALLOTTED_SPACE_EXCEEDED);
		_CASE(STATUS_ALREADY_COMMITTED);
		_CASE(STATUS_ALREADY_DISCONNECTED);
		_CASE(STATUS_ALREADY_WIN32);
		_CASE(STATUS_APP_INIT_FAILURE);
		_CASE(STATUS_ARBITRATION_UNHANDLED);
		_CASE(STATUS_ARRAY_BOUNDS_EXCEEDED);
		_CASE(STATUS_AUDIT_FAILED);
		_CASE(STATUS_BACKUP_CONTROLLER);
		_CASE(STATUS_BAD_COMPRESSION_BUFFER);
		_CASE(STATUS_BAD_CURRENT_DIRECTORY);
		_CASE(STATUS_BAD_DESCRIPTOR_FORMAT);
		_CASE(STATUS_BAD_DEVICE_TYPE);
		_CASE(STATUS_BAD_DLL_ENTRYPOINT);
		_CASE(STATUS_BAD_FUNCTION_TABLE);
		_CASE(STATUS_BAD_IMPERSONATION_LEVEL);
		_CASE(STATUS_BAD_INHERITANCE_ACL);
		_CASE(STATUS_BAD_INITIAL_PC);
		_CASE(STATUS_BAD_INITIAL_STACK);
		_CASE(STATUS_BAD_LOGON_SESSION_STATE);
		_CASE(STATUS_BAD_MASTER_BOOT_RECORD);
		_CASE(STATUS_BAD_NETWORK_NAME);
		_CASE(STATUS_BAD_NETWORK_PATH);
		_CASE(STATUS_BAD_REMOTE_ADAPTER);
		_CASE(STATUS_BAD_SERVICE_ENTRYPOINT);
		_CASE(STATUS_BAD_STACK);
		_CASE(STATUS_BAD_TOKEN_TYPE);
		_CASE(STATUS_BAD_VALIDATION_CLASS);
		_CASE(STATUS_BAD_WORKING_SET_LIMIT);
		_CASE(STATUS_BEGINNING_OF_MEDIA);
		_CASE(STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT);
		_CASE(STATUS_BREAKPOINT);
		_CASE(STATUS_BUFFER_ALL_ZEROS);
		_CASE(xbox::status_buffer_overflow);
		_CASE(xbox::status_buffer_too_small);
		_CASE(STATUS_BUS_RESET);
		_CASE(STATUS_CACHE_PAGE_LOCKED);
		_CASE(STATUS_CANCELLED);
		_CASE(STATUS_CANNOT_DELETE);
		_CASE(STATUS_CANNOT_IMPERSONATE);
		_CASE(STATUS_CANNOT_LOAD_REGISTRY_FILE);
		_CASE(STATUS_CANT_ACCESS_DOMAIN_INFO);
		_CASE(STATUS_CANT_DISABLE_MANDATORY);
		_CASE(STATUS_CANT_ENABLE_DENY_ONLY);
		_CASE(STATUS_CANT_OPEN_ANONYMOUS);
		_CASE(STATUS_CANT_TERMINATE_SELF);
		_CASE(STATUS_CANT_WAIT);
		_CASE(STATUS_CARDBUS_NOT_SUPPORTED);
		_CASE(STATUS_CHECKING_FILE_SYSTEM);
		_CASE(STATUS_CHILD_MUST_BE_VOLATILE);
		_CASE(STATUS_CLIENT_SERVER_PARAMETERS_INVALID);
		_CASE(STATUS_COMMITMENT_LIMIT);
		_CASE(STATUS_COMMITMENT_MINIMUM);
		_CASE(xbox::status_conflicting_addresses);
		_CASE(STATUS_CONNECTION_ABORTED);
		_CASE(STATUS_CONNECTION_ACTIVE);
		_CASE(STATUS_CONNECTION_COUNT_LIMIT);
		_CASE(STATUS_CONNECTION_DISCONNECTED);
		_CASE(STATUS_CONNECTION_INVALID);
		_CASE(STATUS_CONNECTION_IN_USE);
		_CASE(STATUS_CONNECTION_REFUSED);
		_CASE(STATUS_CONNECTION_RESET);
		_CASE(STATUS_CONTROL_C_EXIT);
		_CASE(STATUS_CONVERT_TO_LARGE);
		_CASE(STATUS_CORRUPT_SYSTEM_FILE);
		_CASE(STATUS_COULD_NOT_INTERPRET);
		_CASE(STATUS_CRASH_DUMP);
		_CASE(STATUS_CRC_ERROR);
		_CASE(STATUS_CTL_FILE_NOT_SUPPORTED);
		_CASE(STATUS_CTX_BAD_VIDEO_MODE);
		_CASE(STATUS_CTX_CDM_CONNECT);
		_CASE(STATUS_CTX_CDM_DISCONNECT);
		_CASE(STATUS_CTX_CLIENT_LICENSE_IN_USE);
		_CASE(STATUS_CTX_CLIENT_LICENSE_NOT_SET);
		_CASE(STATUS_CTX_CLIENT_QUERY_TIMEOUT);
		_CASE(STATUS_CTX_CLOSE_PENDING);
		_CASE(STATUS_CTX_CONSOLE_CONNECT);
		_CASE(STATUS_CTX_CONSOLE_DISCONNECT);
		_CASE(STATUS_CTX_GRAPHICS_INVALID);
		_CASE(STATUS_CTX_INVALID_MODEMNAME);
		_CASE(STATUS_CTX_INVALID_PD);
		_CASE(STATUS_CTX_INVALID_WD);
		_CASE(STATUS_CTX_LICENSE_CLIENT_INVALID);
		_CASE(STATUS_CTX_LICENSE_EXPIRED);
		_CASE(STATUS_CTX_LICENSE_NOT_AVAILABLE);
		_CASE(STATUS_CTX_MODEM_INF_NOT_FOUND);
		_CASE(STATUS_CTX_MODEM_RESPONSE_BUSY);
		_CASE(STATUS_CTX_MODEM_RESPONSE_NO_CARRIER);
		_CASE(STATUS_CTX_MODEM_RESPONSE_NO_DIALTONE);
		_CASE(STATUS_CTX_MODEM_RESPONSE_TIMEOUT);
		_CASE(STATUS_CTX_MODEM_RESPONSE_VOICE);
		_CASE(STATUS_CTX_NOT_CONSOLE);
		_CASE(STATUS_CTX_NO_OUTBUF);
		_CASE(STATUS_CTX_PD_NOT_FOUND);
		_CASE(STATUS_CTX_RESPONSE_ERROR);
		_CASE(STATUS_CTX_SHADOW_DENIED);
		_CASE(STATUS_CTX_SHADOW_DISABLED);
		_CASE(STATUS_CTX_SHADOW_INVALID);
		_CASE(STATUS_CTX_TD_ERROR);
		_CASE(STATUS_CTX_WD_NOT_FOUND);
		_CASE(STATUS_CTX_WINSTATION_ACCESS_DENIED);
		_CASE(STATUS_CTX_WINSTATION_BUSY);
		_CASE(STATUS_CTX_WINSTATION_NAME_COLLISION);
		_CASE(STATUS_CTX_WINSTATION_NAME_INVALID);
		_CASE(STATUS_CTX_WINSTATION_NOT_FOUND);
		_CASE(STATUS_DATATYPE_MISALIGNMENT);
		_CASE(STATUS_DATATYPE_MISALIGNMENT_ERROR);
		_CASE(STATUS_DATA_ERROR);
		_CASE(STATUS_DATA_LATE_ERROR);
		_CASE(STATUS_DATA_NOT_ACCEPTED);
		_CASE(STATUS_DATA_OVERRUN);
		_CASE(STATUS_DEBUG_ATTACH_FAILED);
		_CASE(STATUS_DECRYPTION_FAILED);
		_CASE(STATUS_DELETE_PENDING);
		_CASE(STATUS_DESTINATION_ELEMENT_FULL);
		_CASE(STATUS_DEVICE_ALREADY_ATTACHED);
		_CASE(STATUS_DEVICE_BUSY);
		_CASE(STATUS_DEVICE_CONFIGURATION_ERROR);
		_CASE(STATUS_DEVICE_DATA_ERROR);
		_CASE(STATUS_DEVICE_DOES_NOT_EXIST);
		_CASE(STATUS_DEVICE_DOOR_OPEN);
		_CASE(STATUS_DEVICE_NOT_CONNECTED);
		_CASE(STATUS_DEVICE_NOT_PARTITIONED);
		_CASE(STATUS_DEVICE_NOT_READY);
		_CASE(STATUS_DEVICE_OFF_LINE);
		_CASE(STATUS_DEVICE_PAPER_EMPTY);
		_CASE(STATUS_DEVICE_POWERED_OFF);
		_CASE(STATUS_DEVICE_POWER_FAILURE);
		_CASE(STATUS_DEVICE_PROTOCOL_ERROR);
		_CASE(STATUS_DEVICE_REMOVED);
		_CASE(STATUS_DEVICE_REQUIRES_CLEANING);
		_CASE(STATUS_DFS_EXIT_PATH_FOUND);
		_CASE(STATUS_DFS_UNAVAILABLE);
		_CASE(STATUS_DIRECTORY_IS_A_REPARSE_POINT);
		_CASE(STATUS_DIRECTORY_NOT_EMPTY);
		_CASE(STATUS_DIRECTORY_SERVICE_REQUIRED);
		_CASE(STATUS_DISK_CORRUPT_ERROR);
		_CASE(STATUS_DISK_FULL);
		_CASE(STATUS_DISK_OPERATION_FAILED);
		_CASE(STATUS_DISK_RECALIBRATE_FAILED);
		_CASE(STATUS_DISK_RESET_FAILED);
		_CASE(STATUS_DLL_INIT_FAILED);
		_CASE(STATUS_DLL_INIT_FAILED_LOGOFF);
		_CASE(STATUS_DLL_NOT_FOUND);
		_CASE(STATUS_DOMAIN_CONTROLLER_NOT_FOUND);
		_CASE(STATUS_DOMAIN_CTRLR_CONFIG_ERROR);
		_CASE(STATUS_DOMAIN_EXISTS);
		_CASE(STATUS_DOMAIN_LIMIT_EXCEEDED);
		_CASE(STATUS_DOMAIN_TRUST_INCONSISTENT);
		_CASE(STATUS_DRIVER_CANCEL_TIMEOUT);
		_CASE(STATUS_DRIVER_ENTRYPOINT_NOT_FOUND);
		_CASE(STATUS_DRIVER_FAILED_SLEEP);
		_CASE(STATUS_DRIVER_INTERNAL_ERROR);
		_CASE(STATUS_DRIVER_ORDINAL_NOT_FOUND);
		_CASE(STATUS_DRIVER_UNABLE_TO_LOAD);
		_CASE(STATUS_DS_ADMIN_LIMIT_EXCEEDED);
		_CASE(STATUS_DS_ATTRIBUTE_OR_VALUE_EXISTS);
		_CASE(STATUS_DS_ATTRIBUTE_TYPE_UNDEFINED);
		_CASE(STATUS_DS_BUSY);
		_CASE(STATUS_DS_CANT_MOD_OBJ_CLASS);
		_CASE(STATUS_DS_CANT_MOD_PRIMARYGROUPID);
		_CASE(STATUS_DS_CANT_ON_NON_LEAF);
		_CASE(STATUS_DS_CANT_ON_RDN);
		_CASE(STATUS_DS_CANT_START);
		_CASE(STATUS_DS_CROSS_DOM_MOVE_FAILED);
		_CASE(STATUS_DS_GC_NOT_AVAILABLE);
		_CASE(STATUS_DS_GC_REQUIRED);
		_CASE(STATUS_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER);
		_CASE(STATUS_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER);
		_CASE(STATUS_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER);
		_CASE(STATUS_DS_HAVE_PRIMARY_MEMBERS);
		_CASE(STATUS_DS_INCORRECT_ROLE_OWNER);
		_CASE(STATUS_DS_INIT_FAILURE);
		_CASE(STATUS_DS_INVALID_ATTRIBUTE_SYNTAX);
		_CASE(STATUS_DS_INVALID_GROUP_TYPE);
		_CASE(STATUS_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER);
		_CASE(STATUS_DS_LOCAL_MEMBER_OF_LOCAL_ONLY);
		_CASE(STATUS_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED);
		_CASE(STATUS_DS_MEMBERSHIP_EVALUATED_LOCALLY);
		_CASE(STATUS_DS_NO_ATTRIBUTE_OR_VALUE);
		_CASE(STATUS_DS_NO_FPO_IN_UNIVERSAL_GROUPS);
		_CASE(STATUS_DS_NO_MORE_RIDS);
		_CASE(STATUS_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN);
		_CASE(STATUS_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN);
		_CASE(STATUS_DS_NO_RIDS_ALLOCATED);
		_CASE(STATUS_DS_OBJ_CLASS_VIOLATION);
		_CASE(STATUS_DS_RIDMGR_INIT_ERROR);
		_CASE(STATUS_DS_SAM_INIT_FAILURE);
		_CASE(STATUS_DS_SENSITIVE_GROUP_VIOLATION);
		_CASE(STATUS_DS_UNAVAILABLE);
		_CASE(STATUS_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER);
		_CASE(STATUS_DUPLICATE_NAME);
		_CASE(STATUS_DUPLICATE_OBJECTID);
		_CASE(STATUS_EAS_NOT_SUPPORTED);
		_CASE(STATUS_EA_CORRUPT_ERROR);
		_CASE(STATUS_EA_LIST_INCONSISTENT);
		_CASE(STATUS_EA_TOO_LARGE);
		_CASE(STATUS_ENCRYPTION_FAILED);
		_CASE(STATUS_END_OF_FILE);
		_CASE(STATUS_END_OF_MEDIA);
		_CASE(STATUS_ENTRYPOINT_NOT_FOUND);
		_CASE(STATUS_EOM_OVERFLOW);
		_CASE(STATUS_EVALUATION_EXPIRATION);
		_CASE(STATUS_EVENTLOG_CANT_START);
		_CASE(STATUS_EVENTLOG_FILE_CHANGED);
		_CASE(STATUS_EVENTLOG_FILE_CORRUPT);
		_CASE(STATUS_EVENT_DONE);
		_CASE(STATUS_EVENT_PENDING);
		_CASE(STATUS_EXTRANEOUS_INFORMATION);
		_CASE(STATUS_FAIL_CHECK);
		_CASE(STATUS_FATAL_APP_EXIT);
		_CASE(STATUS_FILEMARK_DETECTED);
		_CASE(STATUS_FILES_OPEN);
		_CASE(STATUS_FILE_CLOSED);
		_CASE(STATUS_FILE_CORRUPT_ERROR);
		_CASE(STATUS_FILE_DELETED);
		_CASE(STATUS_FILE_ENCRYPTED);
		_CASE(STATUS_FILE_FORCED_CLOSED);
		_CASE(STATUS_FILE_INVALID);
		_CASE(STATUS_FILE_IS_A_DIRECTORY);
		_CASE(STATUS_FILE_IS_OFFLINE);
		_CASE(STATUS_FILE_LOCK_CONFLICT);
		_CASE(STATUS_FILE_NOT_ENCRYPTED);
		_CASE(STATUS_FILE_RENAMED);
		_CASE(STATUS_FLOAT_DENORMAL_OPERAND);
		_CASE(STATUS_FLOAT_DIVIDE_BY_ZERO);
		_CASE(STATUS_FLOAT_INEXACT_RESULT);
		_CASE(STATUS_FLOAT_INVALID_OPERATION);
		_CASE(STATUS_FLOAT_MULTIPLE_FAULTS);
		_CASE(STATUS_FLOAT_MULTIPLE_TRAPS);
		_CASE(STATUS_FLOAT_OVERFLOW);
		_CASE(STATUS_FLOAT_STACK_CHECK);
		_CASE(STATUS_FLOAT_UNDERFLOW);
		_CASE(STATUS_FLOPPY_BAD_REGISTERS);
		_CASE(STATUS_FLOPPY_ID_MARK_NOT_FOUND);
		_CASE(STATUS_FLOPPY_UNKNOWN_ERROR);
		_CASE(STATUS_FLOPPY_VOLUME);
		_CASE(STATUS_FLOPPY_WRONG_CYLINDER);
		_CASE(STATUS_FOUND_OUT_OF_SCOPE);
		_CASE(xbox::status_free_vm_not_at_base);
		_CASE(STATUS_FS_DRIVER_REQUIRED);
		_CASE(STATUS_FT_MISSING_MEMBER);
		_CASE(STATUS_FT_ORPHANING);
		_CASE(STATUS_FT_READ_RECOVERY_FROM_BACKUP);
		_CASE(STATUS_FT_WRITE_RECOVERY);
		_CASE(STATUS_FULLSCREEN_MODE);
		_CASE(STATUS_GENERIC_NOT_MAPPED);
		_CASE(STATUS_GRACEFUL_DISCONNECT);
		_CASE(STATUS_GROUP_EXISTS);
		_CASE(STATUS_GUARD_PAGE_VIOLATION);
		_CASE(STATUS_GUIDS_EXHAUSTED);
		_CASE(STATUS_GUID_SUBSTITUTION_MADE);
		_CASE(STATUS_HANDLES_CLOSED);
		_CASE(STATUS_HANDLE_NOT_CLOSABLE);
		_CASE(STATUS_HOST_UNREACHABLE);
		_CASE(STATUS_ILLEGAL_CHARACTER);
		_CASE(STATUS_ILLEGAL_DLL_RELOCATION);
		_CASE(STATUS_ILLEGAL_ELEMENT_ADDRESS);
		_CASE(STATUS_ILLEGAL_FLOAT_CONTEXT);
		_CASE(STATUS_ILLEGAL_FUNCTION);
		_CASE(STATUS_ILLEGAL_INSTRUCTION);
		_CASE(STATUS_ILL_FORMED_PASSWORD);
		_CASE(STATUS_ILL_FORMED_SERVICE_ENTRY);
		_CASE(STATUS_IMAGE_ALREADY_LOADED);
		_CASE(STATUS_IMAGE_CHECKSUM_MISMATCH);
		_CASE(STATUS_IMAGE_MACHINE_TYPE_MISMATCH);
		_CASE(STATUS_IMAGE_MACHINE_TYPE_MISMATCH_EXE);
		_CASE(STATUS_IMAGE_MP_UP_MISMATCH);
		_CASE(STATUS_IMAGE_NOT_AT_BASE);
		_CASE(STATUS_INCOMPATIBLE_FILE_MAP);
		_CASE(STATUS_INFO_LENGTH_MISMATCH);
		_CASE(STATUS_INSTANCE_NOT_AVAILABLE);
		_CASE(STATUS_INSTRUCTION_MISALIGNMENT);
		_CASE(STATUS_INSUFFICIENT_LOGON_INFO);
		_CASE(STATUS_INSUFFICIENT_POWER);
		_CASE(xbox::status_insufficient_resources);
		_CASE(STATUS_INSUFF_SERVER_RESOURCES);
		_CASE(STATUS_INTEGER_DIVIDE_BY_ZERO);
		_CASE(STATUS_INTEGER_OVERFLOW);
		_CASE(STATUS_INTERNAL_DB_CORRUPTION);
		_CASE(STATUS_INTERNAL_DB_ERROR);
		_CASE(STATUS_INTERNAL_ERROR);
		_CASE(STATUS_INVALID_ACCOUNT_NAME);
		_CASE(STATUS_INVALID_ACL);
		_CASE(STATUS_INVALID_ADDRESS);
		_CASE(STATUS_INVALID_ADDRESS_COMPONENT);
		_CASE(STATUS_INVALID_ADDRESS_WILDCARD);
		_CASE(STATUS_INVALID_BLOCK_LENGTH);
		_CASE(STATUS_INVALID_BUFFER_SIZE);
		_CASE(STATUS_INVALID_CID);
		_CASE(STATUS_INVALID_COMPUTER_NAME);
		_CASE(STATUS_INVALID_CONNECTION);
		_CASE(STATUS_INVALID_DEVICE_REQUEST);
		_CASE(STATUS_INVALID_DEVICE_STATE);
		_CASE(STATUS_INVALID_DISPOSITION);
		_CASE(STATUS_INVALID_DOMAIN_ROLE);
		_CASE(STATUS_INVALID_DOMAIN_STATE);
		_CASE(STATUS_INVALID_EA_FLAG);
		_CASE(STATUS_INVALID_EA_NAME);
		_CASE(STATUS_INVALID_FILE_FOR_SECTION);
		_CASE(STATUS_INVALID_GROUP_ATTRIBUTES);
		_CASE(STATUS_INVALID_HANDLE);
		_CASE(STATUS_INVALID_HW_PROFILE);
		_CASE(STATUS_INVALID_ID_AUTHORITY);
		_CASE(xbox::status_invalid_image_format);
		_CASE(STATUS_INVALID_IMAGE_LE_FORMAT);
		_CASE(STATUS_INVALID_IMAGE_NE_FORMAT);
		_CASE(STATUS_INVALID_IMAGE_NOT_MZ);
		_CASE(STATUS_INVALID_IMAGE_PROTECT);
		_CASE(STATUS_INVALID_IMAGE_WIN_16);
		_CASE(STATUS_INVALID_INFO_CLASS);
		_CASE(STATUS_INVALID_LDT_DESCRIPTOR);
		_CASE(STATUS_INVALID_LDT_OFFSET);
		_CASE(STATUS_INVALID_LDT_SIZE);
		_CASE(STATUS_INVALID_LEVEL);
		_CASE(STATUS_INVALID_LOCK_SEQUENCE);
		_CASE(STATUS_INVALID_LOGON_HOURS);
		_CASE(STATUS_INVALID_LOGON_TYPE);
		_CASE(STATUS_INVALID_MEMBER);
		_CASE(STATUS_INVALID_NETWORK_RESPONSE);
		_CASE(STATUS_INVALID_OPLOCK_PROTOCOL);
		_CASE(STATUS_INVALID_OWNER);
		_CASE(xbox::status_invalid_page_protection);
		_CASE(STATUS_INVALID_PARAMETER);
		_CASE(STATUS_INVALID_PARAMETER_1);
		_CASE(STATUS_INVALID_PARAMETER_10);
		_CASE(STATUS_INVALID_PARAMETER_11);
		_CASE(STATUS_INVALID_PARAMETER_12);
		_CASE(xbox::status_invalid_parameter_2);
		_CASE(STATUS_INVALID_PARAMETER_3);
		_CASE(STATUS_INVALID_PARAMETER_4);
		_CASE(STATUS_INVALID_PARAMETER_5);
		_CASE(STATUS_INVALID_PARAMETER_6);
		_CASE(STATUS_INVALID_PARAMETER_7);
		_CASE(STATUS_INVALID_PARAMETER_8);
		_CASE(STATUS_INVALID_PARAMETER_9);
		_CASE(STATUS_INVALID_PARAMETER_MIX);
		_CASE(STATUS_INVALID_PIPE_STATE);
		_CASE(STATUS_INVALID_PLUGPLAY_DEVICE_PATH);
		_CASE(STATUS_INVALID_PORT_ATTRIBUTES);
		_CASE(STATUS_INVALID_PORT_HANDLE);
		_CASE(STATUS_INVALID_PRIMARY_GROUP);
		_CASE(STATUS_INVALID_QUOTA_LOWER);
		_CASE(STATUS_INVALID_READ_MODE);
		_CASE(STATUS_INVALID_SECURITY_DESCR);
		_CASE(STATUS_INVALID_SERVER_STATE);
		_CASE(STATUS_INVALID_SID);
		_CASE(STATUS_INVALID_SUB_AUTHORITY);
		_CASE(STATUS_INVALID_SYSTEM_SERVICE);
		_CASE(STATUS_INVALID_UNWIND_TARGET);
		_CASE(STATUS_INVALID_USER_BUFFER);
		_CASE(STATUS_INVALID_VARIANT);
		_CASE(STATUS_INVALID_VIEW_SIZE);
		_CASE(STATUS_INVALID_VOLUME_LABEL);
		_CASE(STATUS_INVALID_WORKSTATION);
		_CASE(STATUS_IN_PAGE_ERROR);
		_CASE(STATUS_IO_DEVICE_ERROR);
		_CASE(STATUS_IO_PRIVILEGE_FAILED);
		_CASE(STATUS_IO_REPARSE_DATA_INVALID);
		_CASE(STATUS_IO_REPARSE_TAG_INVALID);
		_CASE(STATUS_IO_REPARSE_TAG_MISMATCH);
		_CASE(STATUS_IO_REPARSE_TAG_NOT_HANDLED);
		_CASE(STATUS_IO_TIMEOUT);
		_CASE(STATUS_IP_ADDRESS_CONFLICT1);
		_CASE(STATUS_IP_ADDRESS_CONFLICT2);
		_CASE(STATUS_JOURNAL_DELETE_IN_PROGRESS);
		_CASE(STATUS_JOURNAL_ENTRY_DELETED);
		_CASE(STATUS_JOURNAL_NOT_ACTIVE);
		_CASE(STATUS_KERNEL_APC);
		_CASE(STATUS_KEY_DELETED);
		_CASE(STATUS_KEY_HAS_CHILDREN);
		_CASE(STATUS_LAST_ADMIN);
		_CASE(STATUS_LICENSE_QUOTA_EXCEEDED);
		_CASE(STATUS_LICENSE_VIOLATION);
		_CASE(STATUS_LINK_FAILED);
		_CASE(STATUS_LINK_TIMEOUT);
		_CASE(STATUS_LM_CROSS_ENCRYPTION_REQUIRED);
		_CASE(STATUS_LOCAL_DISCONNECT);
		_CASE(STATUS_LOCAL_USER_SESSION_KEY);
		_CASE(STATUS_LOCK_NOT_GRANTED);
		_CASE(STATUS_LOGIN_TIME_RESTRICTION);
		_CASE(STATUS_LOGIN_WKSTA_RESTRICTION);
		_CASE(STATUS_LOGON_FAILURE);
		_CASE(STATUS_LOGON_NOT_GRANTED);
		_CASE(STATUS_LOGON_SERVER_CONFLICT);
		_CASE(STATUS_LOGON_SESSION_COLLISION);
		_CASE(STATUS_LOGON_SESSION_EXISTS);
		_CASE(STATUS_LOGON_TYPE_NOT_GRANTED);
		_CASE(STATUS_LOG_FILE_FULL);
		_CASE(STATUS_LOG_HARD_ERROR);
		_CASE(STATUS_LONGJUMP);
		_CASE(STATUS_LOST_WRITEBEHIND_DATA);
		_CASE(STATUS_LPC_REPLY_LOST);
		_CASE(STATUS_LUIDS_EXHAUSTED);
		_CASE(STATUS_MAGAZINE_NOT_PRESENT);
		_CASE(STATUS_MAPPED_ALIGNMENT);
		_CASE(STATUS_MAPPED_FILE_SIZE_ZERO);
		_CASE(STATUS_MARSHALL_OVERFLOW);
		_CASE(STATUS_MEDIA_CHANGED);
		_CASE(STATUS_MEDIA_CHECK);
		_CASE(STATUS_MEDIA_WRITE_PROTECTED);
		_CASE(STATUS_MEMBERS_PRIMARY_GROUP);
		_CASE(STATUS_MEMBER_IN_ALIAS);
		_CASE(STATUS_MEMBER_IN_GROUP);
		_CASE(STATUS_MEMBER_NOT_IN_ALIAS);
		_CASE(STATUS_MEMBER_NOT_IN_GROUP);
		_CASE(xbox::status_memory_not_allocated);
		_CASE(STATUS_MESSAGE_NOT_FOUND);
		_CASE(STATUS_MISSING_SYSTEMFILE);
		_CASE(STATUS_MORE_ENTRIES);
		_CASE(STATUS_MORE_PROCESSING_REQUIRED);
		_CASE(STATUS_MP_PROCESSOR_MISMATCH);
		_CASE(STATUS_MULTIPLE_FAULT_VIOLATION);
		_CASE(xbox::status_mutant_limit_exceeded);
		_CASE(STATUS_MUTANT_NOT_OWNED);
		_CASE(STATUS_MUTUAL_AUTHENTICATION_FAILED);
		_CASE(STATUS_NAME_TOO_LONG);
		_CASE(STATUS_NETLOGON_NOT_STARTED);
		_CASE(STATUS_NETWORK_ACCESS_DENIED);
		_CASE(STATUS_NETWORK_BUSY);
		_CASE(STATUS_NETWORK_CREDENTIAL_CONFLICT);
		_CASE(STATUS_NETWORK_NAME_DELETED);
		_CASE(STATUS_NETWORK_UNREACHABLE);
		_CASE(STATUS_NET_WRITE_FAULT);
		_CASE(STATUS_NOINTERFACE);
		_CASE(STATUS_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT);
		_CASE(STATUS_NOLOGON_SERVER_TRUST_ACCOUNT);
		_CASE(STATUS_NOLOGON_WORKSTATION_TRUST_ACCOUNT);
		_CASE(STATUS_NONCONTINUABLE_EXCEPTION);
		_CASE(STATUS_NONEXISTENT_EA_ENTRY);
		_CASE(STATUS_NONEXISTENT_SECTOR);
		_CASE(STATUS_NONE_MAPPED);
		_CASE(STATUS_NOTIFY_CLEANUP);
		_CASE(STATUS_NOTIFY_ENUM_DIR);
		_CASE(STATUS_NOT_ALL_ASSIGNED);
		_CASE(STATUS_NOT_A_DIRECTORY);
		_CASE(STATUS_NOT_A_REPARSE_POINT);
		_CASE(STATUS_NOT_CLIENT_SESSION);
		_CASE(xbox::status_not_committed);
		_CASE(STATUS_NOT_EXPORT_FORMAT);
		_CASE(STATUS_NOT_FOUND);
		_CASE(STATUS_NOT_IMPLEMENTED);
		_CASE(STATUS_NOT_LOCKED);
		_CASE(STATUS_NOT_LOGON_PROCESS);
		_CASE(STATUS_NOT_MAPPED_DATA);
		_CASE(STATUS_NOT_MAPPED_VIEW);
		_CASE(STATUS_NOT_REGISTRY_FILE);
		_CASE(STATUS_NOT_SAME_DEVICE);
		_CASE(STATUS_NOT_SERVER_SESSION);
		_CASE(STATUS_NOT_SUPPORTED);
		_CASE(STATUS_NOT_SUPPORTED_ON_SBS);
		_CASE(STATUS_NOT_TINY_STREAM);
		_CASE(STATUS_NO_BROWSER_SERVERS_FOUND);
		_CASE(STATUS_NO_CALLBACK_ACTIVE);
		_CASE(STATUS_NO_DATA_DETECTED);
		_CASE(STATUS_NO_EAS_ON_FILE);
		_CASE(STATUS_NO_EFS);
		_CASE(STATUS_NO_EVENT_PAIR);
		_CASE(STATUS_NO_GUID_TRANSLATION);
		_CASE(STATUS_NO_IMPERSONATION_TOKEN);
		_CASE(STATUS_NO_INHERITANCE);
		_CASE(STATUS_NO_LDT);
		_CASE(STATUS_NO_LOGON_SERVERS);
		_CASE(STATUS_NO_LOG_SPACE);
		_CASE(STATUS_NO_MATCH);
		_CASE(STATUS_NO_MEDIA);
		_CASE(STATUS_NO_MEDIA_IN_DEVICE);
		_CASE(xbox::status_no_memory);
		_CASE(STATUS_NO_MORE_EAS);
		_CASE(STATUS_NO_MORE_ENTRIES);
		_CASE(STATUS_NO_MORE_FILES);
		_CASE(STATUS_NO_MORE_MATCHES);
		_CASE(STATUS_NO_PAGEFILE);
		_CASE(STATUS_NO_QUOTAS_FOR_ACCOUNT);
		_CASE(STATUS_NO_RECOVERY_POLICY);
		_CASE(STATUS_NO_SECURITY_ON_OBJECT);
		_CASE(STATUS_NO_SPOOL_SPACE);
		_CASE(STATUS_NO_SUCH_ALIAS);
		_CASE(STATUS_NO_SUCH_DEVICE);
		_CASE(STATUS_NO_SUCH_DOMAIN);
		_CASE(STATUS_NO_SUCH_FILE);
		_CASE(STATUS_NO_SUCH_GROUP);
		_CASE(STATUS_NO_SUCH_LOGON_SESSION);
		_CASE(STATUS_NO_SUCH_MEMBER);
		_CASE(STATUS_NO_SUCH_PACKAGE);
		_CASE(STATUS_NO_SUCH_PRIVILEGE);
		_CASE(STATUS_NO_SUCH_USER);
		_CASE(STATUS_NO_TOKEN);
		_CASE(STATUS_NO_TRACKING_SERVICE);
		_CASE(STATUS_NO_TRUST_LSA_SECRET);
		_CASE(STATUS_NO_TRUST_SAM_ACCOUNT);
		_CASE(STATUS_NO_USER_KEYS);
		_CASE(STATUS_NO_USER_SESSION_KEY);
		_CASE(STATUS_NO_YIELD_PERFORMED);
		_CASE(STATUS_NT_CROSS_ENCRYPTION_REQUIRED);
		_CASE(STATUS_NULL_LM_PASSWORD);
		_CASE(STATUS_OBJECTID_EXISTS);
		_CASE(xbox::status_object_name_collision);
		_CASE(STATUS_OBJECT_NAME_EXISTS);
		_CASE(STATUS_OBJECT_NAME_INVALID);
		_CASE(xbox::status_object_name_not_found);
		_CASE(STATUS_OBJECT_PATH_INVALID);
		_CASE(STATUS_OBJECT_PATH_NOT_FOUND);
		_CASE(STATUS_OBJECT_PATH_SYNTAX_BAD);
		_CASE(STATUS_OBJECT_TYPE_MISMATCH);
		_CASE(STATUS_ONLY_IF_CONNECTED);
		_CASE(STATUS_OPEN_FAILED);
		_CASE(STATUS_OPLOCK_BREAK_IN_PROGRESS);
		_CASE(STATUS_OPLOCK_NOT_GRANTED);
		_CASE(STATUS_ORDINAL_NOT_FOUND);
		_CASE(STATUS_PAGEFILE_CREATE_FAILED);
		_CASE(STATUS_PAGEFILE_QUOTA);
		_CASE(STATUS_PAGEFILE_QUOTA_EXCEEDED);
		_CASE(STATUS_PAGE_FAULT_COPY_ON_WRITE);
		_CASE(STATUS_PAGE_FAULT_DEMAND_ZERO);
		_CASE(STATUS_PAGE_FAULT_GUARD_PAGE);
		_CASE(STATUS_PAGE_FAULT_PAGING_FILE);
		_CASE(STATUS_PAGE_FAULT_TRANSITION);
		_CASE(STATUS_PARITY_ERROR);
		_CASE(STATUS_PARTIAL_COPY);
		_CASE(STATUS_PARTITION_FAILURE);
		_CASE(STATUS_PASSWORD_EXPIRED);
		_CASE(STATUS_PASSWORD_MUST_CHANGE);
		_CASE(STATUS_PASSWORD_RESTRICTION);
		_CASE(STATUS_PATH_NOT_COVERED);
		_CASE(xbox::status_pending);
		_CASE(STATUS_PIPE_BROKEN);
		_CASE(STATUS_PIPE_BUSY);
		_CASE(STATUS_PIPE_CLOSING);
		_CASE(STATUS_PIPE_CONNECTED);
		_CASE(STATUS_PIPE_DISCONNECTED);
		_CASE(STATUS_PIPE_EMPTY);
		_CASE(STATUS_PIPE_LISTENING);
		_CASE(STATUS_PIPE_NOT_AVAILABLE);
		_CASE(STATUS_PLUGPLAY_NO_DEVICE);
		_CASE(STATUS_PNP_BAD_MPS_TABLE);
		_CASE(STATUS_PNP_IRQ_TRANSLATION_FAILED);
		_CASE(STATUS_PNP_REBOOT_REQUIRED);
		_CASE(STATUS_PNP_RESTART_ENUMERATION);
		_CASE(STATUS_PNP_TRANSLATION_FAILED);
		_CASE(STATUS_POLICY_OBJECT_NOT_FOUND);
		_CASE(STATUS_POLICY_ONLY_IN_DS);
		_CASE(STATUS_PORT_ALREADY_SET);
		_CASE(STATUS_PORT_CONNECTION_REFUSED);
		_CASE(STATUS_PORT_DISCONNECTED);
		_CASE(STATUS_PORT_MESSAGE_TOO_LONG);
		_CASE(STATUS_PORT_UNREACHABLE);
		_CASE(STATUS_POSSIBLE_DEADLOCK);
		_CASE(STATUS_POWER_STATE_INVALID);
		_CASE(STATUS_PREDEFINED_HANDLE);
		_CASE(STATUS_PRIMARY_TRANSPORT_CONNECT_FAILED);
		_CASE(STATUS_PRINT_CANCELLED);
		_CASE(STATUS_PRINT_QUEUE_FULL);
		_CASE(STATUS_PRIVILEGED_INSTRUCTION);
		_CASE(STATUS_PRIVILEGE_NOT_HELD);
		_CASE(STATUS_PROCEDURE_NOT_FOUND);
		_CASE(STATUS_PROCESS_IS_TERMINATING);
		_CASE(STATUS_PROFILING_AT_LIMIT);
		_CASE(STATUS_PROFILING_NOT_STARTED);
		_CASE(STATUS_PROFILING_NOT_STOPPED);
		_CASE(STATUS_PROPSET_NOT_FOUND);
		_CASE(STATUS_PROTOCOL_UNREACHABLE);
		_CASE(STATUS_PWD_HISTORY_CONFLICT);
		_CASE(STATUS_PWD_TOO_RECENT);
		_CASE(STATUS_PWD_TOO_SHORT);
		_CASE(STATUS_QUOTA_EXCEEDED);
		_CASE(STATUS_QUOTA_LIST_INCONSISTENT);
		_CASE(STATUS_RANGE_LIST_CONFLICT);
		_CASE(STATUS_RANGE_NOT_FOUND);
		_CASE(STATUS_RANGE_NOT_LOCKED);
		_CASE(STATUS_RDP_PROTOCOL_ERROR);
		_CASE(STATUS_RECEIVE_EXPEDITED);
		_CASE(STATUS_RECEIVE_PARTIAL);
		_CASE(STATUS_RECEIVE_PARTIAL_EXPEDITED);
		_CASE(STATUS_RECOVERY_FAILURE);
		_CASE(STATUS_REDIRECTOR_HAS_OPEN_HANDLES);
		_CASE(STATUS_REDIRECTOR_NOT_STARTED);
		_CASE(STATUS_REDIRECTOR_PAUSED);
		_CASE(STATUS_REDIRECTOR_STARTED);
		_CASE(STATUS_REGISTRY_CORRUPT);
		_CASE(STATUS_REGISTRY_IO_FAILED);
		_CASE(STATUS_REGISTRY_QUOTA_LIMIT);
		_CASE(STATUS_REGISTRY_RECOVERED);
		_CASE(STATUS_REG_NAT_CONSUMPTION);
		_CASE(STATUS_REINITIALIZATION_NEEDED);
		_CASE(STATUS_REMOTE_DISCONNECT);
		_CASE(STATUS_REMOTE_NOT_LISTENING);
		_CASE(STATUS_REMOTE_RESOURCES);
		_CASE(STATUS_REMOTE_SESSION_LIMIT);
		_CASE(STATUS_REMOTE_STORAGE_MEDIA_ERROR);
		_CASE(STATUS_REMOTE_STORAGE_NOT_ACTIVE);
		_CASE(STATUS_REPARSE);
		_CASE(STATUS_REPARSE_ATTRIBUTE_CONFLICT);
		_CASE(STATUS_REPARSE_OBJECT);
		_CASE(STATUS_REPARSE_POINT_NOT_RESOLVED);
		_CASE(STATUS_REPLY_MESSAGE_MISMATCH);
		_CASE(STATUS_REQUEST_ABORTED);
		_CASE(STATUS_REQUEST_NOT_ACCEPTED);
		_CASE(STATUS_RESOURCE_DATA_NOT_FOUND);
		_CASE(STATUS_RESOURCE_LANG_NOT_FOUND);
		_CASE(STATUS_RESOURCE_NAME_NOT_FOUND);
		_CASE(STATUS_RESOURCE_NOT_OWNED);
		_CASE(STATUS_RESOURCE_REQUIREMENTS_CHANGED);
		_CASE(STATUS_RESOURCE_TYPE_NOT_FOUND);
		_CASE(STATUS_RETRY);
		_CASE(STATUS_REVISION_MISMATCH);
		_CASE(STATUS_RXACT_COMMITTED);
		_CASE(STATUS_RXACT_COMMIT_FAILURE);
		_CASE(STATUS_RXACT_COMMIT_NECESSARY);
		_CASE(STATUS_RXACT_INVALID_STATE);
		_CASE(STATUS_RXACT_STATE_CREATED);
		_CASE(STATUS_SAM_INIT_FAILURE);
		_CASE(STATUS_SAM_NEED_BOOTKEY_FLOPPY);
		_CASE(STATUS_SAM_NEED_BOOTKEY_PASSWORD);
		_CASE(STATUS_SECRET_TOO_LONG);
		_CASE(STATUS_SECTION_NOT_EXTENDED);
		_CASE(STATUS_SECTION_NOT_IMAGE);
		_CASE(STATUS_SECTION_PROTECTION);
		_CASE(STATUS_SECTION_TOO_BIG);
		_CASE(STATUS_SEGMENT_NOTIFICATION);
		_CASE(xbox::status_semaphore_limit_exceeded);
		_CASE(STATUS_SERIAL_COUNTER_TIMEOUT);
		_CASE(STATUS_SERIAL_MORE_WRITES);
		_CASE(STATUS_SERIAL_NO_DEVICE_INITED);
		_CASE(STATUS_SERVER_DISABLED);
		_CASE(STATUS_SERVER_HAS_OPEN_HANDLES);
		_CASE(STATUS_SERVER_NOT_DISABLED);
		_CASE(STATUS_SERVER_SID_MISMATCH);
		_CASE(STATUS_SERVICE_NOTIFICATION);
		_CASE(STATUS_SETMARK_DETECTED);
		_CASE(STATUS_SHARED_IRQ_BUSY);
		_CASE(STATUS_SHARED_POLICY);
		_CASE(STATUS_SHARING_PAUSED);
		_CASE(STATUS_SHARING_VIOLATION);
		_CASE(STATUS_SINGLE_STEP);
		_CASE(STATUS_SOME_NOT_MAPPED);
		_CASE(STATUS_SOURCE_ELEMENT_EMPTY);
		_CASE(STATUS_SPECIAL_ACCOUNT);
		_CASE(STATUS_SPECIAL_GROUP);
		_CASE(STATUS_SPECIAL_USER);
		_CASE(STATUS_STACK_OVERFLOW);
		_CASE(STATUS_STACK_OVERFLOW_READ);
		_CASE(xbox::status_success);
		_CASE(STATUS_SUSPEND_COUNT_EXCEEDED);
		_CASE(STATUS_SYNCHRONIZATION_REQUIRED);
		_CASE(STATUS_SYSTEM_IMAGE_BAD_SIGNATURE);
		_CASE(STATUS_SYSTEM_PROCESS_TERMINATED);
		_CASE(STATUS_THREAD_IS_TERMINATING);
		_CASE(STATUS_THREAD_NOT_IN_PROCESS);
		_CASE(STATUS_THREAD_WAS_SUSPENDED);
		_CASE(STATUS_TIMEOUT);
		_CASE(STATUS_TIMER_NOT_CANCELED);
		_CASE(STATUS_TIMER_RESOLUTION_NOT_SET);
		_CASE(xbox::status_timer_resume_ignored);
		_CASE(STATUS_TIME_DIFFERENCE_AT_DC);
		_CASE(STATUS_TOKEN_ALREADY_IN_USE);
		_CASE(STATUS_TOO_LATE);
		_CASE(STATUS_TOO_MANY_ADDRESSES);
		_CASE(STATUS_TOO_MANY_COMMANDS);
		_CASE(STATUS_TOO_MANY_CONTEXT_IDS);
		_CASE(STATUS_TOO_MANY_GUIDS_REQUESTED);
		_CASE(STATUS_TOO_MANY_LINKS);
		_CASE(STATUS_TOO_MANY_LUIDS_REQUESTED);
		_CASE(STATUS_TOO_MANY_NAMES);
		_CASE(STATUS_TOO_MANY_NODES);
		_CASE(STATUS_TOO_MANY_OPENED_FILES);
		_CASE(STATUS_TOO_MANY_PAGING_FILES);
		_CASE(xbox::status_too_many_secrets);
		_CASE(STATUS_TOO_MANY_SESSIONS);
		_CASE(STATUS_TOO_MANY_SIDS);
		_CASE(STATUS_TOO_MANY_THREADS);
		_CASE(STATUS_TRANSACTION_ABORTED);
		_CASE(STATUS_TRANSACTION_INVALID_ID);
		_CASE(STATUS_TRANSACTION_INVALID_TYPE);
		_CASE(STATUS_TRANSACTION_NO_MATCH);
		_CASE(STATUS_TRANSACTION_NO_RELEASE);
		_CASE(STATUS_TRANSACTION_RESPONDED);
		_CASE(STATUS_TRANSACTION_TIMED_OUT);
		_CASE(STATUS_TRANSLATION_COMPLETE);
		_CASE(STATUS_TRANSPORT_FULL);
		_CASE(STATUS_TRUSTED_DOMAIN_FAILURE);
		_CASE(STATUS_TRUSTED_RELATIONSHIP_FAILURE);
		_CASE(STATUS_TRUST_FAILURE);
		_CASE(STATUS_UNABLE_TO_DECOMMIT_VM);
		_CASE(STATUS_UNABLE_TO_DELETE_SECTION);
		_CASE(xbox::status_unable_to_free_vm);
		_CASE(STATUS_UNABLE_TO_LOCK_MEDIA);
		_CASE(STATUS_UNABLE_TO_UNLOAD_MEDIA);
		_CASE(STATUS_UNDEFINED_CHARACTER);
		_CASE(STATUS_UNEXPECTED_IO_ERROR);
		_CASE(STATUS_UNEXPECTED_MM_CREATE_ERR);
		_CASE(STATUS_UNEXPECTED_MM_EXTEND_ERR);
		_CASE(STATUS_UNEXPECTED_MM_MAP_ERROR);
		_CASE(STATUS_UNEXPECTED_NETWORK_ERROR);
		_CASE(STATUS_UNHANDLED_EXCEPTION);
		_CASE(STATUS_UNKNOWN_REVISION);
		_CASE(STATUS_UNMAPPABLE_CHARACTER);
		_CASE(xbox::status_unrecognized_media);
		_CASE(STATUS_UNRECOGNIZED_VOLUME);
		_CASE(xbox::status_unsuccessful);
		_CASE(STATUS_UNSUPPORTED_COMPRESSION);
		_CASE(STATUS_UNWIND);
		_CASE(xbox::status_user_apc);
		_CASE(STATUS_USER_EXISTS);
		_CASE(STATUS_USER_MAPPED_FILE);
		_CASE(STATUS_USER_SESSION_DELETED);
		_CASE(STATUS_VALIDATE_CONTINUE);
		_CASE(STATUS_VARIABLE_NOT_FOUND);
		_CASE(STATUS_VDM_HARD_ERROR);
		_CASE(STATUS_VERIFY_REQUIRED);
		_CASE(STATUS_VIRTUAL_CIRCUIT_CLOSED);
		_CASE(STATUS_VOLUME_DISMOUNTED);
		_CASE(STATUS_VOLUME_MOUNTED);
		_CASE(STATUS_VOLUME_NOT_UPGRADED);
		_CASE(STATUS_WAIT_1);
		_CASE(STATUS_WAIT_2);
		_CASE(STATUS_WAIT_3);
		_CASE(STATUS_WAIT_63);
		_CASE(STATUS_WAKE_SYSTEM);
		_CASE(STATUS_WAKE_SYSTEM_DEBUGGER);
		_CASE(STATUS_WAS_LOCKED);
		_CASE(STATUS_WAS_UNLOCKED);
		_CASE(STATUS_WMI_GUID_NOT_FOUND);
		_CASE(STATUS_WMI_INSTANCE_NOT_FOUND);
		_CASE(STATUS_WMI_ITEMID_NOT_FOUND);
		_CASE(STATUS_WMI_NOT_SUPPORTED);
		_CASE(STATUS_WMI_READ_ONLY);
		_CASE(STATUS_WMI_SET_FAILURE);
		_CASE(STATUS_WMI_TRY_AGAIN);
		_CASE(STATUS_WORKING_SET_LIMIT_RANGE);
		_CASE(STATUS_WORKING_SET_QUOTA);
		_CASE(STATUS_WOW_ASSERTION);
		_CASE(STATUS_WRONG_EFS);
		_CASE(STATUS_WRONG_PASSWORD);
		_CASE(STATUS_WRONG_PASSWORD_CORE);
		_CASE(STATUS_WRONG_VOLUME);
		_CASE(STATUS_WX86_BREAKPOINT);
		_CASE(STATUS_WX86_CONTINUE);
		_CASE(STATUS_WX86_CREATEWX86TIB);
		_CASE(STATUS_WX86_EXCEPTION_CHAIN);
		_CASE(STATUS_WX86_EXCEPTION_CONTINUE);
		_CASE(STATUS_WX86_EXCEPTION_LASTCHANCE);
		_CASE(STATUS_WX86_FLOAT_STACK_CHECK);
		_CASE(STATUS_WX86_INTERNAL_ERROR);
		_CASE(STATUS_WX86_SINGLE_STEP);
		_CASE(STATUS_WX86_UNSIMULATE);
	default:         return "STATUS_UNKNOWN";
	}

#undef _CASE
}
