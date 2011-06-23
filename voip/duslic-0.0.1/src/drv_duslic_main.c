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
*/

 /******************************************************************************
   Module      : drv_duslic_main.c
   Date        : 2005-07-11
   Description :
      Main driver operations as initializations called in open driver
   Remarks:
      Device member initialization
      General basic functions
      Os and board independent
 *****************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_duslic_api.h"
#include "drv_board.h"
#include "drv_duslic_main.h"
 /* ============================= */
/* Global function definition    */
/* ============================= */

/*****************************************************************************
Description :
   Os and board independent field initializations, called on open for the
   first time.
Arguments	:
   pDev     - pointer to DUSLIC device structure
Return:
   IFX_SUCCESS if ok, otherwise IFX_ERR
Remarks 		:
   Called by InitDevMember to reset all fields
*****************************************************************************/
int ResetDevMember(DUSLIC_DEVICE* pDev)
{
#if defined(DUSLIC_SPI)
   /* for SPI these function pointer are used */
   pDev->write = Board_SerialWrite;
   pDev->read  = Board_SerialRead;
#endif 

   return IFX_SUCCESS;
}

/*******************************************************************************
Description	:
   Updates all member of the device and all channel structures according
   to the flags
Arguments   :
   pDev     - pointer to DUSLIC device structure
   nFlags   - given operation flags for the device or channels, see CH_FLAGS
Remarks:
   Sets the default streaming function
*******************************************************************************/
void UpdateMember(DUSLIC_DEVICE* pDev, unsigned long nFlags)
{

   return;
}
/*****************************************************************************
Description :
   Os and board independent initializations for one channel of the parent
   device, called on channel first open
Arguments	:
   pCh      - pointer to DUSLIC channel structure
Return:
   IFX_SUCCESS if ok, otherwise IFX_ERR
Remarks 		:
   none
*****************************************************************************/
int ResetChMember(DUSLIC_CHANNEL* pCh)
{
   
   pCh->nChState=PDRx;
   pCh->ciop(pCh,PDRx);
   /*initialize the gpio port structure*/
   DUSLIC_GpioInit((INT)pCh);
   int err = IFX_SUCCESS;
   
   return err;
}

/*******************************************************************************
Description	:
   Os and board independent initializations, called on first device open
Arguments   :
   pCh      - pointer to DUSLIC channel structure
Return:
   SUCCESS if ok, otherwise ERR
*******************************************************************************/
static int initChMember(DUSLIC_CHANNEL* pCh)
{
   pCh->ReadReg  = ReadReg;
   pCh->WriteReg = WriteReg;
   pCh->ciop = ciop;
   pCh->ModifyReg = ModifyReg;
   return IFX_SUCCESS;
}

/*******************************************************************************
Description	:
   Os and board independent initializations, called only on first device open
Arguments   :
   pDev     - pointer to DUSLIC device structure
Return:
   IFX_SUCCESS if ok, otherwise IFX_ERR
Remarks:
   Called in open when the device memory was allocated. This happens only
   once, cause the device structure is cached.
   Another open would not lead to this function cause the memory is already
   available.
   This function allocates all neccessary memory and initializes the fields
   by calling ResetDevMember.
*******************************************************************************/
int InitDevMember(DUSLIC_DEVICE* pDev)
{
   int i;

   if (ResetDevMember(pDev) == ERR)
      return IFX_ERR;
    /* reset the driver states to default */
   
    /* initialize all channel structures */
   for (i = 0; i < DUSLIC_MAX_CHANNELS; i++)
   {
      pDev->pChannel[i].pParent = (DUSLIC_DEVICE*)pDev;
      pDev->pChannel[i].nChannel = (BYTE)i;
      if (initChMember(&pDev->pChannel[i]) == IFX_ERR)
         return IFX_ERR;
   }
   
   //UpdateMember(pDev, 0);

   return IFX_SUCCESS;
}




/****************************************************************************
Description:
   Initalize the shadow channel registers
   This functions has to be called after vinetic hw revision was detected
   in DUSLIC_Init.
Arguments:
   pCh  - handle to the channel structure
Remarks:
   none
****************************************************************************/
static int initChRegStruct (DUSLIC_CHANNEL *pCh)
{
   DUSLIC_DEVICE *pDev = pCh->pParent;
   int  err  = IFX_SUCCESS;
      

   DUSLIC_GpioInit((INT)pCh);
   return err;
}


/*******************************************************************************
Description:
  Initialize the DUSLIC chip
Arguments:
   pDev     - pointer to the device structure
   pCmd     - handle to the initialization structure
Return:
    SUCCESS or ERR
Remarks:
   This function sets the initial board configuration, downloads
   PRAM/DRAM Firmware to duslic, and starts the EDSP.
*******************************************************************************/
int DUSLIC_Init(DUSLIC_DEVICE *pDev, DUSLIC_IO_INIT *pInit)
{
   int err = IFX_SUCCESS;
   int i   = 0;
   for(i=0;i<DUSLIC_MAX_CHANNELS;i++)
   {
      if (err == IFX_SUCCESS)
      err = initChRegStruct (&pDev->pChannel[i]);
   }
   return err;
}

