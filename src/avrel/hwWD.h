#ifndef _HW_WATCHDOG_H_
#define _HW_WATCHDOG_H_

#include <avr/io.h>

#define WD_16K_CICLE_RESET		0
#define WD_32K_CICLE_RESET		1
#define WD_64K_CICLE_RESET		2
#define WD_128K_CICLE_RESET		3
#define WD_256K_CICLE_RESET		4
#define WD_512K_CICLE_RESET		5
#define WD_1024K_CICLE_RESET	6
#define WD_2048K_CICLE_RESET	7

#define WD_Enable( presc)			\
{									\
	WDTCR |= ( 1 << WDE) | presc;	\
};

#define WD_Reset()	\
{					\
	asm("WDR");		\
};

void WD_Disable(void);

#endif //_HW_WATCHDOG_H_
