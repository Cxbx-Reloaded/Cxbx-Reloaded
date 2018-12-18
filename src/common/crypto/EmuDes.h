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
// *   common->crypto->EmuDes.h
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

#ifndef EMUDES_H
#define EMUDES_H

#define MBEDTLS_DES_KEY_SIZE 8

#define MBEDTLS_DES_ENCRYPT     1
#define MBEDTLS_DES_DECRYPT     0

#define MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH -0x0032 /**< The data input has an invalid length. */

/**
* \brief          DES context structure
*/
typedef struct
{
	uint32_t sk[32];            /*!<  DES subkeys       */
}
mbedtls_des_context;

/**
* \brief          Triple-DES context structure
*/
typedef struct
{
	uint32_t sk[96];            /*!<  3DES subkeys      */
}
mbedtls_des3_context;

void mbedtls_des_key_set_parity(unsigned char* Key, unsigned long KeyLenght);
void mbedtls_des_setkey_enc(mbedtls_des_context* ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE]);
void mbedtls_des_crypt_ecb(mbedtls_des_context* ctx, const unsigned char input[8], unsigned char output[8], unsigned long encrypt);
int mbedtls_des_crypt_cbc(mbedtls_des_context* ctx, unsigned long mode, unsigned long length, unsigned char iv[8], const unsigned char* input, unsigned char* output);
void mbedtls_des3_set3key_enc(mbedtls_des3_context* ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3]);
void mbedtls_des3_crypt_ecb(mbedtls_des3_context* ctx, const unsigned char input[8], unsigned char output[8], unsigned long encrypt);
int mbedtls_des3_crypt_cbc(mbedtls_des3_context* ctx, unsigned long mode, unsigned long length, unsigned char iv[8], const unsigned char *input, unsigned char *output);

#endif EMUDES_H
