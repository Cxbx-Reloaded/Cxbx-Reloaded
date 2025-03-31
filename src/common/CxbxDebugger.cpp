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

#include "Cxbx.h"
#include "EmuShared.h"
#include "CxbxDebugger.h"

// Note this implementation uses SEH. See the reference docs for RaiseException:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms680552(v=vs.85).aspx

namespace CxbxDebugger
{
	namespace Internal
	{
		// Note: Keep the top 3-bits empty as they are used internally
		enum ReportType : DWORD
		{
			HLECACHE_FILE = 0x1000,

			KERNEL_PATCH = 0x2000,

			FILE_OPENED = 0x3000,
			FILE_READ = 0x3001,
			FILE_WRITE = 0x3002,
			FILE_CLOSED = 0x3003,

			DEBUGGER_INIT = 0x4000,
			DEBUGGER_NEW_TARGET = 0x4001,

			OVERRIDE_EXCEPTION = 0x5000,
		};

		bool IsAttached()
		{
			bool IsDebugging;
			g_EmuShared->GetDebuggingFlag(&IsDebugging);

			return IsDebugging && IsDebuggerPresent() == TRUE;
		}

		class ReportHelper
		{
			ReportType ExceptionCode;

			ULONG_PTR ExceptionInfo[EXCEPTION_MAXIMUM_PARAMETERS];
			DWORD ExceptionInfoCount;

			ReportHelper() = delete;
			ReportHelper(const ReportHelper&) = delete;

			enum StringType : DWORD
			{
				STRING_CHAR,
				STRING_WCHAR,
			};

		public:
			ReportHelper(ReportType Code)
				: ExceptionCode(Code)
				, ExceptionInfoCount(0)
			{ }

			void Send()
			{
				if (ExceptionInfoCount > 0)
				{
					RaiseException(ExceptionCode, 0, ExceptionInfoCount, (ULONG_PTR*)ExceptionInfo);
				}
				else
				{
					RaiseException(ExceptionCode, 0, 0, nullptr);
				}
			}

			template<typename T>
			void Add(T Param)
			{
				if (ExceptionInfoCount + 1 < EXCEPTION_MAXIMUM_PARAMETERS)
				{
					ExceptionInfo[ExceptionInfoCount] = (ULONG_PTR)Param;
					++ExceptionInfoCount;
				}
			}

			void AddString(const char* szString)
			{
				Add(STRING_CHAR);
				Add(std::strlen(szString));
				Add(szString);
			}

			void AddWString(const wchar_t* wszString)
			{
				Add(STRING_WCHAR);
				Add(std::wcslen(wszString));
				Add(wszString);
			}
		};
	}

	bool IsDebuggerException(DWORD ExceptionCode)
	{
		switch (ExceptionCode)
		{
		case Internal::HLECACHE_FILE:
		case Internal::KERNEL_PATCH:
		case Internal::FILE_OPENED:
		case Internal::FILE_READ:
		case Internal::FILE_WRITE:
		case Internal::FILE_CLOSED:
		case Internal::DEBUGGER_INIT:
		case Internal::DEBUGGER_NEW_TARGET:
			return true;

		case Internal::OVERRIDE_EXCEPTION:
			return false;
		}

		return false;
	}

	bool CanReport()
	{
#ifdef BUILD_CXBXR_DEBUGGER
		return Internal::IsAttached();
#else
		return false;
#endif
	}

	void ReportDebuggerInit(const char* XbeTitle)
	{
		Internal::ReportHelper Report(Internal::DEBUGGER_INIT);

		Report.AddString(XbeTitle);

		Report.Send();
	}

	void ReportNewTarget(const char* CommandLine)
	{
		Internal::ReportHelper Report(Internal::DEBUGGER_NEW_TARGET);

		Report.AddString(CommandLine);

		Report.Send();
	}

	void ReportHLECacheFile(const char* Filename)
	{
		Internal::ReportHelper Report(Internal::HLECACHE_FILE);

		Report.AddString(Filename);

		Report.Send();
	}

	void ReportKernelPatch(const char* ImportName, DWORD Address)
	{
		Internal::ReportHelper Report(Internal::KERNEL_PATCH);

		Report.AddString(ImportName);
		Report.Add(Address);

		Report.Send();
	}

	void ReportFileOpened(HANDLE hFile, const wchar_t* Filename, bool Success)
	{
		Internal::ReportHelper Report(Internal::FILE_OPENED);

		Report.Add(hFile);
		Report.AddWString(Filename);
		Report.Add(Success);

		Report.Send();
	}

	void ReportFileRead(HANDLE hFile, unsigned int Size, uint64_t Offset)
	{
		Internal::ReportHelper Report(Internal::FILE_READ);

		Report.Add(hFile);
		Report.Add(Size);
		Report.Add(static_cast<unsigned int>(Offset));

		Report.Send();
	}

	void ReportFileWrite(HANDLE hFile, unsigned int Size, uint64_t Offset)
	{
		Internal::ReportHelper Report(Internal::FILE_WRITE);

		Report.Add(hFile);
		Report.Add(Size);
		Report.Add(static_cast<unsigned int>(Offset));

		Report.Send();
	}

	void ReportFileClosed(HANDLE hFile)
	{
		Internal::ReportHelper Report(Internal::FILE_CLOSED);

		Report.Add(hFile);

		Report.Send();
	}

	void ReportAndHandleException(PEXCEPTION_RECORD Exception, bool& Handled)
	{
		Internal::ReportHelper Report(Internal::OVERRIDE_EXCEPTION);

		Report.Add(&Handled);
		Report.Add(Exception->ExceptionAddress);
		Report.Add(Exception->ExceptionCode);
		Report.Add(Exception->NumberParameters);
		Report.Add(&Exception->ExceptionInformation[0]);

		Report.Send();
	}
}
