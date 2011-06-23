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

#if 1 // by wkhwang, 2006-05-25 4:47¿ÀÈÄ
typedef enum
{
	_GPIO_MODE_INPUT	=	0x0100,
	_GPIO_MODE_OUTPUT =	0x0200,
	_GPIO_MODE_INT =		0x0400,
	_GPIO_INT_RISING	=	0x1000,
	_GPIO_INT_FALLING =	0x2000,
	_GPIO_INT_DUP_05 = 	0x0000,
	_GPIO_INT_DUP_45 = 	0x0001,
	_GPIO_INT_DUP_85 = 	0x0002,
	_GPIO_INT_DUP_125 =	0x0003,
	_GPIO_INT_DUP_165 =	0x0004,
	_GPIO_INT_DUP_205 =	0x0005,
	_GPIO_INT_DUP_245 =	0x0006,
	_GPIO_INT_DUP_285 =	0x0007,
	_GPIO_INT_DUP_325 =	0x0008,
	_GPIO_INT_DUP_365 =	0x0009,
	_GPIO_INT_DUP_405 =	0x000A,
	_GPIO_INT_DUP_445 =	0x000B,
	_GPIO_INT_DUP_485 =	0x000C,
	_GPIO_INT_DUP_525 =	0x000D,
	_GPIO_INT_DUP_565 =	0x000E,
	_GPIO_INT_DUP_605 =	0x000F
} DUSLIC_GPIO_MODE;
#endif

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
    _GPIO_MODE_OUTPUT,                                       					/* OH */
#if 0  // by wkhwang,
    _GPIO_MODE_INT | _GPIO_MODE_INPUT | _GPIO_INT_FALLING ,    	/* RING */
    _GPIO_MODE_INPUT | _GPIO_MODE_INT | _GPIO_INT_FALLING | 
    _GPIO_INT_RISING | _GPIO_INT_DUP_05,                      			/* Battery Detection, rising or falling?? */
    _GPIO_MODE_INPUT | _GPIO_MODE_INT | _GPIO_INT_FALLING,      	/* Polarity Detection, rising or falling?? */
#else
    _GPIO_MODE_INPUT, /*RING*/
    _GPIO_MODE_INPUT, /*BATTERY */
    _GPIO_MODE_INPUT, /*POLARITY */
#endif
    _GPIO_MODE_INPUT/*by wkhwang _GPIO_MODE_OUTPUT*/		/* RING2  */
};

#if 0 // by wkhwang, 
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
    0x01,    /*OH --------------IO2A  (Out) */
    0x02,    /*RING-------------IO3A  (In/IntF)  */ 
    0x04,    /*BAT--------------X     (In/IntFR) */
    0x08,     /*POL--------------IO4A  (In/IntF)  */
    0x10    /*CID--------------IO1A  (Out) */ 
};
#endif
#endif
#endif  // by wkhwang, 

unsigned int MaskTable[DRV_CPC5621_MAX]=
{
                /*CPC5621----------VINETIC*/
    0x01,    /*OH --------------IO2A  (Out) */
    0x02,    /*RING-------------IO3A  (In/IntF)  */ 
    0x04,    /*BAT--------------X     (In/IntFR) */
    0x08,    /*POL--------------IO4A  (In/IntF)  */
    0x10     /*by wkhwang, RING2 (In), CID--------------IO1A  (Out) */ 
};


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
    VINETIC_GPIO_CONFIG ioCfg;
    int i;
    int hd;
    
    if (pDev == NULL)
    {
        printk ("CPC_IO_InstanceInit: pDev=NULL\r\n");
        return ERR;
    }
    
    if ((hd = VINETIC_GpioReserve(pDev->devHandle, DRV_CPC5621_MASK)) == ERR)
    {
       
        printk ("CPC_IO_InstanceInit: GPIO reserve failed\n");
        return ERR;
    }
        
    for (i = 0; i < DRV_CPC5621_MAX; i++)
    {
        pDev->pinHdlTab[i].id = i;

#if 0 // by wkhwang
 #ifdef DUSLIC       
        pDev->pinHdlTab[i].nMask = MaskTable[i];
 #else
  	pDev->pinHdlTab[i].nMask = 1 << i; 
 #endif 
#else //
         pDev->pinHdlTab[i].nMask = MaskTable[i];
#endif
 
	pDev->pinHdlTab[i].ioHandle = hd;
        
        ioCfg.nMode = DefModeTable[i];
#if 0		
 #ifdef DUSLIC
        ioCfg.nGpio = MaskTable[i];
 #else
        ioCfg.nGpio = 1 << i;
 #endif
 #else
        ioCfg.nGpio = MaskTable[i];
 #endif
 
        ioCfg.callback = callbackTable[i];
        if (VINETIC_GpioConfig(pDev->pinHdlTab[i].ioHandle, &ioCfg) == ERR)
        {
            printk ("CPC_IO_InstanceInit: GPIO configuration failed\n");
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
    
    if (VINETIC_GpioRelease(pDev->pinHdlTab[0].ioHandle) == ERR)
    {
        printk ("CPC_IO_InstanceRelease: GPIO failed\r\n");
        return ERR; 
    }

    return OK;
}

/**
   Get the pin value of the CPC device using VINETIC I/O interface.
 
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
        printk ("CPC_IO_GetValue: pHd=NULL\r\n");
        return ERR;
    }
    
    if (VINETIC_GpioGet(pHd->ioHandle, &nGet, pHd->nMask) == ERR)
    {
        printk ("CPC_IO_GetValue: GPIO failed\r\n");
        *val = -1;
        return ERR;
    }
/*
printk("++CPC_IO_GetValue ... nGet=0x%x, pHd->nMask=0x%x, pHd->id=0x%x\n", 
	nGet, pHd->nMask, pHd->id );
*/	
    *val = (nGet & pHd->nMask) >> pHd->id;
 
    return OK;
}


/**
   Set the pin value of the CPC device using VINETIC I/O interface.
 
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
/* 20061015.ksg    
printk("+CPC_IO_SetValue, val=0x%x, pHd->nMask=0x%x \n", val, pHd->nMask );
*/    
    if (VINETIC_GpioSet(pHd->ioHandle, val, pHd->nMask) == ERR)
    {
        printk ("CPC_IO_SetValue: GPIO failed\r\n");
        return ERR;
    }
         
    return OK;
}



