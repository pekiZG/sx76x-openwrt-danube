/*       Copyright (c) 2004, Infineon Technologies.  All rights reserved.

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
   Module      : $RCSfile: drv_vinetic_gpio.c,v $
   Revision    : $Revision: 1.1.2.13 $
   Date        : $Date: 2004/09/24 07:57:46 $
*/
/**
   \file drv_vinetic_gpio.c VINETIC GPIO resource managment module.
   \remarks
  This module provides resource managment for the VINETIC device GPIO pins.
  A usermode application or other kernel mode drivers may reserve GPIO pins
  for exclusive use. Every access to the VINETIC GPIO resources has to be done
  via this module (direct access using VINETIC messages must be avoided!)./
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
IFX_LOCAL IFX_int32_t _GpioConfig(VINETIC_DEVICE *pDev, VINETIC_GPIO_CONFIG *pCfg);
IFX_LOCAL IFX_int32_t _GpioGet(VINETIC_DEVICE *pDev, IFX_uint16_t *pGet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _GpioSet(VINETIC_DEVICE *pDev, IFX_uint16_t nSet, IFX_uint16_t nMask);
IFX_LOCAL IFX_int32_t _GpioIntMask(VINETIC_DEVICE *pDev, IFX_uint16_t nSet, IFX_uint16_t nMask, IFX_int32_t nMode);

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
*/
IFX_LOCAL IFX_int32_t _GpioConfig(VINETIC_DEVICE *pDev, VINETIC_GPIO_CONFIG *pCfg)
{
   IFX_int32_t    err = IFX_SUCCESS;
   IFX_int32_t    i;
   IFX_uint16_t   nReg;
   IFX_uint16_t   nGpio = pCfg->nGpio;

   REG_READ_PROT(pDev, V2CPE_IO_CFG, &nReg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   if (pCfg->nMode & GPIO_MODE_OUTPUT)
   {
      nReg = (nReg & ~nGpio) | 0xFF00;

      REG_WRITE_PROT(pDev, V2CPE_IO_CFG, nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
   }
   else if (pCfg->nMode & GPIO_MODE_INPUT)
   {
      nReg |= nGpio | 0xFF00;

      REG_WRITE_PROT(pDev, V2CPE_IO_CFG, nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);

      if (pCfg->nMode & GPIO_MODE_INT)
      {
         /* Configure GPIO interrupts, according to the requested mode */
         err = _GpioIntMask(pDev, 0, nGpio, pCfg->nMode);

         for (i = 0; i < 8; i++)
         {
            /* Register interrupt routine for the bits affected */
            if (nGpio & (1 << i))
            {
               pDev->GpioRes.gpioCallback[i] = pCfg->callback;
            }
         }
      }
   }

   return err;
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
  IFX_SUCCESS if requested GPIOs were succesfully read, IFX_ERROR if read failed
\remarks
*/
IFX_LOCAL IFX_int32_t _GpioGet(VINETIC_DEVICE *pDev, IFX_uint16_t *pGet, IFX_uint16_t nMask)
{
   IFX_int32_t err = IFX_SUCCESS;

   REG_READ_PROT(pDev, V2CPE_IO_IN, pGet);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   *pGet &= nMask;

   return err;
}

/**
  Write the GPIO value(s) to the VINETIC device
\param
  pDev - pointer to VINETIC device structure
\param
  nSet - new bit values
\param
  nMask - only bits set to '1' will be modified
\return
  IFX_SUCCESS if requested GPIOs were written succesfully, else IFX_ERROR
\remarks
*/
IFX_LOCAL IFX_int32_t _GpioSet(VINETIC_DEVICE *pDev, IFX_uint16_t nSet,
                               IFX_uint16_t nMask)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t nReg;

   nReg = pDev->hostDev.GpioOut; /* get cached IO_OUT register value */

   nReg = (nReg & ~nMask) | (nSet & nMask);
   REG_WRITE_PROT(pDev, V2CPE_IO_OUT, nReg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   pDev->hostDev.GpioOut = nReg; /* cache written IO_OUT register, upon success */

   return err;
}

/**
  Mask or unmask the GPIO interrupt
\param
  pDev - pointer to VINETIC device structure
\param
  nSet - bitmask for interrupts to mask (0 = unmasked, 1 = masked)
\param
  nMask - only bits set to '1' will be affected
\param
  nMode - mode according to VINETIC_GPIO_MODE
\return
   IFX_SUCCESS if requested mask change was succesful, else IFX_ERROR
\remarks
*/
IFX_LOCAL IFX_int32_t _GpioIntMask(VINETIC_DEVICE *pDev, IFX_uint16_t nSet,
                                   IFX_uint16_t nMask, IFX_int32_t nMode)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t nReg;

   if (nMode & GPIO_INT_RISING)
   {
      REG_READ_PROT(pDev, V2CPE_IO_INTR, &nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      nReg = ((nReg & ~nMask) | (nMask & ~nSet)) & 0x00FF;
      REG_WRITE_PROT(pDev, V2CPE_IO_INTR, nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
   }
   if (nMode & GPIO_INT_FALLING)
   {
      REG_READ_PROT(pDev, V2CPE_IO_INTF, &nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      nReg = ((nReg & ~nMask) | (nMask & ~nSet)) & 0x00FF;
      REG_WRITE_PROT(pDev, V2CPE_IO_INTF, nReg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
   }

   return err;
}

/**
  Dummy callback function
\param
  nDev - device number
\param
  nCh - channel for which event occurred
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
   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("VIN%d%d: GPIO: _GpioNull(0x%02x)\n\r",nDev, nCh, nEvt));
}

/* @} */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
  Interrupt dispatcher for GPIO interrupts
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
   IFX_int32_t i;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)devHandle;

   for (i = 0; i < 8; i++)
   {
      if (nMask & (1 << i) & pDev->GpioRes.nGlobalReserve)
      {
/*         TRACE(VINETIC, DBG_LEVEL_LOW,
            ("VIN%ld: GPIO: interrupt occured (bit 0x%04x)\n\r",
            pDev->nDevNr, nMask));
*/
         /* dispatch irq handler */
         pDev->GpioRes.gpioCallback[i](pDev->nDevNr, 0, 1 << i);
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
  Initialize the VINETIC GPIO pin resources.
\param
   devHandle - handle to VINETIC device structure
\return
   IFX_SUCCESS
\remarks
*/
IFX_void_t VINETIC_GpioInit(IFX_int32_t devHandle)
{
   IFX_int32_t      i;
   VINETIC_GPIO_RES *pGpioRes;

   /* Get GPIO resource structure */
   if (((VINETIC_DEVICE *)devHandle)->nChannel == 0)
   {
      pGpioRes = &((VINETIC_DEVICE *)devHandle)->GpioRes;
   }
   else
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error initialising: channel GPIOs not available\n\r",
         ((VINETIC_CHANNEL *)devHandle)->pParent->nDevNr));
      return;
   }
   /* reset reservations */
   pGpioRes->nGlobalReserve = 0;
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
  Reserve a VINETIC GPIO pin resource.
\param
   devHandle - handle to VINETIC device structure
\param
   nGpio - mask for GPIOs to reserve (0 = free, 1 = reserve)
\return
   ioHandle if requested GPIO is available, else IFX_ERROR
\remarks
*/
int VINETIC_GpioReserve(int devHandle, unsigned short nGpio)
{
   IFX_int32_t i, err = IFX_ERROR;
   VINETIC_GPIO_RES *pGpioRes;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)devHandle;

   /* Get GPIO resource structure */
   if (((VINETIC_DEVICE *)devHandle)->nChannel == 0)
   {
      pDev = (VINETIC_DEVICE *)devHandle;
      pGpioRes = &pDev->GpioRes;
   }
   else
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error reserving: channel GPIOs not available\n\r",
         pDev->nDevNr));

      return err;
   }

   /* Check whether the GPIO pins are free */
   if ((pGpioRes->nGlobalReserve & nGpio) != 0)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error reserving: requested pins not free,"
         " (request 0x%02x, reserved 0x%02x)\n\r",
         pDev->nDevNr, nGpio, pGpioRes->nGlobalReserve));
      return err;
   }
   /* Find free GPIO resource */
   for (i = 0; i < 8; i++)
   {
      if (pGpioRes->Gpio[i].nReserve == 0)
      {
         pGpioRes->Gpio[i].nReserve = nGpio;    /* bits reserved at this instance */
         pGpioRes->Gpio[i].pGpioRes = pGpioRes; /* back pointer to device GPIO container */
         pGpioRes->nGlobalReserve   |= nGpio;   /* bitmask of all GPIO pins reserved */
	
         /* Return IO handle in case of success */
         err = (IFX_int32_t)&pGpioRes->Gpio[i];

/*         TRACE(VINETIC, DBG_LEVEL_LOW,
            ("VIN%ld: GPIO: success reserving: pin(s) 0x%x\n\r",
            pDev->nDevNr, nGpio));
	    */
         break;
      }
   }

   return err;
}

/**
  Release a VINETIC GPIO pin resource.
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\return
   IFX_SUCCESS if releasing targeted GPIO is possible, else IFX_ERROR
\remarks
*/
int VINETIC_GpioRelease(int ioHandle)
{
   IFX_int32_t       err = IFX_ERROR;
   IFX_uint8_t       i;
   VINETIC_GPIO_RES  *pGpioRes;
   VINETIC_GPIO      *pGpio = (VINETIC_GPIO *)ioHandle;
   VINETIC_DEVICE    *pDev = (VINETIC_DEVICE *)pGpio->devHandle;

   int released_entry = -1;
   pGpioRes = pGpio->pGpioRes;
   for (i = 0; i < 8; i++)
   {
	   if (&pGpioRes->Gpio[i] == pGpio)
	      released_entry = i;

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
      /* Remove registered callback for freed GPIOs */
      for (i = 0; i < 8; i++)
      {
         if (pGpio->nReserve & (1 << i))
         {
		 pGpioRes->gpioCallback[i] = _GpioNull;

         }
      }

/*      TRACE(VINETIC, DBG_LEVEL_LOW,
         ("VIN%ld: GPIO: success releasing: (pins %02x)\n\r",
          pDev->nDevNr, pGpio->nReserve));
*/
      /* Mark GPIO resources as "free" */
      pGpioRes->nGlobalReserve &= ~pGpio->nReserve;
      pGpio->pGpioRes = NULL;
      pGpio->nReserve = 0;
      pGpio->err      = ERR_OK;
   }
   else
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error releasing: (mask 0x%04x, handle 0x%08x)\n\r",
         pDev->nDevNr, pGpio->nReserve, ioHandle));
   }

   return err;
}

/**
  Configure requested VINETIC GPIO pins
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
   pCfg - handle to the GPIO configuration structure
\return
   IFX_SUCCESS if requested GPIO is available and could be configured, else IFX_ERROR
\remarks
*/
int VINETIC_GpioConfig(int ioHandle, VINETIC_GPIO_CONFIG *pCfg)
{
   IFX_int32_t  err     = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  = (VINETIC_GPIO *)ioHandle;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pGpio->devHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & pCfg->nGpio) != pCfg->nGpio)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error congifuring, not owned by module,"
          " (requested 0x%04x, reserved 0x%04x)\n\r",
         pDev->nDevNr, pCfg->nGpio,  pGpio->nReserve));

      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }

   err = _GpioConfig(pDev, pCfg);

/*   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("VIN%ld: GPIO: %s configuring, (mask 0x%04x, mode 0x%08x, callback %p, "
       "handle 0x%08x)\n\r", pDev->nDevNr, !err ? "success" : "error",
       pCfg->nGpio, pCfg->nMode, pCfg->callback, ioHandle));
*/
   return err;
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
   IFX_SUCCESS if interrupt mask could be set, else IFX_ERROR
\remarks
*/
int VINETIC_GpioIntMask(int ioHandle, unsigned short nSet, unsigned short nMask,
                        unsigned int nMode)
{
   IFX_int32_t  err     = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  = (VINETIC_GPIO *)ioHandle;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pGpio->devHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error setting interrupt mask, not owned by module,"
          " (requsted %04x, reserved %04x)\n\r",
         pDev->nDevNr, nMask,  pGpio->nReserve));

      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }

   err = _GpioIntMask((VINETIC_DEVICE*)(pGpio->devHandle), nSet, nMask, nMode);

/*   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("VIN%ld: GPIO: %s setting interrupt mask,"
      " (set 0x%04x, mask 0x%08x, mode 0x%08x, handle 0x%08x)\n\r",
      pDev->nDevNr, !err ? "success" : "error", nSet, nMask, nMode, ioHandle));
*/
   return err;
}

/**
 Set values of VINETIC GPIO pins
\param
   ioHandle - handle returned by VINETIC_GpioReserve
\param
  nSet - values to write for bits masked by 'nMask'
\param
  nMask - only bits masked with '1' will be modified
\return
  IFX_SUCCESS if requested IO/GPIO is available and could be written, else IFX_ERROR
\remarks
*/
int VINETIC_GpioSet(int ioHandle, unsigned short nSet, unsigned short nMask)
{
   IFX_int32_t err = IFX_SUCCESS;
   VINETIC_GPIO *pGpio = (VINETIC_GPIO *)ioHandle;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pGpio->devHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error setting bits, not owned by module, "
          "(requsted %04x, reserved %04x)\n\r",
         pDev->nDevNr, nMask,  pGpio->nReserve));

      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }

   err = _GpioSet(pDev, nSet, nMask);

/*   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("VIN%ld: GPIO: %s setting bits, (set %04x, mask %04x, handle %08x)\n\r",
      pDev->nDevNr, !err ? "success" : "error", nSet, nMask, ioHandle));
*/
   return err;
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
  IFX_SUCCESS if requested GPIO could successfully be read, else IFX_ERROR
\remarks
*/
int VINETIC_GpioGet(int ioHandle, unsigned short *nGet, unsigned short nMask)
{
   IFX_int32_t err = IFX_SUCCESS;
   VINETIC_GPIO *pGpio  =(VINETIC_GPIO *)ioHandle;
   VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pGpio->devHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpio->nReserve & nMask) != nMask)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
         ("VIN%ld: GPIO: error getting bits,"
         " not owned by module (requsted %04x, reserved %04x)\n\r",
         pDev->nDevNr, nMask,  pGpio->nReserve));

      GPIO_SET_ERROR(ERR_NORESOURCE);
      return IFX_ERROR;
   }

   err = _GpioGet(pDev, nGet, nMask);

/*   TRACE(VINETIC, DBG_LEVEL_LOW,
      ("VIN%ld: GPIO: %s getting bits, (get %04x, mask %04x, handle %08x)\n\r",
      pDev->nDevNr, !err ? "success" : "error", *nGet, nMask, ioHandle));
*/
   return err;
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
   Only one GPIO can be reserved per call to this function i.e.
   call to FIO_VINETIC_GPIO_RESERVE ioctl command.
*/
IFX_int32_t VINETIC_GpioReserveUser(VINETIC_DEVICE *pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
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
      SET_ERROR(ERR_NORESOURCE);
   }

   return ret;
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
IFX_int32_t VINETIC_GpioReleaseUser(VINETIC_DEVICE *pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t err;

   err = VINETIC_GpioRelease(pCtrl->ioHandle);
   if (err != IFX_SUCCESS)
   {
      VINETIC_GPIO *pGpio = (VINETIC_GPIO *)pCtrl->ioHandle;

      SET_ERROR(pGpio->err);  /* report the GPIO error as device error */
      GPIO_SET_ERROR(ERR_OK); /* reset the GPIO error */
   }

   return err;
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
   A GPIO pin can be configured using this function either as input, or output.
   Configuring a GPIO pin as interrupt triggering input, can only be done by calling
   function VINETIC_GpioConfig(), from kernel space.
*/
IFX_int32_t VINETIC_GpioConfigUser(VINETIC_DEVICE *pDev, VINETIC_IO_GPIO_CONTROL *pCtrl)
{
   IFX_int32_t          err = IFX_ERROR;
   IFX_int32_t          i;
   VINETIC_GPIO_CONFIG  pCfg;
   IFX_uint16_t         nInput  = 0;
   IFX_uint16_t         nOutput = 0;

   /* Parse which GPIOs to configure as input or output */
   for (i = 0; i < 8; i++)
   {
      if (pCtrl->nGpio & (1 << i))
      {
         if (pCtrl->nMask & (1 << i))
         {
            /* Pin active and configured as output */
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

      err = VINETIC_GpioConfig(pCtrl->ioHandle, &pCfg);
   }
   if (nOutput != 0)
   {
      /* Configure output pins */
      pCfg.nGpio = nOutput;
      pCfg.nMode = GPIO_MODE_OUTPUT;

      err = VINETIC_GpioConfig(pCtrl->ioHandle, &pCfg);
   }
   if ((err != IFX_SUCCESS) && (((VINETIC_GPIO*)pCtrl->ioHandle)->err != ERR_OK))
   {
      VINETIC_GPIO *pGpio = (VINETIC_GPIO *)pCtrl->ioHandle;

      SET_ERROR(pGpio->err);
      GPIO_SET_ERROR(ERR_OK);
   }

   return err;
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

   return err;
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

   return err;
}
/* @} */
