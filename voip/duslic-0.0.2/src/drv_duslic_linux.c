/****************************************************************************
     Copyright (c) 2002-2004, Infineon Technologies.  All rights reserved.

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
 ****************************************************************************
   Module      : drv_DUSLIC_linux.c
   Description : This file contains the implementation of the linux specific
                 driver functions.
*******************************************************************************/
#include <linux/config.h> /* retrieve the CONFIG_* macros */
#include "duslic_io.h"

#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#   define MODVERSIONS
#endif


#if defined(MODVERSIONS) && !defined(__GENKSYMS__)
#    include <linux/modversions.h>
#endif


#ifndef EXPORT_SYMTAB
#  define EXPORT_SYMTAB /* need this one 'cause we export symbols */
#endif


/* ============================= */
/* Includes                      */
/* ============================= */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <asm/uaccess.h>
#include <linux/tty.h>
#include <linux/selection.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/fcntl.h>


#include "drv_api.h"
#include "drv_board.h"

#include "drv_duslic_api.h"
#include "drv_duslic_main.h"
#include "drv_duslic_int.h"
#include "drv_duslic_basic.h"
#include "drv_duslic_misc.h"


#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
#endif /* CONFIG_DEVFS_FS */


#ifdef USE_VINETIC_GPIO
#include "vinetic_io.h"
int vinetic_gpio_hd;
void DUSLIC_Callback(int nDev, int nCh, WORD nEvt);
DUSLIC_DEVICE* g_pDev;
#endif

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#error "__BYTE_ORDER == __LITTLE_ENDIAN"
#endif

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/* maximum space for data exchange in IOCTL functions */
#define MAX_TRANSSPACE   9216 /* 9 K */
#define DUSLIC_INT_VIN_GPIO 0x20 /* Slicofi interrupt line connected to Vinetic GPIO1 */

MODULE_DESCRIPTION("DUSLIC driver - www.infineon.com");
MODULE_AUTHOR("Infineon Technologies, COM AC SD SW");


/****************************************************************************
Description:
   dispatches ioctrl commands and manages user space data handling in both
   directions
Arguments:
   msg   - ioctrl command id
   func  - function name
   arg   - structure identifier used as argument for the function call and
           for copying user data
Remarks:
   As an alternative use a transfer structure DUSLIC_IO_USR and
   DoDataExchange for handling data exchange.
Example:
   call function DUSLIC_Read_Cmd if id DUSLIC_RMAIL received
   IFX_uint8_t IOcmd [660];
   ...
   switch (nCmd)
   {
      ON_IOCTL(DUSLIC_RMAIL, DUSLIC_Read_Cmd, IO_READ_CMD);
   }
****************************************************************************/
#define ON_IOCTL(msg,func,arg)                                                  \
      case msg:                                                                 \
      if (nArgument) {                                                          \
          copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (arg));        \
          ret = func(filp->private_data, (arg*) IOcmd);                         \
          copy_to_user ((IFX_uint8_t*)nArgument, IOcmd, sizeof (arg));          \
      } else {                                                                  \
          ret = func(filp->private_data, 0);                                    \
      }                                                                         \
      break

/****************************************************************************
Description:
   dispatches ioctrl commands and manages user space data handling in
   driver direction
Arguments:
   msg   - ioctrl command id
   func  - function name
   arg   - structure identifier used as argument for the function call and
           for copying user data
****************************************************************************/
#define ON_IOCTL_FRUSR(msg,func,arg)                                            \
      case msg:                                                                 \
      if (nArgument) {                                                          \
          copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (arg));        \
          ret = func(filp->private_data, (arg*) IOcmd);                         \
      } else {                                                                  \
          ret = func(filp->private_data, 0);                                    \
      }                                                                         \
      break

/****************************************************************************
Description:
   dispatches ioctrl commands and manages user space data handling in
   user space direction
Arguments:
   msg   - ioctrl command id
   func  - function name
   arg   - structure identifier used as argument for the function call and
           for copying user data
****************************************************************************/
#define ON_IOCTL_TOUSR(msg,func,arg)                                    \
      case msg:                                                         \
      if (nArgument) {                                                  \
          ret = func(filp->private_data, (arg*) IOcmd);                 \
          copy_to_user ((IFX_uint8_t*)nArgument, IOcmd, sizeof (arg));  \
      } else {                                                          \
          ret = func(filp->private_data, 0);                            \
      }                                                                 \
      break

/****************************************************************************
Description:
   dispatches ioctrl commands and manages user space data handling in
   user space direction with exchange structure
Arguments:
   msg   - ioctrl command id
   func  - function name
****************************************************************************/
#define ON_EX_IOCTL(msg, func)                  \
   case msg:                                    \
      DoDataExchange (&trans, nArgument, 1);    \
      ret = func(filp->private_data, &trans); \
      DoDataExchange (&trans, nArgument, 0);    \
      break


/* ============================= */
/* Local function declaration    */
/* ============================= */
static int  DUSLIC_Open       (struct inode *inode, struct file *filp);
static int  DUSLIC_Release    (struct inode *inode, struct file *filp);
static ssize_t DUSLIC_Read    (struct file * filp, char *buf, size_t length, loff_t * ppos);
static ssize_t DUSLIC_Write   (struct file *filp, const char *buf, size_t count, loff_t * ppos);
static int  DUSLIC_Ioctl      (struct inode *inode, struct file *filp, unsigned int nCmd, unsigned long nArgument);
//static unsigned int DUSLIC_Poll  (struct file *filp, poll_table *table);      
      
static int  DUSLIC_Read_Proc   (char *buf, char **start, off_t offset, int len, int *eof, void *data);
#ifndef USE_VINETIC_GPIO
static void DUSLIC_irq_handler (int irq, void *pDev, struct pt_regs *regs);
#endif


/* Driver callbacks */
struct file_operations DUSLIC_fops =
{
   owner:   THIS_MODULE,
   open:    DUSLIC_Open,
   release: DUSLIC_Release,
   read:    DUSLIC_Read,
   write:   DUSLIC_Write,
   ioctl:   DUSLIC_Ioctl,
  // poll:    DUSLIC_Poll,
};      


/* ============================= */
/* Global variable definition    */
/* ============================= */
DUSLIC_DEVICE *pDuslic_Devices[DUSLIC_MAX_DEVICES]={0};
SEM_ID SemDrvIF=0;

/* ============================= */
/* Local variable definition     */
/* ============================= */
#ifdef CONFIG_DEVFS_FS
/** handles for Dev FS */
static devfs_handle_t devfs_handle [DEVFS_HANDLE_NUM];
#endif /* CONFIG_DEVFS_FS */

/* IFX_LOCAL memory for Device pointer */
IFX_LOCAL DUSLIC_DEVICE Duslic_Devices[DUSLIC_MAX_DEVICES];

/* locking variable for disable/enable irq */
static char nDuslicDisableIRCount = 0;

static BYTE IOcmd[MAX_TRANSSPACE];


/* ============================= */
/* Local function declaration    */
/* ============================= */
static void ReleaseDevice                   (DUSLIC_DEVICE *pDev);
void        OS_Install_DuslicIRQHandler     (void* pDevice, int nIrq);
static void OS_UnInstall_DUSLICIRQHandler   (void* pDevice);


/****************************************************************************
Description:
   IRQ handler

Arguments:
   irq    - number of the irq
   pDev   - pointer to the device structure
   regs   -
Return:
   none
****************************************************************************/
#ifndef USE_VINETIC_GPIO
static void DUSLIC_irq_handler(int irq, void *pDev, struct pt_regs *regs)
{
   DUSLIC_interrupt_routine((DUSLIC_DEVICE*)pDev);

   /* acknowledge interrupt */
   Board_AckIrq (((DUSLIC_DEVICE*)pDev)->nIrq);
}
#endif

/*****************************************************************************
 * for VINETIC GPIO 
 ****************************************************************************/
#ifdef USE_VINETIC_GPIO
void DUSLIC_Callback(
    int nDev,               /**< device number */
    int nCh,                /**< channel number */
    WORD nEvt               /**< pin event number */
)
{
   DUSLIC_interrupt_routine(g_pDev);
   
   Board_AckIrq (g_pDev->nIrq);

}
#endif


/*******************************************************************************
Description:
	Install the Interrupt Handler
Arguments:
	pDev	- handle of the device structure
   nIrq  - interrupt number
Return:
   none
*******************************************************************************/
void OS_Install_DuslicIRQHandler (void* pDevice, int nIrq)
{
    DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*)pDevice;
    IFX_uint8_t INTREG[5];
    int i;

    /*clear the interrupt before we enable it */ 
    for (i = 0; i < DUSLIC_MAX_CHANNELS; i++)
    {
        pDev->pChannel[i].ReadReg(&pDev->pChannel[i], SOP_CMD, INTREG1, &INTREG[1], 4);
        //pDev->pChannel[i].nChState = PDRx;
        //pDev->pChannel[i].ciop(&pDev->pChannel[i], PDRx);
    }

#if USE_VINETIC_GPIO
    {
        int ctx;
        VINETIC_GPIO_CONFIG ioCfg;
        g_pDev=pDevice;
        memset (&ioCfg, 0, sizeof(VINETIC_GPIO_CONFIG));
        ctx=VINETIC_OpenKernel(0,0);
        vinetic_gpio_hd=VINETIC_GpioReserve(ctx, DUSLIC_INT_VIN_GPIO);
        ioCfg.nMode=_GPIO_MODE_INPUT|_GPIO_MODE_INT|_GPIO_INT_FALLING;
        ioCfg.nGpio=DUSLIC_INT_VIN_GPIO;
        ioCfg.callback=&DUSLIC_Callback;
        VINETIC_GpioConfig(vinetic_gpio_hd, &ioCfg);
    }   
#else
    if (request_irq (pDev->nIrq, DUSLIC_irq_handler,
                     SA_INTERRUPT, DEV_NAME, (void *)pDev) != 0)
    {
        TRACE (DUSLIC, DBG_LEVEL_HIGH, ("ERROR: request interrupt failed\n\r"));
    }
#endif

    /* increase disable irq count as irqs were disabled */
    nDuslicDisableIRCount++;
    pDev->nDevState &= ~DS_INT_ENABLED;
}


/*******************************************************************************
Description:
	Uninstall the Interrupt Handler
Arguments:
   pDev  - pointer to device structure
Return:
   none
*******************************************************************************/
static void OS_UnInstall_DUSLICIRQHandler(void* pDevice)
{
   DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*) pDevice;

   pDev->nDevState &= ~DS_INT_ENABLED;

#if USE_VINETIC_GPIO
   VINETIC_GpioRelease(vinetic_gpio_hd); 
#else   
   free_irq(pDev->nIrq, pDev);
#endif

}


/****************************************************************************
Description:
   Get device pointer
Arguments:
   nr - number of device
   pDev - returned pointer to device structure
Return:
   SUCCESS or ERR in case of an error
Remarks:
   Used by board / main module to reset device states
****************************************************************************/
INT OS_GetDevice (INT nr, DUSLIC_DEVICE** pDev)
{
   if (nr > DUSLIC_MAX_DEVICES || pDuslic_Devices[nr] == NULL)
      return ERR;

   *pDev = pDuslic_Devices[nr];
   
   return SUCCESS;
}


/****************************************************************************
Description:
   Init device structure
Arguments:
   nDevn - number of device to init
Return Value:
   SUCCESS on success, ERR if init fails
Remarks:
   Called form Board_DevOpen().
****************************************************************************/
INT OS_InitDevice (INT nDevn)
{
   /* get device pointer */
   DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*) &Duslic_Devices[nDevn];
   
   /* Initialization */
   memset(pDev, 0, sizeof(DUSLIC_DEVICE));

   pDev->magic = DUSLIC_DEVICE_MAGIC;
   pDev->nDevNr = nDevn;

   if (InitDevMember (pDev) == IFX_ERR)
   {
       TRACE (DUSLIC, DBG_LEVEL_HIGH,
             ("ERROR: Initialization of device structure failed\n\r"));
       return IFX_ERR;
   }

   /* cache the pointer */
   pDuslic_Devices[nDevn] = pDev;

   return IFX_SUCCESS;
}


/** \defgroup VinKernel VINETIC Kernel API
\remarks
  Kernel interface for open/close from Linux kernel mode
*/

/* @{ */

/**
   Open the device from kernel mode.
\param
   nDev - index of the DUSLIC device
\param
   nCh - index of the DUSLIC channel (1 = channel 0, ...)
\return
   handle to device/channel or ERR on error
\remarks
  If not already done this will
   - allocate internal memory for each new device
   - allocate io memory
   - initialize the device
   - set up the interrupt
*/
INT DUSLIC_OpenKernel(INT nDev, INT nCh)
{
    DUSLIC_DEVICE *pDev = NULL;
    DUSLIC_CHANNEL *pCh = NULL;
    INT ret = ERR;

    /* check the device number for the type 0 */
    if ((nDev >= DUSLIC_MAX_DEVICES) || (nDev < 0))
    {
        TRACE (DUSLIC, DBG_LEVEL_NORMAL,
  	          ("Warning: Driver only supports %d DUSLIC(s)\n\r",
              DUSLIC_MAX_DEVICES));
        
        ret = -ENODEV;
        goto OPEN_ERROR;
    }
   
    /* check if first open on this device */
    if (pDuslic_Devices[nDev] == NULL)
    {
        /* Now allocate and initialize all data. This function will call
            OS_InitDevice at first  */
        if (Board_DevOpen(nDev) == ERR)
        {
            ret = -ENODEV;
            goto OPEN_ERROR;
        }
    }
   
    //Sem_Lock(SemDrvIF);
    pDev = pDuslic_Devices[nDev];
    /* increase in use count */
    pDev->nInUse++;

    //printk("pDev->pChannel[0].pParent = %X\n",pDev->pChannel[0].pParent);
    if (nCh == 0)
    {
        /* return the device pointer */
        ret = (INT)pDev;
    }
    else
    {
        /* return the channel pointer */
        pCh = &pDev->pChannel [nCh - 1];
        pCh->nChannel = nCh-1;
        
        if (pCh->nInUse == 0)
        {
            ResetChMember (pCh);
        }

        pCh->nInUse++;
        ret = (INT)pCh;
    }

    /* increment module use counter */
    MOD_INC_USE_COUNT;

    TRACE (VINETIC, DBG_LEVEL_LOW,
            ("Open Kernel VINETIC device Nr. %d, channel %d\n\r", nDev, nCh));

    //Sem_Unlock (SemDrvIF);
    return ret;

OPEN_ERROR:
    if (pDev != NULL)
    {
        ReleaseDevice(pDev);
        TRACE (DUSLIC,DBG_LEVEL_HIGH, ("Open Kernel Error 0x%X\n", pDev->err));
    }

    Board_ResetPlatform();
    return ret;
}


/**
   Release the device from Kernel mode.
\param
   nHandle - handle returned by DUSLIC_OpenKernel
\return
   SUCCESS or error code
\remarks
*/
INT DUSLIC_ReleaseKernel(INT nHandle)
{
    DUSLIC_DEVICE *pDev;
    DUSLIC_CHANNEL *pCh;

    //Sem_Lock(SemDrvIF);
   
    if (((DUSLIC_DEVICE*)nHandle)->magic == DUSLIC_DEVICE_MAGIC)
    {
        pDev = (DUSLIC_DEVICE*)nHandle;
	    TRACE (VINETIC, DBG_LEVEL_LOW,
	            ("Closing Kernel device %d\n\r", pDev->nDevNr));
	    /* memory will be released when module is removed */
	    pDev->nInUse--;
    }
    else
    {
        pCh = (DUSLIC_CHANNEL*)nHandle;
        pCh->nInUse--;
        ((DUSLIC_DEVICE*)(pCh->pParent))->nInUse--;
      
        TRACE (DUSLIC, DBG_LEVEL_LOW, ("Closing device channel %d (%d)\n\r", pCh->nChannel, pCh->nInUse));
    }

    /* decrement use counter */
    MOD_DEC_USE_COUNT;
    
    //Sem_Unlock(SemDrvIF);
    
    return IFX_SUCCESS;
}
/* @} */


/****************************************************************************
Description:
   Open the device.
   - allocating internal memory for each new device
   - allocating io memory
   - initialize the device
   - set up the interrupt
Arguments:
   inode - pointer to the inode
   filp  - file pointer to be opened
Return Value:
   IFX_SUCCESS or error code
Remarks:
   None.
****************************************************************************/
static int DUSLIC_Open (struct inode *inode, struct file *filp)
{
    DUSLIC_DEVICE *pDev  = NULL;
    DUSLIC_CHANNEL *pCh  = NULL;  
    IFX_int32_t dev_num = 0;
    IFX_int32_t ret = IFX_SUCCESS;
    IFX_int32_t num = MINOR(inode->i_rdev);
    IFX_char_t nCh  = 0;

    /* get device number */
    dev_num = (num / DUSLIC_MINOR_BASE) - 1;

    if ((dev_num >= DUSLIC_MAX_DEVICES) || (dev_num < 0))
    {
        printk("Warning: Driver only supports %d DUSLIC(s)\n\r", DUSLIC_MAX_DEVICES);
        ret = -ENODEV;
        goto OPEN_ERROR;
    }
  
    /* check if first open on this device */
    if (pDuslic_Devices[dev_num] == NULL)
    {
        printk("Duslic debug: open device!\n");

        /* Now allocate and initialize all data. */
        if (Board_DevOpen (dev_num) == IFX_ERR)
        {
            ret = -ENODEV;
            goto OPEN_ERROR;
        }
    }
   
    pDev = pDuslic_Devices[dev_num];
   //printk("pDev->pChannel[0].pParent = %X\n",pDev->pChannel[0].pParent);
    /* increase in use count */
    pDev->nInUse++;
    nCh = num % DUSLIC_MINOR_BASE;
     
    if (nCh == 0)
    {
        /* Save the device pointer */
        filp->private_data = pDev;
    }
    else if (nCh > 0 && nCh <= 2)
    {
        /* Save the channel pointer */
        pCh = filp->private_data = &pDev->pChannel [nCh - 1];
        pCh->nChannel = nCh-1;
        
        /* Check if the device is opened as a non-blocking one */
        if (!(filp->f_flags & O_NONBLOCK))
        {
            /* set the CF_NONBLOCK channel specific flag for reading without blocking */
         
            /* writing to the device is implemented as non-blocking */
            pCh->nFlags |= CF_NONBLOCK;
        }
      
        pCh->nFlags |= CF_NONBLOCK;
      
        if (pCh->nInUse == 0)
        {
            ResetChMember (pCh);
        }
        
        pCh->nInUse++;
    }
   
    MOD_INC_USE_COUNT;
   
    return IFX_SUCCESS;

OPEN_ERROR:
    if (pDev != NULL)
    {
        printk("open error!\n");
        ReleaseDevice(pDev);
        TRACE (DUSLIC,DBG_LEVEL_HIGH, ("Open Error 0x%lX\n", pDev->err));
    }

    return ret;
}


/**************************************************************************
**
Description:
   Clean up the device if it is no longer open by some user space program.
Arguments:
   pDev - pointer to the device structure
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
static void ReleaseDevice(DUSLIC_DEVICE *pDev)
{
    /*check if we really have to do with device structure */
    if (pDev == NULL || pDev->magic != DUSLIC_DEVICE_MAGIC)
        return;

    /* free the interrupt */
    if (pDev->nIrq)
        OS_UnInstall_DUSLICIRQHandler ((void*) pDev);
   
    /* clear the cached  pointer */
    pDuslic_Devices [pDev->nDevNr] = NULL;
   
    /* free the memory  */
    pDev = NULL;
}


/****************************************************************************
Description:
   Release the device.
Arguments:
   inode - pointer to the inode
   filp  - file pointer to be opened
Return Value:
   IFX_SUCCESS or error code
Remarks:
  None.
****************************************************************************/
static int DUSLIC_Release (struct inode *inode, struct file *filp)
{
    //DUSLIC_DEVICE *pDev = (DUSLIC_DEVICE *) filp->private_data;
    //DUSLIC_CHANNEL *pCh;
    int num = MINOR(inode->i_rdev);      /* the real device */
    int dev_num = dev_num = (num / DUSLIC_MINOR_BASE) - 1;
   
    /* check the device number */
    if (dev_num >= DUSLIC_MAX_DEVICES)
    {
        TRACE (DUSLIC, DBG_LEVEL_HIGH, ("ERROR: max. device number exceed!\n\r"));
        return -ENODEV;
    }
    ReleaseDevice(pDuslic_Devices[dev_num]);
   
    MOD_DEC_USE_COUNT;
    
    return IFX_SUCCESS;
}


/****************************************************************************
Description:
   Reads data from the DUSLIC.
Arguments:
   filp  - file pointer to be opened
   buf   - destination buffer
   count - max size of data to read
   ppos  - unused
Return Value:
   len   - data length
Remarks:
   None.
****************************************************************************/
static ssize_t DUSLIC_Read(struct file *filp, IFX_char_t *buf, size_t count, loff_t * ppos)
{
    IFX_int32_t ret = IFX_SUCCESS;
   
    return ret;
}


/****************************************************************************
Description:
   Writes data to the DUSLIC.
Arguments:
   filp  - file pointer to be opened
   buf   - source buffer
   count - data length
   ppos  - unused
Return Value:
   nLength - 0 if failure else the length
Remarks:
   None.
****************************************************************************/
static ssize_t DUSLIC_Write(struct file *filp, const char *buf, size_t count, loff_t * ppos)
{
   return 0;
}


/****************************************************************************
Description:
   Configuration / Control for the device.
Arguments:
   inode - pointer to the inode
   filp  - file pointer to be opened
   nCmd  - IoCtrl
   nArgument - additional argument
Return:
   IFX_SUCCESS or error code
Remarks:
   None.
****************************************************************************/
static int DUSLIC_Ioctl(struct inode *inode, struct file *filp,
                        unsigned int nCmd, unsigned long nArgument)
{
/*     DUSLIC_DEVICE  *pDev = (DUSLIC_DEVICE *)filp->private_data; */
/*     DUSLIC_CHANNEL *pCh  = (DUSLIC_CHANNEL *)filp->private_data; */
    IFX_int32_t ret;

    switch (nCmd)
    {
        /* Main Module */
        ON_IOCTL       (FIO_DUSLIC_INIT, DUSLIC_Init, DUSLIC_IO_INIT);
   
        /* basic functions module */
#if 0
        ON_IOCTL_FRUSR (FIO_DUSLIC_WCMD, DUSLIC_Write_Cmd, DUSLIC_IO_MB_CMD);
        ON_IOCTL       (FIO_DUSLIC_RCMD, DUSLIC_Read_Cmd, DUSLIC_IO_MB_CMD);
        ON_IOCTL_FRUSR (FIO_VINETIC_WSC, VINETIC_Write_Sc, VINETIC_IO_WRITE_SC);
        ON_IOCTL       (FIO_VINETIC_RSC, VINETIC_Read_Sc, VINETIC_IO_READ_SC);
#endif
        
        ON_IOCTL       (FIO_DUSLIC_READ_REG, DUSLIC_Read_Reg, DUSLIC_IO_REG_CMD);
        ON_IOCTL_FRUSR (FIO_DUSLIC_WRITE_REG, DUSLIC_Write_Reg, DUSLIC_IO_REG_CMD);
        ON_IOCTL_FRUSR (FIO_DUSLIC_CIOP_CMD, DUSLIC_Ciop, DUSLIC_IO_REG_CMD);
        
        /* PCM configuration */
        ON_IOCTL_FRUSR (FIO_DUSLIC_PCM_CONFIG, DUSLIC_Set_Pcm, DUSLIC_PCM_CONFIG);

    case FIO_DUSLIC_PCM_ACTIVE:
	    DUSLIC_Pcm_Active((DUSLIC_CHANNEL *)filp->private_data, nArgument);
        break;

        /* Misc Module */
        ON_IOCTL       (FIO_DUSLIC_VERS, DUSLIC_Version, DUSLIC_IO_VERSION);
   }
   
   return 0;
}


/****************************************************************************
Description:
   The proc filesystem: function to read an entry.
Arguments:
   page     - pointer to page buffer
   start    - pointer to start address pointer
   off      - offset
   count    - length to read
   eof      - end of file
   data     - data (used as function pointer)
Return Value:
   length
Remarks:
   None.
****************************************************************************/
int DUSLIC_Read_Proc(char *buf, char **start, off_t offset,
                     int count, int *eof, void *data)
{
    int len=0;
    unsigned short value=0;

    len+=sprintf(buf+len,"duslic driver\n");

#ifdef USE_VINETIC_GPIO
    if (vinetic_gpio_hd)
    {
	VINETIC_GpioGet(vinetic_gpio_hd,&value,DUSLIC_INT_VIN_GPIO);
	len+=sprintf(buf+len,"VINETIC_GPIO value=%04x\n",value);
      }
#endif    
    
    len+=sprintf(buf+len,"device=%08x\n",(INT)&pDuslic_Devices[0]);
    len+=sprintf(buf+len,"channel1=%08x\n",(INT)&(pDuslic_Devices[0]->pChannel[0]));
    len+=sprintf(buf+len,"&nChannel=%08x\n",(INT)&(pDuslic_Devices[0]->pChannel[0].nChannel));     
    len+=sprintf(buf+len,"nChannel=%02lx\n",(pDuslic_Devices[0]->pChannel[0]).nChannel);    
/*  len+=sprintf(buf+len,"GpioRes=%08x\n",(INT)&(Duslic_Devices[0].pChannel[0].GpioRes));
    len+=sprintf(buf+len,"&nGlobalReserve=%08x\n",(INT)&(Duslic_Devices[0].pChannel[0].GpioRes.nGlobalReserve));
    len+=sprintf(buf+len,"nGlobalReserve=%08x\n",Duslic_Devices[0].pChannel[0].GpioRes.nGlobalReserve);
*/
    len+=sprintf(buf+len,"channel2=%08x\n",(INT)&(pDuslic_Devices[0]->pChannel[1]));    
    
    if (pDuslic_Devices[0]) {
        int j;
        IFX_uint8_t  temp[256]={0};
        for (j = 0; j < 0x2f; j++)
        {
            ReadReg(&pDuslic_Devices[0]->pChannel[0], SOP_CMD, j, temp, 1);
            printk("%02x ", temp[0]);
        }
        printk("\n");
    }
    return len;
}

/****************************************************************************
Description:
   Initializes the hardware
Arguments:
   none
Return Value:
  SUCCESS or ERR
Remarks:
   initialzes the CPLD address space and calls functions from board module
   to initialize the basic registers.
****************************************************************************/
#if 0 /* TODO: define, whether this has to be done in a board specific driver */
int static init_hardware(void)
{
    Board_InitPlatform();

    return IFX_SUCCESS;
}
#endif

/****************************************************************************
Description:
   Initialize the module.
   Allocate the major number and register the dev nodes (for DEVFS).
Arguments:
   None.
Return Value:
   None
Remarks:
   Called by the kernel.
   Initializes global mutex, sets trace level and registers the module.
   It calls also functions to initialize the hardware which is used.
****************************************************************************/
static int __init duslic_module_init(void)
{
    IFX_int32_t result;

#ifdef CONFIG_DEVFS_FS
    IFX_int32_t i, j;
    IFX_char_t buf[10];
#endif /* CONFIG_DEVFS_FS */

#ifdef CONFIG_DEVFS_FS   
    memset (&(devfs_handle), 0x00, sizeof(devfs_handle));

    for (i = 0; i < DUSLIC_MAX_DEVICES; ++i)
    {
        for (j = 0; j <= DUSLIC_MAX_CHANNELS; j++ )
        {
            sprintf (buf, "dus%ld%ld", i+1, j);
            if ((devfs_handle[i*DUSLIC_MAX_CHANNELS + j]
                                  = devfs_register( NULL,
                                                    buf,
                                                    DEVFS_FL_DEFAULT,
                                                    DUSLIC_MAJOR,
                                                    (i+1)*10+j,
                                                    S_IFCHR | S_IRUGO | S_IWUGO,
                                                    &DUSLIC_fops,
                                                    (void *) 0)) == NULL)
            {
	            printk("add device error!\n"); 
                result = -ENODEV;
            }
        }
    }
#else
   /* Register the caracter device and allocate the major number */
   result = register_chrdev(DUSLIC_MAJOR, DEV_NAME , &DUSLIC_fops);
   if (result < 0)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("Loading %s module ERROR: major %d not found!",
             DEV_NAME, DUSLIC_MAJOR));
      return result;
   }

   printk("Duslic Drv: Register ok!\n");

#endif /* CONFIG_DEVFS_FS */
   
    create_proc_read_entry(DEV_NAME, 
                           S_IFREG|S_IRUGO,
			               NULL,
			               DUSLIC_Read_Proc,
			               &proc_root );
   
    return IFX_SUCCESS;
}


/****************************************************************************
Description:
   Clean up the module if unloaded.
Arguments:
   None.
Return Value:
   None.
Remarks:
   Called by the kernel.
****************************************************************************/
static void __exit duslic_module_exit(void)
{
    IFX_int32_t i;

#ifdef CONFIG_DEVFS_FS
    IFX_int32_t j;
#endif /* CONFIG_DEVFS_FS */

    for (i = 0; i < DUSLIC_MAX_DEVICES; i++)
    {
#ifdef CONFIG_DEVFS_FS
        /* remove duslic devices from dev fs */
        for (j = 0; j <= DUSLIC_MAX_CHANNELS; j++ )
        {
            if (devfs_handle[i*DUSLIC_MAX_CHANNELS + j])
            {
	            //printk("unregister dus%d%d\n",i+1,j);
                devfs_unregister (devfs_handle[i*DUSLIC_MAX_CHANNELS + j]);
            }
        }
#else 
    unregister_chrdev (DUSLIC_MAJOR, DEV_NAME);
#endif /* CONFIG_DEVFS_FS */
    }
   
    for (i=0; i < DUSLIC_MAX_DEVICES; i++)
    {
        if (pDuslic_Devices[i] != NULL)
        {
            ReleaseDevice (pDuslic_Devices[i]);
        }
    }

    remove_proc_entry(DEV_NAME,NULL);
}


module_init(duslic_module_init);
module_exit(duslic_module_exit);
MODULE_LICENSE ("GPL");



