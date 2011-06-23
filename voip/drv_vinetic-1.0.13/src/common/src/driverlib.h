/****************************************************************************
                  Copyright (c) 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE, 
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS 
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING 
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP, 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE 
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY 
   THIRD PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY 
   INTELLECTUAL PROPERTY INFRINGEMENT. 

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : driverlib.h
   Date        : 26.01.01
   Description :
      Device Driver specified defines
******************************************************************************/
#ifndef _DRIVERLIB_H
#define _DRIVERLIB_H

#include "tickLib.h"


#define SYNC  __asm__("	sync")

#define TICKS_PER_16MS    (sysClkRateGet()/60)
#define TICKS_PER_33MS    (sysClkRateGet()/30)
#define TICKS_PER_50MS    (sysClkRateGet()/20)
#define TICKS_PER_100MS   (sysClkRateGet()/10)
#define TICKS_PER_500MS   (sysClkRateGet()/2)
#define TICKS_PER_1S       sysClkRateGet()

#define DELAY_16MS   taskDelay(TICKS_PER_16MS)
#define DELAY_33MS   taskDelay(TICKS_PER_33MS)
#define DELAY_50MS   taskDelay(TICKS_PER_50MS)
#define DELAY_100MS  taskDelay(TICKS_PER_100MS)
#define DELAY_500MS  taskDelay(TICKS_PER_500MS)
#define DELAY_1S     taskDelay(TICKS_PER_1S)


#define WAIT_16MS   round = (UINT32)tickGet(); \
                    while(1) {if ((tickGet() - round) > TICKS_PER_16MS) break;}

#define WAIT_33MS   round = (UINT32)tickGet(); \
                    while(1) {if ((tickGet() - round) > TICKS_PER_33MS) break;}

#define WAIT_50MS   round = (UINT32)tickGet(); \
                    while(1) {if ((tickGet() - round) > TICKS_PER_50MS) break;}

#define WAIT_100MS  round = (UINT32)tickGet(); \
                    while(1) {if ((tickGet() - round) > TICKS_PER_100MS) break;}

#define WAIT_500MS  round = (UINT32)tickGet(); \
                    while(1) {if ((tickGet() - round) > TICKS_PER_500MS) break;}


#endif

