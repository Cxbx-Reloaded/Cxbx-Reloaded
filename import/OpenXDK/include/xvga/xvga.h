// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xvga.h
// *
// * note : XBox VGA
// *
// ******************************************************************
#ifndef XVGA_H
#define XVGA_H

#include "openxdk.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// ******************************************************************
// * xvga video modes
// ******************************************************************
typedef enum _xvga_mode
{
    XVGA_MODE_256x240 = 0,
    XVGA_MODE_320x240,
    XVGA_MODE_320x200
}
xvga_mode;

// ******************************************************************
// * set video mode
// ******************************************************************
extern void xvga_setmode(xvga_mode mode);

// ******************************************************************
// * wait for vblank / end vblank
// ******************************************************************
extern void xvga_wait_vblank();
extern void xvga_wait_vblank_end();

#if defined(__cplusplus)
}
#endif

#endif
