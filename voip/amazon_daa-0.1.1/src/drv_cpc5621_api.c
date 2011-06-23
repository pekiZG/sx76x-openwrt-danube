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
   Module      : $RCSfile: drv_cpc5621_api.c,v $
   Date        : $Date: 2004/08/03 14:32:50 $
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
/** what string support, driver version string */
const char CPC5621_WHATVERSION[] = {DRV_CPC5621_WHAT_STR};


/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */
extern UINT32 CPC_IO_InstanceInit(CPC5621_DEV *pDev);
extern UINT32 CPC_IO_InstanceRelease(CPC5621_DEV *pDev);
extern UINT32 CPC_IO_GetValue(PIN_HANDLE *pHd, UINT32 *val);
extern UINT32 CPC_IO_SetValue(PIN_HANDLE *pHd, UINT32 val);

extern void Timer_Start(TIMER_ID *Tid, int ms, void *func, UINT32 arg);


/**
   Initializes the corresponding driver instance
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Init(
    CPC5621_DEV *pDev,          /**< private device data */
    int devNum,                 /**< device number of the DUSLIC */
    int chNum                   /**< channel number of the DUSLIC */
)
{

    if (pDev == NULL)
        return ERR;
        
    /* initialize a semaphore to protect the exception table */
    Sem_MutexInit(pDev->ExceSem);
    
    /* Create a timer for Ring function */
    INIT_TIMER(&pDev->Ring.TimerID);
    
    /* Create a timer for CID function */
    INIT_TIMER(&pDev->Cid.TimerID);
    
    /* Create a timer for Battery function */
    INIT_TIMER(&pDev->Bat.TimerID);
 
    //TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Init: %d, %d\r\n", devNum, chNum+1));

    /* open DUSLIC driver to get the driver handler */    
    if ((pDev->devHandle = DUSLIC_OpenKernel(devNum, chNum+1)) == ERR)
    {
    
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Init: DUSLIC open failed\r\n"));
        return ERR;
    }
    /* initialize a wait_queue list for the system poll function */ 
    Init_Event(pDev->WakeupList);
     
    return OK;
}


/**
   Release the corresponding driver instance
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Release(
    CPC5621_DEV *pDev       /**< private device data */
)
{
    int rc = OK;
   
    if (pDev == NULL)
        return ERR;
    if (pDev->bInit == TRUE)
    {
        if (CPC_IO_InstanceRelease(pDev) == ERR)
        {
            TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Release: instance release failed\r\n"));
            rc = ERR;
            goto result;
        }
    }
    else
    {
        rc = ERR;
        goto result;
    }
    if (DUSLIC_ReleaseKernel(pDev->devHandle) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Release: DUSLIC release failed\r\n"));
        rc = ERR;
    }

result:
    
    Sem_MutexDelete(pDev->ExceSem);
 
    return (rc);
}

/**
   Set OH pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_DevInit(
    CPC5621_DEV *pDev           /**< private device data */
)
{
    /* initialize the corresponding pin using DUSLIC inteface */
    if (CPC_IO_InstanceInit(pDev) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_DevInit: instance failed\r\n"));
        return ERR;
    }
   
    /* default: OH = 1, CID = 1 */
   
    CPC_IO_SetValue(&pDev->pinHdlTab[DRV_CPC5621_OH], (UINT32)IO_HIGH);
    CPC_IO_SetValue(&pDev->pinHdlTab[DRV_CPC5621_CID], (UINT32)IO_HIGH);
    pDev->HookMode = 1;    
    pDev->CidMode = 1;
    
    /* Set default value */
    pDev->Ring.tMin = RING_EXPIRE_TIME;
    pDev->Cid.tMin = CID_EXPIRE_TIME;
    pDev->Bat.tMin = BAT_EXPIRE_TIME;

    pDev->bInit = TRUE;
    return OK;
}

/**
   config corresponding data
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Config(
    CPC5621_DEV *pDev,      /**< private device data */
    UINT32      nArg        /**< optional argument */
)
{
    CPC5621_CONFIG *cfg;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (nArg == 0)
        return ERR;
    else
        cfg = (CPC5621_CONFIG *)nArg;
            
    if (cfg->Ring_tMin)
        pDev->Ring.tMin = cfg->Ring_tMin;
    
    if (cfg->Cid_tMin)
        pDev->Cid.tMin = cfg->Cid_tMin;

    if (cfg->Bat_tMin)
        pDev->Bat.tMin = cfg->Bat_tMin;
        
    return OK;
}


/**
   Set OH pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Set_OH(
    CPC5621_DEV *pDev,      /**< private device data */
    UINT32      nArg        /**< optional argument */
)
{
    if (pDev->bInit != TRUE)
        return ERR;
        
    if (nArg)
    {
        /* On hook mode */
        nArg = IO_HIGH;
        pDev->HookMode = 1;
    }
    else
    {
        /* Off hook mode */
        pDev->HookMode = 0;
    }
          
    if (CPC_IO_SetValue(&pDev->pinHdlTab[DRV_CPC5621_OH], nArg) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Set_OH: set value failed\r\n"));
        return ERR;
    }
        
    return OK;
}

/**
   Get OH pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_OH(
    CPC5621_DEV *pDev,       /**< private device data */
    UINT32      nArg         /**< optional argument */
)
{
    UINT32 tmp;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (CPC_IO_GetValue(&pDev->pinHdlTab[DRV_CPC5621_OH], &tmp) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_OH: get value failed\r\n"));
        return ERR;
    }
        
    copy_to_user((char*)nArg, (char *)&tmp, sizeof(UINT32));
    
    return OK;
}

/* Timer callback function for CID setting */
void CidTimerCB(unsigned long data)
{
    CPC5621_DEV *pDev = (CPC5621_DEV *)data;
    
    CPC_IO_SetValue(&pDev->pinHdlTab[DRV_CPC5621_CID], IO_HIGH);
    
    Sem_Lock(pDev->ExceSem);
    pDev->nException.Bits.cidTimeOut = 1;
    pDev->nException.Bits.flag = 1;
    Wake_Up_Event(pDev->WakeupList);
    Sem_Unlock(pDev->ExceSem);
}


/**
   Set CID pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Set_CID(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{
    if (pDev->bInit != TRUE)
        return ERR;
        
    if (nArg)
    {
        nArg = IO_HIGH;
        pDev->CidMode = 1;
    }
    else
        pDev->CidMode = 0;
    
    if (CPC_IO_SetValue(&pDev->pinHdlTab[DRV_CPC5621_CID], nArg) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Set_CID: get value failed\r\n"));
        return ERR;
    }
    
    /* start CID timer */
    if (nArg == IO_LOW)
    {
        Timer_Start(&pDev->Cid.TimerID, pDev->Cid.tMin, CidTimerCB, (UINT32)pDev);
    }
    
    return OK;
}

/**
   Get CID pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_CID(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{
    UINT32 tmp;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (CPC_IO_GetValue(&pDev->pinHdlTab[DRV_CPC5621_CID], &tmp) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_CID: get value failed\r\n"));
        return ERR;
    }
    
    copy_to_user((char*)nArg, (char *)&tmp, sizeof(UINT32));
    
    return OK;
}

/**
   Get Ring pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_Ring(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{   
    UINT32  tmp;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (CPC_IO_GetValue(&pDev->pinHdlTab[DRV_CPC5621_RING], &tmp) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_Ring: get value failed\r\n"));
        return ERR;
    }
    
    if (nArg)
        copy_to_user((char*)nArg, (char *)&tmp, sizeof(UINT32));

    return OK;
}

/**
   Get Battery Detect pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_BA_Detect(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{   
    UINT32  tmp;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (CPC_IO_GetValue(&pDev->pinHdlTab[DRV_CPC5621_BA_DETC], &tmp) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_BA_Detect: get value failed\r\n"));
        return ERR;
    }
    
    copy_to_user((char*)nArg, (char *)&tmp, sizeof(UINT32));

    return OK;
}

/**
   Get Polarity Detect pin status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_POL_Detect(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{
    UINT32  tmp;
    
    if (pDev->bInit != TRUE)
        return ERR;
    
    if (CPC_IO_GetValue(&pDev->pinHdlTab[DRV_CPC5621_POL_DETC], &tmp) == ERR)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_POL_Detect: get value failed\r\n"));
        return ERR;
    }
    
    copy_to_user((char*)nArg, (char *)&tmp, sizeof(UINT32));
    
    return OK;
}

/**
   Get Exception status
 
   \return
   OK:    Success
   ERR:   in case of error
*/
UINT32 Drv_CPC5621_Get_Exception(
    CPC5621_DEV *pDev,          /**< private device data */
    UINT32      nArg            /**< optional argument */
)
{
    if (pDev->bInit != TRUE)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("Drv_CPC5621_Get_Exception: get value failed\r\n"));
        return ERR;
    }
    
    Sem_Lock(pDev->ExceSem);
    copy_to_user((char*)nArg, (char *)&pDev->nException, sizeof(CPC5621_EXCEPTION));
    pDev->nException.Status = 0;
    Sem_Unlock(pDev->ExceSem);

    return OK;
}



