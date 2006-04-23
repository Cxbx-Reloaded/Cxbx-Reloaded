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

#ifdef  __cplusplus
extern "C" {
#endif

#include "openxdk.h"

// ******************************************************************
// * User Variables
// ******************************************************************
extern  u32 g_ScreenWidth;                  // Current Screen Width
extern  u32 g_ScreenHeight;                 // Current Screen Height
extern  u32 g_nFlags;
extern  u32 g_nInk;
extern  u32 g_nPaper;
extern  u32 g_nFontFlags;

// ******************************************************************
// * ScreenInfo (returned by GetScreen())
// ******************************************************************
typedef struct _ScreenInfo
{
    u32 ScreenAddress;
    u32 lpitch;
}
ScreenInfo;

// **************************************************************
// * User Functions
// **************************************************************
void       vga_init_mode(int Mode);    // Init VGA screen to a selected mode
void       vga_vsync(void);            // Wait for vertical blank
void       vga_flip(void);             // Flip buffers
ScreenInfo vga_get_screen_info(void);  // get screen base address

// **************************************************************
// * For debugging purposes (serious 2D should use xgfx2d lib)
// **************************************************************
void       vga_clear( void );                          // Clear screen
void       vga_box( int x1,int y1, int x2,int y2 );    // Draw wireframe BOX (WHITE)
void       vga_print( int x, int y, char* pText );     // Draw text using SYSTEM font (32bit modes only just now)
void       vga_testpattern( int shift);                // Draw a test pattern/colour to the screen

// **************************************************************
// * Misc things... will be used later
// **************************************************************
void        vga_set_color( int reg, int R, int G, int B );
void        vga_set_reg( int port, int reg, int data );

// ******************************************************************
// * Pixel resolutions
// ******************************************************************
#define RES_320X200             (1)     // Hardware mode *NOT YET*
#define RES_320X240             (2)     // Software emulated
#define RES_640X200             (3)     // Hardware mode *NOT YET*
#define RES_640X240             (4)     // Hardware mode *NOT YET*
#define RES_640X400             (5)     // Hardware mode *NOT YET*
#define RES_640X480             (6)     // Hardware mode *NOT YET*
#define RES_720X480             (7)     // Hardware mode *NOT YET*
#define RES_800X600             (8)     // Hardware mode *NOT YET*
#define RES_1024X768            (9)     // Hardware mode *NOT YET*
#define RES_MASK                (15)

// ******************************************************************
// * Bit depths
// ******************************************************************
#define _32BITCOLOUR            (0x00000000)
#define _16BITCOLOUR            (0x00000010)
#define _8BITCOLOUR             (0x00000020)
#define COLOUR_MASK             (0x00000030)

// ******************************************************************
// * Extra display flags
// ******************************************************************
#define _DISPLAY_DEBUG          (0x80000000)                    // display into debug mode (single buffer system)

// ******************************************************************
// * Easy to use screen modes
// ******************************************************************
#define MODE_320x200x32         (RES_320X200|_32BITCOLOUR)      // not yet
#define MODE_320x240x32         (RES_320X240|_32BITCOLOUR)
#define MODE_640x200x32         (RES_320X200|_32BITCOLOUR)      // not yet
#define MODE_640x240x32         (RES_320X240|_32BITCOLOUR)
#define MODE_640x400x32         (RES_640X400|_32BITCOLOUR)      // not yet
#define MODE_640x480x32         (RES_640X480|_32BITCOLOUR)      // not yet
#define MODE_320x200x16         (RES_320X200|_16BITCOLOUR)      // not yet
#define MODE_320x240x16         (RES_320X240|_16BITCOLOUR)      // not yet
#define MODE_640x200x16         (RES_320X200|_16BITCOLOUR)      // not yet
#define MODE_640x240x16         (RES_320X240|_16BITCOLOUR)      // not yet
#define MODE_640x400x16         (RES_640X400|_16BITCOLOUR)      // not yet
#define MODE_640x480x16         (RES_640X480|_16BITCOLOUR)      // not yet

// this is really to be used with the character map, so debug can be seen as soon as its output.. single buffered.
#define MODE_640x480x32_DEBUG   (RES_640X480|_32BITCOLOUR|_DISPLAY_DEBUG)       // not yet

// ******************************************************************
// * For print
// ******************************************************************
#define FONT_SOLID          (1)             // render font with SOLID background (with paper colour)
#define FONT_WRAP           (2)             // Wrap PRINT's when they go off the screen

#ifdef  __cplusplus
}
#endif

#endif  // XVGA_H
