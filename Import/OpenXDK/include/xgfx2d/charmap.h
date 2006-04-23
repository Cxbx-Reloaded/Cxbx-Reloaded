// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : charmap.h
// *
// * note : Character map format : U16 per entry
// *       
// *        TxPPPIII_CCCCCCCC
// *        T = character touched flag
// *        x = unused
// *        P = paper colour
// *        I = Ink colour
// *        C = character
// *
// ******************************************************************

#ifndef		__CHARMAP_H_
#define		__CHARMAP_H_

#include <openxdk.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif



// **************************************************************************************************************
//
// constants: #defines, enums etc.
//
// **************************************************************************************************************
#define	CHARMAP_DEBUG			0x80000000		// character map debug mode, force update after every write 
												// slow, but visible. Does NOT update on single character outs

#define	MAP_TOUCHED				0x8000			// this character has been touched, and needs redrawing

#define	FONT_WIDTH				8				// our assumed character width
#define	FONT_HEIGHT				15				// and height. These could be variable for user fonts

#define	CHARMAP_BLACK			(0)				// Genaral character attribute colours
#define	CHARMAP_BLUE			(1)
#define	CHARMAP_GREEN			(2)
#define	CHARMAP_RED				(3)
#define	CHARMAP_CYAN			(4)
#define	CHARMAP_YELLOW			(5)
#define	CHARMAP_PINK			(6)
#define	CHARMAP_WHITE			(7)

#define	INK_BLACK				(0<<8)			// INK attribute colours
#define	INK_BLUE				(1<<8)
#define	INK_GREEN				(2<<8)
#define	INK_RED					(3<<8)
#define	INK_CYAN				(4<<8)
#define	INK_YELLOW				(5<<8)
#define	INK_PINK				(6<<8)
#define	INK_WHITE				(7<<8)

#define	PAPER_BLACK				(0<<11)			// PAPER attribute colours
#define	PAPER_BLUE				(1<<11)
#define	PAPER_GREEN				(2<<11)
#define	PAPER_RED				(3<<11)
#define	PAPER_CYAN				(4<<11)
#define	PAPER_YELLOW			(5<<11)
#define	PAPER_PINK				(6<<11)
#define	PAPER_WHITE				(7<<11)

// **************************************************************************************************************
//
// Structures
//
// **************************************************************************************************************

// Charmap structure
typedef struct	SCharMap
{
		u32		Flags;				// system flags and options
		u8*		pBitmap;			// pointer to our screen bitmap				(pix width*pix height)
		u16*	pCharMap;			// pointer to actuall character array
		s32		x,y;				// screen X,Y
		s32		width,height;		// width and height 
		s32		pixwidth,pixheight;	// pixel width and height

		u16		attrib_char;		// clear attrib and character

		s32		curx,cury;			// current Cursor X,Y
		s32		wx1,wy1,wx2,wy2;	// Text window

}SCharMap,*PCharMap;






// **************************************************************************************************************
//
// Public functions
//
// **************************************************************************************************************
void charmap_init( s32 baseX, s32 baseY, s32 width, s32 height );
void charmap_set_flags( u32 flags );
void charmap_display( void );
void charmap_clear( void );
void charmap_print( char *pStr );
void charmap_printf( const char *format, ...);
void charmap_printat( s32 x, s32 y, char *pStr );
void charmap_outchar( char Ch );
void charmap_newline( void );
void charmap_scroll_window( void );
void charmap_setink( u8 ink );
void charmap_setpaper( u8 paper );
void charmap_set_window( s32 x1,s32 y1,s32 x2, s32 y2 );
void charmap_home( void );
void charmap_move_cursor( s32 x, s32 y);



#if defined(__cplusplus)
}
#endif

#endif		//__CHARMAP_H_
	
