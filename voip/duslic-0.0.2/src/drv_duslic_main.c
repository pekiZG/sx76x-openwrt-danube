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
#include "drv_duslic_main.h"
#include "drv_board.h"
#include "drv_duslic_dwnld.h"


 /* ============================= */
/* Global function definition    */
/* ============================= */
#ifdef DUSLIC_SPI
extern IFX_int32_t Board_SerialRead(struct _DUSLIC_DEVICE* pDev, 
                                    IFX_uint8_t* cmd,
                                    IFX_int32_t cmd_count, 
                                    IFX_uint8_t* pData, 
                                    IFX_int32_t count);

extern IFX_int32_t Board_SerialWrite(struct _DUSLIC_DEVICE* pDev, 
                                     IFX_uint8_t* pData, 
                                     IFX_int32_t count);
#endif


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
    int err = IFX_SUCCESS;

    pCh->nChState = PDRx;
    pCh->ciop(pCh, PDRx);

    /*initialize the gpio port structure*/
    DUSLIC_GpioInit((INT)pCh);

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
        pDev->pChannel[i].magic = DUSLIC_CHANNEL_MAGIC;
        pDev->pChannel[i].pParent = (DUSLIC_DEVICE*)pDev;
        pDev->pChannel[i].nChannel = (BYTE)i;
        if (initChMember(&pDev->pChannel[i]) == IFX_ERR)
            return IFX_ERR;
    }
   
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
/*    DUSLIC_DEVICE *pDev = pCh->pParent; */
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
     DUSLIC_IO_INIT init = {0};
      int err = IFX_SUCCESS;
      int i = 0;
  
     init.param = DUS_IO_INIT_DEFAULT;
 
     for (i = 0; i < DUSLIC_MAX_CHANNELS; i++)
     {
          if (err == IFX_SUCCESS)
              err = initChRegStruct (&pDev->pChannel[i]);
  
         /* download cram */
         if (pInit)
             Dwld_LoadCram (&pDev->pChannel[i], pInit);
         else
             Dwld_LoadCram (&pDev->pChannel[i], &init);
  
         /* set active mode */ 
         pDev->pChannel[i].ciop(&pDev->pChannel[i], ANY_ACTIVE);
  
     }
      return err;
}
 

/*******************************************************************************
Description:
  Config PCM property
Arguments:
   pCh      - pointer to the device structure
   pPcmConf - handle to the PCM structure
Return:
    SUCCESS or ERR
Remarks:
*******************************************************************************/
int DUSLIC_Set_Pcm(DUSLIC_CHANNEL *pCh, DUSLIC_PCM_CONFIG *pPcmConf)
{
    unsigned char reg = 0;

    if (pPcmConf == 0)
    {
        printk("Duslic Driver Error: no pcm configuration data. \n");
    }

    if (pPcmConf->nClockMode == DS_PCM_DOUBLE_CLOCK)
        reg |= 1 << 7;
    
    if (pPcmConf->nTxRxSlope & DS_PCM_TX_FALLING)
        reg |= 1 << 6;
    
    if (pPcmConf->nTxRxSlope & DS_PCM_RX_RISING)
        reg |= 1 << 5;
  
    if (pPcmConf->nDriveMode == DS_PCM_DRIVEN_FIRST_HALF)
        reg |= 1 << 4;
    
    switch (pPcmConf->nShift)
    {
    case DS_PCM_SHIFT_1T:
        reg |= 0x1;
        break;

    case DS_PCM_SHIFT_2T:
        reg |= 0x2;
        break;

    case DS_PCM_SHIFT_3T:
        reg |= 0x3;
        break;

    case DS_PCM_SHIFT_4T:
        reg |= 0x4;
        break;

    case DS_PCM_SHIFT_5T:
        reg |= 0x5;
        break;

    case DS_PCM_SHIFT_6T:
        reg |= 0x6;
        break;

    case DS_PCM_SHIFT_7T:
        reg |= 0x7;
        break;

    case DS_PCM_NO_SHIFT:
    default:
        break;
    }

    pCh->WriteReg(pCh, SOP_CMD, PCMC1, &reg, 1);

    reg = 0;
    pCh->ReadReg(pCh, SOP_CMD, BCR3, &reg, 1);
    reg &= 0x1f;
    
    switch (pPcmConf->nResolution)
    {
    case DS_PCM_U_LAW_8_BIT:
        reg |= 1 << 7;
        break;

    case DS_PCM_LINEAR_16_BIT:
        reg |= 1 << 6;
        break;

    case DS_PCM_A_LAW_8_BIT:
    default:
        break;
    }
    
    if (pPcmConf->nRate == DS_PCM_SAMPLE_16K)
        reg |= 1 << 5;

    pCh->WriteReg(pCh, SOP_CMD, BCR3, &reg, 1);

    reg = 0;

    if (pPcmConf->nTimeslotRX >= 0 && pPcmConf->nTimeslotRX < 128)
        reg = pPcmConf->nTimeslotRX;

    if (pPcmConf->nRxHighway == DS_PCM_HIGHWAY_B)
        reg |= 1 << 7;

    pCh->WriteReg(pCh, SOP_CMD, PCMR1, &reg, 1);

    reg = 0;

    if (pPcmConf->nTimeslotTX >= 0 && pPcmConf->nTimeslotTX < 128)
        reg = pPcmConf->nTimeslotTX;

    if (pPcmConf->nTxHighway == DS_PCM_HIGHWAY_B)
        reg |= 1 << 7;

    pCh->WriteReg(pCh, SOP_CMD, PCMX1, &reg, 1);

    return 0;
}


/*******************************************************************************
Description:
   Enable PCM channel
Arguments:
   pCh      - pointer to the device structure
   status   - enable or disable
Return:
    SUCCESS or ERR
Remarks:
   This function sets the initial board configuration, downloads
   PRAM/DRAM Firmware to duslic, and starts the EDSP.
*******************************************************************************/
int DUSLIC_Pcm_Active(DUSLIC_CHANNEL *pCh, int status)
{
    if (status)
    {
        pCh->ModifyReg(pCh, SOP_CMD, BCR3, 1 << 4, 1 << 4);

        pCh->ciop(pCh, ANY_ACTIVE);
    }
    else
    {
        pCh->ModifyReg(pCh, SOP_CMD, BCR3, 0, 1 << 4);
        pCh->ciop(pCh, PDRx);
    }

    return 0;
}

