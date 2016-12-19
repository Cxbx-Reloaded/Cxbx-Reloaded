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

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "Emu.h"

extern std::string DriveSerial;
extern std::string DriveCdRom0;
extern std::string DriveMbfs;
extern std::string DriveMbcom;
extern std::string DriveMbrom;
extern std::string DriveC;
extern std::string DriveD;
extern std::string DriveE;
extern std::string DriveF;
extern std::string DriveT;
extern std::string DriveU;
extern std::string DriveV;
extern std::string DriveW;
extern std::string DriveX;
extern std::string DriveY;
extern std::string DriveZ;
extern std::string DeviceCdrom0;
extern std::string DeviceHarddisk0;
extern std::string DeviceHarddisk0Partition0;
extern std::string DeviceHarddisk0Partition1;
extern std::string DeviceHarddisk0Partition2;
extern std::string DeviceHarddisk0Partition3;
extern std::string DeviceHarddisk0Partition4;
extern std::string DeviceHarddisk0Partition5;
extern std::string DeviceHarddisk0Partition6;
extern std::string DeviceHarddisk0Partition7;
extern std::string DeviceHarddisk0Partition8;
extern std::string DeviceHarddisk0Partition9;
extern std::string DeviceHarddisk0Partition10;
extern std::string DeviceHarddisk0Partition11;
extern std::string DeviceHarddisk0Partition12;
extern std::string DeviceHarddisk0Partition13;
extern std::string DeviceHarddisk0Partition14;
extern std::string DeviceHarddisk0Partition15;
extern std::string DeviceHarddisk0Partition16;
extern std::string DeviceHarddisk0Partition17;
extern std::string DeviceHarddisk0Partition18;
extern std::string DeviceHarddisk0Partition19;
extern std::string DeviceHarddisk0Partition20;
extern char CxbxDefaultXbeVolumeLetter;

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

#endif
