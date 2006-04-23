/******************************************************************************/
/*																              */
/*  File: pad.h                                                               */
/*  bkenwright@xbdev.net  -  www.xbdev.net                                    */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*
    What on earth!  Whats this file for?
	Well once we have our usb device address set...for the xbox gamepad :)
	This file will do some simple commands...sending bulk test messages
	...an example would be to send a rumble message?
	Also probe for some button presses?

    In Development though.
*/
/******************************************************************************/


#ifndef		__PAD__
#define		__PAD__


#include "ohci.h"
#include "hub.h"

/******************************************************************************/
/*                                                                            */
/* stXPAD - Gamepad Data Structure                                            */
/* This structure will be filled in by our USB Gamepad - its the data that    */
/* is returned to us when we request a gamepad in on the usb bus.             */
/*                                                                            */
/******************************************************************************/


#pragma pack(1)  // We use this, so our data is packed nice and tight!..no space.
struct stXPAD    // Packed to 1 byte alignment.
{
	char reserved1;
	unsigned char structsize;
	
	char pad; /* 1 up 2 down 4 left 8 right */
	char reserved2; 
	unsigned char keys[6]; /* A B X Y Black White */
	
	unsigned char trig_left;
	unsigned char trig_right;
	short stick_left_x;
	short stick_left_y;
	short stick_right_x;
	short stick_right_y;

	char padding[0x40];
};
#pragma pack()


/******************************************************************************/
/*                                                                            */
/* stRumbleXPAD                                                               */
/* We will fill this with data later on, that we will pass to the gamepad,    */
/* it will specify the left or right rumble and speed for example             */
/*                                                                            */
/******************************************************************************/

#pragma pack(1)
struct stRumbleXPAD
{
};
#pragma pack()



/******************************************************************************/
/*                                                                            */
/* Functions                                                                  */
/*                                                                            */
/******************************************************************************/


void usb_bulk_msg( usbd_device * dev, int size, void * data );    // Send data to the USB

void usb_bulk_msg_in( usbd_device * dev, int size, void * data ); // Get data from the USB




#endif // __PAD__
