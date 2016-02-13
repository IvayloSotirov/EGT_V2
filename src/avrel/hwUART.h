#ifndef _HW_UART_H
#define _HW_UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedefs.h"                                    

// UART Control Register Options
// use when configuring the UART
// and the enable/disable specific UART interrupts
#define UART_RX_COMPLETE_INT_ENABLE		( 1 << RXCIE)
#define UART_TX_COMPLETE_INT_ENABLE		( 1 << TXCIE)
#define UART_DATA_REG_EMPTY_INT_ENBALE	( 1 << UDRIE)
#define UART_RX_ENABLE					( 1 << RXEN)
#define UART_TX_ENABLE					( 1 << TXEN)
#define UART_9BIT_CHARS					( 1 << CHR9)
#define UART_INTERRUPT_MASK				UART_RX_COMPLETE_INT_ENABLE	| \
										UART_TX_COMPLETE_INT_ENABLE	| \
										UART_DATA_REG_EMPTY_INT_ENBALE
#define UART_INT_ALL					UART_INTERRUPT_MASK

// UART Status Register Conditional macros
// Prefered usage:
// if( UART_CHECK_FRAME_ERROR)
// {
//     Error_On_Rx( whatever...);
// }
#define UART_IS_RX_COMPLETE		( (USR & ( 1 << RXC)) > 0)
#define UART_IS_TX_COMPLETE		( (USR & ( 1 << TXC)) > 0)
#define UART_IS_DATA_REG_EMPTY	( (USR & ( 1 << UDRE)) > 0)
#define UART_CHECK_FRAME_ERROR	( (USR & ( 1 << FE)) > 0)
#define UART_CHECK_OVERRUN		( (USR & ( 1 << OR)) > 0)

//Data Register
#define UART_DATA	UDR

#define uartConfig( flags, baudConf)	\
{										\
	UCR |= flags;						\
	UBRR = baudConf;					\
}

#define uartInterruptEnable( intName)	\
{										\
	UCR |= intName;						\
}

#define uartInterruptDisable( intName)	\
{										\
	UCR &= ~(intName);					\
}
/*
#define uartSendChar( ch)				\
{										\
	while( ! UART_IS_DATA_REG_EMPTY)	\
	{;									\
	}									\
	cli();								\
	UART_DATA = ((char)ch);					\
	sei();								\
};
*/
//void fprintString( BYTE const *);
void uartSendChar( char ch);
void printString( BYTE *);
void PrintDataBytes( BYTE * , BYTE);
void PrintDataByte( BYTE);

#endif //_HW_UART_H
