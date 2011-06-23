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
 ****************************************************************************/
/** 
   \file drv_vinetic_gpio.c VINETIC GPIO/IO resource managment module.
   \remarks
  This module provides resource managment for the VINETIC device GPIO and the
  VINETIC channel IO pins. A usermode application or other kernel mode drivers
  may reserve IO/GPIO pins for exclusive use.
  Every access to the VINETIC IO/GPIO resources has to be done via this module
  (direct access using VINETIC messages must be avoided!)./
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_main.h"
#include "drv_vinetic_gpio.h"
#include "drv_vinetic_gpio_user.h"
#include "vinetic_io.h"

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_int32_t _GpioConfig(VINETIC_DEVICE* pDev, VINETIC_GPIO_CONFIG const *pCfg);
IFX_LOCAL IFX_int32_t _GpioGet(VINETIC_DEVICE* pDev, IFX_uint16_t *pGet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _GpioSet(VINETIC_DEVICE* pDev, IFX_uint16_t nSet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _GpioIntMask(VINETIC_DEVICE* pDev, IFX_uint16_t nSet, IFX_uint16_t nMask, IFX_int32_t nMode);

IFX_LOCAL IFX_int32_t _IoConfig(VINETIC_CHANNEL *pCh, VINETIC_GPIO_CONFIG const *pCfg);
IFX_LOCAL IFX_int32_t _IoGet(VINETIC_CHANNEL* pCh, IFX_uint16_t *pGet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _IoSet(VINETIC_CHANNEL* pCh, IFX_uint16_t nSet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _IoIntMask(VINETIC_CHANNEL* pCh, IFX_uint16_t nSet, IFX_uint16_t nMask, IFX_int32_t nMode);

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#define GPIO_SET_ERROR(no) \
   do { \
      pGpio->err = no; \
   } while(0)

/* ============================= */
/* Local function definition     */
/* ============================= */

/** \defgroup VinGpioInternal VINETIC GPIO Internal Functions
\remarks
  These functions are only used within <b>module</b> drv_vinetic_gpio.c
*/

/* @{ */

/**
  Configure the GPIO(s) according to passed settings
\param
  pDev - pointer to VINETIC device structure
\param
  pCfg - pointer to GPIO configuration structure
\return
  IFX_SUCCESS if requested GPIO configuration was succesful, else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values will be written during initialization.
*/
IFX_LOCAL IFX_int32_t _GpioConfig(VINETIC_DEVICE* pDev, VINETIC_GPIO_CONFIG const *pCfg)
{
   IFX_int32_t  i, nCur;
   IFX_int32_t  err     = IFX_SUCCESS;
   IFX_uint16_t nGpio   = pCfg->nGpio;

   /* Set GPIO mode of requested resources to 0 (PHI master)*/
   if (pDev->nDevState & DS_FW_DLD)
   {
      err = rdReg((VINETIC_CHANNEL*)pDev, GCR1_REG);
   }
   if (err == IFX_SUCCESS)
   {
      GCR1.value &= (0x00FF | (~(pCfg->nGpio << 8)));
      if (pDev->nDevState & DS_FW_DLD)
      {
         err = wrReg((VINETIC_CHANNEL*)pDev, GCR1_REG);
      }
   }
   /* Do input/output configuration for each resource bit set in nGpio */
   for (i = 0; i < 8; i++)
   {
      if (err == IFX_ERROR)
      {
         break;
      }
      nCur = (1 << i);
      if ((nGpio & nCur) != 0)
      {
         if (pCfg->nMode & GPIO_MODE_OUTPUT)
         {
            /* Enable output /and/ disable according input */
            GCR2.value &= ~(0xFF00 & (nCur << 8));
            GCR2.value |= (0x00FF & nCur);
            if (pDev->nDevState & DS_FW_DLD)
            {
               err = wrReg((VINETIC_CHANNEL*)pDev, GCR2_REG);
            }
         }
         else if (pCfg->nMode & GPIO_MODE_INPUT)
         {
            /* Enable input /and/ disable according output */
            GCR2.value &= ~(0x00FF & nCur);
            GCR2.value |= (0xFF00 & (nCur << 8));
            if (pDev->nDevState & DS_FW_DLD)
            {
               err = wrReg((VINETIC_CHANNEL*)pDev, GCR2_REG);
            }
            if ((err == IFX_SUCCESS) && (pCfg->nMode & GPIO_MODE_INT))
            {
               /* Configure interrupt */
               err = _GpioIntMask(pDev, 0, nCur, pCfg->nMode);
               /* Register callback */
               pDev->GpioRes.gpioCallback[i] = pCfg->callback;
            }
         }
      }
   }
   return(err);
}

/**
  Read the GPIO input value(s) from the VINETIC device
\param
  pDev - pointer to VINETIC device structure
\param
  pGet - pointer where the read value shall be stored
\param
  nMask - only bits set to '1' will be valid on return
\return
   IFX_SUCCESS if requested GPIO was read succesful, IFX_ERROR if read failed or device
   not initialized.
\remarks
*/
IFX_LOCAL IFX_int32_t _GpioGet(VINETIC_DEVICE* pDev, IFX_uint16_t *pGet, IFX_uint16_t nMask)
{
   IFX_int32_t err = IFX_ERROR;

   if (pDev->nDevState & DS_FW_DLD)
   {
      err = ScRead(pDev, SC_RSRGPIO, pGet, 1);
      *pGet &= nMask;
   }
   return(err);
}

/**
  Write the GPIO value(s) to the VINETIC device
\param
  pDev - pointer to VINETIC device structure
\param
  nSet - values to store
\param
  nMask - only bits set to '1' will be stored
\return
   IFX_SUCCESS if requested GPIO was written succesful or stored to shadow variable,
   else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values will be written during initialization.
*/
IFX_int32_t _GpioSet(VINETIC_DEVICE* pDev, IFX_uint16_t nSet, IFX_uint16_t nMask)
{
   IFX_int32_t err = IFX_SUCCESS;

   GCR1.value = (GCR1.value & ~nMask) | (nSet & nMask);
   if (pDev->nDevState & DS_FW_DLD)
   {
      err = wrReg((VINETIC_CHANNEL*) pDev, GCR1_REG);
   }
   return(err);
}

/**
  Mask or unmask the GPIO interrupt
\param
  pDev - pointer to VINETIC device structure
\param
  nSet - bitmask for interrupts to mask (0 = unmasked, 1 = masked)
\param
  nMask - only bits set to '1' will be stored
\param
  nMode - mode according to VINETIC_GPIO_MODE
\return
   IFX_SUCCESS if requested mask change was succesful, else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values will be written during initialization.
*/
IFX_LOCAL IFX_int32_t _GpioIntMask(VINETIC_DEVICE* pDev, IFX_uint16_t nSet,
                                   IFX_uint16_t nMask, IFX_int32_t nMode)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (nMode & GPIO_INT_RISING)
   {
      if (pDev->nDevState & DS_FW_DLD)
      {
         err = rdReg((VINETIC_CHANNEL*)pDev, MRSRGPIO_REG);
      }
      if (err == IFX_SUCCESS)
      {
         MRSRGPIO.value = (MRSRGPIO.value & ~nMask) | nSet;
         if (pDev->nDevState & DS_FW_DLD)
         {
            err = wrReg((VINETIC_CHANNEL*)pDev,
                  MRSRGPIO_REG);
         }
      }
   }
   if ((err == IFX_SUCCESS) && (nMode & GPIO_INT_FALLING))
   {
      if (pDev->nDevState & DS_FW_DLD)
      {
         err = rdReg((VINETIC_CHANNEL*)pDev, MFSRGPIO_REG);
      }

      if (err == IFX_SUCCESS)
      {
         MFSRGPIO.value = (MFSRGPIO.value & ~nMask) | nSet;
         if (pDev->nDevState & DS_FW_DLD)
         {
            err = wrReg((VINETIC_CHANNEL*)pDev,
                  MFSRGPIO_REG);
         }
      }
   }
   return(err);
}

/**
  Configure the IO(s) according to passed settings
\param
  pCh - pointer to VINETIC channel structure
\param
  pCfg - pointer to GPIO configuration structure
\return
   IFX_SUCCESS if requested IO configuration was succesful, else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values are stored in init variables and will
  be written during initialization.
*/
IFX_LOCAL IFX_int32_t _IoConfig(VINETIC_CHANNEL *pCh, VINETIC_GPIO_CONFIG const *pCfg)
{
   IFX_int32_t  i, nCur;
   IFX_int32_t  err           = IFX_SUCCESS;
   IFX_uint16_t nGpio         = pCfg->nGpio;
   VINETIC_DEVICE *pDev = pCh->pParent;

   /* Do input/output configuration for each resource bit set in nGpio */
   for (i = 0; i < 8; i++)
   {
      if (err == IFX_ERROR)
      {
         break;
      }
      nCur = 1 << i;
      if ((nGpio & nCur) != 0)
      {
         if (pCfg->nMode & GPIO_MODE_OUTPUT )
         {
            /* Enable output /and/ disable according input */
            if (pDev->nDevState & DS_FW_DLD)
            {
               IOCTL1.value &= ~(0xFF00 & (nCur << 8));
               IOCTL1.value |= (0x00FF & nCur);
               err = wrReg(pCh, IOCTL1_REG);
            }
            else
            {
               pCh->initIOCTL1 &= ~(0xFF00 & (nCur << 8));
               pCh->initIOCTL1 |= (0x00FF & nCur);
            }
         }
         else if (pCfg->nMode & GPIO_MODE_INPUT )
         {
            /* Enable input /and/ disable according output */
            if (pDev->nDevState & DS_FW_DLD)
            {
               IOCTL1.value &= ~(0x00FF & nCur);
               IOCTL1.value |= (0xFF00 & (nCur << 8));
               err = wrReg(pCh, IOCTL1_REG);
            }
            else
            {
               pCh->initIOCTL1 &= ~(0x00FF & nCur);
               pCh->initIOCTL1 |= (0xFF00 & (nCur << 8));
            }
            if ((err == IFX_SUCCESS) && (pCfg->nMode & GPIO_MODE_INT))
            {
               /* Configure interrupt */
               err = _IoIntMask(pCh, 0, nCur, pCfg->nMode);
               if (err == IFX_SUCCESS)
               {
                  /* Configure data upstream persistence */
                  if (pDev->nDevState & DS_FW_DLD)
                  {
                     IOCTL2.value = (IOCTL2.value & 0xFFF0)
                                  | (pCfg->nMode & 0x000F);
                     err = wrReg(pCh, IOCTL2_REG);
                  }
                  else
                  {
                     pCh->initIOCTL2 = (pCh->initIOCTL2 & 0xFFF0)
                                     | (pCfg->nMode & 0x000F);
                  }
                  /* Register callback */
                  pCh->GpioRes.gpioCallback[i] = pCfg->callback;
               }
            }
         }
      }
   }
   return(err);
}

/**
  Read the IO input value(s) from the VINETIC channel
\param
  pCh - pointer to VINETIC channel structure
\param
  pGet - pointer where the read value shall be stored
\param
  nMask - only bits set to '1' will be returned
\return
   IFX_SUCCESS if requested GPIO was read succesful, IFX_ERROR if read failed or device
   not initialized.
\remarks
*/
IFX_LOCAL IFX_int32_t _IoGet(VINETIC_CHANNEL* pCh, IFX_uint16_t *pGet, IFX_uint16_t nMask)
{
   IFX_int32_t    err   = IFX_ERROR;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t pSr[4]        = {0};
   /* ToDo: This needs to be protected from concurrent access... */
   if (pDev->nDevState & DS_FW_DLD)
   {
      err = ScRead(pDev, (SC_RSR | pCh->nChannel), pSr, 4);
      *pGet = pSr[2] & nMask;
   }
   return(err);
}

/**
  Write the IO value(s) to the VINETIC channel
\param
  pCh - pointer to VINETIC channel structure
\param
  nSet - values to store
\param
  nMask - only bits set to '1' will be stored
\return
   IFX_SUCCESS if requested IO was written succesful, else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values are stored in init variables and will
  be written during initialization.
*/
IFX_LOCAL IFX_int32_t _IoSet(VINETIC_CHANNEL* pCh, IFX_uint16_t nSet, IFX_uint16_t nMask)
{
   IFX_int32_t err      = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   /* ToDo: This needs to be protected from concurrent access... */
   /* ToDo: Protection from writing other bits */
   if (pDev->nDevState & DS_FW_DLD)
   {
      IOCTL2.value = (IOCTL2.value & ~(nMask << 8)) | ((nSet & nMask) << 8);
      err = wrReg(pCh, IOCTL2_REG);
   }
   else
   {
      pCh->initIOCTL2 = (pCh->initIOCTL2 & ~(nMask << 8))
                      | ((nSet & nMask) << 8);
   }
   return(err);
}

/**
  Mask or unmask the GPIO interrupt
\param
  pCh - pointer to VINETIC channel structure
\param
  nSet - bitmask for masked interrupts (0 = unmasked, 1 = masked)
\param
  nMask - only bits set to '1' will be stored
\param
  nMode - mode according to \ref VINETIC_GPIO_MODE
\return
   IFX_SUCCESS if requested mask change was succesful, else IFX_ERROR
\remarks
  Registers are stored in shadow variables and only written in case the device
  is initialized - otherwise the values are stored in init variables and will
  be written during initialization.
*/
IFX_LOCAL IFX_int32_t _IoIntMask(VINETIC_CHANNEL* pCh, IFX_uint16_t nSet,
                                 IFX_uint16_t nMask, IFX_int32_t nMode)
{
   IFX_int32_t err      = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;

   if (nMode & GPIO_INT_RISING)
   {
      if (pDev->nDevState & DS_FW_DLD)
      {
         err = rdReg(pCh, MR_SRS1_REG);
      }
      if (err == IFX_SUCCESS)
      {
         if (pDev->nDevState & DS_FW_DLD)
         {
            MR_SRS1.value = (MR_SRS1.value & ~nMask) | nSet;
            err = wrReg(pCh, MR_SRS1_REG);
         }
         else
         {
            pCh->initMR_SRS1 = (pCh->initMR_SRS1 & ~nMask) | nSet;
         }
      }
   }
   if ((err == IFX_SUCCESS) && (nMode & GPIO_INT_FALLING))
   {
      if (pDev->nDevState & DS_FW_DLD)
      {
         err = rdReg(pCh, MF_SRS1_REG);
      }
      if (err == IFX_SUCCESS)
      {
         if (pDev->nDevState & DS_FW_DLD)
         {
            MF_SRS1.value = (MF_SRS1.value & ~nMask) | nSet;
            err = wrReg(pCh, MF_SRS1_REG);
         }
         else
         {
            pCh->initMF_SRS1 = (pCh->initMF_SRS1 & ~nMask) | nSet;
         }
      }
   }
   return(err);
}

/**
  Dummy callback function
\param
  nDev - device number
\param
  nCh - channel for which event occurred (0 = device, 1..n = channel + 1)
\param
  nEvt - mask of events occured
\return
  none
\remarks
  Used module internal for safety/debug reasons (installed to callback on
  unregistering GPIO resource)
*/
IFX_LOCAL void _GpioNull(int nDev, int nCh, unsigned short nEvt)
{
   TRACE(VINETIC, DBG_LEVEL_LOW, ("_GpioNull(%i, 0x%02x)\n\r", nDev, nEvt));
   printk("_GpioNull(%i, 0x%02x)\n\r", nDev, nEvt);
}

/* @} */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
  Interrupt dispatcher for GPIO or IO interrupts
\param
  devHandle - VINETIC device or channel handle
\param
  nMask - content of VINETIC interrupt register
\return
  none
\remarks
  Called from VINETIC interrupt service routine. Will dispatch to callbacks
  registered for the corresponding GPIO. Should be called only when a GPIO
  resource is registered for interrupt by design.
*/
IFX_void_t VINETIC_GpioIntDispatch(IFX_int32_t devHandle, IFX_uint16_t nMask)
{
//	printk("VINETIC_GpioIntDispatch\n");
   IFX_int32_t i;
   if (((VINETIC_DEVICE*)devHandle)->nChannel == 0)
   {
      /* Dispatch device specific GPIO interrupt */
      VINETIC_DEVICE* pDev = (VINETIC_DEVICE*)devHandle;
      //printk("%08x, %08x\n",pDev->GpioRes.nGlobalReserve,nMask);
      for (i = 0; i < 8; i++)
      {
         if (nMask & (1 << i) & pDev->GpioRes.nGlobalReserve)
         {
            TRACE(VINETIC, DBG_LEVEL_LOW,
               ("VINETIC %li, GPIO interrupt (mask = 0x%04x)\n\r", pDev->nDevNr,
               nMask));
            /* Call irq handler ... */
	  //  printk("GpioRes.gpioCallback 1 %02X %08X %i\n",nMask,pDev->GpioRes.gpioCallback[i],i);
            pDev->GpioRes.gpioCallback[i](pDev->nDevNr, 0, (nMask & (1 << i)));
         }
      }
   }
   else
   {
      /* Dispatch channel specific IO interrupt */
      VINETIC_CHANNEL* pCh = (VINETIC_CHANNEL*)devHandle;
      for (i = 0; i < 8; i++)
      {
         if (nMask & (1 << i) & pCh->GpioRes.nGlobalReserve)
         {
            TRACE(VINETIC, DBG_LEVEL_LOW,
               ("VINETIC channel %i, GPIO interrupt (mask = 0x%04x)\n\r",
               pCh->nChannel, nMask));
            /* Call irq handler ... */
	   // printk("GpioRes.gpioCallback 2 %02X %08X %i\n",nMask,pCh->GpioRes.gpioCallback[i],i);
            pCh->GpioRes.gpioCallback[i](pCh->pParent->nDevNr, pCh->nChannel,
               (nMask & (1 << i)));
         }
      }
   }
}

/** \addtogroup VIN_KERNEL_INTERFACE */
/* @{ */

/** \defgroup VinGpioKernel VINETIC GPIO Kernel API
\remarks
  The kernel mode supports using the interrupt capabilities and registering a
  callback. Both - device specific GPIO and channel specific IO pins - can be
  used with this single function interface. The calling software need to pass
  either the address of the device (for the GPIOs) or channel (for the IOs)
  structure on reservation.
  From this point the calling party uses the IO handle for subsequent
  operations.
*/

/* @{ */

/**
  Initialize the VINETIC IO or GPIO pin resource.
\param
   devHandle - handle to either VINETIC device or channel structure
\return
   IFX_SUCCESS
\remarks
*/
IFX_void_t VINETIC_GpioInit(IFX_int32_t devHandle)
{
   IFX_int32_t      i;
   VINETIC_GPIO_RES *pGpioRes;

   /* Get GPIO resource structure */
   if (((VINETIC_DEVICE*)devHandle)->nChannel == 0)
   {
      pGpioRes = &((VINETIC_DEVICE*)devHandle)->GpioRes;
   }
   else
   {
      pGpioRes = &(((VINETIC_CHANNEL*)devHandle)->GpioRes);
   }
   /* Init GPIO resource structure */
   for (i = 0; i < 8; i++)
   {
      pGpioRes->Gpio[i].devHandle   = devHandle;
      pGpioRes->Gpio[i].nReserve    = 0;
      pGpioRes->Gpio[i].pGpioRes    = NULL;
      pGpioRes->Gpio[i].err         = ERR_OK;
      pGpioRes->gpioCallback[i]     = _GpioNull;
   }
}

/**
  Reserve a VINETIC IO or GPIO pin resource.
\param
   devHandle - handle to either VINETIC device or channel structure
\param
   nGpio - mask for GPIOs to reserve (0 = free, 1 = reserve)
\return
   ioHandle if requested IO/GPIO is available, else IFX_ERROR
\remarks
*/
int VINETIC_GpioReserve(int devHandle, unsigned short nGpio)
{
   IFX_int32_t      i;
   VINETIC_GPIO_RES *pGpioRes;
   IFX_int32_t      err  = IFX_ERROR;
   /* Get GPIO resource structure */
   if (((VINETIC_DEVICE*)devHandle)->nChannel == 0)
   {
      pGpioRes = &((VINETIC_DEVICE*)devHandle)->GpioRes;
   }
   else
   {
      pGpioRes = &(((VINETIC_CHANNEL*)devHandle)->GpioRes);
   }
   /* Check whether GPIO resource is free */
   if ((pGpioRes->nGlobalReserve & nGpio) != 0)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("ERROR GPIO not free (request 0x%02x, reserved 0x%02x)\n\r",
         nGpio,  pGpioRes->nGlobalReserve));
      return(err);
   }
   /* Find free GPIO resource */
   for (i = 0; i < 8; i++)
   {
      if (pGpioRes->Gpio[i].nReserve == 0)
      {
         pGpioRes->Gpio[i].nReserve  = nGpio;
         pGpioRes->Gpio[i].pGpioRes  = pGpioRes;
         pGpioRes->nGlobalReserve   |= nGpio;
         /* Return IO handle in case of success */
         err = (IFX_int32_t)&pGpioRes->Gpio[i];
         TRACE(VINETIC, DBG_LEVEL_LOW,
            ("GPIO reserve (mask 0x%04x, handle 0x%08lx)\n\r", nGpio, err));
         break;
      }
   }
   return(err);
}

/**
  Release a VINETIC IO or GPIO pin resource
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\return
   IFX_SUCCESS if releasing targeted IO/GPIO is possible, else IFX_ERROR
\remarks
*/
int VINETIC_GpioRelease(int ioHandle)
{
//	printk("VINETIC_GpioRelease\n");
   IFX_int32_t      i;
   VINETIC_GPIO_RES *pGpioRes;
   IFX_int32_t      err     = IFX_ERROR;
   VINETIC_GPIO     *pGpio  =(VINETIC_GPIO*)ioHandle;

   /* Check whether GPIO handle is valid - sanity check in case user passes
      invalid ioHandle */
   if (((VINETIC_DEVICE*)(pGpio->devHandle))->nChannel == 0)
   {
      pGpioRes = &((VINETIC_DEVICE*)pGpio->devHandle)->GpioRes;
   }
   else
   {
      pGpioRes = &((VINETIC_CHANNEL*)pGpio->devHandle)->GpioRes;
   }
   for (i = 0; i < 8; i++)
   {
      /* Valid handle if ioHandle equals pointer to GPIO structure /and/
        structure contains reserved pins */
      if (((IFX_int32_t)(&pGpioRes->Gpio[i]) == ioHandle)
       && (pGpioRes->Gpio[i].nReserve != 0))
      {
         err = IFX_SUCCESS;
         break;
      }
   }

   if (err == IFX_SUCCESS)
   {
      /* Remove registered callback for freed GPIOs*/
      for (i = 0; i < 8; i++)
      {
         if (pGpio->nReserve & (1 << i))
         {
            pGpioRes->gpioCallback[i] = _GpioNull;
         }
      }

      TRACE(VINETIC, DBG_LEVEL_LOW,
         ("GPIO release (mask 0x%04x, handle 0x%08x)\n\r",
         pGpio->nReserve, ioHandle));

      /* Mark GPIO resources as "free" */
      pGpioRes->nGlobalReserve &= ~(pGpio->nReserve);
      pGpio->pGpioRes = NULL;
      pGpio->nReserve = 0;
      pGpio->err      = ERR_OK;
   }
   else
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("ERROR GPIO release (mask 0x%04x, handle 0x%08x)\n\r",
         pGpio->nReserve, ioHandle));
   }

   return(err);
}

/**
  Configure a VINETIC IO or GPIO pin
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
   pCfg - handle to the GPIO configuration structure
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be configured, else IFX_ERROR
\remarks
*/
int VINETIC_GpioConfig(int ioHandle, VINETIC_GPIO_CONFIG *pCfg)
{
   IFX_int32_t  err     = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  =(VINETIC_GPIO*)ioHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & pCfg->nGpio) != pCfg->nGpio)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         pCfg->nGpio,  pGpio->nReserve));
      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* Configure IO or GPIO depending on passed structure */
   if (((VINETIC_DEVICE*)(pGpio->devHandle))->nChannel == 0)
   {
      err = _GpioConfig((VINETIC_DEVICE*)(pGpio->devHandle), pCfg);
   }
   else
   {
      err = _IoConfig((VINETIC_CHANNEL*)(pGpio->devHandle), pCfg);
   }

   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("GPIO configure (mask 0x%04x, mode 0x%08x, callback 0x%08x, handle 0x%08x)\n\r",
         pCfg->nGpio, pCfg->nMode, (int)pCfg->callback, ioHandle));
   return(err);
}

/**
   Set the interrupt enable mask
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
  nSet - bitmask for interrupts to mask (0 = unmasked, 1 = masked)
\param
   nMask - mask to write to interrupt enable register
\param
  nMode - mode according to VINETIC_GPIO_MODE
\return
   IFX_SUCCESS if interrupt mask could be written, else IFX_ERROR
\remarks
*/
int VINETIC_GpioIntMask(int ioHandle, unsigned short nSet, unsigned short nMask,
                        unsigned int nMode)
{
   IFX_int32_t  err     = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  =(VINETIC_GPIO*)ioHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpio->nReserve));
      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* Modify interrupt mask for IO or GPIO depending on passed structure */
   if (((VINETIC_DEVICE*)(pGpio->devHandle))->nChannel == 0)
   {
      err = _GpioIntMask((VINETIC_DEVICE*)(pGpio->devHandle), nSet, nMask, nMode);
   }
   else
   {
      err = _IoIntMask((VINETIC_CHANNEL*)(pGpio->devHandle), nSet, nMask, nMode);
   }
   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("GPIO mask (set 0x%04x, mask 0x%08x, mode 0x%08x, handle 0x%08x)\n\r",
         nSet, nMask, nMode, ioHandle));
   return(err);
}

/**
  Set the value of a VINETIC IO or GPIO pin
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
  nSet - values to store
\param
  nMask - only bits set to '1' will be stored
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be written, else IFX_ERROR
\remarks
*/
int VINETIC_GpioSet(int ioHandle, unsigned short nSet, unsigned short nMask)
{
    IFX_int32_t err     = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  =(VINETIC_GPIO*)ioHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpio->nReserve));
      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* Set IO or GPIO value depending on passed structure */
   if (((VINETIC_DEVICE*)(pGpio->devHandle))->nChannel == 0)
   {
      err = _GpioSet((VINETIC_DEVICE*)(pGpio->devHandle), nSet, nMask);
   }
   else
   {
      err = _IoSet((VINETIC_CHANNEL*)(pGpio->devHandle), nSet, nMask);
   }
   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("GPIO set (set 0x%04x, mask 0x%04x, handle 0x%08x)\n\r",
      nSet, nMask, ioHandle));
   return(err);
}

/**
  Read the value from a VINETIC IO or GPIO pin
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
  nGet - pointer where the read value shall be stored
\param
  nMask - only bits set to '1' will be stored
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be read, else IFX_ERROR
\remarks
*/
int VINETIC_GpioGet(int ioHandle, unsigned short *nGet, unsigned short nMask)
{
   IFX_int32_t err      = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  =(VINETIC_GPIO*)ioHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpio->nReserve));
      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }
   /* Read IO or GPIO value depending on passed structure */
   if (((VINETIC_DEVICE*)(pGpio->devHandle))->nChannel == 0)
   {
      err = _GpioGet((VINETIC_DEVICE*)(pGpio->devHandle), nGet, nMask);
   }
   else
   {
      err = _IoGet((VINETIC_CHANNEL*)(pGpio->devHandle), nGet, nMask);
   }
   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("GPIO get (get 0x%04x, mask 0x%08x, handle 0x%08x)\n\r",
      *nGet, nMask, ioHandle));
   return(err);
}

/* @} */
/* @} */

/** \defgroup VinGpioUser VINETIC GPIO User API
\remarks
  The usermode interface provides simplified acces to the IO/GPIO resources. It
  only supports reserving, reading and setting an IO/GPIO. Using interrupts and
  callbacks is not permitted.
*/
/* @{ */

/*****************************************************************************
 * GPIO/IO user interface
 *****************************************************************************/

/**
  Release a VINETIC IO or GPIO pin resource from user mode
\param
   pDev - pointer to either VINETIC device or channel structure
\param
  pCtrl - pointer to user mode control structure
\return
   IFX_SUCCESS if requested IO/GPIO is available, else IFX_ERROR
\remarks
*/
IFX_int32_t VINETIC_GpioReserveUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t ret;
   IFX_int32_t devHandle = (IFX_int32_t)pDev;

   ret = VINETIC_GpioReserve(devHandle, pCtrl->nGpio);
   if (ret != IFX_ERROR)
   {
      pCtrl->ioHandle = ret;
      ret = IFX_SUCCESS;
   }
   else
   {
      VINETIC_GPIO* pGpio = (VINETIC_GPIO*)pCtrl->ioHandle;
      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }
   return(ret);
}

/**
  Release a VINETIC IO or GPIO pin resource from user mode
\param
  pDev - pointer to VINETIC device or channel structure
\param
  pCtrl - pointer to user mode control structure
\return
  IFX_SUCCESS if releasing targeted IO/GPIO is possible, else IFX_ERROR
\remarks
*/
IFX_int32_t VINETIC_GpioReleaseUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t err;
   err = VINETIC_GpioRelease(pCtrl->ioHandle);
   if (err != IFX_SUCCESS)
   {
      VINETIC_GPIO* pGpio = (VINETIC_GPIO*)pCtrl->ioHandle;
      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }
   return(err);
}

/**
  Configure a VINETIC IO or GPIO pin from user mode
\param
  pDev - pointer to VINETIC device or channel structure
\param
  pCtrl - pointer to user mode control structure
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be configured, else IFX_ERROR
\remarks
*/
IFX_int32_t VINETIC_GpioConfigUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t         i;
   VINETIC_GPIO_CONFIG pCfg;
   IFX_uint16_t        nInput  = 0;
   IFX_uint16_t        nOutput = 0;
   IFX_int32_t         err     = IFX_ERROR;

   /* Parse which GPIOs/IOs to configure as input or output */
   for (i = 0; i < 8; i++)
   {
      if (pCtrl->nGpio & (1 << i))
      {
         if (pCtrl->nMask & (1 << i))
         {
            /* Pin active and configured as output*/
            nOutput |= (1 << i);
         }
         else
         {
            /* Pin active and configured as input */
            nInput |= (1 << i);
         }
      }
   }
   if (nInput != 0)
   {
      /* Configure input pins */
      pCfg.nGpio = nInput;
      pCfg.nMode = GPIO_MODE_INPUT;
      err =  VINETIC_GpioConfig(pCtrl->ioHandle, &pCfg);
   }
   if (nOutput != 0)
   {
      /* Configure output pins */
      pCfg.nGpio = nOutput;
      pCfg.nMode = GPIO_MODE_OUTPUT;
      err =  VINETIC_GpioConfig(pCtrl->ioHandle, &pCfg);
   }
   if ((err != IFX_SUCCESS) && (((VINETIC_GPIO*)pCtrl->ioHandle)->err != ERR_OK))
   {
      VINETIC_GPIO* pGpio = (VINETIC_GPIO*)pCtrl->ioHandle;
      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }
   return(err);
}

/**
  Set VINETIC IO or GPIO pin resource from user mode
\param
  pDev - pointer to VINETIC device or channel structure
\param
  pCtrl - pointer to user mode control structure
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be written, else IFX_ERROR
\remarks
*/
IFX_int32_t VINETIC_GpioSetUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t err;

   err = VINETIC_GpioSet(pCtrl->ioHandle, pCtrl->nGpio, pCtrl->nMask);
   if (err != IFX_SUCCESS)
   {
      VINETIC_GPIO* pGpio = (VINETIC_GPIO*)pCtrl->ioHandle;
      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }
   return(err);
}

/**
  Read VINETIC IO or GPIO pin resource from user mode
\param
  pDev - pointer to VINETIC device or channel structure
\param
  pCtrl - pointer to user mode control structure
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be read, else IFX_ERROR
\remarks
*/
IFX_int32_t VINETIC_GpioGetUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t err;

   err = VINETIC_GpioGet(pCtrl->ioHandle, &pCtrl->nGpio, pCtrl->nMask);
   if (err != IFX_SUCCESS)
   {
      VINETIC_GPIO* pGpio = (VINETIC_GPIO*)pCtrl->ioHandle;
      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }
   return(err);
}
/* @} */
