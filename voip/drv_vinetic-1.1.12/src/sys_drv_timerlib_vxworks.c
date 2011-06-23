/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE, 
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS 
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING 
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP, 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE 
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY 
   THIRD PARTY CALIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY 
   INTELLECTUAL PROPERTY INFRINGEMENT. 

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : $RCSfile: sys_drv_timerlib_vxworks.c,v $
   Date        : $Date: 2004/02/05 15:05:47 $
   Description :
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <timers.h>
#include <vxworks.h>
#include "sys_drv_timerlib_vxworks.h"

/*******************************************************************************
Description:
   Creates a new timer.
Arguments:
   pTimerEntry		- specifies the call back fucnction for the driver
   nTime				- specifies the schedule time in milliseconds
   nArgument		- specifies the argument passed to the call back function
   bEnableTimer	- specifies if the timer shall be initially started (TRUE)
Return Value:
   Returnes the new timer.
Remarks:
	The timer runs in the context of the task, which created it
	Thus that task cannot end, until the timer is deleted
   Vxworks reference manual:
   The task creating a timer with timer_create( ) will receive the signal
   no matter which task actually arms the timer.
   When a timer expires and the task has previously exited, logMsg( )
   indicates the expected task is not present.
   Similarly, logMsg( ) indicates when a task arms a timer without
   installing a signal handler.
   Timers may be armed but not created or deleted at interrupt level.
*******************************************************************************/
Timer_ID InstallTimer(TIMER_ENTRY pTimerEntry,
                      UINT32 nTime, INT nArgument, BOOL bEnableTimer)
{
#ifdef INCLUDE_POSIX_TIMERS
   Timer_ID Timer;
   struct   itimerspec   timeToSet;        /* time to be set */
   struct   timespec     timeValue;        /* timer expiration value */
   struct   timespec     timeInterval;     /* timer period */

   /* Initialize timer expiration value */
   timeValue.tv_sec        = (nTime/1000);
   timeValue.tv_nsec       = (nTime%1000) * 1000 * 1000;

   /* Initialize timer period */
   timeInterval.tv_sec     = (nTime/1000);
   timeInterval.tv_nsec    = (nTime%1000) * 1000 * 1000;

   /* reset timer structure */
   memset ((char *) &timeToSet, 0, sizeof (struct itimerspec));

   if( bEnableTimer )
   {
	   /* Set the time to be set value */
	   timeToSet.it_value      = timeValue;
   }
   timeToSet.it_interval   = timeInterval;


   /* derive timer from CLK realtimer, do not use signal handler */
   if(timer_create(CLOCK_REALTIME, NULL, &Timer) == ERROR)
   {
	   return NULL;
   }

   /* connect timer to interrupt function */
   if(timer_connect(Timer, pTimerEntry, nArgument) == ERROR)
   {
	   return NULL;
   }

   /* pass timer value & reload value */
	/* Do not use 'TIMER_ABSTIME' flag because timer will then expire immediatly */
   if(timer_settime(Timer, 0, &timeToSet, NULL) == ERROR)
   {
	   return NULL;
   }

   return Timer;
#else
   return NULL;
#endif
}


/*******************************************************************************
Description:
   Starts an existing timer.
Arguments:
   Timer		- specifies the timer to be started
Return Value:
   Returnes 'FALSE' in case of an error, otherwise 'TRUE'.
Remarks:
   ATTENTION
   The usage of this function might lead to misbehavior at the moment:
   If creating a timer with 'InstallTimer()' which is initially not enabled
   the values for timeout will be set as follows:
      itimerspec.it_value    = 0                  (actual timeout value)
      itimerspec.it_interval = given timeout      (reload timeout value)
   Within the 'StartTimer' function the reload value should be read for copying
   it to the actual timeout value. But the reading of the reload value always
   return 0 and this will of course not start the timer but exit this function
   with an error (FALSE).

   WORKAROUND
   You should use the function 'SetTimeTimer()' which also includes the timeout
   which will be used for setting the timeouts for the timer and thus there
   is no problem to get former reload values (not required).
*******************************************************************************/
BOOL StartTimer(Timer_ID Timer)
{
#ifdef INCLUDE_POSIX_TIMERS
   struct itimerspec value;

   /* get former timer reload value */
   if(timer_gettime(Timer, &value) == ERROR)
   {
      return FALSE;
   }

   /* something goes wrong */
   if(value.it_interval.tv_sec == 0 && value.it_interval.tv_nsec == 0)
   {
      return FALSE;
   }

   /* stop timer */
   if(timer_cancel(Timer) == ERROR)
   {
      return FALSE;
   }

   /* set timer value to reload value for new start */
   value.it_value = value.it_interval;

   /* pass timer value & reload value */
	/* Do not use 'TIMER_ABSTIME' flag because timer will then expire immediatly */
   if(timer_settime(Timer, 0, &value, NULL) == ERROR)
   {
      return FALSE;
   }

   return TRUE;
#else
   return FALSE;
#endif
}


/*******************************************************************************
Description:
   Stops an existing timer.
Arguments:
   Timer - specifies the timer to be stopped
Return Value:
   Returns 'FALSE' in case of an error, otherwise 'TRUE'.
Remarks:
   None
*******************************************************************************/
BOOL StopTimer(Timer_ID Timer)
{
#ifdef INCLUDE_POSIX_TIMERS
   /* stop timer */
   if(timer_cancel(Timer) == ERROR)
   {
      /*printf("error timer_cancel (Stop_Timer)\n");*/
      return FALSE;
   }

   return TRUE;
#else
   return FALSE;
#endif
}


/*******************************************************************************
Description:
   Deletes an existing timer.
Arguments:
   Timer	- specifies the timer to be deleted
Return Value:
   Returnes 'FALSE' in case of an error, otherwise 'TRUE'.
Remarks:
   None
*******************************************************************************/
BOOL DeleteTimer(Timer_ID Timer)
{
#ifdef INCLUDE_POSIX_TIMERS
   /* stop timer */
   if(timer_cancel(Timer) == ERROR)
   {
      printf("error timer_cancel (Delete_Timer)\n");
      return FALSE;
   }
   /* delete timer */
   if(timer_delete(Timer) == ERROR)
   {
      printf("error timer_delete (Delete_Timer)\n");
      return FALSE;
   }
   return TRUE;
#else
   return FALSE;
#endif
}


/*******************************************************************************
Description:
   Change the expiration time of an existing timer.
   Loads the timer value(s) and the timer reload value(s) with the specified
   time. After changing the setting the timer will be activated!
Arguments:
   Timer 			- specifies the timer to be configured
   nTime				- specifies the schedule time in milliseconds
	bPeriodically	- FALSE: timer will run only once
						  TRUE:	timer will run periodically
   bRestart       - specifies if timer will be stoped (if alread running)
                    before starting with new timing
Return Value:
   Returnes 'FALSE' in case of an error, otherwise 'TRUE'.
Remarks:
   None
*******************************************************************************/
BOOL SetTimeTimer(Timer_ID Timer, UINT32 nTime, BOOL bPeriodically, BOOL bRestart)
{
#ifdef INCLUDE_POSIX_TIMERS
	struct itimerspec   timeToSet;        /* time to be set */
	struct timespec     timeValue;        /* timer expiration value */
	struct timespec     timeInterval;     /* timer period */


   if(bRestart)
   {
	   /* stop timer */
   	if(timer_cancel(Timer) == ERROR)
	   {
   	   return FALSE;
	   }
	}

	/* Initialize timer expiration value */
	timeValue.tv_sec        = (nTime/1000);
	timeValue.tv_nsec       = (nTime%1000) * 1000 * 1000;

	/* Initialize timer period */
	if (bPeriodically == TRUE)
	{
		timeInterval.tv_sec     = (nTime/1000);
		timeInterval.tv_nsec    = (nTime%1000) * 1000 * 1000;
	}
	else
	{
		timeInterval.tv_sec     = 0;
		timeInterval.tv_nsec    = 0;
	}

	/* reset timer structure */
	memset ((char *) &timeToSet, 0, sizeof (struct itimerspec));

	/* Set the time to be set value */
	timeToSet.it_value      = timeValue;
	timeToSet.it_interval   = timeInterval;


   /* pass timer value & reload value */
	/* Do not use 'TIMER_ABSTIME' flag because timer will then expire immediatly */
   if(timer_settime(Timer, 0, &timeToSet, NULL) == ERROR)
   {
      return FALSE;
   }

   return TRUE;
#else
   return FALSE;
#endif
}





