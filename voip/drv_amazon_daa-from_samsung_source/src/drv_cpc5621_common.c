/******************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.
 
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
   Module      : $RCSfile: drv_cpc5621_common.c,v $
   Date        : $Date: 2004/08/03 14:32:34 $
   Description : Common functions for the Template Driver
******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_cpc5621_api.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */


/* ============================= */
/* Local function declaration    */
/* ============================= */
void RingTimerCB(unsigned long data);
void BatteryCidCB(unsigned long data);
void BatteryTimerCB(unsigned long data);

void Pin_Ring_Callback(int nDev, int nCh, WORD nEvt);
void Pin_BatDetc_Callback(int nDev, int nCh, WORD nEvt);
void Pin_PolDetc_Callback(int nDev, int nCh, WORD nEvt);
    
/* ============================= */
/* Global variable definition    */
/* ============================= */
/** trace group implementation */
CREATE_TRACE_GROUP(CPC5621_DRV);

/** log group implementation */
CREATE_LOG_GROUP(CPC5621_DRV);

/** callback function table for DUSLIC I/O interface */
_FUNCPTR callbackTable[DRV_CPC5621_MAX] = 
{
    NULL,
#if 0 // by wkhwang, 		
    Pin_Ring_Callback,
    Pin_BatDetc_Callback,
    Pin_PolDetc_Callback,
#else    
    NULL,  
    NULL,
    NULL,
#endif
    NULL
};


/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */
UINT32 before = 0;
UINT32 ringFlag = 0;
UINT32 batFlag = 0;


/**
   Start the corresponding timer
 
   \return
   NULL
*/
void Timer_Start(TIMER_ID *Tid, int ms, void *func, UINT32 arg)
{
#ifdef LINUX
    Tid->expires = jiffies + (HZ*ms/1000);
    Tid->function = func;
    Tid->data = (unsigned long)arg;
#endif

    TIMER_ADD(Tid);
}

#if 1 // by wkhwang,

void RingTimerCB2(unsigned long data)
{
    CPC5621_DEV *pDev = (CPC5621_DEV *)data;
    
//printk("+RingTimerCB, count=%d, before=%d, ringFlag=%d \n", pDev->Ring.count, before, ringFlag);

	// by wkhwang, calculate the pulse number for the ring on time ...
	unsigned int ringCnt = pDev->ringIndex.Ring_Freq * pDev->ringIndex.Ring_OnPeriod / 1000;
	ringCnt = ringCnt>>1;
	
	if(ringCnt < 3)	ringCnt = 3;
	

	if( pDev->Ring.count <  ringCnt  ) {
		
	printk("++RingTimerCB2 ... pDev->Ring.count=%d, ringCnt=%d \n", 	pDev->Ring.count, ringCnt );

	
	        Sem_Lock(pDev->ExceSem);
	        pDev->nException.Bits.ringoff = 1;
	        pDev->nException.Bits.flag= 1;
	        Sem_Unlock(pDev->ExceSem);
	        
	        /* reset count and flag */
	        pDev->Ring.count = 0;
	        ringFlag = 0;
//	        before = 0;

	        /* send ring event */
	        Wake_Up_Event(pDev->WakeupList);
	}
	
}

#endif




/**
   The timer will check the ring finished or not.
 
   \return
   NULL
*/
void RingTimerCB(unsigned long data)
{
    CPC5621_DEV *pDev = (CPC5621_DEV *)data;
    
printk("+RingTimerCB, count=%d, before=%d, ringFlag=%d \n", pDev->Ring.count, before, ringFlag);

#if 1
	// by wkhwang, calculate the pulse number for the ring on time ...
	unsigned int ringCnt = pDev->ringIndex.Ring_Freq * pDev->ringIndex.Ring_OnPeriod / 1000;
	ringCnt = ringCnt>>1;
	
	if(ringCnt < 3)	ringCnt = 3;
	

	// ringing is comming now!!
	if( /*(ringFlag==0) &&*/ (pDev->Ring.count >=  ringCnt ) ) {

	printk("++pDev->Ring.count=%d, ringCnt=%d \n", pDev->Ring.count, ringCnt );
	
	        /* ringing pulse stop, send a exception event */
	        Sem_Lock(pDev->ExceSem);
	        pDev->nException.Bits.ringon = 1;
	        pDev->nException.Bits.flag= 1;
	        Sem_Unlock(pDev->ExceSem);
	        
	        /* reset count and flag */
	        pDev->Ring.count = 0;
	        ringFlag = 1;
//	        before = 0;

	        /* send ring event */
	        Wake_Up_Event(pDev->WakeupList);
	}
#endif
//	        pDev->Ring.count = 0;


#if 0
	
	else {

	        /* reset count and flag */
	        pDev->Ring.count = 0;
	        ringFlag = 0;
	        before = 0;
	}
#endif



#if 0 // by wkhwang, 


	
    if (pDev->Ring.count - before == 1 && !ringFlag)
    {   
/*         TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("%s: ignore single pulse\n", __FUNCTION__)); */
//	 printk("%s: ignore single pulse\n", __FUNCTION__); 

        /* one ring pulse occured, we will skip this single. */
        pDev->Ring.count = 0;
        before = 0;         
    }
    else if (pDev->Ring.count - before > 0)
    {   
/*         TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("%s: restart timer\n", __FUNCTION__)); */
//	 printk("%s: restart timer\n", __FUNCTION__); 
        /* ringing occurted, set flag to tell other callback function */
        if (!ringFlag) 
            ringFlag = 1;
        
        before = pDev->Ring.count;
                  
        /* start timer again */
        Timer_Start(&pDev->Ring.TimerID, pDev->Ring.tMin, RingTimerCB, (UINT32)pDev);    
    }
    else if (pDev->Ring.count - before == 0 && ringFlag)
    {
//         printk ("%s: wakeup application\n", __FUNCTION__); 
        /* ringing pulse stop, send a exception event */
        Sem_Lock(pDev->ExceSem);
        pDev->nException.Bits.ring = 1;
        pDev->nException.Bits.flag = 1;
        Sem_Unlock(pDev->ExceSem);
        
        /* reset count and flag */
        pDev->Ring.count = 0;
        ringFlag = 0;
        before = 0;

        /* send ring event */
        Wake_Up_Event(pDev->WakeupList);
    }
    else
    {
//        printk ("RingTimerCB: ring error: %d, %d\n", pDev->Ring.count, before);
     
        /* reset count and flag */
        ringFlag = 0;
        before = 0;
        pDev->Ring.count = 0;
    }
#endif
	
}
       
/**
   Callback function of the CPC5621's ring_pin
 
   \return
   NULL
*/
void Pin_Ring_Callback(
    int nDev,               /**< device number */
    int nCh,                /**< channel number */
    WORD nEvt               /**< pin event number */
)
{
    CPC5621_DEV *pDev = CPC5621_Devices[nDev][nCh/*by wkhwang, 2006-05-25 6:35오후 -1*/];

//    printk("Pin_Ring_Callback nDev=%d, nCh=%d, pDev=%08x, CPC5621_Devices=%08x !!\n", nDev, nCh, (int)pDev, (int)CPC5621_Devices); 
    if (pDev == NULL)
    {
        printk("Pin_Ring_Callback: device error nDev=%d, nCh=%d\n", nDev, nCh); 
        return;
    }
//     printk("Pin_Ring_Callback count=%d!\n", pDev->Ring.count); 

    // in case of the first ringing ...     
    // detect the first ringing  ... 
    if ( (ringFlag==0) && (pDev->Ring.count== 0) )
    {   
 //       before = pDev->Ring.count;
   	   /* remove driver from list */
   	del_timer_sync(&pDev->Ring.TimerID);
     
        /* start ring_timer */
        printk("Ring on Start timer, timer=%d\n", pDev->ringIndex.Ring_OnPeriod);
        Timer_Start(&pDev->Ring.TimerID, pDev->ringIndex.Ring_OnPeriod, RingTimerCB, (UINT32)pDev);
    }
    // else... detect the ring off ... 
    else if (ringFlag==1) {
	   /* remove driver from list */
   	del_timer_sync(&pDev->Ring.TimerID);

        /* start ring_timer */
	 pDev->Ring.count = 0;
 //      printk("Ring off Start timer, timer=%d\n", pDev->ringIndex.Ring_OffPeriod+pDev->ringIndex.Ring_OnPeriod);
        Timer_Start(&pDev->Ring.TimerID, pDev->ringIndex.Ring_OffPeriod+pDev->ringIndex.Ring_OnPeriod, 
			RingTimerCB2, (UINT32)pDev);
    }

    pDev->Ring.count++;	

}


/**
   The timer will check the BAT signal finished or not.
 
   \return
   NULL
*/
void BatteryTimerCB(unsigned long data)
{
    CPC5621_DEV *pDev = (CPC5621_DEV *)data;
    
    if (pDev->Bat.count == 1 && !ringFlag)
    {
        pDev->Bat.count = 0;
        
        /* send Battery event */
        Sem_Lock(pDev->ExceSem);
        pDev->nException.Bits.battery = 1;
        pDev->nException.Bits.flag = 1; 
        Sem_Unlock(pDev->ExceSem);
        Wake_Up_Event(pDev->WakeupList);
    }
    else if (pDev->Bat.count == 2 && !ringFlag)
    {
        /* this signal will be the polarity event */
        pDev->Bat.count = 0;
        
        /* when system under off hook mode, we skip this signal */
        if (!pDev->HookMode)
            return;
            
        /* send Polarity event */
        Sem_Lock(pDev->ExceSem);
        pDev->nException.Bits.polarity = 1;
        pDev->nException.Bits.flag = 1; 
        Sem_Unlock(pDev->ExceSem);
        Wake_Up_Event(pDev->WakeupList);
    }
    else
    {
        pDev->Bat.count = 0;
    }
}

/**
   The timer will clear the CID mode flag.
 
   \return
   NULL
*/
void BatteryCidCB(unsigned long data)
{
    CPC5621_DEV *pDev = (CPC5621_DEV *)data;
    
    pDev->CidMode = 1;
    batFlag = 0;
}

    
/**
   Callback function of the corresponding pin
 
   \return
   NULL
*/
void Pin_BatDetc_Callback(
    int nDev,           /**< device number */
    int nCh,            /**< channel number */
    WORD nEvt           /**< pin event number */
)
{

    CPC5621_DEV *pDev = CPC5621_Devices[nDev][nCh/*by wkhwang, 2006-05-25 6:35오후 -1*/];
    
    if (pDev == NULL)
    {
        printk("Pin_BatDetc_Callback: device error\n");
        return;
    }
    
    if (!pDev->CidMode)
    {
        if (batFlag)
            return;
        else
            batFlag++;
        
        /* start a timer to clear CidMode */
        Timer_Start(&pDev->Bat.TimerID, pDev->Cid.tMin + 10, BatteryCidCB, (UINT32)pDev);
        return;
    }
    
    if (pDev->Ring.count <= 2)
    {
        if (pDev->Bat.count++ == 0)
        {
            /* start battery timer */
            Timer_Start(&pDev->Bat.TimerID, pDev->Bat.tMin, BatteryTimerCB, (UINT32)pDev);
        }
    }
}

/**
   Callback function of the corresponding pin
 
   \return
   NULL
*/
void Pin_PolDetc_Callback(
    int nDev,               /**< device number */
    int nCh,                /**< channel number */
    WORD nEvt
)
{
    CPC5621_DEV *pDev = CPC5621_Devices[nDev][nCh/*by wkhwang, 2006-05-25 6:35오후 -1*/];
    
    if (pDev == NULL)
    {
        printk("Pin_PolDetc_Callback: device error\n");
        return;
    }
 
    if (!pDev->HookMode)
    {
        /* Off hook mode */
        Sem_Lock(pDev->ExceSem);
        pDev->nException.Bits.polarity = 1;
        pDev->nException.Bits.flag = 1;
        Sem_Unlock(pDev->ExceSem);
        Wake_Up_Event(pDev->WakeupList);
    }
}




