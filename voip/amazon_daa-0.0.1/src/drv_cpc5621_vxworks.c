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
   Module      : $RCSfile: drv_cpc5621_vxworks.c,v $
   Date        : $Date: 2004/07/27 09:46:05 $
   Description : Template Driver, VxWorks part
******************************************************************************/

#include "drv_api.h"
#include "drv_cpc5621_api.h"
#include "drv_cpc5621_interface.h"

#ifdef VXWORKS

/** driver number */
LOCAL INT32 TemplateDrvNum = 0;


/**
   Open the device.
 
   \return
   ERROR - on failure,
   non zero value on success
*/
LOCAL INT32 Template_Open(
    CPC5621_DEV *pDev,    /**< private device data */
    INT8 *pAnnex,          /**< remaing part of string used during open() */
    INT32 flags            /**< additional flags */
)
{
    TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("CPC5621_DRV: open\n"));

    if (pDev->bOpen == TRUE)
    {
        return (ERROR);
    }

    if (pDev->bNotFirst == FALSE)
    {}

    pDev->bOpen = TRUE;

    return (int)pDev;
}



/**
   Close the device.
 
   \return
   OK - on success
*/
LOCAL STATUS Template_Close(
    CPC5621_DEV *pDev     /**< private device data */
)
{
    TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("CPC5621_DRV: close\n"));

    pDev->bOpen = FALSE;

    /*
    intDisable(CPC5621_IRQ);
    */

    return (OK);
}


/**
   Read from device
 
   \return
   number of bytes returned in destination buffer or
   ERROR - on failure
*/
LOCAL INT32 Template_Read(
    CPC5621_DEV *pDev,    /**< private device data */
    UINT8 *pDst,           /**< destination buffer */
    INT32 nLength          /**< max. length to read */
)
{
    INT32 nBytes = ERROR;

    return nBytes;
}


/**
   Write to device
 
   \return
   number of bytes to write or
   ERROR - on failure
*/
LOCAL INT32 Template_Write(
    CPC5621_DEV *pDev,    /**< private device data */
    UINT8 *pSrc,           /**< source buffer */
    INT32 nLength          /**< length to write */
)
{
    INT32 nBytes = ERROR;

    return nBytes;
}


/**
   Configuration / Control for the device.
 
   \return
   OK and positive values - success,
   negative value - ioctl failed
*/
LOCAL INT32 Template_IoCtl(
    CPC5621_DEV *pDev,    /**< private device data */
    INT32 cmd,             /**< function id's */
    INT32 arg              /**< optional argument */
)
{
    INT32 ret = OK;

    switch(cmd)
    {
    case FIOSELECT:
        selNodeAdd(&(pDev->WakeupList), (SEL_WAKEUP_NODE*) arg);
#if 0
        /* check if there are already data in the IRQ fifo */
        if (pDev->pRxFifoRead != pDev->pRxFifoWrite)
        {
            selWakeup((SEL_WAKEUP_NODE*) arg);
        }
#endif
        break;

    case FIOUNSELECT:
        selNodeDelete(&(pDev->WakeupList), (SEL_WAKEUP_NODE*) arg);
        break;

    case FIO_CPC5621_DEBUGLEVEL:
        if ((arg >= DBG_LEVEL_LOW) &&  (arg <= DBG_LEVEL_OFF))
        {
            TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("CPC5621_DRV: DebugLevel %d\n", arg));
            SetTraceLevel(CPC5621_DRV,(UINT32)arg);
            SetLogLevel  (CPC5621_DRV,(UINT32)arg);
        }
        break;

    case FIO_CPC5621_GET_VERSION:
        strncpy((char*)arg, DRV_CPC5621_VER_STR, 80);
        break;

    case FIO_CPC5621_RESET_CONVERGATE_C:
        if (!Template_Reset_TemplateDevice(pDev, (UINT32)arg))
            ret = ERROR;
        break;

    case FIO_CPC5621_SET_LED:
        if (!Template_Set_LED(pDev, (UINT32)arg))
            ret = ERROR;
        break;

    case FIO_CPC5621_REG_SET:
        {
            CPC5621_REG_IO* pReg_Io = (CPC5621_REG_IO*)arg;
            if(!Template_Set_Register(pDev, pReg_Io->addr, pReg_Io->value))
                ret = ERROR;
        }
        break;

    case FIO_CPC5621_REG_GET:
        {
            CPC5621_REG_IO* pReg_Io = (CPC5621_REG_IO*)arg;
            if(!Template_Get_Register(pDev, pReg_Io->addr, &pReg_Io->value))
                ret = ERROR;
        }
        break;

    default:
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,
              ("CPC5621_DRV: Unknown IoCtl (0x%08X).\n", cmd));
        ret = ERROR;
    }

    return ret;
}



/**
   Remove the Template Driver .
 
   \return
   OK or ERROR
*/
INT32 Template_DevDelete(VOID)
{
    UINT32   dev_no;

    for (dev_no=0; dev_no < MAX_CPC5621_INSTANCES; dev_no++)
    {
        if (Template_Devices[dev_no] != NULL)
        {
            /* remove device */
            iosDevDelete((DEV_HDR *)Template_Devices[dev_no]);
            free(Template_Devices[dev_no]);
            Template_Devices[dev_no] = NULL;
        }
    }

    /* remove the driver also */
    iosDrvRemove(TemplateDrvNum, TRUE);
    /* and allow new DevCreate */
    TemplateDrvNum = 0;

    return (OK);
}



/**
   Template Driver initialization.
 
   \return
   OK or ERROR
*/
INT32 Template_DevCreate(
    const char *szDeviceName   /**< device name */
)
{
    CPC5621_DEV   *pDevice = NULL;
    char           buf[64];
    UINT32         dev_no;

    SetTraceLevel(CPC5621_DRV, DBG_LEVEL_LOW);
    SetLogLevel(CPC5621_DRV, DBG_LEVEL_LOW);

    printf("%s\n", &CPC5621_WHATVERSION[4]);
    printf("(c) Copyright 2003, Infineon Technologies AG\n\n\n");

    /* Driver already installed */
    if (TemplateDrvNum <= 0)
    {
        /* add driver to driver table */
        TemplateDrvNum = iosDrvInstall(NULL, NULL,
                                    (FUNCPTR)Template_Open,
                                    (FUNCPTR)Template_Close,
                                    (FUNCPTR)Template_Read,
                                    (FUNCPTR)Template_Write,
                                    (FUNCPTR)Template_IoCtl);

        if (TemplateDrvNum <= 0)
        {
            LOG(CPC5621_DRV, DBG_LEVEL_HIGH,
                ("CPC5621_DRV: unable to install the driver.\n"));
            return (ERROR);
        }
    }
    else
    {
        LOG(CPC5621_DRV, DBG_LEVEL_HIGH,
            ("CPC5621_DRV: driver already installed.\n"));
        return (ERROR);
    }

    for (dev_no=0; dev_no<MAX_CPC5621_INSTANCES; dev_no++)
    {
        pDevice = (CPC5621_DEV*)malloc(sizeof(CPC5621_DEV));

        if (!pDevice)
        {
            LOG(CPC5621_DRV, DBG_LEVEL_HIGH,
                ("CPC5621_DRV: no memory for device structure.\n"));
            return (ERROR);
        }

        memset((char*)pDevice, 0, sizeof(CPC5621_DEV));

        Template_Devices[dev_no] = pDevice;

        sprintf(buf,"%s/%d", DRV_CPC5621_NAME, dev_no);

        /* Add device to the device list */
        if (iosDevAdd(&(pDevice->DevHdr), buf, TemplateDrvNum) == ERROR)
        {
            LOG(CPC5621_DRV, DBG_LEVEL_HIGH,
                ("CPC5621_DRV: unable to create device.\n"));
            goto CPC5621_DEV_ERROR;
        }

        if (Template_Init(pDevice) == FALSE)
        {
            LOG(CPC5621_DRV, DBG_LEVEL_HIGH,("CPC5621_DRV: Init failed!\n"));
            goto CPC5621_DEV_ERROR;
        }

    }

    return (OK);

CPC5621_DEV_ERROR:

    Template_DevDelete();

    return (ERROR);
}


#endif /* VXWORKS */

