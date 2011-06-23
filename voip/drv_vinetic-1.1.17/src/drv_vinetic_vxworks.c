/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_api.h"
#include "drv_vinetic_api.h"
#include "drv_vinetic_vxworks.h"

#include "drv_vinetic_main.h"
#include "drv_vinetic_int.h"

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
#include "drv_vinetic_gr909.h"
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */

#if (CPU_FAMILY==PPC)
/* for INUM_TO_IVEC / IVEC_TO_INUM */
#include <arch/ppc/ivPpc.h>
#endif
#ifdef TAPI_LL_DRV
#include "drv_tapi_interface.h"
#include "drv_tapi_ll_interface.h"
#endif /* TAPI_LL_DRV */

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
#define VIN_SYS_REGISTER_INT_HANDLER(irq,func,arg)             \
            intConnect(INUM_TO_IVEC(irq), (VOIDFUNCPTR)(func), \
            (IFX_int32_t)(arg))
#endif /* VIN_SYS_REGISTER_INT_HANDLER */

/****************************************************************************
Description:
   macro to map the system function with takes care of interrupt handling
   unregistration.
Arguments:
   irq   - irq number
Remarks:
   The macro is by default mapped to the operating system method. For systems
   integrating different routines, this macro must be adapted in the user con-
   figuration header file.
****************************************************************************/
#ifndef VIN_SYS_UNREGISTER_INT_HANDLER
#define VIN_SYS_UNREGISTER_INT_HANDLER(irq)  \
         VIN_SYS_REGISTER_INT_HANDLER((irq), OS_IRQHandler_Dummy, (irq))
#endif /* VIN_SYS_UNREGISTER_INT_HANDLER */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* The Global device driver structure */
Y_S_dev_ctx VIN_dev_ctx;
/* list of vinetic interrupts, neccessary for shared interrupt handling */
IFX_LOCAL vinetic_interrupt_t* VIN_irq_head = IFX_NULL;
#ifdef TAPI_LL_DRV
static TAPI_LL_CTX_t* pTapiDrvCtx;
#endif /* TAPI_LL_DRV */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* Local helper functions */
IFX_LOCAL IFX_int32_t DeviceCreate (Y_S_dev_ctx* pDevCtx);
IFX_LOCAL IFX_int32_t DeviceDelete (Y_S_dev_ctx* pDevCtx);
IFX_LOCAL Y_S_dev_ctx*       GetDeviceCtx(IFX_void_t);
IFX_LOCAL IFX_void_t OS_IRQHandler(vinetic_interrupt_t* pIrq);
IFX_LOCAL IFX_void_t OS_IRQHandler_Dummy(int i);
/* Device Init. Called when the device has been opened for
   the first time. The device structure will be cached */
IFX_LOCAL IFX_int32_t OS_InitDevice (IFX_int32_t nDevn);
/* Functions handling the select/unselect driver system io calls */
IFX_LOCAL IFX_int32_t OS_OnSelect   (VINETIC_CHANNEL *pCh, IFX_int32_t nArgument,
                                     IFX_boolean_t bCtrlDev);
IFX_LOCAL IFX_int32_t OS_OnUnselect (VINETIC_CHANNEL *pCh, IFX_int32_t nArgument,
                                     IFX_boolean_t bCtrlDev);



/****************************************************************************
Description:

This function open a VINETIC device previously registered to the OS during the
device driver creation. For each Vinetic channel, a device have been added in
the device list with the device string "/dev/vin/DC":
    - D: device number
    - C: channel number in the device

The OS passes the Y_S_dev_header structure associated to the device "/dev/vin/DC":
    DevHdr: VxWorks specific header
    ddrvn : VINETIC Device Driver Number (allocated by IOS)
    devn  : device number  (1 to VINETIC_MAX_DEVICES)
    chnn  : channel number (0 to VINETIC_MAX_CH_NR)
    pctx  : pointer to the channel context (will be allocated at the device open)


The function will update the pctx field in the device header with the VINETIC
device channel (control/voice) selected structure.

Arguments:
  pDevHeader - device header pointer
  pAnnex     - tail of device name / not used
  flags      - open flags / not used
Return Value:
  pDevHeader - device header pointer
                <> NULL, the device is open without error
                == NULL, error
Remarks:
 None.
****************************************************************************/
IFX_int32_t vinetic_open(Y_S_dev_header *pDevHeader, INT8 *pAnnex, IFX_int32_t flags)
{
   VINETIC_DEVICE  *pDev = NULL;    /* Current Vinetic device structure         */
   VINETIC_CHANNEL *pCh  = NULL;    /* Current Vinetic device channel structure */
   Y_S_dev_ctx     *pDevCtx = NULL; /* device context pointer                   */

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */
   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
   {
      /* The device driver is not installed */
      return (ERROR);
   }
   /* check device range */
   if ((pDevHeader->devn > pDevCtx->maxdev) || (pDevHeader->devn < 0))
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: max. device number exceed\n\r"));
      goto OPEN_ERROR;
   }
   /* PdevVin: points to the VINETIC device structure selected */
   /*          from "pDevHeader->devn".                        */
   pDev = pDevCtx->pdevv[pDevHeader->devn - 1];

   if (pDev == NULL)
   {
      if (OS_InitDevice (pDevHeader->devn - 1) == IFX_SUCCESS)
      {
         OS_GetDevice(pDevHeader->devn - 1, &pDev);
      }
   }
   /* if pDev is still NULL, something's wrong ?!? */
   if (pDev == NULL)
   {
      goto OPEN_ERROR;
   } /* end (pDev != NULL) */
   /* protect against concurrent access */
   if (pDevCtx->SemDrvIF != NULL)
   {
      IFXOS_MutexLock (pDevCtx->SemDrvIF);
   }
   /* check if first open of control device or channel */
   if (pDevHeader->InUse == 0)
   {
      /* Increase the VINETIC device use counter               */
      /* Get the channel number in order to determine the      */
      /* Vinetic device structure to map in the device header  */
      /* The channel number have been memorized in the device  */
      /* header at the device creation (pDevHeader->chnn)      */
      pDevHeader->InUse++;
      pDev->nInUse++;
      if (pDevHeader->chnn == VINETIC_DEVICE_CONTROL_CHN_TYPE)
      {
         /* hey ! It's a control/configuration channel type  */
         /* which is device wise. We select for this device  */
         /* the VINETIC device structure as working context  */
         pDevHeader->pctx = pDev;
         pDev->nChannel   = pDevHeader->chnn;
      }
      else
      {
         /* It's a regular voice channel type, which is      */
         /* channel wise. We select for this device, the     */
         /* VINETIC channel structure as working context     */
         /* designated by pDevHeader->chnn.                  */
         pDevHeader->pctx = &pDev->pChannel [pDevHeader->chnn - 1];
         pCh              = pDevHeader->pctx;
         pCh->nChannel    = pDevHeader->chnn;
         #ifdef TAPI
         pCh->pTapiCh = &(pDev->TapiDev.pChannelCon [pDevHeader->chnn - 1]);
         #endif
         /* At the first (grand !) opening we initialize the */
         /* channel member structure.                        */
         if (pCh->nInUse == 0)
         {
            VINETIC_UpdateChMember (pCh);
            pCh->nFlags |= CF_NONBLOCK;
         }
         pCh->nInUse++;
      } /* end (pDevHeader->chnn == VINETIC_DEVICE_CONTROL_CHN_TYPE) */

      TRACE (VINETIC, DBG_LEVEL_LOW,
         ("Open VINETIC device Nr. %ld, Channel %ld\n\r",
            pDevHeader->devn, pDevHeader->chnn));
   }
   else
   {
      pDevHeader->InUse++;
      pDev->nInUse++;
      /* The device is already initialized    */
      /* so just increment the In Use counter */
      if (pDevHeader->chnn != VINETIC_DEVICE_CONTROL_CHN_TYPE)
      {
         pCh = (VINETIC_CHANNEL*)pDevHeader->pctx;
         pCh->nInUse++;
      }
   } /* end (pDevHeader->InUse == 0) */
   /* release lock */
   if (pDevCtx->SemDrvIF != NULL)
   {
      IFXOS_MutexUnlock (pDevCtx->SemDrvIF);
   }

   return (IFX_int32_t)pDevHeader;

OPEN_ERROR:
   if (pDevCtx->SemDrvIF != NULL)
   {
      IFXOS_MutexUnlock (pDevCtx->SemDrvIF);
   }
   return (ERROR);
}

/****************************************************************************
Description:
   polling
Arguments:
   filp -
   table -
Return Value:

Remarks:
   None.
****************************************************************************/
IFX_LOCAL unsigned int VINETIC_Poll (Y_S_dev_header* pDevHeader)
{
   IFX_int32_t     ret = 0;
#ifdef EVALUATION
   VINETIC_DEVICE* pDev = NULL;
#endif /* EVALUATION */
   VINETIC_CHANNEL* pCh = NULL;
#ifdef TAPI
   TAPI_CONNECTION* pTapiChannel = NULL;
#endif

   if (((VINETIC_CHANNEL*)(pDevHeader->pctx))->nChannel == 0)
   {
#ifdef EVALUATION
      pDev = (VINETIC_DEVICE*)pDevHeader->pctx;

      /* call the polling routine */
      if (VINETIC_CheckPoll ((IFX_void_t *)pDev) == IFX_SUCCESS)
      {
         /* allowed select to read the data */
         ret |= IFXOS_SYSEXCEPT;
      }
#else
      TRACE (VINETIC, DBG_LEVEL_HIGH,
         ("Vinetic_Poll channel for control device without EVALUATION\n\r"));
#endif /* EVALUATION */
   }
   else
   {
      pCh = (VINETIC_CHANNEL*)pDevHeader->pctx;
#ifdef TAPI
      pTapiChannel = (TAPI_CONNECTION *) pCh->pTapiCh;
#endif

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      if (!Fifo_isEmpty(&pCh->rdFifo))
      {
         /*TRACE (VINETIC,DBG_LEVEL_NORMAL, ("Data Ch%d!\n\r",pCh->nChannel));*/
         ret |= IFXOS_SYSREAD;
      }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */

#ifdef TAPI
      /* check if tapi needs wakeup */
      if (((~pTapiChannel->TapiMiscData.nExceptionMask.Status) &
         pTapiChannel->TapiMiscData.nException.Status) != 0)
      {
         ret |= IFXOS_SYSEXCEPT;
      }
#endif
   }

   return (unsigned int)ret;
}

/****************************************************************************
Description:
   Configuration / Control for the device.
Arguments:
   pDevHeader - device header pointer
   nCmd       - Configuration/Control command
   nArgument  - Configuration/Control arguments
Return Value:
   IFX_SUCCESS    - no problems
   <> IFX_SUCCESS - Function is not implemented or other error
Remarks:
   None.
****************************************************************************/
IFX_int32_t vinetic_ioctl(Y_S_dev_header *pDevHeader, IFX_int32_t nCmd,
                          IFX_int32_t nArgument)
{
   IFX_int32_t ret = IFX_SUCCESS;
   VINETIC_DEVICE  *pDev;            /* Vinetic device context        */
   VINETIC_CHANNEL *pCh;            /* Vinetic channel context        */
   Y_S_dev_ctx*    pDevCtx = NULL;     /* Global device context pointer */
   IFX_boolean_t    bCtrlDev;

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */

   pDevCtx = GetDeviceCtx ();   /* Get the device context */
   if (pDevCtx == NULL)
   {
      /* The device driver is not installed */
      errno = ENODEV;
      return IFX_ERROR;
   }

   /* pDevHeader: device header updated at the device opening      */
   /*     - pctx: points to the VINETIC device channel structure   */

   if (pDevHeader->pctx == NULL)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: no device\n\r"));
      errno = ENODEV;
      return IFX_ERROR;
   }

   if (pDevHeader->chnn)
   {
      /* regular channel: Get the VINETIC device & channel context   */
      pCh = (VINETIC_CHANNEL*)pDevHeader->pctx;
      pDev = (VINETIC_DEVICE*)pCh->pParent;
      bCtrlDev = IFX_FALSE;
   }
   else
   {
      /* Control channel: Get the VINETIC device context   */
      pDev = (VINETIC_DEVICE*) pDevHeader->pctx;
      pCh = &pDev->pChannel [0];
      bCtrlDev = IFX_TRUE;
   }

   /* process io command according to type */
   switch (IFXOX_IO_GETMAGIC((IFX_uint32_t)nCmd))
   {
      case VINETIC_IOC_MAGIC:
         ret = VINETIC_Common_Ioctl (pCh, (IFX_uint32_t)nCmd,
                                     (IFX_uint32_t)nArgument, bCtrlDev);
         break;
   #ifdef EVALUATION
      case  VINETIC_IOC_MAGIC_EVAL:
         if (nCmd == FIO_VINETIC_POLL)
         {
            *(IFX_uint32_t*)nArgument = VINETIC_Poll (pDevHeader);
            ret = IFX_SUCCESS;
         }
         else
         {
            ret = VINETIC_Eval_Ioctl (pCh, (IFX_uint32_t)nCmd,
                                      (IFX_uint32_t)nArgument);
         }
         break;
   #endif /* EVALUATION */
   #ifdef TAPI
      case IFX_TAPI_IOC_MAGIC:
         ret = VINETIC_Tapi_Ioctl (pCh, (IFX_uint32_t)nCmd,
                                   (IFX_uint32_t)nArgument, bCtrlDev);
         break;
   #endif /* TAPI */
   #ifdef QOS_SUPPORT
      case QOS_IOC_MAGIC:
         ret = Qos_Ctrl ((IFX_uint32_t)pCh, (QOS_CMD)nCmd,
                         (IFX_uint32_t)nArgument);
         break;
   #endif /* QOS_SUPPORT */
      default:
         switch (nCmd)
         {
         case FIOSELECT:
            ret = OS_OnSelect (pCh, nArgument, bCtrlDev);
            break;
         case FIOUNSELECT:
            ret = OS_OnUnselect (pCh, nArgument, bCtrlDev);
            break;
         default:
            TRACE (VINETIC, DBG_LEVEL_HIGH,
                   ("Unknown IOCTL command %ld\n\r",nCmd));
            return IFX_ERROR;
         }
         break;
   }

   if ((ret == IFX_ERROR) && (pDev->err != ERR_OK))
   {
      TRACE(VINETIC,DBG_LEVEL_NORMAL, ("Error 0x%02lX occured (cmd %0lX)\n\r",
            pDev->err, nCmd));
#ifdef TESTING
      PrintErrorInfo (pDev);
#endif /* INTERNAL */
   }

   return ret;
}

/****************************************************************************
Description:
   Close the device. The in-use counter will be decreased.
Arguments:
   pDevHeader - device header pointer
Return Value:
   OK - no error
   -ENODEV - MAX_SOC_CHANNELS is exceeded
Remarks:
****************************************************************************/
STATUS vinetic_close(Y_S_dev_header *pDevHeader)
{
   VINETIC_DEVICE* pDev = NULL;     /* Current Vinetic device structure         */
   VINETIC_CHANNEL*pCh  = NULL;     /* Current Vinetic device channel structure */
   Y_S_dev_ctx*    pDevCtx = NULL;     /* device context pointer                   */

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */

   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
       return -ENODEV;           /* The device driver is not installed */

   /* pDevHeader: device header updated at the device opening      */
   /*     - pctx: points to the VINETIC device channel structure   */

   if (pDevHeader->pctx == NULL)
   {
       TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: no device\n\r"));
       return -ENODEV;
   }

   IFXOS_MutexLock (pDevCtx->SemDrvIF);

   /* we first try to cast the context pointer to a device structure, just */
   /* to test if it's a channel (VINETIC_CHANNEL*) or a device (VINETIC_DEVICE*) */
   /* since nChannel is available in both structures this will work */
   /* if pDevHeader->pctx is a channel, pCh is used instead of pDev */
   pDev = (VINETIC_DEVICE*)pDevHeader->pctx;
   pDevHeader->InUse--;

   if (pDevHeader->InUse == 0)
   {
      /* We erase the reference to the Vinetic structure in   */
      /* header.                                              */
      pDevHeader->pctx = NULL;
   }

   /* Test the channel number in order to determine the     */
   /* Vinetic device structure                              */
   if (pDevHeader->chnn != VINETIC_DEVICE_CONTROL_CHN_TYPE)
   {
      pCh = (VINETIC_CHANNEL*)((IFX_void_t *)pDev);
      pDev = (VINETIC_DEVICE*) pCh->pParent;

      pCh->nInUse--;
      pDev->nInUse--;

#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
      /*free Fifo memory */
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
      pDev->nInUse--;
   }

   TRACE (VINETIC, DBG_LEVEL_LOW,
         ("closing device %ld, Minor %ld...\n\r", pDevHeader->devn, pDevHeader->devn));

   IFXOS_MutexUnlock (pDevCtx->SemDrvIF);
   return (OK);
}


/****************************************************************************
Description:
   Read data from the Vinetic.
Arguments:
   pDevHeader - device header pointer
   pDest      - data destination pointer
   nLength    - data length to read
Return Value:
   len        - data length
Remarks:
   not used.
****************************************************************************/
IFX_int32_t vinetic_read (Y_S_dev_header *pDevHeader, IFX_uint8_t *pDest, IFX_int32_t nLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   VINETIC_CHANNEL* pChnVin;        /* Vinetic channel context             */
   IFX_int32_t      len;            /* return length on the read operation */
   Y_S_dev_ctx*     pDevCtx = NULL; /* device context pointer              */

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */

   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
     return 0;            /* The device driver is not installed */

   pChnVin = (VINETIC_CHANNEL*)pDevHeader->pctx;   /* Get the channel context  */

   if (pDevHeader->pctx == NULL || pChnVin->nChannel == 0)
   {
     TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: read failed!\n\r"));
     return 0;
   }
   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pChnVin->chAcc);
   /* read */
   len = pChnVin->if_read (pChnVin, (IFX_uint8_t*) pDest, nLength);
   /* release lock */
   IFXOS_MutexUnlock (pChnVin->chAcc);

   return len;
#else
   return ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
}


/****************************************************************************
Description:
   Writes data to the VINETIC.
Arguments:
   pDevHeader - device header pointer
   pSrc       - data source pointer
   nLength    - data length to write
Return Value:
   nLength - 0 if failure else the length
Remarks:
   None.
****************************************************************************/
IFX_int32_t vinetic_write(Y_S_dev_header *pDevHeader, IFX_uint8_t *pSrc, IFX_int32_t nLength)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   VINETIC_CHANNEL* pChnVin;        /* Vinetic channel context             */
   IFX_int32_t      len;            /* return length on the read operation */
   Y_S_dev_ctx*     pDevCtx = NULL; /* device context pointer              */
   IFX_uint16_t     pData [MAX_PACKET_WORD] = {0};

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */
   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
      return 0;           /* The device driver is not installed */
   /* Get the channel context  */
   pChnVin = (VINETIC_CHANNEL*)pDevHeader->pctx;

   if (pDevHeader->pctx == NULL || pChnVin->nChannel == 0)
   {
        TRACE (VINETIC, DBG_LEVEL_NORMAL, ("ERROR: write failed!\n\r"));
        return 0;
   }
   /* adjust lenght before copying to internal buffer */
   if (nLength > ((MAX_PACKET_WORD - 2) << 1))
      nLength = ((MAX_PACKET_WORD - 2) << 1);

   memcpy(&pData[2], pSrc, (IFX_uint32_t)nLength);

   /* protect channel against concurrent tasks */
   IFXOS_MutexLock (pChnVin->chAcc);
   /* count in words */
   len = pChnVin->if_write (pChnVin, (IFX_uint8_t*)pData, nLength);
   /* release lock */
   IFXOS_MutexUnlock (pChnVin->chAcc);

   return len;
#else
   return ERROR;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
}

/****************************************************************************
Description:
    VINETIC device driver initialization.
    This is the device driver initialization function to call at the system
    startup prior any access to the VINETIC device driver.
    After the initialization the device driver is ready to be accessed by
    the application. The global structure "VIN_dev_ctx" contains all the data
    handling the interface (open, close, ioctl,...).

Arguments:
   None
Return Value:
   OK or ERROR
Remarks:
   None
****************************************************************************/
IFX_int32_t Vinetic_DeviceDriverInit(VOID)
{
   printf("\n\rInstalling %s for max.%d devices ... \n\r",
      &DRV_VINETIC_WHATVERSION[4],
      VINETIC_MAX_DEVICES);

   /* The "VIN_dev_ctx" global area contains all the         */
   /* data related to the VINETIC device driver installation */
   /* The area is used during all the life of the VINETIC    */
   /* device driver. The area is updated during the creation */
   /* and the access to the device driver.                   */
   /* Reset the memory area.                                 */

   memset(&VIN_dev_ctx, 0x00, sizeof(Y_S_dev_ctx));


   /* Set the Default Traces */
#ifdef DEBUG
   SetTraceLevel(VINETIC, DBG_LEVEL_NORMAL);
#else
   SetTraceLevel(VINETIC, DBG_LEVEL_HIGH);
#endif

   /* Vinetic Device Driver installation:          */
   /* The device driver is declared to the         */
   /* the target OS and all the device are         */
   /* installed. The device driver will be ready   */
   /* to be used by the application.               */

   if (DeviceCreate(&VIN_dev_ctx) == ERROR)
      return (ERROR);

   IFXOS_MutexInit (VIN_dev_ctx.SemDrvIF);
#ifdef TAPI_LL_DRV
   {
   result = TAPI_DrvRegister (VIN_dev_ctx->pdevv, &pTapiDrvCtx,
                              VINETIC_MAX_DEVICES * VINETIC_MAX_CH_NR, "vin");
   TAPI_LL_DrvInit (pTapiDrvCtx);
   }
#endif /* TAPI_LL_DRV */

   /* very first call used for calibration  of the */
   /* self-calibrating hard delay routines library */
   /* "DelayLib.c"                 */
   /* For IDES3300 this calibration is done before, */
   /* so this leads only to an additonal affordable */
   /* wait of 1us */
   IFXOS_DELAYUS(1);

   return (OK);
}




/****************************************************************************
Description:
    VINETIC device driver shutdown.
    This is the device driver shutdown function. This function is called by the
    system when the VINETIC device driver is no longer needed. Prior to shutdown
    the device driver all the device channels should be closed.
    This function releases all the resources granted by the device driver.

Arguments:
   None
Return Value:
   OK or ERROR
Remarks:
   None
****************************************************************************/
IFX_int32_t Vinetic_DeviceDriverStop(VOID)
{
   Y_S_dev_ctx*    pDevCtx = NULL;     /* Global device context pointer */

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */

   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
      return ERROR;           /* The device driver is not installed */

   /* Deletion of all the VINETIC devices      */
   /* based on the device driver dimensioning  */

   if (DeviceDelete(&VIN_dev_ctx) == ERROR)
      return (ERROR);

   IFXOS_MutexDelete (pDevCtx->SemDrvIF);
   return (OK);
}

/****************************************************************************
Description:
   VINETIC device driver creation.
   This function is called at the system startup (Vinetic_DeviceDriverInit)
   in order to create and install the VINETIC device driver in the target OS (VxWorks).


   Device driver Creation: the device driver is declared to the IOS system with
   "iosDrvInstall" OS function. We pass to the IOS the device driver interface
   functions (open, close, ioctl, ...), the IOS returns a Device Driver Number which
   will be used for adding the devices to the device driver.


   Device addition: Once the Device driver have been declared to the IOS system,
   we gone add the devices to the device driver list. Each device is identified by
   a unique string which will be used by the application for opening a device
   (the application will get in return a unique file descriptor (fd) allocated by
   the system and will be used for further access to the selected device).

   Device string selection: "/dev/vinDC" with,

        - "/dev/vin", this is a VINETIC device
        - "D",        designate the device number
                        "1",    VINETIC device 1
                        "2",    VINETIC device 2
                        ...
                        "VINETIC_MAX_DEVICES" VINETIC device VINETIC_MAX_DEVICES
        - "C",        designate a channel (C) in the designated device (D)
                        "0",    Control channel (device wise)
                        "1",    voice channel 1 (channel wise)
                        ...,
                        "VINETIC_MAX_CH_NR", voice channel VINETIC_MAX_CH_NR (channel wise)


   VINETIC_MAX_DEVICES, number maximum of VINETIC device
   VINETIC_MAX_CH_NR,       numbre maximum of voice channels

   The VINETIC device driver is a multi-device driver, for each device, a device is
   accessed through channels. We distinghish to types of channels:
    - Control channel, used for control and configuration purpose. It's device wise and
    is always the channel number ZERO (0).
    - Voice channel, used to access a device voice channel. It's channel wise and the
    channel is <> to ZERO (0).

    Each device channel (Control/Voice) is represented by a device in the IOS system.
    The device is added to the device list for the VINETIC device driver, the number
    of devices per VINETIC device is:

          VINETIC_MAX_CH_NR (number of Voice channels) + 1 (Control Channel).


    The "Y_S_dev_header" structure is passed to the IOS sytem:
        DevHdr: VxWorks specific header
        ddrvn : VINETIC Device Driver Number (allocated by IOS)
        devn  : device number  (1 to VINETIC_MAX_DEVICES)
        chnn  : channel number (0 to VINETIC_MAX_CH_NR)
        pctx  : pointer to the channel context (will be allocated at the device open)


Arguments:
   pDevCtx     The Global device driver structure address
Return Value:
   OK or ERROR
Remarks:
   None
****************************************************************************/
IFX_LOCAL IFX_int32_t DeviceCreate(Y_S_dev_ctx* pDevCtx)
{
   Y_S_dev_header *pDevHeader = NULL;      /* device header pointer */
   IFX_uint8_t    nDev,nCh,nMinor;
   IFX_char_t     buf[64];


   /* IOS device driver creation.                  */
   /* We add the VINETIC device driver to the      */
   /* IOS device list providing the device driver  */
   /* function interface.                          */
   /* The IOS returns the Device Driver Number     */
   /* which will be used later on.                 */

   pDevCtx->ddrvn = iosDrvInstall(NULL, NULL,
      (FUNCPTR) vinetic_open,
      (FUNCPTR) vinetic_close,
      (FUNCPTR) vinetic_read,
      (FUNCPTR) vinetic_write,
      (FUNCPTR) vinetic_ioctl);

   if (pDevCtx->ddrvn == ERROR)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
         ("ERROR:  unable to install the driver\n\r"));
      return (ERROR);
   }

   pDevCtx->maxdev = VINETIC_MAX_DEVICES ;
   pDevCtx->maxchn = VINETIC_MAX_CH_NR ;

   /* The device driver is declared to the IOS:    */
   /* Now we add to the device driver IOS list the */
   /* devices for each VINETIC device channel      */

   nMinor = 0;
   for(nDev = 1; nDev < VINETIC_MAX_DEVICES + 1; nDev++)
   {

      /* allocate memory for channels and control device */
      for(nCh = 0; nCh < VINETIC_MAX_CH_NR+1; nCh++)
      {

         /* allocate memory for the device header    */
         /* Reset the memory                         */
         /* Build the device string "/dev/vinij"     */
         /* add the device in the IOS device list    */
         /* Calculate the device channel minor (k)   */
         /* update the device context                */
         pDevHeader = (Y_S_dev_header*)IFXOS_MALLOC(sizeof(Y_S_dev_header));
         if (pDevHeader == NULL)
            return (ERROR);

         memset(pDevHeader, 0x00, sizeof(Y_S_dev_header));

         sprintf(buf, "%s%d%d",VINETIC_DEVICE_NAME, nDev, nCh);

         /* Add the device to the device list    */
         if (iosDevAdd(&pDevHeader->DevHdr, buf, pDevCtx->ddrvn) == ERROR)
         {
            TRACE (VINETIC, DBG_LEVEL_HIGH,
                   ("ERROR: unable to add the device %s.\n\r", buf));
            IFXOS_FREE (pDevHeader);
            return (ERROR);
         }

         pDevHeader->ddrvn   = pDevCtx->ddrvn;
         pDevHeader->devn    = nDev;
         pDevHeader->chnn    = nCh;
         pDevHeader->InUse   = 0;
         pDevCtx->pdevh[nMinor++] = pDevHeader;
      }
   }
   return (OK);
}



/****************************************************************************
Description:
   VINETIC device driver deletion.
   This function is called at the device driver shutdown
   (Vinetic_DeviceDriverStop) in order to release and uninstall the OS-target
   resources (VxWorks) granted by the VINETIC device driver.

Arguments:
   pDevCtx     The Global device driver structure address
Return Value:
   OK or ERROR
Remarks:
   None
****************************************************************************/
IFX_LOCAL IFX_int32_t DeviceDelete(Y_S_dev_ctx* pDevCtx)
{
   IFX_uint8_t    nDev;

   /* Check that we have a valid Global structure address      */
   /* and that a device is installed (OS-target point of view) */

   if ((pDevCtx == NULL) || (pDevCtx->ddrvn == ERROR))
   {
      return (ERROR);
   }

   for(nDev = 0; nDev < MAX_DEVICE_HEADER; nDev++)
   {

      /* Delete the device from the IOS link list */
      /* free the device header                   */

      if(pDevCtx->pdevh[nDev])
      {
         iosDevDelete((DEV_HDR *)pDevCtx->pdevh[nDev]);
         IFXOS_FREE (pDevCtx->pdevh[nDev]);
      }
   }

   /* We remove the VINETIC device driver from the IOS */
   /* We free the Vinetic device structures            */

   iosDrvRemove(pDevCtx->ddrvn, IFX_TRUE);

   for(nDev = 0; nDev < VINETIC_MAX_DEVICES; nDev++)
   {

      /* Delete the device from the IOS link list */
      /* free the device header                   */

      if(pDevCtx->pdevv[nDev])
      {
         IFXOS_FREE (pDevCtx->pdevv[nDev]);
      }
   }

   /* Reset to 0 the Global device driver structure */
   /* in order to reset all the pointers to NULL    */
   /* marking that the device driver is no longer   */
   /* installed.                                    */

   memset(pDevCtx, 0x00, sizeof(Y_S_dev_ctx));
   return (OK);
}

/*******************************************************************************
Description:
   This function initializes the VINETIC base address.
   If necessary, OS specific mapping is done here.
Arguments:
    pDev    - handle of the VINETIC device structure.
   nBaseAddr - physical address for vinetic access.
Return:
   always IFX_SUCCESS
Remarks:
   none
*******************************************************************************/
IFX_int32_t OS_Init_Baseadress(VINETIC_DEVICE* pDev, IFX_uint32_t nBaseAddr)
{
   pDev->nBaseAddr = nBaseAddr;
   pDev->pBaseAddr = (IFX_uint16_t*)nBaseAddr;

   return IFX_SUCCESS;
}


/****************************************************************************
Description:
   VINETIC Interupt Service Routine entry-point.
Arguments:
   irq: number of the irq
   dev_id: pointer to the device structure
   pt_regs:
Remarks:
   This Function is called by the OS when a VINETIC interrupt occurs. This
   function calls the VINETIC OS-independent ISR routine which will serve
   interrupt events.
****************************************************************************/
IFX_LOCAL IFX_void_t OS_IRQHandler(vinetic_interrupt_t* pIrq)
{
   VINETIC_DEVICE* pDev = pIrq->pdev_head;

   while (pDev != IFX_NULL)
   {
      /* call the VINETIC Interrupt Service Routine */
      /* to serve the VINETIC interrupt events.     */
      VINETIC_interrupt_routine(pDev);
      pDev = pDev->pInt_NextDev;
   }
}

/*******************************************************************************
Description:
    Dummy Interrupt Service Routine in lieu of the VINETIC ISR.

Arguments:
   pDev  - handle of the device structure
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_void_t OS_IRQHandler_Dummy(int i)
{
   logMsg ("VINETIC_irq_handler_Dummy called for int %d !",i,0,0,0,0,0);
}

/*******************************************************************************
Description:
    Handles the FIOSELECT system call.
Arguments:
   pCh       - handle of the channel structure
   nArgument - Ioctl argument
   bCtrlDev  - TRUE: Control device / FALSE
Return:
   IFX_SUCCESS
Remarks
   The set of wake up queues are different for the VINETIC configuration/control
   channel and for the voice channel.
*******************************************************************************/
IFX_LOCAL IFX_int32_t OS_OnSelect (VINETIC_CHANNEL *pCh, IFX_int32_t nArgument,
                                   IFX_boolean_t bCtrlDev)
{
   IFX_int32_t ret;
   VINETIC_DEVICE *pDev = pCh->pParent;

   if (bCtrlDev == IFX_TRUE)
   {
#ifdef TAPI
      {
         IFX_int32_t i;
         selNodeAdd(&(pDev->EventList), (SEL_WAKEUP_NODE*) nArgument);
         pDev->bNeedWakeup = IFX_TRUE;
         for (i = 0; i < VINETIC_MAX_ANA_CH_NR; i++)
         {
            VINETIC_CHANNEL *pChTmp = &pDev->pChannel[i];
            if (((~pChTmp->pTapiCh->TapiMiscData.nExceptionMask.Status)
               & pChTmp->pTapiCh->TapiMiscData.nException.Status) != 0)
            {
               selWakeup((SEL_WAKEUP_NODE*) nArgument);
               pDev->bNeedWakeup = IFX_FALSE;
               break; /* fixme: return without other selnodeadd ? save time.. */
            }
         }
      }
#endif /* #ifdef TAPI */
#ifdef ADDED_SUPPORT
      /* select on the Device Interrupt Error Status Queue */
      selNodeAdd(&(pDev->PollErr), (SEL_WAKEUP_NODE*) nArgument);
      if (!Fifo_isEmpty (&pDev->ErrFifo))
      {
         pDev->bNeedWakeup = IFX_FALSE;
         selWakeup((SEL_WAKEUP_NODE*) nArgument);
      }
      else
      {
         pDev->bNeedWakeup = IFX_TRUE;
      }
#endif /* ADDED_SUPPORT */
#ifdef EVALUATION
      {
         IFX_int32_t   ch;
         VINETIC_EVAL* pEval = pDev->pEval;
         /* no LEC tests are supported */
         /* select on a configuration/control  channel */
         if (pEval->bPollRead [POLLDEV_INTINFO] == IFX_TRUE)
         {
            pEval->bNeedWakeup = IFX_TRUE;
            selNodeAdd(&(pEval->PollQueue[POLLDEV_INTINFO]),
                       (SEL_WAKEUP_NODE*) nArgument);
            if (pEval->bPollEvt[POLLDEV_INTINFO] == IFX_TRUE)
            {
               pEval->bNeedWakeup = IFX_FALSE;
               selWakeup((SEL_WAKEUP_NODE*) nArgument);
            }
            /* reset for futher use */
            pEval->bPollEvt[POLLDEV_INTINFO] = IFX_FALSE;
         }
         for (ch = 0; ch < VINETIC_MAX_CH_NR; ch ++)
         {
            if (((VINCH_EVAL*)pDev->pChannel[ch].pEval)->bDialDetection == IFX_TRUE)
            {
               pEval->bNeedWakeup = IFX_TRUE;
               selNodeAdd(&(pEval->PollQueue[POLLDEV_INTINFO]),
                          (SEL_WAKEUP_NODE*) nArgument);
               if (((VINCH_EVAL*)pDev->pChannel[ch].pEval)->bDialDetected == IFX_TRUE)
               {
                  pEval->bNeedWakeup = IFX_FALSE;
                  selWakeup((SEL_WAKEUP_NODE*) nArgument);
               }
            }
         }
      }
#endif /* EVALUATION */
   }
   else
   {
      ret = VINETIC_SelectCh (pCh, nArgument, 0);
      if ((ret & IFXOS_SYSREAD) &&
          (selWakeupType((SEL_WAKEUP_NODE*) nArgument) == SELREAD))
      {
         selWakeup((SEL_WAKEUP_NODE*) nArgument);
      }
      if ((ret & IFXOS_SYSWRITE) &&
          (selWakeupType((SEL_WAKEUP_NODE*) nArgument) == SELWRITE))
      {
         selWakeup((SEL_WAKEUP_NODE*) nArgument);
      }
      /* Select() requires a return value of zero (OK, IFX_SUCCESS) */
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
    Handles the FIOUNSELECT system call.
Arguments:
   pCh       - handle of the channel structure
   nArgument - Ioctl argument
   bCtrlDev  - TRUE: Control device / FALSE
Return:
   IFX_SUCCESS
Remarks
   The set of wake up queues are different for the VINETIC configuration/control
   channel and for the voice channel.
*******************************************************************************/
IFX_LOCAL IFX_int32_t OS_OnUnselect (VINETIC_CHANNEL *pCh, IFX_int32_t nArgument,
                                     IFX_boolean_t bCtrlDev)
{
   VINETIC_DEVICE *pDev = pCh->pParent;

   if (bCtrlDev == IFX_TRUE)
   {
#ifdef TAPI
      selNodeDelete(&(pDev->EventList), (SEL_WAKEUP_NODE*) nArgument);
#endif /* #ifdef TAPI */
#ifdef ADDED_SUPPORT
      /* unselect on the Device Interrupt Error Status Queue */
      selNodeDelete(&(pDev->PollErr), (SEL_WAKEUP_NODE*) nArgument);
#endif /* ADDED_SUPPORT */
#ifdef EVALUATION
      {
         IFX_int32_t ch;
         VINETIC_EVAL* pEval = pDev->pEval;
         /* unselect on a configuration/control  channel */
         if (((VINETIC_EVAL*)pDev->pEval)->bPollRead [POLLDEV_INTINFO] == IFX_TRUE)
         {
            selNodeDelete(&(pEval->PollQueue[POLLDEV_INTINFO]),
                          (SEL_WAKEUP_NODE*) nArgument);
         }
         for (ch = 0; ch < VINETIC_MAX_CH_NR; ch ++)
         {
            if (((VINCH_EVAL*)pDev->pChannel [ch].pEval)->bDialDetection == IFX_TRUE)
            {
               selNodeDelete(&(pEval->PollQueue[POLLDEV_INTINFO]),
                             (SEL_WAKEUP_NODE*) nArgument);
            }
         }
      }
#endif /* #else #ifdef EVALUATION */
   }
   else
   {
      /* Voice Streaming */
      selNodeDelete(&(pCh->SelRead), (SEL_WAKEUP_NODE*) nArgument);
#ifdef EXCHFD_SUPPORT
      selNodeDelete(&(pChannel->WakeupList), (SEL_WAKEUP_NODE*) nArgument);
#endif /* EXCHFD_SUPPORT */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
      selNodeDelete(&(pCh->wqWrite), (SEL_WAKEUP_NODE*) nArgument);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
      /* clear */
      pCh->nFlags &= ~CF_NEED_WAKEUP;
   }

   return IFX_SUCCESS;
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
IFX_void_t OS_Install_VineticIRQHandler(VINETIC_DEVICE* pDev, IFX_int32_t nIrq)
{
   vinetic_interrupt_t* pIrq;
   vinetic_interrupt_t* pIrqPrev = IFX_NULL;
   VINETIC_DEVICE* tmp_pDev;
   int intstatus;

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
      pIrq = malloc(sizeof(vinetic_interrupt_t));
      if (pIrq == IFX_NULL)
      {
         /** \todo error status !*/
         return;
      }
      memset(pIrq, 0, sizeof(vinetic_interrupt_t));
      IFXOS_MutexInit(pIrq->intAcc);
      /** \todo not working, because mutex must be given by same task */
      /*    IFXOS_MutexLock(pIrq->intAcc); */
      taskLock();
      /* register new dedicated or first of shared interrupt */
      pDev->pIrq = pIrq;
      pDev->pInt_NextDev = IFX_NULL;
      pIrq->pdev_head = pDev;
      pIrq->next_irq = IFX_NULL;
      pIrq->nIrq = nIrq;
      /* Install the Interrupt routine in the OS */
      if (VIN_SYS_REGISTER_INT_HANDLER(nIrq, OS_IRQHandler, pIrq) == ERROR)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: request interrupt failed\n\r"));
      }
      else
         pIrq->bRegistered = IFX_TRUE;

      /* add to list of interrupts */
      /* this list is not used by the interrupt handler itself,
         so no additional intLock necessary! */
      if (VIN_irq_head == IFX_NULL)
         VIN_irq_head = pIrq;
      else
      {
         /* pIrqPrev must be valid */
         IFXOS_ASSERT (pIrqPrev == IFX_NULL);
         pIrqPrev->next_irq = pIrq;
      }
      taskUnlock();
/*      pIrq->bIntEnabled = IFX_TRUE;
      VIN_ENABLE_IRQLINE (pIrq->nIrq);
*/
   }
   else
   {
      intstatus = intLock();
      /* add into the list of an existing shared interrupt */
      pDev->pIrq = pIrq;
      pDev->pInt_NextDev = IFX_NULL;
      tmp_pDev = pIrq->pdev_head;
      while (tmp_pDev->pInt_NextDev != IFX_NULL)
      {
         tmp_pDev = tmp_pDev->pInt_NextDev;
      }
      tmp_pDev->pInt_NextDev = pDev;
      intUnlock(intstatus);
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
IFX_void_t OS_UnInstall_VineticIRQHandler (VINETIC_DEVICE* pDev)
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
         /* unregister Interrupt routine */
         if (VIN_SYS_UNREGISTER_INT_HANDLER(pIrq->nIrq) == ERROR)
         {
            TRACE (VINETIC, DBG_LEVEL_HIGH, ("ERROR: request interrupt failed\n\r"));
         }
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
      free(pIrq);
   }
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
      routine (x);    <-----routineX unlocks interrupts. Be carefull!
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
      if (intContext() != TRUE)
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
IFX_void_t Vinetic_IrqUnlockDevice(VINETIC_DEVICE *pDev)
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

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
/****************************************************************************
Description:
   Gr909 task
Arguments:
   chanDev - handle to a channel device
Return Value:
   None.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSTask (IFX_uint32_t device)
{
   VINETIC_CHANNEL *pCh = (VINETIC_CHANNEL *)device;
   IFX_boolean_t bStop = IFX_FALSE;

   if (pCh->nChannel != 0)
   {
      for (;;)
      {
         IFXOS_WaitEvent (pCh->hostCh.Gr909.wqState);
         switch (pCh->hostCh.Gr909.nState)
         {
         case TEST_OK_STATE:
         case TEST_NOTOK_STATE:
         case INTERNAL_ERR_STATE:
            bStop = IFX_TRUE;
            break;
         default:
            break;
         }
         if (bStop == IFX_TRUE)
            break;
         if ((pCh->hostCh.Gr909.Usr.status == GR909_TEST_RUNNING) &&
             (pCh->hostCh.Gr909.Gr909Call != NULL))
         {
            pCh->hostCh.Gr909.Gr909Call (pCh);
         }
         if (pCh->hostCh.Gr909.Gr909Call == NULL)
            break;
      }
      pCh->hostCh.Gr909.Gr909Call (pCh);
      VINETIC_GR909_OSDeleteTask (pCh);
   }

   return IFX_SUCCESS;
}

/****************************************************************************
Description:
   Creates Gr909 task
Arguments:
   pCh - handle to VINETIC_CHANNEL structure
Return Value:
   IFX_SUCCESS / IFX_ERROR.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSCreateTask (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t ret = IFX_SUCCESS;

   pCh->hostCh.Gr909.gr909tskId =
      taskSpawn("tskVin_gr909", 1, 0, 5000, (FUNCPTR)VINETIC_GR909_OSTask,
                 pCh, 0);

   if (pCh->hostCh.Gr909.gr909tskId == -1)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH, ("Gr909 task couldn't be started\n\r"));
      ret = IFX_ERROR;
   }
   else
   {
      /* set running state */
      pCh->hostCh.Gr909.Usr.status = GR909_TEST_RUNNING;
      /* activate the task */
      IFXOS_WakeUpEvent (pCh->hostCh.Gr909.wqState);
   }

   return ret;
}

/****************************************************************************
Description:
   Deletes gr909 task
Arguments:
   pCh - handle to VINETIC_CHANNEL structure
Return Value:
   IFX_SUCCESS / IFX_ERROR.
****************************************************************************/
IFX_int32_t VINETIC_GR909_OSDeleteTask (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t ret;

   /* delete the task */
   ret = taskDelete(pCh->hostCh.Gr909.gr909tskId);
   if (ret == IFX_SUCCESS)
      pCh->hostCh.Gr909.gr909tskId = 0;

   return ret;
}

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */

/****************************************************************************
Description:
    This function returns the Global Device Driver structure address
    (VIN_dev_ctx). This function returns the Global Device Driver structure
    address if the device driver is actually installed or NULL otherwise.


Arguments:
   None
Return Value:
   Y_S_dev_ctx*        Global Device Driver structure address

Remarks:
   None
****************************************************************************/
IFX_LOCAL Y_S_dev_ctx* GetDeviceCtx (IFX_void_t)
{
   if (VIN_dev_ctx.pdevh[0] == NULL)
   {
      printf ("ERROR: Vinetic Device Driver not installed\n");
      return NULL;
   }
   return (&VIN_dev_ctx);
}

/****************************************************************************
Description:
   Get device pointer
Arguments:
   nr - number of device
   pDev - returned pointer
Return Value:
   IFX_ERROR if error, otherwise IFX_SUCCESS
Remarks:
   Used by board / main module to reset device states
****************************************************************************/
IFX_int32_t OS_GetDevice (IFX_int32_t nr, VINETIC_DEVICE** pDev)
{
   if (nr >= VINETIC_MAX_DEVICES || VIN_dev_ctx.pdevv[nr] == NULL)
   {
      printf ("IFX_ERROR in OS_GetDevice: nr=%ld, max=%d\n", nr, VINETIC_MAX_DEVICES);
      return IFX_ERROR;
   }
   *pDev = VIN_dev_ctx.pdevv[nr];
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
   Called form Board_DevOpen() to Used by board / main module to reset device states
****************************************************************************/
IFX_LOCAL IFX_int32_t OS_InitDevice (IFX_int32_t nDevn)
{
   VINETIC_DEVICE* pDev = NULL;     /* Current Vinetic device structure */
   Y_S_dev_ctx*    pDevCtx = NULL;  /* device context pointer           */

   /* Get the Global device driver context */
   /* and check that the device driver is  */
   /* already installed.                   */
   pDevCtx = GetDeviceCtx ();  /* Get the device context */
   if (pDevCtx == NULL)
   {
      /* The device driver is not installed */
      return (IFX_ERROR);
   }
   /* Get the VINETIC device context:              */
   /*  at the first opening for the device we      */
   /*  allocate the memory for the VINETIC device  */
   /*  which includes the device context and the   */
   /*  channel contextes.                          */

   /* Only Initialize if the structure has not been initialized before */
   /* e.g. during startup */
   if (pDevCtx->pdevv[nDevn] == NULL)
   {
      /* Allocate memory for the device */
      pDev = (VINETIC_DEVICE*)IFXOS_MALLOC(sizeof (VINETIC_DEVICE));
      if (pDev == NULL)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: No Memory for the device structure!\n\r"));
         return (IFX_ERROR);
      }
      /* Initialize the memory area to ZERO         */
      /* Update the device number                   */
      /* Address initialization                     */
      /* working space initialization               */
      /* memorize the Vinetic device context in the */
      /* device context.                            */
      memset(pDev, 0, sizeof(VINETIC_DEVICE));
      pDev->nDevNr = nDevn;
      pDevCtx->pdevv[nDevn] = pDev;

      /* OS independent initializations */
      if (VINETIC_InitDevMember (pDev) == IFX_ERROR)
      {
         printf ("OS_InitDevice: VINETIC_InitDevMember dev %ld failed\r\n",
            pDev->nDevNr);
         return (IFX_ERROR);
      }
   }
   return (IFX_SUCCESS);
}


