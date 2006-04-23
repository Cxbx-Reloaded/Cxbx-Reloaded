// ******************************************************************
// *
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : openxdk.h
// *
// * note : Welcome To Heaven
// *
// ******************************************************************
#ifndef OPENXDK_H
#define OPENXDK_H

// ******************************************************************
// * OpenXDK Version Number
// ******************************************************************
#define OPENXDK_VERSION "0.1.0-Pre1"

// ******************************************************************
// * OpenXDK Full Title
// ******************************************************************
#define OPENXDK_TITLE "OpenXDK " OPENXDK_VERSION

// ******************************************************************
// * Humor caustik with some of his favorite typedefs
// ******************************************************************
typedef signed int     sint;
typedef unsigned int   uint;

typedef char           int08;
typedef short          int16;
typedef long           int32;

typedef unsigned char  uint08;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef signed char    sint08;
typedef signed short   sint16;
typedef signed long    sint32;

// ******************************************************************
// * Humor Bigboy with some of his favorite typedefs  :)
// ******************************************************************
typedef unsigned char       byte;
typedef unsigned char       u8;
typedef signed char         s8;
typedef unsigned short      u16;
typedef signed short        s16;
typedef unsigned int        u32;
typedef signed int          s32;

// ******************************************************************
// * XBox Kernel
// ******************************************************************
#include "xboxkrnl/xboxkrnl.h"

// ******************************************************************
// * XHal
// ******************************************************************
#include "xhal/xhal.h"

// ******************************************************************
// * XVGA
// ******************************************************************
#include "xvga/xvga.h"

// ******************************************************************
// * XUSB (being removed...)
// ******************************************************************
//#include "xusb/xusb.h"

// ******************************************************************
// * XINPUT (Gamepad Support)
// ******************************************************************
//#include "xinput/xinput.h"



#endif




