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
   Module      : sys_drv_timerlib_vxworks.h
   Date        : 2004-02-05
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


