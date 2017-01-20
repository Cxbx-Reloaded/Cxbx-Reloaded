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
        // * func: Emusocket
        // ******************************************************************
        SOCKET EMUPATCH(socket)(int af, int type, int protocol);

		// ******************************************************************
		// * func: EmuThis::Emuconnect
		// ******************************************************************
		int EMUPATCH(connect)(SOCKET s, const struct sockaddr FAR *name, int namelen);

		// ******************************************************************
		// * func: EmuThis::Emusend
		// ******************************************************************
		int EMUPATCH(send)(SOCKET s, const char FAR *buf, int len, int flags);

		// ******************************************************************
		// * func: EmuThis::Emurecv
		// ******************************************************************
		int EMUPATCH(recv)(SOCKET s, char FAR *buf, int len, int flags);

        // ******************************************************************
        // * func: Emubind
        // ******************************************************************
        int EMUPATCH(bind)(SOCKET s, const struct sockaddr FAR *name, int namelen);

        // ******************************************************************
        // * func: Emulisten
        // ******************************************************************
        int EMUPATCH(listen)(SOCKET s, int backlog);

        // ******************************************************************
        // * func: Emuioctlsocket
        // ******************************************************************
        int EMUPATCH(ioctlsocket)(SOCKET s, long cmd, u_long FAR *argp);
};

// ******************************************************************
// * func: EmuWSAStartup
// ******************************************************************
int WINAPI EMUPATCH(WSAStartup)
(
    WORD        wVersionRequested,
    WSADATA    *lpWSAData
);

// ******************************************************************
// * func: EmuXNetStartup
// ******************************************************************
INT WINAPI EMUPATCH(XNetStartup)
(
    const PVOID pDummy
);

// ******************************************************************
// * func: EmuXNetGetEthernetLinkStatus
// ******************************************************************
DWORD WINAPI EMUPATCH(XNetGetEthernetLinkStatus)();

// ******************************************************************
// * func: EmuXOnlineLaunchNewImage
// ******************************************************************
HRESULT WINAPI XOnlineLaunchNewImage
(
    LPCSTR	lpImagePath,
    LPVOID	pLaunchData
);

// ******************************************************************
// * func: EmuXOnlineLogon
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
