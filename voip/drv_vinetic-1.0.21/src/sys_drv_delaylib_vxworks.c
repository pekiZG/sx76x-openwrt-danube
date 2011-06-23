/* delayLib.c - self-calibrating hard delay routines */

/*
modification history
--------------------
27Mar96,espin written.

$Id: sys_drv_delaylib_vxworks.c,v 1.5 2004/02/02 13:00:38 langer Exp $

*/

/*
DESCRIPTION
This module provides "hard" [<]delay[>] routines for micro and
millisecond periods.

EXAMPLE
.CS
-> delayMsec (1); /@ very first call used for calibration @/
-> timexN delayMsec, 10
timex: 75 reps, time per rep = 9555 +/- 222 (2%) microsecs
value = 59 = 0x3b = ';'
->
.CE

The routines sysClkRateGet() and tickGet() are used to calibrate
the timing loop the first time either routine is called. Therefore,
the first call is much longer than requested. If the system clock
rate is changed, a new calibration must be explicitly made by
passing -1 as [<]delay[>] duration to delayUsec.

INTERNAL
A one-shot timer could provide high resolution sub-clock tick
delay... but then this would be board dependent.
*/


#ifndef __INCdelayLibc
#define __INCdelayLibc

#include "vxWorks.h"
#include "tickLib.h"
#include "sysLib.h"

#define DELAYLIB_DEBUG FALSE

void delayUsec (unsigned int u);
void delayMsec (unsigned int m);

/*******************************************************************************
 *
 * delayUsec - hard [<]delay[>] for <u> microseconds
 *
 * Parameters : int u = nr of microsecs to wait before returning.
 *
 * RETURNS: N/A
 */

#if DELAYLIB_DEBUG
unsigned int delayLoop = 0;
#endif /* DELAYLIB_DEBUG */

void
delayUsec ( unsigned int u )
{
    #if !DELAYLIB_DEBUG
    static unsigned int delayLoop = 0;
    #endif /* !DELAYLIB_DEBUG */
    unsigned int ix;
    unsigned int iy;

    if (delayLoop == 0 || u == (unsigned int)0xffffffff)      /* need calibration?          */
    {
        unsigned int maxLoop;
        unsigned int start = 0;
        unsigned int stop = 0;
        unsigned int mpt = (1000 * 1000) / sysClkRateGet (); /* microsecs per tick     */

        for (delayLoop = 1; delayLoop < 0x1000 && stop == start; delayLoop<<=1)
        {
            /* wait for clock turn over */
            for (stop = start = tickGet (); start == stop; start = tickGet ());

            delayUsec (mpt);            /* single recursion                   */
            stop = tickGet ();
        }

        maxLoop = delayLoop / 2;        /* loop above overshoots              */
        #if DELAYLIB_DEBUG
        printf ("maxLoop = %d\n", maxLoop);
        #endif /* DELAYLIB_DEBUG */
        start = 0;
        stop = 0;
        if (delayLoop < 4)
            delayLoop = 4;
        for (delayLoop /= 4; delayLoop<maxLoop && stop == start; delayLoop++)
        {
            /* wait for clock turn over */
            for (stop = start = tickGet (); start == stop; start = tickGet ());

            delayUsec (mpt);            /* single recursion                   */
            stop = tickGet ();
        }
        #if DELAYLIB_DEBUG
        printf ("delayLoop = %d\n", delayLoop);
        #endif /* DELAYLIB_DEBUG */
    }

    for (iy = 0; iy < u; iy++)
    {
        for (ix = 0; ix < delayLoop; ix++);
    }
}

/*******************************************************************************
 *
 * delayMsec - hard [<]delay[>] for <m> milliseconds
 *
 * Parameters : int u = nr of millisecs to wait before returning.
 *
 * RETURNS: N/A
 */

void
delayMsec ( unsigned int m)
{
    delayUsec (m * 1000);
}


#endif /* __INCdelayLibc */
