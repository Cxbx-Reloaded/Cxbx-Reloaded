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

#pragma once

namespace CxbxDebugger
{
	// Skip exception codes produced by the debugger (workaround for child process debugging)
	bool IsDebuggerException(DWORD ExceptionCode);

	// Check reporting is supported and enabled before calling a report helper
	bool CanReport();

	// Report helpers:

	void ReportDebuggerInit(const char* XbeTitle);

	void ReportHLECacheFile(const char* Filename);
	void ReportKernelPatch(const char* ImportName, DWORD Address);

	void ReportFileOpened(HANDLE hFile, const wchar_t* Filename, bool Success);
	void ReportFileRead(HANDLE hFile, unsigned int Size, uint64_t Offset);
	void ReportFileWrite(HANDLE hFile, unsigned int Size, uint64_t Offset);
	void ReportFileClosed(HANDLE hFile);

	// Exception helpers:

	void ReportAndHandleException(PEXCEPTION_RECORD Exception, bool& Handled);
}
