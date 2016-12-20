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

// Additional types, exclusively for logging :
enum  CREATE_DISPOSITION;
enum  CREATE_OPTION;
};


#include <windows.h> // for PULONG
#include <sstream> // for std::ostream
//?#include "Logging.h"

// Headers for rendering non-Xbox types :
std::ostream& operator<<(std::ostream& os, const PULONG& value);

// Macro to ease declaration of a render function per Xbox Type:
#define LOGRENDER_HEADER(Type) std::ostream& operator<<(std::ostream& os, const xboxkrnl::Type& value)

// Headers for rendering Xbox enum types :

LOGRENDER_HEADER(BUS_DATA_TYPE);
LOGRENDER_HEADER(CREATE_DISPOSITION); // Not really an enum
LOGRENDER_HEADER(CREATE_OPTION); // Not really an enum
LOGRENDER_HEADER(EEPROM_INDEX);
LOGRENDER_HEADER(EVENT_TYPE);
LOGRENDER_HEADER(FILE_INFORMATION_CLASS);
LOGRENDER_HEADER(FS_INFORMATION_CLASS);
LOGRENDER_HEADER(KINTERRUPT_MODE);
LOGRENDER_HEADER(KWAIT_REASON);
LOGRENDER_HEADER(KOBJECTS);
LOGRENDER_HEADER(MODE);
//LOGRENDER_HEADER(NTSTATUS); // Not really an enum
LOGRENDER_HEADER(RETURN_FIRMWARE);
LOGRENDER_HEADER(TIMER_TYPE);
LOGRENDER_HEADER(WAIT_TYPE);

// Headers for rendering Xbox types without pointer-to-type :
LOGRENDER_HEADER(BOOLEAN);

// Macro to ease declaration of two render functions, for Xbox type and pointer-to-type :
#define LOGRENDER_HEADERS(Type) LOGRENDER_HEADER(Type); LOGRENDER_HEADER(P##Type)

// Headers for rendering functions of Xbox type and pointer-to-type :

// TODO : Implement renderers for each usage of these types (and/or (P/LP) pointers to them)...
//
// TODO : LOGRENDER_HEADER(DEVICE_OBJECT);
// TODO : LOGRENDER_HEADER(DISPATCHER_HEADER);
// TODO : LOGRENDER_HEADER(DVDX2_AUTHENTICATION);
// TODO : LOGRENDER_HEADER(DVDX2_AUTHENTICATION_PAGE);
// TODO : LOGRENDER_HEADER(ERWLOCK);
// TODO : LOGRENDER_HEADER(ETHREAD);
// TODO : LOGRENDER_HEADER(EXCEPTION_RECORD);
LOGRENDER_HEADERS(FILETIME);
// TODO : LOGRENDER_HEADER(FILE_DIRECTORY_INFORMATION);
// TODO : LOGRENDER_HEADER(FILE_FS_SIZE_INFORMATION);
// TODO : LOGRENDER_HEADER(IO_STATUS_BLOCK); // LOGRENDER_HEADER(PIO_STATUS_BLOCK ->u1.Pointer, ->Information
// TODO : LOGRENDER_HEADER(KDEVICE_QUEUE);
// TODO : LOGRENDER_HEADER(KDPC);
// TODO : LOGRENDER_HEADER(KEVENT);
// TODO : LOGRENDER_HEADER(KINTERRUPT);
// TODO : LOGRENDER_HEADER(KIRQL);
// TODO : LOGRENDER_HEADER(KPCR);
// TODO : LOGRENDER_HEADER(KPRCB);
// TODO : LOGRENDER_HEADER(KPROCESSOR_MODE);
// TODO : LOGRENDER_HEADER(KSEMAPHORE);
// TODO : LOGRENDER_HEADER(KSPIN_LOCK);
// TODO : LOGRENDER_HEADER(KSYSTEM_TIME);
// TODO : LOGRENDER_HEADER(KTHREAD);
// TODO : LOGRENDER_HEADER(KTIMER);
LOGRENDER_HEADERS(LARGE_INTEGER);
// TODO : LOGRENDER_HEADER(LAUNCH_DATA_HEADER);
// TODO : LOGRENDER_HEADER(LAUNCH_DATA_PAGE);
// TODO : LOGRENDER_HEADER(LIST_ENTRY);
// TODO : LOGRENDER_HEADER(LPCCH);
// TODO : LOGRENDER_HEADER(LPCSTR);
// TODO : LOGRENDER_HEADER(LPCWSTR);
// TODO : LOGRENDER_HEADER(MEMORY_BASIC_INFORMATION);
LOGRENDER_HEADERS(MM_STATISTICS);
// TODO : LOGRENDER_HEADER(MODE_PARAMETER_HEADER10);
// TODO : LOGRENDER_HEADER(NT_TIB);
// TODO : LOGRENDER_HEADER(NTSTATUS);
LOGRENDER_HEADERS(OBJECT_ATTRIBUTES);
// TODO : LOGRENDER_HEADER(OBJECT_STRING);
// TODO : LOGRENDER_HEADER(OBJECT_TYPE);
// TODO : LOGRENDER_HEADER(PCI_COMMON_CONFIG);
// TODO : LOGRENDER_HEADER(PCI_SLOT_NUMBER);
// TODO : LOGRENDER_HEADER(PCHAR);
// TODO : LOGRENDER_HEADER(PKDEFERRED_ROUTINE);
// TODO : LOGRENDER_HEADER(PKDPC);
// TODO : LOGRENDER_HEADER(PKINTERRUPT);
// TODO : LOGRENDER_HEADER(PKSERVICE_ROUTINE);
// TODO : LOGRENDER_HEADER(PXDEVICE_PREALLOC_TYPE);
// TODO : LOGRENDER_HEADER(PXINPUT_CAPABILITIES);
// TODO : LOGRENDER_HEADER(PXINPUT_STATE);
// TODO : LOGRENDER_HEADER(PXPP_DEVICE_TYPE);
// TODO : LOGRENDER_HEADER(PXTHREAD_NOTIFICATION); // -> pfnNotifyRoutine
// TODO : LOGRENDER_HEADER(RTL_CRITICAL_SECTION);
// TODO : LOGRENDER_HEADER(SCSI_PASS_THROUGH_DIRECT);
// TODO : LOGRENDER_HEADER(SINGLE_LIST_ENTRY);
// TODO : LOGRENDER_HEADER(SLIST_HEADER);
LOGRENDER_HEADERS(STRING); // Alias ANSI_STRING
// TODO : LOGRENDER_HEADER(TIME_FIELDS);
// TODO : LOGRENDER_HEADER(UCHAR);
// TODO : LOGRENDER_HEADER(ULARGE_INTEGER);
LOGRENDER_HEADERS(UNICODE_STRING);
// TODO : LOGRENDER_HEADER(XBOX_HARDWARE_INFO);
// TODO : LOGRENDER_HEADER(XBOX_REFURB_INFO);

#undef LOGRENDER_HEADERS

// Don't #undef LOGRENDER_HEADER, because it's used in EmuKrnlLogging.cpp

#endif _EMU_KERNEL_LOGGING_H
