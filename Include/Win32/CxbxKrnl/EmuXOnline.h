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

// If you've never seen ugly code before, here is some for you
#undef FD_SET
#undef SO_DONTLINGER
#undef AF_IPX
#undef AF_MAX
#undef SOMAXCONN
#undef FD_READ
#undef FD_WRITE
#undef FD_OOB
#undef FD_ACCEPT
#undef FD_CONNECT
#undef FD_CLOSE

#include <winsock2.h>

// ******************************************************************
// * clas: EmuThis
// ******************************************************************
class EmuThis
{
    // ******************************************************************
    // * func: Emusocket
    // ******************************************************************
    SOCKET Emusocket(int af, int type, int protocol);

    // ******************************************************************
    // * func: Emubind
    // ******************************************************************
    int Emubind(SOCKET s, const struct sockaddr FAR *name, int namelen);

    // ******************************************************************
    // * func: Emulisten
    // ******************************************************************
    int Emulisten(SOCKET s, int backlog);

    // ******************************************************************
    // * func: Emuioctlsocket
    // ******************************************************************
    int Emuioctlsocket(SOCKET s, long cmd, u_long FAR *argp);
};

// ******************************************************************
// * func: EmuWSAStartup
// ******************************************************************
int WINAPI EmuWSAStartup
(
    WORD        wVersionRequested,
    WSADATA    *lpWSAData
);

// ******************************************************************
// * func: EmuXNetStartup
// ******************************************************************
INT WINAPI EmuXNetStartup
(
    const PVOID pDummy
);

// ******************************************************************
// * func: EmuXNetGetEthernetLinkStatus
// ******************************************************************
DWORD WINAPI EmuXNetGetEthernetLinkStatus();

#endif
