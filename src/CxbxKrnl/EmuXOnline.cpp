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
// *   Cxbx->Win32->CxbxKrnl->EmuXOnline.cpp
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

#include "Emu.h"
#include "Logging.h"
#include "EmuFS.h"

// ******************************************************************
// * prevent name collisions
// ******************************************************************
namespace NtDll
{
    #include "EmuNtDll.h"
};

#include "EmuXTL.h"

/*
// ******************************************************************
// * patch: WSAStartup
// ******************************************************************
int WINAPI XTL::EMUPATCH(WSAStartup)
(
    WORD        wVersionRequested,
    WSADATA    *lpWSAData
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(wVersionRequested)
		LOG_FUNC_ARG(lpWSAData)
		LOG_FUNC_END;

	// Prevent this function from failing by requesting a version of Winsock that
	// we know for sure is actually supported on Windows.  This also fixes one error
	// in the 4920 dashboard.
    int ret = WSAStartup(MAKEWORD(2,2) /*wVersionRequested, lpWSAData);

	RETURN(ret);
}

/*
// ******************************************************************
// * patch: XNetStartup
// ******************************************************************
INT WINAPI XTL::EMUPATCH(XNetStartup)
(
    const PVOID pDummy
)
{
	FUNC_EXPORTS

	LOG_FUNC_ONE_ARG(pDummy);

	// Fake Successfull...hehehe...sucker...hehehehehe
	INT ret = 0;

	RETURN(ret);
}


// ******************************************************************
// * patch: This::Emusocket
// ******************************************************************
SOCKET WINAPI XTL::EMUPATCH(socket)
(
    int   af,
    int   type,
    int   protocol
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(af)
		LOG_FUNC_ARG(type)
		LOG_FUNC_ARG(protocol)
		LOG_FUNC_END;

    SOCKET ret = socket(af, type, protocol);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emuconnect
// ******************************************************************
int WINAPI XTL::EMUPATCH(connect)
(
	SOCKET s,
	const struct sockaddr FAR *name,
	int namelen
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(name)
		LOG_FUNC_ARG(namelen)
		LOG_FUNC_END;

	int ret = connect(s, name, namelen);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emusend
// ******************************************************************
int WINAPI XTL::EMUPATCH(send)
(
	SOCKET s,
	const char FAR *buf,
	int len,
	int flags
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(buf)
		LOG_FUNC_ARG(len)
		LOG_FUNC_ARG(flags)
		LOG_FUNC_END;

	int ret = send(s, buf, len, flags);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emurecv
// ******************************************************************
int WINAPI XTL::EMUPATCH(recv)
(
	SOCKET s,
	char FAR *buf,
	int len,
	int flags
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(buf)
		LOG_FUNC_ARG(len)
		LOG_FUNC_ARG(flags)
		LOG_FUNC_END;

	int ret = recv(s, buf, len, flags);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emubind
// ******************************************************************
int WINAPI XTL::EMUPATCH(bind)
(
	SOCKET s, 
	const struct sockaddr FAR *name, 
	int namelen
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(name)
		LOG_FUNC_ARG(namelen)
		LOG_FUNC_END;

    // TODO: Host-To-Network order if necessary (probably not?)

    int ret = bind(s, name, namelen);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emulisten
// ******************************************************************
int WINAPI XTL::EMUPATCH(listen)
(
	SOCKET s, 
	int backlog
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(backlog)
		LOG_FUNC_END;

    // TODO: Host-To-Network order if necessary (probably not?)

    int ret = listen(s, backlog);

	RETURN(ret);
}

// ******************************************************************
// * patch: This::Emuioctlsocket
// ******************************************************************
int WINAPI XTL::EMUPATCH(ioctlsocket)
(
	SOCKET s, 
	long cmd, 
	u_long FAR *argp
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(s)
		LOG_FUNC_ARG(cmd)
		LOG_FUNC_ARG(argp)
		LOG_FUNC_END;

    int ret = ioctlsocket(s, cmd, argp);

	RETURN(ret);
}
*/

// ******************************************************************
// * patch: XOnlineLaunchNewImage
// ******************************************************************
HRESULT WINAPI XOnlineLaunchNewImage
(
    LPCSTR	lpImagePath,
    LPVOID	pLaunchData
)
{
	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(lpImagePath)
		LOG_FUNC_ARG(pLaunchData)
		LOG_FUNC_END;

	// TODO: Launch another .xbe from Cxbx someday?

	HRESULT ret = E_FAIL;

	RETURN(ret);
}

/*
// ******************************************************************
// * patch: XOnlineLogon
// ******************************************************************
HRESULT WINAPI XTL::EMUPATCH(XOnlineLogon)
(
    VOID*	pUsers,
    DWORD*	pdwServiceIDs,
    DWORD	dwServices,
    HANDLE	hEvent,
    HANDLE	pHandle
)
{
	FUNC_EXPORTS

	LOG_FUNC_BEGIN
		LOG_FUNC_ARG(pUsers)
		LOG_FUNC_ARG(pdwServiceIDs)
		LOG_FUNC_ARG(dwServices)
		LOG_FUNC_ARG(hEvent)
		LOG_FUNC_ARG(pHandle)
		LOG_FUNC_END;

	// TODO: What will it take to log on to Xbox Live?

	HRESULT ret = HRESULT(0x80151000L);	// XONLINE_E_LOGON_NO_NETWORK_CONNECTION

	RETURN(ret);
}
*/