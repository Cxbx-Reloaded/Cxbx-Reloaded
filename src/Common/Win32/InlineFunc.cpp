// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Common->Win32->InlineFunc.cpp
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
// *  (c) 2018 RadWolfie
// *
// *  All rights reserved
// *
// ******************************************************************

#if defined(_WIN32) || defined(WIN32)

#include "..\..\CxbxKrnl\CxbxKrnl.h"
#include <WinSafer.h> // For the Safer API functions
#include <sddl.h> // For ConvertStringSidToSid

// Source: https://stackoverflow.com/questions/8046097/how-to-check-if-a-process-has-the-administrative-rights
bool CxbxIsElevated() {
	bool fRet = false;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated > 0;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

// Source: https://stackoverflow.com/questions/16099787/removing-administrator-privilages-from-process
bool IsNewProcessLaunched()
{
	// Create the restricted token.

	SAFER_LEVEL_HANDLE hLevel = NULL;
	if (!SaferCreateLevel(SAFER_SCOPEID_USER, SAFER_LEVELID_NORMALUSER, SAFER_LEVEL_OPEN, &hLevel, NULL))
	{
		return false;
	}

	HANDLE hRestrictedToken = NULL;
	if (!SaferComputeTokenFromLevel(hLevel, NULL, &hRestrictedToken, 0, NULL))
	{
		SaferCloseLevel(hLevel);
		return false;
	}

	SaferCloseLevel(hLevel);

	// Set the token to medium integrity.

	TOKEN_MANDATORY_LABEL tml = { 0 };
	tml.Label.Attributes = SE_GROUP_INTEGRITY;
	// alternatively, use CreateWellKnownSid(WinMediumLabelSid) instead...
	if (!ConvertStringSidToSid(TEXT("S-1-16-8192"), &(tml.Label.Sid)))
	{
		CloseHandle(hRestrictedToken);
		return false;
	}

	if (!SetTokenInformation(hRestrictedToken, TokenIntegrityLevel, &tml, sizeof(tml) + GetLengthSid(tml.Label.Sid)))
	{
		LocalFree(tml.Label.Sid);
		CloseHandle(hRestrictedToken);
		return false;
	}

	LocalFree(tml.Label.Sid);

	// Create startup info

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.lpDesktop = "winsta0\\default";

	PROCESS_INFORMATION pi = { 0 };

	// Get the current executable's name
	TCHAR exePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, exePath, MAX_PATH);

	// Start the new (non-elevated) restricted process
	if (!CreateProcessAsUser(hRestrictedToken, exePath, NULL, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		CloseHandle(hRestrictedToken);
		return false;
	}

	CloseHandle(hRestrictedToken);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return true;
}

bool CxbxExec(std::string &execCommand, HANDLE* hProcess, bool requestHandleProcess) {

	STARTUPINFO startupInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };
	size_t szSize = execCommand.size();
	char* szArgsBufferOutput = new char[szSize + 1];

	strncpy(szArgsBufferOutput, execCommand.c_str(), szSize);
	szArgsBufferOutput[szSize] = '\0';

	/* NOTE: CreateProcess's 2nd parameter (lpCommandLine) is char*, not const char*. Plus it has ability to change the input buffer data.
		Source: https://msdn.microsoft.com/en-us/library/ms682425.aspx

		Using ShellExecute has proper implement. Unfortunately, we need created process handle for Debugger monitor.
		Plus ShellExecute is high level whilst CreateProcess is low level. We want to use official low level functions as possible to reduce
		cpu load cycles to get the task done.
	*/
	if (CreateProcess(nullptr, szArgsBufferOutput, nullptr, nullptr, false, 0, nullptr, nullptr, &startupInfo, &processInfo) == 0) {
		delete[] szArgsBufferOutput;
		return 0;
	}
	delete[] szArgsBufferOutput;
	CloseHandle(processInfo.hThread);

	if (requestHandleProcess) {
		*hProcess = processInfo.hProcess;
	}
	else {
		CloseHandle(processInfo.hProcess);
	}

	return 1;
}

#endif
