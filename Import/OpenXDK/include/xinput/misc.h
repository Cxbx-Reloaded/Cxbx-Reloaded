/******************************************************************************/
/*																              */
/* File: misc.h                                                               */
/* Author: bkenwright@xbdev.net                                               */
/* Desc: Misc functions, making our usb/gamepad code lib independent          */
/*                                                                            */
/******************************************************************************/

#ifndef		__XMISC__
#define		__XMISC__




typedef		unsigned char	__u8;
typedef		char			__s8;
typedef		unsigned short	__u16;
typedef		short			__s16;
typedef		unsigned long	__u32;
typedef		int				__s32;

#define     NULL            0

//#include <stdio.h>
//extern char buf[500];
//void dbg(char* str);


/******************************************************************************/
/*																              */
/* StdFunctions                                                               */
/* Didn't want our code being dependent on anything!  For example Sleep(..)   */
/* As its available in the xdk and the openxdk or linux?... Well I could      */
/* write custom functions - giveing us the ultimate in flexiblity!            */
/* We really are now a portible piece of code                                 */
/*                                                                            */
/******************************************************************************/

void xSleep( long timemilliseconds );

void *xmemcpy(void *dest, const void *src, size_t n);

void *xmalloc(size_t size);

void *xmemset(void *s, int c, size_t n);

//unsigned __int64 MyGetTickCount();
unsigned int MyGetTickCount();


/******************************************************************************/
/*																              */
/* Kernel Functions                                                           */
/* Here are the 2 kernal api's from the bios that we need to make this code   */
/* work - everything else can be hand coded but these.  As we need to use     */
/* memory that isn't quite where its suppose to be!  Remember where running   */
/* in protected mode - so for our ohci registers, when we give them an        */
/* address, it has to be the real physical address.  If we where in the       */
/* kernel or writing a kernel bios, we wolndt' need to worry about it..but we */
/* do here as where in the xbe!                                               */
/*                                                                            */
/* These are just wrapper's as we call the exact same function name..but      */
/* without the x at the start.                                                */
/*                                                                            */
/******************************************************************************/

__u32 xMmGetPhysicalAddress(__u32  BaseAddress);
void  xMmLockUnlockBufferPages(__u32 MemoryAddress, __u32 NumberOfBytes, __u32 a);

/******************************************************************************/


#endif		// __XMISC__
