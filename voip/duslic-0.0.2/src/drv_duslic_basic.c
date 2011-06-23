/*
** Copyright (C) 2005 infineon <Qi-Ming.Wu@infineon.com>
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
**************************************************************************
   Module      : drv_duslic_basic.c
 This file contains the implementation of basic DUSLIC
                 access functions as WriteCmd, ReadCmd, register access
                 and so on.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_duslic_api.h"
#include "drv_duslic_basic.h"


/**
   Write DUSLIC command
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR (return value of CmdWrite)
*/
IFX_int32_t DUSLIC_Write_Cmd (DUSLIC_DEVICE * pDev, DUSLIC_IO_MB_CMD * pCmd)
{
   IFX_int32_t  ret = IFX_SUCCESS;
/*    IFX_uint16_t *pData = (IFX_uint16_t *) pCmd; */

   //pDev->write(pDev, pData, IFX_int32_t count);

#if 0
   if ((pData[0] & CMD1_CMD) == CMD1_EOP && (pData[0] & CMD1_RD) != CMD1_RD)
   {
      IFXOS_MutexLock   (pDev->memberAcc);
      DispatchCmd (pDev, pData);
   }
   ret = CmdWrite (pDev, pData, (pCmd->cmd2 & CMD2_LEN));
   if ((pData[0] & CMD1_CMD) == CMD1_EOP)
   {
      IFXOS_MutexUnlock (pDev->memberAcc);
   }
#endif

   return ret;
}


/**
   Write command to read data
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t DUSLIC_Read_Cmd (DUSLIC_DEVICE * pDev, DUSLIC_IO_MB_CMD * pCmd)
{
  int err=IFX_SUCCESS;

#if 0
   IFX_int32_t err = CmdRead (pDev, (IFX_uint16_t *) pCmd,
                              (IFX_uint16_t *) pCmd, pCmd->cmd2 & CMD2_LEN);

   /* if read failed set the cmd length to 0 */
   if (err != IFX_SUCCESS)
      pCmd->cmd2 = 0;
#endif

   return err;
}


/*******************************************************************************
 *******************************************************************************
 *******************************************************************************/
IFX_int32_t DUSLIC_Write_Reg (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd)
{
     int err;
     
     err = pCh->WriteReg(pCh, pCmd->cmd, pCmd->offset, pCmd->pData, pCmd->count);  
     
     return err;
}


IFX_int32_t DUSLIC_Read_Reg (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd)
{
     int err;
          
     err = pCh->ReadReg(pCh, pCmd->cmd, pCmd->offset,pCmd->pData, pCmd->count);
     
     return err;
}


IFX_int32_t DUSLIC_Ciop (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd)
{
     int err;
     
     err = pCh->ciop(pCh,pCmd->cmd);
     
     return err;
}

