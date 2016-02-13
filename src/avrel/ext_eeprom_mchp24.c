// This file is for Microchip's I2C eeproms
//	The Code has been tested with 24LC515

#include "ext_eeprom_mchp24.h"


void I2C_SendBit( char b)
{
	// make SDA output
	I2C_PORT_DIR |= ( 1 << EE_SDA);
	delay_us(1);

	I2C_PORT_OUT &= ~( 1 << EE_SCL);
	delay_us(1);
	if( b == 0)
		I2C_PORT_OUT &= ~( 1 << EE_SDA);
	else
		I2C_PORT_OUT |= ( 1 << EE_SDA);
	delay_us(1);
	I2C_PORT_OUT |= ( 1 << EE_SCL);
	delay_us(1);
	I2C_PORT_OUT &= ~( 1 << EE_SCL);
	delay_us(1);
	I2C_PORT_OUT &= ~( 1 << EE_SDA);
	delay_us(1);
}

char I2C_GetBit()
{              
	char inBit;

	// make SDA input and enable pullups
	delay_us(1);
	I2C_PORT_DIR &= ~( 1 << EE_SDA);
	delay_us(1);
	I2C_PORT_OUT |= ( 1 << EE_SDA);
	delay_us(1);

	I2C_PORT_OUT &= ~( 1 << EE_SCL);
	delay_us(1);
	I2C_PORT_OUT |= ( 1 << EE_SCL);
	delay_us(1);
	inBit = I2C_PORT_IN;
	delay_us(1);
	I2C_PORT_OUT &= ~( 1 << EE_SCL); 
	I2C_PORT_DIR |= ( 1 << EE_SDA);
	delay_us(1);
	return (((inBit) >> EE_SDA) & 0x01);
}

// returns ZERO on success
char I2C_SendByte( unsigned char data)
{                          
	char i;

	for( i = 7; i >= 0; i--)
	{
		I2C_SendBit( ((data >> i) & 0x01));  
	delay_us(1);
	}
	I2C_PORT_OUT &= ~( 1 << EE_SCL);
	delay_us(1);
	// get an ACK
	return I2C_GetBit();
}

char I2C_GetByte()
{                 
	unsigned char data = 0;         
	char i;
	// make SDA input and enable pullups
//	delay_us(1);
//	I2C_PORT_DIR &= ~( 1 << EE_SDA);
//	delay_us(1);
//	I2C_PORT_OUT |= ( 1 << EE_SDA);
//	delay_us(1);

	for( i = 7; i >= 0; i--)
	{
		data |= ((I2C_GetBit() & 0x01) << i);  
	delay_us(1);
	}
	// make SDA output and set
	delay_us(1);
	I2C_PORT_OUT |= ( 1 << EE_SDA);
	delay_us(1);
	I2C_PORT_DIR |= ( 1 << EE_SDA);
	delay_us(1);
	return data;
}                                             

//returns 0 on OK, -1 on error
char EE_Read( WORD addr, char* data)
{              
	char tData = 0; 
	char B0;

	B0 = (((addr & 0x8000) >> 12) & 0x08);

	I2C_START();
	if(I2C_SendByte( WRITE_CMD | B0) == 0)
	{
		if( I2C_SendByte( ((addr >> 8) & 0xFF)) == 0)  
		{
			if( I2C_SendByte( (addr & 0xFF)) == 0)  
			{                    
				I2C_START();
				if( I2C_SendByte( READ_CMD | B0) == 0)  
				{
					tData = I2C_GetByte();                                
				}      
				else
					goto BADEND;
			}      
			else
				goto BADEND;
		}      
		else
			goto BADEND;
	}
	else
		goto BADEND;

	I2C_STOP(); 
	*data = tData;
	return 0;
BADEND:       
	I2C_STOP(); 
	return (-1);
}

//returns 0 on OK, -1 on error
char EE_PageRead( WORD addr, char* data, BYTE len)
{              
//	char tData = 0; 
	char B0;

	if( len > 64 || ( addr <= 0x7FFF && (addr + len) > 0x8000))
	{
		return (-1);
	}

	B0 = (((addr & 0x8000) >> 12) & 0x08);

	I2C_START();
	if(I2C_SendByte( WRITE_CMD | B0) == 0)
	{
		if( I2C_SendByte( ((addr >> 8) & 0xFF)) == 0)  
		{
			if( I2C_SendByte( (addr & 0xFF)) == 0)  
			{                    
				I2C_START();
				if( I2C_SendByte( READ_CMD | B0) == 0)  
				{
					while( len > 0)
					{
						*data = I2C_GetByte();
						I2C_SendBit(0);	// acknoledge and wait for the nex data
						len--;
						data++;
					}
				}      
				else
					goto BADEND;
			}      
			else
				goto BADEND;
		}      
		else
			goto BADEND;
	}
	else
		goto BADEND;

	I2C_STOP(); 
	return 0;
BADEND:       
	I2C_STOP(); 
	return (-1);
}

//returns 0 on OK, -1 on error
char EE_Write( WORD addr, char* data)
{              
	char B0;

	B0 = (((addr & 0x8000) >> 12) & 0x08);

	I2C_START();
	while( I2C_SendByte( WRITE_CMD | B0) != 0)
	{
		I2C_START();
	}
	//I2C_STOP();

	I2C_START();
	if(I2C_SendByte( WRITE_CMD | B0) == 0)
	{
		if( I2C_SendByte( ((addr >> 8) & 0xFF)) == 0)  
		{
			if( I2C_SendByte( (addr & 0xFF)) == 0)  
			{                    
				if( I2C_SendByte( *data)) 
				{
					goto BADEND;
				}
			}      
			else
				goto BADEND;
		}      
		else
			goto BADEND;
	}
	else
		goto BADEND;
        
	I2C_STOP(); 
	return 0;      

BADEND:       
	I2C_STOP(); 
	return (-1);
}

//returns 0 on OK, -1 on error
char EE_PageWrite( WORD addr, BYTE* data, BYTE len)
{              
	char B0;

	if( len > 64 || ( addr <= 0x7FFF && (addr + len) > 0x8000))
	{
		return (-1);
	}
	
	B0 = (((addr & 0x8000) >> 12) & 0x08);

	I2C_START();
	while( I2C_SendByte( WRITE_CMD | B0) != 0)
	{
		I2C_START();
	}
	//I2C_STOP();

	I2C_START();
	if(I2C_SendByte( WRITE_CMD | B0) == 0)
	{
		if( I2C_SendByte( ((addr >> 8) & 0xFF)) == 0)  
		{
			if( I2C_SendByte( (addr & 0xFF)) == 0)  
			{                 
				while( len > 0)
				{
					if( I2C_SendByte( *data)) 
					{
						goto BADEND;
					}
					len--;
					data++;
				}
			}      
			else
				goto BADEND;
		}      
		else
			goto BADEND;
	}
	else
		goto BADEND;
        
	I2C_STOP(); 
	return 0;      

BADEND:       
	I2C_STOP(); 
	return (-1);
}

//// END I2C functionality

