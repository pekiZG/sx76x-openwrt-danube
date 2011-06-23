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
   Module      : drv_duslic_dwnld.c
   Date        : 2005-07-12
 This file contains the implementation of the vinetic download
                 and the CRC Functions.
*****************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_duslic_dwnld.h"
#include "drv_duslic_main.h"
#include "drv_board.h"
#include "FXO_600OHM.BYT"


/**
   Download CRAM Coefficients 
*/
int Dwld_LoadCram (DUSLIC_CHANNEL *pCh, DUSLIC_IO_INIT *pInit)
{
   INT err = IFX_SUCCESS;
   int i;
   IFX_uint8_t  temp[256]={0};
   DUSLIC_DEVICE *pDev = (DUSLIC_DEVICE*)pCh->pParent;
   if(pInit->param & DUS_IO_INIT_DEFAULT)
   {
      if(pInit->pCram)/*if the user specified his coefficient*/
       {
         pCh->WriteReg(pCh, COP_CMD, 0 , pInit->pCram, pInit->nCramSize);
       }
      else /*else use the BYT file instead*/
       {
         for(i=0;i<11;i++)
	  {
	     pDev->write(pDev, ac_coef+i*10, 10);//write ac coefficients	  
	  }      
         for(i=0;i<9;i++)
	  {
	     pDev->write(pDev, dc_coef+i*10, 10);//write dc coefficients	  
	  }
	  
       }
   
   }
   if(pInit->param & DUS_IO_INIT_NO_CRAM_DWLD)
   {
       pCh->ReadReg(pCh, SOP_CMD, BCR3, temp, 1);
       temp[0]|= BCR3_CRAM_EN;
       pCh->WriteReg(pCh, SOP_CMD, BCR3, temp, 1);
   }
   if(pInit->param & DUS_IO_INIT_IO_EMUL)
   {
      /*fix me, add the emulation code here*/
   }

   return err;

}

