/*
** Copyright (C) 2005 Wu Qi Ming <Qi-Ming.Wu@infineon.com>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


 ****************************************************************************
   Module      : $RCSfile: drv_duslic_gpio.c,v $
   Revision    : $Revision: 0.0.1 $
   Date        : $Date: 2005/7/13 03:50:47 $
****************************************************************************/
/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_duslic_api.h"
#include "drv_board.h"
#include "drv_duslic_main.h"
//#include "drv_duslic_gpio.h"


#ifdef USE_VINETIC_GPIO
#include "vinetic_io.h"
#endif

extern DUSLIC_DEVICE *pDuslic_Devices[DUSLIC_MAX_DEVICES];
/* ============================= */
/* Local function declaration    */
/* ============================= */
static INT _GpioConfig(DUSLIC_GPIO* Gpio, DUSLIC_GPIO_CONFIG *pCfg);
static INT _GpioSet(DUSLIC_GPIO* Gpio, UINT16 nSet, UINT16 nMask);
static INT _GpioGet(DUSLIC_GPIO* Gpio, u8* pGet, UINT16 nMask);

static INT _GpioIntMask(DUSLIC_GPIO* Gpio, UINT16 nSet, UINT16 nMask, INT nMode);


/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#define GPIO_SET_ERROR(no) \
   do { \
      pGpio->err = no; \
   } while(0)
            
   
   
   
/**
  Interrupt dispatcher for GPIO or IO interrupts
\param
  devHandle - DUSLIC device or channel handle
\param
  nMask - content of DUSLIC interrupt register
\return
  none
\remarks
  Called from DUSLIC interrupt service routine. Will dispatch to callbacks 
  registered for the corresponding GPIO. Should be called only when a GPIO
  resource is registered for interrupt by design.
*/
void DUSLIC_GpioIntDispatch(INT devHandle, IFX_uint8_t nMask)
{
  DUSLIC_CHANNEL* pCh=(DUSLIC_CHANNEL*)devHandle;
  int i;
/*
  printk("pCh->GpioRes.nGlobalReserve=%02x\n",pCh->GpioRes.nGlobalReserve);
  printk("pGpioRes->nGlobalReserve=%02x\n",pDuslic_Devices[0]->pChannel[0].GpioRes.nGlobalReserve);
  printk("&pCh=%08x\n",(INT)pCh);
  printk("&pDuslic_Devices[0]->pChannel[0]=%08x\n",(INT)&(pDuslic_Devices[0]->pChannel[0]));
*/
  for(i=0;i<4;i++)
  {
    if ( ((1 << i) & pCh->GpioRes.nGlobalReserve) && ((1 << i) & pCh->GpioIntFlag))
       { 
          /*
	  printk("IO%d now\n",i+1);
	  printk("nMask=%08x\n",nMask);
	  printk("pCh->GpioStatus=%08x\n",pCh->GpioStatus);
          */
	  if((nMask^(pCh->GpioStatus))&(1<<i))  
	 {
	    TRACE(DUSLIC, DBG_LEVEL_LOW, 
               ("DUSLIC %i, GPIO interrupt (mask = 0x%04x)\n\r", pDev->nDevNr, 
               nMask));
            /* Call irq handler ... */
	    pCh->GpioRes.Gpio[i].gpioCallback(((DUSLIC_DEVICE*)pCh->pParent)->nDevNr,\
	      pCh->nChannel, (nMask & (1 << i)));
         }
        }
  }

  return;
}   
   
/**
  Configure the GPIO(s) according to passed settings
\param
  pDev - pointer to DUSLIC device structure
\param
  pCfg - pointer to GPIO configuration structure
\return
  SUCCESS if requested GPIO configuration was succesful, else ERR
\remarks
  Registers are stored in shadow variables and only written in case the device 
  is initialized - otherwise the values will be written during initialization.
*/
static INT _GpioConfig(DUSLIC_GPIO* Gpio, DUSLIC_GPIO_CONFIG *pCfg)
{
   int err        = SUCCESS;

   DUSLIC_CHANNEL* pCh = (DUSLIC_CHANNEL*)Gpio->devHandle;
   //printk("nIO=%02x\n",Gpio->nIO);
    if(pCfg->nMode & GPIO_MODE_INPUT)
      {
      //printk("GPIO_MODE_INPUT!\n");
      /*write 1 to IOn-INEN and 0 to IOn-M, generate interrupt by default*/
      pCh->ModifyReg(pCh, SOP_CMD, IOCTL1, \
                  1<<(Gpio->nIO+4),1<<(Gpio->nIO+4)|(1<<Gpio->nIO));  
      }
      
    if(pCfg->nMode & GPIO_MODE_OUTPUT)
      {
      //printk("GPIO_MODE_OUTPUT!\n");
      /*write 1 to IOn-OEN and 0 to IOn-DD, drive 0 to the port by default*/
       pCh->ModifyReg(pCh, SOP_CMD, IOCTL2, \
                  1<<(Gpio->nIO+4),1<<(Gpio->nIO+4)|(1<<Gpio->nIO));  
      }
      
    if(pCfg->nMode & GPIO_MODE_INT)
      {
       //printk("GPIO_MODE_INT!\n"); 
       /*enable the interrupt from IOCTRL*/
       pCh->ModifyReg(pCh, SOP_CMD, IOCTL1, \
                  0,(1<<Gpio->nIO));  
       /*enable the interrupt from the MASK*/
       pCh->ModifyReg(pCh, SOP_CMD, MASK, 0,0x08);  
       pCh->GpioIntFlag |=1<<Gpio->nIO;
       Gpio->gpioCallback=pCfg->callback;
      }
      
    if(pCfg->nMode & GPIO_INT_RISING)
      {
       //printk("GPIO_INT_RISING!\n"); 
       pCh->GpioStatus &=~(1<<Gpio->nIO);
      }
      
    if(pCfg->nMode & GPIO_INT_FALLING)
      {
       //printk("GPIO_INT_FALLING!\n"); 
       pCh->GpioStatus |=1<<Gpio->nIO;     
      }
     
      
    if(pCfg->nMode<=GPIO_INT_DUP_605)
       {
         /*set the DUP value*/
         pCh->ModifyReg(pCh, SOP_CMD, IOCTL3, \
                pCfg->nMode<<(Gpio->nIO+4),0xf0); 
       
       }
    
   return(err);
}
   
/**
  Write the GPIO value(s) to the DUSLIC device
\param
  pDev - pointer to DUSLIC device structure
\param
  nSet - values to store
\param
  nMask - only bits set to '1' will be stored
\return
   SUCCESS if requested GPIO was written succesful or stored to shadow variable,
   else ERR
\remarks
  Registers are stored in shadow variables and only written in case the device 
  is initialized - otherwise the values will be written during initialization.
*/
INT _GpioSet(DUSLIC_GPIO* Gpio, UINT16 nSet, UINT16 nMask)
{
   int err = IFX_SUCCESS; 
   DUSLIC_CHANNEL* pCh = (DUSLIC_CHANNEL*)Gpio->devHandle;
   pCh->ModifyReg(pCh, SOP_CMD, IOCTL2, \
                  nSet,nMask);
   return err;
}      

/**
  Read the GPIO input value(s) from the DUSLIC device
\param
  pDev - pointer to DUSLIC device structure
\param
  pGet - pointer where the read value shall be stored
\param
  nMask - only bits set to '1' will be valid on return
\return
   SUCCESS if requested GPIO was read succesful, ERR if read failed or device
   not initialized.
\remarks
*/
static INT _GpioGet(DUSLIC_GPIO* Gpio, u8* pGet, UINT16 nMask)
{
   int err = IFX_SUCCESS; 
   DUSLIC_CHANNEL* pCh = (DUSLIC_CHANNEL*)Gpio->devHandle;
   pCh->ReadReg(pCh, SOP_CMD, IOCTL1, pGet, 1);
   *pGet &=nMask;
   return err;
}   
   

/**
  Mask or unmask the GPIO interrupt
\param
  pDev - pointer to DUSLIC device structure
\param
  nSet - bitmask for interrupts to mask (0 = unmasked, 1 = masked)
\param
  nMask - only bits set to '1' will be stored
\param
  nMode - mode according to DUSLIC_GPIO_MODE
\return
   SUCCESS if requested mask change was succesful, else ERR
\remarks
  Registers are stored in shadow variables and only written in case the device 
  is initialized - otherwise the values will be written during initialization.
*/
static INT _GpioIntMask(DUSLIC_GPIO* Gpio, UINT16 nSet, UINT16 nMask, INT nMode)
{
   INT err = IFX_SUCCESS;
   DUSLIC_CHANNEL* pCh = (DUSLIC_CHANNEL*)Gpio->devHandle;
   pCh->ModifyReg(pCh, SOP_CMD, INTREG2, nSet, nMask);
   switch (nMode)
   {
    case GPIO_MODE_INPUT:
      /*write 1 to IOn-INEN and 0 to IOn-M, generate interrupt by default*/
      pCh->ModifyReg(pCh, SOP_CMD, IOCTL1, \
                  0x0f,nMask<<4);  
      break;
    case GPIO_MODE_OUTPUT:
      /*write 1 to IOn-OEN and 0 to IOn-DD, drive 0 to the port by default*/
      pCh->ModifyReg(pCh, SOP_CMD, IOCTL2, \
                  0x0f,nMask<<4);  
      break;
      
    case GPIO_MODE_INT:
    case GPIO_INT_RISING:
    case GPIO_INT_FALLING:
    default:
      if(nMode<=GPIO_INT_DUP_605)
       {
         /*set the DUP value*/
         pCh->ModifyReg(pCh, SOP_CMD, IOCTL3, \
                nMode<<4,0xf0); 
       
       }
      break; 
   }
   return err;
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
static void _GpioNull(INT nDev, INT nCh, UINT16 nEvt)
{
   TRACE(DUSLIC, DBG_LEVEL_LOW, ("_GpioNull(%i, 0x%02x)\n\r", nDev, nEvt));
  // printk("GPIO NULL!\n");
}


/**
  Initialize the DUSLIC IO or GPIO pin resource.
\param
   devHandle - handle to either DUSLIC device or channel structure
\return
   SUCCESS
\remarks
*/
void DUSLIC_GpioInit(INT devHandle)
{
   INT i;
   DUSLIC_GPIO_RES *pGpioRes;
   DUSLIC_CHANNEL *pCh=(DUSLIC_CHANNEL*)devHandle;
   pGpioRes = &(((DUSLIC_CHANNEL*)devHandle)->GpioRes);
   pGpioRes->nGlobalReserve=0;
   /* Init GPIO resource structure */
   for (i = 0; i < 5; i++)
   {
      pGpioRes->Gpio[i].devHandle   = devHandle;
      pGpioRes->Gpio[i].nReserve    = 0;
      pGpioRes->Gpio[i].err         = ERR_OK;
      pGpioRes->Gpio[i].gpioCallback = _GpioNull;
      pGpioRes->Gpio[i].nIO          = i;
   }
   pCh->GpioIntFlag=0;
}

/**
  Reserve a DUSLIC IO or GPIO pin resource.
\param
   devHandle - handle to either DUSLIC device or channel structure
\param
   nGpio - mask for GPIOs to reserve (0 = free, 1 = reserve)
\return
   ioHandle if requested IO/GPIO is available, else ERR
\remarks
*/
INT DUSLIC_GpioReserve(INT devHandle, UINT16 nGpio)
{
   INT i,err=IFX_SUCCESS;
   DUSLIC_GPIO_RES *pGpioRes;
   pGpioRes = &(((DUSLIC_CHANNEL*)devHandle)->GpioRes);
   if ((pGpioRes->nGlobalReserve & nGpio) != 0)
   {
      TRACE(DUSLIC, DBG_LEVEL_HIGH,
         ("ERROR GPIO not free (request 0x%02x, reserved 0x%02x)\n\r",
         nGpio,  pGpioRes->nGlobalReserve));
      return(IFX_ERR);
   }
   
   for(i=0;i<5;i++)
   {
        
    if (pGpioRes->Gpio[i].nReserve == 0 && (nGpio&(1<<i)))
      {
         //printk("gpio address=0x%08x\n",(INT)(pGpioRes->Gpio)); 
         pGpioRes->Gpio[i].nReserve  = 1;
         pGpioRes->nGlobalReserve   |= 1<<i;
         /* Return IO handle in case of success */
      
         TRACE(DUSLIC, DBG_LEVEL_LOW, 
            ("GPIO reserve (mask 0x%04x, handle 0x%08x)\n\r", nGpio, err));
         
      }
   else
      {
         err=IFX_ERR;
         break; 
      }
      /* Return IO handle in case of success */
      //printk("&pGpioRes->nGlobalReserve=%08x\n",&(pGpioRes->nGlobalReserve));
      //printk("pGpioRes->nGlobalReserve=%02x\n",pGpioRes->nGlobalReserve);
      //printk("pGpioRes->nGlobalReserve=%02x\n",pDuslic_Devices[0]->pChannel[0].GpioRes.nGlobalReserve);
    
      err = (INT)pGpioRes;
   }
   return(err);
}


/**
  Release a DUSLIC IO or GPIO pin resource
\param
   ioHandle - handle returned by DUSLIC_GpioReserve
\return
   IFX_SUCCESS if releasing targeted IO/GPIO is possible, else IFX_ERR
\remarks
*/
INT DUSLIC_GpioRelease(INT ioHandle)
{
   INT i;
   DUSLIC_GPIO_RES *pGpioRes = (DUSLIC_GPIO_RES*)ioHandle;
   INT err              = IFX_ERR;
     
   for(i=0;i<5;i++){
   if (pGpioRes->nGlobalReserve & (1<<i) ) 
   {
      pGpioRes->Gpio[i].gpioCallback = _GpioNull;
      pGpioRes->Gpio[i].nReserve = 0;
      TRACE(DUSLIC, DBG_LEVEL_LOW, 
         ("GPIO release (mask 0x%04x, handle 0x%08x)\n\r", 
         pGpio->nReserve, ioHandle));

      /* Mark GPIO resources as "free" */
      pGpioRes->nGlobalReserve &= ~(1<<i);
      err      = ERR_OK;
   }
   else
   {
      TRACE(DUSLIC, DBG_LEVEL_HIGH, 
         ("ERROR GPIO release (mask 0x%04x, handle 0x%08x)\n\r", 
         pGpio->nReserve, ioHandle));
      
   }
   }
   return(err);
}


/**
  Configure a DUSLIC IO or GPIO pin
\param
   ioHandle - handle returned by DUSLIC_GpioReserve
\param
   pCfg - handle to the GPIO configuration structure
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be configured, else IFX_ERR
\remarks
*/
INT DUSLIC_GpioConfig(INT ioHandle, DUSLIC_GPIO_CONFIG *pCfg)
{
   int err              = IFX_SUCCESS;
   DUSLIC_GPIO_RES *pGpioRes  =(DUSLIC_GPIO_RES*)ioHandle;
   int i;
   
   for(i=0; i<5; i++)
      {
        if(pCfg->nGpio & (1<<i))
        {  
	  err = _GpioConfig(&(pGpioRes->Gpio[i]), pCfg);
        }
      }
   return(err);
}

/**
  Set the value of a DUSLIC IO or GPIO pin
\param
   ioHandle - handle returned by DUSLIC_GpioReserve
\param
  nSet - values to store
\param
  nMask - only bits set to '1' will be stored
\return
   SUCCESS if requested IO/GPIO is available and could be written, else ERR
\remarks
*/
INT DUSLIC_GpioSet(INT ioHandle, UINT16 nSet, UINT16 nMask)
{
    int err             = SUCCESS;
    DUSLIC_GPIO_RES *pGpioRes  =(DUSLIC_GPIO_RES*)ioHandle;

   /* Check whether GPIO resource is owned by module */
   if ((pGpioRes->nGlobalReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpioRes->nGlobalReserve));
      err = IFX_ERR;
      return ERR;
   }
   
   err = _GpioSet(&pGpioRes->Gpio[0], nSet, nMask);
   return(err);
}

/**
  Read the value from a DUSLIC IO or GPIO pin
\param
   ioHandle - handle returned by DUSLIC_GpioReserve
\param
  nGet - pointer where the read value shall be stored
\param
  nMask - only bits set to '1' will be stored
\return
   IFX_SUCCESS if requested IO/GPIO is available and could be read, else IFX_ERR
\remarks
*/
INT DUSLIC_GpioGet(INT ioHandle, UINT16 *nGet, UINT16 nMask)
{
   int err             = IFX_SUCCESS;
   
   DUSLIC_GPIO_RES *pGpioRes  =(DUSLIC_GPIO_RES*)ioHandle;
   if ((pGpioRes->nGlobalReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpioRes->nGlobalReserve));
      err = IFX_ERR;
      return ERR;
   }
   
   
   err = _GpioGet(&pGpioRes->Gpio[0], (u8*)nGet, nMask);
    
   return(err);

}

/**
   Set the interrupt enable mask
\param
   ioHandle - handle returned by DUSLIC_GpioReserve
\param
  nSet - bitmask for interrupts to mask (0 = unmasked, 1 = masked)
\param
   nMask - mask to write to interrupt enable register
\param
  nMode - mode according to DUSLIC_GPIO_MODE
\return
   IFX_SUCCESS if interrupt mask could be written, else ERR
\remarks
*/
INT DUSLIC_GpioIntMask(INT ioHandle, UINT16 nSet, UINT16 nMask, UINT32 nMode)
{
   int err              = SUCCESS;
   int i;
   DUSLIC_GPIO_RES *pGpioRes  =(DUSLIC_GPIO_RES*)ioHandle;
   if ((pGpioRes->nGlobalReserve & nMask) != nMask)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
         ("ERROR GPIO not owned (request 0x%04x, reserved 0x%04x)\n\r",
         nMask,  pGpioRes->nGlobalReserve));
      err = IFX_ERR;
      return ERR;
   }
   
   for(i=0; i<5; i++)
      {
        if(nMask & (1<<i))
           err = _GpioIntMask(&pGpioRes->Gpio[i], nSet, nMask, nMode);
      }
   return(err);
    
}


