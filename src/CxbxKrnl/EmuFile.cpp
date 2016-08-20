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
#include <ntstatus.h>
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

// Array of EmuHandles in the system
EmuHandle EmuHandle::Handles[EMU_MAX_HANDLES];

// Pointer to first free handle in array, or NULL if none
volatile EmuHandle *EmuHandle::FirstFree;

// Pointer to last free handle in array, or NULL if none
volatile EmuHandle *EmuHandle::LastFree;

// Lock on the handle system
CRITICAL_SECTION EmuHandle::HandleLock;

EmuNtSymbolicLinkObject* NtSymbolicLinkObjects[26];

struct XboxDevice {
	std::string XboxFullPath;
	std::string NativePath;
	HANDLE NativeRootHandle;
};
std::vector<XboxDevice> Devices;

bool CxbxRegisterDeviceNativePath(std::string XboxFullPath, std::string NativePath, bool IsFile)
{
	bool result;
	if (IsFile) {
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

// ******************************************************************
// * Initialize the handle database
// ******************************************************************
bool EmuHandle::Initialize()
{
    size_t x;

    // Initialize the critical section
    InitializeCriticalSection(&HandleLock);

    // Mark all handles as free.  We also set up the linked list of
    // free handles here.
    for (x = 0; x < EMU_MAX_HANDLES; x++)
    {
        Handles[x].m_Type = EMUHANDLE_TYPE_EMPTY;
        Handles[x].m_NextFree = &Handles[x + 1];
    }

    // The last entry should have a NULL next entry
    Handles[EMU_MAX_HANDLES - 1].m_NextFree = NULL;

    // Set up the head and tail pointers
    FirstFree = &Handles[0];
    LastFree = &Handles[EMU_MAX_HANDLES];

    return true;
}

// ******************************************************************
// * func: EmuHandle::Lock
// *    Locks the handle database
// ******************************************************************
inline void EmuHandle::Lock(void)
{
    EnterCriticalSection(&HandleLock);
}

// ******************************************************************
// * func: EmuHandle::Unlock
// *    Unlocks the handle database
// ******************************************************************
inline void EmuHandle::Unlock(void)
{
    LeaveCriticalSection(&HandleLock);
}

// ******************************************************************
// * func: EmuHandle::Allocate
// *    Allocates a new handle
// ******************************************************************
EmuHandle volatile *EmuHandle::Allocate(void)
{
    volatile EmuHandle *Handle;

    // Lock the database
    Lock();

    // Get the first free entry
    Handle = FirstFree;

    // Remove it from the list
    FirstFree = Handle->m_NextFree;

    // If it was the last handle, clear LastFree
    if (!Handle->m_NextFree)
        LastFree = NULL;

    // Initialize the handle's fields
    Handle->m_Type = EMUHANDLE_TYPE_ALLOCATED;
    Handle->m_Object = NULL;

    // Unlock the database
    Unlock();

    return Handle;
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
	if (DriveLetter > 'A' && DriveLetter < 'Z')
	{
		result = STATUS_OBJECT_NAME_COLLISION;
		if (FindNtSymbolicLinkObjectByVolumeLetter(DriveLetter) == NULL)
		{
			// Look up the partition in the list of pre-registered devices :
			result = STATUS_DEVICE_DOES_NOT_EXIST; // TODO : Is this the correct error?

 		   // Make a distinction between Xbox paths (starting with '\Device'...) and Native paths :
			std::string deviceString = "\\Device";
			IsNativePath = aFullPath.compare(0, deviceString.length(), deviceString) != 0;
			if (IsNativePath)
				DeviceIndex = 0;
			else
			{
				DeviceIndex = -1;
				for (int i = 0; i < Devices.size(); i++) {
					if (aFullPath.compare(0, Devices[i].XboxFullPath.length(), Devices[i].XboxFullPath) == 0)
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

void EmuNtSymbolicLinkObject::Free()
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
			return result;
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
		if ((result != NULL) && DeviceName.compare(0, result->XboxFullPath.length(), result->XboxFullPath) == 0)
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

NTSTATUS EmuNtObject::NtClose()
{
	NTSTATUS result = 0;
	RefCount--;
	if (RefCount <= 0)
		Free();
	result = STATUS_SUCCESS;
	return result;
}

EmuNtObject* EmuNtObject::NtDuplicateObject(DWORD Options)
{
	EmuNtObject* result = NULL;
	RefCount++;
	result = this;
	return result;
}