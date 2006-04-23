// **************************************************************************
//
//	CONIO.H	:	Standard Port IO routines
//
// **************************************************************************
#ifndef	__CONIO_H__
#define	__CONIO_H__




void _outb( unsigned short port, unsigned char data );
void _outw( unsigned short port, unsigned short data );
void _outd( unsigned short port, unsigned int data );
unsigned char _inb( unsigned short port );
unsigned short _inw( unsigned short port );
unsigned int _ind( unsigned short port );





#endif	//__CONIO_H__