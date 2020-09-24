#pragma once

// Mode enum values where:
//
//  0xC0000000 - output mode mask:
//
//    0x00000000 - 480 line SDTV
//    0x40000000 - 525 line SDTV
//    0x80000000 - HDTV
//    0xC0000000 - VGA
//
//  0x10000000 - enable WSS (_16x9)
//  0x20000000 - use SCART output (_RGB)
//
//  0x01000000 - disable DAC A
//  0x02000000 - disable DAC B
//  0x04000000 - disable DAC C
//  0x08000000 - disable DAC D
//
//  0x00FF0000 - register table index
//  0x0000FF00 - CRTC table index
//  0x000000FF - TV table index, based on output mode mask above

#define AV_MODE_TABLE_VERSION  0

#define AV_MODE_OFF                                 0x00000000
#define AV_MODE_640x480_TO_NTSC_M_YC                0x04010101
#define AV_MODE_640x480_TO_NTSC_M_YC_16x9           0x14010101
#define AV_MODE_720x480_TO_NTSC_M_YC                0x04020202
#define AV_MODE_720x480_TO_NTSC_M_YC_16x9           0x14020202
#define AV_MODE_640x480_TO_NTSC_M_RGB               0x20010101
#define AV_MODE_640x480_TO_NTSC_M_RGB_16x9          0x30010101
#define AV_MODE_720x480_TO_NTSC_M_RGB               0x20020202
#define AV_MODE_720x480_TO_NTSC_M_RGB_16x9          0x30020202
#define AV_MODE_640x480_TO_NTSC_J_YC                0x04010103
#define AV_MODE_640x480_TO_NTSC_J_YC_16x9           0x14010103
#define AV_MODE_720x480_TO_NTSC_J_YC                0x04020204
#define AV_MODE_720x480_TO_NTSC_J_YC_16x9           0x14020204
#define AV_MODE_640x480_TO_NTSC_J_RGB               0x20010103
#define AV_MODE_640x480_TO_NTSC_J_RGB_16x9          0x30010103
#define AV_MODE_720x480_TO_NTSC_J_RGB               0x20020204
#define AV_MODE_720x480_TO_NTSC_J_RGB_16x9          0x30020204
#define AV_MODE_640x480_TO_PAL_M_YC                 0x04010105
#define AV_MODE_640x480_TO_PAL_M_YC_16x9            0x14010105
#define AV_MODE_720x480_TO_PAL_M_YC                 0x04020206
#define AV_MODE_720x480_TO_PAL_M_YC_16x9            0x14020206
#define AV_MODE_640x480_TO_PAL_M_RGB                0x20010105
#define AV_MODE_640x480_TO_PAL_M_RGB_16x9           0x30010105
#define AV_MODE_720x480_TO_PAL_M_RGB                0x20020206
#define AV_MODE_720x480_TO_PAL_M_RGB_16x9           0x30020206
#define AV_MODE_640x480_TO_PAL_I_YC                 0x44030307
#define AV_MODE_640x480_TO_PAL_I_YC_16x9            0x54030307
#define AV_MODE_720x480_TO_PAL_I_YC                 0x44040408
#define AV_MODE_720x480_TO_PAL_I_YC_16x9            0x54040408
#define AV_MODE_640x576_TO_PAL_I_YC                 0x44050509
#define AV_MODE_640x576_TO_PAL_I_YC_16x9            0x54050509
#define AV_MODE_720x576_TO_PAL_I_YC                 0x4406060A
#define AV_MODE_720x576_TO_PAL_I_YC_16x9            0x5406060A
#define AV_MODE_640x480_TO_PAL_I_RGB                0x60030307
#define AV_MODE_640x480_TO_PAL_I_RGB_16x9           0x70030307
#define AV_MODE_720x480_TO_PAL_I_RGB                0x60040408
#define AV_MODE_720x480_TO_PAL_I_RGB_16x9           0x70040408
#define AV_MODE_640x576_TO_PAL_I_RGB                0x60050509
#define AV_MODE_640x576_TO_PAL_I_RGB_16x9           0x70050509
#define AV_MODE_720x576_TO_PAL_I_RGB                0x6006060A
#define AV_MODE_720x576_TO_PAL_I_RGB_16x9           0x7006060A
#define AV_MODE_640x480_TO_PAL_60_YC                0x0401010B
#define AV_MODE_640x480_TO_PAL_60_YC_16x9           0x1401010B
#define AV_MODE_720x480_TO_PAL_60_YC                0x0402020C
#define AV_MODE_720x480_TO_PAL_60_YC_16x9           0x1402020C
#define AV_MODE_640x480_TO_PAL_60_RGB               0x2001010B
#define AV_MODE_640x480_TO_PAL_60_RGB_16x9          0x3001010B
#define AV_MODE_720x480_TO_PAL_60_RGB               0x2002020C
#define AV_MODE_720x480_TO_PAL_60_RGB_16x9          0x3002020C
#define AV_MODE_640x480_TO_NTSC_YPrPb               0x0801010D
#define AV_MODE_640x480_TO_NTSC_YPrPb_16x9          0x1801010D
#define AV_MODE_720x480_TO_NTSC_YPrPb               0x0802020E
#define AV_MODE_720x480_TO_NTSC_YPrPb_16x9          0x1802020E
#define AV_MODE_640x480_FPAR_TO_NTSC_M_YC           0x040F0D0F
#define AV_MODE_640x480_FPAR_TO_NTSC_M_YC_16x9      0x140F0D0F
#define AV_MODE_640x480_FPAR_TO_NTSC_M_RGB          0x200F0D0F
#define AV_MODE_640x480_FPAR_TO_NTSC_M_RGB_16x9     0x300F0D0F
#define AV_MODE_640x480_FPAR_TO_NTSC_J_YC           0x040F0D10
#define AV_MODE_640x480_FPAR_TO_NTSC_J_YC_16x9      0x140F0D10
#define AV_MODE_640x480_FPAR_TO_NTSC_J_RGB          0x200F0D10
#define AV_MODE_640x480_FPAR_TO_NTSC_J_RGB_16x9     0x300F0D10
#define AV_MODE_640x480_FPAR_TO_PAL_60_YC           0x040F0D11
#define AV_MODE_640x480_FPAR_TO_PAL_60_YC_16x9      0x140F0D11
#define AV_MODE_640x480_FPAR_TO_PAL_60_RGB          0x200F0D11
#define AV_MODE_640x480_FPAR_TO_PAL_60_RGB_16x9     0x300F0D11
#define AV_MODE_640x480_FPAR_TO_NTSC_YPrPb          0x080F0D12
#define AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9     0x180F0D12
#define AV_MODE_640x480_FPAR_TO_PAL_I_YC            0x44100E13
#define AV_MODE_640x480_FPAR_TO_PAL_I_YC_16x9       0x54100E13
#define AV_MODE_640x480_FPAR_TO_PAL_I_RGB           0x60100E13
#define AV_MODE_640x480_FPAR_TO_PAL_I_RGB_16x9      0x70100E13
#define AV_MODE_640x480_TO_PAL_I_YPrPb              0x48030314
#define AV_MODE_640x480_TO_PAL_I_YPrPb_16x9         0x58030314
#define AV_MODE_720x480_TO_PAL_I_YPrPb              0x48040415
#define AV_MODE_720x480_TO_PAL_I_YPrPb_16x9         0x58040415
#define AV_MODE_640x576_TO_PAL_I_YPrPb              0x48050516
#define AV_MODE_640x576_TO_PAL_I_YPrPb_16x9         0x58050516
#define AV_MODE_720x576_TO_PAL_I_YPrPb              0x48060617
#define AV_MODE_720x576_TO_PAL_I_YPrPb_16x9         0x58060617
#define AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb         0x48100E18
#define AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb_16x9    0x58100E18
#define AV_MODE_640x480_TO_PAL_60_YPrPb             0x08010119
#define AV_MODE_640x480_TO_PAL_60_YPrPb_16x9        0x18010119
#define AV_MODE_720x480_TO_PAL_60_YPrPb             0x0802021A
#define AV_MODE_720x480_TO_PAL_60_YPrPb_16x9        0x1802021A
#define AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb        0x080F0D1B
#define AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb_16x9   0x180F0D1B
#define AV_MODE_640x576_FPAR_TO_PAL_I_YC            0x4412101C
#define AV_MODE_640x576_FPAR_TO_PAL_I_YC_16x9       0x5412101C
#define AV_MODE_640x576_FPAR_TO_PAL_I_RGB           0x6012101C
#define AV_MODE_640x576_FPAR_TO_PAL_I_RGB_16x9      0x7012101C
#define AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb         0x4812101D
#define AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb_16x9    0x5812101D
#define AV_MODE_640x480_TO_VGA                      0x88070701
#define AV_MODE_720x480_TO_VGA                      0x88080801
#define AV_MODE_1280x720_TO_VGA                     0x880B0A02
#define AV_MODE_1920x1080_TO_VGA                    0x880E0C03
#define AV_MODE_640x480_TO_480P                     0x88070701
#define AV_MODE_720x480_TO_480P                     0x88080801
#define AV_MODE_1280x720P_TO_720P                   0x880B0A02
#define AV_MODE_1920x1080I_TO_1080I                 0x880E0C03
#define AV_MODE_640x480_FPAR_TO_480P                0x88110F01

#define AV_MODE_FLAGS_DACA_DISABLE                  0x01000000
#define AV_MODE_FLAGS_DACB_DISABLE                  0x02000000
#define AV_MODE_FLAGS_DACC_DISABLE                  0x04000000
#define AV_MODE_FLAGS_DACD_DISABLE                  0x08000000

#define AV_MODE_FLAGS_WSS                           0x10000000 // _16x9
#define AV_MODE_FLAGS_SCART                         0x20000000 // _RGB
#define AV_MODE_FLAGS_NTSCJ                         0x00000080

#define AV_MODE_OUT_MASK                            0xC0000000
#define AV_MODE_OUT_480SDTV                         0x00000000
#define AV_MODE_OUT_525SDTV                         0x40000000
#define AV_MODE_OUT_HDTV                            0x80000000
#define AV_MODE_OUT_VGA                             0xC0000000

// Register Tables
const ULONG AvpRegisters[][26] =
{
	{ /* offset */ 0x00680898, 0x0068089C, 0x006808C0, 0x006808C4, 0x0068084C, 0x00680630, 0x00680800, 0x00680804, 0x00680808, 0x0068080C, 0x00680810, 0x00680814, 0x00680818, 0x00680820, 0x00680824, 0x00680828, 0x0068082C, 0x00680830, 0x00680834, 0x00680838, 0x00680848, 0x00680680, 0x00680684, 0x00680688, 0x0068068C, 0x00680690 }, /* offset */
	{ /*      1 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F2, 0x000001F4, 0x00000000, 0x000001DF, 0x0000027F, 0x000003A7, 0x00000257, 0x000002F3, 0x00000333, 0x00000000, 0x0000027F, 0x10100111, 0x000C6ED0, 0x0000020D, 0x0000009B, 0x0000026C, 0x00000000 }, /*  1 */
	{ /*      2 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F2, 0x000001F4, 0x00000000, 0x000001DF, 0x000002CF, 0x000003A7, 0x000002A7, 0x0000031B, 0x0000035B, 0x00000000, 0x000002CF, 0x10100111, 0x000DF05C, 0x0000020D, 0x000000AE, 0x000002B8, 0x00000000 }, /*  2 */
	{ /*      3 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F4, 0x000001F6, 0x00000000, 0x000001DF, 0x0000027F, 0x0000035F, 0x00000257, 0x000002CF, 0x0000030F, 0x00000000, 0x0000027F, 0x10100111, 0x000F387C, 0x00000271, 0x000000BE, 0x000002F8, 0x00000000 }, /*  3 */
	{ /*      4 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F4, 0x000001F6, 0x00000000, 0x000001DF, 0x000002CF, 0x0000035F, 0x000002AF, 0x0000030B, 0x0000034B, 0x00000000, 0x000002CF, 0x10100111, 0x0010D2A4, 0x00000271, 0x000000D2, 0x00000348, 0x00000000 }, /*  4 */
	{ /*      5 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x0000023F, 0x00000270, 0x0000023F, 0x00000256, 0x00000258, 0x00000000, 0x0000023F, 0x0000027F, 0x0000035F, 0x00000257, 0x000002CF, 0x0000030F, 0x00000000, 0x0000027F, 0x10100111, 0x000F07A8, 0x00000271, 0x0000009D, 0x00000276, 0x00000000 }, /*  5 */
	{ /*      6 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x0000023F, 0x00000270, 0x0000023F, 0x00000256, 0x00000258, 0x00000000, 0x0000023F, 0x000002CF, 0x0000035F, 0x000002AF, 0x0000030B, 0x0000034B, 0x00000000, 0x000002CF, 0x10100111, 0x0010E62C, 0x00000271, 0x000000B1, 0x000002C4, 0x00000000 }, /*  6 */
	{ /*      7 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001E8, 0x000001EE, 0x00000000, 0x000001DF, 0x000002CF, 0x00000359, 0x0000029F, 0x000002E1, 0x00000320, 0x00000000, 0x000002CF, 0x10100011, 0x0000035A, 0x00000001, 0x000000AB, 0x000002AE, 0x00000001 }, /*  7 */
	{ /*      8 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001E8, 0x000001EE, 0x00000000, 0x000001DF, 0x000002CF, 0x00000359, 0x0000029F, 0x000002E1, 0x00000320, 0x00000000, 0x000002CF, 0x10100111, 0x0000035A, 0x00000001, 0x000000AB, 0x000002AE, 0x00000001 }, /*  8 */
	{ /*      9 */ 0x0AA94000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x000002CF, 0x000002ED, 0x000002CF, 0x000002D4, 0x000002D9, 0x00000000, 0x000002CF, 0x000004FF, 0x00000671, 0x000004FF, 0x00000545, 0x00000595, 0x000000A0, 0x0000045F, 0x10100011, 0x00000672, 0x00000001, 0x0000014A, 0x00000528, 0x00000001 }, /*  9 */
	{ /*      A */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x000002CF, 0x000002ED, 0x000002CF, 0x000002D4, 0x000002D9, 0x00000000, 0x000002CF, 0x000004FF, 0x00000671, 0x000004CF, 0x00000545, 0x00000595, 0x00000000, 0x000004FF, 0x10100011, 0x00000672, 0x00000001, 0x0000014A, 0x00000528, 0x00000001 }, /*  A */
	{ /*      B */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x000002CF, 0x000002ED, 0x000002CF, 0x000002D4, 0x000002D9, 0x00000000, 0x000002CF, 0x000004FF, 0x00000671, 0x000004FF, 0x00000545, 0x00000595, 0x00000000, 0x000004FF, 0x10100111, 0x00000672, 0x00000001, 0x0000014A, 0x00000528, 0x00000001 }, /*  B */
	{ /*      C */ 0x071AE000, 0x07183800, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x00000437, 0x00000464, 0x0000043C, 0x0000043C, 0x00000446, 0x00000000, 0x00000437, 0x0000077F, 0x00000897, 0x000007AA, 0x000007AB, 0x00000803, 0x000000F0, 0x0000068F, 0x10133011, 0x00000898, 0x00000001, 0x000001B8, 0x000006E0, 0x00000001 }, /*  C */
	{ /*      D */ 0x10000000, 0x07183800, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x00000437, 0x00000464, 0x0000043C, 0x0000043C, 0x00000446, 0x00000000, 0x00000437, 0x0000077F, 0x00000897, 0x00000759, 0x000007AB, 0x00000803, 0x00000000, 0x0000077F, 0x10133011, 0x00000898, 0x00000001, 0x000001B8, 0x000006E0, 0x00000001 }, /*  D */
	{ /*      E */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000003, 0x00000437, 0x00000464, 0x0000043B, 0x0000043B, 0x00000445, 0x00000000, 0x00000437, 0x0000077F, 0x00000897, 0x000007AB, 0x000007AC, 0x00000804, 0x00000000, 0x0000077F, 0x10133111, 0x00000898, 0x00000001, 0x000001B8, 0x000006E0, 0x00000001 }, /*  E */
	{ /*      F */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F2, 0x000001F4, 0x00000000, 0x000001DF, 0x0000027F, 0x000003A7, 0x000002A7, 0x000002F3, 0x00000333, 0x00000000, 0x0000027F, 0x10100111, 0x000DF05C, 0x0000020D, 0x000000AE, 0x000002B8, 0x00000000 }, /*  F */
	{ /*     10 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001F4, 0x000001F6, 0x00000000, 0x000001DF, 0x0000027F, 0x0000035F, 0x000002A7, 0x000002CF, 0x0000030F, 0x00000000, 0x0000027F, 0x10100111, 0x000DF05C, 0x0000020D, 0x000000AE, 0x000002B8, 0x00000000 }, /* 10 */
	{ /*     11 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x000001DF, 0x0000020C, 0x000001DF, 0x000001E8, 0x000001EE, 0x00000000, 0x000001DF, 0x000002CF, 0x00000359, 0x000002CF, 0x000002DF, 0x0000031E, 0x00000020, 0x000002AD, 0x10100011, 0x0000035A, 0x00000001, 0x000000AB, 0x000002AE, 0x00000001 }, /* 11 */
	{ /*     12 */ 0x10000000, 0x10000000, 0x00000000, 0x40801080, 0x00801080, 0x00000002, 0x0000023F, 0x00000270, 0x0000023F, 0x00000256, 0x00000258, 0x00000000, 0x0000023F, 0x0000027F, 0x0000035F, 0x00000257, 0x000002CF, 0x0000030F, 0x00000000, 0x0000027F, 0x10100111, 0x000F07A8, 0x00000271, 0x0000009D, 0x00000276, 0x00000000 }, /* 12 */
};

const UCHAR AvpSRXRegisters[] =
{
	0x03, 0x21, 0x0F, 0x00, 0x06,
};

const UCHAR AvpGRXRegisters[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
};

const UCHAR AvpARXRegisters[] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x4A, 0x0F, 0x00, 0x00,
};

const UCHAR AvpCRTCRegisters[][34] =
{
	{ /* offset */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x20, 0x25, 0x2D, 0x33, 0x39, 0x41 }, /* offset */
	{ /*      1 */ 0x70, 0x4F, 0x4F, 0x94, 0x5D, 0xBF, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0x04, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      1 */
	{ /*      2 */ 0x70, 0x59, 0x59, 0x94, 0x62, 0xA4, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0x04, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      2 */
	{ /*      3 */ 0x67, 0x4F, 0x4F, 0x8B, 0x59, 0xBB, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x06, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      3 */
	{ /*      4 */ 0x67, 0x59, 0x59, 0x8B, 0x5E, 0xBF, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x06, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      4 */
	{ /*      5 */ 0x67, 0x4F, 0x4F, 0x8B, 0x59, 0xBB, 0x6F, 0xF0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x08, 0x3F, 0x00, 0x00, 0x3F, 0x70, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      5 */
	{ /*      6 */ 0x67, 0x59, 0x59, 0x8B, 0x5E, 0xBF, 0x6F, 0xF0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x08, 0x3F, 0x00, 0x00, 0x3F, 0x70, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      6 */
	{ /*      7 */ 0x59, 0x4F, 0x4F, 0x9D, 0x51, 0x39, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x0E, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      7 */
	{ /*      8 */ 0x63, 0x59, 0x59, 0x87, 0x5B, 0xA3, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x0E, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      8 */
	{ /*      9 */ 0x78, 0x4F, 0x4F, 0x9C, 0x57, 0xA1, 0xFC, 0x1F, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE4, 0x09, 0xDF, 0x00, 0x00, 0xDF, 0xFD, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      9 */
	{ /*      A */ 0xC8, 0x9F, 0x9F, 0x8C, 0xA7, 0x31, 0xEC, 0xF0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD4, 0x09, 0xCF, 0x00, 0x00, 0xCF, 0xED, 0xE3, 0xFF, 0x00, 0x38, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      A */
	{ /*      B */ 0x67, 0x4F, 0x4F, 0x8B, 0x54, 0xBF, 0x03, 0x11, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0x06, 0xEF, 0x00, 0x00, 0xEF, 0x04, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0x36, 0x00 }, /*      B */
	{ /*      C */ 0x04, 0xEF, 0xEF, 0x88, 0xF4, 0x3F, 0x2F, 0xF0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 0x02, 0x1B, 0x00, 0x00, 0x1B, 0x30, 0xE3, 0xFF, 0x00, 0x38, 0x05, 0x80, 0x00, 0x01, 0x11, 0x10, 0x00 }, /*      C */
	{ /*      D */ 0x70, 0x4F, 0x4F, 0x94, 0x5D, 0xBF, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF2, 0x04, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      D */
	{ /*      E */ 0x67, 0x4F, 0x4F, 0x8B, 0x59, 0xBF, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x06, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x00, 0x00, 0x11, 0xFF, 0x00 }, /*      E */
	{ /*      F */ 0x61, 0x57, 0x57, 0x85, 0x59, 0xBF, 0x0B, 0x3E, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x0E, 0xDF, 0x00, 0x00, 0xDF, 0x0C, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*      F */
	{ /*     10 */ 0x67, 0x4F, 0x4F, 0x94, 0x59, 0xBF, 0x6F, 0xF0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x08, 0x3F, 0x00, 0x00, 0x3F, 0x70, 0xE3, 0xFF, 0x00, 0x3A, 0x05, 0x80, 0x10, 0x00, 0x11, 0xFF, 0x00 }, /*     10 */
};

typedef struct
{
    ULONG  AvInfo;
    USHORT Width;
    USHORT Height;
    ULONG  DisplayMode;
} XB_DisplayMode;

const XB_DisplayMode g_DisplayModes[] =
{
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_720p,      1280, 720,  AV_MODE_1280x720P_TO_720P                 },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },                                                                                                                                
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920, 1080, AV_MODE_1920x1080I_TO_1080I               },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920,  540, AV_MODE_1920x1080I_TO_1080I               },

    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB_16x9   },

    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC_16x9    },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC_16x9    },

    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_720p,      1280, 720,  AV_MODE_1280x720P_TO_720P                 },                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },                                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920, 1080, AV_MODE_1920x1080I_TO_1080I               },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920,  540, AV_MODE_1920x1080I_TO_1080I               },                                                                                                                   

    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB_16x9   },

    // All other AV packs are the same.                                                                               
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC_16x9    },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC_16x9    },

    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },                     

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb_16x9  },    
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb_16x9  },    
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb_16x9  },    
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YPrPb_16x9      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YPrPb_16x9      },                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YPrPb_16x9      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YPrPb_16x9      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb_16x9 },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb_16x9 },
    
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_RGB_16x9         },                                                                                                               
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_RGB_16x9    },    
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB_16x9    },    
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB_16x9    },    
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_RGB_16x9        },                                                                                                                
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB_16x9   },

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YC_16x9          },   
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YC_16x9     },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YC_16x9     },    
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YC_16x9     },    
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YC_16x9         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YC_16x9         },                                                                                                                                                                                                                   
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YC_16x9         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YC_16x9         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YC         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YC_16x9    },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YC         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YC_16x9    },                                                                                        
    { 0xFFFFFFFF,                                                                                                                                             0,    0,    0                                         },
};

const uint32_t g_DisplayModeCount = sizeof(g_DisplayModes) / sizeof(XB_DisplayMode);
