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
   Module      : $RCSfile: drv_cpc5621_noOS.c,v $
   Date        : $Date: 2004/08/06 12:01:55 $
   Description : CPC5621 Device Driver, noOS part
******************************************************************************/

#include "drv_cpc5621_api.h"

#ifdef NO_OS



SEM_ID CpcSemDrv = 0;

static BYTE major_number = DRV_CPC5621_MAJOR;
static BYTE debug_level = DBG_LEVEL_LOW;



/* the driver callbacks */
static struct file_operations cpc5621_fops =
{
    owner:
        THIS_MODULE,
    read:
        cpc5621_read,
    write:
        cpc5621_write,
    poll: 
        cpc5621_poll,
    ioctl:
        cpc5621_ioctl,
    open:
        cpc5621_open,
    release:
        cpc5621_release
};

static uint cpc5621_intcount = 0;

/* cpc5621 devices names */
LOCAL const CHAR *Cpc5621_DevPath [MAX_DEVICE][MAX_CHANNEL] =
{
    {
      "/dev/cpc11",
      "/dev/cpc12",
    },
    {
      "/dev/cpc21",
      "/dev/cpc22",
    }
}

   

/**
  Search the major mumber of the CPC5621 
  
  path - the path and name of the device
 
   \return
   nDevn - device mumber
   nCh - device channel number
*/
LOCAL INT  Get_Dev_Num (CHAR *path, int *nDev, int *nCh)
{
   INT i, j;

   for (i = 0; i < MAX_DEVICE; i++)
   {
      for (j = 0; j <= MAX_CHANNEL ; j++)
      {
         if (!strcmp ((path), Cpc5621_DevPath [i][j]))
         {
            nDevn = i;
            nCh = j;
            break;
         }
      }
   }
   
   if (i == MAX_DEVICE && j == MAX_CHANNEL)
       return ERR;

   return OK;
}


/**
  Open the device.
  
  At the first time:
  - allocating internal memory for each new device
  - initialize the device
  - set up the interrupt
 
   \return
   0 - if no error,
   otherwise error code
*/
static int cpc5621_open (
    char    *path,      /**< pointer to the device path */
    INT     flags       /**< flag of the open */
)
{
    int result = 0;
    int devNum, chNum;
    CPC5621_DEV *pDev = NULL;
    
    if (Get_Dev_Num(path, &devNum, &chNum) == ERR)
        return -ENODEV;

    /* check the device number */
    if (chNum >= MAX_CHANNEL)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("cpc5621_open: max. device number exceed\n"));
        result = -ENODEV;
        goto OPEN_ERROR;
    }
    
    Sem_Lock(CpcSemDrv);

    pDev = CPC5621_Devices[devNum][chNum];

    if (pDev == NULL)
    {
        pDev = (CPC5621_DEV*)Alloc_Mem(sizeof(CPC5621_DEV));
        if (!pDev)
        {
            TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("cpc5621_open: no memory for device structure.\n"));
            result = -ENOMEM;
            goto OPEN_ERROR;
        }

        memset((char*)pDev, 0, sizeof(CPC5621_DEV));

        /* The device number and channel number should be getted from upper layer */
        if (Drv_CPC5621_Init(pDev, devNum, chNum) == ERR)
        {
            TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("cpc5621_open: driver init failed!\n"));
            result = -ENOMEM;
            goto OPEN_ERROR;
        }
        
        CPC5621_Devices[devNum][chNum] = pDev;
    }

    if (pDev->bOpen == TRUE)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("cpc5621_open: already opened!\n"));
        return -EBUSY;
    }

    pDev->bOpen = TRUE;
    pDev->nInUse++;

    /* and use filp->private_data to point to the device data */
    filp->private_data = pDev;
    
    Sem_Unlock(CpcSemDrv);

    return (int)(pDev);

OPEN_ERROR:

    if (pDev != NULL)
    {
        Free_Mem(pDev);
        CPC5621_Devices[devNum][chNum] = NULL;
    }
    
    Sem_Unlock(CpcSemDrv);
    
    return result;
}


/**
   Release the device.
 
   \return
   0 - on success
*/
static int cpc5621_release (
    void *device,               /**< pointer to the device */
)
{
    CPC5621_DEV *pDev = (CPC5621_DEV)device;
    int devNum, chNum;
    
    Sem_Lock(CpcSemDrv);
    
    Drv_CPC5621_Release(pDev);  
    
    Free_Mem(pDev);
    
    Sem_Unlock(CpcSemDrv);

    return 0;
}


/**
   Writes data to the device.
 
   \return
   length or a negative error code
*/
static ssize_t cpc5621_write(
    void *pDev,             /**< pointer to the device descriptor */
    const char *buf,        /**< source buffer */
    size_t count,           /**< data length */
    loff_t * ppos           /**< */
)
{
    int total = 0;
    
    CPC5621_DEV *pDev = (CPC5621_DEV*)pDev;

    /* check pointer */
    if (pDev == NULL)
    {
        return -ENODEV;
    }

    return total;
}


/**
   Reads data from the device.
 
   \return
   len - data length
*/
static ssize_t cpc5621_read(
    void *pDev,             /**< pointer to the device descriptor */
    char *buf,              /**< destination buffer */
    size_t count,           /**< max size of data to read */
    loff_t * ppos           /**< */
)
{
    int len = 0;
    CPC5621_DEV *pDev = (CPC5621_DEV*)pDev;

    /* check pointer */
    if (pDev == NULL)
    {
        return -ENODEV;
    }

    return len;
}


/**
   Configuration / Control for the device.
 
   \return
   0 and positive values - success,
   negative value - ioctl failed
*/
static int cpc5621_ioctl(
    void *pDev,             /**< pointer to the device */
    unsigned int nCmd,      /**< function id's */
    unsigned long nArgument /**< optional argument */
)
{
    INT ret = 0;
    CPC5621_DEV *pDev = (CPC5621_DEV*)pDev;
    
    if (pDev == NULL)
    {
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,("cpc5621_ioctl: no device\n"));
        return -ENODEV;
    }
    
    Sem_Lock(CpcSemDrv);

    switch (nCmd)
    {
    case FIO_CPC5621_DEBUGLEVEL:
        if ((nArgument >= DBG_LEVEL_LOW) && (nArgument <= DBG_LEVEL_OFF))
        {
            SetTraceLevel(CPC5621_DRV,nArgument);
            SetLogLevel  (CPC5621_DRV,nArgument);
        }
        break;

    case FIO_CPC5621_GET_VERSION:
        {
            int len = strlen(DRV_CPC5621_VER_STR);
            if (copy_to_user( (char*)nArgument, DRV_CPC5621_VER_STR, ((len<80)?len:80)) <0)
                ret = -EINVAL;
        }
        break;
        
    case FIO_CPC5621_INIT:
        if (Drv_CPC5621_DevInit(pDev) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_CONFIG:
        if (Drv_CPC5621_Config(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;

    case FIO_CPC5621_SET_OH:
        if (Drv_CPC5621_Set_OH(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_OH:
        if (Drv_CPC5621_Get_OH(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_SET_CID:
        if (Drv_CPC5621_Set_CID(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_CID:
        if (Drv_CPC5621_Get_CID(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_RING:
        if (Drv_CPC5621_Get_Ring(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_BAT_DETECT:
        if (Drv_CPC5621_Get_BA_Detect(pDev, nArgument)== ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_POL_DETECT:
        if (Drv_CPC5621_Get_POL_Detect(pDev, nArgument)== ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_CID_CLEAR:
        if (Drv_CPC5621_CID_Clear(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_RING_CLEAR:
        if (Drv_CPC5621_Ring_Clear(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_BAT_CLEAR:
        if (Drv_CPC5621_BAT_Clear(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_POL_CLEAR:
        if (Drv_CPC5621_POL_Clear(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_EXCEPTION:
        if (Drv_CPC5621_Get_Exception(pDev, nArgument)== ERR)
            ret = -EINVAL;
        break;

    default:
        TRACE(CPC5621_DRV, DBG_LEVEL_HIGH,
              ("cpc5621_ioctl: Unknown IoCtl (0x%08X), arg 0x%08lX.\n", nCmd, nArgument));
        ret = -EINVAL;
        break;
    }
    
    Sem_Unlock(CpcSemDrv);

    return ret;
}


/**
   Configuration / Poll for the device.
 
   \return
   0 and positive values - success,
   negative value - ioctl failed
*/
static unsigned int cpc5621_poll(
    void *pDev,                 /**< pointer to the device descriptor */
    poll_table *wait            /**< pointer to poll table */
)
{
    CPC5621_DEV *pDev = (CPC5621_DEV*)pDev;
    int ret = SYS_NOSELECT;
   
    /* install the poll queues of events to poll on */
    poll_wait(file, &pDev->WakeupList, wait);
    
    spin_lock_irq(&pDev->lock);
    
    Sem_Lock(pDev->ExceSem);
    
    if (pDev->nException.Bits.flag)
    {   
        pDev->nException.Bits.flag = 0;
        
        /* exception available so return action */
        ret = SYS_EXCEPT;
    }
    
    Sem_Unlock(pDev->ExceSem);
    
    spin_unlock_irq(&pDev->lock);
    
    return ret;
}



/**
   Initialize the module
 
   \return
   Error code or 0 on success
   \remark
   Called by the kernel.
*/
int CPC5621_Drv_Install(void)
{
    int result;

    printk(KERN_INFO "%s\n", CPC5621_WHATVERSION);
    printk(KERN_INFO "(c) Copyright 2004, Infineon Technologies AG\n");

    TRACE(CPC5621_DRV, DBG_LEVEL_HIGH, ("init_module\r\n"));
    
    SetTraceLevel(CPC5621_DRV, debug_level);
    SetLogLevel  (CPC5621_DRV, debug_level);
    
    Sem_MutexInit(CpcSemDrv);

/* depend on the different OS platform */
#if 0 
    result = NO_OS_Install_Driver(&cpc5621_fops);
#endif

    if (result < 0)
    {
        TRACE(CPC5621_DRV,DBG_LEVEL_HIGH,("CPC5621_DRV: can't get major %d\n",
                                         major_number));
        return result;
    }

    /* reset the device pointer */
    memset(CPC5621_Devices, 0x00, sizeof(CPC5621_Devices));

    return 0;
}


/**
   Clean up the module if unloaded.
 
   \remark
   Called by the kernel.
*/
void cleanup_module(void)
{
#if 0
    NO_OS_Uninstall_Driver(DRV_CPC5621_NAME);
#endif

    TRACE(CPC5621_DRV, DBG_LEVEL_NORMAL, ("CPC5621_DRV: cleanup successful\n"));
}




#endif /* NO_OS */



