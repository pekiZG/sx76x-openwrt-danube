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
   Module      : drv_duslic_api.c
   Date        : 2002-09-18
 This file contains the implementation of the basic access
                 functions for the VINETIC access.     
		 
*****************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_duslic_api.h"
#include "drv_board.h"
#include "drv_duslic_main.h"


IFX_int32_t ReadReg(
    struct _DUSLIC_CHANNEL *pCh, 
    IFX_uint8_t cmd, 
    IFX_uint8_t offset,
    IFX_uint8_t* pRegValue, 
    IFX_int32_t count)
{
    DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*)pCh->pParent;
    IFX_uint8_t pCmd[MAX_CMD_BYTE];
    IFX_uint8_t data[MAX_RECEIVE_DATA_SIZE];

    pCmd[0] = CMD_RD_READ | CMD_OP_REG | (((IFX_uint8_t)pCh->nChannel) << 3) | cmd ;   
    pCmd[1] = offset;
    
    if (pDev->read(pDev, pCmd, 2, data, count+1) == IFX_ERR)
    {
        printk("read register error!\n");
        return IFX_ERR;
    }

    memcpy(pRegValue, data+1, count); 

    return IFX_SUCCESS;
}


IFX_int32_t WriteReg(
     struct _DUSLIC_CHANNEL *pCh, 
     IFX_uint8_t cmd, 
     IFX_uint8_t offset,
     IFX_uint8_t* pRegValue, 
     IFX_int32_t count)
{
    DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*)pCh->pParent;
    IFX_uint8_t pCmd[MAX_CMD_BYTE];
  
    pCmd[0] = CMD_RD_WRITE | CMD_OP_REG | (((IFX_uint8_t)pCh->nChannel) << 3) | cmd ;
    pCmd[1] = offset;
   
    memcpy(pCmd+2, pRegValue, count);
       
    if (pDev->write(pDev, pCmd, count+2) == IFX_ERR)
    {
        printk("write register error!\n");
        return IFX_ERR;
    }

    return IFX_SUCCESS;
}


IFX_int32_t ciop(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd)
{
    DUSLIC_DEVICE* pDev = (DUSLIC_DEVICE*)pCh->pParent;
    IFX_uint8_t pCmd[MAX_CMD_BYTE];
     
    pCmd[0] = CMD_RD_WRITE | CMD_OP_CIOP | (((IFX_uint8_t)pCh->nChannel) << 3) | cmd ;

    if (pDev->write(pDev, pCmd, 1) == IFX_ERR)
    {
        printk("write command error!\n");
        return IFX_ERR;
    }

    return IFX_SUCCESS;
}


IFX_int32_t ModifyReg(
      struct _DUSLIC_CHANNEL *pCh, 
      IFX_uint8_t cmd, 
      IFX_uint8_t offset,
      IFX_uint8_t RegValue, 
      IFX_uint8_t BitMask)
{
    IFX_uint8_t value;

    pCh->ReadReg(pCh, cmd, offset, &value, 1);
    value &= ~BitMask;
    
    value |= (RegValue & BitMask);

    pCh->WriteReg(pCh, cmd, offset, &value, 1);  
    
    return IFX_SUCCESS;
}
