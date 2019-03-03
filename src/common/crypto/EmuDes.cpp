// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

// Acknowledgment: ReactOS (GPLv2)
// https://github.com/reactos/reactos

// Changed from ReactOS: we never swap the subkeys in order to generate a decryption key schedule. Instead, we opt to simply read the subkeys
// in reverse order during a decryption operation. This is necessary because XcKeyTable doesn't provide an "operation" argument, that is, it
// always generates an encryption key schedule.

// NOTE: this des implementation doesn't produce exactly the same ciphertext produced by the Xbox. I found that the implementation of Eric Young
// used in OpenSSL does instead, but we can't use it since it's under the Apache 2.0 license, which is incompatible with GPLv2. For reference,
// the DES-YOUN.zip package at https://www.schneier.com/books/applied_cryptography/source.html contains a previous version of the same code under
// the GPLv2 but again it doesn't produce the same ciphertext, and modifying it to make it so would make it identical to the OpenSSL code, which
// is probably a violation of the license, so I won't do it. In practice, as long as the code correctly decrypts the ciphertext (which it does),
// I don't think that the Xbox games will care if the ciphertext is not exactly the same as on the Xbox and should work fine offline. The only
// problem with this is that cxbxr will be unable to communicate with a real Xbox on the network (des is used in console-to-console communications)
// because the des key schedule generated on the console will be different and so it will fail to decrypt packets encrypted by cxbxr.


#include <stdint.h>
#include <cstring>
#include "EmuDes.h"

/*
* 32-bit integer manipulation macros (big endian)
*/
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
* Expanded DES S-boxes
*/
static const uint32_t SB1[64] =
{
	0x01010400, 0x00000000, 0x00010000, 0x01010404,
	0x01010004, 0x00010404, 0x00000004, 0x00010000,
	0x00000400, 0x01010400, 0x01010404, 0x00000400,
	0x01000404, 0x01010004, 0x01000000, 0x00000004,
	0x00000404, 0x01000400, 0x01000400, 0x00010400,
	0x00010400, 0x01010000, 0x01010000, 0x01000404,
	0x00010004, 0x01000004, 0x01000004, 0x00010004,
	0x00000000, 0x00000404, 0x00010404, 0x01000000,
	0x00010000, 0x01010404, 0x00000004, 0x01010000,
	0x01010400, 0x01000000, 0x01000000, 0x00000400,
	0x01010004, 0x00010000, 0x00010400, 0x01000004,
	0x00000400, 0x00000004, 0x01000404, 0x00010404,
	0x01010404, 0x00010004, 0x01010000, 0x01000404,
	0x01000004, 0x00000404, 0x00010404, 0x01010400,
	0x00000404, 0x01000400, 0x01000400, 0x00000000,
	0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const uint32_t SB2[64] =
{
	0x80108020, 0x80008000, 0x00008000, 0x00108020,
	0x00100000, 0x00000020, 0x80100020, 0x80008020,
	0x80000020, 0x80108020, 0x80108000, 0x80000000,
	0x80008000, 0x00100000, 0x00000020, 0x80100020,
	0x00108000, 0x00100020, 0x80008020, 0x00000000,
	0x80000000, 0x00008000, 0x00108020, 0x80100000,
	0x00100020, 0x80000020, 0x00000000, 0x00108000,
	0x00008020, 0x80108000, 0x80100000, 0x00008020,
	0x00000000, 0x00108020, 0x80100020, 0x00100000,
	0x80008020, 0x80100000, 0x80108000, 0x00008000,
	0x80100000, 0x80008000, 0x00000020, 0x80108020,
	0x00108020, 0x00000020, 0x00008000, 0x80000000,
	0x00008020, 0x80108000, 0x00100000, 0x80000020,
	0x00100020, 0x80008020, 0x80000020, 0x00100020,
	0x00108000, 0x00000000, 0x80008000, 0x00008020,
	0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const uint32_t SB3[64] =
{
	0x00000208, 0x08020200, 0x00000000, 0x08020008,
	0x08000200, 0x00000000, 0x00020208, 0x08000200,
	0x00020008, 0x08000008, 0x08000008, 0x00020000,
	0x08020208, 0x00020008, 0x08020000, 0x00000208,
	0x08000000, 0x00000008, 0x08020200, 0x00000200,
	0x00020200, 0x08020000, 0x08020008, 0x00020208,
	0x08000208, 0x00020200, 0x00020000, 0x08000208,
	0x00000008, 0x08020208, 0x00000200, 0x08000000,
	0x08020200, 0x08000000, 0x00020008, 0x00000208,
	0x00020000, 0x08020200, 0x08000200, 0x00000000,
	0x00000200, 0x00020008, 0x08020208, 0x08000200,
	0x08000008, 0x00000200, 0x00000000, 0x08020008,
	0x08000208, 0x00020000, 0x08000000, 0x08020208,
	0x00000008, 0x00020208, 0x00020200, 0x08000008,
	0x08020000, 0x08000208, 0x00000208, 0x08020000,
	0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const uint32_t SB4[64] =
{
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802080, 0x00800081, 0x00800001, 0x00002001,
	0x00000000, 0x00802000, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00800080, 0x00800001,
	0x00000001, 0x00002000, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002001, 0x00002080,
	0x00800081, 0x00000001, 0x00002080, 0x00800080,
	0x00002000, 0x00802080, 0x00802081, 0x00000081,
	0x00800080, 0x00800001, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00000000, 0x00802000,
	0x00002080, 0x00800080, 0x00800081, 0x00000001,
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802081, 0x00000081, 0x00000001, 0x00002000,
	0x00800001, 0x00002001, 0x00802080, 0x00800081,
	0x00002001, 0x00002080, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const uint32_t SB5[64] =
{
	0x00000100, 0x02080100, 0x02080000, 0x42000100,
	0x00080000, 0x00000100, 0x40000000, 0x02080000,
	0x40080100, 0x00080000, 0x02000100, 0x40080100,
	0x42000100, 0x42080000, 0x00080100, 0x40000000,
	0x02000000, 0x40080000, 0x40080000, 0x00000000,
	0x40000100, 0x42080100, 0x42080100, 0x02000100,
	0x42080000, 0x40000100, 0x00000000, 0x42000000,
	0x02080100, 0x02000000, 0x42000000, 0x00080100,
	0x00080000, 0x42000100, 0x00000100, 0x02000000,
	0x40000000, 0x02080000, 0x42000100, 0x40080100,
	0x02000100, 0x40000000, 0x42080000, 0x02080100,
	0x40080100, 0x00000100, 0x02000000, 0x42080000,
	0x42080100, 0x00080100, 0x42000000, 0x42080100,
	0x02080000, 0x00000000, 0x40080000, 0x42000000,
	0x00080100, 0x02000100, 0x40000100, 0x00080000,
	0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const uint32_t SB6[64] =
{
	0x20000010, 0x20400000, 0x00004000, 0x20404010,
	0x20400000, 0x00000010, 0x20404010, 0x00400000,
	0x20004000, 0x00404010, 0x00400000, 0x20000010,
	0x00400010, 0x20004000, 0x20000000, 0x00004010,
	0x00000000, 0x00400010, 0x20004010, 0x00004000,
	0x00404000, 0x20004010, 0x00000010, 0x20400010,
	0x20400010, 0x00000000, 0x00404010, 0x20404000,
	0x00004010, 0x00404000, 0x20404000, 0x20000000,
	0x20004000, 0x00000010, 0x20400010, 0x00404000,
	0x20404010, 0x00400000, 0x00004010, 0x20000010,
	0x00400000, 0x20004000, 0x20000000, 0x00004010,
	0x20000010, 0x20404010, 0x00404000, 0x20400000,
	0x00404010, 0x20404000, 0x00000000, 0x20400010,
	0x00000010, 0x00004000, 0x20400000, 0x00404010,
	0x00004000, 0x00400010, 0x20004010, 0x00000000,
	0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const uint32_t SB7[64] =
{
	0x00200000, 0x04200002, 0x04000802, 0x00000000,
	0x00000800, 0x04000802, 0x00200802, 0x04200800,
	0x04200802, 0x00200000, 0x00000000, 0x04000002,
	0x00000002, 0x04000000, 0x04200002, 0x00000802,
	0x04000800, 0x00200802, 0x00200002, 0x04000800,
	0x04000002, 0x04200000, 0x04200800, 0x00200002,
	0x04200000, 0x00000800, 0x00000802, 0x04200802,
	0x00200800, 0x00000002, 0x04000000, 0x00200800,
	0x04000000, 0x00200800, 0x00200000, 0x04000802,
	0x04000802, 0x04200002, 0x04200002, 0x00000002,
	0x00200002, 0x04000000, 0x04000800, 0x00200000,
	0x04200800, 0x00000802, 0x00200802, 0x04200800,
	0x00000802, 0x04000002, 0x04200802, 0x04200000,
	0x00200800, 0x00000000, 0x00000002, 0x04200802,
	0x00000000, 0x00200802, 0x04200000, 0x00000800,
	0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const uint32_t SB8[64] =
{
	0x10001040, 0x00001000, 0x00040000, 0x10041040,
	0x10000000, 0x10001040, 0x00000040, 0x10000000,
	0x00040040, 0x10040000, 0x10041040, 0x00041000,
	0x10041000, 0x00041040, 0x00001000, 0x00000040,
	0x10040000, 0x10000040, 0x10001000, 0x00001040,
	0x00041000, 0x00040040, 0x10040040, 0x10041000,
	0x00001040, 0x00000000, 0x00000000, 0x10040040,
	0x10000040, 0x10001000, 0x00041040, 0x00040000,
	0x00041040, 0x00040000, 0x10041000, 0x00001000,
	0x00000040, 0x10040040, 0x00001000, 0x00041040,
	0x10001000, 0x00000040, 0x10000040, 0x10040000,
	0x10040040, 0x10000000, 0x00040000, 0x10001040,
	0x00000000, 0x10041040, 0x00040040, 0x10000040,
	0x10040000, 0x10001000, 0x10001040, 0x00000000,
	0x10041040, 0x00041000, 0x00041000, 0x00001040,
	0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
* PC1: left and right halves bit-swap
*/
static const uint32_t LHs[16] =
{
	0x00000000, 0x00000001, 0x00000100, 0x00000101,
	0x00010000, 0x00010001, 0x00010100, 0x00010101,
	0x01000000, 0x01000001, 0x01000100, 0x01000101,
	0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const uint32_t RHs[16] =
{
	0x00000000, 0x01000000, 0x00010000, 0x01010000,
	0x00000100, 0x01000100, 0x00010100, 0x01010100,
	0x00000001, 0x01000001, 0x00010001, 0x01010001,
	0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
* Initial Permutation macro
*/
#define DES_IP(X,Y)                                             \
{                                                               \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
    X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
}

/*
* Final Permutation macro
*/
#define DES_FP(X,Y)                                             \
{                                                               \
    X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
    T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
    Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
    T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
    T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
    T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
    T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
}

/*
* DES round macro
*/
#define DES_ROUND(X,Y,I)                        \
{                                               \
    T = (SK[I] ^ X);                            \
    Y ^= SB8[ (T      ) & 0x3F ] ^              \
         SB6[ (T >>  8) & 0x3F ] ^              \
         SB4[ (T >> 16) & 0x3F ] ^              \
         SB2[ (T >> 24) & 0x3F ];               \
                                                \
    T = (SK[I+1] ^ ((X << 28) | (X >> 4)));     \
    Y ^= SB7[ (T      ) & 0x3F ] ^              \
         SB5[ (T >>  8) & 0x3F ] ^              \
         SB3[ (T >> 16) & 0x3F ] ^              \
         SB1[ (T >> 24) & 0x3F ];               \
}

// For each number between 0x0 and 0xF, this tells how many set bits there are
static const unsigned char DESParityTable[] = { 0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03,0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04 };


// This function sets the parity on the DES key to be odd
// Test case: Halo, Tenchu, Dashboard, Splinter Cell 1 and 2, ...
void mbedtls_des_key_set_parity(unsigned char* Key, unsigned long KeyLenght)
{
	unsigned long i;

	for (i = 0; i < KeyLenght; i++)
	{
		if (!((DESParityTable[Key[i] >> 4] + DESParityTable[Key[i] & 0x0F]) % 2)) {
			Key[i] = Key[i] ^ 0x01;
		}
	}
}

void mbedtls_des_setkey(uint32_t SK[32], const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
	int i;
	uint32_t X, Y, T;

	GET_UINT32_BE(X, key, 0);
	GET_UINT32_BE(Y, key, 4);

	/*
	* Permuted Choice 1
	*/
	T = ((Y >> 4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T << 4);
	T = ((Y) ^ X) & 0x10101010;  X ^= T; Y ^= (T);

	X = (LHs[(X) & 0xF] << 3) | (LHs[(X >> 8) & 0xF] << 2)
		| (LHs[(X >> 16) & 0xF] << 1) | (LHs[(X >> 24) & 0xF])
		| (LHs[(X >> 5) & 0xF] << 7) | (LHs[(X >> 13) & 0xF] << 6)
		| (LHs[(X >> 21) & 0xF] << 5) | (LHs[(X >> 29) & 0xF] << 4);

	Y = (RHs[(Y >> 1) & 0xF] << 3) | (RHs[(Y >> 9) & 0xF] << 2)
		| (RHs[(Y >> 17) & 0xF] << 1) | (RHs[(Y >> 25) & 0xF])
		| (RHs[(Y >> 4) & 0xF] << 7) | (RHs[(Y >> 12) & 0xF] << 6)
		| (RHs[(Y >> 20) & 0xF] << 5) | (RHs[(Y >> 28) & 0xF] << 4);

	X &= 0x0FFFFFFF;
	Y &= 0x0FFFFFFF;

	/*
	* calculate subkeys
	*/
	for (i = 0; i < 16; i++)
	{
		if (i < 2 || i == 8 || i == 15)
		{
			X = ((X << 1) | (X >> 27)) & 0x0FFFFFFF;
			Y = ((Y << 1) | (Y >> 27)) & 0x0FFFFFFF;
		}
		else
		{
			X = ((X << 2) | (X >> 26)) & 0x0FFFFFFF;
			Y = ((Y << 2) | (Y >> 26)) & 0x0FFFFFFF;
		}

		*SK++ = ((X << 4) & 0x24000000) | ((X << 28) & 0x10000000)
			| ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
			| ((X << 6) & 0x01000000) | ((X << 9) & 0x00200000)
			| ((X >> 1) & 0x00100000) | ((X << 10) & 0x00040000)
			| ((X << 2) & 0x00020000) | ((X >> 10) & 0x00010000)
			| ((Y >> 13) & 0x00002000) | ((Y >> 4) & 0x00001000)
			| ((Y << 6) & 0x00000800) | ((Y >> 1) & 0x00000400)
			| ((Y >> 14) & 0x00000200) | ((Y) & 0x00000100)
			| ((Y >> 5) & 0x00000020) | ((Y >> 10) & 0x00000010)
			| ((Y >> 3) & 0x00000008) | ((Y >> 18) & 0x00000004)
			| ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

		*SK++ = ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
			| ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
			| ((X >> 2) & 0x02000000) | ((X << 1) & 0x01000000)
			| ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
			| ((X << 3) & 0x00080000) | ((X >> 6) & 0x00040000)
			| ((X << 15) & 0x00020000) | ((X >> 4) & 0x00010000)
			| ((Y >> 2) & 0x00002000) | ((Y << 8) & 0x00001000)
			| ((Y >> 14) & 0x00000808) | ((Y >> 9) & 0x00000400)
			| ((Y) & 0x00000200) | ((Y << 7) & 0x00000100)
			| ((Y >> 7) & 0x00000020) | ((Y >> 3) & 0x00000011)
			| ((Y << 2) & 0x00000004) | ((Y >> 21) & 0x00000002);
	}
}

/*
* DES key schedule (56-bit, encryption)
*/
void mbedtls_des_setkey_enc(mbedtls_des_context* ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE])
{
	mbedtls_des_setkey(ctx->sk, key);
}

/*
* DES-ECB block encryption/decryption
*/
void mbedtls_des_crypt_ecb(mbedtls_des_context* ctx,
	const unsigned char input[8],
	unsigned char output[8],
	unsigned long encrypt)
{
	int i;
	uint32_t X, Y, T, *SK;

	SK = ctx->sk;

	GET_UINT32_BE(X, input, 0);
	GET_UINT32_BE(Y, input, 4);

	DES_IP(X, Y);

	if (encrypt == MBEDTLS_DES_ENCRYPT) {
		for (i = 0; i < 32; i += 4)
		{
			DES_ROUND(Y, X, i + 0);
			DES_ROUND(X, Y, i + 2);
		}
	}
	else {
		for (i = 30; i > 0; i -= 4)
		{
			DES_ROUND(Y, X, i - 0);
			DES_ROUND(X, Y, i - 2);
		}
	}

	DES_FP(Y, X);

	PUT_UINT32_BE(Y, output, 0);
	PUT_UINT32_BE(X, output, 4);
}

/*
* DES-CBC buffer encryption/decryption
*/
int mbedtls_des_crypt_cbc(mbedtls_des_context* ctx,
	unsigned long mode,
	unsigned long length,
	unsigned char iv[8],
	const unsigned char* input,
	unsigned char* output)
{
	int i, ret, num_des_blocks;
	unsigned char temp[8];

	ret = 0;
	num_des_blocks = (length + 7) / 8;

	// The original code of ReactOS correctly checks that the input length is a multiple of a des_block (8 bytes) but the
	// kernel doesn't and will encrypt up to block (lenght + 7) / 8. This means that we'll run the risk of reading some
	// random bytes after the buffer end and/or touching invalid memory and crash. Because the real kernel does it, we'll
	// allow this buggy behaviour for the sake of accuracy.
	if (length % 8) {
		ret = MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH;
	}

	if (mode == MBEDTLS_DES_ENCRYPT)
	{
		while (num_des_blocks > 0)
		{
			for (i = 0; i < 8; i++) {
				output[i] = (unsigned char)(input[i] ^ iv[i]);
			}

			mbedtls_des_crypt_ecb(ctx, output, output, MBEDTLS_DES_ENCRYPT);
			memcpy(iv, output, 8);

			input += 8;
			output += 8;
			num_des_blocks--;
		}
	}
	else /* MBEDTLS_DES_DECRYPT */
	{
		while (num_des_blocks > 0)
		{
			memcpy(temp, input, 8);
			mbedtls_des_crypt_ecb(ctx, input, output, MBEDTLS_DES_DECRYPT);

			for (i = 0; i < 8; i++) {
				output[i] = (unsigned char)(output[i] ^ iv[i]);
			}

			memcpy(iv, temp, 8);

			input += 8;
			output += 8;
			num_des_blocks--;
		}
	}

	return ret;
}

/*
* Triple-DES key schedule (168-bit, encryption)
*/
void mbedtls_des3_set3key_enc(mbedtls_des3_context* ctx,
	const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3])
{
	mbedtls_des_setkey(ctx->sk, key);
	mbedtls_des_setkey(ctx->sk + 32, key + 8);
	mbedtls_des_setkey(ctx->sk + 64, key + 16);
}

/*
* 3DES-ECB buffer encryption
*/
void mbedtls_des3_encrypt_ecb(mbedtls_des3_context* ctx,
	const unsigned char input[8],
	unsigned char output[8])
{
	int i;
	uint32_t X, Y, T, *SK;

	SK = ctx->sk;

	GET_UINT32_BE(X, input, 0);
	GET_UINT32_BE(Y, input, 4);

	DES_IP(X, Y);

	for (i = 0; i < 32; i += 4)
	{
		DES_ROUND(Y, X, i + 0);
		DES_ROUND(X, Y, i + 2);
	}

	for (i = 62; i > 32; i -= 4)
	{
		DES_ROUND(X, Y, i - 0);
		DES_ROUND(Y, X, i - 2);
	}

	for (i = 64; i < 96; i += 4)
	{
		DES_ROUND(Y, X, i + 0);
		DES_ROUND(X, Y, i + 2);
	}

	DES_FP(Y, X);

	PUT_UINT32_BE(Y, output, 0);
	PUT_UINT32_BE(X, output, 4);
}

/*
* 3DES-ECB buffer decryption
*/
void mbedtls_des3_decrypt_ecb(mbedtls_des3_context* ctx,
	const unsigned char input[8],
	unsigned char output[8])
{
	int i;
	uint32_t X, Y, T, *SK;

	SK = ctx->sk;

	GET_UINT32_BE(X, input, 0);
	GET_UINT32_BE(Y, input, 4);

	DES_IP(X, Y);

	for (i = 94; i > 64; i -= 4)
	{
		DES_ROUND(Y, X, i - 0);
		DES_ROUND(X, Y, i - 2);
	}

	for (i = 32; i < 64; i += 4)
	{
		DES_ROUND(X, Y, i + 0);
		DES_ROUND(Y, X, i + 2);
	}

	for (i = 30; i > 0; i -= 4)
	{
		DES_ROUND(Y, X, i - 0);
		DES_ROUND(X, Y, i - 2);
	}

	DES_FP(Y, X);

	PUT_UINT32_BE(Y, output, 0);
	PUT_UINT32_BE(X, output, 4);
}

void mbedtls_des3_crypt_ecb(mbedtls_des3_context* ctx,
	const unsigned char input[8],
	unsigned char output[8],
	unsigned long encrypt)
{
	if (encrypt == MBEDTLS_DES_ENCRYPT) {
		mbedtls_des3_encrypt_ecb(ctx, input, output);
	}
	else {
		mbedtls_des3_decrypt_ecb(ctx, input, output);
	}
}

/*
* 3DES-CBC buffer encryption/decryption
*/
int mbedtls_des3_crypt_cbc(mbedtls_des3_context* ctx,
	unsigned long mode,
	unsigned long length,
	unsigned char iv[8],
	const unsigned char *input,
	unsigned char *output)
{
	int i, ret, num_des_blocks;
	unsigned char temp[8];

	ret = 0;
	num_des_blocks = (length + 7) / 8;

	if (length % 8) {
		ret = MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH;
	}

	if (mode == MBEDTLS_DES_ENCRYPT)
	{
		while (num_des_blocks > 0)
		{
			for (i = 0; i < 8; i++) {
				output[i] = (unsigned char)(input[i] ^ iv[i]);
			}

			mbedtls_des3_encrypt_ecb(ctx, output, output);
			memcpy(iv, output, 8);

			input += 8;
			output += 8;
			num_des_blocks--;
		}
	}
	else /* MBEDTLS_DES_DECRYPT */
	{
		while (num_des_blocks > 0)
		{
			memcpy(temp, input, 8);
			mbedtls_des3_decrypt_ecb(ctx, input, output);

			for (i = 0; i < 8; i++) {
				output[i] = (unsigned char)(output[i] ^ iv[i]);
			}

			memcpy(iv, temp, 8);

			input += 8;
			output += 8;
			num_des_blocks--;
		}
	}

	return ret;
}
