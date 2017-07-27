// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuFile.cpp
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
#define _CXBXKRNL_INTERNAL
#define _XBOXKRNL_DEFEXTRN_

#include "EmuFile.h"
#include <vector>
#include <string>
#include <cassert>
#include <Shlobj.h>
#include <Shlwapi.h>
#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include "CxbxKrnl.h"
#include "EmuAlloc.h"
#include "MemoryManager.h"
//#include "Logging.h" // For hex4()


const std::string DrivePrefix = "\\??\\";
const std::string DriveSerial = DrivePrefix + "serial:";
const std::string DriveCdRom0 = DrivePrefix + "CdRom0:"; // CD-ROM device
const std::string DriveMbfs = DrivePrefix + "mbfs:"; // media board's file system area device
const std::string DriveMbcom = DrivePrefix + "mbcom:"; // media board's communication area device
const std::string DriveMbrom = DrivePrefix + "mbrom:"; // media board's boot ROM device
const std::string DriveA = DrivePrefix + "A:"; // A: could be CDROM
const std::string DriveC = DrivePrefix + "C:"; // C: is HDD0
const std::string DriveD = DrivePrefix + "D:"; // D: is DVD Player
const std::string DriveE = DrivePrefix + "E:";
const std::string DriveF = DrivePrefix + "F:";
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
const char CxbxDefaultXbeDriveLetter = 'D';

int CxbxDefaultXbeDriveIndex = -1;
EmuNtSymbolicLinkObject* NtSymbolicLinkObjects[26];
std::vector<XboxDevice> Devices;

EmuHandle::EmuHandle(EmuNtObject* ntObject)
{
	NtObject = ntObject;
}

NTSTATUS EmuHandle::NtClose()
{
	return NtObject->NtClose();
}

NTSTATUS EmuHandle::NtDuplicateObject(PHANDLE TargetHandle, DWORD Options)
{
	*TargetHandle = NtObject->NtDuplicateObject(Options)->NewHandle();
	return STATUS_SUCCESS;
}

EmuNtObject::EmuNtObject()
{
	RefCount = 1;
}

HANDLE EmuNtObject::NewHandle()
{
	RefCount++;
	return EmuHandleToHandle(new EmuHandle(this));
}

NTSTATUS EmuNtObject::NtClose()
{
	if (--RefCount <= 0) {
		delete this;
	}

	return STATUS_SUCCESS;
}

EmuNtObject* EmuNtObject::NtDuplicateObject(DWORD Options)
{
	RefCount++;
	return this;
}

bool IsEmuHandle(HANDLE Handle)
{
	return ((uint32)Handle > 0x80000000) && ((uint32)Handle < 0xFFFFFFFE);
}

EmuHandle* HandleToEmuHandle(HANDLE Handle)
{
	return (EmuHandle*)((uint32_t)Handle & 0x7FFFFFFF);
}

HANDLE EmuHandleToHandle(EmuHandle* emuHandle)
{
	return (HANDLE)((uint32_t)emuHandle | 0x80000000);
}

bool CxbxIsUtilityDrive(NtDll::HANDLE RootDirectory)
{
	EmuNtSymbolicLinkObject* SymbolicLinkObject = FindNtSymbolicLinkObjectByName(DriveZ);
	if (SymbolicLinkObject)
		return (SymbolicLinkObject->RootDirectoryHandle == RootDirectory);
	else
		return false;
}

std::wstring string_to_wstring(std::string const & src)
{
	std::wstring result = std::wstring(src.length(), L' ');
	std::copy(src.begin(), src.end(), result.begin());
	return result;
}

std::wstring PUNICODE_STRING_to_wstring(NtDll::PUNICODE_STRING const & src)
{
	return std::wstring(src->Buffer, src->Length / sizeof(NtDll::WCHAR));
}

std::string PSTRING_to_string(xboxkrnl::PSTRING const & src)
{
	return std::string(src->Buffer, src->Length);
}

void copy_string_to_PSTRING_to(std::string const & src, const xboxkrnl::PSTRING & dest)
{
	memcpy(dest->Buffer, src.c_str(), dest->Length);
}

NTSTATUS _CxbxConvertFilePath(
	std::string RelativeXboxPath, 
	OUT std::wstring &RelativeHostPath, 
	OUT NtDll::HANDLE *RootDirectory,
	std::string aFileAPIName)
{
	std::string OriginalPath = RelativeXboxPath;
	std::string RelativePath = RelativeXboxPath;
	std::string XboxFullPath;
	std::string HostPath;
	EmuNtSymbolicLinkObject* NtSymbolicLinkObject = NULL;
	
	// Always trim '\??\' off :
	if (RelativePath.compare(0, DrivePrefix.length(), DrivePrefix.c_str()) == 0)
		RelativePath.erase(0, 4);

	// Check if we where called from a File-handling API :
	if (!aFileAPIName.empty())
	{
		// Check if the path starts with a volume indicator :
		if ((RelativePath.length() >= 2) && (RelativePath[1] == ':'))
		{
			// Look up the symbolic link information using the drive letter :
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByDriveLetter(RelativePath[0]);
			RelativePath.erase(0, 2); // Remove 'C:'

			// If the remaining path starts with a ':', remove it (to prevent errors) :
			if ((RelativePath.length() > 0) && (RelativePath[0] == ':'))
				RelativePath.erase(0, 1);  // xbmp needs this, as it accesses 'e::\'
		}
		else if (RelativePath[0] == '$')
		{
			if (RelativePath.compare(0, 5, "$HOME") == 0) // "xbmp" needs this
			{
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir);
				RelativePath.erase(0, 5); // Remove '$HOME'
			}
			else
				CxbxKrnlCleanup(("Unsupported path macro : " + OriginalPath).c_str());
		}
		// Check if the path starts with a relative path indicator :
		else if (RelativePath[0] == '.') // "4x4 Evo 2" needs this
		{
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir);
			RelativePath.erase(0, 1); // Remove the '.'
		}
		else
		{
			// TODO : How should we handle accesses to the serial: (?semi-)volume?
			if (RelativePath.compare(0, 7, "serial:") == 0)
				return STATUS_UNRECOGNIZED_VOLUME;

			// The path seems to be a device path, look it up :
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByDevice(RelativePath);
			// Fixup RelativePath path here
			if (NtSymbolicLinkObject != NULL)
				RelativePath.erase(0, NtSymbolicLinkObject->XboxSymbolicLinkPath.length()); // Remove '\Device\Harddisk0\Partition2'
			// else TODO : Turok requests 'gamedata.dat' without a preceding path, we probably need 'CurrentDir'-functionality
		}
		if (NtSymbolicLinkObject == NULL) {
			// Check if the path accesses a partition from Harddisk0 :
			if (_strnicmp(RelativePath.c_str(), DeviceHarddisk0PartitionPrefix.c_str(), DeviceHarddisk0PartitionPrefix.length()) == 0)
			{
				XboxFullPath = RelativePath;
				// Remove Harddisk0 prefix, in the hope that the remaining path might work :
				RelativePath.erase(0, DeviceHarddisk0.length() + 1);
				// And set Root to the folder containing the partition-folders :
				*RootDirectory = CxbxBasePathHandle;
				HostPath = CxbxBasePath;
			} else {
				// Finally, Assume relative to Xbe path
				NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir);
			}
		}

		if (NtSymbolicLinkObject != NULL)
		{
			HostPath = NtSymbolicLinkObject->HostSymbolicLinkPath;

			// If the remaining path starts with a '\', remove it (to prevent working in a native root) :
			if ((RelativePath.length() > 0) && (RelativePath[0] == '\\'))
			{
				RelativePath.erase(0, 1);
				// And if needed, add it to the host path instead :
				if (HostPath.back() != '\\')
					HostPath.append(1, '\\');
			}

			XboxFullPath = NtSymbolicLinkObject->XboxSymbolicLinkPath;
			*RootDirectory = NtSymbolicLinkObject->RootDirectoryHandle;
		}

		// Check for special case : Partition0
		/* TODO : Translate this Dxbx code :
		if (StartsWithText(XboxFullPath, DeviceHarddisk0Partition0))
		{
		CxbxKrnlCleanup("Partition0 access not implemented yet! Tell PatrickvL what title triggers this.");
		// TODO : Redirect raw sector-access to the 'Partition0_ConfigData.bin' file
		// (This file probably needs to be pre-initialized somehow too).
		}
		*/

		DbgPrintf("EmuKrnl : %s Corrected path...\n", aFileAPIName.c_str());
		DbgPrintf("  Org:\"%s\"\n", OriginalPath.c_str());
		if (_strnicmp(HostPath.c_str(), CxbxBasePath.c_str(), CxbxBasePath.length()) == 0)
		{
			DbgPrintf("  New:\"$CxbxPath\\%s%s\"\n", (HostPath.substr(CxbxBasePath.length(), std::string::npos)).c_str(), RelativePath.c_str());
		}
		else
			DbgPrintf("  New:\"$XbePath\\%s\"\n", RelativePath.c_str());

	}
	else
	{
		// For non-file API calls, prefix with '\??\' again :
		RelativePath = DrivePrefix + RelativePath;
		*RootDirectory = 0;
	}

	// Convert the relative path to unicode
	RelativeHostPath = string_to_wstring(RelativePath);

	return STATUS_SUCCESS;
}

NTSTATUS CxbxObjectAttributesToNT(
	xboxkrnl::POBJECT_ATTRIBUTES ObjectAttributes, 
	OUT NativeObjectAttributes& nativeObjectAttributes, 
	const std::string aFileAPIName)
{
	if (ObjectAttributes == NULL)
	{
		// When the pointer is nil, make sure we pass nil to Windows too :
		nativeObjectAttributes.NtObjAttrPtr = nullptr;
		return STATUS_SUCCESS;
	}

	// Pick up the ObjectName, and let's see what to make of it :
	std::string ObjectName = PSTRING_to_string(ObjectAttributes->ObjectName);
	std::wstring RelativeHostPath;
	NtDll::HANDLE RootDirectory = ObjectAttributes->RootDirectory;

	// Handle special root directory constants
	if (RootDirectory == (NtDll::HANDLE)-4) { 
		RootDirectory = NULL;

		if (ObjectName.size() == 0){
			ObjectName = "\\BaseNamedObjects";
		} else {
			ObjectName = "\\BaseNamedObjects\\" + ObjectName;
		}
	}

	// Is there a filename API given?
	if (aFileAPIName.size() > 0)
	{
		// Then interpret the ObjectName as a filename, and update it to host relative :
		NTSTATUS result = _CxbxConvertFilePath(ObjectName, /*OUT*/RelativeHostPath, /*OUT*/&RootDirectory, aFileAPIName);
		if (FAILED(result))
			return result;
	}
	else
		// When not called from a file-handling API, just convert the ObjectName to a wide string :
		RelativeHostPath = string_to_wstring(ObjectName);

	// Copy the wide string to the unicode string
	wcscpy_s(nativeObjectAttributes.wszObjectName, RelativeHostPath.c_str());
	NtDll::RtlInitUnicodeString(&nativeObjectAttributes.NtUnicodeString, nativeObjectAttributes.wszObjectName);
	// And initialize the NT ObjectAttributes with that :
	InitializeObjectAttributes(&nativeObjectAttributes.NtObjAttr, &nativeObjectAttributes.NtUnicodeString, ObjectAttributes->Attributes, RootDirectory, NULL);
	// ObjectAttributes are given, so make sure the pointer we're going to pass to Windows is assigned :
	nativeObjectAttributes.NtObjAttrPtr = &nativeObjectAttributes.NtObjAttr;

	return STATUS_SUCCESS;
}

int CxbxDeviceIndexByDevicePath(const char *XboxDevicePath)
{
	for (size_t i = 0; i < Devices.size(); i++)
		if (_strnicmp(XboxDevicePath, Devices[i].XboxDevicePath.c_str(), Devices[i].XboxDevicePath.length()) == 0)
			return(i);

	return -1;
}

XboxDevice *CxbxDeviceByDevicePath(const std::string XboxDevicePath)
{
	int DeviceIndex = CxbxDeviceIndexByDevicePath(XboxDevicePath.c_str());
	if (DeviceIndex >= 0)
		return &Devices[DeviceIndex];

	return nullptr;
}

int CxbxRegisterDeviceHostPath(std::string XboxDevicePath, std::string HostDevicePath, bool IsFile)
{
	int result = -1;

	if (IsFile) {
		if (!PathFileExists(HostDevicePath.c_str())) {
			HANDLE hf = CreateFile(HostDevicePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS,	0, 0);

			SetFilePointer(hf, 512 * 1024, 0, FILE_BEGIN);
			SetEndOfFile(hf);
			CloseHandle(hf);
		}

		// Actually, this is the Config sectors partition (partition0) registered as a file
	}
	else
	{
		int status = SHCreateDirectoryEx(NULL, HostDevicePath.c_str(), NULL);
		if (status == STATUS_SUCCESS || status == ERROR_ALREADY_EXISTS)
		{
			XboxDevice newDevice;

			newDevice.XboxDevicePath = XboxDevicePath;
			newDevice.HostDevicePath = HostDevicePath;
			Devices.push_back(newDevice);
			result = Devices.size() - 1;
		}
	}

	return result;
}


NTSTATUS CxbxCreateSymbolicLink(std::string SymbolicLinkName, std::string FullPath)
{
	NTSTATUS result = 0;
	EmuNtSymbolicLinkObject* SymbolicLinkObject = FindNtSymbolicLinkObjectByName(SymbolicLinkName);
	
	if (SymbolicLinkObject != NULL)
		// In that case, close it (will also delete if reference count drops to zero)
		SymbolicLinkObject->NtClose();

	// Now (re)create a symbolic link object, and initialize it with the new definition :
	SymbolicLinkObject = new EmuNtSymbolicLinkObject();
	result = SymbolicLinkObject->Init(SymbolicLinkName, FullPath);

	if (result != STATUS_SUCCESS)
		SymbolicLinkObject->NtClose();

	return result;
}


NTSTATUS EmuNtSymbolicLinkObject::Init(std::string aSymbolicLinkName, std::string aFullPath)
{
	NTSTATUS result = STATUS_OBJECT_NAME_INVALID;
	int i = 0;
	int DeviceIndex = 0;

	DriveLetter = SymbolicLinkToDriveLetter(aSymbolicLinkName);
	if (DriveLetter >= 'A' && DriveLetter <= 'Z')
	{
		result = STATUS_OBJECT_NAME_COLLISION;
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
			if (IsHostBasedPath)
				DeviceIndex = CxbxDefaultXbeDriveIndex;
			else
				DeviceIndex = CxbxDeviceIndexByDevicePath(aFullPath.c_str());

			if (DeviceIndex >= 0)
			{
				result = STATUS_SUCCESS;
				SymbolicLinkName = aSymbolicLinkName;
				if (IsHostBasedPath)
				{
					XboxSymbolicLinkPath = "";
					HostSymbolicLinkPath = aFullPath;
				}
				else
				{
					XboxSymbolicLinkPath = aFullPath;
					HostSymbolicLinkPath = Devices[DeviceIndex].HostDevicePath;
					// Handle the case where a sub folder of the partition is mounted (instead of it's root) :
					std::string ExtraPath = aFullPath.substr(Devices[DeviceIndex].XboxDevicePath.length(), std::string::npos);

					if (!ExtraPath.empty())
						HostSymbolicLinkPath = HostSymbolicLinkPath + ExtraPath;
				}

				SHCreateDirectoryEx(NULL, HostSymbolicLinkPath.c_str(), NULL);
				RootDirectoryHandle = CreateFile(HostSymbolicLinkPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				if (RootDirectoryHandle == INVALID_HANDLE_VALUE)
				{
					result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?
					CxbxKrnlCleanup((std::string("Could not map ") + HostSymbolicLinkPath).c_str());
				}
				else
				{
					NtSymbolicLinkObjects[DriveLetter - 'A'] = this;
					DbgPrintf("EmuMain : Linked \"%s\" to \"%s\" (residing at \"%s\")\n", aSymbolicLinkName.c_str(), aFullPath.c_str(), HostSymbolicLinkPath.c_str());
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
	}
}

bool CxbxMountUtilityDrive(bool formatClean)
{
	NTSTATUS status;
	
	// TODO -oDxbx : Select the oldest cache partition somehow.
	// For now, select partition6 as 'Utility data' drive, and link it to Z:
	status = CxbxCreateSymbolicLink(DriveZ, DeviceHarddisk0Partition6);

	// TODO -oDxbx : Implement 'formatting' (cleaning) of the Utility drive
	
	return status == STATUS_SUCCESS;
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


EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDevice(std::string DeviceName)
{
	for (char DriveLetter = 'A';  DriveLetter <= 'Z'; DriveLetter++)
	{
		EmuNtSymbolicLinkObject* result = NtSymbolicLinkObjects[DriveLetter - 'A'];
		if ((result != NULL) && _strnicmp(DeviceName.c_str(), result->XboxSymbolicLinkPath.c_str(), result->XboxSymbolicLinkPath.length()) == 0)
			return result;
	}

	return NULL;
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
	if (*ptr)
		g_MemoryManager.Free(*ptr);
}

// ----------------------------------------------------------------------------
// Xbox to NT converters
// ----------------------------------------------------------------------------

NtDll::FILE_LINK_INFORMATION * _XboxToNTLinkInfo(xboxkrnl::FILE_LINK_INFORMATION *xboxLinkInfo, ULONG *Length)
{
	// Convert the path from Xbox to native
	std::string originalFileName(xboxLinkInfo->FileName, xboxLinkInfo->FileNameLength);
	std::wstring convertedFileName;
	NtDll::HANDLE RootDirectory;
	NTSTATUS res = _CxbxConvertFilePath(originalFileName, /*OUT*/convertedFileName, /*OUT*/&RootDirectory, "NtSetInformationFile");
	// TODO : handle if(FAILED(res))

	// Build the native FILE_LINK_INFORMATION struct
	*Length = sizeof(NtDll::FILE_LINK_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
	NtDll::FILE_LINK_INFORMATION *ntLinkInfo = (NtDll::FILE_LINK_INFORMATION *) g_MemoryManager.AllocateZeroed(1, *Length);
	ntLinkInfo->ReplaceIfExists = xboxLinkInfo->ReplaceIfExists;
	ntLinkInfo->RootDirectory = RootDirectory;
	ntLinkInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
	wmemcpy_s(ntLinkInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());
	
	return ntLinkInfo;
}

NtDll::FILE_RENAME_INFORMATION * _XboxToNTRenameInfo(xboxkrnl::FILE_RENAME_INFORMATION *xboxRenameInfo, ULONG *Length)
{
	// Convert the path from Xbox to native
	std::string originalFileName(xboxRenameInfo->FileName.Buffer, xboxRenameInfo->FileName.Length);
	std::wstring convertedFileName;
	NtDll::HANDLE RootDirectory;
	NTSTATUS res = _CxbxConvertFilePath(originalFileName, /*OUT*/convertedFileName, /*OUT*/&RootDirectory, "NtSetInformationFile");
	// TODO : handle if(FAILED(res))

	// Build the native FILE_RENAME_INFORMATION struct
	*Length = sizeof(NtDll::FILE_RENAME_INFORMATION) + convertedFileName.size() * sizeof(wchar_t);
	NtDll::FILE_RENAME_INFORMATION *ntRenameInfo = (NtDll::FILE_RENAME_INFORMATION *) g_MemoryManager.AllocateZeroed(1, *Length);
	ntRenameInfo->ReplaceIfExists = xboxRenameInfo->ReplaceIfExists;
	ntRenameInfo->RootDirectory = RootDirectory;
	ntRenameInfo->FileNameLength = convertedFileName.size() * sizeof(wchar_t);
	wmemcpy_s(ntRenameInfo->FileName, convertedFileName.size(), convertedFileName.c_str(), convertedFileName.size());

	return ntRenameInfo;
}

// ----------------------------------------------------------------------------
// NT to Xbox converters - common functions
// ----------------------------------------------------------------------------

void _ConvertXboxBasicInfo(xboxkrnl::FILE_BASIC_INFORMATION *xboxBasicInfo)
{
	// Fix up attributes
	xboxBasicInfo->FileAttributes &= FILE_ATTRIBUTE_VALID_FLAGS;
}

NTSTATUS _ConvertXboxNameInfo(NtDll::FILE_NAME_INFORMATION *ntNameInfo, xboxkrnl::FILE_NAME_INFORMATION *xboxNameInfo, int convertedFileNameLength, ULONG Length)
{
	// Convert the file name to ANSI in-place
	xboxNameInfo->FileNameLength = convertedFileNameLength;

	// Check if the new file name fits within the given struct size and copy as many chars as possible if not
	int maxFileNameLength = Length - sizeof(xboxkrnl::FILE_NAME_INFORMATION);
	size_t convertedChars;
	wcstombs_s(&convertedChars, xboxNameInfo->FileName, maxFileNameLength, ntNameInfo->FileName, ntNameInfo->FileNameLength);

	// Return the appropriate result depending on whether the string was fully copied
	return convertedChars == ntNameInfo->FileNameLength / sizeof(NtDll::WCHAR)
		? STATUS_SUCCESS
		: STATUS_BUFFER_OVERFLOW;
}

// ----------------------------------------------------------------------------
// NT to Xbox converters
// ----------------------------------------------------------------------------

NTSTATUS _NTToXboxNetOpenInfo(NtDll::FILE_NETWORK_OPEN_INFORMATION *ntNetOpenInfo, xboxkrnl::FILE_NETWORK_OPEN_INFORMATION *xboxNetOpenInfo, ULONG Length)
{
	// Copy everything from the NT struct
	memcpy_s(xboxNetOpenInfo, Length, ntNetOpenInfo, sizeof(NtDll::FILE_NETWORK_OPEN_INFORMATION));

	// Fix up attributes
	xboxNetOpenInfo->FileAttributes &= FILE_ATTRIBUTE_VALID_FLAGS;

	return STATUS_SUCCESS;
}

NTSTATUS _NTToXboxBasicInfo(NtDll::FILE_BASIC_INFORMATION *ntBasicInfo, xboxkrnl::FILE_BASIC_INFORMATION *xboxBasicInfo, ULONG Length)
{
	// Copy everything from the NT struct
	memcpy_s(xboxBasicInfo, sizeof(xboxkrnl::FILE_BASIC_INFORMATION), ntBasicInfo, sizeof(NtDll::FILE_BASIC_INFORMATION));

	_ConvertXboxBasicInfo(xboxBasicInfo);

	return STATUS_SUCCESS;
}

NTSTATUS _NTToXboxNameInfo(NtDll::FILE_NAME_INFORMATION *ntNameInfo, xboxkrnl::FILE_NAME_INFORMATION *xboxNameInfo, ULONG Length)
{
	// TODO: the FileName probably needs to be converted back to an Xbox path in some cases
	// Determine new file name length
	size_t convertedFileNameLength = ntNameInfo->FileNameLength / sizeof(NtDll::WCHAR);

	// Clean up the Xbox FILE_NAME_INFORMATION struct
	ZeroMemory(xboxNameInfo, Length);

	// Convert file name and return the result
	return _ConvertXboxNameInfo(ntNameInfo, xboxNameInfo, convertedFileNameLength, Length);
}

NTSTATUS _NTToXboxAllInfo(NtDll::FILE_ALL_INFORMATION *ntAllInfo, xboxkrnl::FILE_ALL_INFORMATION *xboxAllInfo, ULONG Length)
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
		case xboxkrnl::FileLinkInformation:
		{
			xboxkrnl::FILE_LINK_INFORMATION *xboxLinkInfo = reinterpret_cast<xboxkrnl::FILE_LINK_INFORMATION *>(xboxFileInformation);
			result = _XboxToNTLinkInfo(xboxLinkInfo, Length);
			break;
		}
		case xboxkrnl::FileRenameInformation:
		{
			xboxkrnl::FILE_RENAME_INFORMATION *xboxRenameInfo = reinterpret_cast<xboxkrnl::FILE_RENAME_INFORMATION *>(xboxFileInformation);
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

	NTSTATUS result = STATUS_SUCCESS;

	switch (FileInformationClass)
	{
		case NtDll::FileAllInformation:
		{
			NtDll::FILE_ALL_INFORMATION *ntAllInfo = reinterpret_cast<NtDll::FILE_ALL_INFORMATION *>(nativeFileInformation);
			xboxkrnl::FILE_ALL_INFORMATION *xboxAllInfo = reinterpret_cast<xboxkrnl::FILE_ALL_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxAllInfo(ntAllInfo, xboxAllInfo, Length);
			break;
		}
		case NtDll::FileBasicInformation:
		{
			NtDll::FILE_BASIC_INFORMATION *ntBasicInfo = reinterpret_cast<NtDll::FILE_BASIC_INFORMATION *>(nativeFileInformation);
			xboxkrnl::FILE_BASIC_INFORMATION *xboxBasicInfo = reinterpret_cast<xboxkrnl::FILE_BASIC_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxBasicInfo(ntBasicInfo, xboxBasicInfo, Length);
			break;
		}
		case NtDll::FileNameInformation:
		{
			NtDll::FILE_NAME_INFORMATION *ntNameInfo = reinterpret_cast<NtDll::FILE_NAME_INFORMATION *>(nativeFileInformation);
			xboxkrnl::FILE_NAME_INFORMATION *xboxNameInfo = reinterpret_cast<xboxkrnl::FILE_NAME_INFORMATION *>(xboxFileInformation);
			result = _NTToXboxNameInfo(ntNameInfo, xboxNameInfo, Length);
			break;
		}
		case NtDll::FileNetworkOpenInformation:
		{
			NtDll::FILE_NETWORK_OPEN_INFORMATION *ntNetOpenInfo = reinterpret_cast<NtDll::FILE_NETWORK_OPEN_INFORMATION *>(nativeFileInformation);
			xboxkrnl::FILE_NETWORK_OPEN_INFORMATION *xboxNetOpenInfo = reinterpret_cast<xboxkrnl::FILE_NETWORK_OPEN_INFORMATION *>(xboxFileInformation);
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
			result = STATUS_SUCCESS;
			break;
		}
	}

	return result;
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
		_CASE(STATUS_ALERTED);
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
		_CASE(STATUS_BUFFER_OVERFLOW);
		_CASE(STATUS_BUFFER_TOO_SMALL);
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
		_CASE(STATUS_CONFLICTING_ADDRESSES);
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
		_CASE(STATUS_FREE_VM_NOT_AT_BASE);
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
		_CASE(STATUS_INSUFFICIENT_RESOURCES);
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
		_CASE(STATUS_INVALID_IMAGE_FORMAT);
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
		_CASE(STATUS_INVALID_PAGE_PROTECTION);
		_CASE(STATUS_INVALID_PARAMETER);
		_CASE(STATUS_INVALID_PARAMETER_1);
		_CASE(STATUS_INVALID_PARAMETER_10);
		_CASE(STATUS_INVALID_PARAMETER_11);
		_CASE(STATUS_INVALID_PARAMETER_12);
		_CASE(STATUS_INVALID_PARAMETER_2);
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
		_CASE(STATUS_MEMORY_NOT_ALLOCATED);
		_CASE(STATUS_MESSAGE_NOT_FOUND);
		_CASE(STATUS_MISSING_SYSTEMFILE);
		_CASE(STATUS_MORE_ENTRIES);
		_CASE(STATUS_MORE_PROCESSING_REQUIRED);
		_CASE(STATUS_MP_PROCESSOR_MISMATCH);
		_CASE(STATUS_MULTIPLE_FAULT_VIOLATION);
		_CASE(STATUS_MUTANT_LIMIT_EXCEEDED);
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
		_CASE(STATUS_NOT_COMMITTED);
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
		_CASE(STATUS_NO_MEMORY);
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
		_CASE(STATUS_OBJECT_NAME_COLLISION);
		_CASE(STATUS_OBJECT_NAME_EXISTS);
		_CASE(STATUS_OBJECT_NAME_INVALID);
		_CASE(STATUS_OBJECT_NAME_NOT_FOUND);
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
		_CASE(STATUS_PENDING);
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
		_CASE(STATUS_SEMAPHORE_LIMIT_EXCEEDED);
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
		_CASE(STATUS_SUCCESS);
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
		_CASE(STATUS_TIMER_RESUME_IGNORED);
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
		_CASE(STATUS_TOO_MANY_SECRETS);
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
		_CASE(STATUS_UNABLE_TO_FREE_VM);
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
		_CASE(STATUS_UNRECOGNIZED_MEDIA);
		_CASE(STATUS_UNRECOGNIZED_VOLUME);
		_CASE(STATUS_UNSUCCESSFUL);
		_CASE(STATUS_UNSUPPORTED_COMPRESSION);
		_CASE(STATUS_UNWIND);
		_CASE(STATUS_USER_APC);
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
