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

// std::ostream& operator<<(std::ostream& os, const PVOID*& value); // * value, *value
// TODO : KINTERRUPT_MODE
// TODO : KIRQL
// TODO : KPROCESSOR_MODE
// TODO : LPCSTR
// TODO : PIO_STATUS_BLOCK ->u1.Pointer, ->Information
// TODO : PKDEFERRED_ROUTINE
// TODO : PKDPC
// TODO : PKINTERRUPT
// TODO : PKSERVICE_ROUTINE
// TODO : PKTIMER
// TODO : PLARGE_INTEGER
// TODO : PUNICODE_STRING
// TODO : PXDEVICE_PREALLOC_TYPE
// TODO : PXINPUT_CAPABILITIES
// TODO : PXINPUT_STATE
// TODO : PXPP_DEVICE_TYPE
// TODO : PXTHREAD_NOTIFICATION -> pfnNotifyRoutine
// TODO : TIMER_TYPE
// TODO : UCHAR

std::ostream& operator<<(std::ostream& os, const PULONG& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::LARGE_INTEGER& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PLARGE_INTEGER& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PMM_STATISTICS& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::POBJECT_ATTRIBUTES& value);
std::ostream& operator<<(std::ostream& os, const xboxkrnl::PSTRING& value);

#endif _EMU_KERNEL_LOGGING_H
