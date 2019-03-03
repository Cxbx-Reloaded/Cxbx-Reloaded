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
// *  (c) 2019 Jannik Vogel
// *
// *  All rights reserved
// *
// ******************************************************************

// Acknowledgment:
// This rsa implementation is directly borrowed from the xbedump tool of XQEMU (GPLv2 license)
// https://github.com/xqemu/xbedump

#define LOG_PREFIX CXBXR_MODULE::RSA

#include "EmuRsa.h"
#include "core\kernel\support\Emu.h" // For EmuLog
#include <cstring>
#include <assert.h>


/* 2^(16*MAX_SHORTS)-1 will fit into a giant, but take care:
* one usually has squares, etc. of giants involved, and
* every intermediate giant in a calculation must fit into
* this many shorts. Thus, if you want systematically to effect
* arithmetic on B-bit operands, you need MAX_SHORTS > B/8,
* preferably a tad larger than this; e.g. MAX_SHORTS > B/7.
*/
#define MAX_SHORTS (1<<19)
#define GIANT_INFINITY (-1)
#define FA 0
#define TR 1
#define TWOPI (double)(2*3.1415926535897932384626433)
#define TWO16 (double)(65536.0)
#define TWOM16 (double)(0.0000152587890625)
#define SQRTHALF (double)(0.707106781186547524400844362104)

#define newmin(a,b) ((a)<(b)? (a) : (b))
/* Size by which to increment the stack used in pushg() and popg(). */
#define	STACK_GROW	16
/* Error codes */
#define DIVIDEBYZERO  1
#define OVFLOW      2
#define SIGN	      3
#define OVERRANGE     4
#define AUTO_MUL 0
#define GRAMMAR_MUL 1
#define FFT_MUL 2
#define KARAT_MUL 3
/* Next, mumber of shorts per operand at which Karatsuba breaks over. */
#define KARAT_BREAK 40
/* Next, mumber of shorts per operand at which FFT breaks over. */
#define FFT_BREAK 200


/* Structure definitions */
typedef struct
{
	int 			sign;
	unsigned short	n[1];       /* number of shorts = abs(sign) */
} giantstruct;

typedef giantstruct* giant;

const unsigned char RSApkcs1paddingtable[3][16] = {
	{ 0x0F, 0x14,0x04,0x00,0x05,0x1A,0x02,0x03,0x0E,0x2B,0x05,0x06,0x09,0x30,0x21,0x30 },
	{ 0x0D, 0x14,0x04,0x1A,0x02,0x03,0x0E,0x2B,0x05,0x06,0x07,0x30,0x1F,0x30,0x00,0x00 },
	{ 0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
};


/* Global variables. */
int			    current_max_size = 0, cur_run = 0;
static int	    stack_glen = 0;
static int	    cur_stack_elem = 0;
static int	    cur_stack_size = 0;
static giant*   stack;
giant		    cur_den = NULL, cur_recip = NULL;
int			    error = 0;
int			    mulmode = AUTO_MUL;
static double*  z = NULL, *z2 = NULL;
int			    checkFFTerror = 0;
double			maxFFTerror;
double*			sinCos = NULL;


/* Global function declarations */
giant newgiant(int numshorts);
void gigimport(giant g, const unsigned char* buff, int len);
void auxmulg(giant a, giant b);
void normal_subg(giant a, giant b);
void reverse_subg(giant a, giant b);
void normal_addg(giant a, giant b);
void FFTmulg(giant y, giant x);
void FFTsquareg(giant x);
void grammarmulg(giant a, giant b);
void grammarsquareg(giant b);
void karatmulg(giant a, giant b);
void karatsquareg(giant b);
void justg(giant x);
void addsignal(giant x, double* z, int n);
void fftinv_hermitian_to_real(double* z, int n);
void mul_hermitian(double* a, double* b, int n);
void square_hermitian(double* b, int n);
void fft_real_to_hermitian(double* z, int n);
void giant_to_double(giant x, int sizex, double* z, int L);
int lpt(int, int *);
double gfloor(double);
void init_sinCos(int);
double s_sin(int);
double s_cos(int);
void scramble_real(double* x, int n);
// Stack functions
static giant popg();
static void	pushg(int a);
// Math Functions
void powermodg(giant x, giant n, giant z); /* x := x^n (mod z). */
void modg(giant d, giant n); /* num := num % den, any positive den. */
void squareg(giant g); /* g *= g. */
void mulg(giant a, giant b); /* b *= a. */
int bitval(giant n, int pos); /* Returns the value of the pos bit of n. */
int bitlen(giant n); /* Returns the bit-length n; e.g. n=7 returns 3. */
void itog(int n, giant g); /* Integer <-> giant. */
void gtog(giant src, giant dest); /* Copies one giant to another. */
void make_recip(giant d, giant r); /* r becomes the steady-state reciprocal 2^(2b)/d, where * b = bit-length of d-1. */
void modg_via_recip(giant d, giant r, giant n); /* n := n % d, d positive, using stored reciprocal directly. */
int gcompg(giant a, giant b); /* Returns 1, 0, -1 as a>b, a=b, a<b. */
void subg(giant a, giant b); /* b -= a. */
void gshiftright(int bits, giant g); /* Shift g right by bits, losing bits on the right. */
void gshiftleft(int bits, giant g); /* Shift g left by bits, introducing zeros on the right. */
void addg(giant a, giant b); /* b += a. */
int isZero(giant g); /* Returns whether g is zero. */
void negg(giant g); /* g := -g. */
void iaddg(int i, giant g); /* g += i, with i non-negative and < 2^16. */
int gsign(giant g); /* Returns the sign of g: -1, 0, 1. */
void absg(giant g); /* g := |g|. */

void ModExp(unsigned char* a_number, const unsigned char* b_number, unsigned int b_len, const unsigned char* c_number, unsigned int c_len, const unsigned char* d_number, unsigned int d_len)
{
	giant b = newgiant(GIANT_INFINITY);
	giant c = newgiant(GIANT_INFINITY);
	giant d = newgiant(GIANT_INFINITY);
	gigimport(b, b_number, b_len);
	gigimport(c, c_number, c_len);
	gigimport(d, d_number, d_len);

	/* a = b := b^c (mod d). */
	powermodg(b, c, d);

	memcpy(a_number, b->n, d_len);
}

void RSAdecrypt(const unsigned char* c_number, unsigned char* cryptbuffer, RSA_PUBLIC_KEY key)
{
	ModExp(cryptbuffer, c_number, 256, key.KeyData.Exponent, 4, key.KeyData.Modulus, 256);
}

bool Verifyhash(const unsigned char* hash, const unsigned char* decryptBuffer, RSA_PUBLIC_KEY key)
{
	unsigned char cmphash[20];
	int a;
	int zero_position = 20;

	// Convert Hash to "Big-Endian Format"
	for (a = 0;a<20;a++) cmphash[a] = hash[19 - a];

	// Compare if the Hash Results (first 20 Bytes) are the same
	if (memcmp(decryptBuffer, cmphash, 20) != 0)   return false;

	unsigned char *pkcspad;
	for (int tableIndex = 0; RSApkcs1paddingtable[tableIndex][0] != 0; tableIndex++) {

		pkcspad = (unsigned char*)RSApkcs1paddingtable[tableIndex];
		int difference = memcmp(pkcspad + 1, &decryptBuffer[20], *pkcspad);

		if (!difference)
		{
			zero_position = *pkcspad + 20;
			break;
		}

	}

	// Padding checking , xbox does exactly the same 

	if (decryptBuffer[zero_position] != 0x00) return false;

	if (decryptBuffer[key.KeyData.ModulusSize] != 0x00) return false;

	if (decryptBuffer[key.KeyData.ModulusSize - 1] != 0x01) return false;

	for (unsigned int i = zero_position + 1; i < (key.KeyData.ModulusSize - 1); i++) {
		if (decryptBuffer[i] != 0xff) return false;
	}

	return true;
}

giant newgiant(int numshorts)
{
	int 		size;
	giant 		thegiant;

	if (numshorts > MAX_SHORTS) {
		EmuLog(LOG_LEVEL::WARNING, "Requested giant too big.");
	}
	if (numshorts <= 0)
		numshorts = MAX_SHORTS;
	size = numshorts * sizeof(short) + sizeof(int);
	thegiant = (giant)malloc(size);
	thegiant->sign = 0;

	if (newmin(2 * numshorts, MAX_SHORTS) > current_max_size)
		current_max_size = newmin(2 * numshorts, MAX_SHORTS);

	/* If newgiant() is being called for the first time, set the
	* size of the stack giants. */
	if (stack_glen == 0) stack_glen = current_max_size;

	return(thegiant);
}

void gigimport(giant g, const unsigned char *buff, int len) {

	// copy buffered 'number' into giant's number buffer
	memcpy(g->n, buff, len);

	assert((len % 2) == 0);

	// Get number of shorts
	g->sign = len / 2;

	// Only count used shorts
	while((g->sign >= 1) && (g->n[g->sign - 1] == 0)) {
		g->sign -= 1;
	}

	assert(g->sign != 0);
}

void powermodg(giant x, giant n, giant g)
/* x becomes x^n (mod g). */
{
	int 		len, pos;
	giant		scratch2 = popg();

	gtog(x, scratch2);
	itog(1, x);
	len = bitlen(n);
	pos = 0;
	while (1)
	{
		if (bitval(n, pos++))
		{
			mulg(scratch2, x);
			modg(g, x);
		}
		if (pos >= len)
			break;
		squareg(scratch2);
		modg(g, scratch2);
	}
	pushg(1);
}

giant popg()
{
	int i;

	if (current_max_size <= 0) current_max_size = MAX_SHORTS;

	if (cur_stack_size == 0) {
		/* Initialize the stack if we're just starting out.
		* Note that all stack giants will be whatever current_max_size is
		* when newgiant() is first called. */
		cur_stack_size = STACK_GROW;
		stack = (giant *)malloc(cur_stack_size * sizeof(giant));
		for (i = 0; i < STACK_GROW; i++)
			stack[i] = NULL;
		if (stack_glen == 0) stack_glen = current_max_size;
	}
	else if (cur_stack_elem >= cur_stack_size) {
		/* Expand the stack if we need to. */
		i = cur_stack_size;
		cur_stack_size += STACK_GROW;
		stack = (giant *)realloc(stack, cur_stack_size * sizeof(giant));
		for (; i < cur_stack_size; i++)
			stack[i] = NULL;
	}
	else if (cur_stack_elem < cur_stack_size - 2 * STACK_GROW) {
		/* Prune the stack if it's too big. Disabled, so the stack can only expand */
		/* cur_stack_size -= STACK_GROW;
		for (i = cur_stack_size - STACK_GROW; i < cur_stack_size; i++)
		free(stack[i]);
		stack = (giant *) realloc (stack,cur_stack_size * sizeof(giant)); */
	}

	/* Malloc our giant. */
	if (stack[cur_stack_elem] == NULL)
		stack[cur_stack_elem] = (giant)malloc(stack_glen * sizeof(short) + sizeof(int));
	stack[cur_stack_elem]->sign = 0;

	return(stack[cur_stack_elem++]);
}

void pushg(int a)
{
	if (a < 0) return;
	cur_stack_elem -= a;
	if (cur_stack_elem < 0) cur_stack_elem = 0;
}

void modg(giant d, giant n)
/* n becomes n%d. n is arbitrary, but the denominator d must be positive! */
{
	if (cur_recip == NULL) {
		cur_recip = newgiant(current_max_size);
		cur_den = newgiant(current_max_size);
		gtog(d, cur_den);
		make_recip(d, cur_recip);
	}
	else if (gcompg(d, cur_den)) {
		gtog(d, cur_den);
		make_recip(d, cur_recip);
	}
	modg_via_recip(d, cur_recip, n);
}

void squareg(giant b)
/* b becomes b^2. */
{
	auxmulg(b, b);
}

void mulg(giant a, giant b)
/* b becomes a*b. */
{
	auxmulg(a, b);
}

void auxmulg(giant a, giant b)
/* Optimized general multiply, b becomes a*b. Modes are:
* AUTO_MUL: switch according to empirical speed criteria.
* GRAMMAR_MUL: force grammar-school algorithm.
* KARAT_MUL: force Karatsuba divide-conquer method.
* FFT_MUL: force floating point FFT method. */
{
	float		grammartime;
	int 		square = (a == b);
	int 		sizea, sizeb;

	switch (mulmode)
	{
		case GRAMMAR_MUL:
			if (square) grammarsquareg(b);
			else grammarmulg(a, b);
			break;
		case FFT_MUL:
			if (square)
				FFTsquareg(b);
			else
				FFTmulg(a, b);
			break;
		case KARAT_MUL:
			if (square) karatsquareg(b);
			else karatmulg(a, b);
			break;
		case AUTO_MUL:
			sizea = abs(a->sign);
			sizeb = abs(b->sign);
			if ((sizea > KARAT_BREAK) && (sizea <= FFT_BREAK) &&
				(sizeb > KARAT_BREAK) && (sizeb <= FFT_BREAK)) {
				if (square) karatsquareg(b);
				else karatmulg(a, b);

			}
			else {
				grammartime = (float)sizea;
				grammartime *= (float)sizeb;
				if (grammartime < FFT_BREAK * FFT_BREAK)
				{
					if (square) grammarsquareg(b);
					else grammarmulg(a, b);
				}
				else
				{
					if (square) FFTsquareg(b);
					else FFTmulg(a, b);
				}
			}
			break;
	}
}

int bitval(giant n, int pos)
{
	int 	i = abs(pos) >> 4, c = 1 << (pos & 15);

	return (((n->n[i]) & c) != 0);
}

int bitlen(giant n)
{
	int 		b = 16, c = 1 << 15, w;

	if (isZero(n))
		return(0);
	w = n->n[abs(n->sign) - 1];
	while ((w&c) == 0)
	{
		b--;
		c >>= 1;
	}
	return (16 * (abs(n->sign) - 1) + b);
}

void itog(int i, giant g)
/* The giant g becomes set to the integer value i. */
{
	unsigned int	j = abs(i);

	if (i == 0)
	{
		g->sign = 0;
		g->n[0] = 0;
		return;
	}
	g->n[0] = (unsigned short)(j & 0xFFFF);
	j >>= 16;
	if (j)
	{
		g->n[1] = (unsigned short)j;
		g->sign = 2;
	}
	else
	{
		g->sign = 1;
	}
	if (i < 0)
		g->sign = -(g->sign);
}

void gtog(giant srcgiant, giant destgiant)
/* destgiant becomes equal to srcgiant. */
{
	int numbytes = sizeof(int) + abs(srcgiant->sign) * sizeof(short);

	memcpy((char *)destgiant, (char *)srcgiant, numbytes);
}

void make_recip(giant d, giant r)
/* r becomes the steady-state reciprocal
* 2^(2b)/d, where b = bit-length of d-1. */
{
	int		b;
	giant 	tmp, tmp2;

	if (isZero(d) || (d->sign < 0))
	{
		exit(SIGN);
	}
	tmp = popg();
	tmp2 = popg();
	itog(1, r);
	subg(r, d);
	b = bitlen(d);
	addg(r, d);
	gshiftleft(b, r);
	gtog(r, tmp2);
	while (1)
	{
		gtog(r, tmp);
		squareg(tmp);
		gshiftright(b, tmp);
		mulg(d, tmp);
		gshiftright(b, tmp);
		addg(r, r);
		subg(tmp, r);
		if (gcompg(r, tmp2) <= 0)
			break;
		gtog(r, tmp2);
	}
	itog(1, tmp);
	gshiftleft(2 * b, tmp);
	gtog(r, tmp2);
	mulg(d, tmp2);
	subg(tmp2, tmp);
	itog(1, tmp2);
	while (tmp->sign < 0)
	{
		subg(tmp2, r);
		addg(d, tmp);
	}
	pushg(2);
}

void modg_via_recip(giant d, giant r, giant n)
/* This is the fastest mod of the present collection.
* n := n % d, where r is the precalculated
* steady-state reciprocal of d. */
{
	int		s = (bitlen(r) - 1), sign = n->sign;
	giant 	tmp, tmp2;

	if (isZero(d) || (d->sign < 0))
	{
		exit(SIGN);
	}

	tmp = popg();
	tmp2 = popg();

	n->sign = abs(n->sign);
	while (1)
	{
		gtog(n, tmp); gshiftright(s - 1, tmp);
		mulg(r, tmp);
		gshiftright(s + 1, tmp);
		mulg(d, tmp);
		subg(tmp, n);
		if (gcompg(n, d) >= 0)
			subg(d, n);
		if (gcompg(n, d) < 0)
			break;
	}
	if (sign >= 0)
		goto done;
	if (isZero(n))
		goto done;
	negg(n);
	addg(d, n);
done:
	pushg(2);
	return;
}

int gcompg(giant a, giant b)
/* Returns -1,0,1 if a<b, a=b, a>b, respectively. */
{
	int 		sa = a->sign, j, sb = b->sign, va, vb, sgn;

	if (sa > sb)
		return(1);
	if (sa < sb)
		return(-1);
	if (sa < 0)
	{
		sa = -sa; /* Take absolute value of sa. */
		sgn = -1;
	}
	else
	{
		sgn = 1;
	}
	for (j = sa - 1; j >= 0; j--)
	{
		va = a->n[j];
		vb = b->n[j];
		if (va > vb)
			return(sgn);
		if (va < vb)
			return(-sgn);
	}
	return(0);
}

void subg(giant a, giant b)
/* b := b - a, any signs, any result. */
{
	int 		asgn = a->sign, bsgn = b->sign;

	if (asgn == 0)
		return;
	if (bsgn == 0)
	{
		gtog(a, b);
		negg(b);
		return;
	}
	if ((asgn < 0) != (bsgn < 0))
	{
		if (bsgn > 0)
		{
			negg(a);
			normal_addg(a, b);
			negg(a);
			return;
		}
		negg(b);
		normal_addg(a, b);
		negg(b);
		return;
	}
	if (bsgn > 0)
	{
		if (gcompg(b, a) >= 0)
		{
			normal_subg(a, b);
			return;
		}
		reverse_subg(a, b);
		negg(b);
		return;
	}
	negg(a);
	negg(b);
	if (gcompg(b, a) >= 0)
	{
		normal_subg(a, b);
		negg(a);
		negg(b);
		return;
	}
	reverse_subg(a, b);
	negg(a);
	return;
}

void gshiftright(int	bits, giant g)
/* shift g right bits bits. Equivalent to g = g/2^bits. */
{
	register int 			j, size = abs(g->sign);
	register unsigned int 	carry;
	int 					words = bits >> 4;
	int 					remain = bits & 15, cremain = (16 - remain);

	if (bits == 0)
		return;
	if (isZero(g))
		return;
	if (bits < 0) {
		gshiftleft(-bits, g);
		return;
	}
	if (words >= size) {
		g->sign = 0;
		return;
	}
	if (remain == 0) {
		memmove(g->n, g->n + words, (size - words) * sizeof(short));
		g->sign += (g->sign < 0) ? (words) : (-words);
	}
	else {
		size -= words;

		if (size)
		{
			for (j = 0;j < size - 1;++j)
			{
				carry = g->n[j + words + 1] << cremain;
				g->n[j] = (unsigned short)((g->n[j + words] >> remain) | carry);
			}
			g->n[size - 1] = (unsigned short)(g->n[size - 1 + words] >> remain);
		}

		if (g->n[size - 1] == 0)
			--size;

		if (g->sign > 0)
			g->sign = size;
		else
			g->sign = -size;
	}
}

void gshiftleft(int	bits, giant g)
/* shift g left bits bits. Equivalent to g = g*2^bits. */
{
	int 			rem = bits & 15, crem = 16 - rem, words = bits >> 4;
	int 			size = abs(g->sign), j, k, sign = gsign(g);
	unsigned short 	carry, dat;

	if (!bits)
		return;
	if (!size)
		return;
	if (bits < 0) {
		gshiftright(-bits, g);
		return;
	}
	if (size + words + 1 > current_max_size) {
		error = OVFLOW;
		exit(error);
	}
	if (rem == 0) {
		memmove(g->n + words, g->n, size * sizeof(short));
		for (j = 0; j < words; j++) g->n[j] = 0;
		g->sign += (g->sign < 0) ? (-words) : (words);
	}
	else {
		k = size + words;
		carry = 0;
		for (j = size - 1; j >= 0; j--) {
			dat = g->n[j];
			g->n[k--] = (unsigned short)((dat >> crem) | carry);
			carry = (unsigned short)(dat << rem);
		}
		do {
			g->n[k--] = carry;
			carry = 0;
		} while (k >= 0);

		k = size + words;
		if (g->n[k] == 0)
			--k;
		g->sign = sign * (k + 1);
	}
}

void addg(giant a, giant b)
/* b := b + a, any signs any result. */
{
	int 		asgn = a->sign, bsgn = b->sign;

	if (asgn == 0)
		return;
	if (bsgn == 0)
	{
		gtog(a, b);
		return;
	}
	if ((asgn < 0) == (bsgn < 0))
	{
		if (bsgn > 0)
		{
			normal_addg(a, b);
			return;
		}
		absg(b);
		if (a != b) absg(a);
		normal_addg(a, b);
		negg(b);
		if (a != b) negg(a);
		return;
	}
	if (bsgn > 0)
	{
		negg(a);
		if (gcompg(b, a) >= 0)
		{
			normal_subg(a, b);
			negg(a);
			return;
		}
		reverse_subg(a, b);
		negg(a);
		negg(b);
		return;
	}
	negg(b);
	if (gcompg(b, a) < 0)
	{
		reverse_subg(a, b);
		return;
	}
	normal_subg(a, b);
	negg(b);
	return;
}

int isZero(giant thegiant)
/* Returns TR if thegiant == 0. */
{
	register int 				count;
	int			 				length = abs(thegiant->sign);
	register unsigned short	*	numpointer = thegiant->n;

	if (length)
	{
		for (count = 0; count < length; ++count, ++numpointer)
		{
			if (*numpointer != 0)
				return(FA);
		}
	}
	return(TR);
}

void negg(giant g)
/* g becomes -g. */
{
	g->sign = -g->sign;
}

void normal_subg(giant a, giant b)
/* b := b - a; requires b, a non-negative and b >= a. */
{
	int 			j, size = b->sign;
	unsigned int	k;

	if (a->sign == 0)
		return;

	k = 0;
	for (j = 0; j < a->sign; ++j)
	{
		k += 0xffff - a->n[j] + b->n[j];
		b->n[j] = (unsigned short)(k & 0xffff);
		k >>= 16;
	}
	for (j = a->sign; j < size; ++j)
	{
		k += 0xffff + b->n[j];
		b->n[j] = (unsigned short)(k & 0xffff);
		k >>= 16;
	}

	if (b->n[0] == 0xffff)
		iaddg(1, b);
	else
		++b->n[0];

	while ((size-- > 0) && (b->n[size] == 0));

	b->sign = (b->n[size] == 0) ? 0 : size + 1;
}

void reverse_subg(giant a, giant b)
/* b := a - b; requires b, a non-negative and a >= b. */
{
	int 			j, size = a->sign;
	unsigned int	k;

	k = 0;
	for (j = 0; j < b->sign; ++j)
	{
		k += 0xffff - b->n[j] + a->n[j];
		b->n[j] = (unsigned short)(k & 0xffff);
		k >>= 16;
	}
	for (j = b->sign; j < size; ++j)
	{
		k += 0xffff + a->n[j];
		b->n[j] = (unsigned short)(k & 0xffff);
		k >>= 16;
	}

	b->sign = size; /* REC, 21 Apr 1996. */
	if (b->n[0] == 0xffff)
		iaddg(1, b);
	else
		++b->n[0];

	while (!b->n[--size]);

	b->sign = size + 1;
}

void iaddg(int	i, giant g)
/* Giant g becomes g + (int)i. */
{
	int 	w, j = 0, carry = 0, size = abs(g->sign);
	giant	tmp;

	if (isZero(g))
	{
		itog(i, g);
	}
	else if (g->sign < 0) {
		tmp = popg();
		itog(i, tmp);
		addg(tmp, g);
		pushg(1);
		return;
	}
	else
	{
		w = g->n[0] + i;
		do
		{
			g->n[j] = (unsigned short)(w & 65535L);
			carry = w >> 16;
			w = g->n[++j] + carry;
		} while ((carry != 0) && (j < size));
	}
	if (carry)
	{
		++g->sign;
		g->n[size] = (unsigned short)carry;
	}
}

int gsign(giant g)
/* Returns the sign of g. */
{
	if (isZero(g))
		return(0);
	if (g->sign > 0)
		return(1);
	return(-1);
}

void absg(giant g)
{
	/* g becomes the absolute value of g. */
	if (g->sign < 0)
		g->sign = -g->sign;
}

void FFTmulg(giant y, giant x)
{
	/* x becomes y*x. */
	int 			lambda, sizex = abs(x->sign), sizey = abs(y->sign);
	int 			finalsign = gsign(x)*gsign(y);
	register int	L;

	if ((sizex <= 4) || (sizey <= 4))
	{
		grammarmulg(y, x);
		return;
	}
	L = lpt(sizex + sizey, &lambda);
	if (!z) z = (double *)malloc(MAX_SHORTS * sizeof(double));
	if (!z2) z2 = (double *)malloc(MAX_SHORTS * sizeof(double));

	giant_to_double(x, sizex, z, L);
	giant_to_double(y, sizey, z2, L);
	fft_real_to_hermitian(z, L);
	fft_real_to_hermitian(z2, L);
	mul_hermitian(z2, z, L);
	fftinv_hermitian_to_real(z, L);
	addsignal(x, z, L);
	x->sign = finalsign * abs(x->sign);
}

void FFTsquareg(giant x)
{
	int 			j, size = abs(x->sign);
	register int 	L;

	if (size < 4)
	{
		grammarmulg(x, x);
		return;
	}
	L = lpt(size + size, &j);
	if (!z) z = (double *)malloc(MAX_SHORTS * sizeof(double));
	giant_to_double(x, size, z, L);
	fft_real_to_hermitian(z, L);
	square_hermitian(z, L);
	fftinv_hermitian_to_real(z, L);
	addsignal(x, z, L);
	x->sign = abs(x->sign);
}

void grammarmulg(giant a, giant b)
/* b becomes a*b. */
{
	int 			i, j;
	unsigned int 	prod, carry = 0;
	int 			asize = abs(a->sign), bsize = abs(b->sign);
	unsigned short 	*aptr, *bptr, *destptr;
	unsigned short	mult;
	giant scratch = popg();

	for (i = 0; i < asize + bsize; ++i)
	{
		scratch->n[i] = 0;
	}

	bptr = &(b->n[0]);
	for (i = 0; i < bsize; ++i)
	{
		mult = *(bptr++);
		if (mult)
		{
			carry = 0;
			aptr = &(a->n[0]);
			destptr = &(scratch->n[i]);
			for (j = 0; j < asize; ++j)
			{
				prod = *(aptr++) * mult + *destptr + carry;
				*(destptr++) = (unsigned short)(prod & 0xffff);
				carry = prod >> 16;
			}
			*destptr = (unsigned short)carry;
		}
	}
	bsize += asize;
	if (!carry)
		--bsize;
	scratch->sign = gsign(a)*gsign(b)*bsize;
	gtog(scratch, b);
	pushg(1);
}

void grammarsquareg(giant a)
/* a := a^2. */
{
	unsigned int	cur_term;
	unsigned int	prod, carry = 0, temp;
	unsigned int	asize = abs(a->sign), max = asize * 2 - 1;
	unsigned short	*ptr = a->n, *ptr1, *ptr2;
	giant scratch;

	if (asize == 0) {
		itog(0, a);
		return;
	}

	scratch = popg();

	asize--;

	temp = *ptr;
	temp *= temp;
	scratch->n[0] = temp;
	carry = temp >> 16;

	for (cur_term = 1; cur_term < max; cur_term++) {
		ptr1 = ptr2 = ptr;
		if (cur_term <= asize) {
			ptr2 += cur_term;
		}
		else {
			ptr1 += cur_term - asize;
			ptr2 += asize;
		}
		prod = carry & 0xFFFF;
		carry >>= 16;
		while (ptr1 < ptr2) {
			temp = *ptr1++ * *ptr2--;
			prod += (temp << 1) & 0xFFFF;
			carry += (temp >> 15);
		}
		if (ptr1 == ptr2) {
			temp = *ptr1;
			temp *= temp;
			prod += temp & 0xFFFF;
			carry += (temp >> 16);
		}
		carry += prod >> 16;
		scratch->n[cur_term] = (unsigned short)(prod);
	}
	if (carry) {
		scratch->n[cur_term] = carry;
		scratch->sign = cur_term + 1;
	}
	else scratch->sign = cur_term;

	gtog(scratch, a);
	pushg(1);
}

/* Improved Karatsuba routines from A. Powell, improvements by G. Woltman. */
void karatmulg(giant x, giant y)
/* y becomes x*y. */
{
	int s = abs(x->sign), t = abs(y->sign), w, bits,
		sg = gsign(x)*gsign(y);
	giant a, b, c, d, e, f;

	if ((s <= KARAT_BREAK) || (t <= KARAT_BREAK)) {
		grammarmulg(x, y);
		return;
	}
	w = (s + t + 2) / 4; bits = 16 * w;
	a = popg(); b = popg(); c = popg();
	d = popg(); e = popg(); f = popg();
	gtog(x, a); absg(a); if (w <= s) { a->sign = w; justg(a); }
	gtog(x, b); absg(b);
	gshiftright(bits, b);
	gtog(y, c); absg(c); if (w <= t) { c->sign = w; justg(c); }
	gtog(y, d); absg(d);
	gshiftright(bits, d);
	gtog(a, e); normal_addg(b, e);	/* e := (a + b) */
	gtog(c, f); normal_addg(d, f);	/* f := (c + d) */
	karatmulg(e, f);			/* f := (a + b)(c + d) */
	karatmulg(c, a);			/* a := a c */
	karatmulg(d, b);			/* b := b d */
	normal_subg(a, f);
	/* f := (a + b)(c + d) - a c */
	normal_subg(b, f);
	/* f := (a + b)(c + d) - a c - b d */
	gshiftleft(bits, b);
	normal_addg(f, b);
	gshiftleft(bits, b);
	normal_addg(a, b);
	gtog(b, y); y->sign *= sg;
	pushg(6);

	return;
}

void karatsquareg(giant x)
/* x becomes x^2. */
{
	int s = abs(x->sign), w, bits;
	giant a, b, c;

	if (s <= KARAT_BREAK) {
		grammarsquareg(x);
		return;
	}
	w = (s + 1) / 2; bits = 16 * w;
	a = popg(); b = popg(); c = popg();
	gtog(x, a); a->sign = w; justg(a);
	gtog(x, b); absg(b);
	gshiftright(bits, b);
	gtog(a, c); normal_addg(b, c);
	karatsquareg(c);
	karatsquareg(a);
	karatsquareg(b);
	normal_subg(b, c);
	normal_subg(a, c);
	gshiftleft(bits, b);
	normal_addg(c, b);
	gshiftleft(bits, b);
	normal_addg(a, b);
	gtog(b, x);
	pushg(3);

	return;
}

void justg(giant x)
{
	int s = x->sign, sg = 1;

	if (s < 0) {
		sg = -1;
		s = -s;
	}
	--s;
	while (x->n[s] == 0) {
		--s;
		if (s < 0) break;
	}
	x->sign = sg * (s + 1);
}

void normal_addg(giant a, giant b)
/* b := a + b, both a,b assumed non-negative. */
{
	int 			carry = 0;
	int 			asize = a->sign, bsize = b->sign;
	long 			k;
	int				j = 0;
	unsigned short	*aptr = a->n, *bptr = b->n;

	if (asize < bsize)
	{
		for (j = 0; j < asize; j++)
		{
			k = *aptr++ + *bptr + carry;
			carry = 0;
			if (k >= 65536L)
			{
				k -= 65536L;
				++carry;
			}
			*bptr++ = (unsigned short)k;
		}
		for (j = asize; j < bsize; j++)
		{
			k = *bptr + carry;
			carry = 0;
			if (k >= 65536L)
			{
				k -= 65536L;
				++carry;
			}
			*bptr++ = (unsigned short)k;
		}
	}
	else
	{
		for (j = 0; j < bsize; j++)
		{
			k = *aptr++ + *bptr + carry;
			carry = 0;
			if (k >= 65536L)
			{
				k -= 65536L;
				++carry;
			}
			*bptr++ = (unsigned short)k;
		}
		for (j = bsize; j < asize; j++)
		{
			k = *aptr++ + carry;
			carry = 0;
			if (k >= 65536L)
			{
				k -= 65536L;
				++carry;
			}
			*bptr++ = (unsigned short)k;
		}
	}
	if (carry)
	{
		*bptr = 1; ++j;
	}
	b->sign = j;
}

void addsignal(giant x, double* z, int n)
{
	register int 		j, k, m, car, last;
	register double 	f, g, err;

	maxFFTerror = 0;
	last = 0;
	for (j = 0;j < n;j++)
	{
		f = gfloor(z[j] + 0.5);
		if (f != 0.0) last = j;
		if (checkFFTerror)
		{
			err = fabs(f - z[j]);
			if (err > maxFFTerror)
				maxFFTerror = err;
		}
		z[j] = 0;
		k = 0;
		do
		{
			g = gfloor(f*TWOM16);
			z[j + k] += f - g * TWO16;
			++k;
			f = g;
		} while (f != 0.0);
	}
	car = 0;
	for (j = 0;j < last + 1;j++)
	{
		m = (int)(z[j] + car);
		x->n[j] = (unsigned short)(m & 0xffff);
		car = (m >> 16);
	}
	if (car)
		x->n[j] = (unsigned short)car;
	else
		--j;

	while (!(x->n[j])) --j;

	x->sign = j + 1;
}

void fftinv_hermitian_to_real(double* z, int n)
/* Input is {Re(z^[0]),...,Re(z^[n/2),Im(z^[n/2-1]),...,Im(z^[1]).
* This is a decimation-in-frequency, split-radix algorithm.
*/
{
	register double 	cc1, ss1, cc3, ss3;
	register int 		is, id, i0, i1, i2, i3, i4, i5, i6, i7, i8,
		a, a3, b, b3, nminus = n - 1, dil, expand;
	register double 	*x, e;
	int 				nn = n >> 1;
	double 				t1, t2, t3, t4, t5;
	int 				n2, n4, n8, i, j;

	init_sinCos(n);
	expand = cur_run / n;
	x = z - 1;
	n2 = n << 1;
	while (nn >>= 1)
	{
		is = 0;
		id = n2;
		n2 >>= 1;
		n4 = n2 >> 2;
		n8 = n4 >> 1;
		do
		{
			for (i = is;i < n;i += id)
			{
				i1 = i + 1;
				i2 = i1 + n4;
				i3 = i2 + n4;
				i4 = i3 + n4;
				t1 = x[i1] - x[i3];
				x[i1] += x[i3];
				x[i2] += x[i2];
				x[i3] = t1 - 2.0*x[i4];
				x[i4] = t1 + 2.0*x[i4];
				if (n4 == 1)
					continue;
				i1 += n8;
				i2 += n8;
				i3 += n8;
				i4 += n8;
				t1 = (x[i2] - x[i1])*SQRTHALF;
				t2 = (x[i4] + x[i3])*SQRTHALF;
				x[i1] += x[i2];
				x[i2] = x[i4] - x[i3];
				x[i3] = -2.0*(t2 + t1);
				x[i4] = 2.0*(t1 - t2);
			}
			is = (id << 1) - n2;
			id <<= 2;
		} while (is < n - 1);
		dil = n / n2;
		a = dil;
		for (j = 2;j <= n8;j++)
		{
			a3 = (a + (a << 1))&nminus;
			b = a * expand;
			b3 = a3 * expand;
			cc1 = s_cos(b);
			ss1 = s_sin(b);
			cc3 = s_cos(b3);
			ss3 = s_sin(b3);
			a = (a + dil)&nminus;
			is = 0;
			id = n2 << 1;
			do
			{
				for (i = is;i < n;i += id)
				{
					i1 = i + j;
					i2 = i1 + n4;
					i3 = i2 + n4;
					i4 = i3 + n4;
					i5 = i + n4 - j + 2;
					i6 = i5 + n4;
					i7 = i6 + n4;
					i8 = i7 + n4;
					t1 = x[i1] - x[i6];
					x[i1] += x[i6];
					t2 = x[i5] - x[i2];
					x[i5] += x[i2];
					t3 = x[i8] + x[i3];
					x[i6] = x[i8] - x[i3];
					t4 = x[i4] + x[i7];
					x[i2] = x[i4] - x[i7];
					t5 = t1 - t4;
					t1 += t4;
					t4 = t2 - t3;
					t2 += t3;
					x[i3] = t5 * cc1 + t4 * ss1;
					x[i7] = -t4 * cc1 + t5 * ss1;
					x[i4] = t1 * cc3 - t2 * ss3;
					x[i8] = t2 * cc3 + t1 * ss3;
				}
				is = (id << 1) - n2;
				id <<= 2;
			} while (is < n - 1);
		}
	}
	is = 1;
	id = 4;
	do
	{
		for (i0 = is;i0 <= n;i0 += id)
		{
			i1 = i0 + 1;
			e = x[i0];
			x[i0] = e + x[i1];
			x[i1] = e - x[i1];
		}
		is = (id << 1) - 1;
		id <<= 2;
	} while (is < n);
	scramble_real(z, n);
	e = 1 / (double)n;
	for (i = 0;i < n;i++)
	{
		z[i] *= e;
	}
}

void mul_hermitian(double* a, double* b, int n)
{
	register int 		k, half = n >> 1;
	register double 	aa, bb, am, bm;

	b[0] *= a[0];
	b[half] *= a[half];
	for (k = 1;k < half;k++)
	{
		aa = a[k];
		bb = b[k];
		am = a[n - k];
		bm = b[n - k];
		b[k] = aa * bb - am * bm;
		b[n - k] = aa * bm + am * bb;
	}
}

void square_hermitian(double* b, int n)
{
	register int 		k, half = n >> 1;
	register double 	c, d;

	b[0] *= b[0];
	b[half] *= b[half];
	for (k = 1;k < half;k++)
	{
		c = b[k];
		d = b[n - k];
		b[n - k] = 2.0*c*d;
		b[k] = (c + d)*(c - d);
	}
}

void fft_real_to_hermitian(double* z, int n)
/* Output is {Re(z^[0]),...,Re(z^[n/2),Im(z^[n/2-1]),...,Im(z^[1]).
*	This is a decimation-in-time, split-radix algorithm.
*/
{
	register double	cc1, ss1, cc3, ss3;
	register int 	is, id, i0, i1, i2, i3, i4, i5, i6, i7, i8,
		a, a3, b, b3, nminus = n - 1, dil, expand;
	register double *x, e;
	int 			nn = n >> 1;
	double 			t1, t2, t3, t4, t5, t6;
	register int 	n2, n4, n8, i, j;

	init_sinCos(n);
	expand = cur_run / n;
	scramble_real(z, n);
	x = z - 1; /* FORTRAN compatibility. */
	is = 1;
	id = 4;
	do
	{
		for (i0 = is;i0 <= n;i0 += id)
		{
			i1 = i0 + 1;
			e = x[i0];
			x[i0] = e + x[i1];
			x[i1] = e - x[i1];
		}
		is = (id << 1) - 1;
		id <<= 2;
	} while (is < n);

	n2 = 2;
	while (nn >>= 1)
	{
		n2 <<= 1;
		n4 = n2 >> 2;
		n8 = n2 >> 3;
		is = 0;
		id = n2 << 1;
		do
		{
			for (i = is;i < n;i += id)
			{
				i1 = i + 1;
				i2 = i1 + n4;
				i3 = i2 + n4;
				i4 = i3 + n4;
				t1 = x[i4] + x[i3];
				x[i4] -= x[i3];
				x[i3] = x[i1] - t1;
				x[i1] += t1;
				if (n4 == 1)
					continue;
				i1 += n8;
				i2 += n8;
				i3 += n8;
				i4 += n8;
				t1 = (x[i3] + x[i4])*SQRTHALF;
				t2 = (x[i3] - x[i4])*SQRTHALF;
				x[i4] = x[i2] - t1;
				x[i3] = -x[i2] - t1;
				x[i2] = x[i1] - t2;
				x[i1] += t2;
			}
			is = (id << 1) - n2;
			id <<= 2;
		} while (is < n);
		dil = n / n2;
		a = dil;
		for (j = 2;j <= n8;j++)
		{
			a3 = (a + (a << 1))&nminus;
			b = a * expand;
			b3 = a3 * expand;
			cc1 = s_cos(b);
			ss1 = s_sin(b);
			cc3 = s_cos(b3);
			ss3 = s_sin(b3);
			a = (a + dil)&nminus;
			is = 0;
			id = n2 << 1;
			do
			{
				for (i = is;i < n;i += id)
				{
					i1 = i + j;
					i2 = i1 + n4;
					i3 = i2 + n4;
					i4 = i3 + n4;
					i5 = i + n4 - j + 2;
					i6 = i5 + n4;
					i7 = i6 + n4;
					i8 = i7 + n4;
					t1 = x[i3] * cc1 + x[i7] * ss1;
					t2 = x[i7] * cc1 - x[i3] * ss1;
					t3 = x[i4] * cc3 + x[i8] * ss3;
					t4 = x[i8] * cc3 - x[i4] * ss3;
					t5 = t1 + t3;
					t6 = t2 + t4;
					t3 = t1 - t3;
					t4 = t2 - t4;
					t2 = x[i6] + t6;
					x[i3] = t6 - x[i6];
					x[i8] = t2;
					t2 = x[i2] - t3;
					x[i7] = -x[i2] - t3;
					x[i4] = t2;
					t1 = x[i1] + t5;
					x[i6] = x[i1] - t5;
					x[i1] = t1;
					t1 = x[i5] + t4;
					x[i5] -= t4;
					x[i2] = t1;
				}
				is = (id << 1) - n2;
				id <<= 2;
			} while (is < n);
		}
	}
}

void giant_to_double(giant x, int sizex, double* z, int L)
{
	register int 	j;

	for (j = sizex;j < L;j++)
	{
		z[j] = 0.0;
	}
	for (j = 0;j < sizex;j++)
	{
		z[j] = x->n[j];
	}
}

int lpt(int n, int* lambda)
/* Returns least power of two greater than n. */
{
	register int	i = 1;

	*lambda = 0;
	while (i < n)
	{
		i <<= 1;
		++(*lambda);
	}
	return(i);
}

double gfloor(double f)
{
	return floor(f);
}

void init_sinCos(int n)
{
	int 		j;
	double 	e = TWOPI / n;

	if (n <= cur_run)
		return;
	cur_run = n;
	if (sinCos)
		free(sinCos);
	sinCos = (double *)malloc(sizeof(double)*(1 + (n >> 2)));
	for (j = 0;j <= (n >> 2);j++)
	{
		sinCos[j] = sin(e*j);
	}
}

double s_cos(int n)
{
	int 	quart = (cur_run >> 2);

	if (n < quart)
		return(s_sin(n + quart));
	return(-s_sin(n - quart));
}

double s_sin(int n)
{
	int 	seg = n / (cur_run >> 2);

	switch (seg)
	{
		case 0: return(sinCos[n]);
		case 1: return(sinCos[(cur_run >> 1) - n]);
		case 2: return(-sinCos[n - (cur_run >> 1)]);
		case 3: return(-sinCos[cur_run - n]);
	}
	return 0;
}

void scramble_real(double* x, int n)
{
	register int 	i, j, k;
	register double	tmp;

	for (i = 0, j = 0;i < n - 1;i++)
	{
		if (i < j)
		{
			tmp = x[j];
			x[j] = x[i];
			x[i] = tmp;
		}
		k = n / 2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}
}
