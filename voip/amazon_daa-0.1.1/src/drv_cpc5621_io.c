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
   Module      : $RCSfile: drv_cpc5621_io.c,v $
   Date        : $Date: 2004/08/06 13:33:43 $
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

/* ============================= */
/* Global variable definition    */
/* ============================= */
/** the modes are from DUSLIC's view, only for DUSLIC I/O interface */
int DefModeTable[DRV_CPC5621_MAX] = 
{
    _GPIO_MODE_OUTPUT,                                       /* OH */
    _GPIO_MODE_OUTPUT,                                       /* CID */
    _GPIO_MODE_INT | _GPIO_MODE_INPUT | _GPIO_INT_FALLING ,    /* RING */
    _GPIO_MODE_INPUT | _GPIO_MODE_INT | _GPIO_INT_FALLING | 
    _GPIO_INT_RISING | _GPIO_INT_DUP_05,                      /* Battery Detection, rising or falling?? */
    _GPIO_MODE_INPUT | _GPIO_MODE_INT | _GPIO_INT_FALLING      /* Polarity Detection, rising or falling?? */
};


//#define PORTA
#define EASY50510V
#ifdef DUSLIC
#ifdef PORTA
unsigned int MaskTable[DRV_CPC5621_MAX]=
{
             /*CPC5621----------DUSLIC*/
    0x01,    /*OH --------------IO1A (Out) */
    0x10,    /*CID--------------x    (Out) */ 
    0x08,    /*RING-------------IO4A (In/IntF) */ 
    0x04,    /*BAT--------------IO3A (In/IntFR */
    0x10     /*POL--------------x    (In/IntF) */
};
#elif defined(EASY50510V)
unsigned int MaskTable[DRV_CPC5621_MAX]=
{
             /*CPC5621----------DUSLIC*/
    0x01,    /*OH --------------IO1A  (Out) */
    0x10,    /*CID--------------X (C2A) (Out) */ 
    0x02,    /*RING-------------IO2A  (In/IntF)  */ 
    0x04,    /*BAT--------------IO3A  (In/IntFR) */
    0x08     /*POL--------------IO4A  (In/IntF)  */
};
#else
unsigned int MaskTable[DRV_CPC5621_MAX]=
{
             /*CPC5621----------DUSLIC*/
    0x02,    /*OH --------------IO2A  (Out) */
    0x01,    /*CID--------------IO1A  (Out) */ 
    0x04,    /*RING-------------IO3A  (In/IntF)  */ 
    0x10,    /*BAT--------------X     (In/IntFR) */
    0x08     /*POL--------------IO4A  (In/IntF)  */
};
#endif
#endif

extern _FUNCPTR callbackTable[];

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Initializes the corresponding driver instance
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 CPC_IO_InstanceInit(
    CPC5621_DEV *pDev               /**< private device data */
)
{
    DUSLIC_GPIO_CONFIG ioCfg;
    int i;
    int hd;
    
    if (pDev == NULL)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_InstanceInit: pDev=NULL\r\n"));
        return ERR;
    }
    
    if ((hd = DUSLIC_GpioReserve(pDev->devHandle, DRV_CPC5621_MASK)) == ERR)
    {
       
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_InstanceInit: GPIO reserve failed"));
        return ERR;
    }
        
    for (i = 0; i < DRV_CPC5621_MAX; i++)
    {
        pDev->pinHdlTab[i].id = i;
 #ifdef DUSLIC       
        pDev->pinHdlTab[i].nMask = MaskTable[i];
 #else
  	pDev->pinHdlTab[i].nMask = 1 << i; 
 #endif 
	pDev->pinHdlTab[i].ioHandle = hd;
        
        ioCfg.nMode = DefModeTable[i];
 #ifdef DUSLIC
        ioCfg.nGpio = MaskTable[i];
 #else
        ioCfg.nGpio = 1 << i;
 #endif
        ioCfg.callback = callbackTable[i];
        if (DUSLIC_GpioConfig(pDev->pinHdlTab[i].ioHandle, &ioCfg) == ERR)
        {
            TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_InstanceInit: GPIO configuration failed"));
            return ERR;
        }
    }  
    
    return OK;
}

/**
   Release the GPIO driver instance
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 CPC_IO_InstanceRelease(
    CPC5621_DEV *pDev               /**< private device data */
)
{
    if (pDev == NULL)
        return ERR;
    
    if (DUSLIC_GpioRelease(pDev->pinHdlTab[0].ioHandle) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_InstanceRelease: GPIO failed\r\n"));
        return ERR; 
    }

    return OK;
}

/**
   Get the pin value of the CPC device using DUSLIC I/O interface.
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 CPC_IO_GetValue(
    PIN_HANDLE *pHd,                /**< private pin handle */
    UINT32     *val                 /**< buffer for saving pin value */
)
{
    WORD nGet;
    
    if (pHd == NULL || val == NULL)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_GetValue: pHd=NULL\r\n"));
        return ERR;
    }
    
    if (DUSLIC_GpioGet(pHd->ioHandle, &nGet, pHd->nMask) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_GetValue: GPIO failed\r\n"));
        *val = -1;
        return ERR;
    }
    
    *val = (nGet & pHd->nMask) >> pHd->id;
 
    return OK;
}


/**
   Set the pin value of the CPC device using DUSLIC I/O interface.
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 CPC_IO_SetValue(
    PIN_HANDLE *pHd,                /**< private pin handle */
    UINT32      val                 /**< value for setting the pin */
)
{   
    if (pHd == NULL)
        return ERR;
   
    if (DUSLIC_GpioSet(pHd->ioHandle, val, pHd->nMask) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("CPC_IO_SetValue: GPIO failed\r\n"));
        return ERR;
    }
         
    return OK;
}



