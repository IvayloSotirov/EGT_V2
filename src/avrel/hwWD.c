#include <avr/io.h>

void WD_Disable( void)
{
	#if defined(WDCE)
		// for the megaAVR targets, use WDCE
		WDTCR |= ( 1 << WDE) | ( 1 << WDCE);
	#elif defined(WDTOE)
		// for the old AVR targets, use WDTOE
		WDTCR |= ( 1 << WDE) | ( 1 << WDTOE);
	#else
		#error "Strange target! Don't know how to access the Watchdog..."
	#endif
	WDTCR &= ~( 1 << WDE);
};
