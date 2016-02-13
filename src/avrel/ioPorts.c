#include "ioPorts.h"                                    

// Note: NEVER call this function directly unless you KNOW what you're doing
void configIO_Output_Internal( BYTE portName, BYTE bitMask)
{
	switch( portName)
	{
	#ifdef PORTA
		case ((BYTE)&PORTA):
			DDRA |= bitMask;
			break;
	#endif

	#ifdef PORTB
		case ((BYTE)&PORTB):
			DDRB |= bitMask;
			break;
	#endif

	#ifdef PORTC
		case ((BYTE)&PORTC):
			DDRC |= bitMask;
			break;
	#endif

	#ifdef PORTD
		case ((BYTE)&PORTD):
			DDRD |= bitMask;
			break;
	#endif

	#ifdef PORTE
		case ((BYTE)&PORTE):
			DDRE |= bitMask;
			break;
	#endif

	#ifdef PORTF
		case ((BYTE)&PORTF):
			DDRF |= bitMask;
			break;
	#endif

	#ifdef PORTG
		case ((BYTE)&PORTG):
			DDRG |= bitMask;
			break;
	#endif

	// NOT available on any AVR yet, but since it
	// doesn't take up CODE, who cares :)
	// maybe the next mega will have it!
	#ifdef PORTH
		case ((BYTE)&PORTH):
			DDRH |= bitMask;
			break;
	#endif
	}
}

// Note: NEVER call this function directly unless you KNOW what you're doing
void configIO_Input_Internal( BYTE portName, BYTE bitMask)
{
	switch( portName)
	{
	#ifdef PORTA
		case ((BYTE)&PORTA):
			DDRA &= ~bitMask;
			break;
	#endif

	#ifdef PORTB
		case ((BYTE)&PORTB):
			DDRB &= ~bitMask;
			break;
	#endif

	#ifdef PORTC
		case ((BYTE)&PORTC):
			DDRC &= ~bitMask;
			break;
	#endif

	#ifdef PORTD
		case ((BYTE)&PORTD):
			DDRD &= ~bitMask;
			break;
	#endif

	#ifdef PORTE
		case ((BYTE)&PORTE):
			DDRE &= ~bitMask;
			break;
	#endif

	#ifdef PORTF
		case ((BYTE)&PORTF):
			DDRF &= ~bitMask;
			break;
	#endif

	#ifdef PORTG
		case ((BYTE)&PORTG):
			DDRG &= ~bitMask;
			break;
	#endif

	// NOT available on any AVR yet, but since it
	// doesn't take up CODE, who cares :)
	// maybe the next mega will have it!
	#ifdef PORTH
		case ((BYTE)&PORTH):
			DDRH &= ~bitMask;
			break;
	#endif
	}
}

// Note: NEVER call this function directly unless you KNOW what you're doing
void configIO_Pullups_Internal( BYTE portName, BYTE bitMask)
{
	switch( portName)
	{
	#ifdef PORTA
		case ((BYTE)&PORTA):
			PORTA |= bitMask;
			break;
	#endif

	#ifdef PORTB
		case ((BYTE)&PORTB):
			PORTB |= bitMask;
			break;
	#endif

	#ifdef PORTC
		case ((BYTE)&PORTC):
			PORTC |= bitMask;
			break;
	#endif

	#ifdef PORTD
		case ((BYTE)&PORTD):
			PORTD |= bitMask;
			break;
	#endif

	#ifdef PORTE
		case ((BYTE)&PORTE):
			PORTE |= bitMask;
			break;
	#endif

	#ifdef PORTF
		case ((BYTE)&PORTF):
			PORTF |= bitMask;
			break;
	#endif

	#ifdef PORTG
		case ((BYTE)&PORTG):
			PORTG |= bitMask;
			break;
	#endif

	// NOT available on any AVR yet, but since it
	// doesn't take up CODE, who cares :)
	// maybe the next mega will have it!
	#ifdef PORTH
		case ((BYTE)&PORTH):
			PORTH |= bitMask;
			break;
	#endif
	}
}
