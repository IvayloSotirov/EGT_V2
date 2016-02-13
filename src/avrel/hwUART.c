#define _TIMERS_C

#include "hwUART.h"                            

void uartSendChar( char ch)
{
	while( ! UART_IS_DATA_REG_EMPTY)
	{
	}
	cli();
	UART_DATA = ((char)ch);
	sei();
}

// FUNCTION: fprintString()
// prints a string locatef in flash
/*
void fprintString( BYTE FLASH *string)
{       
	// loop until the end of the string
	while( *string != NULL)
	{
		uartSendChar(*string);
		string++;
	}
	
}
*/
// FUNCTION: printString()
// prints a string locatef in SRAM
void printString( BYTE *string)
{       
	// loop until the end of the string
uartSendChar(*string);
string++;
	while( *string != NULL)
	{
		uartSendChar(*string);
		string++;
	}
	
}  

void PrintDataBytes( BYTE * p, BYTE len)
{                                             
	while( len > 0)
	{
		PrintDataByte( ((BYTE)*p));
		uartSendChar(' ');
		len--;   
		p++;
	}
}

void PrintDataByte( BYTE b)
{                                             
	if( b < 0x9F)
	{
		uartSendChar((b >> 4) + 0x30);
	}
	else
	{
		uartSendChar((b >> 4) + 0x37);
	}
	
	if( (b & 0xF) < 0xA)
	{
		uartSendChar((b & 0xF) + 0x30);
	}
	else
	{
		uartSendChar((b & 0xF) + 0x37);
	}
}
