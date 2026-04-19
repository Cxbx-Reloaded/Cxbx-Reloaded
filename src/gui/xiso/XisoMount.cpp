#define NOMINMAX

#include "XisoMount.hpp"

#include "dokan/dokan.h"
#include "dokan/fileinfo.h"

#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace {

class DokanApi final
{
public:
	using DokanInitFn = decltype(&::DokanInit);
	using DokanShutdownFn = decltype(&::DokanShutdown);
	using DokanCreateFileSystemFn = decltype(&::DokanCreateFileSystem);
	using DokanCloseHandleFn = decltype(&::DokanCloseHandle);
	using DokanVersionFn = decltype(&::DokanVersion);
	using DokanDriverVersionFn = decltype(&::DokanDriverVersion);
	using DokanMapKernelToUserCreateFileFlagsFn = decltype(&::DokanMapKernelToUserCreateFileFlags);

	bool EnsureLoaded(std::string& error)
	{
		if (m_module != nullptr) {
			return true;
		}

		m_module = ::LoadLibraryW(L"dokan2.dll");
		if (m_module == nullptr) {
			error = "Dokany is not installed. Install the upstream signed Dokany runtime to mount XISO images.";
			return false;
		}

		if (!LoadRequiredSymbol("DokanInit", m_DokanInit) ||
			!LoadRequiredSymbol("DokanShutdown", m_DokanShutdown) ||
			!LoadRequiredSymbol("DokanCreateFileSystem", m_DokanCreateFileSystem) ||
			!LoadRequiredSymbol("DokanCloseHandle", m_DokanCloseHandle) ||
			!LoadRequiredSymbol("DokanVersion", m_DokanVersion) ||
			!LoadRequiredSymbol("DokanDriverVersion", m_DokanDriverVersion) ||
			!LoadRequiredSymbol("DokanMapKernelToUserCreateFileFlags", m_DokanMapKernelToUserCreateFileFlags)) {
			error = "Dokany was found, but one or more required exports are missing from dokan2.dll.";
			Unload();
			return false;
		}

		return true;
	}

	void Unload()
	{
		if (m_module != nullptr) {
			::FreeLibrary(m_module);
			m_module = nullptr;
		}

		m_DokanInit = nullptr;
		m_DokanShutdown = nullptr;
		m_DokanCreateFileSystem = nullptr;
		m_DokanCloseHandle = nullptr;
		m_DokanVersion = nullptr;
		m_DokanDriverVersion = nullptr;
		m_DokanMapKernelToUserCreateFileFlags = nullptr;
	}

	DokanInitFn DokanInit() const { return m_DokanInit; }
	DokanShutdownFn DokanShutdown() const { return m_DokanShutdown; }
	DokanCreateFileSystemFn DokanCreateFileSystem() const { return m_DokanCreateFileSystem; }
	DokanCloseHandleFn DokanCloseHandle() const { return m_DokanCloseHandle; }
	DokanVersionFn DokanVersion() const { return m_DokanVersion; }
	DokanDriverVersionFn DokanDriverVersion() const { return m_DokanDriverVersion; }
	DokanMapKernelToUserCreateFileFlagsFn DokanMapKernelToUserCreateFileFlags() const { return m_DokanMapKernelToUserCreateFileFlags; }

private:
	template<typename T>
	bool LoadRequiredSymbol(const char* name, T& function)
	{
		function = reinterpret_cast<T>(::GetProcAddress(m_module, name));
		return function != nullptr;
	}

	HMODULE m_module{ nullptr };
	DokanInitFn m_DokanInit{ nullptr };
	DokanShutdownFn m_DokanShutdown{ nullptr };
	DokanCreateFileSystemFn m_DokanCreateFileSystem{ nullptr };
	DokanCloseHandleFn m_DokanCloseHandle{ nullptr };
	DokanVersionFn m_DokanVersion{ nullptr };
	DokanDriverVersionFn m_DokanDriverVersion{ nullptr };
	DokanMapKernelToUserCreateFileFlagsFn m_DokanMapKernelToUserCreateFileFlags{ nullptr };
};

DokanApi& GetDokanApi()
{
	static DokanApi api;
	return api;
}

namespace xdvdfs {
class Stream {
public:
	std::ifstream m_file;
	std::streampos m_offset{ 0 };
	std::mutex m_fileMutex;
};

constexpr int SECTOR_SIZE = 2048;
constexpr int VOLUME_DESCRIPTOR_SECTOR = 32;
constexpr uint8_t MAGIC_ID[] = "MICROSOFT*XBOX*MEDIA";

class FileEntry {
public:
	FileEntry() = default;
	FileEntry(const FileEntry&) = default;
	explicit FileEntry(const std::string& name)
		: m_attributes(FILE_DIRECTORY), m_filename(name) {
	}

	void ReadFromFile(Stream& file, std::streampos sector, std::streamoff offset)
	{
		std::vector<char> buffer(SECTOR_SIZE);

		{
			std::lock_guard<std::mutex> lock(file.m_fileMutex);
			file.m_file.seekg((sector * SECTOR_SIZE) + file.m_offset + offset, std::ifstream::beg);
			file.m_file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
		}

		m_sectorNumber = sector;

		std::copy(buffer.begin(), buffer.begin() + 0x02, reinterpret_cast<char*>(&m_leftSubTree));
		std::copy(buffer.begin() + 0x02, buffer.begin() + 0x04, reinterpret_cast<char*>(&m_rightSubTree));
		std::copy(buffer.begin() + 0x04, buffer.begin() + 0x08, reinterpret_cast<char*>(&m_startSector));
		std::copy(buffer.begin() + 0x08, buffer.begin() + 0x0C, reinterpret_cast<char*>(&m_fileSize));
		std::copy(buffer.begin() + 0x0C, buffer.begin() + 0x0D, reinterpret_cast<char*>(&m_attributes));

		if (Validate()) {
			const auto filenameLength = static_cast<size_t>(static_cast<uint8_t>(buffer[0x0D]));
			m_filename.assign(&buffer[0x0E], filenameLength);
		}
	}

	uint32_t Read(Stream& file, void* buffer, uint32_t bufferLength, int64_t offset) const
	{
		if (offset < 0) {
			return 0;
		}

		const auto localOffset = static_cast<uint32_t>(offset);
		if (bufferLength == 0 || localOffset >= m_fileSize) {
			return 0;
		}

		auto readLength = bufferLength;
		if (localOffset + bufferLength >= m_fileSize) {
			readLength = m_fileSize - localOffset;
		}

		const auto baseOffset = SECTOR_SIZE * static_cast<int64_t>(m_startSector) + file.m_offset + localOffset;

		std::lock_guard<std::mutex> lock(file.m_fileMutex);
		file.m_file.seekg(baseOffset, std::ifstream::beg);
		file.m_file.read(static_cast<char*>(buffer), readLength);
		return readLength;
	}

	bool Validate() const
	{
		return m_leftSubTree != static_cast<uint16_t>(-1) &&
			m_rightSubTree != static_cast<uint16_t>(-1) &&
			m_startSector != static_cast<uint32_t>(-1);
	}

	const std::string& GetFilename() const { return m_filename; }
	uint32_t GetFileSize() const { return m_fileSize; }
	uint8_t GetAttributes() const { return m_attributes; }
	bool IsDirectory() const { return (m_attributes & FILE_DIRECTORY) != 0; }
	bool HasLeftChild() const { return m_leftSubTree != 0; }
	bool HasRightChild() const { return m_rightSubTree != 0; }

	FileEntry GetLeftChild(Stream& file) const
	{
		FileEntry entry;
		entry.ReadFromFile(file, m_sectorNumber, static_cast<std::streampos>(m_leftSubTree) * 4);
		return entry;
	}

	FileEntry GetRightChild(Stream& file) const
	{
		FileEntry entry;
		entry.ReadFromFile(file, m_sectorNumber, static_cast<std::streampos>(m_rightSubTree) * 4);
		return entry;
	}

	FileEntry GetFirstEntry(Stream& file) const
	{
		FileEntry entry;
		if (IsDirectory()) {
			entry.ReadFromFile(file, m_startSector, 0);
		}
		return entry;
	}

	static constexpr uint8_t FILE_DIRECTORY = 0x10;

private:
	uint16_t m_leftSubTree{ static_cast<uint16_t>(-1) };
	uint16_t m_rightSubTree{ static_cast<uint16_t>(-1) };
	uint32_t m_startSector{ 0 };
	uint32_t m_fileSize{ 0 };
	uint8_t m_attributes{ 0 };
	std::string m_filename;
	std::streampos m_sectorNumber{ 0 };
};

class VolumeDescriptor {
public:
	void ReadFromFile(Stream& file)
	{
		std::vector<char> buffer(SECTOR_SIZE);

		{
			std::lock_guard<std::mutex> lock(file.m_fileMutex);
			file.m_file.seekg((VOLUME_DESCRIPTOR_SECTOR * SECTOR_SIZE) + file.m_offset, std::ifstream::beg);
			file.m_file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
		}

		std::copy(buffer.begin(), buffer.begin() + 0x14, m_id1);
		std::copy(buffer.begin() + 0x14, buffer.begin() + 0x18, reinterpret_cast<char*>(&m_rootDirTableSector));
		std::copy(buffer.begin() + 0x18, buffer.begin() + 0x1C, reinterpret_cast<char*>(&m_rootDirTableSize));
		std::copy(buffer.begin() + 0x1C, buffer.begin() + 0x24, reinterpret_cast<char*>(&m_filetime));
		std::copy(buffer.begin() + 0x7EC, buffer.end(), m_id2);
	}

	bool Validate() const
	{
		return std::memcmp(m_id1, MAGIC_ID, std::size(m_id1)) == 0 &&
			m_rootDirTableSector != 0 &&
			m_rootDirTableSize != 0 &&
			std::memcmp(m_id1, m_id2, std::size(m_id2)) == 0;
	}

	FileEntry GetRootDirEntry(Stream& file) const
	{
		FileEntry entry;
		entry.ReadFromFile(file, m_rootDirTableSector, 0);
		return entry;
	}

	uint64_t GetCreationTime() const { return m_filetime; }

private:
	uint8_t m_id1[0x14]{};
	uint32_t m_rootDirTableSector{ 0 };
	uint32_t m_rootDirTableSize{ 0 };
	uint64_t m_filetime{ 0 };
	uint8_t m_id2[0x14]{};
};
} // namespace xdvdfs

namespace xiso_vfs {

class Container {
public:
	using EntryHandle = size_t;
	using FileResults = std::vector<EntryHandle>;

	enum class SetupState {
		ErrorFile,
		ErrorFormat,
		Success,
	};

	SetupState Setup(const std::wstring& filename)
	{
		auto stream = std::make_unique<xdvdfs::Stream>();
		stream->m_file.open(filename.c_str(), std::ifstream::binary | std::ifstream::in);
		if (!stream->m_file.is_open()) {
			return SetupState::ErrorFile;
		}

		xdvdfs::VolumeDescriptor descriptor;
		descriptor.ReadFromFile(*stream);

		if (!descriptor.Validate()) {
			constexpr size_t gamePartitionOffset = xdvdfs::SECTOR_SIZE * 32 * 6192;
			stream->m_offset = static_cast<std::streampos>(gamePartitionOffset);
			descriptor.ReadFromFile(*stream);
			if (!descriptor.Validate()) {
				return SetupState::ErrorFormat;
			}
		}

		auto rootDir = descriptor.GetRootDirEntry(*stream);
		Build(*stream, rootDir);

		m_volumeModified = descriptor.GetCreationTime();

		stream->m_file.seekg(0, std::ifstream::end);
		m_volumeSize = static_cast<uint64_t>(stream->m_file.tellg());

		m_name = std::filesystem::path(filename).replace_extension().filename().wstring();
		m_stream = std::move(stream);
		return SetupState::Success;
	}

	const xdvdfs::FileEntry* GetEntry(const std::filesystem::path& path) const
	{
		const auto handle = GetHandle(path);
		return handle != InvalidHandle ? &m_entries[handle] : nullptr;
	}

	const xdvdfs::FileEntry* GetEntry(EntryHandle handle) const
	{
		return handle < m_entries.size() ? &m_entries[handle] : nullptr;
	}

	FileResults GetFolderList(const std::filesystem::path& path) const
	{
		FileResults results;
		const auto handle = GetHandle(path);
		for (size_t i = 0; i < m_parentHandles.size(); ++i) {
			if (m_parentHandles[i] == handle) {
				results.emplace_back(i);
			}
		}
		return results;
	}

	const std::wstring& GetFilename() const { return m_name; }
	uint64_t GetVolumeModified() const { return m_volumeModified; }
	uint64_t GetVolumeSize() const { return m_volumeSize; }
	xdvdfs::Stream* GetFileStream() const { return m_stream.get(); }

private:
	static constexpr EntryHandle InvalidHandle = ~0ULL;

	void Build(xdvdfs::Stream& file, xdvdfs::FileEntry& entry)
	{
		xdvdfs::FileEntry root("\\");
		const auto rootHandle = RegisterFileEntry(root, InvalidHandle);
		BuildRecursive(file, entry, rootHandle);
	}

	void BuildRecursive(xdvdfs::Stream& file, xdvdfs::FileEntry& entry, EntryHandle parent)
	{
		const auto handle = RegisterFileEntry(entry, parent);

		if (entry.IsDirectory()) {
			auto child = entry.GetFirstEntry(file);
			if (child.Validate()) {
				BuildRecursive(file, child, handle);
			}
		}

		if (entry.HasLeftChild()) {
			auto child = entry.GetLeftChild(file);
			if (child.Validate()) {
				BuildRecursive(file, child, parent);
			}
		}

		if (entry.HasRightChild()) {
			auto child = entry.GetRightChild(file);
			if (child.Validate()) {
				BuildRecursive(file, child, parent);
			}
		}
	}

	EntryHandle RegisterFileEntry(const xdvdfs::FileEntry& entry, EntryHandle parent)
	{
		m_entries.emplace_back(entry);
		m_parentHandles.emplace_back(parent);

		const auto handle = m_entries.size() - 1;
		m_entryMap.emplace(ResolveEntryKey(handle), handle);
		return handle;
	}

	std::string MakeEntryKey(const std::filesystem::path& path) const
	{
		auto result = path.string();
		std::transform(result.begin(), result.end(), result.begin(), [](char c) {
			return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			});
		return result;
	}

	std::string ResolveEntryKey(EntryHandle handle) const
	{
		std::vector<EntryHandle> handles{ handle };
		while (m_parentHandles[handle] != InvalidHandle) {
			handle = m_parentHandles[handle];
			handles.emplace_back(handle);
		}

		std::reverse(handles.begin(), handles.end());

		std::filesystem::path finalPath;
		for (const auto childHandle : handles) {
			finalPath /= m_entries[childHandle].GetFilename();
		}

		return MakeEntryKey(finalPath);
	}

	EntryHandle GetHandle(const std::filesystem::path& path) const
	{
		const auto key = MakeEntryKey(path);
		const auto it = m_entryMap.find(key);
		return it != m_entryMap.end() ? it->second : InvalidHandle;
	}

	std::vector<xdvdfs::FileEntry> m_entries;
	std::vector<EntryHandle> m_parentHandles;
	std::map<std::string, EntryHandle> m_entryMap;
	std::wstring m_name;
	uint64_t m_volumeModified{ 0 };
	uint64_t m_volumeSize{ 0 };
	std::unique_ptr<xdvdfs::Stream> m_stream;
};

constexpr DWORD VolumeSerialNumber = 0x11115555;

Container* GetContext(PDOKAN_FILE_INFO fileInfo)
{
	return reinterpret_cast<Container*>(fileInfo->DokanOptions->GlobalContext);
}

void LlongToDwLowHigh(const LONGLONG& value, DWORD& low, DWORD& high)
{
	high = static_cast<DWORD>(value >> 32);
	low = static_cast<DWORD>(value);
}

void LlongToFileTime(LONGLONG value, FILETIME& filetime)
{
	LlongToDwLowHigh(value, filetime.dwLowDateTime, filetime.dwHighDateTime);
}

NTSTATUS DOKAN_CALLBACK CreateFileCallback(LPCWSTR filename, PDOKAN_IO_SECURITY_CONTEXT, ACCESS_MASK desiredAccess,
	ULONG fileAttributes, ULONG, ULONG createDisposition, ULONG createOptions, PDOKAN_FILE_INFO fileInfo)
{
	DWORD genericDesiredAccess = 0;
	DWORD creationDisposition = 0;
	DWORD fileAttributesAndFlags = 0;

	GetDokanApi().DokanMapKernelToUserCreateFileFlags()(desiredAccess, fileAttributes, createOptions, createDisposition,
		&genericDesiredAccess, &fileAttributesAndFlags, &creationDisposition);

	auto* container = GetContext(fileInfo);
	const auto* entry = container->GetEntry(filename);

	fileInfo->IsDirectory = entry != nullptr && entry->IsDirectory();

	if (fileInfo->IsDirectory && (createOptions & FILE_NON_DIRECTORY_FILE)) {
		return STATUS_FILE_IS_A_DIRECTORY;
	}

	if (entry != nullptr && (desiredAccess & FILE_WRITE_DATA)) {
		return STATUS_ACCESS_DENIED;
	}

	if (fileInfo->IsDirectory) {
		if (creationDisposition == CREATE_NEW || creationDisposition == OPEN_ALWAYS) {
			return STATUS_NOT_IMPLEMENTED;
		}
	}
	else {
		switch (creationDisposition) {
		case OPEN_ALWAYS:
			if (entry == nullptr) {
				return STATUS_NOT_IMPLEMENTED;
			}
			break;
		case OPEN_EXISTING:
			if (entry == nullptr) {
				return STATUS_OBJECT_NAME_NOT_FOUND;
			}
			break;
		case CREATE_ALWAYS:
		case CREATE_NEW:
		case TRUNCATE_EXISTING:
			return STATUS_ACCESS_DENIED;
		}
	}

	if (entry != nullptr && (creationDisposition == CREATE_NEW || creationDisposition == OPEN_ALWAYS)) {
		return STATUS_OBJECT_NAME_COLLISION;
	}

	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK ReadFileCallback(LPCWSTR filename, LPVOID buffer, DWORD bufferLength, LPDWORD readLength,
	LONGLONG offset, PDOKAN_FILE_INFO fileInfo)
{
	auto* container = GetContext(fileInfo);
	const auto* entry = container->GetEntry(filename);
	if (entry == nullptr) {
		return STATUS_OBJECT_NAME_NOT_FOUND;
	}

	if (!entry->IsDirectory()) {
		*readLength = entry->Read(*container->GetFileStream(), buffer, bufferLength, offset);
	}
	else {
		*readLength = 0;
	}

	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK GetFileInformationCallback(LPCWSTR filename, LPBY_HANDLE_FILE_INFORMATION buffer,
	PDOKAN_FILE_INFO fileInfo)
{
	auto* container = GetContext(fileInfo);
	const auto* entry = container->GetEntry(filename);
	if (entry == nullptr) {
		return STATUS_OBJECT_NAME_NOT_FOUND;
	}

	DWORD attributes = FILE_ATTRIBUTE_READONLY;
	if (entry->IsDirectory()) {
		attributes |= FILE_ATTRIBUTE_DIRECTORY;
	}

	buffer->dwFileAttributes = attributes;
	LlongToFileTime(container->GetVolumeModified(), buffer->ftCreationTime);
	LlongToFileTime(container->GetVolumeModified(), buffer->ftLastAccessTime);
	LlongToFileTime(container->GetVolumeModified(), buffer->ftLastWriteTime);
	LlongToDwLowHigh(entry->GetFileSize(), buffer->nFileSizeLow, buffer->nFileSizeHigh);
	LlongToDwLowHigh(0, buffer->nFileIndexLow, buffer->nFileIndexHigh);
	buffer->nNumberOfLinks = 1;
	buffer->dwVolumeSerialNumber = VolumeSerialNumber;

	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK FindFilesCallback(LPCWSTR filename, PFillFindData fillFindData, PDOKAN_FILE_INFO fileInfo)
{
	auto* container = GetContext(fileInfo);
	const auto folderContents = container->GetFolderList(filename);

	WIN32_FIND_DATAW findData{};
	for (const auto handle : folderContents) {
		const auto* entry = container->GetEntry(handle);
		if (entry == nullptr) {
			continue;
		}

		const std::wstring name(entry->GetFilename().begin(), entry->GetFilename().end());
		if (name.length() > MAX_PATH) {
			continue;
		}

		wcsncpy_s(findData.cFileName, name.c_str(), _TRUNCATE);

		DWORD attributes = FILE_ATTRIBUTE_READONLY;
		if (entry->IsDirectory()) {
			attributes |= FILE_ATTRIBUTE_DIRECTORY;
		}
		findData.dwFileAttributes = attributes;

		LlongToFileTime(container->GetVolumeModified(), findData.ftCreationTime);
		LlongToFileTime(container->GetVolumeModified(), findData.ftLastAccessTime);
		LlongToFileTime(container->GetVolumeModified(), findData.ftLastWriteTime);
		LlongToDwLowHigh(entry->GetFileSize(), findData.nFileSizeLow, findData.nFileSizeHigh);

		fillFindData(&findData, fileInfo);
	}

	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK GetDiskFreeSpaceCallback(PULONGLONG freeBytesAvailable, PULONGLONG totalNumberOfBytes,
	PULONGLONG totalNumberOfFreeBytes, PDOKAN_FILE_INFO fileInfo)
{
	auto* container = GetContext(fileInfo);
	*freeBytesAvailable = 0;
	*totalNumberOfBytes = container->GetVolumeSize();
	*totalNumberOfFreeBytes = 0;
	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK GetVolumeInformationCallback(LPWSTR volumeNameBuffer, DWORD volumeNameSize,
	LPDWORD volumeSerialNumber, LPDWORD maximumComponentLength, LPDWORD filesystemFlags,
	LPWSTR filesystemNameBuffer, DWORD filesystemNameSize, PDOKAN_FILE_INFO fileInfo)
{
	auto* container = GetContext(fileInfo);

	wcscpy_s(volumeNameBuffer, volumeNameSize, container->GetFilename().c_str());
	*volumeSerialNumber = VolumeSerialNumber;
	*maximumComponentLength = 255;
	*filesystemFlags = FILE_CASE_SENSITIVE_SEARCH | FILE_CASE_PRESERVED_NAMES |
		FILE_UNICODE_ON_DISK | FILE_READ_ONLY_VOLUME;
	wcscpy_s(filesystemNameBuffer, filesystemNameSize, L"Dokan XISO");

	return STATUS_SUCCESS;
}

void SetupOperations(DOKAN_OPERATIONS& operations)
{
	operations.ZwCreateFile = CreateFileCallback;
	operations.ReadFile = ReadFileCallback;
	operations.GetFileInformation = GetFileInformationCallback;
	operations.FindFiles = FindFilesCallback;
	operations.GetDiskFreeSpace = GetDiskFreeSpaceCallback;
	operations.GetVolumeInformation = GetVolumeInformationCallback;
}

std::string StatusToErrorMessage(int status)
{
	switch (status) {
	case DOKAN_SUCCESS:
		return "";
	case DOKAN_ERROR:
		return "Dokany reported an unknown error while mounting the XISO.";
	case DOKAN_DRIVE_LETTER_ERROR:
		return "The configured XISO mount drive letter is invalid.";
	case DOKAN_DRIVER_INSTALL_ERROR:
		return "Dokany is installed, but the Dokany driver is not available.";
	case DOKAN_START_ERROR:
		return "Dokany failed to start the mount.";
	case DOKAN_MOUNT_ERROR:
		return "Dokany failed to mount the XISO at the selected drive letter.";
	case DOKAN_MOUNT_POINT_ERROR:
		return "Dokany rejected the selected XISO mount point.";
	case DOKAN_VERSION_ERROR:
		return "The installed Dokany runtime is not compatible with this build.";
	default:
		return "Dokany returned an unexpected mount error.";
	}
}

} // namespace xiso_vfs

} // namespace

class XisoMount::Impl
{
public:
	bool Mount(const std::string& isoPath, const std::wstring& mountPoint, std::string& error)
	{
		Unmount();

		auto& api = GetDokanApi();
		if (!api.EnsureLoaded(error)) {
			return false;
		}

		auto container = std::make_unique<xiso_vfs::Container>();
		switch (container->Setup(std::filesystem::path(isoPath).wstring())) {
		case xiso_vfs::Container::SetupState::ErrorFile:
			error = "Could not open the selected XISO file.";
			return false;
		case xiso_vfs::Container::SetupState::ErrorFormat:
			error = "The selected file is not a supported Xbox XISO image.";
			return false;
		case xiso_vfs::Container::SetupState::Success:
			break;
		}

		const auto* defaultXbe = container->GetEntry(L"\\default.xbe");
		if (defaultXbe == nullptr || defaultXbe->IsDirectory()) {
			error = "The selected XISO does not contain \\default.xbe.";
			return false;
		}

		api.DokanInit()();
		m_dokanInitialized = true;
		m_mountPoint = mountPoint;

		m_options = {};
		m_options.Version = DOKAN_VERSION;
		m_options.SingleThread = FALSE;
		m_options.Timeout = 0;
		m_options.GlobalContext = reinterpret_cast<ULONG64>(container.get());
		m_options.MountPoint = m_mountPoint.c_str();
		m_options.Options |= DOKAN_OPTION_ALT_STREAM;
		m_options.Options |= DOKAN_OPTION_WRITE_PROTECT;
		m_options.Options |= DOKAN_OPTION_CURRENT_SESSION;

		m_operations = {};
		xiso_vfs::SetupOperations(m_operations);

		DOKAN_HANDLE instance = nullptr;
		const auto status = api.DokanCreateFileSystem()(&m_options, &m_operations, &instance);
		if (status != DOKAN_SUCCESS) {
			error = xiso_vfs::StatusToErrorMessage(status);
			api.DokanShutdown()();
			m_dokanInitialized = false;
			m_mountPoint.clear();
			m_options = {};
			m_operations = {};
			return false;
		}

		m_container = std::move(container);
		m_instance = instance;
		m_sourcePath = isoPath;
		m_mountedDefaultXbePath = (std::filesystem::path(m_mountPoint) / "default.xbe").string();
		return true;
	}

	void Unmount()
	{
		auto& api = GetDokanApi();

		if (m_instance != nullptr && api.DokanCloseHandle() != nullptr) {
			api.DokanCloseHandle()(m_instance);
			m_instance = nullptr;
		}

		if (m_dokanInitialized && api.DokanShutdown() != nullptr) {
			api.DokanShutdown()();
			m_dokanInitialized = false;
		}

		m_container.reset();
		m_mountPoint.clear();
		m_sourcePath.clear();
		m_mountedDefaultXbePath.clear();
		m_options = {};
		m_operations = {};
	}

	bool IsMounted() const
	{
		return m_instance != nullptr;
	}

	const std::string& GetMountedDefaultXbePath() const
	{
		return m_mountedDefaultXbePath;
	}

	const std::string& GetSourcePath() const
	{
		return m_sourcePath;
	}

	const std::wstring& GetMountPoint() const
	{
		return m_mountPoint;
	}

private:
	std::unique_ptr<xiso_vfs::Container> m_container;
	DOKAN_HANDLE m_instance{ nullptr };
	bool m_dokanInitialized{ false };
	DOKAN_OPTIONS m_options{};
	DOKAN_OPERATIONS m_operations{};
	std::wstring m_mountPoint;
	std::string m_sourcePath;
	std::string m_mountedDefaultXbePath;
};

XisoMount::XisoMount()
	: m_impl(std::make_unique<Impl>())
{
}

XisoMount::~XisoMount()
{
	m_impl->Unmount();
}

bool XisoMount::IsDokanAvailable(std::string& error)
{
	return GetDokanApi().EnsureLoaded(error);
}

bool XisoMount::Mount(const std::string& isoPath, const std::wstring& mountPoint, std::string& error)
{
	return m_impl->Mount(isoPath, mountPoint, error);
}

void XisoMount::Unmount()
{
	m_impl->Unmount();
}

bool XisoMount::IsMounted() const
{
	return m_impl->IsMounted();
}

const std::string& XisoMount::GetMountedDefaultXbePath() const
{
	return m_impl->GetMountedDefaultXbePath();
}

const std::string& XisoMount::GetSourcePath() const
{
	return m_impl->GetSourcePath();
}

const std::wstring& XisoMount::GetMountPoint() const
{
	return m_impl->GetMountPoint();
}
