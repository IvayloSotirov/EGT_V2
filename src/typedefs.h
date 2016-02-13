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
#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

// some compilers use the "typedef" key-word, and some use
// "#typedef". Check your compiler's way of defining new datatypes
// and edit the define below
#define TYPEDEF	typedef
//#define TYPEDEF	#typedef

// Basic data types
TYPEDEF void						VOID;	// the void type
TYPEDEF unsigned char				BYTE;	// unsigned 8 bit
TYPEDEF char						sBYTE;	// signed 8 bit
TYPEDEF unsigned int				WORD;	// unsigned 16 bit
TYPEDEF int							sWORD;	// signed 16 bit
TYPEDEF unsigned long				DWORD;	// unsigned 32 bit
TYPEDEF long						sDWORD;	// signed 32 bit
TYPEDEF unsigned long long		QWORD;	// unsigned 64 bit
TYPEDEF long long					sQWORD;	// signed 64 bit

#ifndef NULL
#define NULL		0
#endif

#ifndef NIL
#define NIL		((void *)0)
#endif

// the const & flash key-words represent constants stored in the
// flash memory of the MCU. Used for portability to other compilers
// eeprom is used for variables stored in EEPROM
// and REGISTER is for variables allocated in registers.
// Be careful, you compiler may not understand those terms.
#define CONST		const
#define FLASH		flash
#define EEPROM	eeprom
#define REGISTER	register

#define VOLATILE	volatile
//#define VOLATILE	_volatile

#endif //_TYPEDEFS_H
