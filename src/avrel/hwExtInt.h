#ifndef __EXT_INT_H__
#define __EXT_INT_H__

#include "typedefs.h"
#include <avr/io.h>

// definitions for the Interrupt sense register.
#define INT_ON_LOW_LEVEL		0
#define INT_RESERVED			1
#define INT_ON_FALLING_EDGE		2
#define INT_ON_RISING_EDGE		3

#define	extIntEnable( int_name)		\
{									\
	GIMSK |= (1 << int_name);		\
};

#define	extIntDisable( int_name)	\
{									\
	GIMSK &= ~(1 << int_name);		\
};

#define extIntConfigure( int_name, config)								\
{																		\
	BYTE tmpMCUCR;														\
	tmpMCUCR = (BYTE) MCUCR & ~(0x3 << ((int_name - INT0)*2));			\
	MCUCR = tmpMCUCR | ((BYTE)(config << ((int_name - INT0)*2)));		\
}																		\

#endif // __EXT_INT_H__
