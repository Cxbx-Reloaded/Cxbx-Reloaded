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
#include <Shlobj.h>
#include <Shlwapi.h>
#pragma warning(disable:4005) // Ignore redefined status values
#include <ntstatus.h>
#pragma warning(default:4005)
#include "CxbxKrnl.h"

std::string DriveSerial = "\\??\\serial:";
std::string DriveCdRom0 = "\\??\\CdRom0:"; // CD-ROM device
std::string DriveMbfs = "\\??\\mbfs:"; // media board's file system area device
std::string DriveMbcom = "\\??\\mbcom:"; // media board's communication area device
std::string DriveMbrom = "\\??\\mbrom:"; // media board's boot ROM device
std::string DriveC = "\\??\\C:"; // C: is HDD0
std::string DriveD = "\\??\\D:"; // D: is DVD Player
std::string DriveE = "\\??\\E:";
std::string DriveF = "\\??\\F:";
std::string DriveT = "\\??\\T:"; // T: is Title persistent data region
std::string DriveU = "\\??\\U:"; // U: is User persistent data region
std::string DriveV = "\\??\\V:";
std::string DriveW = "\\??\\W:";
std::string DriveX = "\\??\\X:";
std::string DriveY = "\\??\\Y:"; // Y: is Dashboard volume (contains "xboxdash.xbe" and "XDASH" folder + contents)
std::string DriveZ = "\\??\\Z:"; // Z: is Title utility data region
std::string DeviceCdrom0 = "\\Device\\CdRom0";
std::string DeviceHarddisk0 = "\\Device\\Harddisk0";
std::string DeviceHarddisk0Partition0 = "\\Device\\Harddisk0\\partition0"; // Contains raw config sectors (like XBOX_REFURB_INFO) + entire hard disk
std::string DeviceHarddisk0Partition1 = "\\Device\\Harddisk0\\partition1"; // Data partition. Contains TDATA and UDATA folders.
std::string DeviceHarddisk0Partition2 = "\\Device\\Harddisk0\\partition2"; // Shell partition. Contains Dashboard (cpxdash.xbe, evoxdash.xbe or xboxdash.xbe)
std::string DeviceHarddisk0Partition3 = "\\Device\\Harddisk0\\partition3"; // First cache partition. Contains cache data (from here up to largest number)
std::string DeviceHarddisk0Partition4 = "\\Device\\Harddisk0\\partition4";
std::string DeviceHarddisk0Partition5 = "\\Device\\Harddisk0\\partition5";
std::string DeviceHarddisk0Partition6 = "\\Device\\Harddisk0\\partition6";
std::string DeviceHarddisk0Partition7 = "\\Device\\Harddisk0\\partition7";
std::string DeviceHarddisk0Partition8 = "\\Device\\Harddisk0\\partition8";
std::string DeviceHarddisk0Partition9 = "\\Device\\Harddisk0\\partition9";
std::string DeviceHarddisk0Partition10 = "\\Device\\Harddisk0\\partition10";
std::string DeviceHarddisk0Partition11 = "\\Device\\Harddisk0\\partition11";
std::string DeviceHarddisk0Partition12 = "\\Device\\Harddisk0\\partition12";
std::string DeviceHarddisk0Partition13 = "\\Device\\Harddisk0\\partition13";
std::string DeviceHarddisk0Partition14 = "\\Device\\Harddisk0\\partition14";
std::string DeviceHarddisk0Partition15 = "\\Device\\Harddisk0\\partition15";
std::string DeviceHarddisk0Partition16 = "\\Device\\Harddisk0\\partition16";
std::string DeviceHarddisk0Partition17 = "\\Device\\Harddisk0\\partition17";
std::string DeviceHarddisk0Partition18 = "\\Device\\Harddisk0\\partition18";
std::string DeviceHarddisk0Partition19 = "\\Device\\Harddisk0\\partition19";
std::string DeviceHarddisk0Partition20 = "\\Device\\Harddisk0\\partition20"; // 20 = Largest possible partition number
char CxbxDefaultXbeVolumeLetter = 'C';
EmuNtSymbolicLinkObject* NtSymbolicLinkObjects[26];

struct XboxDevice {
	std::string XboxFullPath;
	std::string NativePath;
	HANDLE NativeRootHandle;
};
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
	RefCount--;

	if (RefCount <= 0) {
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

NTSTATUS CxbxObjectAttributesToNT(xboxkrnl::POBJECT_ATTRIBUTES ObjectAttributes, NativeObjectAttributes& nativeObjectAttributes, std::string aFileAPIName)
{
	NTSTATUS result = 0;
	std::string OriginalPath;
	std::string RelativePath;
	std::string XboxFullPath;
	std::string NativePath;
	EmuNtSymbolicLinkObject* NtSymbolicLinkObject = NULL;
	result = STATUS_SUCCESS;
	if (ObjectAttributes == NULL)
	{
		// When the pointer is nil, make sure we pass nil to Windows too :
		nativeObjectAttributes.NtObjAttrPtr = NULL;
		return result;
	}

	// ObjectAttributes are given, so make sure the pointer we're going to pass to Windows is assigned :
	nativeObjectAttributes.NtObjAttrPtr = &nativeObjectAttributes.NtObjAttr;

	RelativePath = std::string(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
	OriginalPath = RelativePath;

	// Always trim '\??\' off :
	if ((RelativePath.length() >= 4) && (RelativePath[0] == '\\') && (RelativePath[1] == '?') && (RelativePath[2] == '?') && (RelativePath[3] == '\\'))
		RelativePath.erase(0, 4);

	// Check if we where called from a File-handling API :
	if (!aFileAPIName.empty())
	{
		NtSymbolicLinkObject = NULL;
		// Check if the path starts with a volume indicator :
		if ((RelativePath.length() >= 2) && (RelativePath[1] == ':'))
		{
			// Look up the symbolic link information using the drive letter :
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByVolumeLetter(RelativePath[0]);
			RelativePath.erase(0, 2); // Remove 'C:'

			// If the remaining path starts with a ':', remove it (to prevent errors) :
			if ((RelativePath.length() > 0) && (RelativePath[0] == ':'))
				RelativePath.erase(0, 1);  // xbmp needs this, as it accesses 'e::\'
		}
		else if (RelativePath.compare(0, 1, "$") == 0)
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
		else if (RelativePath.compare(0, 1, ".") == 0) // "4x4 Evo 2" needs this
		{
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByRootHandle(g_hCurDir);
			RelativePath.erase(0, 1); // Remove the '.'
		}
		else
		{
			// The path seems to be a device path, look it up :
			NtSymbolicLinkObject = FindNtSymbolicLinkObjectByDevice(RelativePath);
			// Fixup RelativePath path here
			if ((NtSymbolicLinkObject != NULL))
				RelativePath.erase(0, NtSymbolicLinkObject->XboxFullPath.length()); // Remove '\Device\Harddisk0\Partition2'
			// else TODO : Turok requests 'gamedata.dat' without a preceding path, we probably need 'CurrentDir'-functionality
		}

		if ((NtSymbolicLinkObject != NULL))
		{
			NativePath = NtSymbolicLinkObject->NativePath;

			// If the remaining path starts with a '\', remove it (to prevent working in a native root) :
			if ((RelativePath.length() > 0) && (RelativePath[0] == '\\'))
			{
				RelativePath.erase(0, 1);
				// And if needed, add it to the Native path instead :
				if (NativePath.back() != '\\')
					NativePath.append(1, '\\');
			}

			XboxFullPath = NtSymbolicLinkObject->XboxFullPath;
			ObjectAttributes->RootDirectory = NtSymbolicLinkObject->RootDirectoryHandle;
		}
		else
		{
			// No symbolic link - as last resort, check if the path accesses a partition from Harddisk0 :
			if (_strnicmp(RelativePath.c_str(), (DeviceHarddisk0 + "\\partition").c_str(), (DeviceHarddisk0 + "\\partition").length()) != 0)
			{
				result = STATUS_UNRECOGNIZED_VOLUME; // TODO : Is this the correct error?
				EmuWarning((("Path not available : ") + OriginalPath).c_str());
				return result;
			}

			XboxFullPath = RelativePath;
			// Remove Harddisk0 prefix, in the hope that the remaining path might work :
			RelativePath.erase(0, DeviceHarddisk0.length() + 1);
			// And set Root to the folder containing the partition-folders :
			ObjectAttributes->RootDirectory = CxbxBasePathHandle;
			NativePath = CxbxBasePath;
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
		if (_strnicmp(NativePath.c_str(), CxbxBasePath.c_str(), CxbxBasePath.length()) == 0)
		{
			DbgPrintf("  New:\"$CxbxPath\\EmuDisk\\%s%s\"\n", (NativePath.substr(CxbxBasePath.length(), std::string::npos)).c_str(), RelativePath.c_str());
		}
		else
			DbgPrintf("  New:\"$XbePath\\%s\"\n", RelativePath.c_str());

	}
	else
	{
		// For non-file API calls, prefix with '\??\' again :
		RelativePath = "\\??\\" + RelativePath;
		ObjectAttributes->RootDirectory = 0;
	}

	// Convert Ansi to Unicode :
	mbstowcs(nativeObjectAttributes.wszObjectName, RelativePath.c_str(), 160);
	NtDll::RtlInitUnicodeString(&nativeObjectAttributes.NtUnicodeString, nativeObjectAttributes.wszObjectName);

	// Initialize the NT ObjectAttributes :
	InitializeObjectAttributes(&nativeObjectAttributes.NtObjAttr, &nativeObjectAttributes.NtUnicodeString, ObjectAttributes->Attributes, ObjectAttributes->RootDirectory, NULL);

	return result;
}

bool CxbxRegisterDeviceNativePath(std::string XboxFullPath, std::string NativePath, bool IsFile)
{
	bool result;
	if (IsFile) {
		if (!PathFileExists(NativePath.c_str())) {
			HANDLE hf = CreateFile(NativePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS,	0, 0);
			SetFilePointer(hf, 512 * 1024, 0, FILE_BEGIN);
			SetEndOfFile(hf);
			CloseHandle(hf);
		}

		return true; // Actually, this is the Config sectors partition (partition0) registered as a file
	}
	else
	{
		int status = SHCreateDirectoryEx(NULL, NativePath.c_str(), NULL);
		result = status == STATUS_SUCCESS || ERROR_ALREADY_EXISTS;
	}

	if (result)
	{
		XboxDevice newDevice;
		newDevice.XboxFullPath = XboxFullPath;
		newDevice.NativePath = NativePath;
		Devices.push_back(newDevice);
	}

	return result;
}


NTSTATUS CxbxCreateSymbolicLink(std::string SymbolicLinkName, std::string FullPath)
{
	NTSTATUS result = 0;
	EmuNtSymbolicLinkObject* SymbolicLinkObject = FindNtSymbolicLinkObjectByName(SymbolicLinkName);
	
	if ((SymbolicLinkObject != NULL)) {
		// In that case, close it :
		SymbolicLinkObject->NtClose();
	}

	// Now (re)create a symbolic link object, and initialize it with the new definition :
	SymbolicLinkObject = new EmuNtSymbolicLinkObject();
	result = SymbolicLinkObject->Init(SymbolicLinkName, FullPath);

	if (result != STATUS_SUCCESS)
		SymbolicLinkObject->NtClose();

	return result;
}


NTSTATUS EmuNtSymbolicLinkObject::Init(std::string aSymbolicLinkName, std::string aFullPath)
{
	NTSTATUS result = 0;
	bool IsNativePath = false;
	int i = 0;
	std::string ExtraPath;
	int DeviceIndex = 0;
	result = STATUS_OBJECT_NAME_INVALID;
	DriveLetter = SymbolicLinkToDriveLetter(aSymbolicLinkName);
	if (DriveLetter >= 'A' && DriveLetter <= 'Z')
	{
		result = STATUS_OBJECT_NAME_COLLISION;
		if (FindNtSymbolicLinkObjectByVolumeLetter(DriveLetter) == NULL)
		{
			// Look up the partition in the list of pre-registered devices :
			result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?

 		   // Make a distinction between Xbox paths (starting with '\Device'...) and Native paths :
			std::string deviceString = "\\Device";
			IsNativePath = _strnicmp(aFullPath.c_str(), deviceString.c_str(), deviceString.length()) != 0;
			if (IsNativePath)
				DeviceIndex = 0;
			else
			{
				DeviceIndex = -1;
				for (size_t i = 0; i < Devices.size(); i++) {
					if (_strnicmp(aFullPath.c_str(), Devices[i].XboxFullPath.c_str(), Devices[i].XboxFullPath.length()) == 0)
					{
						DeviceIndex = i;
						break;
					}
				}
			}
			if (DeviceIndex >= 0)
			{
				result = STATUS_SUCCESS;
				SymbolicLinkName = aSymbolicLinkName;
				XboxFullPath = aFullPath; // TODO : What path do we remember in IsNativePath mode?
				if (IsNativePath)
				{
					NativePath = "";
					ExtraPath = aFullPath;
				}
				else
				{
					NativePath = Devices[DeviceIndex].NativePath;
					// Handle the case where a sub folder of the partition is mounted (instead of it's root) :
					ExtraPath = aFullPath.substr(Devices[DeviceIndex].XboxFullPath.length(), std::string::npos);
				}
				if (!ExtraPath.empty())
					NativePath = NativePath + ExtraPath;

				SHCreateDirectoryEx(NULL, NativePath.c_str(), NULL);

				RootDirectoryHandle = CreateFile(NativePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				if (RootDirectoryHandle == INVALID_HANDLE_VALUE)
				{
					result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?
					CxbxKrnlCleanup((std::string("Could not map ") + NativePath).c_str());
				}
				else
				{
					NtSymbolicLinkObjects[DriveLetter - 'A'] = this;
					DbgPrintf("EmuMain : Linked \"%s\" to \"%s\" (residing at \"%s\")\n", aSymbolicLinkName.c_str(), aFullPath.c_str(), NativePath.c_str());
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
	Xbe::Certificate* pCertificate;
	std::string TitleStr;
	NTSTATUS status;
	
	// TODO -oDxbx : Select the oldest cache partition somehow.
	// For now, select partition6 as 'Utility data' drive, and link it to Z:
	pCertificate = (Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr;

	char szBuffer[255];
	sprintf(szBuffer, "%08X", ((Xbe::Certificate*)CxbxKrnl_XbeHeader->dwCertificateAddr)->dwTitleId);
	TitleStr = szBuffer;
	status = CxbxCreateSymbolicLink(DriveZ, DeviceHarddisk0Partition6 + "\\Cxbx_ZDATA_" + TitleStr + "\\");

	// Dxbx note : The ZDATA convention is not actually what the Xbox does, but for now
	// allows us to skip the partition-selection and formatting of the Utility drive,
	// by creating a unique subfolder per title.

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

EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByVolumeLetter(const char VolumeLetter)
{
	EmuNtSymbolicLinkObject* result = NULL;
	if (VolumeLetter >= 'A' && VolumeLetter <= 'Z')
		return NtSymbolicLinkObjects[VolumeLetter - 'A'];
		
	if (VolumeLetter >= 'a' && VolumeLetter <= 'z')
		return NtSymbolicLinkObjects[VolumeLetter - 'a'];

	return NULL;
}

EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByName(std::string SymbolicLinkName)
{
	return FindNtSymbolicLinkObjectByVolumeLetter(SymbolicLinkToDriveLetter(SymbolicLinkName));
}


EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDevice(std::string DeviceName)
{
	for (char VolumeLetter = 'A';  VolumeLetter <= 'Z'; VolumeLetter++)
	{
		EmuNtSymbolicLinkObject* result = NtSymbolicLinkObjects[VolumeLetter - 'A'];
		if ((result != NULL) && _strnicmp(DeviceName.c_str(), result->XboxFullPath.c_str(), result->XboxFullPath.length()) == 0)
			return result;
	}


	return NULL;
}


EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByRootHandle(const HANDLE Handle)
{
	for (char VolumeLetter = 'A'; VolumeLetter <= 'Z'; VolumeLetter++)
	{
		EmuNtSymbolicLinkObject* result = NtSymbolicLinkObjects[VolumeLetter - 'A'];
		result = NtSymbolicLinkObjects[VolumeLetter - 'A'];
		if ((result != NULL) && (Handle == result->RootDirectoryHandle))
			return result;
	}
	
	return NULL;
}
