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
#include "fxo-etsi.byt"
//#include "TEST600C.BYT"

/**
   Download CRAM Coefficients 
*/
int Dwld_LoadCram (DUSLIC_CHANNEL *pCh, DUSLIC_IO_INIT *pInit)
{
   INT err = IFX_SUCCESS;
   int i;
   IFX_uint8_t  temp[256]={0};
   IFX_uint8_t *p_coef = 0;
   IFX_uint32_t  coef_size = 0;
   DUSLIC_DEVICE *pDev = (DUSLIC_DEVICE*)pCh->pParent;
   if(pInit->param & DUS_IO_INIT_DEFAULT)
   {
          /* download ac_coef */

       if (pInit->p_ac_coef)/*if the user specified his coefficient*/
      {
/*           printk("%s: User provided ac_coef\n", __FUNCTION__); */
          /* use ac coefficients from application */
          if (pInit->ac_coef_size > sizeof (temp))
              return IFX_ERR;
          
          IFXOS_CPY_USR2KERN(temp, pInit->p_ac_coef, pInit->ac_coef_size);
          p_coef = temp;
          coef_size = pInit->ac_coef_size;
      }
      else
      {
/*           printk("%s: Default ac_coef\n", __FUNCTION__); */
          /* use compiled-in ac coefficients */
          p_coef = ac_coef;
          coef_size = sizeof(ac_coef);
      }
      
      for(i = 0; i < coef_size / 10;i++)
      {
/*           { int _i; printk("p_coef[%2d]: ", i); for (_i = 0; _i < 10; _i++) printk("%02x ", p_coef[i*10 + _i]); printk("\n"); }  */
          pDev->write(pDev, p_coef+i*10, 10);//write ac coefficients	  
      }


      
          /* download dc_coef */

      if (pInit->p_dc_coef)
      {
/*           printk("%s: User provided dc_coef\n", __FUNCTION__); */
          /* use dc coefficients from application */
          if (pInit->dc_coef_size > sizeof (temp))
              return IFX_ERR;
          
          IFXOS_CPY_USR2KERN(temp, pInit->p_dc_coef, pInit->dc_coef_size);
          p_coef = temp;
          coef_size = pInit->dc_coef_size;
      }
      else
      {
/*           printk("%s: Default dc_coef\n", __FUNCTION__); */
          /* use compiled-in dc coefficients */
          p_coef = dc_coef;
          coef_size = sizeof(dc_coef);
      }
      
      for(i = 0; i < coef_size / 10;i++)
      {
/*           { int _i; printk("p_coef[%2d]: ", i); for (_i = 0; _i < 10; _i++) printk("%02x ", p_coef[i*10 + _i]); printk("\n"); }  */
          pDev->write(pDev, p_coef+i*10, 10);//write ac coefficients	  
      }

      

          /* download gen_coef */

      if (pInit->p_gen_coef)
      {
/*           printk("%s: User provided gen_coef\n", __FUNCTION__); */
          /* use geb coefficients from application */
          if (pInit->gen_coef_size > sizeof (temp))
              return IFX_ERR;
          
          IFXOS_CPY_USR2KERN(temp, pInit->p_gen_coef, pInit->gen_coef_size);
          p_coef = temp;
          coef_size = pInit->gen_coef_size;
      }
      else
      {
/*           printk("%s: Default gen_coef\n", __FUNCTION__); */
          /* use compiled-in ac coefficients */
          p_coef = gen_coef;
          coef_size = sizeof(gen_coef);
      }

      i = 0;
      while (i < coef_size)
      {
          IFX_uint8_t cmdlen;
              /* command consists of at least 3 octets (0x44, address, data) */
          if (((coef_size - i) < 3) || (p_coef[i] != 0x44)) 
          {
/*               printk("%s: unexpected command (offset %d, size=%ld, command=0x%02x)", __FUNCTION__, i, coef_size, p_coef[i]); */
              return IFX_ERR;
          }

              /* if the address is BCR1 register, assume that BCR1..4 are written */
              /* else only one register is written */
          if (p_coef[i + 1] == 0x15)
              cmdlen = 6;
          else
              cmdlen = 3;

          if ((coef_size - i) < cmdlen)
          {
/*               printk("%s: not enough octets avaible for command  (offset %d, size=%ld, command=0x%02x)", */
/*                      __FUNCTION__, i, coef_size, p_coef[i]); */
              return IFX_ERR;
          }

/*           { int _i; printk("gen_coef[%2d]: ", i); for (_i = 0; _i < cmdlen; _i++) printk("%02x ", p_coef[i + _i]); printk("\n"); }  */
          pDev->write(pDev, &p_coef[i], cmdlen);
          i += cmdlen;
      }
   }
   if (pInit->param & DUS_IO_INIT_NO_CRAM_DWLD)
   {
       pCh->ReadReg(pCh, SOP_CMD, BCR3, temp, 1);
       temp[0] &= ~BCR3_CRAM_EN;
       pCh->WriteReg(pCh, SOP_CMD, BCR3, temp, 1);
   }
   else
   {
       pCh->ReadReg(pCh, SOP_CMD, BCR3, temp, 1);
       temp[0] |= BCR3_CRAM_EN;
       pCh->WriteReg(pCh, SOP_CMD, BCR3, temp, 1);
   }
   
   if(pInit->param & DUS_IO_INIT_IO_EMUL)
   {
           /*fix me, add the emulation code here*/
   }
   return err;   
}

