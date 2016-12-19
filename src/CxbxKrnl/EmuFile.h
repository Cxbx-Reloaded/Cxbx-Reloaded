// ******************************************************************
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

extern const std::string DrivePrefix;
extern const std::string DriveSerial;
extern const std::string DriveCdRom0;
extern const std::string DriveMbfs;
extern const std::string DriveMbcom;
extern const std::string DriveMbrom;
extern const std::string DriveC;
extern const std::string DriveD;
extern const std::string DriveE;
extern const std::string DriveF;
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
extern const char CxbxDefaultXbeVolumeLetter;

extern std::string CxbxBasePath;
extern HANDLE CxbxBasePathHandle;

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
	std::string XboxFullPath;
	std::string NativePath;
	HANDLE RootDirectoryHandle;
	NTSTATUS Init(std::string aSymbolicLinkName, std::string aFullPath);
	~EmuNtSymbolicLinkObject();
};

// ******************************************************************
// * is Handle a 'special' emulated handle?
// ******************************************************************
bool IsEmuHandle(HANDLE Handle);
EmuHandle* HandleToEmuHandle(HANDLE Handle);
HANDLE EmuHandleToHandle(EmuHandle* emuHandle);

CHAR* NtStatusToString(IN NTSTATUS Status);

char SymbolicLinkToDriveLetter(std::string aSymbolicLinkName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByVolumeLetter(const char VolumeLetter);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByName(std::string SymbolicLinkName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByDevice(std::string DeviceName);
EmuNtSymbolicLinkObject* FindNtSymbolicLinkObjectByRootHandle(HANDLE Handle);
void CleanupSymbolicLinks();
bool CxbxRegisterDeviceNativePath(std::string XboxFullPath, std::string NativePath, bool IsFile = false);
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
#define NTToXboxFileInformation(var, i, c, l)  SMART_PVOID(var, _NTToXboxFileInformation(i, c, l), i)
PVOID _NTToXboxFileInformation
(
	IN  PVOID nativeFileInformation,
	IN  ULONG FileInformationClass,
	OUT ULONG *Length
);
#endif
