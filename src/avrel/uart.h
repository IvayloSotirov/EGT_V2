#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedefs.h"                                    

#define uartSendChar( ch)	\
{							\
	while( !(USR & 0x20))	\
	{						\
	}						\
	cli();					\
	UDR = ((BYTE)ch);		\
	sei();					\
};

//void fprintString( BYTE const *);
//void printString( BYTE *);
void PrintDataBytes( BYTE * , BYTE);
void PrintDataByte( BYTE);

#endif //_UART_H
