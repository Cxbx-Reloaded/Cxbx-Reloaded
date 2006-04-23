// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xnvidia.h
// *
// * note : XBox NVidia Chip
// *
// ******************************************************************
#ifndef XNVIDIA_H
#define XNVIDIA_H

#ifdef  __cplusplus
extern "C" {
#endif

// ******************************************************************
// * Accessible Functions
// ******************************************************************
void    NVSetScreenAddress(void);
void    NVSetBPP(int mode);
int     NVSetVideoMode(u32 dwResolution, u32 dwPixelFormat );
int     NVSetFlickerFilter(DWORD dwLevel);
void    NVSetClippingRectangle(u32 x1,u32 y1,u32 x2,u32 y2);

// ******************************************************************
// * AvSetDisplayMode pixel formats
// ******************************************************************
#define PIXEL_16BITS_555    0x10
#define PIXEL_16BITS_565    0x11
#define PIXEL_32BITS        0x12

// ******************************************************************
// * AvSetDisplayMode resolutions (only 640 is tested)
// ******************************************************************
#define RESOLUTION_640      0x20010101
#define RESOLUTION_720      0x20020101
#define RESOLUTION_800      0x20030101
#define RESOLUTION_1024     0x200B0101

// ******************************************************************
// * Parameters for AvSendTVEncoderOption
// ******************************************************************
#define AV_OPTION_SET_MACROVISION       0x01
#define AV_OPTION_CONFIRM_MACROVISION   0x0A
#define AV_OPTION_SET_FLICKER_FILTER    0x0B

// ******************************************************************
// * NVidia registers base / offsets
// ******************************************************************
#define NV_REGBASE              (0xFD000000)
#define NV_PMC                  (0x000000)
#define NV_PFIFO                (0x002000)      
#define NV_FB                   (0x100000)      
#define NV_EXTDEV               (0x101000)      
#define NV_CRTC                 (0x600000)      
#define NV_RAMDAC               (0x680000)      
#define NV_FIFO                 (0x800000)      

// ******************************************************************
// * CRTC register accessors
// ******************************************************************
#define CRTC_WRITE(a,b)         *((volatile u8*)    (NV_REGBASE + NV_CRTC + (a))) = (b)
#define CRTC_READ(a)            (*((volatile u8*)   (NV_REGBASE + NV_CRTC + (a))))
#define CRTC_WRITEL(a,b)        *((volatile u32*)   (NV_REGBASE + NV_CRTC + (a))) = (b)
#define CRTC_READL(a)           (*((volatile u32*)  (NV_REGBASE + NV_CRTC + (a))))

// ******************************************************************
// * Macros to get pointers to FIFO regs
// ******************************************************************
#define NVPTR_ROP       ((PNV_ROP)(NV_REGBASE + NV_FIFO + 0x0000))
#define NVPTR_PATTERN   ((PNV_PATTERN)(NV_REGBASE + NV_FIFO + 0x4000))
#define NVPTR_CLIP      ((PNV_CLIP)(NV_REGBASE + NV_FIFO + 0x2000))
#define NVPTR_SCREENBLT ((PNV_SCREENBLT)(NV_REGBASE + NV_FIFO + 0x8000))
#define NVPTR_BLIT      ((PNV_BLIT)(NV_REGBASE + NV_FIFO + 0x6000))
#define NVPTR_BITMAP    ((PNV_BITMAP)(NV_REGBASE + NV_FIFO + 0xA000))
#define NVPTR_LINE      ((PNV_LINE)(NV_REGBASE + NV_FIFO + 0xC000))

// ******************************************************************
// * Registers
// ******************************************************************
#define NV_INPUT_STATUS         (0x13DA)
#define NV_CRTC_FB_ADDR         (0x800)             // Display start address
#define NV_CRTC_INDEX           (0x13D4)
#define NV_CRTC_DATA            (0x13D5)
#define NV_CRTC_REGS_LOCK       0x1F
#define NV_CRTC_UNLOCK_VALUE    0x57
#define NV_CRTC_LOCK_VALUE      0x99
#define NV_CRTC_PIXEL           (0x28)

// ******************************************************************
// * Pixel / TV Mode bits
// ******************************************************************
#define NV_TV           0x80            
#define NV_VGA          0x00
#define NV_PAL          0x40
#define NV_NTSC         0x00
#define NV_VGA          0x00        // format
#define NV_8BPP         0x01
#define NV_16BPP        0x02
#define NV_32BPP        0x03
#define NV_BPP_MASK     (~0x03)     // 


//-- FIFO Regs -----------------------------------------------------------------

// Macro to wait until the GPU has enough FIFO slots
#define NV_WAIT_FIFO(Ptr, Cnt) while( ( ((Ptr)->FifoFree) / 4) < (Cnt) );

// FIFO + 0x0000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused;
    ULONG   Reserved1[0x0BB];
    ULONG   Rop3;
} NV_ROP, *PNV_ROP;

// FIFO + 0x4000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused;
    ULONG   Reserved1[0x0BD];
    ULONG   Shape;
    ULONG   Reserved2[0x001];
    ULONG   Color0;
    ULONG   Color1;
    ULONG   Monochrome[2];
} NV_PATTERN, *PNV_PATTERN;

// FIFO + 0x2000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused;
    ULONG   Reserved1[0x0BB];
    ULONG   TopLeft;
    ULONG   WidthHeight;
} NV_CLIP, *PNV_CLIP;

/* 
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused[1];
    ULONG   Reserved1[0x0BC];
    ULONG   Color;
    ULONG   Reserved3[0x03E];
    ULONG   TopLeft;
    ULONG   WidthHeight;
} NV_RECTANGLE, *PNV_RECTANGLE;
*/

// FIFO + 0x8000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused;
    ULONG   Reserved1[0x0BB];
    ULONG   TopLeftSrc;
    ULONG   TopLeftDst;
    ULONG   WidthHeight;
} NV_SCREENBLT, *PNV_SCREENBLT;

// FIFO + 0x6000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused[1];
    ULONG   Reserved1[0x0BC];
    ULONG   TopLeft;
    ULONG   WidthHeight;
    ULONG   WidthHeightIn;
    ULONG   Reserved2[0x03C];
    ULONG   Pixels;
} NV_BLIT, *PNV_BLIT;

// FIFO + 0xA000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused;
    ULONG   Reserved1[0x0BB];
    ULONG   Reserved2[0x03F];
    ULONG   Color1A;
    struct
    {
        ULONG   TopLeft;
        ULONG   WidthHeight;
    } UnclippedRectangle[64];
    ULONG   Reserved3[0x07D];
    struct
    {
        ULONG   TopLeft;
        ULONG   BottomRight;
    } ClipB;
    ULONG   Color1B;
    struct
    {
        ULONG   TopLeft;
        ULONG   BottomRight;
    } ClippedRectangle[64];
    ULONG   Reserved4[0x07B];
    struct
    {
        ULONG   TopLeft;
        ULONG   BottomRight;
    } ClipC;
    ULONG   Color1C;
    ULONG   WidthHeightC;
    ULONG   PointC;
    ULONG   MonochromeData1C;
    ULONG   Reserved5[0x0F9];
    struct
    {
        ULONG   TopLeft;
        ULONG   BottomRight;
    } ClipD;
    ULONG   Color1D;
    ULONG   WidthHeightInD;
    ULONG   WidthHeightOutD;
    ULONG   PointD;
    ULONG   MonochromeData1D;
    ULONG   Reserved6[0x0F8];
    struct
    {
        ULONG   TopLeft;
        ULONG   BottomRight;
    } ClipE;
    ULONG   Color0E;
    ULONG   Color1E;
    ULONG   WidthHeightInE;
    ULONG   WidthHeightOutE;
    ULONG   PointE;
    ULONG   MonochromeData01E;
} NV_BITMAP, *PNV_BITMAP;

// FIFO + 0xC000
typedef volatile struct
{
    ULONG   Reserved0[4];
    USHORT  FifoFree;
    USHORT  Unused[1];
    ULONG   Reserved1[0x0BC];
    ULONG   Color;              // source color                     0304-0307
    ULONG   Reserved2[0x03E];
    struct {                    // start aliased methods in array   0400-
        ULONG   Point0;         // y_x S16_S16 in pixels            0-   3
        ULONG   Point1;         // y_x S16_S16 in pixels            4-   7
    } Lin[16];                  // end of aliased methods in array  -047f
    struct {                    // start aliased methods in array   0480-
        ULONG   Point0X;        // in pixels, 0 at left             0-   3
        ULONG   Point0Y;        // in pixels, 0 at top              4-   7
        ULONG   Point1X;        // in pixels, 0 at left             8-   b
        ULONG   Point1Y;        // in pixels, 0 at top              c-   f
    } Lin32[8];                 // end of aliased methods in array  -04ff
    ULONG   PolyLin[32];        // y_x S16_S16 in pixels            0500-057f
    struct {                    // start aliased methods in array   0580-
        ULONG   x;              // in pixels, 0 at left             0-   3
        ULONG   y;              // in pixels, 0 at top              4-   7
    } PolyLin32[16];            // end of aliased methods in array  -05ff
    struct {                    // start aliased methods in array   0600-
        ULONG   Color;          // source color                     0-   3
        ULONG   Point;          // y_x S16_S16 in pixels            4-   7
    } ColorPolyLin[16];         // end of aliased methods in array  -067f
} NV_LINE, *PNV_LINE;

#ifdef  __cplusplus
};
#endif

#endif // XNVIDIA_H
