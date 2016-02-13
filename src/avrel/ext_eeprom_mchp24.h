#ifndef __EXT_EEPROM_MCHP24_H__
#define __EXT_EEPROM_MCHP24_H__

#include "typedefs.h"
#include <avr/io.h>

#define I2C_PORT_OUT    PORTB
#define I2C_PORT_IN     PINB
#define I2C_PORT_DIR    DDRB
#define EE_SDA          PB4
#define EE_SCL          PB3

#define READ_CMD	0xA1
#define WRITE_CMD	0xA0

//#ifndef I2C_PORT_OUT
//#error PLEASE DEFINE THE EEPROM MACROS
//#endif

#define delay_us(a)		;

#define INIT_I2C_PORT()										\
{															\
	I2C_PORT_OUT |= ( ( 1 << EE_SDA) | ( 1 << EE_SCL));	\
	I2C_PORT_DIR |= ( ( 1 << EE_SDA) | ( 1 << EE_SCL));	\
};                        

#define I2C_START()						\
{										\
	INIT_I2C_PORT();					\
	delay_us(1); 					\
	I2C_PORT_OUT &= ~( 1 << EE_SDA);	\
	delay_us(1); 					\
	I2C_PORT_OUT &= ~( 1 << EE_SCL);	\
	delay_us(1); 					\
};

#define I2C_STOP()						\
{										\
	I2C_PORT_OUT &= ~( 1 << EE_SCL);		\
	delay_us(1);   					\
	I2C_PORT_DIR |= ( 1 << EE_SDA);		\
	delay_us(1);   					\
	I2C_PORT_OUT &= ~( 1 << EE_SDA);	\
	delay_us(1);   					\
	I2C_PORT_OUT |= ( 1 << EE_SCL);		\
	delay_us(1);   					\
	I2C_PORT_OUT |= ( 1 << EE_SDA);	\
	delay_us(1);					\
};

void I2C_SendBit( char b);
char I2C_GetBit(void);
char I2C_SendByte( BYTE data);
char I2C_GetByte(void);
char EE_Read( WORD addr, char* data);
char EE_PageRead( WORD addr, char* data, BYTE len);
char EE_Write( WORD addr, char* data);
char EE_PageWrite( WORD addr, BYTE* data, BYTE len);
#endif // __EXT_EEPROM_MCHP24_H__
