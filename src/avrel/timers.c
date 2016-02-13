#define _TIMERS_C

#include "timers.h"                            

VOLATILE pTIMER	gpTimer = NULL;
extern void timerFunc(void);

// tmrCall() is the routine that should be called at regular intervals by your implementation.
// the best way to do this is by setting up a timer interrupt, and informing the timers that tmrCall()
// is called at the given time interval. This is done in timers.h
VOLATILE void tmrCall()
{
	VOLATILE pTIMER curTmr = {0};	// we need this to be able to walk through all of the timers in the queue
	
	// check if there are any timers in the queue
	if(gpTimer == NULL)
		return;	// No timer found.... nothing to do here.
        
	// ok, get the first timer
	curTmr = gpTimer;

	while( 1)	// endless loop, until we reach the last timer in the queue.... don't know their count...
	{                                         
		// if the current timer is ACTIVE, go on and handle it.
		if( curTmr->timerCfg & TIMER_ACTIVE_MASK)
		{
			// decrement the msToLive and check for underflow
			// since the 2 MSB of msToLive are always cleared, the underflow is detected
			// when those bits get set.
			((DWORD)curTmr->msToLive) -= TIMER_CALL_RATE;
			if( curTmr->msToLive > TIMER_MAX_TIMEOUT)
			{
				// We have an underflow
				// first, call the timer Handler
				(*curTmr->tmrHandler) ();
				
				// if timer is cyclic, reinit it.
				if( ((DWORD)curTmr->timerCfg) & TIMER_CYCLIC_MASK)
				{
					// reinit the msToLive from timerCfg
					((DWORD)curTmr->msToLive) = ((DWORD)(curTmr->timerCfg & TIMER_MAX_TIMEOUT));
				}
				else // timer is a one-shot, delete it.
				{
 					tmrDel( ((pTIMER)curTmr));
				}
			}
		}
		
		// check if there are more timers to update and get the next one
		if(curTmr->next == NULL)	// all timers DONE, return to program   
			break;
			
		// get the next timer in list and handle it in the next cycle
		curTmr = ((pTIMER)curTmr->next);
	}
}

#ifndef MACRO_TMR_ADD
void tmrAdd( pTIMER pNewTmr)
{
	if( gpTimer == NULL) /* adding first timer*/
	{
		gpTimer = ((pTIMER)pNewTmr);
		gpTimer->prev = NULL;
		gpTimer->next = NULL;
  	}
	else	/* adding a timer at the end of the queue*/
	{
		gpTimer->prev = pNewTmr;	// first timer's prev now points to pNewTmr
		pNewTmr->next = gpTimer;	// pNewTmr->next points to first timer
		pNewTmr->prev = NULL;		// pNewTmr->prev should be NULL
		gpTimer = pNewTmr;		// pNewTmr becomes first timer
	}
	// finally, initalize the timer NOTE... this should be moved to a seperate func
//	pNewTmr->msToLive = pNewTmr->timerCfg & 0x3FFFFFFF;
}
#endif
       
#ifndef MACRO_TMR_DEL
void tmrDel( pTIMER pTmr)
{
	if( pTmr->prev == NULL)	/* deleting the first timer*/
	{
		if(pTmr->next == NULL) /* first and only*/
		{
			gpTimer = NULL;
		}
		else	/* first with others following*/
		{
			gpTimer = ((pTIMER)pTmr->next);
			gpTimer->prev = NULL;
			/*((pTIMER)pTmr->next)->prev = NULL;*/
		}
	}
	else if( pTmr->next == NULL)	/* last timer*/
	{
		pTmr->prev->next = NULL;
	}
	else	/* timer is somewhere in the middle*/
	{
		pTmr->prev->next = pTmr->next;
		pTmr->next->prev = pTmr->prev;
	}
}
#endif            

sBYTE tmrSafeInit( pTIMER pNewTmr, BYTE state, BYTE cyclic, DWORD timeout, void * handler)
{                                                           
	// check for accurate timer state
	if( state != TIMER_ACTIVE && state != TIMER_INACTIVE)
		return (-1);

	// check for acurate timer type
	if( cyclic != TIMER_CYCLIC && cyclic != TIMER_ONESHOT)
		return (-2);

	// check for NULL handler
	if( handler == NULL)
		return (-3);
		
	// check for NULL TIMER pointer
	if( pNewTmr == NULL)
		return (-4);

	// check for too long timeout
	if( timeout > TIMER_MAX_TIMEOUT)
		return (-5);

	// checks are OK, initialize the timer
	pNewTmr->timerCfg = timeout;     
	if(state)
		pNewTmr->timerCfg |= 0x80000000;
	if(cyclic)          
		pNewTmr->timerCfg |= 0x40000000;
	pNewTmr->msToLive = pNewTmr->timerCfg & TIMER_MAX_TIMEOUT;

	// assign the timeout handler
	pNewTmr->tmrHandler = handler;
	
	// init the pointers of the structure
	pNewTmr->next = NULL;
	pNewTmr->prev = NULL;
	
	// DONE
	return 0x00;
}

void tmrInit( pTIMER pNewTmr, BYTE state, BYTE cyclic, DWORD timeout, void *handler)
{                                                           
	// clean up the structure
	tmrClean(pNewTmr);
	
	// skip all checks..... HOPE you know what you're doing...  
	// set timerCfg and clear the two config bits.
	pNewTmr->timerCfg = ((DWORD) (TIMER_COEFFICIENT * timeout)) & TIMER_MAX_TIMEOUT;
	// now is a good time to init msToLive since bits 30 and 31 are cleared.
	pNewTmr->msToLive = pNewTmr->timerCfg;
	//The two most significat bits are used as timer flags: active/inactive , cyclic/onshot
	// init the config bits.
	if(state)
		((DWORD)pNewTmr->timerCfg) |= TIMER_ACTIVE_MASK;
	if(cyclic)          
		((DWORD)pNewTmr->timerCfg) |= TIMER_CYCLIC_MASK;

	// assign the timeout handler
	pNewTmr->tmrHandler = handler;
	
	// init the pointers of the timer list
	pNewTmr->next = NULL;
	pNewTmr->prev = NULL;
	// NOTE, this timer can and SHOULD be added to the queue using tmrAdd.
	// DONE
}

void tmrClean( pTIMER timer)
{
	timer->timerCfg = 0;
	//timer->msToLive = 0;
	//timer->tmrHandler = NULL;
	//timer->prev = NULL;
	//timer->next = NULL;
}

