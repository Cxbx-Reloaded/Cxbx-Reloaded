// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xvga_internal.h
// *
// * note : XBox VGA
// *
// ******************************************************************
#ifndef XVGA_DEF_H
#define XVGA_DEF_H

#include "openxdk.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// ******************************************************************
// * vga register flat addresses
// ******************************************************************
static volatile char * const P_ATTR_REG_INDEX = (char * const)0xFD6013c0;
static volatile char * const P_ATTR_REG_DATA  = (char * const)0xFD6013c1;
static volatile char * const P_CRTC_REG_INDEX = (char * const)0xFD6013d4;
static volatile char * const P_CRTC_REG_DATA  = (char * const)0xFD6013d5;
static volatile char * const P_VERTICAL_BLANK = (char * const)0xFD6013DA;
static volatile char * const P_GRA_REG_INDEX  = (char * const)0xFD0c03ce;
static volatile char * const P_GRA_REG_DATA   = (char * const)0xFD0c03cf;
static volatile char * const P_SEQ_REG_INDEX  = (char * const)0xFD0c03c4;
static volatile char * const P_SEQ_REG_DATA   = (char * const)0xFD0c03c5;
static volatile char * const P_MISC_REG       = (char * const)0xFD0c03c2;

// ******************************************************************
// * vga_reg
// ******************************************************************
typedef struct _vga_reg
{
	unsigned short port;
	unsigned char  index;
	unsigned char  value;
}
vga_reg;

// ******************************************************************
// * vga mode definitions
// ******************************************************************
extern vga_reg xvga_mode_256x240[];
extern vga_reg xvga_mode_320x240[];
extern vga_reg xvga_mode_320x200[];

// ******************************************************************
// * mode sizes
// ******************************************************************
extern uint32 MODE256x240SIZE;
extern uint32 MODE320x240SIZE;
extern uint32 MODE320x200SIZE;

// ******************************************************************
// * NVidia registers base / offsets
// ******************************************************************
#define NV_REGBASE				(0xFD000000)
#define NV_PMC					(0x00000000)
#define NV_PFIFO				(0x00002000)
#define NV_FB					(0x00100000)
#define NV_EXTDEV				(0x00101000)
#define	NV_CRTC					(0x00600000)
#define NV_RAMDAC				(0x00680000)
#define NV_FIFO					(0x00800000)
#define NV_MISC                 (0x000C0000)

// ******************************************************************
// * vga registers
// ******************************************************************
static const int ATTR_REG_INDEX = 0x13c0;
static const int ATTR_REG_DATA  = 0x13c1;
static const int CRTC_REG_INDEX = 0x13d4;
static const int CRTC_REG_DATA  = 0x13d5;
static const int FB_REG         = 0x0800;
static const int VERTICAL_BLANK = 0x13DA;
static const int GRA_REG_INDEX  = 0x03ce;
static const int GRA_REG_DATA   = 0x03cf;
static const int SEQ_REG_INDEX  = 0x03c4;
static const int SEQ_REG_DATA   = 0x03c5;
static const int MISC_REG       = 0x03c2;

// ******************************************************************
// * CRTC register accessors
// ******************************************************************
#define CRTC_READ(a)       (*((volatile uint08*)(NV_REGBASE + NV_CRTC + (a))))
#define CRTC_READL(a)      (*((volatile uint32*)(NV_REGBASE + NV_CRTC + (a))))
#define CRTC_WRITE(a,b)     *((volatile uint08*)(NV_REGBASE + NV_CRTC + (a)))  = (b)
#define CRTC_WRITEL(a,b)    *((volatile uint32*)(NV_REGBASE + NV_CRTC + (a)))  = (b)

// ******************************************************************
// * ATTR register accessors
// ******************************************************************
#define ATTR_READ(a)       (*((volatile uint08*)(NV_REGBASE + NV_CRTC + (a))))
#define ATTR_READL(a)      (*((volatile uint32*)(NV_REGBASE + NV_CRTC + (a))))
#define ATTR_WRITE(a,b)     *((volatile uint08*)(NV_REGBASE + NV_CRTC + (a)))  = (b)
#define ATTR_WRITEL(a,b)    *((volatile uint32*)(NV_REGBASE + NV_CRTC + (a)))  = (b)

// ******************************************************************
// * MISC register accessors
// ******************************************************************
#define MISC_READ(a)       (*((volatile uint08*)(NV_REGBASE + NV_MISC + (a))))
#define MISC_READL(a)      (*((volatile uint32*)(NV_REGBASE + NV_MISC + (a))))
#define MISC_WRITE(a,b)     *((volatile uint08*)(NV_REGBASE + NV_MISC + (a)))  = (b)
#define MISC_WRITEL(a,b)    *((volatile uint32*)(NV_REGBASE + NV_MISC + (a)))  = (b)

// ******************************************************************
// * frame buffer
// ******************************************************************
#define FRAMEBUFFER ((void*)(0xF0040240))

#if defined(__cplusplus)
}
#endif

#endif
