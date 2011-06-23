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
   Module      : $RCSfile: drv_cpc5621_linux.c,v $
   Date        : $Date: 2004/08/02 08:41:42 $
   Description : CPC5621 Device Driver, Linux part
******************************************************************************/

#include "drv_cpc5621_api.h"

#ifdef LINUX

static int cpc5621_open (struct inode *inode, struct file *filp);
static int cpc5621_release (struct inode *inode, struct file *filp);
static ssize_t cpc5621_write(struct file *file_p, const char *buf, size_t count, loff_t * ppos);
static ssize_t cpc5621_read(struct file * file_p, char *buf, size_t length, loff_t * ppos);
static int cpc5621_ioctl(struct inode *inode, struct file *filp, unsigned int nCmd, unsigned long nArgument);
static unsigned int cpc5621_poll (struct file *filp, poll_table *table);


SEM_ID CpcSemDrv = 0;

static BYTE major_number = DRV_CPC5621_MAJOR;
MODULE_PARM(major_number, "b");
MODULE_PARM_DESC(major_number, "to override automatic major number");

static BYTE debug_level = DBG_LEVEL_LOW;
MODULE_PARM(debug_level, "b");
MODULE_PARM_DESC(debug_level, "set to get more (1) or fewer (4) debug outputs");

/** pointer to device structures. */
/* static */ CPC5621_DEV* CPC5621_Devices[MAX_DEVICE][MAX_CHANNEL];


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
    struct inode *inode,    /**< pointer to the inode */
    struct file *filp       /**< pointer to the file descriptor */
)
{
    int num = MINOR(inode->i_rdev);  /* the real device */
    int result = 0;
    int devNum, chNum;
    CPC5621_DEV *pDev = NULL;
    
    devNum = (num / DRV_CPC5621_MINOR_BASE) - 1;
    chNum = 0; // by wkhwang, 2006-05-25 5:44오후 num - DRV_CPC5621_MINOR_BASE - 1;

//printk("+cpc5621_open, num=%d, devNum=%d, chNum=%d \n", num, devNum, chNum);

    /* check the device number */
    if (chNum >= MAX_CHANNEL)
    {
        printk ("cpc5621_open: max. device number exceed\n");
        result = -ENODEV;
        goto OPEN_ERROR;
    }
    
    Sem_Lock(CpcSemDrv);

    pDev = CPC5621_Devices[devNum][chNum];
//    printk("%s: open devNum=%d, chNum=%d, pDev=%08x, CPC5621_Devices=%08x\n", __FUNCTION__, devNum, chNum, (int)pDev, (int)CPC5621_Devices);

    if (pDev == NULL)
    {

        pDev = (CPC5621_DEV*)Alloc_Mem(sizeof(CPC5621_DEV));
        //pDev = (CPC5621_DEV*)kmalloc(sizeof(CPC5621_DEV),GFP_KERNEL);
        if (!pDev)
        {
            printk("cpc5621_open: no memory for device structure.\n");
            result = -ENOMEM;
            goto OPEN_ERROR;
        }
        memset((char*)pDev, 0, sizeof(CPC5621_DEV));
        pDev->bOpen=FALSE;	


        /* The device number and channel number should be getted from upper layer */
        if (Drv_CPC5621_Init(pDev, devNum, chNum) == ERR)
        {
            printk("cpc5621_open: driver init failed!\n");
            result = -ENOMEM;
            goto OPEN_ERROR;
        }
        CPC5621_Devices[devNum][chNum] = pDev;
    }
//    printk("%s: open devNum=%d, chNum=%d, pDev=%08x\n", __FUNCTION__, devNum, chNum, (int)pDev);
    
    if (pDev->bOpen == TRUE)
    {
        printk("cpc5621_open: already opened!\n");
        return -EBUSY;
    }

    pDev->bOpen = TRUE;
    pDev->nInUse++;
    /* and use filp->private_data to point to the device data */
    filp->private_data = pDev;

    /* increment module use counter */
    MOD_INC_USE_COUNT;
    
    Sem_Unlock(CpcSemDrv);

    return result;
    
OPEN_ERROR:

    printk("%s: open error\n", __FUNCTION__);
    if (pDev != NULL)
    {
        Free_Mem(pDev);
//          kfree(pDev);
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
    struct inode *inode,    /**< pointer to the inode */
    struct file *filp       /**< pointer to the file descriptor */
)
{
    int num = MINOR(inode->i_rdev);  /* the real device */
    CPC5621_DEV *pDev;
    int devNum, chNum;

    devNum = (num / DRV_CPC5621_MINOR_BASE) - 1;
    chNum = 0; //by wkhwang,2006-05-25 7:55오후  num - DRV_CPC5621_MINOR_BASE - 1;

    /* check the device number */
    if (chNum >= MAX_CHANNEL)
    {
        printk ("cpc5621_release: max. device number exceed\n");
        return -ENODEV;
    }
    pDev = (CPC5621_DEV*)filp->private_data;
//    printk("%s: open devNum=%d, chNum=%d, pDev=%08x\n", __FUNCTION__, devNum, chNum, (int)pDev);

    Sem_Lock(CpcSemDrv);

    pDev = (CPC5621_DEV*)filp->private_data;
    
    Drv_CPC5621_Release(pDev);  
    
    Free_Mem(pDev);
    
    CPC5621_Devices[devNum][chNum] = NULL;

    Sem_Unlock(CpcSemDrv);

    /* decrement use counter */
    MOD_DEC_USE_COUNT;
    return 0;
}


/**
   Writes data to the device.
 
   \return
   length or a negative error code
*/
static ssize_t cpc5621_write(
    struct file *file_p,    /**< pointer to the file descriptor */
    const char *buf,        /**< source buffer */
    size_t count,           /**< data length */
    loff_t * ppos           /**< */
)
{
    int total = 0;
    CPC5621_DEV *pDev = (CPC5621_DEV*)file_p->private_data;

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
    struct file *file_p,    /**< pointer to the file descriptor */
    char *buf,              /**< destination buffer */
    size_t count,           /**< max size of data to read */
    loff_t * ppos           /**< */
)
{
    int len = 0;
    CPC5621_DEV *pDev = (CPC5621_DEV*)file_p->private_data;

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
    struct inode *inode,    /**< pointer to the inode */
    struct file *file_p,    /**< pointer to the file descriptor */
    unsigned int nCmd,      /**< function id's */
    unsigned long nArgument /**< optional argument */
)
{
    INT ret = 0;
    CPC5621_DEV *pDev = (CPC5621_DEV*)file_p->private_data;
    
    if (pDev == NULL)
    {
        printk("cpc5621_ioctl: no device\n");
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
#if 0 // by wkhwang, 		
        if (Drv_CPC5621_DevInit(pDev) == ERR)
#else
	if (Drv_CPC5621_DevInit(pDev, nArgument) == ERR)
#endif
	{
	     printk("return value negative!\n");
	     ret = -EINVAL;
	    }
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

#if 0 // by wkhwang, NO CID function       
    case FIO_CPC5621_SET_CID:
        if (Drv_CPC5621_Set_CID(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
        
    case FIO_CPC5621_GET_CID:
        if (Drv_CPC5621_Get_CID(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
#endif
#if 0 // by wkhwang, 
    case FIO_CPC5621_GET_RING2:
        if (Drv_CPC5621_Get_RING2(pDev, nArgument) == ERR)
            ret = -EINVAL;
        break;
#endif

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
        
    case FIO_CPC5621_EXCEPTION:
        if (Drv_CPC5621_Get_Exception(pDev, nArgument)== ERR)
            ret = -EINVAL;
        break;

    default:
        printk      ("cpc5621_ioctl: Unknown IoCtl (0x%08X), arg 0x%08lX.\n", nCmd, nArgument);
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
    struct file *file,          /**< pointer to the file descriptor */
    poll_table *wait            /**< pointer to poll table */
)
{
    CPC5621_DEV *pDev = (CPC5621_DEV*)file->private_data;
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


#if CONFIG_PROC_FS
/**
   Read the version information from the driver.
 
   \return
   length
*/
static int cpc5621_get_version_proc(
    char *buf      /**< destination buffer */
)
{
    int len;

    len = sprintf(buf, "%s\n", &CPC5621_WHATVERSION[4]);

    len += sprintf(buf + len, "Compiled on %s, %s for Linux kernel %s\n",
                   __DATE__, __TIME__, UTS_RELEASE);

    return len;
}


/**
   Read the status information from the driver.
 
   \return
   length
*/
static int cpc5621_get_status_proc(
    char *buf      /**< destination buffer */
)
{
    int len=0;
    int i, j;

    len += sprintf(buf+len, "Interrupt-Count %d\n", cpc5621_intcount);

    for (i=0; i<MAX_DEVICE; i++)
    { 
        for (j=0; j<MAX_CHANNEL; j++)
        {
            len += sprintf(buf+len, "********************************\n");
            len += sprintf(buf+len, "pDev(%d, %d) = 0x%08X\n", i, j, (int)CPC5621_Devices[i][j]);
            len += sprintf(buf+len, "--------------------------------\n");
            if (CPC5621_Devices[i][j] != NULL)
            {
                len += sprintf(buf+len, "bInit   = %s\n", CPC5621_Devices[i][j]->bInit?"TRUE":"FALSE");
                len += sprintf(buf+len, "bOpen   = %s\n", CPC5621_Devices[i][j]->bOpen?"TRUE":"FALSE");
            }
        }
    }

    return len;
}


/**
   The proc filesystem: function to read and entry.
 
   \return
   length
*/
static INT cpc5621_read_proc(char *page, char **start, off_t off,
                            int count, int *eof, void *data)
{
    int len;

    int (*fn)(char *buf);

    if (data != NULL)
    {
        fn = data;
        len = fn(page);
    }
    else
        return 0;

    if (len <= off+count)
        *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count)
        len = count;
    if (len<0)
        len = 0;
    return len;
}
#endif


/**
   Initialize the module
 
   \return
   Error code or 0 on success
   \remark
   Called by the kernel.
*/
int init_module(void)
{
    int result;
#if CONFIG_PROC_FS
    struct proc_dir_entry *driver_proc_node;
#endif

    printk(KERN_INFO "%s\n", CPC5621_WHATVERSION);
    printk(KERN_INFO "(c) Copyright 2004, Infineon Technologies AG\n");

    printk ("init_module\r\n");
    
    SetTraceLevel(CPC5621_DRV, debug_level);
    SetLogLevel  (CPC5621_DRV, debug_level);
    
    Sem_MutexInit(CpcSemDrv);

    result = register_chrdev(major_number , DRV_CPC5621_NAME , &cpc5621_fops);

    if (result < 0)
    {
        printk("CPC5621_DRV: can't get major %d\n",
                                         major_number);
        return result;
    }
    
    /* dynamic major */
    if (major_number == 0)
    {
        major_number = result;
        printk("Using major number %d\n",
                                         major_number);
    }

    /* reset the device pointer */
    memset(CPC5621_Devices, 0x00, sizeof(CPC5621_Devices));

#if CONFIG_PROC_FS
    /* install the proc entry */
    printk("CPC5621_DRV: using proc fs\n");

    driver_proc_node = proc_mkdir( "driver/" DRV_CPC5621_NAME, NULL);
    if (driver_proc_node != NULL)
    {
        create_proc_read_entry( "version" , S_IFREG|S_IRUGO,
                                driver_proc_node, cpc5621_read_proc, (void *)cpc5621_get_version_proc );
        create_proc_read_entry( "status" , S_IFREG|S_IRUGO,
                                driver_proc_node, cpc5621_read_proc, (void *)cpc5621_get_status_proc );
    }
    else
    {
        printk("init_module: cannot create proc entry\n");
    }
#endif

    return 0;
}


/**
   Clean up the module if unloaded.
 
   \remark
   Called by the kernel.
*/
void cleanup_module(void)
{
    int devNum, chNum;
    CPC5621_DEV *pDev = NULL;
    unregister_chrdev(major_number , DRV_CPC5621_NAME);
 

#if CONFIG_PROC_FS
    remove_proc_entry("driver/" DRV_CPC5621_NAME "/version" ,0);
    remove_proc_entry("driver/" DRV_CPC5621_NAME "/status",0);
    remove_proc_entry("driver/" DRV_CPC5621_NAME ,0);
#endif

    for(devNum=0;devNum<MAX_DEVICE;devNum++)
    {
       for(chNum=0;chNum<MAX_CHANNEL;chNum++)
       {
         pDev = CPC5621_Devices[devNum][chNum];
    
         if(pDev)
         {
          Drv_CPC5621_Release(pDev);  
    
          Free_Mem(pDev);
    
          CPC5621_Devices[devNum][chNum] = NULL;
         }

        }
    }
    printk ("CPC5621_DRV: cleanup successful\n");
}


/****************************************************************************/

MODULE_AUTHOR("Infineon Technologies");
MODULE_DESCRIPTION("CPC5621 I/O Pins Access");
MODULE_SUPPORTED_DEVICE("CPC5621 DEVICE");

#endif /* LINUX */



