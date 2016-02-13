#ifndef _IO_PORTS_H
#define _IO_PORTS_H

#include <avr/io.h>
#include "typedefs.h"                                    

// those are not necessary, since all i/o pins are mapped to a i/o register
// in one and the same way for every port, but this should make code a bit
// more readable
#ifndef PIN7
	#define	PIN7	(1 << 7)
	#define	PIN6	(1 << 6)
	#define	PIN5	(1 << 5)
	#define	PIN4	(1 << 4)
	#define	PIN3	(1 << 3)
	#define	PIN2	(1 << 2)
	#define	PIN1	(1 << 1)
	#define	PIN0	(1 << 0)
#endif

#define ALLPINS			0xFF
#define IO_PORT_MASK	0xFF

// Functions to configure a port direction
// NOTE: The function uses the port name as first argument, NOT the Direction Reg!!!
// Examples:
//		configIO_Output( PORTB, IO_PORT_MASK);	// set all pins to outputs
//		configIO_Output( PORTC, PIN2|PIN5);		// set pins 2 and 5 to outputs
//		configIO_Input( PORTD, IO_PORT_MASK);	// set all pins to inputs
//		configIO_Input( PORTB, PIN7|PIN6|PIN0);	// set pins 0, 6 and 7 to inputs

#define	configIO_Output(portName, bitMask)	\
		configIO_Output_Internal(((BYTE)&portName), bitMask)
#define	configIO_Input(portName, bitMask)	\
		configIO_Input_Internal(((BYTE)&portName), bitMask)


// Function to configure an input port pullups
// NOTE: The function uses the port name as first argument, NOT the Direction Reg!!!
// Examples:
//		configIO_Pullups( PORTA, IO_PORT_MASK);	// enable all pullups
//		configIO_Pullups( PORTA, ALLPINS);		// enable all pullups
//		configIO_Pullups( PORTC, PIN3|PIN4);	// enable pullups on pins 4 and 5
// NOTE: if the port is configured as output, the selected pins will output high

#define	configIO_Pullups(portName, bitMask)	\
		configIO_Pullups_Internal(((BYTE)&portName), bitMask)


// Don't call those fuctions directly if you are not sure how to do it properly.
// Use the MACROS above
void configIO_Pullups_Internal(BYTE /*portName*/, BYTE /*bitMask*/);
void configIO_Output_Internal( BYTE /*portName*/, BYTE /*bitMask*/);
void configIO_Input_Internal(  BYTE /*portName*/, BYTE /*bitMask*/);

#endif //_IO_PORTS_H
