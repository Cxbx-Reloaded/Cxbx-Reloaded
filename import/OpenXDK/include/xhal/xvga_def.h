// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xvga_def.h
// *
// * note : XBox VGA (#defines)
// *
// ******************************************************************
#ifndef XVGA_DEF_H
#define XVGA_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif
    
extern u8   SystemFont[];      // handy to use system font
extern u8  *pFont;             // point to the system Font by default. You can change it to your own
extern u32  pScreenBuffer[];   // Our screen (software emulated for LOW res just now)
extern u32  FrontBuffer;       // Current screen address (visible)
extern u32  BackBuffer;        // Current back buffer
extern u32  _FrontBuffer;      // Current screen address (visible)
extern u32  _BackBuffer;       // Current back buffer
extern u32  _Framebuffer;

// ******************************************************************
// * Internal
// ******************************************************************
#define XBV_BASE            0xfd601000
#define XBV_ATTR_REG_INDEX  0xfd6013c0
#define XBV_ATTR_REG_DATA   0xfd6013c1
#define XBV_ATTR_REG_INDEX2 0xfd6013c2
#define XBV_ATTR_REG_INDEX3 0xfd6013c3
#define XBV_CRTC_REG_INDEX  0xfd6013d4
#define XBV_CRTC_REG_DATA   0xfd6013d5
#define XBV_GRA_REG_INDEX   0xfd0C03ce
#define XBV_GRA_REG_DATA    0xfd0C03cf
#define XBV_SEQ_REG_INDEX   0xfd0C03c4
#define XBV_SEQ_REG_DATA    0xfd0C03c5
#define XBV_MISC_REG        0xfd0C03c2
#define VBL                 0xfd6013da

#define XBV_COLOUR_REG      0xfd6013c8
#define XBV_COLOUR_DATA     0xfd6013c9

#define XBOX_SCREENRAM      0x8003d000      // Xbox screen RAM starts here (power on default)

// ******************************************************************
// * System flags
// ******************************************************************
#define XHAL_320SCREEN      (1)             // Software emulation of 320x??? mode
#define YHAL_200SCREEN      (2)             // Center 200 screen inside a 240 one

// This might be able to be safely removed now
extern int _fltused;

// ******************************************************************
// * Register
// ******************************************************************
typedef struct
{
    short           port;
    unsigned char   index;
    unsigned char   value;
}
Register;

#define ATTRCON_ADDR    0x3c0
#define MISC_ADDR       0x3c2
#define VGAENABLE_ADDR  0x3c3
#define SEQ_ADDR        0x3c4
#define GRACON_ADDR     0x3ce
#define CRTC_ADDR       0x3d4
#define STATUS_ADDR     0x3da

/*
    xxxxADDR defines the base port number used to access VGA component xxxx,
    and is defined for xxxx =
        ATTRCON     -   Attribute Controller
        MISC        -   Miscellaneous Register
        VGAENABLE   -   VGA Enable Register
        SEQ         -   Sequencer
        GRACON      -   Graphics Controller
        CRTC        -   Cathode Ray Tube Controller
        STATUS      -   Status Register
*/

// ******************************************************************
// * VGA Registers
// ******************************************************************
#define VGAREG_ACTL_ADDRESS            0x3c0
#define VGAREG_ACTL_WRITE_DATA         0x3c0
#define VGAREG_ACTL_READ_DATA          0x3c1

#define VGAREG_INPUT_STATUS            0x3c2
#define VGAREG_WRITE_MISC_OUTPUT       0x3c2
#define VGAREG_VIDEO_ENABLE            0x3c3
#define VGAREG_SEQU_ADDRESS            0x3c4
#define VGAREG_SEQU_DATA               0x3c5

#define VGAREG_PEL_MASK                0x3c6
#define VGAREG_DAC_STATE               0x3c7
#define VGAREG_DAC_READ_ADDRESS        0x3c7
#define VGAREG_DAC_WRITE_ADDRESS       0x3c8
#define VGAREG_DAC_DATA                0x3c9

#define VGAREG_READ_FEATURE_CTL        0x3ca
#define VGAREG_READ_MISC_OUTPUT        0x3cc

#define VGAREG_GRDC_ADDRESS            0x3ce
#define VGAREG_GRDC_DATA               0x3cf

#define VGAREG_MDA_CRTC_ADDRESS        0x3b4
#define VGAREG_MDA_CRTC_DATA           0x3b5
#define VGAREG_VGA_CRTC_ADDRESS        0x3d4
#define VGAREG_VGA_CRTC_DATA           0x3d5

#define VGAREG_MDA_WRITE_FEATURE_CTL   0x3ba
#define VGAREG_VGA_WRITE_FEATURE_CTL   0x3da
#define VGAREG_ACTL_RESET              0x3da

#define VGAREG_MDA_MODECTL             0x3b8
#define VGAREG_CGA_MODECTL             0x3d8
#define VGAREG_CGA_PALETTE             0x3d9

/* Video memory */
#define VGAMEM_GRAPH 0xA000
#define VGAMEM_CTEXT 0xB800
#define VGAMEM_MTEXT 0xB000

Register Mode320x200[];

// ******************************************************************
// * functions needed for xvga.c
// ******************************************************************
void outportb(int port, unsigned char data);
void readyVgaRegs(void);
void outReg(Register r);
void vga_set_reg( int port, int reg, int data );
void vga_set_color( int reg, int R, int G, int B );

#ifdef  __cplusplus
}
#endif

#endif  // XVGA_DEF_H


