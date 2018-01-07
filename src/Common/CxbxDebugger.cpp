// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Common->CxbxDebugger.cpp
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
#include "CxbxKrnl/EmuShared.h"
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
            HLECACHE_FILE   = 0x00deed00,
            KERNEL_PATCH    = 0x00deed01,
            FILE_OPENED     = 0x00deed02,
            FILE_READ       = 0x00deed03,
            FILE_CLOSED     = 0x00deed04,            
        };

        bool IsAttached()
        {
            // TODO: Avoid clashing with VS when child process debugging is enabled for all processes
            // TODO: Check the PID of the debugger? Ensure the PID is linked to CxbxDebugger.exe?
            return IsDebuggerPresent() == TRUE;
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

            void AddString(const char* szString, uint StringLength)
            {
                Add(STRING_CHAR);
                Add(StringLength);
                Add(szString);
            }

            void AddWString(const wchar_t* wszString, uint StringLength)
            {
                Add(STRING_WCHAR);
                Add(StringLength);
                Add(wszString);
            }
        };
    }

    bool IsDebuggerException(DWORD ExceptionCode)
    {
        switch (ExceptionCode)
        {
        case Internal::FILE_OPENED:
        case Internal::KERNEL_PATCH:
            return true;
        }

        return false;
    }

    bool CanReport()
    {
        return Internal::IsAttached();
    }

    void ReportHLECacheFile(const char* Filename, uint FilenameLength)
    {
        Internal::ReportHelper Report(Internal::HLECACHE_FILE);

        Report.AddString(Filename, FilenameLength);

        Report.Send();
    }

    void ReportKernelPatch(const char* ImportName, DWORD Address)
    {
        Internal::ReportHelper Report(Internal::KERNEL_PATCH);

        Report.AddString(ImportName, strlen(ImportName));
        Report.Add(Address);

        Report.Send();
    }

    void ReportFileOpened(HANDLE hFile, const wchar_t* Filename, uint FilenameLength)
    {
        Internal::ReportHelper Report(Internal::FILE_OPENED);

        Report.Add(hFile);
        Report.AddWString(Filename, FilenameLength);

        Report.Send();
    }

    void ReportFileRead(HANDLE hFile, uint Size)
    {
        Internal::ReportHelper Report(Internal::FILE_READ);

        Report.Add(hFile);
        Report.Add(Size);

        Report.Send();
    }

    void ReportFileClosed(HANDLE hFile)
    {
        Internal::ReportHelper Report(Internal::FILE_CLOSED);

        Report.Add(hFile);

        Report.Send();
    }
}
