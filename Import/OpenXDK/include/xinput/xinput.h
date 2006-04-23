/******************************************************************************/
/*                                                                            */
/* File: xinput.h                                                             */
/* Auth: bkenwright@xbdev.net                                                 */
/* Desc: GamePad entry code for use on the xbox.                              */
/*                                                                            */
/******************************************************************************/
/*
  This is our xinput header file, this is where all our functions that we'll
  use in our game or app for the xbox that 'require' gamepad support will
  be put.
*/
/******************************************************************************/
/*
    How does this code work?  What lib's does it need?  How would I use it
	in my code?

	Well the aim of the gamepad code was to develop an independent set of 
	code for the openxdk - but because it's lib independent it also works on
	the xdk.

	<1> Include our header file, and the other files which do all the work..
	    later they'll be put into a library
	    e.g. #include "xinput/xinput.h"   // Gamepad input
	    
	<2> Init our gamepad code
	    e.g.
		    stXINPUT xin;
		    xInitInput(&xin);
		You do this ocne at the start of the program, I did it this way as I don't
		like globals, so you need to pass it a stuct called stXINPUT which
		keeps all our usb/gampad information in.

	<3> Get our gamepad information in our main loop e.g:
		stXPAD pad;
		xGetPadInput(&pad, &xin);

	<4> When we've all finished, call xReleaseInput(&xin);

	<Misc> You can also set the rumble effect - still in development
	e.g. xSetPadInput(&pad, &xin);
*/
/******************************************************************************/

#ifndef		__XINPUT__
#define		__XINPUT__

#if defined(__cplusplus)
extern "C"
{
#endif


#include "ohci.h"
#include "pad.h"   // for stXPAD definition



// About this - at the moment it just contains the offset to the ohci memory 
// location (e.g. 0xfed00000...but we will also use this to keep track of
// alocated memory - how many gamepads are plugged in...usb dev drivers etc.

struct stXINPUT
{
	ohci_t my_ohci;
};

struct stXPAD;   // Defined in xpad.h



/******************************************************************************/
/*                                                                            */
/*  Interface Functions                                                       */
/*                                                                            */
/******************************************************************************/

//<1> Creation
int xInitInput(stXINPUT* p);

//<2> Probing/ Getting or Setting Gamepad
int xGetPadInput(stXPAD * p, stXINPUT * xin, int iWhichPad = 0);   // 0 is default pad
int xSetPadInput(stXPAD * p, stXINPUT * xin, int iWhichPad = 0);

//<3> Death End
int xReleaseInput(stXINPUT * p);



#if defined(__cplusplus)
}
#endif


#endif // __XINPUT__

