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
