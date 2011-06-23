/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
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
   Module      : drv_vinetic_linux.c
   Description : This file contains the implementation of the linux specific
                 driver functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_api.h"
#ifdef LINUX
#include "linux/init.h"
#include "drv_vinetic_api.h"
#include "drv_vinetic_main.h"
#include "drv_vinetic_basic.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_int.h"
#include "drv_vinetic_misc.h"
#include "drv_vinetic_stream.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_gpio_user.h"
#ifdef TAPI
#include "drv_vinetic_lt.h"
#include "drv_vinetic_gr909.h"
#endif /* TAPI */

#ifdef CONFIG_DEVFS_FS
#include "linux/devfs_fs_kernel.h"
#endif /* CONFIG_DEVFS_FS */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
/* maximum space for data exchange in IOCTL functions */
#define MAX_TRANSSPACE   9216 /* 9 K */

MODULE_DESCRIPTION("VINETIC driver - www.infineon.com");
MODULE_AUTHOR("Infineon Technologies, COM AC SD SW");

#ifdef TESTING
IFX_void_t PrintErrorInfo(VINETIC_DEVICE *pDev);
#endif /* TESTING */

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
   As an alternative use a transfer structure VINETIC_IO_USR and
   DoDataExchange for handling data exchange.
Example:
   call function VINETIC_Read_Cmd if id VINETIC_RMAIL received
   IFX_uint8_t IOcmd [660];
   ...
   switch (nCmd)
   {
      ON_IOCTL(VINETIC_RMAIL, VINETIC_Read_Cmd, IO_READ_CMD);
   }
****************************************************************************/
#define ON_IOCTL(msg,func,arg)                                  \
      case msg:                                                 \
      copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (arg));   \
      ret = func(filp->private_data, (arg*) IOcmd);             \
      copy_to_user ((IFX_uint8_t*)nArgument, IOcmd, sizeof (arg));     \
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
#define ON_IOCTL_FRUSR(msg,func,arg)                            \
      case msg:                                                 \
      copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (arg));   \
      ret = func(filp->private_data, (arg*) IOcmd);             \
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
#define ON_IOCTL_TOUSR(msg,func,arg)                          \
      case msg:                                               \
      ret = func(filp->private_data, (arg*) IOcmd);           \
      copy_to_user ((IFX_uint8_t*)nArgument, IOcmd, sizeof (arg));   \
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

/****************************************************************************
Description:
   macro to map the system function with takes care of interrupt handling
   registration.
Arguments:
   irq   -  irq number
   func  -  interrupt handler callback function
   arg   -  argument of interrupt handler callback function
Remarks:
   The macro is by default mapped to the operating system method. For systems
   integrating different routines, this macro must be adapted in the user con-
   figuration header file.
****************************************************************************/
#ifndef VIN_SYS_REGISTER_INT_HANDLER
#define VIN_SYS_REGISTER_INT_HANDLER(irq,flags,func,arg)             \
         request_irq ((irq),(func),(flags),DEV_NAME,(void*)(arg))
#endif /* VIN_SYS_REGISTER_INT_HANDLER */

/****************************************************************************
Description:
   macro to map the system function with takes care of interrupt handling
   unregistration.
Arguments:
   irq   -  irq number
Remarks:
   The macro is by default mapped to the operating system method. For systems
   integrating different routines, this macro must be adapted in the user con-
   figuration header file.
****************************************************************************/
#ifndef VIN_SYS_UNREGISTER_INT_HANDLER
#define VIN_SYS_UNREGISTER_INT_HANDLER(irq,arg)  \
         free_irq((irq), (arg))
#endif /* VIN_SYS_UNREGISTER_INT_HANDLER */

/* ============================= */
/* Global variable definition    */
/* ============================= */

VINETIC_DEVICE *sVinetic_Devices[VINETIC_MAX_DEVICES] = {0};
IFXOS_mutex_t SemDrvIF=0;

/* ============================= */
/* Global function declaration   */
/* ============================= */


/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_int32_t OS_Release_Baseadress(VINETIC_DEVICE* pDev);
static int  VINETIC_Open       (struct inode *inode, struct file *filp);
static int  VINETIC_Release    (struct inode *inode, struct file *filp);
static ssize_t VINETIC_Read    (struct file * filp, char *buf, size_t length,
                                 loff_t * ppos);
static ssize_t VINETIC_Write   (struct file *filp, const char *buf,
                                 size_t count, loff_t * ppos);
static int  VINETIC_Ioctl      (struct inode *inode, struct file *filp,
                                 unsigned int nCmd, unsigned long nArgument);
static unsigned int VINETIC_Poll  (struct file *filp, poll_table *table);
static void VINETIC_irq_handler   (int irq, void *pDev, struct pt_regs *regs);
IFX_LOCAL IFX_void_t ReleaseDevice   (VINETIC_DEVICE *pDev);

#ifdef VINETIC_USE_PROC
static int VINETIC_Read_Proc   (char *buf, char **start, off_t offset,
                                 int len, int *eof, void *data);
#endif /* VINETIC_USE_PROC */

/* Driver callbacks */
struct file_operations Vinetic_fops =
{
   owner:   THIS_MODULE,
   open:    VINETIC_Open,
   release: VINETIC_Release,
   read:    VINETIC_Read,
   write:   VINETIC_Write,
   ioctl:   VINETIC_Ioctl,
   poll:    VINETIC_Poll,
};

struct file_operations *sVinetic_fop_Array[] =
{
   /* type 0 */
   &Vinetic_fops,
};

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* IFX_LOCAL memory for Device pointer */
IFX_LOCAL VINETIC_DEVICE VDevices[VINETIC_MAX_DEVICES];
/* list of vinetic interrupts, neccessary for shared interrupt handling */
IFX_LOCAL vinetic_interrupt_t* VIN_irq_head = IFX_NULL;

/* transfer buffer for IOCTLs, see DoDataExchange and macros
   ON_IOCTL, ON_IOCTL_TOUSR and ON_IOCTL_FRUSR */
IFX_LOCAL IFX_uint8_t IOcmd[MAX_TRANSSPACE];

#ifdef CONFIG_DEVFS_FS
/** handles for Dev FS */
static devfs_handle_t devfs_handle [VINETIC_MAX_DEVICES*VINETIC_MINOR_BASE];
#endif /* CONFIG_DEVFS_FS */

/* ============================= */
/* Local function definition     */
/* ============================= */

/*******************************************************************************
Description:
   This function initializes the VINETIC base address.
   If necessary, OS specific mapping is done here.
Arguments:
   pDev      - handle of the VINETIC device structure.
   nBaseAddr - physical address for vinetic access.
Return Value:
   IFX_SUCCESS / IFX_ERROR.
Remarks:
   none
*******************************************************************************/
IFX_int32_t OS_Init_Baseadress(VINETIC_DEVICE* pDev, IFX_uint32_t nBaseAddr)
{
   IFX_uint16_t *ptr;
   int  result;

   if (pDev->pBaseAddr != IFX_NULL)
      OS_Release_Baseadress(pDev);

   /* check the io region */
   result = check_mem_region(nBaseAddr, VINETIC_REGION_SIZE);
   if (result != 0)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: can't get I/O address 0x%X\n\r", (unsigned int)nBaseAddr));
      return IFX_ERROR;
   }
   /* request the io region (can not fail) */
   request_mem_region (nBaseAddr, VINETIC_REGION_SIZE, DEV_NAME);
   /* calculate the virtual address from the physical address */
   if ((ptr = ioremap_nocache(nBaseAddr, VINETIC_REGION_SIZE)) == NULL)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
         ("ERROR: vinetic address region could not be mapped!\n\r"));
      return IFX_ERROR;
   }
   /* remember physical address */
   pDev->nBaseAddr = nBaseAddr;
   /* and remember virtual address */
   pDev->pBaseAddr = ptr;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   This function initializes the VINETIC base address.
   If necessary, OS specific mapping is done here.
Arguments:
   pDev      - handle of the VINETIC device structure.
   nBaseAddr - physical address for vinetic access.
Return Value:
   IFX_SUCCESS / IFX_ERROR.
Remarks:
   none
*******************************************************************************/
IFX_int32_t OS_Release_Baseadress(VINETIC_DEVICE* pDev)
{
   /* unmap the io region */
   if (pDev->pBaseAddr)
      iounmap((IFX_void_t*)pDev->pBaseAddr);
   /* release the io region */
   if(pDev->nBaseAddr)
      release_mem_region(pDev->nBaseAddr, VINETIC_REGION_SIZE);

   /* remember physical address */
   pDev->nBaseAddr = 0;
   /* and remember virtual address */
   pDev->pBaseAddr = IFX_NULL;

   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   This function allocates a temporary kernel buffer
      and copies the user buffer contents to it.
Arguments:
   p_buffer - pointer to the user buffer.
   size     - size of the buffer.
Return Value:
   pointer to kernel buffer or NULL
Remarks:
   Buffer has to be released with \ref OS_UnmapBuffer
*******************************************************************************/
IFX_void_t* OS_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size)
{
   IFX_void_t* kernel_buffer;

   if (p_buffer == NULL)
   {
      return NULL;
   }
   kernel_buffer = vmalloc(size);
   if (kernel_buffer == NULL)
   {
      return NULL;
   }
   if (copy_from_user(kernel_buffer, p_buffer, size) > 0)
   {
      vfree(kernel_buffer);
      return NULL;
   }

   return kernel_buffer;
}

/*******************************************************************************
Description:
   This function releases a temporary kernel buffer.
Arguments:
   p_buffer - pointer to the kernel buffer.
Return Value:
   None.
Remarks:
  Counterpart of \ref OS_MapBuffer
*******************************************************************************/
IFX_void_t OS_UnmapBuffer(IFX_void_t* p_buffer)
{
   if (p_buffer != NULL)
   {
      vfree(p_buffer);
      p_buffer = NULL;
   }
}

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
IFX_LOCAL void VINETIC_irq_handler(int irq, void *pDev, struct pt_regs *regs)
{
   VINETIC_interrupt_routine((VINETIC_DEVICE*)pDev);
}

/*******************************************************************************
Description:
   Install the Interrupt Handler
Arguments:
   pDev  - pointer to device structure
   nIrq  - interrupt number
Return:
   none
*******************************************************************************/
IFX_void_t OS_Install_VineticIRQHandler (VINETIC_DEVICE* pDev, IFX_int32_t nIrq)
{
   vinetic_interrupt_t* pIrq;
   vinetic_interrupt_t* pIrqPrev = IFX_NULL;
   VINETIC_DEVICE* tmp_pDev;
   unsigned long intstatus;

   if (pDev->pIrq != IFX_NULL)
      OS_UnInstall_VineticIRQHandler(pDev);

   pIrq = VIN_irq_head;

   /* try to find existing Irq with same number */
   while (pIrq != IFX_NULL)
   {
      pIrqPrev = pIrq;
      if (pIrq->nIrq == nIrq)
         break;
      pIrq = pIrq->next_irq;
   }

   if (pIrq == IFX_NULL)
   {
      pIrq = kmalloc(sizeof(vinetic_interrupt_t),GFP_KERNEL);
      if (pIrq == IFX_NULL)
      {
         /** \todo set error status! */
         return;
      }
      memset(pIrq, 0, sizeof(vinetic_interrupt_t));
      IFXOS_MutexInit(pIrq->intAcc);
      save_and_cli(intstatus);
      /* register new dedicated or first of shared interrupt */
      pDev->pIrq = pIrq;
      pDev->pInt_NextDev = IFX_NULL;
      pIrq->pdev_head = pDev;
      pIrq->next_irq = IFX_NULL;
      pIrq->nIrq = nIrq;
      /* Install the Interrupt routine */
      if (VIN_SYS_REGISTER_INT_HANDLER (nIrq, (SA_INTERRUPT | SA_SHIRQ),
          VINETIC_irq_handler, pDev) != 0)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: request interrupt failed\n\r"));
      }
      else
      {
         pIrq->bRegistered = IFX_TRUE;
         VIN_DISABLE_IRQLINE(nIrq);
      }

      /* add to list of interrupts */
      /* this list is not used by the interrupt handler itself,
         so no additional intLock necessary! */
      if (VIN_irq_head == IFX_NULL)
         VIN_irq_head = pIrq;
      else
         pIrqPrev->next_irq = pIrq;
      restore_flags(intstatus);
   }
   else
   {
      save_and_cli(intstatus);
      /* Install the Interrupt routine (add to shared irq list) */
      if (VIN_SYS_REGISTER_INT_HANDLER (nIrq, (SA_INTERRUPT | SA_SHIRQ),
          VINETIC_irq_handler, pDev) != 0)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: request shared interrupt failed\n\r"));
      }
      else
      {
         /* add into the list of an existing shared interrupt */
         pDev->pIrq = pIrq;
         pDev->pInt_NextDev = IFX_NULL;
         tmp_pDev = pIrq->pdev_head;
         while (tmp_pDev->pInt_NextDev != IFX_NULL)
         {
            tmp_pDev = tmp_pDev->pInt_NextDev;
         }
         tmp_pDev->pInt_NextDev = pDev;
      }
      restore_flags(intstatus);
   }
}

/*******************************************************************************
Description:
   Uninstall the Interrupt Handler
Arguments:
   pDev  - pointer to device structure
Return:
   none
*******************************************************************************/
IFX_void_t OS_UnInstall_VineticIRQHandler(VINETIC_DEVICE* pDev)
{
   vinetic_interrupt_t* pIrq = pDev->pIrq;
   vinetic_interrupt_t* pIrqPrev;
   VINETIC_DEVICE* tmp_pDev = pIrq->pdev_head;

   /* remove this pDev from the list of this pIrq */
   if (tmp_pDev == pDev)
   {
      pIrq->pdev_head = pDev->pInt_NextDev;
   }
   else
   {
      /* find the element which has this pDev as next */
      while (tmp_pDev->pInt_NextDev != pDev)
      {
         if (tmp_pDev->pInt_NextDev == IFX_NULL)
         {
            TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("ERROR: device not found in interrupt list, "
                "cannot uninstall!\n\r"));
            return;
         }
         tmp_pDev = tmp_pDev->pInt_NextDev;
      }
      tmp_pDev->pInt_NextDev = pDev->pInt_NextDev;
   }
   pDev->pInt_NextDev = IFX_NULL;
   pDev->pIrq = IFX_NULL;

   /* if this was the last device, so cleanup the irq */
   if (pIrq->pdev_head == IFX_NULL)
   {
      /*IFXOS_MutexLock(pIrq->intAcc);*/

      if (pIrq->bIntEnabled == IFX_TRUE)
      {
         VIN_DISABLE_IRQLINE (pIrq->nIrq);
      }
      if (pIrq->bRegistered == IFX_TRUE)
      {
         VIN_SYS_UNREGISTER_INT_HANDLER(pIrq->nIrq, pDev);
      }
      /* remove this pIrq from irq-list */
      if (VIN_irq_head == pIrq)
      {
         VIN_irq_head = pIrq->next_irq;
      }
      else
      {
         pIrqPrev = VIN_irq_head;
         while (pIrqPrev->next_irq != IFX_NULL)
         {
            if (pIrqPrev->next_irq == pIrq)
               break;
            pIrqPrev = pIrqPrev->next_irq;
         }
         if (pIrqPrev->next_irq != IFX_NULL)
            pIrqPrev->next_irq = pIrq->next_irq;
      }
      IFXOS_MutexDelete(pIrq->intAcc);
      kfree(pIrq);
   }
   else
      VIN_SYS_UNREGISTER_INT_HANDLER(pIrq->nIrq, pDev);
}

/*******************************************************************************
Description:
   Install the Poll Queues
Arguments:
   fileptr    - ptr to file structure
   poll_entry - ptr to the poll table
Return:
   none
Remark:
   Call this function in the poll system call
*******************************************************************************/
IFX_LOCAL int OS_InstallPollQueue (void *fileptr, void *poll_entry)
{
   struct file *file = (struct file *)fileptr;
   poll_table  *wait  = (poll_table *)poll_entry;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)file->private_data;
   VINETIC_CHANNEL *pCh = NULL;

   /* check device ptr */
   if (pDev == NULL)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: Install poll queue failed\n\r"));
      return -ENODEV;
   }
   if (pDev->nChannel == 0)
   {
#if (defined(TAPI) && (VIN_CFG_FEATURES & VIN_FEAT_GR909))
      IFX_uint8_t ch;
#endif
#ifdef TAPI
      /* exception on channel 0 */
      poll_wait(file, &(pDev->EventList), wait);
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
      /* for gr909 measurement, the poll routine is used as background task
         to execute the gr909 callback. it is slept on the gr909 event list
         until a new event occurs */
      for (ch = 0; ch < VINETIC_ANA_CH_NR; ch ++)
         poll_wait(file, &(pDev->pChannel[ch].Gr909.wqState), wait);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
#endif /* TAPI */
   }
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   else if (pDev->nChannel > 0 && pDev->nChannel <= 8)
   {
      /* set channel ptr */
      pCh = file->private_data;
      /* for voice channels a wait queue is installed to sleep
       in select/poll syscall until read data is available */
      poll_wait(file, &(pCh->wqRead), wait);
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

   return IFX_SUCCESS;
}

/****************************************************************************
Description:
   Get device pointer
Arguments:
   nr - number of device
   pDev - returned pointer to device structure
Return:
   IFX_SUCCESS or IFX_ERROR in case of an error
Remarks:
   Used by board / main module to reset device states
****************************************************************************/
IFX_int32_t OS_GetDevice (IFX_int32_t nr, VINETIC_DEVICE** pDev)
{
   if (nr > VINETIC_MAX_DEVICES || sVinetic_Devices[nr] == NULL)
      return IFX_ERROR;
   *pDev = sVinetic_Devices[nr];
   return IFX_SUCCESS;
}

/****************************************************************************
Description:
   Init device structure
Arguments:
   nDevn - number of device to init
Return Value:
   IFX_SUCCESS on success, IFX_ERROR if init fails
Remarks:
   Called form Board_DevOpen().
****************************************************************************/
IFX_int32_t OS_InitDevice (IFX_int32_t nDevn)
{
   /* get device pointer */
   VINETIC_DEVICE* pDev = (VINETIC_DEVICE*) &VDevices[nDevn];

   /* Initialization */
   memset(pDev, 0, sizeof(VINETIC_DEVICE));
   pDev->nDevNr = nDevn;
   /* cache the pointer */
   sVinetic_Devices[nDevn] = pDev;

   /* Os and board independend initializations, resets all values */
   if (InitDevMember (pDev) == IFX_ERROR)
   {
       TRACE (VINETIC, DBG_LEVEL_HIGH,
             ("ERROR: Initialization of device structure failed\n\r"));
       return IFX_ERROR;
   }
   return IFX_SUCCESS;
}

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
static int VINETIC_Open(struct inode *inode, struct file *filp)
{
   VINETIC_DEVICE *pDev = NULL;
   VINETIC_CHANNEL *pCh = NULL;
   IFX_int32_t dev_num = 0;
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_int32_t num = MINOR(inode->i_rdev);
   IFX_char_t  nCh = 0;

   /* get device number */
   dev_num = (num / VINETIC_MINOR_BASE) - 1;
   /* check the device number for the type 0 */
   if ((dev_num >= VINETIC_MAX_DEVICES) || (dev_num < 0))
   {
      TRACE (VINETIC, DBG_LEVEL_NORMAL,
            ("Warning: Driver only supports %d VINETIC(s)\n\r",
             VINETIC_MAX_DEVICES));
      ret = -ENODEV;
      goto OPEN_ERROR;
   }
   /* check if first open on this device */
   if (sVinetic_Devices[dev_num] == NULL)
   {
      /* Now allocate and initialize all data. */
      if (OS_InitDevice(dev_num) == IFX_ERROR)
      {
         ret = -ENODEV;
         goto OPEN_ERROR;
      }
   }
   IFXOS_MutexLock (SemDrvIF);
   pDev = sVinetic_Devices[dev_num];
   /* increase in use count */
   pDev->nInUse++;
   nCh = num % VINETIC_MINOR_BASE;
   if (nCh == 0)
   {
      /* Save the device pointer */
      filp->private_data = pDev;
   }
   else if (nCh > 0 && nCh <= 8)
   {
      /* Save the channel pointer */
      pCh = filp->private_data = &pDev->pChannel [nCh - 1];
      pCh->nChannel = nCh;
      /* Check if the device is opened as a non-blocking one */
      if (! (filp->f_flags & O_NONBLOCK))
      {
         /* set the CF_NONBLOCK channel specific flag for reading without
            blocking */
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

   /* increment module use counter */
   MOD_INC_USE_COUNT;

   TRACE (VINETIC, DBG_LEVEL_LOW,
         ("Open VINETIC device Nr. %ld, Channel %d\n\r", num, nCh));

   IFXOS_MutexUnlock (SemDrvIF);
   return IFX_SUCCESS;

OPEN_ERROR:
   if (pDev != NULL)
   {
      ReleaseDevice(pDev);
      TRACE (VINETIC,DBG_LEVEL_HIGH, ("Open Error 0x%lX\n", pDev->err));
   }
   return ret;
}

/****************************************************************************
Description:
   Clean up the device if it is no longer open by some user space program.
Arguments:
   pDev - pointer to the device structure
Return Value:
   None.
Remarks:
   None.
****************************************************************************/
IFX_LOCAL IFX_void_t ReleaseDevice(VINETIC_DEVICE *pDev)
{
   /*check if we really have to do with device structure */
   if (pDev == NULL || pDev->nChannel != 0)
      return;

   /* free the interrupt */
   if (pDev->pIrq != IFX_NULL)
      OS_UnInstall_VineticIRQHandler (pDev);
   /*    ... and the memory mapping */
   if (pDev->pBaseAddr != IFX_NULL)
      OS_Release_Baseadress(pDev);

   /* memory cleanup */
   ExitDev(pDev);
   /* decrease in use count */
   if(pDev->nInUse)
      pDev->nInUse--;
   /* clear the cached  pointer */
   sVinetic_Devices [pDev->nDevNr] = NULL;
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
static int VINETIC_Release (struct inode *inode, struct file *filp)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *) filp->private_data;
   VINETIC_CHANNEL *pCh;
   IFX_int32_t num = MINOR(inode->i_rdev);  /* the real device */
   IFX_int32_t dev_num = (num / VINETIC_MINOR_BASE) - 1;

   /* check the device number */
   if (dev_num >= VINETIC_MAX_DEVICES)
   {
       TRACE (VINETIC, DBG_LEVEL_HIGH,
             ("ERROR: max. device number exceed!\n\r"));
       return -ENODEV;
   }
   IFXOS_MutexLock (SemDrvIF);
   if (pDev->nChannel > 0 && pDev->nChannel <= 8 )
   {
      pCh = (VINETIC_CHANNEL *) filp->private_data;
      pCh->nInUse--;
      pCh->pParent->nInUse--;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      /* free Fifo memory */
      if (pCh->nInUse == 0)
      {
         IFXOS_FREE (pCh->pFifo);
      }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      TRACE (VINETIC, DBG_LEVEL_LOW,
            ("Closing device channel %d (%d)\n\r",
              pCh->nChannel, pCh->nInUse));
   }
   else
   {
      TRACE (VINETIC, DBG_LEVEL_LOW,
            ("closing device %ld, Minor %ld...\n\r", dev_num + 1, num));
      pDev->nInUse--;
      /* memory will be released when module is removed */
   }
   IFXOS_MutexUnlock (SemDrvIF);
   /* decrement use counter */
   MOD_DEC_USE_COUNT;
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
   nDev - index of the VINETIC device
\param
   nCh - index of the VINETIC channel (1 = channel 0, ...)
\return
   handle to device/channel or IFX_ERROR on error
\remarks
  If not already done this will
   - allocate internal memory for each new device
   - allocate io memory
   - initialize the device
   - set up the interrupt
*/
int VINETIC_OpenKernel(int nDev, int nCh)
{
   VINETIC_DEVICE *pDev = NULL;
   VINETIC_CHANNEL *pCh = NULL;
   IFX_int32_t ret = IFX_ERROR;

   /* check the device number for the type 0 */
   if ((nDev >= VINETIC_MAX_DEVICES) || (nDev < 0))
   {
      TRACE (VINETIC, DBG_LEVEL_NORMAL,
            ("Warning: Driver only supports %d VINETIC(s)\n\r",
             VINETIC_MAX_DEVICES));
      ret = -ENODEV;
      goto OPEN_ERROR;
   }
   /* check if first open on this device */
   if (sVinetic_Devices[nDev] == NULL)
   {
      /* Now allocate and initialize all data. */
      if (OS_InitDevice(nDev) == IFX_ERROR)
      {
         ret = -ENODEV;
         goto OPEN_ERROR;
      }
   }
   IFXOS_MutexLock(SemDrvIF);
   pDev = sVinetic_Devices[nDev];
   /* increase in use count */
   pDev->nInUse++;

   if (nCh == 0)
   {
      /* return the device pointer */
      ret = (IFX_int32_t)pDev;
   }
   else
   {
      /* return the channel pointer */
      pCh = &pDev->pChannel [nCh - 1];
      pCh->nChannel = nCh;
      if (pCh->nInUse == 0)
      {
         ResetChMember (pCh);
      }
      pCh->nInUse++;
      ret = (IFX_int32_t)pCh;
   }

   /* increment module use counter */
   MOD_INC_USE_COUNT;

   TRACE (VINETIC, DBG_LEVEL_LOW,
         ("Open Kernel VINETIC device Nr. %d, channel %d\n\r", nDev, nCh));

   IFXOS_MutexUnlock (SemDrvIF);
   return ret;

OPEN_ERROR:
   if (pDev != NULL)
   {
      ReleaseDevice(pDev);
      TRACE (VINETIC,DBG_LEVEL_HIGH, ("Open Kernel Error 0x%lX\n", pDev->err));
   }
   return ret;
}

/**
   Release the device from Kernel mode.
\param
   nHandle - handle returned by VINETIC_OpenKernel
\return
   IFX_SUCCESS or error code
\remarks
*/
int VINETIC_ReleaseKernel(int nHandle)
{
   VINETIC_DEVICE *pDev;
   VINETIC_CHANNEL *pCh;

   IFXOS_MutexLock(SemDrvIF);
   if (((VINETIC_DEVICE*)nHandle)->nChannel == 0)
   {
       pDev = (VINETIC_DEVICE*)nHandle;
      TRACE (VINETIC, DBG_LEVEL_LOW,
            ("Closing Kernel device %ld\n\r", pDev->nDevNr));
      /* memory will be released when module is removed */
      pDev->nInUse--;
   }
   else
   {
      pCh = (VINETIC_CHANNEL*)nHandle;
      pCh->nInUse--;
      pCh->pParent->nInUse--;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      /* free Fifo memory */
      if (pCh->nInUse == 0)
      {
         IFXOS_FREE (pCh->pFifo);
      }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
      TRACE (VINETIC, DBG_LEVEL_LOW,
      ("Closing device channel %d (%d)\n\r",
       pCh->nChannel, pCh->nInUse));
   }

   IFXOS_MutexUnlock(SemDrvIF);
   /* decrement use counter */
   MOD_DEC_USE_COUNT;
   return IFX_SUCCESS;
}
/* @} */

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
/****************************************************************************
Description:
   gr909 task
Arguments:
   device - handle to a device (configuration or channel device)
Return Value:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   will be executed by poll function as a background task.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSTask (IFX_uint32_t device)
{
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)device;
   VINETIC_CHANNEL *pCh;
   IFX_int32_t     i;

   if (pDev->nChannel == 0)
   {
      /* call GR909 callback state machine */
      for (i = 0; i < VINETIC_ANA_CH_NR; i++)
      {
         pCh = &pDev->pChannel [i];
         if (pCh->Gr909.gr909tskId <= 0)
            continue;
         if (pCh->Gr909.Usr.status != GR909_TEST_RUNNING)
            continue;
         pCh->Gr909.Gr909Call (pCh);
      }
   }
   else
   {
      pCh = (VINETIC_CHANNEL *)device;
      if (pCh->Gr909.gr909tskId <= 0)
         return IFX_ERROR;
      if (pCh->Gr909.Usr.status != GR909_TEST_RUNNING)
         return IFX_ERROR;
      pCh->Gr909.Gr909Call (pCh);
   }

   return IFX_SUCCESS;
}

/****************************************************************************
Description:
   starts gr909 task ( in compliance with vxworks code)
Arguments:
   pCh  - handle to VINETIC_CHANNEL structure
Return Value:
   IFX_SUCCESS
Remarks:
   None.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSCreateTask (VINETIC_CHANNEL *pCh)
{
   /* set running state */
   pCh->Gr909.Usr.status = GR909_TEST_RUNNING;
   /* set task id */
   pCh->Gr909.gr909tskId = IFX_TRUE;
   /* wake up for state machine processing */
   wake_up_interruptible(&pCh->Gr909.wqState);
   return 0;
}

/****************************************************************************
Description:
   deletes gr909 task ( in compliance with vxworks code
Arguments:
   pCh  - handle to VINETIC_CHANNEL structure
Return Value:
   IFX_SUCCESS
Remarks:
   None.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSDeleteTask (VINETIC_CHANNEL *pCh)
{
   pCh->Gr909.gr909tskId = IFX_FALSE;
   return IFX_SUCCESS;
}

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */

/****************************************************************************
Description:
   Reads data from the vinetic.
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
static ssize_t VINETIC_Read(struct file *filp, IFX_char_t *buf, size_t count,
                             loff_t * ppos)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   VINETIC_CHANNEL *pCh = (VINETIC_CHANNEL*) filp->private_data;
   IFX_int32_t     ret = IFX_SUCCESS;

   /* check pointer */
   if ((filp == NULL) ||
       (filp->private_data == NULL) ||
       (pCh->nChannel == 0))
   {
       TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: read failed!\n\r"));
       return -ENODEV;
   }

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);
   /* count in bytes */
   ret = pCh->if_read (pCh, (IFX_uint8_t*) buf, count);
   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   if (ret == IFX_ERROR)              /* count smaller than packet size */
       ret = -EINVAL;           /* return linux error code to application */
   else if ((ret == 0) && (pCh->nFlags & CF_NONBLOCK)) /* no data available */
   {
       ret = -EAGAIN;           /* try again later (zero mean EOF) */
   }

   return ret;
#else
   return -EINVAL;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
}

/****************************************************************************
Description:
   Writes data to the VINETIC.
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
static ssize_t VINETIC_Write(struct file *filp, const char *buf, size_t count,
                              loff_t * ppos)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   VINETIC_CHANNEL *pCh = (VINETIC_CHANNEL*)filp->private_data;
   IFX_uint16_t    pData [MAX_PACKET_WORD] = {0};
   IFX_int32_t     len;

   /* check pointer */
   if (filp == NULL ||
       filp->private_data == NULL ||
       pCh->nChannel == 0)
   {
        TRACE (VINETIC, DBG_LEVEL_NORMAL, ("ERROR: write failed!\n\r"));
        return -ENODEV;
   }
   /* adjust lenght before copying to internal buffer */
   if (count > ((MAX_PACKET_WORD - 2) << 1))
      count = ((MAX_PACKET_WORD - 2) << 1);
   /* here the copy is only neccessary for linux. In VxWorks the user
      space could pass a buffer with two leading empty words */
   if (copy_from_user ((void *)&pData[2], (void *)buf, (IFX_uint32_t)count))
   {
      return -EFAULT;
   }
   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pCh->chAcc);
   /* count in words */
   len = pCh->if_write (pCh, (IFX_uint8_t*)pData, count);
   /* release lock */
   IFXOS_MutexUnlock (pCh->chAcc);

   return len;
#else
   return -EINVAL;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
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
static int VINETIC_Ioctl(struct inode *inode, struct file *filp,
                          unsigned int nCmd, unsigned long nArgument)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *) filp->private_data;
   VINETIC_CHANNEL *pCh  = NULL;
#ifdef TAPI
   /* tapi fifo protection */
   IFX_int32_t bTapiFifoProt = IFX_FALSE;
#endif /* TAPI */
   IFX_int32_t ret = IFX_SUCCESS;

   if (pDev == NULL)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: no device\n\r"));
      return -ENODEV;
   }

   if (pDev->nChannel)
   {
      /* regular channel: Get the VINETIC device & channel context   */
      pCh = (VINETIC_CHANNEL*)filp->private_data;
      pDev = (VINETIC_DEVICE*)pCh->pParent;
   }
   /* else
         Control channel: device context already set */

   /* This must be the very first ioctl operation before hardware access. */
   if (nCmd == FIO_VINETIC_BASICDEV_INIT)
   {
      VINETIC_BasicDeviceInit_t VINETIC_BasicDeviceInit;
      copy_from_user (&VINETIC_BasicDeviceInit, (void*)nArgument,
          sizeof(VINETIC_BasicDeviceInit_t));
      ret = BasicDeviceInit(pDev, &VINETIC_BasicDeviceInit);
      return ret;
   }
   /* This can be done to reset the device internal structure without doing
      any basic device initialization. Only the initialization state remains
      unchanged. All other states are reset.
   */
   if (nCmd == FIO_VINETIC_DEV_RESET)
   {
      ret = Device_Reset(pDev);
      return ret;
   }

   switch (nCmd)
   {
   case FIO_VINETIC_DRVVERS:
   case FIO_VINETIC_LASTERR:
   case FIO_VINETIC_REPORT_SET:
   case FIO_VINETIC_RUNTIME_TRACE_SET:
      /* no check */
      break;
   default:
      /* do not allow other ioctl if basic init is missing! */
      if ((pDev->nDevState & DS_BASIC_INIT) != DS_BASIC_INIT)
      {
         SET_ERROR(ERR_NOINIT);
         return IFX_ERROR;
      }
   }

   /* If we received a channel structure, set pDev to pCh->pParent */
   if (pDev->nChannel > 0 && pDev->nChannel <= 8)
   {
      if (!(
#ifdef TAPI
         (((nCmd & 0x0000FFFF) >= (IFXPHONE_GET_VERSION & 0x0000FFFF)) &&
           ((nCmd & 0x0000FFFF) <= (PHONE_PLAY_VOLUME_LINEAR & 0x0000FFFF)))
       || ((nCmd >= FIO_VINETIC_GR909_START  ) &&
           (nCmd <= FIO_VINETIC_GPIO_RELEASE))
       || (nCmd == FIO_VINETIC_LASTERR) || (nCmd == FIO_VINETIC_DOWNLOAD_CRAM)
#endif /* TAPI */
#ifdef QOS_SUPPORT
       || ((nCmd >= FIO_QOS_START ) && (nCmd <= FIO_QOS_CLEAN))
#endif /* QOS_SUPPORT */
       ))
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
               ("ERROR: Wrong channel or command, ch%d, cmd 0x%08x!\n\r",
                pDev->nChannel, nCmd));
         return IFX_ERROR;
      }
   }
#ifdef TAPI
     /* this io commands are reading from fifo and access must
        be therefore protected against interrupts */
     if ((nCmd == IFXPHONE_GET_PULSE)       ||
         (nCmd == IFXPHONE_GET_PULSE_ASCII) ||
         (nCmd == IFXPHONE_PULSE_READY)     ||
         (nCmd == PHONE_GET_DTMF)           ||
         (nCmd == PHONE_GET_DTMF_ASCII)     ||
         (nCmd == PHONE_DTMF_READY)         ||
         (nCmd == IFXPHONE_PHONE_STATUS)    ||
         (nCmd == IFXPHONE_CIDRX_DATA)
        )
     {
        bTapiFifoProt = IFX_TRUE;
     }
#endif /* TAPI */
   /* Dispatch IOCTL */
   switch (nCmd)
   {
#ifdef VIN_POLL
  case FIO_VINETIC_DRV_CTRL:
      copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (VINETIC_IO_DRVCTRL));
      ret = VINETIC_IrqPollConf ((VINETIC_IO_DRVCTRL*)IOcmd);
      break;
   case FIO_VINETIC_POLL_EVT:
      /* check interrupt register on each polling cycle */
      VINETIC_Poll_Events ();
      break;
#endif /* VIN_POLL */

   case FIO_VINETIC_LASTERR:
      copy_to_user ((IFX_int32_t *)nArgument, &pDev->err, sizeof(IFX_int32_t));
      break;
   /* basic functions module */
   ON_IOCTL_FRUSR (FIO_VINETIC_WCMD,    VINETIC_Write_Cmd,     VINETIC_IO_MB_CMD );
   ON_IOCTL       (FIO_VINETIC_RCMD,    VINETIC_Read_Cmd, VINETIC_IO_MB_CMD);
   ON_IOCTL_FRUSR (FIO_VINETIC_WSC,     VINETIC_Write_Sc, VINETIC_IO_WRITE_SC);
   ON_IOCTL       (FIO_VINETIC_RSC,     VINETIC_Read_Sc, VINETIC_IO_READ_SC);
   /* Download Module */
   ON_IOCTL       (FIO_VINETIC_DOWNFPI, VINETIC_DownloadFpi, VINETIC_IO_FPI_DOWNLOAD);
   ON_IOCTL       (FIO_VINETIC_DOW_PHI, VINETIC_DownloadPhi,   VINETIC_IO_PHI);
   ON_IOCTL_FRUSR (FIO_VINETIC_DOWNLOAD_CRAM, VINETIC_DownloadCram, VINETIC_IO_CRAM);
   /* Misc Module */
   ON_IOCTL       (FIO_VINETIC_VERS,    VINETIC_Version, VINETIC_IO_VERSION);
   /* Main Module */
   ON_IOCTL       (FIO_VINETIC_INIT,    VINETIC_Init,          VINETIC_IO_INIT);
   /* GPIO Module */
   ON_IOCTL(FIO_VINETIC_GPIO_RESERVE, VINETIC_GpioReserveUser, VINETIC_IO_GPIO_CONTROL);
   ON_IOCTL_FRUSR(FIO_VINETIC_GPIO_RELEASE, VINETIC_GpioReleaseUser, VINETIC_IO_GPIO_CONTROL);
   ON_IOCTL_FRUSR(FIO_VINETIC_GPIO_CONFIG, VINETIC_GpioConfigUser, VINETIC_IO_GPIO_CONTROL);
   ON_IOCTL(FIO_VINETIC_GPIO_GET, VINETIC_GpioGetUser, VINETIC_IO_GPIO_CONTROL);
   ON_IOCTL_FRUSR(FIO_VINETIC_GPIO_SET, VINETIC_GpioSetUser, VINETIC_IO_GPIO_CONTROL);
   /* report set */
   case FIO_VINETIC_REPORT_SET:
      ret =  VINETIC_Report_Set(nArgument);
      break;
   case FIO_VINETIC_RUNTIME_TRACE_SET:
      ret =  VINETIC_RuntimeTrace_Report_Set(nArgument);
      break;
   case FIO_VINETIC_TCA:
      ret = VINETIC_AccessTest (pDev, nArgument);
      break;
#ifdef TAPI
   case FIO_VINETIC_OFFSET_CALIBRATION:
      copy_from_user (IOcmd, (IFX_uint8_t*)nArgument, sizeof (VINETIC_IO_OFFSET_CALIBRATION));
      ret = VINETIC_DC_Offset_Calibration(pDev, (VINETIC_IO_OFFSET_CALIBRATION*) IOcmd);
      copy_to_user ((IFX_uint8_t*)nArgument, IOcmd, sizeof (VINETIC_IO_OFFSET_CALIBRATION));
      break;

   /* LT 2.0 ioctls */
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
   ON_IOCTL (FIO_VINETIC_LT_CURRENT,         VINETIC_LT_Current,
             VINETIC_IO_LT_CURRENT);
   ON_IOCTL (FIO_VINETIC_LT_VOLTAGE,         VINETIC_LT_Voltage,
             VINETIC_IO_LT_VOLTAGE);
   ON_IOCTL (FIO_VINETIC_LT_RESISTANCE,      VINETIC_LT_Resistance,
             VINETIC_IO_LT_RESISTANCE);
   ON_IOCTL (FIO_VINETIC_LT_CAPACITANCE,     VINETIC_LT_Capacitance,
             VINETIC_IO_LT_CAPACITANCE);
   ON_IOCTL (FIO_VINETIC_LT_IMPEDANCE,       VINETIC_LT_Impedance,
             VINETIC_IO_LT_IMPEDANCE);
   ON_IOCTL (FIO_VINETIC_LT_AC_MEASUREMENTS, VINETIC_LT_AC_Measurements,
             VINETIC_IO_LT_AC_MEASUREMENTS);
   ON_IOCTL (FIO_VINETIC_LT_AC_DIRECT,       VINETIC_LT_AC_Direct,
             VINETIC_IO_LT_AC_DIRECT);
#ifdef VIN_V21_SUPPORT
   ON_IOCTL (FIO_VINETIC_LT_RESISTANCE_RC,   VINETIC_LT_Resistance_RC,
             VINETIC_IO_LT_RESISTANCE_RC);
#endif /* VIN_V21_SUPPORT */
   case FIO_VINETIC_LT_CONFIG:
      ret = VINETIC_LT_Config((VINETIC_IO_LT_CONFIG*) nArgument);
      break;

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */
#endif /* TAPI */
   default:
#ifdef TAPI
      if (pCh == NULL)
      {
         switch (nCmd)
         {
         case IFXPHONE_GET_VERSION:
         case IFXPHONE_PHONE_STATUS:
         case IFXPHONE_VERSION_CHECK:
         case PHONE_CAPABILITIES:
         case PHONE_CAPABILITIES_LIST:
         case PHONE_CAPABILITIES_CHECK:
            IFXOS_MutexLock (pDev->pChannel[0].chAcc);
            ret = TAPI_Ioctl(pDev->pChannel[0].pTapiCh, nCmd, nArgument);
            IFXOS_MutexUnlock (pDev->pChannel[0].chAcc);
            break;
         default:
            /* forbidden ioctl for ctrl channels */
            ret = IFX_ERROR;
            break;
         }
      }
      else /* if (pCh != NULL) */
      {
         /* check if tapi connection pointer is valid */
         if (pCh->pTapiCh == NULL)
         {
            SET_ERROR(ERR_FUNC_PARM);
            ret = IFX_ERROR;
            TRACE (VINETIC, DBG_LEVEL_HIGH,
                  ("ERR: No Tapi Connection with channel %d(0 based)\n\r",
                  (pCh->nChannel - 1)));
            break;
         }
         /* process with channel fd */
         /* TAPI ioctls */
         if (((nCmd & 0x0000FFFF) >= (IFXPHONE_GET_VERSION & 0x0000FFFF))
         &&  ((nCmd & 0x0000FFFF) <= (PHONE_PLAY_VOLUME_LINEAR & 0x0000FFFF)))
         {
            IFXOS_MutexLock (pCh->chAcc);
            /* for tapi functions reading from fifo (consumer),
               protect against interrupts which writes into fifo (producer)
               as tapi is a transparent driver */
            if (bTapiFifoProt == IFX_TRUE)
            {
               Vinetic_IrqLockDevice (pDev);
            }
            ret = TAPI_Ioctl(pCh->pTapiCh, nCmd, nArgument);
            /* release interrupt lock if */
            if (bTapiFifoProt == IFX_TRUE)
            {
               Vinetic_IrqUnlockDevice (pDev);
            }
            IFXOS_MutexUnlock (pCh->chAcc);
         }
         else if ((nCmd >= FIO_VINETIC_GR909_START) && (nCmd <= FIO_VINETIC_FAULTCURR_SWITCHSLIC))
         {
            switch (nCmd)
            {
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
            /* GR909 realtime measurements */
            ON_IOCTL_FRUSR (FIO_VINETIC_GR909_START, VINETIC_GR909_Start, VINETIC_IO_GR909);
            case FIO_VINETIC_GR909_STOP:
               VINETIC_GR909_Stop (pCh, (IFX_int32_t)nArgument);
               break;
            case FIO_VINETIC_GR909_RESULT:
               ret = VINETIC_GR909_Result (pCh, (VINETIC_IO_GR909 *)nArgument);
              break;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
            default:
               TRACE (VINETIC, DBG_LEVEL_HIGH, ("Unknown GR909 IOCTL command %d\n",nCmd));
               ret = IFX_ERROR;
            }
         }
         else
         {
#ifdef QOS_SUPPORT
            /* perhaps the command is for Qos control */
            ret = Qos_Ctrl ((IFX_uint32_t)pCh, nCmd, nArgument);
            if ((ret == IFX_ERROR) && (pCh->pParent->err == ERR_FUNC_PARM))
#endif /* QOS_SUPPORT */
            {
               TRACE (VINETIC, DBG_LEVEL_HIGH, ("Unknown IOCTL command %d\n\r",nCmd));
               ret = IFX_ERROR;
            }
         }
      } /* if (pCh != NULL) */
#else /* TAPI */
      TRACE (VINETIC, DBG_LEVEL_HIGH,
             ("Unknown IOCTL command %d\n\r",nCmd));
      ret = IFX_ERROR;
#endif /* TAPI */
      break;
   }

   if ((ret == IFX_ERROR) && (pDev->err != ERR_OK))
   {
      TRACE(VINETIC,DBG_LEVEL_HIGH,("Error 0x%02lX occured (cmd %0X)\n\r",
            pDev->err, nCmd));
#ifdef TESTING
      PrintErrorInfo (pDev);
#else
#endif /* TESTING */
   }

   return ret;
}

/****************************************************************************
Description:
   polling
Arguments:
   file    - file pointer
   wait    - pointer to poll table
Return Value:
   IFXOS_SYSEXECPT, IFXOS_SYSREAD, IFXOS_SYSWRITE
Remarks:
   None.
****************************************************************************/
static unsigned int VINETIC_Poll(struct file *file, poll_table *wait)
{
   VINETIC_CHANNEL *pCh = file->private_data;
   IFX_int32_t ret = 0;
   IFX_int32_t res;

   /* install the poll queues of events to poll on */
   res = OS_InstallPollQueue ((IFX_void_t *)file, (IFX_void_t *)wait);
#ifdef TAPI
   if (pCh->nChannel == 0)
   {
      VINETIC_DEVICE* pDev = file->private_data;
      IFX_int32_t i;

      pDev->bNeedWakeup = IFX_TRUE;
      for (i = 0; i < VINETIC_ANA_CH_NR; i++)
      {
         VINETIC_CHANNEL *pCh = &pDev->pChannel[i];
         if (pCh->pTapiCh != NULL)
         {
            if (((~pCh->pTapiCh->TapiMiscData.nExceptionMask.Status)
                 & pCh->pTapiCh->TapiMiscData.nException.Status) != 0)
            {
               /* exception available so return action */
               ret |= IFXOS_SYSREAD;
               pDev->bNeedWakeup = IFX_FALSE;
               break;
            }
         }
      }
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
      VINETIC_GR909_OSTask ((IFX_uint32_t) pDev);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
   }
   /* if device ptr, no check for Tapi*/
   if (pCh->nChannel > 0 && pCh->nChannel <= VINETIC_CH_NR)
   {
      ret |= VINETIC_SelectCh (pCh, (IFX_int32_t)wait, (IFX_int32_t)file);
   }
#endif /* TAPI */

   return ret;
}

#ifdef VINETIC_USE_PROC

/******************************************************************************
   Read the version information from the driver.
\param
   buf - destination buffer
\return
   len
******************************************************************************/
static int VINETIC_get_version_proc (char *buf)
{
   int len;

   len = sprintf(buf, "%s\n\r", &DRV_VINETIC_WHATVERSION[4]);

   len += sprintf(buf + len, "Compiled on %s, %s for Linux kernel %s\n\r",
      __DATE__, __TIME__, UTS_RELEASE);

   return len;
}


#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/****************************************************************************
   Read the read fifo status from the driver.
\param
   buf - destination buffer
\return
   len
****************************************************************************/
static int VINETIC_readfifo_proc(char *buf)
{
   VINETIC_DEVICE *pDev;
   IFX_int32_t    dev, ch;
   int len=0;

   for (dev = 0; dev < VINETIC_MAX_DEVICES; dev++)
   {
      pDev = &VDevices[dev];
      if (pDev != IFX_NULL)
      {
         len += sprintf(buf + len, "dev %d: err=0x%lX\n\r",
            pDev->nChannel, pDev->err);

         for (ch = 0; ch < VINETIC_CH_NR; ch++)
         {
            len += sprintf(buf + len, "ch%ld: ", ch);
            if (pDev->pChannel[ch].rdFifo.size)
            {
               len += sprintf(buf + len, "pStart=%p, pEnd=%p, pRead=%p, "
                  "pWrite=%p, size=%ld %c\n\r",
                  pDev->pChannel[ch].rdFifo.pStart,
                  pDev->pChannel[ch].rdFifo.pEnd,
                  pDev->pChannel[ch].rdFifo.pRead,
                  pDev->pChannel[ch].rdFifo.pWrite,
                  pDev->pChannel[ch].rdFifo.size,
                  (Fifo_isEmpty(&pDev->pChannel[ch].rdFifo) ? 'E' : 'F'));
            }
            else
            {
               len += sprintf(buf + len, "not initialized\n\r");
            }

         }
      }
   }
   return len;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

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
static int VINETIC_Read_Proc (char *page, char **start, off_t off,
   int count, int *eof, void *data)
{
   int (*fn)(char *buf), len;

   if (data != NULL)
   {
      fn = data;
      len = fn (page);
   }
   else
   {
      return 0;
   }
   if (len <= off+count)
   {
      *eof = 1;
   }
   *start = page + off;
   len -= off;
   if (len > count)
   {
      len = count;
   }
   if (len < 0)
   {
      len = 0;
   }

   return len;
}



/******************************************************************************/
/**
   Initialize and install the proc entry

\return
   IFX_SUCCESS or IFX_ERROR
\remark
   Called by the kernel.
*/
/******************************************************************************/
static int VINETIC_install_proc_entry (void)
{
   struct proc_dir_entry *driver_proc_node;

   /* install the proc entry */
   TRACE(VINETIC, DBG_LEVEL_NORMAL, ("vinetic: using proc fs\n\r"));

   driver_proc_node = proc_mkdir( "driver/" DEV_NAME, NULL);
   if (driver_proc_node != NULL)
   {
      create_proc_read_entry("version" , 0, driver_proc_node,
                             VINETIC_Read_Proc, (void *)VINETIC_get_version_proc);
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      create_proc_read_entry("readfifo" , 0, driver_proc_node,
                             VINETIC_Read_Proc, (void *)VINETIC_readfifo_proc);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   }
   else
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,("vinetic: cannot create proc entry\n\r"));
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Remove proc entries

\remark
   Called by the kernel.
*/
/******************************************************************************/
static void VINETIC_remove_proc_entry(void)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   remove_proc_entry("driver/" DEV_NAME "/readfifo",0);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   remove_proc_entry("driver/" DEV_NAME "/version" ,0);
   remove_proc_entry("driver/" DEV_NAME ,0);

   return;
}

#endif /* VINETIC_USE_PROC */


/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
Arguments:
   pText - text to be logged
Return:
Remarks:
   implemented to avoid the unresolved symbol problem when loading the module
*******************************************************************************/
IFX_void_t Drv_Log(const IFX_char_t *pText)
{
   printk (pText);
}

/****************************************************************************
Description:
   disables irq line if the driver is in interrupt mode and irq line is
   actually enabled according to device flag bIntEnabled.
   Disable the global interrupt if the device is not in polling mode and no
   interrupt line is connected.
Arguments:
   pDev - vinetic device handle
Return Value:
   none
Remarks
   If the driver works in Polling mode, nothing is done.
   If the driver works in interrupt mode and the irq was already disabled
   (flag bIntEnabled is IFX_FALSE), the os disable function will
   not be called.

   Very important: It is assumed that disable and enable irq are done
   subsequently and that no routine calling disable/enable is executed
   inbetween as stated in following code example:

Allowed :

   Vinetic_IrqLockDevice;
   .. some instructions
   Vinetic_IrqUnlockDevice

Not allowed:

   routineX (IFX_int32_t x)
   {
      Vinetic_IrqLockDevice;
      .. some instructions;
      Vinetic_IrqUnlockDevice
   }

   routineY (IFX_int32_t y)
   {
      Vinetic_IrqLockDevice;
      routine (x);    <---------------- routineX unlocks interrupts..
      be carefull!
      ... some more instructions;
      Vinetic_IrqUnlockDevice;
   }
****************************************************************************/
IFX_void_t Vinetic_IrqLockDevice (VINETIC_DEVICE *pDev)
{
   vinetic_interrupt_t* pIrq = pDev->pIrq;

   /* driver used in polling mode : exit */
   if (pDev->IrqPollMode & VIN_EVENT_POLL)
      return;

   if (pIrq != IFX_NULL)
   {
      /* device specific interrupt routine is initialized */
      /* interrupt line was disabled already: exit */
      if (pIrq->bIntEnabled == IFX_FALSE)
         return;
      /* it must be possible to take the mutex before disabling the irq */
      if (!in_interrupt())
         IFXOS_MutexLock(pIrq->intAcc);
      /* invoke board or os routine to disable irq */
      VIN_DISABLE_IRQLINE (pIrq->nIrq);
      /* reset enable flag to signalize that interrupt line is disabled */
      pIrq->bIntEnabled = IFX_FALSE;
   }
   else
   {
      /* there is no specific interrupt service routine configured for that
         device. Disable the global interrupts instead */
      if (pDev->nIrqMask == 0)
      {
         VIN_DISABLE_IRQGLOBAL(pDev->nIrqMask);
      }
   }

#ifdef DEBUG_INT
   pIrq->nCount++;
#endif /* DEBUG_INT */
}

/****************************************************************************
Description:
   enables irq line if the driver is in interrupt mode and irq line is
   actually disabled according to device flag bIntEnabled.
   Enable the global interrupt, if this was disabled by 'Vinetic_IrqLockDevice'
Arguments:
   pDev - vinetic device handle
Return Value:
   none
Remarks
   cf Remarks of Vinetic_IrqLockDevice () in this file.
****************************************************************************/
IFX_void_t Vinetic_IrqUnlockDevice (VINETIC_DEVICE *pDev)
{
   vinetic_interrupt_t* pIrq = pDev->pIrq;

   /* driver used in polling mode : exit */
   if (pDev->IrqPollMode & VIN_EVENT_POLL)
      return;
   if (pIrq != IFX_NULL)
   {
      /* device specific interrupt routine is initialized */
      /* interrupt line was enabled already: exit */
      if (pIrq->bIntEnabled == IFX_TRUE)
         return;
      /* set enable flag to signalize that interrupt line is enabled */
      pIrq->bIntEnabled = IFX_TRUE;
      /* invoke board or os routine to enable irq */
      VIN_ENABLE_IRQLINE(pIrq->nIrq);
      IFXOS_MutexUnlock(pIrq->intAcc);
   }
   else
   {
      /* there is no specific interrupt service routine configured for that
         device. Enable the global interrupts again */
      if (pDev->nIrqMask != 0)
      {
         IFX_uint32_t nMask = pDev->nIrqMask;
         pDev->nIrqMask = 0;
         VIN_ENABLE_IRQGLOBAL(nMask);
      }
   }

#ifdef DEBUG_INT
   pIrq->nCount--;
#endif /* DEBUG_INT */
}

/****************************************************************************
Description:
   Enable the global interrupt if it was disabled by 'Vinetic_IrqLockDevice'.
   Do not enable the interrupt line if it is configured on the device.
   This function has to be called in error case inside the driver to unblock
   global interrupts but do not device interrupt. The interrupt line for that
   specific device is not enable in case of an error.
Arguments:
   pDev - vinetic device handle
Return Value:
   none
Remarks
   cf Remarks of Vinetic_IrqLockDevice () in this file.
****************************************************************************/
IFX_void_t Vinetic_IrqUnlockGlobal (VINETIC_DEVICE *pDev)
{
   if (pDev->nIrqMask != 0)
   {
      IFX_uint32_t nMask = pDev->nIrqMask;
      pDev->nIrqMask = 0;
      VIN_ENABLE_IRQGLOBAL(nMask);
   }

#ifdef DEBUG_INT
   pIrq->nCount--;
#endif /* DEBUG_INT */
}

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
static int __init vinetic_module_init(void)
{
   IFX_int32_t result, i;
#ifdef CONFIG_DEVFS_FS
   IFX_int32_t j;
   IFX_char_t buf[10];
#endif /* CONFIG_DEVFS_FS */

   IFXOS_MutexInit (SemDrvIF);
   /* Set Default Trace */
#ifdef DEBUG
   SetTraceLevel(VINETIC, DBG_LEVEL_NORMAL);
#else
   SetTraceLevel(VINETIC, DBG_LEVEL_HIGH);
#endif /* DEBUG */
   SetTraceLevel(VINETIC, DBG_LEVEL_NORMAL);

#ifdef CONFIG_DEVFS_FS

   /* clear devfs structures */
   memset (&(devfs_handle), 0x00, sizeof(devfs_handle));

   for (i = 0; i < VINETIC_MAX_DEVICES; ++i)
   {
      for (j = 0; j < VINETIC_MINOR_BASE-1; j++ )
      {
         sprintf (buf, "vin%ld%ld", i+1, j);
         if (( devfs_handle[i*VINETIC_MINOR_BASE + j]
                                  = devfs_register( NULL,
                                                    buf,
                                                    DEVFS_FL_DEFAULT,
                                                    VINETIC_MAJOR,
                                                    (i+1)*VINETIC_MINOR_BASE + j,
                                                    S_IFCHR | S_IRUGO | S_IWUGO,
                                                    &Vinetic_fops,
                                                    (void *) 0
            )) == NULL)
         {
            TRACE( VINETIC, DBG_LEVEL_HIGH,
                 ("VINETIC: unable to add device /dev/%s to dev_fs\n\r", buf));
            result = -ENODEV;
            return result;
         }
      }
   }
#else
   /* Register the caracter device and allocate the major number */
   result = register_chrdev(VINETIC_MAJOR, DEV_NAME , &Vinetic_fops);
   if ( result < 0 )
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("Loading %s module ERROR: major %d not found!",
             DEV_NAME, VINETIC_MAJOR));
      return result;
   }
#endif /* CONFIG_DEVFS_FS */

   TRACE(VINETIC, DBG_LEVEL_HIGH, ("%s ... \n\r", &DRV_VINETIC_WHATVERSION[4]));

   for (i = 0; i < VINETIC_MAX_DEVICES; ++i)
   {
      sVinetic_Devices[i] = NULL;
   }
#ifdef VINETIC_USE_PROC
   VINETIC_install_proc_entry();
#endif /* VINETIC_USE_PROC */
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
static void __exit vinetic_module_exit(void)
{
   IFX_int32_t i;
#ifdef CONFIG_DEVFS_FS
   IFX_int32_t j;
#endif /* CONFIG_DEVFS_FS */

   TRACE (VINETIC, DBG_LEVEL_LOW, ("cleaning up %s module ...\n\r", DEV_NAME));
   for (i=0; i < VINETIC_MAX_DEVICES; i++)
   {
#ifdef CONFIG_DEVFS_FS
      /* remove vinetic devices from dev fs */
      for (j = 0; j < VINETIC_MINOR_BASE-1; j++ )
      {
         if (devfs_handle[i*VINETIC_MINOR_BASE + j])
         {
            devfs_unregister (devfs_handle[i*VINETIC_MINOR_BASE + j]);
         }
      }
#endif /* CONFIG_DEVFS_FS */
      if (sVinetic_Devices[i] != NULL)
      {
         ReleaseDevice (sVinetic_Devices[i]);
      }
   }
#ifdef DEBUG
   dbgResult();
#endif /* DEBUG */
#ifndef CONFIG_DEVFS_FS
   /* unregister the module, free the major number */
   unregister_chrdev (VINETIC_MAJOR, DEV_NAME);
#endif /* CONFIG_DEVFS_FS */
#ifdef VINETIC_USE_PROC
   VINETIC_remove_proc_entry();
#endif /* VINETIC_USE_PROC */
}

module_init(vinetic_module_init);
module_exit(vinetic_module_exit);

#endif /* LINUX */

