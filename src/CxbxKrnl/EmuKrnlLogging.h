// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->EmuKrnlLogging.h
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
// *  (c) 2016 Patrick van Logchem <pvanlogchem@gmail.com>
// *
// *  All rights reserved
// *
// ******************************************************************
#ifndef _EMU_KERNEL_LOGGING_H
#define _EMU_KERNEL_LOGGING_H

#pragma once

// prevent name collisions
namespace xboxkrnl
{
#include <xboxkrnl/xboxkrnl.h>
};


#include <windows.h> // for PULONG
#include <sstream> // for std::ostream
//#include <iostream> // For std::cout
//#include <iomanip> // For std::setw

//?#include "Cxbx.h"
//#include "Logging.h"

// TODO : Implement renderers for each usage of these types (and/or (P/LP) pointers to them)...
// TODO : xboxkrnl::ANSI_STRING
// TODO : xboxkrnl::BUS_DATA_TYPE
// TODO : xboxkrnl::DEVICE_OBJECT
// TODO : xboxkrnl::DISPATCHER_HEADER
// TODO : xboxkrnl::DVDX2_AUTHENTICATION
// TODO : xboxkrnl::DVDX2_AUTHENTICATION_PAGE
// TODO : xboxkrnl::EEPROM_INDEX
// TODO : xboxkrnl::ERWLOCK
// TODO : xboxkrnl::ETHREAD
// TODO : xboxkrnl::EVENT_TYPE
// TODO : xboxkrnl::EXCEPTION_RECORD
// TODO : xboxkrnl::FILE_DIRECTORY_INFORMATION
// TODO : xboxkrnl::FILE_FS_SIZE_INFORMATION
// TODO : xboxkrnl::FILE_INFORMATION_CLASS
// TODO : xboxkrnl::FILETIME
// TODO : xboxkrnl::FS_INFORMATION_CLASS
// TODO : xboxkrnl::IO_STATUS_BLOCK / xboxkrnl::PIO_STATUS_BLOCK ->u1.Pointer, ->Information
// TODO : xboxkrnl::KDEVICE_QUEUE
// TODO : xboxkrnl::KDPC
// TODO : xboxkrnl::KEVENT
// TODO : xboxkrnl::KINTERRUPT
// TODO : xboxkrnl::KINTERRUPT_MODE
// TODO : xboxkrnl::KIRQL
// TODO : xboxkrnl::KOBJECTS
// TODO : xboxkrnl::KPCR
// TODO : xboxkrnl::KPRCB
// TODO : xboxkrnl::KPROCESSOR_MODE
// TODO : xboxkrnl::KSEMAPHORE
// TODO : xboxkrnl::KSPIN_LOCK
// TODO : xboxkrnl::KSYSTEM_TIME
// TODO : xboxkrnl::KTHREAD
// TODO : xboxkrnl::KTIMER /xboxkrnl::PKTIMER
// TODO : xboxkrnl::LAUNCH_DATA_HEADER
// TODO : xboxkrnl::LAUNCH_DATA_PAGE
// TODO : xboxkrnl::LIST_ENTRY
// TODO : xboxkrnl::LPCCH
// TODO : xboxkrnl::LPCSTR
// TODO : xboxkrnl::LPCWSTR
// TODO : xboxkrnl::MEMORY_BASIC_INFORMATION
// TODO : xboxkrnl::MM_STATISTICS
// TODO : xboxkrnl::MODE
// TODO : xboxkrnl::MODE_PARAMETER_HEADER10
// TODO : xboxkrnl::NT_TIB
// TODO : xboxkrnl::NTSTATUS
// TODO : xboxkrnl::OBJECT_ATTRIBUTES
// TODO : xboxkrnl::OBJECT_STRING
// TODO : xboxkrnl::OBJECT_TYPE
// TODO : xboxkrnl::PCI_COMMON_CONFIG
// TODO : xboxkrnl::PCI_SLOT_NUMBER
// TODO : xboxkrnl::PCHAR
// TODO : xboxkrnl::PKDEFERRED_ROUTINE
// TODO : xboxkrnl::PKDPC
// TODO : xboxkrnl::PKINTERRUPT
// TODO : xboxkrnl::PKSERVICE_ROUTINE
// TODO : xboxkrnl::PXDEVICE_PREALLOC_TYPE
// TODO : xboxkrnl::PXINPUT_CAPABILITIES
// TODO : xboxkrnl::PXINPUT_STATE
// TODO : xboxkrnl::PXPP_DEVICE_TYPE
// TODO : xboxkrnl::PXTHREAD_NOTIFICATION -> pfnNotifyRoutine
// TODO : xboxkrnl::RETURN_FIRMWARE
// TODO : xboxkrnl::RTL_CRITICAL_SECTION
// TODO : xboxkrnl::SCSI_PASS_THROUGH_DIRECT
// TODO : xboxkrnl::SINGLE_LIST_ENTRY
// TODO : xboxkrnl::SLIST_HEADER
// TODO : xboxkrnl::STRING
// TODO : xboxkrnl::TIMER_TYPE
// TODO : xboxkrnl::TIME_FIELDS
// TODO : xboxkrnl::UCHAR
// TODO : xboxkrnl::ULARGE_INTEGER
// TODO : xboxkrnl::WAIT_TYPE
// TODO : xboxkrnl::XBOX_HARDWARE_INFO
// TODO : xboxkrnl::XBOX_REFURB_INFO

std::ostream& operator<<(std::ostream& os, const PULONG& value);

std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::MM_STATISTICS& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::OBJECT_ATTRIBUTES& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::STRING& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::UNICODE_STRING& value);

std::ostream& operator<<(std::ostream& os, const xboxkrnl::PLARGE_INTEGER& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PMM_STATISTICS& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::POBJECT_ATTRIBUTES& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PSTRING& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PUNICODE_STRING& value);

#endif _EMU_KERNEL_LOGGING_H
