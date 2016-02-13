/*
	Copyright (C) 2016 Ivaylo Sotirov
	Release: January 2016
	www.insproject.com

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/
 */
 #include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "typedefs.h"
#include <avr/pgmspace.h>


//11.19.2012
//Rev 1.0

//7.13.2015
//Rev 2.0
//	- New display.
//	- Added button.
//	- Added temperature limit.
//	- Added zumer.
//	- Added output.

//=========================================================================================================================
// DEVICE CONFIGURATION
//=========================================================================================================================
#define USE_NEW_DISPLAY

//#define TEST_TYPE
//#define BASIC_TYPE
//#define MIDLE_TYPE
#define PRO_TYPE

#ifdef BASIC_TYPE
	#define ENABLE_LED_DISPLAY
#endif

#ifdef MIDLE_TYPE
	#define ENABLE_LED_DISPLAY
	#define ENABLE_PWM_OUTPUT
#endif

#ifdef PRO_TYPE
	#define ENABLE_LED_DISPLAY
	#define ENABLE_PWM_OUTPUT
	#define ENABLE_BUTTON
#endif

#ifdef TEST_TYPE
	#define	LED_DISPLAY_TEST

	#define ENABLE_LED_DISPLAY
	#define ENABLE_PWM_OUTPUT
	#define ENABLE_BUTTON
#endif

//=========================================================================================================================
// DEVICE CONFIGURATION
//=========================================================================================================================


#define ADC_BUF_LEN	8

WORD ADC_RESULT0 = 0;
WORD ADC_RESULT1 = 0;
WORD ADC_RESULT2 = 0;
WORD PWM_RESULT_OUT = 0;
BYTE adcounter = 0;
BYTE ADCIndex = 0;

WORD ADCBUF0[ADC_BUF_LEN];
WORD ADCBUF1[ADC_BUF_LEN];
WORD ADCBUF2[ADC_BUF_LEN];


// cold end temperature:[C]		  -40,   125
// TC1047 voltages [V]:			  0.1,   1.75
//const WORD TC_ADC_INPUT[2]  ={    0,    360}

// K type thermocouple compensation voltage [uV]
//const sWORD TEMP_COMPENS[2] ={  -1889,   4878}
//#define TC_X1	0
//#define TC_X2	350
//#define TC_Y1	-1889
//#define TC_Y2	4878
// We'll use the formula: y = (((Y2-Y1)/(X2-X1))*(ADC1-X1) + Y1)
// but X1 = 0 and we rearange a bit => y = (((Y2-Y1)*ADC1)/X2 + Y1)
// or simply: y = ((6767*ADC1)/350 - 1889) !!!!!
// or simply: y = ((TC_Y2Y1*ADC1)/TC_X2 + TC_Y1) !!!!!

//---- calculation constants
#define TC_Y2Y1 	6767
#define TC_Y1		-1889
#define TC_X2		360

// K-type thermocouple data
#define NUM_INTERVALS	6
#define K_ADC_MAX	767
// ADC									 0,	128,		256,		384,		512,	640,		768
// ADC voltages [V]:					 0,	0.61875,	1.2375,		1.85625,	2.475,	3.09375,	3.7125
const WORD ADC_values[NUM_INTERVALS+1]={0,	128,		256,		384,		512,	640,		768};

// K-type (*66.866) [uV]:					0.0,       9253,   18507,    27760,  37014,    46267,   55521
const WORD K_real[NUM_INTERVALS+1]	={		0.0,       9253,   18507,    27760,  37014,    46267,   55521};

// Temperature diff,[C]:					0.0,      227.5,     450,    667.5,  892.3,   1130.8,    1385
const WORD FinalTemp[NUM_INTERVALS+1]	={	0.0,        228,     450,      668,    892,     1131,    1385};

#define PWM_CONST 12890
// Output voltage / 0.0038 = Temperature
// 3.04[V] / 0.0038 = 800[*C]

//---- END calculation constants

WORD	curT_Limit = 0xFFFF;
WORD	tmpT_Limit = 0;

WORD	coldEndCompens;
DWORD	Thermocouple,TOut;
WORD	ADC_X1, ADC_X2, ADC_X2X1;
WORD	T_X1, T_X2, T_X2X1;
WORD	Y1, Y2, Y2Y1;
DWORD	tcVal;
//BYTE 	blinker = 0;
//BYTE 	LED_State = 0;

//=============================================
//Alarm definitions
//=============================================
//#define	ALARM_TYPE_ZUMER
#define	ALARM_TYPE_OUTPUT
#define ALARM_INTERVAL 20
#define ALARM_REPEAT_INTERVAL 300

#define	ALARM_OFF_TEMP 20
DWORD	AlarmCounter = 0;
DWORD	AlarmRepeatCounter = 0;
//=============================================
//END of Alarm definitions
//=============================================

//=============================================
//Display definitions
//=============================================
BYTE	DisplayBlink = 0;
//=============================================
//END of Display definitions
//=============================================



//=============================================
//Functions prototypes
//=============================================
void dowork(void);
void SPI_MasterTransmit(uint8_t cData);
void SPI_MasterInit(void);
BYTE ledsByteToBCD(BYTE digit);
void dispOutput(WORD TOut, BYTE Option);
void initCPU(void);

BYTE Button_Check(void);
WORD Calc_PWM_Value(WORD Temperature);
//=============================================
//END of Functions prototypes
//=============================================

//===========================================================================================================================
SIGNAL(SIG_OVERFLOW0) // called every 0.1024ms
{
	//use this for time keeping if needed
/*
	if( blinker > 500)
	{
		blinker = 0;
		if( LED_State == 0)
			PORTD &= ~( 1 << 0);
		else
			PORTD |= ( 1 << 0);
		LED_State = ~LED_State;
	}
*/

}
//===========================================================================================================================

//===========================================================================================================================
void dowork()
{
	BYTE Button_Result;
	BYTE	lowIndex;

	static WORD maxTout = 0;
	static int DelayCounter = 50;

	if( ADC_RESULT1 > TC_X2)
	{
		ADC_RESULT1 = TC_X2;
	}

	if( ADC_RESULT0 > K_ADC_MAX)
	{
		ADC_RESULT0 = K_ADC_MAX;
	}

	tcVal = (DWORD)((DWORD)ADC_RESULT1 * (DWORD)TC_Y2Y1);
	tcVal = (DWORD)(tcVal / TC_X2);
 	coldEndCompens = (WORD)(tcVal + TC_Y1);

	// find the interval for the ADC0
	lowIndex = (BYTE)( (WORD)ADC_RESULT0 >> 7);
	//just in case...
	if( lowIndex > (NUM_INTERVALS-1))
		lowIndex = (NUM_INTERVALS-1);

	//get the values for interpolation
	ADC_X1 = ADC_values[lowIndex];
	ADC_X2 = ADC_values[lowIndex + 1];
	ADC_X2X1 = ADC_X2 - ADC_X1;

	// claculations for K_real VOLTAGE
	Y1 = K_real[lowIndex];
	Y2 = K_real[lowIndex + 1];
	Y2Y1 = Y2 - Y1;

	Thermocouple = (DWORD)( ADC_RESULT0 - ADC_X1);
	Thermocouple = (DWORD)(Y2Y1 * Thermocouple);
	Thermocouple = (DWORD)( Thermocouple / ADC_X2X1);
	Thermocouple = (DWORD)( Thermocouple + Y1);

	// Cold-end compensation
	Thermocouple += coldEndCompens;
	// we now have a voltage that is cold end compensated....

	//get the values for interpolation
	T_X1 = K_real[lowIndex];
	T_X2 = K_real[lowIndex + 1];
	T_X2X1 = T_X2 - T_X1;

	//Calculate the final temperature.
	Y1 = FinalTemp[lowIndex];
	Y2 = FinalTemp[lowIndex + 1];
	Y2Y1 = Y2 - Y1;

	TOut = (DWORD)( Thermocouple - T_X1);
	TOut = (DWORD)(Y2Y1 * TOut);
	TOut = (DWORD)( TOut / T_X2X1);
	TOut = (DWORD)( TOut + Y1);


	//Get max temperature
	if(DelayCounter)
	{
		DelayCounter--;
	}
	else
	{
		if(maxTout < TOut)
		{
			maxTout = TOut;
		}
	}

	#ifdef ENABLE_BUTTON
	Button_Result = Button_Check();
	if((0xFFFF == curT_Limit) && (0 != ADC_RESULT2))
	{
		if((((ADC_RESULT2 * 13) / 100) * 10) != tmpT_Limit)
		{
			tmpT_Limit = ((ADC_RESULT2 * 13) / 100) * 10;
		}
		else
		{
			curT_Limit = ((ADC_RESULT2 * 13) / 100) * 10;
		}
	}
	#else
	Alarm_Enable = 0;
	Button_Result = 0;
	curT_Limit = 1320;
	#endif

	if((TOut >= curT_Limit) && (DisplayBlink == 0) /*|| (DisplayBlink && (TOut > (curT_Limit - ALARM_OFF_TEMP)))*/)
	{
		DisplayBlink = 1;

		#ifdef ALARM_TYPE_ZUMER
		if(AlarmRepeatCounter < (ALARM_REPEAT_INTERVAL - (ALARM_REPEAT_INTERVAL / 3)))
		{
			if(AlarmCounter < (ALARM_INTERVAL / 3))
			{
				PORTD |= ( 1 << 6);
			}
			else
			{
				PORTD &= ~( 1 << 6);
			}
			AlarmCounter++;
			if(AlarmCounter >= ALARM_INTERVAL)
			{
				AlarmCounter = 0;
			}
		}
		else
		{
			PORTD &= ~( 1 << 6);
		}
		AlarmRepeatCounter++;
		if(AlarmRepeatCounter >= ALARM_REPEAT_INTERVAL)
		{
			AlarmRepeatCounter = 0;
		}
		#endif
		#ifdef ALARM_TYPE_OUTPUT
		PORTD |= ( 1 << 6);
		#endif
	}
	else if(TOut  < (curT_Limit - ALARM_OFF_TEMP))
	{
		AlarmCounter = 0;
		AlarmRepeatCounter = 0;
		DisplayBlink = 0;
		PORTD &= ~( 1 << 6);
	}

/*
	if(TOut > curT_Limit)
	{
		DisplayBlink = 1;
		PORTD |= ( 1 << 6);
	}
	else
	{
		DisplayBlink = 0;
		PORTD &= ~( 1 << 6);
	}
*/

	if(Button_Result == 1) //Set Temperature Limit
	{
		tmpT_Limit = ADC_RESULT2 * 13;
		if(((curT_Limit * 10) > tmpT_Limit) && (((curT_Limit * 10) - tmpT_Limit) > 10))
		{
			curT_Limit = (tmpT_Limit / 100) * 10;
		}
		else if((tmpT_Limit > (curT_Limit * 10)) && ((tmpT_Limit - (curT_Limit * 10)) > 10))
		{
			curT_Limit = (tmpT_Limit / 100) * 10;
		}
		dispOutput(curT_Limit, 1);
		PWM_RESULT_OUT = Calc_PWM_Value(curT_Limit);
	}
	else
	{
		dispOutput(TOut, 0);
		PWM_RESULT_OUT = Calc_PWM_Value(TOut);
	}
}
//===========================================================================================================================

//===========================================================================================================================
SIGNAL(SIG_OVERFLOW1)
{
	#ifdef ENABLE_PWM_OUTPUT
		OCR1AH = (BYTE)(PWM_RESULT_OUT >> 8);
		OCR1AL = (BYTE)PWM_RESULT_OUT;
	#else
		OCR1AH = (BYTE)(0x00 >> 8);
		OCR1AL = (BYTE)0x00;
	#endif
}
//===========================================================================================================================

//===========================================================================================================================
SIGNAL(SIG_ADC)
{

	WORD adc_data;
	BYTE i;
	DWORD temp;

	adc_data = (WORD)ADCL;
	adc_data += (WORD)(ADCH << 8);

	ADCSRA &= ~(0x80);

	if( adcounter > 2)
		adcounter = 0;


	if(adcounter == 2)
	{
		//ADC2 result...
		// this is temperature border preset.
		ADCBUF2[ADCIndex] = adc_data;
		ADMUX = 0x40; // sample ADC0 next
		//==================================
		temp = 0;
		for( i = 0; i < ADC_BUF_LEN; i++)
		{
			temp += ADCBUF2[i];
		}
		ADC_RESULT2 = temp / ADC_BUF_LEN;
		//ADC_RESULT2 = adc_data;
		//==================================
		ADCIndex++;
	}
	else if(adcounter == 1)
	{
		//ADC1 result...
		// This is the TC1047 temp->voltage converter.
		ADCBUF1[ADCIndex] = adc_data;
		ADMUX = 0x42; // sample ADC2 next
		//==================================
		temp = 0;
		for( i = 0; i < ADC_BUF_LEN; i++)
		{
			temp += ADCBUF1[i];
		}
		ADC_RESULT1 = temp / ADC_BUF_LEN;
		//ADC_RESULT1 = adc_data;
		//==================================
		//setPWMByte(ADC_RESULT1 >> 2);
	}
	else
	{
		//ADC0 result...
		// this is the K type thermocouple.
		ADCBUF0[ADCIndex] = adc_data;
		ADMUX = 0x41; // sample ADC1 next
		//==================================
		temp = 0;
		for( i = 0; i < ADC_BUF_LEN; i++)
		{
			temp += ADCBUF0[i];
		}
		//==================================
		//ADC_RESULT0 = temp / ADC_BUF_LEN;
		ADC_RESULT0 = adc_data;
	}

	ADCSRA = 0xEF;
	adcounter++;

	if( ADCIndex >= ADC_BUF_LEN)
	{
		ADCIndex = 0;
	}
}
//===========================================================================================================================

//===========================================================================================================================
int main (void)
{

	//Init the MCU
	initCPU();
	//Init all ADC buffers
	memset(ADCBUF0, 0x00, ADC_BUF_LEN*sizeof(WORD));
	memset(ADCBUF1, 0x00, ADC_BUF_LEN*sizeof(WORD));
	memset(ADCBUF2, 0x00, ADC_BUF_LEN*sizeof(WORD));

	OCR1AH = 1;
	OCR1AL = 0xFF;
	OCR2B=0xFF;

	DisplayBlink = 0;
	while(1)
	{
		WORD i;
		cli();
		dowork();
		sei();
		for( i=0; i < 20000; i++);
	}
}
//===========================================================================================================================

//===========================================================================================================================
void initCPU()
{
	CLKPR=0x80;
	CLKPR=0x00;

	// Input/Output Ports initialization
	// Port B initialization
	PORTB=0x00;
	DDRB=0x02; //PB1 - output

	// Port C initialization
	// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
	PORTC=0x00;
	DDRC=0x00;

	// Port D initialization
	// Func7=in Func6=in Func5=Out Func4=in Func3=in Func2=in Func1=in Func0=in
	PORTD=0x00;
	DDRD=0xDF; // all in, PD5 - IN

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	// Mode: Normal top=FFh
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	TCCR0A=0x00;
	TCCR0B=0x02; // clk/8
	TCNT0=0x00;
	OCR0A=0x00;
	OCR0B=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: Timer 1 Stopped
	// Mode: Normal top=FFFFh
	// OC1A output: Discon.
	// OC1B output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	TCCR1A=0x83;	// mode7 (fast PWM 10-bit), TOP=0x3FF, OVF on TOP, update output on TOP
//	TCCR1B=0x0B; 	// 305Hz, OC1B - not used, OC1A - clear on match, set on TOP
//	TCCR1B=0x0A; 	// 2.4KHz, OC1B - not used, OC1A - clear on match, set on TOP
	TCCR1B=0x09; 	// 20MHz, OC1B - not used, OC1A - clear on match, set on TOP
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H= 0x00;
	ICR1L= 0x00;
	OCR1AH=0x00;
	OCR1AL=0x01;
	OCR1BH=0x00;
	OCR1BL=0x00;

	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: Normal top=FFh
	// OC2A output: Disconnected
	// OC2B output: Disconnected
	ASSR=0x00;
	TCCR2A=0x23; // , mode3, Fast PWM top = 0xFF
	TCCR2B=0x03; // clck/32 (2.4KHz), clear OC2B on match, set on top
	TCNT2=0x00;		//OC2A - disconnected
	OCR2A=0x00;
	OCR2B=0x01; // default set to very low output...

	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	// Interrupt on any change on pins PCINT0-7: Off
	// Interrupt on any change on pins PCINT8-14: Off
	// Interrupt on any change on pins PCINT16-23: Off
	EICRA=0x00;
	EIMSK=0x00;
	PCICR=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0=0x01; //overflow interrupt
	// Timer/Counter 1 Interrupt(s) initialization
	TIMSK1=0x01; //overflow interrupt
	// Timer/Counter 2 Interrupt(s) initialization
	TIMSK2=0x00;

	// Analog Comparator initialization
	// Analog Comparator: Off
	// Analog Comparator Input Capture by Timer/Counter 1: Off
	ACSR=0x80;
	ADCSRB=0x00;

	//ADMUX = 0xC7;
	ADMUX = 0x40;
	//ADMUX = 0x86;
	ADCSRA = 0xEF;

	SPI_MasterInit();

	//Enable interrupts
	//    sei();
    //Disable interrupts
	//    cli();
}
//===========================================================================================================================


//===========================================================================================================================
// LED DISPLAY
//===========================================================================================================================
#ifdef USE_NEW_DISPLAY
//===========================================================================================================================
/*
  --0--
|		|
|		|
5		1
|		|
|		|
  --6--
|		|
|		|
4		2
|		|
|		|
  --3--		.7
*/
//NEW						  0		1	  2		3	  4		5	  6		7	  8		9
const BYTE CONST_BCD[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

#define BCD_ERR   0x8F
#define BCD_MINUS 0x40
#define BCD_DOT   0x80
//===========================================================================================================================
#else
//===========================================================================================================================
//OLD
const BYTE CONST_BCD[10] = {0xEE, 0x60, 0xCD, 0xE9, 0x63, 0xAB, 0xAF, 0xE0, 0xEF, 0xEB};

#define BCD_ERR   0x8F
#define BCD_MINUS 0x01
#define BCD_DOT   0x10
#endif
//===========================================================================================================================

//===========================================================================================================================
BYTE ledsByteToBCD(BYTE digit)
{
  BYTE ret = 0x00;

  if( (digit & 0x0F) > 9)
    return 0;

  ret = CONST_BCD[digit];

  if(digit & 0x80)  // "-" sign
    ret = BCD_MINUS;
  if(digit & 0x40)  // decimal point
    ret |= BCD_DOT;

  return ret;
}
//===========================================================================================================================

//===========================================================================================================================
void SPI_MasterInit(void)
{
	PORTB |= ( 1 << PB2);
	// Set MOSI, SS, and SCK output, all others input
	DDRB |= ( 1 << PB3) | ( 1 << PB5) | ( 1 << PB2);
	// Enable SPI, Master, set clock rate fck/4 -> 5MHz
	SPCR = (1<<SPE)|(1<<MSTR)/*|(1<<SPR0)*/;
}
//===========================================================================================================================

//===========================================================================================================================
void SPI_MasterTransmit(uint8_t cData)
{
	PORTB &= ~( 1 << PB2);
	// Start transmission
	SPDR = cData;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	PORTB |= ( 1 << PB2);
}
//===========================================================================================================================

//===========================================================================================================================
char tempString[5];

#define	NUM_ENTRY 10
#define	HISTEREZIS 3
DWORD 	AverageTemperature = 0;
WORD	tmpTemp = 0;
WORD	LastDisp_OUT = 0;
BYTE 	EntryCounter = 0;

#define	DISPLAY_BLINK_INTERVAL 20
BYTE	DisplayBlinkCounter = 0;

void dispOutput(WORD TOut, BYTE Option)
{
	#ifdef LED_DISPLAY_TEST
		if(EntryCounter < 100)
		{
			EntryCounter++;
		}
		else
		{
			LastDisp_OUT +=1111;
			if(LastDisp_OUT > 9999)
			{
				LastDisp_OUT = 0;
			}
			EntryCounter = 0;
		}
	#else
		//=======================================================
		//Histerezis
		//=======================================================
		if(TOut > 9999)
		{
			TOut = 9999;
		}
		if(EntryCounter < NUM_ENTRY)
		{
			AverageTemperature += TOut;
			EntryCounter++;
		}
		else
		{
			tmpTemp = AverageTemperature / NUM_ENTRY;
			if((LastDisp_OUT > tmpTemp) && ((LastDisp_OUT - tmpTemp) > HISTEREZIS))
			{
				LastDisp_OUT = tmpTemp;
			}
			else if((LastDisp_OUT < tmpTemp) && ((tmpTemp - LastDisp_OUT) > HISTEREZIS))
			{
				LastDisp_OUT = tmpTemp;
			}
			AverageTemperature = 0;
			EntryCounter = 0;
		}
		//=======================================================
		//Histerezis
		//=======================================================
	#endif
	//=======================================================
	//Prepare digits for LED display.
	//=======================================================
	if(Option)
	{
		tmpTemp = TOut;
	}
	else
	{
		tmpTemp = LastDisp_OUT;
	}
	tempString[0] = tmpTemp % 10;
	tmpTemp -= tempString[0];
	tmpTemp /= 10;
	tempString[1] = tmpTemp % 10;
	tmpTemp -= tempString[1];
	tmpTemp /= 10;
	tempString[2] = tmpTemp % 10;
	tmpTemp -= tempString[2];
	tmpTemp /= 10;
	tempString[3] = tmpTemp % 10;



	tempString[0] = ledsByteToBCD(tempString[0]);
	tempString[1] = ((tempString[1]) || (tempString[2]) || (tempString[3])) ? ledsByteToBCD(tempString[1]) : 0x00;
	tempString[2] = ((tempString[2]) || (tempString[3])) ? ledsByteToBCD(tempString[2]) : 0x00;
	tempString[3] = tempString[3] ? ledsByteToBCD(tempString[3]) : 0x00;

	if(DisplayBlink)
	{
		if(DisplayBlinkCounter < (DISPLAY_BLINK_INTERVAL / 2))
		{
			tempString[0] = 0x00;
			tempString[1] = 0x00;
			tempString[2] = 0x00;
			tempString[3] = 0x00;
		}
		DisplayBlinkCounter++;
		if(DisplayBlinkCounter >= DISPLAY_BLINK_INTERVAL)
		{
			DisplayBlinkCounter = 0;
		}
	}
	else
	{
		DisplayBlinkCounter = 0;
	}


	//=======================================================
	//Prepare digits for LED display.
	//=======================================================

	//=======================================================
	// Send in reverse order
	//=======================================================
	#ifndef ENABLE_LED_DISPLAY
	tempString[0] = 0x00;
	tempString[1] = 0x00;
	tempString[2] = 0x00;
	tempString[3] = 0x00;
	#endif
	SPI_MasterTransmit(tempString[0]);
	SPI_MasterTransmit(tempString[1]);
	SPI_MasterTransmit(tempString[2]);
	SPI_MasterTransmit(tempString[3]);
	//=======================================================
	// Send in reverse order
	//=======================================================
}
//===========================================================================================================================

//===========================================================================================================================
// LED DISPLAY
//===========================================================================================================================

//===========================================================================================================================
BYTE Button_Check(void)
{
	//PIN D5
	if(PIND & 0x20)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
//===========================================================================================================================

//===========================================================================================================================
WORD Calc_PWM_Value(WORD Temperature)
{
	DWORD tmpPWM;

	tmpPWM = Temperature;
	tmpPWM *= 10000;
	tmpPWM /= PWM_CONST;
	if(tmpPWM > 1023)
	{
		tmpPWM = 1023;
	}
	return tmpPWM;
}
//===========================================================================================================================
