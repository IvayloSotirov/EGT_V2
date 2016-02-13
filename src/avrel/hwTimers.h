#ifndef _HW_TIMERS_H
#define _HW_TIMERS_H

#include "typedefs.h"                                    

// IDs for timer on the AVR/megaAVR/tinyAVR devices.
// NOTE: Not all devices have all those timers.
#define ID_TC0	0	// usually 8-bit Timer/Conter0
#define ID_TC1	1	// usually 16-bit Timer/Conter1
#define ID_TC2	2	// usually 8-bit Timer/Conter2 - with additional RTC functions
#define ID_TC3	3	// usually 16-bit Timer/Conter3 - found in atmega128

// Timer/Counter0 - Prescaler Selection, External Clocking Definitions
#define TC0_CLOCK_MASK	0x7							// Timer/Counter Clock bits
#define TC0_OFF			0x0							// Timer/Counter Stopped
#define TC0_MCK			(1 << CS00)					// MCK
#define TC0_MCK8		(1 << CS01)					// MCK / 8
#define TC0_MCK64		((1 << CS00) & (1 << CS01))	// MCK / 64
#define TC0_MCK256		(1 << CS02)					// MCK / 256
#define TC0_MCK1024		((1 << CS00) & (1 << CS02))	// MCK / 1024
#define TC0_EXT_FALLING	((1 << CS01) & (1 << CS02))	// External Pin Falling edge
#define TC0_EXT_RISING	((1 << CS00) & (1 << CS01) & (1 << CS02))	// External Pin Rising edge

// Timer/Counter1 - Prescaler Selection, External Clocking Definitions
#define TC1_CLOCK_MASK	0x7							// Timer/Counter Clock bits
#define TC1_OFF			0x0							// Timer/Counter Stopped
#define TC1_MCK			(1 << CS10)					// MCK
#define TC1_MCK8		(1 << CS11)					// MCK / 8
#define TC1_MCK64		((1 << CS10) & (1 << CS11))	// MCK / 64
#define TC1_MCK256		(1 << CS12)					// MCK / 256
#define TC1_MCK1024		((1 << CS10) & (1 << CS12))	// MCK / 1024
#define TC1_EXT_FALLING	((1 << CS11) & (1 << CS12))	// External Pin Falling edge
#define TC1_EXT_RISING	((1 << CS10) & (1 << CS11) & (1 << CS12))	// External Pin Rising edge

// Timer/Counter2 - Prescaler Selection, External Clocking Definitions
#define TC2_CLOCK_MASK	0x7							// Timer/Counter Clock bits
#define TC2_OFF			0x0							// Timer/Counter Stopped
#define TC2_MCK			(1 << CS20)					// MCK
#define TC2_MCK8		(1 << CS21)					// MCK / 8
#define TC2_MCK64		((1 << CS20) & (1 << CS21))	// MCK / 64
#define TC2_MCK256		(1 << CS22)					// MCK / 256
#define TC2_MCK1024		((1 << CS20) & (1 << CS22))	// MCK / 1024
#define TC2_EXT_FALLING	((1 << CS21) & (1 << CS22))	// External Pin Falling edge
#define TC2_EXT_RISING	((1 << CS20) & (1 << CS21) & (1 << CS22))	// External Pin Rising edge

#define timerConfigClock( timerID, clkSrc)				\
{														\
	if( timerID == ID_TC0)								\
		TCCR0  = ((TCCR0  & TC0_CLOCK_MASK) | clkSrc);	\
	else if( timerID == ID_TC1)							\
		TCCR1B = ((TCCR1B & TC1_CLOCK_MASK) | clkSrc);	\
	else if( timerID == ID_TC2)							\
		TCCR2  = ((TCCR2  & TC2_CLOCK_MASK) | clkSrc);	\
}

#endif //_HW_TIMERS_H
