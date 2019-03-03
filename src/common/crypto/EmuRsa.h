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
// *  (c) 2018 ergo720
// *
// *  All rights reserved
// *
// ******************************************************************

#ifndef EMURSA_H
#define EMURSA_H

#pragma pack(4)

typedef struct _RSA_PUBLIC_KEY
{
	union
	{
		unsigned char Default[284];
		struct {
			char Magic[4];  		     // "RSA1"
			unsigned int Bloblen; 		 // 264 (Modulus + Exponent + Modulussize)
			unsigned char Bitlen[4];  	 // 2048
			unsigned int ModulusSize;	 // 255 (bytes in the Modulus)
			unsigned char Exponent[4];
			unsigned char Modulus[256];  // Bit endian style
			unsigned char Unknown[8];    // ?
		}KeyData;
	};
} RSA_PUBLIC_KEY;

#pragma pack()

void RSAdecrypt(unsigned char* c_number, unsigned char* cryptbuffer, RSA_PUBLIC_KEY key);
bool Verifyhash(unsigned char* hash, unsigned char* decryptBuffer, RSA_PUBLIC_KEY key);

#endif
