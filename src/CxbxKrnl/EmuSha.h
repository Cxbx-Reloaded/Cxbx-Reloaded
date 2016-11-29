// ******************************************************************
// *
// *    .,-:::::    .,::      .::::::::.    .,::      .:
// *  ,;;;'````'    `;;;,  .,;;  ;;;'';;'   `;;;,  .,;;
// *  [[[             '[[,,[['   [[[__[[\.    '[[,,[['
// *  $$$              Y$$$P     $$""""Y$$     Y$$$P
// *  `88bo,__,o,    oP"``"Yo,  _88o,,od8P   oP"``"Yo,
// *    "YUMMMMMP",m"       "Mm,""YUMMMP" ,m"       "Mm,
// *
// *   Cxbx->Win32->CxbxKrnl->EmuSha.h
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
#ifndef EMUSHA_H
#define EMUSHA_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <Windows.h>

#define A_SHA_DIGEST_LEN 20

typedef struct {
	ULONG Unknown[6];
	ULONG State[5];
	ULONG Count[2];
	UCHAR Buffer[64];
} SHA_CTX, *PSHA_CTX;

typedef VOID (WINAPI *FPTR_A_SHAInit)
(
	PSHA_CTX
);

typedef VOID(WINAPI *FPTR_A_SHAUpdate)
(
	PSHA_CTX,
	const unsigned char *,
	UINT
);

typedef VOID(WINAPI *FPTR_A_SHAFinal)
(
	PSHA_CTX,
	PUCHAR
);

// ******************************************************************
// * Exported API
// ******************************************************************
#define EXTERN(API) \
extern FPTR_##API                          API

EXTERN(A_SHAInit);
EXTERN(A_SHAUpdate);
EXTERN(A_SHAFinal);

#if defined(__cplusplus)
}
#endif

#endif
