/******************************************************************************
       Copyright (c) 2003, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ******************************************************************************
   Module      : $RCSfile: sys_drv_timerlib_vxworks.h,v $
   Date        : $Date: 2004/10/07 03:53:32 $
   Description :
******************************************************************************/
#ifndef SYS_DRV_TIMERLIB_VXWORKS_H
#define SYS_DRV_TIMERLIB_VXWORKS_H

#include <vxworks.h>
#include <timers.h>
#ifndef PRJ_BUILD
#include "sys_drv_defs.h"
#endif

/* alias for timer */
typedef timer_t   Timer_ID;

/* Functionpointer to the callbackfunction */
typedef VOID (*TIMER_ENTRY)(Timer_ID timerid, INT32 arg);


extern Timer_ID InstallTimer(TIMER_ENTRY pTimerEntry,
                               UINT32 nTime, INT nArgument, BOOL bEnableTimer);
extern BOOL StartTimer(Timer_ID Timer);
extern BOOL StopTimer(Timer_ID Timer);
extern BOOL DeleteTimer(Timer_ID Timer);
extern BOOL SetTimeTimer(Timer_ID Timer, UINT32 nTime, BOOL bPeriodically, BOOL bRestart);

#endif /* SYS_DRV_TIMERLIB_VXWORKS_H */


