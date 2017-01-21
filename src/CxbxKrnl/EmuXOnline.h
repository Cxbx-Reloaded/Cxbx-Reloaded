// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuXOnline.h
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
#ifndef EMUXONLINE_H
#define EMUXONLINE_H


// Flags returned by XNetGetEthernetLinkStatus()
#define XNET_ETHERNET_LINK_ACTIVE           0x01
#define XNET_ETHERNET_LINK_100MBPS          0x02
#define XNET_ETHERNET_LINK_10MBPS           0x04
#define XNET_ETHERNET_LINK_FULL_DUPLEX      0x08
#define XNET_ETHERNET_LINK_HALF_DUPLEX      0x10

// ******************************************************************
// * clas: EmuThis
// ******************************************************************
class EmuThis
{
    public:
        // ******************************************************************
        // * patch: socket
        // ******************************************************************
        SOCKET EMUPATCH(socket)(int af, int type, int protocol);

		// ******************************************************************
		// * patch: This::Emuconnect
		// ******************************************************************
		int EMUPATCH(connect)(SOCKET s, const struct sockaddr FAR *name, int namelen);

		// ******************************************************************
		// * patch: This::Emusend
		// ******************************************************************
		int EMUPATCH(send)(SOCKET s, const char FAR *buf, int len, int flags);

		// ******************************************************************
		// * patch: This::Emurecv
		// ******************************************************************
		int EMUPATCH(recv)(SOCKET s, char FAR *buf, int len, int flags);

        // ******************************************************************
        // * patch: bind
        // ******************************************************************
        int EMUPATCH(bind)(SOCKET s, const struct sockaddr FAR *name, int namelen);

        // ******************************************************************
        // * patch: listen
        // ******************************************************************
        int EMUPATCH(listen)(SOCKET s, int backlog);

        // ******************************************************************
        // * patch: ioctlsocket
        // ******************************************************************
        int EMUPATCH(ioctlsocket)(SOCKET s, long cmd, u_long FAR *argp);
};

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


#endif
