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
#ifndef XONLINE_H
#define XONLINE_H

// ******************************************************************
// * patch: WSAStartup
// ******************************************************************
int WINAPI EMUPATCH(WSAStartup)
(
    WORD        wVersionRequested,
    WSADATA    *lpWSAData
);

// ******************************************************************
// * patch: XNetStartup
// ******************************************************************
INT WINAPI EMUPATCH(XNetStartup)
(
    const PVOID pDummy
);

// ******************************************************************
// * patch: XNetGetEthernetLinkStatus
// ******************************************************************
DWORD WINAPI EMUPATCH(XNetGetEthernetLinkStatus)();

// ******************************************************************
// * patch: XOnlineLaunchNewImage
// ******************************************************************
HRESULT WINAPI XOnlineLaunchNewImage
(
    LPCSTR	lpImagePath,
    LPVOID	pLaunchData
);

// ******************************************************************
// * patch: XOnlineLogon
// ******************************************************************
HRESULT WINAPI EMUPATCH(XOnlineLogon)
(
    VOID*	pUsers,
    DWORD*	pdwServiceIDs,
    DWORD	dwServices,
    HANDLE	hEvent,
    HANDLE	pHandle
);

SOCKET WINAPI EMUPATCH(socket)
(
    int   af,
    int   type,
    int   protocol
);

int WINAPI EMUPATCH(connect)
(
    SOCKET s,
    const struct sockaddr FAR *name,
    int namelen
);

int WINAPI EMUPATCH(send)
(
    SOCKET s,
    const char FAR *buf,
    int len,
    int flags
);

int WINAPI EMUPATCH(recv)
(
    SOCKET s,
    char FAR *buf,
    int len,
    int flags
);


int WINAPI EMUPATCH(bind)
(
    SOCKET s, 
    const struct sockaddr FAR *name, 
    int namelen
);

int WINAPI EMUPATCH(listen)
(
    SOCKET s, 
    int backlog
);

int WINAPI EMUPATCH(ioctlsocket)
(
    SOCKET s, 
    long cmd, 
    u_long FAR *argp
);

#endif
