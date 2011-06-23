/******************************************************************************
       Copyright (c) 1999, Infineon Technologies.  All rights reserved.

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

