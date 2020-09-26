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
#if 0 // XOnline.h isn't used, but below is still useful documentation.

#include "core\kernel\support\Emu.h"
#include "core\hle\XAPI\Xapi.h" // For EMUPATCH

namespace xbox {

// ******************************************************************
// * patch: WSAStartup
// ******************************************************************
xbox::int_xt WINAPI EMUPATCH(WSAStartup)
(
    word_xt        wVersionRequested,
    WSADATA    *lpWSAData
);

// ******************************************************************
// * patch: XNetStartup
// ******************************************************************
xbox::int_xt WINAPI EMUPATCH(XNetStartup)
(
    const PVOID pDummy
);

// ******************************************************************
// * patch: XNetGetEthernetLinkStatus
// ******************************************************************
xbox::dword_xt WINAPI EMUPATCH(XNetGetEthernetLinkStatus)();

// ******************************************************************
// * patch: XOnlineLaunchNewImage
// ******************************************************************
xbox::hresult_xt WINAPI XOnlineLaunchNewImage
(
    LPCSTR	lpImagePath,
    LPVOID	pLaunchData
);

// ******************************************************************
// * patch: XOnlineLogon
// ******************************************************************
xbox::hresult_xt WINAPI EMUPATCH(XOnlineLogon)
(
    void_xt*	pUsers,
    dword_xt*	pdwServiceIDs,
    dword_xt	dwServices,
    HANDLE	hEvent,
    HANDLE	pHandle
);

SOCKET WINAPI EMUPATCH(socket)
(
    int_xt   af,
    int_xt   type,
    int_xt   protocol
);

xbox::int_xt WINAPI EMUPATCH(connect)
(
    SOCKET s,
    const struct sockaddr FAR *name,
    int_xt namelen
);

xbox::int_xt WINAPI EMUPATCH(send)
(
    SOCKET s,
    const char FAR *buf,
    int_xt len,
    int_xt flags
);

xbox::int_xt WINAPI EMUPATCH(recv)
(
    SOCKET s,
    char FAR *buf,
    int_xt len,
    int_xt flags
);


xbox::int_xt WINAPI EMUPATCH(bind)
(
    SOCKET s, 
    const struct sockaddr FAR *name, 
    int_xt namelen
);

xbox::int_xt WINAPI EMUPATCH(listen)
(
    SOCKET s, 
    int_xt backlog
);

xbox::int_xt WINAPI EMUPATCH(ioctlsocket)
(
    SOCKET s, 
    long cmd, 
    u_long FAR *argp
);

} // end of namespace xbox

#endif
