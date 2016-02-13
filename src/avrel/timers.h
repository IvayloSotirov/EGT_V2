#ifndef _TIMERS_H
#define _TIMERS_H

#include "typedefs.h"                                    

// TIMER_CALL_RATE will be used as the amount of time that is taken from
// the msToLive field of each timer in the queue when a timer interrupt occurs.
// TIMER_ERROR will be used at initial timer setup to compensate for the uneven interval
// at which tmrCall is called. For example: our timer routine gets called every 4.096ms, so at
// timer interrupt wi should do a 'msToLive -= 4', but this implies an error of 0.096ms on every call
// in an effort to keep the ISR short and fast, we shall leave this error there and compensate for it in the
// tmrInit() routine. This is done by examining how many times the ISR will be called for a timer, calculating
// the cumulative error, and adding it to msToLive. With the example values above, a 1000ms timer will
// expire in 250 ISR( interrupt service routine) calls, when in reallity, it sould expire in 244 ISR calls.
// this is the type of error that we compensate for in tmrInit()

//#define TIMER_CALL_RATE		65		// timer is called every 65 ms
//#define TIMER_ERROR			0.536	// actually, every 65.563 ms (65ms + 536microSec)
#define TIMER_CALL_RATE		4			// timer is called every 4 ms
#define TIMER_ERROR			0.096		// actually, every 4.096 ms (4ms + 96microSec)

// The TIMER_COEFFICIENT if here just to make the compiler do some math before putting
// the number in the code. This saves time, and reduses code. The idea is this:
// (actual msToLive) = ( wanted msToLive) * TIMER_COEFFICIENT
#define TIMER_COEFFICIENT		(1 - ( TIMER_ERROR / (TIMER_CALL_RATE)))

// WARNING, the following passage may not work with every compiler, since some compilers
// are quite restricted in the macro usage. For example, CodeVisionAvr could not handle
// the tmrAdd() macro properly.....

// if you define MACRO_TMR_ADD, the tmrAdd() call will be implemented as a macro
// comment the following line to make tmrAdd() a function
//#define MACRO_TMR_ADD

// if you define MACRO_TMR_DEL, the tmrDel() call will be implemented as a macro
// comment the following line to make tmrDel() a function
#define MACRO_TMR_DEL

// END of Warning passage.

typedef void (*pVoidFunc)(void);

// The All-Mighty TIMER structure. One such struct is allocated for every timer that you use.
// All of them are allocated at compile time, inited in tmrInit(), and accessed in tmrCall().
// TODO: make the timers dynamicly allocatable and unallocatable in tmrDel().
//
// TIMER.timerCfg field
//  ----------------------------------------------------
// | 31 | 30 | msToLive is here - 30 bits ..............|
//  ----------------------------------------------------
//   ^     ^
//   |     `--> "TIMER Cyclic FLAG" '1' is cyclic, '0' is one-shot
//   `--------> "TIMER Active FLAG" '1' is active, '0' is inactive
//
// TIMER.msToLive field
//  ----------------------------------------------------
// | 31 | 30 | msToLive is here - 30 bits ..............|
//  ----------------------------------------------------
// bits 30 & 31 are ZERO in msToLive and are used to detect when an underflow occurs

TYPEDEF struct TIMER_tag
{
	DWORD			timerCfg;			// used for flags and timeout value
	DWORD			msToLive;			// decremented until timeout on underflow
	pVoidFunc		tmrHandler;	// function called on timeout
	struct TIMER_tag	*prev; 			// ponter to previous element in list
	struct TIMER_tag	*next; 			// ponter to next element in list
} TIMER;

TYPEDEF struct TIMER_tag	*pTIMER;

// the duplication in meaning of TIMER_ACTIVE / TIMER_ACTIVE_MASK is so that we dont pass whole DWORD's to tmrInit()
// the same is with TIMER_CYCLIC / TIMER_CYCLIC_MASK
#define TIMER_MAX_TIMEOUT	0x3FFFFFFF		// max delay ~ 298 hours
#define TIMER_ACTIVE		1				// timer is active
#define TIMER_INACTIVE		0				// timer is NOT active
#define TIMER_CYCLIC		1				// timer is called cyclically
#define TIMER_ONESHOT		0				// timer is called and then deleted
#define TIMER_ACTIVE_MASK	0x80000000		// a mask used with timerCfg to distinguish if a timer is Active
#define TIMER_CYCLIC_MASK	0x40000000		// a mask used with timerCfg to distinguish if a timer is Cyclic


#ifdef MACRO_TMR_ADD
#define tmrAdd( pNewTmr)					\
{								\
	if( gpTimer == NULL) /* adding first timer*/		\
	{							\
uart_send_stringf( "first.");				\
		gpTimer = ((pTIMER)pNewTmr);				\
		/*((pTIMER)pNewTmr)->prev = NULL;*/				\
		gpTimer->prev = NULL;				\
		gpTimer->next = NULL;				\
  	}							\
	else	/* adding a timer at the end of the queue*/	\
	{							\
		pTIMER pLast;					\
uart_send_stringf( "QUEUE.");				\
		pLast = gpTimer;				\
		while( pLast->next != NULL)			\
		{						\
			pLast = pLast->next;			\
		}						\
		/* got to the last timer, adding the new one*/	\
		((pTIMER)(pLast->next)) = pNewTmr;		\
		((pTIMER)pNewTmr)->prev = pLast;	\
		((pTIMER)pNewTmr)->next = NULL;	\
	}							\
      /*	((pTIMER)pNewTmr)->next = NULL;				\
	if( ((pTIMER)pNewTmr)->next == NULL)	\
uart_send_stringf( "ok...");				\
	((pTIMER)pNewTmr)->msToLive = 				\
		((pTIMER)pNewTmr)->timerCfg & 0x3FFFFFFF;*/	\
};
#else
void tmrAdd( pTIMER);
#endif

#ifdef MACRO_TMR_DEL
#define tmrDel( pTmr)							\
{											\
	if( pTmr->prev == NULL)	/* deleting the first timer*/	\
	{										\
		if(pTmr->next == NULL) /* first and only*/		\
		{									\
			gpTimer = NULL;					\
		}									\
		else	/* first with others following*/			\
		{									\
			gpTimer = ((pTIMER)pTmr->next);		\
			gpTimer->prev = NULL;				\
		}									\
	}										\
	else if( pTmr->next == NULL)	/* last timer*/		\
	{										\
		pTmr->prev->next = NULL;				\
	}										\
	else	/* timer is somewhere in the middle*/			\
	{										\
		pTmr->prev->next = pTmr->next;			\
		pTmr->next->prev = pTmr->prev;			\
	}										\
};
#else
void tmrDel( pTIMER);
#endif

void	tmrCall( void);
sBYTE	tmrSafeInit( pTIMER , BYTE , BYTE , DWORD, void*);
void	tmrInit( pTIMER , BYTE , BYTE , DWORD , void*);
void	tmrClean( pTIMER);

#endif //_TIMERS_H
